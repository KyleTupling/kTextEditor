#pragma once

#include <SDL.h>
#include <stdbool.h>
#include "renderer.h"
#include "kEvents.h"

#define TITLEBAR_HEIGHT 30

// Forward declaration to allow for callback passing
typedef struct kWindowButton kWindowButton;

struct kWindowButton {
    float rel_x, rel_y;     // Positions relative to window size
    float rel_width;        // Width relative to window width

    char* text;             // Text to display on button

    SDL_Color base_color;   // Base background colour
    SDL_Color hover_color;  // Hovered state background colour

    bool is_hovered;        // Whether button is currently hovered

    void (*on_click)(kWindowButton* btn); // Callback for on-click
};

typedef struct {
    SDL_Window* sdl_window;             // SDL window pointer
    const char* title;                  // Title to display on titlebar
    int width;                          // Width of window
    int height;                         // Height of window
    int x, y;                           // Position of window

    bool is_dragging;                   // Whether window is currently being dragged
    int drag_offset_x, drag_offset_y;   // Current drag offsets

    kWindowButton* window_buttons;      // List of titlebar control buttons
    int num_window_buttons;             // Current number of buttons
    int capacity_window_buttons;        // Current dynamic capacity of buttons
} kWindow;

/**
 * Creates a kWindow instance.
 * Creates and adds default titlebar controls.
 * 
 * @param title Title text to display on window
 * @param x Position of window on x axis
 * @param y Position of window on y axis
 * @param w Width of window
 * @param h Height of window
 * @param flags Flags to pass to SDL window
 * 
 * @return kWindow instance
 */
kWindow window_create(const char* title, int x, int y, int w, int h, Uint32 flags);

/**
 * Destroy and cleanup of window and associated allocations.
 * Frees pointers to titlebar controls.
 * 
 * @param win Pointer to window
 */
void window_destroy(kWindow* win);

/**
 * Renders the window and associated components.
 * 
 * @param win Pointer to window
 * @param r Pointer to renderer
 */
void window_render(kWindow* win, Renderer* r);

/**
 * Adds a button to the window titlebar.
 * Memory is dynamically (re)allocated for reference by window.
 * 
 * @param win Pointer to window
 * @param btn Button to add
 */
void window_add_button(kWindow* win, kWindowButton btn);

/**
 * Handles events passed
 * 
 * @param win Pointer to window
 * @param e Pointer to SDL event
 */
void window_handle_event(kWindow* win, kEvent* e);

/**
 * Maximises the window
 * 
 * @param win Pointer to window
 */
void window_maximise(kWindow* win);

/**
 * Restores the window to original size
 * 
 * @param win Pointer to window
 * 
 * @warning Currently not working
 */
void window_restore(kWindow* win);