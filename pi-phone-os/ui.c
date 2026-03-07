#include <stdio.h>
#include <string.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

/* ============================================================
 *  UI — LVGL Phone Interface
 *
 *  Layout:
 *    ┌─────────────────────┐
 *    │  Status bar          │  ← battery %, signal
 *    │  Number display      │  ← typed/incoming number
 *    │                      │
 *    │  1  2  3             │
 *    │  4  5  6             │  ← dialpad
 *    │  7  8  9             │
 *    │  *  0  #             │
 *    │                      │
 *    │  [CALL]  [END]       │  ← action buttons
 *    └─────────────────────┘
 * ============================================================ */

/* Widgets we need to update from other modules */
static lv_obj_t *status_label   = NULL;
static lv_obj_t *number_display = NULL;
static char dialled_number[32]  = {0};

static const char *keypad_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "*", "0", "#", "\n",
    ""
};

static void keypad_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_VALUE_CHANGED) return;

    lv_obj_t *obj = lv_event_get_target(e);
    uint32_t  id  = lv_btnmatrix_get_selected_btn(obj);
    const char *txt = lv_btnmatrix_get_btn_text(obj, id);
    if (!txt) return;

    /* Append digit to number buffer (max 15 digits — international number length) */
    if (strlen(dialled_number) < 15) {
        strncat(dialled_number, txt, sizeof(dialled_number) - strlen(dialled_number) - 1);
        if (number_display) {
            lv_label_set_text(number_display, dialled_number);
        }
    }

    printf("[UI] Key pressed: %s  |  Buffer: %s\n", txt, dialled_number);
}

static void call_btn_cb(lv_event_t *e) {
    (void)e;
    if (strlen(dialled_number) == 0) {
        printf("[UI] No number entered.\n");
        return;
    }
    printf("[UI] Calling %s...\n", dialled_number);
    make_call(modem_fd, dialled_number);
}

static void end_btn_cb(lv_event_t *e) {
    (void)e;
    printf("[UI] Ending call.\n");
    end_call(modem_fd);
    /* Clear the display */
    memset(dialled_number, 0, sizeof(dialled_number));
    if (number_display) lv_label_set_text(number_display, "");
}

static void answer_btn_cb(lv_event_t *e) {
    (void)e;
    printf("[UI] Answering call.\n");
    answer_call(modem_fd);
}

static void backspace_btn_cb(lv_event_t *e) {
    (void)e;
    int len = strlen(dialled_number);
    if (len > 0) {
        dialled_number[len - 1] = '\0';
        if (number_display) lv_label_set_text(number_display, dialled_number);
    }
}

void build_phone_ui(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), LV_PART_MAIN);

    /* --- STATUS BAR --- */
    lv_obj_t *status_bar = lv_obj_create(scr);
    lv_obj_set_size(status_bar, LV_PCT(100), 30);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x0f0f23), LV_PART_MAIN);
    lv_obj_set_style_border_width(status_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(status_bar, 4, LV_PART_MAIN);

    status_label = lv_label_create(status_bar);
    lv_label_set_text(status_label, "Pi-Phone  |  Battery: --");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xaaaaaa), LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_LEFT_MID, 0, 0);

    /* --- NUMBER DISPLAY --- */
    lv_obj_t *num_box = lv_obj_create(scr);
    lv_obj_set_size(num_box, LV_PCT(100), 50);
    lv_obj_align(num_box, LV_ALIGN_TOP_MID, 0, 35);
    lv_obj_set_style_bg_color(num_box, lv_color_hex(0x16213e), LV_PART_MAIN);
    lv_obj_set_style_border_width(num_box, 0, LV_PART_MAIN);

    number_display = lv_label_create(num_box);
    lv_label_set_text(number_display, "");
    lv_obj_set_style_text_color(number_display, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(number_display, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(number_display, LV_ALIGN_RIGHT_MID, -10, 0);

    /* --- DIALPAD --- */
    lv_obj_t *keypad = lv_btnmatrix_create(scr);
    lv_btnmatrix_set_map(keypad, keypad_map);
    lv_obj_set_size(keypad, LV_PCT(100), 220);
    lv_obj_align(keypad, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_bg_color(keypad, lv_color_hex(0x1a1a2e), LV_PART_MAIN);
    lv_obj_set_style_border_width(keypad, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(keypad, keypad_event_cb, LV_EVENT_ALL, NULL);

    /* --- ACTION BUTTONS ROW --- */
    /* CALL button (green) */
    lv_obj_t *call_btn = lv_button_create(scr);
    lv_obj_set_size(call_btn, 80, 45);
    lv_obj_align(call_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_style_bg_color(call_btn, lv_color_hex(0x27ae60), LV_PART_MAIN);
    lv_obj_add_event_cb(call_btn, call_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *call_lbl = lv_label_create(call_btn);
    lv_label_set_text(call_lbl, "CALL");
    lv_obj_center(call_lbl);

    /* ANSWER button (blue) */
    lv_obj_t *ans_btn = lv_button_create(scr);
    lv_obj_set_size(ans_btn, 80, 45);
    lv_obj_align(ans_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_color(ans_btn, lv_color_hex(0x2980b9), LV_PART_MAIN);
    lv_obj_add_event_cb(ans_btn, answer_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *ans_lbl = lv_label_create(ans_btn);
    lv_label_set_text(ans_lbl, "ANSWER");
    lv_obj_center(ans_lbl);

    /* END button (red) */
    lv_obj_t *end_btn = lv_button_create(scr);
    lv_obj_set_size(end_btn, 80, 45);
    lv_obj_align(end_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_style_bg_color(end_btn, lv_color_hex(0xe74c3c), LV_PART_MAIN);
    lv_obj_add_event_cb(end_btn, end_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *end_lbl = lv_label_create(end_btn);
    lv_label_set_text(end_lbl, "END");
    lv_obj_center(end_lbl);

    /* Backspace button (top-right of number display area) */
    lv_obj_t *bsp_btn = lv_button_create(scr);
    lv_obj_set_size(bsp_btn, 40, 30);
    lv_obj_align(bsp_btn, LV_ALIGN_TOP_RIGHT, -5, 40);
    lv_obj_set_style_bg_color(bsp_btn, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_add_event_cb(bsp_btn, backspace_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *bsp_lbl = lv_label_create(bsp_btn);
    lv_label_set_text(bsp_lbl, "< ");
    lv_obj_center(bsp_lbl);

    printf("[UI] Phone interface built.\n");
}

/* Called from anywhere (e.g. a battery polling timer) to update the status bar */
void update_status_bar(float battery_pct) {
    if (!status_label) return;
    char buf[64];
    if (battery_pct < 0) {
        snprintf(buf, sizeof(buf), "Pi-Phone  |  Battery: N/A");
    } else {
        snprintf(buf, sizeof(buf), "Pi-Phone  |  Battery: %.0f%%", battery_pct);
    }
    lv_label_set_text(status_label, buf);
}
