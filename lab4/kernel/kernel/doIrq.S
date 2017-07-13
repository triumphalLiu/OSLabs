.code32

.global irqEmpty
irqEmpty:
	pushl $0 // push dummy error code
	pushl $-1 // push interrupt vector into kernel
	jmp asmDoIrq

.global irqGProtectFault
irqGProtectFault:
	pushl $0xd
	jmp asmDoIrq

.global irqSyscall
irqSyscall:
	pushl $0 // push dummy error code
	pushl $0x80 // push interrupt vector into kernel stack
	jmp asmDoIrq

.global irqTime
irqTime:
	pushl $0
	pushl $0x20
	jmp asmDoIrq

.extern CurrentPCB

.global asmDoIrq
asmDoIrq:
	cli
	pushal // push process state into kernel stack
	pushl %ds
	pushl %es
	pushl %fs
	pushl %esp
	movl $0x10, %eax
	movl %eax, %ds
	movl %eax, %es
	call irqHandle
	movl (CurrentPCB), %eax
	movl (%eax), %esp
	popl %fs
	popl %es
	popl %ds
	popal
	addl $4, %esp //interrupt vector is on top of kernel stack
	addl $4, %esp //error code is on top of kernel stack
	iret
