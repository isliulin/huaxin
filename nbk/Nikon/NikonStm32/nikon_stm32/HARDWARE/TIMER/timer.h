#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//ͨ�ö�ʱ�� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
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























