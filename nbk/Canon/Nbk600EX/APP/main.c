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
#include "../HARDWARE/DMA/DMA.h"
#include "FlashPara.h"
#include "../Enc_Dec/encrypto.h"
//HX32F100V01
//HX08F32V01

//Options for Target-->C/C++/Language /Code Generation/Optimization (Level 3(-O3))
#define LOW_BAT_LEVEL	72//(1009>>4)

//任务堆栈
//OS_STK  TASK_CANCOM_STK[CANCOM_STK_SIZE];
OS_STK  TASK_KEYPROC_STK[KEYPROC_STK_SIZE];
OS_STK  TASK_START_STK[START_STK_SIZE];
OS_STK  TASK_LED_STK[LED_STK_SIZE];
OS_STK  TASK_REFRESH_STK[REFRESH_STK_SIZE];
OS_STK	TASK_CMDSEND_STK[CMDSEND_STK_SIZE];
OS_STK	TASK_TICK_STK[TICK_STK_SIZE];
OS_STK	TASK_VOLTAGE_STK[VOL_CK_SIZE];
OS_STK  TASK_MOTOTICK_STK[MOTO_TICK_SIZE];

//任务申明
void TaskStart(void *pdata);
void TaskCanCom(void *pdata);
void TaskKeyScan(void *pdata);
//void TaskLed1(void *pdata);
void TaskMonitor(void *pdata);
void TaskCmdSend(void *pdata);
void TaskVoltageCheck(void *pdata);
extern void TaskDspUpdate(void *pdata);
extern void U2SendData(u8 dat);
//void UserInit(void);
extern void DspTest(void);
void WlmCmdSet(void);
void SetMultiFlashTimer(void);
extern unsigned char RfMcodeCovert(unsigned char src, u8 hpMod);
void SlaveMultiFlashPrepear(void);
extern void ChkVoltage(void);
extern void LightSlaveTmrProc(void);
extern void ChanSetConfirm(void);
extern void ChargeBuzzer(void);
extern void TaskMotoProc(void *pdata);
extern void MotoTickDriver(void);
extern unsigned char RfMulTimeCalc(int cov);
void PWR_EnterSLEEPMode(u32 SysCtrl_Set, u8 PWR_SLEEPEntry);
void FlashOkLedTwinkle(void);
void UserScreenOn(void);

MainPara mainPara;
//unsigned char g_dspStyle=DSP_STYLE_CANON;
//RadioSet radioSet={0};
RadioSet* radioSet = &mainPara.rfSet;
unsigned char g_grDspStart=0;
signed char g_grSel=0;
WL_ArrPara *mstPara;// = &(mainPara.modGr[0]);
RadioStatue radioStatue;
Wl_Slave wlSlave;
extern signed char *g_cmdMode;
extern MultiSet multiSet;
extern unsigned char uart_tran_flag;
//SnMac g_snMac;
u16 g_tmEvent;
//char g_ver[10]={0};
//extern Word gWSleepCounter;
extern u32 gWSleepCounter;
//extern char g_stByTime;
extern signed char *pSnrSet;
extern char g_SettingIndex;
extern u16 g_dspUpdateFlag;
signed char g_expTTLPara1=0;
signed char g_expTTLPara2=0;
char g_cmrLinked=0;
char g_cmrPressed=0;
char g_keyAdcEnable=0;
extern char g_cc2530Start;
extern u8 g_expTTLPara3;
extern unsigned char g_getTtlFlashCode;
extern unsigned char g_tmHandleFlag;
extern unsigned char ttl_expCodeRsv;
extern CanWireless canWireless;
extern unsigned char g_shutterFlag;
extern char g_RgbCheckTurn;
extern unsigned int g_rfIdSet;
extern u8 g_chargeflag;
extern unsigned char g_mstPrefla;
extern unsigned char g_menuId;
extern signed char g_rfChanSet;
extern unsigned char g_keyDir;
extern u8 g_shadowKey;
extern u8 g_isIndividualSlave;
extern u8 g_isRfSlaveLinked;
extern char g_slaveReady;
extern unsigned char g_vCmr;
char g_motoInvalidCounter=0;
u8 g_keyVeriBuf[16];
u8 g_keyCpuId[16];

extern WLM_abRation_fillwords const wlm_abRation_fillwords[13];
extern unsigned char abRation_fill1,abRation_fill2;
//extern const timeTable shutterTable[LENOFTIMETABLE];
extern const u16 g_flaExpCode[];

char g_cupIdKey[16] = {0x24,0x98,0xb7,0xA0,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};
//char g_cupIdRst[16] = {0xE8,0xF7,0xff,0x1f,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};
char g_cupIdRst[16] = {0x08,0x3f,0x37,0x9f,0xb6,0xe6,0x5d,0x1f,0x3d,0xb9,0xdb,0x9c,0x4f,0xf3,0x6f,0xf0};

char *g_decKey = "MakeTomorrowBest";			//NBK闪光灯

u8 g_isRfModuleLinked = 0;
u8 g_paraUpdateFlag=0;
u8 g_stepLength=0;
u8 g_isHotProtect=0;
//u8 g_isHotProtect = 1;
//u8 g_stepLength=1;
//u8 g_flaEnalbe=0;
//u8 g_sysStart=0;
u8 g_isRgbFilter=1;
u8 g_upDnSensor=0;
u8 g_isReflectPulled=0;
u8 g_rgbStyle = 0;
u8 g_isFullFrame=0;
//u32 g_expAccHotWarnValue=0;
s32 g_expAccHotWarnValue=0;
//int g_mulCnt=0;
//extern HpSetting hpSetting;

//char g_paraSetBuf[1024];
//u8 g_bkState=0;
//u8 g_bkFlag=0;
#if OS_TMR_EN>0
OS_TMR  *g_bkTmr;
OS_TMR	*g_bzTmr;
OS_TMR	*g_flashTmr;
OS_TMR  *g_paraUpdateTmr;
OS_TMR  *g_shutterPullTmr;
OS_TMR  *g_1ShotTmr;
OS_TMR  *g_slaChanDivTmr;
#endif

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

void CheckBattery(void)
{
	if(GetBatteryAdc()<LOW_BAT_LEVEL)
	{
		delay_ms(500);
		if(GetBatteryAdc()<LOW_BAT_LEVEL)
		{
			LoadLowBatIcon();
			delay_seconds(3);
			//PortUninit();
			BK_TURNOFF(0);
			//LowPow_Init(0);
			LCD_Clear(0);
			PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
			delay_seconds(3);
		}
	}
}

void GolbalParaInit(void)
{
	g_isRgbFilter = 1;
	uart_tran_flag = 0;
	g_isFullFrame = 0;
	if(g_menuLock>0)
	{
		mainPara.workMode = g_menuModRsv;
		*motoSet.mode = g_motoModRsv;
		*motoSet.dest_location = g_motoDestRsv;
		g_menuLock = 0;
	}
	//g_menuLock =0;
	g_isFullFrame = 0;
	g_stepLength = 0;
	g_isRfModuleLinked = 0;
	//g_expAccHotWarnValue = 0;
	g_isHotProtect = 0;
	g_chargeflag = 0;
	g_chargeOk = 0;
	g_menuId = 0;
	g_SettingIndex = 0;
	g_cmrLinked = 0;
	g_cmrPressed = 0;
	gWSleepCounter = 0;
	radioSet->rfChan = g_rfChanSet;
	memset(radioStatue.devCnt,0x00,5);
	memset(radioStatue.devStatue,0,5);
	g_slaveReady = 0;
	g_keyAdcEnable = 0;
	g_isRfSlaveLinked = 0;
	g_spiRcvTotal = 0;
}

void KeyDecrypto(void)
{
	u32 *cpuAddr;
#ifdef BOOT_TEST
	u8 i = 0;
#endif
	Boot_Decrypt(g_cupIdRst,g_cupIdKey,g_cupIdRst,16);
	cpuAddr = (u32*)g_cupIdRst;
	STMFLASH_Read(*cpuAddr,(u16*)(g_keyCpuId+4),6);
	STMFLASH_Read(FLASH_VERI_FLAG_ADDR,(u16*)g_keyVeriBuf,8);
	Boot_Decrypt((char*)g_keyVeriBuf,g_decKey,(char*)g_keyVeriBuf,16);
	
#ifdef BOOT_TEST
	printf("g_keyCpuId: \r\n");
	for(i=0;i<16;i++)
	{
		printf("%x ",g_keyCpuId[i]);
	}
	printf("\r\ng_keyCpuId: \r\n");
	for(i=0;i<16;i++)
	{
		printf("%x ",g_keyVeriBuf[i]);
	}
#endif
}

BOOLEAN IsKeyValid(void)
{
	if(!memcmp(g_keyVeriBuf+4,g_keyCpuId+4,12))
	{
		return TRUE;
	}else
	{
		return FALSE;
	}
}

//#define SWD_TEST	 
//#define DSP_TEST
int main(void)
{			
//	int tick=0;
	Stm32_Clock_Init(9); //系统时钟设置
	delay_init(72);	     //延时初始化 
	//uart2_init(36,115200);	 //只有USART1使用PCLK2(最高72MHz)。其它USART使用PCLK1(最高36MHz)
	//uart1_init(72,115200);
	//LED_Init();		  	 //初始化与LED连接的硬件接口
	//SPIx_Init();
//#ifdef NBK600EX_DEBUG
#if(defined(TTL_DEBUG)||defined(MOTO_TEST)||defined(MOTO_DEBUG_PRT)||defined(EXT_TEST_PRT)\
			||defined(FIBER_ADJUST_TEST)||defined(KEY_DEBUG)||defined(SWD_TEST)\
			||defined(DSP_TEST)||defined(RGB_TEST)||defined(SLAVE_TEST)||defined(BOOT_TEST))
	uart1_init(72,115200);
	DBG_PRINTF("NBK project!!!\r\n");
#ifdef SWD_TEST
	printf("SWD test\r\n");
#endif
	//printf("20151113 the code changed\r\n");
	printf(__DATE__);
	printf("\r\n");
	printf(__TIME__);
	printf("\r\n");
#endif

#ifdef EXT_TEST_PRT
	//ExtCodeSetPrt();
#endif
//#endif

#ifdef SLAVE_TEST
	DBG_PRINTF("os start1\r\n");
#endif
	GolbalParaInit();

	KeyDecrypto();

#ifdef BOOT_TEST
	if(TRUE==IsKeyValid())
	{
		printf("key check ok\r\n");
	}else
	{
		printf("key check failed\r\n");
	}
#endif
	
#ifdef SLAVE_TEST
	DBG_PRINTF("os start2\r\n");
#endif

	UserInit(FLASH_USER_ADDR,1);

#ifdef SLAVE_TEST
	*g_cmdMode = CMD_MODE_LIGH_SLA;
	*wlSet.m_chan = WL_CHANNEL_THREE;
	FuncModuleSw();
	DBG_PRINTF("slave set 2\r\n");
#endif

#ifndef SLAVE_TEST
	uart2_init(36,115200);
	//uart2_init(36,57600);
/************************
************************/
	//g_tmHandleFlag = TM_HANDLE_SHUT;
	//g_comStatue = COM_STATUE_START;
	Timer4_Init(40,72);

// key adc should be initialiazed, otherwise the keyprocess would wait perment
	Adc_Init();

	//while(1);

//Floating cause the AF led lighted, there are must be pull down resistance to keep it state	
	PortInit();
	
#endif
//	GolbalParaInit();			//delete 2015/09/13

//	LCD_Init();
#ifndef SLAVE_TEST
if(KEY_POW_OFF>0)
//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
{
	LCD_Init();
	LCD_Clear(0);
	CheckBattery();
#ifdef _MOTO_
	MotoReset();			//before turn on charge_en
#endif
}else
{
	LcdPortInit();
}
#endif
//#ifdef FIBER_ADJUST_TEST
	//printf("test1\r\n");
//#endif

	//UserInit(1);
	//UserInit(FLASH_USER_ADDR,1);
#ifndef SLAVE_TEST
	UserScreenOn();
#endif
//#ifdef FIBER_ADJUST_TEST
	//printf("test2\r\n");
//#endif

	//g_flaEnalbe = 1;

	//printf("Timer4_Init\r\n");
//	if(KEY_POW_OFF>0)
//	{
		//Adc_Init();
		//printf("Adc_Init\r\n");
		//g_SettingIndex = WLM_SETTING_CHAN;
		//PortInit();
		//WlmCmdSet();
		//printf("PortInit\r\n");
		//LCD_Init();
		//printf("LCD_Init\r\n");
		//KEY_Init();
//#ifdef _MOTO_
//		MotoReset();			//before turn on charge_en
//#endif
//		UserInit();

//		g_sysStart = 0;
//	}else
//	{
//		LCD_Init();
//	}
//	UserInit();
//#ifdef _MOTO_
//	MotoReset();
//#endif
	//SetPreFlashDA();
	//TURN_ON_TRIGER(1000);
	//printf("UserInit\r\n");
	//CHARGE_LED1_OFF;
	//CHARGE_LED2_OFF;
	//TCS3414_Init();
	//delay_ms(10);
	//Lcd_test();
	
	//disp_fram(0xff,0xff,0xff,0xff);
	//delay_ms(500);
	//disp_fram(0,0,0,0);
#if 1
	//Flash_Interrupt_init();
	//DspTest();
	//TmCaptureInit(1000,72);
	//ClearWlcBuf();
	//uart2_init(36,115200);	 //只有USART1使用PCLK2(最高72MHz)。其它USART使用PCLK1(最高36MHz)
	//uart2_init(36,115200);
	//uart2_init(36,57600);
	//UserInit();
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
 //   	OSTmr_Init();                                                /* Initialize the Timer Manager             */
//#endif
		
	OSTaskCreate( TaskStart,	//task pointer
					(void *)0,	//parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
					START_TASK_Prio );	//task priority
	//printf("os start");	

	OSStart();
#else
	while(1);
#endif
	return 0;	   
}

void BkLiteAutoOff(void)
{
#ifdef _600EX_RT
	//if(COM_STATUE_IDLE==g_comStatue)
	if(0==pSnrSet[SNRMENU_BKLITE])	
	{
		//{
		//BK_LIGHT1 = 0;	
		//BK_LIGHT2 = 0;
		BK_TURNOFF(g_isHotProtect);
		//KEYLIGHT = 0;
		KEY_BK_OFF();
		bkParas.bkState= BK_OFF;
		//}	
		bkParas.bkFlag = BK_OFF;
	}

	if(WLM_SETTING_CHAN==g_SettingIndex)
	{
		//radioSet->id = g_rfChanSet;
		ChanSetConfirm();
	}

	if(IsModSwitchValid())
	{
		//g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE)
	}
#endif
}

void FlaOkAutoOff(void)
{
	FLAOK_OFF();
}

void SlaveResume(void)
{
	//TmCaptureInit(1000,72);	
	//ClearWlcBuf();
	TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
	LightSlaveParaClear();
}

void BkLiteOn(void)
{
#ifdef _600EX_RT
	INT8U tmrErr;
	//BK_LIGHT1 = 1;
	//BK_LIGHT2 = 1;
	if((0==pSnrSet[SNRMENU_BKLITE])||(2==pSnrSet[SNRMENU_BKLITE]))
	{
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
				if(pfnSet[PFN_ID_BKCLR1]>0)
				{
					BK_ORANGE(g_isHotProtect);
				}else
				{
					BK_GREEN(g_isHotProtect);
				}
			break;

			case CMD_MODE_RF_MST:
			case CMD_MODE_LIGH_MST:
			case CMD_MODE_RFLS_MST:
				if(pfnSet[PFN_ID_BKCLR2]>0)
				{
					BK_ORANGE(g_isHotProtect);
				}else
				{
					BK_GREEN(g_isHotProtect);
				}
			break;

			case CMD_MODE_RF_SLA:
			case CMD_MODE_LIGH_SLA:
			case CMD_MODE_RFLS_SLA:
				if(pfnSet[PFN_ID_BKCLR3]>0)
				{
					BK_GREEN(g_isHotProtect);
					//BK_ORANGE(g_isHotProtect);
				}else
				{
					BK_ORANGE(g_isHotProtect);
					//BK_GREEN(g_isHotProtect);
				}
			break;
		}
		//KEYLIGHT = 1;
		KEY_BK_ON();
		bkParas.bkState= BK_ON;
		bkParas.bkFlag = BK_ON;
	}
	//if(0==pSnrSet[SNRMENU_BKLITE])	
	{
#if OS_TMR_EN > 0
		OSTmrStart(g_bkTmr, &tmrErr) ;
#endif
	}

#endif
}

void BUZZER_ON(u16 duration,u16 times)
{
//#ifdef _ST_EX_
#if OS_TMR_EN > 0
	INT8U tmrErr;
	if(pSnrSet[SNRMENU_VOICE])
	{
		g_bzCount=times;
		//PWM_Init(2500,9,1250);			//
		//PWM_Init(2500,9,1250);
		PWM_Init(288,72,144);
		OSTmrReload(g_bzTmr,duration*100);
		OSTmrStart(g_bzTmr, &tmrErr);
	}
#endif
//#endif
}

void BzOff(void)
{
//#ifdef _ST_EX_
	static u16 du=0, ts=0;
//	INT8U tmrErr;
	du++;
	if(du>=g_bzDuration)
	{
		du = 0;
		ts++;
		if(ts>=g_bzCount)
		{
			//TIM3->CCMR1&=0xFF8F;
			TIM3->CCMR1&=0x8FFF;
			TIM3->CCMR1|=0x4000;
			TIM3->CCER&=~(1<<12);
			//TIM3->CR1&=0xFE; 
			//OSTmrStop(g_bzTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);	
			ts = 0;
			
			if(CMD_MODE_LIGH_SLA==*g_cmdMode)			//2015/11/16
			{
				StopCapture();
				TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
			}
		}else if(ts&1)
		{
			TIM3->CCMR1&=0x8FFF;
			TIM3->CCMR1|=0x4000;
			TIM3->CCER&=~(1<<12);
			//TIM3->CR1&=0xFE; 				//stop a cycle
			//TIM3->CCER&=~(1<<0);
		}else
		{
			//PWM_Init(2500,9,1250);
			PWM_Init(288,72,144);
			//OSTmrStart(g_bzTmr, &tmrErr) ;	//restart
		}
	}
	
//#endif
}

void CmrShutterPull(void)
{
	if(COM_STATUE_IDLE==g_comStatue)
	{
		ACT_SHUTTER = 0;
		//WIRE_SHUTTER = 0;
		//CHARGE_LED_OFF;
		g_shutterFlag = 0;
	}
	//FLAOK_OFF();
	g_shutterFlag = 0;
	WIRE_SHUTTER = 0;
}

void CmrParaReflash(void)
{
	g_SettingIndex = 0;
	//FLAOK_TOGGLE();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void RfParaSend(void)
{
/******************************************************************************************************
//	send rf parameter
******************************************************************************************************/
	LoadUartSendBuf(UART_ENCODE_RFCH,radioSet->rfChan,0);
	LoadUartSendBuf(UART_ENCODE_RFID, radioSet->id>>8,radioSet->id&0xff);
	//LoadUartArray(UART_ENCODE_RFADDR,g_snMac.mac,8);
	//LoadUartSendBuf(UART_ENCODE_RFMOD, *g_cmdMode,0);
	LoadUartSendBuf(UART_ENCODE_RFMOD,*g_cmdMode,*wlSlave.s_array);
	TrigerUartSend(0);
}

#if 0
void DspTest(void)
{
	//SpiDMAInit(DMA1_Channel5);
	//LcdPortInit();
	u16 i=0,j=0,k=0,h=0;
	u16 dmaBuf[20*64*2];
	char tmp;

	FLA_OK_LED = 0;
	
	LCD_Clear(BLACK);

	delay_ms(1000);

	FLA_OK_LED = 1;
	
	for(k=0;k<2;k++)
	{
		for(i=0;i<20;i++)
		{
			for(j=0;j<8;j++)
			{
				tmp=asc2_1608[i][k*8+j];
				for(h=0;h<8;h++)
				{
					if(tmp&0x01)
					{
						dmaBuf[k*20*64+i*64+j*8+h] = WHITE;
					}else
					{
						dmaBuf[k*20*64+i*64+j*8+h] = BLACK;
					}
					tmp>>=1;
				}
				//SpiDMASend(DMA1_Channel5,(u32)&SPI2->DR,(u32)(asc2_1608[i]+k*8),128);
			}
		}
	}
	//delay_ms(1000);
	AreaSet(20,20,16,20+160-1);
	SET_CD_DATA;
	SET_CS_ENABLE;
	SpiDMASend(DMA1_Channel5,(u32)&SPI2->DR,(u32)dmaBuf,20*64*2);
	SET_CS_DISABLE;
	FLA_OK_LED = 0;
	while(1);
	//LcdFillBlank(50,50,50,50,DSP_NORMAL);
}
#endif

//void UserInit(void)
//void UserInit(u8 initMod)
void UserInit(u32 paraAddr, u8 initMod)
{
	//u16 i;
	//signed char i;
	u8 i;			//the parameter exceed 128byte     2015/07/28
	char sum=0;
	char *pMain;
	//u16 ver=0;

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
#ifdef FIBER_ADJUST_TEST
		printf("before flash read\r\n");
#endif
	//LcdLoadPicture(0,LAYOUT_ETTL_X,LAY_WT_NOR,LAY_HT,ettl_EBitmap,DSP_NORMAL);
	//Draw_Circle(50,50,30);
	//LayModeEttl();
	//memset((u8*)&mainPara,0x00,sizeof(MainPara));
	//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),sizeof(mainPara)/2);
	//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)+2)/2);
	STMFLASH_Read(paraAddr,(u16*)(&mainPara),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)+2)/2);
	//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),((char*)&(mainPara.rsv)-(char*)&(mainPara.workMode)+2)/2);
	//STMFLASH_Read(FLASH_SERNUM_ADDR,(u16*)(&g_snMac),16/2);
	///STMFLASH_Read(FLASH_SERNUM_ADDR,(u16*)(&g_snMac),sizeof(g_snMac)/2);			//serial number and mac address
	//STMFLASH_Read(FLASH_VER_ADDR,&ver,1);
#ifdef FIBER_ADJUST_TEST
		printf("after flash read\r\n");
#endif

	pMain = (char*)&mainPara;
	//for(i=0;i<(sizeof(mainPara)-1);i++)
	for(i=0;i<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode));i++)
	{
		sum += pMain[i];
	}

	if(mainPara.sumVeri!=sum)
	{
		//memset((u8*)&mainPara,0,sizeof(mainPara));	
#ifdef FIBER_ADJUST_TEST
		printf("para error\r\n");
#endif
		memset((u8*)&mainPara,0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	}

/*
	g_ver[0] = (ver>>12)+'0';
	g_ver[1] = '.';
	g_ver[2] = ((ver>>8)&0x0f)+'0';
	g_ver[3] = ',';
	g_ver[4] = ((ver>>4)&0x0f)+'0';
	g_ver[5] = '.';
	g_ver[6] = (ver&0x0f)+'0';	
*/
	
	g_cmdMode = &mainPara.cmdMode;
	wlSet.m_arrPara = mainPara.modGr;
	mstPara = &mainPara.modGr[0];
	wlSet.m_chan = &mainPara.m_chan;
	wlSet.ttl_Ratio = &mainPara.ttl_Ratio;
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
	wlSet.cCmp = compensation_table[(wlSet.m_arrPara[WL_ARRARY_C].cmp1+6)*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2];
	//abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
	//abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;

	//abRation_fill1 = wlm_abRation_fillwords[*wlSet.ttl_Ratio].para1;
	//abRation_fill2 = wlm_abRation_fillwords[*wlSet.ttl_Ratio].para2;

	abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
	abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
	
	pSnrSet = mainPara.snrMenuSet;
	pfnSet = mainPara.snrMenuSet+18;

	g_rfIdSet = radioSet->id;
	g_rfChanSet = radioSet->rfChan;

//for test//
#if 0
	mainPara.extIso = 100;
	mainPara.extApert = 100;
	radioSet->id = 0x00;
	motoSet.focIndex = 1;
	*multiSet.frequence = 500;
	*multiSet.eTimes = 199;
#endif
//end//

	//if((*motoSet.dest_location<20)||(*motoSet.dest_location>200))
	//{
	//	*motoSet.dest_location = 20;
	//}	
/*
	for(i=0;i<sizeof(focLenScape);i++)
	{
		if(*motoSet.dest_location==focLenScape[i])
		{
				break;
		}
	}
*/
//#ifdef _CANON_
if(initMod>0)
{
	*motoSet.dest_location = 35;
	*motoSet.mode = MOTO_MODE_AUTO;
	*g_cmdMode = CMD_MODE_OFF;
	mainPara.workMode = WORKINGMODE_TTL;
	mainPara.synMode = SYNMODE_HEAD;
	*wlSet.is_masterOn = 1;
	motoSet.autoDest = 35;
	motoSet.focIndex = 0;
//#else
}else
{
	if((MOTO_MODE_AUTO!=*motoSet.mode)&&(MOTO_MODE_MANU!=*motoSet.mode))
	{
		*motoSet.mode = MOTO_MODE_AUTO;
	}
	if((*motoSet.dest_location<20)||(*motoSet.dest_location>200))
	{
		*motoSet.dest_location = 35;
	}

	//i = GetMotoStdIndex(*motoSet.dest_location);
	//if(GetMotoStdIndex(*motoSet.dest_location)>0)
	motoSet.focIndex = GetMotoStdIndex(*motoSet.dest_location);

#ifdef _ST_EX_
	if((*g_cmdMode!=CMD_MODE_LIGH_MST)&&(*g_cmdMode!=CMD_MODE_RF_MST)) *g_cmdMode = CMD_MODE_RF_MST;
#else
	//if((*g_cmdMode<CMD_MODE_OFF)||(*g_cmdMode>CMD_MODE_RF_MST)) *g_cmdMode = CMD_MODE_OFF;
	if((*g_cmdMode<CMD_MODE_OFF)||(*g_cmdMode>CMD_MODE_RFLS_MST)) *g_cmdMode = CMD_MODE_OFF;
#endif

	if((mainPara.workMode<WORKINGMODE_TTL)||(mainPara.workMode>WORKINGMODE_F2)) 
	{
		mainPara.workMode = WORKINGMODE_TTL;
	}
//#endif
}
	//if(i>=sizeof(focLenScape))
	/*
	if(i<0)
	{
		*motoSet.mode = MOTO_MODE_AUTO;
		*motoSet.dest_location = 35;
		motoSet.focIndex = 4;
	}else
	{
		motoSet.focIndex = i+1;
	}
	*/

	motoSet.destRsv = *motoSet.dest_location;
	menuPara.motoLoc = *motoSet.dest_location;
	//SetStByDsp();
	//radioSet->extAddr = g_snMac.mac;

	//uartSendData("usart\r\n",sizeof("usart\r\n")-1);
	//RfPreFlashInfo();
	//while(uart_tran_flag);
	//RfPreFlashInfo();
	//*g_cmdMode = CMD_MODE_LIGH;

	if(mainPara.s_array>WL_ARRARY_E)
	{
		mainPara.s_array = WL_ARRARY_A;
	}
	
	//if((mainPara.workMode<WORKINGMODE_TTL)||(mainPara.workMode>WORKINGMODE_F2)) mainPara.workMode = WORKINGMODE_TTL;
	if((90<*multiSet.eTimes)||(*multiSet.eTimes<0)) *multiSet.eTimes = 0;
	if((*multiSet.frequence>500)||(*multiSet.frequence<1)) *multiSet.frequence = 1;
	if((*multiSet.expose>5)||(*multiSet.expose<0)) *multiSet.expose = 0;
	
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);

	menuPara.mode = ((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07);

	for(i=0;i<3;i++)
	{
		menuPara.otpCod[i] = output_table[(7-wlSet.m_arrPara[i].output1)*3-wlSet.m_arrPara[i].output2];
		//menuPara.WlmMul[i] = output_table[(7-(wlSet.m_arrPara[i].multi_exp>>4))*3];
		menuPara.WlmMul[i] = output_table[(7-(wlSet.m_arrPara[i].multi_exp))*3];
		//menuPara.WlmMul[WL_ARRARY_B] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4))*3];
	}

	for(i=0;i<5;i++)
	{
	/*
		if((radioSet->rfGr[i].ttl_cmp1<-3)||(radioSet->rfGr[i].ttl_cmp1>3))
		{
			radioSet->rfGr[i].ttl_cmp1 = 0;
			radioSet->rfGr[i].ttl_cmp2 = 0;
		}
		if((radioSet->rfGr[i].ttl_cmp2<0)||(radioSet->rfGr[i].ttl_cmp2>2))
		{
			radioSet->rfGr[i].ttl_cmp2 = 0;
		}
		if((radioSet->rfGr[i].m_exp1<0)||(radioSet->rfGr[i].m_exp1>7))
		{
			radioSet->rfGr[i].m_exp1 = 0;
			radioSet->rfGr[i].m_exp2 = 0;
		}

		if((radioSet->rfGr[i].m_exp2<0)||(radioSet->rfGr[i].m_exp2>2))
		{
			radioSet->rfGr[i].m_exp2 = 0;
		}
	*/		
		menuPara.rfGrOtp[i] = output_table[(7-radioSet->rfGr[i].m_exp1)*3-radioSet->rfGr[i].m_exp2];
		menuPara.rfGrTmp[i] = compensation_table[(radioSet->rfGr[i].ttl_cmp1+6)*3+radioSet->rfGr[i].ttl_cmp2];
	}

	if((mainPara.extIso<25)||(mainPara.extIso>51200))
	{
		mainPara.extIso = 100;
	}

	if((mainPara.extApert<10)||(mainPara.extApert>910))
	{
		mainPara.extApert = 56;
	}

#ifdef FIBER_ADJUST_PROC
	if((mainPara.fiberCoe<0)||(mainPara.fiberCoe>18))
	{
		mainPara.fiberCoe = 9;
	}
	if(mainPara.isFiberAdjust!=FIBER_ADJUST_FLAG)
	{
		mainPara.fiberCoe = 9;
	}
#endif
	//RfParaSend();

	//mainPara.workMode = WORKINGMODE_TTL;
	//*wlSet.arr_mode = WLM_MODE_RES;

	//LCD_Clear(bkSetArr[pSnrSet[SNRMENU_BKCLR]]);
	//LCD_Clear(0);

//	Mode(mainPara.workMode);
#ifdef FIBER_ADJUST_TEST
	printf("before mode\r\n");
#endif

#if 0
	if(KEY_POW_OFF>0)
	//if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
		Mode();
		RfMoudleSet();			//2015/7/6
		//if(!KEY_POW_ON)
		if(KEY_LOCK_EX>0)
		{
			LoadButtons();
		}
	}
#endif

	//if((KEY_LOCK_EX>0)&&(KEY_POW_OFF>0))
	//if(!KEY_POW_ON)
	//{
	//	LoadButtons();
	//}

	//FuncDsp();

	//delete at 2015/06/14
	//RfParaSend();				//inform the Rf parameters

	//OSTmrCreate(200, 400,  OS_TMR_OPT_PERIODIC,  CanTmrHandle,  NULL, "CanTmr ", tmrErr);

	//CR_READY = 0;
	//BK_LIGHT = 1;
	//BkLiteOn();
	
}

void UserScreenOn(void)
{
	if(KEY_POW_OFF>0)
	//if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
		Mode();
		RfMoudleSet();			//2015/7/6
		//if(!KEY_POW_ON)
		if(KEY_LOCK_EX>0)
		{
			LoadButtons();
		}
	}
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
	
#if(defined SLAVE_TEST)
#else
	OSTaskCreate(TaskKeyScan, (void * )0, (OS_STK *)&TASK_KEYPROC_STK[KEYPROC_STK_SIZE-1], KEYPROC_TASK_Prio);
	//OSTaskCreate(TaskDspUpdate, (void *) 0,( OS_STK *) &TASK_REFRESH_STK[REFRESH_STK_SIZE-1],REFRESH_TASK_Prio);
	OSTaskCreate(TaskMonitor, (void * )0, (OS_STK *)&TASK_LED_STK[LED_STK_SIZE-1], LED_TASK_Prio);
	OSTaskCreate(TaskVoltageCheck,(void *)0,(OS_STK *)&TASK_VOLTAGE_STK[VOL_CK_SIZE-1],VOL_CK_Prio);

	OSTaskCreate(TaskMotoProc,(void *)0, (OS_STK *)&TASK_MOTOTICK_STK[MOTO_TICK_SIZE-1],MOTO_TICK_Prio);
#endif
	//OSTaskCreate(TaskTimerTickCom,(void * )0, (OS_STK *)&TASK_TICK_STK[TICK_STK_SIZE-1],TICK_TASK_Prio);
#if OS_TMR_EN > 0
	//g_osTmr = OSTmrCreate(200, 400,  OS_TMR_OPT_PERIODIC,  BkLiteAutoOff,  NULL, "CanTmr ", &tmrErr);
	g_bkTmr = OSTmrCreate(10000, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)BkLiteAutoOff,  NULL, "bkTmr ", &tmrErr);
	g_flashTmr = OSTmrCreate(200, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "flaTmr ", &tmrErr);
	g_bzTmr = OSTmrCreate(200, 200,  OS_TMR_OPT_PERIODIC,  (OS_TMR_CALLBACK)BzOff,  NULL, "bzTmr ", &tmrErr);
	g_shutterPullTmr = OSTmrCreate(300,0,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)CmrShutterPull,NULL,"shutterPullTmr",&tmrErr);
	g_1ShotTmr = OSTmrCreate(3000, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)FlaOkAutoOff,  NULL, "1ShotTmr ", &tmrErr);
	g_slaChanDivTmr = OSTmrCreate(200,0,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)SlaveResume,NULL,"SlaveResume",&tmrErr);
	//g_paraUpdateTmr = OSTmrCreate(50,0,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)CmrParaReflash,NULL,"paTmr",&tmrErr);
	//BkLiteOn();
	//OSTmrStart(g_osTmr, &tmrErr) ;
#endif
	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
/*if this task is waked up, go to standby mode*/
//	BACKLIGHT_ON();
	
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

#ifdef _MOTO_
	//MotoProc();
	MotoTickDriver();				//delete for moto debug 2015/10/10
#endif
	
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

void RgbTest(void)
{
	u8 cnt;
	u16 vRgb[4]={0};
	for(cnt=0;cnt<6;cnt++)
	{
		RGB_LED_ON();
		//StartTcs3414();
		StartRgbSample();
		HaltSec(200);
		while(!GetRgbCvtState())
		{
			HaltSec(1);
		}
		//TCS3414_ReadChan(vRgb,vRgb+1,vRgb+2,vRgb+3);
		Rgb_ReadChan(vRgb,vRgb+1,vRgb+2,vRgb+3);
		//StopTcs3414();
		StopRgbSample();
		RGB_LED_OFF();
		HaltSec(200);
	}
}


#if(RGB_IC==RGB_TCS3414)

#define RGB_CHK_VAL                    50//100//200//100//200//100

#define CAN_RGB1_CHAN1_DEFAULT     165//135//200//135
#define CAN_RGB1_CHAN2_DEFAULT     290
#define CAN_RGB1_CHAN3_DEFAULT     50//100//100//200//100

#define CAN_RGB2_CHAN1_DEFAULT     420
#define CAN_RGB2_CHAN2_DEFAULT     310//290
#define CAN_RGB2_CHAN3_DEFAULT     480//410

#define NBK_RGB1_CHAN1_DEFAULT     116//135//200//135
#define NBK_RGB1_CHAN2_DEFAULT     200
#define NBK_RGB1_CHAN3_DEFAULT     100//100//200//100

#define NBK_RGB2_CHAN1_DEFAULT     275
#define NBK_RGB2_CHAN2_DEFAULT     200
#define NBK_RGB2_CHAN3_DEFAULT     252

#elif(RGB_IC==RGB_BH1745)

#define RGB_CHK_VAL                    400//100//200//100//200//100

#define CAN_RGB1_CHAN1_DEFAULT     2740//1330//165//135//200//135
#define CAN_RGB1_CHAN2_DEFAULT     1120//520//290
#define CAN_RGB1_CHAN3_DEFAULT     400//76//50//100//100//200//100

#define CAN_RGB2_CHAN1_DEFAULT     2800//1388//420
#define CAN_RGB2_CHAN2_DEFAULT     3800//1880//1930//310//290
#define CAN_RGB2_CHAN3_DEFAULT     3088//1918//480//410

#define NBK_RGB1_CHAN1_DEFAULT     2400//1150//1098//116//135//200//135
#define NBK_RGB1_CHAN2_DEFAULT     1100//450//435//200
#define NBK_RGB1_CHAN3_DEFAULT     400//195//100//100//200//100

#define NBK_RGB2_CHAN1_DEFAULT     2800//1080//983//275
#define NBK_RGB2_CHAN2_DEFAULT     3200//1308//1170//200
#define NBK_RGB2_CHAN3_DEFAULT     2600//1245//1112//252

#endif

//u8 CalcRgbFilterStyle(u16* vRgb, u16* vRst)
u8 CalcRgbFilterStyle(u16* vRgb, u16* vRst, u8 tms)
{
	u8 i=0;
	u8 rgbStyle=0;
	//u16 redChan=0;
	//u16 greenChan=0;
	//u16 blueChan=0;
	//u16 clearChan=0;

	memset(vRst,0x00,4*sizeof(u16));

	//for(i=0;i<RGB_CHECK_TIMES;i++)
	for(i=0;i<tms;i++)
	{
		//redChan+=vRgb[i*4];
		//greenChan+=vRgb[i*4+1];
		//blueChan+=vRgb[i*4+2];	
		//clearChan+=vRgb[i*4+3];
		vRst[0] +=vRgb[i*4];
		vRst[1] +=vRgb[i*4+1];
		vRst[2] +=vRgb[i*4+2];
		vRst[3] +=vRgb[i*4+3];
	}

	//redChan = redChan/RGB_CHECK_TIMES;
	//greenChan = greenChan/RGB_CHECK_TIMES;
	//blueChan = blueChan/RGB_CHECK_TIMES;
	//clearChan = clearChan/RGB_CHECK_TIMES;

	//vRst[0] = vRst[0]/RGB_CHECK_TIMES;
	//vRst[1] = vRst[1]/RGB_CHECK_TIMES;
	//vRst[2] = vRst[2]/RGB_CHECK_TIMES;
	//vRst[3] = vRst[3]/RGB_CHECK_TIMES;

	vRst[0] = vRst[0]/tms;
	vRst[1] = vRst[1]/tms;
	vRst[2] = vRst[2]/tms;
	vRst[3] = vRst[3]/tms;

	for(i=0;i<4;i++)					//2015/09/20
	{
		if(vRst[i]<RGB_CHK_VAL)
		{
			vRst[i]= RGB_CHK_VAL;
		}
	}

	//if((clearChan>320)&&(clearChan<420))
#ifdef FIBER_ADJUST_PROC
	//if(0xA5==mainPara.isRgbChk)
	if(RGB_REFINE_FLAG==mainPara.isRgbChk)
	{
		//if((vRst[3]>(mainPara.rgbRcd[0]-50))&&(vRst[3]<(mainPara.rgbRcd[0]+50)))
		//if((vRst[3]>(mainPara.rgbRcd[0]-RGB_CHK_VAL))&&(vRst[3]<(mainPara.rgbRcd[0]+RGB_CHK_VAL)))
		if((vRst[0]>(CAN_RGB1_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<CAN_RGB1_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(CAN_RGB1_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<CAN_RGB1_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(CAN_RGB1_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<CAN_RGB1_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 1;			//dark
		}//else if((clearChan>800)&&(clearChan<900))
		//else if((vRst[3]>1150)&&(vRst[3]<1250))
		else if((vRst[0]>(CAN_RGB2_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<CAN_RGB2_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(CAN_RGB2_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<CAN_RGB2_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(CAN_RGB2_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<CAN_RGB2_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 2;			//light
		}else if((vRst[0]>(NBK_RGB1_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<NBK_RGB1_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(NBK_RGB1_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<NBK_RGB1_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(NBK_RGB1_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<NBK_RGB1_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 1;			//dark
		}//else if((clearChan>800)&&(clearChan<900))
		//else if((vRst[3]>1150)&&(vRst[3]<1250))
		else if((vRst[0]>(NBK_RGB2_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<NBK_RGB2_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(NBK_RGB2_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<NBK_RGB2_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(NBK_RGB2_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<NBK_RGB2_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 2;			//light
		}else if((vRst[0]>(mainPara.rgb1Chan[0]-RGB_CHK_VAL))&&(vRst[0]<mainPara.rgb1Chan[0]+RGB_CHK_VAL)
			&&(vRst[1]>(mainPara.rgb1Chan[1]-RGB_CHK_VAL))&&(vRst[1]<mainPara.rgb1Chan[1]+RGB_CHK_VAL)
			&&(vRst[2]>(mainPara.rgb1Chan[2]-RGB_CHK_VAL))&&(vRst[2]<mainPara.rgb1Chan[2]+RGB_CHK_VAL))
		{
			rgbStyle = 1;			//dark
		}//else if((vRst[3]>(mainPara.rgbRcd[1]-50))&&(vRst[3]<(mainPara.rgbRcd[1]+50)))
		//else if((vRst[3]>(mainPara.rgbRcd[1]-RGB_CHK_VAL))&&(vRst[3]<(mainPara.rgbRcd[1]+RGB_CHK_VAL)))
		else if((vRst[0]>(mainPara.rgb2Chan[0]-RGB_CHK_VAL))&&(vRst[0]<mainPara.rgb2Chan[0]+RGB_CHK_VAL)
			&&(vRst[1]>(mainPara.rgb2Chan[1]-RGB_CHK_VAL))&&(vRst[1]<mainPara.rgb2Chan[1]+RGB_CHK_VAL)
			&&(vRst[2]>(mainPara.rgb2Chan[2]-RGB_CHK_VAL))&&(vRst[2]<mainPara.rgb2Chan[2]+RGB_CHK_VAL))
		{
			rgbStyle = 2;			//light
		}else if((vRst[0]>(mainPara.rgb3Chan[0]-RGB_CHK_VAL))&&(vRst[0]<mainPara.rgb3Chan[0]+RGB_CHK_VAL)
			&&(vRst[1]>(mainPara.rgb3Chan[1]-RGB_CHK_VAL))&&(vRst[1]<mainPara.rgb3Chan[1]+RGB_CHK_VAL)
			&&(vRst[2]>(mainPara.rgb3Chan[2]-RGB_CHK_VAL))&&(vRst[2]<mainPara.rgb3Chan[2]+RGB_CHK_VAL))
		{
			rgbStyle = 1;
		}
		else if((vRst[0]>(mainPara.rgb4Chan[0]-RGB_CHK_VAL))&&(vRst[0]<mainPara.rgb4Chan[0]+RGB_CHK_VAL)
			&&(vRst[1]>(mainPara.rgb4Chan[1]-RGB_CHK_VAL))&&(vRst[1]<mainPara.rgb4Chan[1]+RGB_CHK_VAL)
			&&(vRst[2]>(mainPara.rgb4Chan[2]-RGB_CHK_VAL))&&(vRst[2]<mainPara.rgb4Chan[2]+RGB_CHK_VAL))
		{
			rgbStyle = 2;
		}
			else
		{
			rgbStyle = 0;
		}
		
	}else
#endif
	{
		//if((vRst[3]>420)&&(vRst[3]<520))
		if((vRst[0]>(CAN_RGB1_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<CAN_RGB1_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(CAN_RGB1_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<CAN_RGB1_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(CAN_RGB1_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<CAN_RGB1_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 1;			//dark
		}//else if((clearChan>800)&&(clearChan<900))
		//else if((vRst[3]>1150)&&(vRst[3]<1250))
		else if((vRst[0]>(CAN_RGB2_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<CAN_RGB2_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(CAN_RGB2_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<CAN_RGB2_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(CAN_RGB2_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<CAN_RGB2_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 2;			//light
		}else if((vRst[0]>(NBK_RGB1_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<NBK_RGB1_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(NBK_RGB1_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<NBK_RGB1_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(NBK_RGB1_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<NBK_RGB1_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 1;			//dark
		}//else if((clearChan>800)&&(clearChan<900))
		//else if((vRst[3]>1150)&&(vRst[3]<1250))
		else if((vRst[0]>(NBK_RGB2_CHAN1_DEFAULT-RGB_CHK_VAL))&&(vRst[0]<NBK_RGB2_CHAN1_DEFAULT+RGB_CHK_VAL)
			&&(vRst[1]>(NBK_RGB2_CHAN2_DEFAULT-RGB_CHK_VAL))&&(vRst[1]<NBK_RGB2_CHAN2_DEFAULT+RGB_CHK_VAL)
			&&(vRst[2]>(NBK_RGB2_CHAN3_DEFAULT-RGB_CHK_VAL))&&(vRst[2]<NBK_RGB2_CHAN3_DEFAULT+RGB_CHK_VAL))
		{
			rgbStyle = 2;			//light
		}else
		{
			rgbStyle = 0;
		}
	}
	
	return rgbStyle;
	
}

#ifdef FIBER_ADJUST_PROC
void RgbCheckProc(void)
{
	u8 cnt=0;
	u8 tm=0;
	u16 vRgb[24]={0};
	u16 vRgbRst[4]={0};
	
	if(g_isRgbFilter!=RGB_DET)
	{
		HaltSec(100);
		if(g_isRgbFilter!=RGB_DET)
		{
			g_isRgbFilter = RGB_DET;

			if(!g_isRgbFilter)
			{
				for(cnt=0;cnt<RGB_CHECK_TIMES;cnt++)
				{
					RGB_LED_ON();
					HaltSec(200);
					//StartTcs3414();
					StartRgbSample();
					while(!GetRgbCvtState())
					{
						HaltSec(1);
						if(tm++>100)
						{
							tm=0;
							break;
						}
					}
					//TCS3414_ReadChan(vRgb+cnt*4,vRgb+cnt*4+1,vRgb+cnt*4+2,vRgb+cnt*4+3);
					Rgb_ReadChan(vRgb+cnt*4,vRgb+cnt*4+1,vRgb+cnt*4+2,vRgb+cnt*4+3);
					//StopTcs3414();
					StopRgbSample();
					RGB_LED_OFF();
					HaltSec(200);
				}
#if 0
				LCD_Clear(0);

				for(cnt=0;cnt<RGB_CHECK_TIMES;cnt++)
				{
					DspIsoValue(LAYOUT_PARA_AREA2, cnt*32,vRgb[cnt*4+0],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+2, cnt*32,vRgb[cnt*4+1],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+4, cnt*32,vRgb[cnt*4+2],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+6, cnt*32,vRgb[cnt*4+3],DSP_NORMAL);
				}
				while(PILOT_IN);
#endif

				g_rgbStyle = CalcRgbFilterStyle(vRgb,vRgbRst,RGB_CHECK_TIMES);

				//mainPara.isRgbChk = 0xA5;
				mainPara.isRgbChk = RGB_REFINE_FLAG;
				/*
				if(!g_RgbCheckTurn)
				{
					mainPara.rgb1Chan[0] = vRgbRst[0];
					mainPara.rgb1Chan[1] = vRgbRst[1];
					mainPara.rgb1Chan[2] = vRgbRst[2];
				}else
				{
					mainPara.rgb2Chan[0] = vRgbRst[0];
					mainPara.rgb2Chan[1] = vRgbRst[1];
					mainPara.rgb2Chan[2] = vRgbRst[2];
				}*/
				switch(g_RgbCheckTurn)
				{
					case 0:
						mainPara.rgb1Chan[0] = vRgbRst[0]>RGB_CHK_VAL?vRgbRst[0]:RGB_CHK_VAL;
						mainPara.rgb1Chan[1] = vRgbRst[1]>RGB_CHK_VAL?vRgbRst[1]:RGB_CHK_VAL;
						mainPara.rgb1Chan[2] = vRgbRst[2]>RGB_CHK_VAL?vRgbRst[2]:RGB_CHK_VAL;
					break;

					case 1:
						mainPara.rgb2Chan[0] = vRgbRst[0]>RGB_CHK_VAL?vRgbRst[0]:RGB_CHK_VAL;
						mainPara.rgb2Chan[1] = vRgbRst[1]>RGB_CHK_VAL?vRgbRst[1]:RGB_CHK_VAL;
						mainPara.rgb2Chan[2] = vRgbRst[2]>RGB_CHK_VAL?vRgbRst[2]:RGB_CHK_VAL;
					break;

					case 2:
						mainPara.rgb3Chan[0] = vRgbRst[0]>RGB_CHK_VAL?vRgbRst[0]:RGB_CHK_VAL;
						mainPara.rgb3Chan[1] = vRgbRst[1]>RGB_CHK_VAL?vRgbRst[1]:RGB_CHK_VAL;
						mainPara.rgb3Chan[2] = vRgbRst[2]>RGB_CHK_VAL?vRgbRst[2]:RGB_CHK_VAL;
					break;

					case 3:
						mainPara.rgb4Chan[0] = vRgbRst[0]>RGB_CHK_VAL?vRgbRst[0]:RGB_CHK_VAL;
						mainPara.rgb4Chan[1] = vRgbRst[1]>RGB_CHK_VAL?vRgbRst[1]:RGB_CHK_VAL;
						mainPara.rgb4Chan[2] = vRgbRst[2]>RGB_CHK_VAL?vRgbRst[2]:RGB_CHK_VAL;
					break;



				}
				//mainPara.rgbRcd[g_RgbCheckTurn] = vRgbRst[3];
				SavePara(FLASH_USER_ADDR);

				FiberAdjustProc();

			}else
			{
				FiberAdjustProc();
			}
			
		}
	}
}
#endif

void SensorMonitor(void)
//u8 SensorMonitor(void)
{
	static u8 upSensor=0;
	static u8 dnSensor=0;
	static u8 autoDestRsv=35;
	u16 vRgb[24]={0};
	u16 vRgbRst[4]={0};
	u8 cnt=0;
	u8 tm=0;
	u8 isMod=0;

	//u8 rgbRt=0;
	
	if(g_isRgbFilter!=RGB_DET)
	{
		HaltSec(500);
		//HaltSec(900);
		if(g_isRgbFilter!=RGB_DET)
		{
			
			g_isRgbFilter = RGB_DET;

			if(!g_isRgbFilter)
			{

				g_rgbStyle = 0;
				
				for(cnt=0;cnt<RGB_CHECK_TIMES;cnt++)
				{
					RGB_LED_ON();
					HaltSec(200);
					//StartTcs3414();
					StartRgbSample();
					while(!GetRgbCvtState())
					{
						HaltSec(1);
						if(tm++>100)
						{
							tm=0;
							break;
						}
					}

					if(!g_rgbStyle)
					{					
						//TCS3414_ReadChan(vRgb+cnt*4,vRgb+cnt*4+1,vRgb+cnt*4+2,vRgb+cnt*4+3);
						Rgb_ReadChan(vRgb+cnt*4,vRgb+cnt*4+1,vRgb+cnt*4+2,vRgb+cnt*4+3);

						//g_rgbStyle = CalcRgbFilterStyle(vRgb,vRgbRst,cnt+1);
						g_rgbStyle = CalcRgbFilterStyle(vRgb+cnt*4,vRgbRst,1);

						if(g_rgbStyle>0)
						{
							SET_UPDATE_EVT(DSP_UPDATE_MODE);
						}
					}
					//StopTcs3414();
					StopRgbSample();
					RGB_LED_OFF();
					if((RGB_DET>0)||(!KEY_POW_OFF))
					{
						HaltSec(30);
						if((RGB_DET>0)||(!KEY_POW_OFF))
						{
							break;
						}
					}
					HaltSec(200);
				}

#if(defined RGB_TEST)
				if(!g_rgbStyle)
				{
					for(cnt=0;cnt<RGB_CHECK_TIMES;cnt++)
					{
						printf("%d, %d, %d, %d",vRgb[cnt*4+0],vRgb[cnt*4+1],vRgb[cnt*4+2],vRgb[cnt*4+3]);
						printf("\r\n");
					}
				}
#endif
#if 0
				LCD_Clear(0);

				for(cnt=0;cnt<2;cnt++)
				{
					DspIsoValue(LAYOUT_PARA_AREA2, cnt*64,vRgb[cnt*4+0],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+2, cnt*64,vRgb[cnt*4+1],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+4, cnt*64,vRgb[cnt*4+2],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+6, cnt*64,vRgb[cnt*4+3],DSP_NORMAL);
				}
				while(PILOT_IN);
				
				LCD_Clear(0);
				for(cnt=2;cnt<4;cnt++)
				{
					DspIsoValue(LAYOUT_PARA_AREA2, (cnt-2)*64,vRgb[cnt*4+0],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+2, (cnt-2)*64,vRgb[cnt*4+1],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+4, (cnt-2)*64,vRgb[cnt*4+2],DSP_NORMAL);
					DspIsoValue(LAYOUT_PARA_AREA2+6, (cnt-2)*64,vRgb[cnt*4+3],DSP_NORMAL);
				}
				while(PILOT_IN);
				
				LCD_Clear(0);
				
				DspIsoValue(LAYOUT_PARA_AREA2, (5-2)*64,vRgb[5*4+0],DSP_NORMAL);
				DspIsoValue(LAYOUT_PARA_AREA2+2, (5-2)*64,vRgb[5*4+1],DSP_NORMAL);
				DspIsoValue(LAYOUT_PARA_AREA2+4, (5-2)*64,vRgb[5*4+2],DSP_NORMAL);
				DspIsoValue(LAYOUT_PARA_AREA2+6, (5-2)*64,vRgb[5*4+3],DSP_NORMAL);

				while(PILOT_IN);
				
#endif
				//g_rgbStyle = CalcRgbFilterStyle(vRgb,vRgbRst,RGB_CHECK_TIMES);

//				rgbRt = 1;
				
			}else
			{
				g_rgbStyle = 0;
			}
			
			isMod = 1;
		}
		
	}

	if((upSensor!=CAP_SENSOR_UP)||(dnSensor!=CAP_SENSOR_DN))
	{
		HaltSec(100);
		if((upSensor!=CAP_SENSOR_UP)||(dnSensor!=CAP_SENSOR_DN))
		{
			if(CAP_SENSOR_DN>0)
			{
				//if(CAP_SENSOR_UP<0)
				if(!CAP_SENSOR_UP)
				{
					g_upDnSensor = SENSOR_UPDN_UP;
				}else
				{
					g_upDnSensor = 0;
					
				}
			}else
			{
				g_upDnSensor = SENSOR_UPDN_DOWN;
			}
			upSensor = CAP_SENSOR_UP;
			dnSensor = CAP_SENSOR_DN;
			isMod = 1;
			if(MOTO_MODE_AUTO==*motoSet.mode)
			{
			/*
				if(!g_upDnSensor)
				{
					motoSet.destRsv = autoDestRsv;
				}else
				{
					autoDestRsv = motoSet.destRsv;
					motoSet.destRsv = 35;					
				}
			*/
				if(SENSOR_UPDN_UP==g_upDnSensor)
				{
					autoDestRsv = motoSet.destRsv;
					motoSet.destRsv = 35;
				}else
				{
					motoSet.destRsv = autoDestRsv;
				}
			}
			
		}
	}

	if(isMod>0)
	{
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}

//	return rgbRt;
}


#ifdef SLAVE_POWER_DOWN
#define SLA_PWR_OFF_T0   (80*60*60)
#define SLA_PWR_OFF_T1   (10*60*60)
//#define SLA_PWR_OFF_T0   (80*6)
//#define SLA_PWR_OFF_T1   (10*6)
void DspSlaveSleepScr(void)
{	
	u32 pwrDwnTmr=0;
	LCD_Clear(0);
	DrawFrame();	
	DisplayPicture(4, (LCD_W-22)/2, 22, 24, font_cfn10, DSP_NORMAL);
	DISABLE_CHARGE();				//Slave mode
	CHARGE_LED_OFF();
	AF_OFF();
	DISABLE_KEY_INT();
	DISABLE_FLASH_INT();
	DISABLE_SYN_INT();
	g_SettingIndex = 0;
	g_chargeOk = 0;						//2015/08/10  to avoid twinkle when waked up

	//g_keyAdcEnable = 0;
	//ADC1->CR2 &= ~(1<<0);			//turn off adc
	KeyAdcDisable();

	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		StopCapture();
	}

	SlaveSleepWakeIntSet();
	
	delay_ms(10);
	g_comStatue = COM_STATUE_SLEEP;
	//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//PWR_EnterSLEEPMode(0,0x01);

	//RCC_Configuration();

	//while(1);

	while(COM_STATUE_SLEEP==g_comStatue)
	{
		//OSTimeDlyHMSM(0,0,0,100);
		delay_ms(100);
		//OSTimeDlyHMSM(0,0,0,100);
		if(pwrDwnTmr++>(pSnrSet[SNRMENU_MSTD]>0?(SLA_PWR_OFF_T1):(SLA_PWR_OFF_T0)))
		{
			pwrDwnTmr = 0;
			Sleep_Init();
		}
	}

	SlaveSleepWakeIntClear();

	ENABLE_FLASH_INT();
	ENABLE_SYN_INT();

	ENABLE_KEY_INT();
	
	//ADC1->CR2|=(1<<0);
	//g_keyAdcEnable = 1;
	KeyAdcEnable();

	if(CLK_IN>0)
	{
		EXTIx_Init(GPIO_A,FTIR);
		//OSTimeDlyHMSM(0,0,0,1);
		delay_ms(1);
		SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
		g_cmrLinked = 1;
		g_comStatue = COM_STATUE_START;
	}else
	{
		EXTIx_Init(GPIO_A,RTIR);
		SPI_MISO_AUX  = 0;
		COM_AUX = 0;
		g_cmrLinked = 0;
		g_comStatue = COM_STATUE_IDLE;
	}

	LCD_Clear(0);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}
#else

void DspSlaveSleepScr(void)
{	
	LCD_Clear(0);
	DrawFrame();	
	DisplayPicture(4, (LCD_W-22)/2, 22, 24, font_cfn10, DSP_NORMAL);
	DISABLE_CHARGE();				//Slave mode
	CHARGE_LED_OFF();
	AF_OFF();
	DISABLE_KEY_INT();
	DISABLE_FLASH_INT();
	DISABLE_SYN_INT();
	g_SettingIndex = 0;
	g_chargeOk = 0;						//2015/08/10  to avoid twinkle when waked up

	//g_keyAdcEnable = 0;
	//ADC1->CR2 &= ~(1<<0);			//turn off adc
	KeyAdcDisable();

	SlaveSleepWakeIntSet();
	
	delay_ms(10);
	g_comStatue = COM_STATUE_SLEEP;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//PWR_EnterSLEEPMode(0,0x01);

	RCC_Configuration();

	SlaveSleepWakeIntClear();

	ENABLE_FLASH_INT();
	ENABLE_SYN_INT();

	ENABLE_KEY_INT();
	
	//ADC1->CR2|=(1<<0);
	//g_keyAdcEnable = 1;
	KeyAdcEnable();

	if(CLK_IN>0)
	{
		EXTIx_Init(GPIO_A,FTIR);
		//OSTimeDlyHMSM(0,0,0,1);
		delay_ms(1);
		SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
		g_cmrLinked = 1;
		g_comStatue = COM_STATUE_START;
	}else
	{
		EXTIx_Init(GPIO_A,RTIR);
		SPI_MISO_AUX  = 0;
		COM_AUX = 0;
		g_cmrLinked = 0;
		g_comStatue = COM_STATUE_IDLE;
	}

	LCD_Clear(0);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}
#endif

void SleepTimerProc(void)
{
	//WORD stbyTmr=0;
	u32 stbyTmr=0;
	//if((!pSnrSet[SNRMENU_STBY])&&(COM_STATUE_IDLE==g_comStatue))
	//{
	if((*g_cmdMode!=CMD_MODE_LIGH_SLA)&&(*g_cmdMode!=CMD_MODE_RF_SLA))
	{
		if((!pSnrSet[SNRMENU_STBY])&&(COM_STATUE_IDLE==g_comStatue))
		{
			gWSleepCounter++;	
		}
		if(gWSleepCounter>20*90)		//A tick 50ms, 20ticks 1s, 90s goto sleep
		//if(gWSleepCounter>20*9)		//A tick 50ms, 20ticks 1s, 9s goto sleep
		{
			gWSleepCounter=0;
#ifdef RF_LOWPWR_SLEEP
			//UartSctSend(UART_ENCODE_EXIT, 1, 0);
			//OSTimeDlyHMSM(0,0,0,50);
			ENABLE_RF_MODULE();
			UartSctSend(UART_ENCODE_EXIT, 1, 0);
			OSTimeDlyHMSM(0,0,0,100);
			UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_SLEEP, 0);
			OSTimeDlyHMSM(0,0,0,30);
#else
			ENABLE_RF_MODULE();
			OSTimeDlyHMSM(0,0,0,500);
			UartSctSend(UART_ENCODE_EXIT, 1, 0);
			OSTimeDlyHMSM(0,0,0,30);
			UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_SLEEP, 0);
			OSTimeDlyHMSM(0,0,0,30);
#endif
			Sleep_Init();
			//SPI_MISO_AUX=1;	
			//PortInit();
			//SPI_MISO_AUX=0;	
			//Adc_Init();
			//SPI_MISO_AUX=1;	
			//LCD_Init();
			//SPI_MISO_AUX=0;	
			//Mode();
			//SPI_MISO_AUX=1;	
		}
	}else
	{
		if((CMD_MODE_RF_SLA!=*g_cmdMode)||(!g_isRfSlaveLinked))
		{
			gWSleepCounter++;
		}
		if(pSnrSet[SNRMENU_SSTD]>0)
		{
			stbyTmr = 20*60*10;				//10 minuter
			//stbyTmr = 20*15;				//10 minuter
		}else
		{
			stbyTmr = 20*60*60;				//60 minuter
			//stbyTmr = 20*15;
		}

		if(gWSleepCounter>stbyTmr)
		{
			gWSleepCounter = 0;
			//UartSctSend(UART_ENCODE_SLASLEEP, pSnrSet[SNRMENU_MSTD], 0);
			//OSTimeDlyHMSM(0,0,0,500);
			//OSTimeDlyHMSM(0,0,0,500);
			//UartSctSend(UART_ENCODE_EXIT, 1, 0);
			DspSlaveSleepScr();
			if(CMD_MODE_LIGH_SLA==*g_cmdMode)
			{
				//OSTimeDlyHMSM(0,0,0,50);
				//TmCaptureInit(1000,72);	
				TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
				//ClearWlcBuf();
				LightSlaveParaClear();	
			}
		}
	}
	//}



	
}

void PowerDownProc(void)
{
	//if(KEY_LOCK_EX&&KEY_POW_ON&&!KEY_POW_OFF)
	//if(!KEY_POW_OFF)
	if(KEY_LOCK_EX&&!KEY_POW_OFF)
	//if((KEY_LOCK_EX>0)&&(KEY_POW_ON>0))
	//if(KEY_LOCK_EX&&KEY_POW_ON&&(!KEY_POW_OFF))
	//if(KEY_LOCK_EX&&KEY_POW_ON)
	{
		OSTimeDlyHMSM(0,0,0,50);
		//if(KEY_LOCK_EX&&KEY_POW_ON&&!KEY_POW_OFF)
		//if(!KEY_POW_OFF)
		//if((KEY_LOCK_EX>0)&&(KEY_POW_ON>0))
		if(KEY_LOCK_EX&&!KEY_POW_OFF)
		//if(KEY_LOCK_EX&&KEY_POW_ON)
		{
		//	if(g_sysStart>0)
		//	{
			if(g_isRfModuleLinked>0)
			{
				UartSctSend(UART_ENCODE_EXIT, 1, 0);
				LCD_Clear(0);
				OSTimeDlyHMSM(0,0,0,50);
			}
		//	}
			//LCD_Clear(0);
			//LCD_WRITE_CMD(0XAF);
			//DisplayOff();
			//SavePara();
			PortUninit();
		
			LowPow_Init(1);
			//delay_init(72);
		//	g_sysStart = 1;
			//iap_load_app(FLASH_APP1_ADDR);
			PortInit();
			GolbalParaInit();
			//uart2_init(36,57600);
			uart2_init(36,115200);
			Adc_Init();
			LCD_Init();
			LCD_Clear(0);
			CheckBattery();
			Mode();
			//delay_ms(500);	//20150813 for debug
#if(defined(TTL_DEBUG)||defined(MOTO_TEST)||defined(EXT_TEST_PRT)||defined(FIBER_ADJUST_TEST))
			uart1_init(72,115200);
#endif
			//GetZoomAdc();
			delay_ms(300);
			if(KEY_POW_OFF>0)
			//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
			{
				RfMoudleSet();
			}
			OSTimeDlyHMSM(0,0,0,100);
			//RfMoudleSet();
			//g_keyDir = 0;
			g_shadowKey = 0;
			MotoReset();
			//g_flaEnalbe = 1;
			//Mode();
		}
	}

}

#define TEMP_WARN_ALERT 	200
#define TEMP_CANCEL_ALERT	180

void TempCheck(void)
{
	static u8 cnt=0;
	u8 tmp;
	static u8 tempWarn=0;
	static u32 tempAdSum=0;

	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_TEMP;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&(1<<1)));

	//g_tempAdBuf[cnt++] = (ADC1->DR>>4);
	tempAdSum += (ADC1->DR>>4);
	cnt++;
	//if(cnt++>16)
	if(cnt>=4)
	{
		cnt = 0;
		//ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
		//ADC1->SQR3|=AD_CHANNEL_TEMP;
		//ADC1->CR2|=1<<22;
		//while(!(ADC1->SR&(1<<1)));

		//tmp = ADC1->DR>>4;
		tmp = tempAdSum/4;
		tempAdSum = 0;
		//if(tmp>140)
		if(tmp>TEMP_WARN_ALERT)
		{
			if(!tempWarn)
			{
				SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
				BK_GREEN(1);
			}
			tempWarn = 1;
		}//else if(tmp<70)
		else if(tmp<TEMP_CANCEL_ALERT)
		{
			if(tempWarn>0)
			{
				tempWarn = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
				BkLiteOn();
				//ENABLE_CHARGE();
			}
		}
		//DisplayFocLen(*motoSet.mode,tmp,DSP_SELE); 
		if(tempWarn>0)
		{
			//CHARGE_LED1_TOGGLE;
			//TOGGLE_CHARGE();
			//g_isHotProtect = 1;
			g_isHotProtect |= HOT_WARN_TEMPERATURE;
		}else 
		{
			g_isHotProtect &= ~HOT_WARN_TEMPERATURE;
		}
	}
}

void FlashTimesCheck(void)
{
	//static u16 expDsp=0;
	if (g_isHotProtect & HOT_WARN_TIMES)
	{
		g_expAccHotWarnValue --;
		if(g_expAccHotWarnValue<=0)
		{
			g_isHotProtect ^= HOT_WARN_TIMES;
			g_expAccHotWarnValue = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			BkLiteOn();
		}
	}else
	{
		if(g_expAccHotWarnValue>EXP_ACC_VALUE_WARN_LEVEL)
		{
			g_expAccHotWarnValue = 20*60*15;           //run as timer
			g_isHotProtect |= HOT_WARN_TIMES;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			BkLiteOn();
		}
	}
	//if(expDsp<g_expAccHotWarnValue)
	//{
	//	DspID(0,DSP_NORMAL,g_expAccHotWarnValue);	
	//	expDsp = g_expAccHotWarnValue;
	//}
}


//u8 g_tempAdBuf[16];
void TempMonitorProc(void)
{
/*
	static u8 cnt=0;
	u8 tmp;
	static u8 tempWarn=0;
	static u32 tempAdSum=0;

	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_TEMP;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&(1<<1)));

	//g_tempAdBuf[cnt++] = (ADC1->DR>>4);
	tempAdSum += (ADC1->DR>>4);
	cnt++;
	//if(cnt++>16)
	if(cnt>=4)
	{
		cnt = 0;
		//ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
		//ADC1->SQR3|=AD_CHANNEL_TEMP;
		//ADC1->CR2|=1<<22;
		//while(!(ADC1->SR&(1<<1)));

		//tmp = ADC1->DR>>4;
		tmp = tempAdSum/4;
		tempAdSum = 0;
		//if(tmp>140)
		if(tmp>TEMP_WARN_ALERT)
		{
			if(!tempWarn)
			{
				SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
				BK_GREEN(1);
			}
			tempWarn = 1;
		}//else if(tmp<70)
		else if(tmp<TEMP_CANCEL_ALERT)
		{
			if(tempWarn>0)
			{
				tempWarn = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
				BkLiteOn();
				//ENABLE_CHARGE();
			}
		}
		//DisplayFocLen(*motoSet.mode,tmp,DSP_SELE); 
		if(tempWarn>0)
		{
			//CHARGE_LED1_TOGGLE;
			//TOGGLE_CHARGE();
			//g_isHotProtect = 1;
			g_isHotProtect |= HOT_WARN_TEMPERATURE;
		}else 
		{
			g_isHotProtect &= ~HOT_WARN_TEMPERATURE;
		}
	}
*/
	TempCheck();
	FlashTimesCheck();
	
	
}

void LightSlaveToggle(void)
{
	static u8 togTmr=0;
	static u8 togflag=0;
	if((CMD_MODE_LIGH_SLA==*g_cmdMode)||(CMD_MODE_RF_SLA==*g_cmdMode))
	{
		if(togTmr<15)
		{
			togTmr++;
		}else if(togTmr<16)
		{
			AF_LED1 = 1;
			togflag = 1;
			togTmr++;
		}else 
		{
			AF_LED1 = 0;
			togTmr = 0;
			togflag = 0;
		}
	}else
	{
		if(togflag>0)
		{
			AF_LED1 = 0;
			togTmr = 0;
			togflag = 0;
		}
	}

}

void TaskVoltageCheck(void *pdata)
{
	//u8 cnt=0;
	while(1)
	{
		ChkVoltage();
		OSTimeDlyHMSM(0,0,0,10);

/*
		cnt++;
		if(cnt>20)
		{
			cnt=0;
			FLAOK_TOGGLE();
		}
*/
	}

}

//#define _DEBUG_INDEX_
//#define _NO_SW_TEST_
//#define _LCD_SER_TEST_
void TaskMonitor(void *pdata)
{

	while(1)
	{

#ifdef _LCD_SER_TEST_
	//*wlSet.arr_mode = WLM_MODE_RES;
	//ModeSw();e
	//Mode(mainPara.workMode);
	//Draw_Circle(100,100,50);
	//FLA_OK_LED=!FLA_OK_LED;
//#ifdef NBK600EX_DEBUG
//	GREEN_LED=!GREEN_LED;
//#else
	//CHARGE_LED2=!CHARGE_LED2;
	//CHARGE_LED1=!CHARGE_LED1;
	//CR_READY=!CR_READY;
//#endif
	OSTimeDlyHMSM(0,0,0,300);

#endif

/*
	if(g_cc2530Start>0)
	{
		RfParaSend();
		g_cc2530Start = 0;
	}
*/
//	ChkVoltage();


#ifdef _MOTO_
if(KEY_POW_OFF>0)
//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
{
	if(g_motoInvalidCounter>0)
	{
		g_motoInvalidCounter--;
	}else
	{
		MotoLocation();                            //delete for moto debug 2015/10/10
	}
}
#endif

	TempMonitorProc();

#ifdef FIBER_ADJUST_PROC
	if((g_SettingIndex>=SETTING_FB_EXP)&&(g_SettingIndex<=SETTING_FIBER))
	{
		RgbCheckProc();
	}else
	{
		SensorMonitor();
	}
#else
		SensorMonitor();
#endif
	//RgbTest();

	LightSlaveToggle();

	SleepTimerProc();

#ifdef SIGNAL_TEST
	SIGNAL_PIN1 = !SIGNAL_PIN1;
	SIGNAL_PIN2 = !SIGNAL_PIN2;
#endif

#ifndef _NO_SW_TEST_

	PowerDownProc();

#if 0
		if(pSnrSet[SNRMENU_STBY]>0)
		{
			gWSleepCounter++;
			if(gWSleepCounter++>pSnrSet[SNRMENU_STBY]*60*200)
			{
				//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));
				gWSleepCounter=0;
				LowPow_Init();
			}
		}
#endif

#if 0
		//CR_READY=!CR_READY;
		if(KEY_LOCK_EX&&KEY_POW_ON&&!KEY_POW_OFF)
		{
			OSTimeDlyHMSM(0,0,0,50);
			if(KEY_LOCK_EX&&KEY_POW_ON&&!KEY_POW_OFF)
			{
				//mainPara.sumVeri = 0;
				//pMain = (char *)&mainPara;
				//for(index=0;index<sizeof(mainPara)-1;index++)
				//for(index=0;index<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode));index++)
				//{
				//	mainPara.sumVeri += pMain[index];
				//}
				//if(pSnrSet[SNRMENU_ATSAVE])
				//{
				//	mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
					//STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),sizeof(mainPara)/2);
				//	STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.sumVeri)-(char*)(&mainPara.workMode)+2)/2);
				//}
				//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));	
				//POW_ON = 0;						//here shut down the POWER
				//D3V3_EN = 1;
				//BOOST_EN = 0;
				//BOOST_EN = 1;
				//DISABLE_CHARGE;
				UartSctSend(UART_ENCODE_EXIT, 1, 0);
				OSTimeDlyHMSM(0,0,0,50);
				PortUninit();
				LowPow_Init();
				//delay_init(72);
				//iap_load_app(FLASH_APP1_ADDR);
				PortInit();
				//uart2_init(36,57600);
				Adc_Init();
				LCD_Init();
				MotoReset();
				Mode();
			}
		}
#endif
		
#endif	
		OSTimeDlyHMSM(0,0,0,50);	
	}
}

#if 0

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

#endif

void WlmCmdSet(void)
{
	//HpExpCodeSend(CMD_THRESHOLD_VAL);
	//HC_CON2 = 1;
	//HC_CON2 = 0;
	//HC_CON2 = 1;
	//HC_CON2 = 1;
#if 0
	HC_CON2 = 1;
	HC_CON1 = 0;
	HC_CON3 = 1;
#endif
	//HC_CON3 = 1;
	//HC_CON3 = 0;
	//HpExpCodeSend(0x0200);
	//HpExpCodeSend(0x0400);
	HpExpCodeSend(HP_CMD_LEVEL<<4);
}

#ifdef FLASH_BASE_FIBER

void WlmFlaADSet(s8 para1, s8 para2)
{
	u8 tmp=0;

#if 0
	if(para1>FLA_FBR_MAX)
	{
		HC_CON3 = 1;
	}else
	{
		HC_CON3 = 0;
		if(para1>FLA_FBR_MCR)
		{
			FLASH_AMP = 1;
		}else
		{
			FLASH_AMP = 0;
		}
	}

	HpExpCodeSend(g_flaExpCode[(para1+3)*3+para2]<<4);
	
#else

	if(20==*motoSet.dest_location)
	{
		tmp = (para1+3)*3+para2>0?((para1+3)*3+para2-1):0;
	}else
	{
		tmp = (para1+3)*3+para2;
	}

	if(tmp>(FLA_FBR_MAX+3)*3+2)
	{
		HC_CON3 = 1;
	}else
	{
		HC_CON3 = 0;
		if(tmp>(FLA_FBR_MCR+3)*3+2)
		{
			FLASH_AMP = 1;
		}else
		{
			FLASH_AMP = 0;
		}
	}
	
	//HpExpCodeSend(g_flaExpCode[(para1+3)*3+para2]<<4);
	HpExpCodeSend(g_flaExpCode[tmp]<<4);

#endif

	delay_us(20);
}

#endif

void WLM_SendChanhead(char chan)
{

	WlmCmdSet();

	delay_us(50);

	switch(chan)
	{
		case (WL_CHANNEL_ONE):
			//Delay_10us(5);
			//Delay_10us(WLM_CHAN1_WIDTH);
			//WLM_HoldOn(WLM_CHAN1_WIDTH);
			//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN1_WIDTH);
			//WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_CHAN1_WIDTH);
			WlmCmdHeadSend(WLM_IMPLUS_TIME+WLM_CHAN1_WIDTH);
		break;

		case (WL_CHANNEL_TWO):
			//Delay_10us(9);
			//Delay_10us(WLM_CHAN2_WIDTH);
			//WLM_HoldOn(WLM_CHAN2_WIDTH);
			//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN2_WIDTH);
			//WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_CHAN2_WIDTH);
			WlmCmdHeadSend(WLM_IMPLUS_TIME+WLM_CHAN2_WIDTH);
		break;

		case (WL_CHANNEL_THREE):
			//Delay_10us(16);
			//Delay_10us(WLM_CHAN3_WIDTH);
			//WLM_HoldOn(WLM_CHAN3_WIDTH);
			//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN3_WIDTH);
			//WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_CHAN3_WIDTH);
			WlmCmdHeadSend(WLM_IMPLUS_TIME+WLM_CHAN3_WIDTH);
		break;

		case (WL_CHANNEL_FOUR):
			//Delay_10us(22);
			//Delay_10us(WLM_CHAN4_WIDTH);
			//WLM_HoldOn(WLM_CHAN4_WIDTH);
			//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN4_WIDTH);
			//WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_CHAN4_WIDTH);
			WlmCmdHeadSend(WLM_IMPLUS_TIME+WLM_CHAN4_WIDTH);
		break;
	}
	//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
	//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
	WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL-20);
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
//	if(g_isHotProtect>0)
//	{
//		return;
//	}

	ENABLE_D30EN_EX();

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
				//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL-20);
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
			//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL-20);
			//WLM_SHOT_ONCE(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
		}
	}
	DISABLE_D30EN_EX();
 
}
//TTL  para1==>exp1, para2==>exp2, para3==>exp3
//MANU  para1==>exp1, para2==>exp2, para3==>exp3
//MULTI  para1==>frequence, para2==>times, para3==>exp1

void FlashInResMode(signed char mode, signed char para1, signed char para2, unsigned char para3)
{
	//switch(mainPara.workMode)
	HpSetting hpSet;
//#ifndef FLASH_BASE_FIBER
	unsigned int flaTime=0;
//#endif

	u8 accValue=0;

	ENABLE_D30EN_EX();
	
	switch(mode)
	{
		case WORKINGMODE_TTL:
			if((1==pSnrSet[SNRMENU_MODE])&&(CMD_MODE_OFF==*g_cmdMode))
			{
				TTLModFlash(5000);
			}else
			{
				//if(flaPara.hpFlash>HP_FLASHMODE_OFF)
				if(para3>HP_FLASHMODE_OFF)
				{
					//HpExpCalc(g_expTTLPara1,g_expTTLPara2,0,0,0,&hpSetting,0);	
					HpExpCalc(para1,para2,0,0,0,&hpSet,0);
					//HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);
					HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
				}else
				{
					//pTime = CanOutputSwitch(g_expTTLPara1,g_expTTLPara2);
					//flaTime = CanOutputSwitch(para1,para2);
#ifdef FLASH_BASE_FIBER
					flaTime = CanOutputSwitch(para1,para2);
					WlmFlaADSet(para1,para2);
					TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
#else
					//TURN_ON_TRIGER(pTime);
					flaTime = CanOutputSwitch(para1,para2);
					TURN_ON_TRIGER(flaTime);
#endif
				}
			}
			if (para1 >= 0)
			{
				accValue = para1*3 + para2;
			}else
			{
				accValue = 0;
			}
		break;

		case WORKINGMODE_MANU:
			//if(flaPara.hpFlash>HP_FLASHMODE_OFF)
			if(para3>HP_FLASHMODE_OFF)
			{
				//HpExpCalc(mstPara->output1,mstPara->output2,0,0,0,&hpSetting,0);
				HpExpCalc(para1,para2,0,0,0,&hpSet,0);
				//HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);
				HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
			}else
			{
				//pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
				//flaTime = CanOutputSwitch(para1,para2);
#ifdef FLASH_BASE_FIBER
				//flaTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
				flaTime = CanOutputSwitch(para1,para2);
				WlmFlaADSet(para1,para2);
				TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
#else
				//TURN_ON_TRIGER(pTime);
				flaTime = CanOutputSwitch(para1,para2);
				TURN_ON_TRIGER(flaTime);
#endif
			}
			if (para1 >= 0)
			{
				accValue = para1*3 + para2;
			}
			else
			{
				accValue = 0;
			}
		break;

		case WORKINGMODE_MULTI:
			//flaTime = CanOutputSwitch(para3,0);
			//flaTime = CanOutputSwitch(para1,0);
#ifdef FLASH_BASE_FIBER
			flaTime = CanOutputSwitch(para1,0);
			//WlmFlaADSet(para1,para2);
			WlmFlaADSet(para1,0);
			TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
#else
			flaTime = CanOutputSwitch(para1,0);
			TURN_ON_TRIGER(flaTime);
#endif
			SetMultiFlashTimer();
			accValue = para1*3;
		break;

		case WORKINGMODE_EXTA:
			//ExtaModSet(motoSet.focIndex,flaPara.isoCmp,menuPara.cApert,mstPara->cmp1,mstPara->cmp2,
						//flaPara.ttl_FEB1,flaPara.ttl_FEB2,flaPara.FEB_count);
			accValue = ExtaModSet(GetMotoStdIndex(*motoSet.dest_location),flaPara.isoCmp,menuPara.cApert,mstPara->cmp1,mstPara->cmp2,
						flaPara.ttl_FEB1,flaPara.ttl_FEB2,flaPara.FEB_count);
			ENABLE_D30EN_EX();
			ExtModFlash(5000);
			DISABLE_D30EN_EX();
		break;

		case WORKINGMODE_EXTM:
			//ExtmModSet(motoSet.focIndex,mainPara.extIso,mainPara.extApert);
			accValue = ExtmModSet(GetMotoStdIndex(*motoSet.dest_location),mainPara.extIso,mainPara.extApert);
			ENABLE_D30EN_EX();
			ExtModFlash(5000);
			DISABLE_D30EN_EX();
		break;

		case WORKINGMODE_GR:
		break;

	
	}

	//g_expAccHotWarnValue += (2^(accValue/3));
	g_expAccHotWarnValue += (1<<(accValue/3));
	gWSleepCounter = 0;

	DISABLE_D30EN_EX();
}

void LightSlaveFlash(void)
{
	switch(wlSlave.s_wlMode)
	{
		case WORKINGMODE_TTL:
			//FLAOK_TOGGLE();
#ifdef TTL_DEBUG	
			//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+8,((g_expTTLPara1<<4)|g_expTTLPara2),DSP_NORMAL);
#endif
			FlashInResMode(WORKINGMODE_TTL,g_expTTLPara1,g_expTTLPara2,canWireless.HpSyn);
			//FlashInResMode(WORKINGMODE_MANU, 3,0, 0);
		break;

		case WORKINGMODE_MANU:
			FlashInResMode(WORKINGMODE_MANU,canWireless.WL_output1,canWireless.WL_output2,canWireless.HpSyn);
		break;

		case WORKINGMODE_MULTI:
			//FLAOK_TOGGLE();
			SlaveMultiFlashPrepear();
			FlashInResMode(WORKINGMODE_MULTI,canWireless.wlMul_expose,0,0);
			//SetMultiFlashTimer();
		break;
	}

	gWSleepCounter = 0;
}


int MultiZero(unsigned char mExp)
{
	int ret;
	switch(mExp)
	{
		case 0:
		case 1:
			ret = 40;
		break;

		case 2:
			ret = 20;
		break;

		case 3:
			ret = 12;
		break;

		case 4:
			ret = 8;
		break;

		case 5:
			ret = 4;
		break;

		case 6:
			ret = 2;
		break;

		default:
			ret = 1;
		break;
	}

	return ret;

}



void MultiFlashPrepear(void)
{
	if(!*multiSet.eTimes)
	{
#ifdef _CANON_
		multiSet.finalExpTimes = MultiZero(*multiSet.expose);
#else
		multiSet.finalExpTimes = MultiZero(*multiSet.expose>>4);
#endif
	}else
	{
		multiSet.finalExpTimes = *multiSet.eTimes;
	}

	multiSet.finalExpTimes--;

	multiSet.lowFrqCnt = (10000/(*multiSet.frequence));
					
	//multiSet.mulCtl=MULTI_FLASH_ENABLE;
	//multiSet.finalExpTimes--;
}

void SlaveMultiFlashPrepear(void)
{
	if(!canWireless.wlMul_time)
	{
		multiSet.finalExpTimes = MultiZero(canWireless.wlMul_expose);
	}else
	{
		multiSet.finalExpTimes = canWireless.wlMul_time;
	}

	multiSet.finalExpTimes--;

	multiSet.lowFrqCnt = (10000/canWireless.wlMul_frq);

	//multiSet.finalExpTimes--;
}

//u8 slvTmsDebug = 0;
void RfSlaveMultiFlashPrepear(void)
{
/*
	if(!canWireless.wlMul_time)
	{
		multiSet.finalExpTimes = MultiZero(canWireless.wlMul_expose);
	}else
	{
		multiSet.finalExpTimes = canWireless.wlMul_time;
	}
*/
	if(!canWireless.wlMul_time)
	{
		canWireless.wlMul_time = MultiZero(canWireless.wlMul_expose);
	}

	//MulTimeRev();
	multiSet.finalExpTimes = RfMulTimeCalc(20);

	//slvTmsDebug = multiSet.finalExpTimes;

	multiSet.finalExpTimes--;

	multiSet.lowFrqCnt = (10000/canWireless.wlMul_frq);
}


void SetMultiFlashTimer(void)
{
	if(multiSet.finalExpTimes>0)
	{
		g_tmHandleFlag = TM_HANDLE_MULTIFLASH;	
		Timer4_Init(multiSet.lowFrqCnt, 7200);
		//Timer4_Init(100,72);
	}
}

void LightSlaveMultiFlash(void)
{
	static int mulCnt=0;
//#ifndef FLASH_BASE_FIBER
	unsigned int flaTime=0;
//#endif

	if(canWireless.wlMul_time>0)
	{
		mulCnt++;
		if(mulCnt>=multiSet.lowFrqCnt)
		{
			ENABLE_D30EN();
			//flaTime = CanOutputSwitch(canWireless.wlMul_expose, 0);
#ifdef FLASH_BASE_FIBER
			flaTime = CanOutputSwitch(canWireless.wlMul_expose, 0);
			WlmFlaADSet(canWireless.wlMul_expose, 0);
			TURN_ON_TRIGER(canWireless.wlMul_expose>FLA_FBR_MAX?flaTime:OUTPUT_MAX);
#else
			flaTime = CanOutputSwitch(canWireless.wlMul_expose, 0);
			TURN_ON_TRIGER(flaTime);
#endif
			mulCnt = 0;

			multiSet.finalExpTimes--;
			if(multiSet.finalExpTimes<=0)
			{
				g_tmHandleFlag = TM_HANDLE_CLR;
				multiSet.mulCtl=MULTI_FLASH_DISABLE;	
				
			}else
			{
				//Timer4_Init(100,72);
				Timer4_Init(multiSet.lowFrqCnt,7200);
				g_tmHandleFlag = TM_HANDLE_MULTIFLASH;
			}
			
		}
	}
}

#if 0
void TakeMultiFlash(u8* flaTms, s8 exp,int frqCnt, u8 lkSignal)
{
	//static int mulCnt=0;
//#ifndef FLASH_BASE_FIBER
	unsigned int flaTime=0;
//#endif
	//if(multiSet.finalExpTimes>0)
	if((*flaTms)>0)
	{
		g_mulCnt++;
		//if(mulCnt>=multiSet.lowFrqCnt)
		if(g_mulCnt>=frqCnt)
		{
			if(lkSignal>0)
			{
				SPI_MISO_AUX = 0;
			}
			//DISABLE_D30EN();
			ENABLE_D30EN_EX();
			//flaTime = CanOutputSwitch(*multiSet.expose, 0);
			//flaTime = CanOutputSwitch(exp,0);

#ifdef FLASH_BASE_FIBER
			flaTime = CanOutputSwitch(exp,0);
			WlmFlaADSet(exp,0);
			TURN_ON_TRIGER(exp>FLA_FBR_MAX?flaTime:OUTPUT_MAX);
#else
			flaTime = CanOutputSwitch(exp,0);
			TURN_ON_TRIGER(flaTime);
#endif
			g_mulCnt = 0;

			DISABLE_D30EN_EX();

			if(lkSignal>0)
			{
				SPI_MISO_AUX = 1;
			}

			//multiSet.finalExpTimes--;
			(*flaTms)--;
			//if(multiSet.finalExpTimes<=0)
			if((*flaTms)<=0)		//error without point
			{
				g_tmHandleFlag = TM_HANDLE_CLR;
				multiSet.mulCtl=MULTI_FLASH_DISABLE;	
				
			}else
			{
				Timer4_Init(100,72);
				g_tmHandleFlag = TM_HANDLE_MULTIFLASH;
			}
		}else
		{
			Timer4_Init(100,72);
			g_tmHandleFlag = TM_HANDLE_MULTIFLASH;
		}
	}else
	{
		g_tmHandleFlag = TM_HANDLE_CLR;
		multiSet.mulCtl=MULTI_FLASH_DISABLE;	
		g_mulCnt = 0;
		(*flaTms) = 0;
		//multiSet.finalExpTimes = 0;
	}
}
#else
void TakeMultiFlash(u8* flaTms, s8 exp,int frqCnt, u8 lkSignal)
{
	unsigned int flaTime=0;
	if((*flaTms)>0)
	{
		if(lkSignal>0)
		{
			SPI_MISO_AUX = 0;
		}
		ENABLE_D30EN_EX();
#ifdef FLASH_BASE_FIBER
		flaTime = CanOutputSwitch(exp,0);
		WlmFlaADSet(exp,0);
		TURN_ON_TRIGER(exp>FLA_FBR_MAX?flaTime:OUTPUT_MAX);
#else
		flaTime = CanOutputSwitch(exp,0);
		TURN_ON_TRIGER(flaTime);
#endif
		DISABLE_D30EN_EX();
		if(lkSignal>0)
		{
			SPI_MISO_AUX = 1;
		}
		(*flaTms)--;
		if((*flaTms)<=0)		//error without point
		{
			g_tmHandleFlag = TM_HANDLE_CLR;
			multiSet.mulCtl=MULTI_FLASH_DISABLE;	
			
		}else
		{
			Timer4_Init(multiSet.lowFrqCnt,7200);
			g_tmHandleFlag = TM_HANDLE_MULTIFLASH;
		}
	}else
	{
		g_tmHandleFlag = TM_HANDLE_CLR;
		multiSet.mulCtl=MULTI_FLASH_DISABLE;	
		(*flaTms) = 0;
	}
}
#endif

void StopMultiFlash(void)
{
	g_tmHandleFlag = TM_HANDLE_CLR;
	multiSet.finalExpTimes = 0;
	//g_mulCnt = 0;
	DISABLE_D30EN();
	DISABLE_D30EN_EX();
	//StartCapture();
	//FLAOK_OFF();	
}


void TtlFebFlashUpdate(void)
{
	if((flaPara.ttl_FEB1>0)||(flaPara.ttl_FEB2>0))
	{
		flaPara.FEB_count++;	
		if(flaPara.FEB_count>2)
		{
			if(pSnrSet[SNRMENU_FEBCANCEL]>0)
			{
				flaPara.FEB_count = 0;
			}else
			{
				flaPara.ttl_FEB1 = 0;
				flaPara.ttl_FEB2 = 0;
				flaPara.FEB_count = 0;
				menuPara.feb = 0;                   // 20170831
			}
		}
	}
}

void TakeFlash(void)
{
	u8  expRt=0;
	
	SPI_MISO_AUX = 0;

	//if(g_isHotProtect>0)
	//{
	//	return;
	//}
	
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			FlashInResMode(WORKINGMODE_TTL,g_expTTLPara1,g_expTTLPara2,flaPara.hpFlash);
			TtlFebFlashUpdate();
			expRt = ttl_expCodeRsv;
			FlashOkLedTwinkle();
		break;

		case WORKINGMODE_MANU:
			FlashInResMode(WORKINGMODE_MANU,mstPara->output1,mstPara->output2,flaPara.hpFlash);
			expRt = RfMcodeCovert(menuPara.otpCod[MASTER],flaPara.hpFlash);
		break;

		case WORKINGMODE_MULTI:
			MultiFlashPrepear();
#ifdef _CANON_
			//FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose,0,0);
			FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose,0,0);
#else
			FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose>>4,0,0);
#endif
			//SetMultiFlashTimer();
			expRt = RfMcodeCovert(menuPara.WlmMul[WL_ARRARY_A],flaPara.hpFlash);
		break;

		case WORKINGMODE_GR:
			if((*wlSet.is_masterOn>0)\
				&&(!radioSet->rfGr[0].isOff))
			{
				switch(radioSet->rfGr[0].mode)
				{
					//case WORKINGMODE_TTL:
					case GRMOD_TTL:
					FlashInResMode(WORKINGMODE_TTL, g_expTTLPara1, g_expTTLPara2, flaPara.hpFlash);
					FlashOkLedTwinkle();
					break;

					//case WORKINGMODE_MANU:
					case GRMOD_M:
					FlashInResMode(WORKINGMODE_MANU, radioSet->rfGr[MASTER].m_exp1, radioSet->rfGr[MASTER].m_exp2, flaPara.hpFlash);
					break;

					case GRMOD_EXTA:
					FlashInResMode(WORKINGMODE_EXTA,0,0,0);
					FlashOkLedTwinkle();
					break;
				}
			}
			TtlFebFlashUpdate();
		break;

		case WORKINGMODE_EXTA:
			//ExtaModSet(motoSet.focIndex,flaPara.isoCmp,menuPara.cApert);
			//ENABLE_D30EN_EX();
			//ExtModFlash(5000);
			//DISABLE_D30EN_EX();
			FlashInResMode(WORKINGMODE_EXTA,0,0,0);
			TtlFebFlashUpdate();
			FlashOkLedTwinkle();
		break;

		case WORKINGMODE_EXTM:
			//ExtmModSet(motoSet.focIndex,mainPara.extIso,mainPara.extApert);
			//ENABLE_D30EN_EX();
			//ExtModFlash(5000);
			//DISABLE_D30EN_EX();
			FlashInResMode(WORKINGMODE_EXTM,0,0,0);
			FlashOkLedTwinkle();
		break;
	}

	ttl_expCodeRsv = expRt;
	g_colorTempRt = g_colorTemperature[21];
	gWSleepCounter = 0;
	g_motoInvalidCounter = MOTO_INVALID_CNT;
	if((!g_isRgbFilter)&&(!pfnSet[PFN_ID_DETEC]))
	{
		if(1==g_rgbStyle)
		{
			//g_colorTempRt -= 0x7000000;
			g_colorTempRt -= 0x115b0bc9;
			//g_colorTempRt -= 0x46300CA;
		}else
		{
			//g_colorTempRt -= 0x3000000;
			//g_colorTempRt -= 0x115b0bc9;
			g_colorTempRt -= 0x46300CA;
			
		}
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);

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

	//SPIx_Disable();
	SET_MISO_FREE();
	
	if(*wlSet.is_masterOn>0)
	{
		SPI_MISO_AUX = 0;
		
		if((0==flaPara.preflashIndex)||(1==flaPara.preflashIndex))
		{
			HpPreflash(HP_PREFLASH_INTERVAL);
			delay_us(380);
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

	//delay_us(100);
#ifdef EOS_750D
	if(CLK_IN>0)
	{
		if(COM_STATUE_IDLE==g_comStatue)
		{
			EXTIx_Init(GPIO_A,FTIR);
			SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
			g_cmrLinked = 1;
			g_comStatue = COM_STATUE_START;	
		}
	}else
	{
		EXTIx_Init(GPIO_A,RTIR);
		g_comStatue = COM_STATUE_IDLE;
	}
#else											//2015/09/03 for 750d
	SPIx_Init();
	//SPI1->DR = 0x80;
	SPI1->DR = 0xFF;
	g_comStatue = COM_STATUE_START;
#endif

	DISABLE_D30EN();

}

void FlashFinishProc(void)
{
//	INT8U tmrErr;
	
	DISABLE_D30EN();
	SPIx_Disable();
	Ex_NVIC_Config(GPIO_A,5,RTIR);
	Ex_NVIC_Enable(5);
	g_comStatue = COM_STATUE_IDLE;
	g_dspUpdateFlag |= DSP_UPDATE_SHOT;
	SPI_MISO_AUX = 1;

	if((WORKINGMODE_GR==mainPara.workMode)&&(VER_CAMERA_OLD==g_vCmr))
	{
		*wlSet.ttl_Ratio = WLM_MODE_ALL;
		mainPara.workMode = WORKINGMODE_TTL;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
	//FLAOK_ON();
	//OSTmrStart(g_1ShotTmr, &tmrErr);	
}

void FlashOkLedTwinkle(void)
{
	INT8U tmrErr;
	
	FLAOK_ON();
	OSTmrStart(g_1ShotTmr, &tmrErr);	
}


void WLMCmdSend(u8 mod)
{
	switch(mod)
	{
		case CMD_MODE_LIGH_MST:
		switch(wlSet.m_cmdProc)
		{
			case WLM_CMDPROC_DAT:
			#if 1
				//Delayus(15);
				SPI_MISO_AUX = 0;
				delay_us(150);		
				OS_ENTER_CRITICAL();
				WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
				delay_us(100);		
				OS_EXIT_CRITICAL();
				//delay_us(400);
				delay_us(800);                                    // 2016/07/25 
			#endif
				//disable_irq();	
				//MEMCTR   |=  0x04;
				//IRQ0ICR &= 0XFE;
				//IRQ1ICR &= 0XFE;
				//MEMCTR   &= ~0x04;
				//enable_irq();
				//DISABLE_D30EN();				//2015/7/4 IGBT KILLED
				if((*wlSet.is_masterOn>0)&&(1==g_mstPrefla))
				{
					//FLAOK_TOGGLE();
					SetPreFlashDA();	
				}
				wlSet.m_cmdProc = WLM_CMDPROC_OFF;	
				SPI_MISO_AUX = 1;
				//FLASH_OUT = 1;
			break;

			case WLM_CMDPROC_FLA:				
				//if((*wlSet.is_masterOn>0)&&(1==g_mstPrefla))
				if(*wlSet.is_masterOn>0)
				{
					//pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
					//TURN_ON_TRIGER(pTime);
					//FlashInResMode();
					TakeFlash();
				}else
				{
					SPI_MISO_AUX = 0;
					ENABLE_D30EN_EX();
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
					WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
					DISABLE_D30EN_EX();
					SPI_MISO_AUX = 1;
				}
				DISABLE_D30EN();
				SPIx_Disable();
				Ex_NVIC_Config(GPIO_A,5,RTIR);
				Ex_NVIC_Enable(5);
				g_comStatue = COM_STATUE_IDLE;
				g_dspUpdateFlag |= DSP_UPDATE_SHOT;
				wlSet.m_cmdProc = WLM_CMDPROC_OFF;					
			break;

			//else if(WLM_CMDPROC_MUL==wlSet.m_cmdProc)
			case WLM_CMDPROC_MUL:
			//if((!BP_IN)||(MULTI_FLASH_ENABLE==multiSet.mulCtl))
			//FLAOK_ON();
			if(((!BP_IN)||(!PILOT_IN))&&(g_chargeOk>0))
			{
				 //SPI_MISO_AUX = 0;
				TakeMultiFlash(&multiSet.finalExpTimes,*multiSet.expose,multiSet.lowFrqCnt,1);
				 //SPI_MISO_AUX = 1;
			}else
			{
				StopMultiFlash();
				//FlashFinishProc();
				SPI_MISO_AUX = 1;
			}
			break;
			//FLAOK_OFF();
				

			case WLM_CMDPROC_PRF:
			if((WLM_MASTER_ON==*wlSet.is_masterOn)&&(1==g_mstPrefla))
			{
				//HpExpCalc(4,0,0,0,0,&hpSetting,0);
				//HighSpeedTriger(&hpSetting,HP_PREFLASH_INTERVAL);			//preflash
				//TrigerMasterOn(0);
				//SPI_MISO_AUX  = 0;
				//ENABLE_D30EN_EX();
				//SetPreFlashDA();	
				//SetPreFlashDA();	
				HpPreflash(HP_PREFLASH_INTERVAL);
				WlmCmdSet();
				//TURN_ON_TRIGER(200);
				//DISABLE_D30EN_EX();
				//SPI_MISO_AUX = 1;
				//SC4MD1 |= 0X30;	
			}else
			{
				//FLASH_OUT = 0;
				ENABLE_D30EN_EX();
				//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				DISABLE_D30EN_EX();
				//FLASH_OUT = 0;
				SPI_MISO_AUX  = 0;
				delay_us(40);
				SPI_MISO_AUX  = 1;
				//FLASH_OUT = 1;

			}
			DISABLE_D30EN();
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
						delay_ms(20);
						//FLASH_CONTROL = !FLASH_CONTROL;
						//SPI_MISO_AUX = 1;
						//FLASH_CONTROL = !FLASH_CONTROL;
						//delay_us(900);
						//delay_us(330);
						//RfCountDownInfo(7000);
						//delay_us(550);
						OS_ENTER_CRITICAL();
						if(0==radioStatue.delay_mod)
						{
							//delay_us(330);
							//delay_us(500);
							/*
							delay_us(1700);
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
							*/
							OS_EXIT_CRITICAL();
							
						}else if(1==radioStatue.delay_mod)
						{
							/*
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
							*/
							OS_EXIT_CRITICAL();
							
						}else if(2==radioStatue.delay_mod)
						{
							//delay_us(330);
							//delay_us(500);
							//delay_us(350);
							//delay_ms(1);
							/*
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
							*/
							OS_EXIT_CRITICAL();
							
						}else if(3==radioStatue.delay_mod)
						{
							OS_EXIT_CRITICAL();
						}

						
						/*
						SPI_MISO_AUX = 0;
						//delay_us(990);
						delay_us(900);
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
						delay_ms(6);              // 20170831
						//delay_ms(4);
						//delay_us(200);
						delay_us(500);           // 20170831
						g_comStatue = COM_STATUE_PREFLASH;	
						//Ex_NVIC_Enable(5);
						SPI_MISO_AUX = 1;
						//g_comStatue = COM_STATUE_PREFLASH;	
						//DEBUG_PIN1 = !DEBUG_PIN1;
					}
					if(*wlSet.is_masterOn>0)
					{
						SetPreFlashDA();	
					}
					SPI_MISO_AUX = 1;
					break;

					case RADIO_STATUE_PRE_FLA:				//for 7DII and 700D new
						//OS_ENTER_CRITICAL();
						if(0==radioStatue.delay_mod)
						{
							delay_us(300+200);
							RfCountDownInfo(7000);
							delay_ms(9);
							//delay_us(820);
							//delay_us(820-300);
							delay_us(820-300-100);
							RfMstPreflaProc();
							/*
							OS_ENTER_CRITICAL();
							SPI_MISO_AUX = 0;
							delay_us(890);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;*/
						}else if(1==radioStatue.delay_mod)
						{
							//delay_us(200);						//2015/10/29 cause 750d flash high
							RfCountDownInfo(6000);
							//delay_us(7540-160);
							delay_us(7540-160-100+1000+200);
							RfMstPreflaProc();
							/*
							OS_ENTER_CRITICAL();
							SPI_MISO_AUX = 0;
							//delay_us(990);
							//delay_us(900);
							delay_us(890);
							//delay_us(880);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;	*/						
						}else if(2==radioStatue.delay_mod)
						{
							RfCountDownInfo(3000);
							//delay_ms(5);
							//delay_us(4400);
							delay_us(4400-100+1150);
							RfMstPreflaProc();
							/*
							OS_ENTER_CRITICAL();
							//PreFlaPullUp();
							SPI_MISO_AUX = 0;
							//delay_us(990);
							//delay_us(900);
							delay_us(890);
							//delay_us(880);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;*/
						}
						else
						{
							//OS_ENTER_CRITICAL();
							RfCountDownInfo(4000);
							//delay_us(5500);
							//delay_us(5500-100);
							delay_us(6530);
							RfMstPreflaProc();
							/*
							OS_ENTER_CRITICAL();
							SPI_MISO_AUX = 0;
							delay_us(890);
							OS_EXIT_CRITICAL();
							delay_us(90);
							SPI_MISO_AUX = 1;*/
						}
						//OS_EXIT_CRITICAL();
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
						/*
						SPI_MISO_AUX = 0;
						delay_ms(1);
						SPI_MISO_AUX = 1;*/
						RfMstPreflaProc();
						//SPIx_Init();
						//g_comStatue = COM_STATUE_START;	
					break;

					case RADIO_STATUE_FLACFM:
					if(CMR_ADEPT_NEW==radioStatue.adept)
					{
						//delay_us(300);                                              // 2016/08/02
						RfFlashConfirm(8,FLASH_PILOT_OFF);
						//SIGNAL = !SIGNAL;
						//delay_us(24000);
						//delay_ms(20);
#ifdef UART1_TRACE_DEBUG
						printf("flacfm");
#endif
						delay_ms(20);				//2014/0928 (1DX + 红圈)兼容
						SPI_MISO_AUX = 1;
					}else if(CMR_ADEPT_OLD==radioStatue.adept)
					{
						//FLASH_CONTROL = !FLASH_CONTROL;
						delay_ms(3);
						RfFlashConfirm(2,FLASH_PILOT_OFF);
						delay_ms(3);
						SPI_MISO_AUX = 1;
					}
					DISABLE_D30EN();
					break;

				//	case RADIO_STATUE_FLASH:
					case RADIO_STATUE_FLAINFO:
					//SPI_MISO_AUX = 0;
					if(CMR_ADEPT_NEW==radioStatue.adept)
					{
						if(0==radioStatue.delay_mod)
						{
							RfCountDownInfo(6000);
						}else if(1==radioStatue.delay_mod)
						{
							//delay_us(500+120);
							if (flaPara.hpFlash > 0)
							{
								delay_us(500+120);                            // 2016/08/02 5d markIII 黑边
							}else
							{
								//delay_ms(2);
								delay_us(500+120+700);                    // 2016/08/06 5d markIII 黑边
							}
							RfCountDownInfo(7000);
						}else
						{
							RfCountDownInfo(6000);
						}
						//RfCountDownInfo(6000);
						if(*wlSet.is_masterOn>0)
						{
							if(HP_FLASHMODE_ON==flaPara.hpFlash)
							{	
								//delay_us(5000);
								delay_ms(8);
								TakeFlash();
								FlashFinishProc();
								
							}else
							{
								delay_us(10);
							}
							
						}else
						{
							SPIx_Disable();
							delay_us(10000-1070);//delay_us(10000);
							//Delay_ms(1);
							SPI_MISO_AUX = 0;
							delay_us(1000);
							SPI_MISO_AUX = 1;
							FlashFinishProc();
							//DISABLE_D30EN();
							//SPIx_Disable();
							//Ex_NVIC_Config(GPIO_A,5,RTIR);
							//Ex_NVIC_Enable(5);
							//g_comStatue = COM_STATUE_IDLE;
							//g_dspUpdateFlag |= DSP_UPDATE_SHOT;
						}
					}else if(CMR_ADEPT_OLD==radioStatue.adept)
					{
						//delay_ms(3);
						//RfCountDownInfo(0);	
						UART_INT = 1;
						U2SendData(0xAB);
						UART_INT = 0;
						//delay_ms(1);
						if(*wlSet.is_masterOn>0)
						{
							if(flaPara.hpFlash>0)
							{
								SPI_MISO_AUX = 0;
								TakeFlash();
								//delay_ms(1);			//2016/02/17 delete for multi flash
								SPI_MISO_AUX = 1;
								FlashFinishProc(); 
							}
						}
						//delay_ms(1);					//2016/02/17 delete for multi flash
						/*
						if(*wlSet.is_masterOn>0)			//2015/06/15 wait verified
						{
							SPI_MISO_AUX = 0;
							TakeFlash();
							delay_ms(1);
							SPI_MISO_AUX = 1;
							FlashFinishProc();
						}else
						{
							SPI_MISO_AUX = 0;
							delay_ms(1);
							SPI_MISO_AUX = 1;
							//SPI_MISO_AUX = 0;
							delay_us(1000);
							FlashFinishProc();
						}
						*/
						//RfMstPreflaProc();
						//SPI_MISO_AUX = 1;
					}
					//DISABLE_D30EN();
					//SPIx_Disable();
					//Ex_NVIC_Config(GPIO_A,5,RTIR);
					//Ex_NVIC_Enable(5);
					//g_comStatue = COM_STATUE_IDLE;
					//g_dspUpdateFlag |= DSP_UPDATE_SHOT;
					SPI_MISO_AUX = 1;
					break;

					case RADIO_STATUE_FLASH:
						if(CMR_ADEPT_NEW==radioStatue.adept)
						{
							if(*wlSet.is_masterOn>0)
							{
								 TakeFlash();
								 //delay_ms(5);
								 //delay_ms(1);                //2016/02/17 delete for multi flash//2016/02/17 delete for multi flash
							}
							FlashFinishProc();
						}else
						{
							SPI_MISO_AUX = 0;
							UART_INT = 1;
							U2SendData(0xAB);
							UART_INT = 0;

							if(*wlSet.is_masterOn>0)
							{								
								TakeFlash();
							}
							
							FlashFinishProc();
							//delay_ms(1);
						}
						//SPI_MISO_AUX = 1;
						 //g_comStatue = COM_STATUE_IDLE;
						// wlSet.m_cmdProc = WLM_CMDPROC_OFF;
						 //Ex_NVIC_Config(GPIO_A,5,RTIR);
						 //Ex_NVIC_Enable(5);
					break;

					case RADIO_STATUE_MULTIFLASH:
						if(((!BP_IN)||(!PILOT_IN))&&(g_chargeOk>0))
						{
							 //SPI_MISO_AUX = 0;
							TakeMultiFlash(&multiSet.finalExpTimes,*multiSet.expose,multiSet.lowFrqCnt,1);
							//SPI_MISO_AUX = 1;
						}else
						{
							StopMultiFlash();
							//FlashFinishProc();
							SPI_MISO_AUX = 1;
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
						//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
						UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, *g_cmdMode);
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							if(HP_FLASHMODE_ON==flaPara.hpFlash)
							{
								TakeFlash();
								SpiRearUp(100);
								Ex_NVIC_Config(GPIO_A,5,RTIR);
								Ex_NVIC_Enable(5);
								g_comStatue = COM_STATUE_IDLE;
							}
							
						}
						
						//delay_ms(1);
					break;

					case RADIO_STATUE_FLASH:
						//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
						UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, *g_cmdMode);
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							TakeFlash();
							 DISABLE_D30EN();
							 //delay_ms(5);
							// delay_ms(1);			//the delay cause the multi flash timeout, so the wlSet.m_cmdProc is overwrited, then the flash is interrupt
						}
						SPI_MISO_AUX = 1;
						 g_comStatue = COM_STATUE_IDLE;
						 //wlSet.m_cmdProc = WLM_CMDPROC_OFF;
						 Ex_NVIC_Config(GPIO_A,5,RTIR);
						 Ex_NVIC_Enable(5);
					break;

					case RADIO_STATUE_PREDAT:
						SetPreFlashDA();
					break;

					case RADIO_STATUE_PRE_FLA:
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							HpPreflash(HP_PREFLASH_INTERVAL);
							delay_us(100);
						}
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
				radioStatue.cmdReady=0;
			}
			//SpiRearUp(100);
		break;

		case CMD_MODE_RFLS_SLA:
			if(1==radioStatue.cmdReady)
			{
				switch(radioStatue.cmdSta)
				{
					case RADIO_STATUE_FLACFM:
						//EXTIx_Init(FTIR);
						EXTIx_Init(GPIO_A,FTIR);
						Ex_NVIC_Enable(5);
						g_comStatue = COM_STATUE_REALFLASH;
						SpiRearUp(100);
					break;

					case RADIO_STATUE_FLASH:
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							TakeFlash();
							 DISABLE_D30EN();
							 //delay_ms(5);
							// delay_ms(1);			//the delay cause the multi flash timeout, so the wlSet.m_cmdProc is overwrited, then the flash is interrupt
						}
						SPI_MISO_AUX = 1;
						 g_comStatue = COM_STATUE_IDLE;
						 //wlSet.m_cmdProc = WLM_CMDPROC_OFF;
						 Ex_NVIC_Config(GPIO_A,5,RTIR);
						 Ex_NVIC_Enable(5);
					break;

					//case RADIO_STATUE_FLASH:
					case RADIO_STATUE_FLAINFO:
						//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							if(HP_FLASHMODE_ON==flaPara.hpFlash)
							{
								TakeFlash();
								SpiRearUp(100);
								Ex_NVIC_Config(GPIO_A,5,RTIR);
								Ex_NVIC_Enable(5);
								g_comStatue = COM_STATUE_IDLE;
							}
							
						}
						//Ex_NVIC_Config(GPIO_A,5,RTIR);
						//Ex_NVIC_Enable(5);
						//g_comStatue = COM_STATUE_IDLE;
					break;

					case RADIO_STATUE_PREDAT:
						SetPreFlashDA();
						SPI_MISO_AUX = 1;
					break;

					case RADIO_STATUE_PRE_FLA:
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							HpPreflash(HP_PREFLASH_INTERVAL);
							delay_us(100);
						}
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
						/*
						if(pfnSet[PFN_ID_LSSW]>0)
						{
							HpPreflash(HP_PREFLASH_INTERVAL);
							delay_us(100);
							SPIx_Init();
							SPI1->DR = 0x80;
							g_getTtlFlashCode = 1;
							g_comStatue = COM_STATUE_START;
						}*/
						SPI_MISO_AUX = 1;
						g_comStatue = COM_STATUE_PREFLASH;	
					break;
				}
				radioStatue.cmdReady=0;
			}
		break;

		case CMD_MODE_RF_SLA:
			if(1==radioStatue.cmdReady)
			{
				if(RADIO_STATUE_SLAPRE==radioStatue.cmdSta)
				{
					//delay_us(650);
					//SetPreFlashDA();
					//OS_ENTER_CRITICAL();
					HpPreflash(HP_PREFLASH_INTERVAL);
					//OS_EXIT_CRITICAL();

					//delay_us(650);
					DISABLE_D30EN();

					g_comStatue = COM_STATUE_IDLE;
					EXTIx_Init(GPIO_A,RTIR);
					
				}else if(RADIO_STATUE_SLAFLA==radioStatue.cmdSta)
				{
					//delay_us(1700);			//2015/7/2
					
					if(WORKINGMODE_MULTI==mainPara.workMode)
					{
						RfSlaveMultiFlashPrepear();
					}
					FlashInResMode(mainPara.workMode,g_expTTLPara1,g_expTTLPara2,g_expTTLPara3);

					DISABLE_D30EN();
					
					g_comStatue = COM_STATUE_IDLE;
					EXTIx_Init(GPIO_A,RTIR);
					
				}else if(RADIO_STATUE_MULTIFLASH==radioStatue.cmdSta)
				{
					if((WORKINGMODE_MULTI==mainPara.workMode)&&(g_chargeOk>0))
					{
						//TakeMultiFlash(&canWireless.wlMul_time,*multiSet.expose,multiSet.lowFrqCnt);
						//TakeMultiFlash(&canWireless.wlMul_time,canWireless.wlMul_expose,multiSet.lowFrqCnt);
						TakeMultiFlash(&multiSet.finalExpTimes,canWireless.wlMul_expose,multiSet.lowFrqCnt,0);
					}else
					{
						StopMultiFlash();
					}

					g_comStatue = COM_STATUE_IDLE;	
					EXTIx_Init(GPIO_A,RTIR);
					
				}else if(RADIO_STATUE_PREDAT == radioStatue.cmdSta)			//set DA(mcp4725) value
				{
					SetPreFlashDA();
					//hpSetting = HP_EXP_50;
					g_comStatue = COM_STATUE_SLAPRE;
					EXTIx_Init(GPIO_B,FTIR);
				}
				//Ex_NVIC_Config(GPIO_A,5,RTIR); 
				//SET_INT5SRC_SCK();
				//EXTIx_Init(GPIO_A,RTIR);
				//g_comStatue = COM_STATUE_IDLE;
				radioStatue.cmdReady=0;
			}
		break;

		case CMD_MODE_LIGH_SLA:
			if(1==radioStatue.cmdReady)
			{
				//OS_ENTER_CRITICAL();
				if(RADIO_STATUE_PREDAT==radioStatue.cmdSta)
				{
					SetPreFlashDA();
				}
				else if(RADIO_STATUE_SLAPRE==radioStatue.cmdSta)
				{
					StopCapture();
#ifdef DBG_PRINTF
					DBG_PRINTF("PRE CH%d\r\n",canWireless.WL_Channel);
#endif
					if(!g_isIndividualSlave)
					{
#ifdef LITE_SLAVE_REVIEW
						if((1==canWireless.activeArr)&&(g_chargeOk>0)
						&&(*wlSet.m_chan==canWireless.WL_Channel))/*2015/11/05 for light slave flash unrelay on channel*/
						//if(1==canWireless.activeArr)
#else
						if((1==canWireless.activeArr)&&(g_chargeOk>0))
#endif
						{
							HpPreflash(HP_PREFLASH_INTERVAL);
						}else
						{
							delay_ms(2);
						}
					}else
					{
						delay_ms(2);
					}
					DISABLE_D30EN();
					LightSlaveParaClear();
					StartCapture();
					//delay_us(400);
				}else if(RADIO_STATUE_SLAFLA==radioStatue.cmdSta)
				{
					//FLAOK_TOGGLE();
					StopCapture();
					if(g_isIndividualSlave>0)
					{
						if(*wlSet.m_chan==canWireless.WL_Channel)
						{
							TakeFlash();
						}
					}else
					{
#ifdef LITE_SLAVE_REVIEW
						if((1==canWireless.activeArr)&&(g_chargeOk>0)
							&&(*wlSet.m_chan==canWireless.WL_Channel))
#else
						if((1==canWireless.activeArr)&&(g_chargeOk>0))
#endif						
						{
							//StopCapture();
#ifdef SLAVE_TEST
					              DBG_PRINTF("FLA %d\r\n",canWireless.WL_Channel);
#endif
							LightSlaveFlash();
							//if(wlSlave.s_wlMode!=WORKINGMODE_MULTI)
							//{
								//delay_ms(10);
							//}
						}else
						{
#ifdef SLAVE_TEST
							DBG_PRINTF("FLAxx %d\r\n",canWireless.WL_Channel);
#endif
							delay_ms(10);
						}
					}
					//delay_us(400);				//this delay will cause multiflash interrupt
					DISABLE_D30EN();
					LightSlaveParaClear();
					StartCapture();
					//delay_us(400);
				}else if(RADIO_STATUE_MULTIFLASH==radioStatue.cmdSta)
				{
					//LightSlaveFlash();
					//FLAOK_TOGGLE();
					if((WORKINGMODE_MULTI==mainPara.workMode)&&(g_chargeOk>0))
					//if(g_chargeOk>0)
					{
						StopCapture();
						if(g_isIndividualSlave>0)
						{
							TakeMultiFlash(&multiSet.finalExpTimes,*multiSet.expose,multiSet.lowFrqCnt,0);
						}else
						{
							TakeMultiFlash(&multiSet.finalExpTimes,canWireless.wlMul_expose,multiSet.lowFrqCnt,0);
						}
						//delay_us(400);
						StartCapture();
					}else
					{
						StopMultiFlash();
					}
					//LightSlaveParaClear();
				}
				//OS_EXIT_CRITICAL();
				radioStatue.cmdReady = 0;
			}
		break;

		case CMD_MODE_OFF:
			if(1==radioStatue.cmdReady)
			{
				if(WLM_CMDPROC_PRF==wlSet.m_cmdProc)
				{
					//SPI_MISO_AUX  = 0;
					HpPreflash(HP_PREFLASH_INTERVAL);
					DISABLE_D30EN();
					//SPI_MISO_AUX = 1;
					delay_us(100);
#ifdef EOS_750D
					g_getTtlFlashCode = 1;
#else
					SPIx_Init();
					SPI1->DR = 0x80;
					g_getTtlFlashCode = 1;
					g_comStatue = COM_STATUE_START;
#endif
					wlSet.m_cmdProc = WLM_CMDPROC_OFF;
				}else if(WLM_CMDPROC_FLA==wlSet.m_cmdProc)
				{
					 //pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
					 //TURN_ON_TRIGER(pTime);
					 //SPI_MISO_AUX = 1;
					 /*
					 if(COM_STATUE_IDLE==g_comStatue)
					 {
						//HaltSec(1);
						delay_us(500);
						if((GetBatteryAdc()<LOW_BAT_LEVEL)||(BP_IN>0))				//1//in case of unexpected flash
						//if(BP_IN>0)
						{
							DISABLE_D30EN();
							//FLAOK_TOGGLE();
							return;
						}
					 }
					 */
					 //FlashInResMode();
					 TakeFlash();
					 DISABLE_D30EN();
					 //delay_ms(5);
					// delay_ms(1);			//the delay cause the multi flash timeout, so the wlSet.m_cmdProc is overwrited, then the flash is interrupt
					 SPI_MISO_AUX = 1;
					 g_comStatue = COM_STATUE_IDLE;
					 wlSet.m_cmdProc = WLM_CMDPROC_OFF;
					 Ex_NVIC_Config(GPIO_A,5,RTIR);
					 Ex_NVIC_Enable(5);
				}else if(WLM_CMDPROC_DAT == wlSet.m_cmdProc)			//set DA(mcp4725) value
				{
					SetPreFlashDA();

				}else if(WLM_CMDPROC_MUL==wlSet.m_cmdProc)
				{
					//if((!BP_IN)||(MULTI_FLASH_ENABLE==multiSet.mulCtl))
					//FLAOK_ON();
					if(((!BP_IN)||(!PILOT_IN))&&(g_chargeOk>0))
					{
						 //SPI_MISO_AUX = 0;
						TakeMultiFlash(&multiSet.finalExpTimes,*multiSet.expose,multiSet.lowFrqCnt,1);
						 //SPI_MISO_AUX = 1;
					}else
					{
						StopMultiFlash();
						 SPI_MISO_AUX = 1;
					}
					//FLAOK_OFF();
				}
#ifdef TTL_DEBUG
//				else if(WLM_CMDPROC_TTLDEBUG==wlSet.m_cmdProc)
//				{
//					TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,ttl_expCodeRsv,DSP_NORMAL);
					//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,0xBB,DSP_NORMAL);
//				}
#endif
				radioStatue.cmdReady=0;
			}else
			{
				 //g_comStatue = COM_STATUE_IDLE;
			}
		break;
	}
}

void TaskCmdSend(void *pdata)
{
	while(1)
	{
		WLMCmdSend(*g_cmdMode);
		//DISABLE_D30EN();
		wlSet.m_cmdProc = WLM_CMDPROC_OFF;
		OSTaskSuspend(CMDSEND_TASK_Prio);
		//OSTimeDlyHMSM(0,0,0,50);
	}
}

void ComIdleFlash(void)
{
	u8 cgBak = 0;
	//u8 batLevel = 0;

	//DISABLE_CHARGE();                       // entern when power up, if disabled then enabled, it will cause check low bat. 2017/07/28
	cgBak = CHARGE_CON;
	CHARGE_ENABLE = 1;

	delay_us(200);

	// in the RF mode,  battery interrupt would flash?
/*
	if ((CMD_MODE_RF_MST == *g_cmdMode) || (CMD_MODE_RF_SLA == *g_cmdMode))
	{
		batLevel = (LOW_BAT_LEVEL - 20);
	}else
	{
		batLevel = LOW_BAT_LEVEL/2 ;
	}
*/
	
	//if((GetBatteryAdc()<(LOW_BAT_LEVEL/2))||(BP_IN>0))
	//if((GetBatteryAdc()<(LOW_BAT_LEVEL/2))||(BP_IN>0))
	//if((GetBatteryAdc() < batLevel)||(BP_IN>0))
	if((GetBatteryAdc()<(LOW_BAT_LEVEL-20))||(BP_IN>0))
	{
		return;
	}

	CHARGE_ENABLE = cgBak;

	//ENABLE_CHARGE();
	
	flaPara.hpFlash = HP_FLASHMODE_OFF;
	g_expTTLPara1 = mstPara->output1;
	g_expTTLPara2 = mstPara->output2;
	g_expTTLPara3 = 0;
	
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		case CMD_MODE_LIGH_MST:
			wlSet.m_cmdProc = WLM_CMDPROC_FLA;
			//wlSet.m_cmdProc = WLM_CMDPROC_SINGLE;
			radioStatue.cmdReady = 1;
			OSTaskResume(CMDSEND_TASK_Prio);
		break;

		case CMD_MODE_RF_MST:
		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			radioStatue.cmdReady = 1;
			g_vCmr = VER_CAMERA_NEW;
			radioStatue.cmdSta = RADIO_STATUE_FLASH;
			OSTaskResume(CMDSEND_TASK_Prio);
		break;

		case CMD_MODE_RF_SLA:
			radioStatue.cmdReady=1;
			radioStatue.cmdSta = RADIO_STATUE_SLAFLA;
			OSTaskResume(CMDSEND_TASK_Prio);
		break;

		case CMD_MODE_LIGH_SLA:
			radioStatue.cmdReady = 1;
			radioStatue.cmdSta = RADIO_STATUE_SLAFLA;
			canWireless.activeArr = 1;
			canWireless.WL_Channel = *wlSet.m_chan;
			wlSlave.s_wlMode = WORKINGMODE_TTL;
			OSTaskResume(CMDSEND_TASK_Prio);
		break;
	}
}

void EXTI4_IRQHandler(void)		//big point interrupt
{
	EXTI->PR=1<<4;	

	//SPI_MISO_AUX = 0;
	//SPI_MISO_AUX = 1;

	switch(g_comStatue)
	{
		case COM_STATUE_REALFLASH:
			ENABLE_D30EN();
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
						SPI_MISO_AUX = 0;
						radioStatue.cmdReady = 1;
						radioStatue.cmdSta = RADIO_STATUE_FLASH;
						OSTaskResume(CMDSEND_TASK_Prio);
					break;

					//case CMD_MODE_RF_MST:
					case CMD_MODE_RFLS_MST:
					case CMD_MODE_RFLS_SLA:
						/*delete 2015/05/28*/
						radioStatue.cmdReady=1;
						radioStatue.cmdSta = RADIO_STATUE_FLASH;
						OSTaskResume(CMDSEND_TASK_Prio);
						/*delete 2015/05/28*/
						//SPI_MISO_AUX = 0;
						//delay_ms(1);
						//SPI_MISO_AUX = 1;
						
					break;

					case CMD_MODE_OFF:
						if(!CLK_IN)
						{
							//SPI_MISO_AUX = 0;
							//SPI_MISO_AUX = 1;
							wlSet.m_cmdProc = WLM_CMDPROC_FLA;
							radioStatue.cmdReady = 1;
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
			//if((CMD_MODE_OFF==*g_cmdMode)&&(g_flaEnalbe>0))
#if 0
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				ENABLE_D30EN();
				flaPara.hpFlash = HP_FLASHMODE_OFF;
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_expTTLPara1 = mstPara->output1;
					g_expTTLPara3 = mstPara->output2;
				}
				g_expTTLPara3 = 0;
				wlSet.m_cmdProc = WLM_CMDPROC_FLA;
				//wlSet.m_cmdProc = WLM_CMDPROC_SINGLE;
				radioStatue.cmdReady = 1;
				OSTaskResume(CMDSEND_TASK_Prio);
			}
#endif
			ComIdleFlash();
		break;

		case COM_STATUE_START:
			//FLAOK_TOGGLE();			//just for test
		break;

		case COM_STATUE_MULTIFLASH:
		break;

		case COM_STATUE_SLEEP:
		break;

		
	}

}


void TURN_ON_TRIGER(unsigned int time)
{ 
#ifdef FLASH_BASE_FIBER
	//FLASH_AMP = 0;
#else
	//HC_CON3 = 1;
	HC_CON1 = 1;
	//HC_CON4 = 1;
	HC_CON3 = 1;
	HC_CON2 = 0;
	HpExpCodeSend(0xA2C0);
#endif

	if(time>OUTPUT_MAX)
	{
		time = OUTPUT_MAX;
	}
#ifdef EXT_TEST
	HC_CON4 = 0;
	EXT_PA = 0;
#endif
	if(!g_isHotProtect)
	{
		//20171208
		WLM_SHOT_ONCE(time);	
	}
#ifdef EXT_TEST
	HC_CON4 = 1;
	EXT_PA = 0;
#endif

#ifdef FLASH_BASE_FIBER
	FLASH_AMP = 0;
#endif
}

void TurnOnChargeLed(u8 init)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		case CMD_MODE_RF_SLA:
		case CMD_MODE_LIGH_SLA:
			//if(g_chargeOk>AD_CHARGE_RDY)
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				CHARGE_LED_OK();
				ChargeBuzzer();
				CR_READY = 1;
			}else
			{
				CHARGE_LED_OFF();
				CR_READY = 0;
			}
		break;
/*
		case CMD_MODE_LIGH_SLA:
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				CHARGE_LED_OK();
				//ChargeBuzzer();				//under CMD_MODE_LIGH_SLA mode, the buzzer could be cause problem
				CR_READY = 1;
			}else
			{
				CHARGE_LED_OFF();
				CR_READY = 0;
			}
			
		break;
*/

		case CMD_MODE_LIGH_MST:
			if(g_chargeOk>AD_CHARGE_RDY)
			{
				CHARGE_LED_OK();
				ChargeBuzzer();
				CR_READY = 1;
			}else
			{
				CHARGE_LED_OFF();
				CR_READY = 0;
			}
			/*
			if(AD_CHARGE_RDY==g_chargeOk)
			{
				CHARGE_LED_RDY();
			}else if(AD_CHARGE_FUL==g_chargeOk)
			{
				CHARGE_LED_OK();
			}else
			{
				CHARGE_LED_OFF();
			}*/
		break;

		case CMD_MODE_RF_MST:
			/*
			if(RfMasterReadyCheck()>0)
			{
				CHARGE_LED_OK();
				CR_READY = 1;
			}else
			{
				CHARGE_LED_OFF();
				CR_READY = 0;
			}*/
			RfMasterReadySet();
			/*
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				if(radioSet->rfGr[0].isOff>0)
				{
					if(radioStatue.devCnt[1]+radioStatue.devCnt[2]+radioStatue.devCnt[3]+radioStatue.devCnt[4]>0)
					{
						CHARGE_LED_OK();
						CR_READY = 1;
					}else
					{
						CHARGE_LED_OFF();
						CR_READY = 0;
					}
					
				}else
				{
					//if(g_chargeOk>AD_CHARGE_RDY)
					if(g_chargeOk>AD_CHARGE_LOW)
					{
						CHARGE_LED_OK();
						ChargeBuzzer();
						CR_READY = 1;
					}else
					{
						CHARGE_LED_OFF();
						CR_READY = 0;
					}
				}
			}else
			{
				if(*wlSet.is_masterOn>0)
				{
					//if(g_chargeOk>AD_CHARGE_RDY)
					if(g_chargeOk>AD_CHARGE_LOW)
					{
						CHARGE_LED_OK();
						ChargeBuzzer();
						CR_READY = 1;
					}else
					{
						CHARGE_LED_OFF();
						CR_READY = 0;
					}
				}else 
				{
					//if(init>0)
					if((init>0)&&(!CalcSum(&radioStatue.devCnt[0], 5)))
					{
						CHARGE_LED_OFF();
						CR_READY = 0;
					}
				}
			}*/
		break;
		
		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				//if(g_chargeOk>AD_CHARGE_RDY)
				if(g_chargeOk>AD_CHARGE_LOW)
				{
					CHARGE_LED_OK();
					ChargeBuzzer();
					CR_READY = 1;
				}else
				{
					CHARGE_LED_OFF();
					CR_READY = 0;
				}
			}else
			{
				CHARGE_LED_OFF();
				CR_READY = 1;
			}
		break;

	}

}






