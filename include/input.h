#pragma once

#include <SDL.h>
#include "kEvents.h"
#include "editor.h"

void input_handle_event(Editor* e, SDL_Event* ev)
{
    switch (ev->type)
    {
        case SDL_MOUSEBUTTONDOWN:
            editor_handle_mouse_down(e, ev->button.button); 
            break;

        case SDL_MOUSEMOTION:
            break;

        case SDL_KEYDOWN:
            editor_handle_key(e, (Uint32)ev->key.keysym.sym);
            break;

        case SDL_MOUSEWHEEL:
            editor_handle_scroll_y(e, ev->wheel.y);
            break;
        
        case SDL_TEXTINPUT:
            editor_insert_char(e, ev->text.text[0]);
            break;
    }
}