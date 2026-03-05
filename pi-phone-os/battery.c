#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "piphone.h"

// The MAX17048 Battery Fuel Gauge Address
#define BATTERY_I2C_ADDR 0x36 

// The specific "Memory Register" inside the chip that holds the percentage
#define SOC_REGISTER 0x04     

int battery_init(void) {
    int fd;
    
    // 1. Open the I2C bus (The physical pins 3 and 5 on the Pi)
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        printf("[POWER] Error: Could not open I2C bus. Is it enabled in raspi-config?\n");
        return -1;
    }

    // 2. Tell the Pi which chip we want to talk to (Address 0x36)
    if (ioctl(fd, I2C_SLAVE, BATTERY_I2C_ADDR) < 0) {
        printf("[POWER] Error: Could not find the Battery Chip at 0x36.\n");
        close(fd);
        return -1;
    }

    printf("[SYSTEM] Power Management IC initialized.\n");
    return fd;
}

float get_battery_percentage(int fd) {
    if (fd < 0) return 0.0;

    unsigned char buffer[2];
    
    // 1. Tell the chip we want to read the SOC (State of Charge) register
    buffer[0] = SOC_REGISTER;
    if (write(fd, buffer, 1) != 1) {
        return -1.0; // Read error
    }

    // 2. The chip replies with 2 bytes of data
    if (read(fd, buffer, 2) != 2) {
        return -1.0; // Read error
    }

    // 3. Bit-math! Combine the two bytes to get the real percentage
    int raw_percent = (buffer[0] << 8) | buffer[1];
    
    // The chip returns the percentage scaled by 256. 
    float final_percent = (float)raw_percent / 256.0;
    
    return final_percent;
}