#include "timer.h"
#include "../../APP/canon.h"
#include "../../APP/golbal.h"
#include "../../APP/Display.h"
//#include "led.h"

extern unsigned char g_comStatue;
extern u16 g_tmEvent;
extern u16 g_dspUpdateFlag;
extern unsigned char g_tmHandleFlag;
extern WlSet wlSet;
extern RadioStatue radioStatue;
extern char g_SettingIndex;

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

//void Timer3_Start(void);
//void Timer3_Stop(void); 

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
	menuPara.cApert=0;
	//g_dspUpdateFlag|=DSP_UPDATE_LINK;
	//SET_UPDATE_EVT(DSP_UPDATE_LINK);			//wried
	SPI_MISO_AUX  = 0;
	//FLAOK_OFF();
#ifdef SIGNAL_TEST
	SIGNAL_PIN2 = !SIGNAL_PIN2;
#endif
	//g_menuLock = 0;
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_LINK);
}

//¶¨Ê±Æ÷3ÖÐ¶Ï·þÎñ³ÌÐò	 
void TIM4_IRQHandler(void)
{ 		    		  			    
//	if(TIM3->SR&0X0001)//Òç³öÖÐ¶Ï
//	{
//		LED1=!LED1;			    				   				     	    	
//	}
/*
	if(TIM3->SR&0X0001)
	{
		FLASH_CONTROL = !FLASH_CONTROL;
	}
	TIM3->SR&=~(1<<0);
*/
	if(TIM4->SR&0x0001)
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
#if 0
				//SPIx_Disable();
				Com_Stop();
				Ex_NVIC_Config(GPIO_A,5,RTIR);
				Ex_NVIC_Enable(5);
				g_comStatue = COM_STATUE_IDLE;
				SET_UPDATE_EVT(DSP_UPDATE_LINK);
				g_cmrLinked = 0;
				g_cmrPressed=0;
				menuPara.cApert=0;
				//g_dspUpdateFlag|=DSP_UPDATE_LINK;
				//SET_UPDATE_EVT(DSP_UPDATE_LINK);			//wried
				SPI_MISO_AUX  = 0;
				//FLAOK_OFF();
#ifdef SIGNAL_TEST
				SIGNAL_PIN2 = !SIGNAL_PIN2;
#endif
#endif
				UnlinkCmrHandle();
				//Timer4_Stop();
			}
		}else if(TM_HANDLE_MULTIFLASH==g_tmHandleFlag)
		{
			wlSet.m_cmdProc = WLM_CMDPROC_MUL;
			radioStatue.cmdSta = RADIO_STATUE_MULTIFLASH;
			radioStatue.cmdReady = 1;
			//ENABLE_D30EN();
			OSTaskResume(CMDSEND_TASK_Prio);
			
		}
		g_tmHandleFlag = TM_HANDLE_CLR;
#endif
	}
	Timer4_Stop();
	TIM4->SR&=~(1<<0);//Çå³ýÖÐ¶Ï±êÖ¾Î» 	    
	//Timer3_Stop();
}

#if 0
void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0x0001)
	{
		//FLASH_CONTROL = !FLASH_CONTROL;
		//OSTaskResume(TICK_TASK_Prio);
		OSTmrSignal();
	}
	TIM4->SR&=~(1<<0);
}

void Timer4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;//TIM5Ê±ÖÓÊ¹ÄÜ    
 	TIM4->ARR=arr;  //Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ//¸ÕºÃ1ms    
	TIM4->PSC=psc;  //Ô¤·ÖÆµÆ÷7200,µÃµ½10KhzµÄ¼ÆÊýÊ±ÖÓ
	//ÕâÁ½¸ö¶«¶«ÒªÍ¬Ê±ÉèÖÃ²Å¿ÉÒÔÊ¹ÓÃÖÐ¶Ï
	TIM4->DIER|=1<<0;   //ÔÊÐí¸üÐÂÖÐ¶Ï				
	TIM4->CNT = 0x00;
	//TIM6->DIER|=1<<6;   //ÔÊÐí´¥·¢ÖÐ¶Ï	   
	TIM4->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷3
	TIM4->SR&=~(1<<0);
	//MY_NVIC_Init(2,3,TIM4_IRQChannel,2);//ÇÀÕ¼1£¬×ÓÓÅÏÈ¼¶3£¬×é2		
	MY_NVIC_Init(2,3,TIM4_IRQn,2);//ÇÀÕ¼1£¬×ÓÓÅÏÈ¼¶3£¬×é2		

}
#endif

void Timer4_Init(u16 arr, u16 psc)		//used for acqure 1ms tick
{
	RCC->APB1ENR|=1<<2;//TIM4Ê±ÖÓÊ¹ÄÜ    
 	TIM4->ARR=arr;  //Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ//¸ÕºÃ1ms    
	TIM4->PSC=psc;  //Ô¤·ÖÆµÆ÷7200,µÃµ½10KhzµÄ¼ÆÊýÊ±ÖÓ
	//ÕâÁ½¸ö¶«¶«ÒªÍ¬Ê±ÉèÖÃ²Å¿ÉÒÔÊ¹ÓÃÖÐ¶Ï
	TIM4->DIER|=1<<0;   //ÔÊÐí¸üÐÂÖÐ¶Ï				
	TIM4->CNT = 0x00;
	//TIM6->DIER|=1<<6;   //ÔÊÐí´¥·¢ÖÐ¶Ï	 
	TIM4->SR&=~(1<<0);
	TIM4->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷3
	//TIM4->SR&=~(1<<0);
	//MY_NVIC_Init(2,3,TIM4_IRQChannel,1);//ÇÀÕ¼1£¬×ÓÓÅÏÈ¼¶3£¬×é2	
	MY_NVIC_Init(2,3,TIM4_IRQn,1);//ÇÀÕ¼1£¬×ÓÓÅÏÈ¼¶3£¬×é2	
}

void Timer4_Update(u16 arr, u16 psc)
{
	Timer4_Stop();
	TIM4->ARR=arr;  //Éè¶¨¼ÆÊýÆ÷×Ô¶¯ÖØ×°Öµ//¸ÕºÃ1ms    
	TIM4->PSC=psc;  //Ô¤·ÖÆµÆ÷7200,µÃµ½10KhzµÄ¼ÆÊýÊ±ÖÓ
	TIM4->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷3
}

#if 1
void TmCaptureInit(u16 arr, u16 psc)
{
	//RCC->APB2ENR|=1<<0;    //  ¸¨Öú¹¦ÄÜIOÊ±ÖÓÊ¹ÄÜ
	RCC->APB1ENR|=1<<1;       //TIM3Ê±ÖÓÊ¹ÄÜ    
	//RCC->APB2ENR|=1<<2;    //ä½¿èƒ½PORTAæ—¶é’Ÿ 
	RCC->APB2ENR|=1<<4;        //ÏÈÊ¹ÄÜÍâÉèPORTCÊ±ÖÓ
	RCC->APB2ENR|=1<<0;	//¸¨Öú¹¦ÄÜIOÊ±ÖÓ

	AFIO->MAPR |= 3<<10;	//Timer3 remap
	   
 	TIM3->ARR=arr;  //è®¾å®šè®¡æ•°å™¨è‡ªåŠ¨é‡è£…å€¼//åˆšå¥½1ms    
	TIM3->PSC=psc;  //é¢„åˆ†é¢‘å™¨,

	GPIOC->CRL&=0XF0FFFFFF;//PA0 è¾“å‡º  
	GPIOC->CRL|=0X04000000;//å¤ç”¨åŠŸèƒ½è¾“å‡º       
	GPIOC->ODR|=1<<6;//PA0 ä¸Šæ‹‰
	  
	//TIM3->SMCR = 0x00F4;//0x00D4; 
	//TIM3->SMCR = 0x00A4;
	TIM3->SMCR = 0x00D4;
	//TIM3->SMCR|=1<<2;//0x00D4; 
	//TIM2->SMCR|= 1<<5; //MSM=1 ä¸»/ä»Žæ¨¡å¼ 
	//TIM2->SMCR|= 5<<4; //TS=101 è§¦å‘é€‰æ‹© 
	//TIM2->SMCR|= 4<<0; //SMS=100 å¤ä½æ¨¡å¼
		   
	//TIM3->CCMR1|=1<<0;//CC1S=01 é€‰æ‹©è¾“å…¥ç«¯  
	//TIM3->CCMR2|=1<<8;//CC4S=01 é€‰æ‹©è¾“å…¥ç«¯  
	TIM3->CCMR1|=1<<0;
	//TIM2->CCMR1|=3<<4; //IC4F=0011é…ç½®è¾“å…¥æ»¤æ³¢å™¨
	//TIM3->CCER|=1<<13; //CC4P=0	²¶»ñÏÂ½µÑØ
	TIM3->CCER|=1<<1;
	TIM3->CCMR1|=0<<2; //IC4PS=00 Êä³ö±È½Ï1¿ìËÙÊ¹ÄÜ
	TIM3->CCER|=1<<0; //CC4E=1 ²¶»ñÊ¹ÄÜ

	//TIM3->DIER|=1<<1;   //ÔÊÐí²¶»ñ/±È½Ï1ÖÐ¶Ï	
	TIM3->DIER|=1<<1;   //ÔÊÐí²¶»ñ/±È½Ï4ÖÐ¶Ï	
	//TIM2->DIER|=0x01;	//ÔÊÐí¸üÐÂÖÐ¶Ï
			   
	TIM3->CR1|=0x01;    //Ê¹ÄÜ¶¨Ê±Æ÷3
  //MY_NVIC_Init(1,3,TIM3_IRQChannel,2);//æŠ¢å 1ï¼Œå­ä¼˜å…ˆçº§3ï¼Œç»„2	
	MY_NVIC_Init(1,3,TIM3_IRQn,2);//æŠ¢å 1ï¼Œå­ä¼˜å…ˆçº§3ï¼Œç»„2	
}

void StopCapture(void)
{
	TIM3->CCER&=~(1<<0);
	TIM3->DIER&=~(1<<1);
	TIM3->DIER&=~(1<<0);
	//FLAOK_OFF();
}

void StartCapture(void)
{
	TIM3->CCER|=1<<0;	
	TIM3->DIER |=1<<1;
	TIM3->SR&=~(1);
	//FLAOK_ON();
}

void TIM3_IRQHandler(void)			//timer2 is for slave light mode overflow
{ 		
#if 0
	static u16 IC2Value[200];
	static u16 cnt=0;
	u8 i;
#endif

	//FLAOK_TOGGLE();

	//if(TIM3->SR&0x02)
	if(TIM3->SR&(1<<1))
	{
		//FLAOK_ON();
		//here load the command data
#ifdef TTL_DEBUG
		//IC2Value[cnt++]=TIM3->CCR1;
#endif
		//FLAOK_TOGGLE();
		LightSlaveProc(TIM3->CCR1);//IC2Value[cnt++]=TIM3->CCR1;			//¶ÁÈ¡CCR1Ò²¿ÉÒÔÇå³ýCC1IF±êÖ¾Î»
		TIM3->DIER|=0x01;					//enable update event
		//TIM3->SR|=(1<<4); 
		TIM3->SR&=~(1);
		//SIGNAL1 = !SIGNAL1;
	//IC2Value = TIM2->CCR1;	//printf("IC2Value = %d\r\n",1000000/IC2Value);			     	    					   
	//TIM2->SR&=~(1<<1);
	}	

	if(TIM3->SR&(1<<0))							//update event
	{
		//SIGNAL2 = !SIGNAL2;
		//printf("Tm2 captured: ");
#if 0
		for(i=0;i<cnt;i++)
		{
			DBG_PRINTF("%d ",IC2Value[i]);
		}
		cnt = 0;
		DBG_PRINTF("\r\n");
#else
		ParseMstCmd();
#endif
		//FLAOK_OFF();
		//FLAOK_TOGGLE();
		//ParseMstCmd();
		//here inparse the data
		//uartSendData(IC2Value,cnt);
		//printf("\r\n");
		//putchar(0xFF);
		//cnt = 0;
		TIM3->DIER&=0xFE;					//disable update event
		TIM3->SR&=~(1);
	}

//	SIGNAL2 = !SIGNAL2;

	//TIM4->SR&=~(1<<0);

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

/*
void Timer3_Start()
{
	//TIM3->ARR = 10;
	TIM3->CR1|=0x01;	 
	//TIM3->SR&=~(1<<0);
}

void Timer3_Stop()
{
	 TIM3->CR1&=0xFE;
	 //TIM3->DIER&=~(1<<0);
	 //TIM3->DIER&=~(1<<6);
}*/














