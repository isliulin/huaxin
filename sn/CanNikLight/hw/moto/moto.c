#include "moto.h"
#include"global.h"
#include <stdarg.h>
#include "../uart/uart.h"

#ifdef MOTO_TEST
u16 g_motoTurn=0;
#endif

u8 g_moto_error = 0;

//MotoSet motoSet;
//u8 code focLenScape[] = {0,24,28,35,50,70,80,105};
/*
u8 code focLenVal[53] = {0,
18,	20,	21,	22,	23,	24,	25,	26,	27,	28,
29,	30,	32,	33,	34,	35,	36,	38,	40,	41,
42,	44,	45,	46,	48,	49,	50,	51,	52,	55,	58,
60,	62,	65,	66,	70,	75,	80,	85,	90,	92,
95, 98,105, 112,120,130,135,150,170,180, 
0,
};
*/

//Byte code focLenVal[20] = {20,24,28,30,32,34,35,38,42,46,50,58,64,70,80,90,105,135,180,200};
//Byte code focLenVal[22] = {20,22,24,28,30,32,34,35,38,42,46,50,58,64,70,80,90,105,135,180,190,200};
//const u8 focLenVal[11] = {20,24,28,35,50,70,80,105,135,200};
//u16 code focLenCoordinate[11] =  {0,108,162,378,594,756,810,864,972,1080};
//u16 code focLenCoordinate[11] =  {0,108,162,378,594,776,850,944,1000,1080};
//u16 code focLenCoordinate[11] =  {0,108,162,378,594,776,850,980,1000,1080};
//u16 code focLenCoordinate[11] =  {20,108,162,378,594,776,850,980,1000,1060};
//u16 code focLenCoordinate[11] =  {20,108,162,378,594,776,850,980,1000,1100};
//u16 code focLenCoordinate[11] =  {20,108,182,398,614,856,910,1040,1080,1140};
//u16 code focLenCoordinate[11] =  {20,108,182,398,614,856,910,1040,1080,1100};
//const u16 focLenCoordinate[11] =  {20,108,182,398,614,856,910,1000,1020,1040};
const u16 focLenCoordinate[11] =  {40,108,182,398,614,856,910,1000,1020,1040};          //20171121 马达20mm位置卡
const u8 focLenCode[10] = {0x35,0x05,0x09,0x0d,0x11,0x15,0x19,0x1d,0x39,0x45};

static u8 s_chargeCon = 0;
static u8 s_chargeLock=0;
u8 g_chargeShadow=0;
u16 g_motoTimer = 0;
u8 g_isReflectPulled=0;


void MotoDrvPinSet(u8 dir, u8 id);
void DiffuserCheck(void);

#ifdef MOTO_TEST_DSP	   
void DspMotoVad(u16 y,u16 val);
#endif


//CANNON MOTO VALUE
//18	20	21	24	27	30	32	33	34	37	40	41	43	47	49	51	53	55

//const u8 MotoDrv_Forward[] = {0x00,0x04,0x14,0x10};
//const u8 MotoDrv_Backward[] = {0x10,0x14,0x04,0x00};
const u8 MotoDrv_Backward[] = {0x00,0x04,0x14,0x10};
const u8 MotoDrv_Forward[] = {0x10,0x14,0x04,0x00};

const u8 focLenScape[10] = {20,24,28,35,50,70,80,105,135,200};

#define MOTO_RESET_COORDINATE	   1040//910
#define MOTO_SW_TRANSITION         105
#define MOTO_RESET_STEP_MAX       1060

#define ZOOM_AD_MIN 		8
#define ZOOM_AD_MAX		160
#define ZOOM_AD_VAL		17
#define ZOOM_AD_OTS		148

void LockCharge(void)			//disable charge
{
	if(!s_chargeLock)
	{
		s_chargeCon = CHARGE_CON;
		DISABLE_CHARGE();
		s_chargeLock = 1;
	}
}

void ReleaseCharge(void)
{
	if(s_chargeLock>0)
	{
		s_chargeLock = 0;
		CHARGE_ENABLE_PIN = s_chargeCon;
	}
	
}

void SetChargeEnable(void)
{
	g_chargeShadow = 1;
	if(s_chargeLock>0)
	{
		s_chargeCon = 0;				//enalbe charge
	}else
	{
		ENABLE_CHARGE();
		s_chargeCon = 0;
	}
}

void SetChargeDisable(void)
{
	g_chargeShadow = 0;
	if(s_chargeLock>0)
	{
		s_chargeCon = 1;
	}else
	{
		DISABLE_CHARGE();
		s_chargeCon = 1;
	}
}

void MotoReset(void)
{
	motoSet.moto_ready = 0;
	motoSet.resetFlag = 1;
	motoSet.motoRstCount = 0;
/*	
	if(motoSet.moto_ready!=MOTO_READY)
	{
		//while(GETBIT(gBUartCon,2));
		//while(uart_tran_flag);
		//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
		//SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
		//DISABLE_CHARGE();
		LockCharge();
		
		if(!MOTOLOCA)
		{
			//motoSet.motoDir = MOTO_DIR_FORWARD;
			motoSet.motoDir = MOTO_DIR_BACKWARD;
			motoSet.moving = 1;
			motoSet.moto_en = 1;
		}else
		{
			//motoSet.motoDir = MOTO_DIR_BACKWARD;
			motoSet.motoDir = MOTO_DIR_FORWARD;
			motoSet.moving = 1;
			motoSet.moto_en = 1;
		}
	}
*/
	if (!MOTOLOCA)                           // 200mm
	{
		motoSet.moto_ready = MOTO_READY;
		motoSet.current_location = 200;
		motoSet.moto_ready = 1;
	}else
	{
		LockCharge();
		motoSet.motoDir = MOTO_DIR_FORWARD;
	}
	
}

s8 GetMotoIndex(char para)
{
	char i;
	if(14 == para)
	{
		return 0;
	}
	
	for(i=0;i<sizeof(focLenScape);i++)
	//for(i=0;i<sizeof(focLenScape);i++)
	{
		if(para==focLenScape[i])
		//if(para==focLenScape[i])
		{
			return i;
		}
	}
	//return 0xff;
	return -1;
}

//u8 g_ref_flag = 0;
void MotoLocation(void)
{
	//u8 motoDestIndex=0,motoOrgIndex=0;
	s8 motoDestIndex=0;
	s8 motoOrgIndex=0;
	s16 motoOrgSet=0;
	s16 motoDestSet=0;
//	signed int stepTmp;
	char i;
	//static u8 ref_flag=0;
	
	
	if(motoSet.moving == 1)return;

	//if(REF_BOARD_DETECT!=g_ref_flag)

	if(motoSet.current_location==*motoSet.dest_location)
	//if(motoSet.currentSet==motoSet.destSet)
	{
		//if(GETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT))
		if(motoSet.uartSendFlag>0)			//马达已到指定位置
		{
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
			//CLRBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
			//SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT);
#ifdef MOTO_RESET_AD
			ENABLE_CHARGE();
#else
			ReleaseCharge();
#endif
			motoSet.uartSendFlag = 0;
		}else
		{
			if(*motoSet.dest_location!=motoSet.destRsv)
			{
				
				for(i=0;i<sizeof(focLenScape);i++)
				{
					//if((focLenScape[i]<motoSet.destRsv)&&(motoSet.destRsv<=focLenScape[i+1]))
					if((focLenScape[i]<=motoSet.destRsv)&&(motoSet.destRsv<focLenScape[i+1]))
					{
						break;
					}
				}
				if(i<=sizeof(focLenScape))
				{
					//DisplayFocLen(*motoSet.mode,motoSet.destRsv);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					*motoSet.dest_location = motoSet.destRsv;
					motoSet.focIndex = i;
				}
				//motoSet.destSet = motoSet.destRsv;
				*motoSet.dest_location = motoSet.destRsv;
			}
		}
		return;
	}

	else//if(motoSet.current_location!=*motoSet.dest_location)
	{
		//if(GETBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT))
#ifdef MOTO_DEBUG_PRT
		printf("dest: %d, cur: %d\r\n",*motoSet.dest_location, motoSet.current_location);
#endif

		if(!motoSet.uartSendFlag)
		{
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
			//CLRBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT);
			//SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
#ifdef MOTO_RESET_AD
			DISABLE_CHARGE();
#else
			LockCharge();
#endif
			motoSet.uartSendFlag = 1;
		}
		if(!motoSet.current_location)
		{
			motoOrgSet= motoSet.currentSet;
		}else
		{
			motoOrgIndex = GetMotoIndex(motoSet.current_location);
#ifdef MOTO_DEBUG_PRT
			printf("org:%d\r\n",motoOrgIndex);
#endif
			//if((motoOrgIndex==0xff))
			if(motoOrgIndex<0)
			{			
				MotoReset();
				//while(GETBIT(gBUartCon,2));
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
				//DISABLE_CHARGE();
#ifdef MOTO_RESET_AD
				ENABLE_CHARGE();
#else
				ReleaseCharge();
#endif
				return;
			}
			motoOrgSet = focLenCoordinate[motoOrgIndex];
			
		}
		motoDestIndex = GetMotoIndex(*motoSet.dest_location);
#ifdef MOTO_DEBUG_PRT
		printf("dest:%d\r\n",motoDestIndex);
#endif
		//if(motoDestIndex==0xff)
		if(motoDestIndex<0)
		{
			*motoSet.dest_location = motoSet.current_location;
			//while(GETBIT(gBUartCon,2));
			//while(uart_tran_flag);
			//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
			//CLRBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
			//SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_DISABLE_BIT);
#ifdef MOTO_RESET_AD
			ENABLE_CHARGE();
#else
			ReleaseCharge();
#endif
			return;
		}

		motoDestSet = focLenCoordinate[motoDestIndex];

		/*else if(focLenVal[motoDestIndex]==105)
		{
			MotoReset();
			*motoSet.dest_location = 105;
			//while(uart_tran_flag);
			//while(GETBIT(gBUartCon,2));
			//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
			DISABLE_CHARGE();
			
		}*///else
		{
			//stepTmp = moto_step[motoDestIndex]-moto_step[motoOrgIndex];
			/*
			if(motoDestIndex>motoOrgIndex)
			{
				motoSet.motoDir = MOTO_DIR_FORWARD;
				//motoSet.motoDir = MOTO_DIR_BACKWARD;
				motoSet.moto_step = ((motoDestIndex-motoOrgIndex)*20*2.7f);
			}else
			{
				motoSet.motoDir = MOTO_DIR_BACKWARD;
				//motoSet.motoDir = MOTO_DIR_FORWARD;
				motoSet.moto_step = ((motoOrgIndex-motoDestIndex)*20*2.7f);
			}
			*/
			if(motoDestSet>motoOrgSet)
			{
				motoSet.motoDir = MOTO_DIR_FORWARD;
				motoSet.moto_step = motoDestSet - motoOrgSet;
			}else
			{
				motoSet.motoDir = MOTO_DIR_BACKWARD;
				motoSet.moto_step = motoOrgSet - motoDestSet;
			}
			
#ifdef MOTO_DEBUG_PRT
			printf("dir:%d,steps:%d\r\n",motoSet.motoDir,motoSet.moto_step);
#endif

			motoSet.moving = 1;
			motoSet.moto_en = 1;
			//motoSet.current_location = *motoSet.dest_location;

/*			
			if(!stepTmp)
			{
				//while(GETBIT(gBUartCon,2));
				//while(uart_tran_flag);
				//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
				ENABLE_CHARGE();
				//motoSet.current_location = *motoSet.dest_location;			//without this may lead unending uartdata sending
				return;
			}else if(stepTmp>0)
			{
				motoSet.motoDir = MOTO_DIR_FORWARD;
				motoSet.moto_step = (stepTmp*5*2.7f);
			}else if(stepTmp<0)
			{
				motoSet.motoDir = MOTO_DIR_BACKWARD;
				motoSet.moto_step = (-stepTmp)*5*2.7f;
			}
			motoSet.moving = 1;
			motoSet.moto_en = 1;
			motoSet.current_location = *motoSet.dest_location;
*/
			
		}
			
		
	}

	
}

void MotoDrvPinSet(u8 dir, u8 id)
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

void MotoProc(void)
{
	static char i = 0;

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
			if( !MOTOLOCA )
			{
				motoSet.moto_ready = MOTO_READY;
				motoSet.current_location = 200;
				motoSet.currentSet = MOTO_RESET_COORDINATE;						
				motoSet.resetFlag = 0;
				motoSet.motoRstCount = 0;
				motoSet.moving = 0;
				motoSet.moto_en = 0;
				ReleaseCharge();
				return;
				//SetFocLen(0,105);
			}

			//MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
			MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
			//i++;				
			if(i>3) 
			{
				i=0;
			}
			//in order to avoid stucking
			if(motoSet.motoRstCount++>MOTO_RESET_STEP_MAX)
			{
				g_moto_error = 1;
				ReleaseCharge();
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return;
			}

#if 0			
			switch( motoSet.motoDir)
			{
				case MOTO_DIR_FORWARD:
					//if(MOTOLOCA==1)
					if(!MOTOLOCA)
					{
						motoSet.moto_ready = MOTO_READY;
						//motoSet.current_location = 105;
						motoSet.current_location = 0;
						motoSet.currentSet = MOTO_RESET_COORDINATE;						
						motoSet.resetFlag = 0;
						motoSet.motoRstCount = 0;
						motoSet.moving = 0;
						motoSet.moto_en = 0;
						ReleaseCharge();
						//SetFocLen(0,105);
						//SendUartData(UART_DECODE_FOCLEN,0,0);
						//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
						//enable charge if need
						return;
					}
					//P2 = (P2&0xeb)| MotoDrv_Forward[i++];
					//MOTOIN1 = (MotoDrv_Forward[i]>>4)&0X01;
					//MOTOIN2 = (MotoDrv_Forward[i]>>2)&0X01;
					MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
					//i++;				
					if(i>3) 
					{
						i=0;
					}
					//in order to avoid stucking
					if(motoSet.motoRstCount++>MOTO_RESET_STEP_MAX)
					{
						g_moto_error = 1;
						ReleaseCharge();
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						return;
					}
				break;

				case MOTO_DIR_BACKWARD:
					//if(MOTOLOCA==0)
					if(MOTOLOCA>0)
					{
						motoSet.moto_ready = MOTO_READY;
						//motoSet.current_location = 105;
						motoSet.current_location = 0;
						motoSet.currentSet = MOTO_RESET_COORDINATE;	
						motoSet.resetFlag = 0;
						motoSet.motoRstCount = 0;
						motoSet.moving = 0;
						motoSet.moto_en=0;
						ReleaseCharge();
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
					if(i>3)
					{
						i=0;
					}
					//in order to avoid stucking
					if(motoSet.motoRstCount++>MOTO_RESET_STEP_MAX)
					{
						g_moto_error = 1;
						ReleaseCharge();
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						return;
					}
				break;
					
			}
#endif
			
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
				ReleaseCharge();
/****************************************************
				the following code is aim at avoid moto 
****************************************************/
				//if(motoSet.current_location<105)
#if 0
				if(motoSet.current_location<MOTO_SW_TRANSITION)
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
#endif
			}
		}
	}
}

void MotoEventLoop(void)
{
	u16 crtTime; 
	//if((motoSet.moto_en!=MOTO_ENABLE)||(g_moto_error!=0))
	//if((MOTO_ENABLE!=motoSet.moto_en)||g_moto_error)
	//{
	//	return;
	//}
        
        crtTime = get_counter();              // get time
        if ( crtTime - g_motoTimer > 1000L)              // key timer overflow
	{
		g_motoTimer = crtTime;
		DiffuserCheck();
		MotoLocation();
		MotoProc();
    	}
}

static u8 s_ref_flag = 0;
static u8 s_mModRsv=0;
static u16 s_mDestRsv=0;
static u16 s_diffuseCnt=0;

void DiffuserCheck(void)
{
/*
	if((MOTO_ENABLE!=motoSet.moto_en)||g_moto_error||(!motoSet.moto_ready))
	{
		return;
	}
*/
#ifdef DETAIL_DBG
/*
	static u16 cnt = 0;
	if (cnt ++ > 1000)
	{
		cnt = 0;
		printf("dfs: %d\r\n", REF_BOARD_DETECT);
	}
*/
#endif
	
	if(REF_BOARD_DETECT!=s_ref_flag)                 // check the input of diffuser is changed
	{
		s_diffuseCnt ++;
		// start counter, execute if counter over 500
		if(s_diffuseCnt > 500)
		{
			// clear the counter
			s_diffuseCnt = 0;
			// double check
			if(REF_BOARD_DETECT!=s_ref_flag)
			{
				// the record is low, means the diffuser is pulled
				if (!s_ref_flag)
				{
					//if(20 != *motoSet.dest_location)                                   // 2017/09/20  set moto to 14 when 20
					{
						s_mDestRsv = *motoSet.dest_location;
						//*motoSet.dest_location = 20;
						*motoSet.dest_location = 14;
						motoSet.destRsv = *motoSet.dest_location;
						s_mModRsv = *motoSet.mode;
						*motoSet.mode = MOTO_MODE_MANU;
						//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
						//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//FLAOK_TOGGLE();
						//while(uart_tran_flag);
						//DecodeUartData(UART_ENCODE_REFLECT,1,0);
						//g_ref_flag = 1;
					}
					s_ref_flag = 1;
					g_isReflectPulled = 1;
				}else
				{
					s_ref_flag = 0;
					*motoSet.mode = s_mModRsv;
					//*motoSet.dest_location = focLenScape[motoSet.focIndex];
					*motoSet.dest_location = s_mDestRsv;
					motoSet.destRsv = *motoSet.dest_location;
					//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 	
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					g_isReflectPulled = 0;
				}
			}else                  // may be last too short
			{
				return;
			}
		}

	}else if(s_diffuseCnt > 0)
	{
		s_diffuseCnt =0;
	}
}

u8 GetMotoStdIndex(u8 dest)
{
	u8 i=0;
	if(dest>=200)
	{
		return MOTO_MAX_INDEX;
	}
	for(i=0;i<sizeof(focLenScape);i++)
	{
		//if(dest==focLenScape[i])
		//{
			//return i-1;
		//}
		//if(dest>=focLenScape[i])
		if((dest>=focLenScape[i])&&(dest<focLenScape[i+1]))
		{
			return i;
		}
	}

	return 0;
}

#ifdef MOTO_TEST

//void DspMotoVad(u16 y,u16 val)
//{
	//DspRfIdChar(7,LAYOUT_IDNUM_CML,val/100,DSP_NORMAL);
	//DspRfIdChar(7,LAYOUT_IDNUM_CML+11,val%100/10,DSP_NORMAL);
	//DspRfIdChar(7,LAYOUT_IDNUM_CML+11+11,val%10,DSP_NORMAL);
	//DspRfIdChar(7,y,val/100,DSP_NORMAL);
	//DspRfIdChar(7,y+11,val%100/10,DSP_NORMAL);
	//DspRfIdChar(7,y+11+11,val%10,DSP_NORMAL);
//}


void MotoTest(u8 dir)
{
	u8 i=0;
	u16 steps=0;
	//u16 zoom=0;
/*
	if(20==*motoSet.dest_location)
	{
		*motoSet.dest_location = 200;
		motoSet.destRsv = 200;
	}else
	{
		*motoSet.dest_location = 20;
		motoSet.destRsv = 20;
	}
*/


	MOTOENA = 1;	

	while(steps++<4)
	{		
		MotoDrvPinSet(dir-1,i++);
		if(i>3) i=0;
		//HaltSec(1);
		delay_ms(1);
	}
	if(KEY_DIR_LEFT==dir)
	{
		g_motoTurn++;
		
	}else
	{
		if(g_motoTurn>0)
		{
			g_motoTurn--;
		}
	}
	//DisplayFocLen(0,g_motoTurn,DSP_SELE); 
	printf("turn: %d\r\n",g_motoTurn);

	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_ZOOM;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));

	//zoom = (ADC1->DR>>4);

	//DspMotoVad(LAYOUT_IDNUM_CML,(ADC1->DR>>4));
	printf("ad %d\r\n",(ADC1->DR>>4));

	MOTOENA = 0;	
	
}
#endif




 
