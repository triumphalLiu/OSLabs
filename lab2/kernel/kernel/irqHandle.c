#include "x86.h"
#include "device.h"

unsigned p = 0xb8000;
int line = 0;
int oneline = 0;

void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */

	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		default:assert(0);
	}
}

void syscallHandle(struct TrapFrame *tf) {
	/* 实现系统调用*/
	for(int i = 0; i < tf->edx; ++i)
	{
		char temp = *(char *)(tf->ecx+i);
		if(temp != '\n')
		{
			*(char *)(p) = temp;
			*(char *)(p+1) = 0x0c;
			oneline += 2;
			p += 2;
			if(oneline == 160)
			{
				line ++;
				oneline = 0;
			}
		}
		else
		{
			line++;
			oneline = 0;
			p = 0xb8000 + line * 160;
		}
	}
	tf->eax = tf->edx;
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
