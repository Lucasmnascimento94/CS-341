# ========= Project Settings =========
MCU     = atmega168
F_CPU   = 16000000UL
TARGET  = main
SRC     = main.c

# ========= Toolchain Settings =========
CC      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size
AVRDUDE = avrdude

CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
LDFLAGS = -mmcu=$(MCU)

PROGRAMMER = usbasp   # change if using ArduinoISP or another
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p m168

# ========= Build Rules =========
all: $(TARGET).hex size

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).eep: $(TARGET).elf
	-$(OBJCOPY) -O ihex -j .eeprom --change-section-lma .eeprom=0 $< $@ || true

size: $(TARGET).elf
	$(SIZE) --mcu=$(MCU) --format=avr $<

flash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<

fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xFF:m

readfuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	rm -f *.elf *.hex *.eep

# =====================================
# Usage:
#   make        → build .elf + .hex + size
#   make flash  → flash program with USBasp
#   make fuse   → set LFUSE=0xFF (16MHz crystal)
#   make clean  → remove build files
# =====================================
