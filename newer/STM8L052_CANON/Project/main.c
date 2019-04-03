#include "includes.h"

extern void UserInit(void);

uint16_t get_counter(void)
{
    uint16_t tim2;
    tim2 = (uint16_t)TIM2->CNTRH << 8;
    tim2 |= TIM2->CNTRL;
    return tim2;
}

uint16_t g_sysTick = 0;
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

void Sys_Init(void)
{
    DMA_Config();
    ADC_Config();
    ITC_Config();
    SPI1_Init();
    InitExtIntPort();
}

int main( void )
{    
    disableInterrupts();
    CLK_Config();    
    UserInit();
    GPIO_Config();
    TM1621_Init();
    TIM2_Config();
    TIM3_Config();
    TIM4_Config();
    TIM5_Config();
    Sys_Init();
    
//#ifndef SN_TEST
//    TIM2_Config();
//    TIM3_Config();
//    TIM4_Config();
//    DMA_Config();
//    ADC_Config();
//    TIM5_Config();
//#else
//    TIM4_Config();
//    //Init_USART1();
//#endif
//    GPIO_Config();
//    TM1621_Init();
//    ITC_Config();
//#ifndef SN_TEST
////    TM1621_Init();
//#endif
        
    SysInfo.KEY_SET_Change_flag = SET;
    /* enable interrupts */
    enableInterrupts(); 
 
    while(1)
    {
#ifndef SN_TEST
    	SysTicks();
        TaskProcess();
#else
        ComMonitor();
#endif
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