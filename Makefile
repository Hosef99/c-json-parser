LIB_DIR = lib/string-builder
LIB_TARGET = stringbuilder

CC := gcc
CFLAGS := -I./include -I./$(LIB_DIR)/include
CPPFLAGS := -g -Wall -Wextra

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

TARGET := json-parser

all: $(LIB_TARGET) $(TARGET)

build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)

$(TARGET): $(LIB_TARGET) $(OBJ)
	$(CC) $(OBJ) -L./$(LIB_DIR) -l$(LIB_TARGET) -o $(TARGET)

$(LIB_TARGET):
	$(MAKE) -C $(LIB_DIR)

run: $(TARGET)
	./$(TARGET)
