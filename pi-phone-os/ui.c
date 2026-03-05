cat << 'EOF' > ui.c
#include <stdio.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

// The map of our keypad layout
static const char * keypad_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "*", "0", "#", "\n",
    "CALL", "END", ""
};

// The Callback: What happens when a button is touched?
static void keypad_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);

        printf("[UI] Touchscreen Input: %s\n", txt);

        // If they press CALL, trigger the modem!
        // (Hardcoded number for now, we can add a text display later)
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

// The Setup: Building the visual screen
void build_phone_ui(void) {
    // 1. Get the current active screen
    lv_obj_t * scr = lv_scr_act();
    
    // 2. Set the background color to a dark grey
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a1a), LV_PART_MAIN);

    // 3. Create the Keypad Button Matrix
    lv_obj_t * keypad = lv_btnmatrix_create(scr);
    lv_btnmatrix_set_map(keypad, keypad_map);
    
    // 4. Size and position it perfectly in the center of the screen
    lv_obj_set_size(keypad, 240, 320);
    lv_obj_center(keypad);

    // 5. Tell it to listen for touches
    lv_obj_add_event_cb(keypad, keypad_event_cb, LV_EVENT_ALL, NULL);
    
    printf("[UI] Graphical Keypad initialized.\n");
}
EOF