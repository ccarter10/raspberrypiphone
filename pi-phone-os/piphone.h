#ifndef PIPHONE_H
#define PIPHONE_H

#include <stdint.h>
#include <pthread.h>

/* ============================================================
 *  PI-PHONE OS — Central Header
 *  Declares all functions and shared state across modules.
 * ============================================================ */

/* --- GLOBAL HARDWARE FILE DESCRIPTORS --- */
extern int modem_fd;
extern int battery_fd;

/* --- BATTERY / POWER (battery.c) ---
 * Uses MAX17048 fuel gauge over I2C (/dev/i2c-1)
 */
int   battery_init(void);
float get_battery_percentage(int fd);

/* --- MODEM / RADIO (modem.c) ---
 * SIM7600E-H over serial. AT command port is typically /dev/ttyUSB2.
 * NOTE: ttyUSB0 is the diagnostic port, ttyUSB2 is AT commands.
 */
int  modem_init(const char* device_path);
void make_call(int fd, const char* phone_number);
void end_call(int fd);
void answer_call(int fd);
void send_sms(int fd, const char* phone_number, const char* message);

/* --- DISPLAY (display.c) ---
 * LVGL bound to Linux framebuffer (/dev/fb0)
 */
int display_init(void);

/* --- TOUCH (touch.c) ---
 * LVGL evdev driver bound to /dev/input/event0
 */
int touch_init(void);

/* --- UI (ui.c) ---
 * Builds the LVGL widget tree and handles user input events
 */
void build_phone_ui(void);
void update_status_bar(float battery_pct);

/* --- MAIN / THREADS (main.c) ---
 * Background thread that listens for unsolicited modem events
 * (incoming calls, new SMS, signal changes, etc.)
 */
void* radio_thread_func(void* arg);
void  run_ui_loop(void);

#endif /* PIPHONE_H */
