#include "uart.h"
#include"iostm8l152r8.h"
#include <stdarg.h> 

void Init_USART1(void)
{
      CLK_PCKENR1 |= (1<<5);//开串口时钟
      
      SYSCFG_RMPCR1 &= ~0x30;
      SYSCFG_RMPCR1 |= 0x10;
      //PC_DDR |= (1<<3);     //TX引脚为输出
      //PC_CR1 |= (1<<3);
      //PC_CR2 |= (1<<3);
      PA_DDR |= (1<<2);
      PA_CR1 |= (1<<2);
      PA_CR2 |= (1<<2);
      
      PA_DDR &= ~(1<<3);
      PA_CR1 |= (1<<3);
      PA_CR2 &= ~(1<<3);
      // (2) BRR1存放的是分频系数的第11位到第4位，
      // (3) BRR2存放的是分频系数的第15位到第12位，和第3位到第0位
      
      // 波特率 115200
      USART1_BRR2=0x0A;   
      USART1_BRR1=0x08; 
      // 9600
      //USART1_BRR2=0x02;   
      //USART1_BRR1=0x68; 
      // 57600
      //USART1_BRR2=0x05;   
      //USART1_BRR1=0x11; 
      
      

      //USART1_CR2=0x2c;//允许接收，发送，开接收中断
      USART1_CR2=0x0c;
}

void putchar(u8 c)
{
      while((USART1_SR & 0x80)==0x00);
      USART1_DR = c;
}

void puts(const char *str)
{
     //UartDecode(str);
    while(*str != '\0')
    {
        putchar(*str++);
    }
}

#ifdef _FORMAT_PRINT_

char *_itoa(long n, int base)
 {
  	char *p;
 	int minus;
 	static char buf[16];

//	puts_("enter _itoa\n");
//	put_hex(n);

 	p = &buf[16];
 	*--p =  0;
 	if (n < 0) 
	{
  		minus = 1;
  		n = -n;
 	}
 	else
  		minus = 0;


 	if (n == 0)
	{
  		*--p = '0';
	}
 	else
  		while (n > 0) 
		{  
	//		puts_("_itoa ");
   			*--p = "0123456789abcdef"[n % base];
	//		putchar(*p);
   			n /= base;
  		}

 	if (minus)
  		*--p = '-';

 	return p;
}

int printf(const char *fmt, ...)
{
//	int i;
	const char *s;
 	int d;
	char buf[16];
	char *p = buf;
//	char buf[16];
//	char *p = buf;
 	va_list ap;

    va_start(ap, fmt);
//	i = 16;

    while (*fmt) 
    {
        if (*fmt != '%') 
	{
            putchar(*fmt++);
            continue;
        }
		
		switch(*++fmt)
		{
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
				{
                    putchar(*s);
                }
                break;
            case 'd':			
                d = va_arg(ap, int);
				//d>>=8;
                p = _itoa(d, 10);
                for (s = p; *s; s++) 
				{
                    putchar(*s);
                }

				break;

            case 'x':
			case 'X':
                d = va_arg(ap, int);
                p = _itoa(d, 16);
                for (s = p; *s; s++) 
				{
                    putchar(*s);
                }
                break;
             //Add other specifiers here...              
            default:  
                puts(fmt);
                break;
        } 
        fmt++;
    }
    va_end(ap);

    return 1;   /* Dummy return value */
} 
#else
int printf(const char *fmt, ...)
{
    return 0;
}
#endif
