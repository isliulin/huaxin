#include "includes.h"

uint16_t g_sysTick = 0;

uint16_t get_counter(void)
{
    uint16_t tim2;
    tim2 = (uint16_t)TIM2->CNTRH << 8;
    tim2 |= TIM2->CNTRL;
    return tim2;
}

void SysTicks()
{
    uint16_t crtTime; 
    crtTime = get_counter(); 
    if ( crtTime - g_sysTick > 1000L)
    {
        g_sysTick = crtTime;
        uwTick++;
        HAL_SYSTICK_Callback();
    }
}

int main( void )
{    
    CLK_Config();
    UserInit();
    //SPI1_Config();
    //SPI1_Init();
    //TIM1_Config();
    
    GPIO_Config();
    TM1621_Init();
    TIM2_Config();
    TIM3_Config();
    TIM4_Config();
    DMA_Config();
    ADC_Config();
    TIM5_Config();
    //GPIO_Config();
    //ITC_Config();
    //TM1621_Init();

	SPIn_Init();
    
    /* enable interrupts */
    enableInterrupts(); 
    
    MOTOR_RESET_EN;
 
    while(1)
    {
        ComMonitor();
        FlashEventLoop();
    	SysTicks();
        TaskProcess();
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif