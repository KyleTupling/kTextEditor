#include "editor.h"
#include "font_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "theme.h"

static void set_current_font(Editor* e, const char* fontname, int fontsize)
{
    size_t len = strlen("resources/fonts/") + strlen(fontname) + 1;
    char* font_path = malloc(len);
    if (font_path)
    {
        snprintf(font_path, len, "resources/fonts/%s", fontname);
        e->current_font = font_manager_get_font(font_path, fontsize);
        e->line_height = TTF_FontLineSkip(e->current_font);
        free(font_path);
    }
}

static void editor_clamp_scroll_y(Editor* e)
{
    int content_height = e->num_lines * e->line_height + 25; // 25 = infobar height
    int max_scroll = content_height - (e->viewport_height + 25) + (e->line_height * e->cursor_margin_lines_y);
    if (max_scroll < 0) max_scroll = 0;
    if (e->scroll_offset_y > max_scroll) e->scroll_offset_y = max_scroll;
}

static void move_cursor(Editor* e, int new_line, int new_col, bool shift)
{
    if (shift)
    {
        if (!e->is_selecting)
        {
            e->selection_start_line = e->cursor_line;
            e->selection_start_col  = e->cursor_col;
            e->is_selecting = true;
        }

        e->selection_end_line = new_line;
        e->selection_end_col  = new_col;
    }
    else
    {
        e->is_selecting = false;
    }

    e->cursor_line = new_line;
    e->cursor_col  = new_col;
    e->cursor_cooldown = 1.0f;

    // infobar height = 25px
    int usable_height = e->viewport_height - 25; //TODO: Update this when infobar is refactored
    int first_visible_line = e->scroll_offset_y / e->line_height;
    int last_visible_line = (e->scroll_offset_y + usable_height) / e->line_height;

    // Ensuring that cursor is always visible on the viewport
    // A number of editor lines around the cursor are set to be always visible (Editor::cursor_margin_lines_y)
    if (e->cursor_line < first_visible_line + e->cursor_margin_lines_y)
    {
        if (e->cursor_line - e->cursor_margin_lines_y >= 0)
        {
            e->scroll_offset_y = (e->cursor_line - e->cursor_margin_lines_y) * e->line_height;
        }
        else // If cursor - margin is not greater than or equal to zero, the cursor must be on
             // line number (cursor_margin_lines_y) at most
        {
            e->scroll_offset_y = 0;
        }
    }
    else if (e->cursor_line > last_visible_line - e->cursor_margin_lines_y)
    {
        int lines_fit = usable_height / e->line_height;
        e->scroll_offset_y = (e->cursor_line - lines_fit + e->cursor_margin_lines_y) * e->line_height;
    }
}

void editor_init(Editor* e)
{
    // Empty editor and original copy
    strcpy(e->lines[0], "");
    strcpy(e->original_lines[0], "");

    e->line_height = 20;
    e->left_margin = 40;

    e->num_lines = 1;

    e->scroll_offset_x = 0;
    e->scroll_offset_y = 0;

    e->cursor_line = 0;
    e->cursor_col = 0;
    e->cursor_timer = 0.0f;
    e->cursor_alpha = 1.0f;
    e->cursor_blink_duration = 1.2f;
    e->cursor_cooldown = 1.0f;

    e->cursor_margin_lines_y = 3;

    e->is_selecting = false;
    e->selection_start_line = 0;
    e->selection_start_col = 0;
    e->selection_end_line = 0;
    e->selection_end_col = 0;
    
    e->text_changed = false;
    e->is_saved = true;

    set_current_filename(e, "");

    //e->current_font = font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 20);
    set_current_font(e, "SourceCodePro-Bold.ttf", 18);

    e->viewport_width = 0;
    e->viewport_height = 0;

    printf("[editor] Editor initialised.\n");
}

void editor_update(Editor* e, float delta_time)
{
    // Update cursor cooldown
    if (e->cursor_cooldown > 0.0f)
    {
        e->cursor_cooldown -= delta_time;
        if (e->cursor_cooldown <= 0.0f) 
        {
            e->cursor_cooldown = 0.0f;

            // Reset timer so fade starts from max alpha
            e->cursor_timer = 0.0f;
        }

        // During cooldown, keep cursor fully visible
        e->cursor_alpha = 1.0f;
    }
    else
    {
        e->cursor_timer += delta_time;
        // Map cursor timer to opacity
        float t = fmodf(e->cursor_timer, e->cursor_blink_duration) / e->cursor_blink_duration;
        e->cursor_alpha = cosf(t * 2.0f * 3.14159256f); // [-1, 1]
        e->cursor_alpha = (e->cursor_alpha + 1.0f) / 2.0f;              // [0, 1]

        // Scale to bounds
        e->cursor_alpha = 0.1f + e->cursor_alpha * (1.0f - 0.1f);
    }

    e->is_saved = editor_is_file_saved(e);
}

int editor_load_file(Editor* e, const char* filename)
{
    fprintf(stdout, "Loading: %s\n", filename);

    FILE* f = fopen(filename, "r");
    if (!f) return 0; // Failed to open file

    editor_init(e);
    e->num_lines = 0; // Completely empty file

    while (e->num_lines < MAX_LINES && fgets(e->lines[e->num_lines], MAX_LINE_LENGTH, f))
    {
        // Remove newline
        size_t len = strlen(e->lines[e->num_lines]);
        if (len > 0 && (e->lines[e->num_lines][len-1] == '\n' || e->lines[e->num_lines][len-1] == '\r'))
        {
            e->lines[e->num_lines][len-1] = '\0';
        }

        strcpy(e->original_lines[e->num_lines], e->lines[e->num_lines]);
    
        e->num_lines++;
    }

    // Set the cursor position to end of file
    e->cursor_line = e->num_lines - 1;
    e->cursor_col = strlen(e->lines[e->cursor_line]);

    set_current_filename(e, filename);

    fclose(f);
    return 1;
}

int editor_save_file(Editor* e)
{
    FILE* f = fopen(e->current_file, "w");
    if (!f) return 0; // Failed to open

    for (size_t i = 0; i < e->num_lines; i++)
    {
        if (i < e->num_lines - 1)
        {
            fprintf(f, "%s\n", e->lines[i]);
        }
        else
        {
            fprintf(f, "%s", e->lines[i]);
        }

        strcpy(e->original_lines[i], e->lines[i]);
    }

    fclose(f);

    return 1;
}

bool editor_is_file_saved(Editor* e)
{
    // ----------------------------------------------------------------
    // OLD METHOD - opening file every X amount of time to check 
    //              if any changes
    // ----------------------------------------------------------------
    // FILE* f = fopen(e->current_file, "r");
    // fprintf(stdout, "%s", !f ? "FAILED\n" : "SUCCESS\n");
    // if (!f) return false;

    // size_t i = 0;

    // char line_buffer[MAX_LINE_LENGTH];
    // while (fgets(line_buffer, MAX_LINE_LENGTH, f) && i < MAX_LINES)
    // {
    //     // Remove newline
    //     size_t len = strlen(line_buffer);
    //     if (len > 0 && line_buffer[len - 1] == '\n')
    //         line_buffer[len - 1] = '\0';

    //     if (strcmp(line_buffer, e->lines[i]) != 0)
    //     {
    //         return false;
    //     }

    //     i++;
    // }

    // fclose(f);
    // return true;  

    // Compare current editor lines to original copy
    for (size_t i = 0; i < e->num_lines; i++) {
        if (strcmp(e->lines[i], e->original_lines[i]) != 0) {
            return false;
        }
    }
    return true;
}

void editor_insert_char(Editor* e, char c)
{
    char* line = e->lines[e->cursor_line];
    size_t len = strlen(line);
    if (len + 1 < MAX_LINE_LENGTH)
    {
        memmove(&line[e->cursor_col + 1], &line[e->cursor_col], len - e->cursor_col + 1);
        line[e->cursor_col] = c;
        e->cursor_col++;
    }

    e->text_changed = true;
}

void editor_backspace(Editor* e)
{
    char* line = e->lines[e->cursor_line];

    if (e->cursor_col > 0)
    {
        memmove(&line[e->cursor_col - 1], &line[e->cursor_col], strlen(&line[e->cursor_col]) + 1);
        e->cursor_col--;
    }
    else if (e->cursor_line > 0) 
    {
        int prev_len = strlen(e->lines[e->cursor_line - 1]);
        if (prev_len + strlen(line) < MAX_LINE_LENGTH) 
        {
            strcat(e->lines[e->cursor_line - 1], line);
            for (int i = e->cursor_line; i < e->num_lines - 1; i++) 
            {
                strcpy(e->lines[i], e->lines[i + 1]);
            }
            e->num_lines--;
            e->cursor_line--;
            e->cursor_col = prev_len;
        }
    }

    e->cursor_cooldown = 1.0f;
    e->text_changed = true;
}

void editor_handle_key(Editor* e, kKeycode key, kKeymod mod)
{
    char* line = e->lines[e->cursor_line];

    switch (key)
    {
        case KKEY_BACKSPACE: // Backspace
            editor_backspace(e);
            break;

        case KKEY_TAB: // Tab
            editor_insert_char(e, ' ');
            editor_insert_char(e, ' ');
            editor_insert_char(e, ' ');
            editor_insert_char(e, ' ');
            break;

        case KKEY_RETURN: // Return
            if (e->num_lines < MAX_LINES) 
            {
                for (int i = e->num_lines; i > e->cursor_line + 1; i--) 
                {
                    strcpy(e->lines[i], e->lines[i - 1]);
                }
                strcpy(e->lines[e->cursor_line + 1], &line[e->cursor_col]);
                line[e->cursor_col] = '\0';
                move_cursor(e, e->cursor_line + 1, 0, false);
                e->num_lines++;
            }
            break;

        case KKEY_LEFT:
        {
            int new_line = e->cursor_line;
            int new_col  = e->cursor_col;

            if (new_col > 0) new_col--;
            else if (new_line > 0) 
            {
                new_line--;
                new_col = strlen(e->lines[new_line]);
            }

            move_cursor(e, new_line, new_col, mod == KKEYMOD_SHIFT);
            break;
        }

        case KKEY_RIGHT:
        {
            int new_line = e->cursor_line;
            int new_col  = e->cursor_col;
            int line_len = strlen(e->lines[new_line]);

            if (new_col < line_len) new_col++;
            else if (new_line + 1 < e->num_lines) 
            {
                new_line++;
                new_col = 0;
            }

            move_cursor(e, new_line, new_col, mod == KKEYMOD_SHIFT);
            break;
        }

        case KKEY_UP:
        {
            if (e->cursor_line > 0)
            {
                int new_line = e->cursor_line - 1;
                int new_col  = e->cursor_col;
                int line_len = strlen(e->lines[new_line]);
                if (new_col > line_len) new_col = line_len;

                move_cursor(e, new_line, new_col, mod == KKEYMOD_SHIFT);
            }
            else if (!(mod == KKEYMOD_SHIFT))
            {
                e->is_selecting = false;
            }
            break;
        }

        case KKEY_DOWN:
        {
            if (e->cursor_line + 1 < e->num_lines)
            {
                int new_line = e->cursor_line + 1;
                int new_col  = e->cursor_col;
                int line_len = strlen(e->lines[new_line]);
                if (new_col > line_len) new_col = line_len;

                move_cursor(e, new_line, new_col, mod == KKEYMOD_SHIFT);
            }
            else if (!(mod == KKEYMOD_SHIFT))
            {
                e->is_selecting = false;
            }
            break;
        }

        case KKEY_F1: // F1
            set_current_font(e, "SourceCodePro-Bold.ttf", font_manager_get_font_size(e->current_font) - 1);
            editor_clamp_scroll_y(e);
            break;

        case KKEY_F2:
            set_current_font(e, "SourceCodePro-Bold.ttf", font_manager_get_font_size(e->current_font) + 1);
            editor_clamp_scroll_y(e);
            break;

        case KKEY_F5: // F5
            editor_save_file(e);

        default:
            break;
    }
}

void editor_handle_mouse_down(Editor* e, kMouseButtonEvent btn)
{
    // if (btn == K_MOUSE_BUTTON_LEFT)
    // {
        
    // }
}

void editor_handle_scroll(Editor* e, kMouseWheelEvent wheel)
{
    if (wheel.mod == KKEYMOD_SHIFT)
    {
        // Horizontal scroll
        int first_visible_line = e->scroll_offset_y / e->line_height;
        int last_visible_line = (e->scroll_offset_y + e->viewport_height) / e->line_height + 1;

        // Find line of text with longest width
        // Only check lines currently visible
        int max_width = 0;
        for (int i = first_visible_line; i < last_visible_line; i++)
        {
            int current_line_width; 
            TTF_SizeText(e->current_font, e->lines[i], &current_line_width, NULL);
            if (current_line_width > max_width) max_width = current_line_width;
        }

        if (max_width > e->viewport_width - e->left_margin - 40)
        {
            e->scroll_offset_x -= wheel.y * e->line_height * 2;
            if (e->scroll_offset_x < 0) e->scroll_offset_x = 0;
            if (e->scroll_offset_x > e->viewport_width + e->left_margin)
            {
                e->scroll_offset_x = e->viewport_width + e->left_margin;
            }
        }
        else
        {
            e->scroll_offset_x = 0;
        }
    }
    else
    {
        // Vertical Scroll
        e->scroll_offset_y -= wheel.y * e->line_height;

        //TODO: Move this to clamp scroll function
        if (e->scroll_offset_y < 0) e->scroll_offset_y = 0;

        editor_clamp_scroll_y(e);
    }    
}

void editor_render(Editor* e, struct Renderer* r)
{
    SDL_Color textColor = { 230, 230, 230, 255 };
    SDL_Color lineNumberColor = { 100, 180, 100, 255};

    //e->line_height = TTF_FontLineSkip(e->current_font);

    // Determine width of line numbers (in pixels) for the current font
    int max_line_number = e->num_lines;
    char buffer[16];
    sprintf(buffer, "%d", max_line_number);
    int line_number_width = 0;
    TTF_SizeText(e->current_font, buffer, &line_number_width, NULL); // get width of widest line number

    int gutter_padding = 5; // space between line number and text

    SDL_Rect vp = renderer_get_viewport(r);
    int first_visible_line = e->scroll_offset_y / e->line_height;
    int last_visible_line = (e->scroll_offset_y + vp.h) / e->line_height + 1;

    if (last_visible_line > e->num_lines) last_visible_line = e->num_lines;

    SDL_Rect lh = {e->left_margin + line_number_width + gutter_padding, e->cursor_line * e->line_height - e->scroll_offset_y, 
                   vp.w - e->left_margin + line_number_width + gutter_padding, e->line_height};
    SDL_Color lh_color = {25, 25, 25, 255};
    renderer_draw_rect(r, lh.x, lh.y, lh.w, lh.h, lh_color);

    for (size_t i = first_visible_line; i < last_visible_line; i++)
    {
        int y = (i * e->line_height) - e->scroll_offset_y;

        if (e->is_selecting)
        {
            int start_line = e->selection_start_line;
            int end_line   = e->selection_end_line;
            int start_col  = e->selection_start_col;
            int end_col    = e->selection_end_col;

            // Normalise selection order
            if (start_line > end_line || (start_line == end_line && start_col > end_col))
            {
                int tmp_line = start_line;
                start_line   = end_line;
                end_line     = tmp_line;

                int tmp_col = start_col;
                start_col   = end_col;
                end_col     = tmp_col;
            }

            if (i >= start_line && i<= end_line)
            {
                int line_start_col = (i == start_line) ? start_col : 0;
                int line_end_col   = (i == end_line)   ? end_col   : strlen(e->lines[i]);

                if (line_end_col > line_start_col)
                {
                    // Compute pixel positions
                    char temp[512];
                    int px_start = 0, px_end = 0;

                    if (line_start_col > 0)
                    {
                        strncpy(temp, e->lines[i], line_start_col);
                        temp[line_start_col] = '\0';
                        TTF_SizeText(e->current_font, temp, &px_start, NULL);
                    }

                    strncpy(temp, e->lines[i], line_end_col);
                    temp[line_end_col] = '\0';
                    TTF_SizeText(e->current_font, temp, &px_end, NULL);

                    SDL_Rect sel_rect = {
                        e->left_margin + line_number_width + gutter_padding + px_start,
                        y,
                        px_end - px_start,
                        e->line_height
                    };
                    SDL_Color sel_bg = {60, 90, 180, 150};
                    renderer_draw_rect(r, sel_rect.x, sel_rect.y, sel_rect.w, sel_rect.h, sel_bg);
                }
            }
        }
        
        renderer_draw_text(r, e->lines[i], e->left_margin + line_number_width + gutter_padding - e->scroll_offset_x, y, e->current_font, ALIGN_LEFT, textColor);
    }

    int cursor_x = 0;
    if (e->cursor_col > 0) {
        char buffer[256];
        // Copy text up to cursor
        strncpy(buffer, e->lines[e->cursor_line], e->cursor_col);
        buffer[e->cursor_col] = '\0';

        // Get width of this substring
        TTF_SizeText(e->current_font, buffer, &cursor_x, NULL);
    }
    int cursor_y = e->cursor_line * e->line_height;
    renderer_draw_cursor(r, e->left_margin + line_number_width + gutter_padding + cursor_x - e->scroll_offset_x, e->cursor_line * e->line_height - e->scroll_offset_y, e->line_height, e->cursor_alpha);

    // Rect behind line numbers
    // TODO: Remove this extra render call, use logic to NOT draw some text under?
    SDL_Color bg = {20, 20, 20, 255};
    renderer_draw_rect(r, 0, 0, e->left_margin + line_number_width - gutter_padding, vp.h, bg);

    for (size_t i = first_visible_line; i < last_visible_line; i++)
    {
        int y = (i * e->line_height) - e->scroll_offset_y;

        char lineNumber[16];
        snprintf(lineNumber, sizeof(lineNumber), "%4d", i + 1);
        if (i == e->cursor_line) lineNumberColor.a = 255;
        else lineNumberColor.a = 100;

        
        renderer_draw_text(r, lineNumber, line_number_width + 20, y, e->current_font, ALIGN_RIGHT, lineNumberColor);
    }
    
    char info[128];
    snprintf(info, sizeof(info), "%s%s | Line %d, Col %d", e->current_file, e->is_saved ? "" : "*", e->cursor_line + 1, e->cursor_col + 1);
    renderer_draw_infobar(r, info);
}