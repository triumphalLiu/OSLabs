CC = gcc
LD = ld

CFLAGS = -m32 -march=i386 -static \
	 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
	 -Wall -Werror -O2
ASFLAGS = -m32
LDFLAGS = -m elf_i386

BSFILES = $(shell find ./ -name "*.S")
BCFILES = $(shell find ./ -name "*.c")
BOBJS = $(BSFILES:.S=.o) $(BCFILES:.c=.o)

bootloader.bin: $(BOBJS)
	$(LD) $(LDFLAGS) -e start -Ttext 0x7c00 -o bootloader.elf $(BOBJS)
	objcopy -O binary bootloader.elf bootloader.bin
	@../utils/genBoot.pl bootloader.bin

clean:
	rm -rf $(BOBJS) bootloader.elf bootloader.bin
