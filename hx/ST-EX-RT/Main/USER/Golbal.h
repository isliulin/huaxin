#ifndef _OLOONG_GOLBAL_
#define _OLOONG_GOLBAL_

//#include "EepromChar.h"
//#include "menu.h"
#include "os_cpu.h"
#include "os_cfg.h"
//#include <stm32f10x_type.h>
#include "delay.h"
#include "uCOS_II.H"
#define _CANON_
#define _CAN_MODE_GR_
//#define _NIKON_
//#define _RMT_TEST_
#define _FONT_EEPROM_
//#define _220x176_
#define _ST_EX_
//#define _LOGO_DSP_
//#define _320x240_
//#define HW_VER_1
//#define MOTO_VERSION
//#define _MOTO_
#define EOS_750D

//#define _128x64_
//#define _132x64_
#define _SN_DEBUG_
#ifdef _SN_DEBUG_
//#define BOOT_UPDATE
#define _LOGO_DSP_
//#define _NO_SN_
//#define _RF_ONLY_
#endif

//#define _RF_ONLY_

#define _NO_SN_

#ifndef _ST_EX_
#define WLM_MASTER_FLA
#endif

//#define _TUHAO_START_
//#define UART1_TRACE_DEBUG
//#define UARTPIN_DEBUG

#define HARD_WARE_VER1 	1
#define HARD_WARE_VER2 	2
#define HARD_WARE_VER3 	3
#define HARD_WARE_RFONLY	4

//#define _HARD_WARE_ HARD_WARE_VER2
//#define _HARD_WARE_ HARD_WARE_VER1
//#define _HARD_WARE_ HARD_WARE_VER3
#define _HARD_WARE_ HARD_WARE_RFONLY

#if(_HARD_WARE_==HARD_WARE_VER1)
#define _220x176_
#define LCD_DRIVER_SUMSUNG
#define LCD_RESET_MANU
#elif(_HARD_WARE_==HARD_WARE_VER2)
#define _320x240_
#define LCD_DRIVER_ILI9320
#elif(_HARD_WARE_==HARD_WARE_VER3)
#define _220x176_
#define LCD_DRIVER_SUMSUNG
#elif(_HARD_WARE_==HARD_WARE_RFONLY)
#define _220x176_
#define LCD_DRIVER_SUMSUNG
#endif

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
#if(_HARD_WARE_==HARD_WARE_VER1)
#define AD_CHANNEL_KEY1	10
#define AD_CHANNEL_KEY2	11
#define AD_CHANNEL_CKV		12
#elif(_HARD_WARE_==HARD_WARE_VER2)
#define AD_CHANNEL_KEY1	10
#define AD_CHANNEL_KEY2	11
#define AD_CHANNEL_CKV		12
#elif(_HARD_WARE_==HARD_WARE_VER3)
#define AD_CHANNEL_KEY		10
#define AD_CHANNEL_BAT		11
#define AD_CHANNEL_CKV		12
#elif(_HARD_WARE_==HARD_WARE_RFONLY)
#define AD_CHANNEL_KEY		10
#define AD_CHANNEL_BAT		11
#define AD_CHANNEL_CKV		12
#endif

#define TM_HANDLE_CLR			0
#define TM_HANDLE_REAR	 		1
#define TM_HANDLE_SHUT			2	

#define BK_PWM_ARR				2500

#define NEW_DSP_SCHEME 1			//define in menu.h

#define BAT_CHECK_VAL	155//70
#define BAT_UNPWR          139

#define code const
//#define EEPROM_PARA_BURN
//#define HZH_IN_SOC

//#define U08 unsigned char
#define Uint unsigned int

#define uchar unsigned char 
#define Uchar unsigned char 
#define uint unsigned int 

//typedef unsigned int u32;

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

#define RF_CHAN_AUTO 0

#define TTL_FEB_EN      1//0		
#define TTL_FEB_DEN    0//1/

#define BK_ON	1
#define BK_OFF	0

#define VER_CAMERA_OLD		0
#define VER_CAMERA_NEW	1

/*****************************************************************************************************
* the following macro is define to indicate the respective hardware
*
*****************************************************************************************************/

#define REF_BOARD_DETECT	PAout(4)//P4IN2
#define SPI_MISO_AUX	PCout(4)
//#define FLASH_CONTROL	PBout(8)
#define FLASH_OUT 		PAout(6)
#define ACT_SHUTTER		PBout(15)			//initiative triger shutter
#define WIRE_SHUTTER	PBout(14)
//#define CHARGE_LED 		PAout(15)
#define BK_LIGHT			PBout(9)
#define BP_IN			PAin(4)				//
#define CLK_IN			PAin(5)
#define CLKSYN_PULLUP() {PAout(5) = 1;}
#define CR_READY		PAout(8)				//if pull down, the camera before 2012 will not take flash

#if (_HARD_WARE_==HARD_WARE_VER1)
#define COM_AUX			PCout(5)//PAout(13)
#define PILOT_IN			PAin(14)
#define KEY_DIR_1		PAin(0)
#define KEY_DIR_2		PAin(1)
#define CLEAR_KEYINT()	{EXTI->PR=1<<1;}
#define DISABLE_DIRINT() Ex_NVIC_Disable(1)
#define SNR_DSP_DOWN 0
#define CHARGE_ENABLE	PCout(10)
#define HC_CON1			PBout(12)
#define FLASH_PWM		PBout(8)
#define CMD_THRESHOLD_INT	PCin(3)
#define HP_I2C_SCK		PBout(10)
#define HP_I2C_SDA		PBout(11)
#define HP_I2C_SIN		PBin(11)
#define FLA_OK_LED 		PDout(2)
#define CHARGE_LED 		PAout(15)

#define ENABLE_RF_MODULE()     	{CC2530_RST=1;}
#define DISABLE_RF_MODULE()   	{CC2530_RST=0;}

#elif(_HARD_WARE_==HARD_WARE_VER2)
#define COM_AUX			PDout(10)//PAout(13)
#define PILOT_IN			PAin(0)
#define KEY_DIR_1		PDin(0)
#define KEY_DIR_2		PDin(1)
#define CLEAR_KEYINT()	{EXTI->PR=1<<1;}
#define DISABLE_DIRINT() Ex_NVIC_Disable(1)
#define SNR_DSP_DOWN 0
#define CHARGE_ENABLE	PCout(10)
#define HC_CON1			PBout(12)
#define FLASH_PWM		PBout(8)
#define CMD_THRESHOLD_INT	PCin(3)
#define HP_I2C_SCK		PBout(10)
#define HP_I2C_SDA		PBout(11)
#define HP_I2C_SIN		PBin(11)
#define FLA_OK_LED 		PDout(2)
#define CHARGE_LED 		PAout(15)
#define ENABLE_RF_MODULE()     	{CC2530_RST=1;}
#define DISABLE_RF_MODULE()   	{CC2530_RST=0;}

#elif(_HARD_WARE_==HARD_WARE_VER3)
#define CC2530_RST		PCout(5)
#define COM_AUX			PAout(1)
#define PILOT_IN			PAin(0)
#define KEY_DIR_1		PBin(11)
#define KEY_DIR_2		PBin(10)
#define CLEAR_KEYINT()	{EXTI->PR=1<<10;}
#define SNR_DSP_DOWN 1
#define DISABLE_DIRINT() Ex_NVIC_Disable(10)
#define CHARGE_ENABLE	PCout(10)
#define HC_CON1			PBout(12)
#define FLASH_PWM		PBout(8)
#define CMD_THRESHOLD_INT	PCin(3)
#define FLA_OK_LED 		PDout(2)
#define CHARGE_LED 		PAout(15)
//#ifdef UARTPIN_DEBUG
//#define DEBUG_PIN1          PAout(9)
//#define DEBUG_PIN2          PAout(10)
//#endif

#define HP_I2C_SCK		PBout(10)
#define HP_I2C_SDA		PBout(11)
#define HP_I2C_SIN		PBin(11)

#define DISABLE_CHARGE		{CHARGE_ENABLE=1;}
#define ENABLE_CHARGE		{CHARGE_ENABLE=0;}

#define ENABLE_RF_MODULE()     	{CC2530_RST=1;}
#define DISABLE_RF_MODULE()   	{CC2530_RST=0;}

#elif(_HARD_WARE_==HARD_WARE_RFONLY)

//#define CC2530_RST		PCout(5)
#define POWER_D3V3         PCout(5)
#define COM_AUX			PAout(1)
#define PILOT_IN			PAin(0)
#define KEY_DIR_1		PBin(11)
#define KEY_DIR_2		PBin(10)
#define UART_INFO		PCout(10)
#define LCD_RST			PBout(8)
#define CHARGE_LED 		PCout(2)//PAout(15)

#define CLEAR_KEYINT()	{EXTI->PR=1<<10;}
#define SNR_DSP_DOWN 1
#define DISABLE_DIRINT() Ex_NVIC_Disable(10)
#define KEY_BK			PAout(14)
#define FLA_OK_LED 		PCout(3)//PDout(2)
#define DISABLE_KEY_INT()   {Ex_NVIC_Disable(10);}
#define ENABLE_KEY_INT()    {Ex_NVIC_Enable(10);}
#define DISABLE_SYN_INT()   {Ex_NVIC_Disable(5);}
#define ENABLE_SYN_INT()     {Ex_NVIC_Enable(5);}
#define DISABLE_FLASH_INT() {Ex_NVIC_Disable(4);}
#define ENABLE_FLASH_INT()   {Ex_NVIC_Enable(4);}
#define SET_KEYINT_RTIG()	{Ex_NVIC_Config(GPIO_B,10,RTIR);}
#define SET_KEYINT_FTIG()	{Ex_NVIC_Config(GPIO_B,10,FTIR);}
#define CLEAR_KEYINT()		{EXTI->PR=1<<10;}

#define ENABLE_RF_MODULE()
#define DISABLE_RF_MODULE()

#define PERIPHERAL_PWR_ON()		{POWER_D3V3=0;}
#define PERIPHERAL_PWR_OFF()		{POWER_D3V3=1;}
			

#define DISABLE_CHARGE
#define ENABLE_CHARGE
#define _RF_ONLY_
//#define _UART_INT_INFO_
#define _UART_INT_INFO_

#endif



#define POW_ON			PCout(11)
#define KEY_LOCK_EX		PCin(9)
#define KEY_POW_ON		PCin(8)
#define AF_CTL_IN		PBin(13)
#define AF_CTL_OUT		PCout(7)

#define FLAOK_ON()             {FLA_OK_LED=0;}
#define FLAOK_OFF()		{FLA_OK_LED=1;}
#define FLAOK_TOGGLE()	{FLA_OK_LED=!FLA_OK_LED;}


//#define ENABLE_RF_MODULE()     	{CC2530_RST=1;}
//#define DISABLE_RF_MODULE()   	{CC2530_RST=0;}

#define UART2_LOCK()		{GPIOA->CRL&=0XFFFF00FF;GPIOA->CRL|=0X00008800;}
#define UART2_UNLOCK()	{GPIOA->CRL&=0XFFFF00FF;GPIOA->CRL|=0X00008B00;}

/*****************************************************************************************************
*end
*****************************************************************************************************/

//设置任务堆栈大小
#define LED_STK_SIZE		64
#define CANCOM_STK_SIZE     128//64
#define KEYPROC_STK_SIZE    128//512//256//128//64
#define REFRESH_STK_SIZE	128
#define CMDSEND_STK_SIZE	128
#define START_STK_SIZE  		128//(256+64+64)//128
#define TICK_STK_SIZE		128


//设置任务优先级
//#define CANCOM_TASK_Prio       5
#define KEYPROC_TASK_Prio      9
#define START_TASK_Prio     17
#define LED_TASK_Prio		8//6
#define REFRESH_TASK_Prio	6//8
#define CMDSEND_TASK_Prio	2//5
#define TICK_TASK_Prio		5


#define LIGHT_READY		//{LIGHT_GREEN = 1;LIGHT_RED= 0;LowBatChk = 0;}
#define LIGHT_CHARGE	//{LIGHT_GREEN = 0;LIGHT_RED= 1;}
#define I2C_DELAY
#define IICSDA_IN		{GPIOB->CRH &= 0xFFFF0FFF; GPIOB->CRH |= 0x00008000;}
#define IICSDA_OUT		{GPIOB->CRH &= 0xFFFF0FFF; GPIOB->CRH |= 0x00003000;}

#define CMD_THRESHOLD_VAL	0x0200

#define SET_SPIO_ON			{GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x0B000000;}
#define SET_SPIO_OFF		{GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x03000000;}

#define CHARGE_LED_ON		{CHARGE_LED=0;}
#define CHARGE_LED_OFF		{CHARGE_LED=1;}

#define SET_MISO_FREE()         {GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x03000000;GPIOA->ODR&=0xFFBF;}
#define SET_MISO_ONRUN()      {GPIOA->CRL&=0xF0FFFFFF; GPIOA->CRL|=0x0B000000;}

#define PUSH_MISO			{GPIOA->ODR &= 0xFFBF;}

#define FLASH_PILOT_ON          1
#define FLASH_PILOT_OFF        0

#ifdef _CANON_
#define WLM_MODE_ALL			0
#define WLM_MODE_AB			1
#define WLM_MODE_RES			2
#endif


//extern void ParaPreserve(Uchar paraLoc,Uchar para);
extern Uchar ParaRead(Uchar paraLoc);
extern void KeyOk(void);

#define SETTING_BIT_MSC		0x80


typedef	unsigned char	Byte;
typedef	unsigned int	Word;
typedef	unsigned long	Dword;
//#define	MAKEWORD(v1,v2)	(((Word)(v1)<<8) + (Word)(v2))
//#define SETBIT(ch,index)	{ch |= index;}
//#define CLRBIT(ch,index)	{ch &=~index;}
//#define GETBIT(ch,index)	(ch&index)
#define H_COV(x)    ((x)>>3)//(((x)>>4)*2+(((x)&0x0f)>>3))
#define L_COV(x)    (((x)&0x07)/3)//((((x)&0x0f)>=0x08)?((((x)&0x0f)-0x08)/3):(((x)&0x0f)/3))

#define EXPH_COV(x)		(H_COV(0x38-(x)))
#define EXPL_COV(x)		(L_COV(0x38-(x)))

#define H_COV_DIV2(x)	((x)>>3)
#define L_COV_DIV2(x)	(((x)&0x07)/2)

#define CMPH_COV(x)	(((signed char)(x)>0)?(-1)*H_COV_DIV2((signed char)(x+7)):H_COV_DIV2((signed char)(x)*(-1)))
#define CMPL_COV(x) (((signed char)(x)>0)?L_COV_DIV2((signed char)(x*(-1))):L_COV_DIV2((signed char)(x)*(-1)))

#define SET_UPDATE_EVT(x)		{g_dspUpdateFlag|=x;}

typedef struct MENU_PARA
{
	unsigned char mode;
	//unsigned char mstTtl;
	unsigned char otpCod[3];
	unsigned char WlmMul[3];
	unsigned char rfGrOtp[5];
	//unsigned char rfGrTmp[5];
	signed char rfGrTmp[5];
	unsigned char motoCod;
	unsigned char motoLoc;
	//unsigned char feb;

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
//extern unsigned char g_SeniorSetting;
//extern char g_bkStart;
//extern char g_bkCount ;
extern unsigned char g_lowBatCheck;
extern Byte code focLenScape[11];
extern unsigned char code focLenVal[53];
//extern char g_snrMenuSet[MENU_ITEMS];
extern Byte gBStaFlag;
extern unsigned char g_temper;
extern Byte LowBatChk;
extern char g_snrCnt;
extern unsigned char g_fpLoc[2];
extern unsigned char g_grDspStart;
//extern unsigned char const compensation_table[37];
extern signed char const compensation_table[37];
extern unsigned char code output_table[22];
extern MenuPara menuPara;
extern unsigned int pTime;
extern u16 penSetArr[3];
extern u16 bkSetArr[3];
extern char g_cmrPressed;
extern char g_SettingIndex;
extern char g_isChScan;
extern u16 g_dspUpdateFlag;
//extern SnrMenu snrMenu;

#define TEMP_WARN_SHUTDOWN_QUIT		5000

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


extern void MotoReset(void);
extern void DspClearAllPara(void);
extern void MultiMaxTimes(int frq,unsigned char exp,int *eTm,unsigned char *maxTm);
extern void DspID(u8 sel,unsigned int rfId);
extern unsigned int CanOutputSwitch(unsigned char mode);
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
extern BOOLEAN  OSTmrStop (OS_TMR  *ptmr, INT8U    opt,void    *callback_arg,   INT8U   *perr);
#endif
extern void Mode(void);
extern void BuildExposeCmd(char mode,unsigned char hpSyn,unsigned char hpLex);
extern void WLM_SendData(char *Arr, char sendSize);
extern void CmdModDisplay(void);
extern void RfMoudleSet(void);
extern void RePaintScreen(void);


#endif





