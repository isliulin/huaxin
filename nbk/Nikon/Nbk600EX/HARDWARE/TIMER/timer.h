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

//#define TIMER_EVENT_BKOFF	0x0001
//#define TIMER_EVENT_SLEEP	0x0002
//#define MY_TIMERS
#ifdef MY_TIMERS

/*
************************************************************************************************************************
*                                                     CONSTANTS
************************************************************************************************************************
*/

#define  OS_TMR_LINK_DLY       0
#define  OS_TMR_LINK_PERIODIC  1

#define OS_TASK_TMR_STK_SIZE    128
typedef  void (*OS_TMR_CALLBACK)(void *ptmr, void *parg);


typedef  struct  os_tmr {
    INT8U            OSTmrType;                       /* Should be set to OS_TMR_TYPE                                  */
    OS_TMR_CALLBACK  OSTmrCallback;                   /* Function to call when timer expires                           */
    void            *OSTmrCallbackArg;                /* Argument to pass to function when timer expires               */
    void            *OSTmrNext;                       /* Double link list pointers                                     */
    void            *OSTmrPrev;
    INT32U           OSTmrMatch;                      /* Timer expires when OSTmrTime == OSTmrMatch                    */
    INT32U           OSTmrDly;                        /* Delay time before periodic update starts                      */
    INT32U           OSTmrPeriod;                     /* Period to repeat timer                                        */
    INT8U            OSTmrOpt;                        /* Options (see OS_TMR_OPT_xxx)                                  */
    INT8U            OSTmrState;                      /* Indicates the state of the timer:                             */
                                                      /*     OS_TMR_STATE_UNUSED                                       */
                                                      /*     OS_TMR_STATE_RUNNING                                      */
                                                      /*     OS_TMR_STATE_STOPPED                                      */
} OS_TMR;

typedef  struct  os_tmr_wheel {
    OS_TMR          *OSTmrFirst;                      /* Pointer to first timer in linked list                         */
    INT16U           OSTmrEntries;
} OS_TMR_WHEEL;

extern  INT16U            OSTmrFree;                /* Number of free entries in the timer pool        */
extern  INT16U            OSTmrUsed;                /* Number of timers used                           */
extern  INT32U            OSTmrTime;                /* Current timer time                              */
extern  OS_EVENT         *OSTmrSem;                 /* Sem. used to gain exclusive access to timers    */
extern  OS_EVENT         *OSTmrSemSignal;           /* Sem. used to signal the update of timers        */

//extern  OS_EVENT         *OSTmrSem;                 /* Sem. used to gain exclusive access to timers    */
//extern  OS_EVENT         *OSTmrSemSignal;           /* Sem. used to signal the update of timers        */

extern  OS_TMR            OSTmrTbl[OS_TMR_CFG_MAX]; /* Table containing pool of timers                 */
extern  OS_TMR           *OSTmrFreeList;            /* Pointer to free list of timers                  */
extern  OS_STK            OSTmrTaskStk[OS_TASK_TMR_STK_SIZE];

extern  OS_TMR_WHEEL      OSTmrWheelTbl[OS_TMR_CFG_WHEEL_SIZE];

#endif

#endif























