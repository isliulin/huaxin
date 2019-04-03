#include "timer.h"
#include "../../user/canon.h"
#include "../../user/golbal.h"
//#include "led.h"

extern unsigned char g_comStatue;
extern u16 g_tmEvent;
extern u16 g_dspUpdateFlag;
extern unsigned char g_tmHandleFlag;
extern char g_cmrLinked;

extern void SetComSpiReady(void);

#ifdef MY_TIMERS

INT16U            OSTmrFree;                /* Number of free entries in the timer pool        */
INT16U            OSTmrUsed;                /* Number of timers used                           */
INT32U            OSTmrTime;                /* Current timer time                              */
OS_EVENT         *OSTmrSem;                 /* Sem. used to gain exclusive access to timers    */
OS_EVENT         *OSTmrSemSignal;           /* Sem. used to signal the update of timers        */

//OS_EVENT         *OSTmrSem;                 /* Sem. used to gain exclusive access to timers    */
//OS_EVENT         *OSTmrSemSignal;           /* Sem. used to signal the update of timers        */

OS_TMR            OSTmrTbl[OS_TMR_CFG_MAX]; /* Table containing pool of timers                 */
OS_TMR           *OSTmrFreeList;            /* Pointer to free list of timers                  */
OS_STK            OSTmrTaskStk[OS_TASK_TMR_STK_SIZE];

OS_TMR_WHEEL      OSTmrWheelTbl[OS_TMR_CFG_WHEEL_SIZE];

static  void  Tmr_Task (void *p_arg);

#endif

/**************************/
//timer2 for communication
//timer3 BUZZER
//timer4 bk light
/**************************/

//void Timer3_Start(void);
//void Timer3_Stop(void); 

// 2017/08/21 和nbk600ex 同步
void UnlinkCmrHandle(void)
{
	//SPIx_Disable();
	Com_Stop();
	Ex_NVIC_Config(GPIO_A,5,RTIR);
	Ex_NVIC_Enable(5);
	g_comStatue = COM_STATUE_IDLE;
	//SET_UPDATE_EVT(DSP_UPDATE_LINK);
	g_cmrLinked = 0;
	g_cmrPressed=0;
	//menuPara.cApert=0;
	//g_spiRcvTotal = 0;
	//g_dspUpdateFlag|=DSP_UPDATE_LINK;
	//SET_UPDATE_EVT(DSP_UPDATE_LINK);			//wried
	SPI_MISO_AUX  = 0;
	//FLAOK_OFF();
#ifdef SIGNAL_TEST
	SIGNAL_PIN2 = !SIGNAL_PIN2;
#endif
	//g_menuLock = 0;
	if (g_SettingIndex != 0)
	{
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE)
	}
	SET_UPDATE_EVT(DSP_UPDATE_LINK);

	//20170404
	/* 实际测试发现，在5d4相机上按对焦点选择按钮，会出现0xB7，0x00，之后通讯中断
	*  在此判断sck 线是否为高，为高则继续等待接收数据
	*/
	if (CLK_IN)
	{
		SetComSpiReady();
		g_cmrLinked = 1;		
	}
}


//定时器3中断服务程序	 
void TIM2_IRQHandler(void)
{ 		    		  			    

	if(TIM2->SR&0x0001)
	{
#if 0
		if(FLASH_PWM)
		{
			Timer4_Init(10,7199);
		}else
		{
			Timer4_Init(10,72);
		}
		
#else
		if(TM_HANDLE_REAR==g_tmHandleFlag)
		{
			//g_tmHandleFlag = TM_HANDLE_CLR;
			SET_SPIO_ON;
			SPI_MISO_AUX=1;
			//Timer4_Stop();
		}else if(TM_HANDLE_SHUT==g_tmHandleFlag)
		{
			if(COM_STATUE_START==g_comStatue)
			{
				//SPIx_Disable();
#if 0
				Com_Stop();
				Ex_NVIC_Config(GPIO_A,5,RTIR);
				Ex_NVIC_Enable(5);
				g_comStatue = COM_STATUE_IDLE;
				g_cmrPressed=0;
				//g_dspUpdateFlag|=DSP_UPDATE_LINK;
				SET_UPDATE_EVT(DSP_UPDATE_LINK);
				SPI_MISO_AUX  = 0;
				//Timer4_Stop();
#endif
				UnlinkCmrHandle();
			}
		}
		g_tmHandleFlag = TM_HANDLE_CLR;
#endif
	}
	Timer2_Stop();
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
	//Timer3_Stop();
}


void Timer2_Init(u16 arr, u16 psc)		//used for acqure 1ms tick
{
	RCC->APB1ENR|=1<<0;//TIM5时钟使能    
 	TIM2->ARR=arr;  //设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  //预分频器7200,得到10Khz的计数时钟
	//这两个东东要同时设置才可以使用中断
	TIM2->DIER|=1<<0;   //允许更新中断				
	TIM2->CNT = 0x00;
	//TIM6->DIER|=1<<6;   //允许触发中断	 
	TIM2->SR&=~(1<<0);
	TIM2->CR1|=0x01;    //使能定时器
	//MY_NVIC_Init(2,3,TIM2_IRQChannel,1);//抢占1，子优先级3，组2	
	MY_NVIC_Init(2,3,TIM2_IRQn,1);//抢占1，子优先级3，组2	
}

void Timer2_Update(u16 arr, u16 psc)
{
	Timer2_Stop();
	TIM2->ARR=arr;  //设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  //预分频器7200,得到10Khz的计数时钟
	TIM2->CR1|=0x01;    //使能定时器3
}

#if 0
void TmCaptureInit(u16 arr, u16 psc)
{
	RCC->APB2ENR|=1<<0;    //  
	RCC->APB1ENR|=1<<0;       //TIM2 鏃堕挓浣胯兘 
	RCC->APB2ENR|=1<<2;    //浣胯兘PORTA鏃堕挓 
	   
 	TIM2->ARR=arr;  //璁惧畾璁℃暟鍣ㄨ嚜鍔ㄩ噸瑁呭�?/鍒氬ソ1ms    
	TIM2->PSC=psc;  //棰勫垎棰戝櫒,

	GPIOA->CRL&=0XFFFFFFF0;//PA0 杈撳嚭  
	GPIOA->CRL|=0X00000004;//澶嶇敤鍔熻兘杈撳嚭       
	GPIOA->ODR|=1<<0;//PA0 涓婃媺
	  
	TIM2->SMCR|=0x00D4;//0x00D4; 
	//TIM2->SMCR|= 1<<5; //MSM=1 涓?浠庢ā寮?
	//TIM2->SMCR|= 5<<4; //TS=101 瑙﹀彂閫夋嫨 
	//TIM2->SMCR|= 4<<0; //SMS=100 澶嶄綅妯″紡
		   
	TIM2->CCMR1|=1<<0;//CC1S=01 閫夋嫨杈撳叆绔? 
	//TIM2->CCMR1|=3<<4; //IC1F=0011閰嶇疆杈撳叆婊ゆ尝鍣?
	TIM2->CCER|=1<<1; //CC1P=0	捕获下降沿
	TIM2->CCMR1|=0<<2; //IC1PS=00 输出比较1快速使能
	TIM2->CCER|=1<<0; //CC1E=1 捕获使能

	TIM2->DIER|=1<<1;   //允许捕获/比较1中断	
	//TIM2->DIER|=0x01;	//允许更新中断
			   
	TIM2->CR1|=0x01;    //使能定时器2
  	MY_NVIC_Init(1,3,TIM2_IRQChannel,2);//鎶㈠崰1锛屽瓙浼樺厛绾?锛岀粍2	
}

void TIM2_IRQHandler(void)			//timer2 is for slave light mode overflow
{ 		    		  	
//	u16 IC2Value[200];
//	static u16 cnt=0;
//	u8 i;

	if(TIM2->SR&0x02)
	{
		//here load the command data
//		IC2Value[cnt++]=TIM2->CCR1;			//读取CCR1也可以清除CC1IF标志位
		TIM2->DIER|=0x01;
		TIM2->SR&=~(1); 
	//IC2Value = TIM2->CCR1;	//printf("IC2Value = %d\r\n",1000000/IC2Value);			     	    					   
	//TIM2->SR&=~(1<<1);
	}	

	if(TIM2->SR&0x01)
	{
		//printf("Tm2 captured: ");
		//for(i=0;i<cnt;i++)
		//{
			//printf("%d",IC2Value[i]);
			//putchar(IC2Value[i]);
		//}
		//here inparse the data
		//uartSendData(IC2Value,cnt);
		//printf("\r\n");
		//putchar(0xFF);
		//cnt = 0;

		TIM2->DIER&=0xFE;
		TIM2->SR&=~(1);
	}

}
#endif

void TmTickHandle()
{
	//FLASH_CONTROL = !FLASH_CONTROL;
	//OSTmrSignal();
	
}

void TaskTimerTickCom(void *pdata)
{
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,200);	
		//TmTickHandle();
		//OSTaskSuspend(TICK_TASK_Prio);
	}
}

#ifdef MY_TIMERS
static  void  Tmr_Lock (void)
{
    INT8U  err;


    OSSemPend(OSTmrSem, 0, &err);
    (void)err;
}

static  void  Tmr_Unlock (void)
{
    (void)OSSemPost(OSTmrSem);
}

static  OS_TMR  *Tmr_Alloc (void)
{
    OS_TMR *ptmr;


    if (OSTmrFreeList == (OS_TMR *)0) {
        return ((OS_TMR *)0);
    }
    ptmr            = (OS_TMR *)OSTmrFreeList;
    OSTmrFreeList   = (OS_TMR *)ptmr->OSTmrNext;
    ptmr->OSTmrNext = (OS_TCB *)0;
    ptmr->OSTmrPrev = (OS_TCB *)0;
    OSTmrUsed++;
    OSTmrFree--;
    return (ptmr);
}

OS_TMR  *TmrCreate (INT32U           dly,
                      INT32U           period,
                      INT8U            opt,
                      OS_TMR_CALLBACK  callback,
                      void            *callback_arg,
                      INT8U           *perr)
{
    OS_TMR   *ptmr;
	
    Tmr_Lock();
	
    ptmr = Tmr_Alloc();                                   /* Obtain a timer from the free pool                      */
    if (ptmr == (OS_TMR *)0) {
        Tmr_Unlock();
        *perr = OS_ERR_TMR_NON_AVAIL;
        return ((OS_TMR *)0);
    }
    ptmr->OSTmrState       = OS_TMR_STATE_STOPPED;          /* Indicate that timer is not running yet                 */
    ptmr->OSTmrDly         = dly;
    ptmr->OSTmrPeriod      = period;
    ptmr->OSTmrOpt         = opt;
    ptmr->OSTmrCallback    = callback;
    ptmr->OSTmrCallbackArg = callback_arg;
	
    Tmr_Unlock();
    *perr = OS_ERR_NONE;
    return (ptmr);
}

static  void  Tmr_InitTask (void)
{
	(void)OSTaskCreate(Tmr_Task,
                       (void *)0,
                       &OSTmrTaskStk[OS_TASK_TMR_STK_SIZE - 1],
                       OS_TASK_TMR_PRIO);
}

void  Tmr_Init (void)
{
    INT16U   i;
    OS_TMR  *ptmr1;
    OS_TMR  *ptmr2;


    OS_MemClr((INT8U *)&OSTmrTbl[0],      sizeof(OSTmrTbl));            /* Clear all the TMRs                         */
    OS_MemClr((INT8U *)&OSTmrWheelTbl[0], sizeof(OSTmrWheelTbl));       /* Clear the timer wheel                      */

    ptmr1 = &OSTmrTbl[0];
    ptmr2 = &OSTmrTbl[1];
    for (i = 0; i < (OS_TMR_CFG_MAX - 1); i++) {                        /* Init. list of free TMRs                    */
        ptmr1->OSTmrType    = OS_TMR_TYPE;
        ptmr1->OSTmrState   = OS_TMR_STATE_UNUSED;                      /* Indicate that timer is inactive            */
        ptmr1->OSTmrNext    = (void *)ptmr2;                            /* Link to next timer                         */
        ptmr1++;
        ptmr2++;
    }
    ptmr1->OSTmrType    = OS_TMR_TYPE;
    ptmr1->OSTmrState   = OS_TMR_STATE_UNUSED;                          /* Indicate that timer is inactive            */
    ptmr1->OSTmrNext    = (void *)0;                                    /* Last OS_TMR                                */
	
    OSTmrTime           = 0;
    OSTmrUsed           = 0;
    OSTmrFree           = OS_TMR_CFG_MAX;
    OSTmrFreeList       = &OSTmrTbl[0];
    OSTmrSem            = OSSemCreate(1);
    OSTmrSemSignal      = OSSemCreate(0);

    Tmr_InitTask();
}

static  void  Tmr_Unlink (OS_TMR *ptmr)
{

}

static  void  Tmr_Link (OS_TMR *ptmr, INT8U type)
{
}

static  void  Tmr_Task (void *p_arg)
{
    INT8U            err;
    OS_TMR          *ptmr;
    OS_TMR          *ptmr_next;
    OS_TMR_CALLBACK  pfnct;
    OS_TMR_WHEEL    *pspoke;
    INT16U           spoke;


    (void)p_arg;                                                 /* Not using 'p_arg', prevent compiler warning       */
    for (;;) {
        OSSemPend(OSTmrSemSignal, 0, &err);                      /* Wait for signal indicating time to update timers  */
        Tmr_Lock();
        OSTmrTime++;                                             /* Increment the current time                        */
        spoke  = (INT16U)(OSTmrTime % OS_TMR_CFG_WHEEL_SIZE);    /* Position on current timer wheel entry             */
        pspoke = &OSTmrWheelTbl[spoke];
        ptmr   = pspoke->OSTmrFirst;
        while (ptmr != (OS_TMR *)0) {
            ptmr_next = (OS_TMR *)ptmr->OSTmrNext;               /* Point to next timer to update because current ... */
                                                                 /* ... timer could get unlinked from the wheel.      */
            if (OSTmrTime == ptmr->OSTmrMatch) {                 /* Process each timer that expires                   */
                pfnct = ptmr->OSTmrCallback;                     /* Execute callback function if available            */
                if (pfnct != (OS_TMR_CALLBACK)0) {
                    (*pfnct)((void *)ptmr, ptmr->OSTmrCallbackArg);
                }
                Tmr_Unlink(ptmr);                              /* Remove from current wheel spoke                   */
                if (ptmr->OSTmrOpt == OS_TMR_OPT_PERIODIC) {
                    Tmr_Link(ptmr, OS_TMR_LINK_PERIODIC);      /* Recalculate new position of timer in wheel        */
                } else {
                    ptmr->OSTmrState = OS_TMR_STATE_COMPLETED;   /* Indicate that the timer has completed             */
                }
            }
            ptmr = ptmr_next;
        }
        Tmr_Unlock();
    }
}



#endif












