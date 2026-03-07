#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include "piphone.h"
#include "lvgl/lvgl.h"

/* ============================================================
 *  PI-PHONE OS — Main Entry Point
 *  Boot sequence, background radio thread, and UI loop.
 * ============================================================ */

/* Global file descriptors — defined here, declared extern in piphone.h */
int modem_fd   = -1;
int battery_fd = -1;

/* The AT port on the SIM7600E is ttyUSB2, not ttyUSB0.
 * ttyUSB0 = diagnostic/NMEA, ttyUSB1 = NMEA, ttyUSB2 = AT commands */
#define MODEM_PORT "/dev/ttyUSB2"

/* ----------------------------------------------------------------
 * BACKGROUND RADIO LISTENER THREAD
 *
 * Uses select() with a timeout instead of a blocking read().
 * This means the thread never hangs — it checks the modem every
 * 100ms and yields properly if there's nothing to read.
 * ---------------------------------------------------------------- */
void* radio_thread_func(void* arg) {
    (void)arg;
    char buffer[512];
    fd_set read_fds;
    struct timeval tv;

    printf("[RADIO] Background listener active.\n");

    while (1) {
        if (modem_fd < 0) {
            usleep(500000); /* Modem not ready yet, wait 500ms */
            continue;
        }

        /* Reset the fd set and timeout on each iteration */
        FD_ZERO(&read_fds);
        FD_SET(modem_fd, &read_fds);
        tv.tv_sec  = 0;
        tv.tv_usec = 100000; /* 100ms timeout */

        int ready = select(modem_fd + 1, &read_fds, NULL, NULL, &tv);

        if (ready < 0) {
            /* select() error — modem may have disconnected */
            perror("[RADIO] select() error");
            usleep(1000000);
            continue;
        }

        if (ready == 0) {
            /* Timeout — nothing to read, loop back */
            continue;
        }

        /* Data is available — read it */
        int n = read(modem_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) continue;
        buffer[n] = '\0';

        /* --- Parse unsolicited modem responses --- */

        if (strstr(buffer, "RING")) {
            /* Incoming call. +CLIP line will follow if AT+CLIP=1 was sent at init */
            printf("\n[ALERT] INCOMING CALL\n");
        }

        if (strstr(buffer, "+CLIP:")) {
            /* Caller ID — format: +CLIP: "+447911123456",145 */
            char *start = strchr(buffer, '"');
            if (start) {
                char *end = strchr(start + 1, '"');
                if (end) {
                    *end = '\0';
                    printf("[ALERT] Caller ID: %s\n", start + 1);
                }
            }
        }

        if (strstr(buffer, "+CMTI:")) {
            /* New SMS notification — format: +CMTI: "SM",1 */
            printf("\n[ALERT] NEW SMS RECEIVED\n");
        }

        if (strstr(buffer, "NO CARRIER")) {
            printf("[RADIO] Call ended (NO CARRIER)\n");
        }

        if (strstr(buffer, "+CSQ:")) {
            /* Signal strength report — format: +CSQ: 18,0 */
            int rssi = 0;
            sscanf(strstr(buffer, "+CSQ:"), "+CSQ: %d", &rssi);
            printf("[RADIO] Signal strength: %d/31\n", rssi);
        }
    }

    return NULL;
}

/* ----------------------------------------------------------------
 * UI LOOP
 * Hands control to LVGL. Runs forever, sleeping 5ms between frames
 * to avoid burning the CPU (important on a Pi Zero 2W).
 * ---------------------------------------------------------------- */
void run_ui_loop(void) {
    printf("[UI] Starting graphical interface...\n");
    build_phone_ui();

    while (1) {
        lv_timer_handler();
        usleep(5000); /* 5ms ≈ 200fps ceiling, LVGL self-limits to ~30fps */
    }
}

/* ----------------------------------------------------------------
 * BOOT SEQUENCE
 * ---------------------------------------------------------------- */
int main(void) {
    printf("==========================================\n");
    printf("    PI-PHONE OS  —  Pi Zero 2W Build     \n");
    printf("==========================================\n\n");

    /* 1. Initialise the display (LVGL + /dev/fb0) */
    if (display_init() < 0) {
        fprintf(stderr, "[FATAL] Display failed to initialise. Halting.\n");
        return 1;
    }

    /* 2. Initialise the touchscreen (/dev/input/event0) */
    touch_init(); /* Non-fatal — system still usable without touch */

    /* 3. Initialise the battery fuel gauge (I2C) */
    battery_fd = battery_init();
    if (battery_fd < 0) {
        printf("[WARNING] Battery monitor unavailable. Continuing.\n");
    } else {
        float pct = get_battery_percentage(battery_fd);
        printf("[POWER] Battery: %.1f%%\n", pct);
    }

    /* 4. Initialise the SIM7600E modem
     *    The modem needs ~3 seconds after power-on before accepting AT commands.
     *    If this is a cold boot, the sleep here prevents a failed handshake. */
    printf("[MODEM] Waiting for modem to settle...\n");
    sleep(3);
    modem_fd = modem_init(MODEM_PORT);
    if (modem_fd < 0) {
        printf("[WARNING] Modem unavailable. Voice/SMS disabled.\n");
        printf("[WARNING] Check: is SIM7600E powered? Is port %s correct?\n", MODEM_PORT);
    }

    /* 5. Launch the background radio listener thread */
    pthread_t radio_thread;
    if (pthread_create(&radio_thread, NULL, radio_thread_func, NULL) != 0) {
        fprintf(stderr, "[FATAL] Failed to create radio thread. Halting.\n");
        return 1;
    }
    pthread_detach(radio_thread); /* Thread manages its own lifecycle */

    /* 6. Hand control to the UI loop (never returns) */
    run_ui_loop();

    return 0;
}
