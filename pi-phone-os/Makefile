cat << 'EOF' > Makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lpthread

# Find all our personal OS files
SRCS = main.c hw_init.c modem.c battery.c ui.c

# Find EVERY C file inside the lvgl/src folder automatically
LVGL_SRCS = $(shell find lvgl/src -name \*.c)

# Combine them all
ALL_SRCS = $(SRCS) $(LVGL_SRCS)

piphone: $(ALL_SRCS)
	$(CC) $(CFLAGS) $(ALL_SRCS) -o piphone $(LDFLAGS)

clean:
	rm -f piphone
EOF