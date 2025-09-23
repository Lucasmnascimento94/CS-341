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
F_CPU    = 20000000UL
DISTRO  := $(shell sh -c '. /etc/os-release 2>/dev/null && echo $$ID')
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

ifeq ($(OS_NAME), POSIX)
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
DEP := $(DIR)/dependencies/dependencies_shell.sh


# ===== Build =====
.PHONY: clean install_deps \
		grid grid_flash grid_clean grid_read_fuse\
		game game_flash game_clean game_read_fuse 


grid:
	$(MAKE) -C ./gridController

grid_flash:
	$(MAKE) -C ./gridController flash

grid_clean:
	$(MAKE) -C ./gridController clean

grid_read_fuse:
	$(MAKE) -C ./gridController readfuse




game:
	$(MAKE) -C ./gridController

game_flash:
	$(MAKE) -C ./gridController flash

game_clean:
	$(MAKE) -C ./gridController clean

game_read_fuse:
	$(MAKE) -C ./gridController readfuse




install_deps: 
	@$(SHELL) $(DEP) -v

endif




#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- MAC OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025  

ifeq ($(INIT), INIT_POSIX)
ROOT = ~/

endif


#######################################################################################################################################################                                                                            
#                               								MAKEFILE --- MAC OS                                  							      #
#######################################################################################################################################################                                                    
#  Author     : Lucas Nascimento                                               
#  Date       : 09/22/2025  
ifeq ($(INIT), INIT_WIN)

endif


DIRS:
	@echo "ROOT>> $(ROOT)"
	@echo "DIR>> $(DIR)"
	@echo "DEP>> $(DEP)"