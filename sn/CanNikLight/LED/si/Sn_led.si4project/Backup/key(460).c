#include "key.h"
#include "sys.h"
#include "drv.h"
#include "include.h"

u16 g_keyTimer = 0;
u8 s_keyFlag = 0;
u32 s_keyShadow = 0;
u16 s_keyCounter = 0;
u8 g_keyDir=0;
u16 g_sysSleepCnt = 0;
u8 g_keyLock = 0;
u8 g_shadowKey=0;

extern u8 g_rfChan;
u8 g_CtrlSelect = 0;
u8 g_isLedON = 0;
u8 g_chan1_duty_bak = 0;
u8 g_chan2_duty_bak = 0;

uint16_t g_pwm_chan1_duty = 0;
uint16_t g_pwm_chan2_duty = 0;

#define SET_KEYINT_RTIG()   EXTI_SetExtIntSensitivity((EXTI_PORT_GPIOB),EXTI_SENSITIVITY_RISE_ONLY);//{EXTI->CR1 &= 0x3F;	EXTI->CR1 |= 0x40;}
#define SET_KEYINT_FTIG()   EXTI_SetExtIntSensitivity((EXTI_PORT_GPIOB),EXTI_SENSITIVITY_FALL_ONLY);//{EXTI->CR1 &= 0x3F;	EXTI->CR1 |= 0x80;}
#define DISABLE_KEY_INT()   GPIO_WriteCr2Bit(GPIOB, GPIO_PIN_4,RESET)//{PE_CR2_C22 = 0;}
#define ENABLE_KEY_INT()    GPIO_WriteCr2Bit(GPIOB, GPIO_PIN_4,SET)//{PE_CR2_C22 = 1;}
#define CLEAR_KEYINT()


u8 ReadPilot()
{
	return 1;
}

u8 KeyToIndex(u8 key)
{
	u8 keyIndex=0;

#if (__DEVICE__ == DEVICE_RC)

	if((key<KEY0_VALUE_MAX)&&(key>KEY0_VALUE_MIN))
	{
		keyIndex = KEY_CHANNEL;
	}else if((key<KEY1_VALUE_MAX)&&(key>KEY1_VALUE_MIN))
	{
		keyIndex = KEY_CONTROL;
	}else if((key<KEY2_VALUE_MAX)&&(key>KEY2_VALUE_MIN))
	{
		keyIndex = KEY_ON_OFF;
	}else if((key<KEY3_VALUE_MAX)&&(key>KEY3_VALUE_MIN))
	{
		keyIndex = KEY_LEFT;
	}else if ((key<KEY4_VALUE_MAX)&&(key>KEY4_VALUE_MIN))
	{
		keyIndex = KEY_RIGHT;
	}
#else

	if((key<KEY0_VALUE_MAX)&&(key>KEY0_VALUE_MIN))
	{
		//keyIndex = KEY_RF_SW;
		keyIndex = KEY_CHANNEL;
	}else if((key<KEY1_VALUE_MAX)&&(key>KEY1_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC1;
		keyIndex = KEY_BAT_1;
	}else if((key<KEY2_VALUE_MAX)&&(key>KEY2_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC2;
		keyIndex = KEY_BAT_2;
	}else if((key<KEY3_VALUE_MAX)&&(key>KEY3_VALUE_MIN))
	{
		//keyIndex = KEY_FUNC3;
		keyIndex = KEY_CONTROL;
	}

#endif

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
				//if (!SW_LOCK)
				if (1)
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
				//if (!SW_LOCK)
				if (1)
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

void KeyUnlockProc(void)
{
	return;
}


void KeyRfChanProc()
{
	g_rfChan ++;
	if ( g_rfChan > 16 )
	{
		g_rfChan = 0;
	}

	LedDspRfChan(g_rfChan);
	
//#if (__DEVICE__ == DEVICE_RC)
	RfSetChan(g_rfChan);
//#endif
}

void KeyBat1()
{
	u8 batPercent;
	batPercent = CheckBatteryPercentage(0);
	LedDspBat(batPercent);
}

void KeyBat2()
{
	u8 batPercent;
	batPercent = CheckBatteryPercentage(1);
	LedDspBat(batPercent);
}

void KeyControl()
{
	if (g_CtrlSelect)
	{
		g_CtrlSelect = 0;
		Led_1_on();
		Led_2_off();
		g_ledChar = 32;
	}else
	{
		g_CtrlSelect = 1;
		Led_1_off();
		Led_2_on();
		g_ledChar = 55;
	}
	
}

//void KeyLeftProc()
void KeyRightProc()
{
	if (g_CtrlSelect)
	{
		if (g_pwm_chan1_duty > PWM_PULSE_WIDTH_STEP)
		{
			g_pwm_chan1_duty -= PWM_PULSE_WIDTH_STEP;
		}else
		{
			g_pwm_chan1_duty = 0;
		}
		CHAN_1_Set_Duty(PWN_PERIOD - g_pwm_chan1_duty);
		g_ledChar = (g_pwm_chan1_duty / PWM_PULSE_WIDTH_STEP);
			
	}else
	{
		if (g_pwm_chan2_duty > PWM_PULSE_WIDTH_STEP)
		{
			g_pwm_chan2_duty -= PWM_PULSE_WIDTH_STEP;
		}else
		{
			g_pwm_chan2_duty = 0;
		}
		CHAN_2_Set_Duty(PWN_PERIOD - g_pwm_chan2_duty);
		g_ledChar = (g_pwm_chan2_duty / PWM_PULSE_WIDTH_STEP);
	}

	ENABLE_KEY_INT();
	
}

//void KeyRightProc()
void KeyLeftProc()

{
	if (g_CtrlSelect)
	{
		if (g_pwm_chan1_duty < (PWN_PERIOD - PWM_PULSE_WIDTH_STEP))
		{
			g_pwm_chan1_duty += PWM_PULSE_WIDTH_STEP;
		}else
		{
			g_pwm_chan1_duty = PWN_PERIOD;
		}
		//CHAN_1_Set_Duty(g_pwm_chan1_duty);
#if (__DEVICE__ == DEVICE_RC)
		RC_ControlData();
#else
		CHAN_1_Set_Duty(PWN_PERIOD - g_pwm_chan1_duty);
#endif
		g_ledChar = (g_pwm_chan1_duty / PWM_PULSE_WIDTH_STEP);
		
	}else
	{
		if (g_pwm_chan2_duty < (PWN_PERIOD - PWM_PULSE_WIDTH_STEP))
		{
			g_pwm_chan2_duty += PWM_PULSE_WIDTH_STEP;
		}else
		{
			g_pwm_chan2_duty = PWN_PERIOD;
		}
#if (__DEVICE__ == DEVICE_RC)
		RC_ControlData();
#else
		CHAN_2_Set_Duty(PWN_PERIOD - g_pwm_chan2_duty);
#endif
		g_ledChar = (g_pwm_chan2_duty / PWM_PULSE_WIDTH_STEP);
	}
	
	ENABLE_KEY_INT();
}


void KeyOnOffProc()
{
	if (g_isLedON)
	{
		g_isLedON = 0;
		g_chan1_duty_bak = g_pwm_chan1_duty;
	}else
	{
		g_isLedON = 1;
		g_chan2_duty_bak = g_pwm_chan2_duty;
	}

	RC_ControlData();
}

void KeyEventLoop( void )
{
	u8 key;
	if(KEY_NULL == ParseKey(&key))
	{
		return;
	}

	g_sysSleepCnt = 0;

	switch (key)
	{
		case KEY_CHANNEL:
			KeyRfChanProc();
		break;

		case KEY_BAT_1:
			KeyBat1();
		break;

		case KEY_BAT_2:
			KeyBat2();
		break;

		case KEY_CONTROL:
			KeyControl();
		break;

		case KEY_LEFT:
			KeyLeftProc();
		break;

		case KEY_RIGHT:
			KeyRightProc();
		break;

		case KEY_ON_OFF:
			KeyOnOffProc();
		break;
		
		default:
		break;
		// 
	}
	
}

INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{

	if(!g_shadowKey)
	{
		if(!KEY_DIR_1)
		{
			//g_keyDir = KEY_DIR_LEFT;
			g_shadowKey = KEY_DIR_LEFT;
		}else
		{
			//g_keyDir = KEY_DIR_RIGHT;
			g_shadowKey = KEY_DIR_RIGHT;
		}
		//Ex_NVIC_Config(GPIO_B,10,RTIR);
		SET_KEYINT_RTIG();
	}else
	{
		if((KEY_DIR_LEFT==g_shadowKey)&&(KEY_DIR_1>0))
		{
			g_keyDir = KEY_DIR_LEFT;                  // 20171230
			//g_keyDir = KEY_DIR_RIGHT;
			SET_KEYINT_FTIG();
			DISABLE_KEY_INT();
		}else if((KEY_DIR_RIGHT==g_shadowKey)&&(!KEY_DIR_1))
		{
			g_keyDir = KEY_DIR_RIGHT;               // 20171230
			//g_keyDir = KEY_DIR_LEFT;
			SET_KEYINT_FTIG();
			DISABLE_KEY_INT();
		}else
		{
			SET_KEYINT_FTIG();
			ENABLE_KEY_INT();
			//FLAOK_TOGGLE();
		}

		g_shadowKey = 0;
		//DISABLE_KEY_INT();
	}
	
	CLEAR_KEYINT();
}


