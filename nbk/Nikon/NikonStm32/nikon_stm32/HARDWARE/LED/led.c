//#include <stm32f10x_lib.h>
#include "stm32f10x.h"
#include "led.h"
#include "../../APP/Nikon.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/5/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved											  
////////////////////////////////////////////////////////////////////////////////// 	   

void Flash_Interrupt_init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	//JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

	GPIOA->CRL&=0XFFF0FFFF;//PA4设置成输入	  
	GPIOA->CRL|=0X00080000;   				   
	GPIOA->ODR|=1<<4;	   //PA4上拉,PA0默认下拉

	Ex_NVIC_Config(GPIO_A,4,FTIR); //下降沿触发

	//MY_NVIC_Init(2,2,EXTI4_IRQChannel,2);    //抢占2，子优先级2，组2
	MY_NVIC_Init(2,2,EXTI4_IRQn,2);    //抢占2，子优先级2，组2
}

void PortInit(void)
{
	//static u8 sysInit=0;
#ifdef ROTATE_DEBUG
	GPIO_InitTypeDef GPIO_initStructure;
#endif
	
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 	 
	//RCC->APB2ENR|=1<<12;      //SPI1时钟使能 
	//RCC->APB2ENR|=1<<2;//先使能外设PORTA时钟
 	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	RCC->APB2ENR|=1<<5;		//enable port D clock
	RCC->APB2ENR|=1<<6;		//enable port E clock

	JTAG_Set(JTAG_SWD_DISABLE);

#if(_HARD_WARE_==HARD_WARE_TSER)

//	GPIOC->CRL&=0x0FF00FFF;	//AUX_SPI_OUT , CMD_THRESHOLD_INT, PC5 INVOID
//	GPIOC->CRL|=0x70038000;
//	GPIOC->ODR&=0xFF6F;

//	GPIOC->CRH = 0x33333388;	//LCD_CTRLS | POW_ON | CHARGE | KEY_LOCK |KEY_ON
//	GPIOC->ODR |= (3<<8);	

	//GPIOB->CRH = 0x33833333;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
	//GPIOB->ODR = 0x0000;		//PB8		FLASH_CONTROL

//	GPIOB->CRH &= 0xFFFF0000;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
//	GPIOB->CRH |= 0x00008833;
//	GPIOB->ODR = 0x0C00;		//PB8		FLASH_CONTROL

//	GPIOB->CRL &= 0XFFF0000F;		//RGB CHIP
//	GPIOB->CRL |= 0X00033330;
//	GPIOB->ODR &= 0xFFE1;

//	GPIOA->CRH&=0xFFFFFF0F; 
//	GPIOA->CRH|=0x00000080;		//PILOT PA(1)

//	GPIOA->CRH&=0x00FFFFF0; 
//	GPIOA->CRH|=0x38000003;//PAOUT15 CHARGE_LED  PAOUT14 PILOT_IN PAOUT8 READY	PAOUT13 COM_AUX implument the tride level voltage

//	GPIOD->CRL&=0xFFFFF000;
//	GPIOD->CRL|=0x00000388;

	GPIOA->CRL&=0X000F0F00; 
	//GPIOA->CRL|=0X83808088;//PA5.6.7复用PA(0) PA(1)  Direction 
	GPIOA->CRL|=0X83803088;
	//GPIOA->ODR|=0x01;
	GPIOA->ODR|=0x03;
	GPIOA->ODR|=0x08;
	//GPIOA->ODR|=0x5<<5;    //PA5.6.7上拉	
	//GPIOA->ODR&=0xFFBF;

	GPIOA->CRH&=0X000FFFF0; 
	GPIOA->CRH|=0X88800003;
	GPIOA->ODR|=(1<<13);
	GPIOA->ODR|=(1<<14);
	GPIOA->ODR&=~(1<<15);

	GPIOB->CRL &= 0x0FF00F00;			//2015/06/24
	GPIOB->CRL |= 0x80083080;
	GPIOB->ODR |= 0x02;

	//GPIOB->ODR |= (1<<3);
	//PIOB->ODR &= ~(1<<6);
	//GPIOB->ODR |= (1<<6);

	GPIOB->CRH &= 0xF0F00000;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
	GPIOB->CRH |= 0x03038883;
	GPIOB->ODR|=(1<<12);
	GPIOB->ODR&=~(1<<14);
	//GPIOB->ODR |= 0x0C00;

	GPIOC->CRL&=0x00FF0000;
	GPIOC->CRL|=0x38008000;
	GPIOC->ODR |= 0x08;

	//GPIOC->CRH&=0x0000000F;
	//GPIOC->CRH|=0x38383330;
	//GPIOC->ODR|=(1<<12);

	GPIOC->CRH&=0xFFF000F0;
	GPIOC->CRH|=0x00083303;
	GPIOC->ODR|=(1<<12);

	//GPIOD->CRL&=0xF0000000;		//COM_AUX
	//GPIOD->CRL|=0x08383833;

	GPIOD->CRL&=0xF0000000;		//COM_AUX
	GPIOD->CRL|=0x08383833;
	//GPIOD->ODR&=~(1<<7);

	GPIOD->CRH&=0x0F0000FF;		//COM_AUX
	GPIOD->CRH|=0x30333300;
	GPIOD->ODR |= 0xA000;

	GPIOE->CRL&=0xF0000F00;		//2015/05/31
	GPIOE->CRL|=0x03333033;
	//GPIOE->ODR &=0xFFF7;

	GPIOE->ODR |= (1<<2);			//TURN OFF FLASH_OK_LED
	GPIOE->ODR &=0xFF8F;

	GPIOE->CRH&=0x000000F0;		//COM_AUX
	GPIOE->CRH|=0x33333303;		//PE14 ExtPA
	//GPIOE->ODR |= 0xC000;

#ifdef ROTATE_DEBUG
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_initStructure);
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSEConfig(RCC_LSE_OFF);
	//BKP_TamperPinCmd(DISABLE);
	//BKP_ITConfig(DISABLE);
	//GPIOC->CRH &= 0xF00FFFFF;
	//GPIOC->CRH &= 0xF33FFFFF;
	
#endif


	HC_CON4 = 1;
	HC_CON3 = 1;					//第一次TTL爆闪
	FLASH_TIG = 0;
	FLASH_PWM = 0;

	delay_ms(50);

	//BkLiteOn();
	if(KEY_POW_OFF>0)		////POWER UP	
	//if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
		BOOST_EN = 1;
		D3V3_EN = 0;
		EXT_CHARGE_CON = 1;
		//CC2530_PWR = 1;			//FOR NEW board 2015/12/04
		//SENSOR_SW = 1;
		SENSOR_SW = 0;
	}else
	{
		BOOST_EN = 0;
		D3V3_EN = 1;  
		//CC2530_PWR = 0;            //FOR NEW board 2015/12/04
	}

	CHARGE_LED2_OFF();
	
#endif



// Rotary Encoder Switch interrupt set
	Ex_NVIC_Config(GPIO_B,10,FTIR); 
	//Ex_NVIC_Config(GPIO_B,10,RTIR); 
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);				//2015/06/05
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,3);				//2015/06/05
	//DISABLE_KEY_INT();
	ENABLE_KEY_INT();
	//Flash_Interrupt_init();

	Flash_Interrupt_init();

	//SetPreFlashDA();

	//TCS3414_Init();

}

void InitKeyInt(void)
{
 	GPIOB->CRH &= 0xFFFF00FF;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
	GPIOB->CRH |= 0x00008800;
	GPIOB->ODR|=(3<<10);
	
	// Rotary Encoder Switch interrupt set
	Ex_NVIC_Config(GPIO_B,10,FTIR); 
	//Ex_NVIC_Config(GPIO_B,10,RTIR); 
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);				//2015/06/05
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,3);				//2015/06/05
	//DISABLE_KEY_INT();
	ENABLE_KEY_INT();
	//Flash_Interrupt_init();
}

void PWM_Init(u16 arr,u16 psc,u16 duty)
{		 					 
	RCC->APB1ENR|=1<<1;       //TIM3时钟使能    
	RCC->APB2ENR|=1<<4;
	RCC->APB2ENR|=1<<2;

	RCC->APB2ENR|=1<<0;	//辅助功能IO时钟

	AFIO->MAPR |= 3<<10;	//Timer3 remap
	GPIOC->CRH&=0xFFFFFF0F;//PC6输出
	GPIOC->CRH|=0x0B0000B0;//复用功能输出 	  
	GPIOC->ODR|=1<<9;//PC9上拉		  	

	TIM3->ARR=arr;//设定计数器自动重装值 
	TIM3->PSC=psc;//预分频器不分频

	//TIM3->CCR1 = duty;
	TIM3->CCR4 = duty;
	
	//TIM3->CCMR1|=7<<12;  //CH2 PWM2模式		 
	//TIM3->CCMR1|=1<<11; //CH2预装载使能	   
	//TIM3->CCMR1|=7<<4;
	TIM3->CCMR2|=7<<12;
	//TIM3->CCMR1|=1<<3;
	TIM3->CCMR2|=1<<11;

	//TIM3->CCER|=1<<4;   //OC2 输出使能	   
	//TIM3->CCER|=1<<0;   //OC4 输出使能
	TIM3->CCER|=1<<12;

	TIM3->CR1=0x0080;   //ARPE使能 
	TIM3->CR1|=0x01;    //使能定时器3 									  
	
}  

void PortUninit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	DISABLE_KEY_INT();
	Ex_NVIC_Disable(5);
	Ex_NVIC_Disable(4);
/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//                ／／配置ＰＡ１３上拉输入 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //上拉输入
       GPIO_Init(GPIOA, &GPIO_InitStructure);

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//                ／／配置ＰＡ１３上拉输入 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //上拉输入
       GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//                ／／配置ＰＡ１３上拉输入 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //上拉输入
       GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//                ／／配置ＰＡ１３上拉输入 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //上拉输入
       GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//                ／／配置ＰＡ１３上拉输入 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //上拉输入
       GPIO_Init(GPIOE, &GPIO_InitStructure);
*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_Init(GPIOE,  &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;     //FOR NEW board 2015/12/04
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOB, &GPIO_InitStructure);
	   
	GPIO_Write(GPIOA, 0X6000);
	GPIO_Write(GPIOB, 0X0001);
	GPIO_Write(GPIOC, 0X0000);
	GPIO_Write(GPIOD, 0X0000);
	GPIO_Write(GPIOE, 0X0000);
	

#if 0
	
	GPIOA->CRL=0X88888888;
	GPIOA->CRH=0X88888888;
	//GPIOA->ODR =0x0000;
	GPIOA->ODR = 0X6000;

	GPIOB->CRL=0X88888888;
	GPIOB->CRH=0X88838888;
	//GPIOB->ODR&=~(1<<14);		//
	//GPIOB->ODR =0x0000;
	GPIOB->ODR =0x0001;

	GPIOC->CRL=0X88888888;
	GPIOC->CRH=0X88888388;
	GPIOC->ODR =0x0000;

	GPIOD->CRL=0X88888888;
	GPIOD->CRH=0X88888388;
	GPIOD->ODR =0x0000;

	GPIOE->CRL=0X88888888;
	GPIOE->CRH=0X88888888;
	//GPIOE->ODR &=0xFF8F;
	GPIOE->ODR =0x0000;
#endif

	//g_SettingIndex = 0;
	//radioSet->id = g_rfChanSet;
	//radioSet->rfChan = g_rfChanSet;

	//memset(radioStatue.devCnt,0x00,5);
}




