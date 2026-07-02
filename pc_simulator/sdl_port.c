#include "sdl_port.h"
#include <stdio.h>

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 600

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture  *texture  = NULL;
static uint32_t      texture_buffer[WINDOW_WIDTH * WINDOW_HEIGHT];

/* LVGL display buffers */
static lv_disp_draw_buf_t disp_buf;
static lv_color_t          buf1[WINDOW_WIDTH * WINDOW_HEIGHT];

/* Keypad input tracking */
static uint32_t      last_key    = 0;
static bool          key_pressed = false;
static lv_indev_drv_t indev_drv;
static lv_indev_t    *keypad_indev = NULL;

/* -----------------------------------------------------------------------
 * SDL Display Flush Callback
 * Converts LVGL's RGB565 pixel buffer to ARGB8888 and uploads to SDL texture.
 * ----------------------------------------------------------------------- */
static void sdl_disp_flush(lv_disp_drv_t *disp_drv,
                            const lv_area_t *area,
                            lv_color_t *color_p)
{
    int32_t x, y;
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            lv_color_t c = *color_p;
            /* Convert RGB565 to 32-bit ARGB8888 for SDL */
            uint32_t r = ((c.full >> 11) & 0x1F) * 255 / 31;
            uint32_t g = ((c.full >>  5) & 0x3F) * 255 / 63;
            uint32_t b = ( c.full        & 0x1F) * 255 / 31;
            texture_buffer[y * WINDOW_WIDTH + x] =
                (0xFF << 24) | (r << 16) | (g << 8) | b;
            color_p++;
        }
    }

    if (lv_disp_flush_is_last(disp_drv)) {
        SDL_UpdateTexture(texture, NULL, texture_buffer,
                          WINDOW_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    lv_disp_flush_ready(disp_drv);
}

/* -----------------------------------------------------------------------
 * sdl_disp_init
 * Creates an SDL window, renderer and texture, then registers an LVGL display
 * driver backed by a full-screen single buffer.
 * ----------------------------------------------------------------------- */
void sdl_disp_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow(
        "Riverdi STM32H7 1024x600 LVGL Simulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return;
    }

    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture error: %s\n", SDL_GetError());
        return;
    }

    /* Register LVGL display driver */
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, WINDOW_WIDTH * WINDOW_HEIGHT);

    static lv_disp_drv_t drv;
    lv_disp_drv_init(&drv);
    drv.draw_buf   = &disp_buf;
    drv.flush_cb   = sdl_disp_flush;
    drv.hor_res    = WINDOW_WIDTH;
    drv.ver_res    = WINDOW_HEIGHT;
    lv_disp_drv_register(&drv);
}

/* -----------------------------------------------------------------------
 * Keypad Input Read Callback (called by LVGL)
 * ----------------------------------------------------------------------- */
static void sdl_keypad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)drv;
    data->state = key_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->key   = last_key;
}

/* -----------------------------------------------------------------------
 * sdl_indev_init
 * Registers an LVGL keypad input driver and creates a default focus group.
 * ----------------------------------------------------------------------- */
void sdl_indev_init(void)
{
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = sdl_keypad_read;
    keypad_indev      = lv_indev_drv_register(&indev_drv);

    /* Create and assign a default LVGL focus group */
    lv_group_t *g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(keypad_indev, g);
}

/* -----------------------------------------------------------------------
 * sdl_handle_key_event
 * Maps SDL keyboard events to LVGL key codes for HMI navigation.
 *
 * Key mapping:
 *   Arrow Up    -> LV_KEY_PREV  (move focus up in sidebar)
 *   Arrow Down  -> LV_KEY_NEXT  (move focus down in sidebar)
 *   Arrow Left  -> LV_KEY_LEFT  (collapse / move left)
 *   Arrow Right -> LV_KEY_RIGHT (expand / move right)
 *   Enter       -> LV_KEY_ENTER (select / confirm)
 *   Escape      -> LV_KEY_ESC   (back / cancel)
 * ----------------------------------------------------------------------- */
void sdl_handle_key_event(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN) {
        key_pressed = true;
        switch (event->key.keysym.sym) {
            case SDLK_UP:                    last_key = LV_KEY_PREV;  break;
            case SDLK_DOWN:                  last_key = LV_KEY_NEXT;  break;
            case SDLK_LEFT:                  last_key = LV_KEY_LEFT;  break;
            case SDLK_RIGHT:                 last_key = LV_KEY_RIGHT; break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:              last_key = LV_KEY_ENTER; break;
            case SDLK_ESCAPE:                last_key = LV_KEY_ESC;   break;
            default:
                key_pressed = false;
                break;
        }
    } else if (event->type == SDL_KEYUP) {
        key_pressed = false;
    }
}
