#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "piphone.h"

/* MAX17048 Battery Fuel Gauge I2C address */
#define BATTERY_I2C_ADDR 0x36

/* State of Charge register */
#define SOC_REGISTER     0x04

int battery_init(void) {
    int fd;

    /* Open the I2C bus (pins 3 and 5 on the Pi) */
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        printf("[POWER] Error: Could not open I2C bus. Is it enabled in raspi-config?\n");
        return -1;
    }

    /* Select the fuel gauge chip at address 0x36 */
    if (ioctl(fd, I2C_SLAVE, BATTERY_I2C_ADDR) < 0) {
        printf("[POWER] Error: Could not find the Battery Chip at 0x36.\n");
        close(fd);
        return -1;
    }

    printf("[SYSTEM] Power Management IC initialized.\n");
    return fd;
}

float get_battery_percentage(int fd) {
    if (fd < 0) return 0.0f;

    unsigned char buffer[2];

    /* Request the SOC register */
    buffer[0] = SOC_REGISTER;
    if (write(fd, buffer, 1) != 1) {
        return -1.0f;
    }

    /* Read 2 bytes back from the chip */
    if (read(fd, buffer, 2) != 2) {
        return -1.0f;
    }

    /* Combine bytes; chip returns percentage scaled by 256 */
    int raw_percent = (buffer[0] << 8) | buffer[1];
    float final_percent = (float)raw_percent / 256.0f;

    return final_percent;
}