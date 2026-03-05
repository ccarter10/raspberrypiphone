# 1. THE BLUEPRINT (piphone.h)
cat << 'EOF' > piphone.h
#ifndef PIPHONE_H
#define PIPHONE_H

#include <stdint.h>
#include <pthread.h>

/* --- 1. HARDWARE MEMORY MAPS --- */
// Base Address for Raspberry Pi Zero 2 W (BCM2837 processor)
#define BCM2837_PERI_BASE 0x3f000000 
#define GPIO_LEN 0xB4

/* --- 2. HARDWARE & POWER FUNCTIONS --- */
int hw_init(void);
int battery_init(void);                     // NEW: Wakes up the I2C battery chip
float get_battery_percentage(int fd);       // NEW: Asks the chip for the fuel level

/* --- 3. RADIO & MODEM FUNCTIONS --- */
int modem_init(const char* device_path);
void make_call(int fd, const char* phone_number);
void end_call(int fd);
void send_sms(int fd, const char* phone_number, const char* message);

/* --- 4. SYSTEM & UI FUNCTIONS --- */
void* radio_thread_func(void* arg);
void run_ui_loop(void);
void build_phone_ui(void);

# Append the new function to the bottom of piphone.h
echo "int display_init(void);" >> piphone.h

#endif
EOFint touch_init(void);
