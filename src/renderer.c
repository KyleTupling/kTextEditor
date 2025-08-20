#include "renderer.h"
#include "font_manager.h"
#include <SDL.h>
#include <SDL_ttf.h>

#include <stdio.h>

Renderer* renderer_create(SDL_Window* window)
{
    Renderer* renderer = malloc(sizeof(Renderer));
    if (!renderer)
    {
        fprintf(stderr, "[renderer] Failed to allocate Renderer.\n");
        return NULL;
    }

    renderer->sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer->sdl_renderer)
    {
        fprintf(stderr, "[renderer] Failed to create SDL renderer: %s\n", SDL_GetError());
        renderer_destroy(renderer);
        return NULL;
    }
    SDL_SetRenderDrawBlendMode(renderer->sdl_renderer, SDL_BLENDMODE_BLEND);

    renderer->font = TTF_OpenFont("resources/fonts/SourceCodePro-Bold.ttf", 16);
    if (!renderer->font)
    {
        fprintf(stderr, "[renderer] Failed to open font: %s\n", TTF_GetError());
        renderer_destroy(renderer);
        return NULL;
    }

    printf("[renderer] TTF font created.\n");
    printf("[renderer] Renderer created.\n");
    return renderer;
}

void renderer_destroy(Renderer* r)
{
    if (!r) return;
    if (r->font) TTF_CloseFont(r->font);
    if (r->sdl_renderer) SDL_DestroyRenderer(r->sdl_renderer);
    free(r);
}

void renderer_clear(Renderer* r)
{
    SDL_SetRenderDrawColor(r->sdl_renderer, 20, 20, 20, 255);
    SDL_RenderClear(r->sdl_renderer);
}

void renderer_present(Renderer* r)
{
    SDL_RenderPresent(r->sdl_renderer);
}

void renderer_draw_text(Renderer* r, const char* text, int x, int y, TTF_Font* font, TextAlign align, SDL_Color color)
{
    int text_width, text_height;
    TTF_SizeText(font, text, &text_width, &text_height);

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(r->sdl_renderer, surface);
    int tex_w, tex_h;
    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);

    switch (align)
    {
        case ALIGN_CENTER:
            x -= text_width / 2;
            break;
        case ALIGN_RIGHT:
            x -= text_width;
            break;
        case ALIGN_LEFT:
        default:
            break;
    }

    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    // Visualise text bounds
    //renderer_draw_rect(r, dest.x, dest.y, dest.w, dest.h, color);
    SDL_RenderCopy(r->sdl_renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

void renderer_draw_rect(Renderer* r, int x, int y, int w, int h, SDL_Color color)
{
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(r->sdl_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(r->sdl_renderer, &rect);
}

void renderer_draw_cursor(Renderer* r, int x, int y, int height, float alpha)
{
    SDL_Rect cursor_rect = { x, y, 2, height };
    SDL_SetRenderDrawColor(r->sdl_renderer, 200, 200, 200, (Uint8)(alpha * 255));
    SDL_RenderFillRect(r->sdl_renderer, &cursor_rect);
}

void renderer_draw_infobar(Renderer* r, const char* text)
{
    SDL_Rect vp;
    SDL_RenderGetViewport(r->sdl_renderer, &vp);

    SDL_Rect rect = { 0, vp.h - 25, vp.w, 25};
    SDL_SetRenderDrawColor(r->sdl_renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(r->sdl_renderer, &rect);

    SDL_Color info_text_color = { 200, 200, 200, 255 };
    TTF_Font* text_font = font_manager_get_font("resources/fonts/SourceCodePro-Bold.ttf", 16);
    renderer_draw_text(r, text, 5, vp.h - 25, text_font, ALIGN_LEFT, info_text_color);
}

SDL_Rect renderer_get_viewport(Renderer* r)
{
    SDL_Rect vp;
    SDL_RenderGetViewport(r->sdl_renderer, &vp);
    return vp;
}