# ============================================================
#  Pi-Phone OS — Makefile
#  Cross-compiles on your PC, produces a binary for Pi Zero 2W
#
#  USAGE:
#    make          — cross-compile for Pi Zero 2W (default)
#    make native   — compile natively (if running on the Pi itself)
#    make clean    — remove build artefacts
#    make deploy   — push binary to Pi over SSH (set PI_HOST below)
#    make run      — deploy and execute on the Pi
# ============================================================

# --- TARGET CONFIGURATION ---
PI_HOST   ?= pi@raspberrypi.local
DEPLOY_DIR = /home/pi

# --- TOOLCHAIN ---
# Default: cross-compile from PC. Override with `make native` or CC=gcc.
CROSS_CC  = aarch64-linux-gnu-gcc
NATIVE_CC = gcc
CC        = $(CROSS_CC)

# --- FLAGS ---
CFLAGS    = -Wall -Wextra -O2 -I. -Ilvgl $(shell pkg-config --cflags libdrm)
LDFLAGS   = -lpthread -lm -ldrm

# --- PROJECT SOURCES ---
SRCS = main.c modem.c battery.c ui.c display.c touch.c

# --- LVGL SOURCES ---
# Only compile the drivers actually used: core, draw, widgets, fbdev, evdev.
# This avoids compiling hundreds of unused files and keeps build times sane.
LVGL_CORE   = $(shell find lvgl/src/core     -name '*.c' 2>/dev/null)
LVGL_DRAW   = $(shell find lvgl/src/draw     -name '*.c' 2>/dev/null)
LVGL_MISC   = $(shell find lvgl/src/misc     -name '*.c' 2>/dev/null)
LVGL_FONT   = $(shell find lvgl/src/font     -name '*.c' 2>/dev/null)
LVGL_LAYOUT = $(shell find lvgl/src/layouts  -name '*.c' 2>/dev/null)
LVGL_WIDGET = $(shell find lvgl/src/widgets  -name '*.c' 2>/dev/null)
LVGL_STDLIB = $(shell find lvgl/src/stdlib   -name '*.c' 2>/dev/null)
LVGL_TICK   = $(shell find lvgl/src/tick     -name '*.c' 2>/dev/null)
LVGL_INDEV  = $(shell find lvgl/src/indev    -name '*.c' 2>/dev/null)
LVGL_DISP   = $(shell find lvgl/src/display  -name '*.c' 2>/dev/null)
# Linux-specific drivers (fbdev + evdev)
LVGL_LINUX  = $(shell find lvgl/src/drivers/display/fb -name '*.c' 2>/dev/null) $(shell find lvgl/src/drivers/evdev -name '*.c' 2>/dev/null) $(shell find lvgl/src/drivers/display/drm -name '*.c' 2>/dev/null)
LVGL_THEMES = $(shell find lvgl/src/themes -name '*.c' 2>/dev/null)
LVGL_OSAL = $(shell find lvgl/src/osal -name '*.c' 2>/dev/null)
LVGL_INIT = lvgl/src/lv_init.c
LVGL_LIBS = $(shell find lvgl/src/libs/bin_decoder -name '*.c' 2>/dev/null)
LVGL_SRCS = $(LVGL_CORE) $(LVGL_DRAW) $(LVGL_MISC) $(LVGL_FONT) \
            $(LVGL_LAYOUT) $(LVGL_WIDGET) $(LVGL_STDLIB) $(LVGL_TICK) \
            $(LVGL_INDEV) $(LVGL_DISP) $(LVGL_LINUX) $(LVGL_THEMES) $(LVGL_OSAL) $(LVGL_INIT) $(LVGL_LIBS)

ALL_SRCS = $(SRCS) $(LVGL_SRCS)

# --- TARGETS ---
.PHONY: all native clean deploy run

all: piphone

piphone: $(ALL_SRCS) piphone.h lv_conf.h
	@echo "[BUILD] Cross-compiling for aarch64 (Pi Zero 2W)..."
	$(CC) $(CFLAGS) $(ALL_SRCS) -o piphone $(LDFLAGS)
	@echo "[BUILD] Done. Binary: ./piphone"
	@echo "[BUILD] Run 'make deploy' to push to $(PI_HOST)"

native:
	@echo "[BUILD] Compiling natively..."
	$(MAKE) CC=$(NATIVE_CC) piphone

clean:
	@echo "[BUILD] Cleaning..."
	rm -f piphone

deploy: piphone
	@echo "[DEPLOY] Copying to $(PI_HOST):$(DEPLOY_DIR)..."
	scp piphone $(PI_HOST):$(DEPLOY_DIR)/piphone
	@echo "[DEPLOY] Done."

run: deploy
	@echo "[RUN] Executing on Pi..."
	ssh $(PI_HOST) "sudo $(DEPLOY_DIR)/piphone"
