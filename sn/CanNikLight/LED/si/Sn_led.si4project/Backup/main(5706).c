#include "stm8s.h"
#include "drv.h"
#include "key.h"
#include "include.h"
#include "uart.h"

uint8_t g_ledChar = 0;
extern uint8_t const led_template[];
u8 g_dot_left = 0;
u8 g_dot_right = 0;
u16 g_generalTaskTimer = 0;

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
		g_ledChar ++;
		if (g_ledChar > 99)
		{
			g_ledChar = 0;
		}
		
	}
}

void GeneralTaskLoop()
{
	u16 nowTime = get_counter();

	if (nowTime - g_generalTaskTimer > 1000L)    // 1ms
	{
		g_generalTaskTimer = nowTime;
		ParaSaveLoop();
		RfSendRcvTest();
	}
}


void StartSystem(void)
{
	//u8 state;
	for (;;)
	{	
		KeyEventLoop();
		//LedDisplayLoop();
#if (__DEVICE__ == DEVICE_RC)
#elif (__DEVICE__ == DEVICE_LED)
		A7105_Slave_Proc();
#endif
		GeneralTaskLoop();
		RcDataSendProcLoop();
		//LedTest();
		//state = GPIOB->IDR & 0x30;
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

	//GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_IT);        // 旋钮中断
	//GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); 

	//GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);   // RF
	//GPIO_Init(GPIOC, GPIO_PIN_HNIB, GPIO_MODE_OUT_PP_LOW_FAST);
	
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST); 
	GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST); 
	
	GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_FL_NO_IT);

	GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST); 
	GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST); 
	GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST); 

}

void SetRfTxRx()
{
#if (__DEVICE__ == DEVICE_LED)
	GPIO_WriteBit(GPIOE,GPIO_PIN_5,SET);
	GPIO_WriteBit(GPIOB,GPIO_PIN_0,RESET);
#elif (__DEVICE__ == DEVICE_RC)
	GPIO_WriteBit(GPIOE,GPIO_PIN_5,RESET);
	GPIO_WriteBit(GPIOB,GPIO_PIN_0,SET);
#endif
}

void TIM1_Config(uint16_t period, uint8_t counter)
{
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
	TIM1_TimeBaseInit(16, TIM1_COUNTERMODE_UP, period, counter);
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

void EXTI_Init()
{
	EXTI_DeInit();
	//EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_RISE_ONLY);//下降沿触发中断
	EXTI_SetExtIntSensitivity((EXTI_PORT_GPIOB),EXTI_SENSITIVITY_FALL_ONLY);
	//EXTI_SetExtIntSensitivity((EXTI_PORT_GPIOB),EXTI_SENSITIVITY_FALL_LOW);
}

/*----------------------------------------------------------------------------*/
void SYSTEM_Init(void)
{
	disableInterrupts();
    CLK_Config();
	GPIO_Config();
	ParaReadProc();
	ADC_Init();
	Timer5Init();
#if (__DEVICE__ == DEVICE_LED)
	TIM1_Config(PWN_PERIOD, 0);                        /* 16分频，4500us周期，自动重载值为0 */
	EXTI_Init();
#endif

#if (__UART_DBG__ > 0)
	UART1_Init(115200,
				UART1_WORDLENGTH_8D,
				UART1_STOPBITS_1,
				UART1_PARITY_NO,
				UART1_SYNCMODE_CLOCK_DISABLE,
				UART1_MODE_TX_ENABLE);
	UART1_Cmd(ENABLE);
	
#if (__DEVICE__ == DEVICE_LED)

	printf("led start %s,%s\r\n",__DATE__, __TIME__);

#elif (__DEVICE__ == DEVICE_RC)

	printf("rc start %s,%s\r\n",__DATE__, __TIME__);

#endif
#endif

	SetRfTxRx();
	A7105_Init();
	enableInterrupts();
	
	//g_ledChar = 32;
	//LedDspRfChan(88);
    //g_ledChar  = 1;
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
