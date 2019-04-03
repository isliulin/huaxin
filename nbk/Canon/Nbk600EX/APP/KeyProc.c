#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "led.h" 
#include "canon.h"
#include "menu.h"
#include "flashpara.h"
#include "../SYSTEM/usart/usart.h"
#include "../HARDWARE/LCD/Font.h"
#include "../HARDWARE/LCD/lcd.h"

//#define KEY_DEBUG
//StruMode	struMode;
signed char g_rfChanSet=0;
signed char g_liteChanSet=0;
unsigned int g_rfIdSet=0;
FlaParameter flaPara={0};
MotoSet motoSet={0};
//MasterPara masterPara;
WlSet wlSet;
//MultiSet multiSet;
char g_SettingIndex = 0;
char g_RgbCheckTurn=0;
signed char g_ArrSel=0;
//char g_modeSwitch=0;
unsigned int g_bzDuration=0;
char g_bzCount = 0;
char g_bzStart = 0;
//char g_bkStart = 0;
//char g_bkCount = 0;
char g_idSetIndex = 0;
char g_chargeOk=0;
//char g_lock = 0;
//unsigned char g_SeniorSetting=0;
Byte  g_zoom_statue = 0;
//Word gWSleepCounter=0;
u32 gWSleepCounter=0;
u8 g_SlavePowerDown=0;
unsigned char uart_tran_flag=0;
unsigned char g_fpLoc[2];
unsigned int pTime=0;
//extern char *pSnrSet;
extern signed char *pSnrSet;
//unsigned int g_key1;
//unsigned int g_key2;
char g_snrCnt;
//unsigned char g_wlCmdMod=CMD_MODE_RF;
extern RadioSet *radioSet;
extern unsigned int g_Key;
unsigned char g_menuId=0;
unsigned char g_menuIdRsv=0;
FuncList *pMenuList=NULL;
extern FuncMenu funcMenu;
unsigned char g_LockMusic=0;
extern unsigned char *g_cmdMode;
extern MultiSet multiSet;
extern signed char g_grSel;
//unsigned char g_keyDir=1;
unsigned char g_keyDir=0;
extern RadioStatue radioStatue;
//extern u8 g_bkState;
//extern u8 g_bkFlag;
BkParas bkParas;
extern signed char g_pfnIndex;
extern signed char g_pfnStart;
extern signed char g_chSan;
extern u16 g_dspUpdateFlag;
extern u8 g_isModeling;
extern unsigned char g_shutterFlag;
extern char g_slaveReady;
//extern signed char g_cfnSubSel;
#ifdef MOTO_TEST_DSP
extern u16 g_motoTurn;
#endif
extern unsigned char g_comStatue;
u8 g_chargeflag;

#ifdef MOTO_TEST
extern u16 g_motoTurn;
#endif

u8 g_isIndividualSlave=0;


extern const char cmpStringEx[];
//extern char g_paraSetBuf[208];
extern unsigned char g_paraSetBufIndex;
extern const char cmpArr[][4];
extern const char m_dotSetLocEx[];
extern const char multi_dotSetLocEx[];
extern const int isoValue[];
extern const int apertValue[];
extern unsigned char const feb_table[10];
extern unsigned char const feb_table_5[7];
extern WLM_abRation_fillwords const wlm_abRation_fillwords[13];
extern unsigned char abRation_fill1,abRation_fill2;
extern u8 g_chargeShadow;
extern u8 g_AfIndex;
extern char g_motoInvalidCounter;

//void Mode(void);
void KeyLeft(signed char mode);
void KeyRight(signed char mode);
//void KeyOk();
void ModLinkedShot(void);
void ResetKeyInt(void);
extern void *BtnHome(void);
extern void DspUpdate(void);

#ifdef FIBER_ADJUST_PROC
extern void FiberAdjustProc(void);
#endif

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
/*
unsigned char canMaxCalc[][9] = {
{90,80,70,55,45,35,35,30,25},//1/128
{90,80,70,55,45,35,35,30,25},//1/64
{60,50,40,35,30,25,25,20,15},//1/32
{30,20,20,20,20,20,10,10, 8},//1/16
{14,12,10, 8, 6, 6, 5, 5, 4},//1/8
{7,6,5,4,3,2,2,2,2},//1/4
};
*/
unsigned char canMaxCalc[][14] = {
{100,100,100,100,100,90,80,70,70,60,50,40,40,30},//1/128
{90,90,90,80,80,70,60,50,40,40,35,30,20,15},//1/64
{60,60,60,50,50,40,30,20,20,20,18,16,12,10},//1/32
{30,30,30,20,20,20,10,8,8,8,8,8,8,8},//1/16
{14,14,12,10,8,6,5,4,4,4,4,4,4,4},//1/8
{7,6,5,4,4,3,3,2,2,2,2,2,2,2},//1/4
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
//#define HaltSec(ms) OSTimeDlyHMSM(0,0,0,ms)

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

unsigned char  ReadKey(void)
{
 	static unsigned char short_once=0;//短按键标志
	static unsigned char long_once=0;
	static unsigned char keyIndexRsv=0;
	//static unsigned char pilotEn=0;
	static unsigned int keytimedata=0;
	//static unsigned char long_do_only_one;
	//static unsigned char do_only_one;
	unsigned int key,vKey;
	unsigned char keyIndex=0;
	
	//unsigned char keynum=0;
	
	//if((g_key1>ADCHAN1_KEY_INVALID)&&(g_key2>ADCHAN2_KEY_INVALID))//no press key
	key = GetKeyAdc();
	//printf("%x ",key);
	//HaltSec(100);
	//return;
#ifdef NBK600EX_DEBUG
	key = 0;
#endif
#ifdef KEY_DEBUG
	if(key>0)
	{
		printf("%d ",key);
	}
#endif

	if(!KeyValid(key))
	{
		if(short_once>0)
		{
			if(long_once>0)
			{
				long_once = 0;
				short_once = 0;
				return RKEY_INVALID;
			}else
			{
				short_once = 0;
				return keyIndexRsv;
			}
		}
		
		key = ReadPilot();
		if(RKEY_INVALID==key)
		{
			keytimedata = 0;
			short_once = 0;
			long_once = 0;
		}else
		{
			return KEY_PILOT;
		}

		if(1==g_isModeling)
		{
			g_isModeling = 2;
			if((0==pSnrSet[SNRMENU_MODELING])||(2==pSnrSet[SNRMENU_MODELING]))
			{
				return KEY_PILOT;
			}
		}
		

		if(KEY_DIR_LEFT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_LEFT;
		}else if(KEY_DIR_RIGHT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_RIGHT;
		}
		
	
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
		//keyIndex = KEY_RF_SW;
		keyIndex = KEY_MODE;
	}else if((key<KEY1_VALUE_MAX)&&(key>KEY1_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC1;
		keyIndex = KEY_RF_SW;
	}else if((key<KEY2_VALUE_MAX)&&(key>KEY2_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC2;
		if(key<169)
		{
			keyIndex = KEY_CLEAR;
		}else
		{
			keyIndex = KEY_FUNC1;
		}
	}else if((key<KEY3_VALUE_MAX)&&(key>KEY3_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC3;
		keyIndex = KEY_FUNC2;
	}else if((key<KEY4_VALUE_MAX)&&(key>KEY4_VALUE_MIN))
	{
		//keyIndex = KEY_MENU_SW;
		keyIndex = KEY_FUNC3;
	}else if((key<KEY5_VALUE_MAX)&&(key>KEY5_VALUE_MIN))
	{
		//keyIndex = KEY_MODE;
		keyIndex = KEY_MENU_SW;
	}else if((key<KEY6_VALUE_MAX)&&(key>KEY6_VALUE_MIN))
	{
		keyIndex = KEY_LEFT;
	}else if((key<KEY7_VALUE_MAX)&&(key>KEY7_VALUE_MIN))
	{
		keyIndex = KEY_OK;
	}else if((key<KEY8_VALUE_MAX)&&(key>KEY8_VALUE_MIN))
	{
		keyIndex = KEY_RIGHT;
	}

	if((KEY_RF_SW<=keyIndex)&&(keyIndex<=KEY_CLEAR))
	{
		//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
		keytimedata++;
		if(!short_once)
		{
			short_once=1;
			keyIndexRsv = keyIndex;
			return RKEY_INVALID;
			//return keyIndex;
		}else if(keytimedata>LONG_KEYPRESS_TIME)
		{
			if(!long_once)
			{
				long_once = 1;
				short_once = 0;
				return (keyIndex|0x10);
			}
		}
	}

	return RKEY_INVALID;
}

void DspCmpIcon(char rvs)
{
	//char page,cml;
	char cml;

	memset(g_paraSetBuf,0x00,FONT_CMP_WT*2);
	g_paraSetBufIndex=0;

	for(cml=0;cml<FONT_CMP_WT*2;cml++)
	{
		g_paraSetBuf[cml] = font_cmp[cml];
	}

	if(CMD_MODE_OFF==*g_cmdMode)
	{
		//page = LAYOUT_PARA_AREA2;
		//cml = LAYOUT_PARAEX_COLUMN+1;//LAYOUT_CMLPART(1);
		AreaSet(LAYOUT_PARA_AREA2,LAYOUT_PARAEX_COLUMN+1,FONT_CMP_WT,FONT_CMP_HT);
	}else
	{
		//page = LAYOUT_PARA_AREA1;
		//cml = LAYOUT_CMLPART(1);//LAYOUT_PARAEX_COLUMN+1;
		//AreaScroll(g_paraSetBuf,FONT_CMP_WT,FONT_CMP_HT,SCROLL_DOWN,);
		AreaSet(LAYOUT_PARA_AREA1,LAYOUT_CMLPART(1),FONT_CMP_WT,FONT_CMP_HT);
	}

	for(cml=0;cml<FONT_CMP_WT*2;cml++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[cml]);
	}
	//DisplayPicture(page,cml,FONT_CMP_WT,FONT_CMP_HT,font_cmp,rvs);
	
}

void DspFebIcon(char rvs)
{
//	char page,cml;
	char cml;
	

	memset(g_paraSetBuf,0x00,FONT_FEB_WT*2);
	g_paraSetBufIndex=0;

	for(cml=0;cml<FONT_FEB_WT*2;cml++)
	{
		g_paraSetBuf[cml] = font_feb[cml];
	}

	if(CMD_MODE_OFF==*g_cmdMode)
	{
		//page = LAYOUT_PARA_AREA2;
		//cml = LAYOUT_PARAEX_COLUMN+1;//LAYOUT_CMLPART(1);
		AreaSet(LAYOUT_PARA_AREA2,90,FONT_FEB_WT,FONT_FEB_HT);
	}else
	{
		//page = LAYOUT_PARA_AREA1;
		//cml = LAYOUT_CMLPART(1);//LAYOUT_PARAEX_COLUMN+1;LAYOUT_PARA_AREA0,LOCATION_TTLFEB_CML,
		//AreaScroll(g_paraSetBuf,FONT_CMP_WT,FONT_CMP_HT,SCROLL_DOWN,);
		AreaSet(LAYOUT_PARA_AREA0,LOCATION_TTLFEB_CML,FONT_FEB_WT,FONT_FEB_HT);
	}

	for(cml=0;cml<FONT_FEB_WT*2;cml++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[cml]);
	}
	//DisplayPicture(page,cml,FONT_CMP_WT,FONT_CMP_HT,font_cmp,rvs);
	
}

#define DSP_FLAG_CMP 0x01
#define DSP_FLAG_LNK 0x02
#define DSP_FLAG_NUM 0x04
#define DSP_FLAG_FEB  0x08


void SetCmpFeb(signed char cmp1,signed char cmp2,signed char feb1,signed char feb2,char isCmp,char sel)
{
	char i=0;
	signed char value1=0,value2=0;
	//char tmp1,tmp2;
	//LCD_TTLPARA_CLEAR;
	
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
		case 0:
		if(pSnrSet[SNRMENU_FEBTURN]>0)
		{
			value1 = cmp1 - feb1;
			value2 = cmp2 - feb2;
		}else
		{
			value1= cmp1;
			value2= cmp2;
		}
		break;

		case 1:
		if(pSnrSet[SNRMENU_FEBTURN]>0)
		{
			value1= cmp1;
			value2= cmp2;
		}else
		{
			value1= cmp1 - feb1;
			value2= cmp2 - feb2;
		}
		break;

		case 2:
		value1= cmp1 + feb1;
		value2= cmp2 + feb2;
		break;
	}

	if(g_stepLength>0)
	{
		if(value2<0) 
		{
			value1--;
			value2 = 2 + value2;		
		}
		else if(value2>2)
		{
			value1++;
			value2 = value2 - 2;
		}

	}else
	{
		if(value2<0) 
		{
			value1--;
			value2 = 3 + value2;		
		}
		else if(value2>2)
		{
			value1++;
			value2 = value2 - 3;
		}
	}

	memset(g_paraSetBuf,0x00,DSP_CMPFEB_LEN*2);
	//RamLoadPicture(char ch, char * gBuf, char dspLen, unsigned char rvs)
	g_paraSetBufIndex=0;
	

	if(SETTING_TTLFEB==sel)
	{
		//LCD_FEB_ON(DSP_SELE);
		//CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
		g_paraSetBufIndex+=DSP_BIAS_LNK;
		LoadCmpFebChar('%', g_paraSetBuf+g_paraSetBufIndex, DSP_CMPFEB_LEN, DSP_NORMAL);
		g_paraSetBufIndex+=(7+1);
		LoadMaskData(g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,0x7F,0xFE,PARA_CMPFEB_WT);
		if(g_stepLength>0)
		{
			LoadCmpFebData(g_paraSetBuf+g_paraSetBufIndex, cmpArr_5[feb1*2+feb2], DSP_SELE);
		}else
		{
			LoadCmpFebData(g_paraSetBuf+g_paraSetBufIndex, cmpArr[feb1*3+feb2], DSP_SELE);
		}
		g_paraSetBufIndex+=(PARA_CMPFEB_WT+2);
		RamLoadPicture((char*)font_feb,FONT_FEB_WT,FONT_FEB_HT,g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
	}else if(SETTING_ADDSUB==sel)
	{
		//LCD_CMP_ON(DSP_SELE);
		//LCD_CMP_ON(DSP_NORMAL);
		RamLoadPicture((char*)font_cmp,FONT_CMP_WT,FONT_CMP_HT,g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
		g_paraSetBufIndex+=FONT_CMP_WT;
		if((!cmp1)&&(!cmp2))
		{
			LoadCmpFebChar('%', g_paraSetBuf+g_paraSetBufIndex, DSP_CMPFEB_LEN, DSP_NORMAL);
		}else
		{
			if(cmp1<0)
			{
				LoadCmpFebChar('-',g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
				if(0==cmp2)
				{
					cmp1 *= (-1);
				}else
				{
					if(g_stepLength>0)
					{
						cmp1 = (-1)*(cmp1 + 1);
						cmp2 = 2 - cmp2;
					}else
					{
						cmp1 = (-1)*(cmp1 + 1);
						cmp2 = 3 - cmp2;
					}
				}
			}else
			{
				LoadCmpFebChar('+',g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
			}
			
		}
		g_paraSetBufIndex+=(7+1);

		LoadMaskData(g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,0x7F,0xFE,PARA_CMPFEB_WT);
		if(g_stepLength>0)
		{
			LoadCmpFebData(g_paraSetBuf+g_paraSetBufIndex,cmpArr_5[cmp1*2+cmp2],DSP_SELE);
		}else
		{
			LoadCmpFebData(g_paraSetBuf+g_paraSetBufIndex,cmpArr[cmp1*3+cmp2],DSP_SELE);
		}
		
		//if(CMD_MODE_OFF==*g_cmdMode)
		//{
		//	DisplayTtlCmp_classStyle(LAYOUT_PARA_AREA2,(LAYOUT_PARAEX_COLUMN+1+FONT_CMP_WT+1),comp1,comp2,DSP_SELE);
		//}else
		//{
		//	DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1),comp1,comp2,DSP_SELE);
		//}
	}else
	{

		if(cmp1||cmp2||isCmp)
		{
			RamLoadPicture((char*)font_cmp,FONT_CMP_WT,FONT_CMP_HT,g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
		}
		g_paraSetBufIndex += (FONT_CMP_WT);
		
		if(value1<0)
		{
			LoadCmpFebChar('-',g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
			if(0==value2)
			{
				value1*= (-1);
			}else
			{
				if(g_stepLength>0)
				{
					value1= (-1)*(value1+ 1);
					value2= 2 - value2;
				}else
				{
					value1= (-1)*(value1+ 1);
					value2= 3 - value2;
				}
			}
		}else
		{
			if(value1||value2)
			{
				LoadCmpFebChar('+',g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
			}
		}
		g_paraSetBufIndex+=7;

		if((value1||value2)||(feb1||feb2))
		{
			//LoadMaskData(g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,0x7F,0xFE,PARA_CMPFEB_WT);
			if(g_stepLength>0)
			{
				LoadCmpFebData(g_paraSetBuf+g_paraSetBufIndex,cmpArr_5[value1*2+value2],DSP_NORMAL);
			}else
			{
				LoadCmpFebData(g_paraSetBuf+g_paraSetBufIndex,cmpArr[value1*3+value2],DSP_NORMAL);
			}
		}

		g_paraSetBufIndex+=(PARA_CMPFEB_WT+4);

		if(feb1||feb2)
		{
			RamLoadPicture((char*)font_feb,FONT_FEB_WT,FONT_FEB_HT,g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
			g_paraSetBufIndex+=(FONT_FEB_WT+2);
			LoadCmpFebChar(flaPara.FEB_count+1+'0',g_paraSetBuf+g_paraSetBufIndex,DSP_CMPFEB_LEN,DSP_NORMAL);
		}
/*		
		if(feb1||feb2)
		{
			//LCD_FEB_ON(lcd_rev);
			LCD_FEB_ON(DSP_NORMAL);
			//LCD_ShowChar(LOCATION_FEBCNT_PAGE,LOCATION_FEBCNT_CML,flaPara.FEB_count+1+'0',FONT_TYPE_BIG, DSP_NORMAL);
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				Lcd_ShowCmpFebChar(LAYOUT_PARA_AREA2,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1+7),flaPara.FEB_count+1+'0',DSP_NORMAL);
				DisplayTtlCmp_classStyle(LAYOUT_PARA_AREA2,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1+7),value1,value2,DSP_NORMAL);
			}else
			{
				Lcd_ShowCmpFebChar(LOCATION_FEBCNT_PAGE,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1),flaPara.FEB_count+1+'0',DSP_NORMAL);
				DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1),value1,value2,DSP_NORMAL);
			}
		}
		else
		{
			LCD_FEB_OFF;
		}

		if(cmp1||cmp2)
		{
			LCD_CMP_ON(DSP_NORMAL);
			DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_CMP_WT+1,value1,value2,DSP_NORMAL);
		}
*/
	
	}
	if((CMD_MODE_OFF==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)
		||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		AreaSet(LAYOUT_PARA_AREA2, LAYOUT_PARAEX_COLUMN, DSP_CMPFEB_LEN, 16);
	}else
	{
		AreaSet(LOCATION_TTLCMP_PAGE, LOCATION_TTLCMP_CML, DSP_CMPFEB_LEN, 16);
	}

	for(i=0;i<DSP_CMPFEB_LEN*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
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

}

void SetManuPara(unsigned char  output1,unsigned char output2,char rvs)
{
	LCD_MANUPARA_CLEAR;
	//DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MANU,output1,output2,DSP_NORMAL);
	//DisplayOutput(LAYOUT_PAGEX_PARA,LAYOUT_PARAEX_COLUMN,WORKINGMODE_MANU,output1,output2,rvs);
	DisplayMstOtp_canStyle(LAYOUT_PAGEX_PARA-1,LAYOUT_PARAEX_COLUMN-3,
						     WORKINGMODE_MANU,DSP_PARAMEX_1LINE,DSP_BLKEX_LEN,
						     output1,output2,3,(char*)m_dotSetLocEx,
						     0,0,
						     rvs);
}

void SetRmtPara(unsigned char  output1,unsigned char output2)
{
	LCD_RMTPARA_CLEAR;
	DisplayOutput(LOCATION_RMTRCT_PAGE,LOCATION_RMTRCT_CML,WORKINGMODE_MANU,output1,output2,DSP_NORMAL);
}

void SetMultiLink(char x, char y)
{
//	u8 i=0;
	memset(g_paraSetBuf,0x01,4);
	memset(g_paraSetBuf+4,0x80,4);
	LcdCopyToRam(x, y+2, g_paraSetBuf, 4, 16);
}

void DspMultiHz(char x, char y)
{
	memset(g_paraSetBuf,0x00,9*2);
	LoadMenuChar('H',g_paraSetBuf,9,DSP_NORMAL);
	LoadMenuChar('z',g_paraSetBuf+6,9,DSP_NORMAL);
	LcdCopyToRam(x, y, g_paraSetBuf, 9, 16);
}

void SetMultiTmFrq(unsigned char mulTimes, int mulFrq, char sel)
{

	if(200==mulFrq)
	{
		mulFrq = 199;
	}
	
	if((CMD_MODE_OFF==*g_cmdMode)||
		(CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		MULTI_SET_ETIMES(3,LAYOUT_PARAEX_COLUMN-1,mulTimes,SETTING_MULTI==sel);
		MULTI_SET_LINK(3,LAYOUT_PARAEX_COLUMN-1+(3*8));
		MULTI_SET_FRQ(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,mulFrq,SETTING_FRQ==sel);
		DspMultiHz(3,LAYOUT_PARAEX_COLUMN-1+(6*8)+8+1);
	}else
	{
		MULTI_SET_ETIMES(2,LAYOUT_PARAEX_COLUMN-1,mulTimes,SETTING_MULTI==sel);
		MULTI_SET_LINK(2,LAYOUT_PARAEX_COLUMN-1+(3*8));
		MULTI_SET_FRQ(2,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,mulFrq,SETTING_FRQ==sel);
		DspMultiHz(2,LAYOUT_PARAEX_COLUMN-1+(6*8)+8+1);
	}
	
}

void SetMultiPara(unsigned char mulExp, unsigned char mulTimes, int mulFrq,char rvs)
{
	LCD_MULPARA_CLEAR;
#ifdef _CANON_
	//DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MULTI,(mulExp>>4),0,DSP_NORMAL);
	DisplayMstOtp_canStyle(LAYOUT_PAGEX_PARA,LAYOUT_PARAEX_COLUMN-3,
							WORKINGMODE_MULTI,DSP_PARAMEX_1LINE,DSP_BLKEX_LEN,
							//(mulExp>>4),0,3,multi_dotSetLocEx,
							mulExp,0,3,(char*)multi_dotSetLocEx,
							SCROLL_UP,2,
							rvs);
#else
	DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MULTI,(mulExp>>4),(mulExp&0x0f),DSP_NORMAL);
	//DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MULTI,(mulExp),(mulExp&0x0f),DSP_NORMAL);
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
	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,chan,DSP_NORMAL,0);
	
	
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
//void CanSynMode(signed char mode,signed char sm,char mDsp)
void CanSynMode(/*signed char mode,*/signed char* sm,char mDsp)
{
	//if((mode>=WORKINGMODE_MULTI)&&(mode!=WORKINGMODE_GR))
	//{
		//return;
	//}				//this is not nessary
	char x,y;

	if((CMD_MODE_OFF==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)
		||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		x = LAYOUT_PARA_AREA0+1;
		y = LAYOUT_PFN_CML+4;
	}else
	{
		x = LAYOUT_PARA_AREA0;
		y = LAYOUT_SYNMODE_LINE;
		if(SYNC_MODE_REAR==*sm)
		{
			*sm = SYNC_MODE_FRON;
		}
	}
	
	switch(*sm)
	{
		case SYNC_MODE_FRON:
			//DisplayPicture(LAYOUT_PARA_AREA1,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFront,mDsp);
			LcdClearFeild(x,y,FONT_SYN_WT,FONT_SYN_HT);
		break;

		case SYNC_MODE_REAR:
			//DisplayPicture(LAYOUT_PARA_AREA1,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synRear,mDsp);
			DisplayPicture(x,y,FONT_SYN_WT,FONT_SYN_HT,synRear,mDsp);
		break;

		case SYNC_MODE_FP:
			DisplayPicture(x,y,FONT_SYN_WT,FONT_SYN_HT,synFp,mDsp);//{DisplayPicture(0,g_fpLoc[mainPara.workMode],19,16,EEPCHAR_PAGE_SYNFP,EEPCHAR_CML_SYNFP,DSP_NORMAL);
		break;
	}
}

#endif

//ttl main display
void SetTtlScr(void)
{
	u8 charLen=0;
	signed char bias=0;
	signed char rbias=0;
	CmpPoint cmpp={0};
	signed char cmp1=0,cmp2=0;
	LCD_MODE_CLEAR;
	//LCD_FEB_OFF;
	//LCD_CMP_CLEAR;
	//LCD_RMT_CLEAR;
	LCD_TTL_ON;

	if(g_menuLock>0)
	{
		return;
	}
	
	if(flaPara.charge_ok>AD_CHARGE_LOW)
	{
		LIGHT_READY;
	}else
	{
		LIGHT_CHARGE;	
	}

#ifdef _CANON_
	//CanSynMode(mainPara.workMode, mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
#endif

	if((mstPara->cmp1||mstPara->cmp2))
	{
		cmp1 = mstPara->cmp1;
		cmp2 = mstPara->cmp2;
	}else if(wlSet.cmrCmp1||wlSet.cmrCmp2)
	{
		//cmp1 = flaPara.cmrCmp1;
		//cmp2 = flaPara.cmrCmp2;
		cmp1 = wlSet.cmrCmp1;
		cmp2 = wlSet.cmrCmp2;
		//if(CMD_MODE_OFF==*g_cmdMode)
		//{
			//AreaSet(LAYOUT_PARA_AREA2, LAYOUT_PARAEX_COLUMN, DSP_CMPFEB_LEN, 16);
			//DisplayPicture(LAYOUT_PARA_AREA2, LAYOUT_PARAEX_COLUMN, FONT_CMP_WT, 16, font_cmp, DSP_NORMAL);
		//}else
		//{
			//AreaSet(LOCATION_TTLCMP_PAGE, LOCATION_TTLCMP_CML, DSP_CMPFEB_LEN, 16);
			//DisplayPicture(LOCATION_TTLCMP_PAGE, LOCATION_TTLCMP_CML, FONT_CMP_WT, 16, font_cmp, DSP_NORMAL);
		//}
	}
	

#ifdef _CANON_

	//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	

	if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);
		LCD_CANMST_ON;
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,(cmp1||cmp2),g_SettingIndex);	
		//SetCmpFeb(cmp1,cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		//DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);WLM_SETTING_ARR==g_SettingIndex
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1)
		|((SETTING_ADDSUB==g_SettingIndex)<<2));
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		//g_SettingIndex = 0;
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		LCD_CANMST_ON;
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,(cmp1||cmp2),g_SettingIndex);	
		//SetCmpFeb(cmp1,cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		//DisplayCanMstHead((g_SettingIndex-2),mainPara.workMode,&wlSet);
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1)
		|((SETTING_ADDSUB==g_SettingIndex)<<2));
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
	}else if(CMD_MODE_OFF==*g_cmdMode)
	{
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
		if(g_stepLength>0)
		{
			cmpp.cmpStr = (char*)cmpStringEx_5;
			cmpp.pDf = 'e';
			cmpp.pOf = 'o';
			charLen = 6;
			bias = 0;
			rbias = 1;
		}else
		{
			cmpp.cmpStr = (char*)cmpStringEx;
			cmpp.pDf = 'd';
			cmpp.pOf = '>';
			charLen= 4;
			bias = 1;
			rbias = 0;
		}
		LCD_CANMST_CLEAR;
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,(cmp1||cmp2),g_SettingIndex);	
		//SetCmpFeb(cmp1,cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		DisplayTtlCmp_canStyle(LAYOUT_PAGEX_PARA,LAYOUT_PARAEX_COLUMN,
		bias,0,rbias,
		cmpp,
		//mstPara->cmp1,mstPara->cmp2,
		cmp1,cmp2,
		///4,DSP_PARAEX_1LINE,
		charLen,DSP_PARAEX_1LINE,
		charLen,
		SCROLL_UP,2,
		SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL
		//DSP_REVERSE
		);
		//DisplayTtlCmp();
		//SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0);	
	}else if(CMD_MODE_RF_SLA==*g_cmdMode)//RF SLAVE,   LIGHT SLAVE
	{
		LCD_CANSLA_ON;
		//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,0,0,0,g_SettingIndex);	
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		//DspSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		//DspSlavePara(mainPara.workMode,SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		//DspLightSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		DspRfSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		LCD_CANSLA_ON;
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,0,0,0,g_SettingIndex);	
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		//DspSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		//DspRfSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		DspLightSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
	}else if(CMD_MODE_RFLS_MST==*g_cmdMode)
	{
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
		//cmpp.cmpStr = (char*)cmpStringEx;
		//cmpp.pDf = 'd';
		//cmpp.pOf = 'o';
		if(g_stepLength>0)
		{
			cmpp.cmpStr = (char*)cmpStringEx_5;
			cmpp.pDf = 'd';
			cmpp.pOf = 'o';
			charLen = 6;
			bias = 0;
			rbias = 1;
		}else
		{
			cmpp.cmpStr = (char*)cmpStringEx;
			cmpp.pDf = 'd';
			cmpp.pOf = 'o';
			charLen= 4;
			bias = 1;
			rbias = 0;
		}
		LCD_CANMST_CLEAR;
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,(cmp1||cmp2),g_SettingIndex);	
		//SetCmpFeb(cmp1,cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		DisplayTtlCmp_canStyle(LAYOUT_PAGEX_PARA,LAYOUT_PARAEX_COLUMN,
		bias,0,rbias,
		cmpp,
		//mstPara->cmp1,mstPara->cmp2,
		cmp1,cmp2,
		///4,DSP_PARAEX_1LINE,
		charLen,DSP_PARAEX_1LINE,
		charLen,
		SCROLL_UP,2,
		SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL
		//DSP_REVERSE
		);
		//DisplayPicture(2,2,68,8,font_sLinkShot,DSP_NORMAL);
		LCD_LINKSHOT_FLA();
		LCD_LSMST_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else if(CMD_MODE_RFLS_SLA==*g_cmdMode)
	{
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);
		//cmpp.cmpStr = (char*)cmpStringEx;
		//cmpp.pDf = 'd';
		//cmpp.pOf = 'o';
		if(g_stepLength>0)
		{
			cmpp.cmpStr = (char*)cmpStringEx_5;
			cmpp.pDf = 'd';
			cmpp.pOf = 'o';
			charLen = 6;
			bias = 0;
			rbias = 1;
		}else
		{
			cmpp.cmpStr = (char*)cmpStringEx;
			cmpp.pDf = 'd';
			cmpp.pOf = 'o';
			charLen= 4;
			bias = 1;
			rbias = 0;
		}
		LCD_CANMST_CLEAR;
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,(cmp1||cmp2),g_SettingIndex);	
		//SetCmpFeb(cmp1,cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		DisplayTtlCmp_canStyle(LAYOUT_PAGEX_PARA,LAYOUT_PARAEX_COLUMN,
		bias,0,rbias,
		cmpp,
		//mstPara->cmp1,mstPara->cmp2,
		cmp1,cmp2,
		///4,DSP_PARAEX_1LINE,
		charLen,DSP_PARAEX_1LINE,
		charLen,
		SCROLL_UP,2,
		SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL
		//DSP_REVERSE
		);
		//DisplayPicture(2,2,68,8,font_sLinkShot,DSP_NORMAL);
		LCD_LINKSHOT_FLA();
		LCD_LSSLA_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
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
	//CanSynMode(mainPara.workMode, mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
#endif

	
#ifdef _CANON_
	//if(WLM_ON==*wlSet.is_masterOn)
	if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		//DisplayCanMstHead(0,mainPara.workMode,wlSet.arr_mode);	
		LCD_CANMST_ON;
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
		//SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		LCD_CANMST_ON;
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
		//SetMstChan(radioSet->rfChan,DSP_NORMAL);
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
	}else if(CMD_MODE_RF_SLA==*g_cmdMode)
	{
		LCD_CANSLA_ON;
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		//DspSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		DspRfSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		if(g_isIndividualSlave>0)
		{
			LCD_CANSLA_INDIVIDUAL;
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
			DspIndividualSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		}else
		{
			LCD_CANSLA_ON;
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
			//DspSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
			DspLightSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		}
	}
	else if(CMD_MODE_OFF==*g_cmdMode)
	{
		LCD_CANMST_CLEAR;
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		SetManuPara(mstPara->output1,mstPara->output2,SETTING_ADDSUB==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else if(CMD_MODE_RFLS_MST==*g_cmdMode)
	{
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		SetManuPara(mstPara->output1,mstPara->output2,SETTING_ADDSUB==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		LCD_LINKSHOT_FLA();
		LCD_LSMST_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else if(CMD_MODE_RFLS_SLA==*g_cmdMode)
	{
		CanSynMode(/*mainPara.workMode, */&mainPara.synMode, SETTING_SYNMODE==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		SetManuPara(mstPara->output1,mstPara->output2,SETTING_ADDSUB==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		LCD_LINKSHOT_FLA();
		LCD_LSSLA_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
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
		LCD_CANMST_ON;
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		if(*multiSet.frequence>200)
		{
			*multiSet.frequence = 200;
		}
		SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		LCD_CANMST_ON;
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
	}else if(CMD_MODE_RF_SLA==*g_cmdMode)
	{
		LCD_CANSLA_ON;
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
		//DspSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		DspRfSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
		//SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		if(g_isIndividualSlave>0)
		{
			LCD_CANSLA_INDIVIDUAL;
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
			DspIndividualSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
			SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
		}else
		{
			LCD_CANSLA_ON;
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
			//DspSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
			DspLightSlavePara(SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);
			//SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,g_SettingIndex);
		}
	}
	else if(CMD_MODE_OFF==*g_cmdMode)
	{
		LCD_CANMST_CLEAR;
		SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq,
		SETTING_ADDSUB==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		
	}
	else if(CMD_MODE_RFLS_MST==*g_cmdMode)
	{
		LCD_CANMST_CLEAR;
		SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq,
		SETTING_ADDSUB==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		LCD_LINKSHOT_FLA();
		LCD_LSMST_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}
	else if(CMD_MODE_RFLS_SLA==*g_cmdMode)
	{
		LCD_CANMST_CLEAR;
		SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq,
		SETTING_ADDSUB==g_SettingIndex?DSP_SELE:DSP_NORMAL);
		LCD_LINKSHOT_FLA();
		LCD_LSSLA_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}
#endif
	//DisplayApert(flaPara.apert);//display apert value	
	//DisplayIso(flaPara.iso);//display iso value
	//DisplayFocLen(motoSet.focLen,0);
}

void SetExtaScr(void)
{
	u8 charLen=0;
	signed char cmp1=0,cmp2=0;
	signed char bias=0;
	signed char rbias=0;
	CmpPoint cmpp={0};
	LayModeExtA();

	if(CMD_MODE_RF_SLA==*g_cmdMode)
	{
		LCD_CANSLA_ON;
		DspRfSlavePara(DSP_NORMAL);
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);
	}else
	{

		if((mstPara->cmp1||mstPara->cmp2))
		{
			cmp1 = mstPara->cmp1;
			cmp2 = mstPara->cmp2;
		}else if(wlSet.cmrCmp1||wlSet.cmrCmp2)
		{
			//cmp1 = flaPara.cmrCmp1;
			//cmp2 = flaPara.cmrCmp2;
			cmp1 = wlSet.cmrCmp1;
			cmp2 = wlSet.cmrCmp2;
			//if(CMD_MODE_OFF==*g_cmdMode)
			//{
				//AreaSet(LAYOUT_PARA_AREA2, LAYOUT_PARAEX_COLUMN, DSP_CMPFEB_LEN, 16);
				//DisplayPicture(LAYOUT_PARA_AREA2, LAYOUT_PARAEX_COLUMN, FONT_CMP_WT, 16, font_cmp, DSP_NORMAL);
			//}else
			//{
				//AreaSet(LOCATION_TTLCMP_PAGE, LOCATION_TTLCMP_CML, DSP_CMPFEB_LEN, 16);
				//DisplayPicture(LOCATION_TTLCMP_PAGE, LOCATION_TTLCMP_CML, FONT_CMP_WT, 16, font_cmp, DSP_NORMAL);
			//}
		}
	
		//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,
		//										(mstPara->cmp1||mstPara->cmp2),g_SettingIndex);	
		SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,
											(cmp1||cmp2),g_SettingIndex);
		
		//cmpp.cmpStr = (char*)cmpStringEx;
		//cmpp.pDf = 'd';
		//cmpp.pOf = 'o';
		if(g_stepLength>0)
		{
			cmpp.cmpStr = (char*)cmpStringEx_5;
			cmpp.pDf = 'd';
			cmpp.pOf = 'o';
			charLen = 6;
			bias = 0;
			rbias = 1;
		}else
		{
			cmpp.cmpStr = (char*)cmpStringEx;
			cmpp.pDf = 'd';
			cmpp.pOf = 'o';
			charLen= 4;
			bias = 1;
			rbias = 0;
		}
		LCD_CANMST_CLEAR;
		DisplayTtlCmp_canStyle(LAYOUT_PAGEX_PARA,LAYOUT_PARAEX_COLUMN,
			bias,0,rbias,
		cmpp,
		//mstPara->cmp1,mstPara->cmp2,
		cmp1,cmp2,
		//4,DSP_PARAEX_1LINE,
		charLen,DSP_PARAEX_1LINE,
		charLen,
		SCROLL_UP,2,
		SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL
		//DSP_REVERSE
		);
	}

	if(CMD_MODE_RFLS_MST==*g_cmdMode)
	{
		LCD_LINKSHOT_FLA();	
		LCD_LSMST_ON(2, 75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else if(CMD_MODE_RFLS_SLA==*g_cmdMode)
	{
		LCD_LINKSHOT_FLA();
		LCD_LSSLA_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}
	
}

void SetExtmScr(void)
{
	LayModeExtM();
	LcdLoadPicture(LAYOUT_PARA_AREA2,LAYOUT_PARAEX_COLUMN+1, 20, 16, fontIsoIcon, DSP_REVERSE);
	//DspIsoValue(mainPara.extIso,g_SettingIndex==SETTING_EXTM_ISO?DSP_REVERSE:DSP_NORMAL);
	DspIsoValue(LAYOUT_PARA_AREA2, 60,mainPara.extIso,g_SettingIndex==SETTING_EXTM_ISO?DSP_REVERSE:DSP_NORMAL);
	DspApertValue(mainPara.extApert,g_SettingIndex==SETTING_EXTM_APERT?DSP_REVERSE:DSP_NORMAL);
	//DrawScale(mainPara.workMode,0,1);
	DrawScale(mainPara.workMode,flaPara.shutter,0,0,1);

	if(CMD_MODE_RFLS_MST==*g_cmdMode)
	{
		LCD_LINKSHOT_FLA();	
		LCD_LSMST_ON(2, 75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else if(CMD_MODE_RFLS_SLA==*g_cmdMode)
	{
		LCD_LINKSHOT_FLA();
		LCD_LSSLA_ON(2,75);
		SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}
}

#ifdef _CANON_


void GrDrawSelBar(unsigned sel)
{
	//LcdClearFeild(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,8,BIG_FONT_HIGHT(3)+LINE_SPACING(2));
	//RFGR_SELBAR_CLEAR;
	//LcdPicFramed(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,8,BIG_FONT_HIGHT(3)+LINE_SPACING(2),DSP_NORMAL);
	//FillArea(LAYOUT_RATIO_A+((BIG_FONT_HIGHT(3)+LINE_SPACING(2))/5*sel),LAYOUT_SELBAR_CML,8,(BIG_FONT_HIGHT(3)+LINE_SPACING(2)+14)/5,DSP_NORMAL);
	char i=0;
	char pa=0,ba=0;
	u32 tmp=0;
	memset(g_paraSetBuf,0x00,4*6);
	for(i=0;i<4;i++)
	{
		g_paraSetBuf[i] |= 0x10; 
		g_paraSetBuf[i+4*5] |= 0x01; 
	}

	g_paraSetBuf[0]|=0x1F;
	g_paraSetBuf[3]|=0x1F;
	
	for(i=1;i<6;i++)
	{
		g_paraSetBuf[i*4] |= 0xFF;
		g_paraSetBuf[i*4+3] |= 0xFF;
	}

	pa = ((45/5)*sel+3)/8;
	//pa = (nDot*sel)/8;
	ba = ((45/5)*sel+3)%8;
	//ba = (nDot*sel)%8;


	tmp = (0xffffffff>>ba)&(0xffffffff<<(32-ba-9));
	
	for(i=0;i<4;i++)
	{
		g_paraSetBuf[i+4*pa] |= (tmp>>24)&0xFF;
		g_paraSetBuf[i+4*(pa+1)] |= (tmp>>16)&0xFF;
		g_paraSetBuf[i+4*(pa+2)] |= (tmp>>8)&0xFF;
		g_paraSetBuf[i+4*(pa+3)] |= tmp&0xFF;
	}
	

	for(i=0;i<4;i++)
	{
		g_paraSetBuf[i] &= 0x1F;
	}
	//g_paraSetBuf[0] &= 0x7f;
	//g_paraSetBuf[4] &= 0x7f;

	LcdCopyToRam(LAYOUT_RATIO_A, 164, g_paraSetBuf, 4, 48);

	//tmp = (1<<nDot)-1;
}

void GrDspPara(u16 page, u8 gr, u8 mode, signed char pa1, signed char pa2,u8 rvs)
{
	//LcdClearFeild(page,0,294,MAIN_FONT_HIGHT(1));
	//u8 grh;
	//LCD_CLEAR_GR(page);
	//gr+='A';
	//grh = gr + 'A';
	//LCD_ShowChar(page, LAYOUT_RATIO_COLUMN+3, 'A'+gr, FONT_TYPE_GRHEAD, rvs);
	//DspMstRatioHead(page,LAYOUT_RATIO_COLUMN,&gr,rvs,LAYOUT_GRAE_WIDTH);
	//LcdPicFramed(page, LAYOUT_RATIO_COLUMN,3+BIG_FONT_WIDTH(1)+3,BIG_FONT_HIGHT(1),rvs);
	//LcdClearFeild(page,LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,24,LAYOUT_RATIO_WIDTH-24);
	//LcdClearFeild(page,LAYOUT_RATIO_COLUMN+3+MAIN_FONT_WIDTH(1)+3+6,LAYOUT_RATIO_WIDTH-24,24);
	switch(mode)
	{
		case WORKINGMODE_TTL:
			//LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1),"TTL", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("TTL")-1)*BIG_FONT_WIDTH(1));
			//DspGrMode(page,LAYOUT_GRMOD_COLUMN,"eTTL",rvs,LAYOUT_GRMOD_WIDTH);
			//FLAOK_TOGGLE();
#ifdef DSP_TEST
			printf("%d,%d\r\n",pa1,pa2);
#endif
			DisplayTtlCmp(page, LAYOUT_RATIO_STPARA,pa1,pa2,rvs);
		break;

		case WORKINGMODE_MANU:
			//LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1), "M", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("M")-1)*BIG_FONT_WIDTH(1));
			//DspGrMode(page,LAYOUT_GRMOD_COLUMN,"M",rvs,LAYOUT_GRMOD_WIDTH);
			DisplayMstOtp(page,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,pa1,pa2,rvs);
		break;

		case WORKINGMODE_EXTA:
			//LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1), "Ext.A", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("Ext.A")-1)*BIG_FONT_WIDTH(1)-BIG_FONT_WIDTH(1)/2);
			//DspGrMode(page,LAYOUT_GRMOD_COLUMN,"Ext.A",rvs,LAYOUT_GRMOD_WIDTH);
			DisplayTtlCmp(page, LAYOUT_RATIO_STPARA,pa1,pa2,rvs);
		break;

		default:
			//DspGrMode(page,LAYOUT_GRMOD_COLUMN,"OFF",rvs,LAYOUT_GRMOD_WIDTH);
			//LcdDspAtCenter(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1), "OFF", FONT_TYPE_GRHEAD,rvs,LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1)-LAYOUT_MAST_SPACING(1),(sizeof("OFF")-1)*BIG_FONT_WIDTH(1)-BIG_FONT_WIDTH(1)/2);
			LcdClearFeild(page, LAYOUT_RATIO_STPARA, DSP_PARA_1LINE, 16);
		break;
	}
	//LcdPicFramed(page, LAYOUT_RATIO_COLUMN+3+BIG_FONT_WIDTH(1)+3+LAYOUT_MAST_SPACING(1),LAYOUT_RATIO_WIDTH-BIG_FONT_WIDTH(1),BIG_FONT_HIGHT(1),rvs);

}

void GrDspHead(u16 page, u8 gr, u8 mode, /*signed char pa1, signed char pa2,*/u8 rvs)
{
	LCD_CLEAR_GR(page);
	gr+='A';
	//grh = gr + 'A';
	//LCD_ShowChar(page, LAYOUT_RATIO_COLUMN+3, 'A'+gr, FONT_TYPE_GRHEAD, rvs);
	DspMstRatioHead(page,LAYOUT_RATIO_COLUMN,(const char*)&gr,rvs,LAYOUT_GRAE_WIDTH);

	switch(mode)
	{
		case WORKINGMODE_TTL:
			DspGrMode(page,LAYOUT_GRMOD_COLUMN,"eTTL",rvs,LAYOUT_GRMOD_WIDTH);
		break;

		case WORKINGMODE_MANU:
			DspGrMode(page,LAYOUT_GRMOD_COLUMN,"M",rvs,LAYOUT_GRMOD_WIDTH);
		break;

		case WORKINGMODE_EXTA:
			//DspGrMode(page,LAYOUT_GRMOD_COLUMN,"Ext.A",rvs,LAYOUT_GRMOD_WIDTH);
			DspGrMode(page,LAYOUT_GRMOD_COLUMN,"ext.A",rvs,LAYOUT_GRMOD_WIDTH);
		break;

		default:
			DspGrMode(page,LAYOUT_GRMOD_COLUMN,"OFF",rvs,LAYOUT_GRMOD_WIDTH);
		break;
	}
}


//void RfGrDspPara(u8 sel)
void RfGrDspPara(void)
{
	unsigned char gr;
	u8 rsv1=0,rsv2=0;
	//unsigned char sf=0;
	
	
	for(gr=0;gr<3;gr++)
	{
	//	if(g_grSel)
	//	{
	//		rsv1 = ((gr+g_grDspStart)==g_grSel)? DSP_SELE:DSP_NORMAL;
	//		rsv2 = ((DSP_SELE==rsv1)&&(SETTING_GR_RES==g_SettingIndex))?DSP_SELE:DSP_NORMAL;
	//	}else
	//	{
	//		rsv1 = DSP_NORMAL;
	//		rsv2 = DSP_NORMAL;
	//	}
#ifdef DSP_TEST
		//printf("%d, %d, %d\r\n",gr,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2);
#endif
		if((SETTING_RFGR_SEL==g_SettingIndex)||(SETTING_GR_RES==g_SettingIndex))
		{
			rsv1 = ((gr+g_grDspStart)==g_grSel)? DSP_SELE:DSP_NORMAL;
			rsv2 = ((DSP_SELE==rsv1)&&(SETTING_GR_RES==g_SettingIndex))?DSP_SELE:DSP_NORMAL;
		}else
		{
			rsv1 = DSP_NORMAL;
			rsv2 = DSP_NORMAL;
		}
		if(radioSet->rfGr[gr+g_grDspStart].isOff>0)
		{
			GrDspHead(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_OFF,/*0,0,*/rsv1);
			GrDspPara(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_OFF,0,0,rsv2);
		}else
		{
			switch(radioSet->rfGr[gr+g_grDspStart].mode)
			{
				case GRMOD_TTL:
					GrDspHead(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_TTL,/*radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,*/rsv1);
					GrDspPara(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_TTL,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,rsv2);
				break;

				case GRMOD_M:
					GrDspHead(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_MANU,/*radioSet->rfGr[gr+g_grDspStart].m_exp1,radioSet->rfGr[gr+g_grDspStart].m_exp2,*/rsv1);
					GrDspPara(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_MANU,radioSet->rfGr[gr+g_grDspStart].m_exp1,radioSet->rfGr[gr+g_grDspStart].m_exp2,rsv2);
				break;

				case GRMOD_EXTA:
					GrDspHead(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_EXTA,/*radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,*/rsv1);
					GrDspPara(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_EXTA,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,rsv2);
				break;

				default:
					GrDspHead(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_OFF,/*0,0,*/rsv1);
					GrDspPara(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,WORKINGMODE_OFF,0,0,rsv2);
					//GrDspPara(LAYOUT_RATIO_A+gr*2,gr+g_grDspStart,radioSet->rfGr[gr+g_grDspStart].mode,0,0,rsv);
				break;
			}
		}
	}
}

u8 IsGrModExta(void)
{
	u8 i = 0;
	for (i=0;i<5;i++)
	{
		if (GRMOD_EXTA==radioSet->rfGr[i].mode)
		{
			return 1;
		}
	}
	return 0;
}

void CheckGrSynMod(void)
{
//	u8 i = 0;
	/*when one group set as exta, set the syn auto*/
	if (SYNC_MODE_FP == mainPara.synMode)
	{
		if (IsGrModExta())
		{
			mainPara.synMode = SYNC_MODE_FRON;
		}
	}
}

void SetRfGrScr(void)
{
	//unsigned char gr;
	//printf("SetRfGrScr\r\n");
	//u8 i = 0;
	
	LCD_CMP_CLEAR;
	
	LayModeGr();

	LCD_CANMST_ON;

	CheckGrSynMod();
#if 0
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
#endif
	
	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);

//printf("SetMstChan\r\n");

	CanSynMode(/*mainPara.workMode, */&mainPara.synMode, g_SettingIndex==SETTING_SYNMODE?DSP_SELE:DSP_NORMAL);

	//printf("CanSynMode\r\n");

	SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,(wlSet.cmrCmp1||wlSet.cmrCmp2),g_SettingIndex);

	//printf("SetCmpFeb\r\n");
	
	//RfGrDspPara(g_grSel);
	RfGrDspPara();

	//printf("RfGrDspPara\r\n");
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
	//g_menuId = 0;
	//if(!KEY_LOCK_EX)
	//{
	//	return;




	//}
	BuildFuncMenu();
	KeyMenuSw();
	//DspMstModIcon(*g_cmdMode);
}

//void DspMenuLocked(void)
void DspMenuIcon(u8 wt,const char* pic)
{
	u8 i=0;
	//memset(g_paraSetBuf,0x1F,LAYOUT_BUTTON_WIDTH);
	//memset(g_paraSetBuf+LAYOUT_BUTTON_WIDTH,0xFF,LAYOUT_BUTTON_WIDTH);
	memset(g_paraSetBuf,0xFF,LCD_W*2);
	//memcpy(g_paraSetBuf+((LCD_W-42)/2),font_locked,42);
	//memcpy(g_paraSetBuf+((LCD_W-42)/2)+LCD_W,font_locked+42,42);
	memcpy(g_paraSetBuf+((LCD_W-wt)/2),pic,wt);
	memcpy(g_paraSetBuf+((LCD_W-wt)/2)+LCD_W,pic+wt,wt);

	for(i=0;i<LCD_W;i++)
	{
		g_paraSetBuf[i] &= 0x1F;
	}

	LcdCopyToRam(LAYOUT_MENU_PAGE, 0, g_paraSetBuf,LCD_W, 16);
	
	
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

		case WORKINGMODE_EXTA:
			SetExtaScr();
		break;

		case WORKINGMODE_EXTM:
			SetExtmScr();
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
//		case WORKINGMODE_RTC:
//			SetRmtC();
//		break;
#endif

#if 0
#ifdef _SPEED_LIGHT_
		case WORKINGMODE_F1:
			SetRmtF1();
		break;

		case WORKINGMODE_F2:
			SetRmtF2();
		break;
#endif
#endif
	}
}

void FuncModuleSw(void)
{
	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		//TmCaptureInit(1000,72);	
#ifdef SLAVE_TEST
		DBG_PRINTF("Tm cp set\r\n");
#endif
		TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
		//ClearWlcBuf();
		LightSlaveParaClear();
	}else
	{
		StopCapture();
	}
	//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
#ifndef SLAVE_TEST
	BkLiteOn();
#endif
}

void UartIntSet(void)
{
	//while(uart_tran_flag);
	WaitForUartIdle(50);
	if(CMD_MODE_RF_SLA==*g_cmdMode)
	{
		GPIOB->CRL &= 0xFF0FFFFF;
		GPIOB->CRL |= 0x00800000;
		GPIOB->ODR |= 0x20;
		//Ex_NVIC_Config(GPIO_B,5,RTIR); 	
		//EXTIx_Init(GPIO_B,FTIR);
	}else
	{
		GPIOB->CRL &= 0xFF0FFFFF;
		GPIOB->CRL |= 0x00300000;
		GPIOB->ODR |= 0x20;
	}

}

void ModeClear(void)
{
	mainPara.workMode = WORKINGMODE_TTL;
	*g_cmdMode = CMD_MODE_OFF;
	*motoSet.mode = MOTO_MODE_AUTO;
	motoSet.destRsv = 35;
	radioSet->id = 0x00;
	DISABLE_RF_MODULE();
	Mode();
	DspMenuIcon(47,font_clear);
	HaltSec(999);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void Mode(void)
{
	//LCD_Clear(BACK_COLOR);
	//DspClearAllPara();

	switch(*g_cmdMode)
	{

		case CMD_MODE_RF_MST:
			DspClearAllPara();
			ModPasteScr(mainPara.workMode);
			RfDevLinkMark(mainPara.workMode,&wlSet);
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
		break;
		
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
		//case CMD_MODE_RF_MST:
		case CMD_MODE_RF_SLA:
		case CMD_MODE_OFF:
			DspClearAllPara();
			ModPasteScr(mainPara.workMode);
			//RfDevLinkMark(mainPara.workMode,&wlSet);
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
			//FuncDsp();	
		break;

		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				DspClearAllPara();
				ModPasteScr(mainPara.workMode);
				//RfDevLinkMark(mainPara.workMode,&wlSet);
			}else
			{
				ModLinkedShot();
			}
		break;
	}

	
	//UartIntSet();
	//g_menuId = 0;
	//BuildFuncMenu();
	if((KEY_LOCK_EX>0)&&(!g_menuLock))
	{
		LoadButtons();
	}else
	{
		LcdClearFeild(LAYOUT_MENU_PAGE,0,LCD_W,LAYOUT_BUTTON_HIGHT);
	}
	//KeyMenuSw();
	DspMstModIcon(*g_cmdMode);
	TakeOnCharge(*g_cmdMode);
	//FuncModuleSw();
	//RfDevLinkMark(mainPara.workMode,&wlSet);
	//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
	//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
	FuncDsp();	

	// 20171208
	//DspIsoValue(LAYOUT_PARA_AREA2, 60,g_expAccHotWarnValue, DSP_NORMAL);

	TurnOnChargeLed(1);
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
#if 0
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
						radioSet->rfGr[g_grSel-1].mode = WORKINGMODE_EXTA;
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
				RfGrDspPara();	
			}
#endif
		break;
	}
}


#if 1
void SwithMode(void)					//mode long press
{
#if 0
#ifdef _ST_EX_
	return;
#else
	if(0==g_modeSwitch)
	{
		if(flaPara.statue_link!=0)	return;		
		g_modeSwitch = 1;
#ifdef _CANON_
		mainPara.workMode = WORKINGMODE_RTC;
		flaPara.ledStatue = 0;
#else
		mainPara.workMode = WORKINGMODE_RTN;
#endif
		Mode(mainPara.workMode);
		LoadButtons();

		LCD_AF_CLEAR;
		FuncDsp();
		
	}else if(1==g_modeSwitch)
	{
		g_modeSwitch = 0;
		LCD_RMTPARA_CLEAR;
		mainPara.workMode = WORKINGMODE_TTL;
		Mode(mainPara.workMode);
		LoadButtons();
		
		//FuncDsp();
		
	}
#endif
#endif
	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		if(!g_isIndividualSlave)
		{
			g_isIndividualSlave = 1;
			mainPara.workMode = WORKINGMODE_MANU;
			Mode();
		}
	}
}
#endif

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
//	unsigned char frqTmp;
	unsigned char arrNum;
	unsigned char tMax;

/*
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
*/
	if(frq<6)
	{
		arrNum = frq - 1;
	}else if(frq<8)
	{
		arrNum = 5;
	}else if(frq<10)
	{
		arrNum = 6;
	}else if(frq<11)
	{
		arrNum = 7;
	}else if(frq<12)
	{
		arrNum = 8;
	}else if(frq<15)
	{
		arrNum = 9;
	}else if(frq<20)
	{
		arrNum = 10;
	}else if(frq<60)
	{
		arrNum = 11;
	}else if(frq<250)
	{
		arrNum = 12;
	}else
	{
		arrNum = 13;
	}

#ifdef _CANON_
	tMax =  canMaxCalc[exp][arrNum];
#else
	tMax =  canMaxCalc[exp>>4][arrNum];
#endif
	
	
	*maxTm = tMax;

	if(*eTm>tMax)
	{
		//tMax = eTm;
		*eTm = tMax;
	}
	
}

void RfDevLinkMark(char mod,WlSet *wlPara)
{
	RFGR_SELBAR_CLEAR;
		
	if(WORKINGMODE_TTL==mod)
	{
		//switch(*wlPara->arr_mode)
		switch(*wlPara->ttl_Ratio)
		{
			case WLM_MODE_ALL:
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;

			case WLM_MODE_AB:
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}	
			break;

			case WLM_MODE_RES:
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[2]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;
		}
	}else if(WORKINGMODE_GR==mod)
	{

			if(radioStatue.devCnt[g_grDspStart]>0)
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[g_grDspStart+1]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[g_grDspStart+2]>0)
			{
				DisplayPicture(LAYOUT_RATIO_C,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}		
			
	}else
	{
		switch(*wlPara->arr_mode)
		{
			case WLM_MODE_ALL:
			//if(radioStatue.devCnt[0]>0)
			if((radioStatue.devCnt[0]>0)||(radioStatue.devCnt[1]>0)||(radioStatue.devCnt[2]>0))
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}	
			break;

			case WLM_MODE_AB:
			if(radioStatue.devCnt[0]>0)
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[1]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;

			case WLM_MODE_RES:
			if(radioStatue.devCnt[0]>0)
			{
				DisplayPicture(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[1]>0)
			{
				DisplayPicture(LAYOUT_RATIO_B,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			if(radioStatue.devCnt[2]>0)
			{
				DisplayPicture(LAYOUT_RATIO_C,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,LAYOUT_LKDEV_HT,LkDevMark,DSP_NORMAL);
			}
			break;
		}
	}
}

void MotoShift(u8 dir)
{
		if(motoSet.moving != 0)return;
#ifdef MOTO_REFLECT_DETECT
		//if(REF_BOARD_DETECT) return;
		if(g_isReflectPulled>0) return;
#endif
		if(KEY_DIR_LEFT==dir)
		{
			motoSet.focIndex--;
		}else
		{
			if(MOTO_MODE_AUTO==*motoSet.mode)
			{
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.focIndex = 0;
			}else
			{
				motoSet.focIndex++;
			}
		}
		
		//if(motoSet.focIndex<=0)
		if(motoSet.focIndex<0)
		{
			motoSet.focIndex = 0;
			//*motoSet.dest_location = focLenScape[0];				//delete 2015/06/14
			//*motoSet.dest_location = 35;
			//motoSet.destRsv = *motoSet.dest_location;				//delete 2015/06/14
			//MotoReset();
			LCD_MMOTO_OFF;
			*motoSet.mode = MOTO_MODE_AUTO;
			menuPara.motoCod = 0x01;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_AUTO;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_AUTO &  motoSet.focIndex);
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//g_paraChange = 1;
			//SetFocLen(0,0);
			*motoSet.dest_location = motoSet.autoDest;
			motoSet.destRsv = *motoSet.dest_location;
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE); 
		}//else if(motoSet.focIndex>11)
		//else if(motoSet.focIndex>10)
		else if(motoSet.focIndex>9)
		{
			//motoSet.focIndex=11;
			//motoSet.focIndex = 10;
			motoSet.focIndex = 9;
			motoSet.focLen = 0x45;
			menuPara.motoCod = 0x17;
			//MotoReset();
		}
		else
		{
			LCD_MMOTO_ON;
			*motoSet.mode = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = motoSet.focIndex;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_MANU | motoSet.focIndex);
			//ParaPreserve(EEPROM_INDEX_MOTOMODE,(MASK_MOTOMODE_MANU|motoSet.focIndex));
			//*motoSet.dest_location = focLenScape[motoSet.focIndex];
			//*motoSet.dest_location = focLenScape[motoSet.focIndex-1];
			*motoSet.dest_location = focLenScape[motoSet.focIndex];
			motoSet.destRsv = *motoSet.dest_location;
			motoSet.focLen = focLenCode[motoSet.focIndex];
			//menuPara.motoCod = (0x11+motoSet.focIndex)>0x17?0x17:(0x11+motoSet.focIndex);
			menuPara.motoCod = motoSet.focIndex>0?((0x10+motoSet.focIndex)>0x17?0x17:(0x11+motoSet.focIndex)):0x11;
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//ParaPreserve(EEPROM_INDEX_LOCATION,*motoSet.dest_location);
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE); 
		}

		//if(g_cmrLinked>0)
		if((g_cmrPressed>0)||(WORKINGMODE_EXTM==mainPara.workMode))
		{
			//SET_UPDATE_EVT(DSP_UPDATE_LINK);
			SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
		}
}


void ExposeAddSub(u8 dir)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_GR:
		case WORKINGMODE_EXTA:
			if(KEY_DIR_LEFT==dir)
			{
				if((-3==mstPara->cmp1)&&(0==mstPara->cmp2))
				{
					return;
				}
				mstPara->cmp2--;
				if(mstPara->cmp2<0)
				{
					if(g_stepLength>0)
					{
						mstPara->cmp2+=2;
					}else
					{
						mstPara->cmp2+=3;
					}
					mstPara->cmp1--;
				}
			}else
			{
				if(3==mstPara->cmp1)
				{
					return;
				}
				mstPara->cmp2++;
				if(g_stepLength>0)
				{
					if(mstPara->cmp2>1)
					{
						mstPara->cmp1++;
						mstPara->cmp2 = 0;
					}

				}else
				{
					if(mstPara->cmp2>2)
					{
						mstPara->cmp1++;
						mstPara->cmp2 = 0;
					}
				}
			}		
			//wlSet.otpCode[MASTER] = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			//menuPara.otpCod[MASTER] = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			if(g_stepLength>0)
			{
				menuPara.mstTtl = compensation_table_5[(mstPara->cmp1+6)*2+mstPara->cmp2];
			}else
			{
				menuPara.mstTtl = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			}
			/*
			if((WORKINGMODE_TTL==mainPara.workMode)||(WORKINGMODE_EXTA==mainPara.workMode))
			{
				SetTtlScr();
			}else
			{
				SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0,g_SettingIndex);	
			}*/
			SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
			//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);
			//ParaPreserve(EEPROM_INDEX_TTLCMP,((mstPara->cmp1+3)<<4)|mstPara->cmp2);				
		break;

		case WORKINGMODE_MANU:
	//	if(CMD_MODE_OFF!=*g_cmdMode) return;
		if(KEY_DIR_LEFT==dir)
		{
			if((0==mstPara->output1)&&(0==mstPara->output2))
			{
				//mstPara->output1 = 7;
				//mstPara->output2 = 0;
				return;
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
				//mstPara->output1 = 0;
				//mstPara->output2 = 0;
				return;
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
		//SetManuPara(mstPara->output1,mstPara->output2,DSP_SELE);
		SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
		if(g_cmrPressed>0)
		{
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
		//ParaPreserve(EEPROM_INDEX_EXP,(mstPara->output1<<4)|mstPara->output2);		
		//while(uart_tran_flag);
		//DecodeUartData(UART_ENCODE_OUTPUT,mstPara->output1,mstPara->output2);				
		break;

		case WORKINGMODE_MULTI:
	//	if(CMD_MODE_OFF!=*g_cmdMode) return;
		if(KEY_DIR_LEFT==dir)
		{
#ifdef _CANON_

			if(0==*multiSet.expose)	
			{
				//*multiSet.expose=0x50;
				//*multiSet.expose= 5;
				//MultiCalMaxTime();	
				//MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
				return;
			}else
			{
				//(*multiSet.expose)-=0x10;
				(*multiSet.expose)--;
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
			if(5==*multiSet.expose)	
			{
				//*multiSet.expose=0;
				return;
			}else
			{
				//(*multiSet.expose)+=0x10;
				(*multiSet.expose)++;
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
		//SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq,DSP_SELE);
		SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
		if(g_cmrPressed>0)
		{
			SET_UPDATE_EVT(DSP_UPDATE_LINK);
		}
		break;
	}
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
	CanSynMode(/*mainPara.workMode,*/&mainPara.synMode,DSP_SELE);
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
			if(g_stepLength>0)
			{
				flaPara.ttl_FEB2+=2;
			}else
			{
				flaPara.ttl_FEB2+=3;					
			}
		}
	}else
	{
		if(flaPara.ttl_FEB1>=3)
		{
			return;
		}
		flaPara.ttl_FEB2++;
		if(g_stepLength>0)
		{
			if(flaPara.ttl_FEB2>=2)
			{
				flaPara.ttl_FEB1++;
				flaPara.ttl_FEB2-=2;
			}
		}else
		{
			if(flaPara.ttl_FEB2>=3)
			{
				flaPara.ttl_FEB1++;
				flaPara.ttl_FEB2-=3;
				
			}
		}
	}		
	//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
	/*
	if(WORKINGMODE_TTL==mainPara.workMode)
	{
		SetTtlScr();
	}else
	{
		CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
	}*/
	if(g_stepLength>0)
	{
		menuPara.feb = feb_table[flaPara.ttl_FEB1*3+flaPara.ttl_FEB2];
	}else
	{
		menuPara.feb = feb_table_5[flaPara.ttl_FEB1*2+flaPara.ttl_FEB2];
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
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
	DisplayCanMstHead(1,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
}

void Arr_SubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		g_ArrSel--;
		if(g_ArrSel<0)
		{
			g_ArrSel = (WORKINGMODE_TTL==mainPara.workMode)?(*wlSet.ttl_Ratio-1):*wlSet.arr_mode;
		}
	}else
	{
		g_ArrSel++;
		if(g_ArrSel>((WORKINGMODE_TTL==mainPara.workMode)?(*wlSet.ttl_Ratio-1):*wlSet.arr_mode))
		{
			g_ArrSel = 0;
		}
	}
	DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
	RfArrSelMenu();
	
}

void RfArrParaSubAdd(u8 dir)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			if(KEY_DIR_LEFT==dir)
			{
				if(g_ArrSel>0)
				{
					if((-3==wlSet.m_arrPara[g_ArrSel+1].cmp1)&&(0==wlSet.m_arrPara[g_ArrSel+1].cmp2))
					{
						return;
					}
					wlSet.m_arrPara[g_ArrSel+1].cmp2--;
					if(wlSet.m_arrPara[g_ArrSel+1].cmp2<0)
					{
						if(g_stepLength>0)
						{
							wlSet.m_arrPara[g_ArrSel+1].cmp2+=2;
						}else
						{
							wlSet.m_arrPara[g_ArrSel+1].cmp2+=3;
						}
						wlSet.m_arrPara[g_ArrSel+1].cmp1--;					
					}
					if(g_stepLength>0)
					{
						wlSet.cCmp = compensation_table_5[(wlSet.m_arrPara[g_ArrSel+1].cmp1+6)*2+wlSet.m_arrPara[g_ArrSel+1].cmp2];
					}else
					{
						wlSet.cCmp = compensation_table[(wlSet.m_arrPara[g_ArrSel+1].cmp1+6)*3+wlSet.m_arrPara[g_ArrSel+1].cmp2];
					}
				}else
				{
					wlSet.m_arrPara[g_ArrSel+1].cmp1--;
					if(wlSet.m_arrPara[g_ArrSel+1].cmp1<0)
					{
						wlSet.m_arrPara[g_ArrSel+1].cmp1 = 0;
					}
					abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
					abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
				}
			}else
			{
				if(g_ArrSel>0)
				{
					if(3==wlSet.m_arrPara[g_ArrSel+1].cmp1)
					{
						return;
					}
					
					wlSet.m_arrPara[g_ArrSel+1].cmp2++;

					if(g_stepLength>0)
					{
						if(wlSet.m_arrPara[g_ArrSel+1].cmp2>1)
						{
							wlSet.m_arrPara[g_ArrSel+1].cmp1++;
							wlSet.m_arrPara[g_ArrSel+1].cmp2 = 0;
						}
					}else
					{
						if(wlSet.m_arrPara[g_ArrSel+1].cmp2>2)
						{
							wlSet.m_arrPara[g_ArrSel+1].cmp1++;
							wlSet.m_arrPara[g_ArrSel+1].cmp2 = 0;
						}
					}

					if(g_stepLength>0)
					{
						wlSet.cCmp = compensation_table_5[(wlSet.m_arrPara[g_ArrSel+1].cmp1+6)*2+wlSet.m_arrPara[g_ArrSel+1].cmp2];
					}else
					{
						wlSet.cCmp = compensation_table[(wlSet.m_arrPara[g_ArrSel+1].cmp1+6)*3+wlSet.m_arrPara[g_ArrSel+1].cmp2];
					}
					
				}else
				{
				//	if(1==wlSet.m_arrPara[g_ArrSel+1].cmp1)
				/*
					if(((wlSet.m_arrPara[g_ArrSel+1].cmp1+3)*3+wlSet.m_arrPara[g_ArrSel+1].cmp2)>=12)
					{
						wlSet.m_arrPara[g_ArrSel+1].cmp1 = 0;
						wlSet.m_arrPara[g_ArrSel+1].cmp2 = 1;
						return;
					}
				*/
				//	if((0==wlSet.m_arrPara[g_ArrSel+1].cmp1)&&(1==wlSet.m_arrPara[g_ArrSel+1].cmp2))
				//	{
				//		return;
				//	}

					wlSet.m_arrPara[WL_ARRARY_B].cmp1++;
					if(wlSet.m_arrPara[WL_ARRARY_B].cmp1>12)
					{
						wlSet.m_arrPara[WL_ARRARY_B].cmp1 = 12;
					}

					abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
					abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
				}
				/*
				wlSet.m_arrPara[g_ArrSel+1].cmp2++;
				if(wlSet.m_arrPara[g_ArrSel+1].cmp2>2)
				{
					wlSet.m_arrPara[g_ArrSel+1].cmp1++;
					wlSet.m_arrPara[g_ArrSel+1].cmp2 = 0;
				}
				*/

			}
			/*
			if(g_ArrSel>0)
			{
				wlSet.cCmp = compensation_table[(wlSet.m_arrPara[g_ArrSel+1].cmp1+6)*3+wlSet.m_arrPara[g_ArrSel+1].cmp2];
				
			}else
			{
				*wlSet.abRatio = (wlSet.m_arrPara[g_ArrSel+1].cmp1+3)*3+wlSet.m_arrPara[g_ArrSel+1].cmp2;
				abRation_fill1 = wlm_abRation_fillwords[*wlSet.abRatio].para1;
				abRation_fill2 = wlm_abRation_fillwords[*wlSet.abRatio].para2;
			}*/
			//menuPara.mstTtl = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
		break;

		case WORKINGMODE_MANU:

			if(KEY_DIR_LEFT==dir)
			{
			/*
				if((0==wlSet.m_arrPara[g_ArrSel].output1)&&(0==wlSet.m_arrPara[g_ArrSel].output2))
				{
					wlSet.m_arrPara[g_ArrSel].output1 = 7;
					wlSet.m_arrPara[g_ArrSel].output2 = 0;
				}else
				{
					wlSet.m_arrPara[g_ArrSel].output2--;
					if(wlSet.m_arrPara[g_ArrSel].output2<0)
					{
						wlSet.m_arrPara[g_ArrSel].output2+=3;
						wlSet.m_arrPara[g_ArrSel].output1--;
					}
				}
				*/
				if((0==wlSet.m_arrPara[g_ArrSel].output1)&&(0==wlSet.m_arrPara[g_ArrSel].output2))
				{
					return;
				}else
				{
					wlSet.m_arrPara[g_ArrSel].output2--;
					if(wlSet.m_arrPara[g_ArrSel].output2<0)
					{
						wlSet.m_arrPara[g_ArrSel].output2+=3;
						wlSet.m_arrPara[g_ArrSel].output1--;
					}
				}
			}else
			{
				/*
				if(7==wlSet.m_arrPara[g_ArrSel].output1)
				{
					wlSet.m_arrPara[g_ArrSel].output1 = 0;
					wlSet.m_arrPara[g_ArrSel].output2 = 0;
				}else
				{
					wlSet.m_arrPara[g_ArrSel].output2++;
					if(wlSet.m_arrPara[g_ArrSel].output2>2)
					{
						wlSet.m_arrPara[g_ArrSel].output1++;
						wlSet.m_arrPara[g_ArrSel].output2=0;
					}
				}*/
				if(7==wlSet.m_arrPara[g_ArrSel].output1)
				{
					return;
				}else
				{
					wlSet.m_arrPara[g_ArrSel].output2++;
					if(wlSet.m_arrPara[g_ArrSel].output2>2)
					{
						wlSet.m_arrPara[g_ArrSel].output1++;
						wlSet.m_arrPara[g_ArrSel].output2=0;
					}
				}
			}
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
			menuPara.otpCod[g_ArrSel] = output_table[(7-wlSet.m_arrPara[g_ArrSel].output1)*3-wlSet.m_arrPara[g_ArrSel].output2];
		break;

		case WORKINGMODE_MULTI:
			if(KEY_DIR_LEFT==dir)
			{
				/*
				if(0x00==(wlSet.m_arrPara[g_ArrSel].multi_exp&0xf0))
				{
					wlSet.m_arrPara[g_ArrSel].multi_exp = 0x50;
				}else
				{
					wlSet.m_arrPara[g_ArrSel].multi_exp -= 0x10;
				}*/
				if(wlSet.m_arrPara[g_ArrSel].multi_exp>0)
				{
					wlSet.m_arrPara[g_ArrSel].multi_exp--;
				}
			}else
			{
				/*
				if(0x50==(wlSet.m_arrPara[g_ArrSel].multi_exp&0xf0))
				{
					wlSet.m_arrPara[g_ArrSel].multi_exp=0x00;
				}else 
				{
					wlSet.m_arrPara[g_ArrSel].multi_exp+=0x10;
				}*/
				if(wlSet.m_arrPara[g_ArrSel].multi_exp<5)
				{
					wlSet.m_arrPara[g_ArrSel].multi_exp++;
				}
				if(!g_ArrSel)
				{
					//MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
					MultiMaxTimes(*multiSet.frequence,wlSet.m_arrPara[g_ArrSel].multi_exp,multiSet.eTimes, &multiSet.maxTimes);
					SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence,DSP_NORMAL);
				}
			}
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
			//menuPara.WlmMul[g_ArrSel] = output_table[(7-(wlSet.m_arrPara[g_ArrSel].multi_exp>>4))*3];
			menuPara.WlmMul[g_ArrSel] = output_table[(7-(wlSet.m_arrPara[g_ArrSel].multi_exp))*3];

			//mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].mulOtp;
			//SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq);	
		break;

		case WORKINGMODE_GR:
		break;
	
	}
	
}

void ChSanSubAdd(u8 dir)
{
//	u8 i=0;
	if(KEY_DIR_LEFT==dir)
	{
		g_chSan--;
		if(g_chSan<1)
		{
			g_chSan = 1;
		}
	}else
	{
		g_chSan++;
		if(g_chSan>15)
		{
			g_chSan = 15;
		}
	}
	SetMstChan(1, 128,g_chSan,DSP_NORMAL,1);
	memset(g_paraSetBuf,0x00,135);
	memset(g_paraSetBuf+9*(g_chSan-1),0x78,5);
	LcdCopyToRam(10,22,g_paraSetBuf,135,8);
/*
	AreaSet(10, 22+9*(g_chSan-1), 5, 8);
	for(i=0;i<5;i++)
	{
		LCD_WRITE_BYTE(0X78);
	}

	//g_chSan = radioSet->rfChan;
	SetMstChan(1, 128,g_chSan,DSP_NORMAL,1);
*/
	
}

#if 0

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
			menuPara.mstTtl = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
			menuPara.WlmMul[WL_ARRARY_A] = output_table[(7-(*multiSet.expose>>4))*3];

			//mainPara.modGr[MASTER].multi_exp = wlSet.m_arrPara[WLM_ARR_A].mulOtp;
			//SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq);	
		break;

		case WORKINGMODE_GR:
		break;
	}
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
			menuPara.WlmMul[WL_ARRARY_B] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_B].multi_exp>>4))*3];
		break;
	}
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
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
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
			menuPara.WlmMul[WL_ARRARY_C] = output_table[(7-(wlSet.m_arrPara[WL_ARRARY_C].multi_exp>>4))*3];
		break;
	}
}

#endif

void LightChanSet(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		(*wlSet.m_chan)--;
		if(*wlSet.m_chan<WL_CHANNEL_ONE)
		{
			//*wlSet.m_chan = WL_CHANNEL_FOUR;
			*wlSet.m_chan = WL_CHANNEL_ONE;
			return;
		}
	}else
	{
		(*wlSet.m_chan)++;
		if(*wlSet.m_chan>WL_CHANNEL_FOUR)
		{
			//*wlSet.m_chan = WL_CHANNEL_ONE;
			*wlSet.m_chan = WL_CHANNEL_FOUR;
			return;
		}
	}
	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,DSP_SELE,0);
	//DisplayCanMstHead(5,mainPara.workMode,&wlSet);
}

void RfChanSet(u8 dir,u8 isSmall)
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
			//g_rfChanSet= 15;
			g_rfChanSet = 0;
		}
	}else
	{
		(g_rfChanSet)++;
		if(g_rfChanSet>15)
		{
			//g_rfChanSet= 0;
			g_rfChanSet = 15;
		}
	}
	//SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_SELE,0);
	if(isSmall>0)
	{
		SetLinkshotChan(2,LAYOUT_CHAN_CML,g_rfChanSet,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	}else
	{
		SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,g_rfChanSet,DSP_SELE,0);
	}
	//UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,0);
}

//#ifdef TTL_DEBUG
void TtlDebugDspDat(u8 pa, u8 cml,u8 dat, u8 rvs)
{
	u8 tmp=0,i;
	memset(g_paraSetBuf,0x00,(2*8)*2);
	g_paraSetBufIndex = 1;
	for(i=0;i<2;i++)
	{
		//tmp = (i>0)?dat&0x0F: dat>>4;
		if(0==i)
		{
			tmp = dat>>4;
		}else
		{
			tmp = dat&0x0F;
		}
		if(tmp<10)
		{
			memcpy(g_paraSetBuf+g_paraSetBufIndex,fontIso+12*(tmp),6);
			memcpy(g_paraSetBuf+g_paraSetBufIndex+(2*8),fontIso+12*(tmp)+6,6);
		}else
		{
			tmp -= 10;
			memcpy(g_paraSetBuf+g_paraSetBufIndex,fontIso+12*12+12*(tmp),6);
			memcpy(g_paraSetBuf+g_paraSetBufIndex+(2*8),fontIso+12*12+12*(tmp)+6,6);
		}
		g_paraSetBufIndex+=8;
	}

	LcdCopyToRam(pa, cml,g_paraSetBuf, 2*8, 16);	
	
}
//#endif

void MultiDspNum(u8 pa, u8 cml,int dat, u8 rvs)
{
	u8 i=0;
	u8 m=100;
	u8 dsp=0;
//	int rDat=dat;
	
	if(DSP_NORMAL==rvs)
	{		
		memset(g_paraSetBuf,0x00,(3*8)*2);
	}else
	{
		memset(g_paraSetBuf,0xFF,(3*8)*2);
	}
	g_paraSetBufIndex = 1;

	if(!dat)
	{
		while(m>0)
		{
			
			if(DSP_NORMAL==rvs)
			{
				memcpy(g_paraSetBuf+g_paraSetBufIndex,fontIso+12*(11),6);
				memcpy(g_paraSetBuf+g_paraSetBufIndex+(3*8),fontIso+12*(11)+6,6);
			}else
			{
				for(i=0;i<6;i++)
				{
					g_paraSetBuf[i+g_paraSetBufIndex] = ~fontIso[i+12*(11)];
					g_paraSetBuf[i+(3*8)+g_paraSetBufIndex] = ~fontIso[i+12*(11)+6];
				}
			}
			m/=10;
			g_paraSetBufIndex+=8;
		}
	}else
	{
		while(m>0)
		{
			if(((dat/m)>0)||(dsp>0))
			{
				if(DSP_NORMAL==rvs)
				{
					memcpy(g_paraSetBuf+g_paraSetBufIndex,fontIso+12*(dat/m),6);
					memcpy(g_paraSetBuf+g_paraSetBufIndex+(3*8),fontIso+12*(dat/m)+6,6);
				}else
				{
					for(i=0;i<6;i++)
					{
						g_paraSetBuf[i+g_paraSetBufIndex] = ~fontIso[i+12*(dat/m)];
						g_paraSetBuf[i+(3*8)+g_paraSetBufIndex] = ~fontIso[i+12*(dat/m)+6];
					}
				}
				dsp = 1;
			}
			g_paraSetBufIndex+=8;
			dat%=m;
			m/=10;
		}
	}

	if(DSP_REVERSE==rvs)
	{
		for(i=0;i<(3*8);i++)
		{
			g_paraSetBuf[i] &= 0x3F;
			g_paraSetBuf[i+(3*8)] &= 0xFC;
		}
	}

	LcdCopyToRam(pa, cml,g_paraSetBuf, 3*8, 16);	
	//LAYOUT_PARAEX_COLUMN-1

}

void MulTiDspFrq(u8 pa, u8 cml,int frq, u8 rvs)
{

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

		if(mainPara.multi_times<0)
		{
			mainPara.multi_times = 0;
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
	SetMultiTmFrq(mainPara.multi_times,mainPara.multi_Frq,g_SettingIndex);
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
/*
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
*/
		if(mainPara.multi_Frq<=20)
		{
			mainPara.multi_Frq--;
		}else if(mainPara.multi_Frq<=50)
		{
			mainPara.multi_Frq-=5;
		}else if(mainPara.multi_Frq<=200)
		{
			mainPara.multi_Frq-=10;
		}else if(mainPara.multi_Frq<=500)
		{
			mainPara.multi_Frq-=50;
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
/*
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
*/
		if(mainPara.multi_Frq<20)
		{
			mainPara.multi_Frq++;
		}else if(mainPara.multi_Frq<50)
		{
			mainPara.multi_Frq+=5;
		}else if(mainPara.multi_Frq<200)
		{
			mainPara.multi_Frq+=10;
			//if(mainPara.multi_Frq==200)
				//mainPara.multi_Frq = 199;
		}else if(mainPara.multi_Frq<500)
		{
			mainPara.multi_Frq+=50;
		}else 
		{
			mainPara.multi_Frq = 500;
		}
		//if(mainPara.multi_Frq>199)
			//mainPara.multi_Frq = 199;
#endif

	}
	if((CMD_MODE_LIGH_MST==*g_cmdMode)&&(mainPara.multi_Frq>200))
	{
		mainPara.multi_Frq = 200;
	}
	//MultiCalMaxTime();
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
	SetMultiTmFrq(mainPara.multi_times,mainPara.multi_Frq,g_SettingIndex);
	//MULTI_SET_ETIMES(mainPara.multi_times,g_SettingIndex);
	//MULTI_SET_FRQ(mainPara.multi_Frq, g_SettingIndex)
	//LCD_MULFRQ_RVS(mainPara.multi_Frq);	
}

static u16 MathPow(u8 a, u8 k)
{
	
/*
	u8 i;u16 ret=1;
	for(i=0;i<k;i++)
	{
		ret*=a;
	}
*/	

	u8 i;u16 ret=1000;

	for(i=0;i<k;i++)
	{
		ret/=10;
	}
	return ret;
}

void IdSelSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		if(g_idSetIndex>0)
		{
			g_idSetIndex--;
		}
	}else
	{
		if(g_idSetIndex<3)
		{
			g_idSetIndex++;
		}
	}

	DspID(g_idSetIndex,DSP_NORMAL,radioSet->id);
	
}

void IdSetSubAdd(u8 dir)
{
	u16 stp = MathPow(10,g_idSetIndex);
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
*/
	if(KEY_DIR_LEFT==dir)
	{
		if((g_rfIdSet/stp)%10)
		{
			g_rfIdSet-=(stp);
		}else
		{
			g_rfIdSet+=(9*stp);
		}
	}else
	{
		if(((g_rfIdSet/stp)%10)<9)
		{
			g_rfIdSet+=stp;
		}else
		{
			g_rfIdSet-=(9*stp);
		}
	}
	//UartSctSend(UART_ENCODE_RFID, radioSet->id>>8, radioSet->id&0xff);
	//DspID(g_idSetIndex,DSP_SELE);
	//DspID(g_idSetIndex,g_rfIdSet);
	DspID(g_idSetIndex,DSP_SELE,g_rfIdSet);
	
}

void RfGrModSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		g_grSel--;
		if(g_grSel<0)
		{
			g_grSel = 4;
			g_grDspStart = 2;
		}else
		{
			g_grDspStart = g_grSel>0?g_grSel-1:0;
		}
	
	}else
	{
		g_grSel++;
		
		if(g_grSel>4)
		{
			g_grSel = 0;
			g_grDspStart=0;
			//g_SettingIndex = 0;
			//RFGR_SELBAR_CLEAR;
		}else
		{
			g_grDspStart = g_grSel>3?2:g_grSel-1;
		}
	}
	RfGrDspPara();
	RfgrSelMenu();

	GrDrawSelBar(g_grSel);

}

void RfGrParaSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		switch(radioSet->rfGr[g_grSel].mode)
		{
			//case WORKINGMODE_TTL:
			case GRMOD_TTL:
			case GRMOD_EXTA:
			//case WORKINGMODE_EXTA:
				if((-3==radioSet->rfGr[g_grSel].ttl_cmp1) &&(!radioSet->rfGr[g_grSel].ttl_cmp2))return;
				radioSet->rfGr[g_grSel].ttl_cmp2--;
				if(radioSet->rfGr[g_grSel].ttl_cmp2<0)
				{
					if(g_stepLength>0)
					{
						radioSet->rfGr[g_grSel].ttl_cmp2+=2;
					}else
					{
						radioSet->rfGr[g_grSel].ttl_cmp2+=3;
					}
					radioSet->rfGr[g_grSel].ttl_cmp1--;					
				}
				if(g_stepLength>0)
				{
					menuPara.rfGrTmp[g_grSel] = compensation_table_5[(radioSet->rfGr[g_grSel].ttl_cmp1+6)*2+radioSet->rfGr[g_grSel].ttl_cmp2];
				}else
				{
					menuPara.rfGrTmp[g_grSel] = compensation_table[(radioSet->rfGr[g_grSel].ttl_cmp1+6)*3+radioSet->rfGr[g_grSel].ttl_cmp2];
				}
			break;

			//case WORKINGMODE_MANU:
			case GRMOD_M:
				if((!radioSet->rfGr[g_grSel].m_exp1)&&(!radioSet->rfGr[g_grSel].m_exp2))return;
				radioSet->rfGr[g_grSel].m_exp2--;
				if(radioSet->rfGr[g_grSel].m_exp2<0)
				{
					radioSet->rfGr[g_grSel].m_exp2+=3;
					radioSet->rfGr[g_grSel].m_exp1--;
				}
				menuPara.rfGrOtp[g_grSel] = output_table[(7-radioSet->rfGr[g_grSel].m_exp1)*3-radioSet->rfGr[g_grSel].m_exp2];
			break;

			case WORKINGMODE_OFF:
			break;
		}
	}else
	{
		switch(radioSet->rfGr[g_grSel].mode)
		{
			//case WORKINGMODE_TTL:
			//case WORKINGMODE_EXTA:
			case GRMOD_TTL:
			case GRMOD_EXTA:
				if(3==radioSet->rfGr[g_grSel].ttl_cmp1)return;
				radioSet->rfGr[g_grSel].ttl_cmp2++;
				if(g_stepLength>0)
				{
					if(radioSet->rfGr[g_grSel].ttl_cmp2>1)
					{
						radioSet->rfGr[g_grSel].ttl_cmp2=0;
						radioSet->rfGr[g_grSel].ttl_cmp1++;					
					}
				}else
				{
					if(radioSet->rfGr[g_grSel].ttl_cmp2>2)
					{
						radioSet->rfGr[g_grSel].ttl_cmp2=0;
						radioSet->rfGr[g_grSel].ttl_cmp1++;					
					}
				}
				if(g_stepLength>0)
				{
					menuPara.rfGrTmp[g_grSel] = compensation_table_5[(radioSet->rfGr[g_grSel].ttl_cmp1+6)*2+radioSet->rfGr[g_grSel].ttl_cmp2];
				}else
				{
					menuPara.rfGrTmp[g_grSel] = compensation_table[(radioSet->rfGr[g_grSel].ttl_cmp1+6)*3+radioSet->rfGr[g_grSel].ttl_cmp2];
				}
			break;

			//case WORKINGMODE_MANU:
			case GRMOD_M:
				if(7==radioSet->rfGr[g_grSel].m_exp1)return;
				radioSet->rfGr[g_grSel].m_exp2++;
				if(radioSet->rfGr[g_grSel].m_exp2>2)
				{
					radioSet->rfGr[g_grSel].m_exp2=0;
					radioSet->rfGr[g_grSel].m_exp1++;
				}
				menuPara.rfGrOtp[g_grSel] = output_table[(7-radioSet->rfGr[g_grSel].m_exp1)*3-radioSet->rfGr[g_grSel].m_exp2];
			break;

			case WORKINGMODE_OFF:
			break;
		}
	}
	//RfGrDspPara(g_grSel);
	RfGrDspPara();
}

void SnrMenuSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		g_cfnIndex--;
		if(g_cfnIndex<0)
		{
			g_cfnIndex = 17;
			g_cfnStart = 16;
		}else
		{
			g_cfnStart = g_cfnIndex;
		}
		
	}else
	{
		g_cfnIndex++;
		if(g_cfnIndex>17)
		{
			g_cfnIndex = 0;
			g_cfnStart = 0;
		}else
		{
			g_cfnStart = g_cfnIndex-1;
		}
	}

	DspCfnMenu();
	
}

void PfnMenuSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		g_pfnIndex--;
		if(g_pfnIndex<0)
		{
			g_pfnIndex = 6;
			g_pfnStart = 5;
		}else
		{
			g_pfnStart = g_pfnIndex;
		}
		
	}else
	{
		g_pfnIndex++;
		if(g_pfnIndex>6)
		{
			g_pfnIndex = 0;
			g_pfnStart = 0;
		}else
		{
			g_pfnStart = g_pfnIndex-1;
		}
	}

	DspPfnMenu();
	
}


void SnrSubMenu(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		pSnrSet[g_cfnIndex]--;
		if(pSnrSet[g_cfnIndex]<0)
		{
			pSnrSet[g_cfnIndex] = 0;
		}
	}else
	{
		pSnrSet[g_cfnIndex]++;
		if(pSnrSet[g_cfnIndex]>(sm_Cfn[g_cfnIndex].subMenu->cnt-1))
		{
			pSnrSet[g_cfnIndex] = sm_Cfn[g_cfnIndex].subMenu->cnt-1;
		}
	}

	if(sm_Cfn[g_cfnIndex].excFunc!=NULL)
	{
		sm_Cfn[g_cfnIndex].excFunc(&pSnrSet[g_cfnIndex]);
	}

	EnterCfnSubMenu(pSnrSet[g_cfnIndex]);
}

void PfnSubMenu(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		pfnSet[g_pfnIndex]--;
		if(pfnSet[g_pfnIndex]<0)
		{
			pfnSet[g_pfnIndex] = 0;
		}
	}else
	{
		pfnSet[g_pfnIndex]++;
		if(pfnSet[g_pfnIndex]>(sm_Pfn[g_pfnIndex].subMenu->cnt-1))
		{
			pfnSet[g_pfnIndex] = sm_Pfn[g_pfnIndex].subMenu->cnt-1;
		}
	}

	if(sm_Pfn[g_pfnIndex].excFunc!=NULL)
	{
		sm_Pfn[g_pfnIndex].excFunc(&pfnSet[g_pfnIndex]);
	}

	EnterPfnSubMenu(pfnSet[g_pfnIndex]);
}

char ExtmIsoIndex(int iso)
{
	u8 i=0;
	for(i=0;i<ISO_ARR_CNT;i++)
	{
		if(iso<=isoValue[i])
		{
			return i;
		}
	}
	return 0;
}

char ExtmApertIndex(int apert)
{
	u8 i=0;
	for(i=0;i<APERT_ARR_CNT;i++)
	{
		if(apert<=apertValue[i])
		{
			return i;
		}
	}
	return 0;
}

void ExtmApertSubAdd(u8 dir)
{
	//signed char up,dn;
	u8 i=ExtmApertIndex(mainPara.extApert);

	if(KEY_DIR_LEFT==dir)
	{
		if(i>0)
		{
			i--;
		}
	}else
	{
		if(i<APERT_ARR_CNT-1)
		{
			i++;
		}
	}
	mainPara.extApert = apertValue[i];
	DspApertValue(mainPara.extApert, DSP_REVERSE);
	//DrawScale(mainPara.workMode,0,1);
	DrawScale(mainPara.workMode,flaPara.shutter,0,0,1);

}

void ExtmIsoSubAdd(u8 dir)
{
	u8 i=ExtmIsoIndex(mainPara.extIso);
	
	if(KEY_DIR_LEFT==dir)
	{
		//mainPara.extIso = isoValue[]
		if(i>0)
		{
			i--;
		}
			
	}else
	{
		if(i<ISO_ARR_CNT-1)
		{
			i++;
		}
		
	}
	mainPara.extIso = isoValue[i];
	DspIsoValue(LAYOUT_PARA_AREA2, 60,mainPara.extIso,DSP_REVERSE);
	//DrawScale(mainPara.workMode,0,1);
	DrawScale(mainPara.workMode,flaPara.shutter,0,0,1);
	
}

#ifdef FIBER_ADJUST_PROC
void FbAdjSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		mainPara.fiberCoe--;
		if(mainPara.fiberCoe<0)
		{
			mainPara.fiberCoe = 0;
		}
	}else
	{
		mainPara.fiberCoe++;
		if(mainPara.fiberCoe>18)
		{
			mainPara.fiberCoe = 18;
		}
	}
	FiberAdjustProc();
}

void FbExpSubAdd(u8 dir)
{
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
	FiberAdjustProc();
}
#endif



void KeySubAdd(u8 dir)
{
	//static char idSel=0;
	switch(g_SettingIndex)
	{
		case SETTING_IDLE:
			if(pSnrSet[SNRMENU_CMP_IDEL]>0)
			{
				if((WORKINGMODE_TTL==mainPara.workMode)
					||(WORKINGMODE_GR==mainPara.workMode)
					||(WORKINGMODE_EXTA==mainPara.workMode))
				{
					ExposeAddSub(dir);
				}
			}
		break;
		
#ifdef _SPEED_LIGHT_
		case SETTING_MOTO:
//#ifdef MOTO_TEST
			//MotoTest(dir);
			//MotoReset();
//#else
			MotoShift(dir);
//#endif
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

		case WLM_SETTING_ARR:
			Arr_SubAdd(dir);
		break;

		case SETTING_ARR_PARA:
			RfArrParaSubAdd(dir);
		break;

		case SETTING_SCAN_SET:
			ChSanSubAdd(dir);
		break;

		//case WLM_SETTING_ARRA:
		//	Arr_ASubAdd(dir);
		//break;

		//case WLM_SETTING_ARRB:
		//	Arr_BSubAdd(dir);
		//break;

		//case WLM_SETTING_ARRC:
			//Arr_CSubAdd(dir);
		//break;

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
					RfChanSet(dir,pfnSet[PFN_ID_LSSW]>0);
				break;
				
				case CMD_MODE_RF_MST:
				case CMD_MODE_RF_SLA:
					RfChanSet(dir,0);
				break;
			}
		break;

		case SETTING_MULTI:
			MultiTimesShift(dir);
		break;

		case SETTING_FRQ:
			MultiFrqShift(dir);
		break;

		case SETTING_ID_SEL:
			//IdSetSubAdd(dir);
			IdSelSubAdd(dir);
		break;

		case SETTING_ID_SET:
			IdSetSubAdd(dir);
		break;

		case SETTING_RFGR_SEL:
			//RfGrSubAdd(dir);
			RfGrModSubAdd(dir);
		break;

		case SETTING_GR_RES:
			RfGrParaSubAdd(dir);
		break;

		case SETTING_SNRMENU:
			SnrMenuSubAdd(dir);
		break;

		case SETTING_SNRSUB:
			SnrSubMenu(dir);
		break;

		case SETTING_PFNMENU:
			PfnMenuSubAdd(dir);
		break;

		case SETTING_PFNSUB:
			PfnSubMenu(dir);
		break;

		case SETTING_EXTM_ISO:
			ExtmIsoSubAdd(dir);
		break;

		case SETTING_EXTM_APERT:
			ExtmApertSubAdd(dir);
		break;

#ifdef FIBER_ADJUST_PROC
		case SETTING_FB_ADJ:
			FbAdjSubAdd(dir);
		break;

		case SETTING_FB_EXP:
			FbExpSubAdd(dir);
		break;
#endif

		
	}
}

void ChanSetConfirm(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
			g_SettingIndex = 0;
			//*wlSet.m_chan = g_liteChanSet;
			//SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);		//set directly
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,DSP_NORMAL,0);
		break;

		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			g_SettingIndex = 0;
			//UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
			if(radioSet->rfChan!=g_rfChanSet)
			{
				radioSet->rfChan = g_rfChanSet;
				SavePara(FLASH_USER_ADDR);
				UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
				//SavePara(FLASH_USER_ADDR);
			}
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
			}else
			{
				SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
			}
		break;
		
		case CMD_MODE_RF_MST:
		case CMD_MODE_RF_SLA:
			g_SettingIndex = 0;
			if(radioSet->rfChan!=g_rfChanSet)
			{
				radioSet->rfChan = g_rfChanSet;
				SavePara(FLASH_USER_ADDR);
				//SetMstChan(radioSet->rfChan,DSP_NORMAL);
				memset(radioStatue.devCnt,0x00,5);
				memset(radioStatue.devStatue,0,5);
				g_slaveReady = 0;
				if((CMD_MODE_RF_MST==*g_cmdMode)&&(!*wlSet.is_masterOn))
				{
					CHARGE_LED1_OFF;
					CHARGE_LED2_OFF;
					g_chargeOk = AD_CHARGE_LOW;
					g_chargeflag = 0;
				}
				RfDevLinkMark(mainPara.workMode,&wlSet);
				UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
			}
			//SetMstChan(radioSet->rfChan,DSP_NORMAL);
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
		break;
	}	

}

void KeyOk(void)
{

//	MenuItem *subItem;
	switch(g_SettingIndex)
	{
		case SETTING_SNRMENU:
			g_SettingIndex = SETTING_SNRSUB;
			EnterCfnSubMenu(pSnrSet[g_cfnIndex]);
		break;

		case SETTING_SNRSUB:
			g_SettingIndex = SETTING_SNRMENU;
			BtnSnrMenu();
		break;

		case SETTING_PFNMENU:
			g_SettingIndex = SETTING_PFNSUB;
			EnterPfnSubMenu(pfnSet[g_pfnIndex]);
		break;

		case SETTING_PFNSUB:
			g_SettingIndex = SETTING_PFNMENU;
			BtnPfnMenu();
		break;

		case SETTING_ARR_PARA:
			g_SettingIndex = WLM_SETTING_ARR;
			ModPasteScr(mainPara.workMode);
			RfArrSelMenu();
		break;	

		case WLM_SETTING_ARR:
			g_SettingIndex = SETTING_ARR_PARA;
			DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
		break;

		case SETTING_RFGR_SEL:
		case SETTING_GR_RES:
			//g_SettingIndex = SETTING_GR_RES;
			//g_SettingIndex = SETTING_GR_RES;
			BtnGrParaRes();
		break;
/*
		case SETTING_GR_RES:
			g_SettingIndex = SETTING_RFGR_SEL;
			RfGrDspPara();
			RfgrSelMenu();
		
			GrDrawSelBar(g_grSel);
		break;
*/

		case SETTING_ID_SEL:
			g_SettingIndex = SETTING_ID_SET;
			DspID(g_idSetIndex,DSP_SELE,radioSet->id);
		break;

		case SETTING_ID_SET:
			g_SettingIndex = SETTING_ID_SEL;
			//DspID(g_idSetIndex,DSP_NORMAL);
			if(radioSet->id!=g_rfIdSet)
			{
				radioSet->id = g_rfIdSet;
				if(CMD_MODE_RF_MST==*g_cmdMode)
				{
					memset(radioStatue.devCnt,0x00,5);
					memset(radioStatue.devStatue,0,5);
					g_slaveReady = 0;
					CHARGE_LED1_OFF;
					g_chargeOk = AD_CHARGE_LOW;
					RfDevLinkMark(mainPara.workMode,&wlSet);
				}
				UartSctSend(UART_ENCODE_RFIDEX, radioSet->id>>8, radioSet->id&0xff);
				
			}
			DspID(g_idSetIndex,DSP_NORMAL,radioSet->id);
		break;

		//case SETTING_SCAN_SET:
			//radioSet->rfChan = g_chSan ;
			//g_SettingIndex = 0;
			//KeyOk();
		//break;

		case WLM_SETTING_CHAN:
#if 0
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				case CMD_MODE_LIGH_SLA:
					g_SettingIndex = 0;
					//*wlSet.m_chan = g_liteChanSet;
					//SetMstChan(*wlSet.m_chan+1,DSP_NORMAL);		//set directly
					SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,DSP_NORMAL,0);
				break;

				case CMD_MODE_RFLS_MST:
				case CMD_MODE_RFLS_SLA:
					g_SettingIndex = 0;
					UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
					if(radioSet->rfChan!=g_rfChanSet)
					{
						radioSet->rfChan = g_rfChanSet;
						UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
						SavePara(FLASH_USER_ADDR);
					}
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						SetLinkshotChan(2,LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL);
					}else
					{
						SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
					}
				break;
				
				case CMD_MODE_RF_MST:
				case CMD_MODE_RF_SLA:
					g_SettingIndex = 0;
					if(radioSet->rfChan!=g_rfChanSet)
					{
						radioSet->rfChan = g_rfChanSet;
						SavePara(FLASH_USER_ADDR);
						//SetMstChan(radioSet->rfChan,DSP_NORMAL);
						memset(radioStatue.devCnt,0x00,5);
						CHARGE_LED1_OFF;
						CHARGE_LED2_OFF;
						g_chargeOk = AD_CHARGE_LOW;
						RfDevLinkMark(mainPara.workMode,&wlSet);
						UartSctSend(UART_ENCODE_RFCH, radioSet->rfChan,EXIT_BEFORE_SET);
					}
					//SetMstChan(radioSet->rfChan,DSP_NORMAL);
					SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
				break;
			}
#endif
			ChanSetConfirm();
		break;

		default:
#ifdef MOTO_TEST_DSP
		if(SETTING_MOTO==g_SettingIndex) g_motoTurn = 0;
#endif
		if(SETTING_ID_SEL==g_SettingIndex){DspClearAllPara();}
		if(SETTING_SCAN_SET==g_SettingIndex){radioSet->rfChan = g_chSan ;}
		g_SettingIndex = 0;
		//g_grSel = 0;
		Mode();
/*
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
			case CMD_MODE_LIGH_MST:
			case CMD_MODE_RF_MST:
				g_SettingIndex = 0;
				g_idSetIndex = 0;
				//g_grSel = 0;
				Mode(mainPara.workMode);
				//ModPasteScr(mainPara.workMode);
				//DspMstModIcon(*g_cmdMode);
				//RfDevLinkMark(mainPara.workMode,&wlSet);
				//LoadButtons();
			break;

			case CMD_MODE_RFLS_MST:
			case CMD_MODE_RFLS_SLA:
				//ModLinkedShot(radioSet->rfMod);
				g_SettingIndex = 0;
				//ModLinkedShot();
				//BuildLinkedShotMenu(radioSet->rfMod);
				//BuildLinkedShotMenu(*g_cmdMode);
				//BuildFuncMenu();
			break;
		}
*/
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

void ModeSw(void)
{
	if(0==g_SettingIndex)
	{
		//if(0==g_modeSwitch)
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
			}else if((CMD_MODE_OFF==*g_cmdMode)||(CMD_MODE_RFLS_MST==*g_cmdMode)
			||(CMD_MODE_RFLS_SLA==*g_cmdMode))
			{
				if((WORKINGMODE_MULTI+1)==mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_EXTA;
				}
				
				if(mainPara.workMode>WORKINGMODE_EXTM)
				{
					mainPara.workMode = WORKINGMODE_TTL;
				}
			}
			else
			{
				if(WORKINGMODE_MULTI<mainPara.workMode)
				{
					mainPara.workMode = WORKINGMODE_TTL;
					if(CMD_MODE_LIGH_SLA==*g_cmdMode)
					{
						g_isIndividualSlave = 0;
					}
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
			flaPara.ttl_FEB1 = 0;					
			flaPara.ttl_FEB2 = 0;
			
		}//clear parameters
#if 0
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
#endif
	}
	//BuildFuncMenu();
	g_ArrSel = 0;
	g_grSel =0;
}

void KeyMenuSw(void)
{
	InsertList(funcMenu.funcList[g_menuId]);
	pMenuList = &funcMenu.funcList[g_menuId];
}

void ModLinkedShot(void)
{
	//DspClearAllPara();	
//	const char *ls = "LINKEDSHOT";
	//char *pLs=ls;

	DspClearAllPara();	
	
	//SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_NORMAL,0);
	SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,WLM_SETTING_CHAN==g_SettingIndex?DSP_SELE:DSP_NORMAL,0);

	memset(g_paraSetBuf,0x00,120*2);	
	LoadPfnString("LI|N|KED SHOT",g_paraSetBuf,120,DSP_NORMAL);
	AreaScroll(g_paraSetBuf, 120, 16/8,72, SCROLL_DOWN, 3);

	if(CMD_MODE_RFLS_MST==*g_cmdMode)
	{
		RamLoadPicture((char*)mstBitMap, 42, 8, g_paraSetBuf+76, 120, DSP_NORMAL);
	}else
	{
		RamLoadPicture((char*)slvBitMap, 41, 8, g_paraSetBuf+76, 120, DSP_NORMAL);
	}

	AreaScroll(g_paraSetBuf+76, 120, 16/8, 42,SCROLL_DOWN, 5);

	
	
	
	LcdCopyToRam(0,3,g_paraSetBuf,120,16);

	
		
	
	//LCD_ShowString(8,8,"LINKED SHOT",FONT_TYPE_BIG,0, DSP_NORMAL);
/*
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
*/
#if 0
	DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_SIZE,rfBitmap,DSP_NORMAL);
#endif
	//radioSet->rfMod= LINKED_SHOT_MST;
}

#ifdef RF_LOWPWR_SLEEP
void RfMoudleSet(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;
		
		case CMD_MODE_RF_MST:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);			//send the information when RF module inquire
		break;
			
		case CMD_MODE_LIGH_MST:
			//CHARGE_LED_OFF;
			UartSctSend(UART_ENCODE_EXIT, 1, 0);
			delay_ms(20);
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RFLS_MST:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_SLA:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_RF_SLA:
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,*wlSlave.s_array);
		break;
	}

}
#else
void RfMoudleSet(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			//UartSctSend(UART_ENCODE_EXIT, 1, 0);
			//delay_ms(20);
			DISABLE_RF_MODULE();
		break;
		
		case CMD_MODE_RF_MST:
			//CHARGE_LED_OFF;
			ENABLE_RF_MODULE();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);			//send the information when RF module inquire
		break;
			
		case CMD_MODE_LIGH_MST:
			//CHARGE_LED_OFF;
			UartSctSend(UART_ENCODE_EXIT, 1, 0);
			delay_ms(20);
			DISABLE_RF_MODULE();
		break;

		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RFLS_MST:
			//CHARGE_LED_OFF;
			ENABLE_RF_MODULE();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_SLA:
			DISABLE_RF_MODULE();
		break;

		case CMD_MODE_RF_SLA:
			ENABLE_RF_MODULE();
			//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,*wlSlave.s_array);
		break;
	}

}
#endif

void MstModCheck()
{
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
			*g_cmdMode = CMD_MODE_LIGH_MST;
			memset(radioStatue.devCnt,0x00,5);
			memset(radioStatue.devStatue,0,5);
			g_slaveReady = 0;
			if(WORKINGMODE_GR==mainPara.workMode)	mainPara.workMode = WORKINGMODE_TTL;
			Mode(mainPara.workMode);
			LoadButtons();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_LIGH_MST:
		case CMD_MODE_RFLS_MST:
			*g_cmdMode = CMD_MODE_RF_MST;
			Mode(mainPara.workMode);
			LoadButtons();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		break;

		case CMD_MODE_RFLS_SLA:
			*g_cmdMode = CMD_MODE_RFLS_MST;
			ModLinkedShot();
			UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
			//BuildLinkedShotMenu(*g_cmdMode);
			BuildFuncMenu();
		break;
	}

#else
	if(CMD_MODE_RFLS_SLA==*g_cmdMode)
	{
		*g_cmdMode = CMD_MODE_RFLS_MST;
	}else
	{
		if((CMD_MODE_OFF==*g_cmdMode)&&(WORKINGMODE_MULTI<mainPara.workMode))
		{
			return;
		}
/*		
		(*g_cmdMode)++;
		
		if(*g_cmdMode>CMD_MODE_LIGH_SLA)
		{
			*g_cmdMode = CMD_MODE_OFF;
		}
*/
		switch(pfnSet[PFN_ID_CMDM])
		{
			case 0:
				(*g_cmdMode)++;		
				if(*g_cmdMode>CMD_MODE_LIGH_SLA)
				{
					*g_cmdMode = CMD_MODE_OFF;
				}
				if(g_cmrLinked>0)
				{
					if(CMD_MODE_RF_SLA==*g_cmdMode)
					{
						*g_cmdMode = CMD_MODE_LIGH_MST;
					}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
					{
						*g_cmdMode = CMD_MODE_OFF;
					}
				}
			break;

			case 1:
				(*g_cmdMode)++;		
				if(*g_cmdMode>CMD_MODE_RF_SLA)
				{
					*g_cmdMode = CMD_MODE_OFF;
				}
				if(g_cmrLinked>0)
				{
					if(CMD_MODE_RF_SLA==*g_cmdMode)
					{
						*g_cmdMode = CMD_MODE_OFF;
					}
				}
			break;

			case 2:
				(*g_cmdMode)++;		
				if(CMD_MODE_RF_MST==*g_cmdMode)
				{
					*g_cmdMode = CMD_MODE_LIGH_MST;
				}
				if(*g_cmdMode>CMD_MODE_LIGH_SLA)
				{
					*g_cmdMode = CMD_MODE_OFF;
				}
				if(g_cmrLinked>0)
				{
					if(CMD_MODE_LIGH_SLA==*g_cmdMode)
					{
						*g_cmdMode = CMD_MODE_OFF;
					}
				}
			break;

			default:
				(*g_cmdMode)++;		
				if(*g_cmdMode>CMD_MODE_LIGH_SLA)
				{
					*g_cmdMode = CMD_MODE_OFF;
				}
				if(g_cmrLinked>0)
				{
					if(CMD_MODE_RF_SLA==*g_cmdMode)
					{
						*g_cmdMode = CMD_MODE_LIGH_MST;
					}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
					{
						*g_cmdMode = CMD_MODE_OFF;
					}
				}
			break;

		}

		if((CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_LIGH_SLA==*g_cmdMode))
		{
			mainPara.workMode = WORKINGMODE_TTL;
		}else if((CMD_MODE_LIGH_MST==*g_cmdMode)||(CMD_MODE_OFF==*g_cmdMode))
		{
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				mainPara.workMode = WORKINGMODE_TTL;
			}
		}
		//mainPara.workMode = WORKINGMODE_TTL;
		memset(radioStatue.devCnt,0x00,5);
		memset(radioStatue.devStatue,0,5);

/*
		if((CMD_MODE_LIGH_SLA==*g_cmdMode)||(CMD_MODE_RF_SLA==*g_cmdMode))
		{
			mainPara.workMode = WORKINGMODE_TTL;
		}else if(CMD_MODE_RF_MST!=*g_cmdMode)
		{
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				mainPara.workMode = WORKINGMODE_TTL;
			}
		}else
		{
			memset(radioStatue.devCnt,0x00,5);			//rf master
		}
*/
	}
	g_menuId = 0;
	g_SettingIndex = 0;
	flaPara.ttl_FEB1 = flaPara.ttl_FEB2 = 0;
//	Mode(mainPara.workMode);
	//Mode();
	//LoadButtons();
	RfMoudleSet();
	FuncModuleSw();
	//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
	Mode();
	UartIntSet();
	//TurnOnChargeLed(1);
#endif
	//printf("%x\r\n",*g_cmdMode);

}

void KeyToLinkMod(void)
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
		//mainPara.workMode = WORKINGMODE_TTL;			//abey the cannon rules
		//ModLinkedShot();
		//BuildLinkedShotMenu(*g_cmdMode);
		//g_menuId = 0;
		//BuildFuncMenu();
		Mode();
		RfMoudleSet();
		//UartSctSend(UART_ENCODE_RFMOD, *g_cmdMode,0);
		memset(radioStatue.devCnt, 0, 5);
		g_slaveReady = 0;
		BkLiteOn();
	}
}

void TtlTestProc(void)
{
	if(pSnrSet[SNRMENU_TEST]>0)
	{
		pTime = CanOutputSwitch(7,0);
#ifdef FLASH_BASE_FIBER
		WlmFlaADSet(7,0);
		TURN_ON_TRIGER(pTime);
#else
		TURN_ON_TRIGER(pTime);
#endif
	}else
	{
		//pTime = CanOutputSwitch(2,0);
		pTime = CanOutputSwitch(3,0);
#ifdef FLASH_BASE_FIBER
		WlmFlaADSet(3,0);
		TURN_ON_TRIGER(OUTPUT_MAX);
#else
		TURN_ON_TRIGER(pTime);
#endif
	}
	//TURN_ON_TRIGER(pTime);
	delay_us(800);
}


void Pilot(void)
{

	u8 cnt=0;
	u8 expAcc=0;
	//if(flaPara.charge_ok<CHARGE_FUL)
	//return;

	//FLAOK_TOGGLE();
//	signed char up=0,dn=0;

//	HpPreflash(HP_PREFLASH_INTERVAL);

//	return;

//	ENABLE_D30EN();
	//HaltSec(1);
	

#ifdef FIBER_ADJUST_PROC

	HpSetting hpSet;
#endif

	//if((g_isHotProtect>0)||(g_cmrLinked>0))
	if(g_isHotProtect>0)
	{
		return;
	}

#ifdef FIBER_ADJUST_PROC

	if(SETTING_FIBER==g_SettingIndex)
	{
		if(CMD_MODE_OFF==*g_cmdMode)
		{
			HpExpCalc(mstPara->output1,mstPara->output2,0,0,0,&hpSet,0);
			//HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);
			HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			UartSctSend(UART_ENCODE_UNMODULATE,0,0);
		}

		return;
	}

#endif


	if((1==pSnrSet[SNRMENU_MODELING])||(2==pSnrSet[SNRMENU_MODELING])||(2==g_isModeling))
	{
		if(((*g_cmdMode!=CMD_MODE_RF_MST)&&(*g_cmdMode!=CMD_MODE_LIGH_MST))
			||(*wlSet.is_masterOn>0))
		{
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				ENABLE_D30EN_EX();
				for(cnt=0;cnt<66;cnt++)
				{
#ifdef FLASH_BASE_FIBER
					WlmFlaADSet(1,0);	
					TURN_ON_TRIGER(OUTPUT_MAX);
#else
					TURN_ON_TRIGER(TTLTRA3);
#endif
					HaltSec(15);
				}
				DISABLE_D30EN_EX();
			}
			g_expAccHotWarnValue+=32;
			g_isModeling = 0;
			return;
		}
		g_isModeling = 0;
	}

	if(g_cmrLinked>0)
	{
		return;
	}
	
	
	ENABLE_D30EN_EX();

	if(WORKINGMODE_MANU==mainPara.workMode)
	{
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			if(g_chargeOk>AD_CHARGE_RDY)
			{
				BuildExposeCmd(mainPara.workMode,HP_FLASHMODE_OFF,0);
				//wlSet.m_cmdProcOn=WLM_CMDPROC_ON;
				//Delay_10us(15);
				OS_ENTER_CRITICAL();
				WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
				OS_EXIT_CRITICAL();
				delay_us(800);
				//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			}
			//Delay_10us(50);
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
			//if(pSnrSet[SNRMENU_MODELING])
			//if((1==pSnrSet[SNRMENU_MODELING])&&(2==pSnrSet[SNRMENU_MODELING]))
			if((1==pSnrSet[SNRMENU_MODELING])||(2==pSnrSet[SNRMENU_MODELING]))
			{
				UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			}else
			{
				RfFlashConfirm(2,FLASH_PILOT_ON);
				delay_ms(20);
				RfCountDownInfo(0);
				if(g_chargeOk>AD_CHARGE_LOW)
				{
					if(*wlSet.is_masterOn>0)
					{
						pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
#ifdef FLASH_BASE_FIBER
						WlmFlaADSet(mstPara->output1,mstPara->output2);
						TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
						TURN_ON_TRIGER(pTime);
#endif
					}
				}
			}
			
		}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			StopCapture();
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
#ifdef FLASH_BASE_FIBER
				WlmFlaADSet(mstPara->output1,mstPara->output2);
				TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
				TURN_ON_TRIGER(pTime);
#endif
				delay_us(800);
			}
			StartCapture();
		}
		else
		{
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
#ifdef FLASH_BASE_FIBER
				WlmFlaADSet(mstPara->output1,mstPara->output2);
				TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
				TURN_ON_TRIGER(pTime);
#endif
			}
		}
		expAcc = (mstPara->output1*3+mstPara->output2);
		//Delay_ms(1);
		
	}
	else if(WORKINGMODE_TTL==mainPara.workMode)
	{
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
#ifdef LIGH_CMD_REBUILD
			wlSet.m_flaCmd[0] = 0xef;
			//switch(*wlSet.arr_mode)
			switch(*wlSet.ttl_Ratio)
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
			//switch(*wlSet.arr_mode)
			switch(*wlSet.ttl_Ratio)
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
			if(g_chargeOk>AD_CHARGE_RDY)
			{
				OS_ENTER_CRITICAL(); 	
				WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
				//OS_EXIT_CRITICAL();
				delay_us(700);
#if 0
				if(*wlSet.is_masterOn>0)
				{
				/*
					if(pSnrSet[SNRMENU_TEST]>0)
					{
						pTime = CanOutputSwitch(7,0);
					}else
					{
						pTime = CanOutputSwitch(2,0);
					}
					TURN_ON_TRIGER(pTime);
					delay_us(800);
				*/
					delay_ms(30);
					//delay_ms(15);
					TtlTestProc();
				}else
				{
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
					WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				}
#endif
				WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				OS_EXIT_CRITICAL();
			}
/*
			pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
			TURN_ON_TRIGER(pTime);
			delay_us(800);
*/
	
			
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
			//UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			//if(pSnrSet[SNRMENU_MODELING])
			//if((1==pSnrSet[SNRMENU_MODELING])&&(2==pSnrSet[SNRMENU_MODELING]))
			if((1==pSnrSet[SNRMENU_MODELING])||(2==pSnrSet[SNRMENU_MODELING]))
			{
				UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			}else
			{
				wlSet.m_ttlCmd[0] = wlSet.m_ttlCmd[1] = wlSet.m_ttlCmd[2] = 0x98;
				RfFlashConfirm(2,FLASH_PILOT_ON);
				delay_ms(20);
				RfCountDownInfo(0);
				if(g_chargeOk>AD_CHARGE_LOW)
				{
					if(*wlSet.is_masterOn>0)
					{
						//pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
						//TURN_ON_TRIGER(pTime);
						/*
						if(pSnrSet[SNRMENU_TEST]>0)
						{
							pTime = CanOutputSwitch(7,0);
						}else
						{
							pTime = CanOutputSwitch(2,0);
						}
						TURN_ON_TRIGER(pTime);
						delay_us(800);
						*/
						TtlTestProc();
					}
				}
			}
		}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				StopCapture();
				TtlTestProc();
				delay_us(800);
				StartCapture();
			}
		}
		else
		{
			//pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
			//TURN_ON_TRIGER(pTime);//HP_REALFLA_INTERVAL
			//TURN_ON_TRIGER(HP_REALFLA_INTERVAL);		
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				TtlTestProc();
			}
		}

		if(pSnrSet[SNRMENU_TEST]>0)
		{
			expAcc=21;
		}else
		{
			expAcc=6;
		}
	}
	else if(WORKINGMODE_MULTI==mainPara.workMode)
	{
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			wlSet.muitl_realTimes = *multiSet.eTimes;
			BuildExposeCmd(mainPara.workMode,HP_FLASHMODE_OFF,0);
			//wlSet.m_cmdProcOn=WLM_CMDPROC_ON;
			//Delay_10us(15);
			if(g_chargeOk>AD_CHARGE_RDY)
			{
				OS_ENTER_CRITICAL(); 
				WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
				OS_EXIT_CRITICAL();
				delay_us(800);
				//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
				WlmCmdOneShot(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			}
			//Delay_10us(50);
			
		}else if(CMD_MODE_RF_MST==*g_cmdMode)
		{
			//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
			//UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			//if(pSnrSet[SNRMENU_MODELING])
			//if((1==pSnrSet[SNRMENU_MODELING])&&(2==pSnrSet[SNRMENU_MODELING]))
			if((1==pSnrSet[SNRMENU_MODELING])||(2==pSnrSet[SNRMENU_MODELING]))
			{
				UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
			}else
			{
				RfFlashConfirm(2,FLASH_PILOT_ON);
				delay_ms(20);
				RfCountDownInfo(0);
				if(g_chargeOk>AD_CHARGE_LOW)
				{
					if(*wlSet.is_masterOn>0)
					{
						MultiFlashPrepear();
#ifdef _CANON_
						FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose,0,0);
#else
						FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose>>4,0,0);
#endif
						SetMultiFlashTimer();
					}
				}
			}
		}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				StopCapture();
				pTime = CanOutputSwitch(*multiSet.expose,0);
#ifdef FLASH_BASE_FIBER
				WlmFlaADSet(*multiSet.expose,0);
				TURN_ON_TRIGER(*multiSet.expose>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
				TURN_ON_TRIGER(pTime);
#endif
				delay_us(800);
				StartCapture();
			}
		}
		else
		{
/*		
			if((unsigned long)(1000*(*multiSet.eTimes))<(unsigned long)(500*(*multiSet.frequence)))
			{
				multiSet.finalExpTimes = *multiSet.eTimes;
			}else
			{
				multiSet.finalExpTimes = ((*multiSet.frequence)/2+1);
			}
			
			while(multiSet.finalExpTimes--)
			{
				pTime = CanOutputSwitch(*multiSet.expose>>4,0);
				TURN_ON_TRIGER(pTime);
				delay_ms(1000/(*multiSet.frequence));
			}
*/
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				MultiFlashPrepear();
#ifdef _CANON_
				FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose,0,0);
#else
				FlashInResMode(WORKINGMODE_MULTI,*multiSet.expose>>4,0,0);
#endif
				SetMultiFlashTimer();
				//multiSet.mulCtl=MULTI_FLASH_ENABLE;			//for pilot
			}
			
		}
		expAcc = (*multiSet.expose)*3;
	}else if(WORKINGMODE_GR==mainPara.workMode)
	{
		//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_PILOT, mainPara.workMode);
		//if(pSnrSet[SNRMENU_MODELING])
		//if((1==pSnrSet[SNRMENU_MODELING])&&(2==pSnrSet[SNRMENU_MODELING]))
		if((1==pSnrSet[SNRMENU_MODELING])||(2==pSnrSet[SNRMENU_MODELING]))
		{
			UartSctSend(UART_ENCODE_PILOT,pSnrSet[SNRMENU_MODELING],mainPara.workMode);
		}else
		{
			flaPara.rfCmuPara[0].ttlExpVal = flaPara.rfCmuPara[1].ttlExpVal = flaPara.rfCmuPara[2].ttlExpVal
			=flaPara.rfCmuPara[3].ttlExpVal=flaPara.rfCmuPara[4].ttlExpVal = 0x98;			
			//memset(flaPara.rfCmuPara,0x98,5);
			RfFlashConfirm(2,FLASH_PILOT_ON);
			delay_ms(20);
			RfCountDownInfo(0);
		}
		if(*wlSet.is_masterOn>0)
		{
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				TtlTestProc();
			}
		}

		if(pSnrSet[SNRMENU_TEST]>0)
		{
			expAcc=21;
		}else
		{
			expAcc=6;
		}
	}else if(WORKINGMODE_EXTM==mainPara.workMode)
	{
		//ExtModSet();
		if(g_chargeOk>AD_CHARGE_LOW)
		{
			expAcc = ExtmModSet(motoSet.focIndex,mainPara.extIso,mainPara.extApert);
			ExtModFlash(5000);
		}
	}
	else if(WORKINGMODE_EXTA==mainPara.workMode)
	{
		if(g_chargeOk>AD_CHARGE_LOW)
		{
			expAcc = ExtmModSet(motoSet.focIndex,100,56);
			ExtModFlash(5000);
		}
	}
	else if(WORKINGMODE_F2==mainPara.workMode)
	{
		if(g_chargeOk>AD_CHARGE_LOW)
		{
			pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
#ifdef FLASH_BASE_FIBER
			WlmFlaADSet(mstPara->output1,mstPara->output2);
			TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
			TURN_ON_TRIGER(pTime);
#endif
			delay_us(800);
		}
		//Timer7SetCapture();
	}else if(WORKINGMODE_F1==mainPara.workMode)
	{
		if(g_chargeOk>AD_CHARGE_LOW)
		{
			pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
#ifdef FLASH_BASE_FIBER
			WlmFlaADSet(mstPara->output1,mstPara->output2);
			TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
			TURN_ON_TRIGER(pTime);
#endif
			delay_us(800);
		}
	}else if(WORKINGMODE_RTC==mainPara.workMode)
	{
		if(g_chargeOk>AD_CHARGE_LOW)
		{
			pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
#ifdef FLASH_BASE_FIBER
			WlmFlaADSet(mstPara->output1,mstPara->output2);
			TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
#else
			TURN_ON_TRIGER(pTime);
#endif
			delay_us(800);
		}
	}

	g_expAccHotWarnValue += (1<<(expAcc/3)); 
	g_motoInvalidCounter = MOTO_INVALID_CNT;

	// 20171208
	//SET_UPDATE_EVT(DSP_UPDATE_MODE);

	//FLAOK_OFF();
	//DISABLE_D30EN();
	DISABLE_D30EN_EX();

}

void ResetKeyInt(void)
{
	//Ex_NVIC_Enable(1);
	Ex_NVIC_Enable(10);
}


/////////////////////////////////////////////////////////////////////////////////
//键处理函数

u8 ModeFuncJudge(void)
{
//	if(g_menuLock>0)
//	{
//		return 0;
//	}
	
	//if((CMD_MODE_RF_SLA==*g_cmdMode)||(CMD_MODE_LIGH_SLA==*g_cmdMode))
	if(CMD_MODE_RF_SLA==*g_cmdMode)			//2015/10/16
	{
		return 0;
	}else if((CMD_MODE_RFLS_MST==*g_cmdMode)||(CMD_MODE_RFLS_SLA==*g_cmdMode))
	{
		if(pfnSet[PFN_ID_LSSW]>0)
		{
			return 1;
		}else 
		{
			return 0;
		}
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)									//2015/10/16
	{
		if(g_isIndividualSlave>0)
		{
			return 1;
		}else
		{
			return 0;
		}
	}else 
	{
		return 1;
	}

}

u8 IsModSwitchValid(void)
{
	switch(g_SettingIndex)
	{
		case WLM_SETTING_CHAN:
			ChanSetConfirm();
		return 1;
		
		case SETTING_ID_SEL:
		case SETTING_SNRMENU:
		case SETTING_SNRSUB:
		case SETTING_PFNMENU:
		case SETTING_PFNSUB:
		case SETTING_SCAN_SET:
		case SETTING_ID_SET:
		case SETTING_RFSCAN:
#ifdef FIBER_ADJUST_PROC
		case SETTING_FB_EXP:
		case SETTING_FB_ADJ:
		case SETTING_FIBER:
#endif
		return 0;

		default:
		//g_SettingIndex = 0;
		return 1;
		//break;
		
	}
}


void KeyProcess(void)
{
	unsigned char rkey;
	static char lockRs=0;
	static char lockTirFlag=0;
	static char lockCnt=0;
//	static char lockRs=0;
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
		Pilot();
		gWSleepCounter = 0;
		HaltSec(300);
		return;
	}

	if(g_menuLock>0)
	{
		return;
	}

#if 1
	if(!KEY_LOCK_EX)
	{
		if(!lockRs)
		{
			lockRs = 1;
			ResetKeyInt();
			LcdClearFeild(LAYOUT_MENU_PAGE,0,LCD_W,LAYOUT_BUTTON_HIGHT);
			//BtnHome();
			//g_SettingIndex = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}else if(rkey>0)
		{
			//DspMenuLocked();
			DspMenuIcon(42,font_locked);
			ResetKeyInt();
			BkLiteOn();
			lockTirFlag = 1;
			lockCnt = 0;
		}else if(lockTirFlag>0)
		{
			if(lockCnt++>40)
			{
				lockCnt=0;
				lockTirFlag=0;
				LcdClearFeild(LAYOUT_MENU_PAGE,0,LCD_W,LAYOUT_BUTTON_HIGHT);
			}
		}
		HaltSec(50);
		return;
	}
	else 
	{
		if((lockRs>0)&&(!KEY_POW_ON))
		{
			lockRs = 0;
			ResetKeyInt();
			LoadButtons();
		}
	}
#endif

#if 0
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
#endif

	//printf("%x",rkey);

	if(!rkey)
	{
		HaltSec(10);
		return;
	}

	//if(pSnrSet[SNRMENU_BKLITE]<=0)
	{
//		BUZZER_ON(BUZZER_TIME_NORMAL,1);
		//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
		BkLiteOn();
		gWSleepCounter = 0;
	}

	//printf("%x\n",rkey);

	switch(rkey)
	{
		case KEY_MODE:
#ifdef KEY_DEBUG
		printf("mode");
#else
		//if(0==g_SettingIndex)
		if(IsModSwitchValid())
		{
			//if((CMD_MODE_RFLS_MST!=*g_cmdMode)&&(CMD_MODE_RFLS_SLA!=*g_cmdMode)
				//&&(CMD_MODE_RF_SLA!=*g_cmdMode)&&(CMD_MODE_LIGH_SLA!=*g_cmdMode))
			g_SettingIndex = 0;
			if(ModeFuncJudge())
			{
				ModeSw();
				//Mode(mainPara.workMode);
				g_menuId = 0;
				Mode();
				//g_menuId = 0;
				//LoadButtons();
			}

		}else 
		{
			SelectMode(mainPara.workMode,wlSet.m_current_arr);
		}
#endif
		HaltSec(50);
		break;

		case KEY_MODE_LONG:
#if 1
#ifdef KEY_DEBUG
		printf("mode long");
#else
		SwithMode();
#endif
#endif
		break;

		case KEY_RF_SW:
#ifdef KEY_DEBUG
			printf("rf");
#else
#ifdef FIBER_ADJUST_PROC
		if((g_SettingIndex>=SETTING_FB_EXP)&&(g_SettingIndex<=SETTING_FIBER))
		{
			return;
		}
#endif
		if(!g_menuLock)
		{
			g_isIndividualSlave = 0;
			if(IsModSwitchValid())
			{
				MstModCheck();
			}
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
#ifdef MOTO_TEST
			printf("zoom adc %d\r\n",GetZoomAdc());
			g_motoTurn = 0;
#endif
#ifdef FIBER_ADJUST_PROC
			if((g_SettingIndex>=SETTING_FB_EXP)&&(g_SettingIndex<=SETTING_FIBER))
			{
				g_SettingIndex = SETTING_FIBER;
				//mainPara.isFiberAdjust = 0xA5;
				mainPara.isFiberAdjust = FIBER_ADJUST_FLAG;
				radioSet->id = 0;
				//mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
				//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
				//SavePara();
				SavePara(FLASH_USER_ADDR);
				FiberAdjustProc();
				return;
			}else
			{
				KeyOk();
				if(CMD_MODE_LIGH_SLA==*g_cmdMode)
				{
					LightSlaveParaClear();	
				}
			}
			
#else
			KeyOk();
#endif
			//MotoReset();
#endif
			HaltSec(50);
		break;


		case KEY_LEFT:
#ifdef KEY_DEBUG
			printf("left");
#else
			//KeyLeft(mainPara.workMode);
//#ifdef MOTO_TEST
//			MotoTest(KEY_DIR_LEFT);
//#else
			KeySubAdd(KEY_DIR_LEFT);
//#endif
#endif
			//HaltSec(200);
			HaltSec(10);
			ResetKeyInt();
		break;

		case KEY_RIGHT:
#ifdef KEY_DEBUG
			printf("right");
#else
//#ifdef MOTO_TEST
//			MotoTest(KEY_DIR_RIGHT);
//#else
			//KeyRight(mainPara.workMode);
			KeySubAdd(KEY_DIR_RIGHT);
//#endif
#endif
			//HaltSec(200);
			HaltSec(10);
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

#ifdef FIBER_ADJUST_PROC
		case KEY_OK_LONG:
			if((1203==radioSet->id)&&(!PILOT_IN))
			{
				g_SettingIndex = SETTING_FIBER;
				FiberAdjustProc();
			}
		break;
#endif

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

		case KEY_CLEAR_LONG:
#ifdef KEY_DEBUG
			printf("ModeClear");
#endif
			if((g_SettingIndex>=SETTING_FB_EXP)&&(g_SettingIndex<=SETTING_FIBER))
			{
				g_SettingIndex = SETTING_FIBER;
				//mainPara.isFiberAdjust = 0xA5;
				mainPara.isFiberAdjust = 0x00;
				mainPara.isRgbChk = 0x00;
				mainPara.fiberCoe = 9;
				radioSet->id = 0;
				//mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
				//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
				//SavePara();
				SavePara(FLASH_USER_ADDR);
				DspMenuIcon(47,font_clear);
				HaltSec(999);
				FiberAdjustProc();
			}else
			{
				ModeClear();
			}
		break;

		case KEY_PILOT:
#ifdef NBK600EX_DEBUG
			PilotTest();
#else
			Pilot();
			//KeyOk();
#endif
			HaltSec(50);
		break;

		default:
			HaltSec(50);
		break;
	}

}

void AfAutoCtrl(void)
{
	//if(!pSnrSet[SNRMENU_AF])
	//if(g_cmrLinked>0)
	static u8 afInRsv=0;
	if(g_cmrPressed>0)
	{
		if((!pSnrSet[SNRMENU_AF])&&(!g_shutterFlag))
		{
		/*
			if(!(AF_CTL_IN^AF_CTL_OUT))
			{
				AF_CTL_OUT = !AF_CTL_IN;
				//FLA_OK_LED=!FLA_OK_LED;
			}
		*/
			if(afInRsv!=AF_CTL_IN)
			{
				afInRsv = AF_CTL_IN;
				if(!AF_CTL_IN)
				{
					switch(g_AfIndex)
					{
						case 0:
							//AF_RIGHT = 1;
							AF_UNDERSIDE = 1;
						break;

						case 1:
							AF_LEFT = 1;
							AF_RIGHT = 1;
						break;

						case 2:
							AF_RIGHT = 1;
							AF_UNDERSIDE = 1;
							//AF_RIGHT = 1;
						break;
					}
				}else
				{
					AF_UNDERSIDE = 0;
					AF_LEFT = 0;
					AF_RIGHT = 0;
				}
			}
		}
	}
}

void ChargeBuzzer(void)
{
	if(!g_chargeflag)
	{
		BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
		g_chargeflag = 1;
	}
}

#define AD_VCHK_TIMS 4//16
#define AD_AVR(x)    (x/AD_VCHK_TIMS)
//#define VCK_LEVEL	185
#define VCK_LOW		135
#define VCK_LEVEL	152//142
#define VFUL_LEVEL	180//170
#define RSV_VALID(x) (x>>4)  
#define LOW_BAT_TIMEOUT   	180
//27.5/3627.5*VOL
//u8 g_adBuf[10]={0};
void ChkVoltage(void)
{
	//static u16 vChk=0;
	static u8 ckTimes=0;
	//static u8 flag=0;
	static u8 lowbatCnt=0;
	static u32 adSum=0;
//	static u8 cIndex=0;
//	u16 adTmp=0;
	
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_CKV;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));

	adSum += RSV_VALID(ADC1->DR);

	ckTimes++;
	
	if(ckTimes>=AD_VCHK_TIMS)
	{
		ckTimes = 0;
		if(AD_AVR(adSum)>VCK_LEVEL)
		{
			if(AD_AVR(adSum)>VFUL_LEVEL)
			{
				g_chargeOk = AD_CHARGE_FUL;
			}else
			{
				g_chargeOk = AD_CHARGE_RDY;
			}
			//CR_READY = 1;

			//if(!g_chargeflag)
			//{
			//	BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
			//	g_chargeflag = 1;
			//}
			lowbatCnt = 0;
		}else if(AD_AVR(adSum)<VCK_LOW)
		{
			//CR_READY = 0;
			g_chargeOk = AD_CHARGE_LOW;

			g_chargeflag = 0;

			if(g_chargeShadow>0)
			{
				lowbatCnt++;
			}else if(lowbatCnt>0)
			{
				lowbatCnt = 0;
			}
		}
		adSum = 0;
		
	}
	
/*
	if((RSV_VALID(ADC1->DR)>VCK_LEVEL)&&(RSV_VALID(vChk)>VCK_LEVEL))
	{
		if((RSV_VALID(ADC1->DR)>VFUL_LEVEL)&&(RSV_VALID(vChk)>VFUL_LEVEL))
		{
			g_chargeOk = AD_CHARGE_FUL;
		}else
		{
			g_chargeOk = AD_CHARGE_RDY;
		}
		//CHARGE_LED2_ON;
		CR_READY = 1;
		//g_chargeOk = AD_CHARGE_RDY;
		if(!g_chargeflag)
		{
			BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
			g_chargeflag = 1;
		}
		lowbatCnt = 0;
		//BUZZER_ON(BUZZER_TIME_NORMAL,1);
	}//else if((RSV_VALID(ADC1->DR)<VCK_LEVEL)&&(RSV_VALID(vChk)<VCK_LEVEL))
	else if((RSV_VALID(ADC1->DR)<VCK_LOW)&&(RSV_VALID(vChk)<VCK_LOW))
	{
#ifdef EXT_TEST_PRT
		CHARGE_LED2_ON;
		CR_READY = 1;
		g_chargeOk = AD_CHARGE_RDY;
#else
		//CHARGE_LED2_OFF;
		CR_READY = 0;
		g_chargeOk = AD_CHARGE_LOW;
#endif
		//flag = 0;
		g_chargeflag = 0;
		lowbatCnt++;
	}
	*/

	TurnOnChargeLed(0);

//#ifndef DSP_TEST
#if ((!defined RGB_TEST)&&(!defined DSP_TEST))
	if(lowbatCnt>LOW_BAT_TIMEOUT)
	{
		LoadLowBatIcon();
		//OSTimeDlyHMSM(0,0,3,0);
		OS_ENTER_CRITICAL();
		delay_seconds(3);
		BK_TURNOFF(0);
		KEY_BK_OFF();
		
		LCD_Clear(0);			//2015/11/27
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		//LowPow_Init(0);
		delay_seconds(3);
	}
#endif
	//vChk = ADC1->DR;

#if 0
	adTmp = ADC1->DR;

	vChk += (adTmp>>4);
	
	if(++cIndex>=AD_VCHK_TIMS)
	{
		cIndex = 0;
		//if((vChk>>4)>VCK_LEVEL)
		if(RSV_VALID(vChk)>VCK_LEVEL)
		{
			CHARGE_LED2_ON;
			CR_READY = 1;
			g_chargeOk = AD_CHARGE_RDY;
		}else
		{
			CHARGE_LED2_OFF;
			CR_READY = 0;
			g_chargeOk = AD_CHARGE_LOW;
		}
		vChk = 0;
	}
#endif
}


void TaskKeyScan(void *pdata)
{
	//u16 tsCnt=0;
	
	while(1)
	{
		if(g_keyAdcEnable>0)
		{
			KeyProcess();	
		}
#if 1
		AfAutoCtrl();			//Times Sensitive
		//if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			//ChkVoltage();
		}
		//ChkVoltage();
#endif
		DspUpdate();
		//TaskDspUpdate();
		//COM_AUX = !COM_AUX;
/*********************************************
		for test
**********************************************/
/*
		if(tsCnt++>20)
		{
			tsCnt = 0;
			FLAOK_TOGGLE();
		//	CHARGE_LED1=!CHARGE_LED1;
		}
*/
/*********************************************
		end
**********************************************/
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
	//g_keyDir = 1;
	//Ex_NVIC_Disable(10);
	//DISABLE_KEY_INT();
	//CLEAR_KEYINT();
	//EXTI->PR=1<<10;  //清除LINE0上的中断标志位  
	//if(COM_STATUE_SLEEP==g_comStatue)
	//{
	//	g_comStatue = COM_STATUE_IDLE;
	//}
#ifdef SLAVE_POWER_DOWN
	if(COM_STATUE_SLEEP==g_comStatue)
	{
		 Ex_NVIC_Disable(1);				//2015/11/04
		g_comStatue = COM_STATUE_IDLE;
	}					//2015/10/31 for slave sleep
#endif
	EXTI->PR=1<<1;  //休眠后关机
}




////////////////////////////////////////////////////////

 
