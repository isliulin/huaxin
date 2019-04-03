#include "sr101ef51a.h"
#include "mn101ef52a.h"
#include "init.h"
//#include "oloong.h"
#include "stdio.h"
#include "uart.h"
#include "nikon.h"
//#include "Golbal.h"


void lowPowerSleep();
void pilot();
#define MAINPARA_ARRARY send_arrary2

#ifdef HP_SAMPLE_TEST
	unsigned int adSample;
#endif

char const preductionId[] = {"OLOONG SP690, PREDUCTED BY ÄÙˆ¼‚ô@²ó¬á¼ÔÈî@cbyibby@163.com\n"};
unsigned char code cmp_calc[] = {
0x12,0x10,0x0E,0X0C,0X0A,0X08,0X06,0X04,0X02,
0X00,0XFE,0XFC,0XFA,0XF8,0XF6,0XF4,0XF2,0XF0,
0XEE,
};

unsigned char code output_calc[] = {
0X2A,0X28,0X26,0X24,0X22,0X20,0X1E,0X1C,0X1A,0X18,0X16,
0X14,0X12,0X10,0X0E,0X0C,0X0A,0X08,0X06,0X04,0X02,0X00,
};
#define WLM_MAST_OFF 0X80
#define WLM_PARA_CLR {MAINPARA_ARRARY[WLM_LOC_MASTSET]=0;MAINPARA_ARRARY[WLM_LOC_MASTPAR]=0;MAINPARA_ARRARY[WLM_LOC_MSL_A]=0;MAINPARA_ARRARY[WLM_LOC_MSL_BC]=0;MAINPARA_ARRARY[WLM_LOC_SLPARA_A]=0;MAINPARA_ARRARY[WLM_LOC_SLPARA_B]=0;MAINPARA_ARRARY[WLM_LOC_SLPARA_C]=0;}
//#define WLM_PARA_SET{MAINPARA_ARRARY[WLM_LOC_MASTSET]=WLM_MAST_OFF;MAINPARA_ARRARY[WLM_LOC_MASTPAR]=0;MAINPARA_ARRARY[WLM_LOC_MSL_A]=wlSet.m_ma;MAINPARA_ARRARY[WLM_LOC_MSL_BC]=(wlSet.m_mb<<4|wlSet.m_mc);MAINPARA_ARRARY[WLM_LOC_SLPARA_A]=wlSet.m_slpara[0];MAINPARA_ARRARY[WLM_LOC_SLPARA_B]=wlSet.m_slpara[1];MAINPARA_ARRARY[WLM_LOC_SLPARA_C]=wlSet.m_slpara[2];}
#define WLM_PARA_SET{MAINPARA_ARRARY[WLM_LOC_MASTSET]=wlSet.m_arrMode[WLM_ARRAY_M];MAINPARA_ARRARY[WLM_LOC_MASTPAR]=wlSet.m_slpara[WLM_ARRAY_M];MAINPARA_ARRARY[WLM_LOC_MSL_A]=wlSet.m_arrMode[WLM_ARRAY_A];MAINPARA_ARRARY[WLM_LOC_MSL_BC]=(wlSet.m_arrMode[WLM_ARRAY_B]<<4|wlSet.m_arrMode[WLM_ARRAY_C]);MAINPARA_ARRARY[WLM_LOC_SLPARA_A]=wlSet.m_slpara[WLM_ARRAY_A];MAINPARA_ARRARY[WLM_LOC_SLPARA_B]=wlSet.m_slpara[WLM_ARRAY_B];MAINPARA_ARRARY[WLM_LOC_SLPARA_C]=wlSet.m_slpara[WLM_ARRAY_C];}


unsigned char   gBUartLen;
unsigned char	gBUartBuf[MAX_UART_DATA_LEN];
extern unsigned int code shortShutter[39];

unsigned char uart_tran_flag=0;
unsigned char gBUartTotalByte=0;

extern UART_SEND xdata uartSend;
extern FLASH_STATUE xdata flashStatue;
extern unsigned char xdata bitChar;
extern unsigned char bitWireless;
extern Wl_set xdata wlSet;
extern NiWireless xdata nikWireless;
extern EXPOSE_PARA xdata flashPara;
extern unsigned char xdata uartRcvBuf[3];
extern MultiSet xdata multiSet;
extern unsigned int pTime;
extern unsigned char charge_ok;
extern UartSendFlag uartSendFlag;

	


#pragma	_interrupt	int_SC0t	
#pragma	_interrupt	int_SC0r
/* ------------------------------------End ---------------------------------
 ===========================================================================

 º¯ÊýÃû   :  void com_start(void)
 
 ¹¦ÄÜ  :     uart0 TM2³õÊ¼»¯
 
¦ÅèÐï™       
 
 ===========================================================================*/
void	com_start(void){
	TM2MD=0x00;  /*CLOSE tm2*/
	
	
	TM2OC=(130-1); /*6.5us = fpll-div * 130*/
	TM2MD|=0x08;   /*turn on TM2*/
	
	PADIR |= 0X08;
	PAPLU |= 0X0C;
//	PAODC |= 0X09;
	
	SC0MD1 = 0x30;
	SC0SEL = 0x02;
	SC0MD0 = 0x98;
	SC0MD1 = 0x39;
//	SC0MD2 = 0xb0;
	SC0MD2 = 0x88;
	SC0MD3 = 0x0F;			

	disable_irq();			
	MEMCTR   |=  0x04;		
	/*SCINTSEL |=  0x01;*/	
	SC0TICR   =  0x82;		/*turn on tx interrupt	 */
	SC0RICR   =  0x82;		/*turn on rx interrupt	*/
	MEMCTR   &= ~0x04;		
	enable_irq();			

	/*RCV_ENABLE_PNL();*/

}

void UartStop()
{
	PADIR &= ~0x08;

	disable_irq();			
	MEMCTR   |=  0x04;		
	/*SCINTSEL |=  0x01;*/	
	SC0TICR   =  0x82;		/*turn on tx interrupt	 */
	SC0RICR   =  0x82;		/*turn on rx interrupt	*/
	MEMCTR   &= ~0x04;		
	enable_irq();
}

void SynPutChar(const unsigned char Tbyte)
{
	disable_irq();
	MEMCTR   |=  0x04;	
	SC0TICR &= 0xfc;            //turn off uart interrupt
	MEMCTR   &= ~0x04;	
	enable_irq();	            //clear flag

	TXBUF0 = Tbyte;
	while(SC0STR & 0x20){};         /* Wait transmission */

	SC0STR &= 0XDF;

	disable_irq();
	MEMCTR   |=  0x04;	
	SC0TICR |= 0X02;
	MEMCTR   &= ~0x04;	
	enable_irq();
}

void putsDialog(const char *str)
{
	 //UartDecode(str);
	while(*str != '\0')
	{
		SynPutChar(*str++);
	}
}

/*
int DecodeUartData(unsigned char id, unsigned char para1, unsigned char para2)
{
#ifdef UART_SENDMODE_ASYC
	unsigned char uLoadIndex=0;
	unsigned char uartLoad[4];
	char i=0;
		
	uartLoad[uLoadIndex++] = 0xcc;
	uartLoad[uLoadIndex++] = id;
	uartLoad[uLoadIndex++] = para1;  
	uartLoad[uLoadIndex++] = para2;

	if(uart_tran_flag) 
	{
		return err;	
	}
	
	else
	{
			while(i<4)
			{
			 	gBUartBuf[gBUartTotalByte++] = uartLoad[i++]; 		//gBUartTotalByte must be 0;
			}

			i = 0;
			uart_tran_flag = 1;									//set transent flag
			//SETBIT(SC0TICR,0);											//set interrupt
			disable_irq();
			MEMCTR   |=  0x04;
			SETBIT(SC0TICR,0);
			MEMCTR   &= ~0x04;
			enable_irq();
			return 0;
	}


#else 
	unsigned char uLoadIndex=0;
	unsigned char uartLoad[4];
//	char i;
		
	uartLoad[uLoadIndex++] = 0xcc;
	uartLoad[uLoadIndex++] = id;
	uartLoad[uLoadIndex++] = para1;  
	uartLoad[uLoadIndex++] = para2;

	uLoadIndex = 0;

	while(uLoadIndex<4)
	{
		putchar(uartLoad[uLoadIndex++]);
	}
#endif
}
*/

/* ------------------------------------End ---------------------------------
 ===========================================================================

Çîyö·ò×òÀ :  void int_SCOt(void)
 
 ¹¦ÄÜ  :     ·¢ËÍÖÐ¶Ï
 
 ÃèÐð:       
 
 ===========================================================================*/
void	int_SC0t(void)
{
	disable_irq();	
	MEMCTR   |=  0x04;
	CLRBIT(SC0TICR,0);
	MEMCTR   &= ~0x04;
	if(uart_tran_flag)								//whether in the send process
	{
		/*-----------------»â®îY¶ºëäÌÏ-----------------*/
		if(gBUartLen <gBUartTotalByte)
		{							
			TXBUF0=	gBUartBuf[gBUartLen];		
			gBUartLen++;						
		}
		/*-----------------»â®îaÓºëäÌÏ-----------------*/
		else
		{
			gBUartLen		= 0x00;				
			gBUartTotalByte = 0x00;				
			uart_tran_flag	    = 0;											
		}
	}
	enable_irq();	
}
/* ------------------------------------End ---------------------------------
 ===========================================================================

 º¯ÊýÃû   :  void int_SC0r(void)
 
 ¹¦ÄÜ  :     ½ÓÊÕÖÐ¶Ï
 
 ÃèÐð:       
 
 ===========================================================================*/
void	int_SC0r(void)
{
	/*	transmit();*/
	static unsigned char uart_rcvData;
	static unsigned char loaden;
	static unsigned char bufIndex=0;
	unsigned char veri;
	
	uart_rcvData = RXBUF0;

	if(loaden)
	{
		uartRcvBuf[bufIndex++]=uart_rcvData;
		if(bufIndex>=4)
		{
			veri = uartRcvBuf[0]+uartRcvBuf[1]+uartRcvBuf[2];
			if(veri!=uartRcvBuf[3])
			{
				bufIndex = 0;
				loaden = 0;
				return;
			}
			switch(uartRcvBuf[0])
			{
				case UART_DECODE_MODE:
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
				if(!flashPara.ledFlag)
					LED_LITE = 0;
#else
					AFCTL = 0;
#endif
				flashPara.workmode = uartRcvBuf[1];
				uartSendFlag.imfo_focLen = 1;
				uartSendFlag.imfo_apert = 1;
				uartSendFlag.imfo_iso = 1;
				switch(flashPara.workmode)
				{
					case WORKINGMODE_MANU:
					#ifdef NIKON_D50
						//MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x40;
						WLM_PARA_CLR;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MANU;
						//MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x48;
						MAINPARA_ARRARY[LOCATION_PARAMETER_OUTPUT]=output_calc[(flashPara.output1-1)*3+flashPara.output2];
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						Timer7SetCount();
						SET_FOLLOWFLASH_CLOSE;
						/*for(i=0;i<22;i++)
						{
							putchar(MAINPARA_ARRARY[i]);
						}*/
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0xf1;
					#else
						MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x02;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MANU;
						MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x12;
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0x08;
					#endif
					break;

					case WORKINGMODE_TTL:
					#ifdef NIKON_D50
						WLM_PARA_CLR;
						//MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0xC0;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_TTLS;
						//MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x00;
						MAINPARA_ARRARY[LOCATION_PARAMETER_CMP]=cmp_calc[(flashPara.ttlcomp1+3)*3+flashPara.ttlcomp2];
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						Timer7SetCount();
						SET_FOLLOWFLASH_CLOSE;
						/*for(i=0;i<22;i++)
						{
							putchar(MAINPARA_ARRARY[i]);
						}*/
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0xed;
					#else
						MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x03;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_TTLS;
						MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x00;
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0x2f;
					#endif						
					break;

					case WORKINGMODE_MASTER:
					#ifdef NIKON_D50
						//MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x40;
						#ifdef WIRELESS_MASTER
						WLM_PARA_SET
						#endif
						//MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MANU;
						//MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x48;
						//MAINPARA_ARRARY[LOCATION_PARAMETER_OUTPUT]=output_calc[(flashPara.output1-1)*3+flashPara.output2];
						Timer7SetCount();
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						SET_FOLLOWFLASH_CLOSE;
						/*for(i=0;i<22;i++)
						{
							putchar(MAINPARA_ARRARY[i]);
						}*/
						
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0xf1;
					#else
						MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x02;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MANU;
						MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x12;
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0x08;
					#endif
					break;

					case WORKINGMODE_RTN:
					#ifdef NIKON_D50
						//MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x40;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MANU;
						//MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x48;
						MAINPARA_ARRARY[LOCATION_PARAMETER_OUTPUT]=output_calc[(flashPara.output1-1)*3+flashPara.output2];
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						wlSet.s_chan = ((uartRcvBuf[2]>>4)&0x0f);
						wlSet.s_arrary = (uartRcvBuf[2]&0x0f);
						Timer7SetCapture();
						SET_FOLLOWFLASH_OPEN;
						/*for(i=0;i<22;i++)
						{
							putchar(MAINPARA_ARRARY[i]);
						}*/
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0xf1;
					#else
						MAINPARA_ARRARY[LOCATION_PARAMETER_ZERO] = 0x02;
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MANU;
						MAINPARA_ARRARY[LOCATION_PARAMETER_SIX] = 0x12;
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						//MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = 0x08;
					#endif
					//#ifdef CANON320EX_COPY
					#ifdef EXT_LED
						if(flashPara.ledFlag)
						{
							LED_LITE = 0;
							flashPara.ledFlag = 0;
							CHARGE_ENABLE = 0;
						}
					#endif
					break;

					case WORKINGMODE_MULTI:
						WLM_PARA_CLR
						MAINPARA_ARRARY[LOCATION_PARAMETER_MODE] = PARAMETER_MODE_MULTI;
						MAINPARA_ARRARY[LOCATION_PARAMETER_OUTPUT] = uartRcvBuf[1];
						multiSet.expose = uartRcvBuf[2];	
						MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
						Timer7SetCount();
						SET_FOLLOWFLASH_CLOSE;
						//puts(MAINPARA_ARRARY);
						/*for(i=0;i<22;i++)
						{
							putchar(MAINPARA_ARRARY[i]);
						}*/
					break;

					case WORKINGMODE_F1:
					case WORKINGMODE_F2:
						Timer7SetCount();
						SET_FOLLOWFLASH_OPEN;
					break;
						
				}
				break;

				case UART_DECODE_MULTMFRQ:
				multiSet.eTimes = (int)uartRcvBuf[1];
				multiSet.frequence = uartRcvBuf[2];
				if(WORKINGMODE_MULTI==flashPara.workmode)
				{
					MAINPARA_ARRARY[LOCATION_PARAMETER_MULTIMES] = (char)multiSet.eTimes;
					MAINPARA_ARRARY[LOCATION_PARAMETER_MULFRQ] = multiSet.frequence;
					MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
				}
				break;
				
				case UART_DECODE_OUTPUT:
				flashPara.output1 = uartRcvBuf[1];
				flashPara.output2 = uartRcvBuf[2];
				if(WORKINGMODE_MANU==flashPara.workmode)
				{
					MAINPARA_ARRARY[LOCATION_PARAMETER_OUTPUT]=output_calc[(flashPara.output1-1)*3+flashPara.output2];
					MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(send_arrary2);
				}
				uartSendFlag.imfo_iso = 1;
				break;

				case  UART_DECODE_TTLCOM:
				flashPara.ttlcomp1 = uartRcvBuf[1];
				flashPara.ttlcomp2 = uartRcvBuf[2];	
				if(WORKINGMODE_TTL==flashPara.workmode)
				{
					MAINPARA_ARRARY[LOCATION_PARAMETER_CMP]=cmp_calc[(int)((flashPara.ttlcomp1+3)*3+flashPara.ttlcomp2)];
					MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(send_arrary2);
				}
				break;

				case UART_DECODE_TTLFEB:
				flashPara.ttlFeb1 = uartRcvBuf[1];
				flashPara.ttlFeb2 = uartRcvBuf[2];
				break;

				case UART_DECODE_WLMDEBUG:
				WLM_SendData(uartRcvBuf,3);
				break;

				case UART_DECODE_FOCLEN:
				uartSendFlag.imfo_focLen = 1;
				/*
				uartSend.focLen = uartRcvBuf[1];
				if(uartSend.focLen==0)
					SETBIT(bitChar,BIT_BITCHAR_MOTOAUTO);
				else
					CLRBIT(bitChar,BIT_BITCHAR_MOTOAUTO);
				*/
				break;

				case UART_DECODE_PRDIMFO:
				switch(uartRcvBuf[1])
					{
						case UART_DECODE_PRDIMFO_AUTHOR:
							putsDialog(preductionId);
						break;

						case UART_DECODE_PRDIMFO_UPDATE:
							SynPutChar(0x7e);
							SynPutChar(0xff);
							SynPutChar(0xfc);
							SynPutChar(0x00);
							SynPutChar(0x02);
							SynPutChar(0x00);
							SynPutChar(0x00);
							SynPutChar(0xfd);
							Delay_ms(10);
							asm( " jsr 0xB800");
						break;

						case UART_DECODE_PRDIMFO_UARTHOLD:
							SynPutChar(0x7e);
							SynPutChar(0xff);
							SynPutChar(0xfd);
							SynPutChar(0x00);
							SynPutChar(0x02);
							SynPutChar(0x00);
							SynPutChar(0x00);
							SynPutChar(0xfe);
							Delay_ms(10);
							UartStop();
						break;
					}
				break;

				case UART_DECODE_MULTI:
					switch(uartRcvBuf[1])
					{
						case UART_DECODE_MULTI_EXPOSE:
							multiSet.expose = uartRcvBuf[2];
						break;
					}
				break;


				case UART_DECODE_WIRELESS:
					switch(uartRcvBuf[1])
					{
						case UART_DECODE_WIRELESS_CHAN:
							//wlSet.s_chan = uartRcvBuf[2];
							wlSet.s_chan = uartRcvBuf[2];
						break;
						case UART_DECODE_WIRELESS_ARR:
							wlSet.s_arrary = uartRcvBuf[2];
						break;
					}
				break;

				case UART_DECODE_WLMSET:
	/************************************************************************************
			EB ARR|MODE P1|P2

	************************************************************************************/
			/*		if(WORKINGMODE_MANU==(uartRcvBuf[1]&0x0f))
					{
						wlSet.m_arrPara[uartRcvBuf[1]>>4].output1 = (uartRcvBuf[2] >>4);
						wlSet.m_arrPara[uartRcvBuf[1]>>4].output2 = (uartRcvBuf[2]&0x0f);
					}else if(WORKINGMODE_TTL==(uartRcvBuf[1]&0x0f))
					{
						wlSet.m_arrPara[uartRcvBuf[1]>>4].cmp1 = (uartRcvBuf[2] >>4);
						wlSet.m_arrPara[uartRcvBuf[1]>>4].cmp2 = (uartRcvBuf[2]&0x0f);
					}else if(WORKINGMODE_HOLD==(uartRcvBuf[1]&0x0f))
					{
						wlSet.m_arrPara[uartRcvBuf[1]>>4].cmp1 = (uartRcvBuf[2] >>4);
						wlSet.m_arrPara[uartRcvBuf[1]>>4].cmp2 = (uartRcvBuf[2]&0x0f);
					}*/
					
					switch((uartRcvBuf[1]>>4)&0xf)
					{
						case UART_DECODE_WLMSET_ARRM:
						if(WORKINGMODE_MANU==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_M].mode= WORKINGMODE_MANU;
							wlSet.m_arrPara[WLM_ARRAY_M].output1 = (uartRcvBuf[2] >>4);
							wlSet.m_arrPara[WLM_ARRAY_M].output2 = (uartRcvBuf[2]&0x0f);
							wlSet.m_arrMode[WLM_ARRAY_M] = (WLM_SLAMODE_MANU|0x80);
							wlSet.m_slpara[WLM_ARRAY_M] =  WLM_SETARR_OPPARA(wlSet.m_arrPara[WLM_ARRAY_M].output1,wlSet.m_arrPara[WLM_ARRAY_M].output2);
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}

						}else if(WORKINGMODE_TTL==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_M].mode = WORKINGMODE_TTL;
							wlSet.m_arrPara[WLM_ARRAY_M].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_M].cmp2= (uartRcvBuf[2]&0x0f);
							//wlSet.m_ma = WLM_SLAMODE_TTL;
							wlSet.m_arrMode[WLM_ARRAY_M] = (WLM_SLAMODE_TTL|0x80);
							wlSet.m_slpara[WLM_ARRAY_M] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_M].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_M].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							
						}else if(WORKINGMODE_AUTO==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_M].mode = WORKINGMODE_TTL;
							wlSet.m_arrPara[WLM_ARRAY_M].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_M].cmp2= (uartRcvBuf[2]&0x0f);
							//wlSet.m_ma = WLM_SLAMODE_TTL;
							//wlSet.m_arrMode[WLM_ARRAY_M] = (WLM_SLAMODE_AUTO|0x80);
							wlSet.m_arrMode[WLM_ARRAY_M] = (WLM_SLAMODE_TTL|0x80);
							wlSet.m_slpara[WLM_ARRAY_M] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_M].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_M].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							
						}else if(WORKINGMODE_HOLD==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_M].mode = WORKINGMODE_TURNOFF;
							wlSet.m_arrMode[WLM_ARRAY_M] = (WLM_SLAMODE_TURNOFF|0x80);
							wlSet.m_slpara[WLM_ARRAY_M] = 0;
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}
						break;

						case UART_DECODE_WLMSET_ARRA:
						if(WORKINGMODE_MANU==(uartRcvBuf[1]&0x0f))
						{		
							wlSet.m_arrPara[WLM_ARRAY_A].mode = WORKINGMODE_MANU;
							wlSet.m_arrPara[WLM_ARRAY_A].output1 = (uartRcvBuf[2] >>4);
							wlSet.m_arrPara[WLM_ARRAY_A].output2 = (uartRcvBuf[2]&0x0f);
							//wlSet.m_ma = WLM_SLAMODE_MANU;
							wlSet.m_arrMode[WLM_ARRAY_A] = WLM_SLAMODE_MANU;
							wlSet.m_slpara[WLM_ARRAY_A] = WLM_SETARR_OPPARA(wlSet.m_arrPara[WLM_ARRAY_A].output1,wlSet.m_arrPara[WLM_ARRAY_A].output2);
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}else if(WORKINGMODE_TTL==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_A].mode = WORKINGMODE_TTL;
							wlSet.m_arrPara[WLM_ARRAY_A].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_A].cmp2= (uartRcvBuf[2]&0x0f);
							//wlSet.m_ma = WLM_SLAMODE_TTL;
							wlSet.m_arrMode[WLM_ARRAY_A] = WLM_SLAMODE_TTL;
							wlSet.m_slpara[WLM_ARRAY_A] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_A].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_A].cmp2];
							//wlSet.m_slpara[0] = cmp_calc[(uartRcvBuf[2] >>4)*3+(uartRcvBuf[2]&0x0f)];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							//Tprintf("%d",(int)wlSet.m_arrPara[UART_DECODE_WLMSET_ARRA].cmp1);
							//Tprintf("%d",(int)wlSet.m_arrPara[UART_DECODE_WLMSET_ARRA].cmp2);
							//putchar(wlSet.m_arrPara[UART_DECODE_WLMSET_ARRA].cmp1);
							//putchar(wlSet.m_arrPara[UART_DECODE_WLMSET_ARRA].cmp2);
						}else if(WORKINGMODE_AUTO==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_A].mode = WORKINGMODE_AUTO;
							wlSet.m_arrPara[WLM_ARRAY_A].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_A].cmp2= (uartRcvBuf[2]&0x0f);
							wlSet.m_arrMode[WLM_ARRAY_A] = WLM_SLAMODE_AUTO;
							wlSet.m_slpara[WLM_ARRAY_A] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_A].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_A].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}
						else if(WORKINGMODE_HOLD==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_A].mode = WORKINGMODE_TURNOFF;
							wlSet.m_arrMode[WLM_ARRAY_A] = WLM_SLAMODE_TURNOFF;
							wlSet.m_slpara[WLM_ARRAY_A] = 0;
							//wlSet.m_ma = WLM_SLAMODE_TURNOFF;
							//wlSet.m_slpara[0] = 0;
							//wlSet.m_arrMode[]
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}
						break;

						case UART_DECODE_WLMSET_ARRB:
						if(WORKINGMODE_MANU==(uartRcvBuf[1]&0x0f))
						{		
							wlSet.m_arrPara[WLM_ARRAY_B].mode = WORKINGMODE_MANU;
							wlSet.m_arrPara[WLM_ARRAY_B].output1 = (uartRcvBuf[2] >>4);
							wlSet.m_arrPara[WLM_ARRAY_B].output2 = (uartRcvBuf[2]&0x0f);
							//wlSet.m_mb = WLM_SLAMODE_MANU;
							wlSet.m_arrMode[WLM_ARRAY_B] = WLM_SLAMODE_MANU;
							wlSet.m_slpara[WLM_ARRAY_B] = WLM_SETARR_OPPARA(wlSet.m_arrPara[WLM_ARRAY_B].output1,wlSet.m_arrPara[WLM_ARRAY_B].output2);
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}else if(WORKINGMODE_TTL==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_B].mode = WORKINGMODE_TTL;
							wlSet.m_arrPara[WLM_ARRAY_B].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_B].cmp2= (uartRcvBuf[2]&0x0f);
							//wlSet.m_mb = WLM_SLAMODE_TTL;
							wlSet.m_arrMode[WLM_ARRAY_B] = WLM_SLAMODE_TTL;
							wlSet.m_slpara[WLM_ARRAY_B] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_B].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_B].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							//Tprintf("%d",(int)wlSet.m_arrPara[UART_DECODE_WLMSET_ARRB].cmp1);
							//Tprintf("%d",(int)wlSet.m_arrPara[UART_DECODE_WLMSET_ARRB].cmp2);
							//putchar(wlSet.m_arrPara[UART_DECODE_WLMSET_ARRB].cmp1);
							//putchar(wlSet.m_arrPara[UART_DECODE_WLMSET_ARRB].cmp2);
						}else if(WORKINGMODE_AUTO==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_B].mode = WORKINGMODE_AUTO;
							wlSet.m_arrPara[WLM_ARRAY_B].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_B].cmp2= (uartRcvBuf[2]&0x0f);
							wlSet.m_arrMode[WLM_ARRAY_B] = WLM_SLAMODE_AUTO;
							wlSet.m_slpara[WLM_ARRAY_B] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_B].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_B].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							
						}else if(WORKINGMODE_HOLD==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_B].mode = WORKINGMODE_TURNOFF;
							//wlSet.m_mb = WLM_SLAMODE_TURNOFF;
							//wlSet.m_slpara[1] = 0;
							wlSet.m_arrMode[WLM_ARRAY_B] = WLM_SLAMODE_TURNOFF;
							wlSet.m_slpara[WLM_ARRAY_B] = 0;
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}
						break;

						case UART_DECODE_WLMSET_ARRC:
						if(WORKINGMODE_MANU==(uartRcvBuf[1]&0x0f))
						{		
							wlSet.m_arrPara[WLM_ARRAY_C].mode = WORKINGMODE_MANU;
							wlSet.m_arrPara[WLM_ARRAY_C].output1 = (uartRcvBuf[2] >>4);
							wlSet.m_arrPara[WLM_ARRAY_C].output2 = (uartRcvBuf[2]&0x0f);
							//wlSet.m_mc = WLM_SLAMODE_MANU;
							wlSet.m_arrMode[WLM_ARRAY_C] = WLM_SLAMODE_MANU;
							wlSet.m_slpara[WLM_ARRAY_C] = WLM_SETARR_OPPARA(wlSet.m_arrPara[WLM_ARRAY_C].output1,wlSet.m_arrPara[WLM_ARRAY_C].output2);
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}else if(WORKINGMODE_TTL==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_C].mode = WORKINGMODE_TTL;
							wlSet.m_arrPara[WLM_ARRAY_C].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_C].cmp2= (uartRcvBuf[2]&0x0f);
							//wlSet.m_mc= WLM_SLAMODE_TTL;
							wlSet.m_arrMode[WLM_ARRAY_C] = WLM_SLAMODE_TTL;
							wlSet.m_slpara[WLM_ARRAY_C] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_C].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_C].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							//Tprintf("%d",(int)wlSet.m_arrPara[UART_DECODE_WLMSET_ARRC].cmp1);
							//Tprintf("%d",(int)wlSet.m_arrPara[UART_DECODE_WLMSET_ARRC].cmp2);
							//putchar(wlSet.m_arrPara[UART_DECODE_WLMSET_ARRC].cmp1);
							//putchar(wlSet.m_arrPara[UART_DECODE_WLMSET_ARRC].cmp2);
						}else if(WORKINGMODE_AUTO==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_C].mode = WORKINGMODE_AUTO;
							wlSet.m_arrPara[WLM_ARRAY_C].cmp1= (uartRcvBuf[2] >>4)-3;
							wlSet.m_arrPara[WLM_ARRAY_C].cmp2= (uartRcvBuf[2]&0x0f);
							wlSet.m_arrMode[WLM_ARRAY_C] = WLM_SLAMODE_AUTO;
							wlSet.m_slpara[WLM_ARRAY_C] = cmp_calc[((int)wlSet.m_arrPara[WLM_ARRAY_C].cmp1+3)*3+wlSet.m_arrPara[WLM_ARRAY_C].cmp2];
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
							
						}else if(WORKINGMODE_HOLD==(uartRcvBuf[1]&0x0f))
						{
							wlSet.m_arrPara[WLM_ARRAY_C].mode = WORKINGMODE_TURNOFF;
							//wlSet.m_mc= WLM_SLAMODE_TURNOFF;
							//wlSet.m_slpara[2] = 0;
							wlSet.m_arrMode[WLM_ARRAY_C] = WLM_SLAMODE_TURNOFF;
							wlSet.m_slpara[WLM_ARRAY_C] = 0;
							if(WORKINGMODE_MASTER==flashPara.workmode)
							{
								WLM_PARA_SET;
								MAINPARA_ARRARY[LOCATION_PARAMETER_REAR] = VerCodeCal(MAINPARA_ARRARY);
							}
						}
						break;

						case UART_DECODE_WLMSET_CHAN:
							wlSet.m_chan = uartRcvBuf[2];
						break;
					}
				break;

				case UART_DECODE_CMD:
				/*if(uartRcvBuf[1]==UART_DECODE_CMD_PILOT)
				pilot();*/
				switch(uartRcvBuf[1])
				{
					case UART_DECODE_CMD_PILOT:
					pilot();
					break;

					case UART_DECODE_CMD_HOTWARN:
					flashStatue.flash_enable |= FLASH_STATE_TEMPALARM;
					break;

//#ifndef CANON320EX_COPY
#ifdef MOTO_VER
					case UART_DECODE_CMD_CHARGE_ENABLE:
					CHARGE_ENABLE = 0;
					break;

					case UART_DECODE_CMD_CHARGE_DISABLE:
					CHARGE_ENABLE = 1;
					break;
#endif

					case UART_DECODE_CMD_LOWBAT:
					flashStatue.flash_enable |= FLASH_STATE_TEMPALARM;	
					break;

					case UART_DECODE_CMD_MUSIC:
					flashPara.music_set = uartRcvBuf[2];
					break;

					case UART_DECODE_CMD_INIT:
					uartSendFlag.imfo_apert = 1;
					uartSendFlag.imfo_focLen = 1;
					uartSendFlag.imfo_iso = 1;
					uartSendFlag.charge = 1;
					uartSendFlag.cls = 1;
					//uartSend.pApert = 0xff;
					//uartSend.pIso = 0xff;
					//uartSend.pFocLen = 0xff;
					//charge_ok = 0xff;
					//uartSendFlag.charge = 1;
					//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
					break;

					case UART_DECODE_CMD_LED:
//#ifdef CANON320EX_COPY
#ifdef EXT_LED

					if(UART_DECODE_CMD_LED_ON==uartRcvBuf[2])
					{
						if((!flashPara.rmtIndex)||(WORKINGMODE_MASTER>=flashPara.workmode))
						{
							LED_LITE = 1;
							CHARGE_ENABLE = 1;
							flashPara.ledFlag = 1;
						}
					}else 
					{
						if((!flashPara.rmtIndex)||(WORKINGMODE_MASTER>=flashPara.workmode))
						{
							LED_LITE = 0;
							CHARGE_ENABLE = 0;
							flashPara.ledFlag = 0;
						}
					}

#endif
					break;

					case UART_DECODE_CMD_FACELITE:
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
					if(uartRcvBuf[2]&0x02)
					{
						flashPara.faceLite = 1;//uartRcvBuf[2];
					}else
					{
						flashPara.faceLite = 0;
					}

					if(uartRcvBuf[2]&0x01)
					{
						if(!flashPara.ledFlag)
						{
							flashPara.rmtIndex = 1;
						}
					}else
					{
						if(!flashPara.ledFlag)
						{
							flashPara.rmtIndex = 0;
							LED_LITE = 0;
						}
					}
#endif
					break;

//#ifdef CANON320EX_COPY
#ifdef EXT_LED
					case UART_DECODE_CMD_AFLITE:
					flashPara.auxLiteSel = uartRcvBuf[2];
					break;
#endif

					case UART_DECODE_CMD_RMTSHUT:
					flashPara.rmtShut = 1;
					break;

					case UART_DECODE_CMD_RMTOPEN:
					flashPara.rmtShut = 0;
					break;

					case UART_DECODE_CMD_SLEEP:
					lowPowerSleep();
					uartSendFlag.charge = 1;
					flashPara.ledFlag = 0;
					break;
				}
				break;
				


			}
			bufIndex = 0;
			loaden = 0;	
		}
		
	}else if(uart_rcvData==0xaa)
	{
		loaden = 1;
	}
}

int DecodeFlashData(unsigned char id, unsigned char para1, unsigned char para2)
{
	unsigned char uLoadIndex=0;
	//unsigned char uartLoad[4];
	unsigned char uartLoad[5]={0};
	char i=0;
		
	//uartLoad[uLoadIndex++] = 0xcc;
	uartLoad[uLoadIndex++] = 0xaa;
	uartLoad[uLoadIndex++] = id;
	uartLoad[uLoadIndex++] = para1;  
	uartLoad[uLoadIndex++] = para2;
	uartLoad[uLoadIndex++] = (id+para1+para2);

	if(uart_tran_flag) 
	{
		return err;	
	}
	
	else
	{
		while(i<MAX_UART_DATA_LEN)
		{
		 	gBUartBuf[gBUartTotalByte++] = uartLoad[i++]; 		//gBUartTotalByte must be 0;
		}

		i = 0;
		uart_tran_flag = 1;									//set transent flag
		//SETBIT(SC0TICR,0);											//set interrupt
		disable_irq();
		MEMCTR   |=  0x04;
		SETBIT(SC0TICR,0);
		MEMCTR   &= ~0x04;
		enable_irq();
		return 0;
	}


	//return(UartDecode(uartLoad));
	
	
//	for	(uLoadIndex=0;uLoadIndex<4;uLoadIndex++)
//	{
//		putchar(uartLoad[uLoadIndex]);
//	}

}	

#if 0
void LoadUartDate()
{
	if((uartSendFlag.charge)/*&&(uartSendFlag.index==0)*/)
	{
		if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CHARGE,charge_ok))
		{
			uartSendFlag.charge = 0;
			//uartSendFlag.index=1;
			//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
			//return;
		}
			//uartSend.sendFlag &= 0xfe;
	}
		
	if((uartSendFlag.tempCheck)/*&&(1==uartSendFlag.index)*/)
	{
		if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_TEMP,UART_ENCODE_FUNC_TEMP_ALARM))
		{
			uartSendFlag.tempCheck = 0;
			//uartSendFlag.index=2;
			//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_TEMPALARM);
			
		}				
				//uartSend.tempAlarm = 0;
	}

		
	if((uartSendFlag.imfo_iso)/*&&(2==uartSendFlag.index)*/)
	{
		if(!DecodeFlashData(UART_ENCODE_ISO,((uartSend.ISO>>8)&0xff),uartSend.ISO&0xff))
		{
			uartSendFlag.imfo_iso = 0;
			//uartSendFlag.index=3;
			//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_ISO);
			//return;
		}
	}

		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_APERT))
		
	if((uartSendFlag.imfo_apert)/*&&(3==uartSendFlag.index)*/)
	{

		if(!DecodeFlashData(UART_ENCODE_APERT,((uartSend.apert>>8)&0xff),uartSend.apert&0xff))
		{
			uartSendFlag.imfo_apert = 0;
			//uartSendFlag.index=4;
		}

	}
		
		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CLS))
		if((uartSendFlag.cls)/*&&(4==uartSendFlag.index)*/)
		{
			if(!uartSend.cls)
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CLS,UART_ENCODE_FUNC_CLS_UNCOM))
				{
					uartSendFlag.cls = 0;
					//uartSendFlag.index=5;
				}
			}else
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CLS,UART_ENCODE_FUNC_CLS_COM))
				{
					uartSendFlag.cls = 0;
					//uartSendFlag.index=5;
				}
			}

		}
		
		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_FOCLEN))
		if((uartSendFlag.imfo_focLen)/*&&(5==uartSendFlag.index)*/)
		{
			if(!DecodeFlashData(UART_ENCODE_FOCLEN,uartSend.focLen,0))
			{
				//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_FOCLEN);
				uartSendFlag.imfo_focLen = 0;
				//uartSendFlag.index=6;
			}				
		}

		if((uartSendFlag.rearCurtain)/*&&(6==uartSendFlag.index)*/)
		{
			if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_MODE,uartSend.mode))
			{
				uartSendFlag.rearCurtain = 0;
				//uartSendFlag.index=7;
			}
		}

		if((uartSendFlag.flashWarn)/*&&(7==uartSendFlag.index)*/)
		{

			if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0))
			{
				uartSendFlag.flashWarn= 0;
				//uartSendFlag.index=8;
			}

		}

		//if(GETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLMODE))
		if((uartSendFlag.wlm_mode)/*&&(8==uartSendFlag.index)*/)
		{
			if(!DecodeFlashData(UART_ENCODE_WIRELESS,wlSet.s_wlMode,(nikWireless.WL_output1<<4|nikWireless.WL_output2)))
			{
				uartSendFlag.wlm_mode = 0;
				wlSet.s_wlMode = WORKINGMODE_TURNOFF;
			}

		}

}
#endif


void LoadUartDate()
{

//#ifdef HP_SAMPLE_TEST
//	static unsigned int adSample;
//#endif

	//if(uartSend.sendFlag)
	{
		#if 0
		if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE))
		{
			/*if(uartSend.chargeOK==CHARGE_OK)
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CHARGE,UART_ENCODE_FUNC_CHARGE_OK))
				{
					CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
				}
					//puts("chargeok\n");
			}else if(uartSend.chargeOK==CHARGE_UNDER)
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CHARGE,UART_ENCODE_FUNC_CHARGE_ING))
				{
					CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
				}
				//puts("charging\n");
			} */
			if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CHARGE,uartSend.chargeOK))
				CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
				//uartSend.sendFlag &= 0xfe;
		}
		#endif
		
		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_TEMPALARM))
		if((uartSendFlag.charge)/*&&(uartSendFlag.index==0)*/)
		{
			if(uartSendFlag.index==0)
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CHARGE,charge_ok))
				{
					uartSendFlag.charge = 0;
					uartSendFlag.index=1;
					//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
					//return;
				}
				return;
			}
				//uartSend.sendFlag &= 0xfe;
		}else
		{
			if(uartSendFlag.index==0)
			uartSendFlag.index=1;
		}
		
		if((uartSendFlag.tempCheck)/*&&(1==uartSendFlag.index)*/)
		{
			if(1==uartSendFlag.index)
			{

				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_TEMP,UART_ENCODE_FUNC_TEMP_ALARM))
				{
					uartSendFlag.tempCheck = 0;
					uartSendFlag.index=2;
					//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_TEMPALARM);
					
				}
				return;					
					//uartSend.tempAlarm = 0;
			}
		}else
		{
			if(1==uartSendFlag.index)
			uartSendFlag.index=2;
		}

		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ISO))
		
		if((uartSendFlag.imfo_iso)/*&&(2==uartSendFlag.index)*/)
		{
			if(2==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_ISO,((uartSend.ISO>>8)&0xff),uartSend.ISO&0xff))
				{
					uartSendFlag.imfo_iso = 0;
					uartSendFlag.index=3;
					//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_ISO);
					//return;
				}
				return;

			}
		}else
		{
			if(2==uartSendFlag.index)
			uartSendFlag.index=3;
		}

		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_APERT))
		
		if((uartSendFlag.imfo_apert)/*&&(3==uartSendFlag.index)*/)
		{
			if(3==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_APERT,((uartSend.apert>>8)&0xff),uartSend.apert&0xff))
				{
					uartSendFlag.imfo_apert = 0;
					uartSendFlag.index=4;
					//return;
					//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_APERT);
					//return;
				}
				return;
			}
		}else
		{
			if(3==uartSendFlag.index)
			uartSendFlag.index=4;
		}
		
		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CLS))
		if((uartSendFlag.cls)/*&&(4==uartSendFlag.index)*/)
		{
			if(4==uartSendFlag.index)
			{
				if(!uartSend.cls)
				{
					if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CLS,UART_ENCODE_FUNC_CLS_UNCOM))
					{
						uartSendFlag.cls = 0;
						uartSendFlag.index=5;
						//return;
						//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CLS);
						//return;
					}
					return;
				}else
				{
					if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CLS,UART_ENCODE_FUNC_CLS_COM))
					{
						uartSendFlag.cls = 0;
						uartSendFlag.index=5;
						//return;
						//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CLS);
						//return;
					}
					return;
				}
			}
		}else
		{
			if(4==uartSendFlag.index)
			uartSendFlag.index=5;
		}
		
		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_FOCLEN))
		if((uartSendFlag.imfo_focLen)/*&&(5==uartSendFlag.index)*/)
		{
			if(5==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_FOCLEN,uartSend.focLen,0))
				{
					//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_FOCLEN);
					uartSendFlag.imfo_focLen = 0;
					uartSendFlag.index=6;
					//return;
					//return;
				}				
				return;
				
			}
		}else
		{
			if(5==uartSendFlag.index)
			uartSendFlag.index=6;
		}
/*		if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_DEBUG))
		{
			//if(!DecodeFlashData(UART_ENCODE_DEBUG,uartSend.debug,0))
				//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_DEBUG);
			//Tprintf("%x %x %x %x \n",(int)flashPara.ttlcomp1,(int)flashPara.ttlcomp2,(int)((flashPara.ttlcomp1+3)*3+flashPara.ttlcomp2),(int)cmp_calc[(int)((flashPara.ttlcomp1+3)*3+flashPara.ttlcomp2)]);
			CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_DEBUG);
		}
*/

		//if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE))
/*
		if(uartSendFlag.charge)
		{
			if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_CHARGE,charge_ok))
			{
				uartSendFlag.charge = 0;
				//CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
				//return;
			}
				//uartSend.sendFlag &= 0xfe;
		}
*/

/*		if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_AUXRLIT))
		{
			if(flashStatue.auxr_light)
			{
				if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_AUXRLIT,UART_ENCODE_CMD_AUXRLIT_ON))
				{
					CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_AUXRLIT);
					return;
				}
			}else
			{
				if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_AUXRLIT,UART_ENCODE_CMD_AUXRLIT_OFF))
				{
					CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_AUXRLIT);
					return;
				}
			}
		}


	//	if(GETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH))
		{
			if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0))
			{
				CLRBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
				return;
			}
		}

*/


		//if(GETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_REAR))
		if((uartSendFlag.rearCurtain)/*&&(6==uartSendFlag.index)*/)
		{
			if(6==uartSendFlag.index)
			{
			/*
				if(GETBIT(uartSend.mode,CAMERA_MODE_REAR))
				{
					if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_MODE,UART_ENCODE_FUNC_REAR_ON))
					{
						uartSendFlag.rearCurtain = 0;
						uartSendFlag.index=7;
						return;
						//CLRBIT(uartSend.sendFlag,UARTFLAGEX_INDEX_REAR);
						//CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_REAR);
						//return;
					}
				}else
				{
					if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_MODE,UART_ENCODE_FUNC_REAR_OFF))
					{
						uartSendFlag.rearCurtain = 0;
						uartSendFlag.index=7;
						return;
						//CLRBIT(uartSend.sendFlag,UARTFLAGEX_INDEX_REAR);
						//CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_REAR);
						//return;
					}
				}
			*/
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_MODE,uartSend.mode))
				{
					uartSendFlag.rearCurtain = 0;
					//uartSendFlag.index=7;
					uartSendFlag.index=7;
				}
				return;
			}
		}else
		{
			if(6==uartSendFlag.index)
			//uartSendFlag.index=7;
			uartSendFlag.index=7;
		}

		//if(GETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_LOWBAT))
/*		if(uartSendFlag.lowBat)
		{
			if(GETBIT(bitChar,BIT_BITCHAR_LOWBATWARN))
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_LOWBAT,UART_ENCODE_FUNC_LOWBAT_WARNING))
				{
					CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_LOWBAT);
					return;
				}
			}else
			{
				if(!DecodeFlashData(UART_ENCODE_FUNC,UART_ENCODE_FUNC_LOWBAT,UART_ENCODE_FUNC_LOWBAT_CANCEL))
				{
					CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_LOWBAT);
					return;
				}
			}
		}
*/


//		if(GETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH))
//		{
//			if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0))
//			{
//				CLRBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
//			}
//		}

		if(uartSendFlag.clear)
		{
			if(7==uartSendFlag.index)
			{
				if(!DecodeFlashData(0x55,0x55,0x55))
				{
					uartSendFlag.clear = 0;
					uartSendFlag.index=8;
				}
				return;
			}
		}else
		{
			if(7==uartSendFlag.index)
			{
				uartSendFlag.index = 8;
			}
		}

		


#if 0
		if((uartSendFlag.wlm_mode)/*&&(8==uartSendFlag.index)*/)
		{
			if(8==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_WIRELESS,wlSet.s_wlMode,(nikWireless.WL_output1<<4|nikWireless.WL_output2)))
				{
					uartSendFlag.wlm_mode = 0;
					wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					uartSendFlag.index=9;
				}
				return;
			}

		}else
		{
			if(8==uartSendFlag.index)
			{
				uartSendFlag.index=9;
			}

		}

		if(uartSendFlag.flashWarn)
		{			
			if(9==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0))
				{
					uartSendFlag.flashWarn= 0;
					uartSendFlag.index=0;
					//CLRBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
					//return;
				}
				return;
			}
		}else
		{
			if(9==uartSendFlag.index)
			uartSendFlag.index=0;
		}
#endif
		

#if 1
		if((uartSendFlag.flashWarn)/*&&(7==uartSendFlag.index)*/)
		{
			//if(GETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH))
			//{
			//	if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0))
			//	{
			//		CLRBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
			//		return;
			//	}
			//}
			HaltInterval(100);
			
			if(8==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0))
				{
					uartSendFlag.flashWarn= 0;
					uartSendFlag.index=9;
					//CLRBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
					//return;
				}
				return;
			}
		}else
		{
			if(8==uartSendFlag.index)
			uartSendFlag.index=9;
		}

		//if(GETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLMODE))
		if((uartSendFlag.wlm_mode)/*&&(8==uartSendFlag.index)*/)
		{
			if(9==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_WIRELESS,wlSet.s_wlMode,(nikWireless.WL_output1<<4|nikWireless.WL_output2)))
				{
					uartSendFlag.wlm_mode = 0;
					wlSet.s_wlMode = WORKINGMODE_TURNOFF;
#ifdef HP_SAMPLE_TEST
					uartSendFlag.index=10;
#else
					uartSendFlag.index=0;
#endif
				}
				return;
			}

		}else
		{
			if(9==uartSendFlag.index)
#ifdef HP_SAMPLE_TEST
					uartSendFlag.index=10;
#else
					uartSendFlag.index=0;
#endif
		}
#endif

#ifdef HP_SAMPLE_TEST
		if(uartSendFlag.hpsFlag)
		{
			if(10==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_HPSTEST,adSample>>8,adSample&0xff))
				{
					uartSendFlag.hpsFlag = 0;
					uartSendFlag.index = 0;
				}
			}
		}else
		{
			if(10==uartSendFlag.index)
			{
			
				uartSendFlag.index = 0;
			}
		}
/*
		{
			HP_SimpleInit();
			HaltSec(1);
			while(ANCTR2&0x80);
			//if(adSample != ((unsigned int)ANBUF1<<8 | ANBUF0))
			if(adSample!=MAKEWORD(ANBUF0,ANBUF1))
			{
				uartSendFlag.hpsFlag = 1;
				adSample = MAKEWORD(ANBUF0,ANBUF1);
			}
			if(uartSendFlag.hpsFlag)
			{
				if(10==uartSendFlag.index)
				{
					if(!DecodeFlashData(UART_ENCODE_HPSTEST,adSample>>8,adSample&0xff))
					{
						uartSendFlag.hpsFlag = 0;
						uartSendFlag.index=0;
					}
				}
			}else
			{
				if(10==uartSendFlag.index)
				{
					uartSendFlag.index = 0;
				}
			}
			//AD_init();
			
		}
*/
#endif
		
/*		if(GETBIT(bitWireless,BIT_bitWireless_mode))
		{
			if(!DecodeFlashData(UART_ENCODE_WIRELESS,UART_ENCODE_WIRELESS_MODE,wlSet.s_wlMode))
			{
					//CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLMODE);
				CLRBIT(bitWireless,BIT_bitWireless_mode);
			}
		}
*/		

//		if((uartSendFlag.wlm_exp)/*&&(9==uartSendFlag.index)*/)
/*		{	
			if(9==uartSendFlag.index)
			{
				if(!DecodeFlashData(UART_ENCODE_WLMOUT,flashPara.output1,flashPara.output2))
				{
					uartSendFlag.wlm_exp= 0;
					uartSendFlag.index=0;
				}
				return;
			}
		}else
		{
			if(9==uartSendFlag.index)
			uartSendFlag.index=0;
		}
*/

		//if(uartSendFlag.index>8)
		//{
			//if(8==uartSendFlag.index)
			//uartSendFlag.index=0;
		//}

#ifdef TTL_DEBUG
#ifndef TTL_DEBUG_FULLTIME
		if(GETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_DEBUGDATA))
		{
			if(GETBIT(bitChar,BIT_BITCHAR_DEBUGERROR))
			{
				if(!DecodeFlashData(UART_ENCODE_DEBUG,UART_ENCODE_DEBUG_ERROR,0))
					CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_DEBUGDATA);
			}else
			{
				if(!DecodeFlashData(UART_ENCODE_DEBUG,ttl_pre2_flag,ttl_debugData))
					CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_DEBUGDATA);
			}
			
		}
#else

		if(GETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_FULLTIMEDEBUG))
		{
			if(!DecodeFlashData(UART_ENCODE_DEBUG,0,flashStatue.full_flash))
			{
				CLRBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_FULLTIMEDEBUG);
				return;
			}
		}
#endif
#endif		
	}
	
}


void lowPowerSleep()
{
//	IRQ0ICR |= 0X20;
	POWER = 0;
	IGBT_CONTROL=0;
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
	LED_LITE = 0;
	CHARGE_ENABLE = 0;
#endif

#ifdef EXT_CHARGE
	EXTERN_CHARGE = 0;
#endif

	IRQCNT |= 0X0F;
	SET_SBT_IN;		//Set clock output as input,
	//SET_NSS_OUTPUT;
	//P0ODC |= 0x20;
	//NIKONNSSO = 0;
	flashPara.sleep_flag = SLEEP_MODE_ON;
	SET_WAKEUP_IN;
//	IRQ0ICR &= 0X3F;
//	IRQ0ICR |= 0X10;
//	IRQ0ICR |= 0X02;

	disable_irq();	
	MEMCTR   |=  0x04;
/*	CLRBIT(IRQ0ICR,1);
	CLRBIT(IRQ0ICR,5);
	CLRBIT(IRQ0ICR,0);
	SETBIT(IRQ0ICR,1);*/
//	IRQ0ICR = 0XA2;
	STB_MSK = 0X0F;
	IRQ0ICR = 0X22;
	IRQ1ICR = 0X02;
	//IRQ2ICR = 0X02;
	IRQ3ICR = 0X02;
	MEMCTR   &= ~0x04;
	enable_irq();
	WAKE_UP = 1;
	SET_WAKEUP_IN;

	CPUM |= 0x08;
	asm ("  nop  \n");
    	asm ("  nop  \n");
    	asm ("  nop  \n");

	//NIKONNSSO = 1;
	disable_irq();	
	MEMCTR   |=  0x04;
	IRQ0ICR = 0X00;
	IRQ1ICR = 0X82;
	IRQ3ICR = 0X00;
	MEMCTR   &= ~0x04;
	enable_irq();

	//P0ODC &= 0xDF;
	//SET_NSS_INPUT;
	WAKE_UP = 1;
	POWER = 1;
	IGBT_CONTROL=1;
//#ifdef CANON320EX_COPY
#ifdef EXT_CHARGE
	EXTERN_CHARGE = 1;
#endif
	flashPara.sleep_flag = SLEEP_MODE_OFF;
	charge_ok = AD_CHARGE_LOW;
	flashStatue.flash_enable &= FLASH_STATE_INCHARGE;
	SET_WAKEUP_IN;

//	TM0_stop();

//	WAKE_UP = 0;
/*
	Delay();
	WAKE_UP = 0;
	Delay();
	WAKE_UP = 1;
	POWER = 1;
	IGBT_CONTROL=1;
	SET_SBT_OUT;
	disable_irq();	
	MEMCTR   |=  0x04;
	IRQ0ICR = 0XA0;
	MEMCTR   &= ~0x04;
	enable_irq();
*/

/*
	switch(flashPara.workmode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_MANU:
		case WORKINGMODE_MULTI:
		case WORKINGMODE_S1:
		disable_irq();	
		MEMCTR   |=  0x04;
		SET_FOLLOWFLASH_CLOSE;
		MEMCTR   &= ~0x04;
		enable_irq();
		break;

		case WORKINGMODE_S2:
		disable_irq();	
		MEMCTR   |=  0x04;
		SET_FOLLOWFLASH_OPEN;
		MEMCTR   &= ~0x04;
		enable_irq();
		break;
	}
*/

//	putsDialog("wakeup\n");

}

void pilot()
{

//#ifdef UART_SENDDATA_CONTINUE
//		if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==1)||(charge_ok==AD_CHARGE_LOW))
//#else
//		if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==1)||(GETBIT(bitIndex,BIT_bitIndex_CHARGE)==0))
//#endif
//		return;
	char muitlCount=0;

	if(flashStatue.flash_enable!=0x01)
	{
		//DecodeFlashData(0xbb,flashStatue.flash_enable,flashStatue.flash_enable);
		return;
	}

	if((WORKINGMODE_MANU==flashPara.workmode)||(WORKINGMODE_TTL==flashPara.workmode)
		/*||(WORKINGMODE_S1==flashPara.workmode)*/||(WORKINGMODE_RTN==flashPara.workmode))
	{
#ifdef HP_SAMPLE_TEST
		 HP_SimpleInit();
#endif
		disable_irq();
		pTime = OutputSwitch(WORKINGMODE_MANU);
		TURN_ON_TRIGER(pTime);
		enable_irq();
#ifdef HP_SAMPLE_TEST
		AD_init(1);
#endif
	}else if(WORKINGMODE_MULTI==flashPara.workmode)
	{
	/*	while(muitlCount<multiSet.eTimes)
		{
			EA = 0;
			pTime = OutputSwitch(WORKINGMODE_MULTI);
			TURN_ON_TRIGER(pTime);
			EA = 1;			
			muitlCount ++;
			HaltSec(1000/multiSet.frequence);
		}
	*/
		//if((int)(1000/multiSet.frequence*multiSet.eTimes)<cameraSetting.shutter)
		if((int)(1000/multiSet.frequence*multiSet.eTimes)<shortShutter[19])
		{
			multiSet.finalExpTimes = multiSet.eTimes;
		}else
		{
			multiSet.finalExpTimes = (int)((((unsigned long)shortShutter[19]*(unsigned long)multiSet.frequence))/1000);
			/*
			if((((unsigned long)shortShutter[19]*(unsigned long)multiSet.frequence)/1000))
			{
				multiSet.finalExpTimes = (int)((((unsigned long)shortShutter[19]*(unsigned long)multiSet.frequence)/1000)+1);
			}else
			{
				multiSet.finalExpTimes = (int)((((unsigned long)shortShutter[19]*(unsigned long)multiSet.frequence))/1000);
			}*/
		}

		//take a flash at first
		disable_irq();
		pTime = OutputSwitch(WORKINGMODE_MULTI);
		TURN_ON_TRIGER(pTime);
		enable_irq();

		HaltSec(1000/multiSet.frequence);

		//while(muitlCount<multiSet.finalExpTimes+1)
		while(muitlCount<multiSet.finalExpTimes-1)
		{
			//EA = 0;
			disable_irq();
			pTime = OutputSwitch(WORKINGMODE_MULTI);
			TURN_ON_TRIGER(pTime);
			enable_irq();			
			muitlCount ++;
			HaltSec(1000/multiSet.frequence);
		}

		
	/*	pTime = OutputSwitch(WORKINGMODE_MULTI);
		TURN_ON_TRIGER(pTime);
		multiSet.finalExpTimes--;
		multiSet.lowFrqCnt = (1000/multiSet.frequence);			
		//UPDATA_TIMER0(0XF8,0XCB);
		STOP_PCA_TIMER
		mulTmp = MAKEWORD(CL,CH);
		mulTmp += CHANNEL1_1MS;
		START_PCA_TIMER
		MUL_PCATIMER_SETVALUE(mulTmp>>8, mulTmp&0XFF);
		MUL_PCATIMER_START;
	*/
	}else if(WORKINGMODE_MASTER==flashPara.workmode)
	{
		disable_irq();
		wlSet.m_sendCmdEn = 1;
		wlSet.m_flaCmd[0] = 0xF1;
		WLM_SendData(&wlSet.m_flaCmd[0],1);
		wlSet.m_sendCmdEn = 0;
		enable_irq();
	}else
	{
		disable_irq();
		pTime = OutputSwitch(WORKINGMODE_MANU);
		TURN_ON_TRIGER(pTime);
		enable_irq();
	}
	
	return;

}


#ifdef _FORMAT_PRINT_
char *_itoa(long n, int base)
 {
  	char *p;
 	int minus;
 	static char buf[16];

//	puts_("enter _itoa\n");
//	put_hex(n);

 	p = &buf[16];
 	*--p =  0;
 	if (n < 0) 
	{
  		minus = 1;
  		n = -n;
 	}
 	else
  		minus = 0;


 	if (n == 0)
	{
  		*--p = '0';
	}
 	else
  		while (n > 0) 
		{  
	//		puts_("_itoa ");
   			*--p = "0123456789abcdef"[n % base];
	//		putchar(*p);
   			n /= base;
  		}

 	if (minus)
  		*--p = '-';

 	return p;
}
/*
int printfDig( long n, int base )
{
	return Tprintf( _itoa( n, base ) );
	put_byte(0x0D);
    put_byte(0x0A);	
}
*/
int Printf(const char *fmt, ...)
{
	int i;
	const char *s;
 	int d;
	char buf[16];
	char *p = buf;
//	char buf[16];
//	char *p = buf;
 	va_list ap;

    va_start(ap, fmt);
	i = 16;

    while (*fmt) 
	{
        if (*fmt != '%') 
		{
            SynPutChar(*fmt++);
            continue;
        }
		
		switch(*++fmt)
		{
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
				{
                    SynPutChar(*s);
                }
                break;
            case 'd':			
                d = va_arg(ap, int);
				//d>>=8;
                p = _itoa(d, 10);
                for (s = p; *s; s++) 
				{
                    SynPutChar(*s);
                }

				break;

            case 'x':
			case 'X':
                d = va_arg(ap, int);
                p = _itoa(d, 16);
                for (s = p; *s; s++) 
				{
                    SynPutChar(*s);
                }
                break;
             //Add other specifiers here...              
            default:  
                putsDialog(fmt);
                break;
        } 
        fmt++;
    }
    va_end(ap);

    return 1;   /* Dummy return value */
} 
#endif

// -------------------- End int_SC0r() ---------------------------------------


