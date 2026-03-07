#include <stdio.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

/* ============================================================
 *  DISPLAY — LVGL bound to Linux Framebuffer
 *
 *  Uses LVGL's built-in fbdev driver which writes directly to
 *  /dev/fb0. No X11, no Wayland, no GPU needed.
 *
 *  Ensure the Pi is NOT booting to desktop (use CLI boot in
 *  raspi-config) so /dev/fb0 is free for us to use.
 * ============================================================ */

int display_init(void) {
    printf("[DISPLAY] Initialising LVGL...\n");

    lv_init();

    printf("[DISPLAY] Connecting to framebuffer /dev/fb1 (SunFounder SPI)...\n");
    lv_display_t *disp = lv_linux_fbdev_create();
    if (disp == NULL) {
        printf("[DISPLAY] Failed to create fbdev display.\n");
        printf("[DISPLAY] Is the screen connected? Is /dev/fb0 available?\n");
        return -1;
    }

    /* SunFounder 3.5" SPI display creates /dev/fb1 via the kernel overlay.
     * (A plain HDMI display would be /dev/fb0 instead.) */
    lv_linux_fbdev_set_file(disp, "/dev/fb1");

    printf("[DISPLAY] Framebuffer display ready.\n");
    return 0;
}
