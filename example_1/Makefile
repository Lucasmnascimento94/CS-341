##### Detect env #####
ifeq ($(OS),Windows_NT)
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
else
  ENV := posix
  SEP := /
  INIT := INIT_POSIX
endif

##### ================= Roots (portable) ================= #####
MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
ROOT         := $(MAKEFILE_DIR)


##### ================= PHONY TARGETS ================= #####
.PHONY: header dirs



ifeq ($(ENV), win)
###############################################################################
##                                                                           ##
##                               INIT WINDOWS                                ##
##                                                                           ##
###############################################################################
##### =================== WINDOWS NATIVE (PowerShell) =================== #####

.SHELLFLAGS := -ExecutionPolicy Bypass -Command
.ONESHELL:

# ---------- Python/venv ----------
VENV_DIR 			:= $(ROOT)venv
VENV_BIN 			:= $(VENV_DIR)$(SEP)Scripts
EXE      			:= .exe
PY       			:= py -3.13

PYTHON 				:= $(VENV_BIN)$(SEP)python$(EXE)
PIP    				:= $(VENV_BIN)$(SEP)pip$(EXE)

# ---------- Project files ----------
MAIN				:= $(ROOT)main.py
REQUIREMENTS     	:= $(ROOT)requirements.txt
UI_FILE 			:= $(ROOT)home_page$(SEP)valve_controller.ui
UI_OUT  			:= $(ROOT)home_page$(SEP)HOME.py
UIC 				:= $(VENV_BIN)$(SEP)pyside6-uic$(EXE)

# ---------- Printing ----------
PRINT := echo

all: header venv install run

run: ui
	$(PYTHON) $(MAIN)

venv: 
	@$(SHELL) -c "if (-Not (Test-Path '$(VENV_DIR)')) { \
		Write-Host 'No venv found.... creating venv dir'; \
		$(PY) -m venv '$(VENV_DIR)'; \
	} "


install:
	@$(SHELL) -c "if(Test-Path "requirements.txt"){\
						Write-Host ('Requirements found....');}\
				  if(!(Test-Path "requirements.txt" -PathType Container)){\
						Write-Host ('Installing Requirements....');\
						$(PIP) install -r $(REQUIREMENTS);\
				  }"

ui: 
	@echo "[+] Regenerating UI Python file from .ui..."
	$(UIC) "$(UI_FILE)" -o "$(UI_OUT)"

##### ===================== END INIT WINDOWS ============================ #####
###############################################################################
endif




ifeq ($(ENV), posix)
###############################################################################
##                                                                           ##
##                               INIT POSIX                                  ##
##                                                                           ##
###############################################################################
##### ==================== Linux / macOS (POSIX) ========================= #####

# ---------- Text files ----------
REQUIREMENTS     	:= $(ROOT)requirements.txt

# ---------- Source Files files ----------
MAIN_DIR			:= $(ROOT)main.c
MAIN				:= main.c

# ---------- Header files ----------


PRINT := echo
CC := /opt/homebrew/bin/avr-gcc
.PHONY: run

# -mmcu=[avr-name] 

# === Chain Init ===
all: header venv install run clean

header: 
	@$(PRINT) "Header Task"

run: $(MAIN)
	@$(PRINT) "Running main file"
	$(CC) -o main $(MAIN)
	./main

venv:
	@$(PRINT) "Venv Task"
	
install: venv
	@$(PRINT) "Install Task"

clean:
	rm -f main

##### ======================= END INIT POSIX ============================= #####
###############################################################################
endif





ifeq ($(ENV), MSYSTEM)
###############################################################################
##                                                                           ##
##                                INIT MSYS                                  ##
##                                                                           ##
###############################################################################
##### ===================== MSYS / Git Bash ============================== #####

##### ======================== END INIT MSYS ============================= #####
###############################################################################
endif







dirs:
	@$(PRINT) ""
	@$(PRINT) "general info"
	@$(PRINT) "ENV:             $(ENV)"
	@$(PRINT) "MAKEFILE_DIR:    $(MAKEFILE_DIR)"
	@$(PRINT) "MAIN_DIR:    	$(MAIN_DIR)"
	@$(PRINT) "REQUIREMENTS:    $(REQUIREMENTS)"
	@$(PRINT) ""


