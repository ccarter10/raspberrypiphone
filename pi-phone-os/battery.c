#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "piphone.h"

/* ============================================================
 *  BATTERY — MAX17048 Fuel Gauge via I2C
 *
 *  Wiring (Pi Zero 2W):
 *    SDA → GPIO 2 (Pin 3)
 *    SCL → GPIO 3 (Pin 5)
 *    VCC → 3.3V   (Pin 1)
 *    GND → GND    (Pin 6)
 *
 *  Enable I2C on the Pi with: sudo raspi-config → Interface Options → I2C
 *  Verify it's visible: sudo i2cdetect -y 1  (should show 0x36)
 * ============================================================ */

#define BATTERY_I2C_BUS  "/dev/i2c-1"
#define BATTERY_I2C_ADDR 0x36  /* MAX17048 fixed address */
#define SOC_REGISTER     0x04  /* State of Charge register */

int battery_init(void) {
    int fd = open(BATTERY_I2C_BUS, O_RDWR);
    if (fd < 0) {
        printf("[POWER] Cannot open %s. Is I2C enabled? (sudo raspi-config)\n", BATTERY_I2C_BUS);
        return -1;
    }

    if (ioctl(fd, I2C_SLAVE, BATTERY_I2C_ADDR) < 0) {
        printf("[POWER] MAX17048 not found at 0x%02X. Check wiring.\n", BATTERY_I2C_ADDR);
        close(fd);
        return -1;
    }

    printf("[POWER] Battery fuel gauge (MAX17048) initialised.\n");
    return fd;
}

float get_battery_percentage(int fd) {
    if (fd < 0) return -1.0f;

    unsigned char buf[2];

    /* Point to SOC register */
    buf[0] = SOC_REGISTER;
    if (write(fd, buf, 1) != 1) {
        printf("[POWER] Failed to write SOC register address.\n");
        return -1.0f;
    }

    /* Read 2 bytes: MSB then LSB */
    if (read(fd, buf, 2) != 2) {
        printf("[POWER] Failed to read SOC register.\n");
        return -1.0f;
    }

    /* MAX17048 returns percentage * 256 across 2 bytes */
    int raw = (buf[0] << 8) | buf[1];
    float pct = (float)raw / 256.0f;

    /* Clamp to valid range */
    if (pct > 100.0f) pct = 100.0f;
    if (pct < 0.0f)   pct = 0.0f;

    return pct;
}
