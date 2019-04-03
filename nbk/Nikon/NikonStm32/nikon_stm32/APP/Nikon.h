#ifndef _CBYINIKON_
#define _CBYINIKON_

//C:\Keil_v5\ARM\ARMACC\bin\fromelf.exe --bin -o  ..\IAP\NikonStm32.bin ..\obj\NikonStm32.axf

#include "os_cpu.h"
#include "os_cfg.h"
//#include <stm32f10x_type.h>
#include "stm32f10x.h"
#include "delay.h"
#include "uCOS_II.H"
#include "../HARDWARE/STMFLASH/stmflash.h"
#include "FlashPara.h"
//#include "../hardware/Lcd/font.h"

//#define PRINT_DEBUG
//#define MOTO_DEBUG_PRT
#define _CANON_WIRELESS_
//#define CANON_RF
#define _MULTIPLY_FLASH_
#define _HP_SYN_
#define RF_MODE_GROUP
#define _SPI_MODULE_
#define FIBER_ADJUST_PROC
#define _NIKON_
#define _MOTO_
#define FLASH_BASE_FIBER
#define MOTO_REFLECT_DETECT
//#define SLAVE_DEBUG
//#define MST_DEBUG
//#define ROTATE_DEBUG

#define HARD_WARE_VER1 1
#define HARD_WARE_VER2 2
#define HARD_WARE_TSER 3
#define HARD_WARE_9325 4

#define _172x104_
#define LCD_DRIVER_TSERIAL

#define AD_CHANNEL_KEY		10
#define AD_CHANNEL_TEMP	11
#define AD_CHANNEL_VOL		8
#define AD_CHANNEL_ZOOM     9

#define MOTO_LEVELS 12
#define EXT_ISO_LEVELS 34//31

#define RGB_REFINE_FLAG             0xB5//0xA5
#define FIBER_ADJUST_FLAG         0xA5

#define _HARD_WARE_ HARD_WARE_TSER

#define uchar unsigned char 
#define Uchar unsigned char 
#define uint unsigned int
typedef unsigned int WORD;
typedef unsigned char BYTE;
typedef unsigned int HpSetting;

#define SPI_NIKDAT_IN()			{SPI1->CR1 &= ~(1<<14);}
#define SPI_NIKDAT_OUT()			{SPI1->CR1 |= (1<<14);}

#define SET_RXONLY()				{SPI1->CR1 |= (1<<10);}
#define CLR_RXONLY()                      {SPI1->CR1 &= ~(1<<10);}

#define ID_INT_SBO		7
#define ID_INT_NSS		2//6

#define SET_MOSI_IN()				{GPIOA->CRL&=0x0FFFFFFF; GPIOA->CRL|=0x80000000; GPIOA->ODR|=0x80;}
#define SET_MOSI_OUT()			{GPIOA->CRL&=0x0FFFFFFF; GPIOA->CRL|=0xA0000000; }
#define SET_MOSI_DO()                   {GPIOA->CRL&=0x0FFFFFFF; GPIOA->CRL|=0x30000000; }
#define SET_MISO_IN()                    {GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x08000000; GPIOA->ODR|=0x40;}
#define SET_SCK_IN()				{GPIOA->CRL&=0xFF0FFFFF; GPIOA->CRL|=0x00800000;GPIOA->ODR |= 0x20;}
#define SET_SCK_OUT()                   {GPIOA->CRL&=0xFF0FFFFF; GPIOA->CRL|=0x00A00000;GPIOA->ODR |= 0x20;}
#define SET_SCK_HIGH()                 {GPIOA->CRL&=0xFF0FFFFF; GPIOA->CRL|=0x00300000;GPIOA->ODR |= 0x20;}
#define SET_SCK_LOW()                  {GPIOA->CRL&=0xFF0FFFFF; GPIOA->CRL|=0x00300000;GPIOA->ODR &= ~0x20;}
#define SET_NSS_IN()                     {GPIOA->CRL&=0xFFFFF0FF; GPIOA->CRL|=0x00000800;GPIOA->ODR |= 0x04;}        

#define PULL_MOSI_HIGH()             {SET_MOSI_DO(); NIKONMOSI = 1;}
#define PULL_MOSI_LOW()              {SET_MOSI_DO(); NIKONMOSI = 0;}

#define TEST_SIGNAL_HIGH()         {NIKONMOSI = 1;}
#define TEST_SIGNAL_LOW()          {NIKONMOSI = 0;}

#define KEY_POW_OFF	PAin(0)
#define PILOT_IN			PAin(1)
#define NIKONSCK 	PAout(5)//P0OUT3
#define NIKONSCKI	PAin(5)//P0IN3
#define NIKONMOSI 	PAout(7)//P0OUT2
#define NIKONMISO 	PAin(7)//P0IN2
//#define NIKONNSSI 	PAin(6)//P0IN4
//#define NIKONNSSO 	PAout(6)//P0OUT4
#define NIKONNSSI 	PAin(2)//P0IN4
#define NIKONNSSO 	PAout(2)//P0OUT4
#define SIGNAL1         PAout(3)
#define KEY_NIKMOD_MST   PAin(3)
#define FLASH_IN       PAin(4)
#define SPI_SS		PAout(6)
#define KEY_NIKMOD_NOR   PAin(13)
#define KEY_NIKMOD_SLV    PAin(14)
//#define SIGNAL_LED           PAout(8)
#define LIGHT_SLAVE_EN     PAout(8)

#define MOTOLOCA		PBin(1)
//PB2 boot
#define SENSOR_SW          PBout(3)
#define CAP_SENSOR_UP	PBin(4)
//PB5 FLOAT
//PB6 FLOAT
#define CAP_SENSOR_DN	PBin(7)
#define FLASH_PWM		PBout(8)
#define FLASH_ENA           PBin(9)
#define KEY_DIR_1		PBin(11)//PBin(10)//PDin(0)
#define KEY_DIR_2		PBin(10)//PBin(11)//PDin(1)
#define D3V3_EN			PBout(12)
//PB13 LCD_SCL
#define D30_EN			PBout(14)
//PB15 LCD_SDA
#ifdef SLAVE_DEBUG
#define SLV_DBG_PIN       PBout(8)
#endif
//PC0 key adc1
//PC1 tmp adc
//PC2 Vol adc
//PC3 FLASH_INT
#define CMD_THRESHOLD_INT	PCin(3)
//PC4 FLOAT
//PC5 FLOAT
//PC6 PHOTOSEN
#define MOTOIN2			PCout(7)
#define MOTOIN1			PCout(8)
#define SLA_FLA_IN          PCin(6)
//PC9 BUZZER
#define LCD_RST			PCout(10)
#define FLASH_TIG		PCout(11)
#define EXT_MODE_DETEC		PCin(12)
#define DEBUG_DATA			PCout(13)
#define DEBUG_CLK                  PCout(14)
//PC15 float

#define RGB_SCL			PDout(0)
#define RGB_SDA_O		PDout(1)
#define RGB_SDA_I		PDin(1)
#define RGB_INT			PDin(2)
#define EXT_CHARGE_CON PDout(3)
#define RGB_DET			PDin(4)
#define RGB_LED			PDout(5)
#define REF_BOARD_DETECT	PDin(6)
//PD7 float
#define LCD_CD			PDout(8)//PBout(10)
#define LCD_CS			PDout(9)//PBout(12)
#define BOOST_EN		PDout(10)
#define BK_LIGHT1		PDout(11)
#define BK_LIGHT2		PDout(12)
#define BK_LIGHT1_IN      PDin(11)
#define CHARGE_ENABLE	PDout(13)
#define CHARGE_CON		PDin(13)
//PD14 float
//#define CHARGE_LED1 		PDout(14)
#define CHARGE_LED2 		PDout(15)

#define HP_I2C_SDA		PEout(0)
#define HP_I2C_SCK		PEout(1)
//PE2 float
#define FLASH_AMP		PEout(3)
#define AF_CTL_BOTTOM	PEout(4)				//bottom
#define AF_CTL_LEFT   	PEout(5)				//left (light to you)
#define AF_CTL_RIGHT	PEout(6)				//right
//#define AF_CTL_IN		PEin(7)
//PE7 float
#define MOTOENA			PEout(8)

#define AF_OFF()                 {AF_CTL_BOTTOM=0;AF_CTL_LEFT=0;AF_CTL_RIGHT=0;}
#define AF_ALL()                 {AF_CTL_BOTTOM=1;AF_CTL_LEFT=1;AF_CTL_RIGHT=1;}
#define AF_LEFTBOTTOM()     {AF_CTL_BOTTOM=1;AF_CTL_LEFT=1;AF_CTL_RIGHT=0;}
#define AF_RIGHTBOTTOM()     {AF_CTL_BOTTOM=1;AF_CTL_LEFT=0;AF_CTL_RIGHT=1;}
#define AF_LEFT()                    {AF_CTL_LEFT=1;}
#define AF_RIGHT()                  {AF_CTL_RIGHT=1;}
#define AF_BOTTOM()               {AF_CTL_BOTTOM=1;}
//PE9 float

#define HC_CON2			PEout(10)
#define HC_CON1			PEout(11)
#define HC_CON3			PEout(12)
#define HC_CON4			PEout(13)
#define EXT_PA			PEout(14)
#define KEYLIGHT			PEout(15)

#define LOW_BAT_LEVEL	72//(1009>>4)
#define LITE_SLAVE_CAPTURE_TIME     1200//850


#ifdef FLASH_BASE_FIBER
#define FLA_FBR_MAX		6
#define FLA_FBR_MCR		2
#endif

#define CHARGE_LED2_ON()		{CHARGE_LED2=0;}
#define CHARGE_LED2_OFF()		{CHARGE_LED2=1;}
#define CHARGE_LED2_TOGGLE()      {CHARGE_LED2=!CHARGE_LED2;}

#define CHARGE_LED_OK()               {CHARGE_LED2_ON();}
#define CHARGE_LED_OFF()                {CHARGE_LED2_OFF();}

#define LIGHT_SLAVE_ENABLE()        {LIGHT_SLAVE_EN = 0;}
#define LIGHT_SLAVE_DISABLE()        {LIGHT_SLAVE_EN = 1;}

#define SET_KEYINT_RTIG()	{Ex_NVIC_Config(GPIO_B,10,RTIR);}
#define SET_KEYINT_FTIG()	{Ex_NVIC_Config(GPIO_B,10,FTIR);}
#define DISABLE_KEY_INT()   {Ex_NVIC_Disable(10);}
#define ENABLE_KEY_INT()    {Ex_NVIC_Enable(10);}
#define CLEAR_KEYINT()	{EXTI->PR=1<<10;}
#define CLEAR_PWINT()	{EXTI->PR=1<<13;}

#define DISABLE_D30EN_EX()	{D30_EN = 0;}
#define ENABLE_D30EN_EX()	{D30_EN = 1;}

#define DISABLE_CHARGE()		{CHARGE_ENABLE=1;}
#define ENABLE_CHARGE()		       {CHARGE_ENABLE=0;}

#define HaltSec(ms) OSTimeDlyHMSM(0,0,0,ms)

#define SPI_NSS_H     {LCD_CS=1;}//{SPI2->CR1 |= (1<<8);}
#define SPI_NSS_L	{LCD_CS=0;}//{SPI2->CR1&=~(1<<8);}

#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
#define GETBIT(targetbit,n) ((targetbit>>n)&0x01)
#define SETBIT(targetbit,n)	(targetbit|=(0x01<<n))
#define CLRBIT(targetbit,n)	 (targetbit&=(~(0x01<<n)))

//extern u8 STMFLASH_BUF[STM_SECTOR_SIZE];
extern char STMFLASH_BUF[STM_SECTOR_SIZE];
#define g_paraSetBuf STMFLASH_BUF

#if (defined SLAVE_DEBUG)
#define _DBG_PRINT_	 1
#else
#define _DBG_PRINT_	 0
#endif

#define WDT_ENABLE 1


#if (_DBG_PRINT_>0)
//#define DBG_PRINTF(format,args...) printf(format,args...)
#define DBG_PRINTF printf
#else
extern int noprintf(const char *fmt, ...);
#define DBG_PRINTF noprintf
#endif

//indicate the three mode after canon 2012
//#define CMD_MODE_OFF			0
//#define CMD_MODE_LIGH			1
//#define CMD_MODE_RF			2

#define SNR_MENU_CNT (19-2)
#define PREFLASH_1_INTERVAL 100
#define PREFLASH_2_INTERVAL	900

/*
#define COM_STATUE_START 0x01
#define COM_STATUE_IDLE 0x00
#define COM_STATUE_RUN		0x02
#define COM_STATUE_PREFLASH 0x03
#define COM_STATUE_REALFLASH 0x04
#define COM_STATUE_SLEEP    0x05
#define COM_STATUE_WLMPRE 0x06
#define COM_STATUE_MULTIFLASH 0x07
#define COM_STATUE_PWRDN	0X0A
*/

typedef enum COM_STATUES
{
	COM_STATUE_IDLE=0,
	COM_STATUE_START,
	COM_STATUE_RUN,
	COM_STATUE_PREFLASH,
	COM_STATUE_PREFLASH2,
	COM_STATUE_PREFLASH3,
	COM_STATUE_MSTPRE,
	COM_STATUE_REALFLASH,
	COM_STATUE_MSTCMD,
	COM_STATUE_PRESIGNAL,
	COM_STATUE_SLEEP,
	COM_STATUE_WLMPRE,
	COM_STATUE_MULTIFLASH,
	COM_STATUE_PWRDN,
	COM_STATUE_SLVPRE,
	COM_STATUE_SLVFLA,
	COM_STATUE_RPT_FLA,
	COM_STATUE_FLASH_READY,
	COM_STATUE_STBY,
	COM_STATUE_FOCUSDEPTH,
}ComStatues;

typedef enum FLASH_MODE
{
	FLASH_MODE_NORMAL,
	FLASH_MODE_REFLECT,
	FLASH_MODE_PARALLEL,
}FlashMode;

typedef enum SENSOR_STATUE
{
	SENSOR_UPDN_NOR,
	SENSOR_UPDN_UP,
	SENSOR_UPDN_DN,
}SensorStatue;

#define LAYOUT_MENU_PAGE                 11

#define WL_ARRARY_A			1// 0
#define WL_ARRARY_B 			2// 1
#define WL_ARRARY_C 			3// 2
#define WL_ARRARY_NULL 		3

#define WL_SCHEDULE_ALL		0X01
#define WL_SCHEDULE_RES		0X07
#define WL_SCHEDULE_AB		0X03
#define WL_HPSYN_ON		1
#define WL_HPSYN_OFF		0

#define WLM_MODE_ALL			0
#define WLM_MODE_AB			1
#define WLM_MODE_RES			2

#define SCROLL_UP	       1
#define SCROLL_DOWN	2

#define HOT_WARN_TEMPERATURE  1
#define HOT_WARN_TIMES               2

typedef enum SNR_TURNS
{
	SNR_APERT=0,
	SNR_MSTRPT,
	SNR_STEP,
	SNR_SU4,
	SNR_MODELING,
	SNR_PILOT,
	//SNR_FXDX,                     
	SNR_ZOOM,
	SNR_AF,
	//SNR_STBY,
	SNR_ISO,
	SNR_REAR,
	SNR_BKLIGHT,
	SNR_CONTRAST,
	SNR_STD,
	SNR_WP,
	SNR_MYMENU,
	SNR_VER,
	SNR_RESET,
	
}SNR_TURNS;

typedef enum MST_RPT_SEL
{
	MSTRPT_SEL_EXP=0,
	MSTRPT_SEL_TIMES,
	MSTRPT_SEL_FRQ,
	MSTRPT_SEL_M,
	MSTRPT_SEL_A,
	MSTRPT_SEL_B,
	MSTRPT_SEL_C,
}MstRptSel;

/*
typedef enum RMT_SU4_MODE
{
	RMT_SU4_M=0,
	RMT_SU4_OFF,
	RMT_SU4_AUTO,
	
}RmtSu4Mode;
*/

typedef enum MST_SU4_MODE
{
	MST_SU4_AUTO=2,
	MST_SU4_GN,
	MST_SU4_MANU,
}MstSu4Mode;

typedef enum NIK_WIRELESS_STATUE
{
	NIKWL_STATUE_HOLD=0,
	NIKWL_STATUE_ENSURECHAN,
	NIKWL_STATUE_RUN,
	NIKWL_STATUE_START,
	NIKWL_STATUE_STOP,
	NIKWL_STATUE_PLUSMEASURE,
	NIKWL_STATUE_PREFLASH,
	NIKWL_STATUE_FLASH,
}NikWirelessStatue;

typedef enum NIK_AF_MODE
{
	AF_MODE_ON,
	AF_MODE_OFF,
	AF_MODE_AFONLY,
}NikAfMode;

typedef enum ZOOM_FRAME_MOD
{
	ZOOM_FRAME_NONE,
	ZOOM_FRAME_FIXED,
	ZOOM_FRAME_SETA,
}ZoomFrameMod;
//#define SNRMENU_STD		0
/*
#define WORKINGMODE_TTL 		0
#define WORKINGMODE_MANU 		1
#define WORKINGMODE_RPT 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC 		5
#define WORKINGMODE_GR		6
#define WORKINGMODE_EXTA		8
*/
/*
#define WORKINGMODE_TTL 		0
#define WORKINGMODE_AUTO 		1
#define WORKINGMODE_GN       	2
#define WORKINGMODE_MANU	       3
#define WORKINGMODE_RPT		4
#define WORKINGMODE_OFF           5
*/

typedef enum WORKING_MODE
{
WORKINGMODE_TTL=0,
WORKINGMODE_TTL_POOR,
WORKINGMODE_AUTO, 	//	1
WORKINGMODE_GN,      // 	2
WORKINGMODE_MANU,	//       3
WORKINGMODE_RPT,	//	4
WORKINGMODE_OFF,   //        5
}WorkingMode;

typedef struct Rpt_Set
{
	signed char *rpt_exp1;
	signed char *rpt_exp2;
	signed char *eTimes;
	signed char *frequence;
	//int finalExpTimes;
	//u8 finalTimesBk;
	u8 finalExpTimes;
	unsigned char cmdCode;
	unsigned char mulCtl;
	//int extIso;
	//unsigned char WlmMul[3];
	//unsigned char *wlmMulExp;
//	unsigned char frqMode;
	unsigned char exp1;
	unsigned char exp2;
	int lowFrqCnt;
	unsigned char sCode;
	unsigned char maxTimes;
//	unsigned char cMode;
	
}RptSet;

typedef struct SLV_PILOT_SET
{
	u8 fla_en;
	s8 tms;
}SlvPilotSet;

#define CMD_MODE_OFF			0
#define CMD_MODE_LIGH_MST		1
#define CMD_MODE_LIGH_SLA		2

//#define WORKINGMODE_OFF		0xff
//#define WORKINGMODE_AUTO		2
//#define WORKINGMODE_TURNOFF	3

#define WIRELESS_MASTER_ON 1
#define MST_MODE_RADIO		0
#define MST_MODE_LIGHT		1

#define WLM_ON 			1
#define WLM_OFF 		0

#define WLM_CMDSEND_PREFLA_ALL	3
#define WLM_CMDSEND_PREFLA_CHA	4
#define WLM_CMDSEND_PREFLA_CHB	5
#define WLM_CMDSEND_PREFLA_CHC	6
#define WLM_CMDSEND_FLAIMF		7

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

#define WLM_CMDFLASH_CONFIRM		1
#define WLM_CMDFLASH_DENY			0

#define WLM_CMDPROC_ON 1
#define WLM_CMDPROC_OFF 0

#ifdef _HP_SYN_
#define HP_PREFLASH_INTERVAL	150
#define HP_REALFLA_INTERVAL	7628
#define HP_FLASHMODE_ON		1
#define HP_FLASHMODE_OFF		0
#endif

#define RADIO_STATUE_PRE			1
#define RADIO_STATUE_FLACFM		2
#define RADIO_STATUE_FLASH		3
#define RADIO_STATUE_RECOVER		4

#define SYNMODE_HEAD 0
#define SYNMODE_REAR 1
#define SYNMODE_HP	   2

#define MOTO_MODE_AUTO 0x00
#define MOTO_MODE_MANU 0x01

#define WLM_MASTON_BIT	1
#define WLM_WLSWTH_BIT	0

#define WLM_MASTER_FLA

#define WLM_MASTER_ON 		1
#define WLM_MASTER_OFF 		0

#define RF_INFOBIT_MODE	0
#define RF_INFOBIT_OUTPUT	1

//#define EXP_ACC_VALUE_WARN_LEVEL         (128*60)
#define EXP_ACC_VALUE_WARN_LEVEL         (128*40)

//#define BK_GREEN()		{BK_LIGHT1=1;BK_LIGHT2=0;}
//#define BK_ORANGE()        {BK_LIGHT1=1;BK_LIGHT2=1;}
//#define BK_TURNOFF()	{BK_LIGHT1=0;BK_LIGHT2=0;}

//#define BK_GREEN(x)		{if(!x){BK_LIGHT1=1;BK_LIGHT2=0;}else{BK_LIGHT2=1;BK_LIGHT1=0;}}
//#define BK_ORANGE(x)	{if(!x){BK_LIGHT1=1;BK_LIGHT2=1;}else{BK_LIGHT2=1;BK_LIGHT1=0;}}
//#define BK_TURNOFF(x)	{if(!x){BK_LIGHT1=0;BK_LIGHT2=0;}else{BK_LIGHT2=1;BK_LIGHT1=0;}}

#define BK_GREEN(x)		{ BK_LIGHT1=1;BK_LIGHT2=0; }
#define BK_ORANGE(x)	{ BK_LIGHT1=1;BK_LIGHT2=1; }
#define BK_TURNOFF(x)	{ BK_LIGHT1=0;BK_LIGHT2=0; }

#define KEY_BK_ON()		{KEYLIGHT = 1;}
#define KEY_BK_OFF()		{KEYLIGHT = 0;}
#define KEY_BK_TOGGLE()       {KEYLIGHT!=KEYLIGHT;}
#define OPEN_3V3()		{D3V3_EN = 0; }
#define CLOSE_3V3()		{D3V3_EN = 1;}

/*
typedef struct arrary_para
{
	unsigned char isMasterOn;
	unsigned char arrMode;
	unsigned char ABRation;
#ifdef CANON_RF
	unsigned char otpCode[3];
#else 
	unsigned char otpCode[5];
#endif
	//unsigned char AotpCode;
	//unsigned char BotpCode;
	//unsigned char CotpCode;
	unsigned char Aeps;
	unsigned char Beps;
	unsigned char Ceps;
	unsigned char Acmp;
	unsigned char Bcmp;
	unsigned char Ccmp;
	unsigned char ResArrCcmp;
}ArrPara;
*/

typedef struct RADIO_SETTING
{
	//unsigned char ratio;				//0==> All	1==> A:B		2==>A:B C
	unsigned char cmdReady;
	unsigned char cmdSta;
	unsigned char devLinked;
	//unsigned char arrExpVal[5];	
	//unsigned char grMods[RF_GROUP_TOTAL];
	unsigned char chan;
	unsigned int id;
}RadioSet;

typedef struct WIRELESS
{
//	unsigned char s_chan;
//	unsigned char s_arrary;
//	unsigned char s_wlMode;
//	unsigned char s_flashStatue;
//	unsigned char s_extCode;

//	unsigned char m_mode;
//	unsigned char m_on;
//	unsigned char m_chan;
//	unsigned char m_preIndex;
//	ArrPara m_arrPara;
	unsigned char isRear;
	unsigned char m_flaCmd[8];
//	unsigned char m_isMasterFla;
	char m_cmdIndex;
//	unsigned char m_ttlCmd[3];
#ifdef CANON_RF
	RadioSet rfSet;
#endif
//	unsigned char m_cmdSendId;
//	unsigned char m_cmdProcOn;
//	unsigned char pullFlag;
//	unsigned int muitl_realTimes;
//	unsigned int testCnt;
//	unsigned char testPilot;

//	unsigned char m_flaConfirm;
	
}Wl_set;

#define RF_GROUP_TOTAL	5
typedef struct RFGROUP_ARR_PARA
{
	char mode;
	char output1;
	char output2;
	unsigned char optCode;
	unsigned char ttlExpVal;
	char cmp1;
	char cmp2;
	char mulOtp;
}RfGroup_ArrPara;

//RfGroup_ArrPara  rfGr_arrPara[RF_GROUP_TOTAL]={0};

typedef struct RFGROUP_PARA
{
	unsigned char isoCmp;
	unsigned char shutter;
	unsigned char preflashIndex;
	unsigned char rfGrInfoFlag[5];					//uart info flag
	RfGroup_ArrPara  rfGr_arrPara[RF_GROUP_TOTAL];
}RfGroupPara;

extern RfGroupPara rfGrPara;

typedef struct MOTO_LOCA
{
	unsigned char mode;
	u16 moto_location;
}MotoLoca;

typedef struct WL_ARR_PARA
{
	signed char mode;
	signed char output1;
	signed char output2;
	signed char cmp1;
	signed char cmp2;
	signed char a_cmp1;
	signed char a_cmp2;
	signed char multi_exp1;
	signed char multi_exp2;
	signed char rpt_isOn;
	
}WL_ArrPara;

typedef struct SELF_PARA
{
	signed char mode;
	signed char output1;
	signed char output2;
	signed char cmp1;
	signed char cmp2;
	signed char multi_exp1;
	signed char multi_exp2;
	//signed char gn_m;
	//char gn_m;
	u16 gn_m;
	signed char a_cmp1;
	signed char a_cmp2;
	signed char gn_cmp1;
	signed char gn_cmp2;
	signed char rpt_times;
	signed char rpt_hz;
	signed char rmt_gr;
	signed char rmt_ch;
	//signed char rmt_su4_mode;
	//signed char mst_su4_mode;
	signed char hpFlash;
}Self_ArrPara;

#define MENU_ITEMS  19
#define MODE_COMMON_ARR	4

typedef struct MAIN_PARA_SET
{
	//signed char workMode;
	signed char  cmdMode;
	//signed char m_mstOn;
	signed char m_chan;	

	Self_ArrPara paraMst;

	WL_ArrPara modGr[MODE_COMMON_ARR];

	unsigned char s_chan;
	unsigned char s_array;
	unsigned char s_voiceOn;

	//int extIso;
	//int extApert;
	u16 extIso;
	u16 extApert;
	
	MotoLoca motoLoca;
	//unsigned char multi_exp;
	signed int multi_Frq;
	signed int multi_times;
	signed char synMode;

	signed char snrMenuSet[MENU_ITEMS];

#ifdef FIBER_ADJUST_PROC
	u8 isFiberAdjust;
	s8 fiberCoe;
	u8 isRgbChk;
#endif

	char sumVeri;

	char rsv;

//	int rsv[2];
}MainPara;

typedef struct MENU_PARA
{
	unsigned char mode;
	unsigned char otpCod;
	unsigned char motoCod;
	unsigned char motoLoc;

}MenuPara;

typedef struct MOTO_SET
{
	char motoDir;      //direction  马达方向，根据定位开关设置，压下定位开关为0，松开定位开关为1
	char moto_en;		//enable   马达移动使能
	int moto_step;		//indicate step to move 记录马达移动的步数
	char moto_ready;	//indicate finished a turn on set up  马达复位时置1， 复位完成清0
	//unsigned char current_location; 	  //当前的位置
	u16 current_location;
	u16 *dest_location;     //需要移动的目的位置
	u16 dspLocation;
	u16 autoDest;
	char destRsv;
#ifdef _CANON_
	unsigned char modRsv;
#endif
	char moving;		//indicate the moto is on the moving, so this is no response to new location 记录马达是否在转动
	u16 currentSet;
	char destSet;
	char focLen;		//显示在屏幕上的马达位置
	char autoRsv;
	char manuRsv;
	signed char focIndex;  //马达在手动模式下的索引值
	unsigned char *mode;			//mode manu or auto 莫达模式，0为auto， 1为manu
	//char motoRstCount; //通知主控停止充电位，当马达转动时，需要通知主控停止充电
	u16 motoRstCount;
	//char motoRstCount;  //复位的时间值，当超过一定时间马达复位没有完成时，显示马达异常
	char resetFlag;  //复位标志，当该标志置位时，马达复位
	char uartSendFlag;
}MotoSet;

typedef struct WLM_AB_RATION
{
	unsigned char para1;
	unsigned char para2;
}WLM_abRation_fillwords;

/*
typedef struct MULTI_Set
{
	unsigned char expose;
	int  eTimes;
	int frequence;
	int finalExpTimes;
	unsigned char cmdCode;
	unsigned char mulCtl;
	unsigned char WlmMul[3];
	unsigned char wlmMulExp[3];
//	unsigned char frqMode;
	int lowFrqCnt;
	unsigned char sCode;
	unsigned char maxTimes;
//	unsigned char cMode;
}MultiSet;
*/

typedef struct NIKON_WIRELESS
{
	//unsigned char WL_mode;
	unsigned char WL_opCode;
	unsigned char WL_output1;
	unsigned char WL_output2;
	unsigned char WL_Ttl;
	unsigned char WL_statue;
	unsigned char s_flashStatue;
	unsigned char s_wlMode;
	unsigned int WL_timeout;
	unsigned char chFlag;

	unsigned char WL_buf[WLBUF_LEN];

	unsigned char ttl_buf[3];
	unsigned char WLBufIndex;
	unsigned int PCA_timer_record;
	unsigned char WL_Channel;
	unsigned char bitLoadIndex;
	signed char preArr;
	unsigned char preTotal;
	unsigned char preTurn;
	unsigned char flash_wizard;

	unsigned char activeArr;
	unsigned char wlMul_frq;
	unsigned char wlMul_time;
	unsigned char wlMul_expose;
	unsigned char rpt_output1;
	unsigned char rpt_output2;

#ifdef _HP_SYN_
	unsigned char hpSyn;
	unsigned char hpLevel;
#endif

	unsigned char pilotFalg;
	unsigned char preflashLock;

	unsigned char preflashLockModeReserve[4];

	unsigned char rearReserve;
	
}NikWireless;

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

typedef struct FLASH_PARA
{
	int gnSt;
	int gnDst;
}FlashPara;

#define PAGE_MAX 13
#define LCD_W 172
#define LCD_H 104

#define BK_ON	1
#define BK_OFF	0

#define COMPROC_TASK_PRIO      5//9//#define LED_TASK_Prio       9
#define LED1_TASK_Prio      9//5
#define START_TASK_Prio     12//10
#define KEYPROC_TASK_Prio     10//9
#define MOTO_TICK_Prio		3
#define VOL_CK_Prio			7
#define RptFlash_TASK_Prio    4

#define TRANSFER_START		1	
#define TRANSFER_ONSEND	2
#define TRANSFER_READY 	0

#define TM_HANDLE_CLR                             0x00
#define TM_HANDLE_COMRST	                   0x01
#define TM_HANDLE_SLAVE_PRE2               0x02   
#define TM_HANDLE_MULTIFLASH	            0x04
#define TM_HANDLE_COMTIMEOUT              0x08

#define BUZZER_TIME_NORMAL 2
#define BACKLIGHT_DELAY_NORMAL 30

#define AD_CHANNEL_KEY1	10
#define AD_CHANNEL_KEY2	11
#define AD_CHANNEL_CKV		12

#define MOTO_MAX_INDEX			13//11//9
#define REFLECT_EN                 0

#define DSP_UPDATE_MODE	0x0001
#define DSP_UPDATE_SAVEPARA   0x0002

#define MOTO_INVALID_CNT	20               //20170311  +10

extern unsigned char g_tmHandleFlag;
extern signed char *g_cmdMode;
extern char g_SettingIndex;
extern Self_ArrPara *mstPara;
extern MotoSet motoSet;
extern MainPara mainPara;
extern signed char *pSnrSet;
extern u16 g_dspUpdateFlag;
//extern const u16 isoValue[];
extern const u32 isoValue[];
extern const u16 apertValue[];
extern s8 g_snrIndex;
extern s8 g_snrStart;
extern s8 g_snrSubIndex;
extern s8 g_snrMyMenuSelStart;
extern s8 g_snrMyMenuSelIndex;
extern u32 g_snrMyMenuBits;
extern u8 g_allMyMenuSw;
extern u8 g_GnScale[34];
extern u8 g_isHotProtect;
extern char g_chargeOk;
extern unsigned char g_comStatue;
extern char g_mstSetting;
extern NikWireless nikWireless;;
extern HpSetting hpSetting;
extern signed char g_expTTLPara1;
extern signed char g_expTTLPara2;
extern CmrPara cmrPara;
extern RptSet rptSet;
extern FlashPara flashPara;
extern const char g_subMenuCnt[SNR_MENU_CNT];
extern u8 g_stbyStatue;
extern u8 g_upDnSensor;
extern const char g_subMenuIsLoop[SNR_MENU_CNT];
extern char const focLenScape[MOTO_MAX_INDEX];
extern u8 g_isReflectPulled;
extern u16 g_flectDestRsv;
extern u8 g_flashMode ;
extern unsigned char macData2[];
extern u8 g_isAfOn;
extern u8 g_afOnCnt;
extern u8 g_dspCmpMod;
extern u8 g_isFlashWeak;
extern u8 g_FlashWeakLevels;
extern u8 ttl_pre2_flag;
extern SlvPilotSet slvPilotSet;
extern s32 g_expAccHotWarnValue;
extern u32 gWSleepCounter;
extern char g_mstFlaEnable;
extern char g_mstEnFlag;
extern char g_motoInvalidCounter;
//extern char g_golbalFlashEn;

#define SET_UPDATE_EVT(x)		{g_dspUpdateFlag|=x;}
#define CLR_UPDATE_EVT(x)           {g_dspUpdateFlag&=~x;}

#if OS_TMR_EN>0
extern OS_TMR  *g_bkTmr;
extern OS_TMR	*g_bzTmr;
extern OS_TMR	*g_flashTmr;
extern OS_TMR  *g_paraUpdateTmr;
extern OS_TMR  *g_shutterPullTmr;
extern OS_TMR  *g_1ShotTmr;
extern OS_TMR  *g_slaChanDivTmr;
#endif


#if OS_TMR_EN > 0
extern BOOLEAN OSTmrReload(OS_TMR  *pTmr,INT32U dly);
extern BOOLEAN  OSTmrStart (OS_TMR   *ptmr, INT8U    *perr);
#endif

extern void Timer4_Init(u16 arr, u16 psc);
extern void SetSBOInt(void);
extern void LightSlaveProc(u16 plusWidth);
extern void ParseMstCmd(void);
extern void PortInit(void);
extern void LCD_Init(void);
extern void LCD_Clear(u16 Color);
extern void LcdPortInit(void);
extern void DisplayPicture(u8 page, u8 column, u8 wide, u8 tall, const char * p, u8 rsv);
extern void DisplayPictureEx(u8 x,u8 y,u8 wide,u8 tall,const char *p,u8 rsv);
extern void PWM_Init(u16 arr, u16 psc, u16 duty);
extern void DspParaNum(u8 ft,u8 x, u8 y,u8 num,u8 rsv);
extern void ManuRptDrawScale(void);
extern void DspClearAllPara(void);
//extern void ModPasteScr(signed char mode);
extern void ModPasteScr(signed char mode, u8 isSu4);
extern void DspApertValue(u16 apert, u8 rsv);;
//extern void DspIsoValue(u16 iso,u8 rvs);
extern void DspIsoValue(u32 iso,u8 rvs);
//extern void DisplayFocLen(uchar motoMode,u16 foc,uchar rsv);
extern void DisplayFocLen(uchar motoMode,u16 foc,u8 frame,uchar rsv);
extern void LoadButtons(void);
extern char WordSearch(const u16* srcArr,char size,u16 cd);
extern void *DspSnrMenu(void);
extern void AreaScroll(u8 *sBuf, char wt,char ht,char aw,char dir,char n);
extern void LcdCopyToRam(u16 page, u16 cml,char *gBuf,u16 wt,u16 ht);
extern void LcdClearFeild(u16 x, u16 y, u16 wt, u16 ht);
extern void BuildSnrMenu(void);
extern void Mode(void);
extern void AreaSet(u16 page,u16 cml,u16 wt,u16 ht);
extern void LCD_WRITE_BYTE(char dat);
extern void CalcArrayMask(char* dest, char *src, u8 len);
//extern signed char GetMotoIncremental(signed char iFoc);
extern signed char GetMotoStdIndex(u16 dest);
extern void SubSnrClear(void);
extern void BufferScroll(u8 * buf, u8 len, u8 ht, u8 dir, u8 n);
extern void DspMyMenuSelect(void);
extern void SnrProcessBar(u8 pa, u8 cl, u8 foc, u8 cnt);
extern char CodeSearch(char *srcArr,char size,const char cd);
extern void CopyDataMaskToBuf(char*dest,const char*src,u8 len);
extern void BufAngleCut(char *buf, int len, char mask1, char mask2);
extern void DspFunctions(void);
extern void WLM_COMMAND_TRIGER(u16 time);
extern void SetChargeEnable(void);
extern signed char GetMotoIncremental(signed char iFoc);
extern void Timer2_Init(u16 arr, u16 psc);
extern void StopCapture(void);
extern void TmCaptureInit(u16 arr, u16 psc);
extern void LightSlaveParaClear(void);
extern void SetSpiComData(void);
extern void LightMasterScr(void);
extern void DspMstChan(u8 ch, u8 rvs);
extern void LightSlaveScr(void);
extern void TakeOnCharge(u8 mod);
extern void TurnOnChargeLed(u8 init);
extern void LoadLowBatIcon(void);
extern unsigned int OutputSwitch(s8 value1,s8 value2);
extern void WlmFlaADSet(s8 para1, s8 para2);
extern void TURN_ON_TRIGER(unsigned int time);
extern u8 FlashInResMode(signed char mode, signed char para1, signed char para2, unsigned char para3, unsigned char para4);
extern void MotoReset(void);
extern void ChkVoltage(void);
extern void MotoLocation(void);
extern void PortUninit(void);
//extern void LowPow_Init(u8 wkEn);
extern void LowPow_Init(u8 wkEn,u8 tMod,u8 wkSrc);
extern void MotoTickDriver(void);
extern void BufAngleCutEx(char *buf, int start,int end,int len, char mask1, char mask2);
extern void KeyAdcDisable(void);
extern unsigned char BuildWlmFlaCode(void);
extern void TakePreflash(u8 pid);
extern u8 GetBatteryAdc(void);
extern void WLM_SendData(unsigned char *Arr, char sendSize);
extern void TakeFlash(void);
extern void DisableUpdateEvent(void);
extern void SlaveTakePreflash(void);
extern void StartCapture(void);
extern void SlaveTakeFlash(void);
//extern void ExposeTimeCal(const unsigned char i_camCode,unsigned char hp);
extern void ExposeTimeCal(const unsigned char i_camCode,unsigned char hp,s8 cmp1, s8 cmp2);
extern char BuildRealFlashCmd(unsigned char para0,unsigned char para1,unsigned char para2,unsigned char para3,char hpLev);
extern void DspSubMenu(u8 subSel,u8 rsvId);
extern void SubMenuSetProc(void);
extern void RstComPara(void);
extern void BuildRfOnlyMenu(void);
extern void DspTempWarning(void);
extern void DspLightBody(u8 isAf, u8 isBk);
extern void SetBkLightInfo(u8 isOn);
extern void CmrLedCtrl(void);
extern void LoadMotoSetMenu(void);
extern void DspSubMenu(u8 subSel,u8 rsvId);
extern void KeyOk(void);
extern void SubMenuHandle(void);
extern void BuildKeyLockMenu(void);
extern void SetChargeDisable(void);
extern void BuildStbyMenu(void);
extern void SetContrast(char lev);
extern void BkLiteOn(void);
extern u16 GetMotoDest(u16 srcLoc);
extern int m2feet(int m);
extern unsigned char VerCodeCal(const unsigned char *arr);
extern void MultiMaxTimes(s8 frq,s8 exp1,s8 exp2,s8 *eTm,u8 *maxTm);
extern void BUZZER_ON(u16 duration,u16 times);
extern void SleepWakeIntSet(void);
extern void BuildSnrSubMenu(void);
extern void SetSpiComModeOff(u8 mod, u8 aSel);
extern void IWDG_Init(u8 prer,u16 rlr);
extern void IWDG_Feed(void);
//extern void BufFramed(u8 * buf,u8 st,u8 ed,u8 aLen,u8 sLen);

#endif


