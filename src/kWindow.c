#include "kWindow.h"
#include "font_manager.h"

static void close_button_clicked(kWindowButton* btn)
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
}

kWindow window_create(const char* title, int x, int y, int w, int h, Uint32 flags)
{
    kWindow win;

    win.sdl_window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | flags);
    win.title = title;
    win.width = w;
    win.height = h;
    win.x = x;
    win.y = y;
    win.is_dragging = false;
    win.drag_offset_x = 0;
    win.drag_offset_y = 0;

    win.window_buttons = NULL;
    win.num_window_buttons = 0;
    win.capacity_window_buttons = 0;

    SDL_Color base_btn_color = {20, 20, 20, 255};

    // Create close button
    kWindowButton close_btn;
    close_btn.rel_x = 0.95;
    close_btn.rel_y = 0;
    close_btn.rel_width = 0.05;
    close_btn.text = "X";
    SDL_Color close_btn_hovered_color = {200, 50, 50, 255};
    close_btn.base_color = base_btn_color;
    close_btn.hover_color = close_btn_hovered_color;
    close_btn.is_hovered = false;
    close_btn.on_click = close_button_clicked;
    ColorTransition ct;
    color_transition_init(&ct, base_btn_color, close_btn_hovered_color, 0.2f, NULL);
    close_btn.color_transition = ct;

    window_add_button(&win, close_btn);
    
    return win;
}

void window_destroy(kWindow* win)
{
    if (win->window_buttons)
    {
        free(win->window_buttons);
        win->window_buttons = NULL;
    }

    if (win->sdl_window)
    {
        SDL_DestroyWindow(win->sdl_window);
        win->sdl_window = NULL;
    }
}

void window_update(kWindow* win, float dt)
{
    for (size_t i = 0; i < win->num_window_buttons; i++)
    {
        kWindowButton* current_btn = &win->window_buttons[i];

        if (current_btn->color_transition.active)
        {
            color_transition_update(&current_btn->color_transition, dt);
        }
    }
}

void window_render(kWindow* win, Renderer* r)
{
    // Titlebar
    SDL_Color titlebar_color = {10, 10, 10, 255};
    renderer_draw_rect(r, 0, 0, win->width, TITLEBAR_HEIGHT, titlebar_color);

    SDL_Color titlebar_title_color = {255, 255, 255, 255};
    TTF_Font* title_font = font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 16);
    renderer_draw_text(r, win->title, 10, TITLEBAR_HEIGHT / 2 - 10, title_font, ALIGN_LEFT, titlebar_title_color);

    for (size_t i = 0; i < win->num_window_buttons; i++)
    {
        kWindowButton* current_btn = &win->window_buttons[i];

        float pos_x = current_btn->rel_x * win->width;
        float width = current_btn->rel_width * win->width;

        //SDL_Color btn_color = current_btn->is_hovered ? current_btn->hover_color : current_btn->base_color;
        SDL_Color btn_color = color_transition_get(&current_btn->color_transition);
        renderer_draw_rect(r, pos_x, 0, width, TITLEBAR_HEIGHT, btn_color);

        if (current_btn->text)
        {
            renderer_draw_text(r, current_btn->text, pos_x + width / 2, (TITLEBAR_HEIGHT / 2) - 10, title_font, ALIGN_CENTER, titlebar_title_color);
        }
    }
}

void window_add_button(kWindow* win, kWindowButton btn)
{
    if (win->num_window_buttons >= win->capacity_window_buttons)
    {
        int new_capacity = (win->capacity_window_buttons == 0) ? 4 : win->capacity_window_buttons * 2;
        win->window_buttons = realloc(win->window_buttons, new_capacity * sizeof(kWindowButton));
        win->capacity_window_buttons = new_capacity;
    }

    win->window_buttons[win->num_window_buttons++] = btn;
}

void window_handle_event(kWindow* win, kEvent* e)
{
    switch (e->type)
    {
        case KEVENT_MOUSEBUTTONDOWN:
            if (e->button.button == KMOUSEBUTTON_LEFT)
            {
                int mx, my;
                SDL_GetGlobalMouseState(&mx, &my); // global screen coords

                int win_x, win_y;
                SDL_GetWindowPosition(win->sdl_window, &win_x, &win_y);

                // Check for hovered titlebar button
                for (size_t i = 0; i < win->num_window_buttons; i++)
                {
                    if (win->window_buttons[i].is_hovered && win->window_buttons[i].on_click)
                    {
                        win->window_buttons[i].on_click(&win->window_buttons[i]);
                    }
                }

                if (my - win_y < TITLEBAR_HEIGHT)
                {
                    win->is_dragging = true;
                    win->drag_offset_x = mx - win_x;
                    win->drag_offset_y = my - win_y;
                }
            }
            break;
        
        case KEVENT_MOUSEBUTTONUP:
            if (e->button.button == KMOUSEBUTTON_LEFT)
            {
                win->is_dragging = false;
            }
            break;

        case KEVENT_MOUSEMOTION:
            int mx, my;
            SDL_GetGlobalMouseState(&mx, &my);

            int local_mx = e->button.x;
            int local_my = e->button.y;
            if (win->is_dragging)
            {
                int new_x = mx - win->drag_offset_x;
                int new_y = my - win->drag_offset_y;
                SDL_SetWindowPosition(win->sdl_window, new_x, new_y);
                win->x = new_x;
                win->y = new_y;
            }
            else
            {
                // Check if mouse inside a titlebar button
                for (size_t i = 0; i < win->num_window_buttons; i++)
                {
                    // Default all buttons to unhovered
                    //win->window_buttons[i].is_hovered = false;

                    // If mouse_y beyond titlebar, skip
                    //if (local_my > TITLEBAR_HEIGHT) continue;

                    float btn_start = win->window_buttons[i].rel_x * win->width;
                    float btn_end = btn_start + win->window_buttons[i].rel_width * win->width;

                    bool hovering = (local_mx > btn_start && local_mx < btn_end && local_my < TITLEBAR_HEIGHT);
                    
                    if (hovering && !win->window_buttons[i].is_hovered)
                    {
                        color_transition_start(&win->window_buttons[i].color_transition, true);
                    }
                    else if (!hovering && win->window_buttons[i].is_hovered)
                    {
                        color_transition_start(&win->window_buttons[i].color_transition, false);
                    }
                    win->window_buttons[i].is_hovered = hovering;
                }
            }
            break;
    }
}

void window_maximise(kWindow* win)
{
    SDL_SetWindowFullscreen(win->sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void window_restore(kWindow* win)
{
    SDL_RestoreWindow(win->sdl_window);
}