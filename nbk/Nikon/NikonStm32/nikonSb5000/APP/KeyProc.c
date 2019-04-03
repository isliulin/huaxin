#include "Nikon.h"
#include "includes.h"
#include "sys.h"
#include "keyproc.h"
#include "menu.h"
#include "..\hardware\lcd\Font.h"
#include "..\hardware\spi\spi.h"

unsigned char g_keyDir=0;
u32 gWSleepCounter=0;
FuncList *pMenuList=NULL;
char g_chargeOk=0;
char g_mstFlaEnable = 0;
char g_mstEnFlag = 0;
u8 g_chargeflag;
extern u8 g_chargeShadow;
char g_SettingIndex=0;
char g_menuLevel = 0;
//char g_mP1Index = 0;
//char g_mP2Index = 0;
//char g_mP3Index = 0;
s8 g_mpIndex[3] = {0};
char g_mstSetRsv = 0;
char g_mstSetting = 0;
u8 g_shadowKey=0;
extern BkParas bkParas;
extern FuncMenu funcMenu;
u8 g_stbyStatue = 0;
//u8 g_slaveStart = 0;
unsigned int g_bzDuration=0;
u8 g_rptPilotFlag = 0;
u8 g_modeRsv[3] = {0,WORKINGMODE_AUTO,WORKINGMODE_AUTO};

extern signed char *pSnrSet;
extern char g_bzCount;
extern char g_keyAdcEnable;
extern MotoSet motoSet;
extern u8 g_isReflectPulled;
extern Wl_set wlSet;
extern u8 g_isSlaveVoice;
extern u8 g_buzzerOn;
extern u8 g_slaveBzEn;
extern u8 g_slaveBzCnt;
//extern char const focLenScape[11];

#define ADCHAN1_KEY_INVALID  24//0xf0
#define ADCHAN2_KEY_INVALID  0x1f0
#define RKEY_INVALID	0x00
#define RKEY_CH_1		0x10
#define RKEY_CH_2		0x02

#define SNRMENU_VOICE 0

void DspUpdate(void);
void ResetKeyInt(void);
void BUZZER_ON(u16 duration,u16 times);
void Pilot(void);
extern void CmpSubAdd(u8 dir,u8 isCircle);

void BUZZER_ON(u16 duration,u16 times)
{
//#ifdef _ST_EX_
#if OS_TMR_EN > 0
	INT8U tmrErr;
	//if(pSnrSet[SNRMENU_VOICE])
	if(g_isSlaveVoice>0)
	{
		g_bzCount=times;
		//PWM_Init(2500,9,1250);			//
		//PWM_Init(2500,9,1250);
		if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			StopCapture();
		}
		PWM_Init(288,72,144);
		OSTmrReload(g_bzTmr,duration*100);
		OSTmrStart(g_bzTmr, &tmrErr);
	}
#endif
//#endif
}

void BzOff(void)
{
//#ifdef _ST_EX_
	static u16 du=0, ts=0;
//	INT8U tmrErr;
	du++;
	if(du>=g_bzDuration)
	{
		du = 0;
		ts++;
		if(ts>=g_bzCount)
		{
			//TIM3->CCMR1&=0xFF8F;
			TIM3->CCMR1&=0x8FFF;
			TIM3->CCMR1|=0x4000;
			TIM3->CCER&=~(1<<12);
			//TIM3->CR1&=0xFE; 
			//OSTmrStop(g_bzTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);	
			ts = 0;
			
			if(CMD_MODE_LIGH_SLA==*g_cmdMode)			//2015/11/16
			{
				StopCapture();
				TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
				//g_slaveStart = 1;
			}
		}else if(ts&1)
		{
			TIM3->CCMR1&=0x8FFF;
			TIM3->CCMR1|=0x4000;
			TIM3->CCER&=~(1<<12);
			//TIM3->CR1&=0xFE; 				//stop a cycle
			//TIM3->CCER&=~(1<<0);
		}else
		{
			//PWM_Init(2500,9,1250);
			PWM_Init(288,72,144);
			//OSTmrStart(g_bzTmr, &tmrErr) ;	//restart
		}
	}
	
//#endif
}


void SlaveBuzzer(void)
{
	if(g_buzzerOn>0)
	{
		g_buzzerOn = 0;
		g_slaveBzEn = 1;//BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
		g_slaveBzCnt = 6;
		//BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
	}
}

u16 GetKeyAdc(void)
{
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_KEY;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&(1<<1)));
	//delay_us(50);
	return ADC1->DR>>4;
}

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

//#define KEY_DEBUG
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
	//if(g_comStatue==COM_STATUE_STBY)
	if(g_stbyStatue>0)
	{
	/*
		if(g_keyDir!=0)
		{
			g_keyDir = 0;
			g_shadowKey = 0;
			ResetKeyInt();	
			return;
		}
	*/
		key = ReadPilot();
		if(KEY_PILOT==key)
		{
			//g_comStatue = COM_STATUE_IDLE;
			g_stbyStatue = 0;
			SetChargeEnable();
			gWSleepCounter = 0;
			ENABLE_KEY_INT();
			//StartCamera();
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//return RKEY_INVALID;
			return KEY_WAKE;
		}else
		{
			//return RKEY_INVALID;
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				if((!KEY_NIKMOD_SLV)||(!KEY_NIKMOD_MST))
				{
					HaltSec(20);
					if((!KEY_NIKMOD_SLV)||(!KEY_NIKMOD_MST))
					{
						g_stbyStatue = 0;
					       SetChargeEnable();
						ENABLE_KEY_INT();
						gWSleepCounter = 0;
						//StartCamera();
						g_SettingIndex = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						return KEY_WAKE;
					}
				}
			}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
			{
				if((!KEY_NIKMOD_SLV)||(!KEY_NIKMOD_NOR))
				{
					HaltSec(20);
					if((!KEY_NIKMOD_SLV)||(!KEY_NIKMOD_NOR))
					{
						g_stbyStatue = 0;
					       SetChargeEnable();
						ENABLE_KEY_INT();
						gWSleepCounter = 0;
						//StartCamera();
						g_SettingIndex = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						return KEY_WAKE;
					}
				}
			}

			return RKEY_INVALID;
			//return KEY_WAKE;
		}
	}
#if 1
	if(!KEY_NIKMOD_NOR)
	{
		HaltSec(20);
		if(!KEY_NIKMOD_NOR)
		{
			if(CMD_MODE_OFF!=*g_cmdMode)
			{
/****************************************************
//if in stand by mode, wake up
****************************************************/
//				g_stbyStatue = 0;
//				SetChargeEnable();
//				gWSleepCounter = 0;
////////////////////////////////////////////////////end
				g_modeRsv[*g_cmdMode] = mstPara->mode;
				*g_cmdMode = CMD_MODE_OFF;
				mstPara->mode = g_modeRsv[*g_cmdMode];
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//return RKEY_INVALID;
				return KEY_WAKE;
			}
		}
	}else if(!KEY_NIKMOD_SLV)
	{
		HaltSec(20);
		if(!KEY_NIKMOD_SLV)
		{
#if 1
			if(CMD_MODE_LIGH_SLA!=*g_cmdMode)
			{
/****************************************************
//if in stand by mode, wake up
****************************************************/
//				g_stbyStatue = 0;
//				SetChargeEnable();
//				gWSleepCounter = 0;
////////////////////////////////////////////////////end
				g_modeRsv[*g_cmdMode] = mstPara->mode;
				*g_cmdMode = CMD_MODE_LIGH_SLA;
				mstPara->mode = g_modeRsv[*g_cmdMode];
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//return RKEY_INVALID;
				return KEY_WAKE;
			}
#else
			if(CMD_MODE_LIGH_MST!=*g_cmdMode)
			{
				*g_cmdMode = CMD_MODE_LIGH_MST;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return RKEY_INVALID;
			}
#endif
		}
	}else if(!KEY_NIKMOD_MST)
	{
		HaltSec(20);
		if(!KEY_NIKMOD_MST)
		{
			if(CMD_MODE_LIGH_MST!=*g_cmdMode)
			{
/****************************************************
//if in stand by mode, wake up
****************************************************/
//				g_stbyStatue = 0;
//				SetChargeEnable();
//				gWSleepCounter = 0;
////////////////////////////////////////////////////end
				g_modeRsv[*g_cmdMode] = mstPara->mode;
				*g_cmdMode = CMD_MODE_LIGH_MST;
				mstPara->mode = g_modeRsv[*g_cmdMode];
				g_SettingIndex = 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//return RKEY_INVALID;
				return KEY_WAKE;
			}
		}
	}
#endif 
	
	//if((g_key1>ADCHAN1_KEY_INVALID)&&(g_key2>ADCHAN2_KEY_INVALID))//no press key
	key = GetKeyAdc();
	//DBG_PRINTF("%x ",key);
	//HaltSec(100);
	//return;
#ifdef KEY_DEBUG
	if(key>0)
	{
		DBG_PRINTF("%d ",key);
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

		if(KEY_DIR_LEFT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_LEFT;
		}else if(KEY_DIR_RIGHT==g_keyDir)
		{
			g_keyDir = 0;
			return KEY_RIGHT;
		}else if(KEY_ERROR==g_keyDir)
		{
			if((KEY_DIR_1>0)&&(KEY_DIR_2>0))
			{
				HaltSec(10);
				g_keyDir = 0;
				g_shadowKey = 0;
				ResetKeyInt();
			}
		}
		
	
	}else
	{
		//DBG_PRINTF("%x, ",key);
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
				//DBG_PRINTF("Key err, %x, %x\n",key, vKey);
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
		keyIndex = KEY_FUNC1;
		/*
		if(key<169)
		{
			keyIndex = KEY_CLEAR;
		}else
		{
			keyIndex = KEY_FUNC1;
		}*/
	}else if((key<KEY3_VALUE_MAX)&&(key>KEY3_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC3;
		keyIndex = KEY_FUNC2;
	}
#ifdef NIKON_STYLE
	else if((key<KEY4_VALUE_MAX)&&(key>KEY4_VALUE_MIN))
	{
		keyIndex = KEY_FUNC3;
	}
	else if((key<KEY5_VALUE_MAX)&&(key>KEY5_VALUE_MIN))
	{
		keyIndex = KEY_OK;
	}
	else if((key<KEY6_VALUE_MAX)&&(key>KEY6_VALUE_MIN))
	{
		keyIndex = KEY_MENU;
	}else if((key<KEY7_VALUE_MAX)&&(key>KEY7_VALUE_MIN))
	{
		keyIndex = KEY_LOCK;
	}else if((key<KEY8_VALUE_MAX)&&(key>KEY8_VALUE_MIN))
	{
		keyIndex = KEY_CLEAR;
	}
#else
	else if((key<KEY4_VALUE_MAX)&&(key>KEY4_VALUE_MIN))
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
#endif

	//if((KEY_RF_SW<=keyIndex)&&(keyIndex<=KEY_CLEAR))
	if((KEY_RF_SW<=keyIndex)&&(keyIndex<=KEY_MENU))
	{
		//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
		keytimedata++;
		if(!short_once)
		{
			short_once=1;
			keyIndexRsv = keyIndex;
			return (keyIndex|0x20);
			//return RKEY_INVALID;
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

void BkLiteOn(void)
{
	INT8U tmrErr;

//	if(!pSnrSet[SNR_BKLIGHT])
	if (1)
	{
		//BK_GREEN();
		KEY_BK_ON();
		BK_GREEN(g_isHotProtect);
		bkParas.bkState= BK_ON;
		bkParas.bkFlag = BK_ON;
		SetBkLightInfo(1);
		//macData2[0] |= 0x02;
		//macData2[COM_DAT_REAR] = VerCodeCal(macData2);
#if OS_TMR_EN > 0
		//BK_ORANGE();
		//BK_GREEN();
		//OSTmrStart(g_bkTmr, &tmrErr);
#endif
	}else
	{
	
		//macData2[0] &= ~0x02;
		//macData2[COM_DAT_REAR] = VerCodeCal(macData2);
		SetBkLightInfo(0);
		KEY_BK_OFF();
		BK_TURNOFF(g_isHotProtect);
	}

#if 0

	if(cmrPara.cls>0)
	{
		macData2[COM_DAT_STATUE]|=0x01;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}
#endif
	

#if OS_TMR_EN > 0
	//BK_ORANGE();
	//BK_GREEN(g_isHotProtect);
	OSTmrStart(g_bkTmr, &tmrErr);
#endif
}

void ResetKeyInt(void)
{
	Ex_NVIC_Enable(10);
}

void ModeSw(void)
{
	if((SETTING_PFNMENU==g_SettingIndex)||(SETTING_SNRSUB==g_SettingIndex))
	{
		return;
	}
	if(CMD_MODE_OFF==*g_cmdMode)
	{
		if(g_SettingIndex!=SETTING_MODE)
		{
			g_SettingIndex = SETTING_MODE;
		}else
		{
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				mstPara->mode++;
				if(mstPara->mode>WORKINGMODE_RPT)
				{
					mstPara->mode = WORKINGMODE_TTL;
				}

				if(WORKINGMODE_TTL_POOR==mstPara->mode)
				{
					if(!cmrPara.cls)
					{
						mstPara->mode = WORKINGMODE_AUTO;
					}else if(!(cmrPara.bkMod&0x04))
					{
						mstPara->mode = WORKINGMODE_AUTO;
					}
				}
				
			}
		}
	}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		//if((g_SettingIndex>=SETTING_MST_M)&&(g_SettingIndex<=SETTING_MST_C))
/*
		if(pSnrSet[SNR_SU4]>0)
		{
			if(SETTING_MODE==g_SettingIndex)
			{
				//mstPara->mst_su4_mode++;
				//if(mstPara->mst_su4_mode>MST_SU4_MANU)
				//{
				//	mstPara->mst_su4_mode = MST_SU4_AUTO;
				//}
				mstPara->mode++;
				if(mstPara->mode>WORKINGMODE_MANU)
				{
					mstPara->mode = WORKINGMODE_AUTO;
				}
			}else
			{
				g_SettingIndex = SETTING_MODE;
			}
		}else
		{
			if((g_SettingIndex<SETTING_MST_M)||(g_SettingIndex>SETTING_MST_C))
			{
				return;
			}

			if(MST_SET_MODE==g_mstSetting)
			{
				//mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode++;
				switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
				{
					case WORKINGMODE_TTL:
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_AUTO;
					break;

					case WORKINGMODE_AUTO:
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_MANU;
					break;

					case WORKINGMODE_MANU:
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_OFF;
					break;

					case WORKINGMODE_OFF:
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_TTL;
					break;
				}
			}else
			{
				g_mstSetting = MST_SET_MODE;
			}
		}
*/
		//SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
/*
		if(pSnrSet[SNR_SU4]>0)
		{
			if(SETTING_MODE==g_SettingIndex)
			{
				//mstPara->rmt_su4_mode++;
				//if(mstPara->rmt_su4_mode>RMT_SU4_AUTO)
				//{
				//	mstPara->rmt_su4_mode = RMT_SU4_M;
				//}
				switch(mstPara->mode)
				{
					case WORKINGMODE_AUTO:
						mstPara->mode = WORKINGMODE_MANU;
					break;

					case WORKINGMODE_MANU:
						mstPara->mode = WORKINGMODE_OFF;
					break;

					case WORKINGMODE_OFF:
						mstPara->mode = WORKINGMODE_AUTO;
					break;

					default:
						mstPara->mode = WORKINGMODE_AUTO;
					break;
					
				}
			}else
			{
				g_SettingIndex = SETTING_MODE;
			}
		}else
		{
			if(g_SettingIndex>0)
			{
				g_SettingIndex= 0;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}
		*/
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void FuncModuleSw(void)
{
	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		//DBG_PRINTF("FuncModuleSw\r\n");
		//TmCaptureInit(1000,72);	
		LIGHT_SLAVE_ENABLE();
		delay_ms(5);
		StopCapture();
		TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
		//g_slaveStart = 1;
		//ClearWlcBuf();
		//LIGHT_SLAVE_ENABLE();
		LightSlaveParaClear();
	}else
	{
		LIGHT_SLAVE_DISABLE();
		StopCapture();
	}
	//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	//BkLiteOn();
}

void DspRfOnlyScr(void)
{
	DspClearAllPara();
	DisplayPicture(4, 4, 32, 32, pic_mod_rfOnly, DSP_NORMAL);
	DisplayPicture(5, 4+30+10, 40, 16, pic_mod_afill, DSP_NORMAL);
	DisplayPicture(5, 4+30+10+40+8, 30, 16, pic_mod_Only, DSP_NORMAL);

	BuildRfOnlyMenu();

	DspTempWarning();//in oder to avioding being coverd
/*
	DspLightBody((!pSnrSet[SNR_AF])&&(*g_cmdMode!=CMD_MODE_LIGH_SLA),!pSnrSet[SNR_BKLIGHT]);
*/
	
}

void DspZoomStatue(void)
{
/*
	if(g_isReflectPulled>0)
	{
		if(!pSnrSet[SNR_WP])
		{
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,ZOOM_FRAME_SETA,SETTING_MOTO==g_SettingIndex);
			DisplayFocLen(*motoSet.mode,motoSet.dspLocation,ZOOM_FRAME_SETA,SETTING_MOTO==g_SettingIndex);
		}else
		{
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,ZOOM_FRAME_FIXED,SETTING_MOTO==g_SettingIndex);
			DisplayFocLen(*motoSet.mode,motoSet.dspLocation,ZOOM_FRAME_FIXED,SETTING_MOTO==g_SettingIndex);
		}
	}else
	{
		if((cmrPara.cls>0)&&(MOTO_MODE_AUTO==*motoSet.mode))
		{
			DisplayFocLen(*motoSet.mode,cmrPara.focLen,ZOOM_FRAME_NONE,SETTING_MOTO==g_SettingIndex);
			//motoSet.destRsv = GetMotoDest(cmrPara.focLen);
			motoSet.destRsv = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
		}else
		{
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,ZOOM_FRAME_NONE,SETTING_MOTO==g_SettingIndex);
			DisplayFocLen(*motoSet.mode,motoSet.dspLocation,ZOOM_FRAME_NONE,SETTING_MOTO==g_SettingIndex);
			motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
		}
	}
	*/
}

void Su4ModCheck(void)
{
	//if((CMD_MODE_LIGH_MST==*g_cmdMode)&&(pSnrSet[SNR_SU4]>0))
/*
	if(pSnrSet[SNR_SU4]>0)
	{
		if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			if((mstPara->mode<WORKINGMODE_AUTO)||(mstPara->mode>WORKINGMODE_MANU))
			{
				mstPara->mode = WORKINGMODE_AUTO;
			}
		}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			//if((mstPara->mode<WORKINGMODE_AUTO)||(mstPara->mode>WORKINGMODE_MANU))
			if((WORKINGMODE_AUTO!=mstPara->mode)&&(WORKINGMODE_MANU!=mstPara->mode)&&(WORKINGMODE_OFF!=mstPara->mode))
			{
				mstPara->mode = WORKINGMODE_AUTO;
			}
		}
	}
*/
}

void Mode(void)
{
	//LCD_Clear(BACK_COLOR);
	SetSpiComData();

	Su4ModCheck();
	
	DspClearAllPara();

	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			//DspClearAllPara();
			ModPasteScr(mstPara->mode,0);
			if(cmrPara.cls>0)
			{
				//if((WORKINGMODE_AUTO==mstPara->mode)&&(pSnrSet[SNR_APERT]>1))
				if(WORKINGMODE_AUTO==mstPara->mode)
				{
					DspApertValue(mainPara.extApert,SETTING_EXTM_APERT==g_SettingIndex);
				}else
				{
					DspApertValue(cmrPara.apert,DSP_NORMAL);
				}

				if(cmrPara.isAutoIso>0)
				{
					DspIsoAuto();
				}else
				{
					DspIsoValue(cmrPara.iso,DSP_NORMAL);
				}
			}else
			{
				DspApertValue(mainPara.extApert,SETTING_EXTM_APERT==g_SettingIndex);
				DspIsoValue(mainPara.extIso,DSP_NORMAL);
			}
			//SPIx_Enable();
		break;

		case CMD_MODE_LIGH_MST:
			LightMasterScr();
			if(cmrPara.cls>0)
			{
				DspApertValue(cmrPara.apert,DSP_NORMAL);
			}else
			{
				DspApertValue(mainPara.extApert,SETTING_EXTM_APERT==g_SettingIndex);
			}/*
			if(pSnrSet[SNR_SU4]>0)
			{
				if(cmrPara.cls>0)
				{
					if(cmrPara.isAutoIso>0)
					{
						DspIsoAuto();
					}else
					{
						DspIsoValue(cmrPara.iso,DSP_NORMAL);
					}
				}else
				{
					DspIsoValue(mainPara.extIso,DSP_NORMAL);
				}
			}else
			{
				DspMstChan(mainPara.m_chan,SETTING_MST_CHAN==g_SettingIndex);
			}*/
			//SPIx_Enable();
		break;

		case CMD_MODE_LIGH_SLA:
			cmrPara.cls = 0;
			LightSlaveScr();
			//SPIx_Disable();
		break;
	}

	FuncModuleSw();

	TakeOnCharge(*g_cmdMode);

	DspZoomStatue();

	DspFunctions();

	CmrLedCtrl();

	if(SETTING_MOTO==g_SettingIndex)
	{
		LoadMotoSetMenu();
	}else
	{
		LoadButtons();
	}

}

void ModeClear(void)
{
	mstPara->mode = WORKINGMODE_TTL;
	mstPara->cmp1 = 0;
	mstPara->cmp2 = 0;
	//*motoSet.dest_location = 24;
	motoSet.dspLocation= 24;
	motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
	g_flashMode = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void ExposeAddSub(u8 dir)
{
	switch(mstPara->mode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_AUTO:
		case WORKINGMODE_GN:
		case WORKINGMODE_TTL_POOR:
			CmpSubAdd(dir,0);
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case WORKINGMODE_MANU:
			if(KEY_DIR_LEFT==dir)
			{
				if((mstPara->output1<=0)&&(mstPara->output2<=0))
				{
					mstPara->output1 = 7;
					mstPara->output2 = 0;
				}else
				{
					//if((pSnrSet[SNR_STEP]>0)&&(mstPara->output1==7))
					if(mstPara->output1==7)
					{
						mstPara->output1--;
						mstPara->output2 = 0;
					}else
					{
						mstPara->output2--;
						if(mstPara->output2<-2)
						{
							mstPara->output1--;
							mstPara->output2+=3;
						}
					}
				}
			}else
			{
				//if((pSnrSet[SNR_STEP]>0)&&(mstPara->output1==6)&&(mstPara->output2>=0))
				if((mstPara->output1==6)&&(mstPara->output2>=0))
				{
					mstPara->output1 = 7;
					mstPara->output2 = 0;
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
							mstPara->output2-=3;
						}
					}
				}
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//Mode();
		break;

		case WORKINGMODE_RPT:
			if(KEY_DIR_LEFT==dir)
			{
				if((mstPara->multi_exp1<=0)&&(mstPara->multi_exp2<=0))
				{
					mstPara ->multi_exp1 = 4;
					mstPara ->multi_exp2 = 0;
				}else
				{
					mstPara->multi_exp2--;
					if(mstPara->multi_exp2<-2)
					{
						mstPara->multi_exp1--;
						mstPara->multi_exp2+=3;
						if(mstPara->multi_exp1<0)
						{
							mstPara->multi_exp1 = 4;
							mstPara ->multi_exp2 = 0;
						}
					}
				}
			}else
			{
				if((4==mstPara->multi_exp1)&&(0==mstPara->multi_exp2))
				{
					mstPara->multi_exp1 = 0;
					mstPara->multi_exp2 = 0;
				}else
				{
					mstPara->multi_exp2++;
					if(mstPara->multi_exp2>2)
					{
						mstPara->multi_exp1++;
						mstPara->multi_exp2-=3;
					}
				}
			}
			MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//Mode();
		break;
	}
}

void SnrAllMenuSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		switch(g_menuLevel)
		{
			case 0:
				g_snrPageIndex --;
				if (g_snrPageIndex<0)
				{
					g_snrPageIndex = 2;
				}
			break;

			case 1:
				g_mpIndex[g_snrPageIndex]--;
				if(g_mpIndex[g_snrPageIndex]<0)
				{
					g_mpIndex[g_snrPageIndex] = g_snrPageCnt[g_snrPageIndex];
				}
			break;

			case 2:
			break;
		}
		
	}else
	{
		switch(g_menuLevel)
		{
			case 0:
				g_snrPageIndex ++;
				if (g_snrPageIndex>2)
				{
					g_snrPageIndex = 0;
				}
			break;

			case 1:
				g_mpIndex[g_snrPageIndex]++;
				if(g_mpIndex[g_snrPageIndex]>g_snrPageCnt[g_snrPageIndex])
				{
					g_mpIndex[g_snrPageIndex] = 0;
				}
			break;

			case 2:
			break;
		}
	}
	DBG_PRINTF("id %d, start %d\r\n",g_snrIndex,g_snrStart);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	//DspSnrMenu();
	//DspSnrMenu(g_allMyMenuSw);
}

void ShiftMyMenubits(s8* sft, u8 dir,u8 cnt)
{
#if 0
	s8 bitNow=*sft;
	u8 i = 0;
	u8 tmp = 0;

	for(i=0;i<19;i++)
	{
//		DBG_PRINTF("%d ",(g_snrMyMenuBits&(1<<i))>0);
	}

//	DBG_PRINTF("\r\n");

	while(1)
	{
		if(KEY_DIR_LEFT==dir)
		{
			bitNow--;
			
			if(bitNow<0)
			{
				bitNow = SNR_MENU_CNT-1;
			}

//			DBG_PRINTF("bitNow %d\r\n",bitNow);
			
		}else
		{
			bitNow++;
			
			if(bitNow>=SNR_MENU_CNT)
			{
				bitNow = 0;
			}

//			DBG_PRINTF("bitNow %d\r\n",bitNow);
		}

//		DBG_PRINTF("%d\r\n",g_snrMyMenuBits&(1<<bitNow));

		if(g_snrMyMenuBits&(1<<bitNow))
		{
			tmp++;
//			DBG_PRINTF("tmp=%d, cnt=%d\r\n",tmp,cnt);
			if(tmp>=cnt)
			{
				*sft = bitNow;
				return;
			}
			
		}

		
	}

/*
	if(KEY_DIR_LEFT==dir)
	{
		while(1)
		{
			bitNow--;
			
			if(bitNow<=0)
			{
				bitNow = SNR_MENU_CNT-1;
			}
			
			if(g_snrMyMenuBits&(1<<bitNow))
			{
				i++;
				if(i>=cnt)
				{
					g_snrIndex = bitNow;
					return;
				}
				
			}
		}
		
	}else
	{
		//while(bitNow<SNR_MENU_CNT)
		while(1)
		{
			bitNow++;
			if(bitNow>=SNR_MENU_CNT)
			{
				bitNow = 0;
			}
			if(g_snrMyMenuBits&(1<<bitNow))
			{
				i++;
				if(i>=cnt)
				{
					g_snrIndex = bitNow;
					return;
				}				
			}
		}
	}
*/
#endif
}

u8 GetStartNowDst(void)
{
	u8 i = 0;
	u8 total = 0;
	for(i=g_snrStart;i<g_snrIndex;i++)
	{
		if(g_snrMyMenuBits&(1<<(i+1)))
		{
			total++;
		}
	}

	return total;

}

u8 GeMyMenuDspIndex(u8 num)
{
	u8 i = 0;
	u8 total = 0;
	for(i=0;i<=num;i++)
	{
		if(g_snrMyMenuBits&(1<<(i+1)))
		{
			total++;
		}
	}

	return total;
}

void GetMyMenuNext(u8 dir)
{
//	u8 i = 0;
	//s8 bitNow=g_snrIndex;

	if(KEY_DIR_LEFT==dir)
	{
		ShiftMyMenubits(&g_snrIndex,KEY_DIR_LEFT,1);
		
		if(g_snrIndex<g_snrStart)
		{
			ShiftMyMenubits(&g_snrStart,KEY_DIR_LEFT,4);
		}//else if((g_snrIndex-g_snrStart)>3)
		if(GetStartNowDst()>3)
		{
			//while((g_snrIndex-g_snrStart)>3)
			while(GetStartNowDst()>3)
			{
				ShiftMyMenubits(&g_snrStart,KEY_DIR_RIGHT,4);
			}
		}

		//DBG_PRINTF("key left: %d, %d\r\n",g_snrIndex,g_snrStart);
		
	}else
	{
		ShiftMyMenubits(&g_snrIndex,KEY_DIR_RIGHT,1);
		//DBG_PRINTF("key right: %d, %d\r\n",g_snrIndex,g_snrStart);
		//if(g_snrIndex>(g_snrStart+3))
		if(GetStartNowDst()>3)
		{
			ShiftMyMenubits(&g_snrStart,KEY_DIR_RIGHT,4);
		}else if(g_snrIndex<g_snrStart)
		{
			g_snrStart = 0;
		}
	}

//	pSnrSet[g_snrIndex] = g_snrSubIndex;                            // set the parameter indirectly
/*
	for(i=0;i<19;i++)
	{
		DBG_PRINTF("%d ",(g_snrMyMenuBits&(1<<i))>0);
	}

	DBG_PRINTF("\r\n");

	DBG_PRINTF("start :%d, index %d\r\n",g_snrStart,g_snrIndex);
*/
	
}

void SnrMyMenuSubAdd(u8 dir)
{
	GetMyMenuNext(dir); 
	DspSnrMenu();
}

void SnrMenuSubAdd(u8 dir)
{
	SnrAllMenuSubAdd(dir);
}

void ApertAddSub(u8 dir)
{
	u8 i=0;

	i = WordSearch(apertValue, EXT_ISO_LEVELS, mainPara.extApert);

	
	if(KEY_DIR_LEFT==dir)
	{
		if(i>0)
		{
			i--;
		}else
		{
			i = EXT_ISO_LEVELS-1;
		}
	}else
	{
		//if((i<33)&&(i>0))
		if(i<EXT_ISO_LEVELS-1)
		{
			i++;
		}else
		{
			i = 0;
		}
	}

	mainPara.extApert = apertValue[i];
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void SnrSubAddSub(u8 dir)
{
/*
	if(motoSet.moving>0)
	{
		return;		
	}
	
	if(KEY_DIR_LEFT==dir)
	{
		g_snrSubIndex--;
		if(g_snrSubIndex<0)
		{
			if(g_subMenuIsLoop[g_snrIndex]>0)
			{
				g_snrSubIndex = g_subMenuCnt[g_snrIndex]-1;//g_snrMenu[g_snrIndex].subMenu->cnt - 1;
			}else
			{
				g_snrSubIndex = 0;
			}
		}
	}else
	{
		g_snrSubIndex++;
		if(g_snrSubIndex > (g_subMenuCnt[g_snrIndex]-1))//g_snrMenu[g_snrIndex].subMenu->cnt)
		{
			if(g_subMenuIsLoop[g_snrIndex]>0)
			{
				g_snrSubIndex = 0;
			}else
			{
				g_snrSubIndex = g_subMenuCnt[g_snrIndex]-1;
			}
		}
	}
	SubSnrClear();
	//if(g_snrMenu[g_snrIndex].subMenu->subDsp!=NULL)
	{
		//g_snrMenu[g_snrIndex].subMenu->subDsp(g_snrSubIndex,pSnrSet[g_snrIndex],DSP_REVERSE);
		DspSubMenu(g_snrSubIndex,DSP_REVERSE);
		SubMenuHandle();
		//SubMenuSetProc();
	}
	//BuildSnrMenu();	
	BuildSnrSubMenu();
*/
}

void SnrMyMenuAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		g_snrMyMenuSelIndex--;
		if(g_snrMyMenuSelIndex<0)
		{
			g_snrMyMenuSelIndex = 15;
			g_snrMyMenuSelStart = 12;
		}
		if(g_snrMyMenuSelIndex<g_snrMyMenuSelStart)
		{
			g_snrMyMenuSelStart -= 4;
		}
	}else
	{
		g_snrMyMenuSelIndex++;
		if(g_snrMyMenuSelIndex>15)
		{
			g_snrMyMenuSelIndex = 0;
			g_snrMyMenuSelStart = 0;
		}
		if(g_snrMyMenuSelIndex>g_snrMyMenuSelStart+3)
		{
			g_snrMyMenuSelStart += 4;
		}

	}

	DspMyMenuSelect();
	
}

void GN_MSubAdd(u8 dir)
{
	u8 i=0;
	i = CodeSearch((char*)g_GnScale, 34, mstPara->gn_m);
	
	if(KEY_DIR_LEFT==dir)
	{
		if(i>0)
		{
			i--;
		}
	}else
	{
		if(i<33)
		{
			i++;
		}
		
	}

	mstPara->gn_m = g_GnScale[i];

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void RptFrqShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
#ifdef _NIKON_
		if(mstPara->rpt_hz <= 10)
		{
			mstPara->rpt_hz --;
		}else if(mstPara->rpt_hz <= 100)
		{
			mstPara->rpt_hz -= 10;
		}
#endif
		if(mstPara->rpt_hz<1)
		{
			mstPara->rpt_hz = 1;
		}
	}else
	{
#ifdef _NIKON_
		if(mstPara->rpt_hz<10)
		{
			mstPara->rpt_hz++;
		}else 
		{
			mstPara->rpt_hz+=10;
		}
		if(mstPara->rpt_hz>100)
		{
			mstPara->rpt_hz = 100;
		}
#endif

	}

	MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}


void RptTimesShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
#ifdef _NIKON_
		if(mstPara->rpt_times<=10)
		{
			mstPara->rpt_times--;
		}else 
		{
			mstPara->rpt_times-=2;
		}

		if(mstPara->rpt_times<1)
		{
			mstPara->rpt_times = 1;
		}
#endif
	}else
	{
#ifdef _NIKON_
			if(mstPara->rpt_times<10)
			{
				mstPara->rpt_times++;
			}else
			{
				mstPara->rpt_times+=2;
			}

			if(mstPara->rpt_times>90)
			{
				mstPara->rpt_times = 90;
			}
#endif
	}

	MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void RmtGrAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		mstPara->rmt_gr--;
		if(mstPara->rmt_gr<0)
		{
			mstPara->rmt_gr = 2;
		}
	}else
	{
		mstPara->rmt_gr++;
		if(mstPara->rmt_gr>2)
		{
			mstPara->rmt_gr = 0;
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void RmtChAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		mstPara->rmt_ch--;
		if(mstPara->rmt_ch<0)
		{
			mstPara->rmt_ch = 3;
		}
	}else
	{
		mstPara->rmt_ch++;
		if(mstPara->rmt_ch>3)
		{
			mstPara->rmt_ch = 0;
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MstArrAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		g_SettingIndex--;
		if(g_SettingIndex<SETTING_MST_M)
		{
			g_SettingIndex = SETTING_MST_C;
		}
		
	}else
	{
		g_SettingIndex++;
		if(g_SettingIndex>SETTING_MST_C)
		{
			g_SettingIndex = SETTING_MST_M;
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MstRspArrModAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
		{
			case WORKINGMODE_TTL:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_OFF;
			break;

			case WORKINGMODE_AUTO:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_TTL;
			break;

			case WORKINGMODE_MANU:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_AUTO;
			break;

			case WORKINGMODE_OFF:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_MANU;
			break;
		}
	}else
	{
		switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
		{
			case WORKINGMODE_TTL:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_AUTO;
			break;

			case WORKINGMODE_AUTO:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_MANU;
			break;

			case WORKINGMODE_MANU:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_OFF;
			break;

			case WORKINGMODE_OFF:
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode = WORKINGMODE_TTL;
			break;
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MstRspArrParaAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
		{
			case WORKINGMODE_TTL:
				if((-3==mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1)&&(0==mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2))
				{
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1 = 3;
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2 = 0;
				}else
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2--;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2<0)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1--;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2+=3;
					}
				}
			break;

			case WORKINGMODE_AUTO:
				if((-3==mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1)&&(0==mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2))
				{
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1 = 3;
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2 = 0;
				}else
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2--;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2<0)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1--;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2+=3;
					}
				}
			break;

			case WORKINGMODE_MANU:
				if((mainPara.modGr[g_SettingIndex-SETTING_MST_M].output1<=0)&&(mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2<=0))
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].output1 = 7;
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2 = 0;
				}else
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2--;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2<-2)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].output1--;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2+=3;
					}
				}
			break;

			default:
			break;
		}
	}else
	{
		switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
		{
			case WORKINGMODE_TTL:
				if(3==mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1)
				{
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1 = -3;
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2 = 0;
				}else
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2++;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2>2)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1++;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2-=3;
					}
				}
			break;

			case WORKINGMODE_AUTO:
				if(3==mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1)
				{
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1 = -3;
					//mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2 = 0;
				}else
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2++;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2>2)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1++;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2-=3;
					}
				}
			break;

			case WORKINGMODE_MANU:
				if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].output1>=7)
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].output1 = 0;
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2 = 0;
				}else
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2++;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2>2)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].output1++;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].output2-=3;
					}
				}
			break;

			default:
			break;
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MstRptParaSetAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		switch(g_mstSetting)
		{
			case MSTRPT_SEL_EXP:
				if((mstPara->multi_exp1<=0)&&(mstPara->multi_exp2<=0))
				{
					mstPara ->multi_exp1 = 4;
					mstPara ->multi_exp2 = 0;
				}else
				{
					mstPara->multi_exp2--;
					if(mstPara->multi_exp2<-2)
					{
						mstPara->multi_exp1--;
						mstPara->multi_exp2+=3;
						if(mstPara->multi_exp1<0)
						{
							mstPara->multi_exp1 = 4;
						}
					}
				}
				
			break;

			case MSTRPT_SEL_TIMES:
				if(mstPara->rpt_times<=10)
				{
					mstPara->rpt_times--;
				}else 
				{
					mstPara->rpt_times-=2;
				}

				if(mstPara->rpt_times<1)
				{
					mstPara->rpt_times = 1;
				}
			break;

			case MSTRPT_SEL_FRQ:
				if(mstPara->rpt_hz <= 10)
				{
					mstPara->rpt_hz --;
				}else if(mstPara->rpt_hz <= 100)
				{
					mstPara->rpt_hz -= 10;
				}
				if(mstPara->rpt_hz<1)
				{
					mstPara->rpt_hz = 1;
				}
			break;

			case MSTRPT_SEL_M:
			case MSTRPT_SEL_A:
			case MSTRPT_SEL_B:
			case MSTRPT_SEL_C:
				if(mainPara.modGr[g_mstSetting-MSTRPT_SEL_M].rpt_isOn>0)
				{
					mainPara.modGr[g_mstSetting-MSTRPT_SEL_M].rpt_isOn = 0;
				}else
				{
					mainPara.modGr[g_mstSetting-MSTRPT_SEL_M].rpt_isOn = 1;
				}
			break;
		}
	}else
	{
		switch(g_mstSetting)
		{
			case MSTRPT_SEL_EXP:
				if(4<=mstPara->multi_exp1)
				{
					mstPara->multi_exp1 = 0;
					mstPara->multi_exp2 = 0;
				}else
				{
					mstPara->multi_exp2++;
					if(mstPara->multi_exp2>2)
					{
						mstPara->multi_exp1++;
						mstPara->multi_exp2-=3;
					}
				}
				MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);
			break;

			case MSTRPT_SEL_TIMES:
				if(mstPara->rpt_times<10)
				{
					mstPara->rpt_times++;
				}else
				{
					mstPara->rpt_times+=2;
				}

				if(mstPara->rpt_times>90)
				{
					mstPara->rpt_times = 90;
				}
				MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);
			break;

			case MSTRPT_SEL_FRQ:
				if(mstPara->rpt_hz<10)
				{
					mstPara->rpt_hz++;
				}else 
				{
					mstPara->rpt_hz+=10;
				}
				if(mstPara->rpt_hz>100)
				{
					mstPara->rpt_hz = 100;
				}
				MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);
			break;

			case MSTRPT_SEL_M:
			case MSTRPT_SEL_A:
			case MSTRPT_SEL_B:
			case MSTRPT_SEL_C:
				if(mainPara.modGr[g_mstSetting-MSTRPT_SEL_M].rpt_isOn>0)
				{
					mainPara.modGr[g_mstSetting-MSTRPT_SEL_M].rpt_isOn = 0;
				}else
				{
					mainPara.modGr[g_mstSetting-MSTRPT_SEL_M].rpt_isOn = 1;
				}
			break;
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MstChaAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		mainPara.m_chan--;
		if(mainPara.m_chan<0)
		{
			mainPara.m_chan = 3;
		}
	}else
	{
		mainPara.m_chan++;
		if(mainPara.m_chan>3)
		{
			mainPara.m_chan = 0;
		}
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);

}

u16 GetMotoDest(u16 srcLoc)
{
#if(defined(REFLECT_EN)&&(REFLECT_EN>0))
	u8 i = 0;
	i = GetMotoStdIndex(srcLoc);
	switch(g_flashMode)
	{
		case FLASH_MODE_NORMAL:
			if(i<1) 
			{
				i = 1;
			}
			return focLenScape[i-1];
		//break;

		case FLASH_MODE_REFLECT:
			if(i<2) 
			{
				i = 2;
			}
			return focLenScape[i-2];
		//break;

		case FLASH_MODE_PARALLEL:
			return focLenScape[i];
		//break;
	}
#else
	return srcLoc;
#endif
	return 0;
}

void MotoShift(u8 dir)
{
	u8 motoLevelMin = 0, motoLevelMax = 0;
	
	if(motoSet.moving != 0)return;

#if(defined(REFLECT_EN)&&(REFLECT_EN>0))

	switch(g_flashMode)
	{
		case FLASH_MODE_NORMAL:
			//motoLevelMin = 1;
			motoLevelMin = 2;
			//motoLevelMax = 11;
			motoLevelMax = MOTO_MAX_INDEX-1;
		break;

		case FLASH_MODE_PARALLEL:
			//motoLevelMin = 0;
			motoLevelMin = 2;
			//motoLevelMax = 9;
			motoLevelMax = MOTO_MAX_INDEX-3;
		break;

		case FLASH_MODE_REFLECT:
			//motoLevelMin = 2;
			motoLevelMin = 3;
			//motoLevelMax = 11;
			motoLevelMax = MOTO_MAX_INDEX-1;
		break;
	}
#else
	//motoLevelMin = 2;
	motoLevelMin = 0;
	//motoLevelMax = 11;
	motoLevelMax = MOTO_MAX_INDEX-2;
	
#endif
	
#ifdef MOTO_REFLECT_DETECT
	//if(REF_BOARD_DETECT) return;
	//if(g_isReflectPulled>0) return;
	//if((g_isReflectPulled>0)&&(pSnrSet[SNR_WP]>0)) return;
	if(g_isReflectPulled>0) return;
#endif

	if(MOTO_MODE_AUTO==*motoSet.mode)
	{
		*motoSet.mode = MOTO_MODE_MANU;
		 motoSet.focIndex = GetMotoStdIndex(motoSet.dspLocation);
	}

	DBG_PRINTF("moto id %d\r\n",motoSet.focIndex);

	if(KEY_DIR_LEFT==dir)
	{
		motoSet.focIndex--;

		if(motoSet.focIndex<motoLevelMin)
		{
			motoSet.focIndex = motoLevelMin;
		}
	}else
	{
		motoSet.focIndex++;
		if(motoSet.focIndex>motoLevelMax)
		{
			motoSet.focIndex = motoLevelMax;
		}
	}

	//*motoSet.dest_location = focLenScape[motoSet.focIndex];
	motoSet.dspLocation = focLenScape[motoSet.focIndex];
	//*motoSet.dest_location = GetMotoDest(motoSet.dspLocation);
	motoSet.destRsv = focLenScape[motoSet.focIndex];
	//motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
	motoSet.manuRsv = motoSet.dspLocation;

	DBG_PRINTF("moto %d,%d,%d\r\n",motoSet.focIndex,motoSet.dspLocation,motoSet.destRsv);

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void ModeSubAdd(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		
		if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			switch(mstPara->mode)
			{
				case WORKINGMODE_AUTO:
					mstPara->mode = WORKINGMODE_OFF;
				break;

				case WORKINGMODE_MANU:
					mstPara->mode = WORKINGMODE_AUTO;
				break;

				case WORKINGMODE_OFF:
					mstPara->mode = WORKINGMODE_MANU;
				break;
			}
		}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			mstPara->mode--;
			if(mstPara->mode<MST_SU4_AUTO)
			{
				mstPara->mode = MST_SU4_MANU;
			}
		}else
		{
			mstPara->mode--;
			if(mstPara->mode<WORKINGMODE_TTL)
			{
				mstPara->mode = WORKINGMODE_RPT;
			}
			if(WORKINGMODE_TTL_POOR==mstPara->mode)
			{
				if(!cmrPara.cls)
				{
					mstPara->mode = WORKINGMODE_TTL;
				}else if(!(cmrPara.bkMod&0x04))
				{
					mstPara->mode = WORKINGMODE_TTL;
				}
			}
		}
		
	}else
	{
		
		if(CMD_MODE_LIGH_SLA==*g_cmdMode)
		{
			switch(mstPara->mode)
			{
				case WORKINGMODE_AUTO:
					mstPara->mode = WORKINGMODE_MANU;
				break;

				case WORKINGMODE_MANU:
					mstPara->mode = WORKINGMODE_OFF;
				break;

				case WORKINGMODE_OFF:
					mstPara->mode = WORKINGMODE_AUTO;
				break;
			}
		}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
		{
			mstPara->mode++;
			if(mstPara->mode>MST_SU4_MANU)
			{
				mstPara->mode = MST_SU4_AUTO;
			}
		}else
		{
			mstPara->mode++;
			if(mstPara->mode>WORKINGMODE_RPT)
			{
				mstPara->mode = WORKINGMODE_TTL;
			}
			if(WORKINGMODE_TTL_POOR==mstPara->mode)
			{
				if(!cmrPara.cls)
				{
					mstPara->mode = WORKINGMODE_AUTO;
				}else if(!(cmrPara.bkMod&0x04))
				{
					mstPara->mode = WORKINGMODE_AUTO;
				}
			}
		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void KeySubAdd(u8 dir)
{
	switch(g_SettingIndex)
	{
		case SETTING_MODE:
			ModeSubAdd(dir);
		break;

		case SETTING_ADDSUB:
			ExposeAddSub(dir);
		break;

		case SETTING_PFNMENU:
			SnrMenuSubAdd(dir);
		break;

		case SETTING_EXTM_APERT:
			ApertAddSub(dir);
		break;

		case SETTING_PFNSUB:
			//SnrSubAddSub(dir);
			//g_snrMenu[g_snrIndex]
		break;

		case SETTING_MYMENU:
			SnrMyMenuAddSub(dir);
		break;

		case SETTING_GR_M:
			GN_MSubAdd(dir);
		break;

		case SETTING_FRQ:
			RptFrqShift(dir);
		break;

		case SETTING_MULTI:
			RptTimesShift(dir);
		break;

		case SETTING_RMT_GR:
			RmtGrAddSub(dir);
		break;

		case SETTING_RMT_CH:
			RmtChAddSub(dir);
		break;

		case SETTING_MST_M:
		case SETTING_MST_A:
		case SETTING_MST_B:
		case SETTING_MST_C:
			//MstRspArrParaAddSub(dir);
			if(MST_SET_ARR==g_mstSetting)
			{
				MstArrAddSub(dir);
			}else if(MST_SET_MODE==g_mstSetting)
			{
				MstRspArrModAddSub(dir);
			}else if(MST_SET_PARA==g_mstSetting)
			{
				MstRspArrParaAddSub(dir);
			}
		break;

		case SETTING_RPT_SET:
			MstRptParaSetAddSub(dir);
		break;

		case SETTING_MST_CHAN:
			MstChaAddSub(dir);
		break;

		case SETTING_MOTO:
			MotoShift(dir);
		break;
		
	}
}

void SnrMenuLevelEnter(void)
{
	g_menuLevel++;
	if(g_menuLevel>2)
	{
		g_menuLevel = 0;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void KeyOk(void)
{
	switch(g_SettingIndex)
	{
		case SETTING_PFNMENU:
			SnrMenuLevelEnter();
		break;

		case SETTING_PFNSUB:
			//g_snrSubIndex = pSnrSet[g_snrIndex];
			//g_snrMenu[g_snrIndex].subMenu->subSetProc(g_snrSubIndex);
			pSnrSet[g_snrIndex] = g_snrSubIndex;
			SubMenuSetProc();
			/*
			SubMenuSetProc();
			BuildSnrMenu();
			*/
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case SETTING_MYMENU:
			/*
			DspSnrMenu();
			*/
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			//DspSnrMenu(g_allMyMenuSw);
		break;

		case SETTING_MST_M:
		case SETTING_MST_A:
		case SETTING_MST_B:
		case SETTING_MST_C:
			if(g_mstSetting>0)
			{
				g_mstSetting = 0;
			}else
			{
				g_mstSetRsv = g_SettingIndex;
				g_SettingIndex = 0;
				
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		default:
			g_SettingIndex = 0;
			g_dspCmpMod = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;
	}

}

void SnrMenuEnterExit(void)
{
	if((SETTING_PFNMENU==g_SettingIndex)||(SETTING_SNRSUB==g_SettingIndex))
	{
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}else
	{
		DspSnrMenu();
	}
}

static u8 s_keyLock = 0;

void KeyLock(void)
{
	s_keyLock = 1;
	BuildKeyLockMenu();
	DisplayPicture(11,10,50,16,picBtnLock,DSP_NORMAL);
}

void StartCamera(void)
{
	if(!cmrPara.cls)
	{
		PULL_MOSI_LOW();
		HaltSec(2);
		PULL_MOSI_HIGH();
		SetSBOInt();
	}else
	{
		macData2[COM_DAT_STATUE]|=0x01;
		macData2[COM_DAT_REAR] = VerCodeCal(macData2);
	}
}

void KeyProcess(void)
{
	unsigned char rkey;
//	static char lockRs=0;
//	static char lockTirFlag=0;
//	static char lockCnt=0;
//	static char lockRs=0;
	rkey = ReadKey();

	if(!rkey)
	{
		HaltSec(10);
		return;
	}

	//DBG_PRINTF("%x\r\n",rkey);
	
	BkLiteOn();

	StartCamera();
	
	//BUZZER_ON(BUZZER_TIME_NORMAL,1);
	
	gWSleepCounter = 0;

	if(s_keyLock>0)
	{
		if(rkey==KEY_LOCK_LONG)
		{
			s_keyLock = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			return;
		}else
		{
			if(KEY_PILOT!=rkey)
			{
				if((KEY_LEFT==rkey)||(KEY_RIGHT==rkey))
				{
					ResetKeyInt();
				}
				return;
			}
		}
	}

	switch(rkey)
	{
		case KEY_MODE:
		ModeSw();
			//Mode();
		HaltSec(50);
		break;

		case KEY_MODE_LONG:
		HaltSec(50);
		break;

		case KEY_RF_SW:
		//SnrMenuEnterExit();
		HaltSec(50);
		break;

		case KEY_FUNC1:
		//if(pMenuList->button[0]->actionFuc.pPressFuc!=NULL)
		//{
		//	pMenuList->button[0]->actionFuc.pPressFuc();
		//}
		if(pMenuList->button[0]->actionFuc.pReleaseFun!=NULL)
		{
			pMenuList->button[0]->actionFuc.pReleaseFun();
		}
		HaltSec(50);
		break;

		case KEY_FUNC2:
		//if(pMenuList->button[1]->actionFuc.pPressFuc!=NULL)
		//{
		//	pMenuList->button[1]->actionFuc.pPressFuc();
		//}
		if(pMenuList->button[1]->actionFuc.pReleaseFun!=NULL)
		{
			pMenuList->button[1]->actionFuc.pReleaseFun();
		}
		HaltSec(50);
		break;

		case KEY_FUNC3:
		//if(pMenuList->button[2]->actionFuc.pPressFuc!=NULL)
		//{
		//	pMenuList->button[2]->actionFuc.pPressFuc();
		//}
		if(pMenuList->button[2]->actionFuc.pReleaseFun!=NULL)
		{
			pMenuList->button[2]->actionFuc.pReleaseFun();
		}
		HaltSec(50);
		break;

		case KEY_MENU_SW:
		//if(pMenuList->button[3]->actionFuc.pPressFuc!=NULL)
		//{
		//	pMenuList->button[3]->actionFuc.pPressFuc();
		//}
		if(pMenuList->button[3]->actionFuc.pReleaseFun!=NULL)
		{
			pMenuList->button[3]->actionFuc.pReleaseFun();
		}
		HaltSec(50);
		break;

		case KEY_OK:
		KeyOk();
		//SavePara(FLASH_USER_ADDR);                  // press "ok" when flash at the same time cause power down failed
		//SET_UPDATE_EVT(DSP_UPDATE_SAVEPARA);
		HaltSec(50);
		break;


		case KEY_LEFT:
		KeySubAdd(KEY_DIR_LEFT);	
		HaltSec(10);
		ResetKeyInt();
		break;

		case KEY_RIGHT:
		KeySubAdd(KEY_DIR_RIGHT);
		HaltSec(10);
		ResetKeyInt();
		break;	

		case KEY_SELE_LONG:
		HaltSec(50);
		break;

#ifdef FIBER_ADJUST_PROC
		case KEY_OK_LONG:
		break;
#endif

		case KEY_LEFT_LONG:
			HaltSec(200);
			HaltSec(100);
		break;

		case KEY_RIGHT_LONG:
			HaltSec(200);
			HaltSec(100);
		break;

		case KEY_RF_SW_LONG:
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

		case KEY_MENU:
			//if((SETTING_PFNMENU==g_SettingIndex)||(SETTING_PFNSUB==g_SettingIndex))
			if (SETTING_PFNMENU==g_SettingIndex)
			{
				g_SettingIndex = 0;
				pSnrSet[g_snrIndex] = g_snrSubIndex;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//SET_UPDATE_EVT(DSP_UPDATE_SAVEPARA);
			}else
			{
				g_SettingIndex = SETTING_PFNMENU;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);			
			}
		break;

		case KEY_CLEAR_LONG:
			ModeClear();
			HaltSec(50);
		break;

		case KEY_LOCK_LONG:
			KeyLock();
		break;

		case KEY_PILOT:
			//CHARGE_LED2_TOGGLE();
			Pilot();
			HaltSec(50);
		break;

		case (KEY_FUNC3|0X20):
			if(pMenuList->button[2]->actionFuc.pPressFuc!=NULL)
			{
				pMenuList->button[2]->actionFuc.pPressFuc();
			}
			HaltSec(50);
		break;

		default:
			HaltSec(50);
		break;
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
				//g_chargeOk = AD_CHARGE_FUL;
				g_chargeOk = AD_CHARGE_RDY;
			}else
			{
				//g_chargeOk = AD_CHARGE_RDY;
				if(*g_cmdMode==CMD_MODE_LIGH_MST)
				{
					g_chargeOk = AD_CHARGE_LOW;
				}else
				{
					g_chargeOk = AD_CHARGE_RDY;
				}
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

			if(g_chargeShadow>0)                  //
			{
				lowbatCnt++;
			}else if(lowbatCnt>0)
			{
				lowbatCnt = 0;
			}
		}
		adSum = 0;
		
	}

	if(!g_stbyStatue)
	{

		TurnOnChargeLed(0);

	//#ifndef DSP_TEST
#if (!_DBG_PRINT_)
		if(lowbatCnt>LOW_BAT_TIMEOUT)
		{
			LoadLowBatIcon();
			//OSTimeDlyHMSM(0,0,3,0);
			OS_ENTER_CRITICAL();
			delay_seconds(3);
			BK_TURNOFF(g_isHotProtect);
			KEY_BK_OFF();
			
			LCD_Clear(0);			//2015/11/27
			//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
			LowPow_Init(0,0,0);
			delay_seconds(3);
		}
#endif
	}

}


void TaskKeyScan(void *pdata)
{
	//u16 tsCnt=0;

	while(1)
	{		
#ifdef MST_DEBUG
#else
		if(g_keyAdcEnable>0)
		{
			KeyProcess();	
		}
		DspUpdate();
#endif
		HaltSec(10);
		//HaltSec(1);
		//HaltSec(1);
	}
}

void EXTI1_IRQHandler(void)
{
#ifdef SLAVE_POWER_DOWN
	if(COM_STATUE_SLEEP==g_comStatue)
	{
		 Ex_NVIC_Disable(1);				//2015/11/04
		g_comStatue = COM_STATUE_IDLE;
	}					//2015/10/31 for slave sleep
#endif
	EXTI->PR=1<<1;  //休眠后关机
}

void EXTI15_10_IRQHandler(void)
{
/*
	if(COM_STATUE_PWRDN==g_comStatue)
	{
		g_comStatue = COM_STATUE_IDLE;
		CLEAR_PWINT();
		return;
	}
*/
	//delay_us(1);

	//static u8 g_shadowKey=0;
	if(EXTI_GetITStatus(EXTI_Line13) == SET)//                 ／／判断是否是１３脚的中断
       {
       	Ex_NVIC_Disable(13);	
       	 g_comStatue = COM_STATUE_IDLE;					//2015/10/31 for slave sleep
               EXTI_ClearITPendingBit(EXTI_Line13);//                ／／清除中断标志位！
               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            ／／点亮ＬＥＤ            
        }

	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		//SPI_MISO_AUX  = 1;
		//OSTimeDlyHMSM(0,0,0,1);
		//EXTIx_Init(FTIR);
		//D3V3_EN = 0;
		//BOOST_EN = 1;
		//EXTIx_Init(GPIO_A,FTIR);
		//SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
		//g_cmrLinked = 1;
		//g_comStatue = COM_STATUE_START;
		EXTI_ClearITPendingBit(EXTI_Line15);
		Ex_NVIC_Disable(15);	
		return;
	}
#if 0
	if(EXTI_GetITStatus(EXTI_Line13) == SET)//                 ／／判断是否是１３脚的中断
       {
#ifdef SLAVE_POWER_DOWN
		Ex_NVIC_Disable(13);	
       	 g_comStatue = COM_STATUE_IDLE;					//2015/10/31 for slave sleep
#endif
               EXTI_ClearITPendingBit(EXTI_Line13);//                ／／清除中断标志位！
               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            ／／点亮ＬＥＤ            
        }

#ifdef SLEEP_PWR_OFF
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		//SPI_MISO_AUX  = 1;
		//OSTimeDlyHMSM(0,0,0,1);
		//EXTIx_Init(FTIR);
		//D3V3_EN = 0;
		//BOOST_EN = 1;
		//EXTIx_Init(GPIO_A,FTIR);
		//SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
		//g_cmrLinked = 1;
		//g_comStatue = COM_STATUE_START;
#ifdef SLAVE_POWER_DOWN
		Ex_NVIC_Disable(15);	
		g_comStatue = COM_STATUE_IDLE;					//2015/10/31 for slave sleep
#endif
		EXTI_ClearITPendingBit(EXTI_Line15);
		return;
	}
#endif
#endif

	//CHARGE_LED2_TOGGLE();

	//CHARGE_LED2_TOGGLE();
	//CHARGE_LED2_ON();
	//delay_ms(50);
	//CHARGE_LED2_OFF();
	//DISABLE_KEY_INT();

	//EXTI_ClearITPendingBit(EXTI_Line10);
	if(EXTI_GetITStatus(EXTI_Line10)==SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line10);

		//CHARGE_LED2_TOGGLE();
		//KEY_BK_TOGGLE();
		
		if(!g_shadowKey)
		{
			if(!KEY_DIR_1)
			{
				//g_keyDir = KEY_DIR_LEFT;
				g_shadowKey = KEY_DIR_LEFT;
				//CHARGE_LED2_TOGGLE();
			}else
			{
				//g_keyDir = KEY_DIR_RIGHT;
				g_shadowKey = KEY_DIR_RIGHT;
				//CHARGE_LED2_TOGGLE();
			}
			//Ex_NVIC_Config(GPIO_B,10,RTIR);
			SET_KEYINT_RTIG();
		}else
		{

			//CHARGE_LED2_TOGGLE();
			if((KEY_DIR_LEFT==g_shadowKey)&&(KEY_DIR_1>0))
			{
				//g_keyDir = KEY_DIR_LEFT;
				g_keyDir = KEY_DIR_RIGHT;
				SET_KEYINT_FTIG();
				DISABLE_KEY_INT();
				g_shadowKey = 0;
				//DBG_PRINTF("kr\r\n");
			}else if((KEY_DIR_RIGHT==g_shadowKey)&&(!KEY_DIR_1))
			{
				//g_keyDir = KEY_DIR_RIGHT;
				g_keyDir = KEY_DIR_LEFT;
				SET_KEYINT_FTIG();
				DISABLE_KEY_INT();
				g_shadowKey = 0;
				//DBG_PRINTF("kl\r\n");
			}else
			{
				//CHARGE_LED2_TOGGLE();
				g_keyDir = KEY_ERROR;
				SET_KEYINT_FTIG();
				//ENABLE_KEY_INT();
				DISABLE_KEY_INT();
				//FLAOK_TOGGLE();
			}

			//g_shadowKey = 0;
			//DISABLE_KEY_INT();
		}
	}else
	{
		//if(EXTI_GetITStatus(EXTI_Line10)==SET)
		//{
		//	EXTI_ClearITPendingBit(EXTI_Line10);	
		//}
		//EXTI_ClearITPendingBit(EXTI_Line10);				 //bug
		//CHARGE_LED2_TOGGLE();
		//delay_ms(200);
		//g_shadowKey = 0;
		//return;
	}

	EXTI_ClearITPendingBit(EXTI_Line10);	

	EXTI_GetITStatus(EXTI_Line10);
	
	//g_keyDir = 1;
	//Ex_NVIC_Disable(1);
	//ENABLE_KEY_INT();
	//DISABLE_KEY_INT();
	//EXTI->PR=1<<1;  //清除LINE0上的中断标志位  
	//EXTI->PR=1<<10;
	//CLEAR_KEYINT();
}

////////////////////////////////////////////////////////
void DspUpdate(void)
{
	if(g_dspUpdateFlag&DSP_UPDATE_MODE)
	{
		if(g_stbyStatue>0)
		{
			LCD_Clear(0);
			DisplayPicture(4,68,36,32,picStby,DSP_NORMAL);
			BuildStbyMenu();						
		}else
		{
			if(SETTING_PFNMENU ==g_SettingIndex)
			{
				//g_SettingIndex = SETTING_PFNMENU;
				DspSnrMenu();
				
			}else
			{
				if(pSnrSet[SNR_AF]!=2)                           //rf only
				{
					Mode();
				}else
				{
					SetSpiComData();
					DspRfOnlyScr();
				}
				
			}
		}
		g_dspUpdateFlag^=DSP_UPDATE_MODE;
	}

	if(g_dspUpdateFlag&DSP_UPDATE_SAVEPARA)
	{
		if(!cmrPara.cls)
		{
			SavePara(FLASH_USER_ADDR);
			g_dspUpdateFlag^=DSP_UPDATE_SAVEPARA;
		}
	}
}


void Pilot(void)
{
//	int pTime=0;
//	u8 accValue = 0;
	if((g_chargeOk<AD_CHARGE_RDY)||(g_isHotProtect>0))
	{
		return;
	}

	/*
	if(pSnrSet[SNR_MODELING])
	{
		g_rptPilotFlag = 0;
		rptSet.finalExpTimes = 66;
		FlashInResMode(WORKINGMODE_RPT,-1,0,66,66);
	}else
	{
	
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
				switch(mstPara->mode)
				{
					case WORKINGMODE_TTL:
					case WORKINGMODE_TTL_POOR:
						switch(pSnrSet[SNR_PILOT])
						{
							case 0:
								//pTime = OutputSwitch(0,0);
								//WlmFlaADSet(0,0);
								//g_expAccHotWarnValue+=1;
								//accValue = 1;
								FlashInResMode(WORKINGMODE_MANU,0,0,0,0);
							break;

							case 1:
								//pTime = OutputSwitch(2,0);
								//WlmFlaADSet(2,0);
								//g_expAccHotWarnValue+=4;
								//accValue = 6;
								FlashInResMode(WORKINGMODE_MANU,2,1,0,0);
							break;

							case 2:
								//pTime = OutputSwitch(7,0);
								//WlmFlaADSet(7,0);
								//accValue = 21;
								FlashInResMode(WORKINGMODE_MANU,7,0,0,0);
							break;

							default:
								//pTime = OutputSwitch(2,0);
								//WlmFlaADSet(2,0);
								//accValue = 6;
								FlashInResMode(WORKINGMODE_MANU,2,0,0,0);
							break;
						}
						//TURN_ON_TRIGER(OUTPUT_MAX);
					break;
					
					case WORKINGMODE_MANU:
						//pTime = OutputSwitch(mstPara->output1,mstPara->output2);
						//WlmFlaADSet(mstPara->output1,mstPara->output2);
						//TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
						//accValue = mstPara->output1*3+mstPara->output2;	
						FlashInResMode(WORKINGMODE_MANU,mstPara->output1,mstPara->output2,0,0);
					break;

					case WORKINGMODE_AUTO:
						FlashInResMode(WORKINGMODE_AUTO,mstPara->a_cmp1,mstPara->a_cmp2,0,0);
					break;

					case WORKINGMODE_RPT:
						g_rptPilotFlag = 1;
						rptSet.finalExpTimes = mstPara->rpt_times;
						FlashInResMode(WORKINGMODE_RPT,mstPara->multi_exp1, mstPara->multi_exp2, mstPara->rpt_hz,mstPara->rpt_times);
					break;

					case WORKINGMODE_GN:
						FlashInResMode(WORKINGMODE_GN,mstPara->gn_m,0,mstPara->hpFlash,0);
					break;
				}
			break;

			case CMD_MODE_LIGH_SLA:
				if((pSnrSet[SNR_SU4]>0)&&(WORKINGMODE_OFF==mstPara->mode))
				{
					return;
				}else
				{
					StopCapture();
					//pTime = OutputSwitch(mstPara->output1,mstPara->output2);
					//WlmFlaADSet(2,0);
					//TURN_ON_TRIGER(mstPara->output1>FLA_FBR_MAX?pTime:OUTPUT_MAX);
					//FlashInResMode(WORKINGMODE_MANU,mstPara->output1,mstPara->output2,0,0);
					FlashInResMode(WORKINGMODE_MANU,0,0,0,0);
					StartCapture();
				}
			break;

			case CMD_MODE_LIGH_MST://mst cmd pilot
				//wlSet.m_sendCmdEn = 1;
				wlSet.m_flaCmd[0] = 0xF1;
				WLM_SendData(&wlSet.m_flaCmd[0],1);
				//wlSet.m_sendCmdEn = 0;
			break;

		}
	}

	g_motoInvalidCounter = MOTO_INVALID_CNT;
*/

}


 
