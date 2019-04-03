#ifndef _SPI_H_
#define _SPI_H_

#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "includes.h"
#define FLASH_ENABLE 1

#define COM_STATUE_START        0x01
#define COM_STATUE_IDLE         0x00
#define COM_STATUE_PREFLASH     0X02
#define COM_STATUE_REALFLASH    0X03
#define COM_STATUE_SLEEP        0X04
#define COM_STATUE_WLMPRE       0X05
#define COM_STATUE_MULTIFLASH   0x06
#define COM_STATUE_MODEL	0x07
#define COM_STATUE_SLAPRE	0X08
#define COM_STATUE_SLAFLA	0X09
#define COM_STATUE_PWRDN	0X0A
#define COM_STATUE_MODELING	0X0B
#define COM_STATUE_STOP         0x0C
#define COM_STATUE_BUSY         0x0D
#define COM_STATUE_FOCUSDEPTH   0x0E

#define CMR_LINK_NULL           0
#define CMR_LINK_CAN            1
#define CMR_LINK_NIK            2

#define TM_HANDLE_CLR           0x00
#define TM_HANDLE_REAR          0x01          
#define TM_HANDLE_SHUT          0x02	
#define TM_HANDLE_MULTIFLASH    0x04     // 3

#define DSP_UPDATE_MODE 	0x0001
#define DSP_UPDATE_MOTO	        0x0002
#define DSP_UPDATE_RFCHAN       0x0004
#define DSP_UPDATE_LINK	        0x0008
#define DSP_UPDATE_LKDEV	0x0010
#define DSP_UPDATE_RFSW	        0x0020
#define DSP_UPDATE_SHOT	        0x0040
#define DSP_UPDATE_CMDMOD       0x0080
#define DSP_SLACMD_TEST	        0x0100
#define DSP_SLACMD_MODEL	0x0200
#define DSP_SLACMD_REL	        0x0400
#define DSP_UPDATE_RFID         0x0800
#define DSP_UPDATE_LINKM2S      0x1000
#define DSP_UPDATE_LITETEST     0x2000
#define DSP_UPDATE_MODENOCLR	0x4000
#define DSP_UPDATE_SETSLATMR	0x8000


//#define TM_nHANDLE_CLR                             0x00
#define TM_nHANDLE_COMRST	                   0x10
#define TM_nHANDLE_SLAVE_PRE2             0x20   
#define TM_nHANDLE_MULTIFLASH	            0x40

#define SET_UPDATE_EVT(x)		{g_dspUpdateFlag|=x;}

#define NIK_SPI_BUF             SPI1->DR
#define NIK_SPI_SR              SPI1->SR

#define MOTO_READY              0X01
#define MOTO_MODE_AUTO          0x00
#define MOTO_MODE_MANU          0x01


#define WORKINGMODE_TTL         0
#define WORKINGMODE_MANU        1
#define WORKINGMODE_MULTI 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC         5
#define WORKINGMODE_GR		6
#define WORKINGMODE_F1		7
#define WORKINGMODE_F2		8

typedef unsigned int WORD;
typedef unsigned char BYTE;
#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 

#define Timer4Start(arr) \
{\
	TIM4->CNTR = (255 - arr); \
	TIM4->CR1 = 0x09; \
}

#define Timer4Stop()  \
{\
    TIM4->CR1 = 0x00;\
}



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
	s8 FEB_count;           //after setting surranding, this parameter record the flash times  ���������Χ֮����������ļ�¼
	//char rear_syn;        //mode certain sychronic 1:rear certain 2: frount certian    ǰ����ģʽ��ǰ��Ϊ0����Ϊ1
	s8 ttl_FEB1;
	s8 ttl_FEB2;
	s8 cmrCmp1;
	s8 cmrCmp2;
	u8 cmrCmpCode;
	s8 sleep;               //recored the sleep statue   �Ƿ����˯��ģʽ
	s8 charge_ok;		//recored the charge statue	ʱ������� 0 ����� 1�������� 2����絽�ɷ���ָ��
	s8 statue_link;	        //recored whether linking the camera��		ʱ���������
	s32 iso;                //record the iso setting of camera				�����iso
	u32 apert;	        //record the apert setting of camera		����Ĺ�Ȧֵ
	u32 flaRecord;	        //record the flash times of flashlite		��¼���������Ĵ���
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
	u8 pIso;              //ISO码
	u8 pIsoBk1;
	u8 pIsoBk2;
	u32 iso;              //iso
	//u8 isoIndex;
	u8 isAutoIso;
	u8 pShutter;
	u32 shutter;
	u8 pFocLen;           //zoom码
	u8 focLen;            //zomm
	u8 mode;        
	u8 cls;	
	u8 rearCurtain;
	u8 cmpCode;           //相机补偿码
        s8 camera_Ev;         //相机补偿值
	u8 bkMod;
	u8 linkTmr;
	u8 redEyeStart;
	u8 redEyeCnt;
	s8 camera_cmp1;
	s8 camera_cmp2;
}CmrPara;

typedef struct MOTO_SET
{
	u8 motoDir;             //direction  ��﷽�򣬸��ݶ�λ�������ã�ѹ�¶�λ����Ϊ0���ɿ���λ����Ϊ1
	u8 moto_en;		//enable   ����ƶ�ʹ��
	u16 moto_step;		//indicate step to move ��¼����ƶ��Ĳ���
	u8 moto_ready;          //indicate finished a turn on set up  ��︴λʱ��1�� ��λ�����0
	//unsigned char current_location; 	  //��ǰ��λ��
	u16 current_location;
	u16 *dest_location;     //��Ҫ�ƶ���Ŀ��λ��
	u16 autoDest;
	u16 destRsv;
#ifdef _CANON_
	unsigned char modRsv;
#endif
	u8 moving;		//indicate the moto is on the moving, so this is no response to new location ��¼����Ƿ���ת��
	u16 currentSet;
	u8 destSet;
	u8 focLen;		//��ʾ����Ļ�ϵ����λ��
	u8 autoRsv;
	s8 focIndex;  //������ֶ�ģʽ�µ�����ֵ
	u8 *mode;			//mode manu or auto Ī��ģʽ��0Ϊauto�� 1Ϊmanu
	//char motoRstCount; //֪ͨ����ֹͣ���λ�������ת��ʱ����Ҫ֪ͨ����ֹͣ���
	u16 motoRstCount;
	//char motoRstCount;  //��λ��ʱ��ֵ��������һ��ʱ����︴λû�����ʱ����ʾ����쳣
	u8 resetFlag;  //��λ��־�����ñ�־��λʱ����︴λ
	u8 uartSendFlag;
}MotoSet;

typedef struct WL_ARR_PARA
{
	//s8 mode;
	s8 output1;
	s8 output2;
	s8 cmp1;
	s8 cmp2;
	s8 multi_exp;
	
}WL_ArrPara;

typedef struct MULTI_Set
{
	s8  *expose;
	u16 *eTimes;
	u16 *frequence;
	//int finalExpTimes;
	u8 finalExpTimes;
	u8 cmdCode;
	u8 mulCtl;
	//int extIso;
	//unsigned char WlmMul[3];
	//unsigned char *wlmMulExp;
        //unsigned char frqMode;
	u16 lowFrqCnt;
	u8 sCode;
	u8 maxTimes;
        //unsigned char cMode;
	
}MultiSet;

typedef struct MOTO_LOCA
{
	u8 mode;
	u16 moto_location;
}MotoLoca;


#define MENU_ITEMS 5
typedef struct MAIN_PARA_SET
{
	u8 workMode;
	u8 cmdMode;
	//signed char m_mstOn;
	//s8 m_chan;
	//s8 ttl_Ratio;
	//s8 arr_mode;
	//u8 m_isMstFla;
	//WL_ArrPara modGr[4];	
	WL_ArrPara modGr;	

	u8 s_chan;
	u8 s_array;
	u8 s_rfMode;

	//int extIso;
	//int extApert;
	
	MotoLoca motoLoca;

	u8 cfnSet[MENU_ITEMS];
	
	//signed char wlSlave_chan;
	//signed char wlSlave_arr;
	//RadioSet rfSet;
	//unsigned char multi_exp;
	//u16 multi_Frq;
	//u16 multi_times;
	u16 multi_Frq;
	u16 multi_times;
	u8 synMode;
	u32 counter;	
	
	char sumVeri;

//	char rsv;

//	int rsv[2];
}MainPara;


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

void SetSBOInt(void);
void ClearComRstTime(void);

void SetSpioOn();

#define VERI_DATA_LEN           21

#define INT_EDGE_R              0
#define INT_EDGE_F              1

#define MAX_BUFFER_SIZE         30           // Maximum buffer Master will send
#define SEND_MAX_SIZE           30

#define TRANSFER_START          1	
#define TRANSFER_ONSEND         2
#define TRANSFER_READY          0

#define SRMOD_R                 0
#define SRMOD_S                 1

extern u8 ttl_expCodeRsv;
extern u8 g_getTtlFlashCode;

// nikon macro
//#define NIKONMISO PG_IDR_IDR7
//#define NIKONNSSI  PG_IDR_IDR4
//#define NIKONMOSI  PG_ODR_ODR6

#define NIKONMISO (((GPIOB->IDR)>>7)&0x01)
#define NIKONNSSI (((GPIOB->IDR)>>4)&0x01)
#define NIKONMOSI(x) (GPIO_WriteBit(GPIOB, GPIO_Pin_6,x))

#define FLASH_IN   (((GPIOD->IDR)>>4)&0x01)

//#define SET_MOSI_DO()                {SPI2_CR1 &= ~0x40;PG_DDR_DDR6=1;}
//#define PULL_MOSI_HIGH()             {SET_MOSI_DO(); NIKONMOSI = 1;}
//#define PULL_MOSI_LOW()              {SET_MOSI_DO(); NIKONMOSI = 0;}
#define SET_MOSI_DO()                {SPI1->CR1 &= ~0x40;GPIOB->DDR|=GPIO_Pin_6;}
#define PULL_MOSI_HIGH()             {SET_MOSI_DO(); NIKONMOSI(SET);}
#define PULL_MOSI_LOW()              {SET_MOSI_DO(); NIKONMOSI(RESET);}


//#define SET_MOSI_IN()                  {PG_DDR_DDR6=0;}
//#define SET_MISO_IN()			 {PG_DDR_DDR7=0;}
//#define SET_SCK_OUT()                  {SPI2_CR1 &= ~0x40;PG_DDR_DDR5=1;} 
//#define SET_MOSI_OUT()                 {PG_DDR_DDR6=1;}
#define SET_MOSI_IN()                   {GPIOB->DDR&=(u8)(~GPIO_Pin_6);}
#define SET_MISO_IN()			{GPIOB->DDR&=(u8)(~GPIO_Pin_7);}
#define SET_SCK_OUT()                   {SPI1->CR1 &= ~0x40;GPIOB->DDR|=GPIO_Pin_5;GPIOB->CR1|=GPIO_Pin_5;GPIOB->CR2|=GPIO_Pin_5;} 
#define SET_MOSI_OUT()                  {GPIOB->DDR|=GPIO_Pin_6;}


#define COM_DAT_MODE            1
//#define COM_DAT_AFON		3
#define COM_DAT_STATUE          3
#define COM_DAT_MASTPAR         5
#define COM_DAT_RPTFRQ          10
#define COM_DAT_RPTTIMES        11
#define COM_DAT_MSL_A		16
#define COM_DAT_MSL_BC		17
#define COM_DAT_SLPARA_A 	18
#define COM_DAT_SLPARA_B 	19
#define COM_DAT_SLPARA_C 	20
#define COM_DAT_REAR            21

#define OUTPUT_SHADOW           20


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
	SPI1->CR3 = 0x00;/*SPI2_ICR = 0x00;*/\
}

#define NikSpiIntEnable() \
{\
	SPI1->CR3 = 0x40;/*SPI2_ICR = 0x40;*/\
}

typedef enum{
	SLEEP_TAG_CAN = 0x10,
	SLEEP_TAG_NIK,
	SLEEP_TAG_PILOT,
	SLEEP_TAG_FLA,
}SLEEP_TAG;


#define IGBT_DRIVER_Start               GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define IGBT_DRIVER_Stop                GPIO_ResetBits(GPIOE, GPIO_Pin_7)

#define AF_ON()                         {GPIO_WriteBit(GPIOC,GPIO_Pin_5,SET);}
#define AF_OFF()                        {GPIO_WriteBit(GPIOC,GPIO_Pin_5,RESET);}

void SetNssSleepInt(void);
u8 VerCodeCal(const u8 *arr);
void SetSpiComModeOff(u8 mod, u8 aSel);
void UserInit(void);
void SPIn_Init(void);
void FlashEventLoop(void);
void GPIO_Pin_4_Callback(void);
void GPIO_Pin_7_Callback(void);
void TIM4_Callback(void);
void SPI1_IT_Callback(void);
void Formalfrash(uint16_t time);
void Multifrash_Handle(void);
void Slave_S2_TimeOut(void);

extern MainPara         mainPara;
extern CmrPara          cmrPara;
//extern _Flash_Time_TypeDef  Flash_Time_Group[];
extern int  Flash_Time_Group[6][28];


#endif
