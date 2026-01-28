EXTLIB_DIR = lib/string-builder
EXTLIB_TARGET = stringbuilder

CC := gcc
CFLAGS := -I./include -I./$(EXTLIB_DIR)/include
CPPFLAGS := -g -Wall -Wextra

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

AR = ar
ARFLAGS = rcs
LIB_TARGET = libjsonparser.a

# Remove main.o for library
FILTER = build/main.o
LIB_OBJS = $(filter-out $(FILTER),$(OBJ))

TARGET := json-parser

all: $(EXTLIB_TARGET) $(TARGET) $(LIB_TARGET)

build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET) $(LIB_TARGET)

$(TARGET): $(EXTLIB_TARGET) $(OBJ)
	$(CC) $(OBJ) -L./$(EXTLIB_DIR) -l$(EXTLIB_TARGET) -o $(TARGET)

# Create static library without main.o
$(LIB_TARGET): $(EXTLIB_TARGET) $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

$(EXTLIB_TARGET):
	$(MAKE) -C $(EXTLIB_DIR)

run: $(TARGET)
	./$(TARGET)

lib: $(LIB_TARGET)

.PHONY: all clean run lib
