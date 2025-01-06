CONF_PREFIX ?= ../build
KERNEL_CONF_PREFIX ?= $(CONF_PREFIX)/kernel
include $(KERNEL_CONF_PREFIX)/config.mk

OUTPUT := kernel.elf

# Kernel source files
SRCFILES := $(shell find -L * -type f | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.S,$(SRCFILES))
NASMFILES := $(filter %.asm,$(SRCFILES))
OBJ := $(addprefix $(OBJ_DIR)/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
HEADER_DEPS := $(addprefix $(OBJ_DIR)/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

.PHONY: all
all: $(BIN_DIR)/$(OUTPUT)

# Dependencies check
ifneq ($(shell ( test '$(MAKECMDGOALS)' = clean || test '$(MAKECMDGOALS)' = distclean ); echo $$?),0)
    ifeq ($(shell ( ! test -d $(CONF_PREFIX)/freestnd-c-hdrs-0bsd || ! test -d $(CONF_PREFIX)/cc-runtime || ! test -f ../include/kernel/limine.h  || ! test -f ../include/lib/_internal/nanoprintf.h ); echo $$?),0)
        $(error Please run the $(KERNEL_CONF_PREFIX)/get-deps script first)
    endif
endif

-include $(HEADER_DEPS)

$(CONF_PREFIX)/cc-runtime/cc-runtime.a: Makefile $(CONF_PREFIX)/cc-runtime/*
	$(MAKE) -C $(CONF_PREFIX)/cc-runtime -f cc-runtime.mk \
		CC="$(CC)" \
		AR="$(AR)" \
		CFLAGS="$(CFLAGS)" \
		CPPFLAGS='-isystem ../$(CONF_PREFIX)/freestnd-c-hdrs-0bsd -DCC_RUNTIME_NO_FLOAT'

$(BIN_DIR)/$(OUTPUT): Makefile $(KERNEL_CONF_PREFIX)/linker.ld $(OBJ) $(CONF_PREFIX)/cc-runtime/cc-runtime.a
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(CONF_PREFIX)/cc-runtime/cc-runtime.a -o $@

$(OBJ_DIR)/%.c.o: %.c Makefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.S.o: %.S Makefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.asm.o: %.asm Makefile
	mkdir -p "$$(dirname $@)"
	$(NASM) $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR) $(CONF_PREFIX)/cc-runtime* $(CONF_PREFIX)/freestnd-c-hdrs-0bsd*