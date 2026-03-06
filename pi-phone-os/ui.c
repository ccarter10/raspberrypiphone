#include <stdio.h>
#include <string.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

static const char * keypad_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "*", "0", "#", "\n",
    "CALL", "END", ""
};

static void keypad_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);

        printf("[UI] Touchscreen Input: %s\n", txt);

        if(strcmp(txt, "CALL") == 0) {
            printf("[UI] Initiating call...\n");
            // make_call(modem_fd, "+447000000000"); 
        }
        else if(strcmp(txt, "END") == 0) {
            printf("[UI] Ending call...\n");
            // end_call(modem_fd);
        }
    }
}

void build_phone_ui(void) {
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a1a), LV_PART_MAIN);

    lv_obj_t * keypad = lv_btnmatrix_create(scr);
    lv_btnmatrix_set_map(keypad, keypad_map);
    
    lv_obj_set_size(keypad, 240, 320);
    lv_obj_center(keypad);

    lv_obj_add_event_cb(keypad, keypad_event_cb, LV_EVENT_ALL, NULL);
    
    printf("[UI] Graphical Keypad initialized.\n");
}