#include "sr101ef51a.h"	
#include "mn101ef52a.h"
#include"init.h"
#include "uart.h"	
//#include "Golbal.h"
#include "output.h"
#include "nikon.h"
/* MN101C SERIES C SAMPLE PROGRAM */
/*-------------------------------------------------------------------*/
#define ICR		*((volatile unsigned char *)0x3FE9)
/*-------------------------------------------------------------------*/
/*  Example of interrupt function parameter

	Timer 0 interrupt
	vector number = 9
	vector address = 0x4024
	ICR address ( 1 byte register ) = 0x3fe9

	ICR bit assignment
	bit0 = interrupt request flag
	bit1 = interrupt enable flag
	bit6,7 = interrupt level
		(If the value is smaller than the interrupt level in PSW,
			an interrupt occurs. )

	PSW bit assignment
	bit4,5 = interrupt mask level
	bit6 = interrupt mask flag
*/
/*-------------------------------------------------------------------*/
void main(void);
void not_use(void);
void initialize(void);
void Interrupt ( void );
void SysInitial();
extern void ComuProc();
extern void LoadUartDate();
extern void ClkSynInit(void);
void HighSpeedTriger(HpSetting *hps,unsigned int tmOv);
/*-------------------------------------------------------------------*/
//volatile int LoopCount = 0;
EXPOSE_PARA xdata flashPara={0};
FLASH_STATUE xdata flashStatue;
UART_SEND xdata uartSend={0};
unsigned char xdata bitChar=0;
unsigned char xdata bitFlag=0;
unsigned int xdata lowBatCount=0;
CAMER_SETTING xdata cameraSetting;
NiWireless xdata nikWireless={0};
Wl_set xdata wlSet;
MultiSet xdata multiSet;
unsigned char bitIndex;
unsigned char bitWireless;
unsigned char xdata ttl_pre2_flag;
unsigned char xdata uartRcvBuf[4];
unsigned char g_flagAftFlash=0;
UartSendFlag uartSendFlag={0};
HpSetting hpSetting={0};
unsigned char xdata tpfRsv=0;
/*-------------------------------------------------------------------*/
void WLM_CMDSEND_DELAY(unsigned int tm);
//#define ExactDelay(x) WLM_CMDSEND_DELAY(x)
void ExactDelay(unsigned int tm);
#define WLM_ADVAL 40//45//56//70//50//45//60//50//40//60//30//50//60
#define HP_DUTY_SET(x,y)	{TM7PR1=x;TM7PR2=y;}


void main(void){
	unsigned char tmp;
//	initialize();
	SysInitial();
//	Printf("panasonic for nikon 2012\n");
	//Printf("panasonic for nikon 2012\n");
	//Printf("panasonic for nikon 2012\n");
	//Printf("panasonic for nikon 2012\n");

	while(DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_MOTRESET,0));
	
/*
	flashPara.workmode = WORKINGMODE_RTN;
	wlSet.s_chan = WL_CHANNEL_ONE;
	wlSet.s_arrary = WL_CHANNEL_ONE;
	SET_FOLLOWFLASH_OPEN;
*/
	
//	while(1)
//	{
//		SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);
//		while(!(TM7ICR&0X01));
//		NIKONMOSI = !NIKONMISO;
//	}
/*	SET_SBO_IN;
//enable synclk receive	
	ENABLE_CLKSYN;
//	SET_NSS_OUTPUT;
	P0DIR |= 0X10; 
	P0PLU |= 0X10;
	while(1)
	{
		
		P0OUT4 = 1;
		HaltInterval(10);
		P0OUT4 = 0;
		HaltInterval(2);
		ClkSynRcv(&tmp,1);	
		HaltInterval(2);
		
	}
*/
	while(1)
	{
		//uartSend.sendFlag = 0xff;
		//LoadUartDate();
		ComuProc();
	}


	//for(;;){
	//	LoopCount++;
	//}
}
/*-------------------------------------------------------------------*/
void not_use(void){
	return;
}
/*-------------------------------------------------------------------*/
void initialize(void){
	ICR = 0x0;
	asm( "	mov 0xf0,D0" );
	asm( "	mov D0,PSW" );
}

void CpuMode()
{
	CPUM |= 0x10;
}

void SysInitial()
{
	CpuMode();
	Port_init();
	set_irq();   /*设置外部中断IRQ4*/
	TimersStart(); /*定时器0设置*/
	Timer7SetCount();//STAT TIMER7
	com_start();/*串口初始化*/
	//TurnOnInit();
	AD_init(1);
	ClkSynInit();
}
/*-------------------------------------------------------------------*/

#if(_TRIGER_MODE_==TRIGER_MODE_SW)
#define WML_TRIGER_VAL 10000
#define WML_CMPA		500
void TrigerMasterOn(unsigned int time)
{
#ifdef WLM_CMD_AD
	HP_SampleInit();
#endif
	
	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	//TM7PR2 = time; 
	//TM7PR1 = time;
	TM7MD2 |= 0X20;

//******************************
//	set flash trig
//******************************
	FLASH_PWM = 1;
	TM7PR1 = FLASH_TRIGER_TIME;	
	Delay();
	FLASH_TRIGER=1;	
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));

	FLASH_TRIGER=0;	
	TM7MD1 &= 0XEF;	
	TM7ICR &= 0xFE;

//******************************
//	ensure the master implus
//******************************
	TM7PR1 = WML_TRIGER_VAL;
	TM7MD1 |= 0X10;
	
	AD_SIMPLE_START;			//start AD simple

	while(!(TM7ICR&0X01))
	{
		if(!(ANCTR2&0x80))
		{
			AD_SIMPLE_START;
			
			if(ANBUF1>WLM_ADVAL)
			{
				//FLASH_TRIGER=0;
				//FLASH_PWM = 0;
				TM7MD1 &= 0XEF;	
				TM7ICR &= 0xFE;
				break;
			}//else
			//{
			//	AD_SIMPLE_START;
			//}
		}
	}
	
	if(time>WML_CMPA)
	{
		TM7PR1 = time-WML_CMPA;
		TM7MD1 |= 0x10;
		while(!(TM7ICR&0X01));
	}
	
	FLASH_PWM=0;

	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	TM7ICR &= 0XFE;	
	CHARGE_ENABLE=0;
	TM7PR2 = 0x00;
	MEMCTR &= ~0X04;	
	enable_irq();

#ifdef WLM_CMD_AD
	AD_init(0);	//recover voltage sampling
#endif
	//TM7PR1 = time;
}

void TURN_ON_TRIGER(unsigned int time)
{

//#ifdef CANON320EX_COPY
#ifdef EXT_LED
	if(flashPara.ledFlag)
	{
		return;
	}
#endif

	
	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	//TM7PR2 = time; 
	//TM7PR1 = time;
	TM7MD2 |= 0X20;

//#ifdef CANON320EX_COPY

	//if(flashPara.faceLite)
	//{
	//	LED_LITE = 1;
	//}
	FLASH_PWM = 1;
	TM7PR1 = FLASH_TRIGER_TIME;	
	Delay();
	FLASH_TRIGER=1;	
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));
	
	FLASH_TRIGER=0;	
	TM7MD1 &= 0XEF;	
	TM7ICR &= 0xFE;

	if(time<FLASH_TRIGER_TIME)
	{
		FLASH_PWM=0;
		FLASH_TRIGER=0;
		CHARGE_ENABLE=0;
		return;
	}
	
	TM7PR1 = time-FLASH_TRIGER_TIME;
	TM7MD1 |= 0X10;	
	//while(!(TM7OC2ICR&0X01));
	while(!(TM7ICR&0X01));

	FLASH_PWM=0;
	
	//if(flashPara.faceLite)
	//{
		LED_LITE	 = 0;
	//}

	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	TM7ICR &= 0XFE;	
	CHARGE_ENABLE=0;
	TM7PR2 = 0x00;
	MEMCTR &= ~0X04;	
	enable_irq();

	if(flashPara.workmode==WORKINGMODE_RTN)
	{
		Timer7SetCapture();
	}
}

void WLM_COMMAND_TRIGER(unsigned int time)
{
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
	if(flashPara.ledFlag)
	{
		return;
	}
#endif
//	disable_irq(); 
//	MEMCTR |= 0X04; 
	HP_SampleInit();

	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	TM7MD2 |= 0X20;
	//TM7PR2 = time; 
	//TM7PR1 = time;
	
//#ifdef CANON320EX_COPY
/*
	FLASH_PWM = 1;
	TM7PR1 = FLASH_TRIGER_TIME;
	Delay();
	FLASH_TRIGER=1;		
	TM7MD1 |= 0X10;	
	//while(!(TM7OC2ICR&0X01));
	while(!(TM7ICR&0X01));
	FLASH_TRIGER=0;	
	TM7MD1 &= 0XEF;	
	
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;	
	MEMCTR &= ~0X04;

	if(time<FLASH_TRIGER_TIME)
	{
		FLASH_PWM=0;
		FLASH_TRIGER=0;
		CHARGE_ENABLE=0;
		return;
	}
	
	TM7PR1 = time-FLASH_TRIGER_TIME;
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));
	FLASH_PWM=0;
*/
#ifdef WLM_CMD_AD
	//TM7PR1 = time;
	FLASH_PWM = 1;
	Delay();
	FLASH_TRIGER=1;
	TM7PR1 = time;
	TM7MD1 |= 0X10;
	Delay();
	Delay();
	Delay();
	Delay();
	AD_SIMPLE_START;			//start AD simple
	while(!(TM7ICR&0X01))
	{
		if(!(ANCTR2&0x80))
		{
			AD_SIMPLE_START;
			
			if(ANBUF1>WLM_ADVAL)
			{
				FLASH_TRIGER=0;
				FLASH_PWM = 0;
			}/*else
			{
				AD_SIMPLE_START;
			}*/
		}
	}
	FLASH_TRIGER=0;
	FLASH_PWM = 0;
#else
	TM7PR1 = time;
	FLASH_PWM = 1;
	Delay();
	FLASH_TRIGER=1;
	TM7MD1 |= 0X10;
	while(!(TM7ICR&0X01));
	FLASH_TRIGER=0;
	FLASH_PWM = 0;
#endif
	
	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;	
	MEMCTR &= ~0X04;

#ifdef WLM_CMD_AD
	AD_init(0);	//recover voltage sampling
#endif
	
	CHARGE_ENABLE=0;
//	MEMCTR &= ~0X04;
	TM7PR2 = 0x00;
//	enable_irq();
}

void HpPwmStart(unsigned int cycle,unsigned int duty )
{
	//TM7MD1 &= 0xEF;				//stop timer7
	TM7MD1 = 0x00;
					//set PA6 as special function
	PAOMD |= 0x40;				//set PA6 as TM7IOB
	PADIR |= 0x40;				//set PA6 as output			
	TM7MD2 = 0x70;				
	TM7MD1 = 0x00;				//set Timer7 prescale
	TM7MD3 = 0x00;
	
	TM7PR1 = cycle;				//set overflow timer
	TM7PR2 = duty;				//set signal vary
	TM7MD1 |= 0x10; 				//start timer7
}
#elif(_TRIGER_MODE_==TRIGER_MODE_TIME)
#define WML_TRIGER_VAL 10000
#define WML_CMPA		500
void TrigerMasterOn(unsigned int time)
{	
	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	//TM7PR2 = time; 
	//TM7PR1 = time;
	TM7MD2 |= 0X20;

//******************************
//	set flash trig
//******************************
	FLASH_PWM = 1;
	TM7PR1 = FLASH_TRIGER_TIME;	
	Delay();
	FLASH_TRIGER=1;	
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));

	FLASH_TRIGER=0;	
	TM7MD1 &= 0XEF;	
	TM7ICR &= 0xFE;

//******************************
//	ensure the master implus
//******************************
	TM7PR1 = WML_TRIGER_VAL;
	TM7MD1 |= 0X10;
	
	if(time>WML_CMPA)
	{
		TM7PR1 = time-WML_CMPA;
		TM7MD1 |= 0x10;
		while(!(TM7ICR&0X01));
	}
	
	FLASH_PWM=0;

	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	TM7ICR &= 0XFE;	
	CHARGE_ENABLE=0;
	TM7PR2 = 0x00;
	MEMCTR &= ~0X04;	
	enable_irq();
	//TM7PR1 = time;
}

void TURN_ON_TRIGER(unsigned int time)
{

//#ifdef CANON320EX_COPY
#ifdef EXT_LED
	if(flashPara.ledFlag)
	{
		return;
	}
#endif

	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	//TM7PR2 = time; 
	//TM7PR1 = time;
	TM7MD2 |= 0X20;


	TM7PR1 = time;
	FLASH_TRIGER=1;	
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));
	FLASH_TRIGER=0;
	
	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	TM7ICR &= 0XFE;	
	CHARGE_ENABLE=0;
	TM7PR2 = 0x00;
	MEMCTR &= ~0X04;	
	enable_irq();

	if(flashPara.workmode==WORKINGMODE_RTN)
	{
		Timer7SetCapture();
	}
}

void WLM_COMMAND_TRIGER(unsigned int time)
{
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
	if(flashPara.ledFlag)
	{
		return;
	}
#endif
//	disable_irq(); 
//	MEMCTR |= 0X04;

	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	TM7MD2 |= 0X20;
	//TM7PR2 = time; 
	//TM7PR1 = time;
	
//#ifdef CANON320EX_COP

	TM7PR1 = time;
	FLASH_TRIGER=1;	
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));
	FLASH_TRIGER=0;
	
	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;	
	MEMCTR &= ~0X04;

	CHARGE_ENABLE=0;
//	MEMCTR &= ~0X04;
	TM7PR2 = 0x00;
//	enable_irq();
}

#elif(_TRIGER_MODE_==TRIGER_MODE_HW)
#define 	I2C_DELAY			//Delay()
#define   IICSDA_OUT              	{P0DIR|=0x08;}
#define   IICSDA_IN               	{P0DIR&=0xF7;}

unsigned char IIcAck()
{
	unsigned char ack;
	IICSDA_IN;
	HP_I2C_SCK=1;
	ack = HP_I2C_SDA;
	HP_I2C_SCK=0;
	return ack;
}

unsigned char IIcStart()
{
	P0DIR |= 0x18;		//p03,p04 set as I2C port
	HP_I2C_SDA=1;
	HP_I2C_SCK=1;
	I2C_DELAY;
	HP_I2C_SDA=0;
	HP_I2C_SCK=0;
	I2C_DELAY;

	//return IIcAck();
	//ack	
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

unsigned char IicSendByte(unsigned char dat)
{
	//P0DIR |= 0x18;		//p03,p04 set as I2C port
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

void HpIicSendByte(unsigned char dat)
{
	//IIcStart();
	IicSendByte(dat);
	//IIcStop();
	
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

void HwTriger(unsigned int time)
{
	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	//TM7PR2 = time; 
	//TM7PR1 = time;
	TM7MD2 |= 0X20;

	FLASH_PWM = 1;
	TM7PR1 = FLASH_TRIGER_TIME;	
	Delay();
	FLASH_TRIGER=1;	
	TM7MD1 |= 0X10;	
	while(!(TM7ICR&0X01));
	
	FLASH_TRIGER=0;	
	TM7MD1 &= 0XEF;	
	TM7ICR &= 0xFE;

	if(time<FLASH_TRIGER_TIME)
	{
		FLASH_PWM=0;
		FLASH_TRIGER=0;
		CHARGE_ENABLE=0;
		return;
	}
	
	TM7PR1 = time-FLASH_TRIGER_TIME;
	TM7MD1 |= 0X10;	
	//while(!(TM7OC2ICR&0X01));
	while(!(TM7ICR&0X01));

	FLASH_PWM=0;
	
	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	TM7ICR &= 0XFE;	
	CHARGE_ENABLE=0;
	TM7PR2 = 0x00;
	MEMCTR &= ~0X04;	
	enable_irq();
}

void TURN_ON_TRIGER(unsigned int time)
{
#ifndef _NOFLASH_TEST_
	HC_CON3 = 1;
	HC_CON4 = 1;
	HpExpCodeSend(0xA2C0);
	HwTriger(time);	
#endif
}

void WlmCmdSet()
{
	HC_CON2 = 1;
	HC_CON3 = 1;
	//HC_CON4 = 0;
	HpExpCodeSend(0x0140);
	P2DIR &= 0xF7;
}

void WLM_COMMAND_TRIGER(unsigned int time)
{
	HC_CON4 = 0;
	TM7MD1 &= 0XEF; 		//disable timer7
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;			//select timer resource
	//TM7OC2ICR &= 0XFC; 		//clear the interrupt flag and disable intterrupt
	TM7ICR &= 0XFC;
	CHARGE_ENABLE=1; 		//disable charge
	TM7MD2 |= 0X20;
	//TM7PR2 = time; 
	//TM7PR1 = time;
	FLASH_PWM = 1;
	Delay();
	FLASH_TRIGER=1;
	TM7PR1 = time;
	
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;	
	MEMCTR &= ~0X04;
	
	TM7MD1 |= 0x10;
	Delay();
	Delay();
	Delay();
	Delay();
	
	while(!(TM7ICR&0x01))
	{
		if(!HP_MNT_PIN)
		{
			break;
		}
	}
	
	FLASH_TRIGER=0;
	FLASH_PWM = 0;
	HC_CON4 = 1;
	

	TM7MD1 &= 0XEF;
//	TM7OC2ICR &= 0XFC;
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;	
	MEMCTR &= ~0X04;

	CHARGE_ENABLE=0;
//	MEMCTR &= ~0X04;
	TM7PR2 = 0x00;
//	enable_irq();
}

void TrigerMasterOn(unsigned int time)
{}

void HighSpeedTriger(HpSetting *hps,unsigned int tmOv)
{
	HC_CON3 = 1;
	HC_CON4 = 0;
	HwTriger(tmOv);
	HC_CON4 = 1;
}

void HpPreflash(unsigned int tmOv)
{
	HpExpCodeSend(0xA2C0);
	HighSpeedTriger(&hpSetting,HP_PREFLASH_INTERVAL);	
}
#endif

#ifdef _HP_SYN_
#if(_HP_SYN_MODE_==HP_SYN_MODE_SW)
void HpPwmStop()
{
	//P0OMD1 &= ~0x40;				//set p01 as special function
	//P0OMD2 &= ~0x02;				//set P01 as TM7IOB
	TM7MD1 &= 0xEF;
	PAOMD &= ~0x40;
	FLASH_PWM = 0;
}


void SetTimerLinked(unsigned int st)
{
/*
	TM0MD &= 0xf7;
	TM1MD &= 0xf7;	//Stop timers
	TM0MD&=0XCF;	//timer0 set
	TM1MD|=0X10;	//set link
	TM0MD|=0X01;	//select prescale
	CK0MD &= 0XF0;	
	CK0MD |=0X06;	//enable prescale	fpll-div/64
	TM0OC=st>>8;
	TM1OC=st&0xff;
*/
	char i=0;

	TM0MD = 0X00;
	TM1MD = 0X00;
	TM1MD = 0X10;
	TM0MD = 0X01;
	CK0MD = 0X06;
	//TM0OC=st>>8;
	//TM1OC=st&0xff;
	TM01OC = st;
	
	//disable_irq(); 	//this work at interrupt closed
	MEMCTR |= 0X04; 
	//TM0ICR &=0XFC;	//Stop interrupt
	TM1ICR = 0x00;
	MEMCTR &= ~0X04;	
	//enable_irq();

	TM0MD |= 0x08;	

	//for(i=0;i<3;i++);
	
	
}

#define HP_INTVAL_START 200
#define HP_TRIGER_TIME	100

void HighSpeedTriger(HpSetting *hps,unsigned int tmOv)
{
	char exCnt=0;
	unsigned int expAcc=0;
	unsigned int accTmp=0;
	unsigned int sampleTmp = 0;
	unsigned int sampleRsv = 0;
	char staCnt = 0;
	char fpDir=0;
	unsigned int stepRcd=0;
	unsigned int stepTmp=0;
	//unsigned int sExpLev=0;
	
	//sExpLev = hps->adVal>>1;
	SET_SBO_IN;
	CHARGE_ENABLE = 1;				//charge disabled
	//if(flashPara.faceLite)
	//{
	//	LED_LITE	 = 1;
	//}

	disable_irq();

	HP_SampleInit();
	SetTimerLinked(tmOv);					//Set timer ovberflow
	
	FLASH_PWM = 1;
	Delay();
	FLASH_TRIGER=1;	
	ExactDelay(hps->head);
	FLASH_TRIGER=0;						//must turned off
	//FLASH_PWM = 0;
	//ExactDelay(100);
	//ExactDelay(hps->head);	
	//FLASH_TRIGER=0;
	AD_SIMPLE_START;			//start AD simple
	while(!(TM1ICR&0X01))
	{
		if(!(ANCTR2&0x80))
		{
			//if(ANBUF1>sExpLev)
			AD_SIMPLE_START;
			if(ANBUF1>=hps->initLev)
			{
				FLASH_TRIGER=0;
				FLASH_PWM = 0;
				break;
			}//else
			//{
			//	AD_SIMPLE_START;
			//}
		}
	}

	if(hps->accFlag)
	{
		ExactDelay(hps->hDelay);
		HpPwmStart(hps->sdt+hps->startVal,hps->sdt);
		while(!(TM1ICR&0x01))
		{
			if(!(ANCTR2&0x80))
			{
				AD_SIMPLE_START;
				if((TM7PR1-TM7PR2)<hps->realVal)
				{
					TM7PR1 += 15;
				}			
				sampleTmp += (MAKEWORD(ANBUF0, ANBUF1)>>6);
				if((++staCnt)>=16)
				{
					staCnt = 0;
					sampleTmp>>=4;
					//TM7PR2 -= 10;
					//TM7PR1 +=30;
					//if(TM7PR2<=hps->dt)
					//{
					//	break;
					//}
					if(sampleTmp>=hps->adVal)
					{
						break;
					}else
					{
						TM7PR2+=10;
					}
				}
				
			}
		}
		HP_DUTY_SET((hps->dt+hps->realVal),(hps->dt));
		
	}else
	{
		ExactDelay(hps->hDelay);
		HpPwmStart(hps->dt+hps->realVal,hps->dt);
	}
	//ExactDelay(hps->head);	
	//ExactDelay(100);

	//FLASH_PWM = 0;
	//SetTimerLinked(tmOv);					//Set timer ovberflow
	//FLASH_TRIGER=0;	
	//Delay();
	//Delay();
	//ExactDelay(hps->hDelay);
	//HpPwmStart(hps->dt+hps->val,hps->dt);
	//HpPwmStart(hps->dt+HP_INTVAL_START,hps->dt);
	//HpPwmStart(hps->sdt+hps->startVal,hps->sdt);hps->dt+hps->realVal,hps->dt
	//HpPwmStart(hps->dt+hps->realVal,hps->dt);

	//HP_SampleInit();

	//accTmp = (hps->dt)/5;
	
	//if(!accTmp)
	//{
	//	accTmp = 1;
	//}

	//SET_SBO_IN;

	//FLASH_TRIGER=0;
	//while((NIKONMISO)&&(!(TM1ICR&0x01)));
	//while(NIKONMISO)
	//{
	//	if(TM1ICR&0x01)
	//		break;
	//}

	//stepRcd = hps->step;

	//while(hps->adVal> (MAKEWORD(ANBUF0, ANBUF1)>>6));


/*
	while(!(TM1ICR&0x01))
	{
		if(!(ANCTR2&0x80))
		{
			AD_SIMPLE_START;
			if((TM7PR1-TM7PR2)<hps->realVal)
			{
				TM7PR1 += 20;
			}			
			sampleTmp += (MAKEWORD(ANBUF0, ANBUF1)>>6);
			if((++staCnt)>=16)
			{
				staCnt = 0;
				sampleTmp>>=4;
				//TM7PR2 -= 10;
				//TM7PR1 +=30;
				//if(TM7PR2<=hps->dt)
				//{
				//	break;
				//}
				if(sampleTmp>=hps->adVal)
				{
					break;
				}
			}
			
		}
	}

	sampleTmp = 0;

*/

	//HpPwmStart(hps->dt+hps->realVal,hps->dt);
	
	//HP_DUTY_SET((hps->dt+hps->realVal),(hps->dt));
	AD_SIMPLE_START;

	
	while(!(TM1ICR&0X01))
	{

/*	
		if(expAcc++>300)
		{
			TM7PR2 += accTmp;//(hps->dt)/4;
			TM7PR1 += accTmp;//(hps->dt)/4;
			expAcc = 0;
			FLASH_TRIGER=1;	
			staCnt ++;
			if(staCnt>=4)
			{
				TM7PR2 += accTmp;
				TM7PR1 += accTmp;
			}
			//WAKE_UP = !WAKE_UP;
		}else if(expAcc>20)
		{
			FLASH_TRIGER=0;	
		}
*/
#if 0
		if(!(ANCTR2&0x80))
		{
			AD_SIMPLE_START;
			sampleTmp += (MAKEWORD(ANBUF0, ANBUF1)>>6);
			if(++staCnt>=8)
			{
				staCnt =0;
				sampleTmp>>=3;
				//if(hps->adVal>(MAKEWORD(ANBUF0, ANBUF1)>>6))			//dack
				//if(hps->adVal>ANBUF1)
				if(hps->adVal>sampleTmp)
				{
					//if((TM7PR1-hps->step)>TM7PR2)
					if((TM7PR1-TM7PR2)>hps->step)					//the interval is larger than the fixed value
					{
						//TM7PR1 -= hps->step; 
						//TM7PR1 -=  hps->step;
						//TM7PR1 -= (simpleTmp - hps->adVal);
						//TM7PR1 -= ((hps->adVal-simpleTmp)>>3);
						stepTmp =  ((hps->adVal-sampleTmp)/hps->coef);
						//TM7PR1 -= ((hps->adVal-simpleTmp)/hps->coef);
						//if(stepTmp<hps->step)
						if((TM7PR1-TM7PR2)>stepTmp)
						{
							TM7PR1 -= stepTmp;
						}
						//TM7PR1 -= ((hps->adVal-(MAKEWORD(ANBUF0,ANBUF1)>>6)));
						//TM7PR1 -= 20;
					}else
					{
						//TM7PR1 = TM7PR2;
						TM7PR2 = (TM7PR1-hps->step);
					}
					/*
					if(2==fpDir)
					{
						hps->step >>= 2;
						if(hps->step<=0)
						{
							//hps->step = 10;
							hps->step=stepRcd;
						}
					}else
					{
						hps->step=stepRcd;
					}
					fpDir = 1;
					*/
				}//else if(hps->adVal<ANBUF1)
				//else if(hps->adVal<(MAKEWORD(ANBUF0,ANBUF1)>>6))		//light
				else if(hps->adVal<sampleTmp)
				{
					
					if(hps->val>(TM7PR1-TM7PR2))				//aviod too thick
					{
						//TM7PR1 += hps->step;
						//TM7PR1 +=  1;
						//TM7PR1 -= (MAKEWORD(ANBUF0,ANBUF1)>>6);
						//TM7PR1 += ((simpleTmp - hps->adVal)>>3);
						TM7PR1 += ((sampleTmp - hps->adVal)/hps->coef);
						//TM7PR1 += (((MAKEWORD(ANBUF0,ANBUF1)>>6)-hps->adVal));
						//TM7PR1 += 20;
					}
					/*
					if(1==fpDir)
					{
						hps->step >>= 2;
						if(hps->step<=0)
						{
							//hps->step = 10;
							hps->step=stepRcd;
						}
					}else
					{
						hps->step+=stepRcd;
					}
					fpDir = 2;
					*/
				}
				sampleTmp = 0;
				//AD_SIMPLE_START;
				//IGBT_CONTROL = !IGBT_CONTROL;
				/*
				if(ANBUF1<hps->adVal)
				{
					TM7PR1 += hps->step;
				}else 
				{
					TM7PR1 -= hps->step;
				}
				ANCTR2 |= 0x80;
				*/
			}
		}
#endif

#if 1
		if(!(ANCTR2&0x80))
		{
			AD_SIMPLE_START;
			sampleTmp += (MAKEWORD(ANBUF0, ANBUF1)>>6);
			//if(++staCnt>=8)
			if((++staCnt)>=4)
			{
				staCnt =0;
				//sampleTmp>>=3;
				sampleTmp>>=2;
				if(hps->adVal>sampleTmp)				//dack
				{
					stepTmp =  ((hps->adVal-sampleTmp)/hps->coef);
					if((TM7PR1-TM7PR2)>stepTmp)
					{
						TM7PR1 -= stepTmp;
						if((TM7PR1-TM7PR2)<hps->step)
						{
							//TM7PR2 = (TM7PR1-hps->step);
							TM7PR1 = TM7PR2 + hps->step;
						}
					}else
					{
						//TM7PR2 = (TM7PR1-hps->step);
						TM7PR1 = TM7PR2 +hps->step;
					}
					
#if 0					
					//if((TM7PR1-hps->step)>TM7PR2)
					//if(((TM7PR1-TM7PR2)>hps->step)&&(sampleRsv>=(sampleTmp-2)))				//the interval is larger than the fixed value
					if((TM7PR1-TM7PR2)>hps->step)
					{
						stepTmp =  ((hps->adVal-sampleTmp)/hps->coef);
						
						if((TM7PR1-TM7PR2)>stepTmp)
						{
							TM7PR1 -= stepTmp;
							//TM7PR2 += stepTmp;											//if dack, increase the duty
						}else
						{
							TM7PR2 = (TM7PR1-hps->step);
						}
						
					}else if((TM7PR1-TM7PR2)<=hps->step)
					{
						TM7PR2 = (TM7PR1-hps->step);
					}
#endif
				}//else if(hps->adVal<ANBUF1)
				//else if(hps->adVal<(MAKEWORD(ANBUF0,ANBUF1)>>6))		//light
				//else if((hps->adVal<sampleTmp)&&(sampleRsv<=(sampleTmp+2)))
				else if(hps->adVal<sampleTmp)
				{
					
					if(hps->val>(TM7PR1-TM7PR2))				//aviod too thick
					{
						TM7PR1 += ((sampleTmp - hps->adVal)/hps->coef);					//if light, increase the interval
					}
				}
				//POWER = !POWER;
				sampleRsv = sampleTmp;
				sampleTmp = 0;

			}
		}
#endif

/*		if(expAcc++>300)
		{
			//AFCTL = !AFCTL;
			if((TM7PR1-TM7PR2)<hps->val)
			{
				//TM7PR2 += 20;
				TM7PR1 += hps->step;
			}
			expAcc = 0;
		}
*/
		
		if(!NIKONMISO)
		{
			Delay();
			//Delay();
			if(!NIKONMISO)
			{
				exCnt++;
				if(exCnt>3)
				{
					break;
				}
			}else
			{
				exCnt = 0;
			}
		}
	}
	//while(NIKONMISO);

	HpPwmStop();

	enable_irq();

	TimersStart();

	AD_init(1);	//recover voltage sampling

	//if(flashPara.faceLite)
	//{
		LED_LITE	 = 0;
	//}

	CHARGE_ENABLE = 0;
/*	
	while(!(TM7ICR&0X01))
	{
#ifdef CANON320EX_COPY
		FLASH_PWM = 1;
#endif
		FLASH_TRIGER=1;
		ExactDelay(interval);
		FLASH_TRIGER=0;
#ifdef CANON320EX_COPY
		FLASH_PWM = 0;
#endif
		ExactDelay(50);	
		if(!NIKONMISO)
		{
			break;
		}
	}
	STOP_TIMER7;
*/	
}


#elif(_HP_SYN_MODE_==HP_SYN_MODE_HW)

#endif
#endif

void SET_TIMER7_OVERFLOW(unsigned scaleSel,unsigned int tm, char mode)
{
//	disable_irq(); 
//	MEMCTR |= 0X04; 
/*	TM7MD1 &= 0XEF;
	TM7ICR &= 0XFD;
	TM7MD2 |= 0X20;
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;
	TM7PR1 = tm;//0X0063;//0x07d0;//0X03E7;
	//TM7ICR |= 0X80;
	//TM7ICR |= 0X02;
	TM7MD1 |= 0X10;
	//TM7ICR &= 0xfe;
	//CLRBIT(TM7ICR,0);
	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7ICR &= 0xfe;
	MEMCTR &= ~0X04;	
	enable_irq();
*/
	TM7MD1 &= 0XEF;
	TM7MD1 &= 0XF0;
	if(TM7SCALE_LOW==scaleSel)
	{
		TM7MD1 |= 0X0C;
	}else
	{
		TM7MD1 |= 0X04;
	}
	TM7PR1 = tm;
	TM7MD2 |= 0X20;
	disable_irq(); 
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;	
	MEMCTR &= ~0X04;	
	enable_irq();
	TM7MD1 |= 0X10;	

	if(mode==TIMER_DELAY_SYN)
	{
		while(!(TM7ICR&0X01));
		TM7MD1 &= 0XEF;
		disable_irq(); 
		MEMCTR |= 0X04; 
		TM7ICR &= 0xfe;
		MEMCTR &= ~0X04;	
		enable_irq();
	}else
	{
		return;
	}
	
}

void WLM_CMDSEND_DELAY(unsigned int tm)
{
	TM7MD1 &= 0XEF;
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;
	TM7PR1 = tm;
	TM7MD2 |= 0X20;
	
	MEMCTR |= 0X04; 
	TM7ICR &= 0XFE;				//clear flag
	MEMCTR &= ~0X04;
	
	TM7MD1 |= 0X10;

	while(!(TM7ICR&0X01));
	TM7MD1 &= 0Xef;
	
	MEMCTR |= 0X04; 
	TM7ICR &= 0xfe;
	MEMCTR &= ~0X04;
	
	
}


void ExactDelay(unsigned int tm)
{
	TM7MD1 = 0x04;
	TM7PR1 = tm;
	TM7MD2 = 0x20;

	MEMCTR |= 0X04; 
	TM7ICR = 0x00;				//clear flag
	MEMCTR &= ~0X04;

	TM7MD1 |= 0X10;

	while(!TM7ICR);
	TM7MD1 = 0x00;
	
}


char READY_FOR_SIGNAL()
{
	NIKONMOSI=1;
	SET_SBO_IN;
#ifdef NIKCOM_TEST
	NIKONNSSO=1;
#endif
	/*puts("$");*/
/*	while(!NIKONSCKI)
	{
		if(!NIKONMISO)
		{
			NIKONMOSI = 1;
		}
	}
*/ 
//	if(g_flagAftFlash)
//	{
//		g_flagAftFlash = 0;
//		while(!NIKONMISO);
//	}
//	while(!NIKONSCKI);
//	P0DIR |= 0X08;
//	NIKONSCK = 1;
//	Delay_ms();
//	NIKONNSSO = 1;
//	SET_NSS_INPUT;
	while(NIKONMISO);
	if(WORKINGMODE_MASTER<flashPara.workmode)
	{
		//Delay_ms();
		return READY_SIGNAL_TIMEOUT;
	}
//	NIKONNSSO = 0;
//	HaltInterval(30);
	if((MULTI_STFLAG_START==multiSet.startFlag))
	{
		//MUL_PCATIMER_STOP
		multiSet.mulCtl = MULTI_FLASH_DISABLE;
		multiSet.finalExpTimes=0;
		multiSet.startFlag = MULTI_STFLAG_HOLD;
		uartSendFlag.clear = 1;
		uartSendFlag.flashWarn= 1;
		//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
		//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
		//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
		NIKONNSSO = 1;
		//InitUart();
	}
	SET_SBO_OUT;
	NIKONMOSI=0;/*puts("@@@"); */ 
	/*NIKONNSS=1;*/	
#ifdef NIKCOM_TEST
	NIKONNSSO=0;
#endif
	NIKONSCK=1;
//	NIKONNSSO=1;
	/*SET_TIMER_WAIT*/
	SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);
	while(!(TM7ICR&0X01))
	{
		if(!NIKONNSSI)
		{
			SET_TIMER_STOP;
			//HaltInterval(30);			//20120407 for flash debug
			break;
		}
	}	
	if(GETBIT(TM7ICR,0))
	{
//		SET_NSS_OUTPUT;
//		NIKONNSSO = 0;	
		disable_irq(); 
		MEMCTR |= 0X04; 
		TM7ICR &= 0xfe;
		MEMCTR &= ~0X04;	
		enable_irq();
		SET_TIMER_STOP
		NIKONMOSI = 1; 
		while(!NIKONMISO);
		//return;
		return READY_SIGNAL_TIMEOUT;
		//CLRBIT(TM7ICR,0);
	}
#ifdef NIKCOM_TEST
	NIKONMOSI = 0;
#else
	NIKONMOSI = 1; 
#endif

	return READY_SIGNAL_ARRIVAL;
/*	NIKONNSS = 0; */
	/*puts("*");*/
}


/*-------------------------------------------------------------------*/
