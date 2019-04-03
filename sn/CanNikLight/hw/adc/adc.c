#include "adc.h"
#include"global.h"
#include <stdarg.h>
#include "../sys/sys.h"

#define LOW_BAT_CHECK_CNT (25*20)

u8 g_keyDir=0;
u16 g_volRef = 0;
u16 g_lowbatCnt=0;
u8 g_lowbat = 0;
u8 g_lowbatDsp = 0;

void CloseAdcClock(void)
{
	CLK_PCKENR2&=~(1<<0);              // adc clock
}

void OpenAdcClock(void)
{
	CLK_PCKENR2|=(1<<0);              // adc clock
}

void Adc_Init(void)
{
	CLK_PCKENR2|=(1<<0);              // adc clock
	ADC1_CR1=0x41;
	ADC1_CR2=0x02; 
	//ADC1_SQR2 = 0x40;                  // adc_in22
}

u8 GetKeyAdc(void)
{
	ADC1_SQR4 = 0x00;                   //  close adc_in6
	ADC1_SQR2 = 0x40;                  // adc_in22
	ADC1_CR1|=0x02;
	while(!(ADC1_SR & 0x01));
	return ADC1_DRL;	
}

u8 GetChargeAdc(void)
{
	ADC1_SQR2 = 0x00; 		// close adc_in22
	ADC1_SQR4 = 0x40;   		// adc_in6	
	ADC1_CR1 |= 0x02;
	while(!(ADC1_SR & 0x01));
	return ADC1_DRL;	
}

u16 g_keyTimer = 0;
u8 s_keyFlag = 0;
u32 s_keyShadow = 0;
u16 s_keyCounter = 0;

u16 g_chargeTimer = 0;

/*
u8 ReadPilot(void)
{
	if (!PILOT_PIN)
	{
		return KEY_PILOT;
	}else
	{
		return KEY_NULL;
	}
}*/
#define ReadPilot()  (PILOT_PIN)

u8 KeyToIndex(u8 key)
{
	u8 keyIndex=0;
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
		keyIndex = KEY_OK;
	}

	return keyIndex;
}

u8 ParseKey(u8 *rKey)
{
	u8 ikey;
	u16 crtTime; 
        
        crtTime = get_counter();              // get time
	
	if (!s_keyFlag)                                     // the last key flag
	{
		if ( crtTime - g_keyTimer > 10000L)              // key timer overflow
		{
			g_keyTimer = crtTime;

			if (ReadPilot())                                           // pilot first
			{
				if (!SW_LOCK)
				{
					ikey = KeyToIndex(GetKeyAdc());	
				}else
				{
					ikey = KEY_LOCK;
				}
			}else
			{
				ikey = KEY_PILOT;
			}
			if(ikey > 0)
			{
				s_keyFlag = 1;
				s_keyShadow = ikey;
			}
		}
	}else
	{
		if ( crtTime - g_keyTimer > 10000L)
		{
			g_keyTimer = crtTime;
			if (ReadPilot())
			{
				if (!SW_LOCK)
				{
					ikey = KeyToIndex(GetKeyAdc());
				}else
				{
					ikey = KEY_LOCK;
				}
			}else
			{
				ikey = KEY_PILOT;
			}
			
			//s_keyFlag = 0;
			

			if (s_keyShadow != ikey)
			{
				s_keyFlag = 0;
				if ((!ikey)&&(s_keyCounter < 200L))
				{
					// key up
					*rKey = s_keyShadow | 0x10;                                          // debounce
					s_keyShadow = 0;
					s_keyCounter = 0;
					return KEY_READY;
				}else
				{
					// key error
					s_keyShadow = ikey;
					s_keyCounter = 0;
					return KEY_NULL;
				}
				
			}else
			{
				*rKey = ikey;                                                 // pressed
				if (!s_keyCounter)
				{
					s_keyCounter ++;
					return KEY_READY;
				}else
				{
					s_keyCounter++;
					if (s_keyCounter == 200L)
					{
						*rKey = ikey | 0x20;                        //
						return KEY_READY;
					}
					
				}
			}
		}
	}

	if(KEY_DIR_LEFT==g_keyDir)
	{
		g_keyDir = 0;
		*rKey = KEY_LEFT;
		return KEY_READY;
		
	}else if (KEY_DIR_RIGHT==g_keyDir)
	{
		g_keyDir = 0;
		*rKey = KEY_RIGHT;
		return   KEY_READY;
	}

	return KEY_NULL;
}

// 330 --> 193
// 290 --> 170
// 270 --> 158
// 170 --> 99
void VolCheckHandle(u8 vol)
{
	if (vol > 170)                   // 290 charge full
	{
		//ENABLE_FAST_CHARGE();
		g_chargeOk = AD_CHARGE_FUL;
		READY_ON();
		LED_RED();
		g_lowbatCnt = 0;
	}else if ( vol > 158 )         // 270 charge ok
	{
		//ENABLE_FAST_CHARGE();
		if (g_chargeOk < AD_CHARGE_RDY)
		{
			if (g_cfnOption[CFN_BEEP] > 0)
			{
				BeeperOn(8,1);
			}
		}
		g_chargeOk = AD_CHARGE_RDY;
		LED_GREEN();	
		READY_ON() 
		g_lowbatCnt = 0;
	}else if ( vol > 99 )                     // enable fast charge
	{
		//ENABLE_FAST_CHARGE();
		g_chargeOk = AD_CHARGE_LOW;
		LED_OFF();
		READY_OFF();
		g_lowbatCnt ++;
	}else                                       // disable fast charge
	{
		//DISABLE_FAST_CHARGE();
		g_chargeOk = AD_CHARGE_LOW;
		LED_OFF();
		READY_OFF();
		g_lowbatCnt ++;
	}

	if ( vol > 99 )
	{
		ENABLE_FAST_CHARGE();
	}else
	{
		DISABLE_FAST_CHARGE();
	}

	if (g_lowbatCnt > LOW_BAT_CHECK_CNT)
	{
		g_lowbat = 1;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
}

#define CVBF_TMS 8
u16 CheckVolBeforeFlash(void)
{
	u8 i = 0;
	u16 vol =0;

	DISABLE_CHARGE();
	delay_us(500);
	
	for (i=0;i<CVBF_TMS;i++)
	{
		vol += GetChargeAdc();
	}

	ENABLE_CHARGE();
	
	return (vol/CVBF_TMS);
}


u8 g_volCheckCnt = 0;
u16 g_volSum = 0;
u8 g_volCkFlag;
// 20ms * 8 every time
#if 1
void ChargeEventLoop(void)
{
	//u8 advol;
	u16 crtTime;
	crtTime = get_counter();
	//if ((crtTime - g_chargeTimer) > 20000L)
	if ((crtTime - g_chargeTimer) > 5000l)
	{
		g_chargeTimer = crtTime;
		DISABLE_CHARGE();
		g_volSum += GetChargeAdc();
		ENABLE_CHARGE();
		g_volCheckCnt ++;
		if (g_volCheckCnt >= 8)
		{
			g_volCheckCnt = 0;
			g_volRef = g_volSum/8;
			VolCheckHandle(g_volSum/8);
			g_volSum = 0;
		}
	}
	
}

#else

void ChargeEventLoop(void)
{
	u16 crtTime;
	if (!g_volCkFlag)
	{
		crtTime = get_counter();
		if ((crtTime - g_chargeTimer) > 20000L)                // 每隔20ms检测一次
		{
			g_chargeTimer = crtTime;
			g_volCkFlag = 1;
			DISABLE_CHARGE();                                     // 停止充电
		}
	}else if (1 == g_volCkFlag)
	{
		crtTime = get_counter();
		if ((crtTime - g_chargeTimer) > 1000l)			// 间隔1 ms
		{
			g_chargeTimer = crtTime;
			g_volCkFlag = 2;
		}
	}else
	{
		crtTime = get_counter();
		if ((crtTime - g_chargeTimer) > 100l)                   // 间隔100us
		{
			g_chargeTimer = crtTime;
			g_volSum += GetChargeAdc();
			g_volCheckCnt ++;
			if (g_volCheckCnt >= 8)
			{
				g_volCheckCnt = 0;
				g_volRef = g_volSum/8;
				VolCheckHandle(g_volSum/8);
				g_volSum = 0;
				ENABLE_CHARGE();
				g_volCkFlag = 0;
			}
		}
	}
}
#endif

