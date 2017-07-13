#include "lib.h"
#include "types.h"
#include <stdarg.h>
/*
 * io lib here
 * 库函数写在这
 */
#define NULL 0
#define MAX 1024
#define toX 16
#define toD 10
int32_t syscall(int num, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	/* 内嵌汇编 保存 num, a1, a2, a3, a4, a5 至通用寄存器*/
	/* num--system call number
	** 
	 */	
	asm volatile("pushl %ebx");
	asm volatile("pushl %esi");
	asm volatile("pushl %edi");
	asm volatile("movl %0, %%eax" :: "m"(num));
	asm volatile("movl %0, %%ecx" :: "m"(a1));
	asm volatile("movl %0, %%edx" :: "m"(a2));
	asm volatile("movl %0, %%ebx" :: "m"(a3));
	asm volatile("movl %0, %%esi" :: "m"(a4));
	asm volatile("movl %0, %%edi" :: "m"(a5));
	asm volatile("int $0x80");
	int ret = -1;
	asm volatile("movl %%eax, %0" :"=m"(ret) :);
	asm volatile("popl %edi");
	asm volatile("popl %esi");
	asm volatile("popl %ebx");
	return ret;
}

int fork()
{
	return syscall(1,0,0,0,0,0);
}

int sleep(uint32_t time)
{
	return syscall(2,time,0,0,0,0);
}

int exit()
{
	return syscall(3,0,0,0,0,0);
}

void printf(char *format,...)
{
	char buffer[MAX]; //MAX IS 1024
	va_list arg;

	char* p_string; //point to the 
	p_string = format;
	char* p_buffer;
	p_buffer = buffer;

	unsigned counter = 0;

	va_start(arg,format);

	while(*p_string != '\0')
	{
		if(*p_string == '%')//if %d %c %% %s %x
		{
			p_string++; // next char
			if(*p_string == 'd' || *p_string == 'D')//type = int
			{
				char inttemp[100]; //MAX 100
				int intlen = 0;
				int temp = va_arg(arg,int);
				if(temp == 0)
				{
					*(p_buffer++) = '0';
					counter++;
				}
				else if(temp > 0)//>0
				{
					while(temp != 0)//loop until 0
					{
						inttemp[intlen] = temp % toD + '0';
						intlen++;
						temp /= toD;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
				else if(temp < 0)
				{
					if(temp == -2147483648) // only one special example
					{
						*(p_buffer++) = '-';
						*(p_buffer++) = '2';
						*(p_buffer++) = '1';
						*(p_buffer++) = '4';
						*(p_buffer++) = '7';
						*(p_buffer++) = '4';
						*(p_buffer++) = '8';
						*(p_buffer++) = '3';
						*(p_buffer++) = '6';
						*(p_buffer++) = '4';
						*(p_buffer++) = '8';
						counter += (11);
					}
					else //normal <0
					{
						temp = -temp;
						while(temp != 0)
						{
							inttemp[intlen] = temp % toD + '0';
							intlen++;
							temp /= toD;
						}
						counter += (intlen+1);
						intlen--;
						*(p_buffer++) = '-';
						while(intlen!=-1)
						{
							*(p_buffer++) = inttemp[intlen]; 
							intlen--;
						}
					}
				}
			}
			else if(*p_string == 'x'|| *p_string == 'X')//HEX
			{
				char inttemp[100];
				int intlen = 0;
				int temp = va_arg(arg,int);
				if(temp == 0)//consider about 0
				{
					*(p_buffer++) = '0';
					counter++;
				}
				else if(temp > 0)//>0 then %16 and /16 directly then reverse
				{
					while(temp != 0)
					{
						int r = temp % toX;
						if(r < 10)	
							inttemp[intlen] = r + '0';
						else 
							inttemp[intlen] = r - 10 + 'a';
						intlen++;
						temp /= toX;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
				else //if < 0, then 0x100000000 - temp
				{
					long long a = 0x100000000;
					a += temp;
					while(a != 0)
					{
						long long r = a % toX;
						if(r < 10)	
							inttemp[intlen] = r + '0';
						else 
							inttemp[intlen] = r - 10 + 'a';
						intlen++;
						a /= toX;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer) = inttemp[intlen]; 
						p_buffer++;
						intlen--;
					}
				}
			}		
			else if(*p_string == 'c'|| *p_string == 'C')//type = char
			{
				int temp = va_arg(arg,int);
				*(p_buffer) = temp;
				p_buffer++;
				counter++;
			}
			else if(*p_string == 's'|| *p_string == 'S')//type = string
			{	
				char *p_temp = va_arg(arg,char*);
				while(*p_temp != '\0')  //loop
				{
					*p_buffer = *p_temp;
					p_buffer++;
					p_temp++;
					counter++;
				}
			}
			else if(*p_string == '%')//thpe == %%
			{
				*p_buffer = '%';
				p_buffer++;
				counter++;
			}
			else //to be complete
			{
				*p_buffer = '%';
				p_buffer++;
				counter++;
				*p_buffer = *p_string;
				p_buffer++;
				p_string++;
				counter++;
			}
			p_string++;
		}
		else //if not % = normal char
		{
			*p_buffer = *p_string;
			p_buffer++;
			p_string++;
			counter++;
		}
	}
	va_end(arg);
	syscall(4, (unsigned)buffer, counter, 0, 0, 0);
}

#undef NULL
#undef MAX
#undef toD
#undef toX
