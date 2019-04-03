#ifndef __INIT_H
#define __INIT_H

#include "stm8l15x.h"

void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void CLK_Config(void);
void ITC_Config(void);
void TIM1_Config(void);
void TIM2_Config(void);
void TIM4_Config(void);
void ADC_Config(void);
void DMA_Config(void);
void TIM3_Config(void);
void GPIO_Config(void);
void SPI1_Config(void);

#define TIM4_PERIOD       124

#define ADC1_DR_ADDRESS        ((uint16_t)(&ADC1->DRH))//((uint16_t)0x5344)
#define BUFFER_SIZE            ((uint8_t) 0x05)
#define BUFFER_ADDRESS         ((uint16_t)(&ADC_Buffer))

extern uint16_t ADC_Buffer[BUFFER_SIZE];

#endif