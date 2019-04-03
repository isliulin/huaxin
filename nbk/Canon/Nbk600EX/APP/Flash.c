//if(WORKINGMODE_RTC==main_parameter.VIP_workmode)
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
#include "../HARDWARE/STMFLASH/stmflash.h"
#include "../HARDWARE/DMA/DMA.h"
#include "FlashPara.h"

void HpExpCodeSend(unsigned int hpv);
void UnexpectHandle(void);

extern signed char *g_cmdMode;
extern char g_chargeOk;
extern MultiSet multiSet;
//char gbRealFlash;
HpSetting hpSetting=0;
INT32U g_slaveTimer=0;
extern signed char g_expTTLPara1;
extern signed char g_expTTLPara2;
extern RadioStatue radioStatue;
void LightSlavePilot(void);
BOOLEAN SetFlashTimeOut(INT32U tm);

#if OS_TMR_EN>0
extern OS_TMR	*g_flashTmr;
extern OS_TMR  *g_slaChanDivTmr;
extern BOOLEAN  OSTmrDel (OS_TMR  *ptmr,
                   INT8U   *perr);
#endif

CanWireless canWireless={0};
extern Wl_Slave wlSlave;
//#define CLR_WLC_BUF {canWireless.WL_buf[0]=canWireless.WL_buf[1]=canWireless.WL_buf[2]=canWireless.WL_buf[3]=canWireless.WL_buf[4]=canWireless.WL_buf[5]=canWireless.WL_buf[6]=canWireless.WL_buf[7]=0;canWireless.WLBufIndex=0;}
#define CLR_WLC_BUF() {memset(canWireless.WL_buf,0x00,WLBUF_LEN);canWireless.WLBufIndex=0;canWireless.bitLoadIndex=0;}
#define WL_PARA_RESET() {canWireless.WL_Channel = WL_CHANNEL_UNSURE;	canWireless.WL_statue = CANWL_STATUE_HOLD;/*wlSlave.s_flashStatue = WL_FLASH_STATUE_HOLD;*/	canWireless.bitLoadIndex = 0;canWireless.WLBufIndex = 0;CLR_WLC_BUF();CLR_WLC_FLASH_TIMEOUT}
#define CLR_WLC_FLASH_TIMEOUT

extern unsigned char wl_opArr[22];//= {0x03,0x05,0x06,0x07,0x09,0x0a,0x0b,0x0d,0x0e,0x0f,0x11,0x12,0x13,0x15,0x16,0x17,0x19,0x1a,0x1b,0x1d,0x1e,0x1f,};

//unsigned char const wlsCntArr[3]={59,119,179};

const u16 g_expTime[] = {TTLTRA0,TTLTRA1,TTLTRA2,TTLTRA3,TTLTRA4,TTLTRA5,
					      OUTPUT_00,OUTPUT_01,OUTPUT_02,OUTPUT_10,OUTPUT_11,OUTPUT_12,
					      OUTPUT_20,OUTPUT_21,OUTPUT_22,OUTPUT_30,OUTPUT_31,OUTPUT_32,
					      OUTPUT_40,OUTPUT_41,OUTPUT_42,OUTPUT_50,OUTPUT_51,OUTPUT_52,
					      OUTPUT_60,OUTPUT_61,OUTPUT_62,OUTPUT_70,OUTPUT_71,OUTPUT_72,
					      OUTPUT_80};


const u16 g_hpExpCode[] = {HP_EXP_10,HP_EXP_11,HP_EXP_12,HP_EXP_20,HP_EXP_21,HP_EXP_22,
						HP_EXP_30,HP_EXP_31,HP_EXP_32,HP_EXP_40,HP_EXP_41,HP_EXP_42,
						HP_EXP_50,HP_EXP_51,HP_EXP_52,HP_EXP_60,HP_EXP_61,HP_EXP_62,
						HP_EXP_70,HP_EXP_71,HP_EXP_72,HP_EXP_80};


const u16 g_extExpCode[] = {EXTCODE_00, EXTCODE_01,EXTCODE_02,EXTCODE_03, EXTCODE_04,EXTCODE_05,
							EXTCODE_10,EXTCODE_11,EXTCODE_12,EXTCODE_20,EXTCODE_21,EXTCODE_22,
							EXTCODE_30,EXTCODE_31,EXTCODE_32,EXTCODE_40,EXTCODE_41,EXTCODE_42,
							EXTCODE_50,EXTCODE_51,EXTCODE_52,EXTCODE_60,EXTCODE_61,EXTCODE_62,
							EXTCODE_70,EXTCODE_71,EXTCODE_72,EXTCODE_80};

#ifdef FLASH_BASE_FIBER
const u16 g_flaExpCode[] = {FLA_TTLTRA0,FLA_TTLTRA1,FLA_TTLTRA2,
						FLA_TTLTRA3,	FLA_TTLTRA4,	FLA_TTLTRA5,FLA_EXP_00,FLA_EXP_01,FLA_EXP_02,
						FLA_EXP_10,FLA_EXP_11,FLA_EXP_12,FLA_EXP_20,FLA_EXP_21,FLA_EXP_22,
						FLA_EXP_30,FLA_EXP_31,FLA_EXP_32,FLA_EXP_40,FLA_EXP_41,FLA_EXP_42,
						FLA_EXP_50,FLA_EXP_51,FLA_EXP_52,FLA_EXP_60,FLA_EXP_61,FLA_EXP_62,
						FLA_EXP_70,FLA_EXP_71,FLA_EXP_72,FLA_EXP_80};
#endif

/*******************************************

1/8
A8 C0 1C 26 82 D5
AB C0 1C 0A 82 A7
AE C0 1B EE 82 79

1/4
B0 C0 1B DB 82 5A
B3 C0 1B AD 82 3F
B6 C0 1B 7E 82 23

1/2
B8 C0 1B 5F 82 10
BB C0 1B 0F 81 E4
BE C0 1A BE 81 B7

1/1
C8 C0 1A 88 81 89

********************************************/
const u32 g_colorTemperature[] = {0x1B8E8319,0x1B8782FC,0x1B7F82DF,0X1B7A82CB,0X1B7E82C4,0X1B8282BC,
								     0X1B8582B7,0X1BBE82C7,0X1BF782D8,0X1C1D82E4,0X1C2082DF,0X1C2382D9,
								     0X1C2682D5,0X1C0A82A7,0X1BEE8279,0X1BDB825A,0X1BAD823F,0X1B7E8223,
								     0X1B5F8210,0X1B0F81E4,0X1ABE81B7,0X1A888189};
//u32 g_colorTempRt=0;
u32 g_colorTempRt = 0X1A888189;
extern u8 g_isIndividualSlave;
u8 g_slaveChanShadow=0;


void ClearWlcBuf(void)
{
	CLR_WLC_BUF();
}

char HpExpCalc(char exp1,char exp2,char cmp1,char cmp2,unsigned int piso,HpSetting *hpSt,unsigned char ttlAccFlag)
{
	char value1, value2;
	//unsigned int xdata TMR2=0;
	//HpSetting hpSt;

	value1 = exp1+cmp1;
	value2 = exp2+cmp2;

	//if(0x02==ttlAccFlag)
	//{
	//	value1+=2;
	//}

/*
	if(value2>2)
	{
		value1 ++;
		value2 -= 3;
	}
	if(value1>=8)
	{
		value1 = 8;
		value2 = 0;
	}else if(value1<1)
	{
		value1 = 1;
		value2 = 0;
	}
*/
	if(value2>2)
	{
		value1++;
		value2-=3;
	}
	if(value1>=7)
	{
		//value1 = 8;
		value1 = 7;
		value2 = 0;
	}

	//*hpSt = g_hpExpCode[(value1-1)*3+value2];
	*hpSt = (g_hpExpCode[value1*3+value2]<<4);

	return 0;
	
}

void HwTriger(u16 tm)
{
	if(!g_isHotProtect)
	{
#ifdef FLASH_BASE_FIBER
		HP_SHOT_ONCE(tm);
#else
		WLM_SHOT_ONCE(tm);
#endif
	}
}

float CalcCmpPower(u8 tms)
{
	float ret=1;
	u8 i=0;
	for(i=0;i<tms;i++)
	{
		ret*=1.12f;
	}

	return ret;
}

/******************************
	NBK600 <====>   SP700
	 CON1   <----->   CON3
	 CON2   <----->   CON2
	 CON3   <----->   CON4
	 CON4   <----->   CON1	
*******************************/
void HighSpeedTriger(HpSetting *hps,unsigned int tmOv)
{
	//HC_CON3 = 1;
	HC_CON1 = 1;
	//HC_CON1 = 0;
	//HC_CON4 = 0;
	//HC_CON3 = 0;
	HC_CON2 = 0;

#ifdef FIBER_ADJUST_PROC
	//if(0xA5==mainPara.isFiberAdjust)
	if(FIBER_ADJUST_FLAG==mainPara.isFiberAdjust)
	{
		//*hps = (*hps*255)/mainPara.fiberCoe;
		if(mainPara.fiberCoe>9)
		{
			*hps *= CalcCmpPower(mainPara.fiberCoe-9);
			//if(*hps>4095)
			if(*hps>DA_MAX)
			{
				//*hps = 4095;
				*hps=DA_MAX;
			}
		}else
		{
			*hps /= CalcCmpPower(9-mainPara.fiberCoe);
		}
#ifdef FIBER_ADJUST_TEST
		printf("FLA AD 0x%x\r\n",*hps);
#endif
	}/*
	else if(0xA6==mainPara.isFiberAdjust)
	{
		*hps /= CalcCmpPower(mainPara.fiberCoe);
	}*/
#endif
	
	HpExpCodeSend(*hps);

	//delay_us(30);			//delay for AD
	
	HC_CON3 = 0;
	HwTriger(tmOv);
	//HC_CON4 = 1;	
	HC_CON3 = 1;	
}

void LightSlaveParaClear(void)
{
//	INT8U tmrErr;

//	canWireless.activeArr=0;
//			gbRealFlash = 1;
	canWireless.WL_statue = CANWL_STATUE_HOLD;
	canWireless.WL_Channel = WL_CHANNEL_UNSURE;
//	wlSlave.s_flashStatue = WL_FLASH_STATUE_HOLD;
	canWireless.preArr = WL_ARRARY_NULL;
	wlSlave.s_wlMode = WORKINGMODE_OFF;
	canWireless.activeArr=0;

	//OSTmrDel(g_flashTmr,&tmrErr);
	//OSTmrStop(g_flashTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);
	//StartCapture();

	//here clear the buffer
	CLR_WLC_BUF();

	StopCapture();
	TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);

	//StartCapture();
}


void LightSlaveProc(u16 plusWidth)
{

//	unsigned int WL_chan1=0;
	u8 bits=0;
//	INT8U tmrErr;
	
	//static unsigned int tmrRcd;
	//static unsigned int WL_chan1=0,WL_chan2=0,plusWidth=0;

	if(canWireless.WL_statue==CANWL_STATUE_HOLD)
	{

		canWireless.WL_statue = CANWL_STATUE_ENSURECHAN;

		//SET_WLC_FLASH_TIMEOUT;
		//while(!WIRELESS_IN);
		//TM7MD1 |= 0X10;	
			
	}else if(canWireless.WL_statue==CANWL_STATUE_ENSURECHAN)
	{
#if 1
		if((plusWidth>WLC_CHAN_SCALE1_GND)&&(plusWidth<WLC_CHAN_SCALE1_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_ONE;
		}else if((plusWidth>WLC_CHAN_SCALE2_GND)&&(plusWidth<WLC_CHAN_SCALE2_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_TWO;
		}else if((plusWidth>WLC_CHAN_SCALE3_GND)&&(plusWidth<WLC_CHAN_SCALE3_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_THREE;
		}else if((plusWidth>WLC_CHAN_SCALE4_GND)&&(plusWidth<WLC_CHAN_SCALE4_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_FOUR;
		}else
		{
			//canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			//canWireless.WL_statue = CANWL_STATUE_HOLD;
#ifdef LITE_SLAVE_REVIEW
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
#else
			UnexpectHandle();	
#endif
			//StopCapture();
			//OSTmrStart(g_slaChanDivTmr, &tmrErr);	
		}
#else

		if((plusWidth>WLC_CHAN_SCALE1-WLC_CHAN_VAL)&&(plusWidth<WLC_CHAN_SCALE1+WLC_CHAN_VAL))
		{
			canWireless.WL_Channel = WL_CHANNEL_ONE;
		}else if((plusWidth>WLC_CHAN_SCALE2-WLC_CHAN_VAL)&&(plusWidth<WLC_CHAN_SCALE2+WLC_CHAN_VAL))
		{
			canWireless.WL_Channel = WL_CHANNEL_TWO;
		}else if((plusWidth>WLC_CHAN_SCALE3-WLC_CHAN_VAL)&&(plusWidth<WLC_CHAN_SCALE3+WLC_CHAN_VAL))
		{
			canWireless.WL_Channel = WL_CHANNEL_THREE;
		}else if((plusWidth>WLC_CHAN_SCALE4-WLC_CHAN_VAL)&&(plusWidth<WLC_CHAN_SCALE4+WLC_CHAN_VAL))
		{
			canWireless.WL_Channel = WL_CHANNEL_FOUR;
		}else
		{
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			UnexpectHandle();			
		}
		
#endif

#ifdef LITE_SLAVE_REVIEW
		canWireless.WL_statue = CANWL_STATUE_RUN;
		CLR_WLC_BUF();
#else
		//if(*wlSlave.s_chan == canWireless.WL_Channel)
		if(*wlSet.m_chan == canWireless.WL_Channel)
		{
			canWireless.WL_statue = CANWL_STATUE_RUN;
			//canWireless.bitLoadIndex = 0;
			//canWireless.WLBufIndex = 0;
			CLR_WLC_BUF();
			//if(wlSlave.s_flashStatue==WL_FLASH_STATUE_HOLD)
			//{
			//	wlSlave.s_flashStatue = WL_FLASH_STATUE_START;
			//}

		}else
		{
			//should be handle there
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			//canWireless.WL_statue = CANWL_STATUE_HOLD;
			canWireless.WL_statue = CANWL_STATUE_ENSURECHAN;
			//wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			//canWireless.bitLoadIndex = 0;
			//canWireless.WLBufIndex = 0;
			CLR_WLC_BUF();
			UnexpectHandle();	
			//FLAOK_ON();
		}
		canWireless.WL_Channel = WL_CHANNEL_UNSURE;
#endif
	//	tmrRcd = TM7IC;
		
	}else if(canWireless.WL_statue==CANWL_STATUE_RUN)
	{
		
		//bits = (plusWidth+30)/100;								//

		/*
		if(plusWidth<70)
		{
			plusWidth += 30;
			
		}else if(plusWidth>300)
		{
			plusWidth -= 30;
		}*/
				
		bits = (plusWidth+WLC_DATA_STD)/100;

		if(bits<=0)
		{
			UnexpectHandle();
			return;
		}
		
		if(canWireless.bitLoadIndex+bits>8)
		{
			canWireless.WLBufIndex++;
			canWireless.bitLoadIndex = 0;
		}else
		{
			canWireless.bitLoadIndex+=bits;
			canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
		}	

		if(canWireless.WLBufIndex>=WLBUF_LEN)			//2015/10/25
		{
			UnexpectHandle();
			return;
		}
		
	}else if(canWireless.WL_statue==CANWL_STATUE_PREFLASH)
	{
		//if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==0)&&(charge_ok!=AD_CHARGE_LOW)&&(canWireless.activeArr==1))
		//if((charge_ok!=AD_CHARGE_LOW)&&(canWireless.preArr==wlSet.s_arrary))
		//if(g_chargeOk>0)							//2015/10/26
		{
/******************************************
			the following is preflash taken
******************************************/
			//HpExpCalc(4,0,0,0,0,&hpSetting,0);
			//HighSpeedTriger(&hpSetting,HP_PREFLASH_INTERVAL);			//preflash
			//TrigerMasterOn(0);
			//HpPreflash(HP_PREFLASH_INTERVAL);						//usually delete for debug
			radioStatue.cmdReady = 1;
			radioStatue.cmdSta = RADIO_STATUE_SLAPRE;
			//StopCapture();
			OSTaskResume(CMDSEND_TASK_Prio);
/********************************************/
			//canWireless.activeArr=0;
//			gbRealFlash = 1;
			//canWireless.WL_statue = CANWL_STATUE_HOLD;
			//canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			//canWireless.preArr = WL_ARRARY_NULL;
			//delay_us(400);

		}/*			//2015/10/26
		else
		{
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;	
			delay_us(1200);
			LightSlaveParaClear();
		}
		*/
		
	}else if(canWireless.WL_statue==CANWL_STATUE_FLASH)
	{
#if 0
		if(PILOT_FLAG_ON==canWireless.pilotFalg)
		{
			canWireless.pilotFalg = PILOT_FLAG_OFF;
			if(canWireless.activeArr==1)
			{
				canWireless.activeArr = 0;
				//if(0x01==flashStatue.flash_enable)
				//if((charge_ok!=AD_CHARGE_LOW)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT)))
				if(g_chargeOk>0)
				{
					//RN_ON_TRIGER(0XFFB6);
					switch(*wlSlave.s_array)
					{
						case WL_ARRARY_A:
							wlSlave.testCnt = 60;
							wlSlave.testPilot = TEST_PILOT_ON;
						break;

						case WL_ARRARY_B:
							wlSlave.testCnt = 120;
							wlSlave.testPilot = TEST_PILOT_ON;
						break;

						case WL_ARRARY_C:
							wlSlave.testCnt = 180;
							wlSlave.testPilot = TEST_PILOT_ON;
						break;
					}
				}
			}

		}else
#endif
		{

			//if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==0)&&(charge_ok!=AD_CHARGE_LOW)&&(canWireless.activeArr==1))
			//if((WORKINGMODE_TTL==wlSlave.s_wlMode)||(WORKINGMODE_MANU==wlSlave.s_wlMode))
			//{
				//if((gbRealFlash)&&(canWireless.activeArr==1)&&(charge_ok!=AD_CHARGE_LOW)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT)))
				//FLAOK_TOGGLE();
				//if(g_chargeOk>0)
				//{
					/*
					if(WL_HPSYN_ON==canWireless.HpSyn)
					{
						if(WORKINGMODE_TTL==wlSlave.s_wlMode)
						{
							HpExpCalc(0,0,0,0,0,&hpSetting,0);

							HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);
						}else
						{	

							HpExpCalc(canWireless.WL_output1,canWireless.WL_output2,0,0,0,&hpSetting,0);
							HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);		
						}
						canWireless.HpSyn = WL_HPSYN_OFF;
					}else
					{
						pTime = CanOutputSwitch(canWireless.WL_output1,canWireless.WL_output2);
						TURN_ON_TRIGER(pTime);
					}*/
				radioStatue.cmdReady = 1;
				radioStatue.cmdSta = RADIO_STATUE_SLAFLA;
				//FLAOK_TOGGLE();
				//StopCapture();
				OSTaskResume(CMDSEND_TASK_Prio);
				//}else
				//{
				//	LightSlaveParaClear();
				//}
			//}
#if 0
			else if(WORKINGMODE_MULTI==wlSlave.s_wlMode)
			{
				//if((charge_ok!=AD_CHARGE_LOW)&&(!GETBIT(bitIndex,BIT_bitIndex_PROTECT))&&(canWireless.activeArr==1))
				if(g_chargeOk>0)
				{
					*multiSet.expose = canWireless.wlMul_expose;
#ifdef _CANON_
					pTime = CanOutputSwitch(*multiSet.expose,0);
#else
					pTime = CanOutputSwitch(*multiSet.expose>>4,0);
#endif
					TURN_ON_TRIGER(pTime);
					multiSet.finalExpTimes = canWireless.wlMul_time;
					//multiSet.eTimes--;
					multiSet.finalExpTimes--;
					multiSet.mulCtl=MULTI_FLASH_ENABLE;
					multiSet.lowFrqCnt = (1000/canWireless.wlMul_frq);	
					//SETBIT(bitWireless,BIT_bitWireless_mode);
				}else
				{
					LightSlaveParaClear();
				}
			}
#endif

		}			//else pilot
#if 0
		canWireless.activeArr=0;		
		//CLR_WLC_FLASH_TIMEOUT		
		canWireless.WLBufIndex = 0;
		canWireless.bitLoadIndex = 0;
		canWireless.WL_statue = CANWL_STATUE_HOLD;
		canWireless.WL_Channel = WL_CHANNEL_UNSURE;
		wlSlave.s_flashStatue = WL_FLASH_STATUE_HOLD;
		CLR_WLC_BUF;
#endif				

	}

}

unsigned char WlmCodeExtract(unsigned char *wlmArr, unsigned char *pt,unsigned char bitSum)
{
	unsigned char ret, cIndex, bIndex;
	unsigned int intMedia;
	cIndex = ((*pt)>>3);
	bIndex = ((*pt)&0x07);

	if((bIndex+bitSum)<8)
	{
		ret = ((wlmArr[cIndex]>>(bIndex))&((1<<bitSum)-1));
	}else
	{
		intMedia = (MAKEWORD(wlmArr[cIndex],wlmArr[cIndex+1])>>bIndex);
		ret = intMedia & ((1<<bitSum)-1);
	}
	*pt += bitSum;
	return ret;	
}


void WlmCodeExcludeNone(unsigned char *wlmArr,unsigned char *pt)
{
	unsigned char cIndex,bIndex;
	cIndex = (*pt)>>3;
	bIndex = ((*pt)&0x07);
	
	//while(!((wlmArr[(*pt)>>3]>>((*pt)&0x07))&0x01))
	//{
	//	(*pt)++;
	//}
	while(!((wlmArr[cIndex]>>bIndex)&0x01))
	{
		bIndex ++;
		if(bIndex>=8)
		{
			cIndex ++;
			bIndex = 0;
		}
	}

	*pt = (cIndex<<3 | bIndex);

	
}

unsigned char CodeSearch(unsigned char *srcArr,unsigned char size,const unsigned char cd)
{
	unsigned char index=0;
	while(index<size)
	{
		if(cd==srcArr[index])
		{
			return index;
		}else
		{
			index++;
		}
	}
	
	return index;
		
}

void IndexToOutput(unsigned char *opt1,unsigned char *opt2,unsigned char id)
{
	//*opt1 = ((id /3)+1);
	*opt1 = (id/3);
	*opt2 = ((id+3) % 3);
}

u16 WlMultiFrqConver(unsigned char rec)
{
	if(rec<0x14)
	{
		return (rec+1);
	}else if(rec<0x1a)
	{
		return (20 +( rec -0x13)*5);
	}else if(rec<0x29)
	{
		return (50 +(rec -0x19)*10);
	}else 
	{
		return (200+(rec-0x28)*50);
	}
}

unsigned char WlMultiTimeConver(unsigned char rec)
{
	unsigned char recTmp;
	//recTmp =  ((rec>>4)*4+((rec&0x0f) - 0x08));
	//recTmp =  ((rec>>4)*4+(rec&0x0f));
	recTmp = rec;
	if(recTmp<20)
	{
		return recTmp;
	}else if(recTmp<26)
	{
		return (20 + 5*(recTmp-20));
	}else 
	{
		return (50+10*(recTmp-26));
	}
}

//#define TTL_CALC_MIN      0X60
#define TTL_CALC_MIN      0x78//0x70//0x68//0x70//0X78
#define TTL_CALC_MAX     ((7+2)*3*3+TTL_CALC_MIN)
#define TTL_HPCALC_MIN   0X70
#define TTL_HPCALC_MAX   (7*3*3+TTL_HPCALC_MIN)

//void CanExposeTimeCal(const unsigned char o_camCode,unsigned char fpMod,unsigned char fpSw)
void CanExposeTimeCal(const unsigned char o_camCode,unsigned char fpMod)
{
	unsigned char camCode=0;
//	u8 tmp1;
//	u8 tmp2;

	//tmp1 = menuPara.cApert>>4;
	//tmp2 = menuPara.cApert&0x0F;	
	//flaPara.apert = CannonApertVal[tmp1*6+(tmp2+2)/3];
	

	if(HP_FLASHMODE_OFF==fpMod)
	{
		//camCode = o_camCode-30;
		//camCode = o_camCode;

		//if(o_camCode<=0x60)
		//if(o_camCode<=0x70)
		if(o_camCode<TTL_CALC_MIN)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 0;
		}//else if(camCode<=0x8F)
		//else if(o_camCode<=0xB8)
		//else if(o_camCode<=0xA8)
		else if(o_camCode<TTL_CALC_MAX)
		{
			//camCode = (o_camCode-0x6B)/2;
			camCode = (o_camCode-TTL_CALC_MIN);
			//camCode = (o_camCode-0x70);
			//tmp1 = camCode>>4;
			//tmp2 = camCode&0x0F;
			//camCode = (tmp1*6+(tmp2+2)/3);
			camCode /= 3;
			//g_expTTLPara1 = (-2)+(camCode/3);
			//g_expTTLPara1 = (-2)+(tmp1*2);
			//g_expTTLPara2 = (camCode%3);
			g_expTTLPara1 = (-2)+camCode/3;
			g_expTTLPara2 = (camCode%3);

			if(g_expTTLPara1>7)
			{
				g_expTTLPara1 = 7;
			}
			//g_expTTLPara1 = ((-2) + (camCode-0x6B)/3);
			//g_expTTLPara2 = ((camCode-0x6B)%3);
			
		}else
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
		}

	}else
	{

		camCode = o_camCode;

		if(o_camCode<=TTL_HPCALC_MIN)
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 0;
		}else if(o_camCode<TTL_HPCALC_MAX)
		{
			//camCode = (o_camCode-0x6B)/2;
			camCode = (o_camCode-TTL_HPCALC_MIN);
			//camCode = (o_camCode-0x70);
			//tmp1 = camCode>>4;
			//tmp2 = camCode&0x0F;
			//camCode = (tmp1*6+(tmp2+2)/3);
			camCode /= 3;
			//g_expTTLPara1 = (-2)+(camCode/3);
			//g_expTTLPara1 = (-2)+(tmp1*2);
			//g_expTTLPara2 = (camCode%3);
			g_expTTLPara1 =  camCode/3;
			g_expTTLPara2 = (camCode%3);

			if(g_expTTLPara1>7)
			{
				g_expTTLPara1 = 7;
			}
			//g_expTTLPara1 = ((-2) + (camCode-0x6B)/3);
			//g_expTTLPara2 = ((camCode-0x6B)%3);
			
		}else
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
		}
/*
		if(camCode<0x5E)
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 0;
		}else
		{
			g_expTTLPara1 = (1+(camCode-0x5E)/3);
			g_expTTLPara2 = ((camCode-0x5e)%3);
		}
*/
/*
		if(camCode<0x5C)//else if((109<=camCode)&&(camCode<112))
		{
			//main_parameter.VIP_ttlTime = 70 - (87-camCode);//camCode - 90 ;	 
			//main_parameter.VIP_ttlTime = 50;
			//if(main_parameter.VIP_ttlTime<0)
			//	main_parameter.VIP_ttlTime = 0;
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 0;
		}
		else if(camCode<0x60)//else if((112<=camCode)&&(camCode<115))
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x64)//else if((115<=camCode)&&(camCode<118))
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x67)//else if((118<=camCode)&&(camCode<121))
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 0;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x69)//else if((121<=camCode)&&(camCode<124))
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x6A)//else if((124<=camCode)&&(camCode<127))
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x6C)//else if((127<=camCode)&&(camCode<130))
		{
			g_expTTLPara1 = 2;
			g_expTTLPara2 = 0;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x6E)//else if((130<=camCode)&&(camCode<133))
		{
			g_expTTLPara1 = 2;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x70)//else if((133<=camCode)&&(camCode<136))
		{
			g_expTTLPara1 = 2;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}

		else if(camCode<0x71)//else if((136<=camCode)&&(camCode<139))
		{
			g_expTTLPara1 = 3;
			g_expTTLPara2 = 0;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x74)//else if((139<=camCode)&&(camCode<142))
		{
			g_expTTLPara1 = 3;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x77)//else if((142<=camCode)&&(camCode<145))
		{
			g_expTTLPara1 = 3;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x78)//else if((145<=camCode)&&(camCode<148))
		{
			g_expTTLPara1 = 4;
			g_expTTLPara2 = 0;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x79)//else if((148<=camCode)&&(camCode<151))
		{
			g_expTTLPara1 = 4;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x7A)//else if((151<=camCode)&&(camCode<154))
		{
			g_expTTLPara1 = 4;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x7D)//else if((154<=camCode)&&(camCode<157))
		{
			g_expTTLPara1 = 5;
			g_expTTLPara2 = 0;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x80)//else if((157<=camCode)&&(camCode<160))
		{
			g_expTTLPara1 = 5;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x82)//else if((160<=camCode)&&(camCode<163))
		{
			g_expTTLPara1 = 5;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x84)//else if((163<=camCode)&&(camCode<166))
		{
			g_expTTLPara1 = 6;
			g_expTTLPara2 = 0;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x86)//else if((166<=camCode)&&(camCode<169))
		{
			g_expTTLPara1 = 6;
			g_expTTLPara2 = 1;
			//main_parameter.VIP_ttlTime = 50;
		}
		else if(camCode<0x87)//else if((169<=camCode)&&(camCode<172))
		{
			g_expTTLPara1 = 6;
			g_expTTLPara2 = 2;
			//main_parameter.VIP_ttlTime = 50;
		}
		else//else if(camCode>=172)
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
		}
*/
	}

}

void LightSlaveTmrProc(void)
{
//	INT8U tmrErr;
	if(CANWL_STATUE_FLASH==canWireless.WL_statue)			//delete here will cause multi flash interrupt
	{
		radioStatue.cmdReady = 1;
		radioStatue.cmdSta = RADIO_STATUE_SLAFLA;
		OSTaskResume(CMDSEND_TASK_Prio);
		//StopCapture();
	}else
	{
		LightSlaveParaClear();	
	}
	//OSTmrStop(g_flashTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);
	//OSTmrDel(g_flashTmr,&tmrErr);
}

void LightSlaveTtlPilot(void)
{
//	INT8U tmrErr;

	g_expTTLPara1=0;
	g_expTTLPara2=0;
	canWireless.HpSyn=0;
	canWireless.WL_statue = CANWL_STATUE_FLASH;
	//canWireless.activeArr = 1;
	
	
	switch(canWireless.WL_buf[1])
	{
		case 0xB0:
			//g_flashTmr = OSTmrCreate(600, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
			//OSTmrStart(g_flashTmr, &tmrErr) ;
			canWireless.activeArr = 1;
			//SetFlashTimeOut(600);
			g_slaveTimer = 600;
			SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
		break;

		case 0xB4:
			if(WL_ARRARY_A==*wlSlave.s_array)
			{
				//g_flashTmr = OSTmrCreate(600, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
				//OSTmrStart(g_flashTmr, &tmrErr) ;
				canWireless.activeArr = 1;
				//SetFlashTimeOut(600);
				g_slaveTimer = 600;
				SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
			}else if(WL_ARRARY_B==*wlSlave.s_array)
			{
				//g_flashTmr = OSTmrCreate(1200, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
				//OSTmrStart(g_flashTmr, &tmrErr) ;
				canWireless.activeArr = 1;
				//SetFlashTimeOut(1200);
				g_slaveTimer = 1200;
				SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
			}else
			{
				LightSlaveParaClear();	
			}
		break;

		case 0xB5:
			if(WL_ARRARY_A==*wlSlave.s_array)
			{
				//g_flashTmr = OSTmrCreate(600, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
				//OSTmrStart(g_flashTmr, &tmrErr) ;
				//SetFlashTimeOut(600);
				g_slaveTimer = 600;
				SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
			}else if(WL_ARRARY_B==*wlSlave.s_array)
			{
				//g_flashTmr = OSTmrCreate(1200, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
				//OSTmrStart(g_flashTmr, &tmrErr) ;
				//SetFlashTimeOut(1200);
				g_slaveTimer = 1200;
				SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
			}else if(WL_ARRARY_C==*wlSlave.s_array)
			{
				//g_flashTmr = OSTmrCreate(1800, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
				//OSTmrStart(g_flashTmr, &tmrErr) ;
				//SetFlashTimeOut(1800);
				g_slaveTimer = 1800;
				SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
			}
			canWireless.activeArr = 1;
		break;

		default:
		UnexpectHandle();
		return;
	}

	wlSlave.s_wlMode = WORKINGMODE_TTL;
	if(!g_isIndividualSlave)
	{
		mainPara.workMode = WORKINGMODE_TTL;
	}
	//LightSlaveParaClear();	
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

	
}

BOOLEAN SetFlashTimeOut(INT32U tm)
{
	INT8U tmrErr;
	BOOLEAN ret;
	//g_flashTmr = OSTmrCreate(tm, 0,  OS_TMR_OPT_ONE_SHOT,  (OS_TMR_CALLBACK)LightSlaveTmrProc,  NULL, "bkTmr ", &tmrErr);
	OSTmrReload(g_flashTmr,tm);
	ret = OSTmrStart(g_flashTmr, &tmrErr) ;
	//ret = ret;
	return ret;

	//if(OS_TRUE!=ret)
	//{
	//	FLAOK_TOGGLE();
	//	TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,tmrErr,DSP_NORMAL);
	//	delay_ms(500);
		//while(1);
	//}
}

void LightSlaveManu(void)
{
	char temp=0;
#if(defined(LIGHT_SLAVE_DEBUG))
	u8 tmp1=0;
#endif
	
	wlSlave.s_wlMode = WORKINGMODE_MANU;

	if(0x0F==canWireless.WL_buf[0])
	{
		canWireless.HpSyn = WL_HPSYN_OFF;
	}else if(0x07==(canWireless.WL_buf[0]&0x0f))
	{
		canWireless.HpSyn = WL_HPSYN_ON;
	}else
	{
		UnexpectHandle();
		return;
	}
		
	switch(canWireless.WL_buf[1]&0x07)
	{
		case 0x01:
			canWireless.WL_mode = WL_SCHEDULE_ALL;
			canWireless.WL_opCode = canWireless.WL_buf[1]>>3;
			canWireless.activeArr=1;
			
#if(defined(LIGHT_SLAVE_DEBUG))
			LCD_Clear(0);
			for(tmp1=0;tmp1<canWireless.WLBufIndex;tmp1++)
			{
				TtlDebugDspDat(2, tmp1*24, canWireless.WL_buf[tmp1], DSP_NORMAL);
			}
			while(1);
#endif			
			//FLAOK_ON();				//for debug
			//gbRealFlash = 1;
			//if(canWireless.WLBufIndex<4)
			//{
			//	canWireless.WL_statue = CANWL_STATUE_FLASH;
			//}else
			//{
			//	LightSlavePilot();
			//}
			//return;
		break;

		case 0x03:
			//canWireless.WL_mode = WL_SCHEDULE_RES;
			canWireless.WL_mode = WL_SCHEDULE_AB;
			//wlSlave.s_wlMode = WORKINGMODE_MANU;
			if(WL_ARRARY_A==*wlSlave.s_array)
			{
				canWireless.WL_opCode = canWireless.WL_buf[1]>>3;
				canWireless.activeArr=1;
				//gbRealFlash = 1;
				//return;
			}else if(WL_ARRARY_B==*wlSlave.s_array)
			{
				canWireless.WL_opCode = canWireless.WL_buf[2]>>3;
				canWireless.activeArr=1;
				//gbRealFlash = 1;
			}else if(WL_ARRARY_C==*wlSlave.s_array)
			{
				canWireless.WL_opCode = 0;
				canWireless.activeArr=0;
				//gbRealFlash = 1;
			}
			//if(canWireless.WLBufIndex<5)
			//{
			//	canWireless.WL_statue = CANWL_STATUE_FLASH;
			//}else
			//{
			//	LightSlavePilot();
			//}
			//temp = CodeSearch(wl_opArr,22,canWireless.WL_opCode);
			//IndexToOutput(&canWireless.WL_output1,&canWireless.WL_output2,temp);
			//canWireless.WL_statue = CANWL_STATUE_FLASH;
			//canWireless.activeArr=1;
		break;

		case 0x05:
			canWireless.activeArr=1;
			canWireless.WL_mode = WL_SCHEDULE_RES;
			if(WL_ARRARY_A==*wlSlave.s_array)
			{
				canWireless.WL_opCode = canWireless.WL_buf[1]>>3;
				//gbRealFlash = 1;
				//return;
			}else if(WL_ARRARY_B==*wlSlave.s_array)
			{
				canWireless.WL_opCode = canWireless.WL_buf[2]>>3;
				//gbRealFlash = 1;
			}else if(WL_ARRARY_C==*wlSlave.s_array)
			{
				canWireless.WL_opCode = canWireless.WL_buf[3]>>3;
				//gbRealFlash = 1;
			}
			//if(canWireless.WLBufIndex<6)
			//{
			//	canWireless.WL_statue = CANWL_STATUE_FLASH;
			//}else
			//{
			//	LightSlavePilot();
			//}
		break;

		default:
			UnexpectHandle();
		return;
		//break;
	}

	temp = CodeSearch(wl_opArr,22,canWireless.WL_opCode);

	if(temp>=22)
	{
		UnexpectHandle();
		return;
	}
	
	IndexToOutput(&canWireless.WL_output1,&canWireless.WL_output2,temp);
	canWireless.WL_statue = CANWL_STATUE_FLASH;
	//SetFlashTimeOut(100);
	g_slaveTimer = 100;
	SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
	ENABLE_D30EN();
	if(!g_isIndividualSlave)
	{
		mainPara.workMode = wlSlave.s_wlMode;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}

void LightSlaveMulti(void)
{
	char temp=0;
	
	canWireless.wlMul_frq = WlMultiFrqConver(canWireless.WL_buf[1]&0x3f);
	
	if(canWireless.wlMul_frq>200)
	{
		UnexpectHandle();
		return;
	}
	canWireless.wlMul_time = WlMultiTimeConver((MAKEWORD(canWireless.WL_buf[1], canWireless.WL_buf[2])>>6)&0x1f);
	
	if(canWireless.wlMul_time>100)
	{
		UnexpectHandle();
		return;
	}	
	
	if(0x9F==canWireless.WL_buf[0])
	{
		canWireless.WL_opCode = (canWireless.WL_buf[2]>>3)&0x1F;
		canWireless.activeArr=1;
		//if(canWireless.WLBufIndex<5)
		//{
		//	canWireless.WL_statue = CANWL_STATUE_FLASH;
		//}else
		//{
		//	LightSlavePilot();
		//}
		//temp = CodeSearch(wl_opArr,22,canWireless.WL_opCode);
	}else if(0xAF==canWireless.WL_buf[0])
	{
		if(WL_ARRARY_A==*wlSlave.s_array)
		{
			canWireless.WL_opCode = (canWireless.WL_buf[2]>>3)&0x1F;
			canWireless.activeArr=1;
			//gbRealFlash = 1;
			//return;
		}else if(WL_ARRARY_B==*wlSlave.s_array)
		{
			canWireless.WL_opCode = (canWireless.WL_buf[3]>>3)&0x1F;
			canWireless.activeArr=1;
			//gbRealFlash = 1;
		}else if(WL_ARRARY_C==*wlSlave.s_array)
		{
			canWireless.WL_opCode = 0;
			canWireless.activeArr=0;
			//gbRealFlash = 1;
		}
		//if(canWireless.WLBufIndex<6)
		//{
		//	canWireless.WL_statue = CANWL_STATUE_FLASH;
		//}else
		//{
		//	LightSlavePilot();
		//}
	}else if(0xBF==canWireless.WL_buf[0])
	{
		if(WL_ARRARY_A==*wlSlave.s_array)
		{
			canWireless.WL_opCode = (canWireless.WL_buf[2]>>3)&0x1f;
			canWireless.activeArr=1;
			//gbRealFlash = 1;
			//return;
		}else if(WL_ARRARY_B==*wlSlave.s_array)
		{
			canWireless.WL_opCode = (canWireless.WL_buf[3]>>3)&0x1f;
			canWireless.activeArr=1;
			//gbRealFlash = 1;
		}else if(WL_ARRARY_C==*wlSlave.s_array)
		{
			canWireless.WL_opCode = (canWireless.WL_buf[4]>>3)&0x1F;
			canWireless.activeArr=1;
			//gbRealFlash = 1;
		}
		//if(canWireless.WLBufIndex<7)
		//{
		//	canWireless.WL_statue = CANWL_STATUE_FLASH;
		//}else
		//{
		//	LightSlavePilot();
		//}
	}else
	{
		UnexpectHandle();
		return;
	}
	temp = CodeSearch(wl_opArr,22,canWireless.WL_opCode);
	
	if(temp>15)
	{
		UnexpectHandle();
		return;
	}	
	
	canWireless.wlMul_expose = ((temp / 3));
	//canWireless.activeArr=1;
	wlSlave.s_wlMode = WORKINGMODE_MULTI;
	if(!g_isIndividualSlave)
	{
		mainPara.workMode = WORKINGMODE_MULTI;
	}
	//SetFlashTimeOut(100);
	g_slaveTimer = 100;
	SET_UPDATE_EVT(DSP_UPDATE_SETSLATMR);
	//canWireless.activeArr=1;
	canWireless.WL_statue = CANWL_STATUE_FLASH;
	ENABLE_D30EN();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void LightSlavePilot(void)
{
#if 0
	switch(canWireless.WL_buf[1]&0x0f)
	{
		case 0x00:
		wlSlave.testCnt = 60-1;
		wlSlave.testPilot = TEST_PILOT_ON;
		break;

		case 0x04:
		if((WL_ARRARY_A==*wlSlave.s_array)||(WL_ARRARY_B==*wlSlave.s_array))
		{
			wlSlave.testCnt = wlsCntArr[*wlSlave.s_array];
			wlSlave.testPilot = TEST_PILOT_ON;
		}
		break;

		case 0x05:
		wlSlave.testCnt = wlsCntArr[*wlSlave.s_array];
		wlSlave.testPilot = TEST_PILOT_ON;
		break;
	}
	canWireless.WL_statue = CANWL_STATUE_HOLD;
	mainPara.workMode = WORKINGMODE_TTL;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
#endif
	LightSlaveTmrProc();
}

void LightSlaveTtl(void)
{

	switch(canWireless.WL_buf[0]&0x0f)
	{
		case 0x0c:
			canWireless.WL_mode = WL_SCHEDULE_ALL;
			canWireless.HpSyn = WL_HPSYN_OFF;
			//wlSlave.s_wlMode = WORKINGMODE_TTL;
			//canWireless.activeArr = 1;
		break;

		case 0x0d:
			canWireless.WL_mode = WL_SCHEDULE_AB;
			canWireless.HpSyn = WL_HPSYN_OFF;
			//wlSlave.s_wlMode = WORKINGMODE_TTL;
			//canWireless.WL_statue = CANWL_STATUE_FLASH;
		break;
		
		case 0x0e:
			//canWireless.activeArr=1;
			canWireless.WL_mode = WL_SCHEDULE_RES;
			canWireless.HpSyn = WL_HPSYN_OFF;
			//wlSlave.s_wlMode = WORKINGMODE_TTL;	
			//canWireless.WL_statue = CANWL_STATUE_FLASH;		
		break;

		case 0x04:
			canWireless.WL_mode = WL_SCHEDULE_ALL;
			canWireless.HpSyn = WL_HPSYN_ON;
		break;

		case 0x05:
			canWireless.WL_mode = WL_SCHEDULE_AB;
			canWireless.HpSyn = WL_HPSYN_ON;
		break;

		case 0x06:
			canWireless.WL_mode = WL_SCHEDULE_RES;
			canWireless.HpSyn = WL_HPSYN_ON;
		break;

		default:
			UnexpectHandle();
		return;
	}

	switch(*wlSlave.s_array)
	{
		case WL_ARRARY_A:
			//CanExposeTimeCal(canWireless.WL_buf[1],0,CALC_FP_SWWL);
			CanExposeTimeCal(canWireless.WL_buf[1],canWireless.HpSyn);
			g_rfSlaveFlashCode = canWireless.WL_buf[1];
			canWireless.activeArr = 1;
		break;

		case WL_ARRARY_B:
			if(WL_SCHEDULE_ALL == canWireless.WL_mode)
			{
				//CanExposeTimeCal(canWireless.WL_buf[1],0,CALC_FP_SWWL);
				CanExposeTimeCal(canWireless.WL_buf[1],canWireless.HpSyn);
				g_rfSlaveFlashCode = canWireless.WL_buf[1];
			}else
			{
				//CanExposeTimeCal(canWireless.WL_buf[2],0,CALC_FP_SWWL);
				CanExposeTimeCal(canWireless.WL_buf[2],canWireless.HpSyn);
				g_rfSlaveFlashCode = canWireless.WL_buf[2];
			}
			canWireless.activeArr = 1;
		break;

		case WL_ARRARY_C:
			if(WL_SCHEDULE_ALL == canWireless.WL_mode)
			{
				//CanExposeTimeCal(canWireless.WL_buf[1],0,CALC_FP_SWWL);
				CanExposeTimeCal(canWireless.WL_buf[1],canWireless.HpSyn);
				g_rfSlaveFlashCode = canWireless.WL_buf[1];
				canWireless.activeArr = 1;
			}else if(WL_SCHEDULE_AB == canWireless.WL_mode)
			{
				//CanExposeTimeCal(0,0,CALC_FP_SWWL);
				canWireless.activeArr = 0;
			}else
			{
				//CanExposeTimeCal(canWireless.WL_buf[3],0,CALC_FP_SWWL);
				CanExposeTimeCal(canWireless.WL_buf[3],canWireless.HpSyn);
				g_rfSlaveFlashCode = canWireless.WL_buf[2];
				canWireless.activeArr = 1;
			}
		break;
		
	}

	wlSlave.s_wlMode = WORKINGMODE_TTL;
	if(!g_isIndividualSlave)
	{
		mainPara.workMode = WORKINGMODE_TTL;
	}
	//canWireless.activeArr = 1;
	canWireless.WL_statue = CANWL_STATUE_FLASH;	
	ENABLE_D30EN();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}

void LightSlaveTtlPreFlash(void)
{

	if(0x30==canWireless.WL_buf[0])
	{
		canWireless.activeArr = 1;
		canWireless.preArr = *wlSlave.s_array;
		canWireless.WL_statue = CANWL_STATUE_PREFLASH;
		ENABLE_D30EN();
		radioStatue.cmdSta = RADIO_STATUE_PREDAT;
		radioStatue.cmdReady = 1;
		OSTaskResume(CMDSEND_TASK_Prio);
	}else
	//else if(0x30==(canWireless.WL_buf[0]&0xf0))
	{
		canWireless.WL_statue = CANWL_STATUE_PREFLASH;
		
		switch(canWireless.WL_buf[0]&0x0f)
		{
			case 1:
			canWireless.preArr = WL_ARRARY_A;
			//if(WL_ARRARY_A==wlSet.s_arrary)
			//{
			//	canWireless.activeArr=1;
			//}
			break;

			case 2:
			canWireless.preArr = WL_ARRARY_B;
			//if(WL_ARRARY_B==wlSet.s_arrary)
			//{
			//	canWireless.activeArr=1;
			//}
			break;

			case 3:
			canWireless.preArr = WL_ARRARY_C;
			//if(WL_ARRARY_C==wlSet.s_arrary)
			//{
			//	canWireless.activeArr=1;
			//}
			break;

			default:
			UnexpectHandle();
			return;
			//break;
		}
		//if(mainPara.s_array == canWireless.preArr)
		if(*wlSlave.s_array==canWireless.preArr)
		{
			//ENABLE_D30EN();
			canWireless.activeArr=1;
			radioStatue.cmdSta = RADIO_STATUE_PREDAT;
			radioStatue.cmdReady = 1;
			OSTaskResume(CMDSEND_TASK_Prio);
		}
	}
}

void UnexpectHandle(void)
{

	canWireless.WL_Channel = WL_CHANNEL_UNSURE;
	canWireless.WL_statue = CANWL_STATUE_HOLD;
	//wlSlave.s_flashStatue = WL_FLASH_STATUE_HOLD;
	wlSlave.s_wlMode = WORKINGMODE_OFF;
	canWireless.preArr = WL_ARRARY_NULL;
	//canWireless.bitLoadIndex = 0;
	//canWireless.WLBufIndex = 0;
	canWireless.activeArr = 0;
	//WL_PARA_RESET();
	CLR_WLC_FLASH_TIMEOUT;
	CLR_WLC_BUF();
	StopCapture();
	TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
}

void ParseMstCmd(void)
{
//	char temp=0;
//	unsigned char intermedia;
//	unsigned char wlmHead;
//	unsigned char bitCalcIndex=0;
#ifdef SLAVE_TEST
	char i = 0;
#endif

	canWireless.WLBufIndex++;
	canWireless.bitLoadIndex = 0;

#ifdef SLAVE_TEST
	for(i=0;i<canWireless.WLBufIndex;i++)
	{
		DBG_PRINTF("%x ",canWireless.WL_buf[i]);
	}
	DBG_PRINTF("\r\n");
#endif

#ifdef TTL_DEBUG
	SET_UPDATE_EVT(DSP_UPDATE_LITETEST);
	return;
#endif

#if 0
	switch(canWireless.WL_buf[0])
	{
		case 0x0f:
			LightSlaveManu();
		break;

		case 0x9f:
		case 0xaf:
		case 0xbf:
			LightSlaveMulti();
		break;

		case 0xef:
			LightSlavePilot();
		break;

		default:
			switch()
		break;
	}
#endif

#if 1
	switch(canWireless.WL_buf[0]&0x0f)	
	//if(0x0f==canWireless.WL_buf[0])				//Mode M
	{
		case 0x0f:	
			switch(canWireless.WL_buf[0]&0xf0)
			{
				case 0x00:
				LightSlaveManu();
				break;
				
				case 0x90:
				case 0xa0:
				case 0xb0:
				LightSlaveMulti();
				break;

				case 0xe0:
				LightSlaveTtlPilot();
				break;

				default:
					UnexpectHandle();
				break;
			}
		break;

		case 0x07:
			LightSlaveManu();
		break;

		case 0x0d:
		case 0x0c:
		case 0x0e:
			LightSlaveTtl();
		break;
		
		case 0x04:
		case 0x05:
		case 0x06:
			LightSlaveTtl();
		break;

		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
			if(0x30==(canWireless.WL_buf[0]&0xf0))
			{
				LightSlaveTtlPreFlash();
			}else
			{
				//FLAOK_TOGGLE();
				UnexpectHandle();
			}
		break;

		default:
			//FLAOK_TOGGLE();
			UnexpectHandle();
		break;

		//canWireless.activeArr=1;
	}
	//canWireless.WL_statue = CANWL_STATUE_HOLD;
#else

	for(temp=0;temp<canWireless.WLBufIndex;temp++)
	{
		printf("%x ",canWireless.WL_buf[temp]);
	}
	printf("\r\n");
	CLR_WLC_BUF;
	canWireless.WLBufIndex = 0;
	canWireless.bitLoadIndex = 0;
	canWireless.WL_statue = CANWL_STATUE_HOLD;
#endif

}

void IIcDelay()
{
	u8 i;
	for(i=0;i<2;i++);
	
}

#define 	hp_I2C_DELAY()				IIcDelay()//Delay()
//#define   hp_IICSDA_OUT()              	{GPIOE->CRL &= 0XFFFFFF0F;GPIOE->CRL |= 0X00000030;}
//#define   hp_IICSDA_IN()               		{GPIOE->CRL &= 0XFFFFFF0F;GPIOE->CRL |= 0X00000080;GPIOE->ODR|=0X02;}
#define   hp_IICSDA_OUT()              	{GPIOE->CRL &= 0XFFFFFFF0;GPIOE->CRL |= 0X00000003;}
#define   hp_IICSDA_IN()               		{GPIOE->CRL &= 0XFFFFFFF0;GPIOE->CRL |= 0X00000008;GPIOE->ODR|=0X01;}

unsigned char hp_IIcAck()
{
	unsigned char ack;
	hp_IICSDA_IN();
	HP_I2C_SCK=1;
	IIcDelay();
	ack = HP_I2C_SDA;
	HP_I2C_SCK=0;
	return ack;
}

unsigned char hp_IIcStart()
{
	//P0DIR |= 0x18;		//p03,p04 set as I2C port
	hp_IICSDA_OUT();
	HP_I2C_SDA=1;
	HP_I2C_SCK=1;
	hp_I2C_DELAY();
	hp_I2C_DELAY();
	HP_I2C_SDA=0;
	HP_I2C_SCK=0;
	hp_I2C_DELAY();

	//return IIcAck();
	//ack	
	return NULL;
}

void hp_IIcStop()
{
	hp_IICSDA_OUT();
	HP_I2C_SCK=0;
	hp_I2C_DELAY();
	HP_I2C_SDA=0;
	hp_I2C_DELAY();
	HP_I2C_SCK=1;
	hp_I2C_DELAY();
	HP_I2C_SDA=1;	
}

unsigned char hp_IicSendByte(unsigned char dat)
{
	//P0DIR |= 0x18;		//p03,p04 set as I2C port
	unsigned char i;
	unsigned char ack;
	hp_IICSDA_OUT();

	for(i=0;i<8;i++)
	{
		HP_I2C_SCK=0;
		hp_I2C_DELAY();
		if(dat&0x80)
		{
			HP_I2C_SDA=1;
		}else
		{
			HP_I2C_SDA=0;
		}
		hp_I2C_DELAY();
		HP_I2C_SCK=1;
		hp_I2C_DELAY();
		dat <<=1;
		hp_I2C_DELAY();		
	}
	
	HP_I2C_SCK=0;
	HP_I2C_SDA=0;
	//IICSDA_IN;
	//I2C_DELAY;
	//HP_I2C_SCK=1;
	//ack=HP_I2C_SDA;
	//HP_I2C_SCK=0;
	ack = hp_IIcAck();

	return ack;
	
}

void HpIicSendByte(unsigned char dat)
{
	//IIcStart();
	hp_IicSendByte(dat);
	//IIcStop();
	
}

#if 0
void HpExpCodeSend(unsigned int hpv)
{
	hp_IIcStart();
	//HpIicSendByte(0xc0);
	//HpIicSendByte(0xc2);	//AJNN
	HpIicSendByte(0xc4);		//APNN
	HpIicSendByte(0x60);
	HpIicSendByte((hpv>>8)&0xff);
	HpIicSendByte(hpv&0xff);
	hp_IIcStop();
}
#else
/*fast mode: this made flash command take effort immediately, and 
make the flash command mode correctly
*/
void HpExpCodeSend(unsigned int hpv)
{
	hp_IIcStart();
	//HpIicSendByte(0xc0);
	//HpIicSendByte(0xc2);	//AJNN
	HpIicSendByte(0xc4);		//APNN
	HpIicSendByte((hpv>>12)&0x0f);
	HpIicSendByte((hpv>>4)&0xff);
	//HpIicSendByte(hpv&0xff);
	hp_IIcStop();
}
#endif


unsigned int CanOutputSwitch(s8 value1,s8 value2)
{
	//char value1, value2;
	//char surtmp1=0,surtmp2=0;
	unsigned int TMR2=0;

	//if((value1<=7)&&(value2<=2))
	if(value1<=7)
	{
		//TMR2 = g_expTime[(value1+2)*3+value2];
		TMR2 = g_expTime[(value1+3)*3+value2];
	}else
	{
		TMR2 = OUTPUT_80;
	}

	return TMR2;
	
}




