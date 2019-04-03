#ifndef __KEY_H__
#define __KEY_H__

#include "stm8s.h"



//20171229
#define KEY_VARIANCE	3//8


#define KEY0_VALUE_MAX 	(0x45+KEY_VARIANCE)//482//(uint)(479+3)//227//KEY RIGHT//1030    //用不到
#define KEY0_VALUE_MIN 		(0x45-KEY_VARIANCE)//(uint)(479-3)//.217//988     //用不到

//#define KEY1_VALUE_MAX 	(0x62+KEY_VARIANCE)//(0x46+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
//#define KEY1_VALUE_MIN 		(0x62-KEY_VARIANCE)//(0x46-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364
// 20171227 量产
#define KEY1_VALUE_MAX 	(0x66+KEY_VARIANCE)//(0x46+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
#define KEY1_VALUE_MIN 		(0x66-KEY_VARIANCE)//(0x46-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364

#define KEY2_VALUE_MAX		(0x8d+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(118+3)//379		//KEY LOCK
#define KEY2_VALUE_MIN		(0x8d-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(118-3)//369

#define KEY3_VALUE_MAX 	(0xae+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(347+3)//93//96		//KEY LEFT	//580   //按键2最大
#define KEY3_VALUE_MIN 		(0xae-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(347-3)//89//86		//500   //按键2最小512

#define KEY4_VALUE_MAX 	(0xd1+KEY_VARIANCE)//(uint)(223+3)//480//483		//KEY  MODE		//932   //按键3最大
#define KEY4_VALUE_MIN 		(0xd1-KEY_VARIANCE)//(uint)(223-3)//476//473		//	//800   //按键3最小892

//#define KEY5_VALUE_MAX 	(0xfd+KEY_VARIANCE)//(uint)(127+3)//385//388//720  KEY SEL //按键4最大
#define KEY5_VALUE_MAX 	(0xfd+2)
#define KEY5_VALUE_MIN 		(0xfd-KEY_VARIANCE)//(uint)(127-3)//381//378//600   //按键4最小680

#define KEY6_VALUE_MAX 	(0x21+KEY_VARIANCE)//(uint)(91+3)//349//352//720   KEY LED//按键4最大
#define KEY6_VALUE_MIN 		(0x21-KEY_VARIANCE)//(uint)(91-3)//345//342//600   //按键4最小680

#define KEY7_VALUE_MAX 	(0x20+KEY_VARIANCE)//(uint)(170+3)
#define KEY7_VALUE_MIN 		(0x20-KEY_VARIANCE)//(uint)(170-3)

#define KEY_RF_SW	1
#define KEY_FUNC1	2
#define KEY_FUNC2	3
#define KEY_FUNC3	4
#define KEY_MENU_SW		5
#define KEY_MODE	6
#define KEY_LEFT		7
#define KEY_OK		8
#define KEY_RIGHT	9
#define KEY_CLEAR	10
#define KEY_LOCK	11
#define KEY_PILOT	12

#define KEY_DIR_LEFT	1
#define KEY_DIR_RIGHT	2

#define KEY_READY  1
#define KEY_NULL    0

#define KEY_DIR_1 GPIO_ReadInputPin(GPIOB, GPIO_PIN_5)
#define KEY_DIR_2 GPIO_ReadInputPin(GPIOB, GPIO_PIN_3)


#endif

