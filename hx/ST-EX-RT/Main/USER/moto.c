//#include	".\head\intrins.h"		
//#include	".\head\CPU32.h"
//#include	".\head\define.h"


#include "Golbal.h"
//#include "uart.h"
#include "display.h"
#include "KeyProc.h"

#define MOTO_CHARGE_ENABLE_BIT		0x01//0
#define MOTO_CHARGE_DISABLE_BIT		0x02//1/

/*
#if(NEW_DSP_SCHEME==0)
#define MOTOLOCA	P0IN0
#define MOTOENA		P0OUT1
#define MOTOIN1		P0OUT2
#define MOTOIN2		P0OUT3
#elif(NEW_DSP_SCHEME==1)
#define MOTOLOCA	P4IN3
#define MOTOENA		P0OUT1
#define MOTOIN1		P0OUT2
#define MOTOIN2		P0OUT0
#endif
*/

unsigned char g_moto_error = 0;

//MotoSet motoSet;
//unsigned char code focLenScape[] = {0,24,28,35,50,70,80,105};
unsigned char code focLenVal[53] = {0,
18,	20,	21,	22,	23,	24,	25,	26,	27,	28,
29,	30,	32,	33,	34,	35,	36,	38,	40,	41,
42,	44,	45,	46,	48,	49,	50,	51,	52,	55,	58,
60,	62,	65,	66,	70,	75,	80,	85,	90,	92,
95, 98,105, 112,120,130,135,150,170,180, 
0,
};


//CANNON MOTO VALUE
//18	20	21	24	27	30	32	33	34	37	40	41	43	47	49	51	53	55

char code MotoDrv_Forward[] = {0x00,0x04,0x14,0x10};
char code MotoDrv_Backward[] = {0x10,0x14,0x04,0x00};

#ifdef _CANON_
Byte code focLenScape[11] = {0,24,28,35,50,70,80,105,120,135,180};
#else
Byte code focLenScape[11] = {0,24,28,35,50,70,85,105,120,135,180};
#endif

#ifdef _MOTO_										//this if especially for 330ex, because of flash space limitation

int code moto_step[] = {160,
160,160,160,160,160,160,158,155,153,150,
145,140,135,130,125,120,115,115,110,105,
105,100,95,  90, 90, 85, 80, 77,75, 70, 65,
60, 55, 50,  45, 40, 35, 30, 25, 20, 15,
12, 10, 0,  -5, -10, -12,-15,-20,-21,-22,
80,
};

void MotoReset(void)
{
	motoSet.moto_ready = 0;
	motoSet.resetFlag = 1;
	motoSet.motoRstCount = 0;
	
	if(motoSet.moto_ready!=1)
	{
		//while(GETBIT(gBUartCon,2));
		//while(uart_tran_flag);
		DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
		SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
		
		if(!MOTOLOCA)
		{
			motoSet.motoDir = MOTO_DIR_FORWARD;
			//motoSet.motoDir = MOTO_DIR_BACKWARD;
			motoSet.moving = 1;
			motoSet.moto_en = 1;
		}else
		{
			motoSet.motoDir = MOTO_DIR_BACKWARD;
			//motoSet.motoDir = MOTO_DIR_FORWARD;
			motoSet.moving = 1;
			motoSet.moto_en = 1;
		}
	}
}

char GetMotoIndex(char para)
{
	char i;
	for(i=0;i<sizeof(focLenVal);i++)
	{
		if(para==focLenVal[i])
		return i;
	}
	return 0xff;
}

void MotoLocation()
{
	unsigned char motoDestIndex=0,motoOrgIndex=0;
	int stepTmp;
	char i;
	static char ref_flag=0;
	
	
	if(motoSet.moving == 1)return;

#ifdef MOTO_REFLECT_DETECT
	if(REF_BOARD_DETECT)
	{
		if(18!=*motoSet.dest_location)
		{
			*motoSet.dest_location = 18;
			motoSet.destRsv = *motoSet.dest_location;
			*motoSet.mode = MOTO_MODE_MANU;
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_REFLECT,1,0);
			ref_flag = 1;
		}
	}else
	{
		if(ref_flag)
		{
			ref_flag = 0;
			*motoSet.dest_location = focLenScape[motoSet.focIndex];
			motoSet.destRsv = *motoSet.dest_location;
			DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 	
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_REFLECT,0,0);
		}
	}
#endif

	if(motoSet.current_location==*motoSet.dest_location)
	{
		if(GETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT))
		{
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
			CLRBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
			SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT);
		}else
		{
			if(*motoSet.dest_location!=motoSet.destRsv)
			{
				for(i=0;i<sizeof(focLenScape);i++)
				{
					if((focLenScape[i]<motoSet.destRsv)&&(motoSet.destRsv<=focLenScape[i+1]))
					{
						break;
					}
				}
				if(i<=sizeof(focLenScape))
				{
					DisplayFocLen(*motoSet.mode,motoSet.destRsv,DSP_NORMAL);
					*motoSet.dest_location = motoSet.destRsv;
					motoSet.focIndex = i+1;
				}	
			}
		}
		return;
	}

	if(motoSet.current_location!=*motoSet.dest_location)
	{
		if(GETBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT))
		{
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
			CLRBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT);
			SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
		}
		motoOrgIndex = GetMotoIndex(motoSet.current_location);
		if((motoOrgIndex==0xff))
		{			
			MotoReset();
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
			return;
		}
		motoDestIndex = GetMotoIndex(*motoSet.dest_location);
		if(motoDestIndex==0xff)
		{
			*motoSet.dest_location = motoSet.current_location;
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
			CLRBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
			SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT);
			return;
		}else if(focLenVal[motoDestIndex]==105)
		{
			MotoReset();
			*motoSet.dest_location = 105;
			//while(uart_tran_flag);
			//while(GETBIT(gBUartCon,2));
			DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
			
		}else
		{
			stepTmp = moto_step[motoDestIndex]-moto_step[motoOrgIndex];
			if(!stepTmp)
			{
				//while(GETBIT(gBUartCon,2));
				//while(uart_tran_flag);
				DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
				motoSet.current_location = *motoSet.dest_location;			//without this may lead unending uartdata sending
				return;
			}else if(stepTmp>0)
			{
				motoSet.motoDir = MOTO_DIR_FORWARD;
				motoSet.moto_step = stepTmp*5;
			}else if(stepTmp<0)
			{
				motoSet.motoDir = MOTO_DIR_BACKWARD;
				motoSet.moto_step = (-stepTmp)*5;
			}
			motoSet.moving = 1;
			motoSet.moto_en = 1;
			
		}
			
		
	}

	
}

void MotoDrvPinSet(unsigned char dir, unsigned char id)
{
	if(MOTO_DIR_FORWARD==dir)
	{
		if((MotoDrv_Forward[id]>>4)&0x01)
		{
			MOTOIN1 = 1;
		}else
		{
			MOTOIN1 = 0;
		}

		if((MotoDrv_Forward[id]>>2)&0x01)
		{
			MOTOIN2 = 1;
		}else
		{
			MOTOIN2 = 0;
		}
	}else
	{
		 //MOTOIN1 = (MotoDrv_Backward[i]>>4)&0X01;
		//MOTOIN2 = (MotoDrv_Backward[i]>>2)&0X01;
		if((MotoDrv_Backward[id]>>4)&0x01)
		{
			MOTOIN1 = 1;
		}else
		{
			MOTOIN1 = 0;
		}

		if((MotoDrv_Backward[id]>>2)&0x01)
		{
			MOTOIN2 = 1;
		}else
		{
			MOTOIN2 = 0;
		}
		
	}
}

void MotoProc()
{
	static char i;

	if((motoSet.moto_en!=MOTO_ENABLE)||(g_moto_error!=0))
	{
		MOTOENA = 0;
		return;
	}else
	{
		if(motoSet.moto_ready!=MOTO_READY)
		{
			MOTOENA = 1;
			//motoSet.moving = 1;
			
			switch( motoSet.motoDir)
			{
				case MOTO_DIR_FORWARD:
					if(MOTOLOCA==1)
					{
						motoSet.moto_ready = MOTO_READY;
						motoSet.current_location = 105;
						motoSet.resetFlag = 0;
						motoSet.motoRstCount = 0;
						motoSet.moving = 0;
						motoSet.moto_en = 0;
						//SetFocLen(0,105);
						//SendUartData(UART_DECODE_FOCLEN,0,0);
						DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
						return;
					}
					//P2 = (P2&0xeb)| MotoDrv_Forward[i++];
					//MOTOIN1 = (MotoDrv_Forward[i]>>4)&0X01;
					//MOTOIN2 = (MotoDrv_Forward[i]>>2)&0X01;
					MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
					//i++;				
					if(i>3) i=0;
				break;

				case MOTO_DIR_BACKWARD:
					if(MOTOLOCA==0)
					{
						motoSet.moto_ready = MOTO_READY;
						motoSet.current_location = 105;
						motoSet.resetFlag = 0;
						motoSet.motoRstCount = 0;
						motoSet.moving = 0;
						motoSet.moto_en=0;
						//SetFocLen(0,105);
						//SendUartData(UART_DECODE_FOCLEN,0,0);
						//SendUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
						return;
					}
					//P2 = (P2&0xeb)| MotoDrv_Backward[i++];
					//MOTOIN1 = (MotoDrv_Backward[i]>>4)&0X01;
					//MOTOIN2 = (MotoDrv_Backward[i]>>2)&0X01;
					MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
					//i++;				
					if(i>3) i=0;
				break;
					
			}
			
		}else
		{		
			if(motoSet.moto_step--)
			{
				MOTOENA = 1;	
				
				switch( motoSet.motoDir)
				{
					case MOTO_DIR_FORWARD:
						//  P3 = (P3&0x5f)| MotoDrv_Forward[i++];
						//P2 = (P2&0xeb)| MotoDrv_Forward[i++];
						//MOTOIN1 = (MotoDrv_Forward[i]>>4)&0X01;
						//MOTOIN2 = (MotoDrv_Forward[i]>>2)&0X01;
						MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
						//i++;						
						if(i>3) i=0;
					break;

					case MOTO_DIR_BACKWARD:
						//P2 = (P2&0xeb)| MotoDrv_Backward[i++];
						 //MOTOIN1 = (MotoDrv_Backward[i]>>4)&0X01;
						 //MOTOIN2 = (MotoDrv_Backward[i]>>2)&0X01;
						 //i++;	
						 MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
						if(i>3) i=0;
					break;
						
				}
			}else
			{
				motoSet.current_location = *motoSet.dest_location;
				motoSet.moto_en = 0;
				motoSet.moving = 0;
				MOTOENA = 0;
/****************************************************
				the following code is aim at avoid moto 
****************************************************/
				if(motoSet.current_location<105)
				{
					if(!MOTOLOCA)
					{
						MotoReset();
					}
				}else
				{
					if(MOTOLOCA)
					{
						MotoReset();
					}
				}
			}
		}
	}
}
#else
void MotoReset(void)
{
}
#endif



 
