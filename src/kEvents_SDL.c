/*
    Serves as a backend to translate SDL events into kEvents.
    Similar backends can be made for other frameworks.
*/

#include "kEvents.h"
#include <SDL.h>
#include <string.h>

static kKeycode translate_key(SDL_Keycode sym)
{
    switch (sym)
    {
        case SDLK_LEFT: return KKEY_LEFT;
        case SDLK_RIGHT: return KKEY_RIGHT;
        case SDLK_UP: return KKEY_UP;
        case SDLK_DOWN: return KKEY_DOWN;
        case SDLK_RETURN: return KKEY_RETURN;
        case SDLK_BACKSPACE: return KKEY_BACKSPACE;
        case SDLK_DELETE: return KKEY_DELETE;
        case SDLK_TAB: return KKEY_TAB;
        case SDLK_ESCAPE: return KKEY_ESCAPE;

        case SDLK_F1: return KKEY_F1;
        case SDLK_F2: return KKEY_F2;
        case SDLK_F3: return KKEY_F3;
        case SDLK_F4: return KKEY_F4;
        case SDLK_F5: return KKEY_F5;
        case SDLK_F6: return KKEY_F6;
        case SDLK_F7: return KKEY_F7;
        case SDLK_F8: return KKEY_F8;

        case SDLK_a: return KKEY_A;
        case SDLK_b: return KKEY_B;
        case SDLK_c: return KKEY_C;
        case SDLK_d: return KKEY_D;
        case SDLK_e: return KKEY_E;
        case SDLK_f: return KKEY_F;
        case SDLK_g: return KKEY_G;
        case SDLK_h: return KKEY_H;
        case SDLK_i: return KKEY_I;
        case SDLK_j: return KKEY_J;
        case SDLK_k: return KKEY_K;
        case SDLK_l: return KKEY_L;
        case SDLK_m: return KKEY_M;
        case SDLK_n: return KKEY_N;
        case SDLK_o: return KKEY_O;
        case SDLK_p: return KKEY_P;
        case SDLK_q: return KKEY_Q;
        case SDLK_r: return KKEY_R;
        case SDLK_s: return KKEY_S;
        case SDLK_t: return KKEY_T;
        case SDLK_u: return KKEY_U;
        case SDLK_v: return KKEY_V;
        case SDLK_w: return KKEY_W;
        case SDLK_x: return KKEY_X;
        case SDLK_y: return KKEY_Y;
        case SDLK_z: return KKEY_Z;

        default: return KKEY_UNKNOWN;
    }
}

static kKeymod translate_mod(Uint16 mod)
{
    kKeycode m = KMOD_NONE;
    if (mod & KMOD_SHIFT) m |= KMOD_SHIFT;
    if (mod & KMOD_CTRL)  m |= KMOD_CTRL;
    if (mod & KMOD_ALT)   m |= KMOD_ALT;
    return m;
}

int kPollEvent(kEvent* ev)
{
    SDL_Event sdl_ev;
    if (!SDL_PollEvent(&sdl_ev)) return 0;

    switch (sdl_ev.type)
    {
        case SDL_QUIT:
            ev->type = KEVENT_QUIT;
            break;

        case SDL_KEYDOWN:
            ev->type = KEVENT_KEYDOWN;
            ev->key.sym = translate_key(sdl_ev.key.keysym.sym);
            ev->key.mod = translate_mod(sdl_ev.key.keysym.mod);
            break;

        case SDL_KEYUP:
            ev->type = KEVENT_KEYUP;
            ev->key.sym = translate_key(sdl_ev.key.keysym.sym);
            ev->key.mod = translate_mod(sdl_ev.key.keysym.mod);
            break;

        case SDL_MOUSEBUTTONDOWN:
            ev->type = KEVENT_MOUSEBUTTONDOWN;
            ev->button.x = sdl_ev.button.x;
            ev->button.y = sdl_ev.button.y;
            ev->button.button = sdl_ev.button.button; // already matches
            break;

        case SDL_MOUSEBUTTONUP:
            ev->type = KEVENT_MOUSEBUTTONUP;
            ev->button.x = sdl_ev.button.x;
            ev->button.y = sdl_ev.button.y;
            ev->button.button = sdl_ev.button.button;
            break;

        case SDL_MOUSEMOTION:
            ev->type = KEVENT_MOUSEMOTION;
            ev->motion.x = sdl_ev.motion.x;
            ev->motion.y = sdl_ev.motion.y;
            ev->motion.dx = sdl_ev.motion.xrel;
            ev->motion.dy = sdl_ev.motion.yrel;
            break;

        case SDL_MOUSEWHEEL:
            ev->type = KEVENT_MOUSEWHEEL;
            ev->wheel.x = sdl_ev.wheel.x;
            ev->wheel.y = sdl_ev.wheel.y;
            break;

        case SDL_TEXTINPUT:
            ev->type = KEVENT_TEXTINPUT;
            size_t len = strlen(sdl_ev.text.text);
            size_t copy_len = (len < KTEXTINPUTEVENT_TEXT_SIZE - 1) ? len : KTEXTINPUTEVENT_TEXT_SIZE;
            memcpy(ev->text.text, sdl_ev.text.text, copy_len);
            ev->text.text[copy_len] = '\0';
            break;

        default:
            ev->type = KEVENT_NONE;
            break;
    }
    return 1;
}