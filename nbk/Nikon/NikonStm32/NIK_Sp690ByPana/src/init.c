#include "sr101ef51a.h"	
#include "mn101ef52a.h"
//#include "serial.h"
#include "uart.h"
#include "nikon.h"
#include "output.h"
#include "init.h"
//#include"oloong.h"
//#include "Golbal.h"

unsigned int Count_2ms;
unsigned int Count_IRQ4_Level;
unsigned char Buf_PA;
//void set_pwm(void);
void enable_irq(void);
void disable_irq(void);
void set_irq(void);
void TimersStart(void);
void TM0_stop(void);
void Port_init(void);
void TurnOnInit(void);
void AD_init(unsigned char swInt);

extern NiWireless xdata nikWireless;
extern UART_SEND xdata uartSend;
extern Wl_set xdata wlSet;
extern CAMER_SETTING xdata cameraSetting;
extern FLASH_STATUE xdata flashStatue;
extern MultiSet xdata multiSet;
extern EXPOSE_PARA xdata flashPara;
extern unsigned char charge_ok;
extern UartSendFlag uartSendFlag;

#if 0
/* ------------------------------------End ---------------------------------
 ===========================================================================

 函数名   :  void set_pwm(void)
 
 功能  :     PWM初始化16位精准模式PWM
 
 描叙:       PA6 PWM输出 1.6MS周期
 
 ===========================================================================*/
void set_pwm(void)
{
	PAOUT|=0x00;   /*pa口输出电平*/
	PAODC|= 0x00;   
	PAPLU|= 0x00;
	PADIR|=0x40;	/*PA6输出方式*/
					/*here set PA0 as a test output flag;*/    
	PAOMD|=0x40;   /*port PA6*/
	TM7MD2|=0x70;  /*TM7设置为PWM输出*/
	TM7MD1|=0x00;   /*时钟为1/1*/
	TM7PR1=0x8000;   /*设置周期0x8000*1/20M=1.6MS*/
	TM7PR2=0x2fff;  /*设置占空比0x6fff/0x8000*/
	TM7MD1|=0x10;    /*开定时器*/ 
}

/* ------------------------------------End ---------------------------------
 ===========================================================================

 函数名   :  void set_irq(void)
 
 功能  :     设置IRQ4为外部中断
 
 描叙:       
 
 ===========================================================================*/
 #endif
 
void set_irq(void)
{
	P2DIR &= 0xf8;
	P2PLU |= 0x07;
	
	disable_irq();
	MEMCTR    |=  0x04;		
	/*P2OUT=0x00;*/
	/*P2DIR=0x08;         
	P2PLU=0x07;         
	P2ODC=0x0c;    */
		       
	IRQCNT    |=  0x0F;   	
	//IRQ0ICR = 0XA2;
	IRQ1ICR = 0X82;
	CLRBIT(IRQ0ICR,0);
	//IRQ0ICR &= 0XFE;
	//IRQ1ICR &= 0XFE;
	//NF0CTR &= 0X10;
	//NF0CTR |= 0X10;
	/*IRQ1ICR=0x82;
	IRQ2ICR=0x82;
	IRQ3ICR=0x82;*/
	//IRQ4ICR=0x82; 
	MEMCTR   &= ~0x04;
	enable_irq();	

	disable_irq();
	MEMCTR    |=  0x04;	
	IRQ0ICR &= 0XFE;
	IRQ1ICR &= 0XFE;
	MEMCTR   &= ~0x04;
	enable_irq();	
}


void TimersStart(void)
{
	TM0MD = 0x01;			
	CK0MD = 0x08;	/*20M/4=5M*/	/*1/128 prescale*/
	TM0OC = (250-1); /*1/5M*250=50us*/			
	disable_irq();
	MEMCTR |=  0x04;		
	TM0ICR  =  0x82;	
	MEMCTR &= ~0x04;		
	enable_irq();
	STOP_TIMER0;
/*	TM0MD |= 0x08;	*/	/*开定时器*/


	TM1MD = 0X01;
	CK1MD = 0X06;
	TM1OC = (155-1);
	disable_irq();
	MEMCTR |= 0X04;
	TM1ICR = 0X82;
	MEMCTR &= ~0X04;
	enable_irq();
	TM1MD |= 0x08;

	
/*	TM7MD1 &= 0XEF;
	TM7ICR &= 0XFD;
	TM7MD2 |= 0X20;
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;
*/

}

void Timer7SetCount()
{
	TM7MD1 &= 0XEF;
	TM7ICR &= 0XFD;
	TM7MD2 |= 0X20;
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;
}

void Timer7SetCapture()
{
	TM7MD1 &= 0XEF;
	IRQ2ICR &= 0XFD;
	TM7MD2 |= 0X20;			//set clear source
	TM7MD1 &= 0xF0;			//set time source
	TM7MD1 |= 0X0c;
	TM7PR1 = 0XFFFF;		//set compare value
	TM7MD2 &= 0XFC;			
	TM7MD2 |= 0X02;			//caputure source, extern interrupt 2
	TM7MD1 &= 0XBF;
	TM7MD2 |= 0X80;			//caputure condition, falling edge
	TM7MD3 &= 0X7F;

	disable_irq();
	MEMCTR |= 0X04;
	IRQ2ICR = 0X82;
	MEMCTR &= ~0X04;
	enable_irq();

	TM7MD2 |= 0X04;
	TM7MD1 |= 0X10;	
	
}

void Delay_10us_sec ()
{
// this function cannot used int the wireless slave mode
	//bit preInt;

/*	TM1MD = 0X01;
	CK1MD = 0X00;
	
	TM1OC = 100;
	TM1BC = 0X00;

	TM1MD |= 0x08;
	

	for(i=0;i<ms;i++)
	{
		//disable_irq();
		while(!(TM1ICR&0x01));	
		TM1ICR &= 0xfe; 
		TM1BC = 0X00;
	} 
	
	TM1MD &= ~0x08;
*/

/*	TM7MD1 &= 0XEF;
	TM7ICR &= 0XFD;
	TM7MD2 |= 0X20;
	TM7MD1 &= 0XF0;
	TM7MD1 |= 0X04;
	TM7PR1 = 0X0063;//0x07d0;//0X03E7;
	TM7ICR |= 0X80;
	//TM7ICR |= 0X02;
	TM7MD1 |= 0X10;
	
	while(!(TM7ICR&0X01));
	disable_irq();
	MEMCTR |= 0X04;
	TM7ICR &= 0xfe;
	MEMCTR &= ~0X04;
	enable_irq();
*/
	SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,TENUS_INTERVAL,TIMER_DELAY_SYN);
	

/*	for(i=0;i<ms;i++)
	{
		while(!(TM7ICR&0X01));
	}
*/
	
   
}

void Delay_10us(char ms)
{
	//this cannot be used int wireless slave mode
	char i;                         // Millisecond counter
	for(i=0;i<ms;i++)
	{
		Delay_10us_sec();
	}

	if(WORKINGMODE_RTN==flashPara.workmode)
	{
		Timer7SetCapture();
	}
}

/* ------------------------------------End ---------------------------------
 ===========================================================================

 函数名   :  void TM0_stop(void)
 
 功能  :     定时器0停止
 
 描叙:       
 
 ===========================================================================*/
void TM0_stop(void)
{
	TM0MD &= ~0x08;			
}

void Port_init(void)
{
/*
	P2DIR = 0X01;
	P2PLU = 0X00;
	P2ODC = 0X00;
	
	P2OUT &= 0XFE; 
*/
//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
	FLASH_PWM = 0;			//aviod a small implus
	PADIR |= 0x52;
//	FLASH_PWM = 0;
	
//	P0DIR |= 0X07;			//Set Ready, triger, Power as Output
	P0DIR |= 0X01;
	P0DIR |= 0X04;
	
	FLASH_TRIGER= 0;		//aviod a small implus
	P0DIR |= 0X02;
	
	
	//P2DIR |= 0x28;
	P2DIR |= 0x20;
	P2DIR &= 0xf7;
	//P2DIR |= 0x02;
	//P2OUT2 |= 0x02;
	//P0LED |= 0x02;
#elif(NEW_DSP_SCHEME==_SP690II_)
	PADIR |= 0XE1; 			//set triger,power,charge_enable as pull_out
	P0DIR |= 0X01;
	P0PLU |= 0X1C;
	P2DIR |= 0x28;			//Set wakeup, external power control pull_out
#elif(NEW_DSP_SCHEME==_SP700_)
	FLASH_PWM = 0;
	FLASH_TRIGER= 0;
	PADIR |= 0xF1;		//set charge_en(PA0), CON2(PA4), POWER(PA5), FLASH_PWM(PA6), AF_EN(PA7) output
	P0DIR |= 0x1F;          //set IGBT_CONTROL(P00), READY(P01), CON4(P02), D_SDA(P03), D_SLK(P04)
	P2DIR |= 0x60;		//external charge control, triger output
	P3DIR |= 0x01;
#endif
	
//	P0DIR |= 0X0C;			//set sck data,nss as output pin
//	PAODC |= 0X40;
	
//	P0PLU |= 0X07;
//	P0PLU |= 0X08;
//	P0PLU |= 0X1C;

//#ifdef CANON320EX_COPY
//	P2DIR |= 0x28;
//#else
//	P2DIR |= 0X28;			//Set wakeup, external power control pull_out
//#endif
	
	SET_NSS_INPUT;
	

//	FLASH_TRIGER= 0;
	POWER = 1;
//	CHARGE_ENABLE = 1;
	WAKE_UP = 1;
	NIKONSCK = 0;
//	NIKONNSSO = 0;
	NIKONMOSI = 1;
//	EXTERN_CHARGE = 1;
//	IGBT_CONTROL = 0;
	IGBT_CONTROL = 1;


//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
	LED_LITE = 0;
	//FLASH_PWM = 0;
	CHARGE_ENABLE = 0;
#elif(NEW_DSP_SCHEME==_SP690II_)
	EXTERN_CHARGE = 1;
	CHARGE_ENABLE = 1;
#elif(NEW_DSP_SCHEME==_SP700_)
#endif

}

void HP_SampleInit()
{
	PADIR &= 0xfe;
	PAIMD |= 0x01;
	PAPLU &= 0xfe;

	ANCTR0 = 0x08;

	ANCTR1 = 0x00;		//PA0
	ANCTR2 = 0x00;

#ifdef HP_SAMPLE_TEST
	//disable_irq();
#endif
	MEMCTR |=  0x04;
	ADICR  = 0x00;                      /* Enable the interrupt, Interrupt level 2 */
	MEMCTR &= ~0x04;	
#ifdef HP_SAMPLE_TEST
	//enable_irq();
#endif

	ANCTR2 |= 0x80;
	//while(ANCTR2&0x80);
	
}

void AD_init(unsigned char swInt)
{
/*-----------------------------------------------
    A/D conversion setting
-----------------------------------------------*/
//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
	PADIR &= 0xDF;
	PAIMD |= 0x20;
	PAPLU &= 0xDF;
#elif(NEW_DSP_SCHEME==_SP690II_)
	PADIR  &= 0xFD;                      /* Set PA input */
	PAIMD  |= 0x02;                      /* Set PA7 A/D IN */
	PAPLU  &= 0xFD;                      /* Set PA no pull-up */
#elif(NEW_DSP_SCHEME==_SP700_)
	PADIR  &= 0xFD;                      /* Set PA input */
	PAIMD  |= 0x02;                      /* Set PA7 A/D IN */
	PAPLU  &= 0xFD;                      /* Set PA no pull-up */
#endif

    ANCTR0 = 0x08;                      /* Set sample and hold time */
                                        /* Set conversion clock */
                                        /* A/D ladder resistance ON */
//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
	ANCTR1 = 0x05;
	ANCTR2 = 0x00;
#elif(NEW_DSP_SCHEME==_SP690II_)
	ANCTR1 = 0x01;                      /* Select analog input channel */
	ANCTR2 = 0x00;                      /* Stop A/D converter */
	                                    /* Select starting factor */
#elif(NEW_DSP_SCHEME==_SP700_)
	ANCTR1 = 0x01;                      /* Select analog input channel */
	ANCTR2 = 0x00;                      /* Stop A/D converter */
#endif

	if(swInt)
	{
		disable_irq();
		MEMCTR |=  0x04;
		ADICR  = 0x82;                      /* Enable the interrupt, Interrupt level 2 */
		MEMCTR &= ~0x04;		
		enable_irq();
	}else
	{
		MEMCTR |=  0x04;
		ADICR  = 0x82;                      /* Enable the interrupt, Interrupt level 2 */
		MEMCTR &= ~0x04;	
	}

	ANCTR2 |= 0x80;

	
}

#if 0
void TurnOnInit()
{
//	int i;

#ifdef NONE_PRESERVE_MODE	
#ifdef NIKON_WIRELESS_VERSION
	flashPara.workmode = WORKINGMODE_RTN;
#endif
//	flashPara.workmode = 0;
//	flashPara.output1 = 0;
//	flashPara.output2 = 0;
	flashPara.TTL_LEVEL1 = 0;
	flashPara.TTL_LEVEL2 = 0;
	flashPara.SyncMode = 0;		
//	flashPara.ttlcomp1 = 0;
//	flashPara.ttlcomp2 = 0;
	flashPara.camera_comp1 = 0;
	flashPara.camera_comp2 = 0;	 
	flashPara.ttlFeb1 = 0;
	flashPara.ttlFeb2 = 0;
	flashPara.music_set = 0;
	flashPara.surrand_state = 0;
//	flashPara.moto_mode = 0;
//	flashPara.moto_location = 0;
#endif

//	nikWireless.WL_command = 0;
	nikWireless.WL_output1= 0;
	nikWireless.WL_output2= 0;
	nikWireless.WL_opCode= 0;
	nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	nikWireless.WL_Ttl = 0;
	nikWireless.WLBufIndex = 0;
	nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
	nikWireless.preTurn=WL_PFCOUNT_RDY;
	nikWireless.WL_buf[0] = nikWireless.WL_buf[1] = nikWireless.WL_buf[2] = nikWireless.WL_buf[3] = 0;
		

	flashStatue.sleep = 0;
	flashStatue.camera_state = 0;
	flashStatue.flash_enable = 0;
	flashStatue.s2_count = 0;
	flashStatue.auxr_light = 0;
	flashStatue.redEye_ctr = 0;
	flashStatue.full_flash = 0;

	uartSend.sendFlag = 0;
	uartSend.sendFlagEx = 0;
	uartSend.focLen = 0;
	uartSend.chargeOK = 0;
	uartSend.cls = 0;
	uartSend.ISO = 0;
	uartSend.apert= 0;
	uartSend.cls_invalid = 0;
	uartSend.tempAlarm = 0;
	uartSend.ISO_index = 0;
	uartSendFlag.index = 0;
	uartSend.mode = 0;

	cameraSetting.rcvCode=0;
	cameraSetting.camera_cmp1=0;
	cameraSetting.camera_cmp2=0;
	cameraSetting.rcvHandShake=0;
//	cameraSetting.wl_chan=WL_CHANNEL_ONE;
//	cameraSetting.wl_arrary=WL_ARRARY_C;

	wlSet.s_arrary = WL_ARRARY_A;
	wlSet.s_chan = WL_CHANNEL_ONE;
	wlSet.m_chan = WL_CHANNEL_ONE;
	wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
	wlSet.s_wlMode	= WORKINGMODE_TTL;
	//wlSet.m_chan = WL_CHANNEL_ONE;
	wlSet.m_flaOn = WLM_FLASH_OFF;
	wlSet.m_slpara[0]=wlSet.m_slpara[1]=wlSet.m_slpara[2]=0;
//	wlSet.m_charge_statue = 0;
	wlSet.m_sendCmdEn = 0;
	wlSet.isRear = WLM_ISREAR_FRONT;

	//wlSet.m_chan = WL_CHANNEL_ONE;
	//wlSet.chan = WL_CHANNEL_ONE;

	wlSet.m_arrPara[WL_ARRARY_A].mode = WORKINGMODE_TTL;
	wlSet.m_arrPara[WL_ARRARY_B].mode = WORKINGMODE_TTL;
	wlSet.m_arrPara[WL_ARRARY_C].mode = WORKINGMODE_TTL;
	
	wlSet.m_arrPara[WL_ARRARY_A].cmp1=0;
	wlSet.m_arrPara[WL_ARRARY_A].cmp2=0;
	wlSet.m_arrPara[WL_ARRARY_B].cmp1=0;
	wlSet.m_arrPara[WL_ARRARY_B].cmp2=0;
	wlSet.m_arrPara[WL_ARRARY_C].cmp1=0;
	wlSet.m_arrPara[WL_ARRARY_C].cmp2=0;

	wlSet.m_arrPara[WL_ARRARY_A].output1=0;
	wlSet.m_arrPara[WL_ARRARY_A].output2=0;

	wlSet.m_arrPara[WL_ARRARY_B].output1=0;
	wlSet.m_arrPara[WL_ARRARY_B].output2=0;

	wlSet.m_arrPara[WL_ARRARY_C].output1=0;
	wlSet.m_arrPara[WL_ARRARY_C].output2=0;

	multiSet.eTimes = 20;
	multiSet.expose = 0x01;
	multiSet.frequence = 20;
	multiSet.finalExpTimes = 0;
	multiSet.lowFrqCnt = 0;
	multiSet.startFlag = MULTI_STFLAG_HOLD;

	//charge_ok = AD_CHARGE_RDY;
	//flashStatue.flash_enable = 0x01;

}
#endif





