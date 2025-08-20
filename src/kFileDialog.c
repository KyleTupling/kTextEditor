#include "kFileDialog.h"
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include "renderer.h"
#include "font_manager.h"
#include "editor.h"

#include "theme.h"

static bool is_directory(kFileDialog* dialog, const char* name)
{
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", dialog->current_path, name);
    struct stat s;
    return (stat(full_path, &s) == 0 && S_ISDIR(s.st_mode));
}

static void load_directory(kFileDialog* dialog, const char* path)
{
    DIR* dir = opendir(path);
    if (!dir) return;

    dialog->num_entries = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (dialog->num_entries < 256)
        {
            strncpy(dialog->entries[dialog->num_entries], entry->d_name, 255);
            dialog->entries[dialog->num_entries][255] = '\0';
            dialog->num_entries++;
        }
    }
    closedir(dir);

    strncpy(dialog->current_path, path, sizeof(dialog->current_path) - 1);
    dialog->scroll_offset = 0;
    dialog->selected_index = -1;
}

static void set_selected_file(kFileDialog* dialog)
{
    snprintf(dialog->selected_file, sizeof(dialog->selected_file), "%s/%s",
             dialog->current_path,
             dialog->entries[dialog->selected_index]);
}

void kFileDialog_init(kFileDialog* dialog, const char* start_path, int x, int y, int width, int height)
{
    memset(dialog, 0, sizeof(*dialog));
    dialog->is_open = false;
    dialog->scroll_offset = 0;
    dialog->selected_index = -1;
    dialog->x = x;
    dialog->y = y;
    dialog->w = width;
    dialog->h = height;
    load_directory(dialog, start_path);
}

void kFileDialog_open(kFileDialog* dialog)
{
    dialog->selected_file[0] = '\0';
    dialog->is_open = true;
}

void kFileDialog_close(kFileDialog* dialog)
{
    dialog->is_open = false;
}

bool kFileDialog_is_open(kFileDialog* dialog)
{
    return dialog->is_open;
}

// Updates the vertical scroll offset to ensure the selected item is
// visible in the viewport.
static void kFileDialog_ensure_selection_visible(kFileDialog* dialog)
{
    int item_top    = FILE_DIALOG_PADDING + dialog->selected_index * (FILE_DIALOG_LINE_HEIGHT + FILE_DIALOG_LINE_GAP);
    int item_bottom = item_top + FILE_DIALOG_LINE_HEIGHT;

    if (item_top < dialog->scroll_offset)
    {
        // Item is above viewport -> scroll up
        dialog->scroll_offset = item_top - FILE_DIALOG_PADDING;
    }
    else if (item_bottom > dialog->scroll_offset + dialog->h)
    {
        // Item is below viewport -> scroll down
        dialog->scroll_offset = item_bottom - dialog->h + FILE_DIALOG_PADDING / 2;
    }
}

static void kFileDialog_clamp_scroll(kFileDialog* dialog)
{
    // Clamp so doesn't scroll beyond content
    if (dialog->scroll_offset < 0)
    {
        dialog->scroll_offset = 0;
    }

    int content_height = FILE_DIALOG_PADDING + dialog->num_entries * (FILE_DIALOG_LINE_HEIGHT + FILE_DIALOG_LINE_GAP);
    int max_scroll = content_height > dialog->h ? content_height - dialog->h : 0;
    if (dialog->scroll_offset > max_scroll)
    {
        dialog->scroll_offset = max_scroll;
    }
}

void kFileDialog_handle_event(kFileDialog* dialog, kEvent* event)
{
    if (!dialog->is_open) return;

    if (event->type == KEVENT_MOUSEBUTTONDOWN && event->button.button == KMOUSEBUTTON_LEFT)
    {
        int mx = event->button.x;
        int my = event->button.y;

        int rel_x = mx - dialog->x;
        int rel_y = my - dialog->y;

        // Check if click is within dialog
        if ((mx < dialog->x || mx > dialog->x + dialog->w) || (my < dialog->y || my > dialog->y + dialog->h))
        {
            return;
        }

        // Directly map y-coordinate to line index
        int adjusted_y = my - dialog->y + dialog->scroll_offset;
        int index = (my - dialog->y - FILE_DIALOG_PADDING + dialog->scroll_offset) / (FILE_DIALOG_LINE_HEIGHT + FILE_DIALOG_LINE_GAP);

        if (index >= 0 && index < dialog->num_entries) {
            dialog->selected_index = index;

            if (is_directory(dialog, dialog->entries[index])) {
                char new_path[512];
                snprintf(new_path, sizeof(new_path), "%s/%s",
                            dialog->current_path,
                            dialog->entries[index]);
                load_directory(dialog, new_path);
            } else {
                set_selected_file(dialog);
            }
        }
    }
    else if (event->type == KEVENT_KEYDOWN)
    {
        switch (event->key.sym)
        {
            case KKEY_DOWN:
                if (dialog->selected_index + 1 < dialog->num_entries)
                {
                    dialog->selected_index++;
                }
                break;

            case KKEY_UP:
                if (dialog->selected_index - 1 >= 0)
                {
                    dialog->selected_index--;
                }
                break;

            case KKEY_RETURN:
                if (dialog->selected_index >= 0 && !is_directory(dialog, dialog->entries[dialog->selected_index]))
                {
                    set_selected_file(dialog);
                }
                break;
        }

        kFileDialog_ensure_selection_visible(dialog);
        kFileDialog_clamp_scroll(dialog);
    }
    else if (event->type == KEVENT_MOUSEWHEEL)
    {
        dialog->scroll_offset -= event->wheel.y * (FILE_DIALOG_LINE_HEIGHT + FILE_DIALOG_PADDING);

        kFileDialog_clamp_scroll(dialog);
    }
}

void kFileDialog_render(kFileDialog* dialog, Renderer* renderer)
{
    if (!dialog->is_open) return;

    SDL_Color bg = {20, 20, 20, 255};
    renderer_draw_rect(renderer, dialog->x, 0, dialog->w, dialog->h, bg);

    for (int i = 0; i < dialog->num_entries; i++)
    {
        int item_y = 0 + FILE_DIALOG_PADDING + (i * (FILE_DIALOG_LINE_HEIGHT + FILE_DIALOG_LINE_GAP)) - dialog->scroll_offset;

        SDL_Color color = {200, 200, 200, 255};
        if (i == dialog->selected_index)
        {
            SDL_Color highlight = COLOR_RED_PINK;
            renderer_draw_rect(renderer, dialog->x + FILE_DIALOG_PADDING, item_y, dialog->w - FILE_DIALOG_PADDING * 2, FILE_DIALOG_LINE_HEIGHT, highlight);
        }
        renderer_draw_text(renderer, dialog->entries[i], dialog->x + FILE_DIALOG_PADDING + 10, item_y + 5, 
                           font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 14), 
                           ALIGN_LEFT, color);
    }
}

const char* kFileDialog_get_selected(kFileDialog* dialog)
{
    // if (dialog->selected_index >= 0)
    // {
    //     return dialog->entries[dialog->selected_index];
    // }
    if (dialog->selected_file[0] != '\0')
    {
        return dialog->selected_file;
    }
    return NULL;
}