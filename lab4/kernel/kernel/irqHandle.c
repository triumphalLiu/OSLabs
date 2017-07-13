#include "x86.h"
#include "device.h"
#include <string.h>
//printf
#define video_mem 0xb8000
unsigned p = video_mem;
int line = 0;
int oneline = 0;
//
SegDesc gdt[NR_SEGMENTS];
TSS tss;
void syscallHandle(struct TrapFrame *tf);
void GProtectFaultHandle(struct TrapFrame *tf);
void TimeBreakHandle(struct TrapFrame *tf);
void exitHandle(struct TrapFrame *tf);
void sleepHandle(struct TrapFrame *tf);
void forkHandle(struct TrapFrame *tf);
void printfHandle(struct TrapFrame *tf);
void initSem(struct TrapFrame *tf);
void postSem(struct TrapFrame *tf);
void waitSem(struct TrapFrame *tf);
void destorySem(struct TrapFrame *tf);
//Debug
extern void putChar(char ch);
//schedule
void InQueue(int pid)
{
	RunningPID[NextPID] = pid;
	NextPID++;
}

void OutQueue(int pid)
{
	for(int j = pid; j < NextPID; ++j)
		RunningPID[j] = RunningPID[j+1];
	NextPID--;
}

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	if(CurrentPCB != NULL)
		memcpy(CurrentPCB->tf, tf, sizeof(struct TrapFrame));
	switch(tf->irq) {
		case -1:	break;
		case 0xd:
			GProtectFaultHandle(tf);	break;
		case 0x20:
			TimeBreakHandle(tf);	break;
		case 0x80:
			syscallHandle(tf);		break;
		default:
			assert(0);
	}
	//Reschedule
	if(ReSchedule == 1)
		switch_process();
	//Enable Interrupt
	if(CurrentPCB == NULL)	
	{
		if(RunningProcessNum != 0)
			enableInterrupt();
		waitForInterrupt();
	}
	/*
	//For debug
	for(int i=0;i<MAX_PCB_NUM;++i)
	{
		putChar(RunningPID[i]+'0');
		putChar(' ');
	}
	putChar('\n');
	*/
}

void printfHandle(struct TrapFrame *tf)
{
	uint32_t segBase = (uint32_t )(gdt[tf->ds >> 3].base_15_0 + (gdt[tf->ds>>3].base_23_16<<16) + (gdt[tf->ds >> 3].base_31_24 << 24));
	for(int i = 0; i < tf->edx; ++i) //edx store the nums of the content to be output
	{
		char temp = *(char *)(tf->ecx+i+ segBase); //temp is the word to be output
		if(temp != '\n') //if not '\n'
		{
			*(char *)(p) = temp;  //to be output
			*(char *)(p+1) = 0x0c; //black background and red word
			oneline += 2; // two words
			p += 2; // two words
			if(oneline == 160) //if one line full
			{
				line ++; //change a line
				oneline = 0; //zero cnt
			}
		}
		else //change a line
		{
			line++;
			oneline = 0;
			p = video_mem + line * 160;//reset point
		}
	}
	tf->eax = tf->edx;
}

void exitHandle(struct TrapFrame *tf)
{
	CurrentPCB->state = EMPTY;
	RunningProcessNum--;
	ReSchedule = 1;
	//putChar('%'); //to check if all program have been exited
}

void forkHandle(struct TrapFrame *tf)
{
	RunningProcessNum++;
	uint32_t pre_base = current_base;
	current_base += 0x400000;
	memcpy((uint32_t *)pre_base, (uint32_t *)(gdt[tf->ds >> 3].base_15_0 + (gdt[tf->ds>>3].base_23_16<<16) + (gdt[tf->ds >> 3].base_31_24 << 24)), 0x400000);
	int new_pcb_id = -1;
	for(int i = 0; i < MAX_PCB_NUM; ++i)
	{
		if(pcb[i].state == EMPTY)
		{
			new_pcb_id = i;
			break;
		}
		if(i == MAX_PCB_NUM - 1)
			assert(0); //pcb size should be bigger
	}	
	memcpy(pcb[new_pcb_id].stack, CurrentPCB->stack, sizeof(CurrentPCB->stack));
	pcb[new_pcb_id].tf = (struct TrapFrame *)(pcb[new_pcb_id].stack + MAX_STACK_SIZE) - 1;
	memcpy(pcb[new_pcb_id].tf, tf, sizeof(tf));
	int newSegCodeIndex = current_index++;
	int newSegDataIndex = current_index++;
	gdt[newSegCodeIndex] = SEG(STA_X | STA_R, pre_base, 0x400000, DPL_USER);
	gdt[newSegDataIndex] = SEG(STA_W, pre_base, 0x400000, DPL_USER);
	pcb[new_pcb_id].tf->es = pcb[new_pcb_id].tf->ss = pcb[new_pcb_id].tf->ds = USEL(newSegDataIndex);
	pcb[new_pcb_id].tf->cs = USEL(newSegCodeIndex);
	pcb[new_pcb_id].state = RUNNABLE;
	pcb[new_pcb_id].timeCount = 10;
	pcb[new_pcb_id].pid = 2;
	pcb[new_pcb_id].tf->eax = 0;	//0
	CurrentPCB->tf->eax = pcb[new_pcb_id].pid;	//pid of child
	InQueue(new_pcb_id);
}

void sleepHandle(struct TrapFrame *tf)
{
	CurrentPCB->state = BLOCKED;
	CurrentPCB->sleepTime = tf->ecx;
	ReSchedule = 1;
}

void initSem(struct TrapFrame *tf)
{
	int i = 0;
	for(i = 0; i < MAX_SEM_NUM; ++i)
		if(Sem[i].state == 0)
		{
			Sem[i].state = 1;
			Sem[i].ID = tf->ecx;
			Sem[i].value = tf->edx;
			break;
		}
	assert(i < MAX_SEM_NUM);
	if(Sem[i].value == tf->edx && Sem[i].ID == tf->ecx)
		tf->eax = 0;
	else 
		tf->eax = -1;
}

void postSem(struct TrapFrame *tf)
{
	int i = 0;
	for(i = 0; i < MAX_SEM_NUM; ++i)
		if(Sem[i].ID == tf->ecx)
			break;
	assert(i < MAX_SEM_NUM);
	Sem[i].value += 1;
	tf->eax = 0;
	if(Sem[i].value <= 0)
	{
		tf->eax = -1;
		for(int j = 0; j < MAX_PCB_NUM; ++j)
			if(pcb[j].state == SEM_BLOCKED)
			{
				pcb[j].state = RUNNABLE;
				InQueue(j);
				tf->eax = 0;
				break;
			}
	}
}

void waitSem(struct TrapFrame *tf)
{
	int i = 0;
	for(i = 0; i < MAX_SEM_NUM; ++i)
		if(Sem[i].ID == tf->ecx)
			break;
	assert(i < MAX_SEM_NUM);
	Sem[i].value -= 1;
    if (Sem[i].value < 0)
    {
		CurrentPCB->state = SEM_BLOCKED;
		ReSchedule = 1;//current process has been blocked so should re-schedule
	}
	tf->eax = 0;
}

void destorySem(struct TrapFrame *tf)
{
	int a_using_num = 0;
	for(int i = 0; i < MAX_SEM_NUM; ++i)
		if(Sem[i].state == 1)
			a_using_num++;
	for(int i = 0; i < MAX_SEM_NUM; ++i)
		if(Sem[i].ID == tf->ecx)
		{
			Sem[i].state = 0;
			Sem[i].value = 0;
			break;
		}
	int b_using_num = 0;
	for(int i = 0; i < MAX_SEM_NUM; ++i)
		if(Sem[i].state == 1)
			b_using_num++;
	if(a_using_num - b_using_num == 1)
		tf->eax = 0;
	else 
		tf->eax = -1;
}

void syscallHandle(struct TrapFrame *tf) {
	/* 实现系统调用*/
	switch(tf->eax){
		case 1: forkHandle(tf); break;
		case 2: sleepHandle(tf); break;
		case 3: exitHandle(tf); break;
		case 4: printfHandle(tf); break;
		case 5: initSem(tf); break;
		case 6: postSem(tf); break;
		case 7: waitSem(tf); break;
		case 8: destorySem(tf); break;
		default: assert(0);
	}
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
}

void TimeBreakHandle(struct TrapFrame *tf)
{
	int modify = 0;
	for(int i = 0; i < MAX_PCB_NUM; i++)
	{		
		if(pcb[i].state == RUNNING)
		{
			if(pcb[i].timeCount == 1)
			{
				pcb[i].state = RUNNABLE;
				InQueue(i);
			}
			pcb[i].timeCount--;
			modify = 1;
			continue;
		}
		if(pcb[i].state == BLOCKED)
		{
			if(pcb[i].sleepTime == 1)
			{
				pcb[i].state = RUNNABLE;
				pcb[i].timeCount = 10;
				InQueue(i);
			}
			pcb[i].sleepTime--;
			modify = 1;
			continue;
		}
	}
	if(modify == 1)
		ReSchedule = modify;
}

void switch_process()
{
	//choose
	ReSchedule = 0;
	struct ProcessTable *next_process = NULL;
	if(CurrentPCB != NULL && CurrentPCB->state == RUNNING) //Running Normally
	{
		ReSchedule = 1;
	}
	else if(NextPID > 0) // BLOCKED OR NO PROCESS RUNNING
	{
		assert(RunningPID[0] >= 0);
		next_process = &pcb[RunningPID[0]];
		OutQueue(0);	
	}
	else //NO PROCESS WATING TO BE RUN
	{
		next_process = NULL;
	}
	//switch
	if(next_process == NULL)
	{
		CurrentPCB = next_process;
	}
	else if(next_process != NULL)
	{
		tss.esp0 = (uint32_t)(next_process->stack + MAX_STACK_SIZE - sizeof(struct TrapFrame));	
		CurrentPCB = next_process;
		CurrentPCB->state = RUNNING;
		CurrentPCB->timeCount = 10;
	}
}