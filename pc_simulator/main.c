/*
 * main.c — PC Simulator Entry Point
 *
 * Initializes LVGL, registers the SDL2 display and keypad drivers,
 * calls ui_init() (the EEZ-generated HMI initializer), then runs the
 * main event loop — identical to the bare-metal CM4 while(1) loop but
 * driven by SDL events and usleep() instead of HAL_Delay().
 *
 * Control Keys (mirrors the physical Riverdi bezel buttons):
 *   Arrow Up    -> Move focus UP / previous item
 *   Arrow Down  -> Move focus DOWN / next item
 *   Arrow Left  -> Collapse / move focus left
 *   Arrow Right -> Expand / move focus right
 *   Enter       -> SELECT / confirm focused item
 *   Escape      -> BACK / cancel
 */

#include "lvgl/lvgl.h"
#include "sdl_port.h"
#include "ui.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>   /* usleep */

/* Tick resolution in ms — keeps simulator responsive at ~60 fps */
#define SIM_TICK_MS 10

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /* 1. Initialise LVGL core */
    lv_init();

    /* 2. Bring up the SDL2 display driver (1024 x 600 window) */
    sdl_disp_init();

    /* 3. Bring up the SDL2 keypad input driver */
    sdl_indev_init();

    /* 4. Initialise the EEZ-generated HMI screens */
    ui_init();

    printf("=======================================================\n");
    printf("  Riverdi STM32H7 1024x600 LVGL PC Simulator\n");
    printf("=======================================================\n");
    printf("  Arrow Up/Down  - Navigate focus UP / DOWN\n");
    printf("  Arrow Left     - Collapse / move focus LEFT\n");
    printf("  Arrow Right    - Expand / move focus RIGHT\n");
    printf("  Enter          - SELECT focused widget\n");
    printf("  Escape         - BACK / cancel\n");
    printf("  Close window   - Quit\n");
    printf("=======================================================\n");

    /* 5. Main event loop */
    bool running = true;
    while (running) {
        /* Process SDL window and keyboard events */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else {
                sdl_handle_key_event(&event);
            }
        }

        /* Advance LVGL tick counter */
        lv_tick_inc(SIM_TICK_MS);

        /* Run LVGL timer handler (draws dirty areas, handles animations) */
        lv_timer_handler();

        /* Run the EEZ UI tick (screen-level logic, variable updates) */
        ui_tick();

        /* ~10 ms sleep to cap CPU usage */
        usleep(SIM_TICK_MS * 1000);
    }

    SDL_Quit();
    return 0;
}
