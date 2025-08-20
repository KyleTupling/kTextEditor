/** 
 * ------------------------------------------------------------------
 * DEPRECATED; NO LONGER USED
 * ------------------------------------------------------------------
 */

#pragma once

#include <SDL.h>
#include <stdbool.h>
#include "renderer.h"

#define TITLEBAR_HEIGHT 30
#define BUTTON_WIDTH 40

typedef struct {
    SDL_Rect close_button;
    SDL_Rect maximise_button;
    SDL_Rect minimise_button;
} TitlebarControls;

/**
 * Initialises the titlebar controls
 * 
 * @param tb Pointer to titlebar controls
 * @param width Width of the titlebar
 * @param height Height of the titlebar
 */
void titlebar_init(TitlebarControls* tb, int width, int height);

/** 
 * Renders the titlebar to the screen
 * 
 * @param r Pointer to Renderer
 * @param tb Pointer to titlebar controls
 */
void titlebar_render(Renderer* r, TitlebarControls* tb);

/**
 * Handles events passed to titlebar
 * 
 * @param tb Pointer to titlebar controls
 * @param e Pointer to SDL event
 * @param running Pointer to bool dictating whether app is running
 * @param window Pointer to SDL window hosting the editor
 */
void titlebar_handle_event(TitlebarControls* tb, SDL_Event* e, bool* running, SDL_Window* window);