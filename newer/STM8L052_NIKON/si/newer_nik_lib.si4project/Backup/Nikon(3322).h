#ifndef _SPI_H_
#define _SPI_H_

#include "stm8l15x.h"
#include "stm8l15x_conf.h"


#define COM_STATUE_START 0x01
#define COM_STATUE_IDLE 0x00
#define COM_STATUE_PREFLASH 0X02
#define COM_STATUE_REALFLASH 0X03
#define COM_STATUE_SLEEP    0X04
#define COM_STATUE_WLMPRE 0X05
#define COM_STATUE_MULTIFLASH 0x06
#define COM_STATUE_MODEL	0x07
#define COM_STATUE_SLAPRE	0X08
#define COM_STATUE_SLAFLA	0X09
#define COM_STATUE_PWRDN	0X0A
#define COM_STATUE_MODELING	0X0B
#define COM_STATUE_STOP      0x0C
#define COM_STATUE_BUSY      0x0D
#define COM_STATUE_FOCUSDEPTH 0x0E


//#include "type.h"
/*
typedef unsigned long u32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed long s32;
typedef signed char s8;
typedef signed short s16;
*/
typedef struct FLASHPARAMETER
{
	s8 FEB_count;		//after setting surranding, this parameter record the flash times  ���������Χ֮����������ļ�¼
	//char rear_syn;		//mode certain sychronic 1:rear certain 2: frount certian    ǰ����ģʽ��ǰ��Ϊ0����Ϊ1
	s8 ttl_FEB1;
	s8 ttl_FEB2;
	s8 cmrCmp1;
	s8 cmrCmp2;
	u8 cmrCmpCode;
	s8 sleep;				//recored the sleep statue   �Ƿ����˯��ģʽ
	s8 charge_ok;		//recored the charge statue	ʱ������� 0 ����� 1�������� 2����絽�ɷ���ָ��
	s8 statue_link;	//recored whether linking the camera��		ʱ���������
	s32 iso;					//record the iso setting of camera				�����iso
	u32 apert;	//record the apert setting of camera		����Ĺ�Ȧֵ
	u32 flaRecord;	//record the flash times of flashlite		��¼���������Ĵ���
	u8 ledStatue;
	u8 surrand_state;
	u8 hpFlash;
	u8 isoCmp;
	u8 shutter;
	u8 preflashIndex;
	u8 s_ttlCode;
	u8 s_flashEn;
	//u8 rfGrInfoFlag[5];
	//RfGr_CmuPara rfCmuPara[5];
	u32 quickTime;	
	//unsigned char staLink;
	//unsigned char hpFlash;
}FlaParameter;

typedef struct CAMERA_PARA
{
	u8 pApert;
	u16 apert;
	u8 pIso;
	u8 pIsoBk1;
	u8 pIsoBk2;
	u32 iso;
	//u8 isoIndex;
	u8 isAutoIso;
	u8 pShutter;
	u32 shutter;
	u8 pFocLen;
	u8 focLen;
	u8 mode;
	u8 cls;	
	u8 rearCurtain;
	u8 cmpCode;
	u8 bkMod;
	u8 linkTmr;
	u8 redEyeStart;
	u8 redEyeCnt;
	s8 camera_cmp1;
	s8 camera_cmp2;
}CmrPara;


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
//#define NIKONMISO PG_IDR_IDR7
//#define NIKONNSSI  PG_IDR_IDR4
//#define NIKONMOSI  PG_ODR_ODR6

#define NIKONMISO (((GPIOB->IDR)>>7)&0x01)
#define NIKONNSSI (((GPIOB->IDR)>>4)&0x01)
#define NIKONMOSI(x) (GPIO_WriteBit(GPIOB, GPIO_Pin_6,x))

//#define SET_MOSI_DO()                  {SPI2_CR1 &= ~0x40;PG_DDR_DDR6=1;}
//#define PULL_MOSI_HIGH()             {SET_MOSI_DO(); NIKONMOSI = 1;}
//#define PULL_MOSI_LOW()              {SET_MOSI_DO(); NIKONMOSI = 0;}
#define SET_MOSI_DO()                  {SPI1->CR1 &= ~0x40;NIKONMOSI(1);}
#define PULL_MOSI_HIGH()             {SET_MOSI_DO(); NIKONMOSI(1);}
#define PULL_MOSI_LOW()              {SET_MOSI_DO(); NIKONMOSI(0);}


//#define SET_MOSI_IN()                   {PG_DDR_DDR6=0;}
//#define SET_MISO_IN()			{PG_DDR_DDR7=0;}
//#define SET_SCK_OUT()                   {SPI2_CR1 &= ~0x40;PG_DDR_DDR5=1;} 
//#define SET_MOSI_OUT()                 {PG_DDR_DDR6=1;}
#define SET_MOSI_IN()                   {GPIOB->DDR&=(u8)(~GPIO_Pin_6)}
#define SET_MISO_IN()					{GPIOB->DDR&=(u8)(~GPIO_Pin_7);}
#define SET_SCK_OUT()                   {SPI1->CR1 &= ~0x40;GPIOB->DDR&=(u8)(~GPIO_Pin_5);} 
#define SET_MOSI_OUT()                  {GPIOB->DDR|=GPIO_Pin_6;}


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
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,RESET);/*PG_ODR_ODR5 = 0;*/\
}

#define SET_SCK_HIGH() \
{\
	SET_SCK_OUT(); \
    GPIO_WriteBit(GPIOB,GPIO_Pin_5,SET);/*PG_ODR_ODR5 = 1;*/ \
}

#define SET_NSS_IN() \
{\
	GPIOB->DDR &= ~0x10/*PG_DDR_DDR4 = 0;*/ \
	GPIOB->CR1 |= 0x10;/*PG_CR1_C14 = 1;*/     \
}

#define DisableSBOInt() \
{\
	GPIOB->CR2 &= (u8)(~GPIO_Pin_7);/*PG_CR2_C26 = 0;*/  \
}

#define EnableSBOInt() \
{\
	GPIOB->CR2 |= GPIO_Pin_7;/*PG_CR2_C26 = 1;*/  \
}

#define SetNssInt()  \
{\
	InitNssIntPort();\
}

#define OpenNssInt() \
{\
	GPIOB->CR2|=GPIO_Pin_4;/*PG_CR2_C24 = 1;*/ \
}

#define ShutNssInt() \
{\
	GPIOB->CR2&=(u8)(GPIO_Pin_4);/*PG_CR2_C24 = 0;*/ \
}

#define  NikSpiTrans() \
{\
	SPI1->CR1|=0X40;/*SPI2_CR1 |= 0x40;*/\
}

#define NikSpiStop() \
{\
	SPI1->CR1 &= ~0x40;/*SPI2_CR1 &= ~0x40;*/\
}

#define NikSpiIntDisable() \
{\
	SPI1->ICR = 0x00;/*SPI2_ICR = 0x00;*/\
}

#define NikSpiIntEnable() \
{\
	SPI1->ICR = 0x40;/*SPI2_ICR = 0x40;*/\
}

void SetNssSleepInt(void);


#endif
