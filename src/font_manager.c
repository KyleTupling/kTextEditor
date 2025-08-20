#include "font_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static FontManager* instance = NULL;

FontManager* font_manager_get()
{
    if (!instance)
    {
        instance = malloc(sizeof(FontManager));
        instance->fonts = NULL;
        instance->sizes = NULL;
        instance->paths = NULL;
        instance->count = 0;
    }
    return instance;
}

void font_manager_load_font(const char* path, int size)
{
    FontManager* fm = font_manager_get();

    // Check if font is already loaded
    for (size_t i = 0; i < fm->count; i++)
    {
        if (fm->sizes[i] == size && strcmp(fm->paths[i], path) == 0)
        {
            return; // Font already loaded
        }
    }

    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font)
    {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        return;
    }

    fm->fonts = realloc(fm->fonts, sizeof(TTF_Font*) * (fm->count + 1));
    fm->sizes = realloc(fm->sizes, sizeof(int) * (fm->count + 1));
    fm->paths = realloc(fm->paths, sizeof(char*) * (fm->count + 1));

    fm->fonts[fm->count] = font;
    fm->sizes[fm->count] = size;
    fm->paths[fm->count] = strdup(path);
    fm->count++;
}

TTF_Font* font_manager_get_font(const char* path, int size)
{
    FontManager* fm = font_manager_get();

    font_manager_load_font(path, size);
    for (size_t i = 0; i < fm->count; i++)
    {
        if (fm->sizes[i] == size && strcmp(fm->paths[i], path) == 0)
        {
            return fm->fonts[i];
        }
    }
    return NULL;
}

const char* font_manager_get_font_path(TTF_Font* font)
{
    FontManager* fm = font_manager_get();
    for (size_t i = 0; i < fm->count; i++)
    {
        if (fm->fonts[i] == font)
        {
            return fm->paths[i];
        }
    }
    return NULL;
}

int font_manager_get_font_size(TTF_Font* font)
{
    FontManager* fm = font_manager_get();
    for (size_t i = 0; i < fm->count; i++)
    {
        if (fm->fonts[i] == font)
        {
            return fm->sizes[i];
        }
    }
    return -1;
}

void font_manager_destroy()
{
    if (!instance) return;

    for (size_t i = 0; i < instance->count; i++)
    {
        if (instance->fonts[i]) TTF_CloseFont(instance->fonts[i]);
        free(instance->paths[i]);
    }

    free(instance->fonts);
    free(instance->sizes);
    free(instance->paths);
    free(instance);
    instance = NULL;
}