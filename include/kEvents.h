#pragma once

#define KTEXTINPUTEVENT_TEXT_SIZE 32

typedef enum {
    KEVENT_NONE = 0,
    KEVENT_QUIT,
    KEVENT_KEYDOWN,
    KEVENT_KEYUP,
    KEVENT_MOUSEBUTTONDOWN,
    KEVENT_MOUSEBUTTONUP,
    KEVENT_MOUSEMOTION,
    KEVENT_MOUSEWHEEL,
    KEVENT_TEXTINPUT
} kEventType;

typedef enum {
    KKEY_UNKNOWN = 0,
    KKEY_LEFT,
    KKEY_RIGHT,
    KKEY_UP,
    KKEY_DOWN,
    KKEY_RETURN,
    KKEY_BACKSPACE,
    KKEY_DELETE,
    KKEY_TAB,
    KKEY_ESCAPE,

    KKEY_F1,
    KKEY_F2,
    KKEY_F3,
    KKEY_F4,
    KKEY_F5,
    KKEY_F6,
    KKEY_F7,
    KKEY_F8,

    KKEY_A,
    KKEY_B,
    KKEY_C,
    KKEY_D,
    KKEY_E,
    KKEY_F,
    KKEY_G,
    KKEY_H,
    KKEY_I,
    KKEY_J,
    KKEY_K,
    KKEY_L,
    KKEY_M,
    KKEY_N,
    KKEY_O,
    KKEY_P,
    KKEY_Q,
    KKEY_R,
    KKEY_S,
    KKEY_T,
    KKEY_U,
    KKEY_V,
    KKEY_W,
    KKEY_X,
    KKEY_Y,
    KKEY_Z,
} kKeycode;

// Modifier flags
typedef enum {
    KKEYMOD_NONE  = 0,
    KKEYMOD_SHIFT = 1 << 0,
    KKEYMOD_CTRL  = 1 << 1,
    KKEYMOD_ALT   = 1 << 2
} kKeymod;

typedef enum {
    KMOUSEBUTTON_LEFT   = 1,
    KMOUSEBUTTON_MIDDLE = 2,
    KMOUSEBUTTON_RIGHT  = 3,
} kMouseButton;

typedef struct {
    int x, y;
    kMouseButton button;
    int clicks;
} kMouseButtonEvent;

typedef struct {
    int x, y;
    kKeymod mod;
} kMouseWheelEvent;

typedef struct {
    kEventType type;
    union {
        struct {
            kKeycode sym;
            kKeymod  mod;
        } key;

        kMouseButtonEvent button;

        struct {
            int x, y;
            int dx, dy; // relative
        } motion;

        kMouseWheelEvent wheel;

        struct {
            char text[KTEXTINPUTEVENT_TEXT_SIZE];
        } text;
    };
} kEvent;

// Event poll function
int kPollEvent(kEvent* ev);
