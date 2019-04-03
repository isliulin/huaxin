#ifndef __DRV_H__
#define __DRV_H__
#include "stm8s.h"

void CHAN_1_Set_Duty(uint16_t duty);
void CHAN_2_Set_Duty(uint16_t duty);
void LedChar(u8 id, u8 ch);
void LedDspRfChan(u8 ch);
u8 GetKeyAdc(void);
u8 CheckBatteryPercentage(u8 id);
void LedDspBat(u8 bat);
void Led_1_on();
void Led_2_on();
void Led_3_on();
void Led_1_off();
void Led_2_off();
void Led_3_off();
void Led_1_twinkle();
void Led_2_twinkle();
void Led_3_twinkle();


void LedDisplayLoop(void);

#endif
