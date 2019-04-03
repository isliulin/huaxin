#ifndef __KEY_H__
#define __KEY_H__

#include "stm8s.h"



//20171229
#define KEY_VARIANCE	3//8


#define KEY0_VALUE_MAX 	(252+KEY_VARIANCE)//482//(uint)(479+3)//227//KEY RIGHT//1030    //用不到
#define KEY0_VALUE_MIN 	(252-KEY_VARIANCE)//(uint)(479-3)//.217//988     //用不到

#define KEY1_VALUE_MAX 	(209+KEY_VARIANCE)//(0x46+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
#define KEY1_VALUE_MIN 	(209-KEY_VARIANCE)//(0x46-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364

#define KEY2_VALUE_MAX	(173+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(118+3)//379		//KEY LOCK
#define KEY2_VALUE_MIN	(173-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(118-3)//369

#define KEY3_VALUE_MAX 	(140+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(347+3)//93//96		//KEY LEFT	//580   //按键2最大
#define KEY3_VALUE_MIN 	(140-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(347-3)//89//86		//500   //按键2最小512

#define KEY_CHANNEL 1
#define KEY_BAT_1   2
#define KEY_BAT_2   3
#define KEY_CONTROL 4

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

