#pragma once

#include <SDL_ttf.h>

typedef struct {
    TTF_Font** fonts;   // Currently loaded fonts
    int* sizes;         // Currently loaded sizes
    char** paths;       // Currently loaded font paths
    size_t count;       // Total loaded fonts
} FontManager;

/**
 * Singleton accessor for the font manager.
 * 
 * @return Pointer to font manager singleton
 */
FontManager* font_manager_get();

/**
 * Attempts to load a given font (if not already loaded).
 * 
 * @param path Path to the font
 * @param size Font size to load
 */
void font_manager_load_font(const char* path, int size);

/**
 * Retrieves a pointer to a given font.
 * If the font isn't already loaded, it will be loaded then returned.
 * 
 * @param path Path to the font
 * @param size Font size
 * 
 * @return Loaded SDL TTF font or NULL if failed
 */
TTF_Font* font_manager_get_font(const char* path, int size);

/**
 * Retrieves the path of a given loaded SDL TTF font.
 * 
 * @param font SDL TTF font to get path for
 * 
 * @return Path of the font or NULL if failed
 */
const char* font_manager_get_font_path(TTF_Font* font);

/**
 * Retrieves the size of a given loaded SDL TTF font.
 * 
 * @param font SDL TTF font to get size for
 * 
 * @return Size of the font or NULL if failed
 */
int font_manager_get_font_size(TTF_Font* font);

/**
 * Destroy and cleanup of font manager.
 * Frees memory tied to font, path, size lists.
 */
void font_manager_destroy();