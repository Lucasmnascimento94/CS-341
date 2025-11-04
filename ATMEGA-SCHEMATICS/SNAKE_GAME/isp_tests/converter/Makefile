APP       := main
SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin
TARGET    := $(BIN_DIR)/$(APP)

FTD_INC_DIR := /usr/local/include/ftd2xx/
FTD_LIB_DIR := /usr/local/lib

LDLIBS  := -lftd2xx -lpthread -ldl
INC_DIRS = $(FTD_INC_DIR)
LDFLAGS := -L$(FTD_LIB_DIR) \
           -Wl,-rpath,'$$ORIGIN' \
           -Wl,-rpath,'$(FTD_LIB_DIR)'
CFLAGS  := -Werror -Wall -Wextra -Wpedantic -O2 \
	   $(addprefix -I,$(INC_DIRS)) $(LDFLAGS) $(LDLIBS)

SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D) && $(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

$(BIN_DIR):
	mkdir -p $@

