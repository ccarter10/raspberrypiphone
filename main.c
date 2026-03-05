#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "piphone.h"

// Global File Descriptors (The "Pipes" to our hardware)
int modem_fd = -1;
int battery_fd = -1;

// --- 1. THE BACKGROUND LISTENER ---
// This thread runs invisibly in the background. If a text or call 
// comes in, it interrupts the system to let us know.
void* radio_thread_func(void* arg) {
    char buffer[256];
    printf("[THREAD] Radio listener active in background.\n");
    
    while(1) {
        if (modem_fd > 0) {
            int bytes_read = read(modem_fd, buffer, sizeof(buffer)-1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                
                // If the modem shouts "RING", we have an incoming call!
                if (strstr(buffer, "RING")) {
                    printf("\n>>> [ALERT] INCOMING CALL DETECTED! <<<\n");
                }
                
                // If the modem shouts "+CMTI", we have a new SMS!
                if (strstr(buffer, "+CMTI")) {
                    printf("\n>>> [ALERT] NEW TEXT MESSAGE RECEIVED! <<<\n");
                }
            }
        }
        usleep(100000); // Sleep for 100ms so we don't melt the CPU
    }
    return NULL;
}

// --- 2. THE USER INTERFACE LOOP ---
void run_ui_loop(void) {
    printf("[UI] System online. Press Ctrl+C to shutdown.\n");
    
    while(1) {
        float current_bat = 0.0;
        
        // Ask the physical I2C chip for the battery voltage
        if (battery_fd > 0) {
            current_bat = get_battery_percentage(battery_fd);
        }

        // Print the simulated screen output
        printf("[UI] Battery: %.1f%% | Signal: 4G LTE\n", current_bat);
        
        sleep(5); // Update the screen every 5 seconds
    }
}

// --- 3. THE BOOT SEQUENCE ---
int main() {
    printf("======================================\n");
    printf("     PI-PHONE OS (ZERO 2 W BUILD)     \n");
    printf("======================================\n");

    // 1. Grab the physical memory (Fail if we aren't root)
    if (hw_init() < 0) {
        printf("Boot halted.\n");
        exit(1);
    }
    
    // 2. Wake up the I2C Battery Fuel Gauge
    battery_fd = battery_init();
    
    // 3. Wake up the SIM7600 4G Modem
    modem_fd = modem_init("/dev/ttyUSB0"); 
    
    // --- TESTING BLOCK (Uncomment when you want to fire a real text) ---
    /*
    if (modem_fd > 0) {
        printf("[SYSTEM] Firing test SMS in 5 seconds...\n");
        sleep(5);
        send_sms(modem_fd, "+447000000000", "Boot Sequence Complete. Pi-Phone Online.");
    }
    */
    // -------------------------------------------------------------------

    // 4. Start the background thread to listen to the radio
    pthread_t radio_thread;
    pthread_create(&radio_thread, NULL, radio_thread_func, NULL);
    
    // 5. Hand control over to the Screen/UI loop
    run_ui_loop();

    return 0;
}
