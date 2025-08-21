#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "renderer.h"
#include "kEvents.h"

#define MAX_LINES       1024
#define MAX_LINE_LENGTH 256

#define MAX_FILENAME_LENGTH 256

typedef struct {
    int line_height;
    int left_margin;

    char lines[MAX_LINES][MAX_LINE_LENGTH]; // Current text
    char original_lines[MAX_LINES][MAX_LINE_LENGTH];
    int num_lines;                          // Current number of lines

    int scroll_offset_x;                    // Horizontal scroll
    int scroll_offset_y;                    // Vertical scroll

    int cursor_line;                        // Current cursor line
    int cursor_col;                         // Current cursor column
    float cursor_timer;                     // Cursor internal timer for animations
    float cursor_alpha;                     // Current cursor alpha
    float cursor_blink_duration;            // Duration of cursor blink
    float cursor_cooldown;                  // Time until blink effect begins

    int cursor_margin_lines_y;               // Number of lines that must always remain in view
                                             // below the cursor

    bool is_selecting;
    int selection_start_line;
    int selection_start_col;
    int selection_end_line;
    int selection_end_col;

    bool text_changed;                      // Whether text has changed since
                                            // the last frame

    bool is_saved;                          // Whether file is currently saved
    //bool needs_save_check;                  // Whether a save check needs to be 
                                            // performed

    char current_file[MAX_FILENAME_LENGTH]; // Current file name

    TTF_Font* current_font;                 // Current font

    int viewport_width;
    int viewport_height;                    // Height of the viewport in which the 
                                            // editor is rendered
} Editor;

/**
 * Helper function to update the current filename in the editor state
 * 
 * @param e Pointer to the editor state
 * @param filename The filename to update state with
 */
static void set_current_filename(Editor* e, const char* filename)
{
    strncpy(e->current_file, filename, MAX_FILENAME_LENGTH);
    e->current_file[MAX_FILENAME_LENGTH-1] = '\0';
}

/**
 * Initialises the editor
 * 
 * @param e Pointer to the editor state
 */
void editor_init(Editor* e);

/**
 * Updates the editor state
 * 
 * @param e Pointer to the editor state
 * @param delta_time Time in seconds since the last frame
 */
void editor_update(Editor* e, float delta_time);

/**
 * Loads the contents of a given file into the editor
 * 
 * @param e Pointer to the editor state
 * @param filename Name of the file to load
 * 
 * @return Success code
 */
int editor_load_file(Editor* e, const char* filename);

/**
 * Saves the contents of the current editor file
 * 
 * @param e Pointer to the editor state
 * 
 * @return Success code
 */
int editor_save_file(Editor* e);

/**
 * Checks whether the current editor file is saved
 * 
 * @param e Pointer to the editor state
 * 
 * @return Whether file is saved
 * 
 * @note This function has been updated to use a memory cache of the original file
 */
bool editor_is_file_saved(Editor* e);

/**
 * Inserts a specified character at the current cursor position
 * 
 * @param e Pointer to the editor state
 * @param c Character to insert
 */
void editor_insert_char(Editor* e, char c);

/**
 * Removes the character behind the current cursor position
 * 
 * @param e Pointer to the editor state
 */
void editor_backspace(Editor* e);

/**
 * Handles key inputs
 * 
 * @param e Pointer to the editor state
 * @param key Inputted key
 * 
 * @note The key is taken as an int, assumed converted from framework's keycode
 */
void editor_handle_key(Editor* e, kKeycode key, kKeymod mod); // Assume input convert SDL keycode to int

void editor_handle_mouse_down(Editor* e, kMouseButtonEvent btn);

void editor_handle_mouse_motion(Editor* e);

/** 
 * Handles vertical scrolling (mouse)
 * 
 * @param e Pointer to the editor state
 * @param wheel_y The vertical scroll amount
 */
void editor_handle_scroll(Editor* e, kMouseWheelEvent wheel);

/**
 * Renders the editor
 * 
 * @param e Pointer to the editor state
 * @param r Pointer to renderer
 */
void editor_render(Editor* e, struct Renderer* r); // forward declare Renderer