#include <stdio.h>
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

int display_init(void) {
    printf("[DISPLAY] Connecting LVGL to Linux Framebuffer (/dev/fb0)...\n");

    // 1. Initialize the LVGL core engine
    lv_init();

    // 2. Create the display output using LVGL's built-in Linux driver
    lv_display_t * disp = lv_linux_fbdev_create();
    if(disp == NULL) {
        printf("[FATAL] Failed to create LVGL display. Is your screen plugged in?\n");
        return -1;
    }

    // 3. Point the engine to the Pi's raw memory buffer
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    printf("[DISPLAY] Screen connected successfully.\n");
    return 0;
}
