# Build folder path
BUILD_DIR := build

# debug build?
DEBUG := 1
# optimization
OPT := -Og

CC := gcc

WARNING := -Wall -Wextra

CFLAGS :=

ifeq ($(DEBUG), 1)
CFLAGS += -g
endif

CFLAGS += $(OPT)


C_SOURCES := 	\
./shell1.c		\
./shell2.c		\
./shell3.c

all: $(BUILD_DIR)/shell1 $(BUILD_DIR)/shell2 $(BUILD_DIR)/shell3

$(BUILD_DIR)/shell1: ./shell1.c | $(BUILD_DIR)
	$(CC) $(WARNING) $(CFLAGS) $< -o $@

$(BUILD_DIR)/shell2: ./shell2.c | $(BUILD_DIR)
	$(CC) $(WARNING) $(CFLAGS) $< -o $@

$(BUILD_DIR)/shell3: ./shell3.c | $(BUILD_DIR)
	$(CC) $(WARNING) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	-rm -fr $(BUILD_DIR)
.PHONY: clean all
