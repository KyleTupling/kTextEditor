#include "app.h"
#include "editor.h"
#include "renderer.h"
#include "input.h"
#include "font_manager.h"
#include "kFileDialog.h"

#include <stdio.h>

static kFileDialog dialog;

bool app_init(App* app)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (TTF_Init() != 0) return false;

    printf("[app] SDL and TTF initialised.\n");

    app->window = window_create("kTextEditor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (!app->window.sdl_window) return false;

    printf("[app] Window created.\n");

    app->renderer = renderer_create(app->window.sdl_window);
    if (!app->renderer) return false;

    editor_init(&app->editor);

    editor_load_file(&app->editor, "test.txt");

    SDL_StartTextInput();

    printf("[app] App initialised.\n");

    kFileDialog_init(&dialog, ".", 0, 0, app->window.width, app->window.height);
    return true;
}

void app_run(App* app)
{
    bool running = true;
    SDL_Event event;

    Uint32 last_tick = SDL_GetTicks();

    while (running)
    {
        Uint32 current_tick = SDL_GetTicks();
        float delta_time = (current_tick - last_tick) / 1000.0f; // converted to seconds
        last_tick = current_tick;

        app->editor.text_changed = false;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) running = false;
            else
            {
                window_handle_event(&app->window, &event);
                //titlebar_handle_event(&app->titlebar, &event, &running, app->window.sdl_window);
                if (app->state == APP_STATE_EDITOR)
                {
                    input_handle_event(&app->editor, &event);  
                }
                else if (app->state == APP_STATE_FILE_DIALOG)
                {
                    kFileDialog_handle_event(&dialog, &event);
                    if (*dialog.selected_file != '\0')
                    {
                        if(!editor_load_file(&app->editor, kFileDialog_get_selected(&dialog)))
                        {
                            fprintf(stderr, "Failed to load file: %s\n", kFileDialog_get_selected(&dialog));
                        }
                        app->state = APP_STATE_EDITOR;
                    }
                } 
                

                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F4)
                {
                    kFileDialog_open(&dialog);
                    app->state = APP_STATE_FILE_DIALOG;
                }
            }
        }

        renderer_clear(app->renderer);    

        editor_update(&app->editor, delta_time);

        window_render(&app->window, app->renderer);
        
        SDL_Rect editor_bounds = { 0, TITLEBAR_HEIGHT + 5, WINDOW_WIDTH, WINDOW_HEIGHT - TITLEBAR_HEIGHT - 5 };
        if (app->state == APP_STATE_EDITOR)
        {
            SDL_RenderSetViewport(app->renderer->sdl_renderer, &editor_bounds);
            app->editor.viewport_height = editor_bounds.h;
            editor_render(&app->editor, app->renderer);
            SDL_RenderSetViewport(app->renderer->sdl_renderer, NULL);
        }
        else if (app->state == APP_STATE_FILE_DIALOG)
        {
            dialog.x = editor_bounds.x;
            dialog.y = editor_bounds.y;
            dialog.w = editor_bounds.w;
            dialog.h = editor_bounds.h;
            //kFileDialog_open(&dialog);
            kFileDialog_render(&dialog, app->renderer);
        }

        Uint32 flags = SDL_GetWindowFlags(app->window.sdl_window);
        if ((flags & SDL_WINDOW_INPUT_FOCUS) == 0)
        {
            SDL_Color unfocus_color = {40, 40, 40, 100};
            renderer_draw_rect(app->renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, unfocus_color);
        }

        // Visualise FPS
        // SDL_Color fps_color = {255, 255, 255, 255};
        // char fps_buffer[100];
        // sprintf(fps_buffer, "%.0f", 1.0f / (delta_time > 0.0001f ? delta_time : 0.0001f));
        // renderer_draw_text(app->renderer, fps_buffer, WINDOW_WIDTH - 100, 20, font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 12), ALIGN_CENTER, fps_color);

        renderer_present(app->renderer);
    }
}

void app_cleanup(App* app)
{
    renderer_destroy(app->renderer);
    if (app->window.sdl_window) SDL_DestroyWindow(app->window.sdl_window);
    SDL_StopTextInput();
    font_manager_destroy();
    TTF_Quit();
    SDL_Quit();
}