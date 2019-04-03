#include "sr101ef51a.h"
#include "MN101EF52A.h"
#include "init.h"
#include "nikon.h"
//#include "Golbal.h"

#define TRANSFER_START		1		
#define TRANSFER_READY 	0

char g_srFlag=0;
#pragma	_interrupt IRQ_ClkSyn	

#define START_TIMERA {TMAMD1= 0X08;}
#define STOP_TIMERA   {TMAMD1 = 0X00;}

void ClkSynInit(void)
{
//	PADIR &= 0XFE;		//PA port set
//	PADIR &= 0XFD;
//	PADIR |= 0X04;

//	TM0MD= 0x00;			
//	CK0MD = 0x08;	/*20M/4=5M*/	/*1/128 prescale*/
//	TM0OC = (80-1); /*1/5M*250=50us*/	
//	TM0MD |= 0x08;
	TMAMD1 = 0X00;
	TMAOC = 80;
	TMAMD2 = 0X40;
	TMAMD1 |= 0X08; 

	//P0DIR &= 0X7F;
	//P0DIR &= 0XDF;
	P0DIR |= 0XC0;
//	P0ODC |= 0XC0;			//cannot be set
	P0PLU  |= 0XE0;
	
	SC4MD1 = 0X00;			//disable clksyn interface
	SC4MD0 = 0X17;			//set falling edge, little endian, 8 bits
//	SC4MD2 = 0X4B;			//keep last bit as send
	SC4MD2 = 0X4F;			//keep last bit as send, select send clock as timer
//	SC4SEL = 0X10;
	SC4SEL = 0X17;
	SC4MD3 = 0X40;			//select clksyn
//	SC4MD1 = 0XF0;			//SBO4 as input
	SC4MD1 = 0XF4;			//the spb 2--1:master     0:slave

	DISABLE_CLKSYN
		
/*	SC4MD3 = 0X00;
	SC4MD0 = 0X87;
	SC4MD1 = 0X70;
	SC4MD2 = 0X0C;*/
	
	disable_irq();			
	MEMCTR   |=  0x04;		
	/*SCINTSEL |=  0x01;*/	
	//SC4ICR |=  0x82;		/*turn on tx interrupt	 */
	SC4ICR = 0x82;
	MEMCTR   &= ~0x04;		
	enable_irq();
	
//	TXBUF4 = 0x55;
			
}

static void ClkSynSend(unsigned char *pSend, char size)
{
	char index=0;
	
	while(index<size)
	{
		g_srFlag = TRANSFER_START;
		TXBUF4 = pSend[index++];
		while(g_srFlag);
	}	
}

static void ClkSynRcv(unsigned char *pSend, char size)
//void ClkSynRcv(unsigned char *pSend, char size)
{
	char index=0;

	while(index++<size)
	{
		//WAKE_UP = 1;
		g_srFlag = TRANSFER_START;
		
		//STOP_TIMERA;
		//TMAOC = 10;	
		TXBUF4 = 0x03;
		//START_TIMERA;
		while(SC4TEMP);
		//STOP_TIMERA;
		//TMAOC = 80;
		//START_TIMERA;
		
		while(g_srFlag);
				
		*pSend = RXBUF4;
		//WAKE_UP = 0;
		
	}
}

void IRQ_ClkSyn(void)
{
	g_srFlag = TRANSFER_READY;
}

char LIT_DATA_Send(unsigned char dat)
{
		SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);

		while(!(TM7ICR&0X01))
		{
			if(!NIKONNSSI)
			{
				SET_TIMER_STOP;
				break;
			}
		}
		
		if(GETBIT(TM7ICR,0))
		{
			SET_TIMER_STOP;
			//CLRBIT(TM7ICR,0);
			disable_irq(); 
			MEMCTR |= 0X04; 
			TM7ICR &= 0xfe;
			MEMCTR &= ~0X04;	
			enable_irq();
			return DATA_SEND_TIMEOUT;
		}

		ENABLE_CLKSYN;	
		
		ClkSynSend(&dat,1);	

		DISABLE_CLKSYN;
}

char LIT_PARA_Send (unsigned char *SPI_byte,char size)
{
	char i=0;

	ENABLE_CLKSYN;	
	
	while(i<size)
	{
		SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);

		while(!(TM7ICR&0X01))
		{
			if(!NIKONNSSI)
			{
				SET_TIMER_STOP;
				break;
			}
		}
		
		if(GETBIT(TM7ICR,0))
		{
			SET_TIMER_STOP;
			//CLRBIT(TM7ICR,0);
			disable_irq(); 
			MEMCTR |= 0X04; 
			TM7ICR &= 0xfe;
			MEMCTR &= ~0X04;	
			enable_irq();
			DISABLE_CLKSYN;
			return DATA_SEND_TIMEOUT;
		}

		//ENABLE_CLKSYN;	
		
		ClkSynSend(&SPI_byte[i++],1);
		if(i==size)
		{
			DISABLE_CLKSYN;
			NIKONMOSI = 1;
		}

//		while(!NIKONNSSI);

		SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);
		
		while(!(TM7ICR&0X01))
		{
			if(NIKONNSSI)
			{
				SET_TIMER_STOP;
				break;
			}
		}
		
		if(GETBIT(TM7ICR,0))
		{
			SET_TIMER_STOP;
			//CLRBIT(TM7ICR,0);
			disable_irq(); 
			MEMCTR |= 0X04; 
			TM7ICR &= 0xfe;
			MEMCTR &= ~0X04;	
			enable_irq();
			DISABLE_CLKSYN
			return DATA_SEND_TIMEOUT;
		}


		//DISABLE_CLKSYN;
	}
	
	DISABLE_CLKSYN;
	
	return DATA_SEND_SUCESS;
	


} // END SPI_Transfer

char DATA_Receive(char flag, unsigned char *pRcv)
{
	unsigned char rcvData=0;

	NIKONMOSI = 1;

	SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);

	while(!(TM7ICR&0X01))
	{
		if(!NIKONNSSI)	 
		{
			SET_TIMER_STOP
			break;
		}
	}
	
	if(GETBIT(TM7ICR,0))
	{
		SET_TIMER_STOP;
		//CLRBIT(TM7ICR,0);
		disable_irq(); 
		MEMCTR |= 0X04; 
		TM7ICR &= 0xfe;
		MEMCTR &= ~0X04;	
		enable_irq();
		return DATA_SEND_TIMEOUT;
	}
//set SB0(P06) as input
	SET_SBO_IN;
//enable synclk receive	
	ENABLE_CLKSYN;
//take receive function	
	ClkSynRcv(&rcvData,1);
	*pRcv = rcvData ;
	
	DISABLE_CLKSYN;
	
	SET_SBO_OUT;
	
	return DATA_SEND_SUCESS;
}



