#include "spi.h"
#include "../APP/Nikon.h"
#include "led.h"
#include "../TIMER/timer.h"
#include "../APP/KeyProc.h"
 
#define MAX_BUFFER_SIZE    20           // Maximum buffer Master will send
#define SEND_MAX_SIZE 20
#define MAX_RCV_LEN 100

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

#define SRMOD_R    0
#define SRMOD_S    1

//unsigned char g_spiRcvIndex=0;//SPI_Receive_Index;
//unsigned char g_spiSendIndex=0;
unsigned char g_spiRcvData[MAX_BUFFER_SIZE] = {0};
unsigned char g_spiSendData[SEND_MAX_SIZE]={0};
//unsigned char g_cmdMode=CMD_MODE_RF;
Wl_set wlSet={0};
unsigned char g_activeControl=0;
unsigned char g_comStart=0;
//MenuPara menuPara={0};
//RfGroupPara rfGrPara={0,0,0,{0},{0}};
MotoSet motoSet = {0};
unsigned char g_mstPrefla=0;
//unsigned char g_mstMode=0;
unsigned char charge_ok=0;
unsigned char g_getTtlFlashCode=0;
unsigned char ttl_expCodeRsv=0;
extern unsigned char g_comStatue;
//unsigned char Parameter_Array[13]= {0x20,0x5d,0x53,0x13,0x23,0x20,0x00,0x00,0x01,0xa8,0x00,0x00,0x5c};
unsigned char receive_arrary[MAX_RCV_LEN]={0};
u8 g_rcvBufIndex = 0;
unsigned char g_srFlag=0;
unsigned char g_srMod=0;
unsigned char g_spiBuf = 0;
unsigned char g_cmdCode=0;
unsigned char g_datRcvFlag=0;
u8 g_isAfOn = 0;
u8 g_afOnCnt = 0;
unsigned char *g_pSendRcv=0;
unsigned char g_cmdSrFlag=0;
unsigned char g_srTotal=0;
unsigned char g_srIndex=0;
unsigned char g_isComStop = 0;
extern unsigned char g_comAck;
extern char g_keyAdcEnable;
extern u8 g_buzzerOn;
u8 g_flaLockFlag = 0;
u8 g_isHotProtect=0;
u8 g_isAutoStby = 0;
extern u8 g_signalFlash;
extern u32 gWSleepCounter;
//u8 g_flaCmd[8];
//u8 flaCmdCnt = 0;
extern const u16 cmrApertVal[];
//extern const u16 cmrIsoVal[];
extern const u32 cmrIsoVal[];
//u8 g_flashCodeRsv = 0;
u8 g_isFlashWeak = 0;
u8 g_FlashWeakLevels = 0;
//extern OS_TMR	*g_slvBzTmr;

CmrPara cmrPara={0};

unsigned char macData1[] = {
0x01,0x01,0x05,0x02,0x08,0x07,0x81,0x78,0xb4,0x64,0x0a,0x4e,
0xff,0x3e,0x90,0x3e,0x90,0x1d
};


unsigned char macData2[] = {
0x01,0x02,0xb6,0x04,0x00,0x00,0x88,0x54,0x30,0x23,0x00,0xff,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xeb,
};

/*
unsigned char macData2[] = {
0x01,0x81,0xb6,0x04,0x00,0xf8,0x88,0x54,0x30,0x24,0x00,0x00,
0xff,0x00,0x00,0x00,0x01,0x11,0x00,0xee,0x00,0x64,
};//master
*/

unsigned char  macData3[] = {
0x03,0x05,0x6d,0x9b,0x90,0x86,0x80,0x7a,0x72,0x6a,0x65,0x5e,
0x06,0x75,0x9e,0x94,0x8a,0x80,0x77,0x6f,0x66,0x60,0x58,0x06,
0x74,0x9e,0x95,0x8b,0x80,0x76,0x6f,0x66,0x60,0x59,0x04,0x5f,
0x96,0x8a,0x82,0x80,0x7e,0x79,0x71,0x6e,0x6a,0xfc,
};

unsigned char macData4[] = 
{
0x0a,0x5e,0x69,
};

unsigned char macData5[] = 
{
0x0a,0x05,0x10,
};

unsigned char sendRecord[3]={0};

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

unsigned char const apert_code[] = {
0x00,0x02,0x04,0x06,0x08,
0x0a,0x0c,0x0e,0x0f,0x10,0x12,0x14,
0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,
0x20,0x21,0x22,0x24,0x26,0x27,0x28,0x2a,0x2c,0x2d,0x2e,
0x30,0x32,0x33,0x34,0x36,0x38,0x39,0x3a,0x3c,0x3d,0x3e,
};

unsigned char const iso_code[] = {
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

unsigned int const nShutter[39] = 
{
/*	5,6,8,10,12,16,20,25,33,40,50,66,76,100,125,160,200,250,333,400,500,625,769,1000,1300,
	1600,2000,2500,3000,4000,5000,6000,8000,10000,13000,15000,20000,25000,30000,*/

	30000,25000,20000,15000,13000,10000,8000,6000,5000,4000,3000,2500,2000,1600,1300,1000,
	769,625,500,400,333,250,200,160,125,100,76,66,50,40,33,25,20,16,12,10,8,6,5
};

unsigned char const focLen_code[] ={
0x2d,0x30,0x31,0x32,0x33,0x34,0x36,0x37,0x38,0x39,0x3a,0x3c,0x3d,0x3e,0x40,0x41,0x42,0x43,0x44,0x45,0x46,
0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x55,0x56,
0x57,0x59,0x5a,0x5c,0x5e,0x60,0x62,0x64,0x65,0x66,0x67,0x6a,0x6c,0x6e,0x71,0x73,0x76,0x7a,0x80,/*0x8f,*/
};

unsigned char const focLen_val[] = {
18,20,21,21,22,23,24,25,25,26,27,28,29,30,32,33,33,34,35,36,38,
38,40,41,42,44,45,46,48,49,50,52,52,55,58,60,
//62,65,66,70,75,80,85,90,92,95,98,105,112,120,130,135,150,170,180,40,
62,65,66,70,75,80,85,90,92,95,98,105,112,120,130,135,150,170,200,/*40,*/
};

unsigned char const output_calc[] = {
0X2A,0X28,0X26,0X24,0X22,0X20,0X1E,0X1C,0X1A,0X18,0X16,
0X14,0X12,0X10,0X0E,0X0C,0X0A,0X08,0X06,0X04,0X02,0X00,
};

unsigned char const cmp_calc[] = {
0x12,0x10,0x0E,0X0C,0X0A,0X08,0X06,0X04,0X02,
0X00,0XFE,0XFC,0XFA,0XF8,0XF6,0XF4,0XF2,0XF0,
0XEE,
};

unsigned char abRation_fill1,abRation_fill2;
#define FLASH_OUT PAout(6)

#ifdef _MULTIPLY_FLASH_
//MultiSet multiSet;
#endif

//void CodeAnalysis(unsigned char rcvCode);
//void FlashHandle(void);
//void GetParameter(void);
//void BuildExposeCmd(char mode,unsigned char hpSyn);
u8 ClkSynSend(u8 liteCode);
void SetNssInt(void);

#define EnableTXEInt()    {SPI1->CR2 |= (1<<7);}
#define DisableTXEInt()    {SPI1->CR2 &= ~(1<<7);}

#define EnableRXNEInt()    {SPI1->CR2 |= 1<<6;}

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
	RCC->APB2ENR|=1<<2;
	RCC->APB2ENR|=1<<12;      //SPI1Ê±ÖÓÊ¹ÄÜ 
	
	GPIOA->CRL&=0x00000FFF; 
	GPIOA->CRL|=0xA8A83000;//PA5.6.7¸´ÓÃ 	    
//	GPIOA->ODR|=0x07<<5;    //PA5.6.7ÉÏÀ
	RCC->APB2RSTR|=(1<<12);
	RCC->APB2RSTR&=~(1<<12);
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

	SPI1->CR1 |= (7<<3);				//1      /256 Fcpu
	//SPI1->CR1 |= (1<<15);			//single line, dublex direction
	SPI1->CR1 |= (1<<14);			//direction in

	SPI1->CR1 &= ~(1<<11);		//8bit
	SPI1->CR1 |= (1<<0);		//CPHA = 1;
	SPI1->CR1 |= (1<<1);
	//SPI1->CR1 &=~(1<<7);
	SPI1->CR1 |= (1<<7); 	//Little endien
	SPI1->CR1 |= (1<<9);
	SPI1->CR1 |=(1<<8);

	SPI1->CR1 |= (1<<2);		//master mode

	//set spi interrupt
	SPI1->CR2 |= 1<<6;		//Enable RXNE interrupt
	//MY_NVIC_Init(0,0,SPI1_IRQChannel,1);
	//SPI1->CR2 |= (1<<7);		//Enable TXE interrupt
	//MY_NVIC_Init(2,3,SPI1_IRQChannel,1);
	MY_NVIC_Init(2,3,SPI1_IRQn,1);

	
	SPI1->CR1 |= (1<<6);
	//SPI1->CR1&=~(1<<6);	//disable spi at first

	
	
	 //SPI_MISO_AUX=1;

	//SPIx_ReadWriteByte(0xEA);//Æô¶¯´«Êä		 
	//ClkSynSend(0xEA);


}

void Delay()
{
	unsigned int i=0;
	for(i=0;i<200;i++);
}


void SPIx_Enable(void)
{
	//RCC->APB2RSTR|=(1<<12);
	//RCC->APB2RSTR&=~(1<<12);
	SPI1->CR1|=(1<<6);
	//Delay();
}

void SPIx_Disable(void)
{
	SPI1->CR1&=~(1<<6);
}

void SPIx_Rst(void)
{
	RCC->APB2RSTR|=(1<<12);
	RCC->APB2RSTR&=~(1<<12);
}

/*
u8 SPIx_ReceiveByte(void)
{
	SPI_NIKDAT_IN();	
	//SET_MOSI_IN;
	SPI1->DR = 0x00;
	while(!(SPI1->SR&1<<0)) ;
	SPI_NIKDAT_OUT();
	return SPI1->DR; 
	
}
*/


//#ifdef _SPI_MODULE_
#if 1
u8 ClkSynSend(u8 liteCode)
{
	//u8 tm=0;

	//SPIx_Enable();
	
	//SPI_NIKDAT_OUT();	
	SET_MOSI_OUT();
	SET_SCK_OUT();
	SET_MISO_IN();

	//SPI_SS = 0;

	g_srFlag = TRANSFER_START;
	g_srMod = SRMOD_S;
	g_spiBuf = liteCode;
	SPI1->DR = liteCode;
	//EnableTXEInt();

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
#else

void SpiInterval(unsigned int cnt)
{	
	unsigned int count;
	unsigned int i;

	for(i=0;i<cnt;i++)
	for (count = 100; count > 0; count--);

}
u8 ClkSynSend(u8 liteCode)
{
	u8 i=0;
	
	SET_MOSI_OUT();
	
	for(i=0;i<8;i++)
	{
		NIKONSCK = 0;
		NIKONMOSI = (liteCode & 0x01);
		liteCode =  liteCode >>1;
		SpiInterval(1);
		NIKONSCK = 1;
		SpiInterval(1);		
	}

	return 0;
	
}
#endif

//#ifdef _SPI_MODULE_
#if 1
u8 ClkSynRcv(u8* rcvDat)
{
//	u8 tm=0;

	//SPIx_Enable();
	
	//SPI_NIKDAT_IN();
	SET_MOSI_IN();
	SET_MISO_IN();
	SET_SCK_OUT();
	//SET_RXONLY();
	
	g_srFlag = TRANSFER_START;
	g_srMod = SRMOD_R;
	SPI1->DR =0xFF;
//	EnableTXEInt();

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
#else
u8 ClkSynRcv(u8* rcvDat)
{
	u8 i=0;
	
	*rcvDat = 0;
	SET_MOSI_IN();
	
	for(i=0;i<8;i++)
	{
		NIKONSCK = 0;
		SpiInterval(1);
		NIKONSCK = 1;
		*rcvDat |= (NIKONMISO<<i);
		SpiInterval(1);	
	}

	return 0;

}
#endif


void GetCmrInfo(unsigned char lead)
{
	char i = 0;
//	int tmp = 0;
	switch(lead)
	{
		case 0x8d:
		break;

		case 0xb0:
		if(cmrPara.pApert!=receive_arrary[RCVDAT_LOCATION_APERTURE])
		{
			//cmrPara.pApert = receive_arrary[RCVDAT_LOCATION_APERTURE];
			i = CodeSearch((char*)apert_code, sizeof(apert_code), receive_arrary[RCVDAT_LOCATION_APERTURE]);
			if(i<sizeof(apert_code))
			{
				//cmrPara.isoIndex = i;
				cmrPara.apert = cmrApertVal[i];
				//cmrPara.apert = i;
				cmrPara.pApert = receive_arrary[RCVDAT_LOCATION_APERTURE];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//here info refrash screen
			}
			DBG_PRINTF("apert:%d\r\n",cmrPara.apert);
		}
		if((cmrPara.pIso!=receive_arrary[RCVDAT_LOCATION_ISO])
			||(cmrPara.pIsoBk1!=receive_arrary[RCVDAT_LOCATION_ISO_1])
			||(cmrPara.pIsoBk2!=receive_arrary[RCVDAT_LOCATION_ISO_2]))
		{
			cmrPara.pIsoBk1 = receive_arrary[RCVDAT_LOCATION_ISO_1];
			cmrPara.pIsoBk2 = receive_arrary[RCVDAT_LOCATION_ISO_2];
			//cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
			//i = CodeSearch((char*)iso_code, sizeof(iso_code), receive_arrary[RCVDAT_LOCATION_ISO]);
			if(receive_arrary[RCVDAT_LOCATION_ISO]<=0x72)
			{
				 if((receive_arrary[RCVDAT_LOCATION_ISO]-0x18)&0x01)
				{
					cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
					cmrPara.iso = cmrIsoValExt[(cmrPara.pIso-0x18)/3];
				}else
				{
					cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
					cmrPara.iso = cmrIsoVal[(cmrPara.pIso-0x18)/2];
				}				
			}
			if((receive_arrary[RCVDAT_LOCATION_ISO]==receive_arrary[RCVDAT_LOCATION_ISO_1])
				&&(receive_arrary[RCVDAT_LOCATION_ISO]==receive_arrary[RCVDAT_LOCATION_ISO_2]))
			{/*
				if(receive_arrary[RCVDAT_LOCATION_ISO]<=0x72)
				{
					 if((receive_arrary[RCVDAT_LOCATION_ISO]-0x18)&0x01)
					{
						cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
						cmrPara.iso = cmrIsoValExt[(cmrPara.pIso-0x18)/3];
					}else
					{
						cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
						cmrPara.iso = cmrIsoVal[(cmrPara.pIso-0x18)/2];
					}

					cmrPara.isAutoIso = 0;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					
				}
			*/
				cmrPara.isAutoIso = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}else
			{
				cmrPara.isAutoIso = 1;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}

			/*
			if(i<sizeof(iso_code))
			{
				//cmrPara.isoIndex = i;
				cmrPara.iso = cmrIsoVal[i];
				cmrPara.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//here info refrash screen
			}*/
			DBG_PRINTF("iso:%d\r\n",cmrPara.iso);
		}

		if(cmrPara.pShutter!=receive_arrary[RCVDAT_LOCATION_SHUTTER])
		{
			cmrPara.pShutter = receive_arrary[RCVDAT_LOCATION_SHUTTER];
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
			/*
			if((1000/(*rptSet.frequence)+1)>(cmrPara.shutter/(*rptSet.eTimes)))
			{
				rptSet.finalTimesBk = cmrPara.shutter*(*rptSet.frequence)/1000;
			}else
			{
				rptSet.finalTimesBk = *rptSet.eTimes;
			}*/
			
			//rptSet.finalExpTimes = 1;
			/*
			if((unsigned long)((1000/(*rptSet.frequence))*(*rptSet.eTimes))<(unsigned long)cmrPara.shutter)
			{
				rptSet.finalExpTimes = *rptSet.eTimes;
				//putchar(multiSet.finalExpTimes);
			}else
			{
				if((((unsigned long)cmrPara.shutter*(unsigned long)(*rptSet.frequence))/1000))
				{
					rptSet.finalExpTimes = (int)((((unsigned long)cmrPara.shutter*(unsigned long)(*rptSet.frequence))/1000));
				}else
				{
					rptSet.finalExpTimes = (int)(((((unsigned long)cmrPara.shutter*(unsigned long)(*rptSet.frequence)))/1000)-1);
				}
			}*/
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			DBG_PRINTF("shutter:%d\r\n",cmrPara.shutter);
		}else if(cmrPara.pShutter == 0x00)
		{
			if(cmrPara.shutter!=1000)
			{
				cmrPara.shutter = 1000;
			}
		}
		

		if(cmrPara.pFocLen!=receive_arrary[RCVDAT_LOCATION_FOCLEN])
		{
			//cmrPara.pFocLen = receive_arrary[RCVDAT_LOCATION_FOCLEN];
			i = CodeSearch((char*)focLen_code, sizeof(focLen_code), receive_arrary[RCVDAT_LOCATION_FOCLEN]);
			if(i<sizeof(focLen_code))
			{
				cmrPara.pFocLen = receive_arrary[RCVDAT_LOCATION_FOCLEN];
				cmrPara.focLen = focLen_val[i];
				
				if(*motoSet.mode==MOTO_MODE_AUTO)
				{
					//motoSet.destRsv = cmrPara.focLen;
					//motoSet.destRsv = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
					motoSet.dspLocation = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
					motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
				}
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//here info refrash screen
			}
			DBG_PRINTF("foclen:%d\r\n",cmrPara.focLen);
		}

		if(cmrPara.mode!=receive_arrary[RCVDAT_LOCATION_MODE])
		{
			cmrPara.mode = receive_arrary[RCVDAT_LOCATION_MODE];
			cmrPara.rearCurtain = 1;
			CmrLedCtrl();
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}

		if(cmrPara.bkMod!=receive_arrary[RCVDAT_LOCATION_BKMOD])
		{
			cmrPara.bkMod = receive_arrary[RCVDAT_LOCATION_BKMOD];
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}

		if(cmrPara.cmpCode!=receive_arrary[RCVDAT_LOCATION_CAMERACMP])
		{
			cmrPara.cmpCode = receive_arrary[RCVDAT_LOCATION_CAMERACMP];
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
			if((SETTING_PFNMENU!=g_SettingIndex)&&(SETTING_SNRSUB!=g_SettingIndex))
			{
				g_SettingIndex = 0;
			}
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


//#ifdef _SPI_MODULE_
#if 1
u8 LiteBlockSend(u8 *pLite, u8 size)
{
	g_pSendRcv = pLite;
	g_srTotal = size;
	g_cmdSrFlag = SRMOD_S;
	g_srIndex = 0;
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
#else

u8 LiteBlockSend(u8 *pLite, u8 size)
{
	u8 i=0;
	u8 tm=0;

	//SET_MOSI_OUT;

	//Ex_NVIC_Enable(ID_INT_NSS);
	SetNssInt();
	
	for(i=0;i<size;i++)
	{
		tm = 0;
		while(tm++<SPI_TIMEOUT)
		{
			if(COM_STATUE_RUN==g_comStatue)
			{
				ClkSynSend(*pLite++);
				g_comStatue = COM_STATUE_START;	
				break;
				
			}else
			{
				//OSTimeDly(1);
				OSTaskSuspend(COMPROC_TASK_PRIO);
			}
		}
		if(tm>=SPI_TIMEOUT)
		{
			return 1;
		}
	}

	Ex_NVIC_Disable(ID_INT_NSS);

	return 0;
}

u8 CameraBlockRcv(u8 *pRcv, u8 size)
{
	u8 i=0;
	u8 tm = 0;

	//SET_MOSI_IN;
	
	//Ex_NVIC_Enable(ID_INT_NSS);
	SetNssInt();

	for(i=0;i<size;i++)
	{
		tm = 0;
		while(tm++<SPI_TIMEOUT)
		{
			if(COM_STATUE_RUN==g_comStatue)
			{
				ClkSynRcv(pRcv++);
				g_comStatue = COM_STATUE_START;
				break;
			}else
			{
				//OSTimeDly(1);
				OSTaskSuspend(COMPROC_TASK_PRIO);
			}
		}
		if(tm>=SPI_TIMEOUT)
		{
			//TEST_SIGNAL = !TEST_SIGNAL;
			return 1;			
		}
	}

	//TEST_SIGNAL = !TEST_SIGNAL;
	Ex_NVIC_Disable(ID_INT_NSS);
	
	return 0;
	
}

#endif

void ComSpiDataClear(void)
{
	//#define WLM_PARA_CLR {MAINPARA_ARRARY[WLM_LOC_MASTSET]=0;MAINPARA_ARRARY[WLM_LOC_MASTPAR]=0;MAINPARA_ARRARY[WLM_LOC_MSL_A]=0;MAINPARA_ARRARY[WLM_LOC_MSL_BC]=0;MAINPARA_ARRARY[WLM_LOC_SLPARA_A]=0;MAINPARA_ARRARY[WLM_LOC_SLPARA_B]=0;MAINPARA_ARRARY[WLM_LOC_SLPARA_C]=0;}
	macData2[COM_DAT_MODE]=0;
	macData2[COM_DAT_MASTPAR]=0;
	macData2[COM_DAT_MSL_A]=0;
	macData2[COM_DAT_MSL_BC]=0;
	macData2[COM_DAT_SLPARA_A]=0;
	macData2[COM_DAT_SLPARA_B]=0;
	macData2[COM_DAT_SLPARA_C]=0;
	
}

unsigned char VerCodeCal(const unsigned char *arr)
{
	unsigned char i=0;
	unsigned char verRear=0;

	for (i=0;i<VERI_DATA_LEN;i++) {
		verRear += arr[i];
	}
	
	return verRear+1;
	
}

//void SetSpiComModeOff(u8 mod)
void SetSpiComModeOff(u8 mod, u8 aSel)
{
	ComSpiDataClear();
	
	//switch(mstPara->mode)
	switch(mod)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_TTL_POOR:
			macData2[COM_DAT_MODE] = 0x02;
			macData2[COM_DAT_STATUE] &= ~0x40;
			macData2[COM_DAT_MASTPAR]=cmp_calc[(mstPara->cmp1+3)*3+mstPara->cmp2];
		break;

		case WORKINGMODE_AUTO:
			//macData2[COM_DAT_MODE] = 0x03;
			macData2[COM_DAT_MASTPAR]=cmp_calc[(mstPara->a_cmp1+3)*3+mstPara->a_cmp2];
			macData2[COM_DAT_STATUE] &= ~0x08;
			//switch(pSnrSet[SNR_APERT])
			switch(aSel)
			{
				case 0:
					macData2[COM_DAT_MODE] = 0x03;
					macData2[COM_DAT_STATUE] &= ~0x40;
					macData2[COM_DAT_STATUE] |= 0x04;
				break;

				case 1:
					macData2[COM_DAT_MODE] = 0x03;
					macData2[COM_DAT_STATUE] |= 0x44;
					//macData2[COM_DAT_STATUE] |= 0x04;
				break;

				case 2:
					macData2[COM_DAT_MODE] = 0x04;
					//macData2[COM_DAT_STATUE] &= ~0x40;
					macData2[COM_DAT_STATUE] &= ~0x40;
					macData2[COM_DAT_STATUE] |= 0x04;
				break;

				case 3:
					macData2[COM_DAT_MODE] = 0x04;
					//macData2[COM_DAT_STATUE] |= 0x40;
					macData2[COM_DAT_STATUE] |= 0x44;
				break;
			}
		break;

		case WORKINGMODE_GN:
			macData2[COM_DAT_MODE] = 0x05;
			macData2[COM_DAT_MASTPAR]=cmp_calc[(mstPara->gn_cmp1+3)*3+mstPara->gn_cmp2];
		break;

		case WORKINGMODE_MANU:
			macData2[COM_DAT_MODE] = 0x06;
			macData2[COM_DAT_MASTPAR]=output_calc[(mstPara->output1)*3+mstPara->output2];
		break;

		case WORKINGMODE_RPT:
			macData2[COM_DAT_MODE] = 0x07;
			macData2[COM_DAT_MASTPAR]=output_calc[(mstPara->multi_exp1)*3+mstPara->multi_exp2];
			macData2[COM_DAT_RPTTIMES] = mstPara->rpt_times;
			macData2[COM_DAT_RPTFRQ] = mstPara->rpt_hz;
		break;
	}

	//macData2[COM_DAT_REAR] = VerCodeCal(macData2);
}

u8 SpiComModeCode(s8 mod)
{
	switch(mod)
	{
		case WORKINGMODE_TTL:			
		return 0x01;

		case WORKINGMODE_AUTO:
		//return 0x03;
		return 0x01;                       //when slave array set at A mode, the preflash info cause unkown error

		case WORKINGMODE_MANU:
		return 0x06;

		case WORKINGMODE_RPT:
		return 0x07;

		default:
		return 0x00;
	}
}

u8 SpiComParaCode(u8 id)
{
	switch(mainPara.modGr[id].mode)
	{
		case WORKINGMODE_TTL:
			return cmp_calc[(mainPara.modGr[id].cmp1+3)*3+mainPara.modGr[id].cmp2];
		//break;

		case WORKINGMODE_AUTO:
		break;

		case WORKINGMODE_MANU:
			return output_calc[mainPara.modGr[id].output1*3+mainPara.modGr[id].output2];
		//break;

		default:
		break;
	}

	return NULL;
}

void SetSpiComModeMstSu4(void)
{
	ComSpiDataClear();
	
	switch(mstPara->mode)
	{

		case WORKINGMODE_AUTO:
			//macData2[COM_DAT_MODE] = 0x03;
			macData2[COM_DAT_MASTPAR]=cmp_calc[(mstPara->a_cmp1+3)*3+mstPara->a_cmp2];;
			macData2[COM_DAT_MODE] = 0x03;
			macData2[COM_DAT_STATUE] = 0x4C;
		break;

		case WORKINGMODE_GN:
			macData2[COM_DAT_MODE] = 0x05;
			macData2[COM_DAT_MASTPAR]=cmp_calc[(mstPara->gn_cmp1+3)*3+mstPara->gn_cmp2];
		break;

		case WORKINGMODE_MANU:
			macData2[COM_DAT_MODE] = 0x06;
			macData2[COM_DAT_MASTPAR]=output_calc[(mstPara->output1)*3+mstPara->output2];
		break;

		default:
			macData2[COM_DAT_MODE] = 0x06;
			macData2[COM_DAT_MASTPAR]=output_calc[(mstPara->output1)*3+mstPara->output2];
		break;
	}

}

void SetSpiComModeMst(void)
{
	if((WORKINGMODE_OFF==mainPara.modGr[1].mode)&&(WORKINGMODE_OFF==mainPara.modGr[2].mode)
		&&(WORKINGMODE_OFF==mainPara.modGr[3].mode))
	{
		SetSpiComModeOff(mainPara.modGr[0].mode,0);
	}else
	{
		ComSpiDataClear();
		macData2[COM_DAT_MODE] = 0x80 | SpiComModeCode(mainPara.modGr[0].mode);
		macData2[COM_DAT_MASTPAR] = SpiComParaCode(0);
		macData2[COM_DAT_MSL_A] = SpiComModeCode(mainPara.modGr[1].mode);
		macData2[COM_DAT_MSL_BC] = (SpiComModeCode(mainPara.modGr[2].mode)<<4|SpiComModeCode(mainPara.modGr[3].mode));
		/*if(WORKINGMODE_AUTO==mainPara.modGr[3].mode)
		{
			macData2[COM_DAT_MSL_BC] = (SpiComModeCode(mainPara.modGr[2].mode)<<4|SpiComModeCode(WORKINGMODE_TTL));
		}else
		{
			macData2[COM_DAT_MSL_BC] = (SpiComModeCode(mainPara.modGr[2].mode)<<4|SpiComModeCode(mainPara.modGr[3].mode));
		}*/
		macData2[COM_DAT_SLPARA_A] = SpiComParaCode(1);
		macData2[COM_DAT_SLPARA_B] = SpiComParaCode(2);
		macData2[COM_DAT_SLPARA_C] = SpiComParaCode(3);
	}
	//macData2[COM_DAT_REAR] = VerCodeCal(macData2);
}

u8 GetMstRptModCode(u8 i)
{
	if(mainPara.modGr[i].rpt_isOn>0)
	{
		return 0x07;
	}else
	{
		return 0x00;
	}
}

u8 GetMstRptParaCode(u8 i)
{
	if(mainPara.modGr[i].rpt_isOn>0)
	{
		return output_calc[mstPara->multi_exp1*3+mstPara->multi_exp2];
	}else
	{
		return 0x00;
	}
}

void SetSpiComMstRpt(void)
{
	ComSpiDataClear();
	macData2[COM_DAT_MODE] = 0x80 | GetMstRptModCode(0);
	macData2[COM_DAT_MASTPAR] = GetMstRptParaCode(0);
	macData2[COM_DAT_MSL_A] = GetMstRptModCode(1);
	macData2[COM_DAT_MSL_BC] = (GetMstRptModCode(2)<<4|GetMstRptModCode(3));
	macData2[COM_DAT_SLPARA_A] = GetMstRptParaCode(1);
	macData2[COM_DAT_SLPARA_B] = GetMstRptParaCode(2);
	macData2[COM_DAT_SLPARA_C] = GetMstRptParaCode(3);
}

void SetSpiComData(void)
{
	if(pSnrSet[SNR_AF]<2)
	{
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
				SetSpiComModeOff(mstPara->mode,pSnrSet[SNR_APERT]);
			break;

			case CMD_MODE_LIGH_MST:
				if(pSnrSet[SNR_SU4]>0)
				{
					SetSpiComModeMstSu4();
				}else
				{
					if(pSnrSet[SNR_MSTRPT]>0)
					{
						SetSpiComModeMst();
					}else
					{
						SetSpiComMstRpt();
					}
				}
			break;
		}
		if(!pSnrSet[SNR_AF])                           //AF ON
		{
			//macData2[COM_DAT_STATUE] = 0x04;
			macData2[COM_DAT_STATUE] &= ~0x20;
		}else                                                  //AF OFF
		{
			macData2[COM_DAT_STATUE] |= 0x20;
		}
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}else
	{
		ComSpiDataClear();
		macData2[COM_DAT_MODE] = 0x00;
		macData2[COM_DAT_MASTPAR]=0x00;
		//macData2[COM_DAT_STATUE] = 0x04;
		macData2[COM_DAT_STATUE] &= ~0x20;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}
	
}

void SetSBOInt(void)
{
	SET_MOSI_IN();
	Ex_NVIC_Config(GPIO_A,ID_INT_SBO,FTIR);
	//MY_NVIC_Init(2,1,EXTI9_5_IRQChannel,2);	
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);	
}

void SetNssInt(void)
{
	SET_NSS_IN();
	Ex_NVIC_Config(GPIO_A,ID_INT_NSS,FTIR);
	//MY_NVIC_Init(2,2,EXTI2_IRQChannel,2);
	MY_NVIC_Init(2,2,EXTI2_IRQn,2);
}

void SetComRstTime(void)
{
	Timer4_Init(1000,72);	
	g_tmHandleFlag |= TM_HANDLE_COMRST;
	g_tmHandleFlag&=(~TM_HANDLE_COMTIMEOUT);
}

/*
void SetComWaitTimeout(void)
{
	Timer4_Init(1000,7200);	
	g_tmHandleFlag |= TM_HANDLE_COMTIMEOUT;
	g_tmHandleFlag &= (~TM_HANDLE_COMRST);
}*/

void ClearComWaitTimeout(void)
{
 	g_tmHandleFlag &= (~TM_HANDLE_COMTIMEOUT);
}

void ClearComRstTime(void)
{

	Timer4_Stop();
	//g_tmHandleFlag ^= TM_HANDLE_COMRST;
	//g_tmHandleFlag^=TM_HANDLE_COMTIMEOUT;
	g_tmHandleFlag &= (~TM_HANDLE_COMRST);
	//g_tmHandleFlag&=(~TM_HANDLE_COMTIMEOUT);

}

void RstComPara(void)
{
	g_datRcvFlag = 0;
	g_srIndex = 0;
	g_srTotal = 0;
	g_comStatue = COM_STATUE_IDLE;
}

void SetSBOPullup(void)
{
	SET_MOSI_OUT();
	NIKONMOSI = 1;
}

void PreFlashSet(void)
{
	g_comStatue = COM_STATUE_PREFLASH;
	ttl_pre2_flag = 0;
	OSTaskResume(COMPROC_TASK_PRIO);
}

void PreFlash2Set(void)
{
	g_comStatue = COM_STATUE_PREFLASH2;
	ttl_pre2_flag = 1;
	OSTaskResume(COMPROC_TASK_PRIO);
}

void PreFlash3Set(void)
{
	g_comStatue = COM_STATUE_PREFLASH3;
	OSTaskResume(COMPROC_TASK_PRIO);
}

void MstSetCheckEnable()
{
	if(!g_mstEnFlag)
	{
		if(g_chargeOk>0)
		{
			g_mstFlaEnable = 1;	
		}else
		{
			g_mstFlaEnable = 0;
		}
		g_mstEnFlag = 1;
	}
}

void MstCmdPrepear(u8 ack)
{
	g_comStatue = COM_STATUE_MSTCMD;
	if(1==ack)
	{
		g_comAck = 1;
	}else if(2==ack)
	{
		g_comAck = 2;
	}else
	{
		g_comAck = 0;
	}
	OSTaskResume(COMPROC_TASK_PRIO);
}

void MstPreFlaCmd(void)
{
	g_comStatue = COM_STATUE_MSTPRE;
	OSTaskResume(COMPROC_TASK_PRIO);
}

void MstCmdPreFlaSignal(void)
{
	g_comStatue = COM_STATUE_PRESIGNAL;
	OSTaskResume(COMPROC_TASK_PRIO);
}

void CameraCmdHandle(unsigned char cmd)
{
	//u8 rts = 0;
	switch(cmd)
	{
		case 0xA1:
			LiteBlockSend(macData1,sizeof(macData1));
		break;

		case 0xA0:
			LiteBlockSend(macData2,sizeof(macData2));
		break;

		case 0xd0:
			CameraBlockRcv(receive_arrary, 2);
		break;
						

		case 0xa2:
			LiteBlockSend(macData3,sizeof(macData3));
		break;


		case 0xb1:
			CameraBlockRcv(receive_arrary, 10);
		break;


		case 0xb0:
			CameraBlockRcv(receive_arrary,15);
			//GetCmrInfo(0xb0);
		break;

		case 0xc0:
			CameraBlockRcv(receive_arrary,2);
		break;

		case 0xd7:
			PreFlashSet();
			MstSetCheckEnable();
		break;


		case 0xe0:				//stop
			CameraBlockRcv(receive_arrary,1);
			//g_isComStop = 1;
			//receive_arrary[0]=0xE0;
			//LiteBlockSend(&receive_arrary[0],1);		
			//OSTimeDlyHMSM(0,0,0,20);			
		break;


		case 0xd3:	
			CameraBlockRcv(receive_arrary, 2);
		break;
		
		case 0xd1:			//RED EYES;
			CameraBlockRcv(receive_arrary,1);
		break;


		case 0xd2:					//especaily for p7100
			CameraBlockRcv(receive_arrary, 1);
		break;

		case 0xd9:
			sendRecord[0] = 0xd9;
			LiteBlockSend(sendRecord,1);
		break;

		case 0xda:
			wlSet.m_flaCmd[0] = 0x90;
			wlSet.m_cmdIndex = 1;
			MstPreFlaCmd();
		break;
		
		case 0xdb:
			wlSet.m_flaCmd[0] = 0xa0;
			wlSet.m_cmdIndex = 1;
			MstPreFlaCmd();
		break;
		
		case 0xdc:
			wlSet.m_flaCmd[0] = 0xb0;
			wlSet.m_cmdIndex = 1;
			MstPreFlaCmd();
		break;

		case 0xdd:
			MstCmdPreFlaSignal();
		break;

		case 0xd4:
			CameraBlockRcv(receive_arrary, 6);
		break;
		
		
		case 0xd8:
			CameraBlockRcv(receive_arrary, 1);
			//PreFlash2Set();
		break;

		case 0xd5:
			//rts = 0xd5;
			sendRecord[0] = 0xd9;
			LiteBlockSend(sendRecord, 1);
		break;

		case 0x00:
		break;

		default:
			//SPIx_Rst();
		break;

	}
}

void CameraCmdReply(unsigned char cmd)
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
			g_dspCmpMod = 0;
			//SetComRstTime();
		break;	

		case 0xd8:
		//case 0xd2:
			g_datRcvFlag = 0;
			Ex_NVIC_Disable(ID_INT_NSS);
			PreFlash2Set();
		break;

		case 0xd2:
			g_datRcvFlag = 0;
			Ex_NVIC_Disable(ID_INT_NSS);
			PreFlash3Set();
		break;

		case 0xd3:
			sendRecord[0] = 0xD3;
			LiteBlockSend(sendRecord,1);	
			g_datRcvFlag = 2;
		break;

		case 0xc0:
			if(0x00c1==MAKEWORD(receive_arrary[1],receive_arrary[0]))
			{
				LiteBlockSend(macData4,sizeof(macData4));
			}else
			{
				LiteBlockSend(macData5,sizeof(macData5));
			}
			g_datRcvFlag = 2;
		break;

		case 0xd4:
			sendRecord[0] = 0xd4;
			LiteBlockSend(sendRecord,1);	
			g_datRcvFlag = 2;
		break;

		case 0xd9:     //send master command here
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
		break;

		case 0xb0:
			GetCmrInfo(0xb0);
			g_datRcvFlag = 0;
			g_comStatue = COM_STATUE_IDLE;
			Ex_NVIC_Disable(ID_INT_NSS);
		break;

		case 0xd0:
/**********************************************************
//           D0 01 FF     all on
//           D0 01 EA    left + buttom
//           D0 00 10    right
//           D0 00 40    buttom
//           D0 00 20    left
***********************************************************/
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
		break;

		case 0xd1:      //red eyes
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
		break;

		case 0xa0:
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
		
		break;

		case 0xd5:
			g_datRcvFlag = 0;
			g_comStatue = COM_STATUE_FOCUSDEPTH;
			OSTaskResume(COMPROC_TASK_PRIO);
		break;

		default:
			g_datRcvFlag = 0;
			g_comStatue = COM_STATUE_IDLE;
			//SetComWaitTimeout();
			//PULL_MOSI_HIGH();
			Ex_NVIC_Disable(ID_INT_NSS);
		break;

	}
}

void NikComInit(void)
{
	g_comStatue = COM_STATUE_IDLE;
	SET_SCK_HIGH();
	SetSBOInt();
	SET_NSS_IN();	
}

void FlashFinishHandle(void)
{
	g_mstEnFlag = 0;
	//g_golbalFlashEn = 0;
}

void NikComProc(void *pdata)
{
//	u8 nikCmd;
	u8 i = 0;
	u8 tmrErr;
	g_comStatue = COM_STATUE_IDLE;
	SET_SCK_HIGH();
	SetSBOInt();
	SET_NSS_IN();
	
	while(1)
	{
		switch(g_comStatue)
		{
			//if(COM_STATUE_IDLE==g_comStatue)
			case COM_STATUE_IDLE:
			{
				//g_comStatue = COM_STATUE_IDLE;
				//TEST_SIGNAL = !TEST_SIGNAL;
				if(CMD_MODE_LIGH_SLA!=*g_cmdMode)
				{
					SetSBOInt();
					//OSTimeDlyHMSM(0,0,0,10);
					OSTaskSuspend(COMPROC_TASK_PRIO);
				}else
				{
					OSTaskSuspend(COMPROC_TASK_PRIO);
				}
			
			}
			break;
			//else if(COM_STATUE_PREFLASH==g_comStatue)
			case COM_STATUE_PREFLASH:
			{
				while(!NIKONNSSI);
				delay_us(30);
				PULL_MOSI_LOW();
				SET_SCK_LOW();
				if(CMD_MODE_LIGH_MST==*g_cmdMode)
				{
					if((!g_isHotProtect)&&(g_mstFlaEnable>0))
					{
						//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
						TakePreflash(1);
					}else
					{
						delay_us(50);
					}
				}else
				{
					if((!g_isHotProtect)&&(g_chargeOk>0))
					{
						//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
						TakePreflash(1);
					}else
					{
						delay_us(50);
					}
				}
				SET_SCK_HIGH();
				g_comStatue = COM_STATUE_IDLE;
				
			}
			break;

			//else if(COM_STATUE_PREFLASH2==g_comStatue)
			case COM_STATUE_PREFLASH2:
			{
				while(!NIKONNSSI);
				delay_us(50);
				PULL_MOSI_LOW();			
				delay_us(400);
				SET_SCK_LOW();
				if(CMD_MODE_LIGH_MST==*g_cmdMode)
				{
					if((!g_isHotProtect)&&(g_mstFlaEnable>0))
					{
						//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
						TakePreflash(2);
					}else
					{
						delay_us(50);
					}
				}else
				{
					if((!g_isHotProtect)&&(g_chargeOk>0))
					{
						//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
						TakePreflash(2);
					}else
					{
						delay_us(50);
					}
				}
				SET_SCK_HIGH();
				g_comStatue = COM_STATUE_IDLE;
			}
			break;

			case COM_STATUE_PREFLASH3:
				delay_ms(27);
				SET_SCK_LOW();
				if((!g_isHotProtect)&&(g_chargeOk>0))
				{
					//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
					TakePreflash(2);
				}else
				{
					delay_us(50);
				}
				SET_SCK_HIGH();
				PULL_MOSI_LOW();	
				delay_us(1500);
				g_comStatue = COM_STATUE_IDLE;
			break;

			//else if(COM_STATUE_REALFLASH==g_comStatue)
			case COM_STATUE_REALFLASH:
			{
				if(g_isAfOn>0)
				{
					AF_OFF();
					g_isAfOn = 0;
				}
				//ClearComWaitTimeout();
				//OS_ENTER_CRITICAL();
				if((CMD_MODE_LIGH_MST==*g_cmdMode)&&(!pSnrSet[SNR_SU4]))
				{
					if((!g_isHotProtect)&&(g_mstFlaEnable>0))
					//if(!g_isHotProtect)
					{
						TakeFlash();
						g_mstFlaEnable = 0;
						g_mstEnFlag = 0;
					}else
					{
						delay_us(50);
					}
				}else
				{
					if(g_signalFlash>0)
					{
						g_signalFlash = 0;
						delay_us(200);
						if(!g_keyAdcEnable)      //if adc is disable, check battery voltage will cause corrupt
						{
							g_comStatue = COM_STATUE_IDLE;
							//FlashFinishHandle();
							//OS_EXIT_CRITICAL();
							break;
						}
						//if((GetBatteryAdc()<LOW_BAT_LEVEL)||(FLASH_IN>0))
						// 2017/07/07 µ¥´¥µãÉÁ¹â²»Á¬Ðø
						if((GetBatteryAdc()<(LOW_BAT_LEVEL/2))||(FLASH_IN>0))
						{
							g_comStatue = COM_STATUE_IDLE;
							//FlashFinishHandle();
							//OS_EXIT_CRITICAL();
							break;
						}
					}else if(FLASH_IN>0)
					{
						g_comStatue = COM_STATUE_IDLE;
						//FlashFinishHandle();
						//OS_EXIT_CRITICAL();
						break;
					}
				
					if((!g_isHotProtect)&&(g_chargeOk>0))
					{
						TakeFlash();
					}else
					{
						delay_us(50);
					}
				}
				g_comStatue = COM_STATUE_IDLE;
				FlashFinishHandle();
				//OS_EXIT_CRITICAL();
			}
			//FlashFinishHandle();
			break;

			case COM_STATUE_MSTPRE:
			{
				while(!NIKONNSSI);
				delay_us(400);
				PULL_MOSI_LOW();
				//OS_ENTER_CRITICAL();
				//g_chargeOk = 2;			//for test
				//if((!g_isHotProtect)&&(g_chargeOk>0))
				if((!g_isHotProtect)&&(g_mstFlaEnable>0))
				{
					WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
				}else
				{
					//SIGNAL1 = !SIGNAL1;
					delay_us(300);
					//SIGNAL1 = !SIGNAL1;
				}
				ENABLE_D30EN_EX();
				delay_us(3500);
				//if((!g_isHotProtect)&&(g_chargeOk>0))
				if((!g_isHotProtect)&&(g_mstFlaEnable>0))
				{
					WLM_COMMAND_TRIGER(WLM_PREFLA_INTERVAL);
					WLM_COMMAND_TRIGER(WLM_BIT_INTERVAL);
				}
				DISABLE_D30EN_EX();
				delay_us(380);
				SET_SCK_LOW();
				delay_us(380);
				SET_SCK_HIGH();
				//delay_us(3000);
				//OS_EXIT_CRITICAL();
				//PULL_MOSI_HIGH();	
				g_comStatue = COM_STATUE_IDLE;
			}
			break;

			case COM_STATUE_MSTCMD:
			{
				while(!NIKONNSSI);
				delay_us(70);
				PULL_MOSI_LOW();	
				//delay_ms(5);
				delay_us(300);
				//SET_SCK_LOW();
				//g_chargeOk = 2;			//for test
				//if((!g_isHotProtect)&&(g_chargeOk>0))
				if((!g_isHotProtect)&&(g_mstFlaEnable>0))
				{
					//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
					WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
				}else
				{
					delay_ms(3);
				}
				delay_ms(2);
				//SET_SCK_HIGH();
				PULL_MOSI_HIGH();	
				if(1==g_comAck)
				{
					g_comStatue = COM_STATUE_IDLE;
				}else if(2==g_comAck)
				{
					g_comStatue = COM_STATUE_FLASH_READY;
				}else
				{
					g_comStatue = COM_STATUE_IDLE;
				}
				//g_comStatue = COM_STATUE_FLASH_READY;
				//g_comStatue = COM_STATUE_REALFLASH;    //2016/04/02
			}
			break;

			case COM_STATUE_FLASH_READY:
				OSTaskSuspend(COMPROC_TASK_PRIO);
			break;

			case COM_STATUE_PRESIGNAL:		//0xdd
				while(!NIKONNSSI);
				//delay_us(420);
				PULL_MOSI_LOW();	
				//delay_us(1060);
				ENABLE_D30EN_EX();
				delay_us(420);
				//if((!g_isHotProtect)&&(g_chargeOk>0))
				if((!g_isHotProtect)&&(g_mstFlaEnable>0))
				{
					WLM_COMMAND_TRIGER(WLM_PREFLA_INTERVAL);
					WLM_COMMAND_TRIGER(WLM_BIT_INTERVAL);
				}else
				{
					delay_us(300);
				}
				DISABLE_D30EN_EX();
				delay_us(380);
				SET_SCK_LOW();
				delay_us(380);
				SET_SCK_HIGH();
				PULL_MOSI_HIGH();	
				g_comStatue = COM_STATUE_IDLE;
			break;

			case COM_STATUE_SLVPRE:
				StopCapture();
				DisableUpdateEvent();
				SlaveTakePreflash();
				StartCapture();
				//LightSlaveParaClear();       //becase there may be the second preflash, it cannot clear the state
				g_comStatue = COM_STATUE_IDLE;
			break;

			case COM_STATUE_FOCUSDEPTH:
				PULL_MOSI_LOW();	
				for(i=0;i<66;i++)
				{
					//OutputSwitch(-1,0);
					OS_ENTER_CRITICAL();
					WlmFlaADSet(-1,0);
					TURN_ON_TRIGER(OUTPUT_MAX);
					delay_ms(1);
					OS_EXIT_CRITICAL();
					delay_ms(14);
				}
				delay_ms(1);
				PULL_MOSI_HIGH();	
				g_comStatue = COM_STATUE_IDLE;
			break;

			case COM_STATUE_SLVFLA:
				//if((!g_isHotProtect)&&(g_chargeOk>0))                  // this will cause serious problem
				{
					StopCapture();
					SlaveTakeFlash();
					//TakeFlash();
					LightSlaveParaClear();
					StartCapture();
					SlaveBuzzer();
					//OSTmrStart(g_slvBzTmr, &tmrErr);
				}
				/*
				if(g_buzzerOn>0)
				{
					g_buzzerOn = 0;
					BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
				}*/
				g_comStatue = COM_STATUE_IDLE;
			break;
/*
			case COM_STATUE_RPT_FLA:
				//TakeRptFlash(&rptSet.finalExpTimes,mstPara->multi_exp1*3+mstPara->multi_exp2,rptSet.lowFrqCnt);
				//TakeRptFlash(&rptSet.finalExpTimes,rptSet.exp1*3+rptSet.exp2,rptSet.lowFrqCnt);
				TakeRptFlash(&rptSet.finalExpTimes,rptSet.exp1*3+rptSet.exp2);
				g_comStatue = COM_STATUE_IDLE;
			break;
*/
			/*else
			{
				CameraBlockRcv(&nikCmd,1);
				g_comStatue = COM_STATUE_START;
				CameraCmdHandle(nikCmd);		
				g_comStatue = COM_STATUE_IDLE;
				SetSBOPullup();
			}*/
		}
#if 0
		if(COM_STATUE_RUN==g_comStatue)
		{
			CameraBlockRcv(&nikCmd,1);
			g_comStatue = COM_STATUE_START;
			CameraCmdHandle(nikCmd);			
		}else
		{
			//OSTimeDly(1);							//delay tick
			g_comStatue = COM_STATUE_IDLE;
			SetSBOInt();
			OSTimeDlyHMSM(0,0,0,10);
		}
#endif

		//OSTimeDlyHMSM(0,0,0,100);
	}
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

#if 0
void ComDataReply(void)
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
		g_datRcvFlag = 0;
		g_comStatue = COM_STATUE_IDLE;
		//PULL_MOSI_HIGH();
		Ex_NVIC_Disable(ID_INT_NSS);
	}
	*/
}
#endif

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

void EXTI2_IRQHandler(void)
{
	//EXTI->PR=(1<<ID_INT_NSS);

	if(CMD_MODE_LIGH_SLA!=*g_cmdMode)
	{
		ClearComRstTime();
		if(COM_STATUE_START==g_comStatue)
		{
			if(!NIKONNSSI)
			{
				ComDataHandleProc();	
			}
		}
	}

	EXTI_ClearITPendingBit(EXTI_Line2);	

	EXTI_GetITStatus(EXTI_Line2);
}

void EXTI9_5_IRQHandler(void)
{
	//TEST_SIGNAL = !TEST_SIGNAL;	
	//EXTI->PR=(1<<ID_INT_SBO);

	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		EXTI->PR=(1<<ID_INT_NSS);
		EXTI->PR=(1<<ID_INT_SBO);
		return;
	}
	
	switch(g_comStatue)
	{
		case COM_STATUE_IDLE:
			
			if((!NIKONMISO)&&(NIKONNSSI>0))
			{
				if(g_isComStop>0)
				{
					g_isComStop = 0;
				}else
				{
					g_comStatue = COM_STATUE_START;
					//TEST_SIGNAL = !TEST_SIGNAL;	
					//SET_MOSI_DO();
					//NIKONMOSI = 0;
					//PULL_MOSI_LOW();
					PULL_MOSI_LOW();                                    //À­µÍÐÅºÅ½Å£¬µÈ´ýNSSÐÅºÅ
					Ex_NVIC_Disable(ID_INT_SBO);
					SetNssInt();
					SetComRstTime();
				}
			}
			if((g_stbyStatue>0)&&(NIKONNSSI>0))
			{
				g_stbyStatue = 0;
				SetChargeEnable();
				ENABLE_KEY_INT();
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
			EXTI->PR=(1<<ID_INT_SBO);                          //Ðè·ÅÔÚ×îºó£¬·ñÔò¸÷ÖÖÎÊÌâ
		break;
/*
		case COM_STATUE_STBY:
			EXTI->PR=(1<<ID_INT_SBO);
			g_comStatue = COM_STATUE_START;
			PULL_MOSI_LOW();                                    //À­µÍÐÅºÅ½Å£¬µÈ´ýNSSÐÅºÅ
			Ex_NVIC_Disable(ID_INT_SBO);
			SetNssInt();
			SetComRstTime();
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;
*/
/*
		case COM_STATUE_START:
			EXTI->PR=(1<<ID_INT_NSS);
			//TEST_SIGNAL = !TEST_SIGNAL;
			if(!NIKONNSSI)
			{
				//TEST_SIGNAL = !TEST_SIGNAL;	
				//g_comStatue = COM_STATUE_RUN;
				//OSTaskResume(COMPROC_TASK_PRIO);
				//Ex_NVIC_Disable(ID_INT_NSS);
				//OSTimeDlyResume(COMPROC_TASK_PRIO);
				ComDataHandleProc();                                          //½ÓÊÕÖ¸Áî
			}
		break;
*/
		case COM_STATUE_START:
		case COM_STATUE_RUN:
			EXTI->PR=(1<<ID_INT_NSS);
			EXTI->PR=(1<<ID_INT_SBO);
		break;
		
		case COM_STATUE_REALFLASH:
		if((!NIKONMISO)&&(NIKONNSSI>0))
		{
			g_comStatue = COM_STATUE_START;
			//TEST_SIGNAL = !TEST_SIGNAL;	
			//SET_MOSI_DO();
			//NIKONMOSI = 0;
			//PULL_MOSI_LOW();
			PULL_MOSI_LOW();                                    //À­µÍÐÅºÅ½Å£¬µÈ´ýNSSÐÅºÅ
			Ex_NVIC_Disable(ID_INT_SBO);
			SetNssInt();
			SetComRstTime();
		}
			EXTI->PR=(1<<ID_INT_SBO);                          //Ðè·ÅÔÚ×îºó£¬·ñÔò¸÷ÖÖÎÊÌâ
		break;
	}

//20170309 for debug
	if(EXTI_GetITStatus(EXTI_Line7) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
	}

	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
	
}

void CameraCmdFlagSet(unsigned char cmd)
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
		break;
		
		case 0xd7:
		case 0xda:
		case 0xdb:
		case 0xdc:
		case 0xdd:
			g_datRcvFlag = 0;
		break;

		default:
			g_datRcvFlag = 0;
		break;
	}
}

void SetBkLightInfo(u8 isOn)
{
	if(isOn>0)
	{
		macData2[0] |= 0x02;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}else
	{
		macData2[0] &= ~0x02;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}
}

void CmrLedCtrl(void)
{
	if(cmrPara.cls>0)
	{
		if(0==pSnrSet[SNR_BKLIGHT])
		{
			if(cmrPara.mode&0x04)
			{
				BK_GREEN(g_isHotProtect);		
				SetBkLightInfo(1);
			}else
			{
				BK_TURNOFF(g_isHotProtect);
				SetBkLightInfo(0);
			}
		}
	}
}

void CameraCmdRearHandle(unsigned char cmd)
{
	s8 tmp1=0,tmp2=0;
	
	switch(cmd)
	{
		case 0xd3:
			g_comStatue = COM_STATUE_REALFLASH;
			if((receive_arrary[0]&0xf0)==0xd0)
			{
				g_flaLockFlag = 1;
			}
			if(((receive_arrary[0]&0x0f)<=0x0c)&&((receive_arrary[0]&0x0f)>=0x09))
			{
				mstPara->hpFlash = HIGH_SPEED;
			}else
			{
				mstPara->hpFlash = LOW_SPEED;
			}
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
			}
			//ExposeTimeCal(receive_arrary[1],mstPara->hpFlash,mstPara->cmp1,mstPara->cmp2);
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				MstSetCheckEnable();
			}
			ExposeTimeCal(receive_arrary[1],mstPara->hpFlash,tmp1,tmp2);
			SetSBOInt();
		break;

		case 0xd4:
			if(pSnrSet[SNR_MSTRPT]>0)
			{
				BuildRealFlashCmd(receive_arrary[0],receive_arrary[2],receive_arrary[3],receive_arrary[4],(cmrPara.pShutter>0x36));
			}else
			{
				BuildRptFlashCmd();
			}
			MstCmdPrepear(2);
		break;

		case 0xE0:
			g_comStatue = COM_STATUE_IDLE;
			//SET_SCK_LOW();
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		default:
			g_comStatue = COM_STATUE_IDLE;
		break;
	}
	Ex_NVIC_Disable(ID_INT_NSS);
}

void SpiSrIntProc(void)
{
	switch(g_datRcvFlag)
	{
		case 0:
			g_cmdCode = SPI1->DR;
			g_rcvBufIndex = 0;
			CameraCmdHandle(g_cmdCode);
			//g_datRcvFlag = 1;
			CameraCmdFlagSet(g_cmdCode);
			SetComRstTime();
		break;

		case 1:
			g_spiBuf = SPI1->DR;
			receive_arrary[g_rcvBufIndex++] = g_spiBuf;
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
			g_spiBuf = SPI1->DR;
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
			g_spiBuf = SPI1->DR;
		break;

		
	}
}

//#ifdef _SPI_MODULE_
#if 1
void SPI1_IRQHandler(void)
{
//	u16 dat=0;
	//LED1=!LED1;
	//delay_ms(50);
	//g_cmdCode = SPI1->DR;

	SpiSrIntProc();

	if(COM_STATUE_IDLE==g_comStatue)				//send or receive ok
	{
		//PULL_MOSI_HIGH();
		SetSBOInt();
		//OSTimeDlyResume(COMPROC_TASK_PRIO);
	}
	//DBG_PRINTF("spi:%x\r\n",g_spiBuf);
	//if(SRMOD_R==g_srMod)
	//{
		//SPIx_Disable();
	//}

/*
	if(TRANSFER_START == g_srFlag)
	{
		//LED1=!LED1;
		if(SRMOD_S==g_srMod)
		{
			SPI1->DR = g_spiBuf;
		}else
		{
			SPI1->DR = 0xFF;
		}
		//g_srFlag = TRANSFER_READY;
		g_srFlag = TRANSFER_ONSEND;
	}else if(TRANSFER_ONSEND == g_srFlag)
	{
		//DisableTXEInt();
		//g_srFlag = TRANSFER_READY;
		//LED1=!LED1;
		//SPI_SS = 1;
		g_srFlag = TRANSFER_READY;
		//SET_SCK_HIGH();
		//DisableTXEInt();
		//SPIx_Disable();
	}
*/
	//SPIx_Disable();
	//receive_arrary[0] = SPI1->DR;
	//SPI1->SR &= ~(1<<1);
}
#endif
























