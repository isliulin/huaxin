#ifndef __KEY_H__
#define __KEY_H__

#include "stm8s.h"



//20171229
#define KEY_VARIANCE	3//8


#define KEY0_VALUE_MAX 	(62+KEY_VARIANCE)
#define KEY0_VALUE_MIN 	(62-KEY_VARIANCE)

#define KEY1_VALUE_MAX 	(52+KEY_VARIANCE)
#define KEY1_VALUE_MIN 	(52-KEY_VARIANCE)

#define KEY2_VALUE_MAX	(43+KEY_VARIANCE)
#define KEY2_VALUE_MIN	(43-KEY_VARIANCE)

#define KEY3_VALUE_MAX 	(34+KEY_VARIANCE)
#define KEY3_VALUE_MIN 	(34-KEY_VARIANCE)

#define KEY4_VALUE_MAX 	(25+KEY_VARIANCE)
#define KEY4_VALUE_MIN 	(25-KEY_VARIANCE)


#define KEY_CHANNEL 1
#define KEY_BAT_1   2
#define KEY_BAT_2   3
#define KEY_CONTROL 4
#define KEY_ON_OFF  5

#define KEY_DIR_LEFT	1
#define KEY_DIR_RIGHT	2

#define KEY_LEFT		7
#define KEY_RIGHT	9


#define KEY_READY  1
#define KEY_NULL    0

#define KEY_DIR_1 GPIO_ReadInputPin(GPIOB, GPIO_PIN_5)
#define KEY_DIR_2 GPIO_ReadInputPin(GPIOB, GPIO_PIN_4)

#define KEY_LOCK	11
#define KEY_PILOT	12

void KeyEventLoop( void );

#endif

