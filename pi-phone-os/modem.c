#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "piphone.h"

/* ============================================================
 *  MODEM — SIM7600E-H AT Command Interface
 *
 *  SIM7600E USB port map (when connected via USB):
 *    /dev/ttyUSB0 — Diagnostic / NMEA GPS
 *    /dev/ttyUSB1 — NMEA GPS
 *    /dev/ttyUSB2 — AT commands  ← USE THIS ONE
 *    /dev/ttyUSB3 — Audio (PCM)
 *
 *  All functions are safe to call with fd < 0 (modem unavailable).
 * ============================================================ */

/* Helper: send an AT command and wait for a response.
 * Returns 1 if 'expected' string appears in response, 0 otherwise. */
static int at_cmd(int fd, const char* cmd, const char* expected, int wait_ms) {
    if (fd < 0) return 0;

    write(fd, cmd, strlen(cmd));
    usleep(wait_ms * 1000);

    char buf[512] = {0};
    read(fd, buf, sizeof(buf) - 1);

    if (expected && strstr(buf, expected)) {
        return 1;
    }
    return 0;
}

int modem_init(const char* device_path) {
    printf("[MODEM] Opening %s...\n", device_path);

    int fd = open(device_path, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("[MODEM] Cannot open %s. Check device is connected.\n", device_path);
        return -1;
    }

    /* Configure serial port: 115200 8N1, no flow control */
    struct termios tty = {0};
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag  = (tty.c_cflag & ~CSIZE) | CS8; /* 8-bit chars */
    tty.c_cflag |= CLOCAL | CREAD;               /* Enable receiver */
    tty.c_cflag &= ~(PARENB | PARODD);           /* No parity */
    tty.c_cflag &= ~CSTOPB;                      /* 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;                     /* No hardware flow control */
    tty.c_lflag  = 0;                            /* Raw mode */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     /* No software flow control */
    tty.c_oflag  = 0;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 1; /* 100ms read timeout */
    tcsetattr(fd, TCSANOW, &tty);

    /* Basic handshake */
    if (!at_cmd(fd, "AT\r\n", "OK", 500)) {
        printf("[MODEM] Handshake failed. No response from modem.\n");
        close(fd);
        return -1;
    }
    printf("[MODEM] Handshake OK.\n");

    /* Disable echo (cleaner AT response parsing) */
    at_cmd(fd, "ATE0\r\n", "OK", 200);

    /* Enable verbose error reporting */
    at_cmd(fd, "AT+CMEE=2\r\n", "OK", 200);

    /* Set SMS to text mode — do this once at init, not per-message */
    if (!at_cmd(fd, "AT+CMGF=1\r\n", "OK", 500)) {
        printf("[MODEM] Warning: Could not set SMS text mode.\n");
    } else {
        printf("[MODEM] SMS text mode set.\n");
    }

    /* Enable caller ID presentation on incoming calls */
    if (!at_cmd(fd, "AT+CLIP=1\r\n", "OK", 500)) {
        printf("[MODEM] Warning: Could not enable caller ID.\n");
    } else {
        printf("[MODEM] Caller ID enabled.\n");
    }

    /* Check signal strength */
    write(fd, "AT+CSQ\r\n", 8);
    usleep(500000);
    char sig_buf[128] = {0};
    read(fd, sig_buf, sizeof(sig_buf) - 1);
    printf("[MODEM] Signal query response: %s\n", sig_buf);

    printf("[MODEM] Initialisation complete. Radio is active.\n");
    return fd;
}

void make_call(int fd, const char* phone_number) {
    if (fd < 0) {
        printf("[MODEM] Cannot dial: modem unavailable.\n");
        return;
    }
    char command[64];
    snprintf(command, sizeof(command), "ATD%s;\r\n", phone_number);
    printf("[MODEM] Dialling %s...\n", phone_number);
    write(fd, command, strlen(command));
}

void answer_call(int fd) {
    if (fd < 0) return;
    printf("[MODEM] Answering call...\n");
    write(fd, "ATA\r\n", 5);
}

void end_call(int fd) {
    if (fd < 0) return;
    printf("[MODEM] Hanging up...\n");
    write(fd, "ATH\r\n", 5);
}

void send_sms(int fd, const char* phone_number, const char* message) {
    if (fd < 0) {
        printf("[MODEM] Cannot send SMS: modem unavailable.\n");
        return;
    }

    /* AT+CMGF=1 was already set at init — no need to repeat it here */
    char command[64];
    snprintf(command, sizeof(command), "AT+CMGS=\"%s\"\r\n", phone_number);

    printf("[SMS] Sending to %s: \"%s\"\n", phone_number, message);

    write(fd, command, strlen(command));
    usleep(500000); /* Wait for '>' prompt */

    write(fd, message, strlen(message));

    /* Ctrl-Z (0x1A) signals the modem to send the message */
    char ctrl_z = 0x1A;
    write(fd, &ctrl_z, 1);

    printf("[SMS] Dispatched.\n");
}
