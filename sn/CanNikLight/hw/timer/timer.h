#ifndef _TIMER_H_
#define _TIMER_H_
#include "type.h"

extern u8 g_tmHandleFlag;

#define TM_HANDLE_CLR			            0x00
#define TM_HANDLE_REAR	 		            0x01          
#define TM_HANDLE_SHUT			            0x02	
#define TM_HANDLE_MULTIFLASH	            0x04     // 3

//#define TM_nHANDLE_CLR                             0x00
#define TM_nHANDLE_COMRST	                   0x10
#define TM_nHANDLE_SLAVE_PRE2             0x20   
#define TM_nHANDLE_MULTIFLASH	            0x40
//#define TM_nHANDLE_COMTIMEOUT            0x80

void Timer2Init(void);
void Timer1Init(void);
void WaitCmrHandle(void);

#define Timer4Start(arr) \
{\
	TIM4_CNTR = (255 - arr); \
	TIM4_CR1 = 0x09; \
}

#define Timer4Stop()  \
{\
    TIM4_CR1 = 0x00;\
}

#endif
