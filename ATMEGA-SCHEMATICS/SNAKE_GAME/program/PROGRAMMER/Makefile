############################################################################################################
#                                        MAKEFILE — TOP-LEVEL (MULTI-MCU)                                  #
############################################################################################################
#  Project     : Snake                                                                                     #
#  Subprojects : PROGRAMMER                                                                                #
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
OS_NAME := $(if $(OS),$(OS),$(shell uname -s 2>/dev/null))

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
SHELL := /usr/bin/env bash

# ===== Config =====
CC          := /usr/bin/gcc
OUT         := main

# FTDI IDs (lsusb shows "ID 0403:6001")
FTD_VID     := 0403
FTD_PID     := 6001

# FTDI headers / libs
FTD_INC_DIR := /usr/local/include
FTD_LIB_DIR := /usr/local/lib
FTD_LIB     := ftd2xx

SRC := \
    ./main.c \
    ./utils/ft_utils.c

INC_DIRS := \
    $(FTD_INC_DIR) \
    ./utils

CFLAGS  += -Wall -Wextra -O2 $(addprefix -I,$(INC_DIRS))
LDFLAGS += -L$(FTD_LIB_DIR) \
           -Wl,-rpath,'$$ORIGIN' \
           -Wl,-rpath,'$(FTD_LIB_DIR)'
LDLIBS  += -l$(FTD_LIB) -lpthread -ldl

OBJ := $(SRC:.c=.o)

.PHONY: all clean usb_list whichlib run run-d2xx unbind-ftdi rebind-ftdi

# ===== Build =====
all: $(OUT)

$(OUT): $(OBJ)
	@echo ">>> Linking $(OUT)"
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@

# Explicit compile rule (more portable than relying on implicit ones)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)

usb_list:
	lsusb

whichlib:
	@echo "==> ldconfig:"
	-ldconfig -p | grep -i ftd2xx || true
	@echo "==> files:"
	-ls -l $(FTD_LIB_DIR)/libftd2xx* || true
	@echo "==> ldd on $(OUT):"
	-ldd ./$(OUT) | grep -i ftd2xx || true

# --- D2XX-friendly run: unbind FTDI if kernel grabbed it, then run
run: $(OUT)
	./$(OUT)

# Find the interface path for this VID:PID and unbind it from ftdi_sio
unbind-ftdi:
	sudo rmmod ftdi_sio

# Rebind the same interface to ftdi_sio (optional)
rebind-ftdi:
	@set -euo pipefail; \
	echo ">> trying to rebind FTDI $(FTD_VID):$(FTD_PID) to ftdi_sio"; \
	iface=$$(lsusb -t | awk -v vid=$(FTD_VID) -v pid=$(FTD_PID) \
		'/ID '$$vid':'$$pid'/ {found=1} found && /Driver=ftdi_sio|Driver=none/ {print $$3; exit}'); \
	if [[ -z $$iface ]]; then \
	  echo "No matching FTDI interface found to rebind."; \
	else \
	  node="$${iface}:1.0"; \
	  echo "Rebinding $$node to ftdi_sio..."; \
	  echo -n $$node | sudo tee /sys/bus/usb/drivers/ftdi_sio/bind >/dev/null || true; \
	fi

# Convenience: unbind, run with D2XX, then (optionally) rebind back
run-d2xx: $(OUT) unbind-ftdi
	./$(OUT)
	@# Uncomment next line if you want to rebind automatically after the run:
	@# $(MAKE) rebind-ftdi

endif




#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- MAC OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025  

ifeq ($(INIT), INIT_POSIX)
USR_ENV := $(HOME)
ROOT := /usr/
SHELL := zsh
DIR := $(CURDIR)
DEP := $(DIR)/dependencies/dependencies_posix.sh

# ===== Tools/Flags =====
CC      := /usr/bin/clang
FTD_LIB_DIR := /usr/local/lib/
FTD_LIB := ftd2xx
DYLIB_PATH := /usr/local/lib/libftd2xx.1.4.30.dylib
SRC := ./main.c
OUT := main

INC_DIRS := \
/usr/local/include/ \
./utils

CFLAGS  += $(addprefix -I,$(INC_DIRS))
# ===== Build =====
all: build

build: link

link:
	@echo ">>> Linking $(OUT)"
	$(CC) $(SRC) $(CFLAGS) -L$(FTD_LIB_DIR) -l$(FTD_LIB) -Wl,-rpath,$(DYLIB_PATH) -o $(OUT)

# convenience
run: all
	@echo ">>> Running $(OUT)"
	@./$(OUT)

clean:
	@rm -f $(OUT)
	@echo ">>> Cleaned"
usb_list: 
	system_profiler SPUSBDataType


endif


#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- WINDOWS OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025  
ifeq ($(INIT), INIT_WIN)
USR_ENV := $(HOME)
ROOT := C:/Users
SHELL := powershell
.SHELLFLAGS := -NoProfile -ExecutionPolicy Bypass -Command
DIR := $(CURDIR)
DEP := $(DIR)/dependencies/dependencies_win.ps1

endif


DIRS:
	@echo "MY SYSTEM>> $(INIT)"
	@echo "ROOT>> $(ROOT)"
	@echo "DIR>> $(DIR)"
	@echo "MAKE >> $(MAKE)"
















