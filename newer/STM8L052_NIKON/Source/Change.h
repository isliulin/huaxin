#ifndef __CHANGE_H
#define __CHANGE_H

#include "stm8l15x.h"

#define CAP_330V_VALUE          3383
#define CAP_170V_VALUE          1743
#define CAP_275V_VALUE          2820

#define CHANGE_CAP_START        {TIM5_CtrlPWMOutputs(ENABLE); GPIO_SetBits(GPIOC, GPIO_Pin_6);}
#define CHANGE_CAP_STOP         {TIM5_CtrlPWMOutputs(DISABLE);GPIO_ResetBits(GPIOC, GPIO_Pin_6);}

#define GPIO_H_CHANGE_ENABLE    GPIO_SetBits(GPIOC, GPIO_Pin_3)
#define GPIO_H_CHANGE_DISABLE   GPIO_ResetBits(GPIOC, GPIO_Pin_3)

#define CAP_LED_RED             {GPIO_ResetBits(GPIOD,GPIO_Pin_6);GPIO_SetBits(GPIOD,GPIO_Pin_7);}
#define CAP_LED_GREEN           {GPIO_SetBits(GPIOD,GPIO_Pin_6);GPIO_ResetBits(GPIOD,GPIO_Pin_7);}
#define CAP_LED_OFF             {GPIO_SetBits(GPIOD,GPIO_Pin_6);GPIO_SetBits(GPIOD,GPIO_Pin_7);}

#define IGBT_30V_ON             GPIO_SetBits(GPIOE, GPIO_Pin_6)
#define IGBT_30V_OFF            GPIO_ResetBits(GPIOE, GPIO_Pin_6)


typedef struct
{
    uint16_t Channel_KEY_VALUE;
    uint16_t Channel_CAP_VALUE;
    uint16_t Channel_LAMP_VALUE;
    uint16_t Channel_AF_VALUE;
    uint16_t Channel_MOS_VALUE;
}_ADC_VALUE;

void TIM5_Config(void);
void Change_Cap(void);
uint16_t filter(uint8_t channel);
void Low_Batt_Check(void);
void Check_Lamp_Temp(void);

extern _ADC_VALUE  ADC_VALUE;

#endif