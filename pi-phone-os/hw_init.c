#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "piphone.h"

static volatile uint32_t *gpio_map;

int hw_init(void) {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        printf("[FATAL] Can't open /dev/mem. Are you running as root (sudo)?\n");
        return -1;
    }

    /* Map the BCM2837 GPIO peripheral block into process memory */
    gpio_map = (uint32_t *)mmap(NULL, GPIO_LEN, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, mem_fd, BCM2837_PERI_BASE);
    close(mem_fd);

    if (gpio_map == MAP_FAILED) {
        printf("[FATAL] Memory map failed!\n");
        return -1;
    }

    printf("[SYSTEM] Hardware Layer initialized. Pi Zero 2 W GPIO mapped.\n");
    return 0;
}