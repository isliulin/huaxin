#ifndef _SYS_H_
#define _SYS_H_
//#include "type.h"
//#include "global.h"
#include "stm8s.h"

void delay_us(u16 us);
void delay_ms(u32 time) ;
u16 get_counter(void);
u8 memcmp(u8* src, u8 *dest, u8 len);


#endif

