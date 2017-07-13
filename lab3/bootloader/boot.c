#include "boot.h"
#include <string.h>

#define SECTSIZE 512
static void (*p_kern)();
static int SectNum = 200;

void bootMain(void) {
	/* 加载内核至内存，并跳转执行 */
	int KernelSize = SECTSIZE * SectNum;
	unsigned char buf[KernelSize];
	for(int i =0;i < SectNum;++i)
		readSect(buf + i * SECTSIZE, i+1);
	struct ELFHeader *elf = (void *)buf;
	struct ProgramHeader *ph = (void *)elf + elf->phoff;
	for(int i = elf->phnum; i > 0; --i)
	{
		memcpy((void *)ph->vaddr, buf + ph->off, ph->filesz);
		memset((void *)ph->vaddr + ph->filesz, 0, ph->memsz - ph->filesz);
		ph = (void *)ph + elf->phentsize;
	}
	p_kern = (void *)elf->entry;
	p_kern();
}

void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
	int i;
	waitDisk();
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}
