#include "hw\uart\uart.h"
#include "hw\boot\boot.h"
#include "hw\eeprom\eeprom.h"
#include "aes\aes.h"
#include "hw\spi\spi.h"
#include "global.h"
#include "hw\adc\adc.h"
#include "hw\moto\moto.h"
#include "hw\flash\flash.h"
#include "hw\lcd\lcd.h"
#include "hw\a7105\a7105.h"
#include "hw\sys\sys.h"
#include "hw\timer\timer.h"

#define SLEEP_SECONDS 6000l                            // 5 minute
#define SLAVE_SLEEP_SECONDS 60000L              // 50 minute
//#define SLAVE_SLEEP_SECONDS 200              // 50 minute

// $PROJ_DIR$\config\lnkstm8l152r8.icf

u8 g_canComStatue=0;
u8 g_nikComStatue=0;
u8 g_nikSuspend = 0;                                //  闪光后暂停一次sbo的下降沿
u8 g_spiRcvFlag = 0;
u8 g_spiDat = 0;
//u8 g_tmCtrl = 0;
WL_ArrPara *mstPara;
MainPara mainPara = {0};
MotoSet motoSet = {0};
u8 g_chargeOk = 1;
MenuPara menuPara={0};
u8 g_shutterFlag=0;
u8 g_vCmr=0;
MultiSet multiSet = {0};
u8 g_cmrLinked = 0;
u8 *g_cmdMode;
u8 *g_lightMode;
u8 *g_rfMode;
u16 g_dspUpdateFlag=0;
u16 g_bkLightCnt = 0;
u16 g_generalTimer = 0;
u16 g_genMsTick = 0;
u16 g_sysSleepCnt = 0;
extern u8 *g_cfnOption;
extern const u8 g_a7105_id1[4];
extern CmrPara cmrPara;
u8 g_sleepRegDdr[7];
u8 g_sleepRegOdr[7];
u8 g_sleepRegCr1[7];
u8 g_sleepRegCr2[7];
u8 g_sleepTag = 0;
extern u8 g_sboPullFlag;                      // d200 needed
extern u16 g_sboPullTimer;

void GeneralHandle(void);
void DspToggle(void);
extern void SlaveLedToggle(void);
void BeeperOn(u8 cnt, u16 tms);
extern void CanSckIntEventLoop(void);
u8 CalcSum(u8 *rcv, u8 len);
void SlaveFlashEnableCounterClear();

#if 1
typedef struct CAN_FONT
{
	char ch;
	char wt;
	char buf[14];
}CanFont;

const CanFont canFontCmpFeb[] =
{
	{'0',7,{0x00,0x0F,0x1F,0x18,0x18,0x1F,0x0F,0x00,0xF0,0xF8,0x18,0x18,0xF8,0xF0,}},
	//{'1',7,{0x00,0x00,0x0C,0x18,0x3F,0x3F,0x00,0x00,0x00,0x00,0x00,0xF8,0xF8,0x00,}},
	{'1',7,{0x00,0x00,0x06,0x0C,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0xF8,0xF8,0x00,}},
	{'2',7,{0x00,0x0C,0x1C,0x18,0x19,0x1F,0x0F,0x00,0x38,0x78,0xF8,0xD8,0x98,0x18,}},
	{'3',7,{0x00,0x18,0x19,0x19,0x19,0x1F,0x0E,0x00,0x18,0x98,0x98,0x98,0xF8,0xF0,}},
	{'4',7,{0x00,0x07,0x0F,0x1C,0x1F,0x1F,0x00,0x00,0xE0,0xE0,0x60,0xF8,0xF8,0x60,}},
	{'5',7,{0x00,0x1F,0x1F,0x19,0x19,0x19,0x18,0x00,0x98,0x98,0x98,0x98,0xF8,0xF0,}},
	{'6',7,{0x00,0x0F,0x1F,0x19,0x19,0x19,0x18,0x00,0xF0,0xF8,0x98,0x98,0xF8,0xF0,}},
	{'7',7,{0x18,0x18,0x18,0x1B,0x1F,0x1E,0x00,0x00,0x38,0xF8,0xE0,0x80,0x00,0x00,}},
	{'8',7,{0x00,0x0E,0x1F,0x19,0x19,0x1F,0x0E,0x00,0xF0,0xF8,0x98,0x98,0xF8,0xF0,}},
	{'9',7,{0x00,0x0F,0x1F,0x19,0x19,0x1F,0x0F,0x00,0x18,0x98,0x98,0x98,0xF8,0xF0,}},
	{'*',5,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,}},		
	{'c',4,{0x00,0x01,0x02,0x02,0x00,0xF0,0x08,0x08,}},
	{'h',5,{0x00,0x03,0x00,0x00,0x00,0x00,0xF8,0x80,0x80,0x78,}},
	{'.',3,{0x00,0x00,0x00,0x00,0x08,0x00,}},
	

	{'+',7,{0x00,0x01,0x01,0x07,0x07,0x01,0x01,0x00,0x80,0x80,0xE0,0xE0,0x80,0x80,}},
	{'%',7,{0x00,0x06,0x06,0x1F,0x1F,0x06,0x06,0x00,0x30,0x30,0xB0,0xB0,0x30,0x30}},
	{'-',7,{0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x80,0x80,0x80,0x80,0x80,0x80,}},	
};

const char font_cfn21[] = {
0x02,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x99,
0xDB,0x99,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE7,0xC3,0xC3,0xE7,0x7E,0x3C,
0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x18,
0x60,0x80,0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xE7,0xC3,0xC3,0xE7,0x7E,0x3C,0x01,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x18,0x60,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xC3,0xC3,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xE7,0xC3,0xC3,0xE7,0x7E,0x3C,0x01,0x00,0x00,0x00,0x40,0xE0,0xC0,0x80,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x78,0x7E,0x7E,0xFE,0xFE,0x7E,0x1E,
0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x18,0x60,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x78,0x7E,0x7E,0xFE,0xFE,0x7E,0x1E,0x06,0x00,0x00,0x00,0x00,
0x00,0x00,0x06,0x18,0x60,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x38,0xF8,0xE0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,
0x78,0x7E,0xFE,0xFE,0x7E,0x1E,0x06,
};/*22x24*///灯头扩散
#endif

static void Init_OSC(void)
{
    
    CLK_HSITRIMR|=0x04;
    while(!(CLK_ICKCR & 0x02));
    CLK_CKDIVR = 0;
    ITC_SPR2 = 0;	
}

void BkLightOn(void)
{
	BK_LIGHT_PIN = 1;
	//g_bkLightCnt = 10000;
	g_bkLightCnt = 10000/50;
	//printf("bk on\r\n");
	if (CMR_LINK_NIK == g_cmrLinked)
	{
		SetBkLightInfo(1);
	}
}

void BkLightOff(void)
{
	BK_LIGHT_PIN = 0;
	g_bkLightCnt = 0;
	//printf("bk off\r\n");
	if (CMR_LINK_NIK == g_cmrLinked)
	{
		SetBkLightInfo(0);
	}
}

void Init_GPIO(void)
{
     //PD_DDR_DDR4 = 1;
     //PD_CR1_C14 = 1; 
     //PD_CR2_C24 = 0;
#if 0
     // sck as int pad
    CPU_CCR=0X28;  //等级3
    EXTI_CONF1=0x00;//PB6->EXTI6
    //EXTI_CR2=0x08;//下降沿触发
    PB_DDR_DDR5=0;//PB6输入
    //PB_CR1_C15=1;//PB6上拉
   //PB_CR2_C25=1;//PB6中断使能

   // test pin
   PB_DDR = 0x08;
   PB_CR1 = 0x08;         // Push-pull,
   PB_CR2 = 0x08;         // speed 10 MHz
   //
   PF_DDR = 0x23;
   PF_CR1 = 0x23;         // Push-pull,
   PF_CR2 = 0x23;         // speed 10 MHz

   PE_CR1 |= 0x01;

   // moto 
   PA_DDR &= 0x3f;
   PA_DDR |= 0x80;           // PA6, PA7
   PA_CR1 |= 0xC0;
   PA_CR2 |= 0x80;

   PG_DDR |= 0x03;           // PG0~PG1 
   PG_CR1 |= 0x03;
   PG_CR2 |= 0x03;
   
   PC_DDR &= ~0x80;
   PC_CR1 |= 0x80;


   // charge pmw
   PD_DDR |= 0x80;
   PD_CR1 |= 0x80;
   PD_CR2 |= 0x80;

   //PC4
   PC_DDR |= 0x10;
   PC_CR1 |= 0x10;
   PC_CR2 |= 0x10;


   //PA5 30V       PA4 FLASH    PA3 Triger
   PA_DDR |= 0x38;                   // 
   PA_CR1 |= 0x38;
   PA_CR2 |= 0x38;  

   //Flash in
   PB_DDR &= ~0x10;
   PB_CR1 |= 0x40;
   PB_CR2 |= 0x40;
   
   D30V_EN = 1;                                   // ENABLE 30V, IGBT ENABLE
   ENABLE_CHARGE();
   //SPI_MISO_AUX ( 1 );
   //COM_AUX = 1;
   EN339 = 1;
#else

     // sck as int pad
    CPU_CCR=0X28;  //??3
    EXTI_CONF1=0x00;//PB6->EXTI6
    //EXTI_CR2=0x08;//?????
    PB_DDR_DDR5=0;//PB6??
    //PB_CR1_C15=1;//PB6??
   //PB_CR2_C25=1;//PB6????
  
   // test pin
   //PB_DDR = 0x08;
   //PB_CR1 = 0x08;         // Push-pull,
   //PB_CR2 = 0x08;         // speed 10 MHz
   //
   PF_DDR = 0x63;
   PF_CR1 = 0x63;         // Push-pull,
   //PF_CR2 = 0x63;         // speed 10 MHz
   PF_CR2 |= 0x02;

   PE_CR1 |= 0x01;
   
   // moto 
   PA_DDR &= 0x3f;
   PA_DDR |= 0x80;           // PA6, PA7
   PA_CR1 |= 0xC0;
   PA_CR2 |= 0x80;

   PG_DDR |= 0x03;           // PG0~PG1 
   PG_CR1 |= 0x03;
   PG_CR2 |= 0x03;
   
   PC_DDR &= ~0x80;
   PC_CR1 |= 0x80;


   // charge pmw
   PD_DDR |= 0x80;
   PD_CR1 |= 0x80;
   //PD_CR2 |= 0x80;                         // set cause unexpect result
 /* */
   PC_DDR |= 0x18;
   PC_CR1 |= 0x18;
   PC_CR2 |= 0x18;

   // charge LED,   AF
   PD_DDR |= 0x70;
   PD_CR1 |= 0x70;
   PD_CR2 |= 0x70;  

   //PC4
   PC_DDR |= 0x10;
   PC_CR1 |= 0x10;
   PC_CR2 |= 0x10;

#if (BOARD_VER == __BOARD_10_)
   //Flash in
   PB_DDR &= ~0x10;
   PB_CR1 |= 0x10;
   PB_CR2 |= 0x10;
   // PB4  Failing edge
   EXTI_CR2 &= 0xFC;
   EXTI_CR2 |= 0x02;
#elif (BOARD_VER == __BOARD_20_)
   //Flash in
   PB_DDR &= ~0x08;
   PB_CR1 |= 0x08;
   PB_CR2 |= 0x08;
   // PB4  Failing edge
   EXTI_CR1 &= 0x3F;
   EXTI_CR1 |= 0x80;                     // ver 2 

#endif

   PB_CR1 |= 0x02;                         // PB1 pull up

    //PA5 30V       PA4 FLASH    PA3 Triger
   PA_DDR |= 0x38;                   // 
   PA_CR1 |= 0x38;
   PA_CR2 |= 0x38;  

   // BK light
   PE_DDR |= 0x80;
   PE_CR1 |= 0x80;

   // BEEP
   //PA_DDR |= 0x01;
   //PA_CR1 |= 0x01;

   // a7105
	PB_DDR &= ~0x01;           // PB0 ~ a7105_gio2
	PD_DDR |= 0x06;           // PD1 ~ a7105_sck PD2 ~ a7105_sdio
	PD_CR1 |= 0x06;           // 
	PD_CR2 |= 0x06;
	PD_DDR &= ~0x80;         // PD3 ~ gpo1
	//PE_DDR |= 0x20;           // PE1 ~ a7105_cs
#if (BOARD_VER == __BOARD_10_)
	PE_DDR |= 0x38;           // PE3 ~ rx PE4 ~ tx
	PE_CR1 |= 0x38;
#elif (BOARD_VER == __BOARD_20_)
	PE_DDR |= 0x28;           // PE3 ~ rx PE4 ~ tx
	PE_CR1 |= 0x28;
#endif
	PE_ODR |= 0x20;            // cs pull up
	//PE_ODR_ODR5 = 1;

	   // link led port
	//PC_DDR |= 0x03;
	//PC_CR1 |= 0x03;
	PC_DDR |= 0x60;
	PC_CR1 |= 0x60;

	// set RF receive int failing edge
	/*需要设置RF 接收中断为下降沿，否则当切换为非RF 模式时
	int   脚已经被拉低，中断不断产生，导致假死现象*/
	EXTI_CR1 &= 0xFC;
	EXTI_CR1 |= 0x02;
    
/*
	#define A7105_SCK PD_ODR_ODR1
	#define A7105_CS   PE_ODR_ODR1
	#define A7105_ODR   PD_ODR_ODR2
	#define A7105_IDR    PD_IDR_IDR3
	#define A7105_IRQ    PB_IDR_IDR0
	#define A7105_RX_AF    PE_ODR_ODR3
	#define A7105_TX_AF    PB_ODR_ODR4
*/  
#if (BOARD_VER == __BOARD_10_)
	PB_DDR |= 0x0C;                             // READY
	PB_CR1 |= 0x0C;
#elif (BOARD_VER == __BOARD_20_)
	PB_DDR |= 0x14;                             // READY, remote
	PB_CR1 |= 0x14;

	PE_DDR |= 0x10;
	PE_CR1 |= 0x10;                           // PE4 act as NIK_DAT_OUT_SW

	NIK_ODR_ALWAYS();
#endif
   

   // sw
   //PG_CR1 |= 0xC0;                          // PG2, PG3 PULL UP
   PG_CR1 |= 0x0C;                          // PG2, PG3 PULL UP

#ifndef _A7105_DBG_
//delete 20180110
/*
   if (!SW_POWER_OFF)                      // the power is not on
   {
   	printf ("halt\r\n");                      // 20180110
	asm("sim");
	asm("halt");
	//while (1);
   }*/
#endif

   PC_DDR |= 0x03;
   PC_CR1 |= 0x03;

   //printf ("power on\r\n");                      // 20180110

   //SPI_MISO_AUX ( 1 );
   D30V_EN = 1;
   BkLightOn();
   ENABLE_CHARGE();
   DC_ON();
   COM_AUX = 1;                    // nikon also needed
   EN339 = 1;
   
   
#endif
}

u16 get_counter(void)
{
	u16 tim2;
	tim2 = (u16)TIM2_CNTRH << 8;
	tim2 |= TIM2_CNTRL;
	return tim2;
}

u8 CalcSum(u8 *rcv, u8 len)
{
	u8 i=0,sum=0;
	for(i=0;i<len;i++)
	{
		sum+=rcv[i];
	}
	return sum;
}

void UserInit(void)
{
	//u8 i = 0;
	u8 *pStart;
	u8 *pEnd;
	
	mstPara = &mainPara.modGr;
	motoSet.mode = &mainPara.motoLoca.mode;
	motoSet.dest_location = &mainPara.motoLoca.moto_location;
	multiSet.eTimes = &mainPara.multi_times;
	multiSet.frequence = &mainPara.multi_Frq;
	multiSet.expose = &mainPara.modGr.multi_exp;
	g_cmdMode = &mainPara.cmdMode;
	g_lightMode = &mainPara.workMode;
	g_cfnOption = mainPara.cfnSet;
	g_rfMode = &mainPara.s_rfMode;
	
	motoSet.moto_en = MOTO_ENABLE;
	motoSet.autoDest = 35;

	EEPROM_Read(0, (u8*)&mainPara, sizeof(mainPara));

	pStart = (u8*)(&mainPara.workMode);
	pEnd = (u8*)(&mainPara.sumVeri);

	if (mainPara.sumVeri != CalcSum(pStart, (pEnd - pStart)))
	{
		memset((u8*)&mainPara, 0, pEnd - pStart);
	}

	//if ((mainPara.cmdMode > CMD_MODE_LIGH_F2) || (mainPara.cmdMode < CMD_MODE_OFF))
        if (mainPara.cmdMode > CMD_MODE_LIGH_F2)
	{
		mainPara.cmdMode = CMD_MODE_OFF;
	}

	if (*multiSet.eTimes < 1)
	{
		*multiSet.eTimes = 1;
	}

	if ((*multiSet.frequence < 1) || (*multiSet.frequence > 200))
	{
		*multiSet.frequence = 1;
	}

	if ((*motoSet.dest_location < 20) || (*motoSet.dest_location > 200))
	{
		*motoSet.dest_location = 20;
	}

	if (*multiSet.expose > 0x50)
	{
		*multiSet.expose = 0x50;
	}

	if (*g_rfMode > 1)
	{
		*g_rfMode = 0;
	}

	//menuPara.WlmMul[MASTER]=output_table[(7-(*multiSet.expose>>4))*3];

	//*motoSet.dest_location = 50;

	motoSet.destRsv = *motoSet.dest_location;
	//motoSet.focLen = *motoSet.dest_location;
	 motoSet.focIndex = GetMotoStdIndex(*motoSet.dest_location);

	//*motoSet.mode = MOTO_MODE_MANU;

       /* 
	for (i = 0; i < ((u8*)(&mainPara.sumVeri) - (u8*)(&mainPara.workMode));i++)
	{
		((u8*)(&mainPara))[i] = i;
	}*/

	//memcpy((u8*)&mainPara,canFontCmpFeb[7].buf,(u8*)&mainPara.sumVeri-(u8*)&mainPara.workMode);
	g_cmrLinked = CMR_LINK_NULL;

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
/*
	for (i = 0;i<sizeof(mainPara); i++)
	{
		printf ("%x",  ((u8*)&mainPara)[i]);
	}
*/
}

void BeepHandleProc(void)
{
	if (g_beepEn > 0)
	{
		if (g_beepCnt > 0)
		{
			g_beepCnt --;
		}else
		{
			g_beepEn = 0;
			BeeperOff();
		}
	}
}

void GeneralEventLoop(void)
{
	//u8 vol;
	u16 crtTime;
	u16 msTick;
	crtTime = get_counter();
	msTick = crtTime;
	//if(crtTime - g_generalTimer > 1000)
	if(crtTime - g_generalTimer > 50000L)                      // 50 ms
	{
		g_generalTimer = crtTime;
		GeneralHandle();
	}

	if (msTick - g_genMsTick > 1000L)
	{
		g_genMsTick = msTick;
		SlaveLedToggle();
		CmrLinkWatchDog();
		SlaveFlashEnableCounterClear();
	}
}

void SwRfMode(void)
{
	if (CMD_MODE_OFF != *g_cmdMode)
	{
		*g_cmdMode = CMD_MODE_OFF;
		//A7105_RX_AF = 0;
//#if (BOARD_VER == __BOARD_10_)
		//A7105_TX_AF = 0;
//#endif
		//*g_lightMode = WORKINGMODE_TTL;
		//RF_LED_OFF();
		//A7105Halt();
		RfHardWareCheck();
		
	}

}

u8 g_nikNssInvalidCnt = 0;
void CanNikComMonitor(void)
{
	if (COM_STATUE_IDLE == g_canComStatue)                   //   take canon as the first priority
	{
		if ( SCK_IN )                                                              // if SCK_IN is high, ready for canon com
		{
			//RF_LED_RED ^= 1;
			//NikTmComRst();
			asm("sim");
			SetSpiTrans();
			asm("rim");
			g_cmrLinked = CMR_LINK_CAN;
			//SwRfMode();
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef CAN_COM_DBG
			printf("can linked\r\n");
#endif
		}else                                                                         // canon have no actives, check nikon port
		{
			if (COM_STATUE_IDLE == g_nikComStatue)
			{
				//if (NIKONNSSI > 0)
				if (PG_IDR & 0x10)
				{
					//WaitCmrHandle();
					NikComInit();
					g_nikComStatue = COM_STATUE_START;
					g_cmrLinked = CMR_LINK_NIK;
					//SwRfMode();
					//SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef NIK_COM_DBG
					printf("Nik link\r\n");
#endif
				}
			}else if (COM_STATUE_START == g_nikComStatue)
			{
				if (!(PG_IDR & 0x10))
				{
					g_nikNssInvalidCnt ++;
					if (g_nikNssInvalidCnt > 30)
					{
						g_nikNssInvalidCnt = 0;
						NikTmComRst();
					}
				}else if(g_nikNssInvalidCnt > 0)
				{
					g_nikNssInvalidCnt = 0;
				}
			}else if(COM_STATUE_REALFLASH == g_nikComStatue)
			{
				if (1 == g_sboPullFlag )
				{
					if (NIKONNSSI > 0)
					{
#if (BOARD_VER == __BOARD_20_)
						NIK_ODR_ON();
#endif
						PULL_MOSI_LOW();	
						
						g_sboPullFlag = 2;
						g_sboPullTimer = get_counter();
					}
				}else if (2 == g_sboPullFlag)
				{
					if (get_counter() - g_sboPullTimer > 2300)
					{
						g_sboPullFlag = 0;
						PULL_MOSI_HIGH();	
#if (BOARD_VER == __BOARD_20_)
						NIK_ODR_OFF();
#endif
					}
				}
			}else if (COM_STATUE_FOCUSDEPTH == g_nikComStatue)
			{
				if (!g_isShadowOn)
				{
					PULL_MOSI_LOW();
					g_isShadowOn = 1;
					//g_flashTime = g_expTime[0];
					g_flashTime = OUTPUT_SHADOW;
					g_flashEnable = 3;
					g_shadowTimer = get_counter();
					g_shadowCnt = 0;
				}else
				{
					if (get_counter() - g_shadowTimer > 15100)
					{
						//g_flashTime = g_expTime[0];
						g_flashTime = OUTPUT_SHADOW;
						g_flashEnable = 3;
						g_shadowTimer = get_counter();
						g_shadowCnt ++;
						if (g_shadowCnt >= 66)
						{
							g_isShadowOn = 0;
							g_nikComStatue = COM_STATUE_IDLE;
						}
					}
				}
				
				
			}
		}
		
	}
}

void StartSystem(void)
{
	MotoReset();
	for (;;)
	{	
		CanNikComMonitor();
#ifndef _A7105_DBG_
		KeyEventLoop();
#endif
		DspUpdateEventLoop();
		MotoEventLoop();
		FlashEventLoop();
//#ifndef FLASH_SYN_DBG
		ChargeEventLoop();
//#endif
#ifndef _A7105_DBG_
		GeneralEventLoop();
#endif
		RfSlaveEventLoop();
		MultiFlashEventLoop();
		AfEventLoop();
		
		//CanSckIntEventLoop();
		//Timer1Test();
		//RF_LED_LINK();
		//RF_LED_READY();
		//RF_LED_GREEN ^= 1;
		//RF_LED_RED ^= 1;
	}
}

//#define _LS_DBG_
int main( void )
{
#ifdef _A7105_DBG_
        int i = 0;
        u8 buf[16];
#endif
        //u16 tm;
        asm("sim");
        Init_OSC();
	//delay_ms(300);
        //asm("rim");//开中断，sim为关中断
	Init_USART1();
	//printf("uart\r\n");     //20180110
	Adc_Init();
	//printf("adc\r\n");    //20180110
	UserInit();
	//printf("user\r\n");    //20180110
        Init_GPIO();
	//printf("gpio\r\n");    //20180110
	/*for 7105 test*/
	//A7105_Init();
	RfHardWareCheck();
        
       printf("Speed light start, %s, %s\r\n",__DATE__, __TIME__);
	//printf("%d\r\n",font_cfn21[10]);
	//printf("%d\r\n",canFontCmpFeb[5].buf[2]);
	//printf("timers\r\n");

	Timer4Init();
	Timer2Init();
	
	//printf("spis\r\n");
	 //TIM4_CNTR = 10;
	 //TIM4_CR1 = 0x01; 
	 //Timer4Start(0x01);
        //Init_ExInt();
        //SetSckInt(INT_EDGE_R);
        //SPI_Init();
        SPI1_Init();
	 SPI2_Init();
	//PG_DDR_DDR5 = 1;
	//PG_CR1_C15 = 1;                 //PG5 输出
	//PG_CR2_C25 = 1;
	//NikSpiTrans();
	//SPI2_CR1 = 0xFF;
	//printf("nikinit\r\n");
	 //NikInit();
	 LcdInit();
	 //printf("dail\r\n");
	 SetDailInt();
	 //printf("pmw\r\n");
	 Timer1Init();

#ifdef FLASH_SYN_DBG
	g_chargeOk = AD_CHARGE_FUL;
	READY_ON();
#endif

         asm("rim");//开中断，sim为关中断
        //PD_DDR_DDR4=1;           //端口数据方向寄存器，输出
        //PD_CR1_C14=1;            //端口控制寄存器1，推挽输出
        //PD_CR2_C24=0;            //端口控制寄存器2，输出速率2MHz
        
        //Boot_Encrypt(__TIME__,"agc",buf,16);
        //Boot_Decrypt(buf,"agc",buf,16);
        //SPI2_DR = 0xFF;

	// for multi flash test
/*	*multiSet.eTimes = 10;
	*multiSet.frequence = 20;
	*multiSet.expose = 0x10;
	*g_lightMode = WORKINGMODE_MULTI;
	*g_cmdMode = CMD_MODE_OFF;
	g_flashEnable = 1;*/
	//if (CFN_ON == g_cfnOption[CFN_BEEP])
	if (g_cfnOption[CFN_BEEP] > 0)
	{
		BeeperOn(4,1);
	}

/***************************************************s
	// 延时5 个毫秒，等待拨盘中断发生
	// 20171229 处理拨盘第一次不响应
****************************************************/
	//printf("shadow=%d\n",g_shadowKey);
	delay_ms(5);
	g_shadowKey = 0;
	asm("sim");  
	ResetDailInt();
	asm("rim");  
/**********************end**************************/

	//CanPreFlash();

	//asm("halt");
	//printf("sys start\r\n");


#if (defined _A7105_DBG_)
	//mainPara.s_chan = 0;
	*g_cmdMode = CMD_MODE_RF;	
	*g_rfMode = RF_MODE_NIK;
	mainPara.s_array = 1;
	A7105_Init();
	RfHardWareCheck();
	StartSystem();
#elif (defined _LS_DBG_)
       *g_cmdMode = CMD_MODE_LIGH_F2;
	StartSystem();
#else
	StartSystem();
#endif
        
        while (1)
        {/*
        	if (!g_tmCtrl)
        	{
        		g_tmCtrl = 1;
			PF_ODR_ODR1 = 0;
			TIM4_CNTR = 0x00;
			//TIM4_ARR = 100;             //auto reload
			TIM4_CR1 = 0x01;
		}*/
		//PG_ODR_ODR5 ^= 1;	
		//TEST_PIN ^= 1;
		//tm = (TIM2_CNTRH << 8 | TIM2_CNTRL);
		//printf("%x\r\n",tm);
		//PE_ODR_ODR5 = !PE_ODR_ODR5;
#if (defined _A7105_DBG_)
		//RfSlaveEventLoop();
		if (!A7105_IRQ)
		{
			A7105_Read_Ram(0x05, buf, 12);
			printf("\r\n");
			for (i=0;i<12;i++)
			{
				printf("%x ",buf[i]);
			}
			A7105_strobe_cmd(0xF0);
			A7105_strobe_cmd(0xC0);
		}

#elif (defined _LS_DBG_)
#endif
		//READY_ON();
		//READY_OFF();
		//RfSlaveEventLoop();
        }
        
        
        //BootProc();
}
/*******************************************************************************
*******************************蓝旗嵌入式系统***********************************
*************************http://lenchimcu.taobao.com****************************
*******************************************************************************/
#if 0
#pragma vector= USART1_R_OR_vector
__interrupt void USART1_R_OR_IRQHandler(void)
{
      unsigned char ch;
      ch=USART1_DR;
      putchar(ch+1);
}
#endif

void Hal_ProcessPoll (void)
{

}

void AfValidOnCheck(void)
{
	if(g_isAfOn>0)
	{
		if(cmrPara.cls>0)
		{
			g_afOnCnt++;   //50ms every increment
			//if(g_afOnCnt>2000)
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

void SleepPortRecord(u8 *pddr, u8 * podr, u8 *pcr1, u8 *pcr2)
{
	pddr[0] = PA_DDR;
	pddr[1] = PB_DDR;
	pddr[2] = PC_DDR;
	pddr[3] = PD_DDR;
	pddr[4] = PE_DDR;
	pddr[5] = PF_DDR;
	pddr[6] = PG_DDR;

	podr[0] = PA_ODR;
	podr[1] = PB_ODR;
	podr[2] = PC_ODR;
	podr[3] = PD_ODR;
	podr[4] = PE_ODR;
	podr[5] = PF_ODR;
	podr[6] = PG_ODR;

	pcr1[0] = PA_CR1;
	pcr1[1] = PB_CR1;
	pcr1[2] = PC_CR1;
	pcr1[3] = PD_CR1;
	pcr1[4] = PE_CR1;
	pcr1[5] = PF_CR1;
	pcr1[6] = PG_CR1;

	pcr2[0] = PA_CR2;
	pcr2[1] = PB_CR2;
	pcr2[2] = PC_CR2;
	pcr2[3] = PD_CR2;
	pcr2[4] = PE_CR2;
	pcr2[5] = PF_CR2;
	pcr2[6] = PG_CR2;


	// should set CR2 first, otherwise cause extern interrput.
        PA_CR2 = 0x00; 
 	PB_CR2 = 0x00; 
 	PC_CR2 = 0x00; 
 	PD_CR2 = 0x00;
 	PE_CR2 = 0x00; 
 	PF_CR2 = 0x00; 
	PG_CR2 = 0x00;

	PA_DDR = 0x00;
 	PB_DDR = 0x00;
 	PC_DDR = 0x00; 
 	PD_DDR = 0x00; 
 	PE_DDR = 0x00; 
 	PF_DDR = 0x00; 
	PG_DDR = 0x00;
  
 	PA_ODR = 0x00; 
 	PB_ODR = 0x00; 
 	PC_ODR = 0x00; 
 	PD_ODR = 0x00; 
 	PE_ODR = 0x00; 
 	PF_ODR = 0x00; 
	PG_ODR = 0x00;

 	PA_CR1 = 0x00; 
 	PB_CR1 = 0x00; 
 	PC_CR1 = 0x00; 
 	PD_CR1 = 0x00; 
 	PE_CR1 = 0x00; 
 	PF_CR1 = 0x00; 
	PG_CR1 = 0x00;

	
}

void SleepPortResume(const u8 *pddr, const u8 * podr, const u8 *pcr1, const u8 *pcr2)
{
	
 	PA_DDR=pddr[0];
 	PB_DDR=pddr[1];
 	PC_DDR=pddr[2]; 
 	PD_DDR=pddr[3]; 
 	PE_DDR=pddr[4]; 
 	PF_DDR=pddr[5]; 
	PG_DDR=pddr[6];
 
 	PA_ODR=podr[0]; 
 	PB_ODR=podr[1]; 
 	PC_ODR=podr[2]; 
 	PD_ODR=podr[3]; 
 	PE_ODR=podr[4]; 
 	PF_ODR=podr[5]; 
	PG_ODR=podr[6];

 	PA_CR1=pcr1[0]; 
 	PB_CR1=pcr1[1]; 
 	PC_CR1=pcr1[2]; 
 	PD_CR1=pcr1[3]; 
 	PE_CR1=pcr1[4]; 
 	PF_CR1=pcr1[5]; 
	PG_CR1=pcr1[6];
 
 	PA_CR2=pcr2[0]; 
 	PB_CR2=pcr2[1]; 
 	PC_CR2=pcr2[2]; 
 	PD_CR2=pcr2[3];
 	PE_CR2=pcr2[4]; 
 	PF_CR2=pcr2[5]; 
	PG_CR2=pcr2[6];

	//delay_ms(5);
	delay_ms(1);
/*
	if (SCK_IN)
	{
		g_cmrLinked = CMR_LINK_CAN;
		asm("sim");
		SetSpiTrans();
		asm("rim");
	}else if (NIKONNSSI)
	{
		NikComInit();
	}else
	{
		NikComInit();
		SetSckInt(INT_EDGE_R);
	}
*/

/*
	switch (g_sleepTag)
	{
		case SLEEP_TAG_CAN:
			//g_cmrLinked = CMR_LINK_CAN;
			if (SCK_IN)
			{
				g_cmrLinked = CMR_LINK_CAN;
				SetSpiTrans();
			}else
			{
				g_cmrLinked = CMR_LINK_NULL;
			}
		break;

		case SLEEP_TAG_NIK:
			g_cmrLinked = CMR_LINK_NIK;
			NikComInit();
		break;

		default:
			g_cmrLinked = CMR_LINK_CAN;
			SetSpiTrans();	
		break;
	}
*/

}

void WakeUpSet(void)
{

//#ifdef SLEEP_DBG
//	printf("int set\r\n");
//#endif
	//asm("sim");

	g_sleepTag = 1;	
	// pilot
	PE_CR1 |= 0x01;
	PE_CR2 |= 0x01;

	if ( CMD_MODE_OFF == *g_cmdMode )
	{
//20171225 before
		PF_DDR = 0x21;    
		PF_CR1 = 0x21;
//after 
		//PF_DDR = 0x61;    
		//PF_CR1 = 0x61;
//end
		COM_AUX = 1;                    // nikon also needed
		//COM_AUX = 0;			// 20171225
	   	EN339 = 1;
		//DC_OFF();				// 20171225

		//sck
		asm("sim");                         // 2017/08/15  must added, otherwise the int edge cannot be set and enter sleep mode error
		SetSckInt(INT_EDGE_R);
		asm("rim");

		//asm("rim");
		
		// sbo
		SetNssSleepInt();
	}
	
}

void WakeUpIntClose(void)
{
	PE_CR2 &= ~0x01;            // pilot int close
	PB_CR2 &= ~0x20;            // sck int close
	PG_CR2 &= ~0x10;           // nss int close
	PG_CR2 &= ~0x40;           // sbo int close

	// reinit flash int,  failing edge
	asm("sim");	
	//CPU_CCR=0X28;  //等级3
	//EXTI_CONF2 |= 0x40;
	//EXTI_CONF2 |= 0x20;                  // ??  may be error 
	//EXTI_CR4 = 0x02;                // PORTG Failing edge only
	EXTI_CR2 &= 0xFC;
	EXTI_CR2 |= 0x02;
        asm("rim");
}


void SleepShutInt(void)
{

}

void SleepHandleProc(void)
{/*
	u8 pddr[6];
	u8 podr[6];
	u8 pcr1[6];
	u8 pcr2[6];*/

#ifdef SLEEP_DBG
	printf("sleep\r\n");
#endif

	CloseAdcClock();
	LcdWriteCmd(LCDOFF);
	LcdWriteCmd(SYSDIS);

//#ifdef SLEEP_DBG
//	printf("lcd closed\r\n");
//#endif

	SleepPortRecord(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);	

//#ifdef SLEEP_DBG
//	printf("port closed\r\n");
//#endif

	WakeUpSet();

//#ifdef SLEEP_DBG
//	printf("int setted\r\n");
//#endif

	delay_ms(10);


	//g_comStatue = COM_STATUE_SLEEP;   // behind the delay, other wise the wakeup Set is be setted
	g_canComStatue = COM_STATUE_SLEEP; 
	g_nikComStatue = COM_STATUE_SLEEP; 

#ifdef SLEEP_DBG
	printf("halt\r\n");
#endif

	asm("halt");

	delay_ms(2);

	//Init_OSC();
	WakeUpIntClose();
	
	SleepPortResume(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);

	LcdWriteCmd(LCDON);
	LcdWriteCmd(SYSEN);

	OpenAdcClock();

#ifdef SLEEP_DBG
	printf("wake up\r\n");
#endif	

	while(!PILOT_PIN);

	g_canComStatue = COM_STATUE_IDLE; 
	g_nikComStatue = COM_STATUE_IDLE;

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	// pilot as interrupt
	// sck as interrupt
	// 
}

void SystemSleepProc(void)
{
	if (CFN_ON == g_cfnOption[CFN_AUTO_SHUT])
	{
		if (CMD_MODE_OFF == *g_cmdMode)
		{
			if (CMR_LINK_NULL == g_cmrLinked)
			{
				g_sysSleepCnt ++;
				if (g_sysSleepCnt > SLEEP_SECONDS)
				{
					g_sysSleepCnt = 0;
					SleepHandleProc();
				}
			}
		}else
		{
			g_sysSleepCnt ++;
			if (g_sysSleepCnt > SLAVE_SLEEP_SECONDS)
			{
				g_sysSleepCnt = 0;
				if (CMD_MODE_RF == *g_cmdMode)
				{
					A7105Halt();
					RfIntPortSet(DISABLE);
				}
				SleepHandleProc();
				if (CMD_MODE_RF == *g_cmdMode)
				{
					//A7105_strobe_cmd(0x90);
					A7105_Init();
					RfIntPortSet(ENABLE);
				}
				
			}
		}
	}
}


void GeneralHandle(void)
{
//   back light
	if (g_bkLightCnt > 0)
	{
		g_bkLightCnt --;
		if (g_bkLightCnt <=0 )
		{
			BkLightOff();
		}
	}

	DspToggle();
	SlaveEventToggle();
	AfValidOnCheck();
	BeepHandleProc();
	SystemSleepProc();

// power off
	if (!SW_POWER_OFF)
	{
		//delay_ms(50);
		delay_ms(5);
		if(!SW_POWER_OFF)
		{
			//printf ("power off\n");
			mainPara.sumVeri = CalcSum((u8*)&mainPara,(u8*)(&mainPara.sumVeri) - (u8*)(&mainPara.workMode));
			EEPROM_Write(0, (u8*)&mainPara, (u8*)(&mainPara.sumVeri) - (u8*)(&mainPara.workMode) + 1);
			printf ("power off\n");
			delay_ms(500);
			//asm("sim");
			//asm("halt");
		}
	}
	
}

