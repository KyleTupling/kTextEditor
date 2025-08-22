/**
 * Notes on usage:
 *      - When a file is selected (i.e. the user has picked a file), the selected_file
 *        property will be set to the absolute path of the selected file and the dialog
 *        will be closed. There are also scenarios in which the dialog may close without
 *        a file having been selected. This means that, for smoothest functionality,
 *        higher-level checks should be performed in this order:
 *          1. Has the dialog closed?
 *          2. Was selected_file set?
 * 
 *        I'll probably update this at some point to use file_selected callbacks or
 *        something similar to give easier flow control.
 */

#pragma once

#include <stdbool.h>
#include "renderer.h"
#include "kEvents.h"

#define FILE_DIALOG_LINE_HEIGHT 30
#define FILE_DIALOG_PADDING 10
#define FILE_DIALOG_LINE_GAP 10
#define FILE_DIALOG_INFOBAR_HEIGHT 30

#define GET_ITEM_TOP(I) FILE_DIALOG_PADDING + I * (FILE_DIALOG_LINE_HEIGHT + FILE_DIALOG_LINE_GAP)

typedef struct {
    bool is_open;
    char current_path[512];
    char selected_file[512];

    int scroll_offset;
    int selected_index;
    int num_entries;

    char entries[256][256]; // list of file/dir names

    int x, y, w, h;
} kFileDialog;

/**
 * Initialises the file dialog.
 * 
 * @param dialog Pointer to kFileDialog
 * @param start_path The path to open the file dialog at
 * @param x Position of dialog on x-axis
 * @param y Position of dialog on y-axis
 * @param width Width of dialog
 * @param height Height of dialog
 */
void kFileDialog_init(kFileDialog* dialog, const char* start_path,
                      int x, int y, int width, int height);

/**
 * Opens the dialog.
 * 
 * @param dialog Pointer to kFileDialog
 * 
 * @note This resets the selected file to '\0' 
 */                      
void kFileDialog_open(kFileDialog* dialog);

/**
 * Closes the dialog.
 * 
 * @param dialog Pointer to kFileDialog
 */
void kFileDialog_close(kFileDialog* dialog);

/**
 * Checks whether the given dialog is currently open.
 * 
 * @param dialog Pointer to kFileDialog
 * 
 * @return Whether dialog is currently open
 */
bool kFileDialog_is_open(kFileDialog* dialog);

/**
 * Handles events passed to dialog.
 * 
 * @param dialog Pointer to kFileDialog
 * @param event Pointer to SDL event
 */
void kFileDialog_handle_event(kFileDialog* dialog, kEvent* event);

/**
 * Renders the given dialog.
 * 
 * @param dialog Pointer to kFileDialog
 * @param renderer Pointer to Renderer
 */
void kFileDialog_render(kFileDialog* dialog, Renderer* renderer);

/**
 * Retrieves the currently selected file.
 * 
 * @param dialog Pointer to kFileDialog
 * 
 * @return Path/name of selected file
 */
const char* kFileDialog_get_selected(kFileDialog* dialog);