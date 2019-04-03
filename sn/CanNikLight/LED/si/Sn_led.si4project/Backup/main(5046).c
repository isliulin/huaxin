#include "stm8s.h"

static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}


/* Private functions ---------------------------------------------------------*/
void GPIO_Config(void)
{
    GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);     //
    GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);    //
    GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_FAST);     //
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);     //
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);     //
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);         //
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);     //
    GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);    //
    GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);         //
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
    
    while (1)        
    {        
        ;
    }    
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