#include "sys.h"
#include "usart.h"
#include "../../user/Golbal.h"
#include "../../user/Display.h"
#include "../../user/Canon.h"

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
extern unsigned char *g_cmdMode;
extern OS_TMR  *g_shutterPullTmr;
char g_cc2530Start=0;
char g_cc2530Wake=0;

void UartRcvHandle(u8 *rcv, u8 len);
extern void RfParaSend(void);

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
#ifdef _UART_INT_INFO_
				UART_INFO = 0;
				//delay_us(10);
				//UART_INFO = 1;
				//SET_UPDATE_EVT(DSP_UPDATE_UARTINT);
#endif
			}
		}
	}
	USART2->SR = USART2->SR;
	
	//usart_sr = USART2->SR;
	//USART2->SR = usart_sr;
}

u8 CalcSum(u8 *rcv, u8 len)
{
	u8 i=0,sum=0;
	for(i=0;i<len;i++)
	{
		sum+=rcv[i];
	}
	return sum;
}

void UartRcvHandle(u8 *rcv, u8 len)
{
	u8 veri=0;
	u8 i=0;
	//INT8U tmrErr;
	veri = CalcSum(rcv,len-1);
	if(veri!=rcv[len-1])
	{
		veri = 0;
		return;
	}

	while(i<(len-1))
	{
		switch(rcv[i])
		{
			case UART_CC_REL:
				if(!g_shutterFlag)
				{
					//if(COM_STATUE_IDLE==g_comStatue)
					{
						//CLKSYN_PULLUP();
						//ACT_SHUTTER = 1;
						//SPI_MISO_AUX = 1;
						//COM_AUX = 1;
						/*
#ifdef UART1_TRACE_DEBUG
							if(OS_FALSE==OSTmrStart(g_shutterPullTmr, &tmrErr))
							{
								printf("Tmr start err: %d\r\n", tmrErr);
							}
#else
							OSTmrStart(g_shutterPullTmr, &tmrErr);
#endif
						*/
						//g_dspUpdateFlag |= DSP_SLACMD_REL;
					}
					//WIRE_SHUTTER = 1;
					g_dspUpdateFlag |= DSP_SLACMD_REL;
					g_shutterFlag = UART_CC_REL_FLA;
				}
				//g_shutterFlag = rcv[i];
				//CHARGE_LED_ON;
				//FLAOK_ON;
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
			#if 0
				radioStatue.devCnt[rcv[i+1]] = rcv[i+2];
				g_dspUpdateFlag |= DSP_UPDATE_LKDEV;
				if(rcv[i+2]>0)
				{
					CHARGE_LED_ON;
					g_chargeOk = AD_CHARGE_RDY;
				}
				i+=3;
			#else
				if((rcv[i+1]>radioStatue.devCnt[0])||(rcv[i+2]>radioStatue.devCnt[1])||(rcv[i+3]>radioStatue.devCnt[2])
					||(rcv[i+4]>radioStatue.devCnt[3])||(rcv[i+5]>radioStatue.devCnt[4]))
				{
					BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
					CHARGE_LED_ON;                                  // 2017/08/27
				}
				radioStatue.devCnt[0] = rcv[i+1];
				radioStatue.devCnt[1] = rcv[i+2];
				radioStatue.devCnt[2] = rcv[i+3];
				radioStatue.devCnt[3] = rcv[i+4];
				radioStatue.devCnt[4] = rcv[i+5];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				//g_dspUpdateFlag |= DSP_UPDATE_LKDEV;
				i+=6;
			#endif
			break;

			case UART_CC_READY:
				if(1==rcv[i+1])
				{
					if(AD_CHARGE_RDY!=g_chargeOk)
					{
						CHARGE_LED_ON;
						g_chargeOk = AD_CHARGE_RDY;
					}
				}else
				{
					if(AD_CHARGE_LOW!=g_chargeOk)
					{
						CHARGE_LED_OFF;
						g_chargeOk = AD_CHARGE_LOW;
					}
				}
				i+=3;
			break;

			case UART_CC_RTSCAN:
				memcpy(radioStatue.rssi, (&rcv[i+1]), 15);
				g_isChScan = 0;
				i+=16;
			break;

			case UART_CC_INIT:
				if(rcv[i+1])
				{
					//g_isRfModuleLinked=1;
					//RfParaSend();
					SET_UPDATE_EVT(DSP_UPDATE_RFINFO);
					//g_cc2530Start = 1;
					g_cc2530Wake = 0;
					//FLAOK_ON();
				}
				i+=3;
			break;

			case UART_CC_MSTSW:
				if(CMD_MODE_RFLS_MST==*g_cmdMode)
				{
					*g_cmdMode = CMD_MODE_RFLS_SLA;
					//g_dspUpdateFlag |= DSP_UPDATE_LINKM2S;
					//g_dspUpdateFlag |= DSP_UPDATE_CMDMOD;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
				}
				i+=3;
			break;

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

//void TrigerUartSend(void)
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

#ifdef _UART_INT_INFO_
	UART_INFO = 1;
#endif
	

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
		
#ifdef _UART_INT_INFO_
		UART_INFO = 0;
#endif
	}
	
	//USART2->DR = gBUartBuf[gBUartLen++];
	//USART2->SR = USART2->SR;
	
	/*
	while(gBUartLen<gBUartTotalByte)
	{
		USART2->DR = gBUartBuf[gBUartLen++];
		while(!(USART2->SR&(1<<6)));
		USART2->SR = USART2->SR;
	}*/
	//TXBUF0 = gBUartBuf[gBUartLen++];
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
	//TrigerUartSend(1);						//why send data synchronously
	//TrigerUartSend(1);
	TrigerUartSend(0);
	//LoadUartSendBuf(UART_ENCODE_RFa,rfGrPara..rfGr_arrPara[0].);
	//LoadUartSendBuf(UART_ENCODE_RFPREARR,);	
}

void RfCountDownInfo(int tick)
{
	while(uart_tran_flag);
	LoadUartSendBuf(UART_ENCODE_RFCOUNTDOWN,tick>>8,tick&0xff);
	//TrigerUartSend(1);						//why send data synchronously
	//TrigerUartSend(1);
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
	while(uart_tran_flag);
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

#if 0

void GrLoadPara(unsigned char mode,unsigned char group)							//GR mode
{
	switch(mode)
	{
		case WORKINGMODE_TTL:
			//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,(flaPara.rfCmuPara[group].ttlExpVal>0xD0)?(flaPara.rfCmuPara[group].ttlExpVal&0x7f):flaPara.rfCmuPara[group].ttlExpVal);
			LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,flaPara.rfCmuPara[group].ttlExpVal);
			//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,flaPara.rfCmuPara[group].ttlExpVal);
			//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_TTL,wlSet.m_ttlCmd[0]);
		break;

		case WORKINGMODE_MANU:
			//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MANU,flaPara.rfCmuPara[group].optCode);
			LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_MANU,menuPara.rfGrOtp[group]);
		break;

		case WORKINGMODE_EXTA:
			//LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_EXTA,0);
			LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_EXTA,GR_EXT_CODE(group));
		break;

		case WORKINGMODE_OFF:
			LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
		break;

		default:
			LoadUartSendBuf(UART_ENCODE_RFa+group,WORKINGMODE_OFF,0);
		break;
	}
}


void TmodLoadPara(char mode,unsigned char group)
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
}
#endif

void GrLoadPara(u8 *sBuf,u8 *len)
{
	u8 i=0;
	u8 index=0;
	//sBuf[index++] = 0xB4;
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			switch(*wlSet.arr_mode)
			{
				case WLM_MODE_ALL:
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_TTL);;
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = wlSet.m_ttlCmd[0];
					sBuf[index++] = wlSet.m_ttlCmd[0];
					sBuf[index++] = wlSet.m_ttlCmd[0];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_AB:
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_TTL);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = wlSet.m_ttlCmd[0];
					sBuf[index++] = wlSet.m_ttlCmd[1];
					sBuf[index++] = 0;
					sBuf[index++] = 0;
					sBuf[index++] = 0;
				break;

				case WLM_MODE_RES:
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_TTL);;
					sBuf[index++] = ((WORKINGMODE_TTL<<4)|WORKINGMODE_OFF);;
					sBuf[index++] = ((WORKINGMODE_OFF<<4)|0);
					sBuf[index++] = wlSet.m_ttlCmd[0];
					sBuf[index++] = wlSet.m_ttlCmd[1];
					sBuf[index++] = wlSet.m_ttlCmd[2];
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
			sBuf[index++] = ((radioSet->rfGr[0].mode<<4)|radioSet->rfGr[1].mode);
			sBuf[index++] = ((radioSet->rfGr[2].mode<<4)|radioSet->rfGr[3].mode);
			sBuf[index++] = ((radioSet->rfGr[4].mode<<4)|0);
			for(i=0;i<5;i++)
			{
				switch(radioSet->rfGr[i].mode)
				{
					case WORKINGMODE_TTL:
						sBuf[index++] = flaPara.rfCmuPara[i].ttlExpVal;
					break;

					case WORKINGMODE_MANU:
						sBuf[index++] = menuPara.rfGrOtp[i];
					break;

					case WORKINGMODE_EXTA:
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
		break;
	}

	*len = index;
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
	while(uart_tran_flag);
	TrigerUartSend(0);
	
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







