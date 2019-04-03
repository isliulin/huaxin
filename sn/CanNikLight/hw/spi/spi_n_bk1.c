#include "spi.h"
#include"global.h"
#include <stdarg.h> 
#include "..\flash\flash.h"
#include "..\timer\timer.h"
#include "..\uart\uart.h"
#include "..\a7105\a7105.h"

#define RCVDAT_LOCATION_APERTURE 0x05
#define RCVDAT_LOCATION_ISO 0x03
#define RCVDAT_LOCATION_ISO_1 12
#define RCVDAT_LOCATION_ISO_2 13
#define RCVDAT_LOCATION_FOCLEN 0x06
#define RCVDAT_LOCATION_MODE 0x01
#define RCVDAT_LOCATION_BKMOD 0x00
#define RCVDAT_LOCATION_EXPOSE 0x01
#define RCVDAT_LOCATION_CAMERACMP 0x07
#define RCVDAT_LOCATION_SHUTTER	0x04

extern u8 g_spiRcvFlag;
extern u8 g_spiDat;
u8 g_datRcvFlag=0;
u8 ttl_pre2_flag = 0;

extern u8 g_spiRcvTotal;
extern u8 g_spiSendIndex;
extern u8 g_spiRcvIndex;//SPI_Receive_Index;
extern u8 g_spiRcvData[MAX_BUFFER_SIZE];
extern u8 g_spiSendData[SEND_MAX_SIZE];
extern u8 g_tmHandleFlag;
extern FlaParameter flaPara;
u8 g_isComStop = 0;
u8 g_isComPullup = 0;
void SetSBOInt(void);
static void SBOintHandle(void);
void SpiRcvIntProc(void);
void CameraCmdReply(u8 cmd);

u8 g_srFlag=0;
u8 g_srMod=0;
u8 g_cmdSrFlag=0;
u8 g_spiBuf = 0;
u8 g_cmdCode=0;
u8 g_srIndex = 0;
u8 g_srTotal=0;
u8 g_isAutoStby = 0;
u8 *g_pSendRcv;

CmrPara cmrPara={0};
u8 g_isAfOn = 0;
u16 g_afOnCnt = 0;

const u8 macData1[] = {
0x01,0x01,0x05,0x02,0x08,0x07,0x81,0x78,0xb4,0x64,0x0a,0x4e,
0xff,0x3e,0x90,0x3e,0x90,0x1d
};


u8 macData2[] = {
0x01,0x02,0xb6,0x04,0x00,0x00,0x88,0x54,0x30,0x23,0x00,0xff,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xeb,
};

/*
unsigned char macData2[] = {
0x01,0x81,0xb6,0x04,0x00,0xf8,0x88,0x54,0x30,0x24,0x00,0x00,
0xff,0x00,0x00,0x00,0x01,0x11,0x00,0xee,0x00,0x64,
};//master
*/

const u8 macData3[] = {
0x03,0x05,0x6d,0x9b,0x90,0x86,0x80,0x7a,0x72,0x6a,0x65,0x5e,
0x06,0x75,0x9e,0x94,0x8a,0x80,0x77,0x6f,0x66,0x60,0x58,0x06,
0x74,0x9e,0x95,0x8b,0x80,0x76,0x6f,0x66,0x60,0x59,0x04,0x5f,
0x96,0x8a,0x82,0x80,0x7e,0x79,0x71,0x6e,0x6a,0xfc,
};

const u8 macData4[] = 
{
0x0a,0x5e,0x69,
};

const u8 macData5[] = 
{
0x0a,0x05,0x10,
};

u8 sendRecord[3]={0};

/*
unsigned char compensation_table[37]={
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

WLM_abRation_fillwords wlm_abRation_fillwords[13] = 
{
	{0x01,0x19},{0x02,0x15},{0x03,0x12},{0x04,0x0f},{0x05,0x0c},{0x06,0x0a},{0x08,0x08},
	{0x0a,0x06},{0x0c,0x05},{0x0f,0x04},{0x12,0x03},{0x15,0x02},{0x19,0x01},
};


unsigned char feb_table[10] = {
0x00,0x03,0x05,0x08,0x0b,0x0d,0x10,0x13,0x15,0x18
};
*/

u8 const apert_code[] = {
0x00,0x02,0x04,0x06,0x08,
0x0a,0x0c,0x0e,0x0f,0x10,0x12,0x14,
0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,
0x20,0x21,0x22,0x24,0x26,0x27,0x28,0x2a,0x2c,0x2d,0x2e,
0x30,0x32,0x33,0x34,0x36,0x38,0x39,0x3a,0x3c,0x3d,0x3e,
};

u8 const iso_code[] = {
0x18,0x1a,0x1c,0x1e,
0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,
0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,
0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,
0x70,0x72,
};

//const u16 cmrIsoVal[] = {
const u32 cmrIsoVal[] = {
50,64,80,100,
125,160,200,250,320,400,500,640,
800,1000,1250,1600,2000,2500,3200,4000,
5000,6400,8000,10000,12800,16000,20000,25600,
32000,40000,51200,64000,81200,102400,128000,160000,
204800,250000,320000,409600,500000,640000,820000,1000000,
1280000,1600000,
};

const u32 cmrIsoValExt[] = 
{
50,72,100,
140,200,280,400,560,
800,1100,1600,2200,3200,4500,6400,9000,12800,18000,25600,
36000,51200,72000,102400,140000,204800,280000,409600,560000,820000,110000,1600000,
};

//250(0x30)  350(0x33)  500(0x36) 750(0x39) 1000(0x3c) 1500(0x3f) 
//2000(0x42) 3000(0x45) 4000(0x48)

u32 const nShutter[39] = 
{
/*	5,6,8,10,12,16,20,25,33,40,50,66,76,100,125,160,200,250,333,400,500,625,769,1000,1300,
	1600,2000,2500,3000,4000,5000,6000,8000,10000,13000,15000,20000,25000,30000,*/

	30000,25000,20000,15000,13000,10000,8000,6000,5000,4000,3000,2500,2000,1600,1300,1000,
	769,625,500,400,333,250,200,160,125,100,76,66,50,40,33,25,20,16,12,10,8,6,5
};

u8 const focLen_code[] ={
0x2d,0x30,0x31,0x32,0x33,0x34,0x36,0x37,0x38,0x39,0x3a,0x3c,0x3d,0x3e,0x40,0x41,0x42,0x43,0x44,0x45,0x46,
0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x55,0x56,
0x57,0x59,0x5a,0x5c,0x5e,0x60,0x62,0x64,0x65,0x66,0x67,0x6a,0x6c,0x6e,0x71,0x73,0x76,0x7a,0x80,/*0x8f,*/
};

u8 const focLen_val[] = {
18,20,21,21,22,23,24,25,25,26,27,28,29,30,32,33,33,34,35,36,38,
38,40,41,42,44,45,46,48,49,50,52,52,55,58,60,
//62,65,66,70,75,80,85,90,92,95,98,105,112,120,130,135,150,170,180,40,
62,65,66,70,75,80,85,90,92,95,98,105,112,120,130,135,150,170,200,/*40,*/
};

u8 const output_calc[] = {
0X2A,0X28,0X26,0X24,0X22,0X20,0X1E,0X1C,0X1A,0X18,0X16,
0X14,0X12,0X10,0X0E,0X0C,0X0A,0X08,0X06,0X04,0X02,0X00,
};

u8 const cmp_calc[] = {
0x12,0x10,0x0E,0X0C,0X0A,0X08,0X06,0X04,0X02,
0X00,0XFE,0XFC,0XFA,0XF8,0XF6,0XF4,0XF2,0XF0,
0XEE,
};

//const u16 isoValue[] = 
const u32 isoValue[] = 
{
	3,4,5,6,8,10,12,16,20,25,
	32,40,50,64,80,100,125,160,200,250,
	320,400,500,640,800,1000,1250,1600,2000,2500,
	3200,4000,5000,6400,8000,
//    the following is not in menu
	10000,12800,16000,20000,25600,
	32000,40000,51200,64000,81200,102400,128000,160000,	204800,250000,
	320000,409600,500000,640000,820000,1000000,1280000,1600000,
	
};

const u16 cmrApertVal[] = {
8,	10, 12, 14, 16,	
18,	20,	22, 24, 25, 28,	32,
33,	35,	38,	40,	42,	45,	48,	50,	53,	56,
63,	67,	71,	80,	90,	95,	100,110,130,130,140,
160,180,190,200,220,250,270,290,320,360,380
};

void RstComPara(void)
{
	g_datRcvFlag = 0;
	g_srIndex = 0;
	g_srTotal = 0;
	//g_comStatue = COM_STATUE_IDLE;
	g_nikComStatue = COM_STATUE_IDLE;
}

void NikTmComRst(void)
{
	ShutNssInt();
	RstComPara();
	//SetSBOInt();	
	g_isComStop = 0;
	g_cmrLinked = CMR_LINK_NULL;                              // reset link state
	//g_comStatue = COM_STATUE_IDLE;
	g_nikComStatue = COM_STATUE_IDLE;
	g_tmHandleFlag^=TM_nHANDLE_COMRST;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

#ifdef NIK_COM_DBG
	printf ("nik com rst\r\n");
#endif
}

void NikTmComTimeout(void)
{
	ShutNssInt();
	RstComPara();
	//SetSBOInt();	
	g_isComStop = 0;
	//g_comStatue = COM_STATUE_IDLE;
	g_nikComStatue = COM_STATUE_IDLE;
	g_tmHandleFlag^=TM_nHANDLE_COMTIMEOUT;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

#ifdef NIK_COM_DBG
	printf("nik time out\r\n");
#endif
}

void SetComRstTime(void)
{
	Timer4Start(1000/8);	
	g_tmHandleFlag |= TM_nHANDLE_COMRST;
	g_tmHandleFlag&=(~TM_nHANDLE_COMTIMEOUT);
}


void SetSBOInt(void)
{
	NikSpiStop();

	PG_DDR_DDR6 = 0;
	PG_DDR_DDR7 = 0;

	//CPU_CCR = 0x28;
	//EXTI_CONF2 |= 0x40;         // portG is use for interrupt generation
	//EXTI_CONF2 |= 0x20;         // portG is use for interrupt generation   
	//EXTI_CR4 = 0x02;                // Failing edge only
	EXTI_CR2 &= 0xCF;
	EXTI_CR2 |= 0x20;

	//PG_CR1_C16 = 1;
	//PG_CR1_C17 = 1;
	//PG_CR1 |= 0xC0;
	// WARNING:    CAN NOT SET PG6 AS PULL UP

	//PG_CR1 |= 0x80;

	//PG_CR2_C26 = 1;
	PG_CR2 |= 0x40;
}

void InitNssIntPort(void)
{
        asm("sim");
	PG_DDR_DDR4 = 0;
	
	//CPU_CCR=0X28;  //等级3
	//EXTI_CONF2 |= 0x40;
	//EXTI_CONF2 |= 0x20;                  // ??  may be error 
	//EXTI_CR4 = 0x02;                // PORTG Failing edge only
	EXTI_CR2 &= 0xFC;
	EXTI_CR2 |= 0x02;

	//PG_CR1_C14 = 1;                 //PG4 not pull up
	//PG_CR2_C24 = 1;
        asm("rim");

}

void SetNssSleepInt(void)
{
        asm("sim");
	PG_DDR_DDR4 = 0;
	
	//CPU_CCR=0X28;  //等级3
	//EXTI_CONF2 |= 0x40;
	//EXTI_CONF2 |= 0x20;                  // ??  may be error 
	//EXTI_CR4 = 0x02;                // PORTG Failing edge only
	EXTI_CR2 &= 0xFC;
	EXTI_CR2 |= 0x01;

	PG_CR1_C14 = 0;                 // pull down
	PG_CR2_C24 = 1;
        asm("rim");
}


void SPI2_Init(void) 
{
/*
    CLK_PCKENR1 |=  ( 1 << 4 ); 
    //PE_DDR = 0;//PE5输入,上拉 NSS 
    PB_DDR &= 0x9F;//5(SCK).6(mosi)输入 7（miso）输出 
    PB_DDR |= 0x80;// 5.6输入 7输出 
    //PB_DDR &= ~0x80;
    PB_CR1 &= 0x9F; 
    PB_CR1 |= 0x80; 
    //PB_ODR |= 0x80;
    SPI1_CR1 = 0x03;//从模式，先发高位，空闲是CLK为高，采样第一个开始，时钟/8 
    SPI1_CR2 = 0x02;//软件从模式 
    //SPI1_ICR = 0X40;//接收完成中断 
    //SPI1_CR1 |= 0x40; 
*/
	CLK_PCKENR3 |= ( 1 << 2 );
  
	PG_DDR &= 0x1F;
	PG_DDR |= 0x60;

	//PG_CR1_C15 = 1;                 //PG5 输出
	//PG_CR2_C25 = 1;
        
        PG_CR1 |= 0x20;
        PG_CR2 |= 0x20;                  //PG5 SCK set 

	//SPI2_CR1 = ~0x40;    //master, CPOL=1, CPHA=1, clk / 256
	//SPI2_CR2 = 0x00;	
	SPI2_CR1 |= ( 7 << 3 );
	SPI2_CR1 |= ( 3 << 0 );
	SPI2_CR1 |= ( 1 << 7 );
	SPI2_CR1 |= ( 1 << 2 );

	//SPI2_CR2 = 0x03;
	SPI2_CR2 |= ( 1 << 0 );       //  SSM: Software slave management
	SPI2_CR2 |= ( 1 << 1 );      //   SSI: Internal slave select

	//SPI2_CR2 |= ( 1 << 7  );      //  BDM: Bidirectional data mode enable

	//PG_DDR_DDR4 = 0;                 // PG4 input, pull up
	//PG_CR1_C14 = 1;                 //can not set as pull up, use PG4 as a start signal

	//EXTI_CR4 = 0x01;                // Failing edge only

	//SPI2_ICR = 0x80;	
        
        //SPI2_CR1 |= ( 1 << 6 );	
}

void NikComInit(void)
{
	SET_SCK_HIGH();
	asm("sim");
	SetSBOInt();
	asm("rim");
	SetNssInt();
}

void ClearComRstTime(void)
{

	Timer4Stop();
	//g_tmHandleFlag ^= TM_HANDLE_COMRST;
	//g_tmHandleFlag^=TM_HANDLE_COMTIMEOUT;
	g_tmHandleFlag &= (~TM_nHANDLE_COMRST);
	//g_tmHandleFlag&=(~TM_HANDLE_COMTIMEOUT);

}

u8 ClkSynRcv(u8* rcvDat)
{
//	u8 tm=0;

	//SPIx_Enable();
	
	//SPI_NIKDAT_IN();
	// d90 should clear the bit, but the D800 D7000 and so on can not
#if 0
	if ( g_isComPullup )
	{
		PG_CR1 &= ~0xC0;                      // 2017/08/03, without set pull down, the leading code will receive error
	}else
	{						 // BUT THE LOGICAL ANALSISY IS APPEARED RIGHT
		PG_CR1 |=  0x40;
	}
#endif

	PG_CR1 |=  0xC0;
								 
	SET_MOSI_IN();
	//SET_MOSI_OUT();
	SET_MISO_IN();

	//PG_CR1 |= 0xC0; 
	//SET_SCK_OUT();
	//SET_RXONLY();
	NikSpiTrans();
	//NikSpiBidirRcv();
	//NikSpiTrans();
	
	g_srFlag = TRANSFER_START;
	g_srMod = SRMOD_R;
	//SPI2_DR = 0xFF;
	SPI2_DR = 0xFF;
	//SPI2_DR = 0xFF;
	//SPI2_DR = 0xFF;
	//SPI2_DR = 0x55;
//	EnableTXEInt();
	NikSpiIntEnable();

/*	
	while(tm++<100)
	{
		if(!g_srFlag)
		{
			return 0;
		}
		OSTimeDly(1);	
	}
*/
	return 0;
}

u8 ClkSynSend(u8 liteCode)
{
	//u8 tm=0;

	//SPIx_Enable();

	
	//PG_CR1 |= 0xC0; 
	//SPI_NIKDAT_OUT();	
	SET_MOSI_OUT();
	//SET_SCK_OUT();
	SET_MISO_IN();
	NikSpiTrans();
	//NikSpiBidirSend();

	//SPI_SS = 0;

	g_srFlag = TRANSFER_START;
	g_srMod = SRMOD_S;
	g_spiBuf = liteCode;
	SPI2_DR = liteCode;
	//EnableTXEInt();
	NikSpiIntEnable();

	//while(!(SPI1->SR&0x02));
	//while((SPI1->SR&0x80));
	//while(!(SPI1->SR&0x01));
	//while(tm++<100)
/*
	while(1)
	{
		if(!g_srFlag)
		{
			//SPI_SS = 1;
			return 0;
		}
		//OSTimeDly(1);	
	}
*/
	//SPI_SS = 1;
	return 0;
}

void ComDataSR(void)
{
	if(SRMOD_S==g_cmdSrFlag)
	{
		ClkSynSend(*(g_pSendRcv+g_srIndex));
	}else
	{
		ClkSynRcv(g_pSendRcv);
	}
	/*
	g_srIndex++;
	if(g_srIndex>=g_srTotal)
	{
		//g_datRcvFlag = 0;
		//g_comStatue = COM_STATUE_IDLE;
		//PULL_MOSI_HIGH();
		//Ex_NVIC_Disable(ID_INT_NSS);
		CameraCmdReply(g_cmdCode);
		//OSTimeDlyResume(COMPROC_TASK_PRIO);
	}else
	{
		g_datRcvFlag = 1;
	}
	*/
}


void ComDataHandleProc(void)
{
	u8 nikCmd;
	
	switch(g_datRcvFlag)
	{
		case 0:
			ClkSynRcv(&nikCmd);                                     //set data pin output or input and spi interrupt
			//g_datRcvFlag = 1;
		break;

		case 1:
		case 2:
			//nikCmd = SPI1->DR;
			//DBG_PRINTF("cmd:%x\r\n",g_spiBuf);
			//CameraCmdHandle(nikCmd);
			//CameraCmdHandle(g_cmdCode);
			ComDataSR();
			//g_datRcvFlag = 2;
		break;

		//case 2:
			//ComDataSR();
		//break;

		//case 3:
		//case 2:
			//ComDataReply();
		//break;
			
	}
}


// nss interrupt, it means should ready for send or receive
#pragma vector= EXTI4_vector
__interrupt void EXTI4_IRQHandler(void)
{
#ifdef SLEEP_DBG
	//printf("fla int\r\n");
#endif

    if(EXTI_SR1 & 0x10) //是否外部中断4 触发
    {		 
        EXTI_SR1 |= 0x10; //清除中断标志
/*
        ClearComRstTime();
	 if (COM_STATUE_START == g_comStatue)
	 {
		if (!NIKONNSSI)
		{
			//TEST_PIN ^= 1;
			ComDataHandleProc();
			//ShutNssInt();
		}
	 }
*/

	if (!FLASH_IN)
	{	
		//TEST_PIN ^= 1;
		FLASHIntProc();
	}
		
#if 1	
	if (!NIKONNSSI)
	{
	
#ifdef SLEEP_DBG
		//if (g_sleepTag == 1)
		//{
		//	printf("SBO before sleep\r\n");
		//	return;
		//}
#endif
		//if (COM_STATUE_START == g_nikComStatue)
		if (COM_STATUE_BUSY == g_nikComStatue)
		{
			ClearComRstTime();
			ComDataHandleProc();
		}
/*
		ClearComRstTime();
		//if (COM_STATUE_START == g_comStatue)
		//if (COM_STATUE_START == g_nikComStatue)
		switch (g_nikComStatue)
	 	{
	 		case COM_STATUE_START:
	 		ComDataHandleProc();
			break;

			case COM_STATUE_SLEEP:
				g_sleepTag = SLEEP_TAG_NIK;
			break;
		}*/
	}else if (COM_STATUE_SLEEP == g_nikComStatue)
	{
		g_sleepTag = SLEEP_TAG_NIK;
		g_nikComStatue = COM_STATUE_IDLE;
		return;
	}
#endif
	
    }
}


#pragma vector= EXTI6_vector
__interrupt void EXTI6_IRQHandler(void)
{
#ifdef SLEEP_DBG
	//printf("sbo int\r\n");
#endif
   	//TEST_PIN ^= 1;
	if(EXTI_SR1 & 0x40) //是否外部中断6触发
	{		
		EXTI_SR1 |= 0x40; //清除中断标志
		//DisableSBOInt();
		SBOintHandle();
	} 
}

void SpiSendIntProc(void)
{
	switch(g_datRcvFlag)
	{
		case 1:
			g_srIndex++;
			if(g_srIndex>=g_srTotal)
			{
				//g_datRcvFlag = 0;
				//g_comStatue = COM_STATUE_IDLE;
				//PULL_MOSI_HIGH();
				//Ex_NVIC_Disable(ID_INT_NSS);
				CameraCmdReply(g_cmdCode);
				//if((0xA0==g_cmdCode)||(0xB0==g_cmdCode))
				//{
				//	SetComWaitTimeout();
				//}
				//OSTimeDlyResume(COMPROC_TASK_PRIO);
			}else
			{
				SetComRstTime();
			}
		break;

		default:
		break;

		
	}
	
}


#pragma vector= I2C_TXE_vector   
 __interrupt void SPI2_RCN( void ) 
{
#if 0
	//if (SPI2_SR & 0x02)             // Tx buffer empty
	if (SPI2_SR & 0x02)             // Tx buffer empty
	{
		NikSpiStop();
		//g_cmdCode = SPI2_DR;
		SpiSendIntProc();
		TEST_PIN1 ^= 1;
	}
#endif

	if (SPI2_SR & 0x02)
	{
		//NikSpiStop();
		//TEST_PIN ^= 1;
		NikSpiIntDisable();
		SpiRcvIntProc();

		//if (COM_STATUE_IDLE == g_comStatue)
		if (COM_STATUE_IDLE == g_nikComStatue)
		{
			SetSBOInt();
		}

		//TEST_PIN2 ^= 1;
		
	}
	//SPI2_DR = g_spiBuf;
       //g_spiBuf ++;
}

static void SBOintHandle(void)
{
	//switch(g_comStatue)
	switch(g_nikComStatue)
	{
		//case COM_STATUE_SLEEP:
			//SleepPortResume(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);
		//case COM_STATUE_IDLE:
		case COM_STATUE_START:                                         // 2017/08/15
			if ((!NIKONMISO)&&(NIKONNSSI>0))
			{
				if ( g_isComStop > 0 )
				{
					g_isComStop = 0;
				}else
				{
					//g_comStatue = COM_STATUE_START;
					g_nikComStatue = COM_STATUE_BUSY;
					PULL_MOSI_LOW();
					DisableSBOInt();
					//SetNssInt();
					OpenNssInt();
					SetComRstTime();
				}
					
			}
		break;

		case COM_STATUE_SLEEP:
                  g_nikComStatue = COM_STATUE_IDLE;
                  g_sleepTag = SLEEP_TAG_NIK;
		break;

		
	}
}

u8 LiteBlockSend(u8 *pLite, u8 size)
{
	g_pSendRcv = pLite;
	g_srTotal = size;
	g_cmdSrFlag = SRMOD_S;
	g_srIndex = 0;

	//SetNssInt();
	//OpenNssInt();
/*
	u8 i=0;

	for(i=0;i<size;i++)
	{		
		if(COM_STATUE_RUN==g_comStatue)
		{
			//SpiInterval(3);
			//OSTimeDly(3);
			//delay_us(20);
			SPI_SS = 0;
			ClkSynSend(*pLite++);
			delay_us(150);
			SPI_SS = 1;
			delay_us(50);
			//g_comStatue = COM_STATUE_START;
		}else
		{
			OSTimeDly(100);	
		}
		//ClkSynSend(*pLite++);
		//OSTimeDly(1);
		//delay_ms(1);
	}
*/

	return 0;

}

u8 CameraBlockRcv(u8 *pRcv, u8 size)
{
	g_pSendRcv = pRcv;
	g_srTotal = size;
	g_cmdSrFlag = SRMOD_R;
	g_srIndex = 0;

	//SetNssInt();
	//OpenNssInt();
/*
	u8 i=0;

	//SPIx_Enable();

	for(i=0;i<size;i++)
	{
		if(COM_STATUE_RUN==g_comStatue)
		{
			//SpiInterval(3);
			delay_us(20);
			ClkSynRcv(pRcv++);
			g_comStatue = COM_STATUE_START;
		}else
		{
			OSTimeDly(1);	
		}
	}

	//SPIx_Disable();
*/
	return 0;

}

void PreFlashSet(void)
{
	//g_comStatue = COM_STATUE_PREFLASH;
	g_nikComStatue = COM_STATUE_PREFLASH;
	ttl_pre2_flag = 0;
	g_flashEnable = 2;
}

void PreFlash2Set(void)
{
	//g_comStatue = COM_STATUE_PREFLASH;
	g_nikComStatue = COM_STATUE_PREFLASH;
	ttl_pre2_flag = 1;
	//OSTaskResume(COMPROC_TASK_PRIO);
	g_flashEnable = 2;
}


void CameraCmdHandle(u8 cmd)
{
	//u8 rts = 0;
	switch(cmd)
	{
		case 0xA1:
			g_cmrLinked = CMR_LINK_NIK;
			LiteBlockSend((u8*)macData1,sizeof(macData1));
		break;

		case 0xA0:
			LiteBlockSend(macData2,sizeof(macData2));
		break;

		case 0xd0:
			CameraBlockRcv(g_spiRcvData, 2);
		break;
						

		case 0xa2:
			LiteBlockSend((u8*)macData3,sizeof(macData3));
		break;


		case 0xb1:
			CameraBlockRcv(g_spiRcvData, 10);
		break;


		case 0xb0:
			CameraBlockRcv(g_spiRcvData,15);
			cmrPara.cls = 1;
			//GetCmrInfo(0xb0);
		break;

		case 0xc0:
			CameraBlockRcv(g_spiRcvData,2);
		break;

		case 0xd7:
			PreFlashSet();
			//MstSetCheckEnable();
		break;


		case 0xe0:				//stop
			CameraBlockRcv(g_spiRcvData,1);
			//g_isComStop = 1;
			//receive_arrary[0]=0xE0;
			//LiteBlockSend(&receive_arrary[0],1);		
			//OSTimeDlyHMSM(0,0,0,20);			
		break;


		case 0xd3:	
			CameraBlockRcv(g_spiRcvData, 2);
		break;
		
		case 0xd1:			//RED EYES;
			CameraBlockRcv(g_spiRcvData,1);
		break;


		case 0xd2:					//especaily for p7100
			CameraBlockRcv(g_spiRcvData, 1);
		break;

		case 0xd9:
			//sendRecord[0] = 0xd9;
			//LiteBlockSend(sendRecord,1);
		break;

		case 0xda:
			//wlSet.m_flaCmd[0] = 0x90;
			//wlSet.m_cmdIndex = 1;
			//MstPreFlaCmd();
		break;
		
		case 0xdb:
			//wlSet.m_flaCmd[0] = 0xa0;
			//wlSet.m_cmdIndex = 1;
			//MstPreFlaCmd();
		break;
		
		case 0xdc:
			//wlSet.m_flaCmd[0] = 0xb0;
			//wlSet.m_cmdIndex = 1;
			//MstPreFlaCmd();
		break;

		case 0xdd:
			//MstCmdPreFlaSignal();
		break;

		case 0xd4:
			CameraBlockRcv(g_spiRcvData, 6);
		break;
		
		
		case 0xd8:
			CameraBlockRcv(g_spiRcvData, 1);
			//PreFlash2Set();
		break;

		case 0xd5:
			//rts = 0xd5;
			//sendRecord[0] = 0xd9;
			//LiteBlockSend(sendRecord, 1);
		break;

		case 0x00:
		break;

		default:
			//SPIx_Rst();
			if ( g_isComPullup )
			{
				g_isComPullup = 0;
			}else
			{
				g_isComPullup = 1;
			}
		break;

	}
}

 char CodeSearch(char *srcArr,char size,const char cd)
{
	char index=0;
	while(index<size)
	{
		if(cd<=srcArr[index])
		{
			return index;
		}else
		{
			index++;
		}
	}
	
	return index;
		
}

 void CmrLedCtrl(void)
{
	if(cmrPara.cls>0)
	{
		if(cmrPara.mode&0x04)
		{
			BkLightOn();
		}else
		{
			BkLightOff();
		}
	}
}


void GetCmrInfo(unsigned char lead)
{
	char i = 0;
//	int tmp = 0;
	switch(lead)
	{
		case 0x8d:
		break;

		case 0xb0:
		if(cmrPara.pApert!=g_spiRcvData[RCVDAT_LOCATION_APERTURE])
		{
			//cmrPara.pApert = receive_arrary[RCVDAT_LOCATION_APERTURE];
			i = CodeSearch((char*)apert_code, sizeof(apert_code), g_spiRcvData[RCVDAT_LOCATION_APERTURE]);
			if(i<sizeof(apert_code))
			{
				//cmrPara.isoIndex = i;
				cmrPara.apert = cmrApertVal[i];
				//cmrPara.apert = i;
				cmrPara.pApert = g_spiRcvData[RCVDAT_LOCATION_APERTURE];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//here info refrash screen
			}
			//DBG_PRINTF("apert:%d\r\n",cmrPara.apert);
		}
		if((cmrPara.pIso!=g_spiRcvData[RCVDAT_LOCATION_ISO])
			||(cmrPara.pIsoBk1!=g_spiRcvData[RCVDAT_LOCATION_ISO_1])
			||(cmrPara.pIsoBk2!=g_spiRcvData[RCVDAT_LOCATION_ISO_2]))
		{
			cmrPara.pIsoBk1 = g_spiRcvData[RCVDAT_LOCATION_ISO_1];
			cmrPara.pIsoBk2 = g_spiRcvData[RCVDAT_LOCATION_ISO_2];
			//cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
			//i = CodeSearch((char*)iso_code, sizeof(iso_code), receive_arrary[RCVDAT_LOCATION_ISO]);
			if(g_spiRcvData[RCVDAT_LOCATION_ISO]<=0x72)
			{
				 if((g_spiRcvData[RCVDAT_LOCATION_ISO]-0x18)&0x01)
				{
					cmrPara.pIso = g_spiRcvData[RCVDAT_LOCATION_ISO];
					cmrPara.iso = cmrIsoValExt[(cmrPara.pIso-0x18)/3];
				}else
				{
					cmrPara.pIso = g_spiRcvData[RCVDAT_LOCATION_ISO];
					cmrPara.iso = cmrIsoVal[(cmrPara.pIso-0x18)/2];
				}				
			}
			if((g_spiRcvData[RCVDAT_LOCATION_ISO]==g_spiRcvData[RCVDAT_LOCATION_ISO_1])
				&&(g_spiRcvData[RCVDAT_LOCATION_ISO]==g_spiRcvData[RCVDAT_LOCATION_ISO_2]))
			{
				cmrPara.isAutoIso = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}else
			{
				cmrPara.isAutoIso = 1;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}

		if(cmrPara.pShutter!=g_spiRcvData[RCVDAT_LOCATION_SHUTTER])
		{
			cmrPara.pShutter = g_spiRcvData[RCVDAT_LOCATION_SHUTTER];
			if(0x2d==cmrPara.pShutter)
			{
				cmrPara.shutter = 5;
			}else if(0x80==cmrPara.pShutter)
			{
				cmrPara.shutter = 60000;
			}else if(cmrPara.pShutter<0x2d)
			{
				cmrPara.shutter = nShutter[((u16)(cmrPara.pShutter+30))/2];
			}else if(cmrPara.pShutter>0xE2)
			{
				cmrPara.shutter = nShutter[(cmrPara.pShutter - 0xE2)/2];
			}else                // 2016/10/12 at d300s, d750, the shutter 250, 320 is at low speed
			{
				cmrPara.shutter = 5; 
			}
			
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//DBG_PRINTF("shutter:%d\r\n",cmrPara.shutter);
		}else if(cmrPara.pShutter == 0x00)
		{
			if(cmrPara.shutter!=1000)
			{
				cmrPara.shutter = 1000;
			}
		}
		

		if(cmrPara.pFocLen!=g_spiRcvData[RCVDAT_LOCATION_FOCLEN])
		{
			//cmrPara.pFocLen = receive_arrary[RCVDAT_LOCATION_FOCLEN];
			i = CodeSearch((char*)focLen_code, sizeof(focLen_code), g_spiRcvData[RCVDAT_LOCATION_FOCLEN]);
			if(i<sizeof(focLen_code))
			{
				cmrPara.pFocLen = g_spiRcvData[RCVDAT_LOCATION_FOCLEN];
				cmrPara.focLen = focLen_val[i];
				
				if(*motoSet.mode==MOTO_MODE_AUTO)
				{
					//motoSet.destRsv = cmrPara.focLen;
					//motoSet.destRsv = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
					//motoSet.dspLocation = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
					motoSet.destRsv = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
					//motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
				}
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//here info refrash screen
			}
			//DBG_PRINTF("foclen:%d\r\n",cmrPara.focLen);
		}

		if(cmrPara.mode!=g_spiRcvData[RCVDAT_LOCATION_MODE])
		{
			cmrPara.mode = g_spiRcvData[RCVDAT_LOCATION_MODE];
			cmrPara.rearCurtain = 1;
			CmrLedCtrl();
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}

		if(cmrPara.bkMod!=g_spiRcvData[RCVDAT_LOCATION_BKMOD])
		{
			cmrPara.bkMod = g_spiRcvData[RCVDAT_LOCATION_BKMOD];
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}

		if(cmrPara.cmpCode!=g_spiRcvData[RCVDAT_LOCATION_CAMERACMP])
		{
			cmrPara.cmpCode = g_spiRcvData[RCVDAT_LOCATION_CAMERACMP];
			//here calc compensating value
			if(!cmrPara.cmpCode)
			{
				cmrPara.camera_cmp1 = 0;
				cmrPara.camera_cmp2 = 0;
			}else if(!(cmrPara.cmpCode&0x80))
			{
				cmrPara.camera_cmp1 = -1 * ((cmrPara.cmpCode / 2) / 3);
				cmrPara.camera_cmp2 = -1 * ((cmrPara.cmpCode / 2) % 3);
			}else{
				//cmrPara.cmpCode &= 0x1f; 
				//cmrPara.cmpCode = 0x20 - cmrPara.cmpCode;
				cmrPara.cmpCode = (0x100 - cmrPara.cmpCode);
				cmrPara.camera_cmp1 = ((cmrPara.cmpCode / 2) / 3);
				cmrPara.camera_cmp2 = ((cmrPara.cmpCode / 2) % 3);
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);

		}

		if(!cmrPara.cls)
		{
			cmrPara.cls = 1;
			g_SettingIndex = 0;
			//*motoSet.mode = MOTO_MODE_AUTO;  
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
		gWSleepCounter = 0;
		cmrPara.linkTmr = 0;    //clear timer
		break;


	/*	case 0xcb:
			flashPara.ExposeData = receive_arrary[RCVDAT_LOCATION_EXPOSE];
			uartSend.debug = flashPara.ExposeData;
			SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_DEBUG);			
		break;
	*/

	}

	//DBG_PRINTF("AIF %d,%d,%d",cmrPara.apert,cmrPara.iso,cmrPara.shutter);

	//for(i=0;i<MAX_RCV_LEN;i++)
	//{
		//receive_arrary[i]=0;
	//}
}


void CameraCmdReply(u8 cmd)
{
	switch(cmd)
	{

		case 0xe0:				//stop
			sendRecord[0] = 0xE0;
			LiteBlockSend(sendRecord,1);	
			g_datRcvFlag = 2;
			g_isComStop = 1;
			g_isAutoStby = 1;
			cmrPara.cls = 0;
			g_cmrLinked = CMR_LINK_NULL;
			//g_dspCmpMod = 0;
			//SetComRstTime();
		break;	

		case 0xd8:
		//case 0xd2:
			g_datRcvFlag = 0;
			//Ex_NVIC_Disable(ID_INT_NSS);
			PreFlash2Set();
			//Timer4Stop();
		break;

		case 0xd2:
			g_datRcvFlag = 0;
			//Ex_NVIC_Disable(ID_INT_NSS);
			//PreFlash3Set();
		break;

		case 0xd3:
			sendRecord[0] = 0xD3;
			LiteBlockSend(sendRecord,1);	
			g_datRcvFlag = 2;
		break;

		case 0xc0:
			if(0x00c1==MAKEWORD(g_spiRcvData[1],g_spiRcvData[0]))
			{
				LiteBlockSend((u8*)macData4,sizeof(macData4));
			}else
			{
				LiteBlockSend((u8*)macData5,sizeof(macData5));
			}
			g_datRcvFlag = 2;
		break;

		case 0xd4:
			sendRecord[0] = 0xd4;
			LiteBlockSend(sendRecord,1);	
			g_datRcvFlag = 2;
		break;

		case 0xd9:     //send master command here
		/*
			if(pSnrSet[SNR_MSTRPT]>0)
			{
				BuildWlmFlaCode();
			}else
			{				
				BuildWlmRptCode();
			}
			MstSetCheckEnable();
			MstCmdPrepear(1);
			g_datRcvFlag = 0;
			//wlSet.m_sendCmdEn = 1;
			*/
		break;

		case 0xb0:
			GetCmrInfo(0xb0);
			/*
			GetCmrInfo(0xb0);
			g_datRcvFlag = 0;
			g_comStatue = COM_STATUE_IDLE;
			Ex_NVIC_Disable(ID_INT_NSS);*/
			g_datRcvFlag = 0;
			//g_comStatue = COM_STATUE_IDLE;
			g_nikComStatue = COM_STATUE_IDLE;
			ShutNssInt();
		break;

		case 0xd0:
/**********************************************************
//           D0 01 FF     all on
//           D0 01 EA    left + buttom
//           D0 00 10    right
//           D0 00 40    buttom
//           D0 00 20    left
***********************************************************/
#if 0
		if(!receive_arrary[0])
		{
			//AF_OFF();
			if(!receive_arrary[1])
			{
				AF_OFF();
				g_isAfOn = 0;
			}else
			{
				if(0x10&receive_arrary[1])
				{
					AF_RIGHT();
				}

				if(0x20&receive_arrary[1])
				{
					AF_LEFT();
				}

				if(0x40&receive_arrary[1])
				{
					AF_BOTTOM();
				}

				if(0x80&receive_arrary[1])
				{
					AF_LEFT();
				}

				if(0x01&receive_arrary[1])
				{
					AF_RIGHT();
				}

				if(0x02&receive_arrary[1])
				{
					AF_LEFT();
				}

				if(0x04&receive_arrary[1])
				{
					AF_BOTTOM();
				}

				if(0x08&receive_arrary[1])
				{
					AF_LEFT();
				}
				
				g_isAfOn = 1;
			}
			
		}else
		{
			if(0xff==receive_arrary[1])
			{
				AF_ALL();
			}else if(0xea==receive_arrary[1])
			{
				AF_LEFTBOTTOM();
			}else
			{
                            AF_RIGHTBOTTOM();
			}
			g_isAfOn = 1;
		}
		g_afOnCnt = 0;
		g_datRcvFlag = 0;
		g_comStatue = COM_STATUE_IDLE;
		//PULL_MOSI_HIGH();
		Ex_NVIC_Disable(ID_INT_NSS);
#endif
		if ((!g_spiRcvData[0])&&(!g_spiRcvData[1]))
		{
			AF_OFF();
			g_isAfOn = 0;
		}else
		{
			if (CFN_ON ==g_cfnOption[CFN_AF_ON])
			{
				AF_ON();
				g_isAfOn = 1;
			}
		}
		g_datRcvFlag = 0;
		//g_comStatue = COM_STATUE_IDLE;
		g_nikComStatue = COM_STATUE_IDLE;
		//PULL_MOSI_HIGH();
		//Ex_NVIC_Disable(ID_INT_NSS);
		break;

		case 0xd1:      //red eyes
#if 0
		if(0x80==receive_arrary[0])   //red eye start
		{
			PreFlash2Set();
			cmrPara.redEyeStart = 1;
			cmrPara.redEyeCnt = 0;
		}else
		{
			cmrPara.redEyeCnt = 0;
			cmrPara.redEyeStart = 0;
			g_comStatue = COM_STATUE_IDLE;
		}
		g_datRcvFlag = 0;
		Ex_NVIC_Disable(ID_INT_NSS);
#endif
		break;

		case 0xa0:
#if 0
		if(cmrPara.redEyeStart>0)
		{
			cmrPara.redEyeCnt++;
			if((cmrPara.redEyeCnt==6)||(cmrPara.redEyeCnt==12))
			{
				PreFlash2Set();
			}else
			{
				g_comStatue = COM_STATUE_IDLE;
			}
			g_datRcvFlag = 0;
			Ex_NVIC_Disable(ID_INT_NSS);
		}else
		{
			g_datRcvFlag = 0;
			g_comStatue = COM_STATUE_IDLE;
			//SetComWaitTimeout();
			//PULL_MOSI_HIGH();
			Ex_NVIC_Disable(ID_INT_NSS);
		}
#endif		
			g_datRcvFlag = 0;
			g_nikComStatue = COM_STATUE_IDLE;
			//g_comStatue = COM_STATUE_IDLE;
		break;

		case 0xd5:
#if 0
			g_datRcvFlag = 0;
			g_comStatue = COM_STATUE_FOCUSDEPTH;
			OSTaskResume(COMPROC_TASK_PRIO);
#endif
		break;

		default:
			g_datRcvFlag = 0;
			//g_comStatue = COM_STATUE_IDLE;
			g_nikComStatue = COM_STATUE_IDLE;
			//SetComWaitTimeout();
			//PULL_MOSI_HIGH();
			ShutNssInt();
		break;

	}
}

static void CameraCmdFlagSet(unsigned char cmd)
{
	switch(cmd)
	{
		case 0xA1:
		case 0xA0:
		case 0xD0:
		case 0xA2:
		case 0xB1:
		case 0xB0:
		case 0xC0:
		case 0xE0:
		case 0xD3:
		case 0xD1:
		case 0xD2:
		case 0xD9:
		case 0xD4:
		case 0xD8:
		case 0xD5:
			g_datRcvFlag = 1;
			g_cmrLinked = CMR_LINK_NIK;
		break;
		
		case 0xd7:
		case 0xda:
		case 0xdb:
		case 0xdc:
		case 0xdd:
			g_datRcvFlag = 0;
			g_cmrLinked = CMR_LINK_NIK;
		break;

		default:
			g_datRcvFlag = 0;
		break;
	}
}

static void CameraCmdRearHandle(unsigned char cmd)
{
	//s8 tmp1=0,tmp2=0;
	
	switch(cmd)
	{
		case 0xd3:
			//g_comStatue = COM_STATUE_REALFLASH;
			g_nikComStatue = COM_STATUE_REALFLASH;
			/* 此处有必要可加入尼康高速同步闪光标志*/
			/*
			if(((g_spiRcvData[0]&0x0f)<=0x0c)&&((g_spiRcvData[0]&0x0f)>=0x09))
			{
				mstPara->hpFlash = HIGH_SPEED;
			}else
			{
				mstPara->hpFlash = LOW_SPEED;
			}*/
			/*
			tmp1 = mstPara->cmp1+cmrPara.camera_cmp1;
			tmp2 = mstPara->cmp2+cmrPara.camera_cmp2;
			if(tmp2>2)
			{
				tmp1 ++;
				tmp2 -= 3;
			}else if(tmp2<-2)
			{
				tmp1--;
				tmp2 += 3;
			}*/
			ttl_expCodeRsv = g_spiRcvData[1];
			//ExposeTimeCal(receive_arrary[1],mstPara->hpFlash,mstPara->cmp1,mstPara->cmp2);
			//NikExposeTimeCal(g_spiRcvData[1],0,tmp1,tmp2);
			SetSBOInt();
		break;

		case 0xd4:
			/*
			if(pSnrSet[SNR_MSTRPT]>0)
			{
				BuildRealFlashCmd(receive_arrary[0],receive_arrary[2],receive_arrary[3],receive_arrary[4],(cmrPara.pShutter>0x36));
			}else
			{
				BuildRptFlashCmd();
			}
			MstCmdPrepear(2);
			*/
		break;

		case 0xE0:
			//g_comStatue = COM_STATUE_IDLE;
			g_nikComStatue = COM_STATUE_IDLE;
			//SET_SCK_LOW();
			//SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		default:
			//g_comStatue = COM_STATUE_IDLE;
			g_nikComStatue = COM_STATUE_IDLE;
		break;
	}
	//Ex_NVIC_Disable(ID_INT_NSS);
	ShutNssInt();
}



void SpiRcvIntProc(void)
{
	switch(g_datRcvFlag)
	{
		case 0:
			g_cmdCode = SPI2_DR;
			g_spiRcvIndex = 0;
			CameraCmdHandle(g_cmdCode);
			//g_datRcvFlag = 1;
			CameraCmdFlagSet(g_cmdCode);
			SetComRstTime();
		break;

		case 1:
			g_spiBuf = SPI2_DR;
			g_spiRcvData[g_spiRcvIndex++] = g_spiBuf;
			g_srIndex++;
			if(g_srIndex>=g_srTotal)
			{
				//g_datRcvFlag = 0;
				//g_comStatue = COM_STATUE_IDLE;
				//PULL_MOSI_HIGH();
				//Ex_NVIC_Disable(ID_INT_NSS);
				CameraCmdReply(g_cmdCode);
				//if((0xA0==g_cmdCode)||(0xB0==g_cmdCode))
				//{
				//	SetComWaitTimeout();
				//}
				//OSTimeDlyResume(COMPROC_TASK_PRIO);
			}else
			{
				SetComRstTime();
			}
		break;

		case 2:
			g_spiBuf = SPI2_DR;
			g_srIndex++;
			if(g_srIndex>=g_srTotal)
			{
				g_datRcvFlag = 0;
				//g_comStatue = COM_STATUE_IDLE;
				CameraCmdRearHandle(g_cmdCode);
				//if(COM_STATUE_IDLE==g_comStatue)
				//{
					//SetComWaitTimeout();
				//}
				//PULL_MOSI_HIGH();
				//Ex_NVIC_Disable(ID_INT_NSS);
			}else
			{
				SetComRstTime();
			}
		break;

		default:
			g_spiBuf = SPI2_DR;
		break;

		
	}
}

char GetIsoStdIndex(const u32* isoArr, char size, u32 cd)
{
	char index=0;
	while(index<size)
	{
		if(cd<=isoArr[index])
		{
			return index;
		}else
		{
			index++;
		}
		
	}

	return index;
}

