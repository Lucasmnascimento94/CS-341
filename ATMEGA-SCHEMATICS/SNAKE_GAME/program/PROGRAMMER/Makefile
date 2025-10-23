############################################################################################################
#                                        MAKEFILE — TOP-LEVEL (MULTI-MCU)                                  #
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
ROOT = $(HOME)/
DIR := $(CURDIR)
BUILD := $(DIR)/
DEP := dependencies/dependencies_ubuntu.sh


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
	@echo "DEP>> $(DEP)"
	@echo "MAKE >> $(MAKE)"
















