#include "key.h"
#include "sys.h"

u16 g_keyTimer = 0;
u8 s_keyFlag = 0;
u32 s_keyShadow = 0;
u16 s_keyCounter = 0;
u8 g_keyDir=0;
u16 g_sysSleepCnt = 0;
u8 g_keyLock = 0;

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
	ADC1->SQR4 = 0x00;                   //  close adc_in6
	ADC1->SQR2 = 0x40;                  // adc_in22
	ADC1->CR1|=0x02;
	while(!(ADC1->SR & 0x01));
	return ADC1->DRL;	
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



