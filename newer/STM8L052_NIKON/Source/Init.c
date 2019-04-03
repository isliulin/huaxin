#include "includes.h"

__IO uint32_t TimingDelay;

uint16_t ADC_Buffer[BUFFER_SIZE]  = {0};

void CLK_Config(void)
{
    /* Select HSI as system clock source */    
    CLK_SYSCLKSourceSwitchCmd(ENABLE);
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    /* system clock prescaler: 1*/
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI){}
}

void ITC_Config(void)
{
    ITC->ISPR1 = 0x55;  //全部设为1
    ITC->ISPR2 = 0x55;
    ITC->ISPR3 = 0x55;
    ITC->ISPR4 = 0x55;
    ITC->ISPR5 = 0x55;
    ITC->ISPR6 = 0x55;
    ITC->ISPR7 = 0x55;
    ITC->ISPR8 = 0x55;

    EXTI_SetPinSensitivity(EXTI_Pin_4, EXTI_Trigger_Falling);
    EXTI_SetPinSensitivity(EXTI_Pin_5, EXTI_Trigger_Rising);
    ITC_SetSoftwarePriority(EXTI4_IRQn, ITC_PriorityLevel_1);
    ITC_SetSoftwarePriority(EXTI5_IRQn, ITC_PriorityLevel_1);
    ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn, ITC_PriorityLevel_3);
//    ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_3);
    ITC_SetSoftwarePriority(SPI1_IRQn, ITC_PriorityLevel_2);
}

void GPIO_Config(void)
{
//    GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);  //NC
//    GPIO_Init(GPIOA, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);  //NC
    GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);        //PILOT
//    GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);  //NC
    GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_In_FL_No_IT);        //AF_IN
    GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_In_FL_No_IT);        //LAMP_TEMP
    GPIO_Init(GPIOA, GPIO_Pin_6, GPIO_Mode_In_FL_No_IT);        //KEY_ADC
    GPIO_Init(GPIOA, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);    //CHANGE_CAP_PWM   
//    GPIO_Init(GPIOG, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);        //NC
//    GPIO_Init(GPIOG, GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);        //NC
//    GPIO_Init(GPIOG, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);        //NC    
    
    GPIO_Init(GPIOG, GPIO_Pin_3, GPIO_Mode_In_FL_No_IT);        //MOTO_RES
    GPIO_Init(GPIOE, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);    //MOTO_IN1
    GPIO_Init(GPIOE, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);    //MOTO_ENA
    GPIO_Init(GPIOE, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);    //MOTO_IN2
    GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);   //LCD_DATA
    GPIO_Init(GPIOE, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);   //LCD_WR
    GPIO_Init(GPIOE, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);   //LCD_CS
    GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);    //BUZ
//    GPIO_Init(GPIOD, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);    //NC
    GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_In_FL_No_IT);        //DIFFUSE_REN
    GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);    //BK_LIGHT    
    GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT    );    //POWER_CHECK
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);    //POWER_CON

    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_In_FL_No_IT);        //SCK input
    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);
    GPIO_ResetBits(GPIOB,GPIO_Pin_3);
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);
    GPIO_ResetBits(GPIOB,GPIO_Pin_4);
    
//    GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);    //READY
//    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);    //SPI_OUT_M
//    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);    //SPI_OUT#
//    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);    //SPI_SCK
//    GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);    //SPI_IN
//    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);    //SPI_OUT
//    GPIO_Init(GPIOF, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);        //AF_VA
//    GPIO_Init(GPIOF, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOF, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOF, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOF, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOF, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);    //NC
    GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_In_PU_IT);           //FLASH_IN
//    GPIO_Init(GPIOD, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);    //NC
    GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);   //GREEN_LED
    GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);   //RED_LED
    
    GPIO_Init(GPIOG, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);   //EXT_330V
//    GPIO_Init(GPIOG, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOG, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOG, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);    //NC   
//    GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);    //NC
//    GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_PU_IT);           //PHOTOSEN
    GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);    //TIG
    GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);    //H_CHANGE
    GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_In_FL_No_IT);        //V_CHECK
    GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);    //AF_LED
    GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);    //CHANGE_CAP_EN
    GPIO_Init(GPIOC, GPIO_Pin_7, GPIO_Mode_In_FL_No_IT);        //MOS_TEMP
    GPIO_Init(GPIOE, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);   //30V_EN
    GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);    //IGBT_DRIVER
}
void TIM1_Config(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  TIM1_TimeBaseInit(15, TIM1_CounterMode_Up,8000,1);  
  TIM1_ClearFlag(TIM1_FLAG_Update);
//  TIM3_Cmd(ENABLE); 
}

#if 0
void TIM2_Config(void)
{/*
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up,65535);
  TIM2_ClearFlag(TIM2_FLAG_Update);*/
    CLK->PCKENR1 |= 0x01; 
    TIM2->EGR = 0x01;	  
    TIM2->PSCR = 0x07; 						  // 8m
    //TIM4_CNTR = arr;
    //TIM4_ARR = 255;
    //TIM4_CR1 = 0x01;
    TIM2->CNTR = 0x01;
    TIM2->IER = 0x01;
    TIM2->CR1 = 0x09;	   

//  TIM3_Cmd(ENABLE); 
}
#endif

void TIM2_Config(void)
{
    CLK->PCKENR1 |= 0x01; 
    //TIM2_CR1 = 0x00;
    TIM2->CR2 = 0x00;
    //TIM2_IER |= 0x02;                     // Capture/compare 1 interrupt enable
    TIM2->PSCR = 0x04;                   
    //TIM2_ARRH = 0xFF;                    // use default
    //TIM2_ARRH = 0xFF;                    // use default
    TIM2->CR1 |= 0x01;
}

void TIM3_Config(void)
{
    GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
    TIM3_TimeBaseInit(TIM3_Prescaler_64, TIM3_CounterMode_Up,63);
    TIM3_ClearFlag(TIM3_FLAG_Update);
    //  TIM5_ITConfig(TIM5_IT_Update, ENABLE);
    TIM3_OC2Init(TIM3_OCMode_PWM1,
                 TIM3_OutputState_Enable,
                 32,
                 TIM3_OCPolarity_Low,
                 TIM3_OCIdleState_Reset);  
    TIM3_Cmd(ENABLE); 
//    TIM3_CtrlPWMOutputs(ENABLE);
}

#if 0
void TIM4_Config(void)
{
    /* Enable TIM4 CLK */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);  
    /* Time base configuration */
    TIM4_TimeBaseInit(TIM4_Prescaler_128, TIM4_PERIOD);
    /* Clear TIM4 update flag */
    TIM4_ClearFlag(TIM4_FLAG_Update);
    /* Enable update interrupt */
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    /* Enable TIM4 */
    TIM4_Cmd(ENABLE);
}
#endif

void TIM4_Config(void)
{
    CLK->PCKENR1 |= 0x04; 
    TIM4->EGR = 0x01;    
    TIM4->PSCR = 0x07;                           // 8m
    //TIM4_CNTR = arr;
    //TIM4_ARR = 255;
    //TIM4_CR1 = 0x01;
    TIM4->CNTR = 0x01;
    TIM4->IER = 0x01;
    TIM4->CR1 = 0x09;  
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
//void Delay(__IO uint32_t nTime)
//{
//  TimingDelay = nTime;
//
//  while (TimingDelay != 0);
//}
//
///**
//  * @brief  Decrements the TimingDelay variable.
//  * @param  None
//  * @retval None
//  */
//void TimingDelay_Decrement(void)
//{
//  if (TimingDelay != 0x00)
//  {
//    TimingDelay--;
//  }
//}

/**
  * @brief  Configure ADC peripheral 
  * @param  None
  * @retval None
  */
void ADC_Config(void)
{
    /* Enable ADC1 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    /* Initialize and configure ADC1 */
    ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_12Bit, ADC_Prescaler_2);
    ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_16Cycles);
    ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_16Cycles);
    
    ADC_DMACmd(ADC1, ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE); 
    /* Enable ADC1 Channels */
    ADC_ChannelCmd(ADC1, ADC_Channel_0, ENABLE);        //KEY
    ADC_ChannelCmd(ADC1, ADC_Channel_1, ENABLE);        //LAMP
    ADC_ChannelCmd(ADC1, ADC_Channel_2, ENABLE);        //AFIN
    ADC_ChannelCmd(ADC1, ADC_Channel_3, ENABLE);        //MOS
    ADC_ChannelCmd(ADC1, ADC_Channel_4, ENABLE);        //CAP
    //ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, ENABLE);  //内部参考
    //ADC_VrefintCmd(ENABLE);//TSON
    
    ADC_SoftwareStartConv(ADC1);
}

/**
* @brief  Configure DMA peripheral 
* @param  None
* @retval None
*/
void DMA_Config(void)
{
    /* Enable DMA1 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);
    /* Connect ADC to DMA channel 0 */
    SYSCFG_REMAPDMAChannelConfig(REMAP_DMA1Channel_ADC1ToChannel0);
            
    DMA_Init(DMA1_Channel0, 
             BUFFER_ADDRESS,
             ADC1_DR_ADDRESS,
             BUFFER_SIZE,
             DMA_DIR_PeripheralToMemory,
             DMA_Mode_Circular,
             DMA_MemoryIncMode_Inc,
             DMA_Priority_High,
             DMA_MemoryDataSize_HalfWord);
    
    /* DMA Channel0 enable */
    DMA_Cmd(DMA1_Channel0, ENABLE);
    
    /* Enable DMA1 channel0 Transfer complete interrupt */
    //DMA_ITConfig(DMA1_Channel0, DMA_ITx_TC, ENABLE);
    
    /* DMA enable */
    DMA_GlobalCmd(ENABLE);
}

void SPI1_Config(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);
    SPI_DeInit(SPI1);
    SPI_Init(SPI1,SPI_FirstBit_LSB, 
             SPI_BaudRatePrescaler_2, 
             SPI_Mode_Slave, 
             SPI_CPOL_Low, 
             SPI_CPHA_2Edge, 
             SPI_Direction_1Line_Rx, 
             SPI_NSS_Soft, 
             0x07);
    
    SPI_ITConfig(SPI1,SPI_IT_RXNE, ENABLE);
    SPI_NSSInternalSoftwareCmd(SPI1,DISABLE);
    
    disableInterrupts();    
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_In_FL_IT);
    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);
    EXTI_SetPinSensitivity(EXTI_Pin_5, EXTI_Trigger_Rising);    
}

                 
   
