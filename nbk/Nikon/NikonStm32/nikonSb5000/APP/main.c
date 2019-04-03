#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	
#include "includes.h"	 
#include "../HARDWARE/spi/spi.h"
#include "Nikon.h"
#include "../hardware/Lcd/font.h"
#include "keyproc.h"
#include "../TIMER/timer.h"

#if OS_TMR_EN>0
OS_TMR  *g_bkTmr;
OS_TMR	*g_bzTmr;
OS_TMR	*g_flashTmr;
OS_TMR  *g_paraUpdateTmr;
OS_TMR  *g_shutterPullTmr;
OS_TMR  *g_1ShotTmr;
OS_TMR  *g_slaChanDivTmr;
OS_TMR  *g_rptFlashTmr;
//OS_TMR	*g_slvBzTmr;
#endif

signed char *pSnrSet;
char g_bzCount = 0;
char g_keyAdcEnable=0;
MainPara mainPara={0};
Self_ArrPara *mstPara=&mainPara.paraMst;
signed char *g_cmdMode = &mainPara.cmdMode;
extern u8 g_shadowKey;
BkParas bkParas;
u8 g_isSlaveVoice = 0;
signed char g_expTTLPara1=0;
signed char g_expTTLPara2=0;
u8 g_isReflectPulled=0;
char g_motoInvalidCounter=0;      //after flash, disable moto adjust for a while, in order to avoiding unexpected moto moving
RptSet rptSet = {0};
FlashPara flashPara;
extern u32 gWSleepCounter;
u8 g_upDnSensor=0;
extern u8 g_isAutoStby;
SlvPilotSet slvPilotSet={0};
s32 g_expAccHotWarnValue=0;
extern u8 g_slaveBzEn;
extern u8 g_slaveBzCnt;

#define STACK_COMPROC_SIZE     128
#define KEYPROC_STK_SIZE    128//128
#define LED1_STK_SIZE    128
#define START_STK_SIZE   64//256
#define VOL_CK_SIZE			64
#define MOTO_TICK_SIZE		64
#define RPT_FLASH_SIZE          64


//设置任务优先级

//任务堆栈
OS_STK  TASK_LED1_STK[LED1_STK_SIZE];
OS_STK	TASK_VOLTAGE_STK[VOL_CK_SIZE];
//OS_STK  TASK_LED_STK[LED_STK_SIZE];
OS_STK TASK_COMPROC_STK[STACK_COMPROC_SIZE];
OS_STK  TASK_KEYPROC_STK[KEYPROC_STK_SIZE];
OS_STK  TASK_START_STK[START_STK_SIZE];
OS_STK  TASK_MOTOTICK_STK[MOTO_TICK_SIZE];
OS_STK TASK_RPTFLASH_STK[];
//任务申明
void TaskStart(void *pdata);
void TaskLed1(void *pdata);
extern void TaskKeyScan(void *pdata);
extern void NikComProc(void *pdata);
extern void NikComInit(void);
extern void NikRptFlashProc(void *pdata);
void TaskMonitor(void *pdata);
void TaskVoltageCheck(void *pdata);
void PowerDownProc(void);
void TaskMotoProc(void *pdata);
void CmrLinkWatchDog(void);
void SleepTimerProc(void);
void LightSlaveToggle(void);
void PowerDownWakeProc(void);
void TempMonitorProc(void);
extern void SlaveBuzzer(void);
extern void BzOff(void);

u8 test_data[] = {
0x01,0x01,0x05,0x02,0x08,0x07,0x81,0x78,0xb4,0x64,0x0a,0x4e,
0xff,0x3e,0x90,0x3e,0x90,0x1d
};

u8 g_GnScale[34] =
{
3,4,5,6,7,8,9,10,11,13,
14,16,18,20,22,25,28,32,36,40,
45,50,56,63,71,80,90,100,110,130,
140,160,180,200,
};

u8 g_GnScaleDotSet[34];

extern const char picStby[];

//系统时钟配置函数
void SysTick_Configuration(void);

unsigned char g_comStatue=0;
unsigned char g_comAck = 0;


void  KeyAdcInit(void)
{    
	//先初始化IO口
 	RCC->APB2ENR|=1<<4;    //使能PORTC口时钟 
	//GPIOA->CRL&=0XFFFFF00F;//PA1 2 anolog输入
	GPIOC->CRL&=0xFFFFF000;
	//通道10/11设置			 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 

	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
	ADC1->CR1&=~(1<<8);    //非扫描模式	  
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐	 
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;  //   //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道0~3的采样时间
	//ADC1->SMPR2&=0XFFFFF000;//通道0,1,2,3采样时间清空	  
	//ADC1->SMPR2|=7<<9;      //通道3  239.5周期,提高采样时间可以提高精确度	 
	//ADC1->SMPR2&=0XFFFFFE07;
	//ADC1->SMPR2|=7<<6;      //通道2  239.5周期,提高采样时间可以提高精确度	 
	//ADC1->SMPR2|=7<<3;      //通道1  239.5周期,提高采样时间可以提高精确度	 
	//ADC1->SMPR2|=7<<0;      //通道0  239.5周期,提高采样时间可以提高精确度	 
	ADC1->SMPR1 |= 7<<0;		//channal 10 ,11 PC0, PC1
	ADC1->SMPR1 |= 7<<3;
	
	ADC1->CR2|=1<<0;	    //开启AD转换器
	g_keyAdcEnable = 1;
	
	ADC1->CR2|=1<<3;        //使能复位校准  
	while(ADC1->CR2&1<<3);  //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  
}

void KeyAdcEnable(void)
{
	g_keyAdcEnable = 1;
	KeyAdcInit();
}

void KeyAdcDisable(void)
{
	g_keyAdcEnable = 0;
	//ADC_DeInit(ADC1);
}

void GolbalParaInit(void)
{
	g_shadowKey = 0;
	g_snrIndex = 0;
	g_snrSubIndex = 0;
	g_snrStart = 0;
	g_snrMyMenuSelStart = 0;
	g_snrMyMenuSelIndex = 0;
	cmrPara.mode = 0;
	cmrPara.cls = 0;
	g_dspCmpMod = 0;
	g_chargeOk = 0;
	g_mstFlaEnable = 0;
	g_mstEnFlag = 0;
	g_expAccHotWarnValue = 0;
	cmrPara.pIso = 0;
}

void UserInit(void)
{
	u8 i;			//the parameter exceed 128byte     2015/07/28
	char sum=0;
	char *pMain;
	
#ifdef MST_DEBUG
	*g_cmdMode = CMD_MODE_LIGH_SLA;
	mstPara->rmt_ch = 0;
	FuncModuleSw();
#else

	pSnrSet = mainPara.snrMenuSet;
	motoSet.mode = &mainPara.motoLoca.mode;
	motoSet.dest_location = &mainPara.motoLoca.moto_location;
	rptSet.frequence= &(mstPara->rpt_hz);
	rptSet.eTimes = &(mstPara->rpt_times);
	rptSet.rpt_exp1= &(mstPara->multi_exp1);
	rptSet.rpt_exp2= &(mstPara->multi_exp2);

	STMFLASH_Read(FLASH_USER_ADDR,(u16*)(&mainPara),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.cmdMode)+2)/2);

	pMain = (char*)&mainPara;
	//for(i=0;i<(sizeof(mainPara)-1);i++)
	for(i=0;i<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.cmdMode));i++)
	{
		sum += pMain[i];
	}

	if(mainPara.sumVeri!=sum)
	{
		//memset((u8*)&mainPara,0,sizeof(mainPara));	
		memset((u8*)&mainPara,0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.cmdMode)));
	}

	if(mainPara.sumVeri!=sum)
	{
		//memset((u8*)&mainPara,0,sizeof(mainPara));	
#ifdef FIBER_ADJUST_TEST
		printf("para error\r\n");
#endif
		memset((u8*)&mainPara,0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.cmdMode)));
	}

	mainPara.extIso = isoValue[pSnrSet[SNR_ISO]];

	if((mainPara.extApert<14)||(mainPara.extApert>640))
	{
		mainPara.extApert = 56;
	}

	if(mainPara.paraMst.gn_m<3)
	{
		mainPara.paraMst.gn_m = 20;
	}

	if((*motoSet.dest_location<20)||(*motoSet.dest_location>200))
	{
		*motoSet.dest_location = 24;
	}
	motoSet.manuRsv = *motoSet.dest_location;
	motoSet.destRsv = *motoSet.dest_location;
	//motoSet.dspLocation = *motoSet.dest_location;
	

	if(*rptSet.eTimes<1)
	{
		*rptSet.eTimes = 1;
	}

	if((*rptSet.frequence<1)||(*rptSet.frequence>100))
	{
		*rptSet.frequence = 1;
	}

	MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);

	motoSet.focIndex = CodeSearch((char*)focLenScape,MOTO_MAX_INDEX,*motoSet.dest_location);

	motoSet.dspLocation = focLenScape[motoSet.focIndex];

	*motoSet.mode = MOTO_MODE_AUTO;

	
	//*g_cmdMode = CMD_MODE_OFF;
	//*g_cmdMode = CMD_MODE_LIGH_MST;
	//*g_cmdMode = CMD_MODE_LIGH_SLA;
	//mainPara.modGr[0].mode = WORKINGMODE_OFF;
	//mainPara.modGr[1].mode = WORKINGMODE_TTL;
	//mainPara.modGr[2].mode = WORKINGMODE_TTL;
	//mainPara.modGr[3].mode = WORKINGMODE_TTL;
	//mainPara.modGr[0].cmp1 = 0;
	//mainPara.modGr[0].cmp2 = 0;
	//mainPara.modGr[1].output1 = 0;
	//mainPara.modGr[1].output2 = 0;
	//mainPara.modGr[2].cmp1 = 0;
	//mainPara.modGr[2].cmp2 = 0;
	//mainPara.extIso = 125;
	//mainPara.extApert = 56;
	//mainPara.paraMst.gn_m = 20;
	//motoSet.mode = &mainPara.motoLoca.mode;
	//*motoSet.mode = MOTO_MODE_AUTO;
	//motoSet.dest_location = &mainPara.motoLoca.moto_location;
	//*motoSet.dest_location = 24;
	//motoSet.manuRsv = *motoSet.dest_location;
	//motoSet.destRsv = 24;
	//motoSet.dspLocation = 24;
	//pSnrSet = mainPara.snrMenuSet;
	//mstPara->rpt_times = 1;
	//mstPara->rpt_hz = 1;
	//rptSet.frequence= &(mstPara->rpt_hz);
	//rptSet.eTimes = &(mstPara->rpt_times);
	//rptSet.rpt_exp1= &(mstPara->multi_exp1);
	//rptSet.rpt_exp2= &(mstPara->multi_exp2);
	//mstPara->mode = WORKINGMODE_RPT;
	//mstPara->mode = WORKINGMODE_TTL;
	//memset(pSnrSet,0x00,19);
	//pSnrSet[SNR_MSTRPT] = 1;
	g_isSlaveVoice = 1;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
#endif
	//WLS_ExposeTimeCal(0xa0,1);
	//pSnrSet[1] = 1;
	//pSnrSet[13] = 5;
}

u8 GetBatteryAdc(void)
{
//	u8 zoom=0;
	
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_VOL;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));

	return (ADC1->DR>>4);
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
			BK_TURNOFF(g_isHotProtect);
			//LowPow_Init(0);
			LCD_Clear(0);
			PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
			delay_seconds(3);
		}
	}
}
//#define TEST  /##/
//Mini STM32开发板ucos范例
//基于ucos2.52内核.
//正点原子@ALIENTEK
//www.openedv.com
//2010.12.15			 
int main(void)
{
		 	   
	Stm32_Clock_Init(9); //系统时钟设置
	delay_init(72);	     //延时初始化 
#if (_DBG_PRINT_>0)
	uart_init(72/4,115200);	 //串口初始化为9600
#endif
	//LED_Init();		  	 //初始化与LED连接的硬件接口  

	UserInit();

#ifndef MOTO_DEBUG_PRT
	
	//PortInit();
	PortInit();
	SPIx_Init();
	
	Timer4_Init(40,72);	     // timer4 use for time out 
	Timer2_Init(40,72);         // timer2 use for rpt flash

	KeyAdcInit();

#endif

//for buzzer test
//	TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
//	PWM_Init(288,72,144);

//	TEST("thisi is a test");

//	DBG_PRINTF("20151218 Nikon Start\r\n");

	DBG_PRINTF(__DATE__);
	DBG_PRINTF("\r\n");
	DBG_PRINTF(__TIME__);
	DBG_PRINTF("\r\n");
	
	if(KEY_POW_OFF>0)
	{
		LCD_Init();
		LCD_Clear(0);
		
#if (!_DBG_PRINT_)
		CheckBattery();
#endif
		BK_GREEN(g_isHotProtect);
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _MOTO_
#ifndef MOTO_DEBUG_PRT
		MotoReset();			//before turn on charge_en
#else
		*motoSet.dest_location= 200;
#endif
#endif
		//BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
 		//InitKeyInt();
		//g_shadowKey = 0;
		//Mode();
		//CHARGE_LED2_ON();
		//PortInit();
	}else
	{
		LcdPortInit();
	}

	//DisplayPicture(4,50,36,32,picStby,0);
	//DspManuExp2OrCmp(0,30,0,3,0,DSP_REVERSE);

       //DBG_PRINTF("check1\r\n");
	//DBG_PRINTF("Nikon APP\n");
	//ClkSynSend(0xEA);
	//PortInit();
	//ClkSynSend(0xEA);
	//DBG_PRINTF("os start");
	//EXTIx_Init();
	SysTick_Configuration();
	OSInit();
	OSTaskCreate( TaskStart,	//task pointer
					(void *)0,	//parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
					START_TASK_Prio );	//task priority
	//DBG_PRINTF("os start");	
	OSStart();
	return 0;	   
}

void BkLiteAutoOff(void)
{
	BK_TURNOFF(g_isHotProtect);
	KEY_BK_OFF();
	bkParas.bkState= BK_OFF;
	bkParas.bkFlag = BK_OFF;
	/*
	if(g_SettingIndex>0)
	{
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}*/
	if((g_SettingIndex>0)&&(SETTING_SNRMENU!=g_SettingIndex)&&(SETTING_SNRSUB!=g_SettingIndex))
	{
		KeyOk();
	}

/*
	if(!cmrPara.cls)
	{
		SET_SCK_LOW();
	}else
	{
		macData2[COM_DAT_STATUE]&=~0x01;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}
*/
	if(cmrPara.cls>0)
	{
		macData2[COM_DAT_STATUE]&=~0x01;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}
}

/*
void RptFlashProc(void)
{
	TakeRptFlash(&rptSet.finalExpTimes,rptSet.exp1*3+rptSet.exp2);
}
*/

//开始任务
void TaskStart(void * pdata)
{
	INT8U tmrErr;
	pdata = pdata; 
	OS_ENTER_CRITICAL();   
	
	OSTaskCreate(NikComProc, (void * )0, (OS_STK *)&TASK_COMPROC_STK[STACK_COMPROC_SIZE-1], COMPROC_TASK_PRIO);
	OSTaskCreate(TaskKeyScan, (void * )0, (OS_STK *)&TASK_KEYPROC_STK[KEYPROC_STK_SIZE-1], KEYPROC_TASK_Prio);
	OSTaskCreate(TaskMonitor, (void * )0, (OS_STK *)&TASK_LED1_STK[LED1_STK_SIZE-1], LED1_TASK_Prio);
	OSTaskCreate(TaskVoltageCheck,(void * )0,(OS_STK *)&TASK_VOLTAGE_STK[VOL_CK_SIZE-1],VOL_CK_Prio);

	OSTaskCreate(TaskMotoProc,(void *)0, (OS_STK *)&TASK_MOTOTICK_STK[MOTO_TICK_SIZE-1],MOTO_TICK_Prio);

	OSTaskCreate(NikRptFlashProc,(void *)0,(OS_STK *)&TASK_RPTFLASH_STK[RPT_FLASH_SIZE-1],RptFlash_TASK_Prio);
	
#if OS_TMR_EN > 0
	//g_osTmr = OSTmrCreate(200, 400,  OS_TMR_OPT_PERIODIC,  BkLiteAutoOff,  NULL, "CanTmr ", &tmrErr);
	g_bkTmr = OSTmrCreate(10000, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)BkLiteAutoOff,  NULL, "bkTmr", &tmrErr);
	g_bzTmr = OSTmrCreate(200, 200,  OS_TMR_OPT_PERIODIC,  (OS_TMR_CALLBACK)BzOff,  NULL, "bzTmr ", &tmrErr);
//	g_slvBzTmr = OSTmrCreate(200, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)SlaveBuzzer,  NULL, "SlvBzTmr", &tmrErr);
	//g_rptFlashTmr = OSTmrCreate(200,0,OS_TMR_OPT_ONE_SHOT,(OS_TMR_CALLBACK)RptFlashProc, NULL, "rptFlashTmr", &tmrErr);
#endif

	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
}

//extern char const focLenScape[11];

void TaskVoltageCheck(void *pdata)
{
//	u8 cnt=0;

	//CHARGE_LED2_ON();
	
	while(1)
	{
		//ChkVoltage();
		OSTimeDlyHMSM(0,0,0,10);
/*
		cnt++;
		if(cnt>20)
		{
			cnt=0;
			CHARGE_LED2_TOGGLE();
		}
*/
	}

}

void LedTestToggle(void)
{
	CHARGE_LED2_TOGGLE();
}

void SlvPilotFlash(void)
{
	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		if(slvPilotSet.fla_en>0)
		{
			slvPilotSet.tms--;
			if(slvPilotSet.tms<=0)
			{
				SlaveSetFlash();
				slvPilotSet.fla_en = 0;
			}
		}

		if(g_slaveBzEn>0)
		{
			if(g_slaveBzCnt>0)
			{
				g_slaveBzCnt--;
			}else
			{
				g_slaveBzEn = 0;
				BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
			}
		}
	}

}


void SensorMonitor(void)
//u8 SensorMonitor(void)
{
	static u8 upSensor=0;
	static u8 dnSensor=0;
	//static u8 autoDestRsv=35;
	//u16 vRgb[24]={0};
	//u16 vRgbRst[4]={0};
//	u8 cnt=0;
//	u8 tm=0;
	u8 isMod=0;

	//u8 rgbRt=0;
/*	
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
*/
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
				g_upDnSensor = SENSOR_UPDN_DN;
			}
			upSensor = CAP_SENSOR_UP;
			dnSensor = CAP_SENSOR_DN;
			isMod = 1;
#if 0
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
#endif
			
		}
	}

	if(isMod>0)
	{
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}

//	return rgbRt;
}

void AfValidOnCheck(void)
{
	if(g_isAfOn>0)
	{
		if(cmrPara.cls>0)
		{
			g_afOnCnt++;   //50ms every increment
			if(g_afOnCnt>40)
			{
				AF_OFF();
				g_isAfOn = 0;
				g_afOnCnt = 0;
			}
		}else
		{
			AF_OFF();
			g_isAfOn = 0;
			g_afOnCnt = 0;
		}
	}
}


void TaskMonitor(void *pdata)
{
//	u8 i=0;
//	s8 e1=-3,e2=0;
	//SPIx_Enable();
	//g_comStatue = COM_STATUE_RUN;
	
	while(1)
	{
		//TEST_SIGNAL = !TEST_SIGNAL;
		//LED1=!LED1;
		//CHARGE_LED2_TOGGLE();

		//if(!PILOT_IN)
		//LcdClearFeild(0,0,50,16);
		//DspResModIcon(i++,DSP_REVERSE);
		//if(i>WORKINGMODE_RPT)i=0;
		//OSTimeDly(900);
		//DspApertValue(apertValue[i++],DSP_REVERSE);
		//if(i>=34)i=0;
		//OSTimeDly(900);
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
		SlvPilotFlash();
		SensorMonitor();
		LightSlaveToggle();
		CmrLinkWatchDog();
		SleepTimerProc();
		PowerDownProc();
		AfValidOnCheck();
		TempMonitorProc();
//		SIGNAL_LED = !SIGNAL_LED;
/*
		LcdClearFeild(0, 30, 41, 16);
		DspManuExp2OrCmp(0,30,e1,e2>0?(e2>1?7:3):0,0,DSP_REVERSE);
		e2++;
		if(e2>2)
		{
			e2-=3;
			e1++;
		}
		if(e1>3)
		{
			e1 = -3;
			e2 = 0;
		}
		OSTimeDly(900);
*/
/*
		//scanCount++;
		DisplayPicture(6,74,24,24,picScan_1,0);
		//delay_ms(300);
		HaltSec(300);
		DisplayPicture(6,74,24,24,picScan_2,0);
		//delay_ms(300);
		HaltSec(300);
		DisplayPicture(6,74,24,24,picScan_3,0);
		//delay_ms(400);
		HaltSec(400);
*/

		//SPIx_Enable();
		//OSTimeDly(10);
		
		//ClkSynSend(0xEA);
		//ClkSynRcv(0);
		//ClkSynRcv(&i);
		//OSTimeDly(10);
		//SPIx_ReadWriteByte(0xEA);
		//SPIx_Disable();
		//for(i=0;i<sizeof(test_data);i++)
		//{
			//test_data[i] = SPIx_ReceiveByte();
			//OSTimeDly(1);
			//delay_us(50);			
		//}
		//g_comStatue = COM_STATUE_RUN;
		//LiteBlockSend(test_data,sizeof(test_data));
		//ClkSynSend(test_data,sizeof(test_data));
		//LiteBlockSend("spi test",sizeof("spi test"));
		//SPI_MISO_AUX = !SPI_MISO_AUX;

		//OSTimeDlyHMSM(0,0,0,200);
		//OSTimeDly(100);
		//OSTimeDly(100);
		//HaltSec(250);
		//HaltSec(20);
		OSTimeDlyHMSM(0,0,0,50);
	}
}

void LightSlaveToggle(void)
{
	static u8 togTmr=0;
	static u8 togflag=0;
	if((CMD_MODE_LIGH_SLA==*g_cmdMode)&&(g_chargeOk>0))
	{
		//if((0==pSnrSet[SNR_REAR])||(2==pSnrSet[SNR_REAR]))
		if(1)
		{
			if(togTmr<15)
			{
				togTmr++;
			}else if(togTmr<16)
			{
				AF_CTL_BOTTOM = 1;
				togflag = 1;
				togTmr++;
			}else 
			{
				AF_CTL_BOTTOM = 0;
				togTmr = 0;
				togflag = 0;
			}
		}else
		{
			if(togflag>0)
			{
				AF_CTL_BOTTOM = 0;
			}
		}
	}else
	{
		if(togflag>0)
		{
			AF_CTL_BOTTOM = 0;
			togTmr = 0;
			togflag = 0;
		}
	}

}

//#define TEMP_WARN_ALERT 	200
//#define TEMP_CANCEL_ALERT	180
#define TEMP_WARN_ALERT 	180
#define TEMP_CANCEL_ALERT	160

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
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				BK_GREEN(1);
			}
			tempWarn = 1;
		}//else if(tmp<70)
		else if(tmp<TEMP_CANCEL_ALERT)
		{
			if(tempWarn>0)
			{
				tempWarn = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
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
	static u8 exitCnt = 0;
	if(g_isHotProtect & HOT_WARN_TIMES)
	{
		if (exitCnt++>10)
		{
			exitCnt = 0;
			g_expAccHotWarnValue --;
			if(g_expAccHotWarnValue<=0)
			{
				g_isHotProtect ^= HOT_WARN_TIMES;
				g_expAccHotWarnValue = 0;
			}
		}
	}else
	{
		if(g_expAccHotWarnValue>EXP_ACC_VALUE_WARN_LEVEL)
		{
			g_isHotProtect |= HOT_WARN_TIMES;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			BkLiteOn();
			BK_GREEN(g_isHotProtect);
		}
	}
	/*
	if(g_expAccHotWarnValue>EXP_ACC_VALUE_WARN_LEVEL)
	{
		g_isHotProtect |= HOT_WARN_TIMES;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		BkLiteOn();
		BK_GREEN(g_isHotProtect);
	}else 
	{
		if((g_isHotProtect & HOT_WARN_TIMES))
		{
			if(exitCnt++>10)
			{
				exitCnt= 0;
				g_expAccHotWarnValue --;
				if(!g_expAccHotWarnValue)
				{
					g_isHotProtect ^= HOT_WARN_TIMES;
				}
			}
		}
	}
	*/
	

}

//u8 g_tempAdBuf[16];
void TempMonitorProc(void)
{

	TempCheck();
	FlashTimesCheck();
	
}



const u16 stbyTime[5] = {30,40,80,160,300};
void SleepTimerProc(void)
{
#if 0
	if((pSnrSet[SNR_STBY]!=5)&&(!cmrPara.cls)&&(CMD_MODE_LIGH_SLA!=*g_cmdMode))
	{

		if((0==pSnrSet[SNR_STBY])&&(g_isAutoStby>0))
		{
			g_stbyStatue = 1;
			SetChargeDisable();
			CHARGE_LED_OFF();
			DISABLE_KEY_INT();
			g_isAutoStby = 0;
			KEY_BK_OFF();
			BK_TURNOFF(g_isHotProtect);
			//LCD_Clear(0);
			//DisplayPicture(4,68,36,32,picStby,DSP_NORMAL);
			//BuildStbyMenu();
			//CLR_UPDATE_EVT(DSP_UPDATE_MODE);
			g_SettingIndex = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}else
		{
			gWSleepCounter++;	

			//if(COM_STATUE_STBY!=g_comStatue)
			if(!g_stbyStatue)
			{
				if(gWSleepCounter>(50*stbyTime[pSnrSet[SNR_STBY]]))
				{
					//g_comStatue = COM_STATUE_STBY;
					g_stbyStatue = 1;
					SetChargeDisable();
					CHARGE_LED_OFF();
					DISABLE_KEY_INT();
					KEY_BK_OFF();
					BK_TURNOFF(g_isHotProtect);
					//LCD_Clear(0);
					//DisplayPicture(4,68,36,32,picStby,DSP_NORMAL);
					//BuildStbyMenu();
					g_SettingIndex = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}else
			{
				//if((gWSleepCounter>50*60*60))
				if((gWSleepCounter>50*60*3))
				//if((gWSleepCounter>50*40))
				//if(gWSleepCounter>(50*stbyTime[pSnrSet[SNR_STBY]]+50))
				{
					PortUninit();
					SleepWakeIntSet();
					//SetSBOInt();
					//LowPow_Init(1,EXTI_Trigger_Rising);
					LowPow_Init(1,EXTI_Trigger_Rising,(CMD_MODE_LIGH_MST==*g_cmdMode));
					g_stbyStatue = 0;
					gWSleepCounter = 0;
					PowerDownWakeProc();
				}
			}
		}
	}
#else
	if((!cmrPara.cls)&&(CMD_MODE_LIGH_SLA!=*g_cmdMode))
	{
		if(g_isAutoStby>0)
		{
			g_stbyStatue = 1;
			SetChargeDisable();
			CHARGE_LED_OFF();
			DISABLE_KEY_INT();
			g_isAutoStby = 0;
			KEY_BK_OFF();
			BK_TURNOFF(g_isHotProtect);
			//LCD_Clear(0);
			//DisplayPicture(4,68,36,32,picStby,DSP_NORMAL);
			//BuildStbyMenu();
			//CLR_UPDATE_EVT(DSP_UPDATE_MODE);
			g_SettingIndex = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}else
		{
			gWSleepCounter++;	

			//if(COM_STATUE_STBY!=g_comStatue)
			if(!g_stbyStatue)
			{
				if(gWSleepCounter>(50*stbyTime[0]))
				{
					//g_comStatue = COM_STATUE_STBY;
					g_stbyStatue = 1;
					SetChargeDisable();
					CHARGE_LED_OFF();
					DISABLE_KEY_INT();
					KEY_BK_OFF();
					BK_TURNOFF(g_isHotProtect);
					//LCD_Clear(0);
					//DisplayPicture(4,68,36,32,picStby,DSP_NORMAL);
					//BuildStbyMenu();
					g_SettingIndex = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}else
			{
				//if((gWSleepCounter>50*60*60))
				if((gWSleepCounter>50*60*3))
				//if((gWSleepCounter>50*40))
				//if(gWSleepCounter>(50*stbyTime[pSnrSet[SNR_STBY]]+50))
				{
					PortUninit();
					SleepWakeIntSet();
					//SetSBOInt();
					//LowPow_Init(1,EXTI_Trigger_Rising);
					LowPow_Init(1,EXTI_Trigger_Rising,(CMD_MODE_LIGH_MST==*g_cmdMode));
					g_stbyStatue = 0;
					gWSleepCounter = 0;
					g_isHotProtect = 0;
					g_expAccHotWarnValue = 0;
					PowerDownWakeProc();
				}
			}
		}
	}
#endif

	if(!cmrPara.cls)
	{
		if((BK_OFF==bkParas.bkState)&&(BK_LIGHT1_IN>0))
		{
			BK_TURNOFF(g_isHotProtect);
			KEY_BK_OFF();
			bkParas.bkState= BK_OFF;
			bkParas.bkFlag = BK_OFF;
		}
	}

}

void CmrLinkWatchDog(void)
{
	if(cmrPara.cls>0)
	{
		//if(COM_STATUE_REALFLASH!=g_comStatue)
		if(!NIKONNSSI)
		{
			cmrPara.linkTmr++;
			if(cmrPara.linkTmr>4)
			{
				cmrPara.linkTmr = 0;
				Ex_NVIC_Disable(ID_INT_NSS);
				RstComPara();
				SetSBOInt();			
				g_comStatue = COM_STATUE_IDLE;
				cmrPara.cls = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}
	}
}

void PowerDownWakeProc(void)
{
	//delay_init(72);
	//	g_sysStart = 1;
	//iap_load_app(FLASH_APP1_ADDR);
	Stm32_Clock_Init(9);
	GolbalParaInit();                          // before portInit(), in order to avoiding flash in master mode, because the g_mstFlaEnable is not set to 0
	PortInit();
	SPIx_Init();
	NikComInit();
	//GolbalParaInit();
	//uart2_init(36,57600);
	//uart2_init(36,115200);
	//Adc_Init();
	KeyAdcInit();
	LCD_Init();
	//LCD_Clear(0);
	CheckBattery();
	//SET_UPDATE_EVT(DSP_UPDATE_MODE);
	OSTaskResume(COMPROC_TASK_PRIO);
	//Mode();
	//delay_ms(500);	//20150813 for debug
#if(defined(TTL_DEBUG)||defined(MOTO_TEST)||defined(EXT_TEST_PRT)||defined(FIBER_ADJUST_TEST))
	uart1_init(72,115200);
#endif
#if (_DBG_PRINT_>0)
	uart_init(72/4,115200);	 //串口初始化为9600
#endif
	//GetZoomAdc();
	delay_ms(300);
	//if(KEY_POW_OFF>0)
	//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
	//{
		//RfMoudleSet();
	//}
	//OSTimeDlyHMSM(0,0,0,100);
	//RfMoudleSet();
	//g_keyDir = 0;
	//g_shadowKey = 0;
	//cmrPara.cls = 0;
	//g_dspCmpMod = 0;
	MotoReset();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	BkLiteOn();
	//BK_GREEN(0);
	//BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
	//NikComInit();
	//g_flaEnalbe = 1;
	//Mode();
}

unsigned char CalcSum(u8 *rcv, u8 len)
{
	u8 i=0,sum=0;
	for(i=0;i<len;i++)
	{
		sum+=rcv[i];
	}
	return sum;
}

void SavePara(u32 sAddr)
{
#if 0
	mainPara.sumVeri=CalcSum((u8*)&(mainPara.cmdMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.cmdMode)));
	//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
	STMFLASH_Write(sAddr,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.cmdMode)+2)/2);
#endif
}

void PowerDownProc(void)
{
	//if(KEY_LOCK_EX&&KEY_POW_ON&&!KEY_POW_OFF)
	if(!KEY_POW_OFF)
	//if(KEY_LOCK_EX&&!KEY_POW_OFF)
	{
		OSTimeDlyHMSM(0,0,0,50);
		//if(KEY_LOCK_EX&&KEY_POW_ON&&!KEY_POW_OFF)
		if(!KEY_POW_OFF)
		//if((KEY_LOCK_EX>0)&&(KEY_POW_ON>0))
		//if(KEY_LOCK_EX&&!KEY_POW_OFF)
		//if(KEY_LOCK_EX&&KEY_POW_ON)
		{
			//SavePara(FLASH_USER_ADDR);
			
			PortUninit();
		
			LowPow_Init(1,EXTI_Trigger_Falling,0);
			
			PowerDownWakeProc();
		}
	}

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
	MY_NVIC_Init(3,3,SysTick_IRQn,2);
	SysTick->CTRL|=1<<0;   //开启SYSTICK
}




















