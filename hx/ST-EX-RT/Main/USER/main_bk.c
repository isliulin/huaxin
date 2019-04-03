#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	
#include "includes.h"	 
#include "../HARDWARE/spi/spi.h"
#include "canon.h"
#include "Golbal.h"
#include "Display.h"
#include "../HARDWARE/lcd/Lcd.h"
#include "keyproc.h"
#include "../HARDWARE/lcd/font.h"
#include "../HARDWARE/TIMER/timer.h"
#include "../HARDWARE/STMFLASH/stmflash.h"
//#include "boot.h"


//任务堆栈
//OS_STK  TASK_CANCOM_STK[CANCOM_STK_SIZE];
OS_STK  TASK_KEYPROC_STK[KEYPROC_STK_SIZE];
OS_STK  TASK_START_STK[START_STK_SIZE];
OS_STK  TASK_LED_STK[LED_STK_SIZE];
OS_STK  TASK_REFRESH_STK[REFRESH_STK_SIZE];
OS_STK	TASK_CMDSEND_STK[CMDSEND_STK_SIZE];
OS_STK	TASK_TICK_STK[TICK_STK_SIZE];

//任务申明
void TaskStart(void *pdata);
void TaskCanCom(void *pdata);
void TaskKeyScan(void *pdata);
//void TaskLed1(void *pdata);
void TaskLed(void *pdata);
void TaskCmdSend(void *pdata);
extern void TaskDspUpdate(void *pdata);
extern void U2SendData(u8 dat);
void UserInit(void);
void RfParaSend(void);
extern void HpPreflash(unsigned int tmOv);

//#ifdef _UART_INT_INFO_
//extern void SpiRearUp(u16 tks,u8 evt);
//#else
void SpiRearUp(u16 tks);
//#endif

MainPara mainPara;
unsigned char g_dspStyle=DSP_STYLE_CANON;
//RadioSet radioSet={0};
RadioSet* radioSet = &mainPara.rfSet;
unsigned char g_grDspStart=0;
unsigned char g_grSel=0;
unsigned char g_grStyle=0;
unsigned char g_grSelRsv=0;
WL_ArrPara *mstPara;// = &(mainPara.modGr[0]);
RadioStatue radioStatue;
Wl_Slave wlSlave;
extern unsigned char *g_cmdMode;
extern MultiSet multiSet;
extern unsigned char uart_tran_flag;
SnMac g_snMac;
u16 g_tmEvent;
char g_ver[10]={0};
extern Word gWSleepCounter;
//extern char g_stByTime;
extern char *pSnrSet;
extern u16 g_dspUpdateFlag;
extern char g_cc2530Start;
extern char g_cc2530Wake;
extern unsigned char g_shutterFlag;
u16 g_bkPwmDuty;
extern unsigned char g_menuId;
extern unsigned char abRation_fill1,abRation_fill2;
extern WLM_abRation_fillwords const wlm_abRation_fillwords[13];
extern unsigned char g_getTtlFlashCode;
extern char g_cmrLinked;
//const u8 cpuIdVeri[14] _at_ (FLASH_APP1_ADDR);
//char xdata LED_Data[50] _at_ 0x8000;
//u8 const TempBuf [16] __attribute__((at(FLASH_APP1_ADDR+0x1000))) = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};
#ifdef _SN_DEBUG_
//u8 g_dspChinese=1;
u8 g_dspChinese=0;
#else
u8 g_dspChinese=0;
#endif

#ifdef _1DX_DEBUG_
extern u16 g_UnknowData[10];
extern u8 g_UkdCnt;
#endif

//u8 g_bkState=0;
//u8 g_bkFlag=0;
#if OS_TMR_EN>0
OS_TMR  *g_bkTmr;
OS_TMR	*g_bzTmr=NULL;
OS_TMR  *g_1ShotTmr;
OS_TMR  *g_sysStartTmr;
OS_TMR  *g_CcRstDownTmr;
//OS_TMR  *g_CcWakeTmr;
OS_TMR  *g_shutterPullTmr;
OS_TMR  *g_slaCmdTmr;
#endif

extern unsigned char g_vCmr;

//系统时钟配置函数
void SysTick_Configuration(void);

unsigned char g_comStatue=0;

void Flash_Interrupt_init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	//JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

	GPIOA->CRL&=0XFFF0FFFF;//PA4设置成输入	  
	GPIOA->CRL|=0X00080000;   				   
	GPIOA->ODR|=1<<4;	   //PA4上拉,PA0默认下拉

	Ex_NVIC_Config(GPIO_A,4,FTIR); //下降沿触发

	//MY_NVIC_Init(2,2,EXTI4_IRQChannel,2);    //抢占2，子优先级2，组2
	MY_NVIC_Init(2,2,EXTI4_IRQn,2);    //抢占2，子优先级2，组2
}

void SetupBatCheck(void)
{
	u16 batVal=0;
	char i=0;
	//uart1_init(72,115200);
#ifdef UART1_TRACE_DEBUG
	printf("SetupBatCheck\r\n");
#endif
	for(i=0;i<16;i++)
	{
		ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
		ADC1->SQR3|=AD_CHANNEL_BAT;
		ADC1->CR2|=1<<22;
		while(!(ADC1->SR&1<<1));
		batVal+=(ADC1->DR>>4);
#ifdef UART1_TRACE_DEBUG
		printf("%d ",batVal);
#endif
	}
	batVal>>=4;
	if(batVal<BAT_CHECK_VAL)
	{
		SysTick->CTRL&=~(1<<0);
		LCD_Clear(BACK_COLOR);
		//uart1_init(72,115200);
		BkLiteOn();
		//POW_ON = 0;	
		if(!g_dspChinese)
		{
			LCD_ShowString(LAYOUT_RATIO_A,20,"Warning! Low battery!",FONT_TYPE_MAIN,0,DSP_NORMAL);
		}else
		{
			LCD_ShowCnString(LAYOUT_RATIO_A, 50, "警告!电池电量低!", DSP_NORMAL);
			//LCD_ShowCnChar(20,20,"警",DSP_NORMAL);
			//LCD_ShowCnString(LAYOUT_RATIO_A, 20, "Warning! Low battery!", DSP_NORMAL);
		}
#ifdef UART1_TRACE_DEBUG
		printf("bat %d\r\n",batVal);
#endif
		POW_ON = 0;	
		BK_LIGHT = 1;	
		DISABLE_CHARGE;
	}
	while(batVal<BAT_CHECK_VAL);
	
}

void SetupDisableCharge()
{
	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	GPIOC->CRH &= 0xFFFFF0FF;
	GPIOC->CRH |= 0x00000300;
	//GPIOC->ODR |= (1<<10);
	DISABLE_CHARGE;
}

void SetPwrOnExti(void)
{
	Ex_NVIC_Config(GPIO_C,9,FTIR); 
	//Ex_NVIC_Config(GPIO_B,10,RTIR);
	//MY_NVIC_Init(0,0,EXTI9_5_IRQChannel,2);
	MY_NVIC_Init(0,0,EXTI9_5_IRQn,2);
}

void CheckPwrSw(void)
{
	//RCC->APB2ENR|=1<<2;//先使能外设PORTA时钟
 	//RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	//RCC->APB2ENR |=1<<5;		//enable port D clock

	GPIOC->CRL &= 0xFF0FFFFF;
	GPIOC->CRL |= 0x00300000;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON

	GPIOC->CRH &= 0xFFFF0F00;
	GPIOC->CRH |= 0x00003088;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON
	//POW_ON = 0;
	POW_ON= 1;
	GPIOC->ODR |= 0x0300;
	
	PERIPHERAL_PWR_OFF();
	
	if((KEY_LOCK_EX>0)&&(KEY_POW_ON>0))	
	{
		delay_ms(100);

		if((KEY_LOCK_EX>0)&&(KEY_POW_ON>0))	
		{

			//PERIPHERAL_PWR_OFF();
			
			delay_ms(500);
			
			POW_ON = 0;

			SetPwrOnExti();
				
			PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

			RCC_Configuration();
		}
	}
}
	 
int main(void)
{			
//	int tick=0;
	Stm32_Clock_Init(9); //系统时钟设置
	delay_init(72);	     //延时初始化 
	//uart2_init(36,115200);	 //只有USART1使用PCLK2(最高72MHz)。其它USART使用PCLK1(最高36MHz)
	//uart1_init(72,115200);
	//LED_Init();		  	 //初始化与LED连接的硬件接口
	//SPIx_Init();
	//printf("start");
	CheckPwrSw();
#ifndef _RF_ONLY_
	SetupDisableCharge();
#endif
	//LCD_Init();
	Timer2_Init(40,72);
	Adc_Init();
	PortInit();
	//StartupBatCheck();
	//LCD_Init();
	LCD_Init();
	Flash_Interrupt_init();
	//TmCaptureInit();
	//uart2_init(36,115200);	 //只有USART1使用PCLK2(最高72MHz)。其它USART使用PCLK1(最高36MHz)
	//uart2_init(36,115200);
#ifdef UART1_TRACE_DEBUG
	uart1_init(72,115200);
	printf("Uart1 trace debug\r\n");
	printf(__DATE__);
	printf("\r\n");
	printf(__TIME__);
	printf("\r\n");
#endif
//Initial the UART port after the timer is created, in order to avoid the g_buzTmr is null
/*
#ifdef _UART_INT_INFO_
	uart2_init(36,115200);		   
#else
	uart2_init(36,57600);
#endif
*/
	
	UserInit();
	//Timer4_Init(10,72);
	//Sys_StopMode();
	//EXTIx_Init(RTIR);
	//WakeUp_Init(FTIR);
	//PWM_Init(2500,9,1250);
	//uartSendData("capture test\r\n",sizeof("capture test\r\n"));
	//USART_Configuration();
	//Timerx_Init(10,7199);
	//EXTIx_Init(RTIR);
	SysTick_Configuration();
	OSInit();
//#if OS_TMR_EN > 0
//    	OSTmr_Init();                                                /* Initialize the Timer Manager             */
//#endif
		
	OSTaskCreate( TaskStart,	//task pointer
					(void *)0,	//parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
					START_TASK_Prio );	//task priority
	//printf("os start");	
/*
	while(1)
	{
		if(tick++>30)
		{
			tick = 0;
			LowPow_Init(FTIR);
		}
		FLA_OK_LED=!FLA_OK_LED;
		delay_ms(300);
	}
*/
	OSStart();
	return 0;	   
}

void FlaOkAutoOff(void)
{
	FLAOK_OFF();
}

void CC2530_RstDown(void)
{
	DISABLE_RF_MODULE();
}

#if 0
void CC2530_WakeUp(void)
{
	if(g_cc2530Wake)
	{
		DISABLE_RF_MODULE();
		OSTimeDlyHMSM(0,0,0,10);
		ENABLE_RF_MODULE();
		g_cc2530Wake = 0;
	}
}
#endif

void CmrShutterPull(void)
{
	if(COM_STATUE_IDLE==g_comStatue)
	{
		ACT_SHUTTER = 0;
		//WIRE_SHUTTER = 0;
		//CHARGE_LED_OFF;
		g_shutterFlag = 0;
	}
	FLAOK_OFF();
	g_shutterFlag = 0;
	WIRE_SHUTTER = 0;
}
	
void BK_PWM(u16 arr,u16 psc,u16 duty)
{
	RCC->APB1ENR|=1<<2;       //TIM4时钟使能    
	RCC->APB2ENR|=1<<3;
	
	//RCC->APB2ENR|=1<<0;
	//AFIO->MAPR |= 3<<10;	//Timer3 remap
	GPIOB->CRH&=0xFFFFFF0F;//PC6输出
	GPIOB->CRH|=0x000000B0;//复用功能输出 	  
	//GPIOB->ODR|=1<<9;//PC6上拉	  	

	TIM4->ARR=arr;//设定计数器自动重装值 
	TIM4->PSC=psc;//预分频器不分频

	TIM4->CCR4 = duty;
	
	//TIM3->CCMR1|=7<<12;  //CH2 PWM2模式		 
	//TIM3->CCMR1|=1<<11; //CH2预装载使能	   
	TIM4->CCMR2|=7<<12;
	TIM4->CCMR2|=1<<11;

	//TIM3->CCER|=1<<4;   //OC2 输出使能	   
	TIM4->CCER|=1<<12;   //OC1 输出使能	   

	TIM4->CR1=0x0080;   //ARPE使能 
	TIM4->CR1|=0x01;    //使能定时器3 

	//(TIMx->CNT>TIMx->CCRx)?
}


void BkLiteOn(void)
{
	INT8U tmrErr;
	//BK_LIGHT = 1;
	//BK_PWM(2500,9,1250);
	BK_PWM(BK_PWM_ARR,9,g_bkPwmDuty);
#if(_HARD_WARE_==HARD_WARE_RFONLY)
	KEY_BK = 1;
#endif
	bkParas.bkState= BK_ON;
	bkParas.bkFlag = BK_ON;
#if OS_TMR_EN > 0
	OSTmrStart(g_bkTmr, &tmrErr) ;
#endif
}

void BkLiteAutoOff(void)
{
	//if(COM_STATUE_IDLE==g_comStatue)
	{
		//BK_LIGHT = 0;
		//TIM4->CCMR2&=0x8FFF;
		//TIM4->CCMR2|=0x4000;
		TIM4->CCER&=~(1<<12); 
		TIM4->CR1&=0xFE; 
		bkParas.bkState= BK_OFF;
	}	
	bkParas.bkFlag = BK_OFF;
#if(_HARD_WARE_==HARD_WARE_RFONLY)
	KEY_BK = 0;
#endif
}

void BUZZER_ON(u16 duration,u16 times)
{
#if OS_TMR_EN > 0
	INT8U tmrErr;
	if(pSnrSet[SNRMENU_SOUND])
	{
		g_bzCount=times;
		if(g_bzTmr!=NULL)
		{
			PWM_Init(2500,9,1250);
			OSTmrReload(g_bzTmr,duration*100);
			OSTmrStart(g_bzTmr, &tmrErr);
		}
	}
#endif
}

/*
void SysStartTmrProc(void)
{
//	INT8U err;
#ifdef UART1_TRACE_DEBUG
	printf("SysStartTmr\r\n");
#endif
	if(g_cc2530Start)
	{
#ifdef UART1_TRACE_DEBUG
	printf("cc2530Start\r\n");
#endif
		RfParaSend();
		g_cc2530Start = 0;
		//OSTmrStop(g_sysStartTmr,OS_TMR_OPT_NONE,NULL,&err);
	}
}
*/

void BzOff()
{
#if 1
	static u16 du=0, ts=0;
//	INT8U tmrErr;
	du++;
	if(du>=g_bzDuration)
	{
		du = 0;
		ts++;
		if(ts>=g_bzCount)
		{
			TIM3->CCMR1&=0xFF8F;
			TIM3->CCMR1|=0x0040;
			TIM3->CR1&=0xFE; 
			//OSTmrStop(g_bzTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);	
			ts = 0;
		}else if(ts&1)
		{
			TIM3->CCMR1&=0xFF8F;
			TIM3->CCMR1|=0x0040;
			TIM3->CR1&=0xFE; 				//stop a cycle
		}else
		{
			PWM_Init(2500,9,1250);
			//OSTmrStart(g_bzTmr, &tmrErr) ;	//restart
		}
	}
#endif
}

void RfParaSend(void)
{
/******************************************************************************************************
//	send rf parameter
******************************************************************************************************/
	LoadUartSendBuf(UART_ENCODE_RFCH,radioSet->rfChan,0);
	LoadUartSendBuf(UART_ENCODE_RFID, radioSet->id>>8,radioSet->id&0xff);
//	LoadUartArray(UART_ENCODE_RFADDR,g_snMac.mac,8);
	LoadUartSendBuf(UART_ENCODE_RFMOD, *g_cmdMode,0);
	//TrigerUartSend();
	TrigerUartSend(0);
}

void ParaFormat(void)
{
	u16 i;
#ifdef _ST_EX_
	if((*g_cmdMode!=CMD_MODE_LIGH_MST)&&(*g_cmdMode!=CMD_MODE_RF_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_SLA)) *g_cmdMode = CMD_MODE_RF_MST;
#else
	if((*g_cmdMode<CMD_MODE_OFF)||(*g_cmdMode>CMD_MODE_RF_MST)) *g_cmdMode = CMD_MODE_OFF;
#endif
	
	if((mainPara.workMode<WORKINGMODE_TTL)||(mainPara.workMode>WORKINGMODE_F2)) mainPara.workMode = WORKINGMODE_TTL;
	if((90<*multiSet.eTimes)||(*multiSet.eTimes<0)) *multiSet.eTimes = 0;
	if((*multiSet.frequence>500)||(*multiSet.frequence<1)) *multiSet.frequence = 1;
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);

	//menuPara.mode = ((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07);

	for(i=0;i<3;i++)
	{
		menuPara.otpCod[i] = output_table[(7-wlSet.m_arrPara[i].output1)*3-wlSet.m_arrPara[i].output2];
		menuPara.WlmMul[i] = output_table[(7-(wlSet.m_arrPara[i].multi_exp>>4))*3];
		//menuPara.WlmMul[WL_ARRARY_B] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4))*3];
	}

	for(i=0;i<5;i++)
	{
		menuPara.rfGrOtp[i] = output_table[(7-radioSet->rfGr[i].m_exp1)*3-radioSet->rfGr[i].m_exp2];
		menuPara.rfGrTmp[i] = compensation_table[(radioSet->rfGr[i].ttl_cmp1+6)*3+radioSet->rfGr[i].ttl_cmp2];
	}

	//RfParaSend();
}

void UserInit(void)
{
	u16 i;
	u8 sum=0;
	char *pMain;
	u16 ver=0;

/*
	LCD_Clear(BACK_COLOR);
	
	for(i=0;i<4;i++)
	{
		LcdFillBlank(
			LAYOUT_MENU_PAGE,
			i*(LAYOUT_BUTTON_WIDTH+LAYOUT_BUTTON_INTERVAL),
			LAYOUT_BUTTON_WIDTH,
			LAYOUT_BUTTON_HIGHT,
			DSP_NORMAL);			//menu fill just once
	}
*/
	//LcdLoadPicture(0,LAYOUT_ETTL_X,LAY_WT_NOR,LAY_HT,ettl_EBitmap,DSP_NORMAL);
	//Draw_Circle(50,50,30);
	//LayModeEttl();
	//memset((u8*)&mainPara,0x00,sizeof(MainPara));
	//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),sizeof(mainPara)/2);
	STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)+2)/2);
	STMFLASH_Read(FLASH_SERNUM_ADDR,(u16*)(&g_snMac),16/2);
	///STMFLASH_Read(FLASH_SERNUM_ADDR,(u16*)(&g_snMac),sizeof(g_snMac)/2);			//serial number and mac address
	STMFLASH_Read(FLASH_VER_ADDR,&ver,1);
	//STMFLASH_Read(FLASH_BOOT_UP,&bootUp,1);

	pMain = (char*)&mainPara.workMode;
	//for(i=0;i<(sizeof(mainPara)-1);i++)
#ifdef UART1_TRACE_DEBUG
	printf("Saved parameter\r\n");
#endif
	for(i=0;i<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode));i++)
	{
#ifdef UART1_TRACE_DEBUG
		printf("%02x ",pMain[i]);
#endif
		sum += pMain[i];
	}

#ifdef UART1_TRACE_DEBUG
	printf("\r\nSum: %02x\r\n",sum);
	printf("SumVer: %02x\r\n",mainPara.sumVeri);
#endif

	if(mainPara.sumVeri!=sum)
	{
		//memset((u8*)&mainPara,0,sizeof(mainPara));	
#ifdef UART1_TRACE_DEBUG
		printf("PARAMETER INVALID\r\n");
#endif
		memset((u8*)&mainPara,0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
		mainPara.snrMenuSet[SNRMENU_STBY] = 1;
	}

	g_ver[0] = (ver>>12)+'0';
	g_ver[1] = '.';
	g_ver[2] = ((ver>>8)&0x0f)+'0';
	//g_ver[3] = ',';
	//g_ver[4] = ((ver>>4)&0x0f)+'0';
	//g_ver[5] = '.';
	//g_ver[6] = (ver&0x0f)+'0';	
	
	g_cmdMode = &mainPara.cmdMode;
	wlSet.m_arrPara = mainPara.modGr;
	mstPara = &mainPara.modGr[0];
	wlSet.m_chan = &mainPara.m_chan;
	//wlSet.m_on = &mainPara.m_mstOn;
	wlSet.is_masterOn = &mainPara.m_isMstFla;
	wlSlave.s_array = &mainPara.s_array;
	wlSlave.s_chan = &mainPara.s_chan;
	motoSet.mode = &mainPara.motoLoca.mode;
	motoSet.dest_location = &mainPara.motoLoca.moto_location;
	wlSet.arr_mode = &mainPara.arr_mode;
	multiSet.eTimes = &mainPara.multi_times;
	multiSet.frequence = &mainPara.multi_Frq;
	multiSet.expose = &mainPara.modGr[MASTER].multi_exp;
	wlSet.abRatio = &wlSet.m_arrPara[WL_ARRARY_B].cmp1;
	pSnrSet = mainPara.snrMenuSet;
	wlSet.cCmp = compensation_table[(wlSet.m_arrPara[WL_ARRARY_C].cmp1+6)*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2];
	abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
	abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
	//radioSet->rfChan = 1;
	//SetStByDsp();
	//radioSet->extAddr = g_snMac.mac;

	//uartSendData("usart\r\n",sizeof("usart\r\n")-1);
	//RfPreFlashInfo();
	//while(uart_tran_flag);
	//RfPreFlashInfo();
	//*g_cmdMode = CMD_MODE_LIGH;
#ifdef _RF_ONLY_
	if((*g_cmdMode!=CMD_MODE_RF_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_SLA))
	{
		*g_cmdMode=CMD_MODE_RF_MST;
	}
#endif
/*
#ifdef _ST_EX_
	if((*g_cmdMode!=CMD_MODE_LIGH_MST)&&(*g_cmdMode!=CMD_MODE_RF_MST)) *g_cmdMode = CMD_MODE_RF_MST;
#else
	if((*g_cmdMode<CMD_MODE_OFF)||(*g_cmdMode>CMD_MODE_RF_MST)) *g_cmdMode = CMD_MODE_OFF;
#endif
	
	if((mainPara.workMode<WORKINGMODE_TTL)||(mainPara.workMode>WORKINGMODE_F2)) mainPara.workMode = WORKINGMODE_TTL;
	if((90<*multiSet.eTimes)||(*multiSet.eTimes<0)) *multiSet.eTimes = 0;
	if((*multiSet.frequence>500)||(*multiSet.frequence<1)) *multiSet.frequence = 1;
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);

	menuPara.mode = ((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07);

	for(i=0;i<3;i++)
	{
		menuPara.otpCod[i] = output_table[(7-wlSet.m_arrPara[i].output1)*3-wlSet.m_arrPara[i].output2];
		menuPara.WlmMul[i] = output_table[(7-(wlSet.m_arrPara[i].multi_exp>>4))*3];
		//menuPara.WlmMul[WL_ARRARY_B] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4))*3];
	}

	for(i=0;i<5;i++)
	{
		menuPara.rfGrOtp[i] = output_table[(7-radioSet->rfGr[i].m_exp1)*3-radioSet->rfGr[i].m_exp2];
		menuPara.rfGrTmp[i] = compensation_table[(radioSet->rfGr[i].ttl_cmp1+6)*3+radioSet->rfGr[i].ttl_cmp2];
	}

	//RfParaSend();
*/
	ParaFormat();

	if((CMD_MODE_RF_MST==*g_cmdMode)||(CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)
		||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		ENABLE_RF_MODULE();
	}

	g_bkPwmDuty = (BK_PWM_ARR/5)*pSnrSet[SNRMENU_BKLITE];	  

	//LCD_Clear(BLACK);

#if 0

	g_UnknowData[0] = 0xABCD;
	g_UnknowData[1] = 0x650E;
	g_UnknowData[2] = 0xFE65;
	g_UnknowData[3] = 0x7788;
	g_UnknowData[4] = 0x138D;

	g_UkdCnt = 5;

	for(i=0;i<g_UkdCnt;i++)
	{
		DspUnknownData(i, g_UnknowData[i]);
	}

	BkLiteOn();

	delay_ms(3000);

#endif

#if 0
#ifdef _TUHAO_START_
	SamheroLogoDsp(40+20,30,160,26,tuhao_1,WHITE);
	LcdTuhaoLogo(40-35+20,30+103,47,46,tuhao_2,ORANGE);
	SamheroLogoDsp(80+20,109,100,16,tuhao_3,WHITE);
	BkLiteOn();
	delay_ms(1000);
	//while(1);
#else
#ifdef _SN_DEBUG_
	LcdSnDspLogo(30,50,120,72,Sn_logo);  
	LcdSnDspLogoWz(104,30,160,24,Sn_logo_wz);
	BkLiteOn();
	delay_ms(1000);
	//while(1);
#else
	SamheroLogoDsp(40,55,100,41,samhero_logo,GOLDEN);
	//SamheroLogoDsp(40,80,100,41,samhero_logo,GOLDEN);
	SamheroLogoDsp(86,30,160,40,samhero_char,GOLDEN);
	BkLiteOn();
	delay_ms(1000);
	//while(1);
#endif
#endif
#endif

#ifdef BOOT_UPDATE
	if(mainPara.bootUp!=1111)
	{
		DspLogo();
		LCD_ShowString(140,100,"First start...",FONT_TYPE_MAIN,0,DSP_NORMAL);
		BkLiteOn();
		STMFLASH_Write(STM32_BOOT_BASE+8*1024,(u16*)(boot+8*1024),2*1024/2);
		//STMFLASH_Write(STM32_BOOT_BASE,(u16*)boot,16);
		delay_ms(1000);
		mainPara.bootUp = 1111;
		mainPara.sumVeri=CalcSum((u8*)&mainPara.workMode,((char*)&mainPara.sumVeri-(char*)&mainPara.workMode));
		//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),sizeof(mainPara)/2);
		STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara.workMode,((char*)&mainPara.rsv-(char*)&mainPara.workMode+2)/2);
	}else
	{
		DspLogo();
		BkLiteOn();
		delay_ms(1000);
	}
#else
#ifdef _LOGO_DSP_
	DspLogo();
	BkLiteOn();
	delay_ms(1000);
#else
	delay_ms(5);
#endif
#endif

	LCD_Clear(bkSetArr[pSnrSet[SNRMENU_BKCLR]]);

	//Mode(mainPara.workMode);
	SetupBatCheck();
	//CmdModDisplay();
	g_menuId = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

	//FuncDsp();

	//RfParaSend();
#ifdef UART1_TRACE_DEBUG
	printf("RF para\r\n");
#endif

	//OSTmrCreate(200, 400,  OS_TMR_OPT_PERIODIC,  CanTmrHandle,  NULL, "CanTmr ", tmrErr);

	CR_READY = 1;
	//POW_ON = 0;
	//BK_LIGHT = 1;
	//BkLiteOn();
	
}

//开始任务
void TaskStart(void * pdata)
{
	INT8U tmrErr;
	pdata = pdata; 
	OS_ENTER_CRITICAL();   
	//OSTaskCreate(TaskCanCom, (void * )0, (OS_STK *)&TASK_CANCOM_STK[CANCOM_STK_SIZE-1], CANCOM_TASK_Prio);
	//OSTaskCreate(TaskCanCom, (void * )0, (OS_STK *)&TASK_CANCOM_STK[CANCOM_STK_SIZE-1], CANCOM_TASK_Prio);
	OSTaskCreate(TaskCmdSend,(void *)0, (OS_STK *)&TASK_CMDSEND_STK[CMDSEND_STK_SIZE-1],CMDSEND_TASK_Prio);
	OSTaskCreate(TaskKeyScan, (void * )0, (OS_STK *)&TASK_KEYPROC_STK[KEYPROC_STK_SIZE-1], KEYPROC_TASK_Prio);
	OSTaskCreate(TaskDspUpdate, (void *) 0,( OS_STK *) &TASK_REFRESH_STK[REFRESH_STK_SIZE-1],REFRESH_TASK_Prio);
	OSTaskCreate(TaskLed, (void * )0, (OS_STK *)&TASK_LED_STK[LED_STK_SIZE-1], LED_TASK_Prio);
	//OSTaskCreate(TaskTimerTickCom,(void * )0, (OS_STK *)&TASK_TICK_STK[TICK_STK_SIZE-1],TICK_TASK_Prio);
#if OS_TMR_EN > 0
	//g_osTmr = OSTmrCreate(200, 400,  OS_TMR_OPT_PERIODIC,  BkLiteAutoOff,  NULL, "CanTmr ", &tmrErr);
	g_bkTmr = OSTmrCreate(15000, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)BkLiteAutoOff,  NULL, "bkTmr ", &tmrErr);
	g_bzTmr = OSTmrCreate(200, 200,  OS_TMR_OPT_PERIODIC,  (OS_TMR_CALLBACK)BzOff,  NULL, "bzTmr ", &tmrErr);
	g_1ShotTmr = OSTmrCreate(3000, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)FlaOkAutoOff,  NULL, "1ShotTmr ", &tmrErr);
	//g_CcRstDownTmr = OSTmrCreate(300, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)CC2530_RstDown,  NULL, "RstDownTmr ", &tmrErr);
	//g_CcWakeTmr = OSTmrCreate(200, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)CC2530_WakeUp,  NULL, "CcWakeupTmr ", &tmrErr);
	g_shutterPullTmr = OSTmrCreate(300,0,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)CmrShutterPull,NULL,"shutterPullTmr",&tmrErr);
	//g_sysStartTmr = OSTmrCreate(1000,1000,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)SysStartTmrProc,NULL,"sysStartTmr",&tmrErr);
	//OSTmrStart(g_sysStartTmr, &tmrErr);
	//BkLiteOn();
	//OSTmrStart(g_osTmr, &tmrErr) ;
	SET_UPDATE_EVT(DSP_UPDATE_PWON);
#endif

#ifdef _UART_INT_INFO_
	uart2_init(36,115200);		   
#else
	uart2_init(36,57600);
#endif

	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
/*if this task is waked up, go to standby mode*/
	//BkLiteOn();
	
	OS_EXIT_CRITICAL();
}


//系统时钟中断服务函数
void SysTick_Handler(void)
{
	OS_ENTER_CRITICAL(); /* Tell uC/OS-II that we are starting an ISR  */
    	OSIntNesting++;
    	OS_EXIT_CRITICAL();

   	OSTimeTick();        /* Call uC/OS-II's OSTimeTick()               */
	OSTmrSignal();		/*by cbyi tmr signal*/
	
    	OSIntExit();         /* Tell uC/OS-II that we are leaving the ISR  */

		
}


//系统时钟配置，设计1ms产生一次中断
void SysTick_Configuration(void)
{
 	SysTick->CTRL&=~(1<<2);//SYSTICK使用外部时钟源
	SysTick->CTRL|=1<<1;   //开启SYSTICK中断
	SysTick->LOAD=9000;    //产生1ms中断
	//bit2清空,选择外部时钟  HCLK/8
	//MY_NVIC_Init(3,3,SystemHandler_SysTick,2);//组2，最低优先级 
	MY_NVIC_Init(3,3,SysTick_IRQn,2);//组2，最低优先级 
	SysTick->CTRL|=1<<0;   //开启SYSTICK
}

//#define _DEBUG_INDEX_
//#define _NO_SW_TEST_
void TaskLed(void *pdata)
{
#ifdef _DEBUG_INDEX_
	int tick=0;
#endif
#ifdef UART1_TRACE_DEBUG
	int i=0;
#endif
	int ccWakeTick=0;
//	int setupBat=0;
//	char batCkFlag=0;

#ifdef UART1_TRACE_DEBUG
	printf("Led  task\r\n");
#endif
	//u8 index;
	//char *pMain;
	while(1)
	{
/*
		if(g_cc2530Start)
		{
#ifdef UART1_TRACE_DEBUG
			printf("cc2530Start\r\n");
#endif
			RfParaSend();
			g_cc2530Start = 0;
		//OSTmrStop(g_sysStartTmr,OS_TMR_OPT_NONE,NULL,&err);
		}
*/

		if(ccWakeTick++>10)
		{
			if(g_cc2530Wake)
			{
				DISABLE_RF_MODULE();
				OSTimeDlyHMSM(0,0,0,10);
				ENABLE_RF_MODULE();
				//g_cc2530Wake = 0;
			}
			ccWakeTick = 0;
		}
#if 0
		if(!batCkFlag)
		{
#ifdef UART1_TRACE_DEBUG
			printf("Bat\r\n");
#endif
			if(setupBat++>100)
			{
				
				SetupBatCheck();
				batCkFlag=1;
			}
		}
#endif
		//AF_CTL_OUT=!AF_CTL_OUT;

#ifndef _NO_SW_TEST_
		if(pSnrSet[SNRMENU_STBY]>0)
		{
			if(COM_STATUE_IDLE==g_comStatue)
			{
				gWSleepCounter++;
			}
			//if(gWSleepCounter++>pSnrSet[SNRMENU_STBY]*60*20)
			if(gWSleepCounter>pSnrSet[SNRMENU_STBY]*60*20*10)
			{
				//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));
				gWSleepCounter=0;
				UartSctSend(UART_ENCODE_EXIT, 1, 0);
				OSTimeDlyHMSM(0,0,0,50);
				LowPow_Init();
			}
		}

		if(KEY_LOCK_EX&&KEY_POW_ON)			//power off
		{
			OSTimeDlyHMSM(0,0,0,10);
			if(KEY_LOCK_EX&&KEY_POW_ON)
			{
				//mainPara.sumVeri = 0;
				//pMain = (char *)&mainPara;
				//for(index=0;index<sizeof(mainPara)-1;index++)
				//for(index=0;index<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode));index++)
				//{
				//	mainPara.sumVeri += pMain[index];
				//}
				UartSctSend(UART_ENCODE_EXIT, 1, 0);
				if(!pSnrSet[SNRMENU_ATSAVE])
				{
					mainPara.sumVeri=CalcSum((u8*)&mainPara.workMode,((char*)&mainPara.sumVeri-(char*)&mainPara.workMode));
					//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),sizeof(mainPara)/2);
					STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara.workMode,((char*)&mainPara.rsv-(char*)&mainPara.workMode+2)/2);
#ifdef UART1_TRACE_DEBUG
					printf("Saved parameter: ");
					for(i=0;i<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode));i++)
					{
						printf("%02x ",((char*)&mainPara)[i]);
					}
					printf("\r\nsumveri: %02x\r\n",mainPara.sumVeri);
#endif
					//UartSctSend(UART_ENCODE_EXIT, 1, 0);
					//OSTimeDlyHMSM(0,0,0,30);
				}
				OSTimeDlyHMSM(0,0,0,30);
				//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));	
				POW_ON = 0;						//here shut down the POWER
#ifdef UART1_TRACE_DEBUG
				printf("Power down\r\n");
				OSTimeDlyHMSM(1,0,0,0);
#endif
				OSTimeDlyHMSM(1,0,0,0);
			}
		}
#endif		
		OSTimeDlyHMSM(0,0,0,50);	
	}
}

#ifndef _RF_ONLY_
unsigned char IIcAck()
{
	unsigned char ack;
	IICSDA_IN;
	HP_I2C_SCK=1;
	//ack = HP_I2C_SDA;
	ack = HP_I2C_SIN;
	HP_I2C_SCK=0;
	return ack;
}

unsigned char IIcStart()
{
	//P0DIR |= 0x18;		//p03,p04 set as I2C port
	HP_I2C_SDA=1;
	HP_I2C_SCK=1;
	I2C_DELAY;
	HP_I2C_SDA=0;
	HP_I2C_SCK=0;
	I2C_DELAY;

	return IIcAck();
	//ack	
}

u8 HpIicSendByte(u8 dat)
{
	unsigned char i;
	unsigned char ack;
	
	IICSDA_OUT;

	for(i=0;i<8;i++)
	{
		HP_I2C_SCK=0;
		I2C_DELAY;
		if(dat&0x80)
		{
			HP_I2C_SDA=1;
		}else
		{
			HP_I2C_SDA=0;
		}
		I2C_DELAY;
		HP_I2C_SCK=1;
		I2C_DELAY;
		dat <<=1;
		I2C_DELAY;		
	}
	
	HP_I2C_SCK=0;
	HP_I2C_SDA=0;
	//IICSDA_IN;
	//I2C_DELAY;
	//HP_I2C_SCK=1;
	//ack=HP_I2C_SDA;
	//HP_I2C_SCK=0;
	ack = IIcAck();

	return ack;
}


void IIcStop()
{
	IICSDA_OUT;
	HP_I2C_SCK=0;
	I2C_DELAY;
	HP_I2C_SDA=0;
	I2C_DELAY;
	HP_I2C_SCK=1;
	I2C_DELAY;
	HP_I2C_SDA=1;	
}


void HpExpCodeSend(unsigned int hpv)
{
	IIcStart();
	HpIicSendByte(0xc0);
	HpIicSendByte(0x60);
	HpIicSendByte((hpv>>8)&0xff);
	HpIicSendByte(hpv&0xff);
	IIcStop();
}

void WlmCmdSet()
{
	HpExpCodeSend(CMD_THRESHOLD_VAL);
}

void WLM_SendChanhead(char chan)
{

	WlmCmdSet();

	switch(chan)
	{
		case (WL_CHANNEL_ONE):
			//Delay_10us(5);
			//Delay_10us(WLM_CHAN1_WIDTH);
			//WLM_HoldOn(WLM_CHAN1_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN1_WIDTH);
		break;

		case (WL_CHANNEL_TWO):
			//Delay_10us(9);
			//Delay_10us(WLM_CHAN2_WIDTH);
			//WLM_HoldOn(WLM_CHAN2_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN2_WIDTH);
		break;

		case (WL_CHANNEL_THREE):
			//Delay_10us(16);
			//Delay_10us(WLM_CHAN3_WIDTH);
			//WLM_HoldOn(WLM_CHAN3_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN3_WIDTH);
		break;

		case (WL_CHANNEL_FOUR):
			//Delay_10us(22);
			//Delay_10us(WLM_CHAN4_WIDTH);
			//WLM_HoldOn(WLM_CHAN4_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN4_WIDTH);
		break;
	}
	//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
	WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
}

void WLM_SendData(char *Arr, char sendSize)
{
	char dataIndex=0;
	char bitIndex=0;

/*	if(wlSet.m_sendCmdEn!=1)
	{
		return;
	}
*/

	WLM_SendChanhead(*wlSet.m_chan);

	while(dataIndex<sendSize)
	{
		while(bitIndex<8)
		{
			//if((Arr[dataIndex]>>(7-bitIndex))&0x01)
			if((Arr[dataIndex]>>bitIndex)&0x01)			//Little endian
			{
				//WLM_TIMER0_DELAY(WLM_BIT1_INTERVAL);
				//Delay_10us(WLM_BIT1_INTERVAL);
				//WLM_HoldOn(WLM_BIT1_INTERVAL);
				//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
				WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			}else
			{
				//WLM_TIMER0_DELAY(WLM_BIT0_INTERVAL);
				//Delay_10us(WLM_BIT0_INTERVAL);
				//WLM_HoldOn(WLM_BIT0_INTERVAL);
				delay_us(WLM_BIT0_INTERVAL);
			}
			bitIndex++;
		}		
		bitIndex = 0;
		dataIndex++;
		if(dataIndex<sendSize)
		{
			delay_us(WLM_BYTE_DELAY);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			//WLM_SHOT_ONCE(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
		}
	}
 
}
#endif

void PreFlaPullUp()
{
	SPI_MISO_AUX = 0;
	//delay_us(990);
	//delay_us(900);
	delay_us(890);
	//delay_us(880);
	OS_EXIT_CRITICAL();
	delay_us(90);
	SPI_MISO_AUX = 1;
}

void RfMstPreflaProc(void)
{
	//FLAOK_TOGGLE();
	//delay_us(100);
	//SPIx_Init();
	//SPI1->DR = 0x80;
	//SPI1->DR = 0xFF;
	//g_comStatue = COM_STATUE_START;

	SET_MISO_FREE();
	
	if(*wlSet.is_masterOn>0)
	{
		SPI_MISO_AUX = 0;
		
		if((0==flaPara.preflashIndex)||(1==flaPara.preflashIndex))
		{
			HpPreflash(HP_PREFLASH_INTERVAL);
			//DISABLE_D30EN();
		}else
		{
			OS_ENTER_CRITICAL();
			delay_us(890);
			OS_EXIT_CRITICAL();
			delay_us(90);
		}
		SPI_MISO_AUX = 1;
	}else
	{
		OS_ENTER_CRITICAL();
		SPI_MISO_AUX = 0;
		delay_us(890);
		OS_EXIT_CRITICAL();
		delay_us(90);
		SPI_MISO_AUX = 1;
	}

	SET_MISO_ONRUN();

#ifdef EOS_750D
	if(CLK_IN>0)
	{
		if(COM_STATUE_IDLE==g_comStatue)
		{
			//EXTIx_Init(GPIO_A,FTIR);
			EXTIx_Init(FTIR);
			SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
			g_cmrLinked = 1;
			g_comStatue = COM_STATUE_START;	
		}
	}else
	{
		//EXTIx_Init(GPIO_A,RTIR);
		EXTIx_Init(RTIR);
		g_comStatue = COM_STATUE_IDLE;
	}
#else	
	//delay_us(100);
	SPIx_Init();
	//SPI1->DR = 0x80;
	SPI1->DR = 0xFF;
	g_comStatue = COM_STATUE_START;
	
#endif

}

void FlashFinishProc(void)
{
	//DISABLE_D30EN();
	SPIx_Disable();
	Ex_NVIC_Config(GPIO_A,5,RTIR);
	Ex_NVIC_Enable(5);
	g_comStatue = COM_STATUE_IDLE;
	g_dspUpdateFlag |= DSP_UPDATE_SHOT;
	SPI_MISO_AUX = 1;
}

void WLMCmdSend(u8 mod)
{
	switch(mod)
	{
#ifndef _RF_ONLY_
		case CMD_MODE_LIGH_MST:
		switch(wlSet.m_cmdProc)
		{
			case WLM_CMDPROC_DAT:
				//Delayus(15);
				SPI_MISO_AUX = 0;
				delay_us(150);		
				OS_ENTER_CRITICAL();
				WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
				delay_us(100);		
				OS_EXIT_CRITICAL();
				delay_us(400);
				//disable_irq();	
				//MEMCTR   |=  0x04;
				//IRQ0ICR &= 0XFE;
				//IRQ1ICR &= 0XFE;
				//MEMCTR   &= ~0x04;
				//enable_irq();
				wlSet.m_cmdProc = WLM_CMDPROC_OFF;	
				SPI_MISO_AUX = 1;
				//FLASH_OUT = 1;
			break;

			case WLM_CMDPROC_FLA:
				SPI_MISO_AUX = 0;
				WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				SPI_MISO_AUX = 1;
				SPIx_Disable();
				Ex_NVIC_Config(GPIO_A,5,RTIR);
				Ex_NVIC_Enable(5);
				g_comStatue = COM_STATUE_IDLE;
				g_dspUpdateFlag |= DSP_UPDATE_SHOT;
				wlSet.m_cmdProc = WLM_CMDPROC_OFF;	
			break;

			case WLM_CMDPROC_PRF:
			if(WLM_MASTER_ON==*wlSet.is_masterOn)
			{
				//HpExpCalc(4,0,0,0,0,&hpSetting,0);
				//HighSpeedTriger(&hpSetting,HP_PREFLASH_INTERVAL);			//preflash
				//TrigerMasterOn(0);
				SPI_MISO_AUX  = 0;
				HpPreflash(HP_PREFLASH_INTERVAL);
				SPI_MISO_AUX = 1;
				//SC4MD1 |= 0X30;	
			}else
			{
				//FLASH_OUT = 0;
				WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				//FLASH_OUT = 0;
				SPI_MISO_AUX  = 0;
				delay_us(40);
				SPI_MISO_AUX  = 1;
				//FLASH_OUT = 1;

			}
#ifdef EOS_750D
#else
			SPIx_Init();
			SPI1->DR = 0x80;
			g_comStatue = COM_STATUE_START;
#endif
			break;

			case WLM_CMDPROC_OFF:
				//Delay_10us(50);
				//FLASH_OUT = 1;
				SPI_MISO_AUX = 1;
			break;
			
		}
		/*
		if(WLM_CMDPROC_DAT==wlSet.m_cmdProc)
		{
			//Delayus(15);
			SPI_MISO_AUX = 0;
			delay_us(150);		
			OS_ENTER_CRITICAL();
			WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
			delay_us(100);		
			OS_EXIT_CRITICAL();
			delay_us(400);
			//disable_irq();	
			//MEMCTR   |=  0x04;
			//IRQ0ICR &= 0XFE;
			//IRQ1ICR &= 0XFE;
			//MEMCTR   &= ~0x04;
			//enable_irq();
			wlSet.m_cmdProc = WLM_CMDPROC_OFF;	
			SPI_MISO_AUX = 1;
			//FLASH_OUT = 1;
		}else if(WLM_CMDPROC_FLA==wlSet.m_cmdProc)
		{
			SPI_MISO_AUX = 0;
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			SPI_MISO_AUX = 1;
			g_dspUpdateFlag |= DSP_UPDATE_SHOT;
			wlSet.m_cmdProc = WLM_CMDPROC_OFF;	
		}else
		{
			//Delay_10us(50);
			//FLASH_OUT = 1;
			SPI_MISO_AUX = 1;
		}*/
		break;
#endif

		case CMD_MODE_RF_MST:
			if(1==radioStatue.cmdReady)
			{	
				switch(radioStatue.cmdSta)
				{
					case RADIO_STATUE_PRE://where should be take especiall notice
					if(CMR_ADEPT_NEW==radioStatue.adept)
					{
						SPI_MISO_AUX = 0;
						delay_us(600);
						RfPreFlashInfo(8);
						//delay_ms(20);		//cbyi debug 2015/05/16 NBK
						//delay_ms(17);
						delay_ms(20);
						SPI_MISO_AUX = 1;
						//FLASH_CONTROL = !FLASH_CONTROL;
						//SPI_MISO_AUX = 1;
						//FLASH_CONTROL = !FLASH_CONTROL;
						//delay_us(900);
						//delay_us(330);
						//RfCountDownInfo(7000);
						//delay_us(550);
						/*
						OS_ENTER_CRITICAL();
						if(0==radioStatue.delay_mod)
						{
							//delay_us(330);
							delay_us(500);
							RfCountDownInfo(7000);
							//delay_us(550);
							delay_us(380);
							//delay_ms(9);
							delay_ms(8);
							//PreFlaPullUp();
							SPI_MISO_AUX = 0;
							//delay_us(990);
							//delay_us(900);
							delay_us(890);
							//delay_us(880);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;
							
						}else if(1==radioStatue.delay_mod)
						{
							RfCountDownInfo(6000);
							//delay_us(8540-160);
							delay_us(7540-160);
							//PreFlaPullUp();
							SPI_MISO_AUX = 0;
							//delay_us(990);
							//delay_us(900);
							delay_us(890);
							//delay_us(880);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;
							
						}else if(2==radioStatue.delay_mod)
						{
							//delay_us(330);
							//delay_us(500);
							//delay_us(350);
							//delay_ms(1);
							delay_us(2000);
							//delay_us(1976);
							RfCountDownInfo(3000);
							//delay_ms(5);
							delay_us(4400);
							//PreFlaPullUp();
							SPI_MISO_AUX = 0;
							//delay_us(990);
							//delay_us(900);
							delay_us(890);
							//delay_us(880);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;
							
						}else if(3==radioStatue.delay_mod)
						{
							OS_EXIT_CRITICAL();
						}
						*/
						/*
						SPI_MISO_AUX = 0;
						//delay_us(990);
						//delay_us(900);
						delay_us(890);
						//delay_us(880);
						OS_EXIT_CRITICAL();
						delay_us(90);
						SPI_MISO_AUX = 1;
						//SPIx_Init();
						//g_comStatue = COM_STATUE_START;
						*/
					}else if(CMR_ADEPT_OLD==radioStatue.adept)
					{
						SPI_MISO_AUX = 0;
						//delay_ms(1);
						RfPreFlashInfo(2);
						//delay_ms(5);
						delay_ms(6);        // 20170901
						//delay_ms(4);
						//delay_us(200);    // 20170901
						delay_us(500);
						g_comStatue = COM_STATUE_PREFLASH;	
						//Ex_NVIC_Enable(5);
						SPI_MISO_AUX = 1;
						//g_comStatue = COM_STATUE_PREFLASH;	
						//DEBUG_PIN1 = !DEBUG_PIN1;
					}
					break;

					case RADIO_STATUE_PRE_FLA:
						switch(radioStatue.delay_mod)
						{
							case 0:
							delay_us(300+200);
							RfCountDownInfo(7000);
							delay_ms(9);
							//delay_us(820);
							//delay_us(820-300);
							delay_us(820-300-100);
							RfMstPreflaProc();
							break;

							case 1:
							//delay_us(200);		//2015/09/18
							RfCountDownInfo(6000);
							//delay_us(7540-160);
							//delay_us(7540-160-100+1000);
							delay_us(7540-160-100+1000+200);
							RfMstPreflaProc();
							break;

							case 2:
							RfCountDownInfo(3000);
							//delay_ms(5);
							//delay_us(4400);
							//delay_us(4400-100);
							delay_us(4400-100+1150);     //same as nbk600ex
							RfMstPreflaProc();
							break;

							case 3:
							//OS_ENTER_CRITICAL();
							RfCountDownInfo(4000);
							//delay_us(5500);
							//delay_us(5500-100);
							delay_us(6530);
							RfMstPreflaProc();
							break;

							default:
							//OS_ENTER_CRITICAL();
							RfCountDownInfo(4000);
							//delay_us(5500);
							//delay_us(5500-100);
							delay_us(6530);
							RfMstPreflaProc();
							break;
						}
					break;

					case RADIO_STATUE_PRE_EX:				//for 7DII
						OS_ENTER_CRITICAL();
						RfCountDownInfo(4000);
						delay_us(5500);
						SPI_MISO_AUX = 0;
						delay_us(890);
						OS_EXIT_CRITICAL();
						delay_us(90);
						SPI_MISO_AUX = 1;
					break;

					case RADIO_STATUE_PRECFM:			//this is especially for camera before 2012
						//FLASH_CONTROL = !FLASH_CONTROL;
						//delay_ms(2);
						RfCountDownInfo(1000);
						//delay_us(2100);
						//delay_us(2400);
						delay_us(3550);
						//delay_ms(3);
						//delay_us(200);
						//delay_us(400);
						RfMstPreflaProc();
						/*
						SPI_MISO_AUX = 0;
						delay_ms(1);
						SPI_MISO_AUX = 1;
						SPIx_Init();
						g_comStatue = COM_STATUE_START;	
						*/
					break;

					case RADIO_STATUE_FLACFM:
					if(CMR_ADEPT_NEW==radioStatue.adept)
					{
						delay_us(400);
						RfFlashConfirm(8,FLASH_PILOT_OFF);
						//SIGNAL = !SIGNAL;
						//delay_us(24000);
						//delay_ms(20);
#ifdef UART1_TRACE_DEBUG
						printf("flacfm");
#endif
						delay_ms(20);				//2014/0928 (1DX + 红圈)兼容
						//delay_ms(19);
						SPI_MISO_AUX = 1;
					}else if(CMR_ADEPT_OLD==radioStatue.adept)
					{
						//FLASH_CONTROL = !FLASH_CONTROL;
						delay_ms(3);
						RfFlashConfirm(2,FLASH_PILOT_OFF);
						delay_ms(3);
						SPI_MISO_AUX = 1;
					}
					break;

					//case RADIO_STATUE_FLASH:
					case RADIO_STATUE_FLAINFO:
					//SPI_MISO_AUX = 0;
					if(CMR_ADEPT_NEW==radioStatue.adept)
					{
						if(0==radioStatue.delay_mod)
						{
							RfCountDownInfo(6000);
						}else if(1==radioStatue.delay_mod)
						{
							//delay_us(500+120+300);           // 2016/08/02 5d markIII 黑边
							if (flaPara.hpFlash > 0)
							{
								delay_us(500+120);                            // 2016/08/02 5d markIII 黑边
							}else
							{
								delay_us(500+120+700); 
							}
							RfCountDownInfo(7000);
						}else
						{
							RfCountDownInfo(6000);
						}
						//RfCountDownInfo(6000);
						//SPIx_Disable();
						//FlashFinishProc();
						delay_us(10000-1070);             // 10.26 ms  2016/08/05
						//Delay_ms(1);
						SPI_MISO_AUX = 0;
						delay_us(1000);
						SPI_MISO_AUX = 1;
						FlashFinishProc();
					}else if(CMR_ADEPT_OLD==radioStatue.adept)
					{
						//delay_ms(3);
						//RfCountDownInfo(0);	
						UART_INFO = 1;
						U2SendData(0xAB);
						UART_INFO = 0;
						delay_ms(1);
						SPI_MISO_AUX = 0;
						delay_ms(1);
						SPI_MISO_AUX = 1;
						//SPI_MISO_AUX = 0;
						//delay_us(1000);
						//SPI_MISO_AUX = 1;
					}
					//SPIx_Disable();
					//Ex_NVIC_Config(GPIO_A,5,RTIR);
					//Ex_NVIC_Enable(5);
					//g_comStatue = COM_STATUE_IDLE;
					//g_dspUpdateFlag |= DSP_UPDATE_SHOT;
					break;

					case RADIO_STATUE_FLASH:
						if(CMR_ADEPT_NEW==radioStatue.adept)
						{
							FlashFinishProc();
						}else
						{
							SPI_MISO_AUX = 0;
							UART_INFO = 1;
							U2SendData(0xAB);
							UART_INFO = 0;

							FlashFinishProc();
						}
					break;

					case RADIO_STATUE_RECOVER:
						RfRecoverInfo();
					break;

					case RADIO_STATUE_MODEL:
						delay_us(10);
						g_comStatue = COM_STATUE_START;
						SPIx_Init();
						SPI1->DR = 0x8E;
					break;
				}
				radioStatue.cmdReady=0;
			}
		break;

		case CMD_MODE_RFLS_MST:
			if(1==radioStatue.cmdReady)
			{
				switch(radioStatue.cmdSta)
				{
					//case RADIO_STATUE_FLASH:
					case RADIO_STATUE_FLAINFO:
						UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
						if(HP_FLASHMODE_ON==flaPara.hpFlash)
						{
//#ifdef _UART_INT_INFO_
	//						SpiRearUp(100,TM_HANDLE_REAR);
	//#else
							SpiRearUp(100);
	//#endif
							Ex_NVIC_Config(GPIO_A,5,RTIR);
							Ex_NVIC_Enable(5);
							g_comStatue = COM_STATUE_IDLE;
						}
						//delay_ms(1);
					break;

					case RADIO_STATUE_FLASH:
						SPI_MISO_AUX = 1;
						g_comStatue = COM_STATUE_IDLE;
						 //wlSet.m_cmdProc = WLM_CMDPROC_OFF;
						 Ex_NVIC_Config(GPIO_A,5,RTIR);
						 Ex_NVIC_Enable(5);
					break;

					case RADIO_STATUE_PRE_FLA:
#ifdef EOS_750D
						g_getTtlFlashCode = 1;
#else
						SPIx_Init();
						SPI1->DR = 0x80;
						g_getTtlFlashCode = 1;
						g_comStatue = COM_STATUE_START;
#endif
					break;

					case RADIO_STATUE_PRE:
						SPI_MISO_AUX = 1;
						g_comStatue = COM_STATUE_PREFLASH;	
					break;
				}
				radioStatue.cmdReady = 0;
			}
			//SpiRearUp(100);
		break;

		case CMD_MODE_RFLS_SLA:
			if(1==radioStatue.cmdReady)
			{
				switch(radioStatue.cmdSta)
				{
					case RADIO_STATUE_FLACFM:
						EXTIx_Init(FTIR);
						Ex_NVIC_Enable(5);
						g_comStatue = COM_STATUE_REALFLASH;
//#ifdef _UART_INT_INFO_
//						SpiRearUp(100,TM_HANDLE_REAR);
//#else
						SpiRearUp(100);
//#endif
					break;

					case RADIO_STATUE_FLASH:
						SPI_MISO_AUX = 1;
						Ex_NVIC_Config(GPIO_A,5,RTIR);
						Ex_NVIC_Enable(5);
						g_comStatue = COM_STATUE_IDLE;
					break;

					case RADIO_STATUE_FLAINFO:
						UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
					break;

					case RADIO_STATUE_PRE_FLA:
#ifdef EOS_750D
						g_getTtlFlashCode = 1;
#else
						SPIx_Init();
						SPI1->DR = 0x80;
						g_getTtlFlashCode = 1;
						g_comStatue = COM_STATUE_START;
#endif
					break;

					case RADIO_STATUE_PRE:
						SPI_MISO_AUX = 1;
						g_comStatue = COM_STATUE_PREFLASH;	
					break;
				}
			}
		break;
	}
}

void TaskCmdSend(void *pdata)
{
	while(1)
	{
		WLMCmdSend(*g_cmdMode);
		OSTaskSuspend(CMDSEND_TASK_Prio);
		//OSTimeDlyHMSM(0,0,0,50);
	}
}

void EXTI4_IRQHandler(void)		//big point interrupt
{
	//printf("ext");
	//CHARGE_LED = 1;
	EXTI->PR=1<<4;	

	switch(g_comStatue)
	{
		case COM_STATUE_REALFLASH:
			//if(!CLK_IN)
			{
				switch(*g_cmdMode)
				{
					case CMD_MODE_LIGH_MST:
						//SPI_MISO_AUX = 0;
						//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
						//SPI_MISO_AUX = 1;
						//g_dspUpdateFlag |= DSP_UPDATE_SHOT;
						if(!CLK_IN)
						{
							wlSet.m_cmdProc = WLM_CMDPROC_FLA;
							OSTaskResume(CMDSEND_TASK_Prio);
						}
					break;

					case CMD_MODE_RF_MST:
					//case CMD_MODE_RFLS_MST:
						SPI_MISO_AUX = 0;
						radioStatue.cmdReady=1;
						radioStatue.cmdSta = RADIO_STATUE_FLASH;
						OSTaskResume(CMDSEND_TASK_Prio);
						//SPI_MISO_AUX = 0;
						//delay_ms(1);
						//SPI_MISO_AUX = 1;
					break;

					case CMD_MODE_RFLS_MST:
					case CMD_MODE_RFLS_SLA:
						radioStatue.cmdReady=1;
						radioStatue.cmdSta = RADIO_STATUE_FLASH;
						OSTaskResume(CMDSEND_TASK_Prio);
					break;

					case CMD_MODE_OFF:
						if(!CLK_IN)
						{
							SPI_MISO_AUX = 0;
							wlSet.m_cmdProc = WLM_CMDPROC_FLA;
							OSTaskResume(CMDSEND_TASK_Prio);
						}
					break;
				}				
				//SPIx_Disable();
				//Ex_NVIC_Config(GPIO_A,5,RTIR);
				//Ex_NVIC_Enable(5);
				//g_comStatue = COM_STATUE_IDLE;
			}
		break;

		case COM_STATUE_IDLE:
		break;

		case COM_STATUE_MULTIFLASH:
		break;

		case COM_STATUE_SLEEP:
		break;

		
	}
}


void TURN_ON_TRIGER(unsigned int time)
{ 
}









