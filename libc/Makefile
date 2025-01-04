CC ?= gcc
AR ?= ar
CFLAGS := -ffreestanding -nostdlib -fno-builtin -Iinclude
ARFLAGS := rcs

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=src/%.o)
LIB := lib/libc.a

all: $(LIB)

$(LIB): $(OBJ)
	mkdir -p lib
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o $(LIB)
