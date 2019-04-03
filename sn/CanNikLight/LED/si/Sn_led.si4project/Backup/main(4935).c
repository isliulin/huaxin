#include "stm8s.h"

uint8_t const led_template[] = {
0x77, 0x06, 0xb3, 0x93, 0xc6, 0xd5, 0xf5, 0x07, 0xf7, 0xd7
};

static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

void Timer2Init(void)
{
	CLK->PCKENR1 |= 0x01; 
	//TIM2_CR1 = 0x00;
	TIM5->CR2 = 0x00;
	//TIM2_IER |= 0x02;                     // Capture/compare 1 interrupt enable
	TIM5->PSCR = 0x04;                   
	//TIM2_ARRH = 0xFF;                    // use default
	//TIM2_ARRH = 0xFF;                    // use default
	TIM5->CR1 |= 0x01;
}

void StartSystem(void)
{
	for (;;)
	{	
		KeyEventLoop();
	}
}

/* Private functions ---------------------------------------------------------*/
void GPIO_Config(void)
{
    /*
	    GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);     //
	    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);    //
	    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);     //
	    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);     //
	    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);     //
	    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);         //
	    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);     //
	    GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);    //
	    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);         //
    */
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);    /* 采样电池电压*/
	GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);    /*采样切换*/
	GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);    /*采样切换*/
	
	GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_FAST);  /*PWM*/
	GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);  /*PWM*/

	GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);   /* LED */
	GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST); 
	GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST); 

	GPIO_Init(GPIOD, GPIO_PIN_ALL, GPIO_MODE_OUT_PP_LOW_FAST);   // LED DSP
	GPIO_Init(GPIOF, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);      // DOT

	GPIO_Init(GPIOB, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);     // B+
	GPIO_Init(GPIOB, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);     // A+

	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);     // 按键
	GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);        // 旋钮中断
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);   

	GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);   // RF
	GPIO_Init(GPIOC, GPIO_PIN_HNIB, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST); 
	GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST); 

}

void LedChar(u8 ch)
{
	if (ch > 9) return;
	GPIOD->ODR |= led_template[ch];
}

void Time1Ch1PwmConfig()
{
	TIM1_CtrlPWMOutputs(ENABLE);
}

void Time1Ch2PwmConfig()
{
	TIM1_CtrlPWMOutputs(ENABLE);
}

void TIM1_Config(uint16_t period, uint8_t counter)
{
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
	TIM1_TimeBaseInit(TIM5_PRESCALER_8, TIM1_COUNTERMODE_UP, period, counter);
	TIM1_ClearFlag(TIM1_FLAG_UPDATE);
	TIM1_OC1Init(TIM1_OCMODE_PWM1,
				 TIM1_OUTPUTSTATE_ENABLE,
				 TIM1_OUTPUTNSTATE_DISABLE,
				 32,
				 TIM1_OCPOLARITY_HIGH,
				 TIM1_OCNPOLARITY_LOW,
				 TIM1_OCIDLESTATE_RESET,
				 TIM1_OCNIDLESTATE_RESET);
	TIM1_Cmd(ENABLE);
}


// PA3, PD3, PA1控制电池电量采样

/*----------------------------------------------------------------------------*/
void SYSTEM_Init(void)
{
    CLK_Config();
	GPIO_Config();
}
/*----------------------------------------------------------------------------*/
void main(void)
{  
    SYSTEM_Init();
    
    StartSystem(); 
}
/*----------------------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{ 
    while (1)
    {
    }
}
#endif