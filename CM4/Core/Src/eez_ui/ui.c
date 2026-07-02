#include "ui.h"
#include "lvgl/lvgl.h"

void ui_init() {
    /* Create dark blue background and centered label with text 'TWERD ENERGO-PLUS' */
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x00003B), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "TWERD ENERGO-PLUS");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void ui_tick() {
    // Nothing to do in the dummy loop
}
