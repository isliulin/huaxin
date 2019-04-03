#include "key.h"
#include "sys.h"
#include "drv.h"

u16 g_keyTimer = 0;
u8 s_keyFlag = 0;
u32 s_keyShadow = 0;
u16 s_keyCounter = 0;
u8 g_keyDir=0;
u16 g_sysSleepCnt = 0;
u8 g_keyLock = 0;
u8 g_shadowKey=0;

extern u8 g_rfChan;
u16 g_chan1_duty = 0;
u16 g_chan2_duty = 0;

#define SET_KEYINT_RTIG()   {EXTI->CR1 &= 0x3F;	EXTI->CR1 |= 0x40;}
#define SET_KEYINT_FTIG()   {EXTI->CR1 &= 0x3F;	EXTI->CR1 |= 0x80;}
#define DISABLE_KEY_INT()   GPIO_WriteCr2Bit(GPIOB, GPIO_PIN_3,RESET)//{PE_CR2_C22 = 0;}
#define ENABLE_KEY_INT()    GPIO_WriteCr2Bit(GPIOB, GPIO_PIN_3,SET)//{PE_CR2_C22 = 1;}
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

	return keyIndex;
}

uint16_t filter(uint8_t N, ADC1_Channel_TypeDef ADC_Channel, ADC1_SchmittTrigg_TypeDef ADC_Schmitt)
{
	uint16_t sum = 0;
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,ADC_Channel,
		ADC1_PRESSEL_FCPU_D18, ADC1_EXTTRIG_TIM, DISABLE,
		ADC1_ALIGN_RIGHT,ADC_Schmitt, DISABLE);

	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_CONTINUOUS,
		ADC_Channel,ADC1_ALIGN_RIGHT);

	ADC1_Cmd(ENABLE);

	for (unsigned char count = 0; count < N; count++)
	{
		while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC));
		sum += ADC1_GetConversionValue();
	}

	ADC1_Cmd(DISABLE);
	ADC1_DeInit();
	return (uint16_t) (sum/N);
		
}


u8 GetKeyAdc(void)
{
	//ADC1->SQR4 = 0x00;                   //  close adc_in6
	//ADC1->SQR2 = 0x40;                  // adc_in22
	//ADC1->CR1|=0x02;
	//while(!(ADC1->SR & 0x01));
	//return ADC1->DRL;	
	uint16_t keyAdc;
	keyAdc = filter(4,ADC1_CHANNEL_4,ADC1_SCHMITTTRIG_CHANNEL4);
    return (keyAdc >> 4);
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
			
		break;

		case KEY_BAT_2:
		break;

		case KEY_CONTROL:
		break;
		
		default:
		break;
		// 
	}
	
}

INTERRUPT void EXTI3_IRQHandler(void)
{
	//if(EXTI->SR1 & 0x08) 
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


