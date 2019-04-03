#ifndef __INCLUDE_H__
#define __INCLUDE_H__
#include "sys.h"
#include "drv.h"
#include "a7105.h"
#include "eeprom.h"

extern uint8_t g_ledChar;
extern u8 g_dot_left;
extern u8 g_dot_right;
extern uint8_t g_ledChar;
extern uint16_t g_pwm_chan1_duty;
extern uint16_t g_pwm_chan2_duty;
extern u8 g_isLedON;
extern u8 g_rfChan;


#define PWM_PULSE_WIDTH_STEP 45
#define PWN_PERIOD           4500

#define DEVICE_RC    0
#define DEVICE_LED   1

#define __DEVICE__   DEVICE_RC
//#define __DEVICE__   DEVICE_LED


#endif
