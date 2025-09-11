# ===== Project =====
MCU      = atmega168
F_CPU    = 16000000UL
TARGET   = main

# Add any source directories here
SRC_DIRS = . StartSetUp lib libC

# Auto-discover sources/headers
SRCS   := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.c))
OBJS   := $(SRCS:.c=.o)
INCS   := $(addprefix -I,$(SRC_DIRS))

# ===== Tools/Flags =====
CC      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size
AVRDUDE = avrdude

CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=c11 $(INCS) -MMD -MP
LDFLAGS = -mmcu=$(MCU)

PROGRAMMER    = usbasp
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p m168

# ===== Build =====
all: $(TARGET).hex size

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

size: $(TARGET).elf
	$(SIZE) --mcu=$(MCU) --format=avr $<

flash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<

fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xFF:m

readfuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	rm -f $(OBJS) $(SRCS:.c=.d) $(TARGET).elf $(TARGET).hex

-include $(SRCS:.c=.d)
