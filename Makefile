CC = gcc
SRC_DIR = src
APP_DIR = app
BUILD_DIR = build
CFLAGS = -std=c99 -Wall -g -I$(SRC_DIR)

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
INC_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

unknown: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(APP_DIR)/main.c $^

.PHONY: build clean run

build: unknown

clean:
	rm -rf $(BUILD_DIR)/*.o unknown

run: clean build
	./unknown

