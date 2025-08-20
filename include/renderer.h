#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct Renderer {
    SDL_Renderer* sdl_renderer;
    TTF_Font* font;
} Renderer;

typedef enum {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} TextAlign;

/**
 * Creates a Renderer:
 *  - Creates SDL renderer
 *  - Creates TTF font
 *
 * @param window Pointer to SDL window
 * 
 * @return a valid Renderer, or NULL on error
 */
Renderer* renderer_create(SDL_Window* window);

/**
 * Cleanup, destroys the Renderer:
 *  - Closes TTF font
 *  - Destroys SDL renderer
 *  - Frees memory allocated for Renderer
 * 
 * @param r Pointer to Renderer
 */
void renderer_destroy(Renderer* r);

/**
 * Clears the render target
 * 
 * @param r Pointer to Renderer
 */
void renderer_clear(Renderer* r);

/** 
 * Updates screen with any rendering performed since last call
 * 
 * @param r Pointer to Renderer
 */
void renderer_present(Renderer* r);

/**
 * Renders text
 * 
 * @param r Pointer to Renderer
 * @param text Text to render
 * @param x Text x position
 * @param y Text y position
 * @param font Pointer to TTF_Font
 * @param color Color to render text
 */
void renderer_draw_text(Renderer* r, const char* text, int x, int y, TTF_Font* font, TextAlign align, SDL_Color color);

/**
 * Renders a rectangle
 * 
 * @param r Pointer to Renderer
 * @param x Position on x-axis (top-left corner)
 * @param y Position on y-axis (top-left corner)
 * @param w Width of rectangle
 * @param h Height of rectangle
 * @param color Colour of rectangle
 */
void renderer_draw_rect(Renderer* r, int x, int y, int w, int h, SDL_Color color);

/**
 * Renders the cursor
 * 
 * @param r Pointer to Renderer
 * @param x Cursor x position
 * @param y Cursor y position
 * @param height Height of cursor (typically line height)
 * 
 * @note May be replaced by renderer_draw_rect
 */
void renderer_draw_cursor(Renderer* r, int x, int y, int height, float alpha);

/**
 * Renders the infobar (current file, line etc.)
 * 
 * @param r Pointer to Renderer
 * @param text Text to display on infobar
 */
void renderer_draw_infobar(Renderer* r, const char* text);

/**
 * Retrieves current viewport of given Renderer
 * 
 * @param r Pointer to Renderer
 * 
 * @return SDL_Rect describing current viewport
 */
SDL_Rect renderer_get_viewport(Renderer* r);