#include "lvgl/lvgl.h"
#include <stdio.h>

// This function builds the physical look of the phone screen
void build_phone_ui(void) {
    // 1. Get the current active screen
    lv_obj_t * screen = lv_scr_act();
    
    // 2. Set the background color to black (Hex 0x000000)
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);

    // 3. Create a Button Object
    lv_obj_t * dial_btn = lv_btn_create(screen);
    lv_obj_set_size(dial_btn, 120, 50);          // Width, Height
    lv_obj_align(dial_btn, LV_ALIGN_BOTTOM_MID, 0, -20); // Center bottom
    
    // 4. Change button color to Green (0x00FF00)
    lv_obj_set_style_bg_color(dial_btn, lv_color_hex(0x00FF00), LV_PART_MAIN);

    // 5. Add Text to the Button
    lv_obj_t * label = lv_label_create(dial_btn);
    lv_label_set_text(label, "DIAL");
    lv_obj_center(label); // Center the text inside the button
}