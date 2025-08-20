#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#include "kWindow.h"
#include "renderer.h"
#include "editor.h"

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 600

typedef enum {
    APP_STATE_EDITOR,
    APP_STATE_FILE_DIALOG
} AppState;

typedef struct {
    AppState state;

    kWindow window;
    Renderer* renderer;
    Editor editor;
} App;

bool app_init(App* app);
void app_run(App* app);
void app_cleanup(App* app);