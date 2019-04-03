#include "stm8s.h"
#include "drv.h"
#include "key.h"
#include "include.h"

uint8_t g_ledChar = 0;

static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

void Timer5Init(void)
{/*
	CLK->PCKENR1 |= 0x01; 
	//TIM2_CR1 = 0x00;
	TIM5->CR2 = 0x00;
	//TIM2_IER |= 0x02;                     // Capture/compare 1 interrupt enable
	TIM5->PSCR = 0x04;                   
	//TIM2_ARRH = 0xFF;                    // use default
	//TIM2_ARRH = 0xFF;                    // use default
	TIM5->CR1 |= 0x01;*/
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER5, ENABLE);
	TIM5_TimeBaseInit(TIM5_PRESCALER_16, 0xffff);
	TIM5_Cmd(ENABLE);
}

void LedTest()
{
	static u16 sTestm = 0;
	static u16 cnt = 0;
	u16 nowTime = get_counter();
	if(nowTime - sTestm > 1000)
	{
		sTestm = nowTime;
		cnt ++;
	}
	if (cnt > 1000)
	{
		cnt = 0;
		g_ledChar *= 2;
		GPIO_WriteBit(GPIOB, GPIO_PIN_6,RESET);	
		GPIO_WriteBit(GPIOB, GPIO_PIN_7,SET);
		GPIOD->ODR = ~g_ledChar;
        if (!g_ledChar)
        {
          g_ledChar = 1;
          GPIOD->ODR = ~g_ledChar;
        }
		//g_ledChar ++;
		
	}
}

void StartSystem(void)
{
	for (;;)
	{	
		KeyEventLoop();
		//LedDisplayLoop();
		LedTest();
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

	
	GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_IN_FL_NO_IT);   //电池采样
	GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT); 

	GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_FAST);  /*PWM*/
	GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);  /*PWM*/

	GPIO_Init(GPIOD, GPIO_PIN_ALL, GPIO_MODE_OUT_PP_LOW_FAST);   // LED DSP
	GPIO_Init(GPIOF, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);      // DOT
	GPIO_Init(GPIOB, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);     // B+
	GPIO_Init(GPIOB, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);     // A+

	GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);     // 按键
	
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_PU_IT);        // 旋钮中断
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);   

	GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);   // RF
	GPIO_Init(GPIOC, GPIO_PIN_HNIB, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST); 
	GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST); 

}

void TIM1_Config(uint16_t period, uint8_t counter)
{
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
	TIM1_TimeBaseInit(TIM5_PRESCALER_16, TIM1_COUNTERMODE_UP, period, counter);
	TIM1_ClearFlag(TIM1_FLAG_UPDATE);
	TIM1_OC1Init(TIM1_OCMODE_PWM2,
				 TIM1_OUTPUTSTATE_ENABLE,
				 TIM1_OUTPUTNSTATE_DISABLE,
				 2000,
				 TIM1_OCPOLARITY_HIGH,
				 TIM1_OCNPOLARITY_LOW,
				 TIM1_OCIDLESTATE_RESET,
				 TIM1_OCNIDLESTATE_RESET);
	
	TIM1_OC2Init(TIM1_OCMODE_PWM2, 
				 TIM1_OUTPUTSTATE_ENABLE, 
				 TIM1_OUTPUTNSTATE_DISABLE, 
				 2000, 
				 TIM1_OCPOLARITY_HIGH, 
				 TIM1_OCNPOLARITY_HIGH, 
				 TIM1_OCIDLESTATE_SET, 
				 TIM1_OCNIDLESTATE_SET);

	TIM1_Cmd(ENABLE);
	TIM1_CtrlPWMOutputs(ENABLE);
	
}



void ADC_Init(void)
{
	ADC1_DeInit();
	ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL4, ENABLE); //channel4 电池采样电压
	// 连续采样模式
	ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS,ADC1_CHANNEL_4,
	ADC1_PRESSEL_FCPU_D18,ADC1_EXTTRIG_TIM,DISABLE,
	ADC1_ALIGN_RIGHT,ADC1_SCHMITTTRIG_CHANNEL4,DISABLE);
	ADC1_DataBufferCmd(ENABLE);

	ADC1_Cmd(ENABLE);
	ADC1_StartConversion();
}

// PA3, PD3, PA1控制电池电量采样

/*----------------------------------------------------------------------------*/
void SYSTEM_Init(void)
{
    CLK_Config();
	GPIO_Config();
	ADC_Init();
	Timer5Init();
	TIM1_Config(4500, 0);                        /* 16分频，4500us周期，自动重载值为0 */
	//g_ledChar = 32;
	//LedDspRfChan(88);
        g_ledChar  = 1;
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
