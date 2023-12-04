CC = gcc
SRC_DIR = src
APP_DIR = app
TST_DIR = test
BUILD_DIR = build
CFLAGS = -std=c99 -Wall -g -I$(SRC_DIR)

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
HDR_FILES := $(wildcard $(SRC_DIR)/*.h)
TST_FILES := $(wildcard $(TST_DIR)/*.c)
APP_FILES := $(wildcard $(APP_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

unknown: $(OBJ_FILES) $(APP_FILES)
	$(CC) $(CFLAGS) -o $@ $(APP_DIR)/main.c $(OBJ_FILES)

unknown_test: $(OBJ_FILES) $(TST_FILES)
	$(CC) $(CFLAGS) -o $@ $(TST_DIR)/main.c $(OBJ_FILES)

.PHONY: build clean run test

build: unknown

clean:
	rm -rf $(BUILD_DIR)/*.o unknown

run: clean build
	./unknown

test: unknown_test $(TST_FILES) $(SRC_FILES) $(HDR_FILES)
	./unknown_test
