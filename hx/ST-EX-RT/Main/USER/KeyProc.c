#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "led.h" 
#include "canon.h"
#include "menu.h"
#include "../SYSTEM/usart/usart.h"
#include "../HARDWARE/LCD/Font.h"
#include "../HARDWARE/LCD/lcd.h"
#include "../HARDWARE/STMFLASH/stmflash.h"

//#define KEY_DEBUG
#define KEY_DIR_LEFT	1
#define KEY_DIR_RIGHT	2

//StruMode	struMode;
FlaParameter flaPara={0};
MotoSet motoSet={0};
//MasterPara masterPara;
WlSet wlSet;
//MultiSet multiSet;
char g_SettingIndex = 0;
char g_cmrLinked=0;
char g_SettingRsv=0;
char g_modeSwitch=0;
unsigned int g_bzDuration=0;
char g_bzCount = 0;
char g_bzStart = 0;
//char g_bkStart = 0;
//char g_bkCount = 0;
char g_idSetIndex = 0;
char g_chargeOk=0;
char g_cmrPressed=0;
//char g_lock = 0;
//unsigned char g_SeniorSetting=0;
Byte  g_zoom_statue = 0;
Word gWSleepCounter=0;
unsigned char uart_tran_flag=0;
unsigned char g_fpLoc[2];
unsigned int pTime;
extern char *pSnrSet;
//unsigned int g_key1;
//unsigned int g_key2;
char g_snrCnt;
//unsigned char g_wlCmdMod=CMD_MODE_RF;
extern RadioSet *radioSet;
extern unsigned int g_Key;
unsigned char g_menuId=0;
FuncList *pMenuList=NULL;
extern FuncMenu funcMenu;
unsigned char g_LockMusic=0;
extern unsigned char *g_cmdMode;
extern MultiSet multiSet;
extern unsigned char g_grSel;
unsigned char g_keyDir=0;
extern RadioStatue radioStatue;
extern u16 g_dspUpdateFlag;
extern OS_TMR  *g_CcRstDownTmr;
//extern u8 g_bkState;
//extern u8 g_bkFlag;
BkParas bkParas;
extern unsigned char g_shutterFlag;
extern unsigned char g_grSelRsv;
extern u8 g_dspChinese;
extern u8 g_isSmExpand;
//u8 g_setFlag=0;

signed char g_rfChanSet=0;
signed char g_liteChanSet=0;
unsigned int g_rfIdSet=0;
extern unsigned char abRation_fill1,abRation_fill2;
extern WLM_abRation_fillwords const wlm_abRation_fillwords[13];
extern unsigned char const feb_table[10];

extern unsigned char g_grStyle;

//void Mode(signed char mode);
void KeyLeft(signed char mode);
void KeyRight(signed char mode);
//void KeyOk();
void ModLinkedShot(u8 rfMod);
void ResetKeyInt(void);
extern void ParaFormat(void);
extern void RfParaSend(void);
extern void Pilot(void);
extern void DspSnrMenu(void);

typedef struct KEY_HANDLE
{
//void (* hKeyFunc1)(void*);
//void (* hKeyFunc2)(void*);
//void (* hKeyFunc3)(void*);
//void (* hKeyMenu)(void*);
void (* hKeyLeft)(void*);
void (* hKeyRight)(void*);
void (* hKeyOk)(void*);
void (* hKeyMod)(void*);
}KeyHandle;

//static KeyHandle keyHandle={&KeyLeft,&KeyRight,&KeyOk,&Mode};



#define ADCHAN1_KEY_INVALID  24//0xf0
#define ADCHAN2_KEY_INVALID  0x1f0
#define RKEY_INVALID	0x00
#define RKEY_CH_1		0x10
#define RKEY_CH_2		0x02

#ifdef _CANON_
unsigned char canMaxCalc[][9] = {
{100,90,80,70,60,50,45,45,40},//{90,80,70,55,45,35,35,30,25},
{90,80,70,55,45,35,35,30,25},//{90,80,70,55,45,35,35,30,25},
{60,50,40,35,30,25,25,20,15},//{60,50,40,35,30,25,25,20,15},
{30,20,20,20,20,20,10,10, 8},//{30,20,20,20,20,20,10,10, 8},
{14,12,10, 8, 6, 6, 5, 5, 4},//{14,12,10, 8, 6, 6, 5, 5, 4},
{7,6,5,4,3,2,2,2,2},//{7,6,5,4,3,2,2,2,2},
};
#else
unsigned char multiMaxFlaTime[][9] = {
{90,80,70,56,44,36,32,28,24},
{90,80,70,56,44,36,32,28,24},
{60,50,40,32,28,24,22,20,12},
{30,20,20,20,20,20,10,10, 8},
{14,12,10, 8, 6, 6, 5, 5, 4},
};
#endif
//按键读程序，具有长按键和短按键功能
//void HaltSec(uint ms)
//{
	//return;
//	OSTimeDlyHMSM(0,0,0,ms);	
//}
#define HaltSec(ms) OSTimeDlyHMSM(0,0,0,ms)

unsigned char KeyValid(u16 sKey)
{
	if(sKey<(ADCHAN1_KEY_INVALID<<8))
	{
		if((sKey&0xff)<ADCHAN1_KEY_INVALID)
		{
			return RKEY_INVALID;
		}else 
		{
			return RKEY_CH_1;
		}
	}else
	{
		return RKEY_CH_2;
	}
}

u16 KeyReArmed(u16 sKey)
{
	if(sKey<(ADCHAN1_KEY_INVALID<<8))
	{
		return (sKey&0xff);
	}else
	{
		return ((sKey>>8)|0x100);
	}
}

u8 ReadPilot()
{
	static unsigned char pilotEn=0;
	if(PILOT_IN)
	{
		pilotEn = 1;
		return RKEY_INVALID;
	}else
	{
		HaltSec(40);
		if(!PILOT_IN)
		{
			if(pilotEn)
			{
				pilotEn = 0;
				return KEY_PILOT;
				
			}
			return RKEY_INVALID;
		}
	}
	return RKEY_INVALID;
}

unsigned char  ReadKey()
{
 	static unsigned char short_once=0;//短按键标志
	static unsigned char long_once=0;
	//static unsigned char pilotEn=0;
	static unsigned int keyTimes=0;
	//static unsigned char long_do_only_one;
	//static unsigned char do_only_one;
	unsigned int key,vKey;
	unsigned char keyIndex=0;
	//unsigned char keynum=0;
	
	//if((g_key1>ADCHAN1_KEY_INVALID)&&(g_key2>ADCHAN2_KEY_INVALID))//no press key
	key = GetKeyAdc();
	//printf("%x\r\n ",key);
	//HaltSec(40);
	//return;

	if(!KeyValid(key))
	{
#if 0
		if(PILOT_IN)
		{
			keytimedata = 0;
			short_only_once = 0;
			pilotEn = 1;
			return RKEY_INVALID;
		}else
		{
			HaltSec(40);
			if(!PILOT_IN)
			{
				if(pilotEn)
				{
					pilotEn = 0;
					return KEY_PILOT;
					
				}
				return RKEY_INVALID;
			}
		}
#endif

		key = ReadPilot();
		if(RKEY_INVALID==key)
		{
			keyTimes = 0;
			short_once = 0;
			long_once = 0;
		}else
		{
			return KEY_PILOT;
		}

#if(_HARD_WARE_==HARD_WARE_RFONLY)
		if(KEY_DIR_LEFT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_LEFT;
		}else if(KEY_DIR_RIGHT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_RIGHT;
		}
#else
		if(1==g_keyDir)
		{
			g_keyDir = 0;
			//HaltSec(5);
			//HaltSec(1);
			if(!KEY_DIR_2)
			{
				if(KEY_DIR_1)
				{
					//HaltSec(1);
					//FLASH_CONTROL = !FLASH_CONTROL;
					if(KEY_DIR_1)
					{
						return KEY_RIGHT;
					}
				}else
				{
					//HaltSec(1);
					//FLASH_CONTROL = !FLASH_CONTROL;
					if(!KEY_DIR_1)
					{
						return KEY_LEFT;
					}
				}
				//g_keyDir = 0;
			}
			HaltSec(50);
			//EXTI->PR=1<<1;				//clear interrupt flag
			CLEAR_KEYINT();
			ResetKeyInt();
			//Ex_NVIC_Enable(1);
			return RKEY_INVALID;
			
		}
#endif

		/*
		if(KEY_DIR_LEFT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_LEFT;
		}else if(KEY_DIR_RIGHT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_RIGHT;
		}*/
		
		
	}else
	{
		//printf("%x, ",key);
		HaltSec(20);
		key = GetKeyAdc();
		if(!KeyValid(key))
		{
			return RKEY_INVALID;
		}else
		{
			HaltSec(10);
			vKey = GetKeyAdc();
			if(!KeyValid(vKey))
			{
				return RKEY_INVALID;
			}

			key = KeyReArmed(key);
			vKey = KeyReArmed(vKey);
	
			if((key>(vKey+5))||(key<(vKey-5)))
			{
				//printf("Key err, %x, %x\n",key, vKey);
				return RKEY_INVALID;
			}
		}		
	}

	if((key<KEY0_VALUE_MAX)&&(key>KEY0_VALUE_MIN))
	{
		keyIndex = KEY_RF_SW;
	}else if((key<KEY1_VALUE_MAX)&&(key>KEY1_VALUE_MIN))
	{
		keyIndex = KEY_FUNC1;
	}else if((key<KEY2_VALUE_MAX)&&(key>KEY2_VALUE_MIN))
	{
		keyIndex = KEY_FUNC2;
	}else if((key<KEY3_VALUE_MAX)&&(key>KEY3_VALUE_MIN))
	{
		keyIndex = KEY_FUNC3;
	}else if((key<KEY4_VALUE_MAX)&&(key>KEY4_VALUE_MIN))
	{
		keyIndex = KEY_MENU_SW;
	}else if((key<KEY5_VALUE_MAX)&&(key>KEY5_VALUE_MIN))
	{
		keyIndex = KEY_MODE;
	}else if((key<KEY6_VALUE_MAX)&&(key>KEY6_VALUE_MIN))
	{
		keyIndex = KEY_LEFT;
	}else if((key<KEY7_VALUE_MAX)&&(key>KEY7_VALUE_MIN))
	{
		keyIndex = KEY_OK;
	}else if((key<KEY8_VALUE_MAX)&&(key>KEY8_VALUE_MIN))
	{
		keyIndex = KEY_RIGHT;
	}else if((key<KEY23_VALUE_MAX)&&(key>KEY23_VALUE_MIN))
	{
		keyIndex = KEY_COMBIN;
	}

	//if((KEY_RF_SW<=keyIndex)&&(keyIndex<=KEY_RIGHT))
	if((KEY_RF_SW<=keyIndex)&&(keyIndex<=KEY_COMBIN))
	{
		keyTimes++;
		if(!short_once)
		{
			short_once=1;
			return keyIndex;
		}else if(keyTimes>LONG_KEYPRESS_TIME)
		{
			if(!long_once)
			{
				long_once = 1;
				return (keyIndex|0x10);
			}
		}
	}

	return RKEY_INVALID;
}

void SetCmpFeb(signed char comp1,signed char comp2,signed char feb1,signed char feb2,char sel)
{
	signed char value1,value2;
	//LCD_TTLPARA_CLEAR;
	
#ifdef _CANON_
	//if(!comp1&&!comp2&&!feb1&&!feb2) return;
	/*
	if(!comp1&&!comp2&&!feb1&&!feb2)		//display cmp
	{
		if(SETTING_ADDSUB==sel)
		{
			LCD_CMP_ON(DSP_SELE);
		}else
		{
			LCD_CMP_CLEAR;
		}
		//return;
	}else
	{
		//LCD_CMP_ON(DSP_REVERSE);
		//LCD_CMP_ON(lcd_rev);
		LCD_CMP_ON(SETTING_ADDSUB==sel?DSP_SELE:DSP_NORMAL);
	}*/
	switch(flaPara.FEB_count)
	{
		//case 0:
		//case 3:
		//value1= comp1;
		//value2= comp2;
		//break;

		case 1:
		value1= comp1;
		value2= comp2;	
		break;

		case 2:
		value1 = comp1 - feb1;
		value2 = comp2 - feb2;
		break;

		case 3:
		value1= comp1 + feb1;
		value2= comp2 + feb2;
		break;

		default:
		value1= comp1;
		value2= comp2;
		break;
	}

	if(value2<0) {
			value1--;
			value2 = 3 + value2;		
		}
		else if(value2>2)
		{
			value1++;
			value2 = value2 - 3;
		}

	if(SETTING_TTLFEB==sel)
	{
		LCD_FEB_ON(DSP_SELE);
		CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
	}else if(SETTING_ADDSUB==sel)
	{
		LCD_CMP_ON(DSP_SELE);
		DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,comp1,comp2,DSP_SELE);
	}else
	{
		if(feb1||feb2)
		{
			//LCD_FEB_ON(lcd_rev);
			LCD_FEB_ON(DSP_NORMAL);
			LCD_ShowChar(LOCATION_FEBCNT_PAGE,LOCATION_FEBCNT_CML,flaPara.FEB_count+'0',FONT_TYPE_BIG, DSP_NORMAL);
			DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,DSP_NORMAL);
		}
		else
		{
			LCD_FEB_OFF;
		}

		if(comp1||comp2)
		{
			LCD_CMP_ON(DSP_NORMAL);
			DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,DSP_NORMAL);
		}
		if(CMD_MODE_OFF==*g_cmdMode)
		DisplayTtlCmp_canStyle(LAYOUT_RATIO_A,LAYOUT_CMLPART(2),value1, value2,DSP_NORMAL);
	
	}
/*		
	if((value1<0)&&value2)
	{
		value1 += 1;
		value2 = 3-value2;
		LCD_ADD1_ON
	}else if((value1==0)&&value2)
	{
		LCD_MNS1_ON
		LCD_ADD1_ON
	}
*/
	//DisplayTtlCmp(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,DSP_NORMAL);
	//DisplayOutput(DSP_OUTPUT_PAGE,DSP_OUTPUT_CMN,value1,value2,DSP_NORMAL);
	//DisplayTtlCmp(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,lcd_rev);
	//DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,SETTING_ADDSUB==sel?DSP_SELE:DSP_NORMAL);
	//if(CMD_MODE_OFF==*g_cmdMode)
	//DisplayTtlCmp_canStyle(LAYOUT_RATIO_A,LAYOUT_CMLPART(2),value1, value2,SETTING_ADDSUB==sel?DSP_SELE:DSP_NORMAL);
#else
	value1 = comp1;
	value2 = comp2;
	//DisplayTtlCmp(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,DSP_NORMAL);
	//DisplayOutput(DSP_OUTPUT_PAGE,DSP_OUTPUT_CMN,value1,value2,DSP_NORMAL);
	//DisplayTtlCmp(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,value1,value2,lcd_rev);
	DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LAYOUT_CMLPART(2),value1,value2,SETTING_ADDSUB==sel?DSP_SELE:DSP_NORMAL);
	
#endif
}

void SetManuPara(unsigned char  output1,unsigned char output2)
{
	LCD_MANUPARA_CLEAR;
	DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MANU,output1,output2,DSP_NORMAL);
}

void SetRmtPara(unsigned char  output1,unsigned char output2)
{
	LCD_RMTPARA_CLEAR;
	DisplayOutput(LOCATION_RMTRCT_PAGE,LOCATION_RMTRCT_CML,WORKINGMODE_MANU,output1,output2,DSP_NORMAL);
}

void SetMultiTmFrq(unsigned char mulTimes, int mulFrq, char sel)
{
	MULTI_SET_ETIMES(mulTimes,sel);
	MULTI_SET_LINK;
	MULTI_SET_FRQ(mulFrq,sel);
}

void SetMultiPara(unsigned char mulExp, unsigned char mulTimes, int mulFrq)
{
	LCD_MULPARA_CLEAR;
#ifdef _CANON_
	DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MULTI,(mulExp>>4),0,DSP_NORMAL);
#else
	DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MULTI,(mulExp>>4),(mulExp&0x0f),DSP_NORMAL);
#endif

	SetMultiTmFrq(mulTimes,mulFrq,g_SettingIndex);
}

#ifdef _NIKON_
void SetMastPara(unsigned char chan, WL_ArrPara* mPara)
{
	//this function should refresh Master mode's parameter
	//LCD_MANUPARA_CLEAR;
	LCD_MASTERPARA_CLEAR;
	
	SetMstHead(&wlSet.m_current_arr);
	SetMstChan(chan,DSP_NORMAL);
	
	
	switch(mPara[WLM_ARR_MASTER].mode)
	{
		case WORKINGMODE_TTL:
			DisplayMstSetting(WLM_ARR_MASTER,WORKINGMODE_TTL,mPara[WLM_ARR_MASTER].cmp1,mPara[WLM_ARR_MASTER].cmp2);
		break;

		case WORKINGMODE_MANU:
			DisplayMstSetting(WLM_ARR_MASTER,WORKINGMODE_MANU,mPara[WLM_ARR_MASTER].output1,mPara[WLM_ARR_MASTER].output2);
		break;

		case WORKINGMODE_AUTO:
			DisplayMstSetting(WLM_ARR_MASTER,WORKINGMODE_AUTO,mPara[WLM_ARR_MASTER].cmp1,mPara[WLM_ARR_MASTER].cmp2);
		break;

		default:
			DisplayMstSetting(WLM_ARR_MASTER,WORKINGMODE_OFF,0,0);
		break;
	}
	
	switch(mPara[WLM_ARR_A].mode)
	{
		case WORKINGMODE_TTL:
			DisplayMstSetting(WLM_ARR_A,WORKINGMODE_TTL,mPara[WLM_ARR_A].cmp1,mPara[WLM_ARR_A].cmp2);
		break;

		case WORKINGMODE_MANU:
			DisplayMstSetting(WLM_ARR_A,WORKINGMODE_MANU,mPara[WLM_ARR_A].output1,mPara[WLM_ARR_A].output2);
		break;

		case WORKINGMODE_AUTO:
			DisplayMstSetting(WLM_ARR_A,WORKINGMODE_AUTO,mPara[WLM_ARR_A].cmp1,mPara[WLM_ARR_A].cmp2);
		break;

		default:
			DisplayMstSetting(WLM_ARR_A,WORKINGMODE_OFF,0,0);
		break;
	}
	
	switch(mPara[WLM_ARR_B].mode)
	{
		case WORKINGMODE_TTL:
			DisplayMstSetting(WLM_ARR_B,WORKINGMODE_TTL,mPara[WLM_ARR_B].cmp1,mPara[WLM_ARR_B].cmp2);
		break;

		case WORKINGMODE_MANU:
			DisplayMstSetting(WLM_ARR_B,WORKINGMODE_MANU,mPara[WLM_ARR_B].output1,mPara[WLM_ARR_B].output2);
		break;

		case WORKINGMODE_AUTO:
			DisplayMstSetting(WLM_ARR_B,WORKINGMODE_AUTO,mPara[WLM_ARR_B].cmp1,mPara[WLM_ARR_B].cmp2);
		break;

		default:
			DisplayMstSetting(WLM_ARR_B,WORKINGMODE_OFF,0,0);
		break;
	}
	
	switch(mPara[WLM_ARR_C].mode)
	{
		case WORKINGMODE_TTL:
			DisplayMstSetting(WLM_ARR_C,WORKINGMODE_TTL,mPara[WLM_ARR_C].cmp1,mPara[WLM_ARR_C].cmp2);
		break;

		case WORKINGMODE_MANU:
			DisplayMstSetting(WLM_ARR_C,WORKINGMODE_MANU,mPara[WLM_ARR_C].output1,mPara[WLM_ARR_C].output2);
		break;

		case WORKINGMODE_AUTO:
			DisplayMstSetting(WLM_ARR_C,WORKINGMODE_AUTO,mPara[WLM_ARR_C].cmp1,mPara[WLM_ARR_C].cmp2);
		break;

		default:
			DisplayMstSetting(WLM_ARR_C,WORKINGMODE_OFF,0,0);
		break;
	}
	
}
#endif


#ifdef _CANON_
void CanSynMode(signed char mode,signed char sm,char mDsp)
{
	if((mode>=WORKINGMODE_MULTI)&&(mode!=WORKINGMODE_GR))
	{
		return;
	}
	
	switch(sm)
	{
		case SYNC_MODE_FRON:
			DisplayPicture(LAYOUT_PARA_AREA1,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFront,mDsp);
		break;

		case SYNC_MODE_REAR:
			DisplayPicture(LAYOUT_PARA_AREA1,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synRear,mDsp);
		break;

		case SYNC_MODE_FP:
			DisplayPicture(LAYOUT_PARA_AREA1,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFp,mDsp);//{DisplayPicture(0,g_fpLoc[mainPara.workMode],19,16,EEPCHAR_PAGE_SYNFP,EEPCHAR_CML_SYNFP,DSP_NORMAL);
		break;
	}
}

#endif

//ttl main display
void SetTtlScr(void)
{
	LCD_MODE_CLEAR;
	LCD_FEB_OFF;
	LCD_RMT_CLEAR;
	LCD_TTL_ON;
	
	if(flaPara.charge_ok>AD_CHARGE_LOW)
	{
		LIGHT_READY;
	}else
	{
		LIGHT_CHARGE;	
	}

#ifdef _CANON_
	CanSynMode(mainPara.workMode, mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
#endif
	

#ifdef _CANON_

	SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	

	if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
		LCD_CANMST_ON;
		DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		//SetMstChan(*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		//g_SettingIndex = 0;
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		LCD_CANMST_ON;
		DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		//SetMstChan(radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}
	else
	{
		LCD_CANMST_CLEAR;
		//SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0);	
	}
#endif
	//DisplayApert(flaPara.apert);//display apert value	
	//DisplayIso(flaPara.iso);//display iso value
	//DisplayFocLen();
	//DisplayFocLen(motoSet.focLen,0);	
}

void SetManuScr(void)
{
	LCD_MODE_CLEAR;
	LCD_MULFT_CLEAR;
	LCD_MANU_ON;

#ifdef _CANON_
	CanSynMode(mainPara.workMode, mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
#endif

	
#ifdef _CANON_
	//if(WLM_ON==*wlSet.is_masterOn)
	if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);	
		DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		//SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);
		//SetMstChan(*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		LCD_CANMST_ON;
		DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		//SetMstChan(radioSet->rfChan,DSP_NORMAL);
		//SetMstChan(radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else
	{
		LCD_CANMST_CLEAR;
		SetManuPara(mstPara->output1,mstPara->output2);
	}
#endif
	//DisplayApert(flaPara.apert);//display apert value	
	//DisplayIso(flaPara.iso);//display iso value
	//DisplayFocLen(motoSet.focLen,0);
}

void SetMultiScr(void)
{
	LCD_MODE_CLEAR;
	LCD_FP_OFF(WORKINGMODE_MANU);
	LCD_MULTI_ON;

	//MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);


#ifdef _CANON_
	//if(WLM_ON==*wlSet.is_masterOn)
	if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);	
		DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		//SetMstChan(*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		LCD_CANMST_ON;
		DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		//SetMstChan(radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
	}else
	{
		LCD_CANMST_CLEAR;
		SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq);
	}
#endif
	//DisplayApert(flaPara.apert);//display apert value	
	//DisplayIso(flaPara.iso);//display iso value
	//DisplayFocLen(motoSet.focLen,0);
}

#ifdef _CANON_


void GrDrawSelBar(unsigned sel)
{
	//LcdClearFeild(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,8,BIG_FONT_HIGHT(3)+LINE_SPACING(2));
	RFGR_SELBAR_CLEAR;
	LcdPicFramed(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,8,BIG_FONT_HIGHT(3)+LINE_SPACING(2),DSP_NORMAL);
	FillArea(LAYOUT_RATIO_A+((BIG_FONT_HIGHT(3)+LINE_SPACING(2))/5*sel),LAYOUT_SELBAR_CML,8,(BIG_FONT_HIGHT(3)+LINE_SPACING(2)+14)/5,DSP_NORMAL);
	
}

void GrDspPara(u16 page, u8 gr, u8 mode, signed char pa1, signed char pa2,u8 rvs)
{
#if(_DSP_FONT_OPTION_==BIG_FONT)
	/*Clear field at every time when parameter changed*/
	//LCD_CLEAR_GR(page);
	LCD_ShowChar(page, LAYOUT_RATIO_COLUMN+3, 'A'+gr, FONT_TYPE_GRHEAD, rvs);
	LcdPicFramed(page, LAYOUT_RATIO_COLUMN,3+MAIN_FONT_WIDTH(1)+3,MAIN_FONT_HIGHT(1),rvs);
	//LcdClearFeild(page,LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,MAIN_FONT_HIGHT(1),LAYOUT_RATIO_WIDTH-24);
	/*Clear field at every time when parameter changed*///
	//LcdClearFeild(page,LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,LAYOUT_RATIO_WIDTH-24,MAIN_FONT_HIGHT(1));
	switch(mode)
	{
		case WORKINGMODE_TTL:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6, "ETTL", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-24,(sizeof("TTL")-1)*MAIN_FONT_WIDTH(1));
			DisplayTtlCmp(page, LAYOUT_RATIO_STPARA,pa1,pa2,rvs);
		break;

		case WORKINGMODE_MANU:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6, "M", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-24,(sizeof("M")-1)*MAIN_FONT_WIDTH(1));
			DisplayMstOtp(page,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,pa1,pa2,rvs);
		break;

		case WORKINGMODE_EXTA:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6, "Ext.A", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-24,(sizeof("Ext.A")-1)*MAIN_FONT_WIDTH(1)-MAIN_FONT_WIDTH(1)/2);
			DisplayTtlCmp(page, LAYOUT_RATIO_STPARA,pa1,pa2,rvs);
		break;

		default:
		break;
	}
	LcdPicFramed(page, LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,LAYOUT_RATIO_WIDTH-24,MAIN_FONT_HIGHT(1),rvs);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	//LcdClearFeild(page,0,294,MAIN_FONT_HIGHT(1));
	LCD_CLEAR_GR(page);
	LCD_ShowChar(page, LAYOUT_RATIO_COLUMN+3, 'A'+gr, FONT_TYPE_GRHEAD, rvs);
	LcdPicFramed(page, LAYOUT_RATIO_COLUMN,3+BIG_FONT_WIDTH(1)+3,BIG_FONT_HIGHT(1),rvs);
	//LcdClearFeild(page,LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,24,LAYOUT_RATIO_WIDTH-24);
	//LcdClearFeild(page,LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,LAYOUT_RATIO_WIDTH-24,24);
	switch(mode)
	{
		case WORKINGMODE_TTL:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1),"ETTL", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("TTL")-1)*BIG_FONT_WIDTH(1));
			DisplayTtlCmp(page, LAYOUT_RATIO_STPARA,pa1,pa2,rvs);
		break;

		case WORKINGMODE_MANU:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1), "M", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("M")-1)*BIG_FONT_WIDTH(1));
			DisplayMstOtp(page,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,pa1,pa2,rvs);
		break;

		case WORKINGMODE_EXTA:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1), "Ext.A", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("Ext.A")-1)*BIG_FONT_WIDTH(1)-BIG_FONT_WIDTH(1)/2);
			DisplayTtlCmp(page, LAYOUT_RATIO_STPARA,pa1,pa2,rvs);
		break;

		default:
			LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1), "OFF", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("OFF")-1)*BIG_FONT_WIDTH(1)-BIG_FONT_WIDTH(1)/2);
			
		break;
	}
	LcdPicFramed(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1),LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1),BIG_FONT_HIGHT(1),rvs);
#endif
}

void RfGrDspPara(u8 sel, u8 isSel)
{
	unsigned char gr;
	u8 rsv;
	u8 grPageStart;
	u8 dspTotal;
	//unsigned char sf=0;

	if(!g_grStyle)
	{
		grPageStart = LAYOUT_RATIO_A;
		dspTotal = 3;
	}else
	{
		grPageStart = LAYOUT_GR_A;
		dspTotal = 5;
	}
	
	
	//for(gr=0;gr<3;gr++)
	for(gr=0;gr<dspTotal;gr++)
	{
		if(isSel)
		{
			rsv = ((gr+g_grDspStart)==(sel-1))? DSP_SELE:DSP_NORMAL;
		}else
		{
			rsv = DSP_NORMAL;
		}

		if (g_SettingIndex != 0)
		{
			LcdClearFeild(grPageStart+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)), LAYOUT_RATIO_COLUMN, 220-10, 20);
		}
		
		switch(radioSet->rfGr[gr+g_grDspStart].mode)
		{
			case WORKINGMODE_TTL:
				//GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_TTL,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,rsv);
				GrDspPara(grPageStart+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_TTL,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,rsv);
			break;

			case WORKINGMODE_MANU:
				//GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_MANU,radioSet->rfGr[gr+g_grDspStart].m_exp1,radioSet->rfGr[gr+g_grDspStart].m_exp2,rsv);
				GrDspPara(grPageStart+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_MANU,radioSet->rfGr[gr+g_grDspStart].m_exp1,radioSet->rfGr[gr+g_grDspStart].m_exp2,rsv);
			break;

			case WORKINGMODE_EXTA:
				//GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_EXTA,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,rsv);
				GrDspPara(grPageStart+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_EXTA,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,rsv);
			break;

			default:
				//GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,radioSet->rfGr[gr+g_grDspStart].mode,0,0,rsv);
				GrDspPara(grPageStart+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,radioSet->rfGr[gr+g_grDspStart].mode,0,0,rsv);
			break;
		}
	}
}

void SetRfGrScr(void)
{
	//unsigned char gr;
	//LCD_CMP_CLEAR;
	u8 i = 0;
	
	LayModeGr();
	
	//SetMstChan(radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	/*when one group set as exta, set the syn auto*/
	if (SYNC_MODE_FP == mainPara.synMode)
	{
		for (i = 0; i < 5; i++)
		{
			if (WORKINGMODE_EXTA==radioSet->rfGr[i].mode)
			{
				mainPara.synMode = SYNC_MODE_FRON;
				break;
			}
		}
	}
	

	if(!g_grStyle)
	{
		LCD_CANMST_ON;
		
		//SetMstChan(radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		
		CanSynMode(mainPara.workMode, mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);

		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);
	}else
	{
		//SetMstChan(radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}	

	
	RfGrDspPara(g_grSel,  SETTING_RFGR_SEL == g_SettingIndex);

	if((g_grSel)&&(!g_grStyle)
		&&(SETTING_RFGR_SEL == g_SettingIndex))               // 2017/07/27
	{
		GrDrawSelBar(g_grSel-1);
	}
#if 0
	for(gr=0;gr<3;gr++)
	{
		switch(radioSet->rfGr[gr+g_grDspStart].mode)
		{
			case WORKINGMODE_TTL:
				GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_TTL,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,DSP_NORMAL);
			break;

			case WORKINGMODE_MANU:
				GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_MANU,radioSet->rfGr[gr+g_grDspStart].m_exp1,radioSet->rfGr[gr+g_grDspStart].m_exp2,DSP_NORMAL);
			break;

			case WORKINGMODE_EXTA:
				GrDspPara(LAYOUT_RATIO_A+gr*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_EXTA,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,DSP_NORMAL);
			break;
		}
	}
#endif
}
#endif

#ifdef _NIKON_
void SetMastScr()
{
	LCD_MODE_CLEAR;
	LCD_MANUPARA_CLEAR;
	LCD_MASTER_ON;
	
	if(flaPara.charge_ok>CHARGE_RDY)
	{
		LIGHT_READY;
	}else
	{
		LIGHT_CHARGE;	
	}
	SetMastPara(*wlSet.m_chan,wlSet.m_arrPara);
	//DisplayApert(flaPara.apert);//display apert value	
	//DisplayIso(flaPara.iso);//display iso value
	//DisplayFocLen(motoSet.focLen,0);
}
#endif

#ifdef _CANON_
#if 0
void CanSetMstPara(unsigned char sel,unsigned char sw)
{
	if(WLM_ON==sw)
	{
		//wlSet.is_masterOn=WLM_ON;
		if(flaPara.charge_ok>CHARGE_RDY)
		{
			LIGHT_READY;
		}else
		{
			LIGHT_CHARGE;	
		}
		
		//DisplayCanMstHead(0,mainPara.workMode);
		//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
		DisplayCanMstHead(0,mainPara.workMode,&wlSet);
		SetMstChan(*wlSet.m_chan,DSP_NORMAL);
		
	}else
	{
		//wlSet.is_masterOn=WLM_OFF;
		LCD_CANMST_CLEAR;
	}
}
#endif

#if 0
void CanMstSet()
{
	//DisplayClearFeild(3,0,115,16);
	//if(WLM_ON==*wlSet.is_masterOn)
	if(CMD_MODE_OFF!=*g_cmdMode)
	{
		//wlSet.is_masterOn = WLM_OFF;	
		*g_cmdMode = CMD_MODE_OFF;
	}else
	{
		if(mainPara.workMode<WORKINGMODE_RTN)
		{
			//*wlSet.is_masterOn = WLM_ON;
			*g_cmdMode = CMD_MODE_LIGH;
			if(SYNC_MODE_REAR==mainPara.synMode)
			{
				mainPara.synMode = SYNC_MODE_FRON;
				CanSynMode(mainPara.workMode,mainPara.synMode,DSP_NORMAL);
			}
		}else
		{
			return;
		}
	}
	//CanSetMstPara(0,*wlSet.is_masterOn);
	CanSetMstPara(0,*g_cmdMode);

}
#endif
#endif

#ifdef _NIKON_
void SetRmtN()
{
	LCD_MODE_CLEAR;
	LCD_RMT_CLEAR;
	LCD_MSTCHAN_CLEAR;
	LCD_RMT_ON;
	LCD_PARA_APERT_OFF;
	LCD_PARA_FOCLEN_OFF;
	FuncDsp();

	if(flaPara.charge_ok>CHARGE_LOW)
	{
		LIGHT_READY;
	}else
	{
		LIGHT_CHARGE;	
	}

	flaPara.ledStatue = 0;

	DisplayRmt(wlSet.s_array,wlSet.s_chan,0);
	//DisplayRmtRct(WORKINGMODE_TTL,-3,2);
	
	
}
#endif

#ifdef _CANON_
void SetRmtC()
{
	LCD_MODE_CLEAR;
	LCD_RMT_CLEAR;
	LCD_MSTCHAN_CLEAR;
	LCD_MSTFLA_OFF;
	LCD_RMT_ON;
	LCD_PARA_APERT_OFF;
	LCD_PARA_FOCLEN_OFF;

	DisplayRmt(*wlSlave.s_array,*wlSlave.s_chan,0);


}
#endif

void SetRmtF1()
{
	LCD_MODE_CLEAR;
	LCD_RMT_CLEAR;
	LCD_RMTPARA_CLEAR;
	LCD_RMTF1_ON;

	//DisplayTtlCmp(LOCATION_RMTRCT_PAGE,LOCATION_RMTRCT_CML+8,mstPara->output1,mstPara->output2,DSP_NORMAL);
	SetRmtPara(mstPara->output1,mstPara->output2);
}

void SetRmtF2()
{
	LCD_MODE_CLEAR;
	LCD_RMT_CLEAR;
	LCD_RMTPARA_CLEAR;
	LCD_RMTF2_ON;

	//DisplayTtlCmp(LOCATION_RMTRCT_PAGE,LOCATION_RMTRCT_CML+8,mstPara->output1,mstPara->output2,DSP_NORMAL);
	SetRmtPara(mstPara->output1,mstPara->output2);

}


#ifdef _NIKON_
void WlmArrLeft()
{
	//char arrIndex;
	
	//arrIndex = wlSet.m_current_arr;
	

	switch(wlSet.m_arrPara[wlSet.m_current_arr].mode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_AUTO:
			if((-3==wlSet.m_arrPara[wlSet.m_current_arr].cmp1)&&(0==wlSet.m_arrPara[wlSet.m_current_arr].cmp2))
			{
				return;
			}else
			{
				wlSet.m_arrPara[wlSet.m_current_arr].cmp2--;
				if(wlSet.m_arrPara[wlSet.m_current_arr].cmp2<0)
				{
					wlSet.m_arrPara[wlSet.m_current_arr].cmp2+=3;
					wlSet.m_arrPara[wlSet.m_current_arr].cmp1--;
				}
			}
		break;

		case WORKINGMODE_MANU:
			if((1==wlSet.m_arrPara[wlSet.m_current_arr].output1)&&(0==wlSet.m_arrPara[wlSet.m_current_arr].output2))
			{
				wlSet.m_arrPara[wlSet.m_current_arr].output1 = 8;
				wlSet.m_arrPara[wlSet.m_current_arr].output2 = 0;
			}else
			{
				wlSet.m_arrPara[wlSet.m_current_arr].output2 --;
				if(wlSet.m_arrPara[wlSet.m_current_arr].output2<0)
				{
					wlSet.m_arrPara[wlSet.m_current_arr].output2+=3;
					wlSet.m_arrPara[wlSet.m_current_arr].output1--;
				}
			}
		break;

/*
		case WORKINGMODE_AUTO:
			if((-3==wlSet.m_arrPara[wlSet.m_current_arr].cmp1)&&(0==wlSet.m_arrPara[wlSet.m_current_arr].cmp2))
			{
				return;
			}else
			{
				wlSet.m_arrPara[wlSet.m_current_arr].cmp2--;
				if(wlSet.m_arrPara[wlSet.m_current_arr].cmp2<0)
				{
					wlSet.m_arrPara[wlSet.m_current_arr].cmp2+=3;
					wlSet.m_arrPara[wlSet.m_current_arr].cmp1--;
				}
			}
		break;
*/
		
	}
#if 0
	if((WORKINGMODE_TTL==wlSet.m_arrPara[wlSet.m_current_arr].mode)||(WORKINGMODE_AUTO==wlSet.m_arrPara[wlSet.m_current_arr].mode))
	{
		if(g_SettingIndex)
		{
			//ParaPreserve(EEPROM_INDEX_MWL_ARRMCMP+wlSet.m_current_arr,(((wlSet.m_arrPara[wlSet.m_current_arr].cmp1+3)<<4) |wlSet.m_arrPara[wlSet.m_current_arr].cmp2));
			//ParaPreserve(EEPROM_INDEX_MWL_ARRMCMP+wlSet.m_current_arr,((wlSet.m_arrPara[wlSet.m_current_arr].cmp1+3)<<4) |wlSet.m_arrPara[wlSet.m_current_arr].cmp2);
		}		
		////while(uart_tran_flag);
		DecodeUartData(UART_ENCODE_WLMSET,(wlSet.m_current_arr<<4)|wlSet.m_arrPara[wlSet.m_current_arr].mode,(wlSet.m_arrPara[wlSet.m_current_arr].cmp1+3)<<4|wlSet.m_arrPara[wlSet.m_current_arr].cmp2);
	}else
	{
		if(g_SettingIndex)
		{
			ParaPreserve(EEPROM_INDEX_MWL_ARRMEXP+wlSet.m_current_arr,((wlSet.m_arrPara[wlSet.m_current_arr].output1<<4) |wlSet.m_arrPara[wlSet.m_current_arr].output2));
		}
		//while(uart_tran_flag);
		DecodeUartData(UART_ENCODE_WLMSET,(wlSet.m_current_arr<<4)|wlSet.m_arrPara[wlSet.m_current_arr].mode,wlSet.m_arrPara[wlSet.m_current_arr].output1<<4|wlSet.m_arrPara[wlSet.m_current_arr].output2);
	}
#endif
}

void WlmArrRight()
{
	char arrIndex;

	arrIndex = wlSet.m_current_arr;
	

	switch(wlSet.m_arrPara[arrIndex].mode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_AUTO:
			if(3==wlSet.m_arrPara[arrIndex].cmp1)
			{
				return;
			}else
			{
				wlSet.m_arrPara[arrIndex].cmp2++;
				if(wlSet.m_arrPara[arrIndex].cmp2>2)
				{
					wlSet.m_arrPara[arrIndex].cmp2-=3;
					wlSet.m_arrPara[arrIndex].cmp1++;
				}
			}
		break;

		case WORKINGMODE_MANU:
			if(8==wlSet.m_arrPara[arrIndex].output1)
			{
				wlSet.m_arrPara[arrIndex].output1 = 1;
				wlSet.m_arrPara[arrIndex].output2 = 0;
			}else
			{
				wlSet.m_arrPara[arrIndex].output2 ++;
				if(wlSet.m_arrPara[arrIndex].output2>2)
				{
					wlSet.m_arrPara[arrIndex].output2-=3;
					wlSet.m_arrPara[arrIndex].output1++;
				}
			}
		break;
	}
#if 0
	if((WORKINGMODE_TTL==wlSet.m_arrPara[wlSet.m_current_arr].mode)||(WORKINGMODE_AUTO==wlSet.m_arrPara[wlSet.m_current_arr].mode))
	{

		if(g_SettingIndex)
		{
			ParaPreserve(EEPROM_INDEX_MWL_ARRMCMP+wlSet.m_current_arr,(((wlSet.m_arrPara[wlSet.m_current_arr].cmp1+3)<<4) |wlSet.m_arrPara[wlSet.m_current_arr].cmp2));
		}
		//while(uart_tran_flag);
		DecodeUartData(UART_ENCODE_WLMSET,(wlSet.m_current_arr<<4)|wlSet.m_arrPara[wlSet.m_current_arr].mode,(wlSet.m_arrPara[wlSet.m_current_arr].cmp1+3)<<4|wlSet.m_arrPara[wlSet.m_current_arr].cmp2);
	}else
	{
		if(g_SettingIndex)
		{
			ParaPreserve(EEPROM_INDEX_MWL_ARRMEXP+wlSet.m_current_arr,((wlSet.m_arrPara[wlSet.m_current_arr].output1<<4) |wlSet.m_arrPara[wlSet.m_current_arr].output2));
		}
		//while(uart_tran_flag);
		DecodeUartData(UART_ENCODE_WLMSET,(wlSet.m_current_arr<<4)|wlSet.m_arrPara[wlSet.m_current_arr].mode,wlSet.m_arrPara[wlSet.m_current_arr].output1<<4|wlSet.m_arrPara[wlSet.m_current_arr].output2);
	}
#endif
}
#endif							//for nikon

/*
#ifdef _CANON_
void UartParaSend(WL_ArrPara *smd,WlSet *wls)
{
	unsigned char tmp=0;
	switch(smd->mode)
	{
		case WORKINGMODE_TTL:
			tmp = ((smd ->cmp1+3)<<4)|smd->cmp2;
		break;

		case WORKINGMODE_MANU:
			tmp = (smd ->output1<<4)|smd->output2;
		break;

		case WORKINGMODE_MULTI:
			tmp = smd ->multi_exp>>4;
		break;

		case WORKINGMODE_RTC:
			//tmp = ((wls->s_chan << 4)|(wls->s_array-1));
		break;

#ifdef _SPEED_LIGHT_
		case WORKINGMODE_F1:
		case WORKINGMODE_F2:
		break;
#endif
		
		//DecodeUartData(UART_ENCODE_MODE,WORKINGMODE_RTC,((wlSet.s_chan<<4)|(wlSet.s_array-1)));
	}
	DecodeUartData(UART_ENCODE_MODE,smd->mode,tmp);
	
}
#endif
*/

void LoadButtons(void)
{
#ifndef _SN_DEBUG_
	//g_menuId = 0;
#endif
	if(!KEY_LOCK_EX)
	{
		return;
	}
	BuildFuncMenu();
	KeyMenuSw();
	//DspMstModIcon(*g_cmdMode);
}

void ModPasteScr(signed char mode)
{
	switch(mode)
	{
		case WORKINGMODE_TTL:
			//printf("TTL\n");
			SetTtlScr();
		break;

		case WORKINGMODE_MANU:
			//printf("M\n");
			SetManuScr();
		break;

		case WORKINGMODE_MULTI:
			//printf("Multi\n");
			SetMultiScr();
		break;

#ifdef _CANON_
		case WORKINGMODE_GR:
			//printf("Gr\n");
			SetRfGrScr();
		break;
#endif

#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			SetMastScr();
		break;

		case WORKINGMODE_RTN:
			SetRmtN();
		break;
#endif

#ifdef _CANON_
		case WORKINGMODE_RTC:
			SetRmtC();
		break;
#endif

#ifdef _SPEED_LIGHT_
		case WORKINGMODE_F1:
			SetRmtF1();
		break;

		case WORKINGMODE_F2:
			SetRmtF2();
		break;
#endif
	}
}

void DspMstChan(void)
{
	if (WLM_SETTING_CHAN == g_SettingIndex)
	{
		SetMstChan(g_rfChanSet,DSP_SELE);	
	}else
	{
		SetMstChan(radioSet->rfChan,DSP_NORMAL);	
	}
}

void Mode(void)
{
	//LCD_Clear(BACK_COLOR);
	//DspClearAllPara();
	//ModPasteScr(mode);
	//g_menuId = 0;
	//BuildFuncMenu();
	//KeyMenuSw();
	//DspMstModIcon(*g_cmdMode);
	//RfDevLinkMark(mode,&wlSet);
	//FuncDsp();

	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		break;
		
		case CMD_MODE_RF_MST:
			//ENABLE_RF_MODULE();
			//g_SettingIndex=0;
			ModPasteScr(mainPara.workMode);
			//g_menuId = 0;
			//BuildFuncMenu();
			//KeyMenuSw();
			DspMstModIcon(*g_cmdMode);
			RfDevLinkMark(mainPara.workMode,&wlSet);
			//Mode(mainPara.workMode);
			//g_menuId = 0;
			LoadButtons();
			DspMstChan();
			//SetMstChan(radioSet->rfChan,DSP_NORMAL);			
			//SetMstChan();
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;
			
		case CMD_MODE_LIGH_MST:
		/*
			//DISABLE_RF_MODULE();
			g_SettingIndex=0;
			Mode(mainPara.workMode);
			g_menuId = 0;
			LoadButtons();
			SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);*/
		break;

		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RFLS_MST:
			ModLinkedShot(*g_cmdMode);
			BuildLinkedShotMenu(*g_cmdMode);
			DspMstChan();
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_SLA:
		break;

		case CMD_MODE_RF_SLA:
		break;
	}

	FuncDsp();

	
}

void DspBtnSubAdd(void)
{
	switch (mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
			if (*wlSet.arr_mode == WLM_MODE_ALL)
			{
				DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlSet.m_arrPara[WL_ARRARY_A].cmp1,wlSet.m_arrPara[WL_ARRARY_A].cmp2,DSP_NORMAL);
			}
		break;

		case WORKINGMODE_GR:
			SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);
		break;

		default:
		break;
	}
}

void DspGrSetting(void)
{
	switch (mainPara.workMode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_MANU:
		case WORKINGMODE_MULTI:
			DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		break;

		case WORKINGMODE_GR:
			ClearRfGrSelBar();
			RfGrDspPara(g_grSel,  SETTING_RFGR_SEL == g_SettingIndex);
			if((g_grSel)&&(!g_grStyle))
			{
				GrDrawSelBar(g_grSel-1);
			}
		break;

	}

}

void RePaintScreen(void)
{
	switch (g_SettingIndex)
	{
		case SETTING_SNRMENU:
			DspSnrMenu();
		break;

		case SETTING_ID:
			DspID(g_idSetIndex,g_rfIdSet);
		break;

		case SETTING_RFSCAN:
			if (!g_isChScan)
			{
				DspScanResult();
			}
		break;

		case WLM_SETTING_CHAN:
		case SETTING_ADDSUB:
		case SETTING_FRQ:
		case SETTING_MULTI:
		case SETTING_RFGR_SEL:
		case WLM_SETTING_ARRA:
		case WLM_SETTING_ARRB:
		case WLM_SETTING_ARRC:
		case SETTING_TTLFEB:
			ClearRfGrSelBar();
			LCD_CMPFEB_CLEAR();
			Mode();
		break;
/*
		case WLM_SETTING_CHAN:
			DspMstChan();
		break;

		case SETTING_ADDSUB:
			DspBtnSubAdd();
		break;

		case SETTING_FRQ:
			SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
		break;

		case SETTING_MULTI:
			SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
		break;

		case SETTING_RFGR_SEL:
			DspGrSetting();
		break;

		case WLM_SETTING_ARRA:
		case WLM_SETTING_ARRB:
		case WLM_SETTING_ARRC:
			DspGrSetting();
		break;
*/
		default:
			//Mode(mainPara.workMode);
			DspClearAllPara();
			Mode();
		break;
	}
}


void SelectMode(signed char mode,signed char arr)
{
	switch(mode)
	{
		case WORKINGMODE_TTL:
		break;

		case WORKINGMODE_MANU:
		break;

		case WORKINGMODE_MULTI:
		break;

#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			if(arr<4)
			{
				wlSet.m_arrPara[arr].mode++;
				if(wlSet.m_arrPara[arr].mode>3)
					wlSet.m_arrPara[arr].mode = 0;
				SetMastPara(*wlSet.m_chan,wlSet.m_arrPara);
				//ParaPreserve(EEPROM_INDEX_MWL_ABCMODE,((wlSet.m_arrPara[WL_ARRARY_M].mode << 6)|(wlSet.m_arrPara[WL_ARRARY_A].mode << 4) | (wlSet.m_arrPara[WL_ARRARY_B].mode << 2) |(wlSet.m_arrPara[WL_ARRARY_C].mode)));				
			}
		break;
#endif

		case WORKINGMODE_RTN:
		break;

		case WORKINGMODE_RTC:
		break;

		case WORKINGMODE_GR:
			//radioSet->rfGr[g_grSel-1].mode++;
			//if(radioSet->rfGr[g_grSel-1]>)
			//if(SETTING_ADDSUB==g_SettingIndex)
			if(SETTING_RFGR_SEL==g_SettingIndex)
			{
				switch(radioSet->rfGr[g_grSel-1].mode)
				{
					case WORKINGMODE_TTL:
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_MANU;
					break;

					case WORKINGMODE_MANU:
						//radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_EXTA;
#ifdef _SN_DEBUG_
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_OFF;
#else					
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_EXTA;
#endif
					break;
					
					case WORKINGMODE_EXTA:
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_OFF;
					break;

					case WORKINGMODE_OFF:
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_TTL;
					break;

					default:
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_TTL;
					break;
				}
				//RfGrDspPara(g_grSel);	
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		break;
	}
}

void SwithMode()					//mode long press
{
#ifdef _ST_EX_
	return;
#else
	
#endif
}

#if 0
void KeySetting()
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
#ifdef _CANON_
			if(0==g_SettingIndex)
			{
				g_SettingIndex++;
				CanSynMode(mainPara.workMode,mainPara.synMode,DSP_SELE);
			}else if(1==g_SettingIndex)
			{
				g_SettingIndex++;
				CanSynMode(mainPara.workMode,mainPara.synMode,DSP_NORMAL);
				CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
				LCD_FEB_ON(DSP_SELE);
			}else if(2==g_SettingIndex)
			{
				KeyOk();
				//LCD_FEB_ON(DSP_NORMAL);
				//if(WLM_ON==*wlSet.is_masterOn)
				if(CMD_MODE_OFF!=*g_cmdMode)
				{
					g_SettingIndex=WLM_SETTING_RATION;
					//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
					DisplayCanMstHead(1,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(1,mode);			//reverse display Ration
				}//else
				//{
				//	KeyOk();
					//g_SettingIndex = 0;
				//}
			}else if(WLM_SETTING_RATION==g_SettingIndex)
			{
				g_SettingIndex++;
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(2,mode);
				//CanSetMstPara(2);	
			}else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if(WLM_MODE_ALL==*wlSet.arr_mode)
				{
					g_SettingIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					g_SettingIndex++;
					DisplayCanMstHead(3,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(3,mode);
					//CanSetMstPara(3);
				}
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				if(WLM_MODE_AB==*wlSet.arr_mode)
				{
					g_SettingIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					g_SettingIndex++;
					DisplayCanMstHead(4,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(4,mode);
					//CanSetMstPara(4);
				}
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				g_SettingIndex ++;
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(0,mode);
				//CanSetMstPara(0);
			}else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				g_SettingIndex=0;
				DisplayCanMstHead(0,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
			}
#endif
		break;

		case WORKINGMODE_MANU:
#ifdef _CANON_
			if(0==g_SettingIndex)
			{
				g_SettingIndex++;
				CanSynMode(mainPara.workMode,mainPara.synMode,DSP_REVERSE);
			}else if(1==g_SettingIndex)
			{
				KeyOk();
				//if(WLM_ON==*wlSet.is_masterOn)
				if(CMD_MODE_OFF!=*g_cmdMode)
				{
					g_SettingIndex=WLM_SETTING_RATION;
					DisplayCanMstHead(1,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(1,mode);
					//CanSetMstPara(1);
				}//else
				//{
				//	KeyOk();
					//g_SettingIndex = 0;
				//}
			}else if(WLM_SETTING_RATION==g_SettingIndex)
			{
				g_SettingIndex++;
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(2,mode);
				//CanSetMstPara(2);				
			}else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if(WLM_MODE_ALL==*wlSet.arr_mode)
				{
					g_SettingIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					g_SettingIndex++;
					DisplayCanMstHead(3,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(3,mode);
					//CanSetMstPara(3);
				}
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				if(WLM_MODE_AB==*wlSet.arr_mode)
				{
					g_SettingIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					g_SettingIndex++;
					DisplayCanMstHead(4,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(4,mode);
					//CanSetMstPara(4);
				}
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				g_SettingIndex = WLM_SETTING_CHAN;
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(0,mode);
				//CanSetMstPara(0);
			}else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				g_SettingIndex=0;
				DisplayCanMstHead(0,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
			}
#endif	
		break;

		case WORKINGMODE_MULTI:
			if(0==g_SettingIndex)
			{
				g_SettingIndex++;
				LCD_MULTIME_RVS(mainPara.multi_times);
			}else if(1==g_SettingIndex)
			{
				g_SettingIndex++;
				MULTI_SET_ETIMES(mainPara.multi_times);
				LCD_MULFRQ_RVS(mainPara.multi_Frq);
			}else if(2==g_SettingIndex)
			{
#ifdef _CANON_
				//if(WLM_ON==*wlSet.is_masterOn)
				if(CMD_MODE_OFF!=*g_cmdMode)
				{
					g_SettingIndex=WLM_SETTING_RATION;
					MULTI_SET_FRQ(mainPara.multi_Frq);	
					DisplayCanMstHead(1,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
					//CanSetMstPara(1);
					//DisplayCanMstHead(1,mode);
					
				}else
				{
					g_SettingIndex = 0;
					MULTI_SET_FRQ(mainPara.multi_Frq);					
				}
#else
				g_SettingIndex = 0;
				MULTI_SET_FRQ(mainPara.multi_Frq);
#endif
			}
#ifdef	_CANON_
			else if(WLM_SETTING_RATION==g_SettingIndex)
			{
				g_SettingIndex++;
				DisplayCanMstHead(2,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(2,mode);
				//CanSetMstPara(2);				
			}else if(WLM_SETTING_ARRA==g_SettingIndex)
			{
				if(WLM_MODE_ALL==*wlSet.arr_mode)
				{
					g_SettingIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					g_SettingIndex++;
					DisplayCanMstHead(3,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(3,mode);
					//CanSetMstPara(3);
				}
			}else if(WLM_SETTING_ARRB==g_SettingIndex)
			{
				if(WLM_MODE_AB==*wlSet.arr_mode)
				{
					g_SettingIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					g_SettingIndex++;
					DisplayCanMstHead(4,mainPara.workMode,&wlSet);
					//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(4,mode);
					//CanSetMstPara(4);
				}
			}else if(WLM_SETTING_ARRC==g_SettingIndex)
			{
				g_SettingIndex = WLM_SETTING_CHAN;
				DisplayCanMstHead(5,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(0,mode);
				//CanSetMstPara(0);
			}else if(WLM_SETTING_CHAN==g_SettingIndex)
			{
				g_SettingIndex=0;
				DisplayCanMstHead(0,mainPara.workMode,&wlSet);
				//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
			}
#endif

		break;

#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			if(0==g_SettingIndex)
			{
				g_SettingIndex++;
				wlSet.m_current_arr=0;
				SetMstHead(&wlSet.m_current_arr);
				//here select master expose 
			}else if(1==g_SettingIndex)
			{
				g_SettingIndex++;
				wlSet.m_current_arr=1;
				SetMstHead(&wlSet.m_current_arr);
				//here select arrary 1 setting
			}else if(2==g_SettingIndex)
			{
				g_SettingIndex++;
				wlSet.m_current_arr=2;
				SetMstHead(&wlSet.m_current_arr);
				//here select arrary 2 setting
			}else if(3==g_SettingIndex)
			{
				g_SettingIndex++;
				wlSet.m_current_arr=3;
				SetMstHead(&wlSet.m_current_arr);
				//here select arrary 3 setting
			}else if(4==g_SettingIndex)
			{
				g_SettingIndex ++;
				wlSet.m_current_arr = 0x0f;				
				SetMstHead(&wlSet.m_current_arr);
				SetMstChan(*wlSet.m_chan,DSP_REVERSE);
				//here cancel master settings
			}else if(5==g_SettingIndex)
			{
				g_SettingIndex = 0;
				SetMstChan(*wlSet.m_chan,DSP_NORMAL);
			}
				
		break;
#endif
		case WORKINGMODE_RTC:
		case WORKINGMODE_RTN:
			g_SettingIndex++;
			if(g_SettingIndex>2)
				g_SettingIndex = 0;
			DisplayRmt(*wlSlave.s_array, *wlSlave.s_chan, g_SettingIndex);
		break;
	}
}
#endif

/*
void KeySetting(StruMode *stm,char *sIndex, WlSet *ws)
{
	switch(stm->mode)
	{
		case WORKINGMODE_TTL:
#ifdef _CANON_
			if(0==*sIndex)
			{
				*sIndex++;
				CanSynMode(stm->mode,stm->synMode,DSP_REVERSE);
				//LCD_REAR_RVS
			}else if(1==*sIndex)
			{
				*sIndex++;
				LCD_FEB_RVS;
			}else if(2==*sIndex)
			{
				if(WLM_ON==ws->is_masterOn)
				{
					*sIndex=WLM_SETTING_RATION;
					//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
					DisplayCanMstHead(1,stm->mode,ws);
					//DisplayCanMstHead(1,mode);			//reverse display Ration
				}else
				{
					*sIndex = 0;
				}
			}else if(WLM_SETTING_RATION==*sIndex)
			{
				*sIndex++;
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				DisplayCanMstHead(2,stm->mode,ws);
				//DisplayCanMstHead(2,mode);
				//CanSetMstPara(2);	
			}else if(WLM_SETTING_ARRA==*sIndex)
			{
				if(WLM_MODE_ALL==ws->arr_mode)
				{
					*sIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,stm->mode,ws);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					*sIndex++;
					DisplayCanMstHead(3,stm->mode,ws);
					//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(3,mode);
					//CanSetMstPara(3);
				}
			}else if(WLM_SETTING_ARRB==*sIndex)
			{
				if(WLM_MODE_AB==ws->arr_mode)
				{
					*sIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,stm->mode,ws);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					*sIndex++;
					DisplayCanMstHead(4,stm->mode,ws);
					//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(4,mode);
					//CanSetMstPara(4);
				}
			}else if(WLM_SETTING_ARRC==*sIndex)
			{
				*sIndex ++;
				DisplayCanMstHead(5,stm->mode,ws);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(0,mode);
				//CanSetMstPara(0);
			}else if(WLM_SETTING_CHAN==*sIndex)
			{
				*sIndex=0;
				DisplayCanMstHead(0,stm->mode,ws);
				//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
			}
#endif
		break;

		case WORKINGMODE_MANU:
#ifdef _CANON_
			if(0==*sIndex)
			{
				*sIndex++;
				CanSynMode(stm->mode,stm->synMode,DSP_REVERSE);
				//LCD_REAR_RVS
			}else if(1==*sIndex)
			{
				if(WLM_ON==ws->is_masterOn)
				{
					*sIndex=WLM_SETTING_RATION;
					DisplayCanMstHead(1,stm->mode,ws);
					//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(1,mode);
					//CanSetMstPara(1);
				}else
				{
					*sIndex = 0;
				}
			}else if(WLM_SETTING_RATION==*sIndex)
			{
				*sIndex++;
				DisplayCanMstHead(2,stm->mode,ws);
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(2,mode);
				//CanSetMstPara(2);				
			}else if(WLM_SETTING_ARRA==*sIndex)
			{
				if(WLM_MODE_ALL==ws->arr_mode)
				{
					*sIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,stm->mode,ws);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					*sIndex++;
					DisplayCanMstHead(3,stm->mode,ws);
					//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(3,mode);
					//CanSetMstPara(3);
				}
			}else if(WLM_SETTING_ARRB==*sIndex)
			{
				if(WLM_MODE_AB==ws->arr_mode)
				{
					*sIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,stm->mode,ws);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					*sIndex++;
					DisplayCanMstHead(4,stm->mode,ws);
					//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(4,mode);
					//CanSetMstPara(4);
				}
			}else if(WLM_SETTING_ARRC==*sIndex)
			{
				*sIndex = WLM_SETTING_CHAN;
				DisplayCanMstHead(5,stm->mode,ws);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(0,mode);
				//CanSetMstPara(0);
			}else if(WLM_SETTING_CHAN==*sIndex)
			{
				*sIndex=0;
				DisplayCanMstHead(0,stm->mode,ws);
				//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
			}
#endif	
		break;

		case WORKINGMODE_MULTI:
			if(0==*sIndex)
			{
				*sIndex++;
				//LCD_MULTIME_RVS(mainPara.multi_times);
				LCD_MULTIME_RVS(stm->multi_eTimes);
			}else if(1==*sIndex)
			{
				*sIndex++;
				//MULTI_SET_ETIMES(mainPara.multi_times);
				LCD_MULTIME_RVS(stm->multi_eTimes);
				//LCD_MULFRQ_RVS(mainPara.multi_Frq);
				LCD_MULFRQ_RVS(stm->multi_frequence);
			}else if(2==*sIndex)
			{
#ifdef _CANON_
				if(WLM_ON==ws->is_masterOn)
				{
					*sIndex=WLM_SETTING_RATION;
					//MULTI_SET_FRQ(mainPara.multi_Frq);
					MULTI_SET_FRQ(stm->multi_frequence);
					DisplayCanMstHead(1,stm->mode,ws);
					//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
					//CanSetMstPara(1);
					//DisplayCanMstHead(1,mode);
					
				}else
				{
					*sIndex = 0;
					//MULTI_SET_FRQ(mainPara.multi_Frq);		
					MULTI_SET_FRQ(stm->multi_frequence);	
				}
#else
				*sIndex = 0;
				//MULTI_SET_FRQ(mainPara.multi_Frq);
				MULTI_SET_FRQ(stm->multi_frequence);
#endif
			}
#ifdef	_CANON_
			else if(WLM_SETTING_RATION==*sIndex)
			{
				*sIndex++;
				DisplayCanMstHead(2,stm->mode,ws);
				//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(2,mode);
				//CanSetMstPara(2);				
			}else if(WLM_SETTING_ARRA==*sIndex)
			{
				if(WLM_MODE_ALL==ws->arr_mode)
				{
					*sIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,stm->mode,ws);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					*sIndex++;
					DisplayCanMstHead(3,stm->mode,ws);
					//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(3,mode);
					//CanSetMstPara(3);
				}
			}else if(WLM_SETTING_ARRB==*sIndex)
			{
				if(WLM_MODE_AB==ws->arr_mode)
				{
					*sIndex = WLM_SETTING_CHAN;
					DisplayCanMstHead(5,stm->mode,ws);
					//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(0,mode);
					//CanSetMstPara(0);
				}else
				{
					*sIndex++;
					DisplayCanMstHead(4,stm->mode,ws);
					//DisplayCanMstHead(4,mainPara.workMode,wlSet.arr_mode);
					//DisplayCanMstHead(4,mode);
					//CanSetMstPara(4);
				}
			}else if(WLM_SETTING_ARRC==*sIndex)
			{
				*sIndex = WLM_SETTING_CHAN;
				DisplayCanMstHead(5,stm->mode,ws);
				//DisplayCanMstHead(5,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(0,mode);
				//CanSetMstPara(0);
			}else if(WLM_SETTING_CHAN==*sIndex)
			{
				*sIndex=0;
				DisplayCanMstHead(0,stm->mode,ws);
				//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
			}
#endif

		break;

#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			if(0==*sIndex)
			{
				*sIndex++;
				ws->m_current_arr=0;
				SetMstHead(&ws->m_current_arr);
				//here select master expose 
			}else if(1==*sIndex)
			{
				*sIndex++;
				ws->m_current_arr=1;
				SetMstHead(&ws->m_current_arr);
				//here select arrary 1 setting
			}else if(2==*sIndex)
			{
				*sIndex++;
				ws->m_current_arr=2;
				SetMstHead(&ws->m_current_arr);
				//here select arrary 2 setting
			}else if(3==*sIndex)
			{
				*sIndex++;
				ws->m_current_arr=3;
				SetMstHead(&ws->m_current_arr);
				//here select arrary 3 setting
			}else if(4==*sIndex)
			{
				*sIndex ++;
				ws->m_current_arr = 0x0f;				
				SetMstHead(&ws->m_current_arr);
				SetMstChan(ws->m_chan,DSP_REVERSE);
				//here cancel master settings
			}else if(5==*sIndex)
			{
				*sIndex = 0;
				SetMstChan(ws->m_chan,DSP_NORMAL);
			}
				
		break;
#endif
		case WORKINGMODE_RTC:
		case WORKINGMODE_RTN:
			*sIndex++;
			if(*sIndex>2)
				*sIndex = 0;
			DisplayRmt(ws->s_array, ws->s_chan, *sIndex);
		break;
	}
}
*/


/*
void MultiCalMaxTime()
{
#ifdef _CANON_
	unsigned char frqTmp;
	unsigned char arrNum;
	if(mainPara.multi_Frq>10) 
	{
		frqTmp = 11;
	}else
	{
		frqTmp = mainPara.multi_Frq;
	}
	
	if(mainPara.multi_Frq<=3)
	{
		arrNum = 0;
	}else
	{
		arrNum = frqTmp - 3;
	}
	
	if(mainPara.multi_times>canMaxCalc[(*multiSet.expose>>4)-1][arrNum])
	{
		mainPara.multi_times = canMaxCalc[(*multiSet.expose>>4)-1][arrNum];
	}

	//MULTI_SET_ETIMES(mainPara.multi_times);
	//LCD_MULTIME_RVS(mainPara.multi_times);
	
#else
	unsigned char frqTmp;
	unsigned char arrNum;
	if(mainPara.multi_Frq>10) 
	{
		frqTmp = 11;
	}else
	{
		frqTmp = mainPara.multi_Frq;
	}
	
	if(mainPara.multi_Frq<=3)
	{
		arrNum = 0;
	}else
	{
		arrNum = frqTmp - 3;
	}
	
	if(mainPara.multi_times>multiMaxFlaTime[(*multiSet.expose>>4)-1][arrNum])
	{
		mainPara.multi_times = multiMaxFlaTime[(*multiSet.expose>>4)-1][arrNum];
	}

	//MULTI_SET_ETIMES(mainPara.multi_times);
	//LCD_MULTIME_RVS(mainPara.multi_times);
#endif

	
}*/


void MultiMaxTimes(int frq,unsigned char exp,int *eTm,unsigned char *maxTm)
{
	unsigned char frqTmp;
	unsigned char arrNum;
	unsigned char tMax;

	if(frq>10)
	{
		frqTmp = 11;
	}else
	{
		frqTmp = frq;
	}

	if(frq<=3)
	{
		arrNum = 0;
	}else
	{
		arrNum = frqTmp - 3;
	}

	tMax =  canMaxCalc[exp>>4][arrNum];
	*maxTm = tMax;

	if(*eTm>tMax)
	{
		//tMax = eTm;
		*eTm = tMax;
	}
	
}

void RfDevLinkMark(char mod,WlSet *wlPara)
{
#ifdef UART1_TRACE_DEBUG
	u8 i;
	for(i=0;i<5;i++)
	{
		printf("% d",radioStatue.devCnt[i]);
	}
	printf("\r\n");
#endif

	RFGR_SELBAR_CLEAR;
		
	if(WORKINGMODE_TTL==mod)
	{
		switch(*wlPara->arr_mode)
		{
			case WLM_MODE_ALL:
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;

			case WLM_MODE_AB:
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}	
			break;

			case WLM_MODE_RES:
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[2]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;
		}
	}else if(WORKINGMODE_GR==mod)
	{
			if (SETTING_RFGR_SEL == g_SettingIndex)
			{
				return;
			}
			if(g_grStyle>0)
			{
				if(radioStatue.devCnt[g_grDspStart]>0)
				{
					DisplayPicture(LAYOUT_GR_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
				if(radioStatue.devCnt[g_grDspStart+1]>0)
				{
					DisplayPicture(LAYOUT_GR_B+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
				if(radioStatue.devCnt[g_grDspStart+2]>0)
				{
					DisplayPicture(LAYOUT_GR_C+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
				if(radioStatue.devCnt[g_grDspStart+3]>0)
				{
					DisplayPicture(LAYOUT_GR_D+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
				if(radioStatue.devCnt[g_grDspStart+4]>0)
				{
					DisplayPicture(LAYOUT_GR_E+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
			}else
			{
				if(radioStatue.devCnt[g_grDspStart]>0)
				{
					DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
				if(radioStatue.devCnt[g_grDspStart+1]>0)
				{
					DisplayPicture(LAYOUT_RATIO_B+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}
				if(radioStatue.devCnt[g_grDspStart+2]>0)
				{
					DisplayPicture(LAYOUT_RATIO_C+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
				}		
			}
	}else
	{
		switch(*wlPara->arr_mode)
		{
			case WLM_MODE_ALL:
			//if(radioStatue.devCnt[0]>0)
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}	
			break;

			case WLM_MODE_AB:
			if(radioStatue.devCnt[0]>0)
			{
				DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[1]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;

			case WLM_MODE_RES:
			if(radioStatue.devCnt[0]>0)
			{
				DisplayPicture(LAYOUT_RATIO_A+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[1]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[2]>0)
			{
				DisplayPicture(LAYOUT_RATIO_C+LAYOUT_LKDEV_VER,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;
		}
	}
}

void MotoShift(u8 dir)
{
		if(motoSet.moving != 0)return;
#ifdef MOTO_REFLECT_DETECT
		if(REF_BOARD_DETECT) return;
#endif
		if(KEY_DIR_LEFT==dir)
		{
			motoSet.focIndex--;
		}else
		{
			motoSet.focIndex++;
		}
		if(motoSet.focIndex<=0)
		{
			motoSet.focIndex = 0;
			*motoSet.dest_location = focLenScape[0];
			motoSet.destRsv = *motoSet.dest_location;
			MotoReset();
			LCD_MMOTO_OFF;
			*motoSet.mode = MOTO_MODE_AUTO;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_AUTO;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_AUTO &  motoSet.focIndex);
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//g_paraChange = 1;
			//SetFocLen(0,0);
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE); 
		}else if(motoSet.focIndex>10)
		{
			motoSet.focIndex=10;
			MotoReset();
		}
		else
		{
			LCD_MMOTO_ON;
			*motoSet.mode = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = motoSet.focIndex;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_MANU | motoSet.focIndex);
			//ParaPreserve(EEPROM_INDEX_MOTOMODE,(MASK_MOTOMODE_MANU|motoSet.focIndex));
			*motoSet.dest_location = focLenScape[motoSet.focIndex];
			motoSet.destRsv = *motoSet.dest_location;
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//ParaPreserve(EEPROM_INDEX_LOCATION,*motoSet.dest_location);
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE); 
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
}


void ExposeAddSub(u8 dir)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_GR:
			if(KEY_DIR_LEFT==dir)
			{
				if((-3==mstPara->cmp1)&&(0==mstPara->cmp2))
				{
					return;
				}
				mstPara->cmp2--;
				if(mstPara->cmp2<0)
				{
					mstPara->cmp2+=3;
					mstPara->cmp1--;
				}
			}else
			{
				if(3==mstPara->cmp1)
				{
					return;
				}
				mstPara->cmp2++;
				if(mstPara->cmp2>2)
				{
					mstPara->cmp1++;
					mstPara->cmp2 = 0;
				}
			}		
			//wlSet.otpCode[MASTER] = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			//menuPara.otpCod[MASTER] = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			//menuPara.mstTtl = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			/*
			if(WORKINGMODE_TTL==mainPara.workMode)
			{
				SetTtlScr();
			}else
			{
				SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
			}*/
			//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);
			//ParaPreserve(EEPROM_INDEX_TTLCMP,((mstPara->cmp1+3)<<4)|mstPara->cmp2);				
		break;

		case WORKINGMODE_MANU:
		if(CMD_MODE_OFF!=*g_cmdMode) return;
		if(KEY_DIR_LEFT==dir)
		{
			if((0==mstPara->output1)&&(0==mstPara->output2))
			{
				mstPara->output1 = 7;
				mstPara->output2 = 0;
			}else
			{
				mstPara->output2--;
				if(mstPara->output2<0)
				{
					mstPara->output2+=3;
					mstPara->output1--;
				}			
			}
		}else
		{
			if(7==mstPara->output1)
			{
				mstPara->output1 = 0;
				mstPara->output2 = 0;
			}else
			{
				mstPara->output2++;
				if(mstPara->output2>2)
				{
					mstPara->output1++;
					mstPara->output2=0;
				}
				
			}
		}
		menuPara.otpCod[MASTER] = output_table[(7-mstPara->output1)*3-mstPara->output2];
		//SetManuPara(mstPara->output1,mstPara->output2);
		//ParaPreserve(EEPROM_INDEX_EXP,(mstPara->output1<<4)|mstPara->output2);		
		//while(uart_tran_flag);
		//DecodeUartData(UART_ENCODE_OUTPUT,mstPara->output1,mstPara->output2);				
		break;

		case WORKINGMODE_MULTI:
		if(CMD_MODE_OFF!=*g_cmdMode) return;
		if(KEY_DIR_LEFT==dir)
		{
#ifdef _CANON_

			if(0x00==*multiSet.expose)	
			{
				*multiSet.expose=0x50;
				//MultiCalMaxTime();	
				MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
			}else
			{
				(*multiSet.expose)-=0x10;
			}
#else
			if(0x00==*multiSet.expose)
			{
				*multiSet.expose = 0x40;
				//MultiCalMaxTime();
				MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
			}else
			{
				if(0==(*multiSet.expose&0x0f))
				{
					(*multiSet.expose) -=0x10;
					(*multiSet.expose) += 2;
				}else
				{
					(*multiSet.expose) --;
				}
			}
#endif
		}else
		{
#ifdef _CANON_
			if(0x50==*multiSet.expose)	
			{
				*multiSet.expose=0x00;
			}else
			{
				(*multiSet.expose)+=0x10;
			}
			
			//MultiCalMaxTime();
			MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);

#else
			if(0x40==*multiSet.expose)
			{
				*multiSet.expose = 0x00;
			}else
			{
				if(2==(*multiSet.expose&0x0f))
				{
					(*multiSet.expose) +=0x10;
					(*multiSet.expose) &= 0xf0;
				}else
				{
					(*multiSet.expose) ++;
				}
			}
			//MultiCalMaxTime();
			MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
#endif
		}
		//multiSet.WlmMul[MASTER]=output_table[(7-*multiSet.expose)*3];
		menuPara.WlmMul[MASTER]=output_table[(7-*multiSet.expose)*3];
		//SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq);
		break;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void SynModShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		mainPara.synMode--;
		if(mainPara.synMode<SYNC_MODE_FRON)
		{
			mainPara.synMode = SYNC_MODE_FP;
		}
	}else
	{
		mainPara.synMode++;
		if(mainPara.synMode>SYNC_MODE_FP)
		{
			mainPara.synMode = SYNC_MODE_FRON;
		}
		
	}	
	//if((SYNC_MODE_REAR==mainPara.synMode)&&(WLM_ON==*wlSet.is_masterOn))
	if((SYNC_MODE_REAR==mainPara.synMode)&&(CMD_MODE_OFF!=*g_cmdMode))
	{
		mainPara.synMode=SYNC_MODE_FRON;
	}
	//CanSynMode(mainPara.workMode,mainPara.synMode,DSP_SELE);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void TtlFebAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		if((!flaPara.ttl_FEB2)&&(!flaPara.ttl_FEB1))
			return;
		flaPara.ttl_FEB2--;
		if(flaPara.ttl_FEB2<0)
		{
			flaPara.ttl_FEB1--;
			flaPara.ttl_FEB2+=3;					
		}
	}else
	{
		if(flaPara.ttl_FEB1>=3)
		{
			return;
		}
		flaPara.ttl_FEB2++;
		if(flaPara.ttl_FEB2>=3)
		{
			flaPara.ttl_FEB1++;
			flaPara.ttl_FEB2-=3;
			
		}
	}		

	flaPara.FEB_count = 0;

	if(flaPara.ttl_FEB1||flaPara.ttl_FEB2)
	{
		flaPara.FEB_count = 1;
		flaPara.febEn = TTL_FEB_EN;
	}

	//menuPara.feb = feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2];
	/*
	//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
	if(WORKINGMODE_TTL==mainPara.workMode)
	{
		SetTtlScr();
	}else
	{
		CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
	}*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}

void RatioSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		(*wlSet.arr_mode)--;
		if(*wlSet.arr_mode<WLM_MODE_ALL)
			*wlSet.arr_mode = WLM_MODE_RES;
	}else
	{
		(*wlSet.arr_mode)++;
		if(*wlSet.arr_mode>WLM_MODE_RES)
		{
			*wlSet.arr_mode = WLM_MODE_ALL;
		}
	}
	//DisplayCanMstHead(1,mainPara.workMode,wlSet.arr_mode);
	//DisplayCanMstHead(1,mainPara.workMode,&wlSet);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void Arr_ASubAdd(u8 dir)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			if(KEY_DIR_LEFT==dir)
			{
				if((-3==wlSet.m_arrPara[WL_ARRARY_A].cmp1)&&(0==wlSet.m_arrPara[WL_ARRARY_A].cmp2))
				{
					return;
				}
				wlSet.m_arrPara[WL_ARRARY_A].cmp2--;
				if(wlSet.m_arrPara[WL_ARRARY_A].cmp2<0)
				{
					wlSet.m_arrPara[WL_ARRARY_A].cmp2+=3;
					wlSet.m_arrPara[WL_ARRARY_A].cmp1--;					
				}
			}else
			{
				if(3==wlSet.m_arrPara[WL_ARRARY_A].cmp1)
				{
					return;
				}
				wlSet.m_arrPara[WL_ARRARY_A].cmp2++;
				if(wlSet.m_arrPara[WL_ARRARY_A].cmp2>2)
				{
					wlSet.m_arrPara[WL_ARRARY_A].cmp1++;
					wlSet.m_arrPara[WL_ARRARY_A].cmp2 = 0;
				}
			}
			//menuPara.mstTtl = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			//DisplayCanMstHead(2,mainPara.workMode,&wlSet);
		break;

		case WORKINGMODE_MANU:

			if(KEY_DIR_LEFT==dir)
			{

				if((0==mstPara->output1)&&(0==mstPara->output2))
				{
					mstPara->output1 = 7;
					mstPara->output2 = 0;
				}else
				{
					mstPara->output2--;
					if(mstPara->output2<0)
					{
						mstPara->output2+=3;
						mstPara->output1--;
					}
					
				}
			}else
			{
				if(7==mstPara->output1)
				{
					mstPara->output1 = 0;
					mstPara->output2 = 0;
				}else
				{
					mstPara->output2++;
					if(mstPara->output2>2)
					{
						mstPara->output1++;
						mstPara->output2=0;
					}
					
				}
			}
			//SetManuPara(mstPara->output1,mstPara->output2);
			//DisplayCanMstHead(2,mainPara.workMode,&wlSet);
			menuPara.otpCod[WL_ARRARY_A] = output_table[(7-mstPara->output1)*3-mstPara->output2];
		break;

		case WORKINGMODE_MULTI:
			if(KEY_DIR_LEFT==dir)
			{
				if(0x00==(wlSet.m_arrPara[WL_ARRARY_A].multi_exp&0xf0))
				{
					*multiSet.expose = 0x50;
					//MultiCalMaxTime();
					MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
				}else
				{
					(*multiSet.expose) -= 0x10;
				}
				
			}else
			{
				if(0x50==(*multiSet.expose&0xf0))
				{
					*multiSet.expose=0x00;
				}else 
				{
					(*multiSet.expose)+=0x10;
				}
				//MultiCalMaxTime();
				MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
			}
			//DisplayCanMstHead(2,mainPara.workMode,wlSet.arr_mode);
			//DisplayCanMstHead(2,mainPara.workMode,&wlSet);
			menuPara.WlmMul[WL_ARRARY_A] = output_table[(7-(*multiSet.expose>>4))*3];

			//mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].mulOtp;
			//SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq);	
		break;

		case WORKINGMODE_GR:
		break;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void Arr_BSubAdd(u8 dir)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			if(KEY_DIR_LEFT==dir)
			{
				wlSet.m_arrPara[WL_ARRARY_B].cmp1--;
				if(wlSet.m_arrPara[WL_ARRARY_B].cmp1<0)
				{
					wlSet.m_arrPara[WL_ARRARY_B].cmp1 = 0;
				}
				//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
				//DisplayCanMstHead(3,mainPara.workMode,&wlSet);
			}else
			{
				wlSet.m_arrPara[WL_ARRARY_B].cmp1++;
				if(wlSet.m_arrPara[WL_ARRARY_B].cmp1>12)
				{
					wlSet.m_arrPara[WL_ARRARY_B].cmp1 = 12;
				}
			}
			abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
			abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
			//DisplayCanMstHead(3,mainPara.workMode,&wlSet);
		break;

		case WORKINGMODE_MANU:
			if(KEY_DIR_LEFT==dir)
			{
				if((0==wlSet.m_arrPara[WL_ARRARY_B].output1)&&(0==wlSet.m_arrPara[WL_ARRARY_B].output2))
				{
					wlSet.m_arrPara[WL_ARRARY_B].output1 = 7;
					wlSet.m_arrPara[WL_ARRARY_B].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WL_ARRARY_B].output2--;
					if(wlSet.m_arrPara[WL_ARRARY_B].output2<0)
					{
						wlSet.m_arrPara[WL_ARRARY_B].output2+=3;
						wlSet.m_arrPara[WL_ARRARY_B].output1--;
					}
				}
			}else
			{
				if(7==wlSet.m_arrPara[WL_ARRARY_B].output1)
				{
					wlSet.m_arrPara[WL_ARRARY_B].output1 = 0;
					wlSet.m_arrPara[WL_ARRARY_B].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WL_ARRARY_B].output2++;
					if(wlSet.m_arrPara[WL_ARRARY_B].output2>2)
					{
						wlSet.m_arrPara[WL_ARRARY_B].output1++;
						wlSet.m_arrPara[WL_ARRARY_B].output2=0;
					}
				}
			}
			//DisplayCanMstHead(3,mainPara.workMode,&wlSet);
			menuPara.otpCod[WL_ARRARY_B] = output_table[(7-wlSet.m_arrPara[WL_ARRARY_B].output1)*3-wlSet.m_arrPara[WL_ARRARY_B].output2];
		break;

		case WORKINGMODE_MULTI:
			if(KEY_DIR_LEFT==dir)
			{
				if(0x00==(wlSet.m_arrPara[WL_ARRARY_B].multi_exp&0xf0))
				{
					wlSet.m_arrPara[WL_ARRARY_B].multi_exp = 0x50;
				}else
				{
					wlSet.m_arrPara[WL_ARRARY_B].multi_exp -= 0x10;
				}
			}else
			{
				if(0x50==(wlSet.m_arrPara[WL_ARRARY_B].multi_exp&0xf0))
				{
					wlSet.m_arrPara[WL_ARRARY_B].multi_exp=0x00;
				}else 
				{
					wlSet.m_arrPara[WL_ARRARY_B].multi_exp+=0x10;
				}
				//DisplayCanMstHead(3,mainPara.workMode,wlSet.arr_mode);
			}
			//DisplayCanMstHead(3,mainPara.workMode,&wlSet);
			menuPara.WlmMul[WL_ARRARY_B] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4))*3];
		break;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void Arr_CSubAdd(u8 dir)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			if(KEY_DIR_LEFT==dir)
			{
				if((-3==wlSet.m_arrPara[WL_ARRARY_C].cmp1)&&(0==wlSet.m_arrPara[WL_ARRARY_C].cmp2))
				{
					return;
				}
				wlSet.m_arrPara[WL_ARRARY_C].cmp2--;
				if(wlSet.m_arrPara[WL_ARRARY_C].cmp2<0)
				{
					wlSet.m_arrPara[WL_ARRARY_C].cmp2+=3;
					wlSet.m_arrPara[WL_ARRARY_C].cmp1--;					
				}
			}else
			{
				if(3==wlSet.m_arrPara[WL_ARRARY_C].cmp1)
				{
					return;
				}
				wlSet.m_arrPara[WL_ARRARY_C].cmp2++;
				if(wlSet.m_arrPara[WL_ARRARY_C].cmp2>2)
				{
					wlSet.m_arrPara[WL_ARRARY_C].cmp1++;
					wlSet.m_arrPara[WL_ARRARY_C].cmp2 = 0;
				}
			}
			wlSet.cCmp = compensation_table[(wlSet.m_arrPara[WL_ARRARY_C].cmp1+6)*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2];
			//DisplayCanMstHead(4,mainPara.workMode,&wlSet);
		break;

		case WORKINGMODE_MANU:
			if(KEY_DIR_LEFT==dir)
			{
				if((0==wlSet.m_arrPara[WL_ARRARY_C].output1)&&(0==wlSet.m_arrPara[WL_ARRARY_C].output2))
				{
					wlSet.m_arrPara[WL_ARRARY_C].output1 = 7;
					wlSet.m_arrPara[WL_ARRARY_C].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WL_ARRARY_C].output2--;
					if(wlSet.m_arrPara[WL_ARRARY_C].output2<0)
					{
						wlSet.m_arrPara[WL_ARRARY_C].output2+=3;
						wlSet.m_arrPara[WL_ARRARY_C].output1--;
					}
				}
			}else
			{
				if(7==wlSet.m_arrPara[WL_ARRARY_C].output1)
				{
					wlSet.m_arrPara[WL_ARRARY_C].output1 = 0;
					wlSet.m_arrPara[WL_ARRARY_C].output2 = 0;
				}else
				{
					wlSet.m_arrPara[WL_ARRARY_C].output2++;
					if(wlSet.m_arrPara[WL_ARRARY_C].output2>2)
					{
						wlSet.m_arrPara[WL_ARRARY_C].output1++;
						wlSet.m_arrPara[WL_ARRARY_C].output2=0;
					}
				}
			}
			//DisplayCanMstHead(4,mainPara.workMode,&wlSet);
			menuPara.otpCod[WL_ARRARY_C] = output_table[(7-wlSet.m_arrPara[WL_ARRARY_C].output1)*3-wlSet.m_arrPara[WL_ARRARY_C].output2];
		break;

		case WORKINGMODE_MULTI:
			if(KEY_DIR_LEFT==dir)
			{
				if(0x00==(wlSet.m_arrPara[WL_ARRARY_C].multi_exp&0xf0))
				{
					wlSet.m_arrPara[WL_ARRARY_C].multi_exp = 0x50;
				}else
				{
					wlSet.m_arrPara[WL_ARRARY_C].multi_exp -= 0x10;
				}
			}else
			{
				if(0x50==(wlSet.m_arrPara[WL_ARRARY_C].multi_exp&0xf0))
				{
					wlSet.m_arrPara[WL_ARRARY_C].multi_exp=0x00;
				}else 
				{
					wlSet.m_arrPara[WL_ARRARY_C].multi_exp+=0x10;
				}
			}
			//DisplayCanMstHead(4,mainPara.workMode,&wlSet);
			menuPara.WlmMul[WL_ARRARY_C] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_C].multi_exp>>4))*3];
		break;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void LightChanSet(u8 dir)
{
/*
	if(KEY_DIR_LEFT==dir)
	{
		(*wlSet.m_chan)--;
		if(*wlSet.m_chan<WL_CHANNEL_ONE)
		{
			*wlSet.m_chan = WL_CHANNEL_FOUR;
		}
	}else
	{
		(*wlSet.m_chan)++;
		if(*wlSet.m_chan>WL_CHANNEL_FOUR)
		{
			*wlSet.m_chan = WL_CHANNEL_ONE;
		}
	}
	SetMstChan(*wlSet.m_chan+1,DSP_SELE);
*/
	if(KEY_DIR_LEFT==dir)
	{
		(g_liteChanSet)--;
		if(g_liteChanSet<WL_CHANNEL_ONE)
		{
			g_liteChanSet = WL_CHANNEL_FOUR;
		}
	}else
	{
		(g_liteChanSet)++;
		if(g_liteChanSet>WL_CHANNEL_FOUR)
		{
			g_liteChanSet = WL_CHANNEL_ONE;
		}
	}
	//SetMstChan(g_liteChanSet+1,DSP_SELE);
	//SetMstChan(*wlSet.m_chan+1,DSP_SELE);
	//DisplayCanMstHead(5,mainPara.workMode,&wlSet);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void RfChanSet(u8 dir)
{
/*
	if(KEY_DIR_LEFT==dir)
	{
		(radioSet->rfChan)--;
		if(radioSet->rfChan<0)
		{
			radioSet->rfChan = 15;
		}
	}else
	{
		(radioSet->rfChan)++;
		if(radioSet->rfChan>15)
		{
			radioSet->rfChan = 0;
		}
	}
*/
	if(KEY_DIR_LEFT==dir)
	{
		(g_rfChanSet)--;
		if(g_rfChanSet<0)
		{
			g_rfChanSet= 15;
		}
	}else
	{
		(g_rfChanSet)++;
		if(g_rfChanSet>15)
		{
			g_rfChanSet= 0;
		}
	}
	//SetMstChan(g_rfChanSet,DSP_SELE);
	//SetMstChan(radioSet->rfChan,DSP_SELE);
	//UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MultiTimesShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
#ifdef _NIKON_
		if(mainPara.multi_times<=10)
		{
			mainPara.multi_times--;
		}else 
		{
			mainPara.multi_times-=2;
		}

		if(mainPara.multi_times<1)
		{
			mainPara.multi_times = 1;
		}
#else
		if(mainPara.multi_times<=20)
		{
			mainPara.multi_times--;
		}else if(mainPara.multi_times<=50)
		{
			mainPara.multi_times-=5;
		}else
		{
			mainPara.multi_times-=10;
		}

		if(mainPara.multi_times<1)
		{
			mainPara.multi_times = 1;
		}
#endif
	}else
	{
#ifdef _NIKON_
			if(mainPara.multi_times<10)
			{
				mainPara.multi_times++;
			}else
			{
				mainPara.multi_times+=2;
			}

			if(mainPara.multi_times>90)
			{
				mainPara.multi_times = 90;
			}
#else
			if(mainPara.multi_times<20)
			{
				mainPara.multi_times++;
			}else if(mainPara.multi_times<50)
			{
				mainPara.multi_times+=5;
			}else
			{
				mainPara.multi_times+=10;
			}
#endif
	}
	//MultiCalMaxTime();
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
	
	//LCD_MULTIME_RVS(mainPara.multi_times);
	//MULTI_SET_ETIMES(mainPara.multi_times, g_SettingIndex)
	//MULTI_SET_FRQ(mainPara.multi_Frq,g_SettingIndex);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MultiFrqShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
#ifdef _NIKON_
		if(mainPara.multi_Frq<=10)
		{
			mainPara.multi_Frq--;
		}else if(mainPara.multi_Frq<=100)
		{
			mainPara.multi_Frq-=10;
		}
#else
		if(mainPara.multi_Frq<=20)
		{
			mainPara.multi_Frq--;
		}else if(mainPara.multi_Frq<=50)
		{
			mainPara.multi_Frq-=5;
		}else if(mainPara.multi_Frq<=190)
		{
			mainPara.multi_Frq-=10;
		}else
		{
			mainPara.multi_Frq = 190;
		}
#endif
		if(mainPara.multi_Frq<1)
		{
			mainPara.multi_Frq = 1;
		}
	}else
	{
#ifdef _NIKON_
		if(mainPara.multi_Frq<10)
		{
			mainPara.multi_Frq++;
		}else 
		{
			mainPara.multi_Frq+=10;
		}
		if(mainPara.multi_Frq>100)
		{
			mainPara.multi_Frq = 100;
		}

#else
		if(mainPara.multi_Frq<20)
		{
			mainPara.multi_Frq++;
		}else if(mainPara.multi_Frq<50)
		{
			mainPara.multi_Frq+=5;
		}else
		{
			mainPara.multi_Frq+=10;
			if(mainPara.multi_Frq==200)
				mainPara.multi_Frq = 199;
		}
		if(mainPara.multi_Frq>199)
			mainPara.multi_Frq = 199;
#endif

	}
	//MultiCalMaxTime();
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
	//MULTI_SET_ETIMES(mainPara.multi_times,g_SettingIndex);
	//MULTI_SET_FRQ(mainPara.multi_Frq, g_SettingIndex)
	//LCD_MULFRQ_RVS(mainPara.multi_Frq);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

static u16 MathPow(u8 a, u8 k)
{
	u8 i;u16 ret=1;
	for(i=0;i<k;i++)
	{
		ret*=a;
	}
	return ret;
}

void IdSetSubAdd(u8 dir)
{
	u16 stp = MathPow(10,g_idSetIndex-1);
/*
	if(KEY_DIR_LEFT==dir)
	{
		if((radioSet->id/stp)%10)
		{
			radioSet->id-=(stp);
		}
	}else
	{
		if(((radioSet->id/stp)%10)<9)
		{
			radioSet->id+=stp;
		}
	}
	UartSctSend(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
*/
	if(KEY_DIR_LEFT==dir)
	{
		if((g_rfIdSet/stp)%10)
		{
			g_rfIdSet-=(stp);
		}
	}else
	{
		if(((g_rfIdSet/stp)%10)<9)
		{
			g_rfIdSet+=stp;
		}
	}
	//UartSctSend(UART_ENCODE_RFID, radioSet->id>>8, radioSet->id&0xff);
	//UartSctSend(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
	//DspID(g_idSetIndex,g_rfIdSet);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void RfGrSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		switch(radioSet->rfGr[g_grSel-1].mode)
		{
			case WORKINGMODE_TTL:
			case WORKINGMODE_EXTA:
				if((-3==radioSet->rfGr[g_grSel-1].ttl_cmp1) &&(!radioSet->rfGr[g_grSel-1].ttl_cmp2))return;
				radioSet->rfGr[g_grSel-1].ttl_cmp2--;
				if(radioSet->rfGr[g_grSel-1].ttl_cmp2<0)
				{
					radioSet->rfGr[g_grSel-1].ttl_cmp2+=3;
					radioSet->rfGr[g_grSel-1].ttl_cmp1--;					
				}
				menuPara.rfGrTmp[g_grSel-1] = compensation_table[(radioSet->rfGr[g_grSel-1].ttl_cmp1+6)*3+radioSet->rfGr[g_grSel-1].ttl_cmp2];
			break;

			case WORKINGMODE_MANU:
				if((!radioSet->rfGr[g_grSel-1].m_exp1)&&(!radioSet->rfGr[g_grSel-1].m_exp2))return;
				radioSet->rfGr[g_grSel-1].m_exp2--;
				if(radioSet->rfGr[g_grSel-1].m_exp2<0)
				{
					radioSet->rfGr[g_grSel-1].m_exp2+=3;
					radioSet->rfGr[g_grSel-1].m_exp1--;
				}
				menuPara.rfGrOtp[g_grSel-1] = output_table[(7-radioSet->rfGr[g_grSel-1].m_exp1)*3-radioSet->rfGr[g_grSel-1].m_exp2];
			break;

			case WORKINGMODE_OFF:
			break;
		}
	}else
	{
		switch(radioSet->rfGr[g_grSel-1].mode)
		{
			case WORKINGMODE_TTL:
			case WORKINGMODE_EXTA:
				if(3==radioSet->rfGr[g_grSel-1].ttl_cmp1)return;
				radioSet->rfGr[g_grSel-1].ttl_cmp2++;
				if(radioSet->rfGr[g_grSel-1].ttl_cmp2>2)
				{
					radioSet->rfGr[g_grSel-1].ttl_cmp2=0;
					radioSet->rfGr[g_grSel-1].ttl_cmp1++;					
				}
				menuPara.rfGrTmp[g_grSel-1] = compensation_table[(radioSet->rfGr[g_grSel-1].ttl_cmp1+6)*3+radioSet->rfGr[g_grSel-1].ttl_cmp2];
			break;

			case WORKINGMODE_MANU:
				if(7==radioSet->rfGr[g_grSel-1].m_exp1)return;
				radioSet->rfGr[g_grSel-1].m_exp2++;
				if(radioSet->rfGr[g_grSel-1].m_exp2>2)
				{
					radioSet->rfGr[g_grSel-1].m_exp2=0;
					radioSet->rfGr[g_grSel-1].m_exp1++;
				}
				menuPara.rfGrOtp[g_grSel-1] = output_table[(7-radioSet->rfGr[g_grSel-1].m_exp1)*3-radioSet->rfGr[g_grSel-1].m_exp2];
			break;

			case WORKINGMODE_OFF:
			break;
		}
	}
	//RfGrDspPara(g_grSel);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void ChScanSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		(g_rfChanSet)--;
		if(g_rfChanSet<1)
		{
			g_rfChanSet= 1;
		}
	}else
	{
		(g_rfChanSet)++;
		if(g_rfChanSet>15)
		{
			g_rfChanSet= 15;
		}
	}
	//SetMstChan(g_rfChanSet,DSP_NORMAL);
	//LcdClearFeild(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL+SCANRST_DOT_HT+8, 0, LCD_W, 16);
	//DisplayMstSetDot(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL+SCANRST_DOT_HT+8, 5+LAYOUT_SCANRST_Y+SCANRST_DOT_WT+(g_rfChanSet-1)*(SCANRST_BAR_VAL+SCANRST_DOT_WT), DSP_NORMAL);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}

void SnrMenuSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		snrMenu.userSel--;
		//DspSnrMenu();
	}else
	{
		
		snrMenu.userSel++;
		//DspSnrMenu();
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void SnrSubMenuSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		snrMenu.subSel--;
		//SmSubExpand();
	}else
	{
		snrMenu.subSel++;
		//SmSubExpand();
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void KeySubAdd(u8 dir)
{
	//static char idSel=0;
	switch(g_SettingIndex)
	{
#ifdef _SPEED_LIGHT_
		case SETTING_MOTO:
			MotoShift(dir);
		break;
#endif
		case SETTING_ADDSUB:
			ExposeAddSub(dir);
		break;

		case SETTING_SYNMODE:
			SynModShift(dir);
		break;

		case SETTING_TTLFEB:
			TtlFebAddSub(dir);
		break;

		case WLM_SETTING_RATION:
			RatioSubAdd(dir);
		break;

		case WLM_SETTING_ARRA:
			Arr_ASubAdd(dir);
		break;

		case WLM_SETTING_ARRB:
			Arr_BSubAdd(dir);
		break;

		case WLM_SETTING_ARRC:
			Arr_CSubAdd(dir);
		break;

		case WLM_SETTING_CHAN:
			/*
			if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				LightChanSet(dir);
			}
			else if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				RfChanSet(dir);
			}*/
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_LIGH_SLA:
					LightChanSet(dir);
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
				case CMD_MODE_RF_MST:
				case CMD_MODE_RF_SLA:
					RfChanSet(dir);
				break;
			}
		break;

		case SETTING_MULTI:
			MultiTimesShift(dir);
		break;

		case SETTING_FRQ:
			MultiFrqShift(dir);
		break;

		case SETTING_ID:
			IdSetSubAdd(dir);
		break;

		case SETTING_RFGR_SEL:
			RfGrSubAdd(dir);
		break;

		case SETTING_RFSCAN:
			ChScanSubAdd(dir);
		break;

		case SETTING_SNRMENU:
			if(g_isSmExpand>0)
			{
				SnrSubMenuSubAdd(dir);
			}else
			{
				SnrMenuSubAdd(dir);
			}
		break;
		
	}
}

void KeyOk(void)
{

//	MenuItem *subItem;
	if(!g_SettingIndex)    return;
	switch(g_SettingIndex)
	{
		case SETTING_SNRMENU:
			if(g_isSmExpand>0)
			{/*
				CLR_SUBMENU_FILD;
				pSnrSet[snrMenu.userSel] = snrMenu.subSel;
				subItem = (MenuItem *)snrMenu.munuItem[snrMenu.userSel].childrenMenus;
				//DspSnrFucStr(userSel-dspStart+2,SUBMENU_COLUMN,subItems[subSel].displayString,0); 
				if(!g_dspChinese)
				{
					DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,MAINMENU_COLUMN,snrMenu.munuItem[snrMenu.userSel].displayString,DSP_NORMAL);
					DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2, SUBMENU_COLUMN, subItem[snrMenu.subSel].displayString,DSP_NORMAL);
				}else
				{
					DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,MAINMENU_COLUMN,snrMenu.munuItem[snrMenu.userSel].cnMenuString,DSP_NORMAL);
					DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2, SUBMENU_COLUMN, subItem[snrMenu.subSel].cnMenuString,DSP_NORMAL);
				}
				if(snrMenu.munuItem[snrMenu.userSel].mFunc!=NULL)
				{
					snrMenu.munuItem[snrMenu.userSel].mFunc();
				}*/
				pSnrSet[snrMenu.userSel] = snrMenu.subSel;
				if(snrMenu.munuItem[snrMenu.userSel].mFunc!=NULL)
				{
					snrMenu.munuItem[snrMenu.userSel].mFunc();
				}
				g_isSmExpand = 0;
			}else
			{
				//SmSubExpand();
				g_isSmExpand = 1;
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case WLM_SETTING_CHAN:
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_LIGH_SLA:
					g_SettingIndex = 0;
					*wlSet.m_chan = g_liteChanSet;
					//SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
				case CMD_MODE_RF_MST:
				case CMD_MODE_RF_SLA:
					g_SettingIndex = 0;
					if(radioSet->rfChan!=g_rfChanSet)
					{
						radioSet->rfChan = g_rfChanSet;
						//SetMstChan(radioSet->rfChan,DSP_NORMAL);
						memset(radioStatue.devCnt,0x00,5);
						CHARGE_LED_OFF;
						g_chargeOk = AD_CHARGE_LOW;
						//RfDevLinkMark(mainPara.workMode,&wlSet);
						UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
					}
					//SetMstChan(radioSet->rfChan,DSP_NORMAL);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				break;
			}
		break;

		default:
		//if((SETTING_ID==g_SettingIndex)||(SETTING_RFSCAN==g_SettingIndex)){DspClearAllPara();}
		if(SETTING_ID==g_SettingIndex)
		{
			if(radioSet->id != g_rfIdSet)
			{
				radioSet->id = g_rfIdSet;
				memset(radioStatue.devCnt,0x00,5);
				CHARGE_LED_OFF;
				g_chargeOk = AD_CHARGE_LOW;
				//RfDevLinkMark(mainPara.workMode,&wlSet);
				UartSctSend(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
			}
			//DspClearAllPara();			
			
		}else if(SETTING_RFSCAN==g_SettingIndex)
		{
			 //radioSet->rfChan = g_rfChanSet;
			 if(radioSet->rfChan!=g_rfChanSet)
			 {
				radioSet->rfChan = g_rfChanSet;
				memset(radioStatue.devCnt,0x00,5);
				CHARGE_LED_OFF;
				g_chargeOk = AD_CHARGE_LOW;
				//RfDevLinkMark(mainPara.workMode,&wlSet);
				UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
			 }
			//DspClearAllPara();
		}
		
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
			case CMD_MODE_LIGH_MST:
			case CMD_MODE_RF_MST:
				//g_SettingRsv = g_SettingIndex;
				if(WORKINGMODE_GR==mainPara.workMode)
				{
					g_grSelRsv = g_grSel;
				}else
				{
					if((WLM_SETTING_ARRA<=g_SettingIndex)&&(g_SettingIndex<=WLM_SETTING_ARRC))
					{
						g_SettingRsv = g_SettingIndex;
					}
				}
				g_SettingIndex = 0;
				g_idSetIndex = 0;
				g_grSel = 0;
				//Mode(mainPara.workMode);
				/*
				ModPasteScr(mainPara.workMode);
				DspMstModIcon(*g_cmdMode);
				RfDevLinkMark(mainPara.workMode,&wlSet);
				LoadButtons();*/
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			break;

			case CMD_MODE_RFLS_MST:
			case CMD_MODE_RFLS_SLA:
				//ModLinkedShot(radioSet->rfMod);
				g_idSetIndex = 0;
				g_SettingIndex = 0;
				//ModLinkedShot(*g_cmdMode);
				//BuildLinkedShotMenu(radioSet->rfMod);
				//BuildLinkedShotMenu(*g_cmdMode);
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			break;
		}
/*
		switch(radioSet->rfMod)
		{
			case RF_LKSHOT_MASTER:
			case RF_LKSHOT_SLAVE:
				ModLinkedShot(radioSet->rfMod);
				BuildLinkedShotMenu(radioSet->rfMod);
			break;

			case RF_SLAVE:
			case RF_MASTER:
			case RF_OFF:
			g_SettingIndex = 0;
			g_idSetIndex = 0;
			Mode(mainPara.workMode);
			LoadButtons();
			break;
		}
*/
		break;
	}
	//LoadButtons();
}

void ExitEditMode(void)
{
	//DspClearAllPara();
	
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_RF_MST:
			//g_SettingIndex = 0;
			//g_idSetIndex = 0;
			//g_grSel = 0;
			//Mode(mainPara.workMode);
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				g_grSelRsv = g_grSel;
			}else
			{
				if((WLM_SETTING_ARRA<=g_SettingIndex)&&(g_SettingIndex<=WLM_SETTING_ARRC))
				{
					g_SettingRsv = g_SettingIndex;
				}
			}
			g_SettingIndex = 0;
			g_idSetIndex = 0;
			g_grSel = 0;
			/*
			ModPasteScr(mainPara.workMode);
			DspMstModIcon(*g_cmdMode);
			RfDevLinkMark(mainPara.workMode,&wlSet);
			LoadButtons();*/
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			//ModLinkedShot(radioSet->rfMod);
			//g_SettingIndex = 0;
			g_SettingIndex = 0;
			g_idSetIndex = 0;
			/*
			ModLinkedShot(*g_cmdMode);
			//BuildLinkedShotMenu(radioSet->rfMod);
			BuildLinkedShotMenu(*g_cmdMode);*/
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;
	}

}


/*
void KeyOk()
{
	g_SettingIndex = 0;
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
#ifdef _CANON_
			//if(WLM_ON==*wlSet.is_masterOn)
			if(CMD_MODE_OFF!=*g_cmdMode)
			{
				DisplayCanMstHead(0,mainPara.workMode,&wlSet);
			}
			CanSynMode(mainPara.workMode,mainPara.synMode,DSP_NORMAL);
			SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_NORMAL);
#endif
		break;

		case WORKINGMODE_MANU:
#ifdef _CANON_
			//if(WLM_ON==*wlSet.is_masterOn)
			if(CMD_MODE_OFF!=*g_cmdMode)
			{
				DisplayCanMstHead(0,mainPara.workMode,&wlSet);
			}
			CanSynMode(mainPara.workMode,mainPara.synMode,DSP_NORMAL);
#endif
		break;

		case WORKINGMODE_MULTI:
#ifdef _CANON_
			//if(WLM_ON==*wlSet.is_masterOn)
			if(CMD_MODE_OFF!=*g_cmdMode)
			{
				DisplayCanMstHead(0,mainPara.workMode,&wlSet);
			}
#endif
			MULTI_SET_ETIMES(mainPara.multi_times);
			MULTI_SET_FRQ(mainPara.multi_Frq);
		break;

#ifdef _NIKON_
		case WORKINGMODE_MASTER:
			wlSet.m_current_arr= 0x0f;
			SetMstHead(&wlSet.m_current_arr);
			SetMstChan(*wlSet.m_chan,0);
			//SetMastPara(*wlSet.m_chan,wlSet.m_arrPara);
		break;
#endif
		case WORKINGMODE_RTC:
		case WORKINGMODE_RTN:
			DisplayRmt(*wlSlave.s_array,*wlSlave.s_chan,g_SettingIndex);
		break;

	}

#if(NEW_DSP_SCHEME==1)
	DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
#endif

}*/

#if(NEW_DSP_SCHEME==1)
void KeyMotoSet()
{
	if(g_SettingIndex)
	{
		KeyOk();
	}
	g_SettingIndex = SETTING_MOTO;
	//*motoSet.mode = MOTO_MODE_MANU;
	DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE); 	
}
#endif

void ModeSw()
{
	if(0==g_SettingIndex)
	{
		if(0==g_modeSwitch)
		{
			//if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			//{
			//	return;
			//}
			mainPara.workMode++;
#ifdef _CANON_
#ifdef _CAN_MODE_GR_
			if(CMD_MODE_RF_MST==*g_cmdMode)
			{
				if((WORKINGMODE_MULTI+1)==mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_GR;
				}
				if(mainPara.workMode>WORKINGMODE_GR)
					mainPara.workMode = WORKINGMODE_TTL;
			}else
			{
				if(WORKINGMODE_MULTI<mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_TTL;
				}
			}
#else
			if(mainPara.workMode>WORKINGMODE_MULTI)
				mainPara.workMode= WORKINGMODE_TTL;
#endif
#else
			if(mainPara.workMode>WORKINGMODE_MASTER)
			{
				LCD_RMT_CLEAR;					//clear wireless master display
				mainPara.workMode = WORKINGMODE_TTL;
			}
#endif
		}
#ifdef _SPEED_LIGHT_
		else if(1==g_modeSwitch)
		{
			mainPara.workMode++;
#ifdef _CANON_
			if(WORKINGMODE_RTN==mainPara.workMode)
				mainPara.workMode = WORKINGMODE_RTC;
#else
			if(WORKINGMODE_RTC==mainPara.workMode)
				mainPara.workMode = WORKINGMODE_F1;	
#endif
			if(mainPara.workMode>WORKINGMODE_F2)
			{
#ifdef _CANON_
				mainPara.workMode = WORKINGMODE_RTC;
#else
				mainPara.workMode = WORKINGMODE_RTN;	
#endif
			}
		}
#endif
	}
	//BuildFuncMenu();
}

void KeyMenuSw(void)
{
	InsertList(funcMenu.funcList[g_menuId]);
	pMenuList = &funcMenu.funcList[g_menuId];
}

void ModLinkedShot(u8 rfMod)
{
	DspClearAllPara();	
	//SetMstChan(radioSet->rfChan,DSP_NORMAL);
	LCD_ShowString(8,8,"LINKED SHOT",FONT_TYPE_BIG,0, DSP_NORMAL);
	switch(rfMod)
	{
		//case RF_LKSHOT_MASTER:
		case CMD_MODE_RFLS_MST:
		LCD_ShowString(8,8+BIG_FONT_WIDTH(12),"master",FONT_TYPE_MAIN,0,DSP_NORMAL);
		break;

		//case RF_LKSHOT_SLAVE:
		case CMD_MODE_RFLS_SLA:
		LCD_ShowString(8,8+BIG_FONT_WIDTH(12),"slave",FONT_TYPE_MAIN,0,DSP_NORMAL);
		break;
	}
	DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_SIZE,rfBitmap,DSP_NORMAL);
	//radioSet->rfMod= LINKED_SHOT_MST;
}

void RfMoudleSet(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			DISABLE_RF_MODULE();
		break;
		
		case CMD_MODE_RF_MST:
			CHARGE_LED_OFF;
			ENABLE_RF_MODULE();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;
			
		case CMD_MODE_LIGH_MST:
			CHARGE_LED_OFF;
			DISABLE_RF_MODULE();
		break;

		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RFLS_MST:
			CHARGE_LED_OFF;
			ENABLE_RF_MODULE();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_SLA:
			DISABLE_RF_MODULE();
		break;

		case CMD_MODE_RF_SLA:
			ENABLE_RF_MODULE();
		break;
	}

}

void RfModSwInfo(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		break;
		
		case CMD_MODE_RF_MST:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;
			
		case CMD_MODE_LIGH_MST:
		break;

		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RFLS_MST:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_SLA:
		break;

		case CMD_MODE_RF_SLA:
		break;
	}

}

/*
void CmdModDisplay(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		break;
		
		case CMD_MODE_RF_MST:
			//ENABLE_RF_MODULE();
			g_SettingIndex=0;
			Mode(mainPara.workMode);
#ifdef _SN_DEBUG_
			g_menuId = 0;
#else
			g_menuId = 0;
#endif
			LoadButtons();
			SetMstChan(radioSet->rfChan,DSP_NORMAL);
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;
			
		case CMD_MODE_LIGH_MST:
			//DISABLE_RF_MODULE();
			g_SettingIndex=0;
			Mode(mainPara.workMode);
#ifdef _SN_DEBUG_
			g_menuId = 0;
#else
			g_menuId = 0;
#endif
			LoadButtons();
			SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RFLS_MST:
			ModLinkedShot(*g_cmdMode);
			BuildLinkedShotMenu(*g_cmdMode);
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_SLA:
		break;

		case CMD_MODE_RF_SLA:
		break;
	}

	FuncDsp();
}*/

void MstModCheck()
{
//	INT8U tmrErr;
#ifdef _ST_EX_
/*
	switch(radioSet->rfMod)
	{
		case RF_OFF:
		case RF_LKSHOT_MASTER:
			*g_cmdMode = CMD_MODE_RF;
			radioSet->rfMod = RF_MASTER;
			Mode(mainPara.workMode);
			LoadButtons();
			UartSctSend(UART_ENCODE_RFMOD, radioSet->rfMod,0);
		break;

		case RF_LKSHOT_SLAVE:
			radioSet->rfMod = RF_LKSHOT_MASTER;
			ModLinkedShot(radioSet->rfMod);
			UartSctSend(UART_ENCODE_RFMOD, radioSet->rfMod,0);
			BuildLinkedShotMenu(radioSet->rfMod);
		break;

		case RF_MASTER:
			*g_cmdMode = CMD_MODE_LIGH_MST;
			radioSet->rfMod = RF_OFF;
			if(WORKINGMODE_GR==mainPara.workMode)	mainPara.workMode = WORKINGMODE_TTL;
			Mode(mainPara.workMode);
			LoadButtons();
			UartSctSend(UART_ENCODE_RFMOD, radioSet->rfMod,0);
		break;

		case RF_SLAVE:
		break;
	}
*/
	switch(*g_cmdMode)
	{
		case CMD_MODE_RF_MST:
#ifdef _RF_ONLY_
			return;
#else
			*g_cmdMode = CMD_MODE_LIGH_MST;	
			UartSctSend(UART_ENCODE_EXIT, 1, 0);
			OSTimeDlyHMSM(0,0,0,30);
			//DISABLE_RF_MODULE();
			//OSTmrStart(g_CcRstDownTmr, &tmrErr);//Disable cc2530
			memset(radioStatue.devCnt,0x00,5);
			if(WORKINGMODE_GR==mainPara.workMode)	mainPara.workMode = WORKINGMODE_TTL;
			//Mode(mainPara.workMode);
			//LoadButtons();
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
			break;
#endif
		//break;

		case CMD_MODE_LIGH_MST:
		case CMD_MODE_RFLS_MST:
			*g_cmdMode = CMD_MODE_RF_MST;
			//ENABLE_RF_MODULE();
			//Mode(mainPara.workMode);
			//LoadButtons();
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_RFLS_SLA:
			*g_cmdMode = CMD_MODE_RFLS_MST;
			//ENABLE_RF_MODULE();
			//ModLinkedShot(*g_cmdMode);
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
			//BuildLinkedShotMenu(*g_cmdMode);
		break;
	}

	//CmdModDisplay();
	RfMoudleSet();
	RfModSwInfo();
	//FuncDsp();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

#else
	(*g_cmdMode)++;
	if(*g_cmdMode>CMD_MODE_RF_MST)
	{
		*g_cmdMode = CMD_MODE_OFF;
	}
	if(CMD_MODE_RF_MST!=*g_cmdMode)
	{
		if(WORKINGMODE_GR==mainPara.workMode)
		{
			mainPara.workMode = WORKINGMODE_TTL;
		}
	}
#endif
	//printf("%x\r\n",*g_cmdMode);

}

void KeyToLinkMod()
{
/*
	if(RF_LKSHOT_SLAVE!=radioSet->rfMod)
	{
		radioSet->rfMod = RF_LKSHOT_SLAVE;
		*g_cmdMode = CMD_MODE_RF_MST;
		ModLinkedShot(radioSet->rfMod);
		BuildLinkedShotMenu(radioSet->rfMod);
		UartSctSend(UART_ENCODE_RFMOD, radioSet->rfMod,0);
	}
*/
	if(CMD_MODE_RFLS_SLA!=*g_cmdMode)
	{
		*g_cmdMode = CMD_MODE_RFLS_SLA;
		//mainPara.workMode = WORKINGMODE_TTL;
		g_SettingIndex = 0;
		ENABLE_RF_MODULE();
		//ModLinkedShot(*g_cmdMode);
		//BuildLinkedShotMenu(*g_cmdMode);
		UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		memset(radioStatue.devCnt, 0, 5);
		//FuncDsp();
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
}

void LcdStandByTest()
{
	static char flag=0;
	if(!flag)
	{
		LCD_StandBy();
		flag = 1;
	}else
	{
		LCD_StandByExit();
		flag = 0;
	}
}

void Pilot(void)
{

	//if(flaPara.charge_ok<CHARGE_FUL)
	//return;


	if(WORKINGMODE_MANU==mainPara.workMode)
	{
#ifdef _RF_ONLY_
		if(CMD_MODE_RF_MST==*g_cmdMode)
#else
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			BuildExposeCmd(mainPara.workMode,HP_FLASHMODE_OFF,0);
			//wlSet.m_cmdProc=WLM_CMDPROC_ON;
			//Delay_10us(15);
			OS_ENTER_CRITICAL();
			WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
			OS_EXIT_CRITICAL();
			delay_us(800);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			//Delay_10us(50);
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
#endif
		{
			//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
			if(pSnrSet[SNRMENU_MODELING])
			{
				UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			}else
			{
				RfFlashConfirm(2,FLASH_PILOT_ON);
				delay_ms(20);
				RfCountDownInfo(1000);
			}
			
		}else
		{
			pTime = CanOutputSwitch(WORKINGMODE_MANU);
			TURN_ON_TRIGER(pTime);
		}
		//Delay_ms(1);
		
	}
	else if(WORKINGMODE_TTL==mainPara.workMode)
	{
#ifdef _RF_ONLY_
		if(CMD_MODE_RF_MST==*g_cmdMode)
#else
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
#ifdef LIGH_CMD_REBUILD
			wlSet.m_flaCmd[0] = 0xef;

			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
				wlSet.m_flaCmd[1] = 0xB0;
				break;

				case WLM_MODE_AB:
				wlSet.m_flaCmd[1] = 0xB4;
				break;

				case WLM_MODE_RES:
				wlSet.m_flaCmd[1] = 0xB5;
				break;
			}

			wlSet.m_flaCmd[2] = 0x55;
			wlSet.m_flaCmd[3] = 0xAA;
			wlSet.m_flaCmd[4] = 0x8F;
			wlSet.m_cmdIndex = 5;
#else
			wlSet.m_flaCmd[0] = 0xef;
			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
				wlSet.m_flaCmd[1] = 0x84;
				break;

				case WLM_MODE_AB:
				wlSet.m_flaCmd[1] = 0xa4;
				break;

				case WLM_MODE_RES:
				wlSet.m_flaCmd[1] = 0xac;
				break;
			}

			wlSet.m_flaCmd[2] = 0xb5;
			wlSet.m_flaCmd[3] = 0x4a;
			wlSet.m_flaCmd[4] = 0x55;
			wlSet.m_flaCmd[5] = 0x1f;
			wlSet.m_flaCmd[6] = 0x01;
			wlSet.m_cmdIndex = 7;
#endif

			OS_ENTER_CRITICAL(); 	
			WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
			OS_EXIT_CRITICAL();
			delay_us(800);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
	
			
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
#endif
		{
			//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
			//UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			if(pSnrSet[SNRMENU_MODELING])
			{
				UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			}else
			{
				RfFlashConfirm(2,FLASH_PILOT_ON);
				//wlSet.m_ttlCmd[0] = wlSet.m_ttlCmd[1] = wlSet.m_ttlCmd[2] = 0x98;
				delay_ms(20);
				RfCountDownInfo(1000);
			}
			//g_dspUpdateFlag |= DSP_UPDATE_SHOT;
		}else
		{
			pTime = CanOutputSwitch(WORKINGMODE_MANU);
			TURN_ON_TRIGER(pTime);//HP_REALFLA_INTERVAL
			//TURN_ON_TRIGER(HP_REALFLA_INTERVAL);			
		}
	}
	else if(WORKINGMODE_MULTI==mainPara.workMode)
	{
#ifdef _RF_ONLY_
		if(CMD_MODE_RF_MST==*g_cmdMode)
#else
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			multiSet.sCode = 0x40;
			wlSet.muitl_realTimes = *multiSet.eTimes;
			BuildExposeCmd(mainPara.workMode,HP_FLASHMODE_OFF,0);
			//wlSet.m_cmdProc=WLM_CMDPROC_ON;
			//Delay_10us(15);
			OS_ENTER_CRITICAL(); 
			WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
			OS_EXIT_CRITICAL();
			delay_us(800);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			//Delay_10us(50);
			
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
#endif
		{
			//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
			//UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			if(pSnrSet[SNRMENU_MODELING])
			{
				UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			}else
			{
				RfFlashConfirm(2,FLASH_PILOT_ON);
				delay_ms(20);
				RfCountDownInfo(1000);
			}
		}else
		{
		
			if((unsigned long)(1000*(*multiSet.eTimes))<(unsigned long)(500*(*multiSet.frequence)))
			{
				multiSet.finalExpTimes = *multiSet.eTimes;
			}else
			{
				multiSet.finalExpTimes = ((*multiSet.frequence)/2+1);
			}
			
			while(multiSet.finalExpTimes--)
			{
				pTime = CanOutputSwitch(WORKINGMODE_MULTI);
				TURN_ON_TRIGER(pTime);
				delay_ms(1000/(*multiSet.frequence));
			}
		}
	}else if(WORKINGMODE_GR==mainPara.workMode)
	{
		//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
		if(pSnrSet[SNRMENU_MODELING])
		{
			UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
		}else
		{
			//flaPara.rfCmuPara[0].ttlExpVal = flaPara.rfCmuPara[1].ttlExpVal = flaPara.rfCmuPara[2].ttlExpVal
			//=flaPara.rfCmuPara[3].ttlExpVal=flaPara.rfCmuPara[4].ttlExpVal = 0x98;			
			RfFlashConfirm(2,FLASH_PILOT_ON);
			delay_ms(20);
			RfCountDownInfo(1000);
		}
		//g_dspUpdateFlag |= DSP_UPDATE_SHOT;
	}else if(WORKINGMODE_F2==mainPara.workMode)
	{
		pTime = CanOutputSwitch(WORKINGMODE_MANU);
		TURN_ON_TRIGER(pTime);
		delay_us(800);
		//Timer7SetCapture();
	}else if(WORKINGMODE_F1==mainPara.workMode)
	{
		pTime = CanOutputSwitch(WORKINGMODE_MANU);
		TURN_ON_TRIGER(pTime);
		delay_us(800);
	}else if(WORKINGMODE_RTC==mainPara.workMode)
	{
		pTime = CanOutputSwitch(WORKINGMODE_MANU);
		TURN_ON_TRIGER(pTime);
		delay_us(800);
	}

}

void ResetKeyInt(void)
{
#if(_HARD_WARE_==HARD_WARE_VER3)
	Ex_NVIC_Enable(10);
#elif(_HARD_WARE_==HARD_WARE_RFONLY)
	Ex_NVIC_Enable(10);
#else
	Ex_NVIC_Enable(1);
#endif
}

void ParaClear(void)
{
	memset((u8*)&mainPara,0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	wlSet.m_arrPara[WL_ARRARY_B].cmp1 = 6;
	mainPara.snrMenuSet[SNRMENU_STBY] = 1;
	mainPara.sumVeri=CalcSum((u8*)&mainPara.workMode,((char*)&mainPara.sumVeri-(char*)&mainPara.workMode));
	STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara.workMode,((char*)&mainPara.rsv-(char*)&mainPara.workMode+2)/2);
	ParaFormat();
	//Mode(mainPara.workMode);
#ifdef _SN_DEBUG_
	g_menuId = 0;
#else
	g_menuId = 0;
#endif
	//LoadButtons();
	RfParaSend();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
	
}


/////////////////////////////////////////////////////////////////////////////////
//键处理函数
void KeyProcess(void)
{
	unsigned char rkey;
	static char lockRs=0;
	static char lockTirFlag=0;
	static char lockCnt=0;
	static char unModulateFlag=0;
	rkey = ReadKey();

#if 0
	if((GETBIT(g_LockMusic,SYS_LOCK))&&(rkey!=KEY_LOCK))
	{
		HaltSec(50);
		return;
	}
#endif

/*
	if(!KEY_LOCK_EX)
	{
		HaltSec(50);
		return;
	}
*/
	if(KEY_PILOT==rkey)
	{
		if(g_cmrLinked>0)
		{
			return;
		}else
		{
			Pilot();
			HaltSec(50);
			return;
		}
	}

	
	if(!KEY_LOCK_EX)
	{
		if(!lockRs)
		{
			lockRs = 1;
			//DISABLE_DIRINT();
			ResetKeyInt();
			if(g_dspChinese>0)
			{
				LcdClearFeild(LAYOUT_MENU_PAGE-2,0,LCD_W,LAYOUT_BUTTON_HIGHT+2);
			}else
			{
				LcdClearFeild(LAYOUT_MENU_PAGE,0,LCD_W,LAYOUT_BUTTON_HIGHT);
			}
		}else if(rkey>0)
		{
			LcdFillBlank(LAYOUT_MENU_PAGE, 0, LCD_W, LAYOUT_BUTTON_HIGHT, DSP_NORMAL);
			LcdDspAtCenter(LAYOUT_MENU_PAGE,0,"LOCKED",FONT_TYPE_MENU,DSP_REVERSE,LCD_W,MAIN_FONT_WIDTH(6));
			if(BK_OFF==bkParas.bkState)
			{
				BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
				gWSleepCounter = 0;
			}
			ResetKeyInt();
			lockTirFlag = 1;
			lockCnt = 0;
		}else if(lockTirFlag>0)
		{
			if(lockCnt++>40)
			{
				//LcdFillBlank(LAYOUT_MENU_PAGE, 0, LCD_W, LAYOUT_BUTTON_HIGHT, DSP_NORMAL);
				LcdClearFeild(LAYOUT_MENU_PAGE,0,LCD_W,LAYOUT_BUTTON_HIGHT);
				lockCnt = 0;
				lockTirFlag = 0;
			}
		}
		
		//ResetKeyInt();
		HaltSec(50);
		return;
	}else
	{
		if((lockRs)&&(!KEY_POW_ON))
		{
			lockRs = 0;
			if(g_dspChinese>0)
			{
				LcdClearFeild(LAYOUT_MENU_PAGE-2,0,LCD_W,LAYOUT_BUTTON_HIGHT+2);
			}else
			{
				LcdClearFeild(LAYOUT_MENU_PAGE,0,LCD_W,LAYOUT_BUTTON_HIGHT);
			}
			ResetKeyInt();
			LoadButtons();
		}
	}


	if(rkey>0)
	{
		if(BK_OFF==bkParas.bkState)
		{
			//BUZZER_ON(BUZZER_TIME_NORMAL,1);
			BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
			gWSleepCounter = 0;
			ResetKeyInt();
			HaltSec(50);
			return;
		}else
		{
			//BUZZER_ON(BUZZER_TIME_NORMAL,1);
			BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
			gWSleepCounter = 0;
		}
	}else
	{
		//HaltSec(50);
		//HaltSec(10);
		return;
	}

	//printf("%x\n",rkey);

	switch(rkey)
	{
		case KEY_MODE:
#ifdef KEY_DEBUG
		printf("mode");
#else
		if(0==g_SettingIndex)
		{
			if((CMD_MODE_RFLS_MST!=*g_cmdMode)&&(CMD_MODE_RFLS_SLA!=*g_cmdMode))
			{
				ModeSw();
				//Mode(mainPara.workMode);
#ifdef _SN_DEBUG_
				g_menuId = 0;
#else
				g_menuId = 0;
#endif
				g_SettingRsv = 0;
				g_grSelRsv = 0;
				//LoadButtons();
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}

		}else 
		{
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				SelectMode(mainPara.workMode,wlSet.m_current_arr);
			}else if(g_SettingIndex<=SETTING_ADDSUB)
			{
				g_SettingIndex = 0;
				g_SettingRsv = 0;
				g_grSelRsv = 0;
				if((CMD_MODE_RFLS_MST!=*g_cmdMode)&&(CMD_MODE_RFLS_SLA!=*g_cmdMode))
				{
					ModeSw();
					//Mode(mainPara.workMode);
					g_menuId = 0;
					//LoadButtons();
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
			}
		}
#endif
		HaltSec(50);
		break;

		case KEY_MODE_LONG:
#ifdef KEY_DEBUG
		printf("mode long");
#else
		SwithMode();
#endif
		break;

		case KEY_CLEAR:
			ParaClear();
		break;

		case KEY_RF_SW:
#ifdef KEY_DEBUG
			printf("rf");
#else
		if(g_SettingIndex<=SETTING_ADDSUB)
		{
			MstModCheck();
			g_SettingRsv=0;
			g_grSelRsv=0;
		}
		
#endif
		HaltSec(50);
		break;

		case KEY_FUNC1:
#ifdef KEY_DEBUG
		printf("func1");
#else
		if(pMenuList->button[0]->actionFuc.pPressFuc!=NULL)
		{
			pMenuList->button[0]->actionFuc.pPressFuc();
		}
		//KeySetting(&mainPara.workMode,&g_SettingIndex,&wlSet);
		//KeySetting();
#endif
		HaltSec(50);
		break;

		case KEY_FUNC2:
#ifdef KEY_DEBUG
		printf("func2");
#else
		if(pMenuList->button[1]->actionFuc.pPressFuc!=NULL)
		{
			pMenuList->button[1]->actionFuc.pPressFuc();
		}
#endif
		HaltSec(50);
		break;

		case KEY_FUNC3:
#ifdef KEY_DEBUG
			printf("func3");
#else
			//KeyLeft(mainPara.workMode);
			if(pMenuList->button[2]->actionFuc.pPressFuc!=NULL)
			{
				pMenuList->button[2]->actionFuc.pPressFuc();
			}
#endif
			HaltSec(50);
		break;

		case KEY_MENU_SW:
#ifdef KEY_DEBUG
			printf("menu");
#else
			if(pMenuList->button[3]->actionFuc.pPressFuc!=NULL)
			{
				pMenuList->button[3]->actionFuc.pPressFuc();
			}
			//KeyRight(mainPara.workMode);
#endif
			HaltSec(50);
		break;

		case KEY_OK:
#ifdef KEY_DEBUG
			printf("ok");
#else
			KeyOk();
#endif
			HaltSec(50);
		break;


		case KEY_LEFT:
#ifdef KEY_DEBUG
			printf("left");
#else
			//KeyLeft(mainPara.workMode);
			KeySubAdd(KEY_DIR_LEFT);
#endif
			//HaltSec(200);
			//HaltSec(100);
			//HaltSec(10);
			ResetKeyInt();
		break;

		case KEY_RIGHT:
#ifdef KEY_DEBUG
			printf("right");
#else
			//KeyRight(mainPara.workMode);
			KeySubAdd(KEY_DIR_RIGHT);
#endif
			//HaltSec(200);
			//HaltSec(100);
			//HaltSec(10);
			ResetKeyInt();
		break;	
		

/*
		case KEY_LOCK:
			if(GETBIT(g_LockMusic,SYS_LOCK))
			{
				LCD_LOCK_OFF;
				CLRBIT(g_LockMusic,SYS_LOCK);
			}else
			{
				LCD_LOCK_ON;
				SETBIT(g_LockMusic,SYS_LOCK);
			}
		break;
*/

		case KEY_SELE_LONG:
#ifdef KEY_DEBUG
			printf("sel long");
#else
			KeyOk();			
			//CanMstSet();
#endif
		break;

		case KEY_LEFT_LONG:
#ifdef KEY_DEBUG
			printf("left long");
#else
			//KeyLeft(mainPara.workMode);
			KeySubAdd(KEY_DIR_LEFT);
#endif
			HaltSec(200);
			HaltSec(100);
		break;

		case KEY_RIGHT_LONG:
#ifdef KEY_DEBUG
			printf("right long");
#else
			//KeyRight(mainPara.workMode);
			KeySubAdd(KEY_DIR_RIGHT);
#endif
			HaltSec(200);
			HaltSec(100);
		break;

		case KEY_RF_SW_LONG:
			KeyToLinkMod();
			HaltSec(500);
		break;

		case KEY_FUNC1_LONG:
		if(pMenuList->button[0]->actionFuc.pHoldFuc!=NULL)
		{
			pMenuList->button[0]->actionFuc.pHoldFuc();
		}
		HaltSec(200);
		//WakeUp_Init(FTIR);
		break;

		case KEY_PILOT:
			//LcdStandByTest();
			Pilot();
			//KeyOk();
			HaltSec(50);
		break;

		case KEY_OK_LONG:
			if((1203==radioSet->id)&&(!PILOT_IN))
			{
				if(!unModulateFlag)
				{
					unModulateFlag = 1;
					UartSctSend(UART_ENCODE_UNMODULATE,0,0);
				}else
				{
					unModulateFlag = 0;
					DISABLE_RF_MODULE();	
					delay_ms(20);
					ENABLE_RF_MODULE();
				}
			}
			HaltSec(50);
		break;

		default:
			HaltSec(50);
		break;
	}

}

void AfAutoCtrl(void)
{
	if(g_cmrPressed>0)
	{
		if((!pSnrSet[SNRMENU_AF])&&(!g_shutterFlag))
		{
#ifdef _RF_ONLY_
			//if(AF_CTL_IN^AF_CTL_OUT)
			//{
				//AF_CTL_OUT = AF_CTL_IN;
				//FLA_OK_LED=!FLA_OK_LED;
			//}
			/*
			if(AF_CTL_IN)
			{
				AF_CTL_OUT = 1;
			}else
			{
				AF_CTL_OUT = 0;
			}*/
			if((!AF_CTL_IN)^(AF_CTL_OUT))
			{
				AF_CTL_OUT = !AF_CTL_IN;
			}
			
#else
			if(AF_CTL_IN^AF_CTL_OUT)
			{
				AF_CTL_OUT = AF_CTL_IN;
				//FLA_OK_LED=!FLA_OK_LED;
			}
#endif
		}
	}
}

#define AD_VCHK_TIMS 16
//300v~1100mv
//280v~1050mv
#define VCK_LEVEL	81//85//185
#define RSV_VALID(x) (x>>4)  
void ChkVoltage()
{
	static u16 vChk=0;
	static u8 cIndex=0;
	u16 adTmp=0;
	
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_CKV;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));
	adTmp = ADC1->DR;

	vChk += (adTmp>>4);
	
	if(++cIndex>=AD_VCHK_TIMS)
	{
		cIndex = 0;
		//if((vChk>>4)>VCK_LEVEL)
#ifdef UART1_TRACE_DEBUG
		printf("VCK: %d\r\n", RSV_VALID(vChk));
#endif
		if(RSV_VALID(vChk)>VCK_LEVEL)
		{
			CHARGE_LED_ON;
			g_chargeOk = AD_CHARGE_RDY;
		}else
		{
			CHARGE_LED_OFF;
			g_chargeOk = AD_CHARGE_LOW;
		}
		vChk = 0;
	}
}


void TaskKeyScan(void *pdata)
{
//	u16 tsCnt=0;
	
	while(1)
	{
		//g_setFlag =1;
		KeyProcess();	
		//g_setFlag =0;
		AfAutoCtrl();
#ifndef _RF_ONLY_ 
		if(CMD_MODE_LIGH_MST==*g_cmdMode)		//2015/09/16
		{
			ChkVoltage();
		}
#endif
		//COM_AUX = !COM_AUX;
		//if(tsCnt++>200)
		//{
		//	tsCnt = 0;
		//	CHARGE_LED=!CHARGE_LED;
		//}
		HaltSec(10);
		//HaltSec(1);
		//HaltSec(1);
	}
}

void EXTI1_IRQHandler(void)
{
	//CR_READY = !CR_READY;
	//delay_ms(20);
	/*
	if(KEY_DIR_1)
	{
		g_keyDir = KEY_DIR_LEFT;
	}else
	{
		g_keyDir = KEY_DIR_RIGHT;
	}*/
#if(_HARD_WARE_==HARD_WARE_VER3)
	CLEAR_KEYINT();
	//EXTI->PR=1<<1;
#elif(_HARD_WARE_==HARD_WARE_RFONLY)
	CLEAR_KEYINT();
#else
	g_keyDir = 1;
	DISABLE_DIRINT();
	//Ex_NVIC_Disable(1);
	CLEAR_KEYINT();
	//EXTI->PR=1<<1;  //清除LINE0上的中断标志位  
#endif
}

void EXTI15_10_IRQHandler()
{
/*
	WFI_SET();
	EXTI->PR=1<<14;
	Ex_NVIC_Disable(14);
	ADC1->CR2 |= 0x01;
*/
	static u8 shadowKey=0;
	//CHARGE_LED = !CHARGE_LED;
	//Ex_NVIC_Disable(14);
	//BK_LIGHT = !BK_LIGHT;
#if(_HARD_WARE_==HARD_WARE_VER3)
	g_keyDir = 1;
	//Ex_NVIC_Disable(10);
	DISABLE_DIRINT();
	CLEAR_KEYINT();
	//EXTI->PR=1<<10;  //清除LINE0上的中断标志位 
#elif(_HARD_WARE_==HARD_WARE_RFONLY)
	if(EXTI_GetITStatus(EXTI_Line15) == SET)//                 ／／判断是否是１３脚的中断
       {
               EXTI_ClearITPendingBit(EXTI_Line15);//                ／／清除中断标志位！
               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            ／／点亮ＬＥＤ            
        }
	
	if(!shadowKey)
	{
	/*
		if(KEY_DIR_1)
		{
			g_keyDir = KEY_DIR_RIGHT;
		}else
		{
			g_keyDir = KEY_DIR_LEFT;
		}
	*/
		if(!KEY_DIR_1)
		{
			shadowKey = KEY_DIR_LEFT;
		}else
		{
			shadowKey = KEY_DIR_RIGHT;
		}
		SET_KEYINT_RTIG();
	}else
	{
		if((KEY_DIR_LEFT==shadowKey)&&(KEY_DIR_1>0))
		{
			//g_keyDir = KEY_DIR_RIGHT;
			g_keyDir = KEY_DIR_LEFT;
			SET_KEYINT_FTIG();
			DISABLE_KEY_INT();
		}else if((KEY_DIR_RIGHT==shadowKey)&&(!KEY_DIR_1))
		{
			//g_keyDir = KEY_DIR_LEFT;
			g_keyDir = KEY_DIR_RIGHT;
			SET_KEYINT_FTIG();
			DISABLE_KEY_INT();
		}else
		{	
			SET_KEYINT_FTIG();
			ENABLE_KEY_INT();
			//FLAOK_TOGGLE();
		}
		shadowKey = 0;
	}
	//DISABLE_KEY_INT();
	CLEAR_KEYINT();
#else
	EXTI->PR=1<<14;
#endif
	//Sys_Soft_Reset();
	//iap_load_app(FLASH_APP1_ADDR);
	
	
}




////////////////////////////////////////////////////////

 
