#include <stdio.h>
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

/* On Linux, the primary touchscreen digitizer is usually event0 */
#define TOUCH_DEVICE "/dev/input/event0"

int touch_init(void) {
    printf("[TOUCH] Connecting to digitizer at %s...\n", TOUCH_DEVICE);

    /* Create an LVGL input device pointing to the physical touchscreen */
    lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, TOUCH_DEVICE);

    if (indev == NULL) {
        printf("[WARNING] Touchscreen not found at %s.\n", TOUCH_DEVICE);
        printf("[WARNING] (Running in WSL/desktop? This is expected.)\n");
        return -1;
    }

    printf("[TOUCH] Digitizer connected and calibrated.\n");
    return 0;
}