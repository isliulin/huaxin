#include "drv.h"
#include "include.h"


uint8_t const led_template[] = {
0x77, 0x06, 0xb3, 0x97, 0xc6, 0xd5, 0xf5, 0x07, 0xf7, 0xd7
};

u16 g_ledDisplayTimer = 0;


void CHAN_1_Set_Duty(uint16_t duty)
{
	TIM1->CCR1H = (uint8_t)(duty>>8);
	TIM1->CCR1L = (duty & 0xff);
}


void CHAN_2_Set_Duty(uint16_t duty)
{
	TIM1->CCR2H = (uint8_t)(duty>>8);
	TIM1->CCR2L = (duty & 0xff);
}

void LedChar(u8 id, u8 ch)
{
	if (ch > 9) return;
	
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
	keyAdc = filter(8,ADC1_CHANNEL_3,ADC1_SCHMITTTRIG_CHANNEL3); // key adc chan3
	// for adc test
	//g_ledChar = (keyAdc >> 4);
    return (keyAdc >> 4);
}

void LedDspRfChan(u8 ch)
{
	g_ledChar = ch;
}

void LedDspBat(u8 bat)
{
	g_ledChar = bat;
}

u8 CheckBatteryPercentage(u8 id)
{
	uint16_t batAdc;
	uint16_t batAdcTotal;
	uint16_t tmp;

	//GPIO_WriteBit(GPIOA,GPIO_PIN_3,RESET);
	//GPIO_WriteBit(GPIOA,GPIO_PIN_1,SET);

	delay_ms(10);

	/* 获得总电压值 */
	batAdcTotal = filter(4,ADC1_CHANNEL_2,ADC1_SCHMITTTRIG_CHANNEL2);
	batAdcTotal >>= 4;   // 12bit adc to 8bit adc

	//GPIO_WriteBit(GPIOA,GPIO_PIN_3,SET);
	//GPIO_WriteBit(GPIOA,GPIO_PIN_1,RESET);

	delay_ms(10);

	batAdc = filter(4,ADC1_CHANNEL_1,ADC1_SCHMITTTRIG_CHANNEL1);
	batAdc >>= 4;

	if (0 == id)
	{
		//return (batAdc*100/142);              // 1.85/3.3*255
		tmp = batAdc*51;
	}else
	{
		//return (batAdcTotal*3300/255*57/10)
		tmp = (batAdcTotal*74) - (batAdc*51);    // 总电压-一路
	}

	if (tmp > 4000)
	{
		return (tmp - 4000)/34;
	}else
	{
		return 0;
	}
	
	/* */
	
}

void Led_1_on()
{
	GPIO_WriteBit(GPIOA,GPIO_PIN_1,RESET);
}

void Led_2_on()
{
	GPIO_WriteBit(GPIOA,GPIO_PIN_2,RESET);
}

void Led_3_on()
{
	GPIO_WriteBit(GPIOA,GPIO_PIN_3,RESET);
}

void Led_1_off()
{
	GPIO_WriteBit(GPIOA,GPIO_PIN_1,SET);
}

void Led_2_off()
{
	GPIO_WriteBit(GPIOA,GPIO_PIN_2,SET);
}

void Led_3_off()
{
	GPIO_WriteBit(GPIOA,GPIO_PIN_3,SET);
}

void LedDisplayLoop(void)
{
	static u8 id = 0;
	u16 nowTime = get_counter();
	u8 ledchar = 0;

	ledchar = g_ledChar;
	if (ledchar > 99)
	{
		ledchar = 99;
	}

	if (nowTime - g_ledDisplayTimer > 10000L)
	{
		g_ledDisplayTimer = nowTime;
	
		if (id == 0)
		{
			id = 1;                    // 高位
			GPIO_WriteBit(GPIOB, GPIO_PIN_7,RESET);	
			GPIO_WriteBit(GPIOB, GPIO_PIN_6,SET);
			GPIOD->ODR = ~led_template[ledchar/10];
			if (g_dot_left > 0)
			{
				GPIO_WriteBit(GPIOF,GPIO_PIN_4, RESET);
			}else
			{
				GPIO_WriteBit(GPIOF,GPIO_PIN_4, SET);
			}
			
		}else
		{
			id = 0;                    // 低位
			GPIO_WriteBit(GPIOB, GPIO_PIN_6,RESET);	
			GPIO_WriteBit(GPIOB, GPIO_PIN_7,SET);
			GPIOD->ODR = ~led_template[ledchar%10];
			if (g_dot_right > 0)
			{
				GPIO_WriteBit(GPIOF,GPIO_PIN_4, RESET);
			}else
			{
				GPIO_WriteBit(GPIOF,GPIO_PIN_4, SET);
			}
		}

	}
	
	
}



