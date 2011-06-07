include env.mk

AVRDUDE := avrdude -C $(AVRDUDE_CONF) \
           -v \
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
install: all $(PORT)
	$(AVRDUDE) -U flash:w:hello_world.hex:i

.PHONY: clean
clean:
	$(RM) $(GENERATED_FILES)

%.s: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -S $(OUTPUT_OPTION) $<

serial.o: CFLAGS += -DBAUD=$(UPLOAD_SPEED)
serial.o: serial.c serial.h $(PREFERENCES_FILE)
OBJS += serial.o
GENERATED_FILES += serial.o

kernel.s: kernel.c serial.h $(PREFERENCES_FILE)
GENERATED_FILES += kernel.s

kernel.o: kernel.c serial.h $(PREFERENCES_FILE)
OBJS += kernel.o
GENERATED_FILES += kernel.o

.SECONDEXPANSION:
hello_world.elf: $$(OBJS) $(PREFERENCES_FILE)
	$(LINK.o) $(OBJS) -o $@
GENERATED_FILES += hello_world.elf

hello_world.hex: hello_world.elf
	$(RM) $@
	avr-objcopy -j .text -j .data -O ihex $< $@
GENERATED_FILES += hello_world.hex
