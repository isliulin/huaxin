#include "spi.h"
#include "../APP/canon.h"
#include "led.h"
#include "../APP/golbal.h"
#include "../APP/Display.h"
#include "../APP/KeyProc.h"
#include "../TIMER/timer.h"
#include "../system/usart/usart.h"
#include "../LCD/FONT.h"
#include "FlashPara.h"

#define MAX_BUFFER_SIZE    100//20           // Maximum buffer Master will send
#define SEND_MAX_SIZE 100//20
#define LENOFTIMETABLE 42

#define CLEAR_EXTINT_FLAG

//parameter location defination
#define PARA_MODE		0
#define PARA_SYNC1	 	0
#define PARA_SYNC2	 	1
#define PARA_SYNC3	 	2
#define PARA_FOCIND		3
#define PARA_FOCLEN		4 
#define PARA_VOUT		5
#define PARA_SRD		6
#define PARA_WIRELESS	7
#define PARA_FREQ		8
#define PARA_MOUT		9
#define PARA_A5			10
#define PARA_COMP       11
#define PARA_WLM_MODE       12

unsigned char g_spiRcvIndex=0;//SPI_Receive_Index;
unsigned char g_spiSendIndex=0;
u8 g_spiRcvTotal = 0;
//unsigned char g_spiRear=0;
unsigned char g_tmHandleFlag=0;
unsigned char g_spiRcvData[MAX_BUFFER_SIZE] = {0};
unsigned char g_spiSendData[SEND_MAX_SIZE]={0};
signed char *g_cmdMode;
unsigned char g_vCmr=0;
u8 g_AfIndex=0;
extern WlSet wlSet;
unsigned char g_activeControl=0x00;
unsigned char g_comStart=0;
MenuPara menuPara={0};
//RfGroupPara rfGrPara={0,0,0,{0},{0}};
extern MotoSet motoSet;
unsigned char g_mstPrefla=0;
u8 g_isModeling = 0;
//unsigned char g_mstMode=0;
//unsigned char charge_ok=0;
unsigned char g_getTtlFlashCode=0;
unsigned char ttl_expCodeRsv=0;
extern unsigned char g_comStatue;
extern RadioSet* radioSet ;
extern RadioStatue radioStatue;
extern char g_SettingIndex;
extern unsigned char uart_tran_flag;
extern signed char *pSnrSet;
extern u8 g_isRgbFilter;
//RadioPara radioPara;
u16 g_dspUpdateFlag=0;
unsigned char g_shutterFlag=0;
u8 g_hpLev;
extern char g_chargeOk;
unsigned char Parameter_Array[13]= {0x20,0x5d,0x53,0x13,0x23,0x20,0x00,0x00,0x01,0xa8,0x00,0x00,0x5c};
const unsigned char wl_opArr[22] = {0x03,0x05,0x06,0x07,0x09,0x0a,0x0b,0x0d,0x0e,0x0f,0x11,0x12,0x13,0x15,0x16,0x17,0x19,0x1a,0x1b,0x1d,0x1e,0x1f};
const char cfnCntMask[] = {0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x00,};

u8 g_cmrCode;
u8 g_cmrA9Id;
extern char g_cmrLinked;
extern char g_cmrPressed;
//extern Word gWSleepCounter;
extern u32 gWSleepCounter;
extern CanWireless canWireless;
extern u8 g_paraUpdateFlag;
extern INT32U g_slaveTimer;
u8 g_menuLock=0;
u8 g_menuModRsv=0;
u8 g_motoModRsv=0;
u8 g_motoDestRsv=0;
u8 g_cmdModRsv=0;
u8 g_sckFlash = 0;
extern signed char g_rfChanSet;
extern unsigned int g_rfIdSet;
extern u8 g_isRfSlaveLinked;
extern s8 g_slaveReady;
extern char g_motoInvalidCounter;

#if OS_TMR_EN>0
extern OS_TMR  *g_paraUpdateTmr;
extern OS_TMR  *g_shutterPullTmr;
#endif

void SpiRearUp(u16 tks);
extern signed char GrModConvert(u8 i);
extern void Pilot(void);

unsigned char const compensation_table[37]={
0x30,0x2d,0x2b,
0x28,0x25,0x23,
0x20,0x1d,0x1b,
0x18,0x15,0x13,
0x10,0x0d,0x0b,
0x08,0x05,0x03,
0x00,0xfd,0xfb,
0xf8,0xf5,0xf3,
0xf0,0xed,0xeb,
0xe8,0xe5,0xe3,
0xe0,0xcd,0xcb,
0xc8,0xc5,0xc3,
0xc0,
};

unsigned char const compensation_table_5[] = 
{
0x30,
0x2c,0x28,0x24,0x20,
0x1c,0x18,0x14,0x10,
0x0c,0x08,0x04,0x00,
0xfc,0xf8,0xf4,0xf0,
0xec,0xe8,0xe4,0xe0,
0xcc,0xc8,0xc4,0xc0,
};



typedef struct TIMEINDEX{	
	unsigned char index;
	int ms;
}timeTable;

const timeTable shutterTable[LENOFTIMETABLE]={
{0x77,4},{0x78,4},
{0x75,5},{0x73,6},{0x70,8},{0x6d,10},{0x6b,12},{0x68,16},{0x65,20},{0x63,25},{0x60,33},{0x5d,40},{0x5b,50},{0x58,66},{0x55,76},{0x53,100},
{0x50,125},{0x4d,160},{0x4b,200},{0x48,250},{0x45,300},{0x43,400},{0x40,500},{0x3d,600},{0x3b,800},{0x38,1000},{0x35,1300},
{0x33,1600},{0x30,2000},{0x2d,2500},{0x2b,3200},{0x28,4000},{0x25,5000},{0x23,6000},{0x20,8000},{0x1d,10000},{0x1b,13000},
{0x18,15000},{0x15,20000},{0x13,25000},{0x10,30000},/*{0x0c,60000},*/{0x0c,1000},
} ;

unsigned char code output_table[22]={0x00,0x02,0x05,0x08,0x0a,0x0d,
									0x10,0x12,0x15,0x18,0x1a,0x1d,
									0x20,0x22,0x25,0x28,0x2a,0x2d,
									0x30,0x32,0x35,0x38,};

WLM_abRation_fillwords const wlm_abRation_fillwords[13] = 
{
	{0x01,0x19},{0x02,0x15},{0x03,0x12},{0x04,0x0f},{0x05,0x0c},{0x06,0x0a},{0x08,0x08},
	{0x0a,0x06},{0x0c,0x05},{0x0f,0x04},{0x12,0x03},{0x15,0x02},{0x19,0x01},
};

unsigned char const feb_table[10] = {
0x00,0x03,0x05,0x08,0x0b,0x0d,0x10,0x13,0x15,0x18
};

unsigned char const feb_table_5[7] = {
0x00,0x04,0x08,0x0c,0x10,0x14,0x18,
};

unsigned int code CannonApertVal[] = {		6,	8,	9,	10,	11,	12,
										14,	16,	18,	20,	22,	25,	
										28,	32,	35,	40,	45,	50,	
										56,	63,	71,	80,	90,	100,
										110, 130, 140,160, 180, 200,
										220, 250, 290,320, 360, 400,
										450, 510	};

unsigned char abRation_fill1=0,abRation_fill2=0;

#ifdef _MULTIPLY_FLASH_
MultiSet multiSet;
#endif

extern u8 g_sleepFlag;

void CodeAnalysis(unsigned char rcvCode);
void FlashHandle(void);
void GetParameter(void);
void BuildExposeCmd(char mode,unsigned char hpSyn,unsigned char hpLex);
extern void BkLiteAutoOff(void);
extern void SetFlashTimeOut(INT32U tm);
extern signed char GrModRevert(signed char mode);

void SPIx_Init(void)
{	 
/***************PortInit************************************
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 	 
	RCC->APB2ENR|=1<<12;      //SPI1时钟使能 
		   
	//这里只针对SPI口初始化
	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0X8B800000;//PA5.6.7复用 	    
	GPIOA->ODR|=0X7<<5;    //PA5.6.7上拉
*****************************************************/
	//SPIx_Disable();
	RCC->APB2RSTR|=(1<<12);
	RCC->APB2RSTR&=~(1<<12);

	//RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟//20150903

	RCC->APB2ENR|=1<<2;
	RCC->APB2ENR|=1<<12;      //SPI1时钟使能 
	
	GPIOA->CRL&=0x000FFFFF; 
	GPIOA->CRL|=0x8B800000;//PA5.6.7复用PA4 input 	    
	//GPIOA->ODR|=0x07<<5;    //PA5.6.7上?
	//GPIOC->CRL&=0xFFF0FFFF;
	//GPIOC->CRL|=0x00030000;

	//GPIOE->CRH&=0xFFFFFFF0;
	//GPIOE->CRH|=0x00000003;

	//RCC->APB2ENR|=1<<4;		//enable portc clock
	//GPIOC->CRL&=0xFFF0FFFF;
	//GPIOC->CRL|=0x00030000;
	//GPIOC->CRL&=0xFF00FFFF;
	//GPIOC->CRL|=0x00330000;
/*	
	SPI1->CR1|=0<<10;//全双工模式	
	SPI1->CR1|=1<<9; //软件nss管理
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=0<<2; //SPI从机
	SPI1->CR1|=0<<11;//8bit数据格式	
	SPI1->CR1|=1<<1; //空闲模式下SCK为1 CPOL=1
	SPI1->CR1|=1<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	SPI1->CR1|=7<<0; //Fsck=Fcpu
	SPI1->CR1|=0<<7; //MSBfirst   
*/

	SPI1->CR1 &= ~(1<<11);
	SPI1->CR1 |= (1<<0);		//CPHA = 1;
	SPI1->CR1 |= (1<<1);
	SPI1->CR1 &=~(1<<7);
	SPI1->CR1 |= (1<<9);
	SPI1->CR1 &=~(1<<8);

	SPI1->CR1 &= ~(1<<2);

	//set spi interrupt
	SPI1->CR2 |= 1<<6;		//Enable RXNE interrupt
	//MY_NVIC_Init(0,0,SPI1_IRQChannel,1);
	MY_NVIC_Init(0,0,SPI1_IRQn,1);
	SPI1->CR1 |= (1<<6);
	
	 SPI_MISO_AUX=1;
	 COM_AUX = 1;
	 //g_spiSendIndex=0;
	// g_spiRcvIndex=0;
	//SPI1->DR = 0x00;
	SPI1->DR = 0x00;

	//SPIx_ReadWriteByte(0x00);//启动传输		 


}

void EXTIx_Init(u8 GPIOx,u8 TirEdge)
{
	//RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	//JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

	if(GPIO_A==GPIOx)
	{
		GPIOA->CRL&=0XFF0FFFFF;//PA5设置成输入	  
		GPIOA->CRL|=0X00800000;   
		GPIOA->ODR&=~(1<<5);
		//GPIOA->ODR|=1<<5;	   //PA13上拉,PA0默认下拉
	}else if(GPIO_B==GPIOx)
	{
		GPIOB->CRL&=0XFF0FFFFF;
		GPIOB->CRL|=0X00800000;   				   
		GPIOB->ODR|=1<<5;	   
	}

	//Ex_NVIC_Config(GPIO_A,5,FTIR); //下降沿触发
	//Ex_NVIC_Config(GPIO_A,5,TirEdge);

	//MY_NVIC_Init(0,0,EXTI9_5_IRQChannel,2);    //抢占2，子优先级2，组2
	MY_NVIC_Init(0,0,EXTI9_5_IRQn,2);    //抢占2，子优先级2，组2

	//Ex_NVIC_Config(GPIO_A,5,TirEdge);
	Ex_NVIC_Config(GPIOx,5,TirEdge);

	
}


void SetPreFlashDA(void)
{
	//hpSetting = HP_EXP_50;
	//hpSetting = HP_EXP_52;
	hpSetting = HP_EXP_PREFLA;

	//FLAOK_TOGGLE();

#ifdef FIBER_ADJUST_PROC
	//if(0xA5==mainPara.isFiberAdjust)
	if(FIBER_ADJUST_FLAG==mainPara.isFiberAdjust)
	{
		//*hps = (*hps*255)/mainPara.fiberCoe;
		if(mainPara.fiberCoe>9)
		{
			hpSetting *= CalcCmpPower(mainPara.fiberCoe-9);
		}else 
		{
			hpSetting /= CalcCmpPower(9-mainPara.fiberCoe);
		}
#ifdef FIBER_ADJUST_TEST
		printf("PRE AD %x\r\n",hpSetting);
#endif
	}/*
	else if(0xA6==mainPara.isFiberAdjust)
	{
		hpSetting /= CalcCmpPower(mainPara.fiberCoe);
	}
	*/
#endif
	//hpSetting = HP_EXP_60;
	//hpSetting = HP_EXP_61;
	//HC_CON1 = 1;
	//HC_CON4 = 0;
	//HC_CON3 = 0;
	//HC_CON2 = 0;
#if 0
	HC_CON2 = 1;
	HC_CON1 = 0;
	HC_CON3 = 1;
#endif
	HpExpCodeSend(hpSetting<<4);
	//HwTriger(tmOv);
	//TakePreFlash(tmOv);
	//HC_CON4 = 1;	
	//HC_CON3 = 1;
}

void HpPreflash(unsigned int tmOv)
{
	//FLASH_OUT = 0;
	//HpExpCodeSend(HP_EXP_80);
	//hpSetting = HP_EXP_80;
	//FLASH_OUT = 0;
	//HighSpeedTriger(&hpSetting,HP_PREFLASH_INTERVAL);	
	//hpSetting = HP_EXP_50;
	//hpSetting = HP_EXP_60;
	//hpSetting = HP_EXP_70;
	//HighSpeedTriger(&hpSetting, tmOv);
	//HC_CON3 = 1;
	if(g_isHotProtect>0)
	{
		return;
	}

	if(FALSE == IsKeyValid())
	{
		return;
	}
	
	ENABLE_D30EN_EX();
	HC_CON1 = 1;
	//HC_CON4 = 0;
	HC_CON3 = 0;
	HC_CON2 = 0;
	//HpExpCodeSend(hpSetting);
	//HwTriger(tmOv);
	TakePreFlash(tmOv);
	//HC_CON4 = 1;	
	HC_CON3 = 1;
	DISABLE_D30EN_EX();
	g_motoInvalidCounter = MOTO_INVALID_CNT;
	//HighSpeedTriger(&hpSetting,tmOv);
	//TURN_ON_TRIGER(OUTPUT_30);
	//FLASH_OUT = 1;
}

void SetComResumeInt(void)
{
	SPIx_Disable();
	Ex_NVIC_Config(GPIO_A,5,RTIR);
	Ex_NVIC_Enable(5);
	//g_comStatue = COM_STATUE_IDLE;
}

void SetComSpiReady(void)
{
	SPI_MISO_AUX=1;
	EXTIx_Init(GPIO_A, FTIR);
	SPIx_Init();
	g_comStatue = COM_STATUE_START;
}

void EXTI9_5_IRQHandler(void)
{
	//SPI_MISO_AUX = !SPI_MISO_AUX;
	//LED0=!LED0;
	
	switch(g_comStatue)
	{
		case COM_STATUE_START:
		if(!CLK_IN)			//2015/03/09 2017/04/06 增加，避免按选择对角点按钮会反复进入中断问题
		{
			SPI_MISO_AUX=0;
			//TEST_SIGNAL = !TEST_SIGNAL;
			//EXTI->PR=1<<5;
			Ex_NVIC_Disable(5);
			Timer4_Init(1000,72);
			g_tmHandleFlag = TM_HANDLE_SHUT;
		}//Timer3_Start();
		break;

		case COM_STATUE_IDLE:
		//if(!CLK_IN)	return;
		if(CLK_IN)
		{
		//FLASH_CONTROL = !FLASH_CONTROL;
		//PAout(6) = 1;
		//SPI_MISO_AUX=1;			//2015/06/17
		//OSTimeDlyHMSM(0,0,0,1);
		//EXTIx_Init(FTIR);
		//EXTIx_Init(GPIO_A, FTIR);
		//SPIx_Init();
		//g_comStatue = COM_STATUE_START;
		SetComSpiReady();
		g_cmrLinked = 1;
		//g_dspUpdateFlag|=DSP_UPDATE_LINK;
		if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			radioStatue.cmdReady = 1;
			radioStatue.cmdSta = RADIO_STATUE_RECOVER;
			OSTaskResume(CMDSEND_TASK_Prio);
		}else if((CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_LIGH_SLA==*g_cmdMode))
		{
			*g_cmdMode = CMD_MODE_OFF;
			mainPara.workMode = WORKINGMODE_TTL;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			DISABLE_RF_MODULE();
			//FLAOK_ON();
			
		}
		//FLAOK_TOGGLE();
		ACT_SHUTTER = 0;		//camera actived, pull down the pin
		//FLAOK_ON();
		//WIRE_SHUTTER = 0;
		//FLAOK_OFF();
		//CHARGE_LED_OFF;
#ifdef SIGNAL_TEST
		SIGNAL_PIN1 = !SIGNAL_PIN1;
#endif
		}
		break;

		case COM_STATUE_PREFLASH:
			//DEBUG_PIN2 = !DEBUG_PIN2;
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
			/*
				HpPreflash(HP_PREFLASH_INTERVAL);
				SPI_MISO_AUX  = 0;
				delay_us(30);
				//FLASH_OUT = 0;
				g_spiSendIndex = 0;
				g_comStatue = COM_STATUE_START;
				SPIx_Init();
				SPI1->DR = 0x80;
			*/
			ENABLE_D30EN();
			radioStatue.cmdReady = 1;
			wlSet.m_cmdProc = WLM_CMDPROC_PRF;
			OSTaskResume(CMDSEND_TASK_Prio);
			break;

			case CMD_MODE_LIGH_MST:
				/*
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
			SPIx_Init();
			SPI1->DR = 0x80;
			g_comStatue = COM_STATUE_START;
			*/
			ENABLE_D30EN();
			wlSet.m_cmdProc = WLM_CMDPROC_PRF;
			radioStatue.cmdReady = 1;
			OSTaskResume(CMDSEND_TASK_Prio);
			break;

			case CMD_MODE_RF_MST:
			if(CMR_ADEPT_NEW==radioStatue.adept)
			{
				//delay_us(100);
				//SPIx_Init();
				//SPI1->DR = 0x80;
				//SPI1->DR = 0xFF;
				//g_comStatue = COM_STATUE_START;
				//if((0==radioStatue.delay_mod)||(radioStatue.delay_mod>2))//all mode
				{
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_PRE;
					radioStatue.cmdSta = RADIO_STATUE_PRE_FLA;
					radioStatue.adept = CMR_ADEPT_NEW;
					OSTaskResume(CMDSEND_TASK_Prio);
				}
			}else if(CMR_ADEPT_OLD==radioStatue.adept)
			{
				//SPIx_Disable();
				radioStatue.cmdReady=1;
				radioStatue.cmdSta = RADIO_STATUE_PRECFM;
				OSTaskResume(CMDSEND_TASK_Prio);
				//DEBUG_PIN2 = !DEBUG_PIN2;
			}
			break;

			case CMD_MODE_RFLS_MST:
			case CMD_MODE_RFLS_SLA:
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				ENABLE_D30EN();
				radioStatue.cmdReady = 1;
				wlSet.m_cmdProc = WLM_CMDPROC_PRF;
				radioStatue.cmdSta = RADIO_STATUE_PRE_FLA;
				OSTaskResume(CMDSEND_TASK_Prio);
			}else
			{
#ifndef EOS_750D
				if(CMR_ADEPT_OLD==radioStatue.adept)	
				{
					delay_us(100);
					SPIx_Init();
					//SPI1->DR = 0x80;
					SPI1->DR = 0xFF;
					g_comStatue = COM_STATUE_START;
				}else
				{
					delay_us(100);
					SPIx_Init();
					//SPI1->DR = 0x80;
					SPI1->DR = 0xFF;
					g_comStatue = COM_STATUE_START;
				}
#endif
			}
			break;
		}
/***********************************************************/
//this is in order to competible with canon EOS750D
/***********************************************************/
#ifdef EOS_750D
		EXTIx_Init(GPIO_A,RTIR);							//2015/09/03
		g_comStatue = COM_STATUE_IDLE;
#endif
		break;

		case COM_STATUE_REALFLASH:		
		ENABLE_D30EN();
		if(!CLK_IN)
		{
			//if((SYNMODE_REAR==mainPara.synMode)&&(!BP_IN))
			if(!BP_IN)			//rear
			{
				if(CMD_MODE_OFF==*g_cmdMode)
				{
					if(SYNMODE_REAR==mainPara.synMode)					//2015/10/06 Rear cartain debug
					{
						wlSet.m_cmdProc = WLM_CMDPROC_FLA;
						radioStatue.cmdReady = 1;
						OSTaskResume(CMDSEND_TASK_Prio);
					}else if (g_sckFlash > 0)
					{
						// 
						wlSet.m_cmdProc = WLM_CMDPROC_FLA;
						radioStatue.cmdReady = 1;
						OSTaskResume(CMDSEND_TASK_Prio);
					}
				}else
				{
					SPIx_Disable();
					Ex_NVIC_Config(GPIO_A,5,RTIR);
					Ex_NVIC_Enable(5);
					g_comStatue = COM_STATUE_IDLE;
				}
			}else //if(HP_FLASHMODE_ON==flaPara.hpFlash)//else if((SYNC_MODE_FP==mainPara.synMode)&&(HP_FLASHMODE_ON==flaPara.hpFlash))
			{
				switch(*g_cmdMode)
				{
					case CMD_MODE_LIGH_MST:
					case CMD_MODE_OFF:
						if((HP_FLASHMODE_ON==flaPara.hpFlash)
							||(g_sckFlash>0))                                        //2016/08/11 for 5dmarkIII
						{
							//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);	
							radioStatue.cmdReady = 1;
							wlSet.m_cmdProc = WLM_CMDPROC_FLA;
							OSTaskResume(CMDSEND_TASK_Prio);
						}else
						{
							SPI_MISO_AUX = 0;			//if without pull down this pin, the flash interrupt cannot induce, the reason was unknown
						}
						SetComResumeInt();			//2016/02/22 when big shoe is not cotacted well, it result deadlock
					break;

					case CMD_MODE_RF_MST:
						if(CMR_ADEPT_NEW==radioStatue.adept)
						{
							radioStatue.cmdReady=1;
							//radioStatue.cmdSta = RADIO_STATUE_FLASH;
							radioStatue.cmdSta = RADIO_STATUE_FLAINFO;
							OSTaskResume(CMDSEND_TASK_Prio);
						}else
						{
							if(HP_FLASHMODE_ON==flaPara.hpFlash)
							{
								radioStatue.cmdReady=1;
								//radioStatue.cmdSta = RADIO_STATUE_FLASH;
								radioStatue.cmdSta = RADIO_STATUE_FLAINFO;
								OSTaskResume(CMDSEND_TASK_Prio);
							}
						}
						SetComResumeInt();			//2016/02/22 when big shoe is not cotacted well, it result deadlock
					break;

					//case CMD_MODE_RF_MST:
					case CMD_MODE_RFLS_MST:
					case CMD_MODE_RFLS_SLA:
						radioStatue.cmdReady=1;
						//radioStatue.cmdSta = RADIO_STATUE_FLASH;
						radioStatue.cmdSta = RADIO_STATUE_FLAINFO;
						OSTaskResume(CMDSEND_TASK_Prio);
						SetComResumeInt();			//2016/02/22 when big shoe is not cotacted well, it result deadlock
					break;

					//case CMD_MODE_OFF:
						//wlSet.m_cmdProc = WLM_CMDPROC_FLA;
						//OSTaskResume(CMDSEND_TASK_Prio);
					//break;
				}
				//SPIx_Disable();
				//Ex_NVIC_Config(GPIO_A,5,RTIR);
				//Ex_NVIC_Enable(5);
				//g_comStatue = COM_STATUE_IDLE;
			}
		}else
		{
			SetComSpiReady();
		}
		/*
		if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			if(CMR_ADEPT_NEW==radioStatue.adept)
			{
				radioStatue.cmdReady=1;
				radioStatue.cmdSta = RADIO_STATUE_FLASH;
				OSTaskResume(CMDSEND_TASK_Prio);
			}
		}else if(CMD_MODE_RFLS_MST==*g_cmdMode)
		{
			radioStatue.cmdReady=1;
			radioStatue.cmdSta = RADIO_STATUE_FLASH;
			OSTaskResume(CMDSEND_TASK_Prio);
		}else if(CMD_MODE_RFLS_SLA==*g_cmdMode)
		{
			radioStatue.cmdReady=1;
			radioStatue.cmdSta = RADIO_STATUE_FLASH;
			OSTaskResume(CMDSEND_TASK_Prio);
		}*/
		break;

		case COM_STATUE_MODEL:
			radioStatue.cmdReady = 1;
			radioStatue.cmdSta = RADIO_STATUE_MODEL;
			OSTaskResume(CMDSEND_TASK_Prio);
		break;

/********************************************
			CMD_MODE_RF_SLA
********************************************/
		case COM_STATUE_SLAFLA:
			if(!UART_INT_IN)
			{
				radioStatue.cmdReady = 1;
				radioStatue.cmdSta = RADIO_STATUE_SLAFLA;
				OSTaskResume(CMDSEND_TASK_Prio);
			}
		break;

		case COM_STATUE_SLAPRE:
			if(!UART_INT_IN)
			{
				radioStatue.cmdReady = 1;
				radioStatue.cmdSta = RADIO_STATUE_SLAPRE;
				OSTaskResume(CMDSEND_TASK_Prio);
			}
			//FLAOK_TOGGLE();
		break;
/********************************************
				END
********************************************/
		case COM_STATUE_SLEEP:
		if(CLK_IN)
		{
			g_comStatue = COM_STATUE_START;
			EXTIx_Init(GPIO_A, FTIR);
			SPIx_Init();
			SPI_MISO_AUX=1;			//2015/06/17
			COM_AUX = 1;
			//FLAOK_ON();
			//SpiRearUp(100);
		}else if(!UART_INT_IN)
		{
#ifdef SLAVE_POWER_DOWN
			Ex_NVIC_Disable(5);	
			g_comStatue = COM_STATUE_IDLE;
#endif
			g_isRfSlaveLinked = 1;
		}else 
		{
			if(EXTI_GetITStatus(EXTI_Line6) == SET)//                 ／／判断是否是１３脚的中断
		       {
#ifdef SLAVE_POWER_DOWN
		       	 g_comStatue = COM_STATUE_IDLE;				//2015/10/31 for slave sleep
#endif
				 Ex_NVIC_Disable(6);						//2015/11/03 this must be closed, otherwise cause the error
		               EXTI_ClearITPendingBit(EXTI_Line6);//                ／／清除中断标志位！
		               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            ／／点亮ＬＥＤ            
		        }
		}
		break;

		case COM_STATUE_MODELING:
			Com_Stop();
			Ex_NVIC_Config(GPIO_A,5,RTIR);
			Ex_NVIC_Enable(5);
			g_comStatue = COM_STATUE_IDLE;
			SPI_MISO_AUX=0;
		break;
		
	}
	EXTI->PR=1<<5;
	
}

void SPIx_Enable()
{
	COM_AUX = 1;
	SPI1->CR1|=1<<6;
}

void SPIx_Disable()
{
	//SPI1->CR1&=~(1<<6);
	RCC->APB2RSTR|=(1<<12);
	RCC->APB2RSTR&=~(1<<12);
	//g_spiRcvData[0] = SPI1->SR;
	SPI1->CR1 = 0x00;
	GPIOA->CRL&=0xF0FFFFFF;
	GPIOA->CRL|=0x07000000;
	//FLASH_OUT = 0;
	//COM_AUX = 0;
	//SPI_MISO_AUX = 0;
	g_spiRcvIndex = 0;
	g_spiSendIndex=0;
	g_comStart = 0;
}

void Com_Stop(void)
{
	SPIx_Disable();
	COM_AUX = 0;
	SPI_MISO_AUX = 0;
	FLASH_OUT = 0;
}

//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//二分频
	{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI_SPEED_8)//八分频 
	{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI_SPEED_16)//十六分频
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256分频
	{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz 低速模式
	}
	SPI1->CR1|=1<<6; //SPI设备使能	  
} 


//void SpiRearUp(void)
void SpiRearUp(u16 tks)
{
	//Timer4_Init(40,72);
	Timer4_Init(tks,72);
	g_tmHandleFlag = TM_HANDLE_REAR;	
	//g_spiRear = 1;
	//delay_us(30);
	//SET_SPIO_ON;
	//SPI_MISO_AUX=1;
	
}

void SPI1_IRQHandler(void)
{
	unsigned char datRcv;
	//TEST_SIGNAL = !TEST_SIGNAL;
	//TEST_SIGNAL = !TEST_SIGNAL;
	//tmp = SPI1->SR;
	//g_spiRcvData[g_spiRcvIndex]=SPI1->DR;
	Timer4_Stop();
	SET_SPIO_OFF;
	PUSH_MISO;
	datRcv = SPI1->DR;
	g_spiRcvTotal ++;
	if(g_spiSendIndex>0)
	{
		g_spiRcvData[g_spiRcvIndex++] = datRcv;
		SPI1->DR = g_spiSendData[--g_spiSendIndex];
		if(!g_spiSendIndex)
		{
			FlashHandle();
			g_spiRcvIndex = 0;
		}else
		{
			//SPI_MISO_AUX=1;	
			SpiRearUp(15);
		}
	}else
	{
		//get the last parameter
		GetParameter();			//extract infomation
		g_spiRcvData[g_spiRcvIndex++]=datRcv;
		CodeAnalysis(datRcv);	//load spi data
		SPI1->DR = g_spiSendData[--g_spiSendIndex]; 	//send a byte	 
		//SPI_MISO_AUX=1;	
		SpiRearUp(100);
		
	}
	//SPI_MISO_AUX=1;	
	
	Ex_NVIC_Enable(5);
	//Timer4_Stop();
	//Timer4_Init(10,71);
	//g_dspUpdateFlag|=DSP_UPDATE_LINK;
	//SET_UPDATE_EVT(DSP_UPDATE_LINK);				//wried
	EXTI->PR=1<<5;
}

void CodeAnalysis(unsigned char rcvCode)
{	
	//GetParameter();
	s8 i=0;
//	INT8U tmrErr;
	//g_spiRcvIndex= 0 ;
	//switch(g_spiRcvData[0])
	switch(rcvCode)
	{
		case 0xff:
		//g_spiRcvData[g_spiRcvIndex++]=0xff;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		/*case 0xb5: 
		g_spiRcvData[g_spiRcvIndex++]=0xb5;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x0d;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xde;
			break;*/
		case 0xb5:
		//g_spiRcvData[g_spiRcvIndex++]=0xb5;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xde;	
			//if(g_paraUpdateFlag>0)
			//{
			//	OSTmrStop(g_paraUpdateTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);
			//	g_paraUpdateFlag = 0;
			//}
			break;
		case 0xe5:														//*ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xe5;
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x0d;
			g_spiSendData[g_spiSendIndex++]=0x8E;

			/*
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				//g_spiSendData[g_spiSendIndex++]=0x03;
				g_spiSendData[g_spiSendIndex++]=0x03;//0x07;			//for active shutter
				//g_spiSendData[g_spiSendIndex++]=(g_activeControl?0x05:0x03);//((mainPara.workMode==WORKINGMODE_GR)<<4)|(g_activeControl?0x05:0x03);//0x03;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x01;
			}*/
			switch(*g_cmdMode)
			{
				case CMD_MODE_RF_MST:
					g_spiSendData[g_spiSendIndex++]=0x03;
					//g_spiSendData[g_spiSendIndex++]=0x13;
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						g_spiSendData[g_spiSendIndex++]=0x15;				//RF LINK_SHOT with flash open
					}else
					{
						g_spiSendData[g_spiSendIndex++]=0x07;
					}
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x01;
					//g_spiSendData[g_spiSendIndex++]=0x11;                              // FOR 5DMARKIII NO FLASH 2016/08/11
				break;

				
			}
			g_spiSendData[g_spiSendIndex++]=0x48;
			g_spiSendData[g_spiSendIndex++]=0x4d;
			break;
		case 0xa9:														//*ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xa9;  
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			//g_spiSendData[g_spiSendIndex++]=0x97;
			break;
		case 0xa1:														//*ST-E3
			//g_spiSendData[g_spiSendIndex++]=0x4b;
			//g_spiSendData[g_spiSendIndex++]=0x4b;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x4C;
			g_spiSendData[g_spiSendIndex++]=0x4C;
			//g_spiSendData[g_spiSendIndex++]=0x4C;
			/*
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=0x29;
				g_spiSendData[g_spiSendIndex++]=0x29;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x4B;
				g_spiSendData[g_spiSendIndex++]=0x4B;
			}*/
			break;
		case 0xa5:
			//g_spiRcvData[g_spiRcvIndex++]=0xa5;
			g_spiSendData[g_spiSendIndex++]=0x96+(*g_cmdMode==CMD_MODE_RF_MST);;
			//g_spiSendData[g_spiSendIndex++]=0x97;
			g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_A5];//0x00;//0x00;//this should never ignored, otherwith cause error when work on the canon 600d
			//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_A5];//0x00;	           //2010/10/20  for ttl test 
			//if(GETBIT(bitTerminate,BIT_bitTerminate_A5FLAG))
			//{
			//	g_spiSendData[g_spiSendIndex++]=0x01;//Parameter_Array[PARA_A5];//0x00;	           //2010/10/20  for ttl test 
			//	CLRBIT(bitTerminate,BIT_bitTerminate_A5FLAG);
			//}else
			//{
			//	g_spiSendData[g_spiSendIndex++]=0x00;
			//}
			//g_spiSendData[g_spiSendIndex++]=0x01;
			break;
		case 0xb9:
			//g_spiRcvData[g_spiRcvIndex++]=0xb9;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			if(g_stepLength>0)
			{
				//switch(flaPara.surrand_state)
				switch(flaPara.FEB_count)
				{
					//case 0:
					//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_COMP];//0x00;  //....this place index a surrand flash
					//break;
					case 0:
						if(pSnrSet[SNRMENU_FEBTURN]>0)
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table_5[(mstPara->cmp1+6-flaPara.ttl_FEB1)*2+mstPara->cmp2-flaPara.ttl_FEB2];
						}else
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table_5[(mstPara->cmp1+6)*2+mstPara->cmp2];
						}
					break;

					case 1:
						if(pSnrSet[SNRMENU_FEBTURN]>0)
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table_5[(mstPara->cmp1+6)*2+mstPara->cmp2];
						}else
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table_5[(mstPara->cmp1+6-flaPara.ttl_FEB1)*2+mstPara->cmp2-flaPara.ttl_FEB2];
						}
					break;

					case 2:
						g_spiSendData[g_spiSendIndex++]=compensation_table_5[(mstPara->cmp1+6+flaPara.ttl_FEB1)*2+mstPara->cmp2+flaPara.ttl_FEB2];
					break;
#if 0				
					case 1:									
					g_spiSendData[g_spiSendIndex++]=menuPara.mstTtl;//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
					break;

					case 2:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+3+flaPara.ttl_FEB1)*3+mstPara->cmp2+flaPara.ttl_FEB2];
					break;

					case 3:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+3-flaPara.ttl_FEB1)*3+mstPara->cmp2-flaPara.ttl_FEB2];
					break;
#endif
				}
			}else
			{
				//switch(flaPara.surrand_state)
				switch(flaPara.FEB_count)
				{
					//case 0:
					//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_COMP];//0x00;  //....this place index a surrand flash
					//break;
					case 0:
						if(pSnrSet[SNRMENU_FEBTURN]>0)
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6-flaPara.ttl_FEB1)*3+mstPara->cmp2-flaPara.ttl_FEB2];
						}else
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
						}
					break;

					case 1:
						if(pSnrSet[SNRMENU_FEBTURN]>0)
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
						}else
						{
							g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6-flaPara.ttl_FEB1)*3+mstPara->cmp2-flaPara.ttl_FEB2];
						}
					break;

					case 2:
						g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6+flaPara.ttl_FEB1)*3+mstPara->cmp2+flaPara.ttl_FEB2];
					break;
#if 0				
					case 1:									
					g_spiSendData[g_spiSendIndex++]=menuPara.mstTtl;//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
					break;

					case 2:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+3+flaPara.ttl_FEB1)*3+mstPara->cmp2+flaPara.ttl_FEB2];
					break;

					case 3:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+3-flaPara.ttl_FEB1)*3+mstPara->cmp2-flaPara.ttl_FEB2];
					break;
#endif
				}		
			}
			break;
		case 0xbd:
			//g_spiRcvData[g_spiRcvIndex++]=0xbd;
/*
			if(!g_comStart)
			{
				g_spiSendData[g_spiSendIndex++]=0xFF;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x59;
				g_spiSendData[g_spiSendIndex++]=0xAA;
				g_comStart = 1;
			}else
*/
			{
				//g_spiSendData[g_spiSendIndex++]=0x02;
#ifdef _ST_EX_
				g_spiSendData[g_spiSendIndex++]=0x02;//Parameter_Array[PARA_FOCLEN];
				g_spiSendData[g_spiSendIndex++]=0x18;//0x69;//moto range
				g_spiSendData[g_spiSendIndex++]=0x18;
				g_spiSendData[g_spiSendIndex++]=0x18;//0x18;
				
#else
				g_spiSendData[g_spiSendIndex++]=*motoSet.dest_location;//Parameter_Array[PARA_FOCLEN];
				g_spiSendData[g_spiSendIndex++]=0xC8;//0x69;//moto range
				g_spiSendData[g_spiSendIndex++]=0x14;//0x18;
				g_spiSendData[g_spiSendIndex++]=0x02;
				//g_spiSendData[g_spiSendIndex++]=0x18;//0x18;
#endif
		
			}
		break;
			
		case 0xfb:
			//g_spiRcvData[g_spiRcvIndex++]=0xfb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			/*
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2];
			}else
			{
				g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2];
			}*/
			if(WORKINGMODE_GR==mainPara.workMode)				//2015/10/06
			{
				g_spiSendData[g_spiSendIndex++] = 0x55;
			}else
			{
				g_spiSendData[g_spiSendIndex++] = (0x5d|((SYNC_MODE_REAR==mainPara.synMode)<<1));

			}
			g_spiSendData[g_spiSendIndex++]=0x8e;
#ifdef RF_MODE_GROUP
			if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
#else
			if(WORKINGMODE_TTL==mainPara.workMode)
#endif
			{
				g_spiSendData[g_spiSendIndex++]=0x02;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
			}							
			break;
			
		case 0xf9:
			//g_spiRcvData[g_spiRcvIndex++]=0xf9;
			g_spiSendData[g_spiSendIndex++]=0x8e;
//#ifdef _ST_EX_
			switch(*g_cmdMode)
			{
				//case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
					//g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54; 			//in the rf Master Mode, if there are SLAVEs linked, it send 0x55
					g_spiSendData[g_spiSendIndex++]=(RfMasterReadyCheck()>AD_CHARGE_LOW)?0x55:0x54; 
				break;

				case CMD_MODE_LIGH_MST:
					g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_RDY)?0x55:0x54;
				break;
				
				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54;
					}else
					{
						g_spiSendData[g_spiSendIndex++]=0x55;	
					}
				break;

				case CMD_MODE_OFF:
					g_spiSendData[g_spiSendIndex++]=(((g_chargeOk>AD_CHARGE_LOW)?0x55:0x54)|((SYNC_MODE_REAR==mainPara.synMode)<<1));

				break;

				default:
					//g_spiSendData[g_spiSendIndex++]=0x55;	
					g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54;
				break;
			}
//#else
//			g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2]; 
//#endif
			break;
		case 0xb2:
			//g_spiRcvData[g_spiRcvIndex++]=0xb2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
		break;
		case 0xb3:
			//g_spiRcvData[g_spiRcvIndex++]=0xb3;
			g_spiSendData[g_spiSendIndex++]=0x00;
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
					if(WORKINGMODE_TTL==mainPara.workMode)
					{
						if(WLM_MODE_ALL==*wlSet.ttl_Ratio)
						{
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
						}else
						{
							g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;
							g_spiSendData[g_spiSendIndex++]=abRation_fill2;
							g_spiSendData[g_spiSendIndex++]=abRation_fill1;
						}
					}else
					{
						g_spiSendData[g_spiSendIndex++]=0x00;
						g_spiSendData[g_spiSendIndex++]=0x00;
						g_spiSendData[g_spiSendIndex++]=0x00;
					/*
						if(WLM_MODE_ALL==*wlSet.arr_mode)
						{
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
							g_spiSendData[g_spiSendIndex++]=0x00;
						}else
						{
							if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
							{
								g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;
								g_spiSendData[g_spiSendIndex++]=abRation_fill2;
								g_spiSendData[g_spiSendIndex++]=abRation_fill1;
							}else
							{
								g_spiSendData[g_spiSendIndex++]=0x00;
								g_spiSendData[g_spiSendIndex++]=0x00;
								g_spiSendData[g_spiSendIndex++]=0x00;
							}
							
						}
					*/
					}
				break;
				
/*
				case CMD_MODE_RF:
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x08;
					g_spiSendData[g_spiSendIndex++]=0x08;
				break;
*/			
				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
				case CMD_MODE_OFF:		
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x08;
					g_spiSendData[g_spiSendIndex++]=0x08;
				break;

			}
			//SETBIT(paraBit,0);
			break;
		case 0xf5:
			//g_spiRcvData[g_spiRcvIndex++]=0xf5;
			g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x0f;
			g_spiSendData[g_spiSendIndex++]=(0x0f|((!g_isRgbFilter)<<4));
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=0x5d+*wlSet.ttl_Ratio;
				}else
				{
					g_spiSendData[g_spiSendIndex++]=0x5d+*wlSet.arr_mode;
				}
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=(0x5D+*wlSet.ttl_Ratio);
					//g_spiSendData[g_spiSendIndex++]=(0x4D+*wlSet.ttl_Ratio);
				}else
				{
					//2015/08/24 0X4D is invalid when modeling
					g_spiSendData[g_spiSendIndex++]=(0x5D+*wlSet.arr_mode);//0x4d+*wlSet.arr_mode+(WORKINGMODE_GR==mainPara.workMode);
				}
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x5c;
			}
			if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
			{
				g_spiSendData[g_spiSendIndex++]=(0x49|((flaPara.surrand_state<<4)&0x30));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x48;
			}
/******************************************************************************************
//			here restrain shtter speed in mode except TTL, MANU, Gr
*******************************************************************************************/
			if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_MANU==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x53|((SYNMODE_HP==mainPara.synMode)<<2);//Parameter_Array[PARA_SYNC3];
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x53;
			}
			break;
		case 0xbb:
			//g_spiRcvData[g_spiRcvIndex++]=0xbb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=menuPara.feb;//Parameter_Array[PARA_SRD];//0x00;			//feb
//#ifdef _ST_EX_
			g_spiSendData[g_spiSendIndex++]=menuPara.mstTtl;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//if(!g_paraUpdateFlag)
			//{
			//	OSTmrStart(g_paraUpdateTmr, &tmrErr) ;
			//	g_paraUpdateFlag = 1;
			//}
//#else
//			g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
//#endif
			//value = Parameter_Array[PARA_COMP];
			break;

		//indicate how many devices have linked to the master
		case 0xD6:				//surpose all groups have device monted
			//g_spiRcvData[g_spiRcvIndex++]=0xD6;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				//g_spiSendData[g_spiSendIndex++]=0x0E;
				//g_spiSendData[g_spiSendIndex++]=0x3E;
				/*
				g_spiSendData[g_spiSendIndex++]=(((WORKINGMODE_TTL==radioSet->rfGr[4].mode)&&(flaPara.rfCmuPara[4].totalLinkedDevice>0))<<5|
												     ((WORKINGMODE_TTL==radioSet->rfGr[3].mode)&&(flaPara.rfCmuPara[4].totalLinkedDevice>0))<<4|
												     ((WORKINGMODE_TTL==radioSet->rfGr[2].mode)&&(flaPara.rfCmuPara[4].totalLinkedDevice>0))<<3|
												     ((WORKINGMODE_TTL==radioSet->rfGr[1].mode)&&(flaPara.rfCmuPara[4].totalLinkedDevice>0))<<2|
												     ((WORKINGMODE_TTL==radioSet->rfGr[0].mode)&&(flaPara.rfCmuPara[4].totalLinkedDevice>0))<<1);
				*/
/*				
				g_spiSendData[g_spiSendIndex++]=((WORKINGMODE_TTL==radioSet->rfGr[4].mode)<<5|
													(WORKINGMODE_TTL==radioSet->rfGr[3].mode)<<4|
													(WORKINGMODE_TTL==radioSet->rfGr[2].mode)<<3|
													(WORKINGMODE_TTL==radioSet->rfGr[1].mode)<<2|
													(WORKINGMODE_TTL==radioSet->rfGr[0].mode)<<1);
*/
				g_spiSendData[g_spiSendIndex++]=(((!radioSet->rfGr[4].isOff)&&(WORKINGMODE_TTL==radioSet->rfGr[4].mode)&&(radioStatue.devCnt[4]>0))<<5|
													((!radioSet->rfGr[3].isOff)&&(WORKINGMODE_TTL==radioSet->rfGr[3].mode)&&(radioStatue.devCnt[3]>0))<<4|
													((!radioSet->rfGr[2].isOff)&&(WORKINGMODE_TTL==radioSet->rfGr[2].mode)&&(radioStatue.devCnt[2]>0))<<3|
													((!radioSet->rfGr[1].isOff)&&(WORKINGMODE_TTL==radioSet->rfGr[1].mode)&&(radioStatue.devCnt[1]>0))<<2|
													((!radioSet->rfGr[0].isOff)&&(WORKINGMODE_TTL==radioSet->rfGr[0].mode)&&(radioStatue.devCnt[0]>0))<<1|
													(((*wlSet.is_masterOn>0)&&(!radioSet->rfGr[0].isOff)&&(WORKINGMODE_TTL==radioSet->rfGr[0].mode))<<1));
			}else
			{
				//switch(*wlSet.arr_mode)
				switch((WORKINGMODE_TTL==mainPara.workMode)?*wlSet.ttl_Ratio:*wlSet.arr_mode)
				{
					case WLM_MODE_ALL:
						//g_spiSendData[g_spiSendIndex++]=((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0));//0x01;
						g_spiSendData[g_spiSendIndex++]=((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0)||(*wlSet.is_masterOn>0));
					break;

					case WLM_MODE_AB:
						//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2));//0x06;
						//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|(*wlSet.is_masterOn>0));
						g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((*wlSet.is_masterOn>0)<<1));
					break;

					case WLM_MODE_RES:
						//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3));//0x0E;
						//g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3)|(*wlSet.is_masterOn>0));
						g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3)|((*wlSet.is_masterOn>0)<<1));
					break;
				}
			}
			/*
			if(WLM_MODE_ALL==wlSet.arr_mode)
			{
				g_spiSendData[g_spiSendIndex++]=0x01;
			}else
			{
				if(wlSet.rfSet.devLinked>0)
				{
					g_spiSendData[g_spiSendIndex++]=0x06;
				}else
				{
					g_spiSendData[g_spiSendIndex++]=0x02;
				}
			}*/
			
			//g_spiSendData[g_spiSendIndex++]=0x01;
		break;

		case 0xd5:
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
		break;

		case 0xD2:
			//g_spiRcvData[g_spiRcvIndex++]=0xD2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x00;		
			if(UART_CC_REL_FLA==g_shutterFlag)						//shutter control, only used in camera issued after 2011
			{
				g_spiSendData[g_spiSendIndex++]=0x11;//g_shutterFlag?(0x11;g_shutterFlag=0):0x00;
				g_shutterFlag = 0;
			}else if(UART_CC_REL_MODEL==g_shutterFlag)
			{
				g_spiSendData[g_spiSendIndex++]=0x22;
				g_shutterFlag = 0;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
			}
			//0x00;
		break;

		case 0x01:
			//g_spiRcvData[g_spiRcvIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;
		
		case 0xe2:
			//g_spiRcvData[g_spiRcvIndex++]=0xe2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=menuPara.motoCod;//Parameter_Array[PARA_FOCIND];			//焦距模式
			break;
		case 0xea:						//determine the Wireless command mode: RF,LIGHT,OFF
			//g_spiRcvData[g_spiRcvIndex++]=0xea;
			g_spiSendData[g_spiSendIndex++]=0x8e;							
			g_spiSendData[g_spiSendIndex++]=*wlSet.m_chan;
			g_spiSendData[g_spiSendIndex++]=0x03;//0x03;
			
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					if(WORKINGMODE_TTL==mainPara.workMode)
					{
						g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.ttl_Ratio)<<4)|((*wlSet.is_masterOn)<<3));
					}else
					{
						g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.arr_mode)<<4)|((*wlSet.is_masterOn)<<3));
					}
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++]=0x01;
				break;

				default:
					if(WORKINGMODE_TTL==mainPara.workMode)
					{
						g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.ttl_Ratio)<<4));
					}else
					{
						g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.arr_mode)<<4));
					}
				break;
			}
			/*
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.arr_mode)<<4)|((*wlSet.is_masterOn)<<3));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.arr_mode)<<4));
				
			}*/
			break;
			
		case 0xeb:
		//g_spiRcvData[g_spiRcvIndex++]=0xeb;
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
				g_spiSendData[g_spiSendIndex++]=0x8e;
				g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;//0x00;
				//g_spiSendData[g_spiSendIndex++]=0x00;
				//switch(*wlSet.arr_mode)
				switch(*wlSet.ttl_Ratio)
				{
					case WLM_MODE_AB:
						g_spiSendData[g_spiSendIndex++]=*wlSet.abRatio;
					break;

					case WLM_MODE_RES:
						//g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;
						g_spiSendData[g_spiSendIndex++]=*wlSet.abRatio;
						//wlSet.m_arrPara.ABRation = g_spiRcvData[3];
					break;

					default :
						g_spiSendData[g_spiSendIndex++]=0;
					break;
				}
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
				case CMD_MODE_OFF:
					g_spiSendData[g_spiSendIndex++]=0x8e;
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x06;
				break;
				
			}
		break;
		
		case 0xec:
		g_spiSendData[g_spiSendIndex++]=0x8e;
#ifdef _MULTIPLY_FLASH_
		if(WORKINGMODE_MULTI==mainPara.workMode)
		{
			g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[WL_ARRARY_C];
			g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[WL_ARRARY_B];
			//g_spiSendData[g_spiSendIndex++]=multiSet.WlmMul[WL_ARRARY_A];
			g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[WL_ARRARY_A];//menuPara.WlmMul[WL_ARRARY_A]&0x7F;//multiSet.cmdCode&0x7F;
		}else
#endif
		{						
			//g_spiSendData[g_spiSendIndex++]=wlSet.m_arrPara.CotpCode;
			//g_spiSendData[g_spiSendIndex++]=wlSet.m_arrPara.BotpCode;
			g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[WL_ARRARY_C];//wlSet.otpCode[WL_ARRARY_C];
			g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[WL_ARRARY_B];//wlSet.otpCode[WL_ARRARY_B];
			g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER]&0x7f;//wlSet.m_arrPara.AotpCode;
		}
		break;
		
		case 0xe6:
			//g_spiRcvData[g_spiRcvIndex++]=0xe6;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=menuPara.feb;//Parameter_Array[PARA_SRD];			//闪光包围
			//g_spiSendData[g_spiSendIndex++]=(VER_CAMERA_OLD==g_vCmr)?(WORKINGMODE_GR==mainPara.workMode)?():((mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
			if((WORKINGMODE_GR==mainPara.workMode)&&(VER_CAMERA_OLD==g_vCmr))
			{
				g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(WORKINGMODE_TTL&0x07));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(mainPara.workMode&0x07));
			}
			//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC1];//mainPara.workMode;//Parameter_Array[PARA_SYNC1];//mainPara.workMode;//0x00;//Parameter_Array[PARA_SYNC1];			//mode闪光模式
			//g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=0)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07)|(g_activeControl<<5));
//#ifdef _ST_EX_
//			g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=CMD_MODE_OFF)<<4)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
//#else
//			g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=CMD_MODE_OFF)<<4)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
//#endif
			g_spiSendData[g_spiSendIndex++]=0x7F;//0x3f;
			break;
			
		case 0xe7:
			//g_spiRcvData[g_spiRcvIndex++]=0xe7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x10;//0x64;//multiSet.maxTimes;//0x10;
			//g_spiSendData[g_spiSendIndex++]=multiSet.eTimes;//Parameter_Array[PARA_MOUT];
			//g_spiSendData[g_spiSendIndex++]=multiSet.frequence;//Parameter_Array[PARA_FREQ];
			//g_spiSendData[g_spiSendIndex++]=0x38;
#ifdef _MULTIPLY_FLASH_
			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=(multiSet.maxTimes>0)?multiSet.maxTimes:10;//0x5a;//0x64;//0x10;//0x64;//multiSet.maxTimes;//0x10;
				g_spiSendData[g_spiSendIndex++]=*multiSet.eTimes;//Parameter_Array[PARA_MOUT];
				g_spiSendData[g_spiSendIndex++]=*multiSet.frequence;//Parameter_Array[PARA_FREQ];
				g_spiSendData[g_spiSendIndex++]=0x38;
				g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[MASTER];
				//g_spiSendData[g_spiSendIndex++]=multiSet.cmdCode;
			}else
#endif
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x38;
				g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];//Parameter_Array[PARA_VOUT];menuPara.otpCod
				
/*
				if(WLM_ON==*wlSet.m_on)
				{
					g_spiSendData[g_spiSendIndex++] = wlSet.m_arrPara.AotpCode;
				}else
				{
					g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_VOUT];
				}
*/
			}
			break;
		case 0xa7:
			//g_spiRcvData[g_spiRcvIndex++]=0xa7;
#ifdef CANON_RF
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
#endif
			break;
		case 0xa6:
			//g_spiRcvData[g_spiRcvIndex++]=0xa6;
#ifdef CANON_RF
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x91;
#endif
			break;
		case 0xb4:
			//g_spiRcvData[g_spiRcvIndex++]=0xb4;
//							g_spiSendData[g_spiSendIndex++]=0x8e;
//							g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xb0:
			//g_spiRcvData[g_spiRcvIndex++]=0xb0;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
//							pre_flash = 1;
			break;
		case 0xb1:
			//g_spiRcvData[g_spiRcvIndex++]=0xb1;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xf8:
			//g_spiRcvData[g_spiRcvIndex++]=0xf8;
//			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0xff;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x4C;
			g_spiSendData[g_spiSendIndex++]=0xff;
			//g_spiSendData[g_spiSendIndex++]=0x5d;
			break;
		case 0xb7:
			//g_spiRcvData[g_spiRcvIndex++]=0xb7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xb8:
			//g_spiRcvData[g_spiRcvIndex++]=0xb8;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x91;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
		case 0xa8:
			//g_spiRcvData[g_spiRcvIndex++]=0xa8;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x97;//0x96;//0x91;
			g_spiSendData[g_spiSendIndex++] = 0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			break;

		case 0xC4:
			//g_spiRcvData[g_spiRcvIndex++]=0xC4;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			g_spiSendData[g_spiSendIndex++]=0x8e;	
		break;

		case 0xC2:
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[4];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[3];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[2];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[1];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[0];//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;	
			}else
			{
				if(WLM_MODE_ALL==*wlSet.ttl_Ratio)
				{
					g_spiSendData[g_spiSendIndex++]=0x8e;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
					g_spiSendData[g_spiSendIndex++]=0x8e;	
				}else
				{
					g_spiSendData[g_spiSendIndex++]=0x8e;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=0x00;	
					g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;	
					g_spiSendData[g_spiSendIndex++]=abRation_fill2;	
					g_spiSendData[g_spiSendIndex++]=abRation_fill1;//0x80;//0x00;	cause serious problem 
					g_spiSendData[g_spiSendIndex++]=0x8e;	
				}
				
			}
#if 0
			//g_spiRcvData[g_spiRcvIndex++]=0xC2;
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;	
			g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
			g_spiSendData[g_spiSendIndex++]=0x8e;	
#endif
		break;

		case 0xC8:
			//g_spiRcvData[g_spiRcvIndex++]=0xC8;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;

		case 0xCA:
			//g_spiRcvData[g_spiRcvIndex++]=0xCA;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;

		case 0xC7:
			//g_spiRcvData[g_spiRcvIndex++]=0xC7;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
		break;

		case 0xCC:
			//g_spiRcvData[g_spiRcvIndex++]=0xCC;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
		break;

		case 0xD7:
			//g_spiRcvData[g_spiRcvIndex++]=0xD7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=(multiSet.maxTimes>0)?multiSet.maxTimes:10;//multiSet.maxTimes;
				g_spiSendData[g_spiSendIndex++]=*multiSet.eTimes;
				g_spiSendData[g_spiSendIndex++]=(*multiSet.frequence)&0xFF;
				g_spiSendData[g_spiSendIndex++]=(*multiSet.frequence)>>8;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
			}
			g_spiSendData[g_spiSendIndex++]=0x38;
			switch(mainPara.workMode)
			{
				case WORKINGMODE_TTL:
				case WORKINGMODE_GR:
					g_spiSendData[g_spiSendIndex++]=0x00;
				break;

				case WORKINGMODE_MANU:
					g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];
				break;

				case WORKINGMODE_MULTI:
					//g_spiSendData[g_spiSendIndex++]=multiSet.cmdCode;
					g_spiSendData[g_spiSendIndex++]=menuPara.WlmMul[MASTER];
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x00;
				break;
			}
		break;

		case 0xD8:										//ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xD8;
#ifdef _ST_EX_
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x7f;
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x13;
			g_spiSendData[g_spiSendIndex++]=0x7f;
			g_spiSendData[g_spiSendIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=motoSet.focLen;//0x05;
#endif
		break;
		
		case 0xa3:
			//g_spiRcvData[g_spiRcvIndex++]=0xa3;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			break;
			
		case 0xf2:
			//g_spiRcvData[g_spiRcvIndex++]=0xf2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xa0;//cameraSetting.prefla_com;
			break;
		case 0xfc:	 
			//g_spiRcvData[g_spiRcvIndex++]=0xfc;
#if 0
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x32;
			g_spiSendData[g_spiSendIndex++]=0x85;
			g_spiSendData[g_spiSendIndex++]=0x21;
			g_spiSendData[g_spiSendIndex++]=0x1a;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
#endif
			
			g_spiSendData[g_spiSendIndex++]=0x8E;//0x8e;
			g_spiSendData[g_spiSendIndex++]=g_colorTempRt&0xFF;//0x32;
			g_spiSendData[g_spiSendIndex++]=(g_colorTempRt>>8)&0xFF;//0x85;
			g_spiSendData[g_spiSendIndex++]=(g_colorTempRt>>16)&0xFF;//0x21;
			g_spiSendData[g_spiSendIndex++]=(g_colorTempRt>>24)&0xFF;//0x1a;
			g_spiSendData[g_spiSendIndex++]=0xC0;
			g_spiSendData[g_spiSendIndex++]=ttl_expCodeRsv;
			break;
		case 0xa2:
			//g_spiRcvData[g_spiRcvIndex++]=0xa2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
		break;
		case 0xe1:
			//g_spiRcvData[g_spiRcvIndex++]=0xe1;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x91;
			//SETBIT(bitExtra,BIT_bitExtra_A8index);
#if 0							
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xf8;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0d;
			g_spiSendData[g_spiSendIndex++]=0x3c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x3c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c; 
#else
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xf8;

			for(i=23;i>19;i--)
			{
				g_spiSendData[g_spiSendIndex++] = (cfnCntMask[i]|0x0C|pSnrSet[i-6]);
			}
			for(i=19;i>13;i--)
			{
				g_spiSendData[g_spiSendIndex++] = 0x00;
			}
			for(i=13;i>=0;i--)
			{
				g_spiSendData[g_spiSendIndex++] = (cfnCntMask[i]|0x0C|pSnrSet[i]);;
			}
			//g_spiSendData[g_spiSendIndex++] = 0xF8;
			
			//g_spiSendData[g_spiSendIndex++]=0x0C;
			//g_spiSendData[g_spiSendIndex++]=0x1C;
			//g_spiSendData[g_spiSendIndex++]=0x1C;
			//g_spiSendData[g_spiSendIndex++]=0x0D;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x00;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0C;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x3F;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
			//g_spiSendData[g_spiSendIndex++]=0x0c;
#endif
			break;	
		case 0xaa:
			//g_spiRcvData[g_spiRcvIndex++]=0xaa;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			break;	
		case 0xab:
			//g_spiRcvData[g_spiRcvIndex++]=0xab;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
		break;
		case 0xac:
			//g_spiRcvData[g_spiRcvIndex++]=0xac;
			g_spiSendData[g_spiSendIndex++]=0x8e; 						
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
		break;

		case 0xDA:											//just for st-e3
			//g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(radioSet->id>>8); 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfChan;//wlSet.rfSet.chan;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=(0x04|(*wlSet.ttl_Ratio<<5)|(*wlSet.is_masterOn<<4));
				}else
				{
					g_spiSendData[g_spiSendIndex++]=(0x04|(*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
				}
			}else
			{
				if(WORKINGMODE_TTL==mainPara.workMode)
				{
					g_spiSendData[g_spiSendIndex++]=((*wlSet.ttl_Ratio<<5)|(*wlSet.is_masterOn<<4));
				}else
				{
					g_spiSendData[g_spiSendIndex++]=((*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
				}
			}
			//g_spiSendData[g_spiSendIndex++]=(0x10|(wlSet.arr_mode<<5));
		/*
		if(CMD_MODE_RF==*g_cmdMode)
		{
			g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id>>8); 
			g_spiSendData[g_spiSendIndex++]=wlSet.rfSet.chan;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			//g_spiSendData[g_spiSendIndex++]=(0x14|(radioSet.ratio<<5));
			g_spiSendData[g_spiSendIndex++]=(0x14|(wlSet.arr_mode<<5));
			//g_spiSendData[g_spiSendIndex++]=(0x10|(wlSet.arr_mode<<5));
		}else if(CMD_MODE_LIGH==*g_cmdMode)
		{
			g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x00; 						
			g_spiSendData[g_spiSendIndex++]=0x00; 
			g_spiSendData[g_spiSendIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			g_spiSendData[g_spiSendIndex++]=(0x10|(wlSet.arr_mode<<5));
		}else
		{
			g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(wlSet.rfSet.id>>8); 
			g_spiSendData[g_spiSendIndex++]=wlSet.rfSet.chan;;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			g_spiSendData[g_spiSendIndex++]=0x10;
		}*/
		break;

		case 0xC3:
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
		break;

		//RF mode 
		//0x00: ETTL
		//0x01: MANU
		//0x04: Ext.A Auto external lightting measure
		//0x06: OFF
		case 0xDB:
			//g_spiRcvData[g_spiRcvIndex++]=0xDB;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[4].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[4]:menuPara.rfGrTmp[4];//flaPara.rfCmuPara[4].optCode;//rfGrPara.rfGr_arrPara[4].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=GrModConvert(4);//radioSet->rfGr[4].mode;//rfGrPara.rfGr_arrPara[4].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[3].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[3]:menuPara.rfGrTmp[3];//flaPara.rfCmuPara[3].optCode;//rfGrPara.rfGr_arrPara[3].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=GrModConvert(3);//radioSet->rfGr[3].mode;//rfGrPara.rfGr_arrPara[3].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[2].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[2]:menuPara.rfGrTmp[2];//flaPara.rfCmuPara[2].optCode;//rfGrPara.rfGr_arrPara[2].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=GrModConvert(2);//radioSet->rfGr[2].mode;//rfGrPara.rfGr_arrPara[2].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[1].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[1]:menuPara.rfGrTmp[1];//flaPara.rfCmuPara[1].optCode;//rfGrPara.rfGr_arrPara[1].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=GrModConvert(1);//radioSet->rfGr[1].mode;//rfGrPara.rfGr_arrPara[1].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[0].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[0]:menuPara.rfGrTmp[0];//flaPara.rfCmuPara[0].optCode;//rfGrPara.rfGr_arrPara[0].optCode; 
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=GrModConvert(0);//radioSet->rfGr[0].mode;//rfGrPara.rfGr_arrPara[0].mode;//0x01; 
			g_spiSendData[g_spiSendIndex++]=0x0F; 
		break;

		case 0xCB:										//just for st-e3
			//g_spiRcvData[g_spiRcvIndex++]=0xCB;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
		break;
		
		case 0xbe:
			//g_spiRcvData[g_spiRcvIndex++]=0xbe;
		    	g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			break;
			
		case 0x7e:
			//SPIx_Disable();
			//g_spiRcvData[g_spiRcvIndex++]=0x7e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//SPIx_Enable();
			g_spiSendData[g_spiSendIndex++]=0xd9;
			g_spiSendData[g_spiSendIndex++]=0x82;
			g_spiSendData[g_spiSendIndex++]=0xbc;
			g_spiSendData[g_spiSendIndex++]=0x18;
			g_spiSendData[g_spiSendIndex++]=0xbe;
			g_spiSendData[g_spiSendIndex++]=0xbe; 
			break;	
			
		default:
			//SPIx_Disable();
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//SPIx_Enable();
			break;
	}
}

unsigned char CanInquire(unsigned char mac,unsigned char cmdSta)
{
	if(RADIO_STATUE_PRE==cmdSta)
	{
		switch(mac)
		{
			case 0x64:		//1//700D
			return 0;
			//break;

			case 0x55:		//1//5D MARKIII
			return 1;

			case 0x45:		//1//7DMARKII
			case 0x43:
			return 3;

			case 0x3A:		//1//1DX
			case 0x39:
			case 0x38:
			return 2;

			default:
#ifdef _1DX_DEBUG_
			return 0xff;
#else
			//return ((mac>64)?0:1);
			switch(mac&0xf0)
			{
				case 0x60:
				return 0;

				case 0x50:
				return 1;

				case 0x40:
				return 3;

				case 0x30:
				return 2;

				default:
				return 2;
			}
#endif
		}
	}else
	{
		switch(mac)
		{
			case 0x54:		//1//700d 1DX
			return 0;

			case 0x65:		//1//5D MARKIII
			case 0x67:		
			case 0x57:		//1//6D
			return 1;

			default:
#ifdef _1DX_DEBUG_
			return 0xff;
#else
			return ((mac>54)?1:0);
#endif
		}
	}
}



void FlashHandle(void)
{
	unsigned int tmp;
//	s8 i=0;
	static char info_get=0;
	//static char menuLockRsv=0;
	//static char motoMod=0;
	//static char motoDest=0;

	if(info_get>0)
	{
		//pSnrSet[(g_spiRcvData[1]>>3)&0x1F] = (g_spiRcvData[1]&0x07);
		tmp = (g_spiRcvData[1]>>3);
		if(tmp>=20) tmp-=6;
		pSnrSet[tmp] = (g_spiRcvData[1]&0x07);
		//g_spiSendData[g_spiSendIndex++] = (cfnCntMask[0]|0x0C|pSnrSet[0]);
		//g_spiSendData[g_spiSendIndex++] = 0x4C;
		g_spiSendData[g_spiSendIndex++] = 0x4C;//g_spiRcvData[0];//0x4C;
		info_get = 0;
		SpiRearUp(4000);
		return;
	}
	
	switch(g_spiRcvData[0])
	{
		case 0xb4:
		switch(g_spiRcvData[1])
		{
			case 0x23: 			// 2//be ready for previous flash
			//NrfWarnStart(g_nrfBuf,NRF_HEAD_CFM_PRE);
			//SC4MD1 &= 0xCF;		//suspend clock syn transfer
			SPIx_Disable();
			//FLASH_OUT = 0;
			SPI_MISO_AUX = 0;
			g_comStatue= COM_STATUE_PREFLASH;
			g_mstPrefla = 1;
			// 1/*the following process is about to set interrupt0 to hold mode*/
#ifdef WIRELESS_MASTER_ON
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				//if((WLM_MODE_ALL==*wlSet.arr_mode))
				if(WLM_MODE_ALL==*wlSet.ttl_Ratio)
				{
#ifdef LIGH_CMD_REBUILD
					wlSet.m_flaCmd[0]=0x30;
					wlSet.m_flaCmd[1]=0xA0;
					wlSet.m_cmdIndex = 2;
#else
					wlSet.m_flaCmd[0]=0x30;
					wlSet.m_flaCmd[1]=0x82;
					wlSet.m_flaCmd[2]=0x02;
					wlSet.m_cmdIndex = 3;
#endif
					wlSet.m_cmdSendId = WLM_CMDSEND_PREFLA_ALL;
					
					wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
					wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;	
					//WLMCmdSend();
					OSTaskResume(CMDSEND_TASK_Prio);
					//WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
					FLASH_OUT = 0;
				}else
				{
					FLASH_OUT = 1;
				}
				ENABLE_D30EN();
				break;

				case CMD_MODE_RF_MST:
					//flaPara.preflashIndex =1;
				flaPara.preflashIndex =0;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
				if(*wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}

				OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case CMD_MODE_OFF:
				SpiRearUp(100);
				radioStatue.cmdReady = 1;
				wlSet.m_cmdProc = WLM_CMDPROC_DAT;
				ENABLE_D30EN();
				//ENABLE_D30EN_EX();
				OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						//SpiRearUp(100);
						SpiRearUp(100);
						radioStatue.cmdReady = 1;
						wlSet.m_cmdProc = WLM_CMDPROC_DAT;
						radioStatue.cmdSta = RADIO_STATUE_PREDAT;
						ENABLE_D30EN();
						//ENABLE_D30EN_EX();
						OSTaskResume(CMDSEND_TASK_Prio);
					}else
					{
						SpiRearUp(100);
						radioStatue.cmdSta = RADIO_STATUE_PRE;
						radioStatue.adept = CMR_ADEPT_OLD;
						radioStatue.cmdReady = 1;
						OSTaskResume(CMDSEND_TASK_Prio);
					}
				break;
				
			}
#if 0
			//if((WLM_ON==*wlSet.m_on)/*&&(WLM_MODE_ALL==wlSet.m_wlMode)*/)
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				//if((WLM_ON==*wlSet.m_on)&&(WLM_MODE_ALL==*wlSet.arr_mode))
				if((WLM_MODE_ALL==*wlSet.arr_mode))
				{
#ifdef LIGH_CMD_REBUILD
					wlSet.m_flaCmd[0]=0x30;
					wlSet.m_flaCmd[1]=0xA0;
					wlSet.m_cmdIndex = 2;
#else
					wlSet.m_flaCmd[0]=0x30;
					wlSet.m_flaCmd[1]=0x82;
					wlSet.m_flaCmd[2]=0x02;
					wlSet.m_cmdIndex = 3;
#endif
					wlSet.m_cmdSendId = WLM_CMDSEND_PREFLA_ALL;
					
					wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
					wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;	
					//WLMCmdSend();
					OSTaskResume(CMDSEND_TASK_Prio);
					//WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
					FLASH_OUT = 0;
				}else
				{
					FLASH_OUT = 1;
				}
				ENABLE_D30EN();
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				//flaPara.preflashIndex =1;
				flaPara.preflashIndex =0;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
				if(*wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}

				OSTaskResume(CMDSEND_TASK_Prio);
			}else if(CMD_MODE_OFF==*g_cmdMode)
			{
				SpiRearUp(100);
				radioStatue.cmdReady = 1;
				wlSet.m_cmdProc = WLM_CMDPROC_DAT;
				ENABLE_D30EN();
				//ENABLE_D30EN_EX();
				OSTaskResume(CMDSEND_TASK_Prio);
			}
			else
			{
				//flaPara.preflashIndex =0;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
				OSTaskResume(CMDSEND_TASK_Prio);
			}
#endif
#endif
			break;

		
#ifdef WIRELESS_MASTER_ON

			case 0x63:
			//NrfWarnStart(g_nrfBuf,NRF_HEAD_CFM_PRE);
			//SC4MD1 &= 0xCF;
			SPIx_Disable();
			SPI_MISO_AUX = 0;
			//FLASH_OUT = 0;
			//wlSet.m_preIndex=1;
			//FLASH_OUT = 1;				//pull down data output pin, till the flash comand has been finished
			g_comStatue= COM_STATUE_PREFLASH;			
			g_mstPrefla = 1;
			//if(WLM_ON== *wlSet.m_on)
			//if(CMD_MODE_OFF!=*g_cmdMode)
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
#ifdef LIGH_CMD_REBUILD
				wlSet.m_flaCmd[0]=0x31;
				wlSet.m_flaCmd[1]=0xA0;
				wlSet.m_cmdIndex = 2;
#else
				wlSet.m_flaCmd[0]=0x31;
				wlSet.m_flaCmd[1]=0x82;
				wlSet.m_flaCmd[2]=0x02;
				wlSet.m_cmdIndex = 3;
#endif
				wlSet.m_cmdSendId = WLM_CMDSEND_PREFLA_CHA;				
				//wlSet.m_cmdProc = WLM_CMDPROC_ON;
				wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
				wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;	
				ENABLE_D30EN();
				//WLMCmdSend();
				OSTaskResume(CMDSEND_TASK_Prio);
				
				//WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				flaPara.preflashIndex=1;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
				if(*wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}
				OSTaskResume(CMDSEND_TASK_Prio);
			}
//			wlSet.m_preIndex=1;
//			FLASH_OUT = 1;
//			com_statue = COM_STATUE_PREFLASH;
			break;

			case 0xA3:
			SPIx_Disable();
			SPI_MISO_AUX = 0;
			//FLASH_OUT = 0;
			//wlSet.m_preIndex=2;
//			FLASH_OUT = 1;			//keep pulling down till the command being finished
			g_comStatue= COM_STATUE_PREFLASH;
			g_mstPrefla = 2;
			//if(WLM_ON== *wlSet.m_on)
			//if(CMD_MODE_OFF!=*g_cmdMode)
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
#ifdef LIGH_CMD_REBUILD
				wlSet.m_flaCmd[0]=0x32;
				wlSet.m_flaCmd[1]=0xA0;
				wlSet.m_cmdIndex = 2;
#else
				wlSet.m_flaCmd[0]=0x32;
				wlSet.m_flaCmd[1]=0x82;
				wlSet.m_flaCmd[2]=0x02;
				wlSet.m_cmdIndex = 3;
#endif
				wlSet.m_cmdSendId = WLM_CMDSEND_PREFLA_CHB;
				if(WLM_CMDFLASH_CONFIRM==wlSet.m_flaConfirm)
				{
					wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;
				}
				//WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
				//if(wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}
				OSTaskResume(CMDSEND_TASK_Prio);
				
				//WLMCmdSend();
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				flaPara.preflashIndex=2;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.cmdReady = 1;
				radioStatue.adept = CMR_ADEPT_OLD;
				if(*wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}

				OSTaskResume(CMDSEND_TASK_Prio);
			}

			break;

			case 0xE3:
			SPIx_Disable();
			SPI_MISO_AUX=0;	
			//FLASH_OUT = 0;
			//wlSet.m_preIndex=3;
//			FLASH_OUT = 1;
			g_comStatue= COM_STATUE_PREFLASH;
			g_mstPrefla = 3;
			//if(WLM_ON== *wlSet.m_on)
			//if(CMD_MODE_OFF!=*g_cmdMode)
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
#ifdef LIGH_CMD_REBUILD
				wlSet.m_flaCmd[0]=0x33;
				wlSet.m_flaCmd[1]=0xA0;
				wlSet.m_cmdIndex = 2;
#else
				wlSet.m_flaCmd[0]=0x33;
				wlSet.m_flaCmd[1]=0x82;
				wlSet.m_flaCmd[2]=0x02;
				wlSet.m_cmdIndex = 3;
#endif
				wlSet.m_cmdSendId = WLM_CMDSEND_PREFLA_CHC;
				if(WLM_CMDFLASH_CONFIRM==wlSet.m_flaConfirm)
				{
					wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;
				}
				//if(wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}

				OSTaskResume(CMDSEND_TASK_Prio);
				//WLMCmdSend();
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				flaPara.preflashIndex=3;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.cmdReady = 1;
				radioStatue.adept = CMR_ADEPT_OLD;
				if(*wlSet.is_masterOn>0)
				{
					ENABLE_D30EN();
				}

				OSTaskResume(CMDSEND_TASK_Prio);
			}
			break;
#endif

#ifdef _HP_SYN_
			case 0x25:
				//NrfWarnStart(g_nrfBuf,NRF_HEAD_CFM_FLA);
				flaPara.hpFlash = HP_FLASHMODE_ON;
				SPIx_Disable();
				//SETBIT(bitIndex,BIT_bitIndex_REALFLASH);
				g_comStatue= COM_STATUE_REALFLASH;
				SPI_MISO_AUX=0;	
				//FLASH_OUT = 0;
#ifdef	WIRELESS_MASTER_ON
				switch(*g_cmdMode)
				{
					case CMD_MODE_LIGH_MST:
						BuildExposeCmd(mainPara.workMode,flaPara.hpFlash,(g_hpLev>0x46));
						wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;
						wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
						CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON);
						ENABLE_D30EN();
						OSTaskResume(CMDSEND_TASK_Prio);
					break;

					case CMD_MODE_RF_MST:
						radioStatue.cmdSta = RADIO_STATUE_FLACFM;
						radioStatue.cmdReady = 1;
						radioStatue.adept = CMR_ADEPT_OLD;
						if(*wlSet.is_masterOn>0)
						{
							CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON);
							ENABLE_D30EN();
						}
						OSTaskResume(CMDSEND_TASK_Prio);
					break;

					case CMD_MODE_OFF:
						//CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON,CALC_FP_SWHEAD);
					ENABLE_D30EN();
					CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON);
#ifdef TTL_DEBUG
					//radioStatue.cmdReady = 1;
					//wlSet.m_cmdProc = WLM_CMDPROC_TTLDEBUG;
					//OSTaskResume(CMDSEND_TASK_Prio);
#endif
					SpiRearUp(100);
					break;

					case CMD_MODE_RFLS_MST:
					case CMD_MODE_RFLS_SLA:
						if(pfnSet[PFN_ID_LSSW]>0)
					{
						ENABLE_D30EN();
						CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON);
					}
						SpiRearUp(100);
					break;
				}
#if 0
				//if(WLM_ON==*wlSet.m_on)
				//if(CMD_MODE_OFF!=*g_cmdMode)
				if(CMD_MODE_LIGH_MST==*g_cmdMode)
				{
					//BuildExposeCmd(mainPara.workMode,flaPara.hpFlash);
					//if((AD_CHARGE_FUL==charge_ok)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT)))
					BuildExposeCmd(mainPara.workMode,flaPara.hpFlash,(g_hpLev>0x46));
					wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;
					wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
					ENABLE_D30EN();
					OSTaskResume(CMDSEND_TASK_Prio);
					//WLMCmdSend();
					
				}else if(CMD_MODE_RF_MST==*g_cmdMode)
				{
					radioStatue.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.cmdReady = 1;
					radioStatue.adept = CMR_ADEPT_OLD;
					if(*wlSet.is_masterOn>0)
					{
						ENABLE_D30EN();
					}
					OSTaskResume(CMDSEND_TASK_Prio);
				}else if(CMD_MODE_OFF==*g_cmdMode)
				{
					//CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON,CALC_FP_SWHEAD);
					ENABLE_D30EN();
					CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_ON);
#ifdef TTL_DEBUG
					//radioStatue.cmdReady = 1;
					//wlSet.m_cmdProc = WLM_CMDPROC_TTLDEBUG;
					//OSTaskResume(CMDSEND_TASK_Prio);
#endif
					SpiRearUp(100);
				}
#endif
#endif
			break;
#endif
			case 0x3d:
			case 0x2d:
//			TESTFLAG = ~TESTFLAG;
/*			SPI0CN = 0;*/
			//NrfWarnStart(g_nrfBuf,NRF_HEAD_CFM_FLA);
			//g_nrfBuf[2] = ttl_expCodeRsv;
			//NrfTransfer(g_nrfBuf,2);
			//NrfFlaConfirm(g_nrfBuf,NRF_HEAD_CFM_FLA,ttl_expCodeRsv);
			//SC4MD1 &= 0xCF;
			SPIx_Disable();
			SPI_MISO_AUX=0;	
//			XBR0 &= 0XFD;
//			P0SKIP |= 0X07;
//			SPI0DAT = 0XFF;
//			real_flash = 1;
//			SETBIT(bitIndex,4);
			//SETBIT(bitIndex,BIT_bitIndex_REALFLASH);
			flaPara.hpFlash = HP_FLASHMODE_OFF;
			CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_OFF);
			//if(WORKINGMODE_MULTI==mainPara.workMode)
			//{
			//	com_statue = COM_STATUE_MULTIFLASH;
			//}else
			{
				g_comStatue = COM_STATUE_REALFLASH;
			}
			//FLASH_OUT = 1;
			CLEAR_EXTINT_FLAG;
			/*
			disable_irq();
			MEMCTR |=  0x04;	
			//IRQ1ICR = 0XA0;
			IRQ0ICR &= 0Xef;
			//IRQCNT &= 0XFD;
			MEMCTR   &= ~0x04;
			enable_irq();*/
#ifdef	WIRELESS_MASTER_ON
			FLASH_OUT = 0;
			//if(WLM_ON== *wlSet.m_on)
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				BuildExposeCmd(mainPara.workMode,flaPara.hpFlash,0);
				wlSet.m_cmdSendId = WLM_CMDSEND_FLAIMF;
				//if((AD_CHARGE_FUL==charge_ok)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT)))				
				wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;
				wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
				OSTaskResume(CMDSEND_TASK_Prio);
				//WLMCmdSend();
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				radioStatue.cmdSta = RADIO_STATUE_FLACFM;
				radioStatue.cmdReady = 1;
				radioStatue.adept = CMR_ADEPT_OLD;
				OSTaskResume(CMDSEND_TASK_Prio);
			}else
			{
				//FLASH_OUT = 1;
				//SPI_MISO_AUX = 1;
				//CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_OFF,CALC_FP_SWHEAD);	
				CanExposeTimeCal(ttl_expCodeRsv,WL_HPSYN_OFF);	
#ifdef TTL_DEBUG
					//radioStatue.cmdReady = 1;
					//wlSet.m_cmdProc = WLM_CMDPROC_TTLDEBUG;
					//OSTaskResume(CMDSEND_TASK_Prio);
#endif
				SpiRearUp(100);
			}
#endif
			if(0x2D==g_spiRcvData[1])                             // 5D markIII has no flash infomation
			{
				// 2018/05/15 鍦ㄦ櫘閫氭ā寮忎笅娌℃湁闂厜
				g_sckFlash = 1;
			}else
			{
				g_sckFlash = 0;
			}
			//FLAOK_TOGGLE();
			//FLASH_OUT = 1;
			break;
 
			case 0x1d:
//			TESTFLAG = ~TESTFLAG;
			//SETBIT(paraBit,BIT_paraBit_COMPENSATION_SET);
//			EX0 = 1;
				//SPI_MISO_AUX=1;	
				SpiRearUp(60);
			break;

			case 0x65:						//MOUDLING
				g_isModeling = 1;
				g_comStatue = COM_STATUE_MODELING;
				SpiRearUp(60);
			break;

			default:
				//SPI_MISO_AUX=1;	
				SpiRearUp(60);
			break;
			


		}
		//SPI_MISO_AUX=1;	
		break;

		case 0xC4:											//st-e3
			switch(g_spiRcvData[2])
			{
				case 0x09:
				case 0x19:
				case 0x29:
				case 0x39:
				case 0x49:
				case 0x59:
					if(*wlSet.is_masterOn>0)
					{
						ENABLE_D30EN();
					}
					g_cmrCode = g_spiRcvData[1];
					flaPara.preflashIndex = (g_spiRcvData[2]>>4)&0x0F;
					/*								//why? cause ETTL ALL mode flash too high
					if(!flaPara.preflashIndex)
					{
						flaPara.preflashIndex= 1;
					}*/
					//wlSet.rfSet.cmdReady = 1;
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_PRE;
					radioStatue.cmdSta = RADIO_STATUE_PRE;
					radioStatue.adept = CMR_ADEPT_NEW;
					g_comStatue= COM_STATUE_PREFLASH;
					//radioStatue.cmpFlag = 1;				//SN debug delete
#if 0
					if((g_spiRcvData[1]&0x0F)>4)
					{
						radioStatue.delay_mod = 1;			// different  in 5dmarkIII 
					}else
					{
						radioStatue.delay_mod = 0;
					}
#endif
					radioStatue.delay_mod = CanInquire(g_spiRcvData[1], radioStatue.cmdSta);
#ifdef _1DX_DEBUG_
					if(0xFF==radioStatue.delay_mod)
					{
						g_dspUpdateFlag |= DSP_UPDATE_DEBUG;
						g_UnknowData[g_UkdCnt++] = MAKEWORD(g_spiRcvData[1], g_spiRcvData[2]);
					}
#endif
					SPIx_Disable();
					FLASH_OUT = 0;	
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x0A:
					g_cmrCode = g_spiRcvData[1];
					flaPara.hpFlash = HP_FLASHMODE_ON;
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.cmdSta = RADIO_STATUE_FLACFM;
					if(*wlSet.is_masterOn>0)
					{
						ENABLE_D30EN();
						CanExposeTimeCal(flaPara.rfCmuPara[0].ttlExpVal,WL_HPSYN_ON);
					}
					//radioStatue.delay_mod = 0;
/*
					if((g_spiRcvData[1]&0x0F)>4)
					{
						radioStatue.delay_mod = 1;			// different  in 5dmarkIII 
					}else
					{
						radioStatue.delay_mod = 0;
					}
*/
					radioStatue.delay_mod = CanInquire(g_spiRcvData[1], radioStatue.cmdSta);
#ifdef _1DX_DEBUG_
					if(0xFF==radioStatue.delay_mod)
					{
						g_dspUpdateFlag |= DSP_UPDATE_DEBUG;
						g_UnknowData[g_UkdCnt++] = MAKEWORD(g_spiRcvData[1], g_spiRcvData[2]);;
					}
#endif
					radioStatue.adept = CMR_ADEPT_NEW;
					g_comStatue = COM_STATUE_REALFLASH;
					SPIx_Disable();
					FLASH_OUT = 0; 
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x0E:
					g_cmrCode = g_spiRcvData[1];
					flaPara.hpFlash = HP_FLASHMODE_OFF;
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.cmdSta = RADIO_STATUE_FLACFM;
					
					//radioStatue.delay_mod = 0;
/*
					if((g_spiRcvData[1]&0x0F)>4)
					{
						radioStatue.delay_mod = 1;			// different  in 5dmarkIII 
					}else
					{
						radioStatue.delay_mod = 0;
					}
*/
					if(*wlSet.is_masterOn>0)
					{
						ENABLE_D30EN();
						CanExposeTimeCal(flaPara.rfCmuPara[0].ttlExpVal,WL_HPSYN_OFF);
					}
					
					radioStatue.delay_mod = CanInquire(g_spiRcvData[1], radioStatue.cmdSta);
#ifdef _1DX_DEBUG_
					if(0xFF==radioStatue.delay_mod)
					{
						g_dspUpdateFlag |= DSP_UPDATE_DEBUG;
						g_UnknowData[g_UkdCnt++] = MAKEWORD(g_spiRcvData[1], g_spiRcvData[2]);;
					}
#endif
					radioStatue.adept = CMR_ADEPT_NEW;
					g_comStatue = COM_STATUE_REALFLASH;
					SPIx_Disable();
					FLASH_OUT = 0; 
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x0F:
					g_cmrCode = g_spiRcvData[1];
					g_dspUpdateFlag |= DSP_SLACMD_MODEL;
					//radioStatue.cmdReady = 1;
					//radioStatue.cmdSta = RADIO_STATUE_MODEL;
					g_comStatue = COM_STATUE_MODEL;
					SPIx_Disable();
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				default:
					SpiRearUp(100);
				break;
				
			}
			//g_spiRcvData[1] = 0;
			//g_spiRcvData[2] = 0;
		break;

		case 0xA5:				//this is in order to compliment with 700d
		//in the case of 600ex&700d, there if no (0xA5 0x6D 0x01) at RF mode, at present at light cmd mode
		if((g_spiRcvData[1]&0xf0)>=0x60)				//
		{
			//g_spiSendData[g_spiSendIndex++] = 0x97;
			g_spiSendData[g_spiSendIndex++] = 0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			if((g_spiRcvData[1]&0x0f)==0x0d)   	//this determined the flash code 0xb4 1d b4 3d
			{
				//Parameter_Array[PARA_A5] = 0x01;
				if(g_cmrA9Id == 0x11)                          // 5d MarkIII
				{
					 Parameter_Array[PARA_A5] = 0x00;           // FOR 5DMARKIII NO FLASH 2016/08/11
				}else
				{
					 Parameter_Array[PARA_A5] = 0x01;           
				}
			}else{ 
				Parameter_Array[PARA_A5] = 0x00;
			}
			g_vCmr = VER_CAMERA_NEW;
		}else
		{
			g_vCmr = VER_CAMERA_OLD;
		}
		//SPI_MISO_AUX=1;	
		SpiRearUp(20);
		break;
/*
		case 0xF8:
			if(0xE1==g_spiRcvData[1])
			{
				g_spiSendData[g_spiSendIndex++]=0x8e;
				g_spiSendData[g_spiSendIndex++]=0xf8;

				for(i=23;i>19;i--)
				{
					g_spiSendData[g_spiSendIndex++] = (cfnCntMask[i]|0x0C|pSnrSet[i-6]);
				}
				for(i=19;i>13;i--)
				{
					g_spiSendData[g_spiSendIndex++] = 0x00;
				}
				for(i=13;i>0;i--)
				{
					g_spiSendData[g_spiSendIndex++] = (cfnCntMask[i]|0x0C|pSnrSet[i]);
				}
			}else
			{
				g_spiSendData[g_spiSendIndex++] = 0x00;
			}
			SpiRearUp(20);
		break;
*/

		case 0xA1:
			//if(0x5227==MAKEWORD(g_spiRcvData[2],g_spiRcvData[1]))
			if(0x52==g_spiRcvData[1])
			{
				g_spiSendData[g_spiSendIndex++] = 0x4C;
				g_spiSendData[g_spiSendIndex++] = 0x4C;
				info_get = 1;
				//pSnrSet[(g_spiRcvData[2]>>3)&0x1F] = (g_spiRcvData[3]&0x07);
			}
			SpiRearUp(20);
		break;

		case 0xB5:
			//if((!g_menuLock)&&(0x4E==g_spiRcvData[1]))
			//if((!g_menuLock)&&(g_spiRcvData[1]&0x02))
			if(g_menuLock>0)
			{
				if(!(g_spiRcvData[1]&0x02))
				{
					g_menuLock = 0;
					//mainPara.workMode = menuLockRsv;
					//motoSet.mode = motoMod;
					//motoSet.dest_location = motoDest;
					mainPara.workMode = g_menuModRsv;
					*motoSet.mode = g_motoModRsv;
					motoSet.destRsv = g_motoDestRsv;
					*g_cmdMode = g_cmdModRsv;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}else
			{
				if(g_spiRcvData[1]&0x02)
				{
					g_menuLock = 1;
					//menuLockRsv = mainPara.workMode;
					g_menuModRsv = mainPara.workMode;
					g_motoModRsv = *motoSet.mode;
					g_motoDestRsv = *motoSet.dest_location;
					g_cmdModRsv = *g_cmdMode;
					//motoMod = motoSet.mode;
					//motoDest = *motoSet.dest_location;
					mainPara.workMode = WORKINGMODE_TTL;
					*motoSet.mode = MOTO_MODE_AUTO;
					//motoSet.destRsv = 50;
					*g_cmdMode = CMD_MODE_OFF;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}/*
			if()
			{
				g_menuLock = 1;
				//menuLockRsv = mainPara.workMode;
				g_menuModRsv = mainPara.workMode;
				g_motoModRsv = *motoSet.mode;
				g_motoDestRsv = *motoSet.dest_location;
				g_cmdModRsv = *g_cmdMode;
				//motoMod = motoSet.mode;
				//motoDest = *motoSet.dest_location;
				mainPara.workMode = WORKINGMODE_TTL;
				*motoSet.mode = MOTO_MODE_AUTO;
				motoSet.destRsv = 50;
				*g_cmdMode = CMD_MODE_OFF;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				
			}//else if((g_menuLock>0)&&((0x4C==g_spiRcvData[1])||(0x48==g_spiRcvData[1])))
			else if((g_menuLock>0)&&(!(g_spiRcvData[1]&0x02)))
			//else if((g_menuLock>0)&&(!(g_spiRcvData[1]&0x02)))
			{
				g_menuLock = 0;
				//mainPara.workMode = menuLockRsv;
				//motoSet.mode = motoMod;
				//motoSet.dest_location = motoDest;
				mainPara.workMode = g_menuModRsv;
				*motoSet.mode = g_motoModRsv;
				motoSet.destRsv = g_motoDestRsv;
				*g_cmdMode = g_cmdModRsv;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}*/

			if((g_spiRcvData[1]&0x04)>0)
			{
				if(g_stepLength>0)
				{
					g_stepLength = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}else
			{
				if(g_stepLength<=0)
				{
					g_stepLength = 1;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}
			SpiRearUp(20);
		break;
		
		default:
		//SPI_MISO_AUX=1;	
		SpiRearUp(20);
		break;

	}
}


void GetParameter(void)
{
//	int paraIndex=0;
	unsigned char search_index;
	unsigned char tmp;
//	signed char cvtTmp;
	//static unsigned char modCodeRsv;
	static unsigned char motoRsv;

	switch(g_spiRcvData[0])
	{
		case 0xa6:
			//Parameter_Array[PARA_SRD]=g_spiRcvData[4];			//get flash surranding parameter
			if(menuPara.feb!=g_spiRcvData[4])
			{
				menuPara.feb = g_spiRcvData[4];
				if(g_stepLength>0)
				{
					flaPara.ttl_FEB1 = H_COV_5(menuPara.feb);
					flaPara.ttl_FEB2 = L_COV_5(menuPara.feb);
				}else
				{
					flaPara.ttl_FEB1 = H_COV(menuPara.feb+1);
					flaPara.ttl_FEB2 = L_COV(menuPara.feb+1);
				}
				flaPara.FEB_count = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
/*
			{
				for(search_index=0;search_index<10;search_index++)
				{
					if(menuPara.feb==feb_table[search_index])//may be error
					{
						//main_parameter.flash_surrand=search_index;
						//flaPara.ttl_FEB1 = ((search_index+2)/3);
						//flaPara.ttl_FEB2 = ((search_index+2)%3);
						//flaPara.ttl_FEB1 = (search_index/3);
						//flaPara.ttl_FEB2 = (search_index%3);
						break;
					}	
				}
				//if((search_index<=sizeof(feb_table))&&(search_index))	//it should be verify
				if(search_index<sizeof(feb_table))
				{
			    		//SETBIT(bitPara3,BIT_bitPara3_FLASH_SURRAND_INDEX);
			    		flaPara.ttl_FEB1 = (search_index/3);
					flaPara.ttl_FEB2 = ((search_index+3)%3); 
			    		//flaPara.surrand_state = 0x01;
					//SETBIT(bitTerminate,BIT_bitTerminate_TTLFEB);
				}

				if(search_index>0)
				{
					flaPara.surrand_state = 0x01;
				}
				
				
			}
*/
			//if(menuPara.mode!=g_spiRcvData[3])
			if(((g_spiRcvData[3]&0x07)!=mainPara.workMode)
				||(((g_spiRcvData[3]>>3)&0x03)!=mainPara.synMode))
			{
				//menuPara.mode = g_spiRcvData[3];

				mainPara.workMode = (g_spiRcvData[3]&0x07);
				mainPara.synMode = (g_spiRcvData[3]>>3)&0x03;

				if(WORKINGMODE_GR==mainPara.workMode)
				{
					Parameter_Array[PARA_SYNC2]=0x55;
					Parameter_Array[PARA_SYNC3]=0x53;
				}else
				{
					Parameter_Array[PARA_SYNC2]=(0x5d|((SYNC_MODE_REAR==mainPara.synMode)<<1));
					Parameter_Array[PARA_SYNC3]=(0x53|((SYNC_MODE_FP==mainPara.synMode)<<2));
				}

				SET_UPDATE_EVT(DSP_UPDATE_MODE);//g_dspUpdateFlag |= DSP_UPDATE_MODE;			//update the display

#if 0				
				switch(g_spiRcvData[3])
				{
					case 0:
	//				Parameter_Array[PARA_MODE]=0x20;
					mainPara.workMode=WORKINGMODE_TTL;	   //front&TTL
					mainPara.synMode=SYNMODE_HEAD;
					//menuPara.mode = 0x00;
					//Parameter_Array[PARA_SYNC1]=0x00;
					/*
					if((CMD_MODE_LIGH==*g_cmdMode)&&(WLM_ON== *wlSet.m_on))
					{
						Parameter_Array[PARA_SYNC1]=0x00;
					}else
					{
						Parameter_Array[PARA_SYNC1]=0x20;
					}*/
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
	//				CLRBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
					break;
					case 0x01:
					mainPara.workMode=WORKINGMODE_MANU;	  //front&MANU
					mainPara.synMode=SYNMODE_HEAD;
					//menuPara.mode = 0x01;
					//Parameter_Array[PARA_SYNC1]=0x01;
					//if(WLM_ON== *wlSet.m_on)
					/*
					if((CMD_MODE_LIGH==*g_cmdMode)&&(WLM_ON== *wlSet.m_on))
					{
						Parameter_Array[PARA_SYNC1]=0x01;
					}else
					{
						Parameter_Array[PARA_SYNC1]=0x21;
					}*/
					//Parameter_Array[PARA_SYNC1]=0x21;
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					break;
					case 0x02:								   //front&multiple//reserve for future multiple flash
#ifdef _MULTIPLY_FLASH_
					//SETBIT(paraBit,0);
					mainPara.workMode = WORKINGMODE_MULTI;
					mainPara.synMode=SYNMODE_HEAD;
					//menuPara.mode = 0x02;
					//Parameter_Array[PARA_SYNC1]=0x02;
					//Parameter_Array[PARA_MODE] =0x30;
					//Parameter_Array[PARA_SYNC2]=0x5f;
					//Parameter_Array[PARA_SYNC3]=0x53;
					//Parameter_Array[PARA_MODE] =0x22;
					//if(WLM_ON== *wlSet.m_on)
					/*
					if((CMD_MODE_LIGH==*g_cmdMode)&&(WLM_ON== *wlSet.m_on))
					{
						Parameter_Array[PARA_SYNC1]=0x02;
					}else
					{
						Parameter_Array[PARA_SYNC1]=0x22;
					}*/
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
#endif
					break;

					case 0x04:
						mainPara.workMode = WORKINGMODE_EXTA;
					break;

					
					case 0x08:							//ttl&back
					//if(WLM_OFF==*wlSet.m_on)
					//if((CMD_MODE_LIGH!=*g_cmdMode)||(WLM_OFF==*wlSet.m_on))
					if(CMD_MODE_LIGH_MST!=*g_cmdMode)
					{
						mainPara.workMode=WORKINGMODE_TTL;
						mainPara.synMode=SYNMODE_REAR;
						//menuPara.mode = 0x08;
						//Parameter_Array[PARA_SYNC1]=0x08;
						//Parameter_Array[PARA_SYNC1]=0x28;
						Parameter_Array[PARA_SYNC2]=0x5f;
						Parameter_Array[PARA_SYNC3]=0x53;
						//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
						//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
						//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					}
					break;
					case 0x09:						    //manu&back
					//if(WLM_OFF==*wlSet.m_on)
					//if((CMD_MODE_LIGH!=*g_cmdMode)||(WLM_OFF==*wlSet.m_on))
					if(CMD_MODE_LIGH_MST!=*g_cmdMode)
					{
						mainPara.workMode=WORKINGMODE_MANU;
						mainPara.synMode=SYNMODE_REAR;
						//menuPara.mode = 0x09;
						//Parameter_Array[PARA_SYNC1]=0x29;
						//Parameter_Array[PARA_SYNC1]=0x09;
						Parameter_Array[PARA_SYNC2]=0x5f;
						Parameter_Array[PARA_SYNC3]=0x53;
						//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
						//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
						//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					}
					break;
					case 0x0a:							//multi&back
#ifdef _MULTIPLY_FLASH_
					mainPara.workMode = WORKINGMODE_MULTI;
					mainPara.synMode=SYNMODE_REAR;
					//menuPara.mode = 0x0A;
					//Parameter_Array[PARA_SYNC1]=0x2a;
					//Parameter_Array[PARA_SYNC1]=0x0A;
					Parameter_Array[PARA_SYNC2]=0x5f;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
#endif
					break;
#ifdef _HP_SYN_
					case 0x10:						    //ttl&fast
					mainPara.workMode=WORKINGMODE_TTL;
					mainPara.synMode=SYNMODE_HP;
					//menuPara.mode = 0x10;
					//Parameter_Array[PARA_SYNC1]=0x30;
					//Parameter_Array[PARA_SYNC1]=0x10;
					//Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x57;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					break;
					case 0x11:							 //manu&fast
					mainPara.workMode=WORKINGMODE_MANU;
					mainPara.synMode=SYNMODE_HP;
					//menuPara.mode = 0x11;
					//Parameter_Array[PARA_SYNC1]=0x31;
					//Parameter_Array[PARA_SYNC1]=0x11;
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x57;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					break;
					case 0x12:							//multi&fast
					mainPara.workMode=WORKINGMODE_MULTI;
					mainPara.synMode=SYNMODE_HP;
					//menuPara.mode = 0x12;
					//Parameter_Array[PARA_SYNC1]=0x32;
					//Parameter_Array[PARA_SYNC1]=0x12;
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					//SetNrfPara(&nrfSendPara,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					//NrfTransfer(g_nrfBuf,UART_ENCODE_MODE,mainPara.workMode,main_parameter.SyncMode);
					break;
#endif

#ifdef RF_MODE_GROUP
					case 0x06:
					mainPara.workMode=WORKINGMODE_GR;
					mainPara.synMode=SYNMODE_HEAD;
					//menuPara.mode = 0x06;
					//Parameter_Array[PARA_SYNC1]=0x06;
					Parameter_Array[PARA_SYNC2]=0x55;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					break;

					case 0x16:
					mainPara.workMode=WORKINGMODE_GR;
					mainPara.synMode=SYNMODE_HP;
					//menuPara.mode = 0x16;
					//Parameter_Array[PARA_SYNC1]=0x16;
					Parameter_Array[PARA_SYNC2]=0x55;
					Parameter_Array[PARA_SYNC3]=0x53;
					//SETBIT(paraBit,BIT_paraBit_WORKMODE_VARY);
					break;
#endif
					default:
					break;
				
				}
#endif
			}
			//modCodeRsv = g_spiRcvData[3];
		break;

		case 0xa2:									 //focus lenghth
		//case 0xC8:
			if(motoRsv!=g_spiRcvData[3])
			{
				motoRsv = g_spiRcvData[3];
				//g_dspUpdateFlag |= DSP_UPDATE_MOTO;
				SET_UPDATE_EVT(DSP_UPDATE_MOTO);
				switch(g_spiRcvData[3])
				{
					case 0x01:				  //auto
	//				Parameter_Array[PARA_SYNC2]=0x7d;
	//				main_parameter.moto_location= 0x00;
					//main_parameter.moto_mode = MOTO_MODE_AUTO;
					*motoSet.mode = MOTO_MODE_AUTO;
					//Parameter_Array[PARA_FOCIND]=0x01;
					menuPara.motoCod = 0x01;
					//Parameter_Array[PARA_FOCLEN]=0x50;
					//Parameter_Array[PARA_FOCLEN]=*motoSet.dest_location;//main_parameter.moto_location;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x11:				 //0x18
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					//*motoSet.dest_location = 0x18;//main_parameter.moto_location= 0x18;
					motoSet.destRsv = 0x18;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x11;
					menuPara.motoCod = 0x11;
					//Parameter_Array[PARA_FOCLEN]=0x18;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x12:				 //0x1c
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					//*motoSet.dest_location = 0x1C;//main_parameter.moto_location= 0x1c;
					motoSet.destRsv = 0x1C;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x12;
					menuPara.motoCod = 0x12;
					//Parameter_Array[PARA_FOCLEN]=0x1c;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x13:				//0x23
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					//*motoSet.dest_location = 0x23;//main_parameter.moto_location= 0x23;
					motoSet.destRsv= 0x23;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x13;
					menuPara.motoCod = 0x13;
					//Parameter_Array[PARA_FOCLEN]=0x23;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x14:				//0x32
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x32;//main_parameter.moto_location= 0x32;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x14;
					menuPara.motoCod = 0x14;
					//Parameter_Array[PARA_FOCLEN]=0x32;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x15:				//0x46
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x46;//main_parameter.moto_location= 0x46;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x15;
					menuPara.motoCod = 0x15;
					//Parameter_Array[PARA_FOCLEN]=0x46;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x16:			   //0x50
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x50;//main_parameter.moto_location= 0x50;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x16;
					menuPara.motoCod = 0x16;
					//Parameter_Array[PARA_FOCLEN]=0x50;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x17:			   //0x69
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					motoSet.destRsv= 0x69;//main_parameter.moto_location= 0x69;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x17;
					menuPara.motoCod = 0x17;
					//Parameter_Array[PARA_FOCLEN]=0x69;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					default:
					motoSet.destRsv= 0x50;//main_parameter.moto_location= 0x50;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_AUTO;
					//Parameter_Array[PARA_FOCIND]=0x01;
					menuPara.motoCod = 0x01;
					//Parameter_Array[PARA_FOCLEN]=0x32;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

				}
			}
		break;

		case 0xC8:
		switch(g_spiRcvData[3])
		{
			case 0x35:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 20;
			break;

			case 0x05:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 24;
			break;

			case 0x09:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 28;
			break;

			case 0x0D:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 35;
			break;

			case 0x11:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 50;
			break;

			case 0x15:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 70;
			break;

			case 0x19:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 80;
			break;

			case 0x1D:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 105;
			break;

			case 0x39:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 135;
			break;

			case 0x45:
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv= 200;
			break;

			case 0x44:
				*motoSet.mode = MOTO_MODE_AUTO;
				menuPara.motoLoc = 0;
				//*motoSet.dest_location = 20;
			break;

			default:
				*motoSet.mode = MOTO_MODE_AUTO;
				menuPara.motoLoc = 0;
			break;
		}
		motoSet.focLen = g_spiRcvData[3];
		SET_UPDATE_EVT(DSP_UPDATE_MOTO);
		//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
		break;

		case 0xa5:
		{
			switch(g_spiRcvData[1]&0x0f)
			{
				case 0x0d:
				//case 0x2d:
				//case 0x6d:
				Parameter_Array[PARA_A5]=0x01;
				//g_isFullFrame = 2;
				//SETBIT(bitTerminate,BIT_bitTerminate_A5FLAG);
				break;

				case 0x0c:
				Parameter_Array[PARA_A5]=0x00;
				//g_isFullFrame = 2;
				break;

				default:
				break;
			/*	case 0x0c:
				Parameter_Array[PARA_A5]=0x00;
				break;*/
			}
		}
		break;
			
////////////////////////////////////////////////////////
		case 0xa7:
			//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
			//Parameter_Array[PARA_FREQ]=g_spiRcvData[4];
			//Parameter_Array[PARA_MOUT]=g_spiRcvData[5];
#ifdef _MULTIPLY_FLASH_
			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				//Parameter_Array[PARA_MOUT]=g_spiRcvData[5];
				if((*multiSet.frequence !=g_spiRcvData[4])||(*multiSet.eTimes != g_spiRcvData[5])||(menuPara.WlmMul[WL_ARRARY_A]!=g_spiRcvData[3]))
				{
					*multiSet.frequence =g_spiRcvData[4] ;//Parameter_Array[PARA_FREQ];
					*multiSet.eTimes = g_spiRcvData[5];//Parameter_Array[PARA_MOUT];
					menuPara.WlmMul[WL_ARRARY_A] = g_spiRcvData[3];
					mstPara->multi_exp = EXPH_COV(g_spiRcvData[3]&0x7F);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
				//if(WLM_OFF==*wlSet.m_on)
				/*
				if(CMD_MODE_OFF==*g_cmdMode)
				{
					//multiSet.cmdCode = g_spiRcvData[3];
					if(menuPara.WlmMul[WL_ARRARY_A] != g_spiRcvData[3])
					{
						menuPara.WlmMul[WL_ARRARY_A] = g_spiRcvData[3];
						mstPara->multi_exp = EXPH_COV(menuPara.WlmMul[WL_ARRARY_A]&0x7F);//(7-H_COV(menuPara.WlmMul[MASTER]));
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
					}
					//multiSet.frequence =g_spiRcvData[4] ;//Parameter_Array[PARA_FREQ];
					//multiSet.eTimes = g_spiRcvData[5];//Parameter_Array[PARA_MOUT];
					//multiSet.WlmMul[WL_ARRARY_A] = multiSet.cmdCode;
					//SETBIT(bitMultip,BIT_bitMultip_Exp);
					//SETBIT(bitMultip,BIT_bitMultip_TmFrq);
				}else
				{
					//SETBIT(bitMultip,BIT_bitMultip_Exp);
					//SETBIT(bitMultip,BIT_bitMultip_TmFrq);
				}*/
				//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
			}else if(WORKINGMODE_MANU==mainPara.workMode)
#else 
			if(WORKINGMODE_MANU==mainPara.workMode)				
#endif
			{
				//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
				//wlSet.m_arrPara.AotpCode = g_spiRcvData[3];
				//if(WLM_OFF==*wlSet.m_on)
				//if(CMD_MODE_OFF==*g_cmdMode)
				{
					//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
					if(menuPara.otpCod[MASTER]!=g_spiRcvData[3])
					{
						menuPara.otpCod[MASTER] = g_spiRcvData[3];
						mstPara->output1 =EXPH_COV(menuPara.otpCod[MASTER]&0x7F);//(7- H_COV(menuPara.otpCod[MASTER]));
						mstPara->output2 = EXPL_COV(menuPara.otpCod[MASTER]&0x7F);//(2-L_COV(menuPara.otpCod[MASTER]));
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
					}
				}
				
				//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
				//wlSet.m_arrPara.AotpCode = Parameter_Array[PARA_VOUT];
				//wlSet.m_arrPara.AotpCode = g_spiRcvData[3];
				//if(*wlSet.m_on==WLM_ON)
				//{
					//wlSet.m_arrPara.AotpCode=Parameter_Array[PARA_VOUT];
					//wlSet.m_arrPara.AotpCode = g_spiRcvData[3];
					//SETBIT(bitWLMaster,BIT_bitWLM_Aotp);
					
				//}else
				//{
					//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
					//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
					//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
				//}
			}
		    	
		//	for(search_index=0;search_index<sizeof(output_table);search_index++)
		//	{
		//		if((Parameter_Array[PARA_VOUT]&0x7f)==output_table[search_index])
		//		{
		//			main_parameter.outputChangeCode=search_index/*(0xc0|search_index)*/;
		//			main_parameter.VIP_output1=(8-(search_index+2)/3);
		//			main_parameter.VIP_output2=(2-(search_index+2)%3);
		//			break;
		//		}
		//	}
		//	if(mainPara.workMode==WORKINGMODE_MANU)
		//	{
		//		SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
		//	}
		break;
////////////////////////////////////////////////////////
		case 0xaa:
			//*wlSet.arr_mode = ((g_spiRcvData[3]>>2)&0x03);
			if(WORKINGMODE_TTL==mainPara.workMode)
			{
				if(*wlSet.ttl_Ratio!= ((g_spiRcvData[3]>>2)&0x03))
				{
					*wlSet.ttl_Ratio = ((g_spiRcvData[3]>>2)&0x03);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}else
			{
				if(*wlSet.arr_mode != ((g_spiRcvData[3]>>2)&0x03))
				{
					*wlSet.arr_mode = ((g_spiRcvData[3]>>2)&0x03);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}
/*
			switch((g_spiRcvData[3]>>2)&0x03)
			{
				case 0x00:
				if(WLM_MODE_ALL!=wlSet.arr_mode)
				{
					*wlSet.arr_mode=WLM_MODE_ALL;
					//Parameter_Array[PARA_WLM_MODE] = 0x5d;
					//WlmRatioVary();
					//SETBIT(bitWireless,BIT_bitMaster_Switch);
				}
				break;
				
				case 0x01:
				if(WLM_MODE_AB!=*wlSet.arr_mode)
				{
					*wlSet.arr_mode=WLM_MODE_AB;
					//Parameter_Array[PARA_WLM_MODE] = 0x5e;
					//WlmRatioVary();
					//SETBIT(bitWireless,BIT_bitMaster_Switch);
				}
				break;

				case 0x02:
				if(WLM_MODE_RES!=*wlSet.arr_mode)
				{
					*wlSet.arr_mode=WLM_MODE_RES;
					//Parameter_Array[PARA_WLM_MODE] = 0x5f;
					//WlmRatioVary();
					//SETBIT(bitWireless,BIT_bitMaster_Switch);
				}
				break;					
			}
*/
	/*		if(GETBIT(g_spiRcvData[3],WLM_MASTON_BIT))
			{
				Parameter_Array[PARA_WIRELESS]|=WLM_MASTER_ON;
			}else
			{
				Parameter_Array[PARA_WIRELESS]&=(~WLM_MASTER_ON);
			}
*/
#ifdef WIRELESS_MASTER_ON
			if(GETBIT(g_spiRcvData[3],WLM_WLSWTH_BIT))
			//if(g_spiRcvData[3]&0x01)				//bit 0
			{			

				if(WORKINGMODE_GR == mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_TTL;
					menuPara.mode = 0x00;
					//Parameter_Array[PARA_SYNC1]=0x00;					
				}

				if(CMD_MODE_LIGH_MST!=*g_cmdMode)
				{
					//*wlSet.m_on = WLM_ON;
					*g_cmdMode = CMD_MODE_LIGH_MST;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					SET_UPDATE_EVT(DSP_UPDATE_RFSW);
				}
				//main_parameter.SyncMode = SYNMODE_HEAD;
				if(SYNMODE_REAR==mainPara.synMode)
				{
					mainPara.synMode = SYNMODE_HEAD;
					//Parameter_Array[PARA_SYNC1]=0x00;
					menuPara.mode = 0x00;
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x53;
				}
				
			}else
			{
				
				if(WORKINGMODE_GR==mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_TTL;
					menuPara.mode = 0x00;
					//Parameter_Array[PARA_SYNC1]=0x00;
				}
				//Parameter_Array[PARA_WIRELESS] &= 0xFD; 
				Parameter_Array[PARA_WLM_MODE] = 0x5c;
				//*g_cmdMode = CMD_MODE_OFF;
				//if(WLM_OFF!=*wlSet.m_on)
#ifdef _ST_EX_
				if(CMD_MODE_LIGH_MST==*g_cmdMode)
				{
					*g_cmdMode = CMD_MODE_RF_MST;			
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					SET_UPDATE_EVT(DSP_UPDATE_RFSW);
				}
#else
				if(CMD_MODE_OFF!=*g_cmdMode)
				{
					//*wlSet.m_on = WLM_OFF;
					*g_cmdMode = CMD_MODE_OFF;
					menuPara.mode &= 0xDF;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					SET_UPDATE_EVT(DSP_UPDATE_RFSW);
				}
#endif
			}
			
//here reserved for the master take part in flashing
#ifdef WLM_MASTER_FLA
			if(GETBIT(g_spiRcvData[3],WLM_MASTON_BIT))
			{
				if(*wlSet.is_masterOn!=WLM_MASTER_ON)
				{
					*wlSet.is_masterOn = WLM_MASTER_ON;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					//SETBIT(Parameter_Array[PARA_WIRELESS],3);
					//SETBIT(bitWLMaster,BIT_bitWLM_Mfla);
				}
				
			}else
			{
				if(*wlSet.is_masterOn!=WLM_MASTER_OFF)
				{
					*wlSet.is_masterOn = WLM_MASTER_OFF;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					//CLRBIT(Parameter_Array[PARA_WIRELESS],3);
					//SETBIT(bitWLMaster,BIT_bitWLM_Mfla);
				}
			}
#endif



			//Parameter_Array[PARA_WIRELESS] = g_spiRcvData[3];
			if(*wlSet.m_chan!=g_spiRcvData[4])
			{
				*wlSet.m_chan = g_spiRcvData[4];		//in the camera, the 0,1,2,3 represent channel 1,2,3,4
				 SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SETBIT(bitWireless,BIT_bitMaster_Switch);
			}
			
		break;
#endif

		case 0xab:
			//switch(*wlSet.arr_mode)
			switch(*wlSet.ttl_Ratio)
			{
				case WLM_MODE_ALL:
				break;

				case WLM_MODE_AB:
					if(*wlSet.abRatio != g_spiRcvData[3])
					{
						*wlSet.abRatio = g_spiRcvData[3];
						//wlSet.m_arrPara[WL_ARRARY_B].cmp1 = *wlSet.abRatio;
						abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
						abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(bitWLMaster,BIT_bitWLM_abRatio);
					}
				break;

				case WLM_MODE_RES:
					if(*wlSet.abRatio != g_spiRcvData[3])
					{
						*wlSet.abRatio = g_spiRcvData[3];
						//wlSet.m_arrPara[WL_ARRARY_B].cmp1 = *wlSet.abRatio;
						abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
						abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(bitWLMaster,BIT_bitWLM_abRatio);
					}
					if(wlSet.cCmp != g_spiRcvData[4])
					{
						wlSet.cCmp = g_spiRcvData[4];
						if(g_stepLength>0)
						{
							wlSet.m_arrPara[WL_ARRARY_C].cmp1 = CMPH_COV_5(wlSet.cCmp);
							wlSet.m_arrPara[WL_ARRARY_C].cmp2 = CMPL_COV_5(wlSet.cCmp);
						}else
						{
							wlSet.m_arrPara[WL_ARRARY_C].cmp1 = CMPH_COV(wlSet.cCmp);
							wlSet.m_arrPara[WL_ARRARY_C].cmp2 = CMPL_COV(wlSet.cCmp);
						}
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(bitWLMaster,BIT_bitWLM_Ccmp);
					}					
					
				break;
			}
		break;

#ifdef _MULTIPLY_FLASH_
		case 0xac:
		if(WORKINGMODE_MULTI==mainPara.workMode)
		{
/*
			if(menuPara.WlmMul[WL_ARRARY_A] != g_spiRcvData[3])
			{
				menuPara.WlmMul[WL_ARRARY_A] = g_spiRcvData[3];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SETBIT(bitMultip,BIT_bitMultip_WLMA);
			}

			if(menuPara.WlmMul[WL_ARRARY_B] != g_spiRcvData[4])
			{
				menuPara.WlmMul[WL_ARRARY_B] = g_spiRcvData[4];
				SET_UP DATE_EVT(DSP_UPDATE_MODE);
				//SETBIT(bitMultip,BIT_bitMultip_WLMB);
			}

			if(menuPara.WlmMul[WL_ARRARY_C] != g_spiRcvData[5])
			{
				menuPara.WlmMul[WL_ARRARY_C] = g_spiRcvData[5];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SETBIT(bitMultip,BIT_bitMultip_WLMC);
			}
*/
			for(search_index=0;search_index<3;search_index++)
			{
				if(menuPara.WlmMul[MASTER+search_index] != g_spiRcvData[3+search_index])
				{
					menuPara.WlmMul[MASTER+search_index] = g_spiRcvData[3+search_index];
					//wlSet.m_arrPara[MASTER+search_index].multi_exp = EXPH_COV(menuPara.WlmMul[MASTER+search_index])<<4;
					wlSet.m_arrPara[MASTER+search_index].multi_exp = EXPH_COV(menuPara.WlmMul[MASTER+search_index]&0x7f);
					//wlSet.m_arrPara[MASTER+search_index].multi_exp = EXPH_COV(menuPara.WlmMul[MASTER+search_index]);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}
/*			if(*wlSet.m_on)
			{
				for(search_index=0;search_index<sizeof(output_table);search_index++)
				{
					if((multiSet.WlmMul[WL_ARRARY_A]&0x7f)==output_table[search_index])
					{
							//main_parameter.VIP_output1=(8-(search_index+2)/3);
							//main_parameter.VIP_output2=(2-(search_index+2)%3);
							//multiSet.expose = (((8-(search_index+2)/3)<<4)|(2-(search_index+2)%3));
							multiSet.expose = (8-(search_index+2)/3);
							break;
					}
				}
				SETBIT(bitMultip,BIT_bitMultip_Exp);
			}
*/
			
		}else
#endif
		{
			//if(wlSet.m_arrPara.AotpCode!=g_spiRcvData[3])
			//if(wlSet.otpCode[WL_ARRARY_A]!=g_spiRcvData[3])
			for(search_index=0;search_index<3;search_index++)
			{
				if(menuPara.otpCod[MASTER+search_index]!=g_spiRcvData[3+search_index])
				{
					menuPara.otpCod[MASTER+search_index]=g_spiRcvData[3+search_index];
					wlSet.m_arrPara[MASTER+search_index].output1 = EXPH_COV(menuPara.otpCod[MASTER+search_index]&0x7F);
					wlSet.m_arrPara[MASTER+search_index].output2 = EXPL_COV(menuPara.otpCod[MASTER+search_index]&0x7F);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}
/*
			if(menuPara.otpCod[MASTER]!=g_spiRcvData[3])
			{
				//wlSet.m_arrPara.AotpCode= g_spiRcvData[3];
				//wlSet.otpCode[WL_ARRARY_A] = g_spiRcvData[3];
				//Parameter_Array[PARA_VOUT]=g_spiRcvData[3];
				menuPara.otpCod[MASTER] = g_spiRcvData[3];
				wlSet.m_arrPara[MASTER].output1 = 
					
				SET_UPDATE_EVT(DSP_UPDATE_MODE)
				//SETBIT(bitWLMaster,BIT_bitWLM_Aotp);
			}
			//if(wlSet.m_arrPara.BotpCode!= g_spiRcvData[4])
			//if(wlSet.otpCode[WL_ARRARY_B]!= g_spiRcvData[4])
			if(menuPara.otpCod[WL_ARRARY_B]!=g_spiRcvData[4])
			{
				//wlSet.m_arrPara.BotpCode= g_spiRcvData[4];
				//wlSet.otpCode[WL_ARRARY_B] = g_spiRcvData[4];
				//SETBIT(bitWLMaster,BIT_bitWLM_Botp);
				menuPara.otpCod[WL_ARRARY_B] = g_spiRcvData[4];
				SET_UPDATE_EVT(DSP_UPDATE_MODE)
			}
			//if(wlSet.m_arrPara.CotpCode!= g_spiRcvData[5])
			//if(wlSet.otpCode[WL_ARRARY_C] != g_spiRcvData[5])
			if(menuPara.otpCod[WL_ARRARY_C]!=g_spiRcvData[5])
			{
				//wlSet.m_arrPara.CotpCode= g_spiRcvData[5];
				//wlSet.otpCode[WL_ARRARY_C] = g_spiRcvData[5];
				//SETBIT(bitWLMaster,BIT_bitWLM_Cotp);
				menuPara.otpCod[WL_ARRARY_C]=g_spiRcvData[5];
				SET_UPDATE_EVT(DSP_UPDATE_MODE)
			}
*/
			
		}			
		break;

		case 0xb2:
			wlSet.m_ttlCmd[0]=g_spiRcvData[1];
			wlSet.m_ttlCmd[1]=g_spiRcvData[2];
			wlSet.m_ttlCmd[2]=g_spiRcvData[3];

			flaPara.rfCmuPara[0].ttlExpVal=g_spiRcvData[1];
			flaPara.rfCmuPara[1].ttlExpVal=g_spiRcvData[2];
			flaPara.rfCmuPara[2].ttlExpVal=g_spiRcvData[3];

			ttl_expCodeRsv = g_spiRcvData[1];
/*
			
			if((WORKINGMODE_GR==mainPara.workMode)&&(CMR_ADEPT_OLD==radioStatue.adept))
			{
				flaPara.rfCmuPara[0].ttlExpVal=g_spiRcvData[1];
				flaPara.rfCmuPara[1].ttlExpVal=g_spiRcvData[2];
				flaPara.rfCmuPara[2].ttlExpVal=g_spiRcvData[3];
			}else
			{
				flaPara.rfCmuPara[0].ttlExpVal=g_spiRcvData[1];
				flaPara.rfCmuPara[1].ttlExpVal=g_spiRcvData[2];
				flaPara.rfCmuPara[2].ttlExpVal=g_spiRcvData[3];
			}
*/			
		break;
		

		case 0xbd:
		//main_parameter.focLen = g_spiRcvData[2];
		//if((main_parameter.moto_mode == MOTO_MODE_AUTO)&&(menuPara.motoLoc!=g_spiRcvData[2]))
		if(menuPara.motoLoc!=g_spiRcvData[2])
		{
			//main_parameter.moto_location= main_parameter.focLen;
			//menuPara.motoLoc=g_spiRcvData[2];
			menuPara.motoLoc = g_spiRcvData[2];
			
			if(MOTO_MODE_AUTO==*motoSet.mode)
			{
				//if(((*motoSet.destRsv>menuPara.motoLoc)&&(*motoSet.destRsv<g_spiRcvData))
					//||(*motoSet.destRsv<menuPara.motoLoc)&&(*motoSet.destRsv>g_spiRcvData))
					//if(*motoSet.destRsv<menuPara.motoLoc)&&(menuPara.mo)
				//{
					//menuPara.motoLoc = g_spiRcvData[2];
					//*motoSet.destRsv = g_spiRcvData[2];
				//}
				SET_UPDATE_EVT(DSP_UPDATE_MOTO);
			}
			//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
			//menuPara.motoLoc = g_spiRcvData[2];
		}
		break;

		case 0xb7:
		if(g_spiRcvData[1]>0)
		{
			if(!g_cmrPressed)
			{
				g_SettingIndex = 0;
			}
			g_cmrPressed = 1;
		}else
		{
			g_cmrPressed = 0;
		}
		g_isFullFrame = 1;
			//g_cmrPressed = 1;
		//if(flaPara.apert!=g_spiRcvData[1])
		if(menuPara.cApert!=g_spiRcvData[1])
		{
			//flaPara.apert = g_spiRcvData[1];
			
			menuPara.cApert = g_spiRcvData[1];	
			//tmp1 = menuPara.cApert>>4;
			//tmp2 = menuPara.cApert&0x0F;
			
			flaPara.apert = CannonApertVal[(menuPara.cApert>>4)*6+((menuPara.cApert&0x0F)+2)/3];
			
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
			//SPI_MISO_AUX=1;
			//g_dspUpdateFlag|=DSP_UPDATE_LINK;
			//SETBIT(bitTerminate,BIT_bitTerminate_APERT);
		}
		break;
		

/***************************************************************************
		-3          -2          -1           0		1		2		3
		|___|___|___|___|___|___|___|___|___|___|___|___|				//步长为0.5EV
		0x18     0x10       0x08       0x00      0xF8       0xF0       0xE8
		     0x14       0x0C      0x04        0xFC      0xF4        0xEC

		-3          -2              -1             0		1		 2		   3
		|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|
		18,15, 13, 10,0D,0B,08, 05, 03, 0,   FD,FB,F8, F5,  F3, F0, ED, EB E8

***************************************************************************/

		case 0xA8:							//TTL compensation
			//cvtTmp = (signed char)g_spiRcvData[1];
			//flaPara.cmrCmp1 = CMPH_COV(g_spiRcvData[1]);
			//flaPara.cmrCmp2 = CMPL_COV(g_spiRcvData[1]);
			if(flaPara.cmrCmpCode!=g_spiRcvData[1])
			{
				flaPara.cmrCmpCode = g_spiRcvData[1];
				if(g_stepLength>0)
				{
					wlSet.cmrCmp1 = CMPH_COV_5(g_spiRcvData[1]);
					wlSet.cmrCmp2 = CMPL_COV_5(g_spiRcvData[1]);
				}else
				{
					wlSet.cmrCmp1 = CMPH_COV(g_spiRcvData[1]);
					wlSet.cmrCmp2 = CMPL_COV(g_spiRcvData[1]);
				}
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SET_UPDATE_EVT(DSP_UPDATE_LINK);
			}
		break;


		case 0xb8:
//#ifdef _MULTIPLY_FLASH_
		//if(multiSet.sCode!=g_spiRcvData[1])
		//{
			//multiSet.sCode = g_spiRcvData[1];
			//g_dspUpdateFlag |= DSP_UPDATE_MULCAL;
			//SETBIT(bitExtra,BIT_bitExtra_Shutter);
		//}
//#endif
#ifdef CANON_RF
		if(flaPara.shutter!=g_spiRcvData[1])
		{
			flaPara.shutter = g_spiRcvData[1];
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
#endif
		break; 



		case 0xe6:
		break;

		case 0xfc:
		break;

		case 0xb1:
			g_hpLev = g_spiRcvData[1];
		break;

		case 0xbb:
		if(flaPara.isoCmp!=g_spiRcvData[1])
		{
			flaPara.isoCmp = g_spiRcvData[1];
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
/*		if(mainPara.workMode==WORKINGMODE_TTL)
		{
			for(search_index=0;search_index<sizeof(follow_bb_table);search_index++)
			{
				if(g_spiRcvData[1]==follow_bb_table[search_index])
				{
					main_parameter.flashCompensation=(0xa0|search_index);
						
				}
			}
		}
		SETBIT(paraBit,BIT_paraBit_TTL_COMPENSATION_VARY); 	   */
		break;

		case 0xb0:
//		if(ttl_time_calculate)
		//if(GETBIT(bitIndex,BIT_bitIndex_TTLTIMECALC))
		if(g_getTtlFlashCode)
		{
			ttl_expCodeRsv = g_spiRcvData[1];
			g_getTtlFlashCode = 0;
#ifdef TTL_DEBUG
//			radioStatue.cmdReady = 1;
//			wlSet.m_cmdProc = WLM_CMDPROC_TTLDEBUG;
//			OSTaskResume(CMDSEND_TASK_Prio);
#endif
				 
		 }
		break;	 

		case 0xC2:
		/*
		if(WORKINGMODE_GR==mainPara.workMode)
		{
			flaPara.rfCmuPara[0].ttlExpVal = g_spiRcvData[2];
			flaPara.rfCmuPara[1].ttlExpVal = g_spiRcvData[3];
			flaPara.rfCmuPara[2].ttlExpVal = g_spiRcvData[4];
			flaPara.rfCmuPara[3].ttlExpVal = g_spiRcvData[5];
			flaPara.rfCmuPara[4].ttlExpVal = g_spiRcvData[6];
			//rfGrPara.rfGr_arrPara[0].ttlExpVal = g_spiRcvData[2];
			//rfGrPara.rfGr_arrPara[1].ttlExpVal = g_spiRcvData[3];
			//rfGrPara.rfGr_arrPara[2].ttlExpVal = g_spiRcvData[4];
			//rfGrPara.rfGr_arrPara[3].ttlExpVal = g_spiRcvData[5];
			//rfGrPara.rfGr_arrPara[4].ttlExpVal = g_spiRcvData[6];
		}else
		{
			wlSet.m_ttlCmd[0] = g_spiRcvData[2];
			wlSet.m_ttlCmd[1] = g_spiRcvData[3];
			wlSet.m_ttlCmd[2] = g_spiRcvData[4];
			//wlSet.m_ttlCmd[3] = g_spiRcvData[5];
			//wlSet.m_ttlCmd[4] = g_spiRcvData[6];
		}*/
		flaPara.rfCmuPara[0].ttlExpVal = g_spiRcvData[2];
		flaPara.rfCmuPara[1].ttlExpVal = g_spiRcvData[3];
		flaPara.rfCmuPara[2].ttlExpVal = g_spiRcvData[4];
		flaPara.rfCmuPara[3].ttlExpVal = g_spiRcvData[5];
		flaPara.rfCmuPara[4].ttlExpVal = g_spiRcvData[6];
		//rfGrPara.rfGr_arrPara[0].ttlExpVal = g_spiRcvData[2];
		//rfGrPara.rfGr_arrPara[1].ttlExpVal = g_spiRcvData[3];
		//rfGrPara.rfGr_arrPara[2].ttlExpVal = g_spiRcvData[4];
		//rfGrPara.rfGr_arrPara[3].ttlExpVal = g_spiRcvData[5];
		//rfGrPara.rfGr_arrPara[4].ttlExpVal = g_spiRcvData[6];
		//radioSet.arrExpVal[0] = g_spiRcvData[2];
		//radioSet.arrExpVal[1] = g_spiRcvData[3];
		//radioSet.arrExpVal[2] = g_spiRcvData[4];
		//radioSet.arrExpVal[3] = g_spiRcvData[5];
		//radioSet.arrExpVal[4] = g_spiRcvData[6];
		break;

#ifdef CANON_RF
		case 0xCA:
		if((*g_cmdMode != CMD_MODE_RF_MST)
			&&(*g_cmdMode!=CMD_MODE_RFLS_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_SLA))
		{
			*g_cmdMode = CMD_MODE_RF_MST;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			SET_UPDATE_EVT(DSP_UPDATE_RFSW);
		}
		tmp = ((g_spiRcvData[3]>>2)&0x03);
		//if(wlSet.rfSet.ratio!=tmp)
		if(WORKINGMODE_TTL==mainPara.workMode)
		{
			if(*wlSet.ttl_Ratio!=tmp)
			{
				*wlSet.ttl_Ratio = tmp;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}else
		{
			if(*wlSet.arr_mode!=tmp)
			{
				//radioSet.ratio = tmp;
				*wlSet.arr_mode=tmp;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}
		//*wlSet.is_masterOn = ((g_spiRcvData[3]>>1)&0x01);
		tmp = ((g_spiRcvData[3]>>1)&0x01);
		if(tmp!=*wlSet.is_masterOn)
		{
			*wlSet.is_masterOn = tmp;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
		if(radioSet->rfChan!=g_spiRcvData[4])
		{
			radioSet->rfChan = g_spiRcvData[4];
			g_rfChanSet = radioSet->rfChan;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			SET_UPDATE_EVT(DSP_UPDATE_RFCHAN);
			//LoadUartSendBuf(UART_ENCODE_RFCH,radioSet->rfChan,0);
			
		}
		if(radioSet->id!= MAKEWORD(g_spiRcvData[6],g_spiRcvData[5]))
		{
			radioSet->id = MAKEWORD(g_spiRcvData[6],g_spiRcvData[5]);
			//g_rfChanSet = radioSet->id;
			g_rfIdSet = radioSet->id;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			SET_UPDATE_EVT(DSP_UPDATE_RFID);
			//LoadUartSendBuf(UART_ENCODE_RFID, radioSet->id>>8, radioSet->id&0xff);
		}
		//TrigerUartSend();
		break;

		case 0xCB:															//GR mode
			for(search_index=0;search_index<5;search_index++)
			{
				//if(rfGrPara.rfGr_arrPara[search_index].mode!=g_spiRcvData[4+search_index])
				if(radioSet->rfGr[search_index].isOff>0)
				{
					if(g_spiRcvData[4+search_index]!=WORKINGMODE_OFF)
					{
						radioSet->rfGr[search_index].isOff = 0;
						radioSet->rfGr[search_index].mode = GrModRevert(g_spiRcvData[4+search_index]);
						g_grDspStart = (search_index>2)? 2:0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
					}
				}else
				{
					if(WORKINGMODE_OFF==g_spiRcvData[4+search_index])
					{
						if(!radioSet->rfGr[search_index].isOff)
						{
							radioSet->rfGr[search_index].isOff = 1;
							g_grDspStart = (search_index>2)? 2:0;
							SET_UPDATE_EVT(DSP_UPDATE_MODE);
						}
					}else
					{
						if(radioSet->rfGr[search_index].mode!=g_spiRcvData[4+search_index])
						{
							radioSet->rfGr[search_index].mode = GrModRevert(g_spiRcvData[4+search_index]);
							g_grDspStart = (search_index>2)? 2:0;
							SET_UPDATE_EVT(DSP_UPDATE_MODE);
						}
					}
#if 0
					if(radioSet->rfGr[search_index].mode!=g_spiRcvData[4+search_index])
					{
						radioSet->rfGr[search_index].mode = g_spiRcvData[4+search_index];
						/*
						if(search_index>2)
						{
							g_grDspStart = 2;
						}else
						{
							g_grDspStart = 0
						}*/
						g_grDspStart = (search_index>2)? 2:0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(flaPara.rfGrInfoFlag[search_index],RF_INFOBIT_MODE);
						//here should send info
					}
#endif
				}
			}
		break;

		case 0xCC:														//GR output
			for(search_index=0;search_index<5;search_index++)
			{
				//if(radioPara.optCode[search_index]!= g_spiRcvData[4+search_index])
				switch(radioSet->rfGr[search_index].mode)
				{
					//case WORKINGMODE_MANU:
					case GRMOD_M:
					if(menuPara.rfGrOtp[search_index]!=g_spiRcvData[4+search_index])
					{
						//radioPara.optCode[search_index] = g_spiRcvData[4+search_index];
						menuPara.rfGrOtp[search_index]=g_spiRcvData[4+search_index];
						radioSet->rfGr[search_index].m_exp1 = EXPH_COV(menuPara.rfGrOtp[search_index]&0x7F);
						radioSet->rfGr[search_index].m_exp2 = EXPL_COV(menuPara.rfGrOtp[search_index]&0x7F);
						g_grDspStart = (search_index>2)? 2:0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(flaPara.rfGrInfoFlag[search_index],RF_INFOBIT_OUTPUT);
					}
					break;

					//case WORKINGMODE_TTL:
					//case WORKINGMODE_EXTA:
					case GRMOD_TTL:
					case GRMOD_EXTA:
					if(menuPara.rfGrTmp[search_index]!=g_spiRcvData[4+search_index])
					{
						menuPara.rfGrTmp[search_index] = g_spiRcvData[4+search_index];
						if(g_stepLength>0)
						{
							radioSet->rfGr[search_index].ttl_cmp1 = CMPH_COV_5(menuPara.rfGrTmp[search_index]);
							radioSet->rfGr[search_index].ttl_cmp2 = CMPL_COV_5(menuPara.rfGrTmp[search_index]);
						}else
						{
							radioSet->rfGr[search_index].ttl_cmp1 = CMPH_COV(menuPara.rfGrTmp[search_index]);
							radioSet->rfGr[search_index].ttl_cmp2 = CMPL_COV(menuPara.rfGrTmp[search_index]);
						}
						g_grDspStart = (search_index>2)? 2:0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
					}
					break;
				}
			}
		break;
#endif

		case 0xC7:						//multiply parameter
		if(WORKINGMODE_MULTI==mainPara.workMode)
		{
			//multiSet.cmdCode = g_spiRcvData[3];
			if((menuPara.WlmMul[WL_ARRARY_A] != g_spiRcvData[3])||
			(*multiSet.frequence !=MAKEWORD(g_spiRcvData[5], g_spiRcvData[4])/* g_spiRcvData[5]*/)||
			(*multiSet.eTimes != g_spiRcvData[6]))
			{
				menuPara.WlmMul[WL_ARRARY_A] = (g_spiRcvData[3]&0x7F);
				//wlSet.m_arrPara[WL_ARRARY_A].multi_exp = EXPH_COV(menuPara.WlmMul[WL_ARRARY_A])<<4;
				wlSet.m_arrPara[WL_ARRARY_A].multi_exp = EXPH_COV(menuPara.WlmMul[WL_ARRARY_A]);
				*multiSet.frequence = MAKEWORD(g_spiRcvData[5], g_spiRcvData[4]);//g_spiRcvData[5];
				*multiSet.eTimes = g_spiRcvData[6];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			
			//SETBIT(bitMultip,BIT_bitMultip_Exp);
			//SETBIT(bitMultip,BIT_bitMultip_TmFrq);
		}else
		{
			if(menuPara.otpCod[MASTER]!=g_spiRcvData[3])
			{
				menuPara.otpCod[MASTER] = g_spiRcvData[3];
				mstPara->output1 = EXPH_COV(menuPara.otpCod[MASTER]&0x7F);
				mstPara->output2 = EXPL_COV(menuPara.otpCod[MASTER]&0x7f);
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
		}
		
		break;

		case 0xb5:			//step length 2015/07/06
		break;

		case 0xB3:
			if((0X12==g_spiRcvData[1])&&(0X46==g_spiRcvData[2]))
			{
				g_AfIndex = 0;
			}else if((0X12==g_spiRcvData[1])&&(0X47==g_spiRcvData[2]))
			{
				g_AfIndex = 1;
			}else if((0x13==g_spiRcvData[1])&&(0x46==g_spiRcvData[2]))
			{
				g_AfIndex = 2;
			}else
			{
				g_AfIndex = 0;
			}
		break;

		// 0x17: 700d  0x11: 5dmarkIII
		case 0xA9:                                    // may be indicater the camera type
			g_cmrA9Id = g_spiRcvData[1];
		break;
	
/*		case 0xa8:
			switch(SPI_Data_Array[1])
			{
			}*/

//		break;
		default:
		break;
}
}

#ifdef _MULTIPLY_FLASH_
unsigned char WlmMulFrqRev(void)
{
	unsigned char mulFrqCov;
	
	if(*multiSet.frequence<20)
	{
		mulFrqCov = *multiSet.frequence - 1;
	}else if(*multiSet.frequence<50)
	{
		mulFrqCov = (*multiSet.frequence-20)/5 +19;
	}else if(*multiSet.frequence<200)
	{
		mulFrqCov = (*multiSet.frequence+1-50)/10+25;
	}else
	{
		mulFrqCov = (*multiSet.frequence-200)/50+40;
	}

	return mulFrqCov;
}

//extern u8 slvTmsDebug;
unsigned char RfMulTimeCalc(int cov)
//unsigned char MulTimeRev(int cov)
{
	//unsigned char tmp0;
	//unsigned long tmp3;
	u8 search_index;
	unsigned char tmp=0;

	if(0xFF == flaPara.shutter)
	{
		flaPara.quickTime = 30000;
		
	}else
	{
		for(search_index=0;search_index<LENOFTIMETABLE;search_index++)
		{
			//if(multiSet.sCode==shutterTable[search_index].index)
			if(flaPara.shutter==shutterTable[search_index].index)
			{
				flaPara.quickTime = shutterTable[search_index].ms;
				break;
			}
		}

	}

	//slvTmsDebug = flaPara.shutter>>4;

	flaPara.quickTime += cov;			//reserve some space

	//if(WORKINGMODE_MULTI==mainPara.workMode)
	//{
	if(((unsigned long)flaPara.quickTime*(unsigned long)(canWireless.wlMul_frq))>((unsigned long)(canWireless.wlMul_time)*1000))
	{
		tmp = canWireless.wlMul_time;
	}else
	{
		tmp = (unsigned long)((unsigned long)flaPara.quickTime*(unsigned long)canWireless.wlMul_frq)/1000+1;
	}

	return tmp;
	//}	
	
	//if(!wlSet.muitl_realTimes)
	//{
	//	wlSet.muitl_realTimes = (((unsigned long)flaPara.quickTime*(unsigned long)(canWireless.wlMul_frq))/1000);		
	//}
}


unsigned char MulTimeRev(void)
//unsigned char MulTimeRev(int cov)
{
	unsigned char tmp0;
	unsigned long tmp3;
	u8 search_index;

	for(search_index=0;search_index<LENOFTIMETABLE;search_index++)
	{
		//if(multiSet.sCode==shutterTable[search_index].index)
		if(flaPara.shutter==shutterTable[search_index].index)
		{
			flaPara.quickTime = shutterTable[search_index].ms;
			break;
		}
	}

	//flaPara.quickTime += cov;			//reserve some space

	if(WORKINGMODE_MULTI==mainPara.workMode)
	{
		if(((unsigned long)flaPara.quickTime*(unsigned long)(*multiSet.frequence))>((unsigned long)(*multiSet.eTimes)*1000))
		{
			wlSet.muitl_realTimes = *multiSet.eTimes;
		}else
		{
			wlSet.muitl_realTimes = (unsigned long)(flaPara.quickTime*(*multiSet.frequence))/1000+1;
		}
	}	
	
	if(!wlSet.muitl_realTimes)
	{
		tmp3 = (((unsigned long)flaPara.quickTime*(unsigned long)(*multiSet.frequence))/1000)+1;

		if(tmp3<20)
		{
			tmp0 = (unsigned char)tmp3;
		}else if(tmp3<50)
		{
			tmp0 = (tmp3-20)/5+20;
		}else if(tmp3<100)
		{
			tmp0 = (tmp3-50)/10+26;
		}else
		{
			tmp0 = (100-50)/10+26;
		}
		
	}else if(wlSet.muitl_realTimes<20)
	{
		tmp0 = wlSet.muitl_realTimes;
	}else if(wlSet.muitl_realTimes<50)
	{
		tmp0 = (wlSet.muitl_realTimes-20)/5+20;
	}else
	{
		tmp0 = (wlSet.muitl_realTimes-50)/10+26;
	}

	return tmp0;
}


unsigned char WlmMulTimeRev()
{
	//unsigned char mulTimeCov;
	//unsigned char tmp0,tmp1,tmp2;
//	unsigned long tmp3;
	unsigned char tmp0;

	tmp0= MulTimeRev();
	//tmp0 = MulTimeRev(0);

	//tmp1 = tmp0/4;
	//tmp2 = (tmp0+4)%4;

	//mulTimeCov = (tmp1<<4) | (tmp2 +0x08);

	//return mulTimeCov;
	return tmp0;
	
}

unsigned char RfMulTimeRev(void)
{
	unsigned char tmp0;
	if(*multiSet.eTimes<20)
	{
		tmp0 = *multiSet.eTimes;
	}else if(*multiSet.eTimes<50)
	{
		tmp0 = ((*multiSet.eTimes)-20)/5+20;
	}else if(*multiSet.eTimes<200)
	{
		tmp0 = ((*multiSet.eTimes)-50)/10+26;
	}else
	{
		tmp0 = ((*multiSet.eTimes)-200)/50+40;
			
	}
	return tmp0;
}


#endif


void BuildExposeCmd(char mode,unsigned char hpSyn,unsigned char hpLex)
{
	unsigned char tmp;
	
	//wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1]=wlSet.m_flaCmd[2] =wlSet.m_flaCmd[3]=
	//wlSet.m_flaCmd[4] =	wlSet.m_flaCmd[5] = wlSet.m_flaCmd[6]=wlSet.m_flaCmd[7] =0;
	memset(wlSet.m_flaCmd,0,8);
	
	if(WORKINGMODE_TTL==mode)
	{
		//switch(*wlSet.arr_mode)
		switch(*wlSet.ttl_Ratio)
		{
			case WLM_MODE_ALL:
#ifdef _HP_SYN_
			if(HP_FLASHMODE_ON==hpSyn)
			{
				if(hpLex)
				{
					wlSet.m_flaCmd[0]=0xb4;
				}else
				{
					wlSet.m_flaCmd[0]=0xA4;
				}
				
			}else
			{
				wlSet.m_flaCmd[0]=0x3c;
			}
#else
			wlSet.m_flaCmd[0]=0x3c;	
#endif

#ifdef LIGH_CMD_REBUILD
			wlSet.m_flaCmd[1] = wlSet.m_ttlCmd[0];			
			wlSet.m_cmdIndex = 2;
#else
			wlSet.m_flaCmd[1]|=0x02;
			wlSet.m_flaCmd[1]|=(wlSet.m_ttlCmd[0]<<2);
			wlSet.m_flaCmd[2]|=(wlSet.m_ttlCmd[0]>>6);
			wlSet.m_cmdIndex = 3;
#endif
			break;

			case WLM_MODE_AB:
#ifdef _HP_SYN_
			if(HP_FLASHMODE_ON==hpSyn)
			{
				if(hpLex)
				{
					wlSet.m_flaCmd[0]=0xb5;
				}else
				{
					wlSet.m_flaCmd[0]=0xA5; 
				}
			}else
			{
				wlSet.m_flaCmd[0]=0x3d;
			}
#else
			wlSet.m_flaCmd[0]=0x3d;
#endif
			//wlSet.m_flaCmd[0]=0x3d;
#ifdef LIGH_CMD_REBUILD
			wlSet.m_flaCmd[1] = wlSet.m_ttlCmd[0];
			wlSet.m_flaCmd[2] = wlSet.m_ttlCmd[1];
			wlSet.m_cmdIndex = 3;
#else
			wlSet.m_flaCmd[1]|=0x02;
			wlSet.m_flaCmd[1]|=(wlSet.m_ttlCmd[0]<<2);
			wlSet.m_flaCmd[2]|=(wlSet.m_ttlCmd[0]>>6);
			wlSet.m_flaCmd[2]|=(2<<2);
			wlSet.m_flaCmd[2]|=(wlSet.m_ttlCmd[1]<<4);
			wlSet.m_flaCmd[3]|=(wlSet.m_ttlCmd[1]>>4);
			wlSet.m_cmdIndex = 4;
#endif
			break;

			case WLM_MODE_RES:
#ifdef _HP_SYN_
			if(HP_FLASHMODE_ON==hpSyn)
			{
				if(hpLex)
				{
					wlSet.m_flaCmd[0]=0xB6;
				}else
				{
					wlSet.m_flaCmd[0]=0xA6;
				}
			}else
			{
				wlSet.m_flaCmd[0]=0x3e;
			}
#else
			wlSet.m_flaCmd[0]=0x3e;
#endif
#ifdef LIGH_CMD_REBUILD
			wlSet.m_flaCmd[1] = wlSet.m_ttlCmd[0];
			wlSet.m_flaCmd[2] = wlSet.m_ttlCmd[1];
			wlSet.m_flaCmd[3] = wlSet.m_ttlCmd[2];
			wlSet.m_cmdIndex = 4;
#else
			wlSet.m_flaCmd[1]|=0x02;
			wlSet.m_flaCmd[1]|=(wlSet.m_ttlCmd[0]<<2);
			wlSet.m_flaCmd[2]|=(wlSet.m_ttlCmd[0]>>6);
			wlSet.m_flaCmd[2]|=(2<<2);
			wlSet.m_flaCmd[2]|=(wlSet.m_ttlCmd[1]<<4);
			wlSet.m_flaCmd[3]|=(wlSet.m_ttlCmd[1]>>4);
			wlSet.m_flaCmd[3]|=(2<<4);
			wlSet.m_flaCmd[3]|=(wlSet.m_ttlCmd[2]<<6);
			wlSet.m_flaCmd[4]|=(wlSet.m_ttlCmd[2]>>2);
			wlSet.m_cmdIndex = 5;
#endif
			break;
		}
		
	}else if(WORKINGMODE_MANU==mode)
	{
#ifdef _HP_SYN_
		if(HP_FLASHMODE_ON==hpSyn)
		{
			if(hpLex)
			{
				wlSet.m_flaCmd[0] = 0xB7;
			}else
			{
				wlSet.m_flaCmd[0] = 0xA7;
			}
		}else
		{
			wlSet.m_flaCmd[0] = 0x0f;
		}
#else
			wlSet.m_flaCmd[0] = 0x0f;
#endif
		switch(*wlSet.arr_mode)
		{
			case WLM_MODE_ALL:
#ifdef LIGH_CMD_REBUILD
				wlSet.m_flaCmd[1] = (*wlSet.arr_mode<<1)+1;
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_A].output1*3+wlSet.m_arrPara[WL_ARRARY_A].output2];
				wlSet.m_flaCmd[1] |= (tmp<<3);
				wlSet.m_cmdIndex = 2;
#else
				wlSet.m_flaCmd[1] |= 0x06; 
				//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].output1>>4)*3+(wlSet.m_arrPara[WL_ARRARY_A].output2&0x0f)];
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_A].output1*3+wlSet.m_arrPara[WL_ARRARY_A].output2];
				wlSet.m_flaCmd[1] |= ((tmp&0x7)<<5);
				wlSet.m_flaCmd[2] |= (tmp>>3);
				wlSet.m_cmdIndex = 3;
#endif
			break;

			case WLM_MODE_AB:
#ifdef LIGH_CMD_REBUILD
				wlSet.m_flaCmd[1] = (*wlSet.arr_mode<<1)+1;
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_A].output1*3+wlSet.m_arrPara[WL_ARRARY_A].output2];
				wlSet.m_flaCmd[1] |= (tmp<<3);
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_B].output1*3+wlSet.m_arrPara[WL_ARRARY_B].output2];
				wlSet.m_flaCmd[2] = tmp<<3;
				wlSet.m_cmdIndex = 3;								
#else
				//wlSet.m_flaCmd[1] |= 0x16;
				wlSet.m_flaCmd[1]|=0x0e;
				//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].output1>>4)*3+(wlSet.m_arrPara[WL_ARRARY_A].output2&0x0f)];
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_A].output1*3+wlSet.m_arrPara[WL_ARRARY_A].output2];
				wlSet.m_flaCmd[1] |= ((tmp&0x7)<<5);
				wlSet.m_flaCmd[2] |= (tmp>>3);
				wlSet.m_flaCmd[2] |= (1<<3);		//01000
				//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].output1>>4)*3+(wlSet.m_arrPara[WL_ARRARY_B].output2&0x0f)];
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_B].output1*3+wlSet.m_arrPara[WL_ARRARY_B].output2];
				//wlSet.m_flaCmd[3] |= (tmp<<2);
				//wlSet.m_flaCmd[4] |= (tmp>>6);
				wlSet.m_flaCmd[2] |= (tmp<<7);
				wlSet.m_flaCmd[3] |= ((tmp>>1)&0x0f);
				wlSet.m_cmdIndex = 4;
#endif
			break;

			case WLM_MODE_RES:
				//wlSet.m_flaCmd[1] |= 0x0e;
#ifdef LIGH_CMD_REBUILD
				wlSet.m_flaCmd[1] = (*wlSet.arr_mode<<1)+1;
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_A].output1*3+wlSet.m_arrPara[WL_ARRARY_A].output2];
				wlSet.m_flaCmd[1] |= (tmp<<3);
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_B].output1*3+wlSet.m_arrPara[WL_ARRARY_B].output2];
				wlSet.m_flaCmd[2] = tmp<<3;
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_C].output1*3+wlSet.m_arrPara[WL_ARRARY_C].output2];
				wlSet.m_flaCmd[3] = tmp<<3;
				wlSet.m_cmdIndex = 4;	
#else
				wlSet.m_flaCmd[1] |= 0x16;
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_A].output1*3+wlSet.m_arrPara[WL_ARRARY_A].output2];
				wlSet.m_flaCmd[1] |= ((tmp&0x7)<<5);
				wlSet.m_flaCmd[2] |= (tmp>>3);
				wlSet.m_flaCmd[2] |= (1<<3);		//01000
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_B].output1*3+wlSet.m_arrPara[WL_ARRARY_B].output2];
				//wlSet.m_flaCmd[3] |= (tmp<<2);
				//wlSet.m_flaCmd[4] |= (tmp>>6);
				wlSet.m_flaCmd[2] |=(tmp<<7);
				wlSet.m_flaCmd[3] |= ((tmp>>1)&0x0f);
				wlSet.m_flaCmd[3] |= (1<<5);
				//wlSet.m_flaCmd[4] |= (1<<3);		//01000
				tmp = wl_opArr[wlSet.m_arrPara[WL_ARRARY_C].output1*3+wlSet.m_arrPara[WL_ARRARY_C].output2];
				//wlSet.m_flaCmd[4] |= ((tmp&0x01)<<7);
				//wlSet.m_flaCmd[5] |= (tmp>>1);
				wlSet.m_flaCmd[4] |=(( tmp<<1)&0x3e);
				wlSet.m_cmdIndex = 5;
#endif
			break;
		}
	}
#ifdef _MULTIPLY_FLASH_
	else if(WORKINGMODE_MULTI==mode)
	{

			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
#ifdef LIGH_CMD_REBUILD
					wlSet.m_flaCmd[0] = 0x9f;
					wlSet.m_flaCmd[1] = WlmMulFrqRev()&0x3F;
					tmp = WlmMulTimeRev(); 
					wlSet.m_flaCmd[1] |= ((tmp&0x03)<<6);
					wlSet.m_flaCmd[2] = (tmp>>2)&0x07;
					//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp)*3];
					wlSet.m_flaCmd[2]|= (tmp<<3);		//5bits
					wlSet.m_cmdIndex = 3;
					
					
#else
					wlSet.m_flaCmd[0] = 0x9f;
					tmp = (WlmMulFrqRev()<<2 | 0x02);
					wlSet.m_flaCmd[1] = tmp;
					tmp = (WlmMulTimeRev()&0X7F);
					wlSet.m_flaCmd[2] = tmp;
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					wlSet.m_flaCmd[2] |=( tmp<<7)&0x80;
					wlSet.m_flaCmd[3]=(tmp>>1)&0x0f;
					wlSet.m_cmdIndex = 4;
#endif
				break;

				case WLM_MODE_AB:
#ifdef LIGH_CMD_REBUILD
					wlSet.m_flaCmd[0] = 0xaf;
					wlSet.m_flaCmd[1] = WlmMulFrqRev()&0x3F;
					tmp = WlmMulTimeRev(); 
					wlSet.m_flaCmd[1] |= ((tmp&0x03)<<6);
					wlSet.m_flaCmd[2] = (tmp>>2)&0x07;
					//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp)*3];
					wlSet.m_flaCmd[2]|= (tmp<<3);		//5bits
					//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4)*3];
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp)*3];
					wlSet.m_flaCmd[3]  = tmp<<3;
					wlSet.m_cmdIndex = 4;
					
#else
					wlSet.m_flaCmd[0] = 0xaf;
					tmp = (WlmMulFrqRev()<<2 | 0x02);
					wlSet.m_flaCmd[1] = tmp;
					tmp = (WlmMulTimeRev()&0X7F);
					wlSet.m_flaCmd[2] = tmp;
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					wlSet.m_flaCmd[2] |=( tmp<<7)&0x80;
					wlSet.m_flaCmd[3]=(tmp>>1)&0x0f;
					wlSet.m_flaCmd[3] |= (1<<5);
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4)*3];
					wlSet.m_flaCmd[4] = ((tmp&0x1f)<<1);//((tmp<<1)&0x1f);
					wlSet.m_cmdIndex = 5;
#endif
				break;

				case WLM_MODE_RES:
#ifdef LIGH_CMD_REBUILD
					wlSet.m_flaCmd[0] = 0xbf;
					wlSet.m_flaCmd[1] = WlmMulFrqRev()&0x3F;
					tmp = WlmMulTimeRev(); 
					wlSet.m_flaCmd[1] |= ((tmp&0x03)<<6);
					wlSet.m_flaCmd[2] = (tmp>>2)&0x07;
					//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp)*3];
					wlSet.m_flaCmd[2]|= (tmp<<3);		//5bits
					//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4)*3];
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp)*3];
					wlSet.m_flaCmd[3]  = tmp<<3;
					//tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_C].multi_exp>>4)*3];
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_C].multi_exp)*3];
					wlSet.m_flaCmd[4]  = tmp<<3;
					wlSet.m_cmdIndex = 5;
#else
					wlSet.m_flaCmd[0] = 0xbf;
					tmp = (WlmMulFrqRev()<<2 | 0x02);
					wlSet.m_flaCmd[1] = tmp;
					tmp = (WlmMulTimeRev()&0X7F);
					wlSet.m_flaCmd[2] = tmp;
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					wlSet.m_flaCmd[2] |=( tmp<<7)&0x80;
					wlSet.m_flaCmd[3]=(tmp>>1)&0x0f;
					wlSet.m_flaCmd[3] |= (1<<5);
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4)*3];
					wlSet.m_flaCmd[4] = ((tmp&0x1f)<<1);//((tmp<<1)&0x1f);
					wlSet.m_flaCmd[4] |= (1<<7);
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_C].multi_exp>>4)*3];
					wlSet.m_flaCmd[5] = ((tmp&0x1f)<<3);//((tmp<<3)&0x1f);
					wlSet.m_cmdIndex = 6;
#endif
				break;
			}
			
	}
#endif
}


#if 0
void WlmRatioVary(void)
{
	//if(WLM_ON==*wlSet.m_on)
	if(CMD_MODE_LIGH==*g_cmdMode)
	{
		switch(*wlSet.arr_mode)
		{
			case 0x00:
			*wlSet.arr_mode=WLM_MODE_ALL;
			//Parameter_Array[PARA_WLM_MODE] = 0x5d;
			//Parameter_Array[PARA_WIRELESS] &= 0xCF;
			//Parameter_Array[PARA_WIRELESS] |= (*wlSet.arr_mode<<4);
			//SETBIT(bitWireless,BIT_bitMaster_Para);
			break;
			
			case 0x01:
			*wlSet.arr_mode=WLM_MODE_AB;
			//Parameter_Array[PARA_WLM_MODE] = 0x5e;
			Parameter_Array[PARA_WIRELESS] &= 0xCF;
			Parameter_Array[PARA_WIRELESS] |= (*wlSet.arr_mode<<4);
			//SETBIT(bitWireless,BIT_bitMaster_Para);
			break;

			case 0x02:
			*wlSet.arr_mode=WLM_MODE_RES;
			//Parameter_Array[PARA_WLM_MODE] = 0x5f;
			//Parameter_Array[PARA_WIRELESS] &= 0xCF;
			//Parameter_Array[PARA_WIRELESS] |= (*wlSet.arr_mode<<4);
			//SETBIT(bitWireless,BIT_bitMaster_Para);
			break;					
		}
	}

}
#endif

void FoclenRefresh(void)
{
//	u8 i;

	if(MOTO_MODE_AUTO==*motoSet.mode)
	{
		//if(menuPara.motoLoc!=motoSet.destRsv)
		//if(menuPara.motoLoc!=motoSet.focLen)
		if(menuPara.motoLoc!=motoSet.autoRsv)
		{
			motoSet.autoRsv = menuPara.motoLoc;
			/*
			if(menuPara.motoLoc>=focLenScape[motoSet.focIndex+1])
			{
				motoSet.destRsv = focLenScape[motoSet.focIndex+1];
				
			}else if(menuPara.motoLoc<=focLenScape[motoSet.focIndex-1])
			{
				motoSet.destRsv = focLenScape[motoSet.focIndex-1];
			}
			*/
			motoSet.focIndex = GetMotoStdIndex(menuPara.motoLoc);
			motoSet.destRsv = focLenScape[motoSet.focIndex];
			//motoSet.destRsv = focLenScape[GetMotoStdIndex(menuPara.motoLoc)];
			motoSet.autoDest = motoSet.destRsv;
			
		}
	}

	/*
	if(motoSet.destRsv!=*motoSet.dest_location)
	{
		*motoSet.dest_location = motoSet.destRsv;
		for(i=0;i<sizeof(focLenScape);i++)
		{ 
			//if((focLenScape[i]<*motoSet.dest_location)&&(*motoSet.dest_location<=focLenScape[i+1]))
			if((focLenScape[i]<=*motoSet.dest_location)&&(*motoSet.dest_location<focLenScape[i+1]))
			{
				//motoSet.focIndex = i+1;
				motoSet.focIndex = i;
				break;
			}
		}
	}
	*/
	DisplayFocLen(*motoSet.mode,*motoSet.dest_location,0); 
}

int ChkAllNone(uchar* arr, uchar sz)
{
	uchar i=0;
	for(i=0;i<sz;i++)
	{
		if(arr[i]!=0)
		{
			return 1;
		}
	}

	return 0;
}

void ClearSlavesStatue(void)
{
	u8 i=0;
	u8 voidCnt=0;
	for(i=0;i<5;i++)
	{
		if(!radioStatue.devCnt[i])
		{
			voidCnt++;
			radioStatue.devStatue[i] = 0;
		}
	}
	if(voidCnt>=5)
	{
		g_slaveReady = 0;
	}

}

void DspUpdate(void)
{
	INT8U tmrErr;
#if 1
	if(g_dspUpdateFlag&DSP_UPDATE_MODE)
	{
		//if(g_SettingIndex!=SETTING_RFSCAN)
		//{
			g_SettingIndex=0;
			//Mode(mainPara.workMode);
			Mode();
		//}
		//LoadButtons();
		//if(CMD_MODE_LIGH_MST==*g_cmdMode)
		//{
		//	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,DSP_NORMAL,0);
		//}
		//else if(CMD_MODE_RF_MST==*g_cmdMode)
		//{
		//	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
		//}
		g_dspUpdateFlag ^= DSP_UPDATE_MODE;
	}
#endif

	if(g_dspUpdateFlag&DSP_UPDATE_MODENOCLR)
	{
		//if(g_SettingIndex!=SETTING_RFSCAN)
		//{
			Mode();
		//}
		g_dspUpdateFlag ^= DSP_UPDATE_MODENOCLR;
	}

	if(g_dspUpdateFlag&DSP_SLACMD_REL)
	{
		if(CMD_MODE_RFLS_MST!=*g_cmdMode)
		{
			if(COM_STATUE_IDLE==g_comStatue)
			{
				ACT_SHUTTER = 1;
			}
			WIRE_SHUTTER = 1;
			g_shutterFlag = UART_CC_REL_FLA;
			//gWSleepCounter = 0;
			//FLAOK_ON();
			OSTmrStart(g_shutterPullTmr, &tmrErr);			//it cannot start a timer in interrupt routine
		}
		gWSleepCounter = 0;
		g_dspUpdateFlag^=DSP_SLACMD_REL;
	}

#if 1

#if 1

#ifdef _SPEED_LIGHT_
	if(g_dspUpdateFlag&DSP_UPDATE_MOTO)
	{
		FoclenRefresh();
		g_dspUpdateFlag^=DSP_UPDATE_MOTO;
		SET_UPDATE_EVT(DSP_UPDATE_LINK);
	}
#endif


	if(g_dspUpdateFlag&DSP_UPDATE_RFCHAN)
	{
		//while(uart_tran_flag);
		WaitForUartIdle(50);			// 2016/02/17 for deadlock
		//UartSctSend(UART_ENCODE_EXIT, 1, 0);
		//OSTimeDlyHMSM(0,0,0,50);
		//LoadUartSendBuf(UART_ENCODE_RFID, radioSet->id>>8, radioSet->id&0xff);
		LoadUartSendBuf(UART_ENCODE_RFCH,radioSet->rfChan,EXIT_BEFORE_SET);
		TrigerUartSend(0);
		g_dspUpdateFlag^=DSP_UPDATE_RFCHAN;
	}

	if(g_dspUpdateFlag&DSP_UPDATE_RFID)
	{
		//while(uart_tran_flag);
		WaitForUartIdle(50);			// 2016/02/17 for deadlock
		UartSctSend(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
		TrigerUartSend(0);
		g_dspUpdateFlag^=DSP_UPDATE_RFID;
	}

	if(g_dspUpdateFlag&DSP_UPDATE_RFSW)
	{
		RfMoudleSet();
		//while(uart_tran_flag);
		//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		g_dspUpdateFlag^=DSP_UPDATE_RFSW;
	}
#endif
	
#if 1
	if(g_dspUpdateFlag&DSP_UPDATE_LINK)
	{
		//if(COM_STATUE_START==g_comStatue)
		//{
			//BkLiteOn();
		//}else if(COM_STATUE_IDLE==g_comStatue)
		//{
			//BkLiteAutoOff();
			//if(BK_OFF==bkParas.bkFlag)
			//{
			//	BkLiteAutoOff();
			//}
		//}
		if(!g_cmrLinked)
		{
			//flaPara.cmrCmp1 = 0;
			//flaPara.cmrCmp2 = 0;
			wlSet.cmrCmp1 = 0;
			wlSet.cmrCmp2 = 0;
			flaPara.cmrCmpCode =0;
		}

		if((CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_LIGH_SLA==*g_cmdMode))
		{
			*g_cmdMode = CMD_MODE_OFF;
			mainPara.workMode = WORKINGMODE_TTL;
			DISABLE_RF_MODULE();
		}

		//g_cmrLinked = 1;
		//if(g_cmrPressed>0)
		//{
			//g_SettingIndex = 0;
		//}
		
		//tmp1 = menuPara.cApert>>4;
		//tmp2 = menuPara.cApert&0x0F;
		
		//flaPara.apert = CannonApertVal[tmp1*6+(tmp2+2)/3];

		gWSleepCounter = 0;

		//Mode();

		//if((g_cmrLinked>0)&&(CMD_MODE_OFF==*g_cmdMode))
		/*
		if(CMD_MODE_OFF==*g_cmdMode)
		{
			//if(g_cmrLinked>0)
			if(g_cmrPressed>0)
			{
				DspApertValue(flaPara.apert,DSP_NORMAL);
			}else
			{
				LcdClearFeild(7, 141, 27, 16);
			}
		}*///else
		//{
		//	LcdClearFeild(7, 141, 27, 16);
		//}
#if 0			
		if(CMD_MODE_OFF==*g_cmdMode)
		{
			if(WORKINGMODE_EXTM==mainPara.workMode)
			{
				DrawScale(mainPara.workMode,0,1);
			}else if(WORKINGMODE_MANU==mainPara.workMode)
			{
				//DrawScale(mainPara.workMode,(mstPara->output1-1)*3+mstPara->output2,g_cmrLinked);
				DrawScale(mainPara.workMode,(mstPara->output1)*3+mstPara->output2,g_cmrPressed);
			}else if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				//DrawScale(mainPara.workMode,((*multiSet.expose>>4)-1)*3,g_cmrLinked);
#ifdef _CANON_
				DrawScale(mainPara.workMode,*multiSet.expose*3,g_cmrPressed);
#else
				DrawScale(mainPara.workMode,((*multiSet.expose>>4))*3,g_cmrPressed);
#endif
			}else
			{
				//DrawScale(mainPara.workMode,0,g_cmrLinked);
				DrawScale(mainPara.workMode,0,g_cmrPressed);
			}
		}
#endif
		g_dspUpdateFlag^=DSP_UPDATE_LINK;
		//SET_UPDATE_EVT(DSP_UPDATE_MODE);
		SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
	}
#endif

#if 1
	if(g_dspUpdateFlag&DSP_UPDATE_LKDEV)
	{
		if((SETTING_RFSCAN!=g_SettingIndex)&&(SETTING_ID_SEL!=g_SettingIndex)&&(SETTING_SNRMENU!=g_SettingIndex)
			 &&(SETTING_RFGR_SEL !=g_SettingIndex)&&(SETTING_ID_SET!=g_SettingIndex)&&(SETTING_PFNMENU!=g_SettingIndex)
			 &&(SETTING_PFNSUB!=g_SettingIndex)&&(SETTING_SNRSUB!=g_SettingIndex)
			 &&(*g_cmdMode==CMD_MODE_RF_MST))
		{
			RfDevLinkMark(mainPara.workMode,&wlSet);
			/*
			if(!ChkAllNone(radioStatue.devCnt,5))
			{
				CHARGE_LED1_OFF;
				g_chargeOk = AD_CHARGE_LOW;	
			}else
			{
				CHARGE_LED1_ON;
				g_chargeOk = AD_CHARGE_RDY;
			}
			*/
			/*
			if(RfMasterReadyCheck()>0)
			{
				CHARGE_LED_OK();
				CR_READY = 1;
			}else
			{
				CHARGE_LED_OFF();
				CR_READY = 0;
			}
			*/
		}

		ClearSlavesStatue();
		RfMasterReadySet();
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
		g_dspUpdateFlag^=DSP_UPDATE_LKDEV;
	}
#endif

#ifdef TTL_DEBUG
	if(g_dspUpdateFlag&DSP_UPDATE_LITETEST)
	{
		LCD_Clear(0);
		for(tmp1=0;tmp1<canWireless.WLBufIndex;tmp1++)
		{
			TtlDebugDspDat(2, tmp1*24, canWireless.WL_buf[tmp1], DSP_NORMAL);
		}
		canWireless.WLBufIndex = 0;
		LightSlaveParaClear();
		g_dspUpdateFlag^=DSP_UPDATE_LITETEST;
	}
#endif

	if(g_dspUpdateFlag&DSP_UPDATE_SETSLATMR)
	{
		SetFlashTimeOut(g_slaveTimer);
		g_dspUpdateFlag^=DSP_UPDATE_SETSLATMR;
	}

	if(g_dspUpdateFlag&DSP_SLACMD_TEST)
	{
		Pilot();
		gWSleepCounter = 0;
		g_dspUpdateFlag^=DSP_SLACMD_TEST;
	}

	if(g_dspUpdateFlag&DSP_SLACMD_MODEL)
	{
#ifdef UART1_TRACE_DEBUG
		printf("Model\r\n");
#endif
		UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
		gWSleepCounter = 0;
		//SpiRearUp(100);
		SpiRearUp(20);
		g_isModeling = 1;
		g_dspUpdateFlag^=DSP_SLACMD_MODEL;
		
	}

/*
	if(g_dspUpdateFlag&DSP_UPDATE_MULCAL)
	{
		for(search_index=0;search_index<LENOFTIMETABLE;search_index++)
		{
			if(multiSet.sCode==shutterTable[search_index].index)
			{
				flaPara.quickTime = shutterTable[search_index].ms;
				break;
			}
		}

		if(WORKINGMODE_MULTI==mainPara.workMode)
		{
			if(((unsigned long)flaPara.quickTime*(unsigned long)(*multiSet.frequence))>((unsigned long)(*multiSet.eTimes)*1000))
			{
				wlSet.muitl_realTimes = *multiSet.eTimes;
			}else
			{
				wlSet.muitl_realTimes = (unsigned long)(flaPara.quickTime*(*multiSet.frequence))/1000+1;
			}
		}

		
		g_dspUpdateFlag^=DSP_UPDATE_MULCAL;
	}
*/
#endif

}

void TaskDspUpdate(void *pdata)
{
//	int i;
//	unsigned char search_index;
//	u8 tmp1,tmp2;
//	INT8U tmrErr;

	while(1)
	{
		DspUpdate();
#if 0
#if 1
		if(g_dspUpdateFlag&DSP_UPDATE_MODE)
		{
			//if(g_SettingIndex!=SETTING_RFSCAN)
			//{
				g_SettingIndex=0;
				//Mode(mainPara.workMode);
				Mode();
			//}
			//LoadButtons();
			//if(CMD_MODE_LIGH_MST==*g_cmdMode)
			//{
			//	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,DSP_NORMAL,0);
			//}
			//else if(CMD_MODE_RF_MST==*g_cmdMode)
			//{
			//	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
			//}
			g_dspUpdateFlag ^= DSP_UPDATE_MODE;
		}
#endif

		if(g_dspUpdateFlag&DSP_UPDATE_MODENOCLR)
		{
			//if(g_SettingIndex!=SETTING_RFSCAN)
			//{
				Mode();
			//}
			g_dspUpdateFlag ^= DSP_UPDATE_MODENOCLR;
		}

		if(g_dspUpdateFlag&DSP_SLACMD_REL)
		{
			if(COM_STATUE_IDLE==g_comStatue)
			{
				ACT_SHUTTER = 1;
			}
			WIRE_SHUTTER = 1;
			gWSleepCounter = 0;
			//FLAOK_ON();
			OSTmrStart(g_shutterPullTmr, &tmrErr);			//it cannot start a timer in interrupt routine
			g_dspUpdateFlag^=DSP_SLACMD_REL;
		}

#if 1

#if 1

#ifdef _SPEED_LIGHT_
		if(g_dspUpdateFlag&DSP_UPDATE_MOTO)
		{
			FoclenRefresh();
			g_dspUpdateFlag^=DSP_UPDATE_MOTO;
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
#endif


		if(g_dspUpdateFlag&DSP_UPDATE_RFCHAN)
		{
			while(uart_tran_flag);
			//UartSctSend(UART_ENCODE_EXIT, 1, 0);
			//OSTimeDlyHMSM(0,0,0,50);
			//LoadUartSendBuf(UART_ENCODE_RFID, radioSet->id>>8, radioSet->id&0xff);
			LoadUartSendBuf(UART_ENCODE_RFCH,radioSet->rfChan,EXIT_BEFORE_SET);
			TrigerUartSend(0);
			g_dspUpdateFlag^=DSP_UPDATE_RFCHAN;
		}

		if(g_dspUpdateFlag&DSP_UPDATE_RFID)
		{
			while(uart_tran_flag);
			UartSctSend(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
			TrigerUartSend(0);
			g_dspUpdateFlag^=DSP_UPDATE_RFID;
		}

		if(g_dspUpdateFlag&DSP_UPDATE_RFSW)
		{
			RfMoudleSet();
			//while(uart_tran_flag);
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
			g_dspUpdateFlag^=DSP_UPDATE_RFSW;
		}
#endif
		
#if 1
		if(g_dspUpdateFlag&DSP_UPDATE_LINK)
		{
			//if(COM_STATUE_START==g_comStatue)
			//{
				//BkLiteOn();
			//}else if(COM_STATUE_IDLE==g_comStatue)
			//{
				//BkLiteAutoOff();
				//if(BK_OFF==bkParas.bkFlag)
				//{
				//	BkLiteAutoOff();
				//}
			//}
			if(!g_cmrLinked)
			{
				//flaPara.cmrCmp1 = 0;
				//flaPara.cmrCmp2 = 0;
				wlSet.cmrCmp1 = 0;
				wlSet.cmrCmp2 = 0;
				flaPara.cmrCmpCode =0;
			}

			if((CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_LIGH_SLA==*g_cmdMode))
			{
				*g_cmdMode = CMD_MODE_OFF;
				mainPara.workMode = WORKINGMODE_TTL;
				DISABLE_RF_MODULE();
			}

			//g_cmrLinked = 1;
			//if(g_cmrPressed>0)
			//{
				//g_SettingIndex = 0;
			//}
			
			//tmp1 = menuPara.cApert>>4;
			//tmp2 = menuPara.cApert&0x0F;
			
			//flaPara.apert = CannonApertVal[tmp1*6+(tmp2+2)/3];

			gWSleepCounter = 0;

			//Mode();

			//if((g_cmrLinked>0)&&(CMD_MODE_OFF==*g_cmdMode))
			/*
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				//if(g_cmrLinked>0)
				if(g_cmrPressed>0)
				{
					DspApertValue(flaPara.apert,DSP_NORMAL);
				}else
				{
					LcdClearFeild(7, 141, 27, 16);
				}
			}*///else
			//{
			//	LcdClearFeild(7, 141, 27, 16);
			//}
#if 0			
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				if(WORKINGMODE_EXTM==mainPara.workMode)
				{
					DrawScale(mainPara.workMode,0,1);
				}else if(WORKINGMODE_MANU==mainPara.workMode)
				{
					//DrawScale(mainPara.workMode,(mstPara->output1-1)*3+mstPara->output2,g_cmrLinked);
					DrawScale(mainPara.workMode,(mstPara->output1)*3+mstPara->output2,g_cmrPressed);
				}else if(WORKINGMODE_MULTI==mainPara.workMode)
				{
					//DrawScale(mainPara.workMode,((*multiSet.expose>>4)-1)*3,g_cmrLinked);
#ifdef _CANON_
					DrawScale(mainPara.workMode,*multiSet.expose*3,g_cmrPressed);
#else
					DrawScale(mainPara.workMode,((*multiSet.expose>>4))*3,g_cmrPressed);
#endif
				}else
				{
					//DrawScale(mainPara.workMode,0,g_cmrLinked);
					DrawScale(mainPara.workMode,0,g_cmrPressed);
				}
			}
#endif
			g_dspUpdateFlag^=DSP_UPDATE_LINK;
			//SET_UPDATE_EVT(DSP_UPDATE_MODE);
			SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
		}
#endif

#if 1
		if(g_dspUpdateFlag&DSP_UPDATE_LKDEV)
		{
			if((SETTING_RFSCAN!=g_SettingIndex)&&(SETTING_ID_SEL!=g_SettingIndex)&&(SETTING_SNRMENU!=g_SettingIndex)
				 &&(SETTING_RFGR_SEL !=g_SettingIndex)&&(SETTING_ID_SET!=g_SettingIndex)&&(*g_cmdMode==CMD_MODE_RF_MST))
			{
				RfDevLinkMark(mainPara.workMode,&wlSet);
				if(!ChkAllNone(radioStatue.devCnt,5))
				{
					CHARGE_LED1_OFF;
					g_chargeOk = AD_CHARGE_LOW;	
				}else
				{
					CHARGE_LED1_ON;
					g_chargeOk = AD_CHARGE_RDY;
				}
			}
			g_dspUpdateFlag^=DSP_UPDATE_LKDEV;
		}
#endif

#ifdef TTL_DEBUG
		if(g_dspUpdateFlag&DSP_UPDATE_LITETEST)
		{
			LCD_Clear(0);
			for(tmp1=0;tmp1<canWireless.WLBufIndex;tmp1++)
			{
				TtlDebugDspDat(2, tmp1*24, canWireless.WL_buf[tmp1], DSP_NORMAL);
			}
			canWireless.WLBufIndex = 0;
			LightSlaveParaClear();
			g_dspUpdateFlag^=DSP_UPDATE_LITETEST;
		}
#endif

		if(g_dspUpdateFlag&DSP_UPDATE_SETSLATMR)
		{
			SetFlashTimeOut(g_slaveTimer);
			g_dspUpdateFlag^=DSP_UPDATE_SETSLATMR;
		}

		if(g_dspUpdateFlag&DSP_SLACMD_TEST)
		{
			Pilot();
			gWSleepCounter = 0;
			g_dspUpdateFlag^=DSP_SLACMD_TEST;
		}

		if(g_dspUpdateFlag&DSP_SLACMD_MODEL)
		{
#ifdef UART1_TRACE_DEBUG
			printf("Model\r\n");
#endif
			UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			gWSleepCounter = 0;
			//SpiRearUp(100);
			SpiRearUp(20);
			g_isModeling = 1;
			g_dspUpdateFlag^=DSP_SLACMD_MODEL;
			
		}

/*
		if(g_dspUpdateFlag&DSP_UPDATE_MULCAL)
		{
			for(search_index=0;search_index<LENOFTIMETABLE;search_index++)
			{
				if(multiSet.sCode==shutterTable[search_index].index)
				{
					flaPara.quickTime = shutterTable[search_index].ms;
					break;
				}
			}

			if(WORKINGMODE_MULTI==mainPara.workMode)
			{
				if(((unsigned long)flaPara.quickTime*(unsigned long)(*multiSet.frequence))>((unsigned long)(*multiSet.eTimes)*1000))
				{
					wlSet.muitl_realTimes = *multiSet.eTimes;
				}else
				{
					wlSet.muitl_realTimes = (unsigned long)(flaPara.quickTime*(*multiSet.frequence))/1000+1;
				}
			}

			
			g_dspUpdateFlag^=DSP_UPDATE_MULCAL;
		}
*/
#endif
#endif
		OSTimeDlyHMSM(0,0,0,50);
		
		//g_dspUpdateFlag=0;
	}
	
}

unsigned char RfSlaveZeroTimes(void)
{
//	unsigned char tmp0;
//	unsigned long tmp3;
	u8 search_index;
	
	for(search_index=0;search_index<LENOFTIMETABLE;search_index++)
	{
		//if(multiSet.sCode==shutterTable[search_index].index)
		if(flaPara.shutter==shutterTable[search_index].index)
		{
			flaPara.quickTime = shutterTable[search_index].ms;
			break;
		}
	}

	if(((unsigned long)flaPara.quickTime*(unsigned long)(canWireless.wlMul_frq))>((unsigned long)(canWireless.wlMul_time)*1000))
	{
		//wlSet.muitl_realTimes = *multiSet.eTimes;
		return canWireless.wlMul_time;
	}else
	{
		//wlSet.muitl_realTimes = (unsigned long)(flaPara.quickTime*(*multiSet.frequence))/1000+1;
		return ((unsigned long)(flaPara.quickTime*(canWireless.wlMul_frq))/1000+1);
	}

}





















