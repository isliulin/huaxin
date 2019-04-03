#ifndef __CANON_H
#define __CANON_H

#include "stm8l15x.h"

typedef unsigned int WORD;
typedef unsigned char BYTE;

#define NO_FP_MODE             //屏蔽高速模式
//#define _MULTIPLY_FLASH_

#define INT_EDGE_R              0
#define INT_EDGE_F              1

#define MAX_BUFFER_SIZE         30           // Maximum buffer Master will send
#define SEND_MAX_SIZE           30

#define CANON_SCK_PIN           GPIO_Pin_5

#define SPI_MISO_AUX_HIGH       GPIOB->ODR |=GPIO_Pin_4;
#define SPI_MISO_AUX_LOW        GPIOB->ODR &= (uint8_t)(~GPIO_Pin_4);
#define SPI_MISO_AUX(x)         {if(x){GPIOB->ODR |=GPIO_Pin_4;}else{GPIOB->ODR &= (uint8_t)(~GPIO_Pin_4);}}
#define COM_AUX_HIGH            GPIOB->ODR |= GPIO_Pin_3
#define COM_AUX_LOW             GPIOB->ODR &= (uint8_t)(~GPIO_Pin_3)
#define SCK_IN                  GPIOB->IDR & (uint8_t)CANON_SCK_PIN

#define IGBT_DRIVER_Start      GPIO_SetBits(GPIOE, GPIO_Pin_7)
#define IGBT_DRIVER_Stop       GPIO_ResetBits(GPIOE, GPIO_Pin_7)

#define Enable_Flash_IN_Int    GPIOD->CR2 |= GPIO_Pin_4;              
#define Disable_Flash_IN_Int   GPIOD->CR2 &= (uint8_t)(~(GPIO_Pin_4));  
//#define Enable_Flash_IN_Int     GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_In_PU_IT); 
//#define Disable_Flash_IN_Int    GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_In_PU_No_IT); 

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

#define WL_ARRARY_A			0
#define WL_ARRARY_B 			1
#define WL_ARRARY_C 			2
#define WL_ARRARY_D 			3
#define WL_ARRARY_E 			4
#define WL_ARRARY_NULL 		        5
#define MASTER				0

#define VER_CAMERA_OLD		0
#define VER_CAMERA_NEW	        1

#define CMR_LINK_NULL           0
#define CMR_LINK_CAN            1
#define CMR_LINK_NIK            2

#define WORKINGMODE_TTL         0
#define WORKINGMODE_MANU        1
#define WORKINGMODE_MULTI 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC         5
#define WORKINGMODE_GR		6
#define WORKINGMODE_F1		7
#define WORKINGMODE_F2		8

#define DSP_UPDATE_MODE	        0x0001
#define DSP_UPDATE_MOTO	        0x0002
#define DSP_UPDATE_RFCHAN       0x0004
#define DSP_UPDATE_LINK	        0x0008
#define DSP_UPDATE_LKDEV	0x0010
#define DSP_UPDATE_RFSW	        0x0020
#define DSP_UPDATE_SHOT	        0x0040
#define DSP_UPDATE_CMDMOD       0x0080
#define DSP_SLACMD_TEST	        0x0100
#define DSP_SLACMD_MODEL	0x0200
#define DSP_SLACMD_REL          0x0400
#define DSP_UPDATE_RFID         0x0800
#define DSP_UPDATE_LINKM2S      0x1000
#define DSP_UPDATE_LITETEST     0x2000
#define DSP_UPDATE_MODENOCLR	0x4000
#define DSP_UPDATE_SETSLATMR	0x8000

#define MOTO_MODE_AUTO          0x00
#define MOTO_MODE_MANU          0x01

#define AD_CHARGE_LOW           0
#define AD_CHARGE_RDY           1
#define AD_CHARGE_FUL           2

#define SYNMODE_HEAD            0
#define SYNMODE_REAR            1
#define SYNMODE_HP              2

#define UART_CC_REL_FLA         0x01
#define UART_CC_REL_MODEL       0x02

//#define SET_SPIO_OFF()   {SPI1->CR1 &= ~0x40;/* SPI Enable */  SPI_ITConfig(SPI1,SPI_IT_RXNE, DISABLE);  /*  spi receive int */}
//#define SET_SPIO_ON()    {SPI1->CR1 |=  0x40;/* SPI Enable */  SPI_ITConfig(SPI1,SPI_IT_RXNE, ENABLE);   /*  spi receive int */}

#define SET_SPIO_OFF()   \
{\
        SPI1->CR1 &= ~0x40;              /* SPI Enable */    \
        SPI1->CR3 = 0x00;                  /*  spi receive int */   \
}

#define SET_SPIO_ON() \
{\
	\
        SPI1->CR1 |= 0x40;              /* SPI Enable */    \
        SPI1->CR3 = 0x40;                  /*  spi receive int */   \
}

#define Timer4Start(arr) \
{\
	TIM4->CNTR = (255 - arr); \
	TIM4->CR1  = 0x09; \
}

#define Timer4Stop()  \
{\
    TIM4->CR1 = 0x00;\
}

#define PUSH_MISO()     GPIOB->ODR &= (uint8_t)(~GPIO_Pin_5)   
#define FLASH_IN        (GPIOD->IDR & (uint8_t)GPIO_Pin_4)

#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
#define GETBIT(targetbit,n) ((targetbit&(1<<n)))
//#define SETBIT(targetbit,n)	(targetbit|=(n))
#define CLRBIT(targetbit,n)	 (targetbit&=(~(n)))

#define H_COV(x)        ((x)>>3)//(((x)>>4)*2+(((x)&0x0f)>>3))
#define L_COV(x)        (((x)&0x07)/3)//((((x)&0x0f)>=0x08)?((((x)&0x0f)-0x08)/3):(((x)&0x0f)/3))

#define H_COV_5(x)	(x>>3)
#define L_COV_5(x)	(x>>2)

#define EXPH_COV(x)     (H_COV(0x38-(x)))
#define EXPL_COV(x)     (L_COV(0x38-(x)))

#define H_COV_DIV2(x)	((x)>>3)
#define L_COV_DIV2(x)	(((x)&0x07)/2)

#define CMPH_COV(x)	(((signed char)(x)>0)?(-1)*H_COV_DIV2((signed char)(x+7)):H_COV_DIV2((signed char)(x)*(-1)))
#define CMPL_COV(x) (((signed char)(x)>0)?L_COV_DIV2((signed char)(x*(-1))):L_COV_DIV2((signed char)(x)*(-1)))

#define CMPH_COV_5(x)  ((((signed char)(0x30-x))>>3)-6)//((signed char)(x)/8*(-1))
#define CMPL_COV_5(x)   ((((signed char)(0x30-x))>>2)&0x01)//((signed char)(x)%8/4)

#define SET_UPDATE_EVT(x)		{g_dspUpdateFlag|=x;}

#define SYNC_MODE_FRON  		0
#define SYNC_MODE_REAR  		1
#define SYNC_MODE_FP			2

typedef struct MOTO_SET
{
	u8 motoDir;      //direction  马达方向，根据定位开关设置，压下定位开关为0，松开定位开关为1
	u8 moto_en;		//enable   马达移动使能
	u16 moto_step;		//indicate step to move 记录马达移动的步数
	u8 moto_ready;	//indicate finished a turn on set up  马达复位时置1， 复位完成清0
	//unsigned char current_location; 	  //当前的位置
	u16 current_location;
	u16 *dest_location;     //需要移动的目的位置
	u16 autoDest;
	u16 destRsv;
#ifdef _CANON_
	unsigned char modRsv;
#endif
	u8 moving;              //indicate the moto is on the moving, so this is no response to new location 记录马达是否在转动
	u16 currentSet;
	u8 destSet;
	u8 focLen;              //显示在屏幕上的马达位置
	u8 autoRsv;
	s8 focIndex;            //马达在手动模式下的索引值
	u8 *mode;               //mode manu or auto 莫达模式，0为auto， 1为manu
	//char motoRstCount;    //通知主控停止充电位，当马达转动时，需要通知主控停止充电
	u16 motoRstCount;
	//char motoRstCount;    //复位的时间值，当超过一定时间马达复位没有完成时，显示马达异常
	u8 resetFlag;           //复位标志，当该标志置位时，马达复位
	u8 uartSendFlag;
}MotoSet;

typedef enum{
	SLEEP_TAG_CAN = 0x10,
	SLEEP_TAG_NIK,
	SLEEP_TAG_PILOT,
	SLEEP_TAG_FLA,
}SLEEP_TAG;

//#define SLEEP_DBG

typedef enum {
	CFN_AUTO_SHUT,                 // auto poweroff
	CFN_FEB_OFF,                      // FEB auto off
	CFN_FEB_TURN,                    // FEB TURN
	CFN_AF_ON,                          // is af on
	CFN_BEEP,                             // is beep on
}CFN_ITEMS;

typedef struct WL_ARR_PARA
{
	//s8 mode;
	s8 output1;
	s8 output2;
	s8 cmp1;
	s8 cmp2;
	s8 multi_exp;
	
}WL_ArrPara;

typedef struct MOTO_LOCA
{
	u8 mode;
	u16 moto_location;
}MotoLoca;

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

typedef struct MENU_PARA
{
	u8 mode;
	//unsigned char mstTtl;
	s8 mstTtl;
	u8 otpCod[3];
	u8 WlmMul[3];
	u8 rfGrOtp[5];
	u8 rfGrTmp[5];
	u8 motoCod;
	u8 motoLoc;
	u8 motoIndex;
	u8 feb;
	u8 cApert;
	u8 para_a5;
	//u8 syn2;
	//u8 syn3;
}MenuPara;

typedef struct MULTI_Set
{
	s8 *expose;
	u16  *eTimes;
	u16 *frequence;
	//int finalExpTimes;
	u8 finalExpTimes;
	u8 cmdCode;
	u8 mulCtl;
	//int extIso;
	//unsigned char WlmMul[3];
	//unsigned char *wlmMulExp;
//	unsigned char frqMode;
	u16 lowFrqCnt;
	u8 sCode;
	u8 maxTimes;
//	unsigned char cMode;
	
}MultiSet;

typedef struct FLASHPARAMETER
{
	s8 FEB_count;		//after setting surranding, this parameter record the flash times  设置闪光包围之后闪光次数的记录
	//char rear_syn;		//mode certain sychronic 1:rear certain 2: frount certian    前后帘模式，前帘为0后帘为1
	s8 ttl_FEB1;
	s8 ttl_FEB2;
	s8 cmrCmp1;
	s8 cmrCmp2;
	u8 cmrCmpCode;
	s8 sleep;				//recored the sleep statue   是否进入睡眠模式
	s8 charge_ok;		//recored the charge statue	时候充电完成 0 充电中 1：充电可闪 2：充电到可发送指令
	s8 statue_link;	//recored whether linking the camera；		时候连接相机
	s32 iso;					//record the iso setting of camera				相机的iso
	u32 apert;	//record the apert setting of camera		相机的光圈值
	u32 flaRecord;	//record the flash times of flashlite		记录闪光灯闪光的次数
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


extern MainPara        mainPara;
extern MenuPara        menuPara;
extern CmrPara         cmrPara ;
extern MotoSet         motoSet ;
extern MultiSet        multiSet;

void SPI1_Init(void);
void InitExtIntPort(void);
void GPIO_Pin_5_Callback(void);
void TIM4_Callback(void);
void SPI1_IT_Callback(void);
void SetSpiTrans(void);
uint8_t Flash_Time_out(uint8_t value);

#endif