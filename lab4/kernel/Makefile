CC = gcc
LD = ld

CFLAGS = -m32 -march=i386 -static \
	 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
	 -Wall -Werror -O2 -I./include
ASFLAGS = -m32
LDFLAGS = -m elf_i386

KCFILES = $(shell find ./ -name "*.c")
KSFILES = $(shell find ./ -name "*.S")
KOBJS = $(KCFILES:.c=.o) $(KSFILES:.S=.o)

kmain.bin: $(KOBJS)
	$(LD) $(LDFLAGS) -e kEntry -Ttext 0x00100000 -o kMain.elf $(KOBJS)
	@../utils/genKernel.pl kMain.elf
	
	
clean:
	rm -rf $(KOBJS) kMain.elf kMain.bin
