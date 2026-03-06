#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "piphone.h"

int modem_init(const char* device_path) {
    printf("[MODEM] Attempting to connect to %s...\n", device_path);
    int fd = open(device_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("[ERROR] Modem not found at %s.\n", device_path);
        return -1;
    }
    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    tty.c_lflag = 0;
    tcsetattr(fd, TCSANOW, &tty);

    write(fd, "AT\r\n", 4);
    usleep(500000);

    char buffer[256] = {0};
    read(fd, buffer, sizeof(buffer)-1);

    if (strstr(buffer, "OK")) {
        printf("[MODEM] Handshake SUCCESS. Radio is active.\n");
        return fd;
    } else {
        printf("[MODEM] Handshake FAILED.\n");
        return -1;
    }
}

void make_call(int fd, const char* phone_number) {
    if (fd < 0) return;
    char command[64];
    snprintf(command, sizeof(command), "ATD%s;\r\n", phone_number);
    printf("[RADIO] Dialing: %s", command);
    write(fd, command, strlen(command));
}

void end_call(int fd) {
    if (fd < 0) return;
    printf("[RADIO] Hanging up...\n");
    write(fd, "ATH\r\n", 5);
}

void send_sms(int fd, const char* phone_number, const char* message) {
    if (fd < 0) return;
    char command[64];
    
    printf("[SMS] Setting Text Mode...\n");
    write(fd, "AT+CMGF=1\r\n", 11);
    usleep(200000);

    printf("[SMS] Prepping message to %s\n", phone_number);
    snprintf(command, sizeof(command), "AT+CMGS=\"%s\"\r\n", phone_number);
    write(fd, command, strlen(command));
    usleep(500000);

    printf("[SMS] Sending payload...\n");
    write(fd, message, strlen(message));
    char ctrl_z = 0x1A; // The Hex command to dispatch the text
    write(fd, &ctrl_z, 1);
    printf("[SMS] Dispatched!\n");
}