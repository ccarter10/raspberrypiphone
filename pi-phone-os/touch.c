#include <stdio.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

/* ============================================================
 *  TOUCH — LVGL evdev driver bound to touchscreen digitizer
 *
 *  The primary touchscreen is usually /dev/input/event0.
 *  If you have multiple input devices, check with:
 *    cat /proc/bus/input/devices
 *  Look for your touchscreen and note its event number.
 *
 *  Touch failure is non-fatal — the system boots without it.
 * ============================================================ */

#define TOUCH_DEVICE "/dev/input/event0"

int touch_init(void) {
    printf("[TOUCH] Connecting to %s...\n", TOUCH_DEVICE);

    lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, TOUCH_DEVICE);
    if (indev == NULL) {
        printf("[TOUCH] Touchscreen not found at %s.\n", TOUCH_DEVICE);
        printf("[TOUCH] Continuing without touch input.\n");
        return -1;
    }

    printf("[TOUCH] Touchscreen ready.\n");
    return 0;
}
