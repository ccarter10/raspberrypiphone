#ifndef PIPHONE_H
#define PIPHONE_H

#include <stdint.h>
#include <pthread.h>

/* --- GLOBAL HARDWARE PIPES --- */
extern int modem_fd;
extern int battery_fd;

/* --- 1. HARDWARE MEMORY MAPS --- */
#define BCM2837_PERI_BASE 0x3f000000 
#define GPIO_LEN 0xB4

/* --- 2. HARDWARE & POWER FUNCTIONS --- */
int hw_init(void);
int battery_init(void);
float get_battery_percentage(int fd);

/* --- 3. RADIO & MODEM FUNCTIONS --- */
int modem_init(const char* device_path);
void make_call(int fd, const char* phone_number);
void end_call(int fd);
void send_sms(int fd, const char* phone_number, const char* message);

/* --- 4. SYSTEM & UI FUNCTIONS --- */
void* radio_thread_func(void* arg);
void run_ui_loop(void);
void build_phone_ui(void);
int display_init(void);
int touch_init(void);
void update_status_bar(float battery_pct);

#endif