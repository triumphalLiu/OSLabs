#include "common.h"
#include "x86.h"
#include "device.h"

void kEntry(void) {

	initSerial();// initialize serial port
	initIdt(); // initialize idt
	initIntr(); // iniialize 8259a
	initSeg(); // initialize gdt, tss
	loadUMain(); // load user program, enter user space

	while(1);
	assert(0);
}
