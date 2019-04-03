#include "key.h"
#include "sys.h"

u16 g_keyTimer = 0;
u8 s_keyFlag = 0;
u32 s_keyShadow = 0;
u16 s_keyCounter = 0;
u8 g_keyDir=0;
u16 g_sysSleepCnt = 0;
u8 g_keyLock = 0;

#define SET_KEYINT_RTIG()   {EXTI->CR1 &= 0x3F;	EXTI->CR1 |= 0x40;}
#define SET_KEYINT_FTIG()   {EXTI->CR1 &= 0x3F;	EXTI->CR1 |= 0x80;}
#define DISABLE_KEY_INT()   GPIO_WriteCr2Bit(GPIOB, GPIO_PIN_3,0)//{PE_CR2_C22 = 0;}
#define ENABLE_KEY_INT()    GPIO_WriteCr2Bit(GPIOB, GPIO_PIN_3,1)//{PE_CR2_C22 = 1;}
#define CLEAR_KEYINT()


u8 ReadPilot()
{
	return 1;
}

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
	}

	return keyIndex;
}

u8 GetKeyAdc(void)
{
	//ADC1->SQR4 = 0x00;                   //  close adc_in6
	//ADC1->SQR2 = 0x40;                  // adc_in22
	//ADC1->CR1|=0x02;
	//while(!(ADC1->SR & 0x01));
	//return ADC1->DRL;	
        return 0;
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


void KeyEventLoop( void )
{
	u8 key;
	if(KEY_NULL == ParseKey(&key))
	{
		return;
	}

	//BkLightOn();
/*
	if (g_keyLock)
	{
		if ((key!=KEY_LOCK)&&(key!=KEY_LOCK_DEBOUNCE))
		{
			KeyUnlockProc();
		}
	}
*/

	g_sysSleepCnt = 0;

	switch (key)
	{
		case 0:
		default:
		break;
		// 
	}
	
}

INTERRUPT void EXTI3_IRQHandler(void)
{
	//if(EXTI->SR1 & 0x08) 
	if (EXT)
	{
		EXTI->SR1 |= 0x08; 

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
		
		//g_keyDir = 1;
		//Ex_NVIC_Disable(1);
		//ENABLE_KEY_INT();
		//DISABLE_KEY_INT();
		//EXTI->PR=1<<1;  //清除LINE0上的中断标志位  
		//EXTI->PR=1<<10;
		CLEAR_KEYINT();
	}
}


