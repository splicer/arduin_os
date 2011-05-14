uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S),Linux)
    PREFERENCES := ~/.arduino/preferences.txt
endif
ifeq ($(uname_S),Darwin)
    PREFERENCES_FILE := ~/Library/Arduino/preferences.txt
    HARDWARE_DIR := /Applications/Arduino.app/Contents/Resources/Java/hardware
    # FIXME setting the PATH on Mac OS isn't working... bug in OS X?
    PATH := $(HARDWARE_DIR)/tools/avr/bin:$(PATH)
    BOARDS_FILE := $(HARDWARE_DIR)/arduino/boards.txt
    AVR_INCLUDE := $(HARDWARE_DIR)/tools/avr/avr/include
endif

ifneq ($(shell test -f $(PREFERENCES_FILE) && echo y),y)
    $(error No arduino preferences.txt found. Try running the Arduino IDE)
endif
ifneq ($(shell test -f $(BOARDS_FILE) && echo y),y)
    $(error No boards.txt found. You need to install the Arduino IDE)
endif

#DEVICE = atmega8
DEVICE = atmega328p
CLOCK = 16000000 # 16 MHz (we're using an external crystal)

#PORT := /dev/tty.usbserial-*
PORT := /dev/ttyUSB0
#PORT_SPEED := 19200
PORT_SPEED := 57600
#AVRDUDE_CONF = /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf
AVRDUDE_CONF = /usr/share/arduino/hardware/tools/avrdude.conf
AVRDUDE = avrdude -C$(AVRDUDE_CONF)\
		  -v -v -v -v -p $(DEVICE) -carduino -P$(PORT) -b$(PORT_SPEED) -D

# These variables are used by make's built-in rules.
CC = avr-gcc
CFLAGS = -Wall -Wextra -Werror -Wno-unused-parameter \
		 -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields \
		 -fpack-struct -fshort-enums -DF_CPU=$(CLOCK)
TARGET_ARCH = -mmcu=$(DEVICE)
LDFLAGS = -Wall
AS = avr-as
TARGET_MACH = $(TARGET_ARCH)
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
