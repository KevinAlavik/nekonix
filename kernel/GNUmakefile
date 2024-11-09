MAKEFLAGS += -rR
.SUFFIXES:

override OUTPUT := kernel
override USER_VARIABLE = $(if $(filter $(origin $(1)),default undefined),$(eval override $(1) := $(2)))

$(call USER_VARIABLE,DESTDIR,)
$(call USER_VARIABLE,PREFIX,/usr/local)
$(call USER_VARIABLE,CC,cc)
$(call USER_VARIABLE,AR,ar)
$(call USER_VARIABLE,LD,ld)
$(call USER_VARIABLE,CFLAGS,-g -O2 -pipe)
$(call USER_VARIABLE,CPPFLAGS,)
$(call USER_VARIABLE,NASMFLAGS,-F dwarf -g)
$(call USER_VARIABLE,LDFLAGS,)

ifneq ($(shell ( test '$(MAKECMDGOALS)' = clean || test '$(MAKECMDGOALS)' = distclean ); echo $$?),0)
    ifeq ($(shell ( ! test -d freestnd-c-hdrs-0bsd || ! test -d cc-runtime || ! test -f src/limine.h ); echo $$?),0)
        $(error Please run the ./get-deps script first)
    endif
endif

override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

ifeq ($(CC_IS_CLANG),1)
    override CC += \
        -target x86_64-unknown-none
endif

override CFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -nostdinc \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel

override CPPFLAGS := \
    -I src \
    -isystem freestnd-c-hdrs-0bsd \
    $(CPPFLAGS) \
    -MMD \
    -MP

override NASMFLAGS += \
    -Wall \
    -f elf64

override LDFLAGS += \
    -m elf_x86_64 \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -gc-sections \
    -T linker.ld

override CFILES := $(shell cd src && find -L * -type f -name '*.c' | LC_ALL=C sort)
override ASFILES := $(shell cd src && find -L * -type f -name '*.S' | LC_ALL=C sort)
override NASMFILES := $(shell cd src && find -L * -type f -name '*.asm' | LC_ALL=C sort)
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

.PHONY: all
all: bin/$(OUTPUT)

cc-runtime-x86_64/cc-runtime.a: cc-runtime/*
	rm -rf cc-runtime-x86_64
	cp -r cc-runtime cc-runtime-x86_64
	$(MAKE) -C cc-runtime-x86_64 -f cc-runtime.mk \
		CC="$(CC)" \
		AR="$(AR)" \
		CFLAGS="$(CFLAGS)" \
		CPPFLAGS='-isystem ../freestnd-c-hdrs-0bsd -DCC_RUNTIME_NO_FLOAT'

bin/$(OUTPUT): GNUmakefile linker.ld $(OBJ) cc-runtime-x86_64/cc-runtime.a
	mkdir -p "$$(dirname $@)"
	$(LD) $(OBJ) cc-runtime-x86_64/cc-runtime.a $(LDFLAGS) -o $@

-include $(HEADER_DEPS)

obj/%.c.o: src/%.c GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.S.o: src/%.S GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.asm.o: src/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf bin obj cc-runtime-x86_64

.PHONY: distclean
distclean: clean
	rm -rf freestnd-c-hdrs-0bsd cc-runtime src/limine.h

.PHONY: install
install: all
	install -d "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
	install -m 644 bin/$(OUTPUT) "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/"

.PHONY: uninstall
uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/$(OUTPUT)"
	-rmdir "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
