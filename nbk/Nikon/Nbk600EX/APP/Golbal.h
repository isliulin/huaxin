#ifndef _OLOONG_GOLBAL_
#define _OLOONG_GOLBAL_

//#include "EepromChar.h"
//#include "menu.h"
#include "os_cpu.h"
#include "os_cfg.h"
//#include <stm32f10x_type.h>
#include "delay.h"
#include "uCOS_II.H"
#include "../HARDWARE/STMFLASH/stmflash.h"
#define _CANON_
#define _CAN_MODE_GR_
//#define _NIKON_
//#define _RMT_TEST_
#define _FONT_EEPROM_
#define MOTO_REFLECT_DETECT
//#define _220x176_
//#define _ST_EX_
#define _600EX_RT
//#define TTL_DEBUG
//#define _320x240_
//#define HW_VER_1
#define _MOTO_
#define EOS_750D
#define LITE_SLAVE_REVIEW
#define FLASH_BASE_FIBER
//#define LIGHT_SLAVE_DEBUG

//#define EXT_TEST
//#define EXT_TEST_PRT
//#define MOTO_TEST
//#define MOTO_DEBUG_PRT
//#define MOTO_TEST_DSP
//#define MOTO_DEBUG
//#define SIGNAL_TEST
//#define SWD_TEST	 
//#define DSP_TEST
//#define RGB_TEST
//#define SLAVE_TEST

#if(defined SLAVE_TEST)
#define DBG_PRINTF printf
#else
extern int noprintf(const char *fmt, ...);
#define DBG_PRINTF noprintf
#endif

//#define _128x64_
//#define _132x64_

//#ifndef _ST_EX_
#define WLM_MASTER_FLA
#define FIBER_ADJUST_PROC
#define SLEEP_PWR_OFF
#define SLAVE_POWER_DOWN
//#define MOTO_RESET_AD						//2015/10/10
//#define RF_LOWPWR_SLEEP
//#define FIBER_ADJUST_TEST
//#endif
//#define KEY_DEBUG

#define HARD_WARE_VER1 1
#define HARD_WARE_VER2 2
#define HARD_WARE_TSER 3
#define HARD_WARE_9325 4

//#define _HARD_WARE_ HARD_WARE_VER2
//#define _HARD_WARE_ HARD_WARE_VER1
//#define _HARD_WARE_ HARD_WARE_TSER
#define _HARD_WARE_ HARD_WARE_TSER
#define _SPEED_LIGHT_


#if(_HARD_WARE_==HARD_WARE_VER1)
#define _220x176_
#define LCD_DRIVER_SUMSUNG
#elif(_HARD_WARE_==HARD_WARE_VER2)
#define _320x240_
#define LCD_DRIVER_ILI9320
#elif(_HARD_WARE_==HARD_WARE_TSER)
#define _172x104_
#define LCD_DRIVER_TSERIAL
//#define DRV_DMA_TEST
#define SPI_NSS_H     {LCD_CS=1;}//{SPI2->CR1 |= (1<<8);}
#define SPI_NSS_L	{LCD_CS=0;}//{SPI2->CR1&=~(1<<8);}
#define AD_CHANNEL_KEY		10
#define AD_CHANNEL_TEMP	11
#define AD_CHANNEL_VOL		8
#define AD_CHANNEL_ZOOM     9
#elif(_HARD_WARE_==HARD_WARE_9325)
#define _320x240_
#define LCD_DRIVER_ILI9325
#endif

#define RGB_TCS3414 1
#define RGB_BH1745   2
//#define RGB_IC RGB_TCS3414   
#define RGB_IC RGB_BH1745   

#define FLAOK_ON()             {FLA_OK_LED=0;}
#define FLAOK_OFF()		{FLA_OK_LED=1;}
#define FLAOK_TOGGLE()	{FLA_OK_LED=!FLA_OK_LED;}
#ifdef RF_LOWPWR_SLEEP
#define ENABLE_RF_MODULE()     {/*CC2530_RESET = 1;*/}
#define DISABLE_RF_MODULE()	{/*CC2530_RESET = 0;*/g_isRfModuleLinked=0;}
#else
//#define ENABLE_RF_MODULE()     {CC2530_RESET = 1;CC2530_PWR = 1;}
//#define DISABLE_RF_MODULE()	{CC2530_RESET = 0;CC2530_PWR = 0;g_isRfModuleLinked=0;}
#define ENABLE_RF_MODULE()     {CC2530_RESET = 1;/*CC2530_PWR = 1;*/}
#define DISABLE_RF_MODULE()	{CC2530_RESET = 0;/*CC2530_PWR = 0;*/g_isRfModuleLinked=0;}
#endif
#define RGB_LED_ON()				{RGB_LED=1;}
#define RGB_LED_OFF()				{RGB_LED=0;}

//extern u8 STMFLASH_BUF[STM_SECTOR_SIZE];
extern char STMFLASH_BUF[STM_SECTOR_SIZE];
//extern char g_paraSetBuf[1024];
#define g_paraSetBuf STMFLASH_BUF

/*
#ifdef _220x176_
#define LCD_DRIVER_SUMSUNG
#define _HARD_WARE_ HARD_WARE_VER1
#endif

#ifdef _320x240_
#define LCD_DRIVER_ILI9320
#define _HARD_WARE_ HARD_WARE_VER2
#endif
*/

#define AD_CHANNEL_KEY1	10
#define AD_CHANNEL_KEY2	11
#define AD_CHANNEL_CKV		12

#define TM_HANDLE_CLR			0
#define TM_HANDLE_REAR	 		1
#define TM_HANDLE_SHUT			2	
#define TM_HANDLE_MULTIFLASH	3

#define MOTO_INVALID_CNT	10
#define NEW_DSP_SCHEME 1			//define in menu.h

#define code const
//#define EEPROM_PARA_BURN
//#define HZH_IN_SOC

//#define U08 unsigned char
#define Uint unsigned int

#define uchar unsigned char 
#define Uchar unsigned char 
#define uint unsigned int 
typedef unsigned int HpSetting;
extern HpSetting hpSetting;
//typedef unsigned int u32;

#define RGB_CHECK_TIMES 5

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

#define ISO_ARR_CNT    34
#define APERT_ARR_CNT 40

#define BK_ON	1
#define BK_OFF	0

#define VER_CAMERA_OLD		0
#define VER_CAMERA_NEW	1

#define SENSOR_DSP_LEN	28//27
#define SENSOR_UPDN_DOWN 1
#define SENSOR_UPDN_UP		2
#define SENSOR_RGB			1
#define SENSOR_MS_MST		1
#define SENSOR_MS_SLA		2


#define SCROLL_UP	1
#define SCROLL_DOWN	2

#define DA_MAX 65535

#define HaltSec(ms) OSTimeDlyHMSM(0,0,0,ms)

#define SET_INT5SRC_SCK()           {Ex_NVIC_Config(GPIO_A,5,RTIR); }
#define SET_INT5SRC_UART()	      {Ex_NVIC_Config(GPIO_B,5,FTIR); }

/*****************************************************************************************************
* the following macro is define to indicate the respective hardware
*
*****************************************************************************************************/

//#define REF_BOARD_DETECT	PAout(4)//P4IN2
#define SPI_MISO_AUX	PCout(4)
//#define FLASH_CONTROL	PBout(8)
#define FLASH_OUT 		PAout(6)
#define CHARGE_ENABLE	PDout(13)//PCout(10)
#define CHARGE_CON		PDin(13)
//#define HC_CON1			PBout(12)
#define FLASH_PWM		PBout(8)
#define CMD_THRESHOLD_INT	PCin(3)
#define EXT_MODE_DETEC		PCin(12)
//#define ACT_SHUTTER		PBout(15)			//initiative triger shutter
//#define WIRE_SHUTTER	PBout(14)
#define HP_I2C_SIN		PBin(11)
//#define CHARGE_LED 		PAout(15)
#define CHARGE_LED1 		PDout(14)
#define CHARGE_LED2 		PDout(15)
#define FLA_OK_LED 		PEout(2)//PDout(2)
//#define BK_LIGHT			PBout(9)
#define BP_IN			PAin(4)
#define CLK_IN			PAin(5)
#define CR_READY		PAout(8)				//if pull down, the camera before 2012 will not take flash
#define SET_MISO_FREE()         {GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x03000000;GPIOA->ODR&=0xFFBF;}
#define SET_MISO_ONRUN()      {GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x0B000000;}

#if(_HARD_WARE_==HARD_WARE_TSER)
//#define _MOTO_
//#define COM_AUX			PDout(10)//PAout(13)
//#define COM_AUX			PCout(5)
#define COM_AUX				PEout(9)
//#define PILOT_IN			PAin(0)
#ifdef NBK600EX_DEBUG
#define PILOT_IN			PAin(13)
#else
#define PILOT_IN			PAin(1)
#endif
#define KEY_POW_OFF	PAin(0)
#define KEY_DIR_1		PBin(11)//PBin(10)//PDin(0)
#define KEY_DIR_2		PBin(10)//PBin(11)//PDin(1)
#define CAP_SENSOR_UP	PBin(4)
#define CAP_SENSOR_DN	PBin(7)
//#define LCD_RESET		PBout(11)
#define LCD_CD			PDout(8)//PBout(10)
#define LCD_CS			PDout(9)//PBout(12)
#define FLASH_TIG		PCout(11)
#define HC_CON1			PEout(11)
#define HC_CON2			PEout(10)
#define HC_CON3			PEout(12)
#define HC_CON4			PEout(13)
//#define ACT_SHUTTER		PDout(2)
#define ACT_SHUTTER		PCout(15)
#define WIRE_SHUTTER	PCout(5)
#define REF_BOARD_DETECT	PDin(6)
#define RGB_DET			PDin(4)
#define RGB_LED			PDout(5)
//#define HP_I2C_SCK		PBout(0)
//#define HP_I2C_SDA		PBout(1)
//#define HP_I2C_SCK		PEout(0)
//#define HP_I2C_SDA		PEout(1)
#define HP_I2C_SCK		PEout(1)
#define HP_I2C_SDA		PEout(0)
#define RGB_SCL			PDout(0)
#define RGB_SDA_O		PDout(1)
#define RGB_SDA_I		PDin(1)
#define D3V3_EN			PBout(12)
#define D30_EN			PBout(14)
#define LCD_RST			PCout(10)
#define UART_INT		PBout(5)
#define UART_INT_IN		PBin(5)
//#define CC2530_RESET	PBout(6)
//#define CC2530_PWR		PDout(7)		//2015/11/27
#define CC2530_RESET      PDout(7)
#define CC2530_PWR         PBout(6)
//#define MOTOLOCA		PDin(7)
#define MOTOLOCA		PBin(1)
//#define MOTOIN1			PEout(7)
//#define MOTOIN2			PEout(9)
#define MOTOIN1			PCout(8)
#define MOTOIN2			PCout(7)
#define MOTOENA			PEout(8)
#define KEYLIGHT			PEout(15)
#define EXT_PA			PEout(14)
#define LSEN				PBout(3)
#define EXT_CHARGE_CON PDout(3)
#ifdef NBK600EX_DEBUG
#define GREEN_LED       	PAout(8)
#define RED_LED       		PDout(2)
#endif

#ifdef FLASH_BASE_FIBER
#define FLASH_AMP		PEout(3)
#define FLA_FBR_MAX		6
#define FLA_FBR_MCR		2
#endif

#ifdef SIGNAL_TEST
#define SIGNAL_PIN1		PAout(11)
#define SIGNAL_PIN2		PAout(12)
#endif
//#define BOOST_EN		PDout(10)
//#define RGB_INT			PDout(2)
//#define RGB_SYNC		PBout(4)
//#define HC_CON1			PBout(1)
//#define ACT_SHUTTER		PBout(2)
//#define HP_I2C_SCK		PBout(3)
//#define HP_I2C_SDA		PBout(4)
#define SET_CS_ENABLE	{LCD_CS=0;}
#define SET_CS_DISABLE	{LCD_CS=1;}
#define SET_CD_DATA	{LCD_CD=1;}
//#define BK_LIGHT			PBout(9)
#define BK_LIGHT1		PDout(11)
#define BK_LIGHT2		PDout(12)
#define DISABLE_KEY_INT()   {Ex_NVIC_Disable(10);}
#define ENABLE_KEY_INT()    {Ex_NVIC_Enable(10);}
#define DISABLE_SYN_INT()   {Ex_NVIC_Disable(5);}
#define ENABLE_SYN_INT()     {Ex_NVIC_Enable(5);}
#define DISABLE_FLASH_INT() {Ex_NVIC_Disable(4);}
#define ENABLE_FLASH_INT()   {Ex_NVIC_Enable(4);}
#define SET_KEYINT_RTIG()	{Ex_NVIC_Config(GPIO_B,10,RTIR);}
#define SET_KEYINT_FTIG()	{Ex_NVIC_Config(GPIO_B,10,FTIR);}
#define CLEAR_KEYINT()	{EXTI->PR=1<<10;}
#define CLEAR_PWINT()	{EXTI->PR=1<<13;}
#else
#endif

#define BK_GREEN(x)		{if(!x){BK_LIGHT1=1;BK_LIGHT2=0;}else{BK_LIGHT2=1;BK_LIGHT1=0;}}
#define BK_ORANGE(x)	{if(!x){BK_LIGHT1=1;BK_LIGHT2=1;}else{BK_LIGHT2=1;BK_LIGHT1=0;}}
#define BK_TURNOFF(x)	{if(!x){BK_LIGHT1=0;BK_LIGHT2=0;}else{BK_LIGHT2=1;BK_LIGHT1=0;}}

#define KEY_BK_ON()		{KEYLIGHT = 1;}
#define KEY_BK_OFF()		{KEYLIGHT = 0;}
#define OPEN_3V3()		{D3V3_EN = 0; }
#define CLOSE_3V3()		{D3V3_EN = 1;}

#define DISABLE_D30EN()	     	//{D30_EN = 0;}//{D30_EN = 0;}
#define ENABLE_D30EN()		//{D30_EN = 1;}


#define DISABLE_D30EN_EX()	{D30_EN = 0;}
#define ENABLE_D30EN_EX()	{D30_EN = 1;}



//#define POW_ON			PCout(11)
#define BOOST_EN		PDout(10)
#define KEY_LOCK_EX		PAin(13)//PCin(9)
#define KEY_POW_ON		PAin(14)//PCin(8)
//#define AF_CTL_IN		PBin(13)
//#define AF_CTL_OUT		PCout(7)
#define AF_CTL_OUT1		PEout(4)				//bottom
#define AF_CTL_OUT2		PEout(5)				//left (light to you)
#define AF_CTL_OUT3		PEout(6)				//right
#define AF_CTL_IN		PEin(7)
#define AF_CTL_OUT		AF_CTL_OUT1
#define AF_LED1			AF_CTL_OUT1//AF_CTL_OUT2
#define AF_UNDERSIDE	AF_CTL_OUT1
#define AF_LEFT			AF_CTL_OUT2
#define AF_RIGHT		AF_CTL_OUT3

#define AF_OFF()			{AF_UNDERSIDE=0;AF_RIGHT=0;AF_RIGHT=0;}
/*****************************************************************************************************
*end
*****************************************************************************************************/

//????????????????
#define LED_STK_SIZE		64//256//64
#define CANCOM_STK_SIZE     128//64
#define KEYPROC_STK_SIZE    128//128//512//256//128//64
#define REFRESH_STK_SIZE	128
#define CMDSEND_STK_SIZE	128
#define START_STK_SIZE  		128//(256+64+64)//128
#define TICK_STK_SIZE		128
#define VOL_CK_SIZE			64
#define MOTO_TICK_SIZE		64


//??????????????
//#define CANCOM_TASK_Prio       5
#define KEYPROC_TASK_Prio      9
#define START_TASK_Prio     17
#define LED_TASK_Prio		8//6
#define REFRESH_TASK_Prio	6//8
#define CMDSEND_TASK_Prio	2//5
#define MOTO_TICK_Prio		3
#define TICK_TASK_Prio		5
#define VOL_CK_Prio			10


#define LIGHT_READY		//{LIGHT_GREEN = 1;LIGHT_RED= 0;LowBatChk = 0;}
#define LIGHT_CHARGE	//{LIGHT_GREEN = 0;LIGHT_RED= 1;}
//#define I2C_DELAY
//#define IICSDA_IN		//{GPIOB->CRH &= 0xFFFF0FFF; GPIOB->CRH |= 0x00008000;}
//#define IICSDA_OUT		//{GPIOB->CRH &= 0xFFFF0FFF; GPIOB->CRH |= 0x00003000;}

#define CMD_THRESHOLD_VAL	0x0200

#define LITE_SLAVE_CAPTURE_TIME     850

#define SET_SPIO_ON			{GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x0B000000;}
#define SET_SPIO_OFF		{GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x03000000;}

#define CHARGE_LED1_ON		{CHARGE_LED1=0;}
#define CHARGE_LED1_OFF		{CHARGE_LED1=1;}
#define CHARGE_LED1_TOGGLE	{CHARGE_LED1=!CHARGE_LED1;}

#define CHARGE_LED2_ON		{CHARGE_LED2=0;}
#define CHARGE_LED2_OFF		{CHARGE_LED2=1;}

	
#define CHARGE_LED_RDY()			{CHARGE_LED1_ON;CHARGE_LED2_OFF;}
#define CHARGE_LED_OK()				{CHARGE_LED2_ON;CHARGE_LED1_OFF;}
#define CHARGE_LED_OFF()			{CHARGE_LED1_OFF;CHARGE_LED2_OFF;}

#define PUSH_MISO			{GPIOA->ODR &= 0xFFBF;}

#ifdef _CANON_
#define WLM_MODE_ALL			0
#define WLM_MODE_AB			1
#define WLM_MODE_RES			2
#endif


//extern void ParaPreserve(Uchar paraLoc,Uchar para);
extern Uchar ParaRead(Uchar paraLoc);
extern void KeyOk(void);

#define SETTING_BIT_MSC		0x80

#define RGB_REFINE_FLAG             0xB5//0xA5
#define FIBER_ADJUST_FLAG         0xA5


typedef	unsigned char	Byte;
typedef	unsigned int	Word;
typedef	unsigned long	Dword;
//#define	MAKEWORD(v1,v2)	(((Word)(v1)<<8) + (Word)(v2))
//#define SETBIT(ch,index)	{ch |= index;}
//#define CLRBIT(ch,index)	{ch &=~index;}
//#define GETBIT(ch,index)	(ch&index)
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

typedef struct MENU_PARA
{
	unsigned char mode;
	//unsigned char mstTtl;
	signed char mstTtl;
	unsigned char otpCod[3];
	unsigned char WlmMul[3];
	unsigned char rfGrOtp[5];
	unsigned char rfGrTmp[5];
	unsigned char motoCod;
	unsigned char motoLoc;
	unsigned char motoIndex;
	unsigned char feb;
	unsigned char cApert;

}MenuPara;


#define BUZZER_TIME_NORMAL 2
#define BACKLIGHT_DELAY_NORMAL 30

void BUZZER_ON(u16 duration,u16 times);
//#define BUZZER_ON(duration,times) 	{if(GETBIT(g_SeniorSetting,SETTING_BIT_MSC)) {BuzPwmStart();g_bzDuration=duration;g_bzCount=times;g_bzStart=1;}}
//#define BUZZER_ON(duration,times) 	{PWM_Init(2500,9,1250);OSTmrStart(g_osTmr, &tmrErr) ;}//{if(g_snrMenuSet[SNRMENU_SOUND]) {BuzPwmStart();g_bzDuration=duration;g_bzCount=times;g_bzStart=1;}}
#define BUZZER_OFF		 			//{BuzPwmStop();g_bzCount=0;g_bzStart=0;g_bzDuration=0;}
#define BACKLIGHT_ON(count) 		{BkLiteOn();}//{if(g_snrMenuSet[SNRMENU_BKLITE]){BACKLIGHT=1;g_bkCount=count;g_bkStart=1;}}
#define BACKLIGHT_OFF				//{BACKLIGHT=0;g_bkCount=0;g_bkStart=0;}

extern unsigned int g_key1;
extern unsigned int g_key2;
//extern unsigned char short_only_once;
//extern unsigned char long_keypress_flag;
extern unsigned char sample;
extern unsigned int g_bzDuration;
extern char g_bzCount;
extern char g_bzStart;
extern char g_keyAdcEnable;
//extern unsigned char g_SeniorSetting;
//extern char g_bkStart;
//extern char g_bkCount ;
extern unsigned char g_lowBatCheck;
//extern Byte code focLenScape[12];
//extern Byte code focLenScape[11];
extern Byte code focLenScape[10];
//extern unsigned char code focLenVal[53];
//extern char g_snrMenuSet[MENU_ITEMS];
extern Byte gBStaFlag;
extern unsigned char g_temper;
extern Byte LowBatChk;
extern char g_snrCnt;
extern unsigned char g_fpLoc[2];
extern unsigned char g_grDspStart;
extern unsigned char const compensation_table[37];
extern unsigned char const compensation_table_5[];
extern unsigned char code output_table[22];
extern MenuPara menuPara;
extern unsigned int pTime;
extern u16 penSetArr[3];
extern u16 bkSetArr[3];
extern char g_cmrLinked;
extern char g_cmrPressed;
extern u8 g_isRgbFilter;
extern unsigned char g_comStatue;
extern u8 g_rgbStyle;
extern u32 g_colorTempRt;
extern u16 g_dspUpdateFlag;
extern char g_chargeOk;
extern u8 g_rfSlaveFlashCode;
extern const u16 g_extExpCode[];
extern Byte code focLenCode[10];
extern const char cmpArr_5[][4];
extern const char cmpString_5[];
extern const char cmpStringEx_5[];
extern u8 g_menuLock;
extern u8 g_menuModRsv;
extern u8 g_motoModRsv;
extern u8 g_motoDestRsv;
extern u8 g_cmdModRsv;
extern u8 g_isFullFrame;
extern u8 g_stepLength;
extern u8 g_isRfModuleLinked;
extern u8 g_isHotProtect;
extern u32 g_expAccHotWarnValue;
extern u8 g_isReflectPulled;

//extern SnrMenu snrMenu;

#define WLM_CMDPROC_OFF 	0
#define WLM_CMDPROC_DAT 	1
#define WLM_CMDPROC_FLA 	2
#define WLM_CMDPROC_PRF 	3	
#define WLM_CMDPROC_MUL	4
//#define WLM_CMDPROC_SINGLE	6
#define WLM_CMDPROC_TTLDEBUG	5

#define TEMP_WARN_SHUTDOWN_QUIT		5000
#define EXP_ACC_VALUE_WARN_LEVEL         (128*60)

#define HOT_WARN_TEMPERATURE  1
#define HOT_WARN_TIMES               2

#define CMD_MODE_OFF			0
#define CMD_MODE_RF_MST		1
#define CMD_MODE_RF_SLA		2
#define CMD_MODE_LIGH_MST		3
#define CMD_MODE_LIGH_SLA		4


#define CMD_MODE_RFLS_SLA		5
#define CMD_MODE_RFLS_MST		6

#define DSP_STYLE_CANON	0
#define DSP_STYLE_CLASSIC	1

#define CMR_ADEPT_OLD		0
#define CMR_ADEPT_NEW		1

#define DISABLE_CHARGE()		{CHARGE_ENABLE=1;}
#define ENABLE_CHARGE()		{CHARGE_ENABLE=0;}
#define TOGGLE_CHARGE()		{CHARGE_ENABLE=!CHARGE_ENABLE;}

extern void MotoReset(void);
extern void DspClearAllPara(void);
extern void MultiMaxTimes(int frq,unsigned char exp,int *eTm,unsigned char *maxTm);
extern void DspID(u8 sel,u8 rvs,unsigned int rfId);
extern unsigned int CanOutputSwitch(s8 value1,s8 value2);
extern void TURN_ON_TRIGER(unsigned int time);
extern void PWM_Init(u16 arr,u16 psc,u16 duty);
extern INT8U  OSTimeDlyHMSM (INT8U hours, INT8U minutes, INT8U seconds, INT16U milli);
extern void  Adc_Init(void);
extern void PortInit(void);
extern void uart2_init(u32 pclk2,u32 bound);
extern void  OSInit (void);
#if OS_TMR_EN > 0
extern BOOLEAN OSTmrReload(OS_TMR  *pTmr,INT32U dly);
extern BOOLEAN  OSTmrStart (OS_TMR   *ptmr, INT8U    *perr);
#endif
//extern void Mode(signed char mode);
extern void Mode(void);
extern void BuildExposeCmd(char mode,unsigned char hpSyn,unsigned char hpLex);
extern void WLM_SendData(char *Arr, char sendSize);
extern void HpExpCodeSend(unsigned int hpv);
extern void TakePreFlash(u16 time);
extern void CanExposeTimeCal(const unsigned char o_camCode,unsigned char fpMod);
extern void SetExtmScr(void);
extern void UserInit(u32 paraAddr, u8 initMod);
extern void DspFebIcon(char rvs);
extern char ExtmIsoIndex(int iso);
extern char ExtmApertIndex(int apert);
extern void LightSlaveProc(u16 plusWidth);
extern void ParseMstCmd(void);
extern void AreaSet(u16 page,u16 cml,u16 wt,u16 ht);
extern void LCD_WRITE_BYTE(char dat);
extern char LoadCmpFebChar(char ch,char *gBuf,char dspLen,unsigned char rvs);
extern void LoadMaskData(char *gBuf, char dspLen, char mask1, char mask2,char n);
extern char RamLoadPicture(char *p,char wt, char ht ,char *gBuf, char dspLen, unsigned char rvs);
extern void LoadCmpFebData(char *gBuf, const char *p, u8 rvs);

extern void DisplayMstOtp_canStyle(u16 page,u16 cml,
							     char mod,char dspLen,char blockLen,
							     unsigned char para1,unsigned char para2,
							     u8 spLen,char *pDot,
							     char dir,char n,
							     unsigned char rvs);
/*
void DisplayTtlCmp_canStyle(u16 page,u16 cml,
								char bias,
								CmpPoint cp,
								signed char cmp1,signed char cmp2,
								//char chWt,char dspLen,
								float chWt,char dspLen,
								char dir,char n,
								unsigned char rvs);
*/
extern void LcdCopyToRam(u16 page, u16 cml,char *gBuf,u16 wt,u16 ht) ;
extern char LoadMenuChar(char ch,char *gBuf,char dspLen,unsigned char rvs);
//extern void DspSlavePara(u8 rvs);
extern void DspSlavePara(char mod, char para1, char para2, char arr, char rvs);
extern void LayModeExtA(void);
extern void LayModeExtM(void);
extern void DspIsoValue(u16 x, u16 y,int iso,u8 rvs);
extern void DspApertValue(int apert, u8 rvs);
extern void DrawScale(signed char mode,unsigned char st,unsigned char isHp,u8 para,char on);
extern void DspMstRatioHead(u16 x, u16 y, const char* p, u8 rsv,u8 sz);
extern void DspGrMode(u16 x, u16 y, const char* p, u8 rsv,u8 sz);
extern void EnterCfnSubMenu(char sel);
extern void TmCaptureInit(u16 arr, u16 psc);
extern void ClearWlcBuf(void);
extern void StopCapture(void);
extern void StartCapture(void);
extern void RfArrSelMenu(void);
extern void RfgrSelMenu(void);
extern void DspCfnMenu(void);
extern void DspPfnMenu(void);
extern void EnterPfnSubMenu(char sel);
extern void *BtnSnrMenu(void);
extern void *BtnPfnMenu(void);
extern void *BtnGrParaRes(void);
extern void LoadPfnString(char *str,char *gBuf,char dspLen,unsigned char rvs);
extern void MultiFlashPrepear(void);
extern void FlashInResMode(signed char mode, signed char para1, signed char para2, unsigned char para3);
extern void SetMultiFlashTimer(void);
//extern void ExtModSet(void);
//extern void ExtmModSet(signed char  foc, int iso, int apert);
extern u8 ExtmModSet(signed char  foc, int iso, int apert);
//extern void ExtaModSet(signed char foc, unsigned char isoCmp, unsigned char apert,
extern u8 ExtaModSet(signed char foc, unsigned char isoCmp, unsigned char apert,
					signed char cmp1, signed char cmp2, 
					signed char feb1, signed char feb2, unsigned char febCnt);
extern void ExtModFlash(u16 time);
extern void SetContrast(char lev);
extern void LcdPortInit(void);
extern signed char GetMotoStdIndex(u8 dest);
extern void MotoProc(void);
extern void Sleep_Init(void);
extern void PortUninit(void);
extern void MotoLocation(void);
extern char HpExpCalc(char exp1,char exp2,char cmp1,char cmp2,unsigned int piso,HpSetting *hpSt,unsigned char ttlAccFlag);
extern void HighSpeedTriger(HpSetting *hps,unsigned int tmOv);
extern void HpPreflash(unsigned int tmOv);
extern void SpiRearUp(u16 tks);
extern void SetPreFlashDA(void);
extern void LCD_Clear(u16 Color);
extern void LCD_Init(void);
//extern unsigned char WlMultiFrqConver(unsigned char rec);
extern u16 WlMultiFrqConver(unsigned char rec);
extern unsigned char WlMultiTimeConver(unsigned char rec);
extern void EXTIx_Init(u8 GPIOx,u8 TirEdge);
//extern void TCS3414_ReadChan(u16 *ch1, u16 *ch2, u16 *ch3, u16 *ch4);
extern void Rgb_ReadChan(u16 *ch1, u16 *ch2, u16 *ch3, u16 *ch4);
//extern void TCS3414_Init(void);
extern void Rgb_Init(void);
//extern void StartTcs3414(void);
extern void StartRgbSample(void);
//extern void StopTcs3414(void);
extern void StopRgbSample(void);
extern u8 GetRgbCvtState(void);
extern void RfMoudleSet(void);
extern void LightSlaveParaClear(void);
extern u8 GetBatteryAdc(void);
extern void DspLightSlavePara(char rvs);
extern void DspRfSlavePara(char rvs);
extern void SetMultiTmFrq(unsigned char mulTimes, int mulFrq, char sel);
extern void DspMotoWarn(void);
extern float CalcCmpPower(u8 tms);
extern void DisplayOff(void);
extern void SavePara(u32 sAddr);
extern unsigned char CalcSum(u8 *rcv, u8 len);
extern void TurnOnChargeLed(u8 init);
extern void TTLModFlash(u16 time);
extern void FiberAdjustProc(void);
extern u8 IsModSwitchValid(void);
extern void TakeOnCharge(u8 mod);
extern void SetChargeEnable(void);
extern void SetChargeDisable(void);
extern void DrawFrame(void);
extern void SlaveSleepWakeIntSet(void);
extern void RCC_Configuration(void);
extern void SlaveSleepWakeIntClear(void);
extern void UserScreenOn(void);
extern u8 RfMasterReadyCheck(void);
extern void RfMasterReadySet(void);
extern void KeyAdcEnable(void);
extern void KeyAdcDisable(void);
extern void WlmCmdHeadSend(u16 time);
extern u8 WaitForUartIdle(u8 tick);
#ifdef FLASH_BASE_FIBER
extern void HP_SHOT_ONCE(u16 time);
extern void WlmFlaADSet(s8 para1, s8 para2);
#endif
//extern void ReadRgbChannals(u16 * rgbs);

#endif





