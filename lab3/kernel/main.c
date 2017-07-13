#include "common.h"
#include "x86.h"
#include "device.h"

void Initial()
{
	initSerial();// initialize serial port
	initIdt(); // initialize idt
	initIntr(); // iniialize 8259a
	initSeg(); // initialize gdt, tss
	initTimer(); // iniialize timer
	initPCB(); // iniialize pcb table
}

void kEntry(void) 
{
	Initial();
	loadUMain();
	enableInterrupt();
	while(1){
		waitForInterrupt();
	}
	while(1);
	assert(0);
}
