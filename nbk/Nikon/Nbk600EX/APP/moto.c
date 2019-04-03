//#include	".\head\intrins.h"		
//#include	".\head\CPU32.h"
//#include	".\head\define.h"

//TP4: 2.83
//TP3: 2.514
//TP5: 2.2
//TP7: 1.886
//TP8:1.571
//TP10:1.257
//TP11:0.943
//TP12:0.629
//TP13:0.314
//TP14:0


#include "Golbal.h"
//#include "uart.h"
#include "display.h"
#include "KeyProc.h"
#include "canon.h"

#define MOTO_CHARGE_ENABLE_BIT		0x01//0
#define MOTO_CHARGE_DISABLE_BIT		0x02///1/
#define MOTO_STEPS_TIMEOUT			100//0xFF
#define _MOTO_CHK_4STEP_
#define MOTO_MAX_INDEX			9
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
#ifdef MOTO_TEST
u16 g_motoTurn=0;
#endif

unsigned char g_moto_error = 0;

//MotoSet motoSet;
//unsigned char code focLenScape[] = {0,24,28,35,50,70,80,105};
/*
unsigned char code focLenVal[53] = {0,
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
Byte code focLenVal[11] = {20,24,28,35,50,70,80,105,135,200};
//u16 code focLenCoordinate[11] =  {0,108,162,378,594,756,810,864,972,1080};
//u16 code focLenCoordinate[11] =  {0,108,162,378,594,776,850,944,1000,1080};
//u16 code focLenCoordinate[11] =  {0,108,162,378,594,776,850,980,1000,1080};
//u16 code focLenCoordinate[11] =  {20,108,162,378,594,776,850,980,1000,1060};
//u16 code focLenCoordinate[11] =  {20,108,162,378,594,776,850,980,1000,1100};
//u16 code focLenCoordinate[11] =  {20,108,182,398,614,856,910,1040,1080,1140};
//u16 code focLenCoordinate[11] =  {20,108,182,398,614,856,910,1040,1080,1100};
u16 code focLenCoordinate[11] =  {20,108,182,398,614,856,910,1000,1020,1040};

static u8 s_chargeCon = 0;
static u8 s_chargeLock=0;
u8 g_chargeShadow=0;


void MotoDrvPinSet(unsigned char dir, unsigned char id);

#ifdef MOTO_TEST_DSP	   
void DspMotoVad(u16 y,u16 val);
#endif


//CANNON MOTO VALUE
//18	20	21	24	27	30	32	33	34	37	40	41	43	47	49	51	53	55

char code MotoDrv_Forward[] = {0x00,0x04,0x14,0x10};
char code MotoDrv_Backward[] = {0x10,0x14,0x04,0x00};

#ifdef _CANON_
//Byte code focLenScape[12] = {35,20,24,28,35,50,70,80,105,135,180,200};
//Byte code focLenScape[12] = {35,20,24,28,35,50,70,80,105,135,180,200};
//Byte code focLenScape[11] = {35,20,24,28,35,50,70,80,105,135,200};
Byte code focLenScape[10] = {20,24,28,35,50,70,80,105,135,200};
Byte code focLenCode[10] = {0x35,0x05,0x09,0x0d,0x11,0x15,0x19,0x1d,0x39,0x45};
//Byte code focLenScape[20] = {20,24,28,30,32,34,35,38,42,46,50,58,64,70,80,90,105,135,180,200};
#else
Byte code focLenScape[11] = {35,24,28,35,50,70,85,105,120,135,180};
#endif

#ifdef MOTO_RESET_AD
#else
#define MOTO_RESET_COORDINATE	   910
#define MOTO_SW_TRANSITION         105
#define MOTO_RESET_STEP_MAX       1060
#endif

#ifdef _MOTO_										//this if especially for 330ex, because of flash space limitation

/*
int code moto_step[] = {160,
160,160,160,160,160,160,158,155,153,150,
145,140,135,130,125,120,115,115,110,105,
105,100,95,  90, 90, 85, 80, 77,75, 70, 65,
60, 55, 50,  45, 40, 35, 30, 25, 20, 15,
12, 10, 0,  -5, -10, -12,-15,-20,-21,-22,
80,
};
*/

//float code moto_step[] = {12,18,17,16,12,8,5,4,2,1,0.5,0};
//float code moto_step[] = {0,0,1,2,4,5,8,12,16,17,18};
//float code moto_step[] = {6,0,1,2,6,10,13,14,16,17,18,19};

#define ZOOM_AD_MIN 		8
#define ZOOM_AD_MAX		160
#define ZOOM_AD_VAL		17
#define ZOOM_AD_OTS		148


#if 0
u8 GetZoomAdc(void)
{
//	u8 zoom=0;
	
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_ZOOM;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));

	return (ADC1->DR>>4);
}
#else
u8 CheckZmAdc(void)
{
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=AD_CHANNEL_ZOOM;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));

	return (ADC1->DR>>4);
}

u8 GetZoomAdc(void)
{
	u8 adcBuf[5];
	u8 adcRef=0xFF;
	u8 i,j;

	for(j=0;j<5;j++)
	{	
		for(i=0;i<5;i++)
		{
			adcBuf[i] = CheckZmAdc();
			if(adcRef>adcBuf[i])
			{
				adcRef = adcBuf[i];
			}
			//delay_ms(1);
		}

		if((adcBuf[4]-adcRef<3)&&(adcBuf[3]-adcRef<3)&&(adcBuf[2]-adcRef<3)
			&&(adcBuf[1]-adcRef<3)&&(adcBuf[0]-adcRef<3))
		{
			return adcBuf[0];
		}
	}

	return ZOOM_AD_MAX+10;
}
#endif


u16 GetZoomByAdc(u8 zAd)
{
	if(zAd<ZOOM_AD_MIN)
	{
		return 19;
		//return 21;
		//return 20;
	}else
	{
		//return 19-(((zAd+3)/6 - 1)*2);
		return 19-(((zAd+(ZOOM_AD_VAL/2))/ZOOM_AD_VAL)*2);
		//return 21-(((zAd+(ZOOM_AD_VAL/2))/ZOOM_AD_VAL)*2);
		//return 20-(((zAd+(ZOOM_AD_VAL/2))/ZOOM_AD_VAL)*2);
	}
}

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
		CHARGE_ENABLE = s_chargeCon;
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

#ifdef MOTO_RESET_AD
void MotoReset(void)
{
	u8 zoomAdc=0;
	u8 zmZdcTmp=0;
	u16 errCnt=0;
	u8 i=0;
	u8 steps=0;
	
	motoSet.moto_ready = 0;
	motoSet.resetFlag = 1;
	motoSet.motoRstCount = 0;
	g_moto_error = 0;
	motoSet.moving = 0;


	zoomAdc = GetZoomAdc();
#ifdef MOTO_DEBUG_PRT
	printf("first %d\r\n",zoomAdc);
#endif

#ifdef MOTO_TEST_DSP
	DspMotoVad(3,zoomAdc);
#endif
	//if(zoomAdc<3)			//most inside
	if(zoomAdc<ZOOM_AD_MIN)
	{
		//motoSet.current_location = *motoSet.dest_location = 200;
		//motoSet.current_location = 200;
		MOTOENA = 1;
		//while(errCnt++<20)
		while(errCnt++<MOTO_STEPS_TIMEOUT)
		{
#ifdef _MOTO_CHK_4STEP_
			for(steps=0;steps<4;steps++)
#endif
			{
				MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
				if(i>3) i=0;
				delay_ms(1);
			}
			zoomAdc = GetZoomAdc();

			//if(zoomAdc>58)
			if(zoomAdc>ZOOM_AD_MAX)
			{
				break;
			}			
		}

		if(errCnt>=MOTO_STEPS_TIMEOUT)
		{
			g_moto_error = 1;
			errCnt = 0;
			
#ifdef MOTO_DEBUG_PRT
			printf("Moto Error\r\n");
#endif
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			return;
		}else
		{
			errCnt = 0;
		}

		/*
		for(steps=0;steps<4*7*5;steps++)
		{
			MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
			if(i>3) i=0;
			delay_ms(1);
		}	
		*/

		//motoSet.current_location = 200;
		//motoSet.current_location = 18;
		//motoSet.currentSet = 18;
		//motoSet.currentSet = 19;
		//motoSet.currentSet = 253;
		//motoSet.currentSet = 19*20*2.7f;
		motoSet.currentSet = 18*20*2.7f;
		motoSet.current_location = 0;
		//motoSet.current_location = 135;
		//motoSet.current_location = 180;
		
#ifdef MOTO_TEST
		//*motoSet.dest_location = 180;
		//motoSet.destRsv = *motoSet.dest_location;
		//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
#endif

		MOTOENA = 0;
		
#ifdef MOTO_DEBUG_PRT
		//printf("%d\r\n",motoSet.currentSet);
		printf("set:%d,cur:%d\r\n",motoSet.currentSet,motoSet.current_location);
#endif

#ifdef MOTO_TEST_DSP
		//zoomAdc = GetZoomAdc();
		DspMotoVad(LAYOUT_IDNUM_CML,motoSet.currentSet);
#endif
		
	}//else if((zoomAdc>50)&&(zoomAdc<58))			//most outside
	else if((zoomAdc>ZOOM_AD_OTS)&&(zoomAdc<ZOOM_AD_MAX))
	{
		//motoSet.current_location = *motoSet.dest_location = 20;
		//motoSet.current_location = 20;
		
		MOTOENA = 1;

		//while(errCnt++<20)
		//while(errCnt<20)
		while(errCnt++<MOTO_STEPS_TIMEOUT)
		{
#ifdef _MOTO_CHK_4STEP_
			for(steps=0;steps<4;steps++)
#endif
			{
				//MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
				MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
				if(i>3) i=0;
				delay_ms(1);
			}
			zoomAdc = GetZoomAdc();

			//if(zoomAdc>58)
			if(zoomAdc>ZOOM_AD_MAX)
			{
				break;
			}			
		}

		if(errCnt>=MOTO_STEPS_TIMEOUT)
		{
			g_moto_error = 1;
			errCnt = 0;
#ifdef MOTO_DEBUG_PRT
			printf("Moto Error\r\n");
#endif
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
			return;
		}else
		{
			errCnt = 0;
		}
/*
		while(errCnt<20)
		{
			for(steps=0;steps<4;steps++)
			{
				//MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
				MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
				if(i>3) i=0;
				delay_ms(1);
			}
			zoomAdc = GetZoomAdc();

			//if(zoomAdc>58)
			if(zoomAdc>50)&&(zoomAdc<58)
			{
				break;
			}			
		}

		if(errCnt>=20)
		{
			g_moto_error = 1;
			errCnt = 0;
		}
*/

		/*

		for(steps=0;steps<(4*7)*5;steps++)
		{
			MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
			if(i>3) i=0;
			delay_ms(1);
		}	

		*/

		//motoSet.current_location = 20;
		//motoSet.currentSet = 1;
		//motoSet.currentSet = 2;
		//motoSet.currentSet = 27;
		motoSet.currentSet = 2*20*2.7f;
		motoSet.current_location = 0;
		//motoSet.currentSet = 3;
		//motoSet.current_location = 24;
		//motoSet.current_location = 22;
		//motoSet.current_location = 24;
		//motoSet.current_location = 28;
		
#ifdef MOTO_TEST
		//*motoSet.dest_location = 24;
		//motoSet.destRsv = *motoSet.dest_location;
		//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
#endif

#ifdef MOTO_DEBUG_PRT
		printf("set:%d,cur:%d\r\n",motoSet.currentSet,motoSet.current_location);
#endif

		MOTOENA = 0;
		
#ifdef MOTO_TEST_DSP
		//zoomAdc = GetZoomAdc();
		DspMotoVad(LAYOUT_IDNUM_CML,motoSet.currentSet);
#endif
		
	}else
	{
		//MOTOENA = 1;

		//if(zoomAdc>58)			//VCC
		if(zoomAdc>ZOOM_AD_MAX)
		{
			//while(errCnt++<20)	
			MOTOENA = 1;
			
			while(errCnt++<MOTO_STEPS_TIMEOUT)
			{
#ifdef _MOTO_CHK_4STEP_
				for(steps=0;steps<4;steps++)
#endif
				{
					MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
					if(i>3) i=0;
					delay_ms(1);
				}
				zoomAdc = GetZoomAdc();
				//if(zoomAdc<58)
				if(zoomAdc<ZOOM_AD_MAX)
				{
											//get location here
					break;					//
				}
			}

			MOTOENA = 0;

	
			if(errCnt>=MOTO_STEPS_TIMEOUT)
			{
				g_moto_error = 1;
				//errCnt = 0;
#ifdef MOTO_DEBUG_PRT
			printf("Moto Error\r\n");
#endif
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return;
			}else
			{
				errCnt = 0;
			}

			zmZdcTmp = zoomAdc;

			delay_ms(1);
			MOTOENA = 1;

			//while(errCnt++<20)	
			while(errCnt++<MOTO_STEPS_TIMEOUT)
			{
#ifdef _MOTO_CHK_4STEP_
				for(steps=0;steps<4;steps++)
#endif
				{
					MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
					if(i>3) i=0;
					delay_ms(1);
				}
				zoomAdc = GetZoomAdc();

				if(zoomAdc<zmZdcTmp)
				{
					zmZdcTmp = zoomAdc;
				}
				//if(zoomAdc>58)
				if(zoomAdc>ZOOM_AD_MAX)
				{
											//get location here
					break;					//
				}//else if(zoomAdc<3)
				else if(zoomAdc<ZOOM_AD_MIN)
				{
					//motoSet.currentSet = 0;
					//return;
					break;
				}
			}

			MOTOENA = 0;

			if(errCnt>=MOTO_STEPS_TIMEOUT)
			{
				g_moto_error = 1;
				errCnt = 0;
#ifdef MOTO_DEBUG_PRT
				printf("Moto Error\r\n");
#endif
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return;
			}else
			{
				errCnt = 0;
			}

			
			

/*
			for(steps=0;steps<4*7*5;steps++)
			{
				MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
				if(i>3) i=0;
				delay_ms(1);
			}
*/
#ifdef MOTO_DEBUG_PRT
			printf("adc:%d\r\n",zmZdcTmp);
#endif
			//zoomAdc = GetZoomAdc();

			//motoSet.current_location = GetZoomByAdc(zoomAdc);	
			//motoSet.currentSet = GetZoomByAdc(zmZdcTmp);	
			//motoSet.currentSet = (GetZoomByAdc(zmZdcTmp)*5*2.7f);
			motoSet.currentSet = (GetZoomByAdc(zmZdcTmp)*20*2.7f);
			//motoSet.current_location = focLenScape[motoSet.currentSet+1];
			//motoSet.current_location = focLenVal[motoSet.currentSet];
			motoSet.current_location = 0;

#ifdef MOTO_TEST
			//*motoSet.dest_location = focLenVal[motoSet.currentSet];
			//motoSet.destRsv = *motoSet.dest_location;
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
#endif
			
#ifdef MOTO_DEBUG_PRT
			printf("set:%d,cur:%d\r\n",motoSet.currentSet,motoSet.current_location);
#endif

#ifdef MOTO_TEST_DSP
			//zoomAdc = GetZoomAdc();
			//DspMotoVad(LAYOUT_IDNUM_CML,zmZdcTmp);
			DspMotoVad(LAYOUT_IDNUM_CML+40,motoSet.currentSet);
#endif			

			
		}else
		{
			//while(errCnt++<20)	
			MOTOENA = 1;
			
			while(errCnt++<MOTO_STEPS_TIMEOUT)
			{
#ifdef _MOTO_CHK_4STEP_
				for(steps=0;steps<4;steps++)
#endif
				{
					MotoDrvPinSet(MOTO_DIR_BACKWARD,i++);
					if(i>3) i=0;
					delay_ms(1);
				}
				zoomAdc = GetZoomAdc();
				//if(zoomAdc>58)
				if(zoomAdc>ZOOM_AD_MAX)
				{
					break;
				}else if(zoomAdc>ZOOM_AD_OTS)
				{
					break;
				}/*else if(zoomAdc>50)			//底部边界
				{
					//motoSet.current_location = 200;
					MOTOENA = 0;
					motoSet.currentSet = 0;
					//return;
					break;
				}*/
			}

			MOTOENA = 0;

			if(errCnt>=MOTO_STEPS_TIMEOUT)
			{
				g_moto_error = 1;
				errCnt = 0;
				zoomAdc = GetZoomAdc();
#ifdef MOTO_DEBUG_PRT
				printf("Moto Error, zoomAdc %d\r\n",zoomAdc);
#endif
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return;
			}else
			{
				errCnt = 0;
			}

#ifdef MOTO_DEBUG_PRT
			printf("step1: %d\r\n",zoomAdc);
#endif
			delay_ms(2);

			MOTOENA = 1;

			zmZdcTmp = zoomAdc;

			//while(errCnt<20)
			while(errCnt++<MOTO_STEPS_TIMEOUT)
			{
#ifdef _MOTO_CHK_4STEP_
				for(steps=0;steps<4;steps++)
#endif
				{
					MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
					if(i>3) i=0;
					delay_ms(1);
				}
				zoomAdc = GetZoomAdc();

				if(zoomAdc<zmZdcTmp)
				{
					zmZdcTmp = zoomAdc;
				}
			//	if(zoomAdc>58)
				//if(zoomAdc<58)
				if(zoomAdc<ZOOM_AD_MAX)
				{
											//get location here
					break;					//
				}//else if(zoomAdc<3)
				//{
					//motoSet.currentSet = 0;
					//return;
					//break;
				//}
			}

			MOTOENA = 0;

			if(errCnt>=MOTO_STEPS_TIMEOUT)
			{
				g_moto_error = 1;
				errCnt = 0;
#ifdef MOTO_DEBUG_PRT
			printf("Moto Error\r\n");
#endif
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return;
			}else
			{
				errCnt = 0;
			}

#ifdef MOTO_DEBUG_PRT
			printf("step2:%d\r\n",zoomAdc);
#endif

			zmZdcTmp = zoomAdc;

			MOTOENA = 1;

			while(errCnt++<MOTO_STEPS_TIMEOUT)
			{
#ifdef _MOTO_CHK_4STEP_
				for(steps=0;steps<4;steps++)
#endif
				{
					MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
					if(i>3) i=0;
					delay_ms(1);
				}
				zoomAdc = GetZoomAdc();

				if(zoomAdc<zmZdcTmp)
				{
					zmZdcTmp = zoomAdc;
				}
			//	if(zoomAdc>58)
				//if(zoomAdc>58)
				if(zoomAdc>ZOOM_AD_MAX)
				{
											//get location here
					break;					//
				}//else if(zoomAdc<3)
				else if(zoomAdc<ZOOM_AD_MIN)
				{
					//motoSet.currentSet = 0;
					//return;
					break;
				}
			}

			MOTOENA = 0;

			if(errCnt>=MOTO_STEPS_TIMEOUT)
			{
				g_moto_error = 1;
				errCnt = 0;
#ifdef MOTO_DEBUG_PRT
			printf("Moto Error\r\n");
#endif
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
				return;
			}else
			{
				errCnt = 0;
			}

			
/*
			for(steps=0;steps<4*7*5;steps++)
			{
				MotoDrvPinSet(MOTO_DIR_FORWARD,i++);
				if(i>3) i=0;
				delay_ms(1);
			}

			zoomAdc = GetZoomAdc();
*/
#ifdef MOTO_DEBUG_PRT
			printf("step3:%d\r\n",zoomAdc);
#endif
#ifdef MOTO_DEBUG_PRT
			printf("adc:%d\r\n",zmZdcTmp);
#endif
			//motoSet.current_location = GetZoomByAdc(zmZdcTmp);
			//motoSet.currentSet = GetZoomByAdc(zmZdcTmp);
			//motoSet.currentSet = (GetZoomByAdc(zmZdcTmp)*5*2.7f);
			motoSet.currentSet = (GetZoomByAdc(zmZdcTmp)*20*2.7f);
			//motoSet.current_location = focLenScape[motoSet.currentSet+1];
			//motoSet.current_location = focLenVal[motoSet.currentSet];
			motoSet.current_location = 0;

#ifdef MOTO_TEST
			//*motoSet.dest_location = focLenVal[motoSet.currentSet];
			//motoSet.destRsv = *motoSet.dest_location;
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
#endif

#ifdef MOTO_DEBUG_PRT
			printf("set:%d,cur:%d\r\n",motoSet.currentSet,motoSet.current_location);
#endif

#ifdef MOTO_TEST_DSP
			//zoomAdc = GetZoomAdc();
			//DspMotoVad(LAYOUT_IDNUM_CML,zoomAdc);
			DspMotoVad(LAYOUT_IDNUM_CML+40,motoSet.currentSet);
#endif			

		}

		//motoSet.moving = 0;
		//MOTOENA = 0;

	}
#if 0	
	if(motoSet.moto_ready!=1)
	{
		//while(GETBIT(gBUartCon,2));
		//while(uart_tran_flag);
		//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_DISABLE,0);
		//SETBIT(motoSet.uartSendFlag,MOTO_CHARGE_ENABLE_BIT);
		
		if(!MOTOLOCA)
		//if(MOTOLOCA)
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
#endif

	
}

#else

void MotoReset(void)
{
	motoSet.moto_ready = 0;
	motoSet.resetFlag = 1;
	motoSet.motoRstCount = 0;
	
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
	
}

#endif

s8 GetMotoIndex(char para)
{
	char i;
	if(14 == para)
	{
		return 0;
	}
	
	for(i=0;i<sizeof(focLenVal);i++)
	//for(i=0;i<sizeof(focLenScape);i++)
	{
		if(para==focLenVal[i])
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
	//unsigned char motoDestIndex=0,motoOrgIndex=0;
	s8 motoDestIndex=0;
	s8 motoOrgIndex=0;
	s16 motoOrgSet=0;
	s16 motoDestSet=0;
//	signed int stepTmp;
	char i;
	static u8 ref_flag=0;
	static u8 mModRsv=0;
	static u16 mDestRsv=0;
//	static char ref_flag=0;
	
	
	if(motoSet.moving == 1)return;

#ifdef MOTO_REFLECT_DETECT

	//if(REF_BOARD_DETECT!=g_ref_flag)
	if(REF_BOARD_DETECT!=ref_flag)
	{
		//HaltSec(100);
		//if(REF_BOARD_DETECT>0)
		//{
			//HaltSec(100);
			if(REF_BOARD_DETECT>0)
			{
				//HaltSec(100);
				HaltSec(500);					//2015/10/06  
				if(REF_BOARD_DETECT>0)
				{
					if(14!=*motoSet.dest_location)
					{
						mDestRsv = *motoSet.dest_location;
						*motoSet.dest_location = 14;
						motoSet.destRsv = *motoSet.dest_location;
						mModRsv = *motoSet.mode;
						*motoSet.mode = MOTO_MODE_MANU;
						//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 
						//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
						//FLAOK_TOGGLE();
						//while(uart_tran_flag);
						//DecodeUartData(UART_ENCODE_REFLECT,1,0);
						//g_ref_flag = 1;
						ref_flag = 1;
						g_isReflectPulled = 1;
					}

				}
			}else
		//}else
		{
			//if(g_ref_flag>0)
			//HaltSec(120);
			HaltSec(500);					//2015/10/06  
			if(!REF_BOARD_DETECT)
			{
				if(ref_flag>0)
				{
					//g_ref_flag = 0;
					ref_flag = 0;
					*motoSet.mode = mModRsv;
					//*motoSet.dest_location = focLenScape[motoSet.focIndex];
					*motoSet.dest_location = mDestRsv;
					motoSet.destRsv = *motoSet.dest_location;
					//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL); 	
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
					g_isReflectPulled = 0;
					//FLAOK_TOGGLE();
					//while(uart_tran_flag);
					//DecodeUartData(UART_ENCODE_REFLECT,0,0);
				}
			}
		}
	}
#endif

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
					DisplayFocLen(*motoSet.mode,motoSet.destRsv,DSP_NORMAL);
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

#ifdef MOTO_RESET_AD
void MotoProc(void)
{
	static char i=0;
	u8 zoomAdc=0;
	static u8 zoomAdcRsv=0;

	if((motoSet.moto_en!=MOTO_ENABLE)||(g_moto_error!=0))
	{
		MOTOENA = 0;
		return;
	}else
	{

		if(MOTO_DIR_FORWARD==motoSet.motoDir)
		{
			if(*motoSet.dest_location<135)
			{
				zoomAdc = GetZoomAdc();
				if(zoomAdc<ZOOM_AD_MIN)
				{
					MotoReset();
					motoSet.moto_en = 0;
					motoSet.moving = 0;
					motoSet.moto_step = 0;
					MOTOENA = 0;
					return;
				}
			}else if(*motoSet.dest_location==135)
			{
				zoomAdc = GetZoomAdc();
				if(zoomAdc<ZOOM_AD_MIN)
				{
					motoSet.current_location = *motoSet.dest_location;
					motoSet.moto_en = 0;
					motoSet.moving = 0;
					motoSet.moto_step = 0;
					MOTOENA = 0;
					return;
				}
			}else if(*motoSet.dest_location==200)
			{
				zoomAdc = GetZoomAdc();
				if((zoomAdc<ZOOM_AD_MIN)&&(zoomAdcRsv>ZOOM_AD_MIN+5))
				{
					motoSet.moto_step = (focLenCoordinate[9] - focLenCoordinate[8]);
				}
				zoomAdcRsv = zoomAdc;
			}
		}
		/*
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
						ENABLE_CHARGE();
						//SetFocLen(0,105);
						//SendUartData(UART_DECODE_FOCLEN,0,0);
						//DecodeUartData(UART_ENCODE_CMD,UART_ENCODE_CMD_CHARGE_ENABLE,0);
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
		*/
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
/*
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
*/
			}
		}
	}
}
#else


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
			}
		}
	}
}


#endif


void TaskMotoProc(void *pdata)
{
	while(1)
	{
		MotoProc();
		OSTaskSuspend(MOTO_TICK_Prio);
	}
}

void MotoTickDriver(void)
{
	//if((motoSet.moto_en!=MOTO_ENABLE)||(g_moto_error!=0))
	if((MOTO_ENABLE==motoSet.moto_en)&&(!g_moto_error))
	{
		//MotoProc();
		OSTaskResume(MOTO_TICK_Prio);
	}
}

#else
void MotoReset(void)
{
}
#endif

/*
signed char GetMotoStdIndex(u8 dest)
{
	u8 i;
	for(i=1;i<sizeof(focLenScape);i++)
	{
		if(dest==focLenScape[i])
		{
			return i-1;
		}
	}

	return -1;
}
*/
signed char GetMotoStdIndex(u8 dest)
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




 
