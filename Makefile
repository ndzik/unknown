CC = gcc
SRC_DIR = src
APP_DIR = app
TST_DIR = test
BUILD_DIR = build
CFLAGS = -std=c99 -Wall -g -I$(SRC_DIR)

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

unknown: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(APP_DIR)/main.c $^

unknown_test: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(TST_DIR)/main.c $^

.PHONY: build clean run test

build: unknown

clean:
	rm -rf $(BUILD_DIR)/*.o unknown

run: clean build
	./unknown

test: unknown_test
	./unknown_test
