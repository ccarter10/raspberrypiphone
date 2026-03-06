CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lpthread

SRCS = main.c hw_init.c modem.c battery.c ui.c display.c touch.c
LVGL_SRCS = $(shell find lvgl/src -name '*.c')
ALL_SRCS = $(SRCS) $(LVGL_SRCS)

piphone: $(ALL_SRCS)
	$(CC) $(CFLAGS) $(ALL_SRCS) -o piphone $(LDFLAGS)

clean:
	rm -f piphone