#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//通用定时器 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

#define Timer4_Start()	{TIM4->CR1|=0x01;/*TIM4->SR&=~(1<<0);*/}
#define Timer4_Stop()	{TIM4->CR1&=0xFE;/*TIM4->SR&=~(1<<0);*/}	
#define Timer2_Start()	{TIM2->CR1|=0x01;/*TIM4->SR&=~(1<<0);*/}
#define Timer2_Stop()	{TIM2->CR1&=0xFE;/*TIM4->SR&=~(1<<0);*/}	
//void Timer5_Init(u16 arr,u16 psc);
void Timer4_Init(u16 arr, u16 psc);
void Timer4_Update(u16 arr, u16 psc);
void TaskTimerTickCom(void *pdata);
extern void Com_Stop(void);

#if OS_TMR_EN > 0
extern OS_TMR  *OSTmrCreate (INT32U           dly,
                      INT32U           period,
                      INT8U            opt,
                      OS_TMR_CALLBACK  callback,
                      void            *callback_arg,
                      INT8U           *pname,
                      INT8U           *perr);

INT8U  OSTmrSignal (void);

#endif



#endif























