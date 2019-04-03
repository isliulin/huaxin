#include "spi.h"
#include "../user/canon.h"
#include "led.h"
#include "../user/golbal.h"
#include "../user/Display.h"
#include "../user/KeyProc.h"
#include "../TIMER/timer.h"
#include "../system/usart/usart.h"

#define MAX_BUFFER_SIZE    100//20           // Maximum buffer Master will send
#define SEND_MAX_SIZE 100//20
#define LENOFTIMETABLE 40

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


extern OS_TMR  *g_bkTmr;
unsigned char g_spiRcvIndex=0;//SPI_Receive_Index;
unsigned char g_spiSendIndex=0;
//unsigned char g_spiRear=0;
unsigned char g_tmHandleFlag=0;
unsigned char g_spiRcvData[MAX_BUFFER_SIZE] = {0};
unsigned char g_spiSendData[SEND_MAX_SIZE]={0};
unsigned char *g_cmdMode;
unsigned char g_vCmr=0;
extern WlSet wlSet;
unsigned char g_activeControl=0x00;
unsigned char g_comStart=0;
MenuPara menuPara={0};
//RfGroupPara rfGrPara={0,0,0,{0},{0}};
extern MotoSet motoSet;
unsigned char g_mstPrefla=0;
//unsigned char g_mstMode=0;
//unsigned char charge_ok=0;
unsigned char g_getTtlFlashCode=0;
unsigned char ttl_expCodeRsv=0;
extern unsigned char g_comStatue;
extern RadioSet* radioSet ;
extern RadioStatue radioStatue;
extern char g_SettingIndex;
extern unsigned char uart_tran_flag;
//RadioPara radioPara;
u16 g_dspUpdateFlag=0;
unsigned char g_shutterFlag=0;
extern OS_TMR  *g_1ShotTmr;
extern OS_TMR  *g_shutterPullTmr;
u8 g_hpLev;
extern char g_chargeOk;
extern Word gWSleepCounter;
unsigned char Parameter_Array[13]= {0x20,0x5d,0x53,0x13,0x23,0x20,0x00,0x00,0x01,0xa8,0x00,0x00,0x5c};
const unsigned char wl_opArr[22] = {0x03,0x05,0x06,0x07,0x09,0x0a,0x0b,0x0d,0x0e,0x0f,0x11,0x12,0x13,0x15,0x16,0x17,0x19,0x1a,0x1b,0x1d,0x1e,0x1f};
extern void CmdModDisplay(void);
extern void RfModSwInfo(void);
extern char *pSnrSet;
#ifdef UART1_TRACE_DEBUG
extern unsigned char rcvBuf[64];
#endif

u8 g_cmrCode;
extern char g_cmrLinked;
//extern u8 g_setFlag;

extern void ExitEditMode(void);

#ifdef _1DX_DEBUG_
u16 g_UnknowData[10] = {0};
u8 g_UkdCnt=0;
#endif


#if 0
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
#endif

signed char const compensation_table[37]={
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
0xe0,0xDd,0xDb,
0xD8,0xD5,0xD3,
0xD0,
};

typedef struct TIMEINDEX{	
	unsigned char index;
	int ms;
}timeTable;

const timeTable shutterTable[LENOFTIMETABLE]={
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

unsigned char abRation_fill1,abRation_fill2;

#ifdef _MULTIPLY_FLASH_
MultiSet multiSet;
#endif

extern u8 g_sleepFlag;

void CodeAnalysis(unsigned char rcvCode);
void FlashHandle(void);
void GetParameter(void);
void BuildExposeCmd(char mode,unsigned char hpSyn,unsigned char hpLex);
extern void BkLiteAutoOff(void);
extern void Pilot(void);

void SPIx_Init(void)
{	 
/***************PortInit************************************
	RCC->APB2ENR|=1<<2;       //PORTAÊ±ÖÓÊ¹ÄÜ 	 
	RCC->APB2ENR|=1<<12;      //SPI1Ê±ÖÓÊ¹ÄÜ 
		   
	//ÕâÀïÖ»Õë¶ÔSPI¿Ú³õÊ¼»¯
	GPIOA->CRL&=0X000FFFFF; 
	GPIOA->CRL|=0X8B800000;//PA5.6.7¸´ÓÃ 	    
	GPIOA->ODR|=0X7<<5;    //PA5.6.7ÉÏÀ­
*****************************************************/
	//SPIx_Disable();
	RCC->APB2RSTR|=(1<<12);
	RCC->APB2RSTR&=~(1<<12);

	RCC->APB2ENR|=1<<2;
	RCC->APB2ENR|=1<<12;      //SPI1Ê±ÖÓÊ¹ÄÜ 
	
	GPIOA->CRL&=0x000FFFFF; 
	GPIOA->CRL|=0x8B800000;//PA5.6.7¸´ÓÃPA4 input 	    
	GPIOA->ODR|=0x07<<5;    //PA5.6.7ÉÏÀ

	//RCC->APB2ENR|=1<<4;		//enable portc clock
	//GPIOC->CRL&=0xFFF0FFFF;
	//GPIOC->CRL|=0x00030000;
/*	
	SPI1->CR1|=0<<10;//È«Ë«¹¤Ä£Ê½	
	SPI1->CR1|=1<<9; //Èí¼þnss¹ÜÀí
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=0<<2; //SPI´Ó»ú
	SPI1->CR1|=0<<11;//8bitÊý¾Ý¸ñÊ½	
	SPI1->CR1|=1<<1; //¿ÕÏÐÄ£Ê½ÏÂSCKÎª1 CPOL=1
	SPI1->CR1|=1<<0; //Êý¾Ý²ÉÑù´ÓµÚ¶þ¸öÊ±¼ä±ßÑØ¿ªÊ¼,CPHA=1  
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
	SPI1->DR = 0xFF;

	//SPIx_ReadWriteByte(0x00);//Æô¶¯´«Êä		 


}

void EXTIx_Init(u8 TirEdge)
{
	//RCC->APB2ENR|=1<<2;     //Ê¹ÄÜPORTAÊ±ÖÓ
	//JTAG_Set(JTAG_SWD_DISABLE);//¹Ø±ÕJTAGºÍSWD   

	GPIOA->CRL&=0XFF0FFFFF;//PA5ÉèÖÃ³ÉÊäÈë	  
	GPIOA->CRL|=0X00800000;   				   
	GPIOA->ODR|=1<<5;	   //PA13ÉÏÀ­,PA0Ä¬ÈÏÏÂÀ­

	//Ex_NVIC_Config(GPIO_A,5,FTIR); //ÏÂ½µÑØ´¥·¢
	//Ex_NVIC_Config(GPIO_A,5,TirEdge);

	//MY_NVIC_Init(0,0,EXTI9_5_IRQChannel,2);    //ÇÀÕ¼2£¬×ÓÓÅÏÈ¼¶2£¬×é2
	MY_NVIC_Init(0,0,EXTI9_5_IRQn,2);    //ÇÀÕ¼2£¬×ÓÓÅÏÈ¼¶2£¬×é2

	Ex_NVIC_Config(GPIO_A,5,TirEdge);
}

void HpPreflash(unsigned int tmOv)
{
	delay_us(40);
}

void SetComSpiReady(void)
{
	SPI_MISO_AUX=1;
	EXTIx_Init( FTIR);
	SPIx_Init();
	g_comStatue = COM_STATUE_START;
}

void EXTI9_5_IRQHandler(void)
{
	//SPI_MISO_AUX = !SPI_MISO_AUX;
	//LED0=!LED0;

	if(EXTI_GetITStatus(EXTI_Line9) == SET)//                 £¯£¯ÅÐ¶ÏÊÇ·ñÊÇ£±£³½ÅµÄÖÐ¶Ï
       {
               EXTI_ClearITPendingBit(EXTI_Line9);//                £¯£¯Çå³ýÖÐ¶Ï±êÖ¾Î»£¡
               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            £¯£¯µãÁÁ£Ì£Å£Ä            
        }
	
	switch(g_comStatue)
	{
		case COM_STATUE_START:
		if (!CLK_IN)
		{
			SPI_MISO_AUX=0;
			//TEST_SIGNAL = !TEST_SIGNAL;
			//EXTI->PR=1<<5;
			Ex_NVIC_Disable(5);
			Timer2_Init(1000,72);
			g_tmHandleFlag = TM_HANDLE_SHUT;
			//Timer3_Start();
		}
		break;

		case COM_STATUE_IDLE:
		if (CLK_IN)
		{
			//FLASH_CONTROL = !FLASH_CONTROL;
			//PAout(6) = 1;			//2015/08/08
			SPI_MISO_AUX = 1;
			//OSTimeDlyHMSM(0,0,0,1);
			EXTIx_Init(FTIR);
			SPIx_Init();
			g_comStatue = COM_STATUE_START;
			//g_dspUpdateFlag|=DSP_UPDATE_LINK;
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				radioStatue.cmdReady = 1;
				radioStatue.cmdSta = RADIO_STATUE_RECOVER;
				OSTaskResume(CMDSEND_TASK_Prio);
			}
			ACT_SHUTTER = 0;		//camera actived, pull down the pin
			//WIRE_SHUTTER = 0;
			FLAOK_OFF();
			//CHARGE_LED_OFF;
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
			wlSet.m_cmdProc = WLM_CMDPROC_PRF;
			radioStatue.cmdReady = 1;
			OSTaskResume(CMDSEND_TASK_Prio);
			break;

			case CMD_MODE_RF_MST:
			if(CMR_ADEPT_NEW==radioStatue.adept)
			{
/*
				delay_us(100);
				SPIx_Init();
				//SPI1->DR = 0x80;
				SPI1->DR = 0xFF;
				g_comStatue = COM_STATUE_START;
				if(radioStatue.delay_mod>2)
				{
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_PRE;
					radioStatue.cmdSta = RADIO_STATUE_PRE_EX;
					radioStatue.adept = CMR_ADEPT_NEW;
					OSTaskResume(CMDSEND_TASK_Prio);
				}
*/
				radioStatue.cmdReady = 1;
				radioStatue.cmdSta = RADIO_STATUE_PRE_FLA;
				radioStatue.adept = CMR_ADEPT_NEW;
				OSTaskResume(CMDSEND_TASK_Prio);
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
#ifndef EOS_750D
			if(CMR_ADEPT_OLD==radioStatue.adept)	
			{
				delay_us(100);
				SPIx_Init();
				//SPI1->DR = 0x80;
				SPI1->DR = 0xFF;
				g_comStatue = COM_STATUE_START;
			}
#endif
			break;
		}
/***********************************************************/
//this is in order to competible with canon EOS750D
/***********************************************************/
#ifdef EOS_750D
		//EXTIx_Init(GPIO_A,RTIR);							//2015/09/03
		EXTIx_Init(RTIR);	
		g_comStatue = COM_STATUE_IDLE;
#endif
		break;

		case COM_STATUE_REALFLASH:		
		//if((SYNMODE_REAR==mainPara.synMode)&&(!BP_IN))
		if(!BP_IN)
		{
			SPIx_Disable();
			Ex_NVIC_Config(GPIO_A,5,RTIR);
			Ex_NVIC_Enable(5);
			g_comStatue = COM_STATUE_IDLE;
		}else //if(HP_FLASHMODE_ON==flaPara.hpFlash)//else if((SYNC_MODE_FP==mainPara.synMode)&&(HP_FLASHMODE_ON==flaPara.hpFlash))
		{
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);		
					if(HP_FLASHMODE_ON==flaPara.hpFlash)
					{
						radioStatue.cmdReady = 1;
						wlSet.m_cmdProc = WLM_CMDPROC_FLA;
						OSTaskResume(CMDSEND_TASK_Prio);
					}else
					{
						SPI_MISO_AUX = 0;
					}
				break;

				case CMD_MODE_RF_MST:
				//case CMD_MODE_RFLS_MST:
				//case CMD_MODE_RFLS_SLA:
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
							radioStatue.cmdReady = 1;
							radioStatue.cmdSta = RADIO_STATUE_FLAINFO;
							OSTaskResume(CMDSEND_TASK_Prio);
						}
					}
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					radioStatue.cmdReady = 1;
					radioStatue.cmdSta = RADIO_STATUE_FLAINFO;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case CMD_MODE_OFF:
					g_comStatue = COM_STATUE_REALFLASH;
				break;
			}
			//SPIx_Disable();
			//Ex_NVIC_Config(GPIO_A,5,RTIR);
			//Ex_NVIC_Enable(5);
			//g_comStatue = COM_STATUE_IDLE;
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
	//GPIOA->CRL&=0xF00FFFFF;
	//GPIOA->CRL|=0x07300000;
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

//SPI ËÙ¶ÈÉèÖÃº¯Êý
//SpeedSet:
//SPI_SPEED_2   2·ÖÆµ   (SPI 36M@sys 72M)
//SPI_SPEED_8   8·ÖÆµ   (SPI 9M@sys 72M)
//SPI_SPEED_16  16·ÖÆµ  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256·ÖÆµ (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//¶þ·ÖÆµ
	{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI_SPEED_8)//°Ë·ÖÆµ 
	{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI_SPEED_16)//Ê®Áù·ÖÆµ
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256·ÖÆµ
	{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz µÍËÙÄ£Ê½
	}
	SPI1->CR1|=1<<6; //SPIÉè±¸Ê¹ÄÜ	  
} 


//void SpiRearUp(void)
//#ifdef _UART_INT_INFO_
//void SpiRearUp(u16 tks,u8 evt)
//{
//	Timer2_Init(tks,72);
//	g_tmHandleFlag = evt;	
//}
//#else
void SpiRearUp(u16 tks)
{
	Timer2_Init(tks,72);
	g_tmHandleFlag = TM_HANDLE_REAR;	
	//g_spiRear = 1;
	//delay_us(30);
	//SET_SPIO_ON;
	//SPI_MISO_AUX=1;
	
}
//#endif

void SPI1_IRQHandler(void)
{
	unsigned char datRcv;
	//TEST_SIGNAL = !TEST_SIGNAL;
	//TEST_SIGNAL = !TEST_SIGNAL;
	//tmp = SPI1->SR;
	//g_spiRcvData[g_spiRcvIndex]=SPI1->DR;
	Timer2_Stop();
	SET_SPIO_OFF;
	PUSH_MISO;
	datRcv = SPI1->DR;
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
//#ifdef _UART_INT_INFO_
//						SpiRearUp(15,TM_HANDLE_REAR);
//#else
						SpiRearUp(15);
//#endif
		}
	}else
	{
		//get the last parameter
		GetParameter();			//extract infomation
		g_spiRcvData[g_spiRcvIndex++]=datRcv;
		CodeAnalysis(datRcv);	//load spi data
		SPI1->DR = g_spiSendData[--g_spiSendIndex]; 	//send a byte	 
		//SPI_MISO_AUX=1;	
		//SpiRearUp(100);
//#ifdef _UART_INT_INFO_
		//SpiRearUp(100,TM_HANDLE_REAR);
//#else
		//SpiRearUp(100);
//#endif
		
	}
	//SPI_MISO_AUX=1;	
	
	Ex_NVIC_Enable(5);
	//g_dspUpdateFlag|=DSP_UPDATE_LINK;
	SET_UPDATE_EVT(DSP_UPDATE_LINK);
	EXTI->PR=1<<5;
}

void CodeAnalysis(unsigned char rcvCode)
{	
	//GetParameter();
	//g_spiRcvIndex= 0 ;
	//switch(g_spiRcvData[0])
	switch(rcvCode)
	{
		case 0xff:
		//g_spiRcvData[g_spiRcvIndex++]=0xff;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
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
			SpiRearUp(100);
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
			}
*/
			switch(*g_cmdMode)
			{
				case CMD_MODE_RF_MST:
					g_spiSendData[g_spiSendIndex++]=0x03;
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++]=0x07;
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x01;
				break;

				
			}
			g_spiSendData[g_spiSendIndex++]=0x48;
			g_spiSendData[g_spiSendIndex++]=0x4d;
			SpiRearUp(100);
			break;
		case 0xa9:														//*ST-E3
			//g_spiRcvData[g_spiRcvIndex++]=0xa9;  
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			//g_spiSendData[g_spiSendIndex++]=0x2b;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			//g_spiSendData[g_spiSendIndex++]=0x97;
			SpiRearUp(100);
			break;
		case 0xa1:														//*ST-E3
			//g_spiSendData[g_spiSendIndex++]=0x4b;
			//g_spiSendData[g_spiSendIndex++]=0x4b;
			if((CMD_MODE_RF_MST==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x29;
				g_spiSendData[g_spiSendIndex++]=0x29;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x4B;
				g_spiSendData[g_spiSendIndex++]=0x4B;
			}
			SpiRearUp(100);
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
			SpiRearUp(100);
			break;
		case 0xb9:
			//g_spiRcvData[g_spiRcvIndex++]=0xb9;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//switch(flaPara.surrand_state)
			if((CMD_MODE_OFF==*g_cmdMode)||(CMD_MODE_LIGH_MST==*g_cmdMode)||(CMD_MODE_RF_MST==*g_cmdMode))
			{
				switch(flaPara.FEB_count)
				{
					//case 0:
					//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_COMP];//0x00;  //....this place index a surrand flash
					//break;
					//case 0:
					//case 3:	
					case 1:
					g_spiSendData[g_spiSendIndex++]= compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];//menuPara.mstTtl;//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
					break;

					case 2:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6-flaPara.ttl_FEB1)*3+mstPara->cmp2-flaPara.ttl_FEB2];
					break;

					case 3:
					g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6+flaPara.ttl_FEB1)*3+mstPara->cmp2+flaPara.ttl_FEB2];
					break;

					default:
						g_spiSendData[g_spiSendIndex++]= compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];//menuPara.mstTtl;//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
					break;
				}	
			}else
			{
					g_spiSendData[g_spiSendIndex++]=0x00;
			}
			SpiRearUp(100);
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
#endif
		
			}
			SpiRearUp(100);
		break;
			
		case 0xfb:
			//g_spiRcvData[g_spiRcvIndex++]=0xfb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2];
/*
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2];
			}else
			{
				g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2];
			}
*/
			g_spiSendData[g_spiSendIndex++]=0x8e;
			if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x02;
			}else
			{
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
			}
			SpiRearUp(100);
			break;
			
		case 0xf9:
			//g_spiRcvData[g_spiRcvIndex++]=0xf9;
			g_spiSendData[g_spiSendIndex++]=0x8e;
#ifdef _ST_EX_
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
					g_spiSendData[g_spiSendIndex++]=(g_chargeOk>AD_CHARGE_LOW)?0x55:0x54; 			//in the rf Master Mode, if there are SLAVEs linked, it send 0x55
				break;
				
				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++]=0x55;	
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x55;	
				break;
			}
#else
			g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC2]; 
#endif
			SpiRearUp(100);
			break;
		case 0xb2:
			//g_spiRcvData[g_spiRcvIndex++]=0xb2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
		break;
		case 0xb3:
			//g_spiRcvData[g_spiRcvIndex++]=0xb3;
			g_spiSendData[g_spiSendIndex++]=0x00;
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_RF_MST:
					if(WLM_MODE_ALL==*wlSet.arr_mode)
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
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x00;
				break;
/*
				case CMD_MODE_RF:
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x08;
					g_spiSendData[g_spiSendIndex++]=0x08;
				break;
*/			

				case CMD_MODE_OFF:		
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x08;
					g_spiSendData[g_spiSendIndex++]=0x08;
				break;
			}
			//SETBIT(paraBit,0);
			SpiRearUp(100);
			break;
		case 0xf5:
			//g_spiRcvData[g_spiRcvIndex++]=0xf5;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x0f;
			if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))		//20140515
			{
				g_spiSendData[g_spiSendIndex++]=0x4d;
			}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=(0x5d+*wlSet.arr_mode);
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				//g_spiSendData[g_spiSendIndex++]=(0x4D+*wlSet.arr_mode);//0x4d+*wlSet.arr_mode+(WORKINGMODE_GR==mainPara.workMode);
				g_spiSendData[g_spiSendIndex++]=(0x5d+*wlSet.arr_mode);
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x5c;
			}
			if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x49;// otherwise swith to RFLK error
			
			}else
			{
				if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
				{
					g_spiSendData[g_spiSendIndex++]=(0x49|((flaPara.surrand_state<<4)&0x30));
				}else
				{
					g_spiSendData[g_spiSendIndex++]=0x48;
				}
			}
			if(WORKINGMODE_MULTI!=mainPara.workMode)
			{
				g_spiSendData[g_spiSendIndex++]=0x53|((SYNMODE_HP==mainPara.synMode)<<2);//Parameter_Array[PARA_SYNC3];
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x53;
			}
			SpiRearUp(100);
			break;
		case 0xbb:
			//g_spiRcvData[g_spiRcvIndex++]=0xbb;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2];//menuPara.feb;//Parameter_Array[PARA_SRD];//0x00;			//feb
//#ifdef _ST_EX_
			if((CMD_MODE_LIGH_MST==*g_cmdMode)||(CMD_MODE_OFF==*g_cmdMode)||(CMD_MODE_RF_MST==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];//menuPara.mstTtl;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
			}

//#else
//			g_spiSendData[g_spiSendIndex++]=menuPara.otpCod[MASTER];//wlSet.otpCode[WL_ARRARY_A];//Parameter_Array[PARA_COMP];
//#endif
			//value = Parameter_Array[PARA_COMP];
			SpiRearUp(100);
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
				g_spiSendData[g_spiSendIndex++]=(((WORKINGMODE_TTL==radioSet->rfGr[4].mode)&&(radioStatue.devCnt[4]>0))<<5|
													((WORKINGMODE_TTL==radioSet->rfGr[3].mode)&&(radioStatue.devCnt[3]>0))<<4|
													((WORKINGMODE_TTL==radioSet->rfGr[2].mode)&&(radioStatue.devCnt[2]>0))<<3|
													((WORKINGMODE_TTL==radioSet->rfGr[1].mode)&&(radioStatue.devCnt[1]>0))<<2|
													((WORKINGMODE_TTL==radioSet->rfGr[0].mode)&&(radioStatue.devCnt[0]>0))<<1);
			}else
			{
				switch(*wlSet.arr_mode)
				{
					case WLM_MODE_ALL:
						g_spiSendData[g_spiSendIndex++]=((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0));//0x01;
						//g_spiSendData[g_spiSendIndex++]=0x01;
					break;

					case WLM_MODE_AB:
						g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2));//0x06;
						//g_spiSendData[g_spiSendIndex++]=0x06;
					break;

					case WLM_MODE_RES:
						g_spiSendData[g_spiSendIndex++]=(((radioStatue.devCnt[0]>0)<<1)|((radioStatue.devCnt[1]>0)<<2)|((radioStatue.devCnt[2]>0)<<3));//0x0E;
						//g_spiSendData[g_spiSendIndex++]=0x0E;
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
			SpiRearUp(100);
		break;

		case 0xd5:
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			SpiRearUp(100);
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
			SpiRearUp(100);
			//0x00;
		break;

		case 0x01:
			//g_spiRcvData[g_spiRcvIndex++]=0x01;
			g_spiSendData[g_spiSendIndex++]=0x96;
			SpiRearUp(100);
		break;
		
		case 0xe2:
			//g_spiRcvData[g_spiRcvIndex++]=0xe2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=menuPara.motoCod;//Parameter_Array[PARA_FOCIND];			//½¹¾àÄ£Ê½
			SpiRearUp(100);
			break;
		case 0xea:						//determine the Wireless command mode: RF,LIGHT,OFF
			//g_spiRcvData[g_spiRcvIndex++]=0xea;
			g_spiSendData[g_spiSendIndex++]=0x8e;							
			g_spiSendData[g_spiSendIndex++]=*wlSet.m_chan;
			g_spiSendData[g_spiSendIndex++]=0x03;//0x03;
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.arr_mode)<<4)|((*wlSet.is_masterOn)<<3));
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++]=0x01;
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.arr_mode)<<4));
				break;
			}/*
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=(0x02|((*wlSet.arr_mode)<<4)|((*wlSet.is_masterOn)<<3));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=(((*wlSet.is_masterOn)<<3)|((*wlSet.arr_mode)<<4));
				
			}*/
			SpiRearUp(100);
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
				switch(*wlSet.arr_mode)
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

				case CMD_MODE_OFF:
					g_spiSendData[g_spiSendIndex++]=0x8e;
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x06;
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=0x8e;
					g_spiSendData[g_spiSendIndex++]=0x00;
					g_spiSendData[g_spiSendIndex++]=0x06;
				break;
				
			}
			SpiRearUp(100);
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
			SpiRearUp(100);
		break;
		
		case 0xe6:
			//g_spiRcvData[g_spiRcvIndex++]=0xe6;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			/*
			if((CMD_MODE_OFF==*g_cmdMode)||(CMD_MODE_LIGH_MST==*g_cmdMode)||(CMD_MODE_RF_MST==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=menuPara.feb;//Parameter_Array[PARA_SRD];			//ÉÁ¹â°üÎ§
				//g_spiSendData[g_spiSendIndex++]=(VER_CAMERA_OLD==g_vCmr)?(WORKINGMODE_GR==mainPara.workMode)?():((mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
				if((WORKINGMODE_GR==mainPara.workMode)&&(VER_CAMERA_OLD==g_vCmr))
				{
					g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(WORKINGMODE_TTL&0x07));
				}else
				{
					g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(mainPara.workMode&0x07));
				}
			}else
			{
				g_spiSendData[g_spiSendIndex++] = 0x00;
				g_spiSendData[g_spiSendIndex++] = 0x00;
			}*/
			g_spiSendData[g_spiSendIndex++]=feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2];//menuPara.feb;//Parameter_Array[PARA_SRD];			//ÉÁ¹â°üÎ§
			//g_spiSendData[g_spiSendIndex++]=(VER_CAMERA_OLD==g_vCmr)?(WORKINGMODE_GR==mainPara.workMode)?():((mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
			if((WORKINGMODE_GR==mainPara.workMode)&&(VER_CAMERA_OLD==g_vCmr))
			{
				g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(WORKINGMODE_TTL&0x07));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=((mainPara.synMode<<3)|(mainPara.workMode&0x07));
			}
			//g_spiSendData[g_spiSendIndex++]=Parameter_Array[PARA_SYNC1];//mainPara.workMode;//Parameter_Array[PARA_SYNC1];//mainPara.workMode;//0x00;//Parameter_Array[PARA_SYNC1];			//modeÉÁ¹âÄ£Ê½
			//g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=0)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07)|(g_activeControl<<5));
//#ifdef _ST_EX_
//			g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=CMD_MODE_OFF)<<4)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
//#else
//			g_spiSendData[g_spiSendIndex++]=(((*g_cmdMode!=CMD_MODE_OFF)<<4)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));//(((*g_cmdMode!=CMD_MODE_OFF)<<6)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
//#endif
			if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				if(CMR_ADEPT_NEW==radioStatue.adept)
				{
					g_spiSendData[g_spiSendIndex++]=0x7F;
				}else if(CMR_ADEPT_OLD==radioStatue.adept)
				{
					g_spiSendData[g_spiSendIndex++]=0x07;
				}
				
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x7F;//0x3f;
			}
			SpiRearUp(100);
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
			SpiRearUp(100);
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
			SpiRearUp(100);
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
			SpiRearUp(100);
			break;
		case 0xb4:
			//g_spiRcvData[g_spiRcvIndex++]=0xb4;
//							g_spiSendData[g_spiSendIndex++]=0x8e;
//							g_spiSendData[g_spiSendIndex++]=0x91;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
			break;
		case 0xb0:
			//g_spiRcvData[g_spiRcvIndex++]=0xb0;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
//							pre_flash = 1;
			SpiRearUp(100);
			break;
		case 0xb1:
			//g_spiRcvData[g_spiRcvIndex++]=0xb1;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
			break;
		case 0xf8:
			//g_spiRcvData[g_spiRcvIndex++]=0xf8;
//			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xff;
			SpiRearUp(100);
			//g_spiSendData[g_spiSendIndex++]=0x5d;
			break;
		case 0xb7:
			//g_spiRcvData[g_spiRcvIndex++]=0xb7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
			break;
		case 0xb8:
			//g_spiRcvData[g_spiRcvIndex++]=0xb8;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x91;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
			break;
		case 0xa8:
			//g_spiRcvData[g_spiRcvIndex++]=0xa8;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			//g_spiSendData[g_spiSendIndex++]=0x97;//0x96;//0x91;
			g_spiSendData[g_spiSendIndex++] = 0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			SpiRearUp(100);
			break;

		case 0xC4:
			//g_spiRcvData[g_spiRcvIndex++]=0xC4;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			g_spiSendData[g_spiSendIndex++]=0x8e;	
			SpiRearUp(100);
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
				g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=wlSet.cCmp;	
				g_spiSendData[g_spiSendIndex++]=abRation_fill2;	
				g_spiSendData[g_spiSendIndex++]=abRation_fill1;//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;	
			}
			
#if 0
			//g_spiRcvData[g_spiRcvIndex++]=0xC2;
			if(!radioStatue.cmpFlag)
			{
				
				g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[4];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[3];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[2];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[1];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[0];//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;	
			/*	g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;	*/
			}else
			{
			/*	radioStatue.cmpFlag = 0;
				g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[4];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[3];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[2];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[1];	
				g_spiSendData[g_spiSendIndex++]=menuPara.rfGrTmp[0];//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;	*/
				radioStatue.cmpFlag = 0;
				g_spiSendData[g_spiSendIndex++]=0x8e;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;	
				g_spiSendData[g_spiSendIndex++]=0x00;//0x80;//0x00;	cause serious problem 
				g_spiSendData[g_spiSendIndex++]=0x8e;				
			}
#endif
		SpiRearUp(100);
		break;

		case 0xC8:
			//g_spiRcvData[g_spiRcvIndex++]=0xC8;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			g_spiSendData[g_spiSendIndex++]=0x96;
			SpiRearUp(100);
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
			SpiRearUp(100);
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
			SpiRearUp(100);
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
			SpiRearUp(100);
		break;

		case 0xD7:
			//g_spiRcvData[g_spiRcvIndex++]=0xD7;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
				g_spiSendData[g_spiSendIndex++]=0x00;
			}else
			{
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
			}
			g_spiSendData[g_spiSendIndex++]=0x38;
			if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				g_spiSendData[g_spiSendIndex++]=0x00;
			}else
			{
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
				}
			}
			SpiRearUp(100);
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
		SpiRearUp(100);
		break;
		
		case 0xa3:
			//g_spiRcvData[g_spiRcvIndex++]=0xa3;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
			break;
			
		case 0xf2:
			//g_spiRcvData[g_spiRcvIndex++]=0xf2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0xa0;//cameraSetting.prefla_com;
			SpiRearUp(100);
			break;

		case 0xF4:
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0xC0;
			SpiRearUp(100);
			break;
		case 0xfc:	 
			//g_spiRcvData[g_spiRcvIndex++]=0xfc;
			if(3==radioStatue.delay_mod)				//cbyi 7dII debugy 2015/05/16
			{
				g_spiSendData[g_spiSendIndex++]=0x8e;
				g_spiSendData[g_spiSendIndex++]=0xd9;
				g_spiSendData[g_spiSendIndex++]=0x7f;
				g_spiSendData[g_spiSendIndex++]=0x7c;
				g_spiSendData[g_spiSendIndex++]=0x17;
				g_spiSendData[g_spiSendIndex++]=0xc0;
				g_spiSendData[g_spiSendIndex++]=0x87;
			}else
			{
				g_spiSendData[g_spiSendIndex++]=0x8e;
				g_spiSendData[g_spiSendIndex++]=0x32;
				g_spiSendData[g_spiSendIndex++]=0x85;
				g_spiSendData[g_spiSendIndex++]=0x21;
				g_spiSendData[g_spiSendIndex++]=0x1a;
				g_spiSendData[g_spiSendIndex++]=0x8e;
				g_spiSendData[g_spiSendIndex++]=0x8e;
			}
			SpiRearUp(420);
			break;
		case 0xa2:
			//g_spiRcvData[g_spiRcvIndex++]=0xa2;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
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
			g_spiSendData[g_spiSendIndex++]=0x0C;
			g_spiSendData[g_spiSendIndex++]=0x1C;
			g_spiSendData[g_spiSendIndex++]=0x1C;
			g_spiSendData[g_spiSendIndex++]=0x0D;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x00;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0C;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x3F;
			g_spiSendData[g_spiSendIndex++]=0x0c;
			g_spiSendData[g_spiSendIndex++]=0x0c;
#endif
			SpiRearUp(100);
			break;	
		case 0xaa:
			//g_spiRcvData[g_spiRcvIndex++]=0xaa;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			SpiRearUp(100);
			break;	
		case 0xab:
			//g_spiRcvData[g_spiRcvIndex++]=0xab;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			g_spiSendData[g_spiSendIndex++]=0x96; 
			SpiRearUp(100);
		break;
		case 0xac:
			//g_spiRcvData[g_spiRcvIndex++]=0xac;
			g_spiSendData[g_spiSendIndex++]=0x8e; 						
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			g_spiSendData[g_spiSendIndex++]=0x91; 
			SpiRearUp(100);
		break;

		case 0xDA:											//just for st-e3
			//g_spiRcvData[g_spiRcvIndex++]=0xDA;
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->id&0xFF); 						
			g_spiSendData[g_spiSendIndex++]=(radioSet->id>>8); 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfChan;//wlSet.rfSet.chan;
			g_spiSendData[g_spiSendIndex++]=0x1F;
			switch(*g_cmdMode)
			{
				case CMD_MODE_RF_MST:
					g_spiSendData[g_spiSendIndex++]=(0x04|(*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_spiSendData[g_spiSendIndex++] = 0x0E;
				break;

				default:
					g_spiSendData[g_spiSendIndex++]=((*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
				break;
			}
			/*
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				g_spiSendData[g_spiSendIndex++]=(0x04|(*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
			}else
			{
				g_spiSendData[g_spiSendIndex++]=((*wlSet.arr_mode<<5)|(*wlSet.is_masterOn<<4));
			}*/
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
		SpiRearUp(100);
		break;

		case 0xC3:
			g_spiSendData[g_spiSendIndex++]=0x8E;
			g_spiSendData[g_spiSendIndex++]=0x8E;
			SpiRearUp(100);
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
			g_spiSendData[g_spiSendIndex++]=radioSet->rfGr[4].mode;//rfGrPara.rfGr_arrPara[4].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[3].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[3]:menuPara.rfGrTmp[3];//flaPara.rfCmuPara[3].optCode;//rfGrPara.rfGr_arrPara[3].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfGr[3].mode;//rfGrPara.rfGr_arrPara[3].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[2].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[2]:menuPara.rfGrTmp[2];//flaPara.rfCmuPara[2].optCode;//rfGrPara.rfGr_arrPara[2].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfGr[2].mode;//rfGrPara.rfGr_arrPara[2].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[1].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[1]:menuPara.rfGrTmp[1];//flaPara.rfCmuPara[1].optCode;//rfGrPara.rfGr_arrPara[1].optCode;
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfGr[1].mode;//rfGrPara.rfGr_arrPara[1].mode;//0x00; 
			g_spiSendData[g_spiSendIndex++]=(radioSet->rfGr[0].mode==WORKINGMODE_MANU)?menuPara.rfGrOtp[0]:menuPara.rfGrTmp[0];//flaPara.rfCmuPara[0].optCode;//rfGrPara.rfGr_arrPara[0].optCode; 
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			g_spiSendData[g_spiSendIndex++]=radioSet->rfGr[0].mode;//rfGrPara.rfGr_arrPara[0].mode;//0x01; 
			g_spiSendData[g_spiSendIndex++]=0x0F; 
			SpiRearUp(100);
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
			SpiRearUp(100);
		break;
		
		case 0xbe:
			//g_spiRcvData[g_spiRcvIndex++]=0xbe;
		    	g_spiSendData[g_spiSendIndex++]=0x8e; 
			g_spiSendData[g_spiSendIndex++]=0x8e; 
			SpiRearUp(100);
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
			SpiRearUp(100);
			break;	
			
		default:
			//SPIx_Disable();
			g_spiSendData[g_spiSendIndex++]=0x8e;
			SpiRearUp(100);
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
			case 0x64:		//700D
			return 0;
			//break;

			case 0x55:		//5D MARKIII
			return 1;

			case 0x45:		//7D MARKII
			case 0x43:
			return 3;

			case 0x3A:		//1DX
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
			case 0x54:		//700d 1DX
			return 0;

			case 0x65:		//5D MARKIII
			case 0x67:		
			case 0x57:		//6D
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
//	unsigned int tmp;
	switch(g_spiRcvData[0])
	{
		case 0xb4:
		switch(g_spiRcvData[1])
		{
			case 0x23: 			//be ready for previous flash
			//NrfWarnStart(g_nrfBuf,NRF_HEAD_CFM_PRE);
			//SC4MD1 &= 0xCF;		//suspend clock syn transfer
			SPIx_Disable();
			//FLASH_OUT = 0;
			SPI_MISO_AUX = 0;
			g_comStatue= COM_STATUE_PREFLASH;
			g_mstPrefla = 1;
			/*the following process is about to set interrupt0 to hold mode*/
#ifdef WIRELESS_MASTER_ON
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				//if(WLM_MODE_ALL==*wlSet.ttl_Ratio)
				if(WLM_MODE_ALL==*wlSet.arr_mode)
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
				//ENABLE_D30EN();
				break;

				case CMD_MODE_RF_MST:
				flaPara.preflashIndex =0;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
				OSTaskResume(CMDSEND_TASK_Prio);
				break;

				default:
					/*
					radioStatue.cmdSta = RADIO_STATUE_PRE;
					radioStatue.adept = CMR_ADEPT_OLD;
					radioStatue.cmdReady = 1;
					OSTaskResume(CMDSEND_TASK_Prio);
					*/
					SpiRearUp(100);
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
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				//flaPara.preflashIndex =1;
				flaPara.preflashIndex =0;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
				OSTaskResume(CMDSEND_TASK_Prio);
			}else
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
				//WLMCmdSend();
				OSTaskResume(CMDSEND_TASK_Prio);
				//WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				flaPara.preflashIndex=1;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.adept = CMR_ADEPT_OLD;
				radioStatue.cmdReady = 1;
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
				OSTaskResume(CMDSEND_TASK_Prio);
				//WLMCmdSend();
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				flaPara.preflashIndex=2;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.cmdReady = 1;
				radioStatue.adept = CMR_ADEPT_OLD;
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
				OSTaskResume(CMDSEND_TASK_Prio);
				//WLMCmdSend();
			}else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				flaPara.preflashIndex=3;
				radioStatue.cmdSta = RADIO_STATUE_PRE;
				radioStatue.cmdReady = 1;
				radioStatue.adept = CMR_ADEPT_OLD;
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
				//if(WLM_ON==*wlSet.m_on)
				//if(CMD_MODE_OFF!=*g_cmdMode)
				switch(*g_cmdMode)
				{
					case CMD_MODE_LIGH_MST:
						//BuildExposeCmd(mainPara.workMode,flaPara.hpFlash);
						//if((AD_CHARGE_FUL==charge_ok)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT)))
						BuildExposeCmd(mainPara.workMode,flaPara.hpFlash,(g_hpLev>0x46));
						wlSet.m_cmdProc = WLM_CMDPROC_DAT;//WLM_CMDPROC_ON;
						wlSet.m_flaConfirm = WLM_CMDFLASH_CONFIRM;
						OSTaskResume(CMDSEND_TASK_Prio);
						//WLMCmdSend();
					break;

					case CMD_MODE_RF_MST:
						radioStatue.cmdSta = RADIO_STATUE_FLACFM;
						radioStatue.cmdReady = 1;
						radioStatue.adept = CMR_ADEPT_OLD;
						OSTaskResume(CMDSEND_TASK_Prio);
					break;

					case CMD_MODE_RFLS_MST:
					case CMD_MODE_RFLS_SLA:
						SpiRearUp(100);
					break;

					default:
						SpiRearUp(100);
					break;
				}
/*
				if(CMD_MODE_LIGH_MST==*g_cmdMode)
				{
					//BuildExposeCmd(mainPara.workMode,flaPara.hpFlash);
					//if((AD_CHARGE_FUL==charge_ok)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT)))
					BuildExposeCmd(mainPara.workMode,flaPara.hpFlash,(g_hpLev>0x46));
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
					//SpiRearUp(100);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(100,TM_HANDLE_REAR);
//#else
						SpiRearUp(100);
//#endif
				}*/
#endif
			break;
#endif
			case 0x3d:
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
				//SpiRearUp(100);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(100,TM_HANDLE_REAR);
//#else
						SpiRearUp(100);
//#endif
			}
#endif
			//FLASH_OUT = 1;
			break;
 
			case 0x1d:
//			TESTFLAG = ~TESTFLAG;
			//SETBIT(paraBit,BIT_paraBit_COMPENSATION_SET);
//			EX0 = 1;
				//SPI_MISO_AUX=1;	
				//SpiRearUp(60);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(60,TM_HANDLE_REAR);
//#else
						SpiRearUp(60);
//#endif
			break;

			default:
				//SPI_MISO_AUX=1;	
				//SpiRearUp(60);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(60,TM_HANDLE_REAR);
//#else
						SpiRearUp(60);
//#endif
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
					g_cmrCode = g_spiRcvData[1];
					flaPara.preflashIndex = (g_spiRcvData[2]>>4)&0x0F;
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
					//SpiRearUp(100);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(100,TM_HANDLE_REAR);
//#else
						SpiRearUp(100);
//#endif					
				break;
				
			}
#if 0
			//switch(MAKEWORD(g_spiRcvData[1],g_spiRcvData[2]))
			//SPIx_Disable();
			tmp = MAKEWORD(g_spiRcvData[2],g_spiRcvData[1]);
			//rfGrPara.preflashIndex = g_spiRcvData[2]&0xF0;				
			switch(tmp)
			{
				case 0x6409:					//ratio off mode preflash info
				case 0x6419:					//group A preflash info
				case 0x6429:					//group B preflash info
				case 0x6439:					//group C preflash info
				case 0x6449:					//group D preflash info
				case 0x6459:					//group E preflash info
					flaPara.preflashIndex = (g_spiRcvData[2]>>4)&0x0F;
					//wlSet.rfSet.cmdReady = 1;
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_PRE;
					radioStatue.cmdSta = RADIO_STATUE_PRE;
					radioStatue.adept = CMR_ADEPT_NEW;
					g_comStatue= COM_STATUE_PREFLASH;
					//radioStatue.cmpFlag = 1;				//SN debug delete
					radioStatue.delay_mod = 0;			// different  in 5dmarkIII 
					SPIx_Disable();
					FLASH_OUT = 0;	
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x5509:					//observed in 5d-mark III
				case 0x5519:					//observed in 5d-mark III
				case 0x5529:					//observed in 5d-mark III
				case 0x5539:					//observed in 5d-mark III
				case 0x5549:					//observed in 5d-mark III
				case 0x5559:					//observed in 5d-mark III
					flaPara.preflashIndex = (g_spiRcvData[2]>>4)&0x0F;
					//wlSet.rfSet.cmdReady = 1;
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_PRE;
					radioStatue.cmdSta = RADIO_STATUE_PRE;
					radioStatue.adept = CMR_ADEPT_NEW;
					g_comStatue= COM_STATUE_PREFLASH;
					//radioStatue.cmpFlag = 1;			//SN debug delete
					radioStatue.delay_mod = 1;
					SPIx_Disable();
					FLASH_OUT = 0;	
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x540E:
				case 0x540A:
				//case 0x650E:				//observed in 5d-mark III
				//case 0x650A:				//observed in 5d-mark III
					//wlSet.rfSet.cmdReady = 1;
					if(0x0A==g_spiRcvData[2])
					{
						flaPara.hpFlash = HP_FLASHMODE_ON;
					}else
					{
						flaPara.hpFlash = HP_FLASHMODE_OFF;
					}
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.delay_mod = 0;
					radioStatue.adept = CMR_ADEPT_NEW;
					g_comStatue = COM_STATUE_REALFLASH;
					SPIx_Disable();
					FLASH_OUT = 0; 
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x650E:				//observed in 5d-mark III
				case 0x650A:				//observed in 5d-mark III
				case 0x670E:
				case 0x670A:
				case 0x570E:				//6d
				case 0x570A:
				if(0x0A==g_spiRcvData[2])
					{
						flaPara.hpFlash = HP_FLASHMODE_ON;
					}else
					{
						flaPara.hpFlash = HP_FLASHMODE_OFF;
					}
					radioStatue.cmdReady = 1;
					//wlSet.rfSet.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.cmdSta = RADIO_STATUE_FLACFM;
					radioStatue.adept = CMR_ADEPT_NEW;
					radioStatue.delay_mod = 1;
					g_comStatue = COM_STATUE_REALFLASH;
					SPIx_Disable();
					FLASH_OUT = 0; 
					SPI_MISO_AUX = 0;
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x6401:
					//radioStatue.cmpFlag = 1;
					SpiRearUp(100);
				break;

				case 0x550F:
					g_dspUpdateFlag |= DSP_SLACMD_MODEL;
					//radioStatue.cmdReady = 1;
					//radioStatue.cmdSta = RADIO_STATUE_MODEL;
					g_comStatue = COM_STATUE_MODEL;
					SPIx_Disable();
					OSTaskResume(CMDSEND_TASK_Prio);
				break;

				case 0x0A06:
					SpiRearUp(100);
				break;
				

				//case 0x540A:
				//break;

				default:
					//SPI_MISO_AUX = 1;
#ifdef _1DX_DEBUG_
					g_dspUpdateFlag |= DSP_UPDATE_DEBUG;
					g_UnknowData[g_UkdCnt++] = tmp;
#endif
					SpiRearUp(100);
				break;

			}
			//g_spiRcvData[1] = 0;
			//g_spiRcvData[2] = 0;
#endif
		break;

		case 0xA5:				//this is in order to compliment with 700d
		//in the case of 600ex&700d, there if no (0xA5 0x6D 0x01) at RF mode, at present at light cmd mode
		if((g_spiRcvData[1]&0xf0)>=0x60)				//
		{
			//g_spiSendData[g_spiSendIndex++] = 0x97;
			g_spiSendData[g_spiSendIndex++] = 0x96+(*g_cmdMode==CMD_MODE_RF_MST);
			if((g_spiRcvData[1]&0x0f)==0x0d)   	//this determined the flash code 0xb4 1d b4 3d
			{
				Parameter_Array[PARA_A5] = 0x01;
			}else{ 
				Parameter_Array[PARA_A5] = 0x00;
			}
			g_vCmr = VER_CAMERA_NEW;
		}else
		{
			g_vCmr = VER_CAMERA_OLD;
			//if(WORKINGMODE_GR==mainPara.workMode)
			//{
				//mainPara.workMode = WORKINGMODE_TTL;
				//SET_UPDATE_EVT(DSP_UPDATE_MODE);				
			//}
		}
		//SPI_MISO_AUX=1;	
		//SpiRearUp(20);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(20,TM_HANDLE_REAR);
//#else
						SpiRearUp(20);
//#endif
		break;

		case 0xFC:
			SpiRearUp(95);
		break;

		default:
		//SPI_MISO_AUX=1;	
		//SpiRearUp(20);
//#ifdef _UART_INT_INFO_
//		SpiRearUp(20,TM_HANDLE_REAR);
//#else
		SpiRearUp(20);
//#endif
		break;

	}
}


void GetParameter(void)
{
//	int paraIndex=0;
	unsigned char search_index;
	unsigned char tmp;
	//static unsigned char modCodeRsv;
	static unsigned char motoRsv;

	switch(g_spiRcvData[0])
	{
		case 0xa6:
			//Parameter_Array[PARA_SRD]=g_spiRcvData[4];			//get flash surranding parameter
			//if(menuPara.feb!=g_spiRcvData[4])
			if (feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2] != g_spiRcvData[4])
			{
				//menuPara.feb = g_spiRcvData[4];
				//flaPara.ttl_FEB1 = H_COV(menuPara.feb+1);
				//flaPara.ttl_FEB2 = L_COV(menuPara.feb+1);
				flaPara.ttl_FEB1 = H_COV(g_spiRcvData[4]+1);
				flaPara.ttl_FEB2 = L_COV(g_spiRcvData[4]+1);
				flaPara.FEB_count = 1;
				flaPara.febEn = TTL_FEB_EN;
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}

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
			}
			//modCodeRsv = g_spiRcvData[3];
		break;

		case 0xa2:									 //focus lenghth
		//case 0xC8:
#ifndef CANON_NOMOTO_VERSION
			if(motoRsv!=g_spiRcvData[3])
			{
				motoRsv = g_spiRcvData[3];
				g_dspUpdateFlag |= DSP_UPDATE_MOTO;
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
					*motoSet.dest_location = 0x18;//main_parameter.moto_location= 0x18;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x11;
					menuPara.motoCod = 0x11;
					//Parameter_Array[PARA_FOCLEN]=0x18;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x12:				 //0x1c
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					*motoSet.dest_location = 0x1C;//main_parameter.moto_location= 0x1c;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x12;
					menuPara.motoCod = 0x12;
					//Parameter_Array[PARA_FOCLEN]=0x1c;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x13:				//0x23
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					*motoSet.dest_location = 0x23;//main_parameter.moto_location= 0x23;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x13;
					menuPara.motoCod = 0x13;
					//Parameter_Array[PARA_FOCLEN]=0x23;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x14:				//0x32
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					*motoSet.dest_location = 0x32;//main_parameter.moto_location= 0x32;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x14;
					menuPara.motoCod = 0x14;
					//Parameter_Array[PARA_FOCLEN]=0x32;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x15:				//0x46
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					*motoSet.dest_location = 0x46;//main_parameter.moto_location= 0x46;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x15;
					menuPara.motoCod = 0x15;
					//Parameter_Array[PARA_FOCLEN]=0x46;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x16:			   //0x50
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					*motoSet.dest_location = 0x50;//main_parameter.moto_location= 0x50;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x16;
					menuPara.motoCod = 0x16;
					//Parameter_Array[PARA_FOCLEN]=0x50;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					case 0x17:			   //0x69
	//				Parameter_Array[PARA_SYNC2]=0x7d;
					*motoSet.dest_location = 0x69;//main_parameter.moto_location= 0x69;
					//main_parameter.moto_mode = MOTO_MODE_MANU;
					*motoSet.mode = MOTO_MODE_MANU;
					//Parameter_Array[PARA_FOCIND]=0x17;
					menuPara.motoCod = 0x17;
					//Parameter_Array[PARA_FOCLEN]=0x69;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

					default:
					*motoSet.dest_location = 0x50;//main_parameter.moto_location= 0x50;
					*motoSet.mode = MOTO_MODE_MANU;
					//main_parameter.moto_mode = MOTO_MODE_AUTO;
					//Parameter_Array[PARA_FOCIND]=0x01;
					menuPara.motoCod = 0x01;
					//Parameter_Array[PARA_FOCLEN]=0x32;
					//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
					break;

				}
			}
#endif
		break;

		case 0xC8:
		switch(g_spiRcvData[3])
		{
			case 0x35:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 20;
			break;

			case 0x05:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 24;
			break;

			case 0x09:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 28;
			break;

			case 0x0D:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 35;
			break;

			case 0x11:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 50;
			break;

			case 0x15:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 70;
			break;

			case 0x19:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 80;
			break;

			case 0x1D:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 105;
			break;

			case 0x39:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 135;
			break;

			case 0x45:
				*motoSet.mode = MOTO_MODE_MANU;
				*motoSet.dest_location = 200;
			break;

			case 0x44:
				*motoSet.mode = MOTO_MODE_AUTO;
				//*motoSet.dest_location = 20;
			break;
		}
		motoSet.focLen = g_spiRcvData[3];
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
				//SETBIT(bitTerminate,BIT_bitTerminate_A5FLAG);
				break;

				case 0x0c:
				Parameter_Array[PARA_A5]=0x00;
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
					g_SettingIndex = 0;
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
						g_SettingIndex = 0;
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
			if(*wlSet.arr_mode != ((g_spiRcvData[3]>>2)&0x03))
			{
				*wlSet.arr_mode = ((g_spiRcvData[3]>>2)&0x03);
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
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
#ifdef _RF_ONLY_
#else
				if(WORKINGMODE_GR == mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_TTL;
					//menuPara.mode = 0x00;
					//Parameter_Array[PARA_SYNC1]=0x00;					
				}

				if(CMD_MODE_LIGH_MST!=*g_cmdMode)
				{
					//*wlSet.m_on = WLM_ON;
					*g_cmdMode = CMD_MODE_LIGH_MST;
					g_SettingIndex = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					SET_UPDATE_EVT(DSP_UPDATE_RFSW);
					DISABLE_RF_MODULE();
				}
				//main_parameter.SyncMode = SYNMODE_HEAD;
				if(SYNMODE_REAR==mainPara.synMode)
				{
					mainPara.synMode = SYNMODE_HEAD;
					//Parameter_Array[PARA_SYNC1]=0x00;
					//menuPara.mode = 0x00;
					Parameter_Array[PARA_SYNC2]=0x5d;
					Parameter_Array[PARA_SYNC3]=0x53;
				}
#endif
				
			}else
			{
				
				if(WORKINGMODE_GR==mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_TTL;
					//menuPara.mode = 0x00;
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
					g_SettingIndex = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					SET_UPDATE_EVT(DSP_UPDATE_RFSW);
					ENABLE_RF_MODULE();
				}
#else
				if(CMD_MODE_OFF!=*g_cmdMode)
				{
					//*wlSet.m_on = WLM_OFF;
					*g_cmdMode = CMD_MODE_OFF;
					//menuPara.mode &= 0xDF;
					g_SettingIndex = 0;
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
					g_SettingIndex = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					//SETBIT(Parameter_Array[PARA_WIRELESS],3);
					//SETBIT(bitWLMaster,BIT_bitWLM_Mfla);
				}
				
			}else
			{
				if(*wlSet.is_masterOn!=WLM_MASTER_OFF)
				{
					*wlSet.is_masterOn = WLM_MASTER_OFF;
					g_SettingIndex = 0;
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
				g_SettingIndex = 0;
				 SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SETBIT(bitWireless,BIT_bitMaster_Switch);
			}
			
		break;
#endif

		case 0xab:
			switch(*wlSet.arr_mode)
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
						g_SettingIndex = 0;
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
						g_SettingIndex = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(bitWLMaster,BIT_bitWLM_abRatio);
					}
					if(wlSet.cCmp != g_spiRcvData[4])
					{
						wlSet.cCmp = g_spiRcvData[4];
						wlSet.m_arrPara[WL_ARRARY_C].cmp1 = CMPH_COV(wlSet.cCmp);
						wlSet.m_arrPara[WL_ARRARY_C].cmp2 = CMPL_COV(wlSet.cCmp);
						g_SettingIndex = 0;
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
					wlSet.m_arrPara[MASTER+search_index].multi_exp = EXPH_COV(menuPara.WlmMul[MASTER+search_index])<<4;
					//wlSet.m_arrPara[MASTER+search_index].multi_exp = EXPH_COV(menuPara.WlmMul[MASTER+search_index]);
					g_SettingIndex = 0;
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
					g_SettingIndex = 0;
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
			if((WORKINGMODE_GR==mainPara.workMode)&&(CMR_ADEPT_OLD==radioStatue.adept))
			{
				flaPara.rfCmuPara[0].ttlExpVal=g_spiRcvData[1];
				flaPara.rfCmuPara[1].ttlExpVal=g_spiRcvData[2];
				flaPara.rfCmuPara[2].ttlExpVal=g_spiRcvData[3];
			}
		break;
		

		case 0xbd:
		//main_parameter.focLen = g_spiRcvData[2];
		//if((main_parameter.moto_mode == MOTO_MODE_AUTO)&&(menuPara.motoLoc!=g_spiRcvData[2]))
		if(menuPara.motoLoc!=g_spiRcvData[2])
		{
			//main_parameter.moto_location= main_parameter.focLen;
			menuPara.motoLoc=g_spiRcvData[2];
			//SETBIT(bitTerminate,BIT_bitTerminata_FOCCHANGE);
		}
		break;

		case 0xb7:
		if(g_spiRcvData[1]>0)
		{
			g_cmrPressed = 1;
		}else
		{
			g_cmrPressed = 0;
		}
		if(flaPara.apert!=g_spiRcvData[1])
		{
			flaPara.apert = g_spiRcvData[1];
			//SETBIT(bitTerminate,BIT_bitTerminate_APERT);
		}
		break;

		case 0xA8:							//TTL compensation
		break;


		case 0xb8:
#ifdef _MULTIPLY_FLASH_
		if(multiSet.sCode!=g_spiRcvData[1])
		{
			multiSet.sCode = g_spiRcvData[1];
			//g_dspUpdateFlag |= DSP_UPDATE_MULCAL;
			//SETBIT(bitExtra,BIT_bitExtra_Shutter);
		}
#endif
#ifdef CANON_RF
		if(flaPara.shutter!=g_spiRcvData[1])
		{
			flaPara.shutter = g_spiRcvData[1];
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
				 
		 }
		break;	 

		case 0xC2:
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
		}
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
		//if(*g_cmdMode != CMD_MODE_RF_MST)
		if((*g_cmdMode!=CMD_MODE_RF_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_MST)&&(*g_cmdMode!=CMD_MODE_RFLS_SLA))
		{
			*g_cmdMode = CMD_MODE_RF_MST;
			g_SettingIndex = 0;
			//SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//SET_UPDATE_EVT(DSP_UPDATE_CMDMOD);
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			SET_UPDATE_EVT(DSP_UPDATE_RFSW);
		}
		if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			tmp = ((g_spiRcvData[3]>>2)&0x03);
			//if(wlSet.rfSet.ratio!=tmp)
			if(*wlSet.arr_mode!=tmp)
			{
				//radioSet.ratio = tmp;
				*wlSet.arr_mode=tmp;
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			//*wlSet.is_masterOn = ((g_spiRcvData[3]>>1)&0x01);
#ifdef WLM_MASTER_FLA
			tmp = ((g_spiRcvData[3]>>1)&0x01);
			if(tmp!=*wlSet.is_masterOn)
			{
				*wlSet.is_masterOn = tmp;
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
#endif
		}
		if(radioSet->rfChan!=g_spiRcvData[4])
		{
			radioSet->rfChan = g_spiRcvData[4];
			//SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//SET_UPDATE_EVT(DSP_UPDATE_CMDMOD);
			g_SettingIndex = 0;
			memset(radioStatue.devCnt,0x00,5);
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			SET_UPDATE_EVT(DSP_UPDATE_RFCHAN);
			//LoadUartSendBuf(UART_ENCODE_RFCH,radioSet->rfChan,0);
			
		}
		if(radioSet->id!= MAKEWORD(g_spiRcvData[6],g_spiRcvData[5]))
		{
			radioSet->id = MAKEWORD(g_spiRcvData[6],g_spiRcvData[5]);
			g_SettingIndex = 0;
			memset(radioStatue.devCnt,0x00,5);
			//SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//SET_UPDATE_EVT(DSP_UPDATE_CMDMOD);
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
				if(radioSet->rfGr[search_index].mode!=g_spiRcvData[4+search_index])
				{
#ifdef _SN_DEBUG_
					if(WORKINGMODE_EXTA==g_spiRcvData[4+search_index])
					{
						continue;
					}else
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
						g_SettingIndex = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
					}
#else
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
						g_SettingIndex = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
#endif
					//SETBIT(flaPara.rfGrInfoFlag[search_index],RF_INFOBIT_MODE);
					//here should send info
				}
			}
		break;

		case 0xCC:														//GR output
			for(search_index=0;search_index<5;search_index++)
			{
				//if(radioPara.optCode[search_index]!= g_spiRcvData[4+search_index])
				switch(radioSet->rfGr[search_index].mode)
				{
					case WORKINGMODE_MANU:
					if(menuPara.rfGrOtp[search_index]!=g_spiRcvData[4+search_index])
					{
						//radioPara.optCode[search_index] = g_spiRcvData[4+search_index];
						menuPara.rfGrOtp[search_index]=g_spiRcvData[4+search_index];
						radioSet->rfGr[search_index].m_exp1 = EXPH_COV(menuPara.rfGrOtp[search_index]&0x7F);
						radioSet->rfGr[search_index].m_exp2 = EXPL_COV(menuPara.rfGrOtp[search_index]&0x7F);
						g_grDspStart = (search_index>2)? 2:0;
						g_SettingIndex = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//SETBIT(flaPara.rfGrInfoFlag[search_index],RF_INFOBIT_OUTPUT);
					}
					break;

					case WORKINGMODE_TTL:
					case WORKINGMODE_EXTA:
					if(menuPara.rfGrTmp[search_index]!=g_spiRcvData[4+search_index])
					{
						menuPara.rfGrTmp[search_index] = g_spiRcvData[4+search_index];
						radioSet->rfGr[search_index].ttl_cmp1 = CMPH_COV(menuPara.rfGrTmp[search_index]);
						radioSet->rfGr[search_index].ttl_cmp2 = CMPL_COV(menuPara.rfGrTmp[search_index]);
						g_grDspStart = (search_index>2)? 2:0;
						g_SettingIndex = 0;
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
				wlSet.m_arrPara[WL_ARRARY_A].multi_exp = EXPH_COV(menuPara.WlmMul[WL_ARRARY_A])<<4;
				*multiSet.frequence = MAKEWORD(g_spiRcvData[5], g_spiRcvData[4]);//g_spiRcvData[5];
				*multiSet.eTimes = g_spiRcvData[6];
				g_SettingIndex = 0;
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
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			//SETBIT(paraBit,BIT_paraBit_OUTPUT_CHANGE);
		}
		
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
	}else
	{
		mulFrqCov = (*multiSet.frequence+1-50)/10+25;
	}

	return mulFrqCov;
}

unsigned char MulTimeRev(void)
{
	unsigned char tmp0;
	unsigned long tmp3;
	u8 search_index;

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
	
	if(!wlSet.muitl_realTimes)
	{
		tmp3 = (((unsigned long)flaPara.quickTime*(unsigned long)(*multiSet.frequence))/1000);

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
	}else
	{
		tmp0 = ((*multiSet.eTimes)-50)/10+26;
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
		switch(*wlSet.arr_mode)
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
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
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
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					wlSet.m_flaCmd[2]|= (tmp<<3);		//5bits
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4)*3];
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
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_A].multi_exp>>4)*3];
					wlSet.m_flaCmd[2]|= (tmp<<3);		//5bits
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4)*3];
					wlSet.m_flaCmd[3]  = tmp<<3;
					tmp = wl_opArr[(wlSet.m_arrPara[WL_ARRARY_C].multi_exp>>4)*3];
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

void FoclenRefresh()
{
	u8 i;
	if(motoSet.destRsv!=*motoSet.dest_location)
	{
		*motoSet.dest_location = motoSet.destRsv;
		for(i=0;i<sizeof(focLenScape);i++)
		{ 
			if((focLenScape[i]<*motoSet.dest_location)&&(*motoSet.dest_location<=focLenScape[i+1]))
			{
				motoSet.focIndex = i+1;
				break;
			}
		}
	}
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

void OneTimeFlashHandle()
{
	INT8U tmrErr;
	if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_GR==mainPara.workMode))
	{
		if(TTL_FEB_EN==flaPara.febEn)
		{
			flaPara.FEB_count++;
			if(flaPara.FEB_count>3)
			{
				flaPara.ttl_FEB1=flaPara.ttl_FEB2=0;
				flaPara.febEn=TTL_FEB_DEN;
				flaPara.FEB_count=0;
			}
			/*
			if(WORKINGMODE_TTL==mainPara.workMode)
			{
				SetTtlScr();
			}else if(WORKINGMODE_GR==mainPara.workMode)
			{
				SetRfGrScr();
				//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_NORMAL);
			}*/
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
	}

	if((WORKINGMODE_GR==mainPara.workMode)&&(VER_CAMERA_OLD==g_vCmr))
	{
		mainPara.workMode = WORKINGMODE_TTL;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}

	//BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
	gWSleepCounter = 0;

	FLAOK_ON();
	OSTmrStart(g_1ShotTmr, &tmrErr);	
}

void TaskDspUpdate(void *pdata)
{
//	int i;
//	unsigned char search_index;
#ifdef _1DX_DEBUG_
	u8 i=0;
#endif
	INT8U tmrErr;
	while(1)
	{
#if 0
		if(g_dspUpdateFlag&DSP_UPDATE_MODE)
		{
			g_SettingIndex=0;
			Mode(mainPara.workMode);
			LoadButtons();
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);
			}
			else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				SetMstChan(radioSet->rfChan,DSP_NORMAL);
			}
			g_dspUpdateFlag ^= DSP_UPDATE_MODE;
		}
#else
		if(g_dspUpdateFlag&DSP_UPDATE_MODE)
		{
			//if(!g_setFlag)
			//{
				//CmdModDisplay();
				//Mode();
				RePaintScreen();
				g_dspUpdateFlag ^= DSP_UPDATE_MODE;
			//}
		}
#endif
#ifdef _SPEED_LIGHT_
		if(g_dspUpdateFlag&DSP_UPDATE_MOTO)
		{
			FoclenRefresh();
			g_dspUpdateFlag^=DSP_UPDATE_MOTO;
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
			LoadUartSendBuf(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
			TrigerUartSend(0);
			g_dspUpdateFlag^=DSP_UPDATE_RFID;
		}

		if(g_dspUpdateFlag&DSP_UPDATE_RFSW)
		{
			while(uart_tran_flag);
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
			g_dspUpdateFlag^=DSP_UPDATE_RFSW;
		}
		
#if 1
		if(g_dspUpdateFlag&DSP_UPDATE_LINK)
		{
			if(COM_STATUE_START==g_comStatue)
			{
#ifdef LINK_BKLIGHT_ON
				BkLiteOn();
#endif
				gWSleepCounter = 0;
				//if(g_SettingIndex>0)
				//{
				//	ExitEditMode();
				//}
				if(!g_cmrLinked)
				{
					if(g_SettingIndex>0)
					{
						ExitEditMode();
					}
					g_cmrLinked = 1;
				}
			}else if(COM_STATUE_IDLE==g_comStatue)
			{
				//BkLiteAutoOff();
#ifdef LINK_BKLIGHT_ON
				if(BK_OFF==bkParas.bkFlag)
				{
					BkLiteAutoOff();
				}
#endif
				g_cmrLinked = 0;
			}
			g_dspUpdateFlag^=DSP_UPDATE_LINK;
		}
#endif

#if  0
		if(g_dspUpdateFlag&DSP_UPDATE_LKDEV)
		{
			if((SETTING_RFSCAN!=g_SettingIndex)&&(SETTING_ID!=g_SettingIndex)&&(SETTING_SNRMENU!=g_SettingIndex)
				 &&(SETTING_RFGR_SEL !=g_SettingIndex))
			{
				RfDevLinkMark(mainPara.workMode,&wlSet);
				if(!ChkAllNone(radioStatue.devCnt,5))
				{
					CHARGE_LED_OFF;
					g_chargeOk = AD_CHARGE_LOW;	
				}else
				{
					CHARGE_LED_ON;
					g_chargeOk = AD_CHARGE_RDY;
				}
			}
#ifdef UART1_TRACE_DEBUG
			printf("DSP_UPDATE_LKDEV\r\n");
			for(tmrErr=0;tmrErr<10;tmrErr++)
			{
				printf("%x ",rcvBuf[tmrErr]);
			}
			printf("\r\n");
#endif
			
			g_dspUpdateFlag^=DSP_UPDATE_LKDEV;
		}
#endif

		if(g_dspUpdateFlag&DSP_UPDATE_SHOT)
		{
			OneTimeFlashHandle();
			g_dspUpdateFlag^=DSP_UPDATE_SHOT;
		}
/*
		if(g_dspUpdateFlag&DSP_UPDATE_CMDMOD)
		{
			CmdModDisplay();
			RfMoudleSet();
			RfModSwInfo();
			g_dspUpdateFlag^=DSP_UPDATE_CMDMOD;
		}
*/

/*		if(g_dspUpdateFlag&DSP_UPDATE_LINKM2S)
		{
			CmdModDisplay();
			g_dspUpdateFlag^=DSP_UPDATE_LINKM2S;
		}
*/

		

		if(g_dspUpdateFlag&DSP_SLACMD_MODEL)
		{
#ifdef UART1_TRACE_DEBUG
			printf("Model\r\n");
#endif
			UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			gWSleepCounter = 0;
			//SpiRearUp(100);
//#ifdef _UART_INT_INFO_
//						SpiRearUp(20,TM_HANDLE_REAR);
//#else
						SpiRearUp(20);
//#endif
			g_dspUpdateFlag^=DSP_SLACMD_MODEL;
			
		}

		if(g_dspUpdateFlag&DSP_SLACMD_TEST)
		{
			//wlSet.m_ttlCmd[0] = wlSet.m_ttlCmd[1] = wlSet.m_ttlCmd[2] = 0x98;
			//flaPara.rfCmuPara[0].ttlExpVal = flaPara.rfCmuPara[1].ttlExpVal = flaPara.rfCmuPara[2].ttlExpVal
			//=flaPara.rfCmuPara[3].ttlExpVal=flaPara.rfCmuPara[4].ttlExpVal = 0x98;
			//RfFlashConfirm(2,FLASH_PILOT_ON);
			//delay_ms(20);
			//RfCountDownInfo(0);
			Pilot();
			gWSleepCounter = 0;
			g_dspUpdateFlag^=DSP_SLACMD_TEST;
		}

		if(g_dspUpdateFlag&DSP_SLACMD_REL)
		{
			if(COM_STATUE_IDLE==g_comStatue)
			{
				ACT_SHUTTER = 1;
			}
			WIRE_SHUTTER = 1;
			gWSleepCounter = 0;
			FLAOK_ON();
			OSTmrStart(g_shutterPullTmr, &tmrErr);			//it cannot start a timer in interrupt routine
			g_dspUpdateFlag^=DSP_SLACMD_REL;
		}

		if (g_dspUpdateFlag&DSP_UPDATE_PWON)
		{
			BkLiteOn();
			g_dspUpdateFlag ^= DSP_UPDATE_PWON;
		}

		if (g_dspUpdateFlag&DSP_UPDATE_RFINFO)
		{
			if(!uart_tran_flag)
			{
				RfParaSend();	
				g_dspUpdateFlag ^= DSP_UPDATE_RFINFO;
			}
			
		}
		
//#ifdef _UART_INT_INFO_
//		if(g_dspUpdateFlag&DSP_UPDATE_UARTINT)
//		{
//			UART_INFO =1;
//			g_dspUpdateFlag^=DSP_UPDATE_UARTINT;
//		}
//#endif

#ifdef _1DX_DEBUG_
		if(g_dspUpdateFlag&DSP_UPDATE_DEBUG)
		{
			LCD_Clear(bkSetArr[pSnrSet[SNRMENU_BKCLR]]);
			for(i=0;i<g_UkdCnt;i++)
			{
				DspUnknownData(i, g_UnknowData[i]);
			}
			if(g_UkdCnt>8)
			{
				g_UkdCnt = 0;
			}
			g_dspUpdateFlag^=DSP_UPDATE_DEBUG;
		}
#endif

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

		OSTimeDlyHMSM(0,0,0,50);
		
		//g_dspUpdateFlag=0;
	}
	
}


unsigned int CanOutputSwitch(unsigned char mode)
{
	return 0;
}





















