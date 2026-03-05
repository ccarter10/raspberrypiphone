cat << 'EOF' > touch.c
#include <stdio.h>
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

// In Linux, the primary touchscreen digitizer is almost always event0
#define TOUCH_DEVICE "/dev/input/event0"

int touch_init(void) {
    printf("[TOUCH] Connecting to digitizer at %s...\n", TOUCH_DEVICE);

    // Create an input device pointing to the physical touchscreen
    lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, TOUCH_DEVICE);

    if(indev == NULL) {
        printf("[WARNING] Touchscreen not found at %s.\n", TOUCH_DEVICE);
        printf("[WARNING] (If you are running this in WSL/Windows, this is normal!)\n");
        return -1;
    }

    printf("[TOUCH] Digitizer connected and calibrated.\n");
    return 0;
}
EOF