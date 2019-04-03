#ifndef _UART_H_
#define _UART_H_

#include "type.h"

#define _FORMAT_PRINT_

void putchar(u8 c);
void Init_USART1(void);

#ifdef _FORMAT_PRINT_
int printf(const char *fmt, ...);
#else
//#define printf /##/
int printf(const char *fmt, ...);
#endif

#endif