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
F_CPU    = 16000000UL
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


.PHONY: clean install_deps \
		grid grid_flash grid_clean grid_read_fuse grid_read_obj grid_read_elf\
		game game_flash game_clean game_read_fuse game_rad_obj game_read_elf

# ===== Grid controller MCU =====
grid: # Build files
	$(MAKE) -C ./gridController

grid_flash: # Flash files built with the above command
	$(MAKE) -C ./gridController flash

grid_clean: # Clean old files
	$(MAKE) -C ./gridController clean

grid_read_fuse: # Read fuses in the MCU
	$(MAKE) -C ./gridController readfuse

grid_read_obj: # Read object file
	
	
grid_read_elf: # Read elf file



# ===== Game controller MCU =====
game: # Build files
	$(MAKE) -C ./gridController 

game_flash: # Flash files built with the above command
	$(MAKE) -C ./gridController flash

game_clean: # Clean old files
	$(MAKE) -C ./gridController clean

game_read_fuse: # Read fuses in the MCU
	$(MAKE) -C ./gridController readfuse

game_read_obj: # Read object file


game_read_elf: # Read elf file



# ===== Install dependencies out of the dependencies.txt file =====
install_deps: 
	@$(SHELL) $(DEP) -v

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


# ===== Grid controller MCU =====
grid: # Build files
	$(MAKE) -C ./gridController

grid_flash: # Flash files built with the above command
	$(MAKE) -C ./gridController flash

grid_clean: # Clean old files
	$(MAKE) -C ./gridController clean

grid_read_fuse: # Read fuses in the MCU
	$(MAKE) -C ./gridController readfuse



# ===== Game controller MCU =====
game: # Build files
	$(MAKE) -C ./gridController 

game_flash: # Flash files built with the above command
	$(MAKE) -C ./gridController flash

game_clean: # Clean old files
	$(MAKE) -C ./gridController clean

game_read_fuse: # Read fuses in the MCU
	$(MAKE) -C ./gridController readfuse


# ===== Install dependencies out of the dependencies.txt file =====
install_deps: 
	@$(SHELL) $(DEP) -v

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
DIR := $(CURDIR)
DEP := $(DIR)/dependencies/dependencies_win.ps1


# ===== Grid controller MCU =====
grid: # Build files
	$(MAKE) -C ./gridController

grid_flash: # Flash files built with the above command
	$(MAKE) -C ./gridController flash

grid_clean: # Clean old files
	$(MAKE) -C ./gridController clean

grid_read_fuse: # Read fuses in the MCU
	$(MAKE) -C ./gridController readfuse



# ===== Game controller MCU =====
game: # Build files
	$(MAKE) -C ./gridController 

game_flash: # Flash files built with the above command
	$(MAKE) -C ./gridController flash

game_clean: # Clean old files
	$(MAKE) -C ./gridController clean

game_read_fuse: # Read fuses in the MCU
	$(MAKE) -C ./gridController readfuse


# ===== Install dependencies out of the dependencies.txt file =====
install_deps: 
	@$(SHELL) -ExecutionPolicy Bypass -File $(DEP) -verbose
endif


DIRS:
	@echo "MY SYSTEM>> $(INIT)"
	@echo "ROOT>> $(ROOT)"
	@echo "DIR>> $(DIR)"
	@echo "DEP>> $(DEP)"