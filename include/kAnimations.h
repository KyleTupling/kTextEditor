#pragma once
#include <stdbool.h>
#include <SDL.h>

// Interpolation function type
typedef float (*Interpolator)(float t);

// Linear interpolator (default)
static inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

typedef struct {
    SDL_Color start_color;
    SDL_Color end_color;
    SDL_Color current_color;

    float duration;      // seconds
    float elapsed;       // seconds
    bool active;       
    bool forward;
    Interpolator interp; // optional custom interpolator
} ColorTransition;

/**
 * Initialises a ColorTransition.
 * 
 * @param ct Pointer to which the initialised ColorTransition will be passed
 * @param start SDL color to start with
 * @param end SDL color to end with
 * @param duration Duration of transition in seconds
 * @param interp Optional interpolator (can be NULL, linear interpolation will be used)
 */
static inline void color_transition_init(ColorTransition* ct, SDL_Color start, SDL_Color end, float duration, Interpolator interp)
{
    ct->start_color   = start;
    ct->end_color     = end;
    ct->current_color = start;
    ct->duration      = duration;
    ct->elapsed       = 0.0f;
    ct->active        = false;
    ct->forward       = true;
    ct->interp        = interp;
}

/**
 * Starts the transition.
 * 
 * @param ct The ColorTransition to start
 * @param forward Whether the transition should go from start to end, or end to start
 */
static inline void color_transition_start(ColorTransition* ct, bool forward)
{
    ct->active = true;
    //ct->elapsed = 0.0f;
    ct->forward = forward;
}

/**
 * Updates the transition state according to time passed.
 * 
 * @param ct The ColorTransition to update
 * @param dt Time in seconds since last frame
 */
static inline void color_transition_update(ColorTransition* ct, float dt)
{
    if (!ct->active) return;

    ct->elapsed += dt;
    float t = ct->elapsed / ct->duration;
    if (t >= 1.0f)
    {
        t = 1.0f;
        ct->active = false;
        ct->elapsed = 0.0f;
    }

    if (ct->interp)
    {
        t = ct->interp(t);
    }

    SDL_Color from = ct->forward ? ct->start_color : ct->end_color;
    SDL_Color to   = ct->forward ? ct->end_color   : ct->start_color;

    ct->current_color.r = (Uint8)lerp(from.r, to.r, t);
    ct->current_color.g = (Uint8)lerp(from.g, to.g, t);
    ct->current_color.b = (Uint8)lerp(from.b, to.b, t);
    ct->current_color.a = (Uint8)lerp(from.a, to.a, t);
}

/**
 * Gets the current colour.
 * 
 * @param ct The ColorTransition to get the colour from
 * 
 * @return The SDL color
 */
static inline SDL_Color color_transition_get(ColorTransition* ct)
{
    return ct->current_color;
}