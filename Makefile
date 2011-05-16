uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S),Linux)
    PREFERENCES := ~/.arduino/preferences.txt
    AVRDUDE_CONF = /usr/share/arduino/hardware/tools/avrdude.conf
endif
ifeq ($(uname_S),Darwin)
    PREFERENCES_FILE := ~/Library/Arduino/preferences.txt
    HARDWARE_DIR := /Applications/Arduino.app/Contents/Resources/Java/hardware
    # FIXME setting the PATH on Mac OS isn't working... bug in OS X?
    PATH := $(HARDWARE_DIR)/tools/avr/bin:$(PATH)
    BOARDS_FILE := $(HARDWARE_DIR)/arduino/boards.txt
    AVR_INCLUDE := $(HARDWARE_DIR)/tools/avr/avr/include
    AVRDUDE_CONF := $(HARDWARE_DIR)/tools/avr/etc/avrdude.conf
endif

ifneq ($(shell test -f $(PREFERENCES_FILE) && echo y),y)
    $(error No arduino preferences.txt found. Try running the Arduino IDE)
endif
ifneq ($(shell test -f $(BOARDS_FILE) && echo y),y)
    $(error No boards.txt found. You need to install the Arduino IDE)
endif

BOARD := $(shell sh -c 'grep board= $(PREFERENCES_FILE) | cut -d = -f 2')
ifeq ($(BOARD),)
    $(error No board set in preferences file)
endif

MCU := $(shell sh -c 'grep ^$(BOARD).build.mcu $(BOARDS_FILE) | cut -d = -f 2')
ifeq ($(MCU),)
    $(error No MCU found in boards.txt)
endif

F_CPU := $(shell sh -c 'grep ^$(BOARD).build.f_cpu $(BOARDS_FILE) | cut -d = -f 2')
ifeq ($(F_CPU),)
    $(error No F_CPU found in boards.txt)
endif

UPLOAD_SPEED := $(shell sh -c 'grep ^$(BOARD).upload.speed $(BOARDS_FILE) | cut -d = -f 2')
ifeq ($(UPLOAD_SPEED),)
    $(error No UPLOAD_SPEED found in boards.txt)
endif

UPLOAD_PROTOCOL := $(shell sh -c 'grep ^$(BOARD).upload.protocol $(BOARDS_FILE) | cut -d = -f 2')
ifeq ($(UPLOAD_PROTOCOL),)
    $(error No UPLOAD_PROTOCOL found in boards.txt)
endif

PORT := $(shell sh -c 'grep port= $(PREFERENCES_FILE) | cut -d = -f 2')
ifeq ($(PORT),)
    $(error No PORT set in preferences file)
endif

AVRDUDE := avrdude -C $(AVRDUDE_CONF) \
           -v -v -v -v \
           -p $(MCU) \
           -c $(UPLOAD_PROTOCOL) \
           -P $(PORT) \
           -b $(UPLOAD_SPEED) \
           -D

# These variables are used by make's built-in rules.
CC := avr-gcc
CFLAGS = -Wall -Wextra -Werror -Wno-unused-parameter \
         -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields \
         -fpack-struct -fshort-enums -DF_CPU=$(F_CPU)
TARGET_ARCH := -mmcu=$(MCU)
LDFLAGS = -Wall
AS := avr-as
TARGET_MACH := $(TARGET_ARCH)
ASFLAGS = --gstabs

.PHONY: all
all: hello_world.hex

.PHONY: install
install: all
	$(AVRDUDE) -U flash:w:hello_world.hex:i

.PHONY: clean
clean:
	$(RM) -r *.hex *.elf *.o

hello_world.o: hello_world.c

hello_world.elf: hello_world.o
	$(LINK.o) $^ -o $@

hello_world.hex: hello_world.elf
	$(RM) $@
	avr-objcopy -j .text -j .data -O ihex $< $@
