#include <stdio.h>
#include "lvgl/lvgl.h"
#include "piphone.h"

int display_init(void) {
    printf("[DISPLAY] Initialising LVGL...\n");

    lv_init();

    printf("[DISPLAY] Connecting via DRM/KMS (/dev/dri/card0)...\n");
    lv_display_t *disp = lv_linux_drm_create();
    if (disp == NULL) {
        printf("[DISPLAY] Failed to create DRM display.\n");
        return -1;
    }

    lv_linux_drm_set_file(disp, "/dev/dri/card0", -1);

    printf("[DISPLAY] Display ready.\n");
    return 0;
}
