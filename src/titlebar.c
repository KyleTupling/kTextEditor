#include "titlebar.h"
#include "font_manager.h"

void titlebar_init(TitlebarControls* tb, int width, int height)
{
    tb->close_button = (SDL_Rect){width - BUTTON_WIDTH, 0, BUTTON_WIDTH, TITLEBAR_HEIGHT};
    tb->maximise_button = (SDL_Rect){width - BUTTON_WIDTH * 2, 0, BUTTON_WIDTH, TITLEBAR_HEIGHT};
    tb->minimise_button = (SDL_Rect){width - BUTTON_WIDTH * 3, 0, BUTTON_WIDTH, TITLEBAR_HEIGHT};
}

void titlebar_render(Renderer* r, TitlebarControls* tb)
{
    SDL_Rect vp = renderer_get_viewport(r);

    // Background
    SDL_Color titlebar_color = {10, 10, 10, 255};
    renderer_draw_rect(r, 0, 0, vp.w, TITLEBAR_HEIGHT, titlebar_color);

    SDL_Color titlebar_title_color = {255, 255, 255, 255};
    TTF_Font* title_font = font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 16);
    renderer_draw_text(r, "kTextEditor", 10, TITLEBAR_HEIGHT / 2 - 10, title_font, ALIGN_LEFT, titlebar_title_color);

    TTF_Font* icon_font = font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 20);

    // Close button
    if (&tb->close_button)
    {
        SDL_Color close_color = {255, 50, 50, 255};
        renderer_draw_rect(r, vp.w - BUTTON_WIDTH, 0, BUTTON_WIDTH, TITLEBAR_HEIGHT, close_color);
        renderer_draw_text(r, "x", tb->close_button.x + BUTTON_WIDTH / 2, tb->close_button.y, icon_font, ALIGN_CENTER, titlebar_title_color);
    }

    if (&tb->maximise_button)
    {
        SDL_Color maximise_color = {50, 50, 50, 255};
        renderer_draw_rect(r, tb->maximise_button.x, tb->maximise_button.y, tb->maximise_button.w, tb->maximise_button.h, maximise_color);
    }

    if (&tb->minimise_button)
    {
        SDL_Color minimise_color = {50, 50, 50, 255};
        renderer_draw_rect(r, tb->minimise_button.x, tb->minimise_button.y, tb->minimise_button.w, tb->minimise_button.h, minimise_color);
    }
}

void titlebar_handle_event(TitlebarControls* tb, SDL_Event* e, bool* running, SDL_Window* window)
{
    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
    {
        int mouse_x = e->button.x;
        int mouse_y = e->button.y;
        SDL_Point pt = { mouse_x, mouse_y };

        // Close button
        if (SDL_PointInRect(&pt, &tb->close_button))
        {
            *running = false;
        }
        else if (SDL_PointInRect(&pt, &tb->maximise_button))
        {
            Uint32 flags = SDL_GetWindowFlags(window);
            if (flags & SDL_WINDOW_MAXIMIZED) SDL_RestoreWindow(window);
            else SDL_MaximizeWindow(window);

            int display_index = SDL_GetWindowDisplayIndex(window);
            SDL_DisplayMode dm;
            SDL_GetCurrentDisplayMode(display_index, &dm);
            SDL_SetWindowSize(window, dm.w, dm.h);
            SDL_SetWindowPosition(window, 0, 0);
        }
        else if (SDL_PointInRect(&pt, &tb->minimise_button))
        {
            SDL_MinimizeWindow(window);
        }
    }
}