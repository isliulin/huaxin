#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//Mini STM32������
//LED��������			 
//����ԭ��@ALIENTEK
//2010/5/27

//LED�˿ڶ���
#define LED0 PAout(8)// PA8
#define LED1 PDout(2)// PD2	

extern void EXTIx_Init(u8 GPIOx,u8 TirEdge);
extern void SPIx_Init(void);
void LED_Init(void);//��ʼ��		 				    

#endif

















