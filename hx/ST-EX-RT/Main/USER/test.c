#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	
#include "includes.h"	 
#include "../HARDWARE/spi/spi.h"
#include "canon.h"
#include "Golbal.h"
#include "Display.h"
#include "../HARDWARE/lcd/Lcd.h"
#include "keyproc.h"
#include "../HARDWARE/lcd/font.h"
#include "../HARDWARE/TIMER/timer.h"


//任务堆栈
//OS_STK  TASK_CANCOM_STK[CANCOM_STK_SIZE];
OS_STK  TASK_KEYPROC_STK[KEYPROC_STK_SIZE];
OS_STK  TASK_START_STK[START_STK_SIZE];
OS_STK  TASK_LED_STK[LED_STK_SIZE];
OS_STK  TASK_REFRESH_STK[REFRESH_STK_SIZE];
OS_STK	TASK_CMDSEND_STK[CMDSEND_STK_SIZE];

//任务申明
void TaskStart(void *pdata);
void TaskCanCom(void *pdata);
void TaskKeyScan(void *pdata);
//void TaskLed1(void *pdata);
void TaskLed(void *pdata);
void TaskCmdSend(void *pdata);
extern void TaskDspUpdate(void *pdata);
void UserInit();

MainPara mainPara;
unsigned char g_dspStyle=DSP_STYLE_CANON;
//RadioSet radioSet={0};
RadioSet* radioSet = &mainPara.rfSet;
unsigned char g_grDspStart=0;
unsigned char g_grSel=0;
WL_ArrPara *mstPara = &(mainPara.modGr[0]);
RadioStatue radioStatue;
Wl_Slave wlSlave;
extern unsigned char *g_cmdMode;
extern MultiSet multiSet;
extern unsigned char uart_tran_flag;

//系统时钟配置函数
void SysTick_Configuration(void);

unsigned char g_comStatue=0;

void Flash_Interrupt_init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	//JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

	GPIOA->CRL&=0XFFF0FFFF;//PA4设置成输入	  
	GPIOA->CRL|=0X00080000;   				   
	GPIOA->ODR|=1<<4;	   //PA4上拉,PA0默认下拉

	Ex_NVIC_Config(GPIO_A,4,FTIR); //下降沿触发

	MY_NVIC_Init(2,2,EXTI4_IRQChannel,2);    //抢占2，子优先级2，组2
}
	 
int main(void)
{			 	   
	Stm32_Clock_Init(9); //系统时钟设置
	delay_init(72);	     //延时初始化 
	//uart2_init(36,115200);	 //只有USART1使用PCLK2(最高72MHz)。其它USART使用PCLK1(最高36MHz)
	//uart1_init(72,115200);
	//LED_Init();		  	 //初始化与LED连接的硬件接口
	//SPIx_Init();
	//printf("start");
	Adc_Init();
	PortInit();
	LCD_Init();
	Flash_Interrupt_init();
	uart2_init(36,115200);	 //只有USART1使用PCLK2(最高72MHz)。其它USART使用PCLK1(最高36MHz)
	UserInit();	
	
	//USART_Configuration();
	//Timerx_Init(10,7199);
	//EXTIx_Init(RTIR);
	SysTick_Configuration();
	OSInit();
	OSTaskCreate( TaskStart,	//task pointer
					(void *)0,	//parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
					START_TASK_Prio );	//task priority
	//printf("os start");	
	OSStart();
	return 0;	   
}


void UserInit()
{
	u8 i;
	LCD_Clear(BACK_COLOR);
	
	for(i=0;i<4;i++)
	{
		LcdFillBlank(
			LAYOUT_MENU_PAGE,
			i*(LAYOUT_BUTTON_WIDTH+LAYOUT_BUTTON_INTERVAL),
			LAYOUT_BUTTON_WIDTH,
			LAYOUT_BUTTON_HIGHT,
			DSP_NORMAL);			//menu fill just once
	}
	//LcdLoadPicture(0,LAYOUT_ETTL_X,LAY_WT_NOR,LAY_HT,ettl_EBitmap,DSP_NORMAL);
	//Draw_Circle(50,50,30);
	//LayModeEttl();
	memset((u8*)&mainPara,0x00,sizeof(MainPara));
	g_cmdMode = &mainPara.cmdMode;
	wlSet.m_arrPara = mainPara.modGr;
	wlSet.m_chan = &mainPara.m_chan;
	//wlSet.m_on = &mainPara.m_mstOn;
	wlSet.is_masterOn = &mainPara.m_isMstFla;
	wlSlave.s_array = &mainPara.s_array;
	wlSlave.s_chan = &mainPara.s_chan;
	motoSet.mode = &mainPara.motoLoca.mode;
	motoSet.dest_location = &mainPara.motoLoca.moto_location;
	wlSet.arr_mode = &mainPara.arr_mode;
	multiSet.eTimes = &mainPara.multi_times;
	multiSet.frequence = &mainPara.multi_Frq;
	multiSet.expose = &mainPara.modGr[MASTER].multi_exp;
	wlSet.abRatio = &wlSet.m_arrPara[WL_ARRARY_B].cmp1;

	//uartSendData("usart\r\n",sizeof("usart\r\n")-1);
	//RfPreFlashInfo();
	//while(uart_tran_flag);
	//RfPreFlashInfo();
	//*g_cmdMode = CMD_MODE_LIGH;
	Mode(mainPara.workMode);
	LoadButtons();

	CR_READY = 1;
	//BK_LIGHT = 1;
	
	
}

//开始任务
void TaskStart(void * pdata)
{
	pdata = pdata; 
	OS_ENTER_CRITICAL();   
	//OSTaskCreate(TaskCanCom, (void * )0, (OS_STK *)&TASK_CANCOM_STK[CANCOM_STK_SIZE-1], CANCOM_TASK_Prio);
	//OSTaskCreate(TaskCanCom, (void * )0, (OS_STK *)&TASK_CANCOM_STK[CANCOM_STK_SIZE-1], CANCOM_TASK_Prio);
	OSTaskCreate(TaskCmdSend,(void *)0, (OS_STK *)&TASK_CMDSEND_STK[CMDSEND_STK_SIZE-1],CMDSEND_TASK_Prio);
	OSTaskCreate(TaskKeyScan, (void * )0, (OS_STK *)&TASK_KEYPROC_STK[KEYPROC_STK_SIZE-1], KEYPROC_TASK_Prio);
	OSTaskCreate(TaskDspUpdate, (void *) 0,( OS_STK *) &TASK_REFRESH_STK[REFRESH_STK_SIZE-1],REFRESH_TASK_Prio);
	OSTaskCreate(TaskLed, (void * )0, (OS_STK *)&TASK_LED_STK[LED_STK_SIZE-1], LED_TASK_Prio);
	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
}
//任务1
//控制DS0的亮灭.
#define COM_TIME_OUT 20
void TaskCanCom(void *pdata)
{
	unsigned int comTick;
	while(1)
	{
	//OSTimeDlyHMSM(0,0,0,500);
		switch(g_comStatue)
		{
			case COM_STATUE_IDLE:
				if(PAin(5))
				{
					//TEST_SIGNAL = !TEST_SIGNAL;
					PAout(6) = 1;
					OSTimeDlyHMSM(0,0,0,1);
					EXTIx_Init(FTIR);
					SPIx_Init();
					g_comStatue = COM_STATUE_START;
				}
				OSTimeDlyHMSM(0,0,0,10);
				//SPI_MISO_AUX = 0;
			break;

			case COM_STATUE_START:
				if(!PAin(5))
				{
					comTick++;
					if(comTick>COM_TIME_OUT)
					{
						SPIx_Disable();
						//TEST_SIGNAL = !TEST_SIGNAL;
						g_comStatue = COM_STATUE_IDLE;
					}
					OSTimeDlyHMSM(0,0,0,10);
				}else
				{
					comTick=0;
					OSTimeDlyHMSM(0,0,0,10);
				}
			break;
			
		}
		
	}
}

//系统时钟中断服务函数
void SysTick_Handler(void)
{
	OS_ENTER_CRITICAL(); /* Tell uC/OS-II that we are starting an ISR  */
    	OSIntNesting++;
    	OS_EXIT_CRITICAL();

   	OSTimeTick();        /* Call uC/OS-II's OSTimeTick()               */

    	OSIntExit();         /* Tell uC/OS-II that we are leaving the ISR  */
}


//系统时钟配置，设计1ms产生一次中断
void SysTick_Configuration(void)
{
 	SysTick->CTRL&=~(1<<2);//SYSTICK使用外部时钟源
	SysTick->CTRL|=1<<1;   //开启SYSTICK中断
	SysTick->LOAD=9000;    //产生1ms中断
	//bit2清空,选择外部时钟  HCLK/8
	MY_NVIC_Init(3,3,SystemHandler_SysTick,2);//组2，最低优先级 
	SysTick->CTRL|=1<<0;   //开启SYSTICK
}


void TaskLed(void *pdata)
{
	while(1)
	{
		FLA_OK_LED=!FLA_OK_LED;
		OSTimeDlyHMSM(0,0,0,200);	
	}
}

unsigned char IIcAck()
{
	unsigned char ack;
	IICSDA_IN;
	HP_I2C_SCK=1;
	//ack = HP_I2C_SDA;
	ack = HP_I2C_SIN;
	HP_I2C_SCK=0;
	return ack;
}

unsigned char IIcStart()
{
	//P0DIR |= 0x18;		//p03,p04 set as I2C port
	HP_I2C_SDA=1;
	HP_I2C_SCK=1;
	I2C_DELAY;
	HP_I2C_SDA=0;
	HP_I2C_SCK=0;
	I2C_DELAY;

	//return IIcAck();
	//ack	
}

u8 HpIicSendByte(u8 dat)
{
	unsigned char i;
	unsigned char ack;
	
	IICSDA_OUT;

	for(i=0;i<8;i++)
	{
		HP_I2C_SCK=0;
		I2C_DELAY;
		if(dat&0x80)
		{
			HP_I2C_SDA=1;
		}else
		{
			HP_I2C_SDA=0;
		}
		I2C_DELAY;
		HP_I2C_SCK=1;
		I2C_DELAY;
		dat <<=1;
		I2C_DELAY;		
	}
	
	HP_I2C_SCK=0;
	HP_I2C_SDA=0;
	//IICSDA_IN;
	//I2C_DELAY;
	//HP_I2C_SCK=1;
	//ack=HP_I2C_SDA;
	//HP_I2C_SCK=0;
	ack = IIcAck();

	return ack;
}


void IIcStop()
{
	IICSDA_OUT;
	HP_I2C_SCK=0;
	I2C_DELAY;
	HP_I2C_SDA=0;
	I2C_DELAY;
	HP_I2C_SCK=1;
	I2C_DELAY;
	HP_I2C_SDA=1;	
}


void HpExpCodeSend(unsigned int hpv)
{
	IIcStart();
	HpIicSendByte(0xc0);
	HpIicSendByte(0x60);
	HpIicSendByte((hpv>>8)&0xff);
	HpIicSendByte(hpv&0xff);
	IIcStop();
}

void WlmCmdSet()
{
	HpExpCodeSend(CMD_THRESHOLD_VAL);
}

void WLM_SendChanhead(char chan)
{

	WlmCmdSet();

	switch(chan)
	{
		case (WL_CHANNEL_ONE-1):
			//Delay_10us(5);
			//Delay_10us(WLM_CHAN1_WIDTH);
			//WLM_HoldOn(WLM_CHAN1_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN1_WIDTH);
		break;

		case (WL_CHANNEL_TWO-1):
			//Delay_10us(9);
			//Delay_10us(WLM_CHAN2_WIDTH);
			//WLM_HoldOn(WLM_CHAN2_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN2_WIDTH);
		break;

		case (WL_CHANNEL_THREE-1):
			//Delay_10us(16);
			//Delay_10us(WLM_CHAN3_WIDTH);
			//WLM_HoldOn(WLM_CHAN3_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN3_WIDTH);
		break;

		case (WL_CHANNEL_FOUR-1):
			//Delay_10us(22);
			//Delay_10us(WLM_CHAN4_WIDTH);
			//WLM_HoldOn(WLM_CHAN4_WIDTH);
			WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_CHAN4_WIDTH);
		break;
	}
	//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
	WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
}

void WLM_SendData(char *Arr, char sendSize)
{
	char dataIndex=0;
	char bitIndex=0;

/*	if(wlSet.m_sendCmdEn!=1)
	{
		return;
	}
*/

	WLM_SendChanhead(*wlSet.m_chan);

	while(dataIndex<sendSize)
	{
		while(bitIndex<8)
		{
			//if((Arr[dataIndex]>>(7-bitIndex))&0x01)
			if((Arr[dataIndex]>>bitIndex)&0x01)			//Little endian
			{
				//WLM_TIMER0_DELAY(WLM_BIT1_INTERVAL);
				//Delay_10us(WLM_BIT1_INTERVAL);
				//WLM_HoldOn(WLM_BIT1_INTERVAL);
				//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
				WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
			}else
			{
				//WLM_TIMER0_DELAY(WLM_BIT0_INTERVAL);
				//Delay_10us(WLM_BIT0_INTERVAL);
				//WLM_HoldOn(WLM_BIT0_INTERVAL);
				delay_us(WLM_BIT0_INTERVAL);
			}
			bitIndex++;
		}
		bitIndex = 0;
		dataIndex++;
	}
 
}


void WLMCmdSend(u8 mod)
{
	switch(mod)
	{
		case CMD_MODE_LIGH:
		if(wlSet.m_cmdProcOn==WLM_CMDPROC_ON)
		{
			//Delayus(15);
			SPI_MISO_AUX = 0;
			delay_us(150);		
			OS_ENTER_CRITICAL();
			WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
			delay_us(100);		
			OS_EXIT_CRITICAL();
			delay_us(400);
			//disable_irq();	
			//MEMCTR   |=  0x04;
			//IRQ0ICR &= 0XFE;
			//IRQ1ICR &= 0XFE;
			//MEMCTR   &= ~0x04;
			//enable_irq();
			wlSet.m_cmdProcOn = WLM_CMDPROC_OFF;	
			SPI_MISO_AUX = 1;
			//FLASH_OUT = 1;
		}else
		{
			//Delay_10us(50);
			//FLASH_OUT = 1;
			SPI_MISO_AUX = 1;
		}
		break;

		case CMD_MODE_RF:
			if(1==radioStatue.cmdReady)
			{	
				switch(radioStatue.cmdSta)
				{
					case RADIO_STATUE_PRE:
						SPI_MISO_AUX = 0;
						RfPreFlashInfo();
						delay_ms(23);
						RfCountDownInfo(7000);
						delay_ms(1);
						SPI_MISO_AUX = 1;
						delay_ms(8);
						SPI_MISO_AUX = 0;
						delay_ms(1);
						SPI_MISO_AUX = 1;
						SPIx_Init();
						g_comStatue = COM_STATUE_START;
					break;

					case RADIO_STATUE_FLACFM:
						RfFlashConfirm();
						//SIGNAL = !SIGNAL;
						delay_us(24000);
						//Delay_ms(23);
						SPI_MISO_AUX = 1;
					break;

					case RADIO_STATUE_FLASH:
						RfCountDownInfo(6000);
						SPIx_Disable();
						delay_us(10000);
						//Delay_ms(1);
						SPI_MISO_AUX = 0;
						delay_us(1000);
						SPI_MISO_AUX = 1;
					break;

					case RADIO_STATUE_RECOVER:
						RfRecoverInfo();
					break;
				}
				radioStatue.cmdReady=0;
			}
		break;
	}
}

void TaskCmdSend(void *pdata)
{
	while(1)
	{
		WLMCmdSend(*g_cmdMode);
		OSTaskSuspend(CMDSEND_TASK_Prio);
		//OSTimeDlyHMSM(0,0,0,50);
	}
}

void EXTI4_IRQHandler(void)
{
	//CHARGE_LED = !CHARGE_LED;
	//printf("ext");
	//CHARGE_LED = 1;
	EXTI->PR=1<<4;
	

	switch(g_comStatue)
	{
		case COM_STATUE_REALFLASH:
			if(!CLK_IN)
			{
				switch(*g_cmdMode)
				{
					case CMD_MODE_LIGH:
						WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+WLM_BIT1_INTERVAL);
					break;

					case CMD_MODE_RF:
					break;

					case CMD_MODE_OFF:
					break;
				}				
				SPIx_Disable();
				Ex_NVIC_Config(GPIO_A,5,RTIR);
				Ex_NVIC_Enable(5);
				g_comStatue = COM_STATUE_IDLE;
			}
		break;

		case COM_STATUE_IDLE:
		break;

		case COM_STATUE_MULTIFLASH:
		break;

		case COM_STATUE_SLEEP:
		break;

		
	}
}


void TURN_ON_TRIGER(unsigned int time)
{
}









