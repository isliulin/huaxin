#include "sys.h"
#include "usart.h"
#include "../../APP/Golbal.h"
#include "../../APP/Display.h"
#include "../../APP/Canon.h"
#include "../../APP/FlashPara.h"

#define	MAX_UART_DATA_LEN	100//64
unsigned char gBUartTotalByte=0;
unsigned char   gBUartLen=0;
unsigned char	gBUartBuf[MAX_UART_DATA_LEN];
extern FlaParameter flaPara;
extern MainPara mainPara;
extern unsigned char uart_tran_flag;
extern RadioSet* radioSet;
extern RadioStatue radioStatue;
extern unsigned char g_shutterFlag;
extern u16 g_dspUpdateFlag;
extern unsigned char g_comStatue;
extern char g_chargeOk;
extern char g_isChScan;
extern void RfParaSend(void);
u8 g_rfSlaveFlashCode=0;
u8 g_expTTLPara3=0;
u8 g_rfSlavePreOn = 0;
u8 g_isRfSlaveLinked=0;
extern signed char g_expTTLPara1;
extern signed char g_expTTLPara2;
extern MultiSet multiSet;
extern CanWireless canWireless;
extern u8 g_isModeling;
//extern Word gWSleepCounter;
extern u32 gWSleepCounter;
//char g_slaveReady=0;
s8 g_slaveReady=0;

void UartRcvHandle(u8 *rcv, u8 len);
extern unsigned char RfSlaveZeroTimes(void);
extern int MultiZero(unsigned char mExp);

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
#define UART_RCVBUF_LEN 64
unsigned char rcvBuf[UART_RCVBUF_LEN];
  
void USART2_IRQHandler(void)
{
	u8 res;	    
	static u8 loaden=0;
	static u8 rcvLen=0;
	static u8 bufIndex=0;
	//u16 usart_sr;
	
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR; 
		
		if(!loaden)
		{
			if(0xaa==res)
			{
				loaden = 1;
			}
		}else if(1==loaden)
		{
			rcvLen = res;
			loaden = 0;
			if(rcvLen>UART_RCVBUF_LEN)
			{
				rcvLen = 0;
				loaden = 0;
				return;
			}
			loaden = 2;				
		}else if(2==loaden)
		{
/*
			if(bufIndex<(rcvLen-1))
			{
				rcvBuf[bufIndex++] = res;
			}else
			{
				rcvBuf[bufIndex++] = res;
				UartRcvHandle(rcvBuf,rcvLen);
				rcvLen = 0;
				loaden = 0;
				bufIndex = 0;
			}
*/
			rcvBuf[bufIndex++] = res;
			if(bufIndex>=rcvLen)
			{
				UartRcvHandle(rcvBuf,rcvLen);
				rcvLen = 0;
				loaden = 0;
				bufIndex = 0;
			}
		}
	}
	
	//if(USART2->SR&(1<<6))		//send complete
	if(USART2->SR&(1<<6))	
	{
		if(uart_tran_flag)
		{
			if(gBUartLen<gBUartTotalByte)
			{
				USART2->DR = gBUartBuf[gBUartLen];
				gBUartLen++;
			}else
			{
				gBUartLen = 0x00;
				gBUartTotalByte = 0x00;				
				uart_tran_flag	    = 0;
				USART2->CR1&=~(1<<6);
				UART_INT = 0;
			}
		}
	}
	USART2->SR = USART2->SR;
	
	//usart_sr = USART2->SR;
	//USART2->SR = usart_sr;
}

unsigned char RfMcodeCovert(unsigned char src, u8 hpMod)
{
	if(HP_FLASHMODE_OFF==hpMod)
	{
		src = 0x38 -src;
		return (src + 0x88);
	}else
	{
		src = 0x38 -src;
		return (src + 0x68);
	}
}

unsigned char RfMcodeInvert(unsigned char src, u8 hpMod)
{
	if(!src)
	{
		return 0;
	}
	
	if(HP_FLASHMODE_OFF==hpMod)
	{
		//src = 0x38 -src;
		if(src>=0xC0)
		{
			src = 0x38;			
		}else if(src>=0x88)
		{
			src -= 0x88;
		}else
		{
			src = 0x00;
		}
		//return (src + 0x88);
		return (0x38-src);
	}else
	{
		//src = 0x38 -src;
		if(src>=0xA0)
		{
			src = 0x38;
		}else if(src>=0x68)
		{
			src -= 0x68;
		}else
		{
			src = 0x00;
		}
		//return (src + 0x68);
		return (0x38-src);
	}
}


unsigned char CalcSum(u8 *rcv, u8 len)
{
	u8 i=0,sum=0;
	for(i=0;i<len;i++)
	{
		sum+=rcv[i];
	}
	return sum;
}

u8 RfSlaveMcodeInvert(u8 mCode,u8 hpMod,signed char* e1,signed char *e2)
{
	u8 index;

	if(mCode>0x2A)
	{
		mCode = 0x2A;
	}

	mCode = 0x2A - mCode;
	mCode /= 2;
	*e1 = mCode/3;
	*e2 = mCode%3;

	//return index-1;
	return NULL;
	
	
}

u8 RfSlaveMulticodeInvert(u8 mCode,u8 hpMod,signed char* e1,signed char *e2)
{

	if(mCode>0x2A)
	{
		mCode = 0x2A;
	}

	mCode = 0x2A - mCode;
	mCode /= 2;
	*e1 = mCode/3;
	*e2 = mCode%3;

	//return index-1;
	return NULL;
	
	
}


//void RfSlaveFlashCalc(void)
void RfSlaveFlashCalc(signed char mod)
{
	//switch(mainPara.workMode)
	switch(mod)
	{
		case WORKINGMODE_TTL:
			g_rfSlaveFlashCode+=((mstPara->cmp1*3+mstPara->cmp2)*3);			//2015/08/22 for slave compensation
			CanExposeTimeCal(g_rfSlaveFlashCode,g_expTTLPara3);
		break;

		case WORKINGMODE_MANU:
			//RfSlaveMcodeInvert(g_rfSlaveFlashCode,g_expTTLPara3,&mstPara->output1,&mstPara->output2);
			RfSlaveMcodeInvert(g_rfSlaveFlashCode,g_expTTLPara3,(signed char *)&canWireless.WL_output1,(signed char *)&canWireless.WL_output2);
			//g_expTTLPara1 = mstPara->output1;					//use golbal var as expose parameter
			//g_expTTLPara2 = mstPara->output2;
			g_expTTLPara1 = canWireless.WL_output1;
			g_expTTLPara2 = canWireless.WL_output2;
		break;

		case WORKINGMODE_MULTI:
			//RfSlaveMcodeInvert(g_rfSlaveFlashCode, 0, multiSet.expose, NULL);
			//RfSlaveMcodeInvert(g_rfSlaveFlashCode, 0, (signed char *)&canWireless.wlMul_expose, NULL);
			RfSlaveMulticodeInvert(g_rfSlaveFlashCode, 0, (signed char *)&canWireless.wlMul_exp1, (signed char *)&canWireless.wlMul_exp2);
			//*multiSet.expose<<=4;
			//*multiSet.frequence = WlMultiFrqConver(canWireless.wlMul_frq);
			//*multiSet.eTimes = WlMultiTimeConver(canWireless.wlMul_time);
			//canWireless.wlMul_frq = WlMultiFrqConver(canWireless.wlMul_frq);
			//canWireless.wlMul_time = WlMultiTimeConver(canWireless.wlMul_time);
			/*
			if(!canWireless.wlMul_time)
			{
				canWireless.wlMul_time = MultiZero(canWireless.wlMul_expose);
				canWireless.wlMul_time = RfSlaveZeroTimes();
			}*/
			//g_expTTLPara1 = *multiSet.frequence;
			//g_expTTLPara2 = *multiSet.eTimes;
			//g_expTTLPara3 = *multiSet.expose;		
			g_expTTLPara1 = canWireless.wlMul_exp1;
			g_expTTLPara2 = canWireless.wlMul_time;
			g_expTTLPara3 = canWireless.wlMul_frq;
		break;

		case WORKINGMODE_EXTA:
		break;

		
	}

	//g_dspUpdateFlag |= DSP_UPDATE_MODE;
}

u8 IsSlaveModValid(signed char mod)
{
	switch(mod)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_MANU:
		case WORKINGMODE_EXTA:
		case WORKINGMODE_MULTI:
			return 1;
		//break;

		default:
			return 0;
	}
}


//calculate the lights available
s8 SlaveStatueCheck(void)
{
	u8 i=0;
	s8 devTotal=0;
	u8 validGr=0;

	//mainPara.workMode,&wlSet,

	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			switch(*wlSet.ttl_Ratio)
			{
				case WLM_MODE_ALL:
					validGr = 3;
				break;

				case WLM_MODE_AB:
					validGr = 2;
				break;

				case WLM_MODE_RES:
					validGr = 3;
				break;
			}
		break;

		case WORKINGMODE_MANU:
		case WORKINGMODE_MULTI:
			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
					validGr = 3;
				break;

				case WLM_MODE_AB:
					validGr = 2;
				break;

				case WLM_MODE_RES:
					validGr = 3;
				break;
			}
		break;

		case WORKINGMODE_GR:
			validGr = 5;
		break;
	}

	if(WORKINGMODE_GR==mainPara.workMode)
	{
		for(i=0;i<validGr;i++)
		{
			if(!radioSet->rfGr[i].isOff)
			{
				devTotal++;
				if(radioStatue.devCnt[i]>0)
				{
					if((!radioStatue.devStatue[i]))		//this group has lights no charged
					{
						//return NULL;
						return -1;
						//return 1;
					}
				}
			}
		}
		
	}else
	{
		for(i=0;i<validGr;i++)
		{
			if(radioStatue.devCnt[i]>0)
			{
				devTotal++;
				if((!radioStatue.devStatue[i]))		//this group has lights no charged
				{
					//return NULL;
					return -1;
					//return 1;
				}
			}
		}
	}

/*
	if(devTotal>0)
	{
		return AD_CHARGE_RDY;
	}else
	{
		return AD_CHARGE_LOW;
	}
*/
	return devTotal;
//	return 1;
}


u8 RfMasterReadyCheck(void)
{
	if(WORKINGMODE_GR==mainPara.workMode)
	{
		if((radioSet->rfGr[0].isOff>0)||(!*wlSet.is_masterOn))
		{
			if(g_slaveReady>0)
			{
				//CHARGE_LED_OK();
				return AD_CHARGE_RDY;
			}else
			{
				//CHARGE_LED_OFF();	
				return AD_CHARGE_LOW;
			}
		}else					//The master flash is open
		{
			if((g_slaveReady>=0)&&(g_chargeOk>0))
			{
				return AD_CHARGE_RDY;
			}else
			{
				return AD_CHARGE_LOW;
			}
			
		/*
			if(CalcSum(&radioStatue.devCnt[0], 5)>0)
			{
				if((g_slaveReady>0)&&(g_chargeOk>0))
				{
					//CHARGE_LED_OK();
					return AD_CHARGE_RDY;
				}else
				{
					//CHARGE_LED_OFF();	
					return AD_CHARGE_LOW;
				}
			}else
			{
				if(g_chargeOk>0)
				{
					//CHARGE_LED_OK();
					return AD_CHARGE_RDY;
				}else
				{
					//CHARGE_LED_OFF();	
					return AD_CHARGE_LOW;
				}
			}
			*/
		}
	}else
	{
		if(*wlSet.is_masterOn>0)
		{
			if((g_slaveReady>=0)&&(g_chargeOk>0))
			{
				return AD_CHARGE_RDY;
			}else
			{
				return AD_CHARGE_LOW;
			}
		/*
			if(CalcSum(&radioStatue.devCnt[0], 5)>0)
			{
				if((g_slaveReady>0)&&(g_chargeOk>0))
				{
					//CHARGE_LED_OK();
					return AD_CHARGE_RDY; 
				}else
				{
					//CHARGE_LED_OFF();	
					return AD_CHARGE_LOW;
				}
			}else
			{
				if(g_chargeOk>0)
				{
					//CHARGE_LED_OK();
					return AD_CHARGE_RDY;
				}else
				{
					//CHARGE_LED_OFF();	
					return AD_CHARGE_LOW;
				}
			}*/
			
		}else
		{
			if(g_slaveReady>0)
			{
				//CHARGE_LED_OK();
				return AD_CHARGE_RDY; 
			}else
			{
				//CHARGE_LED_OFF();	
				return AD_CHARGE_LOW;
			}
		}
	}
}

void RfMasterReadySet(void)
{
	g_slaveReady = SlaveStatueCheck();
	if(RfMasterReadyCheck()>0)
	{
		CHARGE_LED_OK();
		CR_READY = 1;
	}else
	{
		CHARGE_LED_OFF();
		CR_READY = 0;
	}
}

void UartRcvHandle(u8 *rcv, u8 len)
{
	u8 veri=0;
	u8 i=0;
	u8 iDev=0;
	signed char modTmp=0;
	veri = CalcSum(rcv,len-1);
	if(veri!=rcv[len-1])
	{
		veri = 0;
		//FLAOK_TOGGLE();
		return;
	}

	while(i<(len-1))
	{
		switch(rcv[i])
		{
			case UART_CC_REL:
				//if(COM_STATUE_IDLE==g_comStatue)
				//{
				//	ACT_SHUTTER = 1;
				//}
				//g_shutterFlag = 1;
				if(!g_shutterFlag)
				{
					g_dspUpdateFlag |= DSP_SLACMD_REL;
					//g_shutterFlag = UART_CC_REL_FLA;
				}
				i+=3;
			break;


			case UART_CC_SLACMD:
				switch(rcv[i+1])
				{
					case UART_CC_SLACMD_MODEL:
						if(!g_shutterFlag)
						{
							//g_dspUpdateFlag |= DSP_SLACMD_MODEL;
							if(COM_STATUE_IDLE==g_comStatue)
							{
								//CLKSYN_PULLUP();
								//ACT_SHUTTER = 1;
								//SPI_MISO_AUX = 1;
								//COM_AUX = 1;
								g_dspUpdateFlag |= DSP_SLACMD_REL;
							}
							g_shutterFlag = UART_CC_REL_MODEL;
						}
					break;

					case UART_CC_SLACMD_TEST:
						g_dspUpdateFlag |= DSP_SLACMD_TEST;
					break;
				}
				i+=3;
			break;

			case UART_CC_DEV:
			#if 1
				radioStatue.devCnt[0] = rcv[i+1];
				radioStatue.devCnt[1] = rcv[i+2];
				radioStatue.devCnt[2] = rcv[i+3];
				radioStatue.devCnt[3] = rcv[i+4];
				radioStatue.devCnt[4] = rcv[i+5];
				//g_slaveReady = SlaveStatueCheck();
				for(iDev=0;iDev<5;iDev++)						//2015/11/16
				{
					if(radioStatue.devCnt[iDev]>0)
					{
						radioStatue.devStatue[iDev] = 1;
					}
				}
				g_dspUpdateFlag |= DSP_UPDATE_LKDEV;
				gWSleepCounter = 0;
				i+=6;
			#else
				radioStatue.devCnt[rcv[i+1]] = rcv[i+2];
				g_dspUpdateFlag |= DSP_UPDATE_LKDEV;
				if(rcv[i+2]>0)
				{
					CHARGE_LED1_ON;
					g_chargeOk = AD_CHARGE_RDY;
				}
				i+=3;
			#endif
			break;

			case UART_CC_RTSCAN:
				memcpy(radioStatue.rssi, (&rcv[i+1]), 15);
				g_isChScan = 0;
				i+=16;
			break;

			case UART_CC_INIT:
				g_isRfModuleLinked=1;
				RfParaSend();
				i+=3;
			break;

			case UART_CC_READY:
				/*
				if(1==rcv[i+1])
				{
					//if(AD_CHARGE_RDY!=g_chargeOk)
					if(g_chargeOk<AD_CHARGE_RDY)
					{
						//CHARGE_LED1_ON;
						CHARGE_LED_OK();
						g_chargeOk = AD_CHARGE_RDY;
					}
				}else
				{
					if(AD_CHARGE_LOW!=g_chargeOk)
					{
						//CHARGE_LED1_OFF;
						CHARGE_LED_OFF();						
						g_chargeOk = AD_CHARGE_LOW;
					}
				}*/
				radioStatue.devStatue[rcv[i+1]] = rcv[i+2];
				//g_slaveReady = SlaveStatueCheck();
				g_dspUpdateFlag |= DSP_UPDATE_LKDEV;
				/*
				if(RfMasterReadyCheck()>0)
				{
					CHARGE_LED_OK();
				}else
				{
					CHARGE_LED_OFF();
				}*/
				i+=3;
			break;

/*****************************************************************
					RF slave mode uart cammond
******************************************************************/

			case UART_SLAVE_FLA:
				//flaPara.apert = rcv[i+1];
				menuPara.cApert = rcv[i+1];
				flaPara.shutter = rcv[i+2];
				flaPara.isoCmp = rcv[i+3];
				//mainPara.workMode = rcv[i+4+mainPara.s_array*2]&0x7f;
				//mainPara.workMode = rcv[i+4+mainPara.s_array*2]&0x0f;
				//modTmp = rcv[i+4+mainPara.s_array*2]&0x0f;
				mainPara.workMode = rcv[i+4+((*wlSlave.s_array)*2)];
				//g_isModeling = ((rcv[i+4+((*wlSlave.s_array)*2)]>>4)&0x01);
				//modTmp = rcv[i+4]&0x0F;
				//g_expTTLPara3 = rcv[i+4+mainPara.s_array*2]>>7;
				g_rfSlaveFlashCode = rcv[i+4+((*wlSlave.s_array)*2)+1];
				RfSlaveFlashCalc(mainPara.workMode);
				g_comStatue = COM_STATUE_SLAFLA;
				EXTIx_Init(GPIO_B,FTIR);
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//FLAOK_ON();
				i+=15;
			break;

			case UART_SLAVE_MULFLA:
				flaPara.apert = rcv[i+1];
				flaPara.shutter = rcv[i+2];
				flaPara.isoCmp = rcv[i+3];
				//mainPara.workMode = rcv[i+4+mainPara.s_array*2];
				//modTmp = rcv[i+4+mainPara.s_array*2];
				modTmp = rcv[i+4+(*wlSlave.s_array)*2];
				//g_rfSlaveFlashCode = rcv[i+4+mainPara.s_array*2+1];
				g_rfSlaveFlashCode = rcv[i+4+(*wlSlave.s_array)*2+1];
				canWireless.wlMul_time = rcv[i+4+10+1];
				canWireless.wlMul_frq = rcv[i+4+10];
				//*multiSet.frequence = rcv[i+4+10];
				//*multiSet.eTimes = rcv[i+4+10+1];
				ENABLE_D30EN();
				//RfSlaveFlashCalc();
				RfSlaveFlashCalc(modTmp);
				//g_comStatue = COM_STATUE_SLAFLA;
				//Ex_NVIC_Config(GPIO_B,5,RTIR); 
				//SET_INT5SRC_UART();
				//EXTIx_Init(GPIO_B,FTIR);
				//if(IsSlaveModValid(mainPara.workMode))
				if(IsSlaveModValid(modTmp))
				{
					mainPara.workMode = modTmp;
					g_comStatue = COM_STATUE_SLAFLA;
					EXTIx_Init(GPIO_B,FTIR);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
				i+=13;
			break;

			case UART_SLAVE_PRE:
				//g_rfSlavePreOn = (1<<mainPara.s_array)&rcv[4];
				g_rfSlavePreOn = (1<<(*wlSlave.s_array))&rcv[i+4];
				if(g_rfSlavePreOn>0)
				{
					ENABLE_D30EN();
					g_comStatue = COM_STATUE_SLAPRE;
					//SET_UPDATE_EVT(DSP_UPDATE_MODE);							//delete 2015/07/29
					//SetPreFlashDA();
					radioStatue.cmdSta = RADIO_STATUE_PREDAT; 
					radioStatue.cmdReady = 1;
					OSTaskResume(CMDSEND_TASK_Prio);
					//Ex_NVIC_Config(GPIO_B,5,RTIR); 
					//SET_INT5SRC_UART();
					EXTIx_Init(GPIO_B,FTIR);
					//FLAOK_TOGGLE();
				}
				i+=6;
			break;

			case UART_SLAVE_PREFLA:
				if(g_rfSlavePreOn>0)
				{
					radioStatue.cmdReady = 1;
					radioStatue.cmdSta = RADIO_STATUE_SLAPRE;
					OSTaskResume(CMDSEND_TASK_Prio);
				}
				i+=3;
			break;

			case UART_SLAVE_LINKED:
				g_isRfSlaveLinked  = rcv[i+1];
				i+=3;
			break;

			case UART_ENCODE_SLAPWROFF:
				i+=3;
			break;

/*****************************************************************
						   		END
******************************************************************/
			default:
				i+=3;
			break;
		}
	}
}

//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率
//CHECK OK
//091209

void uart1_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    	mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO状态设置
		  
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
#ifdef EN_USART1_RX		  //如果使能了接收
	//使能接收中断
	USART1->CR1|=1<<8;    //PE中断使能
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//组2，最低优先级 
#endif
}

void Uart2PortSet()
{
	GPIOA->CRL&=0XFFFF00FF;
	GPIOA->CRL|=0X00008B00;
}


void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    	mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB1ENR|=1<<17;  //使能串口2 时钟 
	//GPIOA->CRH&=0XFFFFF00F; 
	//GPIOA->CRH|=0X000008B0;//IO状态设置
	GPIOA->CRL&=0XFFFF00FF;
	GPIOA->CRL|=0X00008B00;
	//Uart2PortSet();

	
	RCC->APB1RSTR|=1<<17;   //复位串口2
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=mantissa; // 波特率设置	 
	USART2->CR1|=0X200C;  //1位停止,无校验位.

	//使能接收中断
	//USART2->CR1|=1<<8;    //PE中断使能
	USART2->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	//USART2->CR1|=1<<6;		//TCIE transfer complete interrupt enable
	//USART2->CR1|=1<<7;
	//MY_NVIC_Init(1,2,USART2_IRQChannel,2);//组2，最低优先级 
	MY_NVIC_Init(1,2,USART2_IRQn,2);//组2，最低优先级 
}

/*
void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStruct;

	USART_RCC_Configuration();

	RCC->APB1RSTR|=1<<17;   //复位串口1
	RCC->APB1RSTR&=~(1<<17);//停止复位	 

//	GPIOA->CRH&=0xFFFFF00F; 
//	GPIOA->CRH|=0x000008B0;//IO状态设置

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
//	USART_Init(USART1, &USART_InitStruct);
	USART_Init(USART2, &USART_InitStruct);
	USART_ITConfig(USART2,USART_IT_TC,ENABLE);
//	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//使能接收中断
//	USART_Cmd(USART1, ENABLE);//使能串口1
	USART_Cmd(USART2, ENABLE);

	//USART_GPIO_Configuration();

	//RCC->APB2RSTR|=1<<14;   //复位串口1
	//RCC->APB2RSTR&=~(1<<14);//停止复位	 
	GPIOA->CRL&=0xFFFF00FF; 
	GPIOA->CRL|=0x00008B00;//IO状态设置
	
}
*/


unsigned char LoadUartSendBuf(unsigned char id, unsigned char pa1, unsigned char pa2)
{
	if(0==gBUartTotalByte)
	{
		gBUartBuf[gBUartTotalByte++] = 0xAA;
		gBUartBuf[gBUartTotalByte++] = 0;			//len
	}
	gBUartBuf[gBUartTotalByte++] = id;
	gBUartBuf[gBUartTotalByte++] = pa1;
	gBUartBuf[gBUartTotalByte++] = pa2;
	return gBUartTotalByte;		
}

unsigned char LoadUartArray(u8 id, u8 *pBuf, u8 len)
{
	if(0==gBUartTotalByte)
	{
		gBUartBuf[gBUartTotalByte++] = 0xAA;
		gBUartBuf[gBUartTotalByte++] = 0;			//len
	}
	gBUartBuf[gBUartTotalByte++] = id;
	memcpy(gBUartBuf+gBUartTotalByte,pBuf,len);
	gBUartTotalByte+=len;
	return gBUartTotalByte;
	
}

void TrigerUartSend(u8 mod)
{
	char i=0;
	unsigned char sum=0;
	for(i=2;i<gBUartTotalByte;i++)
	{
		sum+= gBUartBuf[i];
	}
	gBUartBuf[gBUartTotalByte++] = sum;
	gBUartBuf[1] = (gBUartTotalByte-2);
	uart_tran_flag = 1;
	UART_INT = 1;

	if(!mod)
	{	
		USART2->CR1|=1<<6;
	}else
	{
		OS_ENTER_CRITICAL();
		while(gBUartLen<gBUartTotalByte)
		{
			USART2->DR = gBUartBuf[gBUartLen++];
			while(!(USART2->SR&(1<<6)));
			USART2->SR = USART2->SR;
		}
		gBUartTotalByte = 0;	
		gBUartLen = 0;
		uart_tran_flag = 0;
		OS_EXIT_CRITICAL();
		UART_INT = 0;
	}
	
}

unsigned char UartSctSend(unsigned char id, unsigned char pa1, unsigned char pa2)
{
	//while(uart_tran_flag);
	LoadUartSendBuf(id, pa1,pa2);
	TrigerUartSend(0);	
	return 0;
}

void RfPreFlashInfo(u8 tms)
{
	//LoadUartSendBuf(UART_ENCODE_ISOCMP,main_parameter.VIP_workmode,rfGrPara.isoCmp);
	LoadUartSendBuf(UART_ENCODE_APERTSHUTER,flaPara.shutter,flaPara.apert);
	LoadUartSendBuf(UART_ENCODE_ISOCMP,flaPara.isoCmp,tms);
	//LoadUartSendBuf(UART_ENCODE_RFPREARR,mainPara.workMode,flaPara.preflashIndex);
	LoadUartSendBuf(UART_ENCODE_RFPREARR,(mainPara.synMode<<3)|(mainPara.workMode&0x07),flaPara.preflashIndex);
	//while(uart_tran_flag);
	TrigerUartSend(0);
	//LoadUartSendBuf(UART_ENCODE_RFa,rfGrPara..rfGr_arrPara[0].);
	//LoadUartSendBuf(UART_ENCODE_RFPREARR,);	
}

void RfCountDownInfo(int tick)
{
	//while(uart_tran_flag);
	WaitForUartIdle(20);			// 2016/02/17 for deadlock
	LoadUartSendBuf(UART_ENCODE_RFCOUNTDOWN,tick>>8,tick&0xff);
	TrigerUartSend(0);
	/*
	while(tick>=0)
	{
		LoadUartSendBuf(UART_ENCODE_RFCOUNTDOWN,(tick>>8),(tick&0xff));
		TrigerUartSend();
		tick-=1000;
		delay_ms(1);
	}*/
}

void RfRecoverInfo()
{
	//while(uart_tran_flag);
	WaitForUartIdle(20);			// 2016/02/17 for deadlock
	LoadUartSendBuf(UART_ENCODE_RFRECOVER,0,0);
	TrigerUartSend(0);
}

char GR_EXT_CODE(u8 gr)
{
	signed char c1;signed char c2;
	c1 = mstPara->cmp1+radioSet->rfGr[gr].ttl_cmp1;
	c2 = mstPara->cmp2+radioSet->rfGr[gr].ttl_cmp2;

	if(c2>=3)
	{
		c1++;
		c2-=3;
	}
	if((c1<-6)||(c1>6))
	{
		c1 = 0;
		c2 = 0;
	}

	switch(flaPara.FEB_count)
	{
		case 1:
		break;

		case 2:
			c1 -= flaPara.ttl_FEB1;
			c2 -= flaPara.ttl_FEB2;
			if(c2<0)
			{
				c2+=3;
				c1--;
			}
		break;

		case 3:
			c1 += flaPara.ttl_FEB1;
			c2 += flaPara.ttl_FEB2;
			if(c2>3)
			{
				c1++;
				c2-=3;
			}
		break;

		default:
		break;
	}

	if((c1<-6)||(c1>6))
	{
		c1 = 0;
		c2 = 0;
	}

	return (0x64-compensation_table[(c1+6)*3+c2]);
	
}

signed char GrModConvert(u8 i)
{
	if(radioSet->rfGr[i].isOff>0)
	{
		return WORKINGMODE_OFF;
	}else
	{
		if(GRMOD_EXTA==radioSet->rfGr[i].mode)
		{
			return WORKINGMODE_EXTA;
		}else
		{
			return radioSet->rfGr[i].mode;
		}
	}
}

signed char GrModRevert(signed char mode)
{
	switch(mode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_MANU:
		return mode;

		case WORKINGMODE_EXTA:
		return GRMOD_EXTA;

	}

	return NULL;
}



void GrLoadPara(u8 *sBuf,u8 *len)
{
	u8 i=0;
	u8 index=0;
	//sBuf[index++] = 0xB4;
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			//switch(*wlSet.arr_mode)
			switch(*wlSet.ttl_Ratio)
			{
				case WLM_MODE_ALL:
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_TTL);;
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = flaPara.rfCmuPara[0].ttlExpVal;//0X7A;//wlSet.m_ttlCmd[0];
					sBuf[index++] = flaPara.rfCmuPara[0].ttlExpVal;//wlSet.m_ttlCmd[0];
					sBuf[index++] = flaPara.rfCmuPara[0].ttlExpVal;//wlSet.m_ttlCmd[0];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_AB:
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_TTL);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = flaPara.rfCmuPara[0].ttlExpVal;//wlSet.m_ttlCmd[0];
					sBuf[index++] = flaPara.rfCmuPara[1].ttlExpVal;//wlSet.m_ttlCmd[1];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_RES:
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_TTL);;
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = flaPara.rfCmuPara[0].ttlExpVal;//wlSet.m_ttlCmd[0];
					sBuf[index++] = flaPara.rfCmuPara[1].ttlExpVal;//wlSet.m_ttlCmd[1];
					sBuf[index++] = flaPara.rfCmuPara[2].ttlExpVal;//wlSet.m_ttlCmd[2];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;
			}
		break;

		case WORKINGMODE_MANU:
			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
					sBuf[index++] = ((WORKINGMODE_MANU<<4)|WORKINGMODE_MANU);;
					sBuf[index++] = ((WORKINGMODE_MANU<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = menuPara.otpCod[0];
					sBuf[index++] = menuPara.otpCod[0];
					sBuf[index++] = menuPara.otpCod[0];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_AB:
					sBuf[index++] = ((WORKINGMODE_MANU<<4)|WORKINGMODE_MANU);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = menuPara.otpCod[0];
					sBuf[index++] = menuPara.otpCod[1];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_RES:
					sBuf[index++] = ((WORKINGMODE_MANU<<4)|WORKINGMODE_MANU);;
					sBuf[index++] = ((WORKINGMODE_MANU<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = menuPara.otpCod[0];
					sBuf[index++] = menuPara.otpCod[1];
					sBuf[index++] = menuPara.otpCod[2];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;
			}
		break;

		case WORKINGMODE_MULTI:
			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
					sBuf[index++] = ((WORKINGMODE_MULTI<<4)|WORKINGMODE_MULTI);;
					sBuf[index++] = ((WORKINGMODE_MULTI<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = menuPara.WlmMul[0];
					sBuf[index++] = menuPara.WlmMul[0];
					sBuf[index++] = menuPara.WlmMul[0];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_AB:
					sBuf[index++] = ((WORKINGMODE_MULTI<<4)|WORKINGMODE_MULTI);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = menuPara.WlmMul[0];
					sBuf[index++] = menuPara.WlmMul[1];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_RES:
					sBuf[index++] = ((WORKINGMODE_MULTI<<4)|WORKINGMODE_MULTI);;
					sBuf[index++] = ((WORKINGMODE_MULTI<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = menuPara.WlmMul[0];
					sBuf[index++] = menuPara.WlmMul[1];
					sBuf[index++] = menuPara.WlmMul[2];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;
			}
		break;

		case WORKINGMODE_GR:
			//sBuf[index++] = ((radioSet->rfGr[0].mode<<4)|radioSet->rfGr[1].mode);
			//sBuf[index++] = ((radioSet->rfGr[2].mode<<4)|radioSet->rfGr[3].mode);
			//sBuf[index++] = ((radioSet->rfGr[4].mode<<4)|0);
			sBuf[index++] = ((GrModConvert(0)<<4)|GrModConvert(1));
			sBuf[index++] = ((GrModConvert(2)<<4)|GrModConvert(3));
			sBuf[index++] = ((GrModConvert(4)<<4)|0);
			for(i=0;i<5;i++)
			{
				if(radioSet->rfGr[i].isOff>0)
				{
					sBuf[index++] = 0;
				}else
				{
					switch(radioSet->rfGr[i].mode)
					{
						//case WORKINGMODE_TTL:
						case GRMOD_TTL:
							sBuf[index++] = flaPara.rfCmuPara[i].ttlExpVal;
						break;

						//case WORKINGMODE_MANU:
						case GRMOD_M:
							sBuf[index++] = menuPara.rfGrOtp[i];
						break;

						//case WORKINGMODE_EXTA:
						case GRMOD_EXTA:
							sBuf[index++] = GR_EXT_CODE(i);
						break;

						case WORKINGMODE_OFF:
							sBuf[index++] = 0;
						break;
						
						default:
							sBuf[index++] = 0;
						break;

						
					}
				}
				
			}
		break;
	}

	*len = index;
}


/*void TmodLoadPara(char mode,unsigned char group)
{
	if(WORKINGMODE_TTL==mode)
	{
		switch(*wlSet.arr_mode)
		{
			case WLM_MODE_ALL:
				if(group<=WL_ARRARY_C)
				{
					//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,(wlSet.m_ttlCmd[0]>0xD0)?(wlSet.m_ttlCmd[0]&0x7F):wlSet.m_ttlCmd[0]);	
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,wlSet.m_ttlCmd[0]);	
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}					
			break;

			case WLM_MODE_AB:
				if(group<=WL_ARRARY_B)
				{
					//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,(wlSet.m_ttlCmd[group]>0xD0)?(wlSet.m_ttlCmd[group]&0x7F):wlSet.m_ttlCmd[group]);	
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,wlSet.m_ttlCmd[group]);	
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;

			case WLM_MODE_RES:
				if(group<=WL_ARRARY_C)
				{
					//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,(wlSet.m_ttlCmd[group]>0x7F)?(wlSet.m_ttlCmd[group]&0x7F):wlSet.m_ttlCmd[group]);	
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,wlSet.m_ttlCmd[group]);	
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;
		}
	}else if(WORKINGMODE_MANU==mode)
	{

		switch(*wlSet.arr_mode)
		{
			case WLM_MODE_ALL:
				if(group<=WL_ARRARY_C)
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MANU,menuPara.otpCod[0]);
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;

			case WLM_MODE_AB:
				if(group<=WL_ARRARY_B)
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MANU,menuPara.otpCod[group]);

				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;

			case WLM_MODE_RES:
				if(group<=WL_ARRARY_C)
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MANU,menuPara.otpCod[group]);
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;
		}
	}else if(WORKINGMODE_MULTI==mode)
	{
		
		switch(*wlSet.arr_mode)
		{
			case WLM_MODE_ALL:
				if(group<=WL_ARRARY_C)
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MULTI,menuPara.WlmMul[0]);
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;
			
			case WLM_MODE_AB:
				if(group<=WL_ARRARY_B)
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MULTI,menuPara.WlmMul[group]);
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;
			
			case WLM_MODE_RES:
				if(group<=WL_ARRARY_C)
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MULTI,menuPara.WlmMul[group]);
				}else
				{
					LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
				}
			break;
		}
	}
}*/

u8 WaitForUartIdle(u8 tick)
{
	u8 i = 0;
	while(uart_tran_flag)
	{
		delay_ms(1);
		i++;
		if(i>=tick)
		{
			uart2_init(36,115200);
			gBUartTotalByte = 0;	
			gBUartLen = 0;
			uart_tran_flag = 0;
			return 1;
		}
	}
	return 0;
}

void RfFlashConfirm(u8 tms,u8 fPilot)
{
//	unsigned char i=0;
	unsigned char times,frq;
	u8 pBuf[100];
	u8 pLen;
	
	//LoadUartSendBuf(UART_ENCODE_APERTSHUTER,flaPara.shutter,flaPara.apert);
	//LoadUartSendBuf(UART_ENCODE_ISOCMP,flaPara.isoCmp,tms);
	/*
	for(i=0;i<5;i++)
	{
		(WORKINGMODE_GR==mainPara.workMode)? GrLoadPara(radioSet->rfGr[i].mode,i) : TmodLoadPara(mainPara.workMode,i);
	}*/

	GrLoadPara(pBuf,&pLen);

	LoadUartArray(UART_ENCODE_GRM,pBuf,pLen);	
	
	if(WORKINGMODE_MULTI==mainPara.workMode)
	{
		//LoadUartSendBuf(UART_ENCODE_RFMUL,multiSet.eTimes,multiSet.frequence);
		times = RfMulTimeRev();
		frq = WlmMulFrqRev();
		LoadUartSendBuf(UART_ENCODE_RFMUL,times,frq);
		LoadUartSendBuf(UART_ENCODE_APERTSHUTER,flaPara.shutter,flaPara.apert);
		
	}else if(WORKINGMODE_MANU==mainPara.workMode)
	{
		LoadUartSendBuf(UART_ENCODE_APERTSHUTER,flaPara.shutter,flaPara.apert);
	}
	LoadUartSendBuf(UART_ENCODE_ISOCMP,flaPara.isoCmp,tms);
	//LoadUartSendBuf(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_FLA,mainPara.workMode);(mainPara.synMode<<3)|(mainPara.workMode&0x07)
	//LoadUartSendBuf(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_FLA,(fPilot<<6)|(flaPara.hpFlash<<5)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
	LoadUartSendBuf(UART_ENCODE_RFFLA,tms,(fPilot<<6)|(flaPara.hpFlash<<5)|(mainPara.synMode<<3)|(mainPara.workMode&0x07));
	//while(uart_tran_flag);
	if(!WaitForUartIdle(20))			// 2016/02/17 for deadlock
	{
		TrigerUartSend(0);
	}

	
}

void U2SendData(u8 dat)
{
	USART2->DR = dat;
	while(!(USART2->SR&(1<<6)));
	USART2->SR = USART2->SR;
}

void uartSendData(unsigned char *buf, unsigned char len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		//USART_SendData(USART2,buf[i]);//putchar(buf[i]);
		//USART_SendData();
		U2SendData(buf[i]);
	}
}

int noprintf(const char *fmt, ...)
{
   return 0;
}





