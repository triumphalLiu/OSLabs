#include "lib.h"
#include "types.h"
#include <stdarg.h>
/*
 * io lib here
 * 库函数写在这
 */
#define NULL 0

int32_t syscall(int num, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret = 0;

	/* 内嵌汇编 保存 num, a1, a2, a3, a4, a5 至通用寄存器*/
	asm volatile("movl %0,%%eax"::"g"(num));
	asm volatile("movl %0,%%ebx"::"g"(a1));
	asm volatile("movl %0,%%ecx"::"g"(a2));
	asm volatile("movl %0,%%edx"::"g"(a3));
	asm volatile("movl %0,%%edi"::"g"(a4));
	asm volatile("movl %0,%%esi"::"g"(a5));
	
	asm volatile("int $0x80");
		
	return ret;
}

void printf(char *format,...)
{
	char buffer[1024];
	int temp = 0;
	va_list arg;

	char* p_string = format;
	char* p_buffer = buffer;
	char* p_temp   = NULL;

	unsigned counter = 0;

	va_start(arg,format);

	while(*p_string != '\0')
	{
		if(*p_string == '%')
		{
			p_string++;
			if(*p_string == 'd')//int
			{
				char inttemp[100];
				int intlen = 0;
				temp = va_arg(arg,int);
				if(temp == 0)
				{
					*(p_buffer++) = '0';
					counter++;
				}
				else if(temp > 0)
				{
					while(temp != 0)
					{
						inttemp[intlen] = temp % 10 + '0';
						intlen++;
						temp /= 10;
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
					else
					{
						temp = -temp;
						while(temp != 0)
						{
							inttemp[intlen] = temp % 10 + '0';
							intlen++;
							temp /= 10;
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
			else if(*p_string == 'x')//16
			{
				char inttemp[100];
				int intlen = 0;
				temp = va_arg(arg,int);
				if(temp == 0)
				{
					*(p_buffer++) = '0';
					counter++;
				}
				else if(temp > 0)
				{
					while(temp != 0)
					{
						int r = temp % 16;
						if(r < 10)	
							inttemp[intlen] = r + '0';
						else 
							inttemp[intlen] = r - 10 + 'a';
						intlen++;
						temp /= 16;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
				else
				{
					long long a = 0x100000000;
					a += temp;
					while(a != 0)
					{
						long long r = a % 16;
						if(r < 10)	
							inttemp[intlen] = r + '0';
						else 
							inttemp[intlen] = r - 10 + 'a';
						intlen++;
						a /= 16;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
			}		
			else if(*p_string == 'c')
			{
				temp = va_arg(arg,int);
				*(p_buffer++) = temp;
				counter++;
			}
			else if(*p_string == 's')
			{	
				p_temp = va_arg(arg,char*);
				while(*p_temp != '\0')
				{
					*(p_buffer) = *(p_temp);
					p_buffer++;
					p_temp++;
					counter++;
				}
			}
			p_string++;
		}
		else //not %
		{
			*(p_buffer++) = *(p_string++);
			counter++;
		}
	}

	va_end(arg);

	syscall(4, 0, (unsigned)buffer, counter,0 ,0);
}
