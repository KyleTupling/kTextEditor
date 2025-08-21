#pragma once

#include <SDL.h>
#include "kEvents.h"
#include "editor.h"

void input_handle_event(Editor* e, kEvent* ev)
{
    switch (ev->type)
    {
        case KEVENT_MOUSEBUTTONDOWN:
            editor_handle_mouse_down(e, ev->button); 
            break;

        case KEVENT_MOUSEMOTION:
            break;

        case KEVENT_KEYDOWN:
            editor_handle_key(e, ev->key.sym, ev->key.mod);
            break;

        case KEVENT_MOUSEWHEEL:
            editor_handle_scroll(e, ev->wheel);
            break;
        
        case KEVENT_TEXTINPUT:
            editor_insert_char(e, ev->text.text[0]);
            break;
    }
}