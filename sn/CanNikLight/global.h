
#ifndef _GLOBAL_H_
#define _GLOBAL_H_
/************************************************************************/
/* Include files                                                                     */
/************************************************************************/
#include"iostm8l152r8.h"
#include "type.h"

/************************************************************************/
/* macro                                                                       */
/************************************************************************/
//#define SLEEP_DBG
//#define FLASH_SYN_DBG
//#define NIK_COM_DBG
//#define CAN_COM_DBG
//#define _A7105_DBG_
//#define MOTO_DBG
//#define DETAIL_DBG
//#define _RF_MULTI_DBG_
#define __BOARD_10_  0
#define __BOARD_20_  1
#define BOARD_VER __BOARD_20_

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

#define SPI_MISO_AUX(x)     { PF_ODR_ODR1 = x;}
#define COM_AUX          PF_ODR_ODR0
#define SCK_IN              PB_IDR_IDR5
#define EN339               PF_ODR_ODR5
#define PILOT_PIN        PE_IDR_IDR0
//#define TEST_PIN         PB_ODR_ODR3
//#define TEST_PIN         PD_ODR_ODR7
//#define TEST_PIN           PD_ODR_ODR1
#define CHARGE_CON   PC_IDR_IDR4
#define CHARGE_ENABLE_PIN  PC_ODR_ODR4
#define ENABLE_CHARGE()         {CHARGE_ENABLE_PIN = 1;}
#define DISABLE_CHARGE()        {CHARGE_ENABLE_PIN = 0;}
#define BK_LIGHT_PIN  PE_ODR_ODR7
#define SW_POWER_OFF       PG_IDR_IDR2
#define SW_LOCK                   PG_IDR_IDR3
#define AF_IN                       PF_IDR_IDR4
#define AF_OUT                    PD_ODR_ODR6
#define AF_ON()                      {AF_OUT=1;}
#define AF_OFF()                    {AF_OUT=0;}
#define DC_5V                      PF_ODR_ODR6
#define DC_ON()                   {DC_5V = 1;}
#define DC_OFF()			  {DC_5V = 0;}
#define LIGHT_SLAVE_IRQ   PB_IDR_IDR1

#if (BOARD_VER == __BOARD_20_)
#define NIK_ODR_SW           PE_ODR_ODR4
//#define NIK_ODR_OFF()          {NIK_ODR_SW = 0;}
//#define NIK_ODR_ON()           {NIK_ODR_SW = 1;}
#define NIK_ODR_OFF() 
#define NIK_ODR_ON()
#define NIK_ODR_ALWAYS()	{NIK_ODR_SW = 1;}

#define READY                      PB_ODR_ODR4
#define READY_ON()             {PB_ODR_ODR4=1;}
#define READY_OFF()             {PB_ODR_ODR4=0;}

#elif (BOARD_VER == __BOARD_10_)
#define READY                      PB_ODR_ODR3
#define READY_ON()             {PB_ODR_ODR3=1;}
#define READY_OFF()             {PB_ODR_ODR3=0;}
#endif

#define WORKINGMODE_TTL 		0
#define WORKINGMODE_MANU 		1
#define WORKINGMODE_MULTI 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC 		5
#define WORKINGMODE_GR		6
#define WORKINGMODE_F1		7
#define WORKINGMODE_F2		8


#define SYNC_MODE_FRON  		0
#define SYNC_MODE_REAR  		1
#define SYNC_MODE_FP			2

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

#define SYNMODE_HEAD 0
#define SYNMODE_REAR 1
#define SYNMODE_HP	    2

#define UART_CC_REL_FLA					0x01
#define UART_CC_REL_MODEL					0x02

#define WL_ARRARY_A			0
#define WL_ARRARY_B 			1
#define WL_ARRARY_C 			2
#define WL_ARRARY_D 			3
#define WL_ARRARY_E 			4
#define WL_ARRARY_NULL 		5
#define MASTER				0

#define VER_CAMERA_OLD		0
#define VER_CAMERA_NEW	1

#define CMR_LINK_NULL           0
#define CMR_LINK_CAN             1
#define CMR_LINK_NIK              2

#define NULL 0

#define SETTING_IDLE			0
#define SETTING_SYNMODE		1
#define SETTING_TTLFEB			2
#define SETTING_MULTI			3
#define SETTING_FRQ				4
#define SETTING_SNRMENU		5		//ModeNoExit
#define SETTING_ADDSUB			6
#define SETTING_MOTO			7
#define SETTING_GR				8
#define SETTING_CH				9
#define SETTING_CFN				10

#define DSP_UPDATE_MODE	0x0001
#define DSP_UPDATE_MOTO	0x0002
#define DSP_UPDATE_RFCHAN		0x0004
#define DSP_UPDATE_LINK	0x0008
#define DSP_UPDATE_LKDEV	0x0010
#define DSP_UPDATE_RFSW	0x0020
#define DSP_UPDATE_SHOT	0x0040
#define DSP_UPDATE_CMDMOD 0x0080
#define DSP_SLACMD_TEST	0x0100
#define DSP_SLACMD_MODEL	0x0200
#define DSP_SLACMD_REL	       0x0400
#define DSP_UPDATE_RFID      0x0800
#define DSP_UPDATE_LINKM2S  0x1000
#define DSP_UPDATE_LITETEST 0x2000
#define DSP_UPDATE_MODENOCLR	0x4000
#define DSP_UPDATE_SETSLATMR	0x8000

#define MOTO_MODE_AUTO      0x00
#define MOTO_MODE_MANU      0x01

#define CMD_MODE_OFF			0
#define CMD_MODE_RF			1
#define CMD_MODE_LIGH_F1    	2
#define CMD_MODE_LIGH_F2		3

#define ENABLE    1
#define DISABLE  0


#define _MULTIPLY_FLASH_

typedef unsigned int WORD;
typedef unsigned char BYTE;

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2


//#define get_counter()   ((TIM2_CNTRH << 8) | TIM2_CNTRL)
//#define get_counter()   (TIM2_CNTRL + (TIM2_CNTRH << 8))
//#define get_counter()   ((TIM1_CNTRH << 8) | TIM1_CNTRL)                      // cannot use timer1 as system timer, max time 26 us
extern u16 get_counter(void);

#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
#define GETBIT(targetbit,n) ((targetbit&(1<<n)))
//#define SETBIT(targetbit,n)	(targetbit|=(n))
#define CLRBIT(targetbit,n)	 (targetbit&=(~(n)))

#define H_COV(x)    ((x)>>3)//(((x)>>4)*2+(((x)&0x0f)>>3))
#define L_COV(x)    (((x)&0x07)/3)//((((x)&0x0f)>=0x08)?((((x)&0x0f)-0x08)/3):(((x)&0x0f)/3))

#define H_COV_5(x)	(x>>3)
#define L_COV_5(x)	(x>>2)

#define EXPH_COV(x)		(H_COV(0x38-(x)))
#define EXPL_COV(x)		(L_COV(0x38-(x)))

#define H_COV_DIV2(x)	((x)>>3)
#define L_COV_DIV2(x)	(((x)&0x07)/2)

#define CMPH_COV(x)	(((signed char)(x)>0)?(-1)*H_COV_DIV2((signed char)(x+7)):H_COV_DIV2((signed char)(x)*(-1)))
#define CMPL_COV(x) (((signed char)(x)>0)?L_COV_DIV2((signed char)(x*(-1))):L_COV_DIV2((signed char)(x)*(-1)))

#define CMPH_COV_5(x)  ((((signed char)(0x30-x))>>3)-6)//((signed char)(x)/8*(-1))
#define CMPL_COV_5(x)   ((((signed char)(0x30-x))>>2)&0x01)//((signed char)(x)%8/4)

#define SET_UPDATE_EVT(x)		{g_dspUpdateFlag|=x;}

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
	u8 moving;		//indicate the moto is on the moving, so this is no response to new location 记录马达是否在转动
	u16 currentSet;
	u8 destSet;
	u8 focLen;		//显示在屏幕上的马达位置
	u8 autoRsv;
	s8 focIndex;  //马达在手动模式下的索引值
	u8 *mode;			//mode manu or auto 莫达模式，0为auto， 1为manu
	//char motoRstCount; //通知主控停止充电位，当马达转动时，需要通知主控停止充电
	u16 motoRstCount;
	//char motoRstCount;  //复位的时间值，当超过一定时间马达复位没有完成时，显示马达异常
	u8 resetFlag;  //复位标志，当该标志置位时，马达复位
	u8 uartSendFlag;
}MotoSet;

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

/************************************************************************/
/* Global var                                                                      */
/************************************************************************/
extern u8 g_canComStatue;
extern u8 g_nikComStatue;
extern u8 g_chargeOk;
extern u8 g_shutterFlag;
extern MotoSet motoSet;
extern MenuPara menuPara;
extern WL_ArrPara *mstPara;
extern MainPara mainPara;
extern MultiSet multiSet;
extern u8 g_vCmr;
extern u8 g_cmrLinked;
extern u8 *g_cmdMode;
extern u8 *g_lightMode;
extern FlaParameter flaPara;
extern u16 g_dspUpdateFlag;
extern u8 g_SettingIndex;
extern const u8 compensation_table[37];
extern u8 g_isReflectPulled;
extern u8 ttl_pre2_flag;
extern u8 g_fnIndex;
extern u8 *g_cfnOption;
extern const u8 focLenScape[10];
extern u8 ttl_expCodeRsv;
extern u8 g_sFlashTig;
extern u8 g_flashEnable;
extern u8 g_sRcvCnt;
extern u8 g_sFlashEn;
extern u8 g_cmrPressed;
extern u8 g_isAfOn;
extern u16 g_afOnCnt;
extern u8 g_sRcvState;
extern u16 g_s2Timer;
extern u8 g_s2Flag;
extern u16 g_sysSleepCnt ;
extern const u8 focLenCode[10];
extern u32 gWSleepCounter;
extern u8 g_sleepTag;
extern u8 *g_rfMode;
extern u8 g_isShadowOn;
extern u16 g_shadowTimer;
extern u8 g_shadowCnt;
extern const u16 g_expTime[6][31];
extern u8 g_canCmrLink;
extern u8 g_sRearFlag;
extern u8 g_nikSuspend;
extern u16 g_volRef;
extern CmrPara cmrPara;
extern u8 g_rfStatue;
extern u16 g_lowbatCnt;
extern u8 g_lowbat;
extern u8 g_lowbatDsp;
extern u8 g_shadowKey;
extern u16 g_slaveFlashCounter;

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

#define CFN_ON 0

extern const u8 g_a7105_id1[4];
extern const u8 g_a7105_id2[4];
extern const u8 g_a7105_id3[4];

extern const u8 output_table[22];

extern void A7105_Write_Ram(u8 addr, const u8 * ram, u8 len);
extern void A7105_Write_Reg(u8 addr, u8 val);

#define SET_SPIO_OFF()   \
{\
        SPI1_CR1 &= ~0x40;              /* SPI Enable */    \
        SPI1_ICR = 0x00;                  /*  spi receive int */   \
}

#define SET_SPIO_ON() \
{\
	\
        SPI1_CR1 |= 0x40;              /* SPI Enable */    \
        SPI1_ICR = 0x40;                  /*  spi receive int */   \
}

#define PUSH_MISO() \
{\
       PB_ODR_ODR5 = 0; \
}

/************************************************************************/
/* function                                                                     */
/************************************************************************/
extern void Timer4Init(void);
void memset(u8 * src, u8 dat, u8 len);
void BkLightOn(void);
void BkLightOff(void);
void SetTtlScr(void);
void KeyEventLoop( void );
void DspUpdateEventLoop(void);
void MotoEventLoop(void);
void FlashEventLoop(void);
void ChargeEventLoop(void);
void RfSlaveEventLoop(void);
void MultiFlashEventLoop(void);
void AfEventLoop(void);
void SetDailInt(void);
void SleepPortResume(const u8 *pddr, const u8 * podr, const u8 *pcr1, const u8 *pcr2);
u8 GetMotoStdIndex(u8 dest);
void RfSetId(u8 ch, u8* id);
void BeeperOn(u8 cnt, u16 tms);
void RfHardWareCheck(void);
void SetSpiComModeOff(u8 mod, u8 aSel);
u8 VerCodeCal(const u8 *arr);
void SetBkLightInfo(u8 isOn);
void SwRfMode(void);
void CanPreFlash(void);
void NikPreFlash(void);
void RfIntPortSet(u8 en);
void SlaveEventToggle(void);
void CmrLinkWatchDog(void);
u16 CheckVolBeforeFlash(void);
void ResetDailInt(void);

extern u8 g_sleepRegDdr[7];
extern u8 g_sleepRegOdr[7];
extern u8 g_sleepRegCr1[7];
extern u8 g_sleepRegCr2[7];

#define st(x)      do { x } while (__LINE__ == -1)

typedef unsigned char halIntState_t;
#define HAL_ENTER_CRITICAL_SECTION(x)   st( asm("sim"); )
#define HAL_EXIT_CRITICAL_SECTION(x)    st( asm("rim"); )
#define HAL_CRITICAL_STATEMENT(x)       //st( halIntState_t _s; HAL_ENTER_CRITICAL_SECTION(_s); x; HAL_EXIT_CRITICAL_SECTION(_s); )

#endif

