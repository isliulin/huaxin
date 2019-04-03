#include "uart.h"
#include <stdarg.h> 

void putchar(uint8_t data)
{
	UART1_SendData8(data);
	while (UART1_GetFlagStatus(UART1_FLAG_TC) == RESET);
}

void puts(const char *str)
{
     //UartDecode(str);
    while(*str != '\0')
    {
        putchar(*str++);
    }
}

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