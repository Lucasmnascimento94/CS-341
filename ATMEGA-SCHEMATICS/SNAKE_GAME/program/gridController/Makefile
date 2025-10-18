############################################################################################################
#                                        MAKEFILE — GRID_CONTROLLER (MULTI-MCU)                                  #
############################################################################################################
#  Project     : Snake                                                                                     #
#  Subprojects : gridController (grid/ISRs), gameController (game/UI/audio)                                #
#  MCUs/Board  : ATmega168A (x2)  |  F_CPU: 20 MHz                                                         #
#  Toolchain   : avr-gcc | avr-libc | avrdude | make                                                       #
#  Author      : Lucas Nascimento                                                                          #
#  Date        : 09/22/2025                                                                                #
#                                                                                                          #
#  Description : Orchestrates builds/flash/fuses for both MCUs by delegating to sub-Makefiles.             #
#                                                                                                          #
#  Targets     : all          – Build both subprojects                                                     #
#                grid         – Build gridController                                                        #
#                game         – Build gameController                                                        #
#                flash-grid   – Flash gridController MCU                                                    #
#                flash-game   – Flash gameController MCU                                                    #
#                fuse-grid    – Program fuses for gridController (use LFUSE/HFUSE/EFUSE vars)              #
#                fuse-game    – Program fuses for gameController (use LFUSE/HFUSE/EFUSE vars)              #
#                clean        – Clean both subprojects                                                      #
############################################################################################################


# ===== Project =====
MCU := atmega168
F_CPU    = 16000000UL
OS_NAME := $(if $(OS),$(OS),$(shell uname -s 2>/dev/null))
TARGET := main

#######################################################################################################################################################                                                                            
#                               					 			GRID CONTROLLER MCU                                  	             				  #
#######################################################################################################################################################

##### Detect env #####
ifeq ($(OS_NAME),Windows_NT)
  ifdef MSYSTEM
    ENV := msys
	SEP := /
	INIT := INIT_MYSYS
  else
    ENV := win
	SEP := /
	SHELL := powershell.exe
	INIT := INIT_WIN
  endif
endif

ifeq ($(OS_NAME), Linux)
  ENV := Ubuntu
  SEP := /
  INIT := INIT_UBUNTU
endif

ifeq ($(OS_NAME), Darwin)
  ENV := POSIX
  SEP := /
  INIT := INIT_POSIX
endif



#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- LINUX OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025                                                  
ifeq ($(INIT), INIT_UBUNTU)

# Add any source directories here
SRC_DIRS = \
  . \
  startSetUp \
  startSetUp/spi \
  startSetUp/uart \
  ISP

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
endif




#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- MAC OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025  
ifeq ($(INIT), INIT_POSIX)
# Add any source directories here
SRC_DIRS = \
  . \
  lib lib/console lib/esp lib/screen \
  libC \
  snake \
  startSetUp \
  startSetUp/flash startSetUp/i2c startSetUp/isr startSetUp/spi \
  startSetUp/sram startSetUp/timer startSetUp/uart startSetUp/ws2812b startSetUp/allocation
  
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
AVRDUDE_FLAGS = -c $(PROGRAMMER) -p m328p

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

endif


#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- WINDOWS OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025  
ifeq ($(INIT), INIT_WIN)

# Add any source directories here
SRC_DIRS = \
  . \
  lib lib/console lib/esp lib/screen \
  libC \
  snake \
  startSetUp \
  startSetUp/flash startSetUp/i2c startSetUp/isr startSetUp/spi \
  startSetUp/sram startSetUp/timer startSetUp/uart startSetUp/ws2812b

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
	$(SIZE) --format=sysv $<

flash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<

fuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xFF:m

readfuse:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	rm -f $(OBJS) $(SRCS:.c=.d) $(TARGET).elf $(TARGET).hex

-include $(SRCS:.c=.d)

endif


DIRS:
	@echo "ROOT>> $(ROOT)"
	@echo "DIR>> $(DIR)"
	@echo "DEP>> $(DEP)"