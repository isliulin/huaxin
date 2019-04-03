//#include <stm32f10x_lib.h>
#include "stm32f10x.h"
#include "led.h"
#include "../../APP/golbal.h"
#include "../../APP/canon.h"
#include "../../APP/display.h"
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

//初始化PA8和PD2为输出口.并使能这两个口的时钟		    
//LED IO初始化
/*
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 
	RCC->APB2ENR|=1<<5;    //使能PORTD时钟	
	   	 
	GPIOA->CRH&=0XFFFFFFF0; 
	GPIOA->CRH|=0X00000003;//PA8 推挽输出   	 
    	GPIOA->ODR|=1<<8;      //PA8 输出高
											  
	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000300;//PD.2推挽输出
	GPIOD->ODR|=1<<2;      //PD.2输出高 
}*/

extern unsigned char g_comStatue;
extern char g_SettingIndex;
extern unsigned char uart_tran_flag;
extern void Flash_Interrupt_init(void);
extern RadioSet* radioSet;
extern signed char g_rfChanSet;

void PortInit(void)
{
	//static u8 sysInit=0;
	
	RCC->APB2ENR|=1<<2;       //PORTA时钟使能 	 
	//RCC->APB2ENR|=1<<12;      //SPI1时钟使能 
	//RCC->APB2ENR|=1<<2;//先使能外设PORTA时钟
 	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	RCC->APB2ENR|=1<<5;		//enable port D clock
	RCC->APB2ENR|=1<<6;		//enable port E clock

	JTAG_Set(JTAG_SWD_DISABLE);
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
#if 0								//for RGB chip debug
	
	//这里只针对SPI口初始化
	GPIOA->CRL&=0X0000FF00; 
	GPIOA->CRL|=0X83880088;//PA5.6.7复用PA(0) PA(1)  Direction 	    
	GPIOA->ODR|=0x5<<5;    //PA5.6.7上拉	
	GPIOA->ODR&=0xFFBF;

#endif

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

	GPIOA->CRL&=0X000FFF00; 
	GPIOA->CRL|=0X83800088;//PA5.6.7复用PA(0) PA(1)  Direction 
	//GPIOA->ODR|=0x01;
	GPIOA->ODR|=0x03;
	//GPIOA->ODR|=0x5<<5;    //PA5.6.7上拉	
	GPIOA->ODR&=0xFFBF;

	//if(sysInit>0)
	//{
	//	GPIOA->CRL&=0XFFFF00FF;
	//	GPIOA->CRL|=0X00008B00;//uart2 port set
	//}
#ifdef SLEEP_PWR_OFF
	GPIOA->CRH&=0X000FFFF0; 
	GPIOA->CRH|=0X88800003;
	GPIOA->ODR|=(1<<13);
	GPIOA->ODR|=(1<<14);
	GPIOA->ODR&=~(1<<15);
#else
	GPIOA->CRH&=0XF00FFFF0; 
	GPIOA->CRH|=0X08800003;
	//GPIOA->ODR|=(3<<13);
	GPIOA->ODR|=(1<<13);
	GPIOA->ODR|=(1<<14);
	GPIOA->ODR&=~(1<<15);
#endif

#ifdef SIGNAL_TEST
	GPIOA->CRH&=0xFFF00FFF;
	GPIOA->CRH|=0x00033000;
	GPIOA->ODR&=~(3<<11);
#endif

	//GPIOB->CRL &= 0x00000F00;
	//GPIOB->CRL |= 0x88333033;

	//GPIOB->CRL &= 0x00000F00;			//2015/05/31
	//GPIOB->CRL |= 0x88383000;
	//GPIOB->ODR |= (1<<3);

#ifdef MOTO_RESET_AD
	GPIOB->CRL &= 0x00000F00;			//2015/06/24
	GPIOB->CRL |= 0x83383000;
#else
	GPIOB->CRL &= 0x00000F00;			//2015/06/24
	GPIOB->CRL |= 0x83383080;
	GPIOB->ODR |= 0x02;
#endif

	GPIOB->ODR |= (1<<3);
	GPIOB->ODR &= ~(1<<6);
	//GPIOB->ODR |= (1<<6);

	GPIOB->CRH &= 0xF0F000F0;	//ACT_CMR | CTL_CMR | FB_INT | HP_CTL | EP_SDA | EP_SCK |BK_LIGHT | FLASH_PWM 
	GPIOB->CRH |= 0x03038803;
	GPIOB->ODR|=(1<<12);
	GPIOB->ODR&=~(1<<14);
	//GPIOB->ODR |= 0x0C00;

	GPIOC->CRL&=0x00000000;
	GPIOC->CRL|=0x38338000;
	GPIOC->ODR |= 0x08;

	//GPIOC->CRH&=0x0000000F;
	//GPIOC->CRH|=0x38383330;
	//GPIOC->ODR|=(1<<12);

	GPIOC->CRH&=0x0FF000F0;
	GPIOC->CRH|=0x30083303;
	GPIOC->ODR|=(1<<12);

	//GPIOD->CRL&=0xF0000000;		//COM_AUX
	//GPIOD->CRL|=0x08383833;

	GPIOD->CRL&=0x00000000;		//COM_AUX
	GPIOD->CRL|=0x38383833;
	GPIOD->ODR&=~(1<<7);

	GPIOD->CRH&=0x000000FF;		//COM_AUX
	GPIOD->CRH|=0x33333300;
	GPIOD->ODR |= 0xE000;


	//GPIOE->CRL&=0x0FFFFFFF;		//COM_AUX
	//GPIOE->CRL|=0x30000000;
	//GPIOE->CRL&=0x0FFFF000;		//2015/05/31
	//GPIOE->CRL|=0x30000333;
	//GPIOE->ODR |= (1<<2);			//TURN OFF FLASH_OK_LED
	//GPIOE->ODR |= 0xC000;
#ifdef FLASH_BASE_FIBER
	GPIOE->CRL&=0x00000000;		//2015/05/31
	GPIOE->CRL|=0x83333333;
	GPIOE->ODR &=0xFFF7;
#else
	GPIOE->CRL&=0x0000F000;		//2015/05/31
	GPIOE->CRL|=0x83330333;
#endif
	GPIOE->ODR |= (1<<2);			//TURN OFF FLASH_OK_LED
	GPIOE->ODR &=0xFF8F;

	GPIOE->CRH&=0x00000000;		//COM_AUX
	GPIOE->CRH|=0x33333333;		//PE14 ExtPA
	//GPIOE->ODR |= 0xC000;

#ifdef NBK600EX_DEBUG

	GPIOA->CRH&=0XFFFFFFF0; 
	GPIOA->CRH|=0X00000003;

	GPIOD->CRL&=0xFFFFF0FF;		//COM_AUX
	GPIOD->CRL|=0x00000300;

	GPIOA->CRH&=0XFF0FFFFF; 
	GPIOA->CRH|=0X00800000;
	GPIOA->ODR|=(3<<13);
	
#endif

	delay_ms(50);

	//BkLiteOn();
	if(KEY_POW_OFF>0)		////POWER UP	
	//if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
		BOOST_EN = 1;
		D3V3_EN = 0;
		EXT_CHARGE_CON = 1;
		CC2530_PWR = 1;			//FOR NEW board 2015/12/04
	}else
	{
		BOOST_EN = 0;
		D3V3_EN = 1;  
		CC2530_PWR = 0;            //FOR NEW board 2015/12/04
	}
#if 0
	if(!sysInit)
	{
		if(KEY_POW_OFF>0)		////POWER UP	
		{
			BOOST_EN = 1;
			D3V3_EN = 0;
		}else
		{
			BOOST_EN = 0;
			D3V3_EN = 1;
		}
	}else
	{
		if(KEY_POW_OFF>0)		////POWER UP	
		{
			BOOST_EN = 1;
			D3V3_EN = 0;
		}else
		{
			BOOST_EN = 0;
			D3V3_EN = 1;
		}
	/*
		if(KEY_POW_OFF>0)
		{
			RfMoudleSet();
		}
	*/
	}
#endif

	//sysInit = 1;

	//BOOST_EN = 1;
	//D3V3_EN = 0;
	//BOOST_EN = 0;
	//DISABLE_D30EN();
	HC_CON4 = 1;
	HC_CON3 = 1;					//第一次TTL爆闪
	FLASH_TIG = 0;
	//CHARGE_LED1_ON;
	CHARGE_LED2_OFF;

#ifdef RF_LOWPWR_SLEEP
	CC2530_RESET = 1;
#endif

	//HP_I2C_SDA=1;
	//HP_I2C_SCK=1;
	
#endif

	//if(KEY_POW_OFF>0)
	//{
	//	RfMoudleSet();
	//}

	//SPI_MISO_AUX=1;
	//RCC->APB2ENR|=1<<3;
	//GPIOB->CRH&=0xFFFFFFF0;
	//GPIOB->CRH|=0x00000003;


// Rotary Encoder Switch interrupt set
	Ex_NVIC_Config(GPIO_B,10,FTIR); 
	//Ex_NVIC_Config(GPIO_B,10,RTIR); 
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);				//2015/06/05
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,3);				//2015/06/05
	//DISABLE_KEY_INT();
	ENABLE_KEY_INT();

	Flash_Interrupt_init();

	//SetPreFlashDA();

	//TCS3414_Init();

	
#if 1                                              //for RGB chip debug

	//if(PAin(5))					//if detect the sck is high, turn on spi
	if(CLK_IN)
	{
		//PAout(6) = 1;
		SPI_MISO_AUX  = 1;
		//OSTimeDlyHMSM(0,0,0,1);
		//EXTIx_Init(FTIR);
		EXTIx_Init(GPIO_A,FTIR);
		SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
		g_cmrLinked = 1;
		g_comStatue = COM_STATUE_START;
		
	}else
	{
		//EXTIx_Init(RTIR);
		EXTIx_Init(GPIO_A,RTIR);
		SPI_MISO_AUX  = 0;
		COM_AUX = 0;
		g_comStatue = COM_STATUE_IDLE;
	}

	//BK_LIGHT1 = 1;
	//BK_LIGHT2 = 0;

	//FLASH_PWM = 0;
	//POW_ON = 1;
	//BK_LIGHT = 1;
	//BK_LIGHT=0;
	//AF_CTL_OUT = 1;
	
#endif
	

}

//void SavePara(void)
void SavePara(u32 sAddr)
{
	mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
	STMFLASH_Write(sAddr,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
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

/*
void SleepPortUninit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	DISABLE_KEY_INT();
	Ex_NVIC_Disable(5);
	Ex_NVIC_Disable(4);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//                
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//                
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOB, &GPIO_InitStructure);

	//BOOST_EN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//                
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
       GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//                
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
       GPIO_Init(GPIOA, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //
       GPIO_Init(GPIOA, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //下拉
       GPIO_Init(GPIOA, &GPIO_InitStructure);  	
	   
	GPIO_Write(GPIOA, 0X6003);
	GPIO_Write(GPIOB, 0X0001);
	GPIO_Write(GPIOC, 0X0000);
	GPIO_Write(GPIOD, 0X0400);
	GPIO_Write(GPIOE, 0X0000);
}*/

void PWM_Init(u16 arr,u16 psc,u16 duty)
{		 					 
#ifndef LCD_DRIVER_ILI9325
	RCC->APB1ENR|=1<<1;       //TIM3时钟使能    
	RCC->APB2ENR|=1<<4;
	RCC->APB2ENR|=1<<2;

	RCC->APB2ENR|=1<<0;	//辅助功能IO时钟

	AFIO->MAPR |= 3<<10;	//Timer3 remap
	GPIOC->CRH&=0xFFFFFF0F;//PC6输出
	GPIOC->CRH|=0x0B0000B0;//复用功能输出 	  
	GPIOC->ODR|=1<<9;//PC6上拉		  	

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
#endif
}  

//#define TCS3414_ADDRESS 0X39
#define 	RGB_I2C_DELAY(x)			delay_us(x)//Delay()
#define   RGB_IICSDA_OUT()              	{GPIOD->CRL &= 0XFFFFFF0F;GPIOD->CRL |= 0X00000030;}//PB2
#define   RGB_IICSDA_IN()                 	{GPIOD->CRL &= 0XFFFFFF0F;GPIOD->CRL |= 0X00000080;/*GPIOD->ODR|=1<<1;*/}
//#define   RGB_IICSDA_OUT()              	{GPIOD->CRL &= 0XFFFFFF00;GPIOD->CRL |= 0X00000033;}//PB2
//#define   RGB_IICSDA_IN()                 	{GPIOD->CRL &= 0XFFFFFF00;GPIOD->CRL |= 0X00000083;/*GPIOD->ODR|=1<<1;*/}

unsigned char RGB_IIcAck()
{
	unsigned char ack;
	RGB_IICSDA_IN();
	RGB_SCL=1;
	RGB_I2C_DELAY(1);
	ack = RGB_SDA_I;
	RGB_I2C_DELAY(1);
	RGB_SCL=0;
	RGB_I2C_DELAY(1);
	return ack;
}

unsigned char RGB_IIcStart()
{
	RGB_IICSDA_OUT();
	RGB_SDA_O = 1;
	RGB_I2C_DELAY(1);
	RGB_SCL = 1;
	RGB_I2C_DELAY(50);
	RGB_SDA_O = 0;
	RGB_I2C_DELAY(1);
	RGB_SCL = 0;
	RGB_I2C_DELAY(50);

	//return IIcAck();
	//ack
	return 0;	
}

void RGB_IIcStop()
{
	RGB_IICSDA_OUT();
	RGB_SCL = 0;
	RGB_I2C_DELAY(50);
	RGB_SDA_O = 0;
	RGB_I2C_DELAY(50);
	RGB_SCL = 1;
	RGB_I2C_DELAY(50);
	RGB_SDA_O = 1;	
	RGB_I2C_DELAY(1);
}

unsigned char RGB_IicSendByte(unsigned char dat)
{
	//P0DIR |= 0x18;		//p03,p04 set as I2C port
	unsigned char i;
	unsigned char ack;
	RGB_IICSDA_OUT();

	for(i=0;i<8;i++)
	{
		RGB_SCL = 0;
		RGB_I2C_DELAY(1);
		if(dat&0x80)
		{
			RGB_SDA_O = 1;
		}else
		{
			RGB_SDA_O = 0;
		}
		RGB_I2C_DELAY(1);
		RGB_SCL = 1;
		RGB_I2C_DELAY(1);
		dat <<=1;
		RGB_I2C_DELAY(1);		
	}

	RGB_I2C_DELAY(1);
	RGB_SCL = 0;
	RGB_I2C_DELAY(1);
	RGB_SDA_O = 0;
	//IICSDA_IN;
	//I2C_DELAY;
	//HP_I2C_SCK=1;
	//ack=HP_I2C_SDA;
	//HP_I2C_SCK=0;
	ack = RGB_IIcAck();

	return ack;
	
}

Uchar RGB_IicReadByte( Uchar ack ) 
{ 
	Uchar i; 
	Uchar data = 0; 


	RGB_SDA_O = 0;
	RGB_IICSDA_IN();    
	                                         
	for (i=0; i < 8; i++) 
	{ 
		RGB_SCL = 0;
		RGB_I2C_DELAY(1); 
		RGB_SCL = 1; 
		RGB_I2C_DELAY(1); 
		data <<= 1; 
		if (RGB_SDA_I) 
		{ 
		    ++data; 
		} 
		RGB_I2C_DELAY(1); 
	} 

	RGB_SCL = 0;                           
	RGB_I2C_DELAY(1); 

	RGB_IICSDA_OUT();   
	if (ack == 0) 
	{ 
		RGB_SDA_O = 0;
	} 
	else 
	{ 
		RGB_SDA_O = 1;
	} 
	RGB_I2C_DELAY(1); 
	RGB_SCL = 1;    
	RGB_I2C_DELAY(1); 
	RGB_SCL = 0; 
	//I2C_DELAY(1); 

	return ( data ); 
}

//#define TCS3414_WR	0
//#define TCS3414_RD  1
#define IIC_WR  0
#define IIC_RD   1

void Rgb_IIC_WriteByte(u8 address, u8 command, u8 data)
{
	RGB_IIcStart();
	//RGB_IicSendByte((address<<1)|TCS3414_WR);
	RGB_IicSendByte((address<<1)|IIC_WR);
	RGB_IicSendByte(command);
	RGB_IicSendByte(data);

	RGB_IIcStop();
	
}

void Rgb_IIC_ReadByte(u8 address, u8 command,u8* data)
{
	RGB_IIcStart();
	//RGB_IicSendByte((address<<1)|TCS3414_WR);
	RGB_IicSendByte((address<<1)|IIC_WR);
	RGB_IicSendByte(command);
	
	RGB_IIcStart();
	//RGB_IicSendByte((address<<1)|TCS3414_RD);
	RGB_IicSendByte((address<<1)|IIC_RD);
	
	*data = RGB_IicReadByte(0);

	RGB_IIcStop();
}

void Rgb_IIC_ReadWord(u8 address, u8 command,u16* data)
{
	RGB_IIcStart();
	//RGB_IicSendByte((address<<1)|TCS3414_WR);
	RGB_IicSendByte((address<<1)|IIC_WR);
	RGB_IicSendByte(command);
	
	RGB_IIcStart();
	//RGB_IicSendByte((address<<1)|TCS3414_RD);
	RGB_IicSendByte((address<<1)|IIC_RD);
	
	*data = RGB_IicReadByte(0);
	//*data<<=8;
	*data |= (RGB_IicReadByte(0)<<8);

	RGB_IIcStop();
	
}

#if(RGB_IC==RGB_TCS3414)

#define TCS3414_ADDRESS 0X39

void TCS3414_ReadChan(u16 *ch1, u16 *ch2, u16 *ch3, u16 *ch4)
{
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB0,ch1);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB2,ch2);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB4,ch3);
	//TCS3414_ReadWord(TCS3414_ADDRESS,0xB8,ch4);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB6,ch4);
}

void StartTcs3414(void)
{
	Rgb_IIC_WriteByte(TCS3414_ADDRESS,0x80,0x03);
	delay_us(10);
}

void StopTcs3414(void)
{
	Rgb_IIC_WriteByte(TCS3414_ADDRESS,0x80,0x00);
}

/*
u8 GetRgbCvtState(void)
{
	u8 rdDat=0;
	Rgb_IIC_ReadByte(TCS3414_ADDRESS,0x80,&rdDat);
	return (rdDat&0x10);
}
*/

/*
void ReadRgbChannals(u16* rgbs)
{
	TCS3414_ReadWord(TCS3414_ADDRESS,0xB0,rgbs);
	TCS3414_ReadWord(TCS3414_ADDRESS,0xB2,rgbs+1);
	TCS3414_ReadWord(TCS3414_ADDRESS,0xB4,rgbs+2);
	TCS3414_ReadWord(TCS3414_ADDRESS,0xB8,rgbs+3);
}
*/

void TCS3414_Init(void)
{
	u16 rcvDat=0;

	Rgb_IIC_WriteByte(TCS3414_ADDRESS,0x80,0x03);
	delay_us(10);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0x80,&rcvDat);

	delay_ms(15);

	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB0,&rcvDat);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB2,&rcvDat);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB4,&rcvDat);
	Rgb_IIC_ReadWord(TCS3414_ADDRESS,0xB8,&rcvDat);		
	
}

#elif(RGB_IC==RGB_BH1745)

#define BH1745_ADDRESS 0x38

void StartBH1745(void)
{
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x42,0x10);
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x44,0x02);
	delay_us(10);
}

void StopBH1745(void)
{
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x42,0x00);
}

void BH1745_ReadChan(u16 *ch1, u16 *ch2, u16 *ch3, u16 *ch4)
{
	Rgb_IIC_ReadWord(BH1745_ADDRESS,0x50,ch1);                   //RED
	Rgb_IIC_ReadWord(BH1745_ADDRESS,0x52,ch2);		     //GREEN
	Rgb_IIC_ReadWord(BH1745_ADDRESS,0x54,ch3);                  //BLUE
	//TCS3414_ReadWord(TCS3414_ADDRESS,0xB8,ch4);
	Rgb_IIC_ReadWord(BH1745_ADDRESS,0x56,ch4);                    //CLEAR
}

void BH1745_Init(void)
{
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x40,0x8B);	
	delay_ms(30);

	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x40,0x0B);
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x41,0x00);
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x42,0x00);
	Rgb_IIC_WriteByte(BH1745_ADDRESS,0x44,0x02);
	
}

#endif

void StartRgbSample(void)
{
#if(RGB_IC==RGB_TCS3414)
	StartTcs3414();
#elif(RGB_IC==RGB_BH1745)
	StartBH1745();
#endif
}

void StopRgbSample(void)
{
#if(RGB_IC==RGB_TCS3414)
	StopTcs3414();
#elif(RGB_IC==RGB_BH1745)
	StopBH1745();
#endif
}

u8 GetRgbCvtState(void)
{
	u8 rdDat=0;
#if(RGB_IC==RGB_TCS3414)
	Rgb_IIC_ReadByte(TCS3414_ADDRESS,0x80,&rdDat);
       return (rdDat&0x10);
	   
#elif(RGB_IC==RGB_BH1745)
       Rgb_IIC_ReadByte(BH1745_ADDRESS, 0x60, &rdDat);
       return (rdDat&0x80);
	   
#endif

}

void Rgb_ReadChan(u16 *ch1, u16 *ch2, u16 *ch3, u16 *ch4)
{
#if(RGB_IC==RGB_TCS3414)
	TCS3414_ReadChan(ch1,ch2,ch3,ch4);
#elif(RGB_IC==RGB_BH1745)
	BH1745_ReadChan(ch1,ch2,ch3,ch4);
#endif
}

void Rgb_Init(void)
{
#if(RGB_IC==RGB_TCS3414)
	TCS3414_Init();
#elif(RGB_IC==RGB_BH1745)
	BH1745_Init();
#endif
}



