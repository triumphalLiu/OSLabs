#include "x86.h"

#define PORT_PIC_MASTER 0x20
#define PORT_PIC_SLAVE  0xA0
#define IRQ_SLAVE       2

/* 初始化8259中断控制器：
 * 硬件中断IRQ从32号开始，自动发送EOI */
void initIntr(void) {
	outByte(PORT_PIC_MASTER + 1, 0xFF);
	outByte(PORT_PIC_SLAVE + 1 , 0xFF);
	outByte(PORT_PIC_MASTER, 0x11); // Initialization command
	outByte(PORT_PIC_MASTER + 1, 32); // Interrupt Vector Offset 0x20
	outByte(PORT_PIC_MASTER + 1, 1 << 2); // Tell Master PIC that there is a slave
	outByte(PORT_PIC_MASTER + 1, 0x3); // Auto EOI in 8086/88 mode
	outByte(PORT_PIC_SLAVE, 0x11); // Initialization command
	outByte(PORT_PIC_SLAVE + 1, 32 + 8); // Interrupt Vector Offset 0x28
	outByte(PORT_PIC_SLAVE + 1, 2); // Tell Slave PIC its cascade identity
	outByte(PORT_PIC_SLAVE + 1, 0x3); // Auto EOI in 8086/88 mode

	outByte(PORT_PIC_MASTER, 0x68);
	outByte(PORT_PIC_MASTER, 0x0A);
	outByte(PORT_PIC_SLAVE, 0x68);
	outByte(PORT_PIC_SLAVE, 0x0A);
}

#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

void initTimer() {
    int counter = FREQ_8253 / HZ;
    outByte(TIMER_PORT + 3, 0x34);
    outByte(TIMER_PORT + 0, counter % 256);
    outByte(TIMER_PORT + 0, counter / 256);
}

void initPCB()
{
	int i;
	for(i = 0; i < MAX_PCB_NUM; i++)
	{
		if(i == 0)
		{
			pcb[0].state = RUNNABLE;	
			pcb[0].timeCount = 10;
			pcb[0].sleepTime = 0;
			pcb[0].pid = 1;
			pcb[0].tf = (struct TrapFrame *)(pcb[0].stack + MAX_STACK_SIZE) - 1;
			pcb[0].tf->ss = pcb[0].tf->ds = pcb[0].tf->es = USEL(SEG_UDATA);
			pcb[0].tf->cs = USEL(SEG_UCODE);
			pcb[0].tf->eflags = 0x2;
			pcb[0].tf->esp = 0x400000;
		}
		else
		{
			pcb[i].state = EMPTY;
			pcb[i].timeCount = pcb[i].sleepTime = pcb[i].pid = 0;
		}
		RunningPID[i] = -1;
	}
	CurrentPCB = NULL;
	NextPID = RunningProcessNum = ReSchedule = 0;
}