#ifndef SDL_PORT_H
#define SDL_PORT_H

#include "lvgl/lvgl.h"
#include <SDL2/SDL.h>

/* Initialize SDL2 display window (1024x600) and register LVGL display driver */
void sdl_disp_init(void);

/* Initialize SDL2 keypad input and register LVGL keypad driver */
void sdl_indev_init(void);

/* Handle SDL keyboard events and map them to LVGL key codes */
void sdl_handle_key_event(SDL_Event *event);

#endif /* SDL_PORT_H */
