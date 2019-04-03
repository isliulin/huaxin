#ifndef _SPI_H_
#define _SPI_H_

#include "type.h"

//#define _FORMAT_PRINT_

void putchar(u8 c);
void SPI1_Init(void);
void Init_ExInt(void);
void InitExtIntPort(void);
void SetSckInt(u8 tig);

void NikComInit(void);
void SPI2_Init(void);
void RstComPara(void);
void NikTmComRst(void);
//void NikTmComTimeout(void);
char GetIsoStdIndex(const u32* isoArr, char size, u32 cd);

void SetSpiTrans(void);
void SetSBOInt(void);
void ClearComRstTime(void);

void SetSpioOn();

#define VERI_DATA_LEN 21

#define INT_EDGE_R     0
#define INT_EDGE_F     1

#define MAX_BUFFER_SIZE    30           // Maximum buffer Master will send
#define SEND_MAX_SIZE        30

#define TRANSFER_START		1	
#define TRANSFER_ONSEND	2
#define TRANSFER_READY 	0

#define SRMOD_R    0
#define SRMOD_S    1

extern u8 ttl_expCodeRsv;
extern u8 g_getTtlFlashCode;

// nikon macro
#define NIKONMISO PG_IDR_IDR7
#define NIKONNSSI  PG_IDR_IDR4
#define NIKONMOSI  PG_ODR_ODR6

#define SET_MOSI_DO()                  {SPI2_CR1 &= ~0x40;PG_DDR_DDR6=1;}
#define PULL_MOSI_HIGH()             {SET_MOSI_DO(); NIKONMOSI = 1;}
#define PULL_MOSI_LOW()              {SET_MOSI_DO(); NIKONMOSI = 0;}

#define SET_MOSI_IN()                   {PG_DDR_DDR6=0;}
#define SET_MISO_IN()			{PG_DDR_DDR7=0;}
#define SET_SCK_OUT()                   {SPI2_CR1 &= ~0x40;PG_DDR_DDR5=1;} 
#define SET_MOSI_OUT()                 {PG_DDR_DDR6=1;}

#define COM_DAT_MODE	       1
//#define COM_DAT_AFON		3
#define COM_DAT_STATUE        3
#define COM_DAT_MASTPAR	5
#define COM_DAT_RPTFRQ        10
#define COM_DAT_RPTTIMES    11
#define COM_DAT_MSL_A		16
#define COM_DAT_MSL_BC		17
#define COM_DAT_SLPARA_A 	18
#define COM_DAT_SLPARA_B 	19
#define COM_DAT_SLPARA_C 	20
#define COM_DAT_REAR            21

#define SET_SCK_LOW() \
{\
	SET_SCK_OUT();\
	PG_ODR_ODR5 = 0;\
}

#define SET_SCK_HIGH() \
{\
	PG_CR1_C15 = 1;  \
	PG_CR2_C25 = 1;   \
	PG_DDR_DDR5 = 1; \
       PG_ODR_ODR5 = 1; \
}

#define SET_NSS_IN() \
{\
	PG_DDR_DDR4 = 0; \
	PG_CR1_C14 = 1;     \
}

#if(BOARD_VER == __BOARD_10_)
#define DisableSBOInt() \
{\
	PG_CR2_C26 = 0;  \
}

#define EnableSBOInt() \
{\
	PG_CR2_C26 = 1;  \
}

#elif (BOARD_VER == __BOARD_20_)
#define DisableSBOInt() \
{\
	PG_CR2_C27 = 0;  \
}

#define EnableSBOInt() \
{\
	PG_CR2_C27 = 1;  \
}

#endif

#define SetNssInt()  \
{\
	InitNssIntPort();\
}

#define OpenNssInt() \
{\
	PG_CR2_C24 = 1; \
}

#define ShutNssInt() \
{\
	PG_CR2_C24 = 0; \
}

#define  NikSpiTrans() \
{\
	SPI2_CR1 |= 0x40;\
}

#define NikSpiStop() \
{\
	SPI2_CR1 &= ~0x40;\
}

#define NikSpiIntDisable() \
{\
	SPI2_ICR = 0x00;\
}

#define NikSpiIntEnable() \
{\
	SPI2_ICR = 0x40;\
}

void SetNssSleepInt(void);


#endif
