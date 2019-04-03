//#include <stm32f10x_lib.h>	   
#include "stm32f10x.h"
#include "led.h"
#include "../../user/golbal.h"
#include "../../user/canon.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved											  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PA8��PD2Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
/*
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 
	RCC->APB2ENR|=1<<5;    //ʹ��PORTDʱ��	
	   	 
	GPIOA->CRH&=0XFFFFFFF0; 
	GPIOA->CRH|=0X00000003;//PA8 �������   	 
    	GPIOA->ODR|=1<<8;      //PA8 �����
											  
	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000300;//PD.2�������
	GPIOD->ODR|=1<<2;      //PD.2����� 
}*/

extern unsigned char g_comStatue;

void PortInit(void)
{
	//RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 	 
	//RCC->APB2ENR|=1<<12;      //SPI1ʱ��ʹ�� 
	RCC->APB2ENR|=1<<2;//��ʹ������PORTAʱ��
 	RCC->APB2ENR|=1<<3;//��ʹ������PORTBʱ��
 	RCC->APB2ENR|=1<<4;//��ʹ������PORTCʱ��
#if(_HARD_WARE_!=HARD_WARE_RFONLY)
	RCC->APB2ENR |=1<<5;	//enable port D clock
#endif

#if(_HARD_WARE_==HARD_WARE_VER2)
	RCC->APB2ENR|=1<<6;
#endif


	//JTAG_Set(JTAG_SWD_DISABLE);
	
	//����ֻ���SPI�ڳ�ʼ��
	//GPIOA->CRL&=0X0000FF00; 
	//GPIOA->CRL|=0X83880088;//PA5.6.7����PA(0) PA(1)  Direction 	    
	//GPIOA->ODR|=0x5<<5;    //PA5.6.7����	
	//GPIOA->ODR&=0xFFBF;

	//RCC->APB2ENR|=1<<4;		//enable portc clock
#if(_HARD_WARE_==HARD_WARE_VER1)

	GPIOA->CRL&=0X0000FF00; 
	GPIOA->CRL|=0X83880088;//PA5.6.7����PA(0) PA(1)  Direction 
	GPIOA->ODR|=0x5<<5;    //PA5.6.7����	
	GPIOA->ODR&=0xFFBF;

	GPIOC->CRL&=0x0F000FFF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, COM_AUX
	GPIOC->CRL|=0x70338000;
	GPIOC->ODR&=0xFF6F;

	GPIOC->CRH = 0x33333388;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON
	GPIOC->ODR |= (3<<8);

	GPIOB->CRH = 0x33833333;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
	GPIOB->ODR = 0x0000;		//PB8		FLASH_CONTROL

	GPIOA->CRH&=0x00FFFFF0; 
	GPIOA->CRH|=0x38000003;//PAOUT15 CHARGE_LED  PAOUT14 PILOT_IN PAOUT8 READY	PAOUT13 COM_AUX implument the tride level voltage

	GPIOD->CRL&=0xFFFFF0FF;
	GPIOD->CRL|=0x00000300;
	
#endif

#if(_HARD_WARE_==HARD_WARE_VER2)

	GPIOA->CRL&=0X0000FF00; 
	GPIOA->CRL|=0X83880088;//PA5.6.7����PA(0) PA(1)  Direction 
	GPIOA->ODR|=0x5<<5;    //PA5.6.7����	
	GPIOA->ODR&=0xFFBF;

	GPIOC->CRL&=0x0FF00FFF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, PC5 INVOID
	GPIOC->CRL|=0x70038000;
	GPIOC->ODR&=0xFF6F;

	GPIOC->CRH = 0x33333388;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON
	GPIOC->ODR |= (3<<8);	

	GPIOB->CRH = 0x33833333;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
	GPIOB->ODR = 0x0000;		//PB8		FLASH_CONTROL

	GPIOA->CRH&=0x00FFFFF0; 
	GPIOA->CRH|=0x38000003;//PAOUT15 CHARGE_LED  PAOUT14 PILOT_IN PAOUT8 READY	PAOUT13 COM_AUX implument the tride level voltage

	GPIOD->CRL&=0xFFFFF000;
	GPIOD->CRL|=0x00000388;

	GPIOD->CRH&=0xFFFFF0FF;		//COM_AUX
	GPIOD->CRH|=0xFFFFF3FF;
	

#endif


#if(_HARD_WARE_==HARD_WARE_VER3)

	GPIOA->CRL&=0X0000FF00; 
	GPIOA->CRL|=0X83880038;//PA5.6.7����PA(0) PA(1)  Direction 
	GPIOA->ODR|=0x5<<5;    //PA5.6.7����	
	GPIOA->ODR&=0xFFBF;

	GPIOC->CRL&=0x0F000FFF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, COM_AUX
	GPIOC->CRL|=0x70338000;
	GPIOC->ODR&=0xFF6F;
	GPIOC->ODR|=0x20;			//CC2530 RESET

	GPIOC->CRH = 0x33333388;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON
	//GPIOC->ODR |= (3<<8);

	GPIOB->CRH = 0x33838833;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | ROTATE1 | ROTATE2 |BK_LIGHT | FLASH_PWM 
	GPIOB->ODR = 0x0C00;		//PB8		FLASH_CONTROL

	GPIOA->CRH&=0x000FFFF0; 
	//GPIOA->CRH|=0x38300003;//PAOUT15 CHARGE_LED  PAOUT14 PILOT_IN PAOUT8 READY	PAOUT13 COM_AUX implument the tride level voltage
	GPIOA->CRH|=0x38300003;


	GPIOD->CRL&=0xFFFFF0FF;
	GPIOD->CRL|=0x00000300;

	DISABLE_RF_MODULE();

//#ifdef UARTPIN_DEBUG
//	GPIOA->CRH&=0xFFFFF00F; 
//	GPIOA->CRH|=0x00000330;
//#endif
	
	
#endif


#if(_HARD_WARE_==HARD_WARE_RFONLY)

	JTAG_Set(JTAG_SWD_DISABLE);

	GPIOA->CRL&=0X0000FF00; 
	GPIOA->CRL|=0X83880038;//PA5.6.7����PA(0) PA(1)  Direction 
	GPIOA->ODR|=0x5<<5;    //PA5.6.7����	
	GPIOA->ODR&=0xFFBF;

	//GPIOA->CRH&=0x00FFFFF0; 
	//GPIOA->CRH|=0x38300003;//PAOUT15 CHARGE_LED  PAOUT14 PILOT_IN PAOUT8 READY	PAOUT13 COM_AUX implument the tride level voltage
	//GPIOA->CRH|=0x33000003;

	GPIOA->CRH&=0x00FFFFF0; 
	GPIOA->CRH|=0x83000003;
	GPIOA->ODR&=0x7FFF;

	//GPIOB->CRH &= 0x000F0000;
	//GPIOB->CRH |= 0x33808833;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | ROTATE1 | ROTATE2 |BK_LIGHT |LCD_RESET 
	//GPIOB->ODR |= 0x0C00;		//		FLASH_CONTROL

	GPIOB->CRH &= 0x000F00FF;
	GPIOB->CRH |= 0x33808800;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | ROTATE1 | ROTATE2 |BK_LIGHT |LCD_RESET 
	GPIOB->ODR |= 0x0C00;		//		FLASH_CONTROL


	//GPIOC->CRL&=0x0F00FFFF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, COM_AUX
	//GPIOC->CRL|=0x30330000;
	GPIOC->CRL&=0x0F0000FF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, COM_AUX
	GPIOC->CRL|=0x30333300;
	GPIOC->ODR&=0xFFCF;
	//GPIOC->ODR|=0x80;
	GPIOC->ODR|=0x0C;
	//GPIOC->ODR|=0xA0;			//CC2530 RESET

	GPIOC->CRH &= 0xFFFF0000;
	GPIOC->CRH |= 0x00003388;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON

	//GPIOD->CRL&=0xFFFFF0FF;
	//GPIOD->CRL|=0x00000300;

	//DISABLE_RF_MODULE();
	
#endif

	

	//SPI_MISO_AUX=1;
	//RCC->APB2ENR|=1<<3;
	//GPIOB->CRH&=0xFFFFFFF0;
	//GPIOB->CRH|=0x00000003;


// Rotary Encoder Switch interrupt set
#if(_HARD_WARE_==HARD_WARE_VER1)
	Ex_NVIC_Config(GPIO_A,1,FTIR); 
	MY_NVIC_Init(2,3,EXTI1_IRQChannel,3);
#elif(_HARD_WARE_==HARD_WARE_VER2)
	Ex_NVIC_Config(GPIO_D,1,FTIR); 
	MY_NVIC_Init(2,3,EXTI1_IRQChannel,3);
#elif(_HARD_WARE_==HARD_WARE_VER3)
	Ex_NVIC_Config(GPIO_B,10,FTIR); 
	MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);
#elif(_HARD_WARE_==HARD_WARE_RFONLY)
	Ex_NVIC_Config(GPIO_B,10,FTIR); 
	//Ex_NVIC_Config(GPIO_B,10,RTIR);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,3);
#endif
	

	//if(PAin(5))					//if detect the sck is high, turn on spi
	if(CLK_IN)
	{
		PAout(6) = 1;
		//OSTimeDlyHMSM(0,0,0,1);
		EXTIx_Init(FTIR);
		SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?					
		g_comStatue = COM_STATUE_START;
		
	}else
	{
		EXTIx_Init(RTIR);
		SPI_MISO_AUX  = 0;
		COM_AUX = 0;
		g_comStatue = COM_STATUE_IDLE;
	}

	
	FLAOK_OFF();
	//FLASH_PWM = 0;
	POW_ON = 1;
	//BK_LIGHT = 1;
	BK_LIGHT=0;
	
#if(_HARD_WARE_==HARD_WARE_RFONLY)
	AF_CTL_OUT = 0;
	KEY_BK = 1;
	UART_INFO = 1;
#else
	AF_CTL_OUT = 0;
#endif
	
	//while(1);

}


void PWM_Init(u16 arr,u16 psc,u16 duty)
{		 					 

	RCC->APB1ENR|=1<<1;       //TIM3ʱ��ʹ��    
	RCC->APB2ENR|=1<<4;
	RCC->APB2ENR|=1<<2;

	RCC->APB2ENR|=1<<0;

	AFIO->MAPR |= 3<<10;	//Timer3 remap
	GPIOC->CRL&=0xF0FFFFFF;//PC6���
	GPIOC->CRL|=0x0B000000;//���ù������ 	  
	GPIOC->ODR|=1<<6;//PC6����	

	  	

	TIM3->ARR=arr;//�趨�������Զ���װֵ 
	TIM3->PSC=psc;//Ԥ��Ƶ������Ƶ

	TIM3->CCR1 = duty;
	
	//TIM3->CCMR1|=7<<12;  //CH2 PWM2ģʽ		 
	//TIM3->CCMR1|=1<<11; //CH2Ԥװ��ʹ��	   
	TIM3->CCMR1|=7<<4;
	TIM3->CCMR1|=1<<3;

	//TIM3->CCER|=1<<4;   //OC2 ���ʹ��	   
	TIM3->CCER|=1<<0;   //OC1 ���ʹ��	   

	TIM3->CR1=0x0080;   //ARPEʹ�� 
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3 									  
}  





