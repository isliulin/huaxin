#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "includes.h"	 
#include "../HARDWARE/spi/spi.h"   
#include "Nikon.h"
#include "../TIMER/timer.h"

extern u8  fac_us;
extern u16 fac_ms;
NikWireless nikWireless;
unsigned int pTime;
u8 ttl_pre2_flag = 0;
extern Wl_set wlSet;
u8 g_signalFlash=0;
u8 g_buzzerOn = 0;
u8 g_slaveBzEn = 0;
u8 g_slaveBzCnt = 0;
extern u8 g_rptPilotFlag;
extern unsigned char const iso_code[];
unsigned char wl_preIndex=0;
u16 g_wtch_wt1=0,g_wtch_wt2=0;
#ifdef SLAVE_DEBUG
extern u8 g_mstDgbPrt;
#endif

#define MANU_CODE_MAX 7
//extern u8 g_slaveStart;

#define SET_WL_FLASH_TIMEOUT()
void LightSlaveParaClear(void);
unsigned int OutputSwitch(s8 value1,s8 value2);
extern void SlaveSetPre2Timer(void);
extern void SlaveStopPre2Timer(void);
void SlaveRcvStateRst(void);
void WlmFlaADSet(s8 para1, s8 para2);
void RptFlashPrepear(u8 hz, u8 tms);
void SetRptFlashTimer(u16 ticks);
u8 SlaveAutoFlash(void);
//u8 ExtmModSet(signed char  foc, u16 iso, u16 apert);
u8 ExtmModSet(signed char  foc, u16 iso, u16 apert,s8 cmp1, s8 cmp2);
extern OS_TMR  *g_rptFlashTmr;

extern char GetIsoStdIndex(const u32* isoArr, char size, u32 cd);

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


const u16 g_flaExpCode[] = {FLA_TTLTRA0,FLA_TTLTRA1,FLA_TTLTRA2,
						FLA_TTLTRA3,	FLA_TTLTRA4,	FLA_TTLTRA5,FLA_EXP_00,FLA_EXP_01,FLA_EXP_02,
						FLA_EXP_10,FLA_EXP_11,FLA_EXP_12,FLA_EXP_20,FLA_EXP_21,FLA_EXP_22,
						FLA_EXP_30,FLA_EXP_31,FLA_EXP_32,FLA_EXP_40,FLA_EXP_41,FLA_EXP_42,
						FLA_EXP_50,FLA_EXP_51,FLA_EXP_52,FLA_EXP_60,FLA_EXP_61,FLA_EXP_62,
						FLA_EXP_70,FLA_EXP_71,FLA_EXP_72,FLA_EXP_80};

void LightSlaveProc(u16 plusWidth)
{
	//static u16 WL_chan1=0,WL_chan2=0;
	//static unsigned char wl_preIndex=0;
	u8 bits=0;

	//if((NIKWL_STATUE_HOLD!=nikWireless.WL_statue)&&(plusWidth<30))
// the timer thorhold is 1700us, So before take flash the time pluswidth is unsure, it is occationally have no flash
/*
	if((NIKWL_STATUE_HOLD!=nikWireless.WL_statue)&&(NIKWL_STATUE_PREFLASH!=nikWireless.WL_statue))
	{
		if(plusWidth<30)
		{
			return;
		}
	}
*/
	
	if(NIKWL_STATUE_HOLD==nikWireless.WL_statue)
	{
		
		nikWireless.WL_statue = NIKWL_STATUE_ENSURECHAN;

		SET_WL_FLASH_TIMEOUT();

	}else if(NIKWL_STATUE_ENSURECHAN==nikWireless.WL_statue)
	{
		if(!nikWireless.chFlag)
		{
			g_wtch_wt1 = plusWidth;
			nikWireless.chFlag++;			
		}else
		{
			nikWireless.chFlag = 0;

			if(WL_PFCOUNT_TWO == nikWireless.preTurn)
			{
				SlaveStopPre2Timer();
				nikWireless.preTurn = WL_PFCOUNT_RDY;
			}
			
			g_wtch_wt2 = plusWidth;			//here handle preflash2

			nikWireless.WL_Channel = 0x0f;

			if((WL_CHAN_SCALE1_GND<g_wtch_wt1)&&(g_wtch_wt1<WL_CHAN_SCALE1_ROOF))
			{
				if((WL_CHAN_SCALE3_GND<g_wtch_wt2)&&(g_wtch_wt2<WL_CHAN_SCALE3_ROOF))
				{
					nikWireless.WL_Channel = WL_CHANNEL_ONE;
				}else if((WL_CHAN_SCALE4_GND<g_wtch_wt2)&&(g_wtch_wt2<WL_CHAN_SCALE4_ROOF))
				{
					nikWireless.WL_Channel = WL_CHANNEL_FOUR;
				}

			}else if((WL_CHAN_SCALE2_GND<g_wtch_wt1)&&(g_wtch_wt1<WL_CHAN_SCALE2_ROOF))
			{
				if((WL_CHAN_SCALE1_GND<g_wtch_wt2)&&(g_wtch_wt2<WL_CHAN_SCALE1_ROOF))
				{
					nikWireless.WL_Channel = WL_CHANNEL_TWO;
				}else if((WL_CHAN_SCALE3_GND<g_wtch_wt2)&&(g_wtch_wt2<WL_CHAN_SCALE3_ROOF))
				{
					nikWireless.WL_Channel = WL_CHANNEL_THREE;
				}
			}
/*
			if(0x0f==nikWireless.WL_Channel)
			{
				LightSlaveParaClear();
				
			}else
			{
				nikWireless.WL_statue = NIKWL_STATUE_RUN;

				//if(nikWireless.s_flashStatue ==WL_FLASH_STATUE_HOLD)
				//{
				//	nikWireless.s_flashStatue = WL_FLASH_STATUE_START;
				//}

				nikWireless.bitLoadIndex = 0;
				nikWireless.WLBufIndex = 0;

				WL_chan1 = 0;

				WL_chan2 = 0;
			}
*/
			nikWireless.WL_statue = NIKWL_STATUE_RUN;

			nikWireless.bitLoadIndex = 0;
			nikWireless.WLBufIndex = 0;

			//g_wtch_wt1 = 0;
			//g_wtch_wt2 = 0;
			
		}
	}else if(NIKWL_STATUE_RUN==nikWireless.WL_statue)
	{
			bits = (plusWidth+WLN_DATA_STD)/150;
		
			//if(bits<0)
			if(!bits)
			{
				LightSlaveParaClear();	   //unexpected
				return;
			}

			if(nikWireless.bitLoadIndex+bits>8)
			{
				nikWireless.WLBufIndex++;
				nikWireless.bitLoadIndex = (nikWireless.bitLoadIndex+bits)-8;
				nikWireless.WL_buf[nikWireless.WLBufIndex] |=(0x80>>(nikWireless.bitLoadIndex-1));				
			}else
			{
				nikWireless.bitLoadIndex+=bits;
				nikWireless.WL_buf[nikWireless.WLBufIndex] |=(0x80>>(nikWireless.bitLoadIndex-1));
			}

			if(nikWireless.WLBufIndex>=WLBUF_LEN)
			{
				LightSlaveParaClear();	   //unexpected
				return;
			}
	}else if(NIKWL_STATUE_PREFLASH==nikWireless.WL_statue)
	{
	
		if(nikWireless.preTurn == WL_PFCOUNT_ONE)
		{
			if(!wl_preIndex)
			{
				wl_preIndex = 1;
			}else
			{
				wl_preIndex = 0;
				if((WL_PREWIDTH_GND<=plusWidth)&&(plusWidth<=WL_PREWIDTH_ROOF))
				{
					//SlaveSetPreflash();
					SlaveSetPre2Timer();
					//nikWireless.preTurn = WL_PFCOUNT_TWO;
				}else
				{
					LightSlaveParaClear();
				}
			}
			
		}else if(nikWireless.preTurn==WL_PFCOUNT_TWO)
		{
			
			if(!wl_preIndex)
			{
				wl_preIndex = 1;
			}else
			{
				wl_preIndex = 0;
				g_wtch_wt1 = plusWidth;
//in this point, it is need to judge whether the next occured is a secondly previous flash or a next communication
//the handle method is start the timer0, if the timer0 overflows, it prove to be a previous flash, otherwise, record the
//plus width as the channel judger.
				nikWireless.WL_statue = NIKWL_STATUE_ENSURECHAN;
				//nikWireless.WL_Channel = WL_CHANNEL_MAKESURE;
				nikWireless.chFlag = 1;
				nikWireless.WLBufIndex = 0;
				nikWireless.bitLoadIndex = 0;
				SlaveSetPre2Timer();

			}
		}
	}else if(NIKWL_STATUE_FLASH==nikWireless.WL_statue)
	{
//#ifdef SLAVE_DEBUG
//	DBG_PRINTF("INT\r\n");
//#endif
		SlaveSetFlash();
	}
}

#define WLS_TTL_NRBASE 0xa0//0xa8
#define WLS_TTL_HPBASE 0x86
void WLS_ExposeTimeCal(const unsigned char i_camCode,unsigned char hp)
{
	unsigned char camCode = 0;
	
	if(LOW_SPEED==hp)
	{
		if(i_camCode>WLS_TTL_NRBASE)
		{
			camCode = (i_camCode - WLS_TTL_NRBASE)>>1;
		}else
		{
			camCode = 0;
		}
		
		g_expTTLPara1 = camCode/3 - 1;
		g_expTTLPara2 = camCode%3;
	}else
	{
		camCode = i_camCode + 18;

		if(camCode>0xbf)
		{
			//camCode = (camCode&0x7f);
			camCode &= 0x7f;
		}

		if(camCode>WLS_TTL_HPBASE)
		{
			camCode = (camCode - WLS_TTL_HPBASE)>>1;
		}else
		{
			camCode = 0;
		}

		//g_expTTLPara1 = camCode/3 + 1;
		g_expTTLPara1 = camCode/3;
		g_expTTLPara2 = camCode%3;
		
	}
	if(g_expTTLPara1>=7)
	{
		g_expTTLPara1 = 7;
		g_expTTLPara2 = 0;
	}
	//DBG_PRINTF("TTL %d,%d\r\n",g_expTTLPara1, g_expTTLPara2);
	
}

#if 0
#define TTL_VAL_MIN 0x32//0x26//0x23//0x1d//0x32//0x54
#define TTL_VAL_MIN_HP 0x5A//0x57//0x66//0x4E//0x60
#define VAL_STEP 4//6(When 0.5Ev step)
#define TTL_VAL_MAX (TTL_VAL_MIN+(9*3*VAL_STEP))
#define TTL_VAL_MAX_HP (TTL_VAL_MIN_HP+(7*3*VAL_STEP))

void ExposeTimeCal(const unsigned char i_camCode,unsigned char hp,s8 cmp1, s8 cmp2)
{
	unsigned char camCode;
//	signed char tmp = 0;
	u8 i = 0;
	s8 cmpTmp = 0;
	//camCode = i_camCode - 8;
	camCode = i_camCode;

	cmpTmp = (cmp1*3+cmp2)*VAL_STEP;

	if(cmpTmp>0)
	{
		if(((u16)cmpTmp+(u16)camCode)<TTL_VAL_MAX)
		{
			camCode+=cmpTmp;
		}else
		{
			camCode = TTL_VAL_MAX;
		}
	}else
	{
		if(camCode>((-1)*cmpTmp))
		{
			camCode+=cmpTmp;
		}else
		{
			camCode = 0;
		}
	}
	
	g_isFlashWeak = 0;
	if(LOW_SPEED==hp)
	{
		//camCode = i_camCode - 8;

		//camCode-=(cmrPara.isoIndex*2*3);
		camCode -= (cmrPara.isoIndex*VAL_STEP);

		//i = GetMotoStdIndex(*motoSet.dest_location);
		i = GetMotoIncremental(GetMotoStdIndex(*motoSet.dest_location));

		//camCode-=(i*2*3);
		camCode -= (i*VAL_STEP);

		if(camCode<TTL_VAL_MIN)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_VAL_MAX)
		{
			camCode = (camCode-TTL_VAL_MIN)/VAL_STEP;
			g_expTTLPara1 = (camCode)/3-2;
			g_expTTLPara2 = (camCode)%3;
		}else
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
			g_isFlashWeak = 1;
			g_FlashWeakLevels = (camCode-TTL_VAL_MAX)/VAL_STEP;
		}
/*
		if(camCode<TTL_TIMECAL_POINT1)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT2)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT3)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT4)
		{
			g_expTTLPara1 = -1;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT5)
		{
			g_expTTLPara1 = -1;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT6)
		{
			g_expTTLPara1 = -1;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT7)
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT8)
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT9)
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT10)
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT11)
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT12)
		{
			g_expTTLPara1 = 1;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT13)
		{
			g_expTTLPara1 = 2;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT14)
		{
			g_expTTLPara1 = 2;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT15)
		{
			g_expTTLPara1 = 2;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT16)
		{
			g_expTTLPara1 = 3;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT17)
		{
			g_expTTLPara1 = 3;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT18)
		{
			g_expTTLPara1 = 3;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT19)
		{
			g_expTTLPara1 = 4;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT20)
		{
			g_expTTLPara1 = 4;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT21)
		{
			g_expTTLPara1 = 4;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT22)
		{
			g_expTTLPara1 = 5;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT23)
		{
			g_expTTLPara1 = 5;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT24)
		{
			g_expTTLPara1 = 5;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT25)
		{
			g_expTTLPara1 = 6;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_TIMECAL_POINT26)
		{
			g_expTTLPara1 = 6;
			g_expTTLPara2 = 1;
		}else if(camCode<TTL_TIMECAL_POINT27)
		{
			g_expTTLPara1 = 6;
			g_expTTLPara2 = 2;
		}else if(camCode<TTL_TIMECAL_POINT28)
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
		}else
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
			g_isFlashWeak = 1;
			g_FlashWeakLevels = (camCode - TTL_TIMECAL_POINT28)/4;
		}
*/
/*
		tmp = g_expTTLPara1*3 + g_expTTLPara2;
		tmp -= cmrPara.isoIndex;
		i = GetMotoStdIndex(*motoSet.dest_location);
		if(i>2)
		{
			i = ((i-2)/2);
			tmp -= i;
		}

		if(tmp<-6)
		{
			tmp = -6;
		}else if(tmp>21)
		{
			tmp = 21;
		}

		g_expTTLPara1 = tmp/3;
		g_expTTLPara2 = tmp%3;
*/
	}else
	{
		//camCode = i_camCode;

		if(cmrPara.pShutter>0x30)
		{
			//camCode += (((cmrPara.pShutter - 0x30)/3)*2);
			camCode += (cmrPara.pShutter - 0x30);
		}
/*
		if(camCode>(cmrPara.isoIndex*2*3))
		{
			camCode -= (cmrPara.isoIndex*2*3);
		}else
		{
			camCode = 0;
		}
*/
		if(camCode>(cmrPara.isoIndex*VAL_STEP))
		{
			camCode += (cmrPara.isoIndex*VAL_STEP);
		}

		if(camCode>=TTL_VAL_MIN_HP)
		{
			camCode = (camCode - TTL_VAL_MIN_HP)/VAL_STEP;
		}else
		{
			camCode = 0;
		}
		
		g_expTTLPara1 = camCode/3;
		g_expTTLPara2 = camCode%3;

		if(g_expTTLPara1>=7)
		{
			g_isFlashWeak = 1;
			g_FlashWeakLevels = (g_expTTLPara1*3)+g_expTTLPara2-21;
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
		}
		
	}

	//i = CodeSearch((char*)iso_code, sizeof(iso_code), (char)cmrPara.pIso);
	//i = CodeSearch((char*)iso_code, sizeof(iso_code), receive_arrary[RCVDAT_LOCATION_ISO]);
/*
	tmp = g_expTTLPara1*3 + g_expTTLPara2;
	tmp -= cmrPara.isoIndex;
	if(tmp < 0)
	{
		tmp = 0;
	}

	g_expTTLPara1 = tmp/3;
	g_expTTLPara2 = tmp%3;
*/	
}
#else 

#define TTL_VAL_MIN 0x2A//0x32//0x26//0x23//0x1d//0x32//0x54
#define TTL_VAL_MIN_HP 0x52//0x5A//0x57//0x66//0x4E//0x60
#define VAL_STEP 4//6(When 0.5Ev step)
#define TTL_VAL_MAX (TTL_VAL_MIN+(9*3*VAL_STEP))
#define TTL_VAL_MAX_HP (TTL_VAL_MIN_HP+(7*3*VAL_STEP))

void ExposeTimeCal(const unsigned char i_camCode,unsigned char hp,s8 cmp1, s8 cmp2)
{
	u16 camCode;
//	signed char tmp = 0;
//	u8 i = 0;
//	s8 cmpTmp = 0;
	u8 isoIndex = 0;
	//camCode = i_camCode - 8;
	camCode = i_camCode;

	if(ttl_pre2_flag > 0)
	{
		camCode += (6*VAL_STEP);
	}

	//cmrPara.isoIndex = GetParaStdIndex(isoValue, 35, cmrPara.iso);
	if(cmrPara.iso<50)
	{
		isoIndex = 0;
	}else
	{
		isoIndex = (GetIsoStdIndex(isoValue, 58, cmrPara.iso) - 12);          // sub iso50 index
	}

/*
	cmpTmp = (cmp1*3+cmp2)*VAL_STEP;

	if(cmpTmp>0)
	{
		if(((u16)cmpTmp+(u16)camCode)<TTL_VAL_MAX)
		{
			camCode+=cmpTmp;
		}else
		{
			camCode = TTL_VAL_MAX;
		}
	}else
	{
		if(camCode>((-1)*cmpTmp))
		{
			camCode+=cmpTmp;
		}else
		{
			camCode = 0;
		}
	}
*/
	
	g_isFlashWeak = 0;
	if(LOW_SPEED==hp)
	{

		//camCode -= (cmrPara.isoIndex*VAL_STEP);
		/*
		if (camCode > (cmrPara.isoIndex*VAL_STEP))
		{
			camCode -= (cmrPara.isoIndex*VAL_STEP);
		}else
		{
			camCode = 0;
		}
		*/

		if (camCode > (isoIndex*VAL_STEP))
		{
			camCode -= (isoIndex*VAL_STEP);
		}else
		{
			camCode = 0;
		}

/*
		i = GetMotoIncremental(GetMotoStdIndex(*motoSet.dest_location));

		if (camCode > (i*VAL_STEP))
		{
			camCode -= (i*VAL_STEP);
		}else
		{
			camCode = 0;
		}
*/
/*
		cmpTmp = (cmp1*3+cmp2)*VAL_STEP;

		if(cmpTmp>0)
		{
			if(((u16)cmpTmp+(u16)camCode)<TTL_VAL_MAX)
			{
				camCode+=cmpTmp;
			}else
			{
				camCode = TTL_VAL_MAX;
			}
		}else
		{
			if(camCode>((-1)*cmpTmp))
			{
				camCode+=cmpTmp;
			}else
			{
				camCode = 0;
			}
		}		
*/

		if(camCode<TTL_VAL_MIN)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 0;
		}else if(camCode<TTL_VAL_MAX)
		{
			camCode = (camCode-TTL_VAL_MIN)/VAL_STEP;
			g_expTTLPara1 = (camCode)/3-2;
			g_expTTLPara2 = (camCode)%3;
		}else
		{
			g_expTTLPara1 = 7 + ((camCode - TTL_VAL_MAX)/3);
			g_expTTLPara2 = 0 + ((camCode - TTL_VAL_MAX)%3);
			//g_isFlashWeak = 1;
			//g_FlashWeakLevels = (camCode-TTL_VAL_MAX)/VAL_STEP;
		}

	}else
	{
		//camCode = i_camCode;

		if(cmrPara.pShutter>0x30)
		{
			//camCode += (((cmrPara.pShutter - 0x30)/3)*2);
			camCode += (cmrPara.pShutter - 0x30);
		}
/*
		if(camCode>(cmrPara.isoIndex*VAL_STEP))
		{
			camCode += (cmrPara.isoIndex*VAL_STEP);
		}
*/

		if (camCode > (isoIndex*VAL_STEP))
		{
			camCode -= (isoIndex*VAL_STEP);
		}else
		{
			camCode = 0;
		}
/*
		cmpTmp = (cmp1*3+cmp2)*VAL_STEP;

		if(cmpTmp>0)
		{
			if(((u16)cmpTmp+(u16)camCode)<TTL_VAL_MAX_HP)
			{
				camCode+=cmpTmp;
			}else
			{
				camCode = TTL_VAL_MAX_HP;
			}
		}else
		{
			if(camCode>((-1)*cmpTmp))
			{
				camCode+=cmpTmp;
			}else
			{
				camCode = 0;
			}
		}
*/

		if(camCode>=TTL_VAL_MIN_HP)
		{
			camCode = (camCode - TTL_VAL_MIN_HP)/VAL_STEP;
		}else
		{
			camCode = 0;
		}
		
		g_expTTLPara1 = camCode/3;
		g_expTTLPara2 = camCode%3;

		//if(g_expTTLPara1>=7)
		//{
			//g_isFlashWeak = 1;
			//g_FlashWeakLevels = (g_expTTLPara1*3)+g_expTTLPara2-21;
			//g_expTTLPara1 = 7;
			//g_expTTLPara2 = 0;
		//}
		
	}

	g_expTTLPara1 += cmp1;
	g_expTTLPara2 += cmp2;

	if(g_expTTLPara2<0)
	{
		g_expTTLPara2+=3;
		g_expTTLPara1-=1;
	}

	if(LOW_SPEED==hp)
	{
		if(g_expTTLPara1<-2)
		{
			g_expTTLPara1 = -2;
			g_expTTLPara2 = 0;
		}
	}else
	{
		if(g_expTTLPara1<0)
		{
			g_expTTLPara1 = 0;
			g_expTTLPara2 = 0;
		}
	}

	if(g_expTTLPara1>=7)
	{
		g_isFlashWeak = 1;
		g_FlashWeakLevels = (g_expTTLPara1*3)+g_expTTLPara2-21;
		g_expTTLPara1 = 7;
		g_expTTLPara2 = 0;
	}

}


#endif

void Nik_AutoShotSet(unsigned char rcvCmd)
{
	u16 flaCode = 0;
	if(rcvCmd<0xC0)
	{
		flaCode = rcvCmd-0x80+0x100;
	}else
	{
		flaCode = rcvCmd;
	}

	if(flaCode<0xeb)
	{
		flaCode = 0xeb;
	}
	g_expTTLPara1 = (flaCode - 0xeb)/2;
}

unsigned char SlaveModeConvert(u8 code)
{
	switch(code)
	{
		case 0x0b:
		return WORKINGMODE_MANU;

		case 0x08:
		return WORKINGMODE_OFF;

		case 0x09:
		return WORKINGMODE_TTL;

		case 0x0a:
		return WORKINGMODE_AUTO;

		default:
		return WORKINGMODE_OFF;
	}
}

void SlaveFlashParaSet(u8 mod,u8 opCode)
{
	switch(mod)
	{
		case WORKINGMODE_TTL:
			//nikWireless.s_wlMode = WORKINGMODE_TTL;
			//WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.hpSyn);
			WLS_ExposeTimeCal(opCode,nikWireless.hpSyn);
		break;

		case WORKINGMODE_MANU:
			//nikWireless.s_wlMode = WORKINGMODE_MANU;
			//nikWireless.WL_output1 = WL_CVT_EXP1(nikWireless.WL_opCode);
			//nikWireless.WL_output2 = WL_CVT_EXP2(nikWireless.WL_opCode);
			nikWireless.WL_output1 = WL_CVT_EXP1(opCode);
			nikWireless.WL_output2 = WL_CVT_EXP2(opCode);
			if(nikWireless.WL_output1>7)
			{
				nikWireless.WL_output1 = 3;
			}
			if(nikWireless.WL_output2>2)
			{
				nikWireless.WL_output2 = 0;
			}
		break;

		case WORKINGMODE_AUTO:
			//nikWireless.s_wlMode = WORKINGMODE_AUTO;
			//Nik_AutoShotSet(nikWireless.WL_opCode);
			Nik_AutoShotSet(opCode);
		break;

		case WORKINGMODE_RPT:
			nikWireless.rpt_output1 = WL_CVT_EXP1(opCode);
			nikWireless.rpt_output2 = WL_CVT_EXP2(opCode);
			if(nikWireless.rpt_output1>4)
			{
				nikWireless.rpt_output1 = 4;
			}
			if(nikWireless.rpt_output2>2)
			{
				nikWireless.rpt_output2 = 0;
			}
		break;

		default:
			//nikWireless.s_wlMode = WORKINGMODE_OFF;
		break;
	}
}

void SlaveGetFlashCode(void)
{
	switch(mstPara->rmt_gr)
	{
		case 0:
			nikWireless.WL_opCode= ((nikWireless.WL_buf[0]<<4) | (nikWireless.WL_buf[1]>>4));
		break;

		case 1:
			nikWireless.WL_opCode= ((nikWireless.WL_buf[1]<<4) | (nikWireless.WL_buf[2]>>4));
		break;

		case 2:
			nikWireless.WL_opCode= ((nikWireless.WL_buf[2]<<4) | (nikWireless.WL_buf[3]>>4));
		break;
	}

	nikWireless.rearReserve = nikWireless.WL_opCode;
}

void SlaveSetPre2Timer(void)
{
	Timer4_Init(470,72);	
	g_tmHandleFlag |= TM_HANDLE_SLAVE_PRE2;
}

void SlaveStopPre2Timer(void)
{
	Timer4_Stop();
	g_tmHandleFlag &= ~TM_HANDLE_SLAVE_PRE2;
}


#if 0
void SlaveStartProc(void)
{
	u16 tmp = 0;
	
	if((nikWireless.WL_buf[0]==WL_REARFLASH_COMMAND)||(nikWireless.WL_buf[0]==WL_REARFLASH_COMMANDex))
	{
		if(WORKINGMODE_TTL==nikWireless.s_wlMode)
		{
			WLS_ExposeTimeCal(nikWireless.rearReserve,nikWireless.hpSyn);
		}else if(WORKINGMODE_AUTO==nikWireless.s_wlMode)
		{
			Nik_AutoShotSet(nikWireless.WL_opCode);
		}
		pTime = OutputSwitch(WL_CVT_EXP1(nikWireless.WL_opCode), WL_CVT_EXP1(nikWireless.WL_opCode));
		SlaveSetFlash();
	}else if(BLFLAG_READY_FRONT==(nikWireless.WL_buf[0]&0xf0))
	{
		nikWireless.hpSyn = 0;
		nikWireless.WL_opCode = (nikWireless.WL_buf[mstPara->rmt_gr]<<4)|(nikWireless.WL_buf[mstPara->rmt_gr+1]>>4);
		if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
		{
			SlaveFlashParaSet(nikWireless.preflashLockModeReserve[mstPara->rmt_gr+1]);
		}else
		{
			nikWireless.s_wlMode = WORKINGMODE_OFF;
		}
		nikWireless.WL_statue = NIKWL_STATUE_FLASH;
		nikWireless.bitLoadIndex = 0;
		nikWireless.WLBufIndex = 0;
	}else if((nikWireless.WL_buf[0]>=0xd8)&&(nikWireless.WL_buf[0]<=0xdc))
	{
		if(0xdb==nikWireless.WL_buf[0])
		{
			nikWireless.hpLevel = HIGH_SPEED_LEVEL2;
		}else
		{
			nikWireless.hpLevel = HIGH_SPEED_LEVEL1;
		}

		nikWireless.WL_opCode = nikWireless.WL_buf[mstPara->rmt_gr+1];
		
		if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
		{
			SlaveFlashParaSet(nikWireless.preflashLockModeReserve[mstPara->rmt_gr+1]);
		}else
		{
			nikWireless.s_wlMode = WORKINGMODE_OFF;
		}
		nikWireless.WL_statue = NIKWL_STATUE_FLASH;
		nikWireless.bitLoadIndex = 0;
		nikWireless.WLBufIndex = 0;		
	}else if(WL_PILOT_CMD==nikWireless.WL_buf[0])
	{
		switch(mstPara->rmt_gr)
		{
			case 0:
			break;

			case 1:
			break;

			case 2:
			break;
		}
	}else if(BLFLAG_START==(nikWireless.WL_buf[0]&0xf0))
	{
		tmp = MAKEWORD(nikWireless.WL_buf[1], nikWireless.WL_buf[0]);
		nikWireless.s_wlMode = SlaveModeConvert((tmp>>(8-mstPara->rmt_gr*4))&0x0f);
		nikWireless.preflashLockModeReserve[0] = SlaveModeConvert(nikWireless.WL_buf[0]&0x0f);
		nikWireless.preflashLockModeReserve[1] = SlaveModeConvert((nikWireless.WL_buf[1]>>4)&0x0f);
		nikWireless.preflashLockModeReserve[2] = SlaveModeConvert(nikWireless.WL_buf[1]&0x0f);

		if(((nikWireless.WL_buf[0]&0x0f)==0x09)||((nikWireless.WL_buf[1]&0xf0)==0x90)||((nikWireless.WL_buf[1]&0x0f)==0x09)\
				||((nikWireless.WL_buf[0]&0x0f)==0x0a)||((nikWireless.WL_buf[1]&0xf0)==0xa0)||((nikWireless.WL_buf[1]&0x0f)==0x0a))
		{
			nikWireless.preTotal = 0;
			
			if(((nikWireless.WL_buf[0]&0x0f)==0x09)||((nikWireless.WL_buf[0]&0x0f)==0x0a))
			{
				nikWireless.preTotal ++;
			}
			if(((nikWireless.WL_buf[1]&0xf0)==0x90)||((nikWireless.WL_buf[1]&0xf0)==0xa0))
			{
				nikWireless.preTotal ++;
			}
			if(((nikWireless.WL_buf[1]&0x0f)==0x09)||((nikWireless.WL_buf[1]&0x0f)==0x0a))
			{
				nikWireless.preTotal ++;
			}
			//SET_WL_FLASH_TIMEOUT
			nikWireless.s_flashStatue = WL_FLASH_STATUE_BFRPF;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			//nikWireless.WLBufIndex= 0;
			nikWireless.preflashLock = PREFLASH_LOCK_OFF;
			CLR_WLN_BUF();
				
		}else
		{
			nikWireless.s_flashStatue = WL_FLASH_STATUE_AFTPF;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			nikWireless.WLBufIndex= 0;
			CLR_WLN_BUF();
		}
	}
	else
	{
		LightSlaveParaClear();
	}

	
}

void SlaveBeforePreFlashProc(void)
{
	if(nikWireless.preTurn == WL_PFCOUNT_TWO)
	{
		nikWireless.preTurn = WL_PFCOUNT_RDY;
		if(nikWireless.preArr==mstPara->rmt_gr)
		{
			if(nikWireless.flash_wizard==WLN_WIZARD_PRE)
			{
				SlaveSetPreflash();
			}
		}

		nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
		nikWireless.WL_statue = NIKWL_STATUE_HOLD;
		nikWireless.preTotal --;
		if(nikWireless.preTotal==0)
		{
			nikWireless.s_flashStatue = WL_FLASH_STATUE_AFTPF;					
		}
	}else 
	{
		//here handle arrays previose previous flash respectively
			if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE1)
			{
				nikWireless.preArr= WL_ARRARY_A;
				nikWireless.preTurn = WL_PFCOUNT_ONE;
				nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
#ifdef WIRELESS_DEBUG
				nikWireless.wl_debug[0] = nikWireless.WL_buf[0];
#endif
				CLR_WLN_BUF();
			}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE2)
			{
				nikWireless.preArr = WL_ARRARY_B;
				nikWireless.preTurn = WL_PFCOUNT_ONE;
				nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
#ifdef WIRELESS_DEBUG
				nikWireless.wl_debug[1] = nikWireless.WL_buf[0];
#endif
				CLR_WLN_BUF();
			}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE3)
			{
				nikWireless.preArr = WL_ARRARY_C;
				nikWireless.preTurn = WL_PFCOUNT_ONE;
				nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
#ifdef WIRELESS_DEBUG
				nikWireless.wl_debug[2] = nikWireless.WL_buf[0];
#endif
				CLR_WLN_BUF();
			}
	}
}

void SlaveAfterPreFlashProc(void)
{
	if((nikWireless.WL_buf[0]==WL_REARFLASH_COMMAND)||(nikWireless.WL_buf[0]==WL_REARFLASH_COMMANDex))
	{
		if(WORKINGMODE_TTL==nikWireless.s_wlMode)
		{
			WLS_ExposeTimeCal(nikWireless.rearReserve,nikWireless.hpSyn);
		}
		SlaveSetFlash();
	}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_READY_FRONT)
	{
		nikWireless.hpSyn = LOW_SPEED;
		SlaveGetFlashCode();
		SlaveFlashParaSet(nikWireless.s_wlMode);
	}else if(nikWireless.WL_buf[0]&0XF0==BLFLAG_READY_REAR)                //head at rear synchronic
	{
		nikWireless.hpSyn= LOW_SPEED;
		SlaveGetFlashCode();
		SlaveFlashParaSet(nikWireless.s_wlMode);
	}else if((nikWireless.WL_buf[0]&0xf0)==PREFLASH_LOCK)
	{
		nikWireless.preflashLock = PREFLASH_LOCK_ON;
		nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	}else if((nikWireless.WL_buf[0]&0xf0)==FLASH_HPSYN)
	{
		if(((nikWireless.WL_buf[0]&0x0f)>=0x08)&&((nikWireless.WL_buf[0]&0x0f)<=0x0c))
		{
			nikWireless.WL_opCode = nikWireless.WL_buf[mstPara->rmt_gr+1];
			SlaveFlashParaSet(nikWireless.s_wlMode);
			if(0x0b==(nikWireless.WL_buf[0]&0x0f))
			{
				nikWireless.hpSyn= HIGH_SPEED_LEVEL2;
			}else
			{
				nikWireless.hpSyn= HIGH_SPEED_LEVEL1;
			}

			nikWireless.WL_statue = NIKWL_STATUE_FLASH;
			CLR_WLN_BUF();
		}else
		{
			LightSlaveParaClear();
		}
	}else if((nikWireless.WL_buf[0]&0xf0)==FLASH_HPSYN_LOCK)
	{	
		nikWireless.preflashLock = PREFLASH_LOCK_ON;
		nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	}else
	{
		LightSlaveParaClear();
	}
}
#endif

void SlaveFlashProc(void)
{

}

void ParseCmdStart(void)
{
	// tmp = 0;
	//tmp = MAKEWORD(nikWireless.WL_buf[1], nikWireless.WL_buf[0]);
	//nikWireless.s_wlMode = SlaveModeConvert((tmp>>(8-mstPara->rmt_gr*4))&0x0f);
	nikWireless.preflashLockModeReserve[0] = SlaveModeConvert(nikWireless.WL_buf[0]&0x0f);
	nikWireless.preflashLockModeReserve[1] = SlaveModeConvert((nikWireless.WL_buf[1]>>4)&0x0f);
	nikWireless.preflashLockModeReserve[2] = SlaveModeConvert(nikWireless.WL_buf[1]&0x0f);
	nikWireless.s_wlMode = nikWireless.preflashLockModeReserve[mstPara->rmt_gr];
	DBG_PRINTF("m:%d\r\n",nikWireless.s_wlMode);
	//DBG_PRINTF("m: %d,%d,%d\r\n",nikWireless.preflashLockModeReserve[0],nikWireless.preflashLockModeReserve[1],nikWireless.preflashLockModeReserve[2]);

/*
	if(((nikWireless.WL_buf[0]&0x0f)==0x09)||((nikWireless.WL_buf[1]&0xf0)==0x90)||((nikWireless.WL_buf[1]&0x0f)==0x09)\
			||((nikWireless.WL_buf[0]&0x0f)==0x0a)||((nikWireless.WL_buf[1]&0xf0)==0xa0)||((nikWireless.WL_buf[1]&0x0f)==0x0a))
	{
		nikWireless.preTotal = 0;
		
		if(((nikWireless.WL_buf[0]&0x0f)==0x09)||((nikWireless.WL_buf[0]&0x0f)==0x0a))
		{
			nikWireless.preTotal ++;
		}
		if(((nikWireless.WL_buf[1]&0xf0)==0x90)||((nikWireless.WL_buf[1]&0xf0)==0xa0))
		{
			nikWireless.preTotal ++;
		}= 
		if(((nikWireless.WL_buf[1]&0x0f)==0x09)||((nikWireless.WL_buf[1]&0x0f)==0x0a))
		{
			nikWireless.preTotal ++;
		}
		//SET_WL_FLASH_TIMEOUT
		//nikWireless.s_flashStatue = WL_FLASH_STATUE_BFRPF;
		//nikWireless.WL_statue = NIKWL_STATUE_HOLD;
		//nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
		//nikWireless.WLBufIndex= 0;
		//nikWireless.preflashLock = PREFLASH_LOCK_OFF;
		//CLR_WLN_BUF();
			
	}else
	{
		nikWireless.s_flashStatue = WL_FLASH_STATUE_AFTPF;
		nikWireless.WL_statue = NIKWL_STATUE_HOLD;
		nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
		nikWireless.WLBufIndex= 0;
		CLR_WLN_BUF();
	}*/
	//nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	//nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
	//nikWireless.WLBufIndex= 0;
	//nikWireless.preflashLock = PREFLASH_LOCK_OFF;
	//CLR_WLN_BUF();
	SlaveRcvStateRst();
}

void ParseCmdPreFlash(void)
{/*
	if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE1)
	{
		nikWireless.preArr= WL_ARRARY_A;
		nikWireless.preTurn = WL_PFCOUNT_ONE;
		nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
		CLR_WLN_BUF();
	}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE2)
	{
		nikWireless.preArr = WL_ARRARY_B;
		nikWireless.preTurn = WL_PFCOUNT_ONE;
		nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
		CLR_WLN_BUF();
	}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE3)
	{
		nikWireless.preArr = WL_ARRARY_C;
		nikWireless.preTurn = WL_PFCOUNT_ONE;
		nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
		CLR_WLN_BUF();
	}*/
	nikWireless.preArr = (nikWireless.WL_buf[0]>>4)-0x09;
	nikWireless.preTurn = WL_PFCOUNT_ONE;
	nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
	wl_preIndex = 0;
	CLR_WLN_BUF();
//	DBG_PRINTF("pre %d\r\n",nikWireless.preArr);
}

void ParseCmdReadyFront(void)
{
	nikWireless.hpSyn = LOW_SPEED;
	SlaveGetFlashCode();
	SlaveFlashParaSet(nikWireless.s_wlMode,nikWireless.WL_opCode);
	DBG_PRINTF("tc %x\r\n",nikWireless.WL_opCode);
	nikWireless.WL_statue = NIKWL_STATUE_FLASH;
#ifdef SLAVE_DEBUG
	SLV_DBG_PIN = !SLV_DBG_PIN;
#endif
}

void ParseCmdReadyRear(void)
{
	nikWireless.hpSyn= LOW_SPEED;
	SlaveGetFlashCode();
	SlaveFlashParaSet(nikWireless.s_wlMode,nikWireless.WL_opCode);
	nikWireless.WL_statue = NIKWL_STATUE_FLASH;
}

void ParseCmdFlashLock(void)
{
	nikWireless.preflashLock = PREFLASH_LOCK_ON;
	nikWireless.WL_statue = NIKWL_STATUE_HOLD;
}

void ParseCmdHpSynFlash(void)
{
	if(((nikWireless.WL_buf[0]&0x0f)>=0x08)&&((nikWireless.WL_buf[0]&0x0f)<=0x0c))
	{
		nikWireless.WL_opCode = nikWireless.WL_buf[mstPara->rmt_gr+1];
		SlaveFlashParaSet(nikWireless.s_wlMode,nikWireless.WL_opCode);
		if(0x0b==(nikWireless.WL_buf[0]&0x0f))
		{
			nikWireless.hpSyn= HIGH_SPEED_LEVEL2;
		}else
		{
			nikWireless.hpSyn= HIGH_SPEED_LEVEL1;
		}

		nikWireless.WL_statue = NIKWL_STATUE_FLASH;
		//CLR_WLN_BUF();
	}else
	{
		LightSlaveParaClear();
	}
}

void ParseCmdHpSynFlashLock(void)
{
	nikWireless.preflashLock = PREFLASH_LOCK_ON;
	nikWireless.WL_statue = NIKWL_STATUE_HOLD;
}

void ParseCmdRearFlash(void)
{
	SlaveFlashParaSet(nikWireless.rearReserve,nikWireless.WL_opCode);
	SlaveSetFlash();
}

void ParseCmdPilot(void)
{
	//LightSlaveParaClear();
	slvPilotSet.fla_en = 1;
	slvPilotSet.tms = 20*(mstPara->rmt_gr+1);
	nikWireless.s_wlMode = WORKINGMODE_MANU;
	nikWireless.WL_output1 = 0;
	nikWireless.WL_output2 = 0;
	nikWireless.hpSyn = LOW_SPEED;
}

void SlaveRcvStateRst(void)
{
	nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	nikWireless.chFlag = 0;
	CLR_WLN_BUF();
	StopCapture();
	TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
	
}

u8 SlaveRptTimes(u8 rCode)
{
	if(rCode<=10)
	{
		return rCode;
	}else if(rCode<=50)
	{
		return 10+(rCode-10)*2;
	}else
	{
		return 1;
	}
}

u8 MstTimesConvert(s8 tms)
{
	if(tms<=0)
	{
		return 1;
	}
	else if(tms<=10)
	{
		return tms;
	}else if(tms<=90)
	{
		return 10+((tms-10)/2);
	}else
	{
		return 50;
	}
}

u8 SlaveRptFrq(u8 rCode)
{
	if(rCode<=10)
	{
		return rCode;
	}else if(rCode<=19)
	{
		return (rCode-10)*10+10;
	}else
	{
		return 100;
	}
}

u8 MstFrqConvert(s8 frq)
{
	if(frq<=10)
	{
		return frq;
	}else if(frq<=100)
	{
		return (frq-10)/10+10;
	}else
	{
		return 19;
	}
}

void SlaveRptFlash(void)
{
//the slave repeate flash 
//head 0x4f
//frequency the second byte's 2nd bit to 6th bit totally 5bits
//resolution: 1 to 10 represent 1 to 10, then every 1 step means 10hz increase
//the scope is 0x01~~0x13
//times the second byte's last bit combine with the 3rd byte's first 5bits totally 6bits
//thes scope is 0x01~~0x31
	u16 tmp = 0;
	//nikWireless.s_wlMode = WORKINGMODE_RPT;
	tmp = (nikWireless.WL_buf[0]&0x0f);
	if(tmp&(1<<(2-mstPara->rmt_gr)))
	{
		nikWireless.s_wlMode = WORKINGMODE_RPT;
	}else
	{
		nikWireless.s_wlMode = WORKINGMODE_OFF;
	}
	tmp = (nikWireless.WL_buf[1]>>2)&0x1f;		//5 bits
	nikWireless.wlMul_frq = SlaveRptFrq(tmp);
	tmp = (MAKEWORD(nikWireless.WL_buf[2], nikWireless.WL_buf[1])>>3)&0x3f;		//6 bits
	nikWireless.wlMul_time = SlaveRptTimes(tmp);
	SlaveRcvStateRst();
	DBG_PRINTF("RPT %d,%d\r\n",nikWireless.wlMul_frq,nikWireless.wlMul_time);
}

void ParseMstCmd(void)
{
	
#if 0
	u8 i=0;
	nikWireless.WLBufIndex++;
	nikWireless.bitLoadIndex = 0;
	DBG_PRINTF("ch %d\r\n",nikWireless.WL_Channel);
	for(i=0;i<nikWireless.WLBufIndex;i++)
	{
		DBG_PRINTF("%x ",nikWireless.WL_buf[i]);
	}
	DBG_PRINTF("\r\n");
	nikWireless.WLBufIndex=0;
	LightSlaveParaClear();
	return;
#endif

#ifdef SLAVE_DEBUG
	u8 i=0;
#endif

	nikWireless.WLBufIndex++;
	nikWireless.bitLoadIndex = 0;

#ifdef SLAVE_DEBUG
	//g_mstDgbPrt = 1;
	//for(i=0;i<nikWireless.WLBufIndex;i++)
	//{
		//DBG_PRINTF("%x ",nikWireless.WL_buf[i]);
	//}
	//DBG_PRINTF("\r\n");
#endif
	//DBG_PRINTF("c %x, m %d\r\n",nikWireless.WL_buf[0],nikWireless.s_wlMode);

	switch(nikWireless.WL_buf[0]&0xf0)
	{
		case BLFLAG_START:
			ParseCmdStart();
		break;

		case BLFLAG_PRE1:
		case BLFLAG_PRE2:
		case BLFLAG_PRE3:
			ParseCmdPreFlash();
		break;

		case BLFLAG_READY_FRONT:
			ParseCmdReadyFront();
		break;

		case BLFLAG_READY_REAR:
			ParseCmdReadyRear();
		break;

		case BLFLAG_FLALOCK:
			ParseCmdFlashLock();
		break;

		case BLFLAG_HPSYN:
			ParseCmdHpSynFlash();
		break;

		case BLFLAG_HPSYN_LOCK:
			ParseCmdHpSynFlashLock();
		break;

		case BLFLAG_REARFLASH:
			ParseCmdRearFlash();			
		break;

		case BLFLAG_PILOT:
			ParseCmdPilot();
		break;

		case BLFLAG_RPTFLA:
			SlaveRptFlash();
		break;

		default:
			SlaveRcvStateRst();
		break;
		
	}

	//SlaveRcvStateRst();
/*
	switch(nikWireless.s_flashStatue)
	{
		case WL_FLASH_STATUE_START:
			SlaveStartProc();
		break;

		case WL_FLASH_STATUE_BFRPF:
			SlaveBeforePreFlashProc();
		break;

		case WL_FLASH_STATUE_AFTPF:
			SlaveAfterPreFlashProc();
		break;

		case WL_FLASH_STATUE_FLASH:
			SlaveFlashProc();
		break;		
	}
*/
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
	hp_I2C_DELAY();
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

void TakeOnCharge(u8 mod)
{
	SetChargeEnable();
}


void FlashBaseFiber(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	//HC_CON3 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	FLASH_TIG = 0;
	delay_us(30);
	//delay_us(125);
	//Delay();
	SysTick->LOAD=time*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_TIG = 0;
			//FLASH_PWM = 0;
			//HC_CON3 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			//ENABLE_CHARGE();
			break;
			
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	//SysTick->CTRL=0x00;       //关闭计数器
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //清空计数器	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON3 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
}

void WLM_SHOT_ONCE(u16 time)
{
	HC_CON2 = 1;		//2015/11/12
	HC_CON1 = 0;
	FlashBaseFiber(time);
	HC_CON3 = 1;
}

void TURN_ON_TRIGER(unsigned int time)
{ 
	ENABLE_D30EN_EX();
	delay_us(20);
#ifdef FLASH_BASE_FIBER
	//FLASH_AMP = 0;
#else
	//HC_CON3 = 1;
	HC_CON1 = 1;
	//HC_CON4 = 1;
	HC_CON3 = 1;
	HC_CON2 = 0;
	HpExpCodeSend(0xA2C0);
#endif

	if(time>OUTPUT_MAX)
	{
		time = OUTPUT_MAX;
	}
#ifdef EXT_TEST
	HC_CON4 = 0;
	EXT_PA = 0;
#endif
	//if(!g_isHotProtect)
	{
		WLM_SHOT_ONCE(time);	
	}
#ifdef EXT_TEST
	HC_CON4 = 1;
	EXT_PA = 0;
#endif

#ifdef FLASH_BASE_FIBER
	FLASH_AMP = 0;
#endif

	DISABLE_D30EN_EX();
}

void HP_SHOT_ONCE(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	//HC_CON1 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	//delay_us(20);
	FLASH_TIG = 1;
	//delay_us(20);
	delay_us(5);
	FLASH_TIG = 0;

	//Delay();
	
	SysTick->LOAD=time*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;		
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达   
	//SysTick->CTRL=0x00;       //关闭计数器
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //清空计数器	 

	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON1 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
}

void ExtModFlash(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	HC_CON4 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	FLASH_TIG = 0;
	//delay_us(125);
	delay_us(40);				//2016/02/17 in rf slave mode, when mst flash , its flash cannot be seen, delay for a while to release the flash
	//Delay();
	SysTick->LOAD=time*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
		if(!EXT_MODE_DETEC)
		{
			//FLASH_TIG = 0;
			FLASH_PWM = 0;
			HC_CON4 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			ENABLE_CHARGE();
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	//SysTick->CTRL=0x00;       //关闭计数器
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //清空计数器	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	HC_CON4 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
	
}


void TakePreflash(u8 pid)
{
	OS_ENTER_CRITICAL();
	if(1==pid)
	{
		//TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
#ifdef FLASH_BASE_FIBER
		//flaTime = CanOutputSwitch(para1,para2);
		//WlmFlaADSet(0,2);
		WlmFlaADSet(-2,0);
		TURN_ON_TRIGER(OUTPUT_MAX);
#else
		//TURN_ON_TRIGER(pTime);
		//flaTime = CanOutputSwitch(para1,para2);
		TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
#endif
	}else if(2==pid)
	{
		//TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
#ifdef FLASH_BASE_FIBER
		//flaTime = CanOutputSwitch(para1,para2);
		//WlmFlaADSet(2,0);
		WlmFlaADSet(2,0);
		TURN_ON_TRIGER(OUTPUT_MAX);
#else
		//TURN_ON_TRIGER(pTime);
		//flaTime = CanOutputSwitch(para1,para2);
		TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
#endif
	}
	OS_EXIT_CRITICAL();
}

void SlaveTakePreflash(void)
{
	//DBG_PRINTF("pre %d, %d\r\n",nikWireless.preTurn,nikWireless.WL_statue);
	//SIGNAL1 =!SIGNAL1;
	if(nikWireless.WL_Channel==mstPara->rmt_ch)
	{
		if(mstPara->rmt_gr==nikWireless.preArr)
		{
#ifdef SLAVE_DEBUG
			DBG_PRINTF("pre %d\r\n",nikWireless.preTurn);
#else
			if((g_chargeOk>0)&&(!g_isHotProtect))
			{
				TakePreflash(nikWireless.preTurn);
			}else 
			{
				delay_us(500);
			}
#endif
		}else
		{
			delay_us(500);
		}
	}else
	{
#ifdef SLAVE_DEBUG
		DBG_PRINTF("Chan%d, %d, %d\r\n",nikWireless.WL_Channel,g_wtch_wt1,g_wtch_wt2);
#else
		delay_us(500);
#endif
	}
	
	if(WL_PFCOUNT_ONE==nikWireless.preTurn)
	{
		nikWireless.preTurn = WL_PFCOUNT_TWO;
		ttl_pre2_flag = 0;
	}else //if(WL_PFCOUNT_TWO==nikWireless.preTurn)
	{
		//nikWireless.WL_statue = NIKWL_STATUE_HOLD;
		ttl_pre2_flag = 1;
		SlaveRcvStateRst();
		nikWireless.preTurn = WL_PFCOUNT_RDY;
	}
}


void WlmFlaADSet(s8 para1, s8 para2)
{
	u8 tmp=0;

	if(20==*motoSet.dest_location)
	{
		tmp = (para1+3)*3+para2>0?((para1+3)*3+para2-1):0;
	}else
	{
		tmp = (para1+3)*3+para2;
	}

	if(tmp>(FLA_FBR_MAX+3)*3+2)
	{
		HC_CON3 = 1;
	}else
	{
		HC_CON3 = 0;
		if(tmp>(FLA_FBR_MCR+3)*3+2)
		{
			FLASH_AMP = 1;
		}else
		{
			FLASH_AMP = 0;
		}
	}
	
	//HpExpCodeSend(g_flaExpCode[(para1+3)*3+para2]<<4);
	HpExpCodeSend(g_flaExpCode[tmp]<<4);

	delay_us(20);
}



void TurnOnChargeLed(u8 init)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		//case CMD_MODE_LIGH_SLA:
			//if(g_chargeOk>AD_CHARGE_RDY)
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				CHARGE_LED_OK();
				//ChargeBuzzer();
			}else
			{
				CHARGE_LED_OFF();
			}
		break;

		case CMD_MODE_LIGH_SLA:
			if((pSnrSet[SNR_SU4]>0)&&(WORKINGMODE_OFF==mstPara->mode))
			{
				CHARGE_LED_OFF();
			}else
			{
				if((0==pSnrSet[SNR_REAR])||(1==pSnrSet[SNR_REAR]))
				{
					if(g_chargeOk>AD_CHARGE_LOW)
					{
						CHARGE_LED_OK();
						//ChargeBuzzer();
					}else
					{
						CHARGE_LED_OFF();
					}
				}else
				{
					CHARGE_LED_OFF();
				}
			}
		break;

		

		case CMD_MODE_LIGH_MST:
			//if(g_chargeOk>AD_CHARGE_RDY)
			if(g_chargeOk>AD_CHARGE_LOW)
			{
				CHARGE_LED_OK();
				//ChargeBuzzer();
			}else
			{
				CHARGE_LED_OFF();
			}
		break;

	}

}

unsigned int OutputSwitch(s8 value1,s8 value2)
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
	if(ttl_pre2_flag>0)
	{
		value1+=2;
	}

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
		DBG_PRINTF("FLA AD 0x%x\r\n",*hps);
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

void ReStdPara(signed char *p1, signed char *p2)
{
	if((*p2)<0)
	{
		(*p1)--;
		(*p2)+=3;
	}else if((*p2)>2)
	{
		(*p1)+=1;
		(*p2)-=3;
	}
}

#define EXT_VAL_LEN		27
#define EXTA_DASET_STD 19//22//19//15//18
#define EXTA_PA_ST	5

//void ExtaModSet(signed char foc, unsigned char isoCmp, unsigned char apert,
u8 ExtaModSet(signed char foc, unsigned char isoCmp, unsigned char apert,
					signed char cmp1, signed char cmp2 
					/*signed char feb1, signed char feb2, unsigned char febCnt*/)
{
	u8 tmp;
	s8 para1=0,para2=0;

#ifdef EXT_TEST_PRT
	//printf("foc %d,iso %x, apert %x\r\n",foc,isoCmp,apert);
#endif


	para1+=(EXTA_DASET_STD/3);
	para2+=(EXTA_DASET_STD%3);

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

	//GetMotoIncremental(foc);

	if(foc>0)
	{
		//para1 -= (foc-1)/3;
		//para2 -= (foc-1)%3;
		//para1 -= (foc)/3;
		//para2 -= (foc)%3;
		para1 -= (GetMotoIncremental(foc)/3);
		para2 -= (GetMotoIncremental(foc)%3);
	}

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif
/*
	if(!foc)
	{
		//motoMulti = 1;		
		para1 = 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		//para1 = (motoSet.focIndex-1)/3;
		//para2 = ((motoSet.focIndex-1)%3);
		//para1 += (motoSet.focIndex-1);
		para1 += (foc-1);
	}
*/
	
	//para1+=(EXTA_DASET_STD);

	//tmp = ((menuPara.cApert>>4)*6)+(((menuPara.cApert&0x0F)+2)/3);
	tmp = ((apert>>4)*6)+(((apert&0x0F)+2)/3);

	para1+=(tmp/3);
	para2 += ((tmp+3)%3);

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

	tmp = ((isoCmp>>4)*6)+(((isoCmp&0x0F)+2)/3);

	para1-=( tmp/3);
	para2-=((tmp+3)%3);
	

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

/*
	if(para2<0)
	{
		para1--;
		para2+=3;
	}else if(para2>2)
	{
		para1+=1;
		para2-=3;
	}
*/
	ReStdPara(&para1,&para2);


	para1+=cmp1;
	para2+=cmp2;

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	DBG_PRINTF("cmp %d, %d\r\n",para1,para2);
#endif

/*
	switch(febCnt)
	{
		case 0:
		if(pSnrSet[SNRMENU_FEBTURN]>0)
		{
			para1-= feb1;
			para2-= feb2;
		}
		break;

		case 1:
		if(!pSnrSet[SNRMENU_FEBTURN])
		{
			para1-= feb1;
			para2-= feb2;
		}
		break;

		case 2:
		para1+=feb1;
		para2+=feb2;
		break;
	}
*/

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	DBG_PRINTF("feb %d, %d\r\n",para1,para2);
#endif

	if(para1<0)
	{
		para1 = 0;
		para2 = 0;
	}else if(para1>=9)
	{
		para1 = 9;
		para2 = 0;
	}

#ifdef EXT_TEST_PRT
	//DBG_PRINTF("para %d, %d\r\n",para1,para2);
#endif

	if(para1>EXTA_PA_ST)
	{
		EXT_PA = 1;
#ifdef EXT_TEST_PRT
		//DBG_PRINTF("EXTA_PA\r\n");
#endif
	}else
	{
		EXT_PA = 0;
	}

#ifdef EXT_TEST_PRT
	//DBG_PRINTF("para %d, %d\r\n",para1,para2);
#endif

	//HpExpCodeSend(g_hpExpCode[para1*3+para2]);	
	HpExpCodeSend((g_extExpCode[para1*3+para2]<<4));	

	//DspIsoValue(LAYOUT_PARA_AREA2, 60,g_hpExpCode[para1*3+para2],DSP_NORMAL);
	//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,g_hpExpCode[para1*3+para2]>>8,DSP_NORMAL);
	//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(5*8)+8,g_hpExpCode[para1*3+para2]&0xff,DSP_NORMAL);
#ifdef EXT_TEST_PRT
	DBG_PRINTF("DA %d\r\n",g_extExpCode[para1*3+para2]);
#endif

	delay_us(100);

	return (para1*3+para2);
	

}

u8 GetApertIndex(u16 apert)
{
	return 1;			//add
}

u8 GetIsoIndex(u16 iso)
{
	return 1;
}


u8 FlashInResMode(signed char mode, signed char para1, signed char para2, unsigned char para3, unsigned char para4)
{
	HpSetting hpSet;
	unsigned int flaTime=0;
	u8 accValue=0;
	u32 accSum = 0;
//	s8 gnCmp=0;
//	u16 gnRst = 0;
	//u8 i = 0;
	char i = 0;

	ENABLE_D30EN_EX();

	delay_us(30);

	switch(mode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_TTL_POOR:
			if(para3>HP_FLASHMODE_OFF)
			{
				HpExpCalc(para1,para2,0,0,0,&hpSet,0);
				HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
			}else
			{
#ifdef FLASH_BASE_FIBER
				flaTime = OutputSwitch(para1,para2);
				WlmFlaADSet(para1,para2);
				TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
#else
				//TURN_ON_TRIGER(pTime);
				flaTime = OutputSwitch(para1,para2);
				TURN_ON_TRIGER(flaTime);
#endif
			}
			if(para1>=0)
			{
				accValue = para1*3 + para2;   //calculate the flash value
				accSum = (1<<(accValue/3));
			}
		break;

		case WORKINGMODE_MANU:
			if(para3>HP_FLASHMODE_OFF)
			{
				//HpExpCalc(mstPara->output1,mstPara->output2,0,0,0,&hpSetting,0);
				HpExpCalc(para1,para2,0,0,0,&hpSet,0);
				//HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);
				HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
			}else
			{
				//pTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
				//flaTime = CanOutputSwitch(para1,para2);
#ifdef FLASH_BASE_FIBER
				//flaTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
				flaTime = OutputSwitch(para1,para2);
				WlmFlaADSet(para1,para2);
				TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
#else
				//TURN_ON_TRIGER(pTime);
				flaTime = OutputSwitch(para1,para2);
				TURN_ON_TRIGER(flaTime);
#endif
			}
			if(para1>=0)
			{
				accValue = para1*3 + para2;
				accSum = (1<<(accValue/3));
			}
		break;

		case WORKINGMODE_AUTO:
			//accValue = ExtaModSet(GetMotoStdIndex(*motoSet.dest_location),GetIsoIndex(cmrPara.iso),GetApertIndex(cmrPara.apert),mstPara->cmp1,mstPara->cmp2
			//			/*flaPara.ttl_FEB1,flaPara.ttl_FEB2,flaPara.FEB_count*/);
			//accValue = ExtaModSet(GetMotoStdIndex(*motoSet.dest_location),para1,para2,mstPara->cmp1,mstPara->cmp2
			//			/*flaPara.ttl_FEB1,flaPara.ttl_FEB2,flaPara.FEB_count*/);
			if((cmrPara.cls>0)&&(pSnrSet[SNR_APERT]<1))
			{
				accValue = ExtmModSet(GetMotoStdIndex(*motoSet.dest_location),cmrPara.iso,cmrPara.apert,mstPara->a_cmp1,mstPara->a_cmp2);
			}else
			{
				accValue = ExtmModSet(GetMotoStdIndex(*motoSet.dest_location),mainPara.extIso,mainPara.extApert,mstPara->a_cmp1,mstPara->a_cmp2);
			}
			ENABLE_D30EN_EX();
			if((para3>0)&&(cmrPara.cls>0))
			{
				HpExpCalc(accValue/3,accValue%3,0,0,0,&hpSet,0);
				HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
			}else
			{
				ExtModFlash(5000);
			}
			accSum = (1<<(accValue/3));
			DISABLE_D30EN_EX();
		break;

		case WORKINGMODE_GN:
			/*
			gnCmp = (mstPara->gn_cmp1*3+mstPara->gn_cmp2);
			if(gnCmp>0)
			{
				i = CodeSearch((char*)g_GnScale, 34, mstPara->gn_m);
				if((gnCmp+i)<34)
				{
					gnRst = g_GnScale[gnCmp+i];
				}else
				{
					gnRst = g_GnScale[33];
				}
			}else
			{
				i = CodeSearch((char*)g_GnScale, 34, mstPara->gn_m);
				if((gnCmp+i)>0)
				{
					gnRst = g_GnScale[gnCmp+i];
				}else
				{
					gnRst = g_GnScale[0];
				}
			}*/
			//CMP is accord with gnSt
			//if(mstPara->gn_m<flashPara.gnSt)
			if(mstPara->gn_m<flashPara.gnSt)
			{
				para1 = 0;
				para2 = 0;
			}else if(mstPara->gn_m>=flashPara.gnDst)
			{
				para1 = 9;
				para2 = 0;
			}else
			{
				//i = (CodeSearch((char*)g_GnScale,34, mstPara->gn_m) - CodeSearch((char*)g_GnScale,34, flashPara.gnSt));
				i = (CodeSearch((char*)g_GnScale,34, mstPara->gn_m) - CodeSearch((char*)g_GnScale,34, flashPara.gnSt));
				para1 = i/3;
				para2 = i%3;
			}
			if(para3>HP_FLASHMODE_OFF)
			{
				//HpExpCalc(mstPara->output1,mstPara->output2,0,0,0,&hpSetting,0);
				HpExpCalc(para1,para2,0,0,0,&hpSet,0);
				//HighSpeedTriger(&hpSetting,HP_REALFLA_INTERVAL);
				HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
			}else
			{
				flaTime = OutputSwitch(para1,para2);
				WlmFlaADSet(para1-2,para2);
				TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
			}
			if(para1>=0)
			{
				accValue = para1*3+para2;
			}
			accSum = (1<<(accValue/3));
		break;

		case WORKINGMODE_RPT:
#ifdef FLASH_BASE_FIBER
			//flaTime = CanOutputSwitch(mstPara->output1,mstPara->output2);
			flaTime = OutputSwitch(para1,para2);
			WlmFlaADSet(para1,para2);
			TURN_ON_TRIGER(para1>FLA_FBR_MAX? flaTime:OUTPUT_MAX);
#else
			//TURN_ON_TRIGER(pTime);
			flaTime = OutputSwitch(para1,para2);
			TURN_ON_TRIGER(flaTime);
#endif
			rptSet.exp1 = para1;
			rptSet.exp2 = para2;
			RptFlashPrepear(para3,para4-1);
			if(para1>=0)
			{
				accValue = (para1*3+para2);
				accSum = (1<<(accValue/3))*rptSet.finalExpTimes;
			}else
			{
				accValue = (rptSet.finalExpTimes / 4);
				accSum = (1<<(accValue/3));
			}
		break;
		
	}

	//g_expAccHotWarnValue += (1<<(accValue/3));
	g_expAccHotWarnValue += accSum;
	gWSleepCounter = 0;

	DISABLE_D30EN_EX();

	return accValue;
}

void CalcWithCmp(s8 *pa1, s8* pa2, s8 cmp1, s8 cmp2,u8 isHp)
{
	*pa1 += cmp1;
	*pa2 += cmp2;
	if(*pa2>2)
	{
		*pa1++;
		*pa2-=3;
	}else if(*pa2<0)
	{
		*pa2+=3;
		*pa1--;
	}

	if(*pa1>=7)
	{
		*pa1 = 7;
		*pa2 = 0;
	}else if(*pa1<-3)
	{
		*pa1 = -3;
		*pa2 = 0;
	}

	if((isHp>0)&&(*pa1<0))
	{
		*pa1 = 0;
		*pa2 = 0;
	}
}

/*
void CalcWithPreFlash(void)
{
	if(ttl_pre2_flag>0)
	{
		g_expTTLPara1+=2;
		if(g_expTTLPara1>=7)
		{
			g_expTTLPara1 = 7;
			g_expTTLPara2 = 0;
		}
	}
}
*/

void CalcRptFlashTimes(void)
{
	if((1000/(*rptSet.frequence)+1)>(cmrPara.shutter/(*rptSet.eTimes)))
	{
		rptSet.finalExpTimes= cmrPara.shutter*(*rptSet.frequence)/1000;
	}else
	{
		rptSet.finalExpTimes = *rptSet.eTimes;
	}
}

void RptFlashCom(void)
{
	g_rptPilotFlag = 0;
	/*
	if((1000/(*rptSet.frequence)+1)>(cmrPara.shutter/(*rptSet.eTimes)))
	{
		rptSet.finalExpTimes= cmrPara.shutter*(*rptSet.frequence)/1000;
	}else
	{
		rptSet.finalExpTimes = *rptSet.eTimes;
	}*/
	CalcRptFlashTimes();
	//FlashInResMode(WORKINGMODE_RPT,mstPara->multi_exp1,mstPara->multi_exp2,mstPara->rpt_hz,mstPara->rpt_times);
	FlashInResMode(WORKINGMODE_RPT,mstPara->multi_exp1,mstPara->multi_exp2,mstPara->rpt_hz,mstPara->rpt_times);
}

void TakeFlash(void)
{
	//delay_ms(5);
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			switch(mstPara->mode)
			{
				case WORKINGMODE_TTL:
				case WORKINGMODE_TTL_POOR:
					//CalcWithPreFlash();
					//CalcWithCmp(&g_expTTLPara1,&g_expTTLPara2,mstPara->cmp1,mstPara->cmp2,mstPara->hpFlash);
					FlashInResMode(WORKINGMODE_TTL,g_expTTLPara1,g_expTTLPara2,mstPara->hpFlash,0);
					if(g_isFlashWeak>0)
					{
						g_dspCmpMod = 1;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
					}else if(g_dspCmpMod>0)
					{
						g_dspCmpMod = 0;
						SET_UPDATE_EVT(DSP_UPDATE_MODE);
					}
				break;

				case WORKINGMODE_MANU:
					FlashInResMode(WORKINGMODE_MANU,mstPara->output1,mstPara->output2,mstPara->hpFlash,0);
				break;

				case WORKINGMODE_RPT:
					RptFlashCom();
				break;

				case WORKINGMODE_GN:
					FlashInResMode(WORKINGMODE_GN,mstPara->gn_m,0,mstPara->hpFlash,0);
				break;

				case WORKINGMODE_AUTO:
					FlashInResMode(WORKINGMODE_AUTO,mstPara->a_cmp1,mstPara->a_cmp2,mstPara->hpFlash,0);
				break;
			}
		break;

		case CMD_MODE_LIGH_MST:
			if(pSnrSet[SNR_SU4]>0)
			{
				switch(mstPara->mode)
				{
					case WORKINGMODE_TTL:
						FlashInResMode(WORKINGMODE_TTL,g_expTTLPara1,g_expTTLPara2,mstPara->hpFlash,0);
					break;

					case WORKINGMODE_MANU:
						FlashInResMode(WORKINGMODE_MANU,mstPara->output1,mstPara->output2,mstPara->hpFlash,0);
					break;

					case WORKINGMODE_RPT:
						/*
						g_rptPilotFlag = 0;
						if((1000/(*rptSet.frequence)+1)>(cmrPara.shutter/(*rptSet.eTimes)))
						{
							rptSet.finalExpTimes = cmrPara.shutter*(*rptSet.frequence)/1000;
						}else
						{
							rptSet.finalExpTimes = *rptSet.eTimes;
						}
						FlashInResMode(WORKINGMODE_RPT,mstPara->multi_exp1,mstPara->multi_exp2,mstPara->rpt_hz,mstPara->rpt_times);
						*/
						RptFlashCom();
					break;

					case WORKINGMODE_GN:
						FlashInResMode(WORKINGMODE_GN,mstPara->gn_m,0,mstPara->hpFlash,0);
					break;

					case WORKINGMODE_AUTO:
						FlashInResMode(WORKINGMODE_AUTO,mstPara->a_cmp1,mstPara->a_cmp2,mstPara->hpFlash,0);
					break;
				}	
			}else
			{
				if(pSnrSet[SNR_MSTRPT]>0)
				{
					if(WORKINGMODE_OFF==mainPara.modGr[0].mode)
					{
						ENABLE_D30EN_EX();
						delay_us(30);
						WLM_COMMAND_TRIGER(WLM_PREFLA_INTERVAL);		
						DISABLE_D30EN_EX();
					}else
					{
						switch(mainPara.modGr[0].mode)
						{
							case WORKINGMODE_TTL:
								FlashInResMode(WORKINGMODE_TTL,g_expTTLPara1,g_expTTLPara2,mstPara->hpFlash,0);
							break;

							case WORKINGMODE_MANU:
								FlashInResMode(WORKINGMODE_MANU,mainPara.modGr[0].output1,mainPara.modGr[0].output2,mstPara->hpFlash,0);
							break;

							case WORKINGMODE_AUTO:
								FlashInResMode(WORKINGMODE_AUTO,mainPara.modGr[0].a_cmp1,mainPara.modGr[0].a_cmp2,0,0);
							break;

							case WORKINGMODE_RPT:
							break;
						}
					}
				}else
				{
					if(mainPara.modGr[0].rpt_isOn>0)
					{
						//FlashInResMode(WORKINGMODE_RPT,mstPara->multi_exp1,mstPara->multi_exp2,mstPara->rpt_hz,mstPara->rpt_times);
						RptFlashCom();
					}else
					{
						ENABLE_D30EN_EX();
						delay_us(30);
						WLM_COMMAND_TRIGER(WLM_PREFLA_INTERVAL);		
						DISABLE_D30EN_EX();
					}
				}
			}
		break;

		case CMD_MODE_LIGH_SLA:
			switch(nikWireless.s_wlMode)
			{
				case WORKINGMODE_TTL:
					FlashInResMode(WORKINGMODE_TTL, g_expTTLPara1, g_expTTLPara2, nikWireless.hpSyn,0);
					DBG_PRINTF("TTL %d,%d\r\n",g_expTTLPara1, g_expTTLPara2);
				break;

				case WORKINGMODE_AUTO:
					SlaveAutoFlash();
					//FlashInResMode(WORKINGMODE_AUTO,GetIsoIndex(cmrPara.iso),GetApertIndex(cmrPara.apert),0,0);
					//DBG_PRINTF("AUTO\r\n");
				break;

				case WORKINGMODE_MANU:
					FlashInResMode(WORKINGMODE_MANU,nikWireless.WL_output1,nikWireless.WL_output2,nikWireless.hpSyn,0);
					//DBG_PRINTF("MANU %d,%d\r\n",nikWireless.WL_output1,nikWireless.WL_output2);
				break;		

				case WORKINGMODE_RPT:
					rptSet.finalExpTimes = nikWireless.wlMul_time;
					FlashInResMode(WORKINGMODE_RPT, nikWireless.rpt_output1, nikWireless.rpt_output2, nikWireless.wlMul_frq,nikWireless.wlMul_time);
					rptSet.finalExpTimes--;
				break;
			}
/*
			if(WORKINGMODE_OFF!=nikWireless.s_wlMode)
			{
				BUZZER_ON(BUZZER_TIME_NORMAL*2,1);
			}
*/
		break;

		
	}
	
	g_motoInvalidCounter = MOTO_INVALID_CNT;
	
}

void RptFlashPrepear(u8 hz, u8 tms)
{
	//rptSet.finalExpTimes = mstPara->rpt_times;
	//rptSet.finalExpTimes = tms;

	//rptSet.lowFrqCnt = (1000/mstPara->rpt_hz);
	rptSet.lowFrqCnt = (10000/hz);
	//rptSet.lowFrqCnt = (1000/hz);

	SetRptFlashTimer(rptSet.lowFrqCnt);

	//g_comStatue = COM_STATUE_RPT_FLA;
	//OSTaskResume(COMPROC_TASK_PRIO);
}

void SetRptFlashTimer(u16 ticks)
{

	if(rptSet.finalExpTimes>0)
	{
		g_tmHandleFlag |= TM_HANDLE_MULTIFLASH;
		Timer2_Init(ticks,7200);
	}
/*
	INT8U tmrErr;
	OSTmrReload(g_rptFlashTmr,ticks);
	OSTmrStart(g_rptFlashTmr, &tmrErr);
*/
}

void TakeRptFlash(u8* flaTms,s8 exp)
{
	unsigned int flaTime=0;

	if((g_rptPilotFlag>0)&&(PILOT_IN>0))
	{
		*flaTms = 0;
		g_tmHandleFlag &= ~TM_HANDLE_MULTIFLASH;
		return;
	}
	
	if((*flaTms)>0)
	{
		ENABLE_D30EN_EX();
#ifdef FLASH_BASE_FIBER
		flaTime = OutputSwitch(exp/3,exp%3);
		WlmFlaADSet(exp/3,exp%3);
		TURN_ON_TRIGER(exp>FLA_FBR_MAX?flaTime:OUTPUT_MAX);
#else
		flaTime = OutputSwitch(exp/3,exp%3);
		TURN_ON_TRIGER(flaTime);
#endif
		DISABLE_D30EN_EX();

		(*flaTms)--;
		if((*flaTms)<=0)		//error without point
		{
			g_tmHandleFlag &= ~TM_HANDLE_MULTIFLASH;
			//multiSet.mulCtl=MULTI_FLASH_DISABLE;	
			
		}else
		{
			Timer2_Init(rptSet.lowFrqCnt,7200);
			g_tmHandleFlag |= TM_HANDLE_MULTIFLASH;
			//RptFlashPrepear(mstPara->rpt_hz,*flaTms);
		}
	}else
	{
		//g_tmHandleFlag = TM_HANDLE_CLR;
		//multiSet.mulCtl=MULTI_FLASH_DISABLE;
		g_tmHandleFlag &= ~TM_HANDLE_MULTIFLASH;
		(*flaTms) = 0;
	}
}
/*
void TakeRptFlash(u8* flaTms, s8 exp,int frqCnt)
{
	static int mulCnt=0;
	unsigned int flaTime=0;
	if((*flaTms)>0)
	{
		mulCnt++;
		if(mulCnt>frqCnt)
		{
			ENABLE_D30EN_EX();
#ifdef FLASH_BASE_FIBER
			flaTime = OutputSwitch(exp/3,exp%3);
			WlmFlaADSet(exp/3,exp%3);
			TURN_ON_TRIGER(exp>FLA_FBR_MAX?flaTime:OUTPUT_MAX);
#else
			flaTime = OutputSwitch(exp/3,exp%3);
			TURN_ON_TRIGER(flaTime);
#endif
			mulCnt = 0;

			DISABLE_D30EN_EX();

			(*flaTms)--;

			if((*flaTms)<=0)		//error without point
			{
				g_tmHandleFlag = TM_HANDLE_CLR;
				//multiSet.mulCtl=MULTI_FLASH_DISABLE;	
				rptSet.mulCtl = MULTI_FLASH_DISABLE;
			}else
			{
				//Timer4_Init(1000,72);
				//g_tmHandleFlag = TM_HANDLE_MULTIFLASH;
				SetRptFlashTimer();
			}
		}else
		{
			//Timer4_Init(100,72);
			//g_tmHandleFlag = TM_HANDLE_MULTIFLASH;
			SetRptFlashTimer();
		}
	}else
	{
		g_tmHandleFlag = TM_HANDLE_CLR;
		rptSet.mulCtl=MULTI_FLASH_DISABLE;	
	}
}
*/

void SlaveTakeFlash(void)
{	
	//DBG_PRINTF("S flash %x\r\n",nikWireless.WL_opCode);
	
	if(nikWireless.WL_Channel==mstPara->rmt_ch)
	{
#ifdef SLAVE_DEBUG
		DBG_PRINTF("fla\r\n");
#else
		if((WORKINGMODE_OFF!=nikWireless.s_wlMode)&&
			(!g_isHotProtect)&&(g_chargeOk>0))
		{
			g_buzzerOn =1;
			TakeFlash();
		}else
		{
			delay_ms(10);
		}
#endif
	}else
	{
#ifdef SLAVE_DEBUG
		DBG_PRINTF("chan%x, fla \r\n",nikWireless.WL_Channel);
#else
		delay_ms(10);
#endif
	}
}

void WlmCmdSet(void)
{
	//HpExpCodeSend(CMD_THRESHOLD_VAL);
	//HC_CON2 = 1;
	//HC_CON2 = 0;
	//HC_CON2 = 1;
	//HC_CON2 = 1;
#if 0
	HC_CON2 = 1;
	HC_CON1 = 0;
	HC_CON3 = 1;
#endif
	//HC_CON3 = 1;
	//HC_CON3 = 0;
	//HpExpCodeSend(0x0200);
	//HpExpCodeSend(0x0400);
	HpExpCodeSend(HP_CMD_LEVEL<<4);
}

void WLM_COMMAND_TRIGER(u16 time)
{

#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;

	//if(g_isHotProtect>0)
	//{
	//	return;
	//}

	//TEST_SIGNAL_HIGH();
	DISABLE_CHARGE();//disable charge
	HC_CON3 = 0;
	FLASH_PWM = 1;
	//delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	FLASH_TIG = 0;
	delay_us(20);
	//delay_us(125);
	//Delay();
	SysTick->LOAD=time*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_TIG = 0;
			FLASH_PWM = 0;
			HC_CON3 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			ENABLE_CHARGE();
			//TEST_SIGNAL_LOW();
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	//SysTick->CTRL=0x00;       //关闭计数器
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //清空计数器	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	HC_CON3 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
	
}


void WLM_HEAD_TRIGER(u16 time)
{

#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;

	//if(g_isHotProtect>0)
	//{
	//	return;
	//}
	
	DISABLE_CHARGE();//disable charge
	HC_CON3 = 0;
	FLASH_PWM = 1;
	//delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	FLASH_TIG = 0;
	delay_us(20);
	//delay_us(125);
	//Delay();
	SysTick->LOAD=time*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_TIG = 0;
			FLASH_PWM = 0;
			HC_CON3 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			ENABLE_CHARGE();
			break;
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   

	SysTick->LOAD=time*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 

	do
	{
		temp=SysTick->CTRL;
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达  
	
	//SysTick->CTRL=0x00;       //关闭计数器
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //清空计数器	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	HC_CON3 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif

}

void WlmCmdHeadSend(u16 time)
{
	if(!g_isHotProtect)
	{
		HC_CON2 = 1;		//2015/11/12
		HC_CON1 = 0;
		WLM_HEAD_TRIGER(time);
		//WLM_COMMAND_TRIGER(time);
	}
}

//#define WLM_CHAN_WT1 (148-30-30)
//#define WLM_CHAN_WT2 (228-30-30+6)
//#define WLM_CHAN_WT3 (348-30-30+10)
//define WLM_CHAN_WT4 (208-30)
#define WLM_CH1_WT1  (105-30)
#define WLM_CH1_WT2  (190-20)
#define WLM_CH2_WT1  (190-30)
#define WLM_CH2_WT2  (117-30)
#define WLM_CH3_WT1  (194-30)
#define WLM_CH3_WT2  (184-30)
#define WLM_CH4_WT1  (116-30)
#define WLM_CH4_WT2  (334-20)


void WLM_SendChanhead(signed char chan)
{

	WlmCmdSet();

	delay_us(50);

	switch(chan)
	{
		case (WL_CHANNEL_ONE):
			WlmCmdHeadSend(WLM_CH1_WT1);
			WlmCmdHeadSend(WLM_CH1_WT2);
		break;

		case (WL_CHANNEL_TWO):
			WlmCmdHeadSend(WLM_CH2_WT1);
			WlmCmdHeadSend(WLM_CH2_WT2);
		break;

		case (WL_CHANNEL_THREE):
			WlmCmdHeadSend(WLM_CH3_WT1);
			WlmCmdHeadSend(WLM_CH3_WT2);
		break;

		case (WL_CHANNEL_FOUR):
			WlmCmdHeadSend(WLM_CH4_WT1);
			WlmCmdHeadSend(WLM_CH4_WT2);
		break;
	}
	WlmCmdHeadSend(WLM_BIT_INTERVAL-20-30);                                //2016/04/02

}


void WLM_SendData(unsigned char *Arr, char sendSize)
{
	char dataIndex=0;
	char bitIndex=0;

	ENABLE_D30EN_EX();

	WLM_SendChanhead(mainPara.m_chan);

	while(dataIndex<sendSize)
	{
		while(bitIndex<8)
		{
			if((Arr[dataIndex]>>(7-bitIndex))&0x01)
			{
				WLM_COMMAND_TRIGER(WLM_BIT_INTERVAL-30);
			}else
			{
				delay_us(WLM_BIT_INTERVAL);
			}
			bitIndex++;
		}
		bitIndex = 0;
		dataIndex++;
	}
	
	DISABLE_D30EN_EX();
}

void LightSlaveParaClear(void)
{
//	INT8U tmrErr;

//	canWireless.activeArr=0;
//			gbRealFlash = 1;
//#ifdef SLAVE_DEBUG
//	DBG_PRINTF("CLR\r\n");
//#endif
	nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
//	wlSlave.s_flashStatue = WL_FLASH_STATUE_HOLD;
	nikWireless.preArr = WL_ARRARY_NULL;
	nikWireless.s_wlMode = WORKINGMODE_OFF;
	nikWireless.preTurn = WL_PFCOUNT_RDY;
	nikWireless.activeArr=0;
	nikWireless.chFlag = 0;

	//OSTmrDel(g_flashTmr,&tmrErr);
	//OSTmrStop(g_flashTmr,OS_TMR_OPT_NONE,NULL,&tmrErr);
	//StartCapture();

	//here clear the buffer
	//CLR_WLC_BUF();
	memset(nikWireless.WL_buf,0x00,WLBUF_LEN);
	nikWireless.WLBufIndex=0;
	nikWireless.bitLoadIndex=0;

	StopCapture();
	TmCaptureInit(LITE_SLAVE_CAPTURE_TIME,72);
	//g_slaveStart = 1;

	//StartCapture();
}

void SingleFlashSet(void)
{
	g_signalFlash = 1;
	g_comStatue = COM_STATUE_REALFLASH;
	g_expTTLPara1 = mstPara->output1;
	g_expTTLPara2 = mstPara->output2;
	if(g_stbyStatue>0)
	{
		g_stbyStatue = 0;                           // 20161010 wake up
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
}


void EXTI4_IRQHandler(void)		//big point interrupt
{
	//EXTI->PR=1<<4;	

	switch(g_comStatue)
	{
		case COM_STATUE_REALFLASH:
			//SET_SCK_LOW();
			g_signalFlash = 0;
			OSTaskResume(COMPROC_TASK_PRIO);
			//TakeFlash();
		break;

		case COM_STATUE_IDLE:
			SingleFlashSet();
			OSTaskResume(COMPROC_TASK_PRIO);
		break;

		case COM_STATUE_FLASH_READY:
			g_comStatue = COM_STATUE_REALFLASH;
			OSTaskResume(COMPROC_TASK_PRIO);
		break;

		default:
		break;
	}

	EXTI_ClearITPendingBit(EXTI_Line4);	

	if(SET==EXTI_GetITStatus(EXTI_Line4))
	{
		EXTI_ClearITPendingBit(EXTI_Line4);	
	}
}

unsigned char BuildWlmFlaCode(void)
{

	wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1] = wlSet.m_flaCmd[2] = 0 ;
	wlSet.m_flaCmd[0] = 0x06;
	wlSet.m_flaCmd[0] <<= 4;	

	if(WORKINGMODE_MANU==mainPara.modGr[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x0b;
		wlSet.m_cmdIndex = 1;
	}else if(WORKINGMODE_TTL==mainPara.modGr[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x09;
		wlSet.m_cmdIndex = 1;
	}
	else if(WORKINGMODE_AUTO==mainPara.modGr[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x0A;
		wlSet.m_cmdIndex = 1;
	}
	else if(WORKINGMODE_OFF==mainPara.modGr[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x08;
		wlSet.m_cmdIndex = 0;
	}else
	{
		wlSet.m_flaCmd[0] |= 0x08;
		wlSet.m_cmdIndex = 0;
	}

	if(WORKINGMODE_MANU==mainPara.modGr[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0b;
		wlSet.m_cmdIndex = 2;
	}else if(WORKINGMODE_TTL==mainPara.modGr[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x09;
		wlSet.m_cmdIndex = 2;
	}
	else if(WORKINGMODE_AUTO==mainPara.modGr[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0A;
		wlSet.m_cmdIndex = 2;
	}
	else if(WORKINGMODE_OFF==mainPara.modGr[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x08;
	}else
	{
		wlSet.m_flaCmd[1] |= 0x08;
	}

	wlSet.m_flaCmd[1] <<= 4;

	if(WORKINGMODE_MANU==mainPara.modGr[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0b;
		wlSet.m_cmdIndex = 3;
	}else if(WORKINGMODE_TTL==mainPara.modGr[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x09;
		wlSet.m_cmdIndex = 3;
	}
	else if(WORKINGMODE_AUTO==mainPara.modGr[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0A;
		wlSet.m_cmdIndex = 3;
	}
	else if(WORKINGMODE_OFF==mainPara.modGr[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x00;
	}else
	{
		wlSet.m_flaCmd[1] |= 0x00;
	}	

	return wlSet.m_cmdIndex;

/*
	if(wlSet.m_cmdIndex)
	{
		return 1;
	}else 
	{
		return 0;
	}
*/
}

unsigned char BuildWlmRptCode(void)
{
	u8 tmp = 0;
	CalcRptFlashTimes();
	if((mainPara.modGr[1].rpt_isOn>0)||(mainPara.modGr[2].rpt_isOn>0)||(mainPara.modGr[3].rpt_isOn>0))
	{
		wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1] = wlSet.m_flaCmd[2] = 0 ;
		//wlSet.m_flaCmd[0] = 0x4F;
		tmp = (((mainPara.modGr[1].rpt_isOn>0)<<2)|((mainPara.modGr[2].rpt_isOn>0)<<1)|(mainPara.modGr[3].rpt_isOn>0));
		wlSet.m_flaCmd[0] = 0x40|0x08|tmp;
		wlSet.m_flaCmd[1] |= 0x82;   //the 7th bit and 1st bit is 1
		tmp = MstFrqConvert(mstPara->rpt_hz);
		wlSet.m_flaCmd[1] |= (tmp<<2);
		//tmp = MstTimesConvert(mstPara->rpt_times);
		tmp = MstTimesConvert(rptSet.finalExpTimes);
		wlSet.m_flaCmd[1] |= (tmp>>6)&0x01;
		wlSet.m_flaCmd[2] |= (tmp<<3)&0xf8;
		wlSet.m_cmdIndex = 3;
		return 3;
	}else
	{
		wlSet.m_cmdIndex = 0;
		return 0;
	}
	
}

char BuildRptFlashCmd(void)
{
	u8 tmp, tmpH, tmpL = 0;
	if((mainPara.modGr[1].rpt_isOn>0)||(mainPara.modGr[2].rpt_isOn>0)||(mainPara.modGr[3].rpt_isOn>0))
	{
		wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1] = wlSet.m_flaCmd[2] = 0 ;
		tmp = (0x80+((MANU_CODE_MAX-mstPara->multi_exp1)*3-((3+mstPara->multi_exp2)%3))*2);
		//tmp1 = (0x80+((MANU_CODE_MAX-mainPara.modGr[WL_ARRARY_A].output1)*3-((3+mainPara.modGr[WL_ARRARY_A].output2)%3))*2);
		tmpH = ((tmp>>4)&0x0f);
		tmpL = ((tmp<<4)&0xf0);
		wlSet.m_flaCmd[0] |= 0x20;
		if(mainPara.modGr[1].rpt_isOn>0)
		{
			wlSet.m_flaCmd[0] |= tmpH;
			wlSet.m_flaCmd[1] |= tmpL;
		}else
		{
			wlSet.m_flaCmd[0] |= 0x08;
			wlSet.m_flaCmd[1] |= 0x00;
		}

		if(mainPara.modGr[2].rpt_isOn>0)
		{
			wlSet.m_flaCmd[1] |= tmpH;
			wlSet.m_flaCmd[2] |= tmpL;
		}else
		{
			wlSet.m_flaCmd[1] |= 0x08;
			wlSet.m_flaCmd[2] |= 0x00;
		}

		if(mainPara.modGr[3].rpt_isOn>0)
		{
			wlSet.m_flaCmd[2] |= tmpH;
			wlSet.m_flaCmd[3] |= tmpL;
		}else
		{
			wlSet.m_flaCmd[2] |= 0x08;
			wlSet.m_flaCmd[3] |= 0x00;
		}
		
		return 4;
	}else
	{
		wlSet.m_cmdIndex = 0;
		return 0;
	}
	
	
}


char BuildRealFlashCmd(unsigned char para0,unsigned char para1,unsigned char para2,unsigned char para3,char hpLev)
{
	unsigned int tmp1,tmp2,tmp3;
	//unsigned int cmpTmp;

	wlSet.m_cmdIndex = 0;
	wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1] = wlSet.m_flaCmd[2] = wlSet.m_flaCmd[3] = 0;

//when the shutter is lower than 1/400, the max flash value is 0xBF
	if(WORKINGMODE_TTL==mainPara.modGr[WL_ARRARY_A].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))       //high speed
		{
			//tmp1 = (para1>>1) +0x8d;
			tmp1 = (para1>>1) + (unsigned int)(0xab - cmrPara.pIso);
		}else
		{
			//tmp1 = (para1>>1)+0x50;
			tmp1 = (para1>>1) + (unsigned int)(0x6e - cmrPara.pIso);
			//tmp1 |= 0x80;
			
		}
		//cmpTmp = tmp1+(wlSet.m_arrPara[WL_ARRARY_A].cmp1*3+wlSet.m_arrPara[WL_ARRARY_A].cmp2)*2;
		tmp1 += (mainPara.modGr[WL_ARRARY_A].cmp1*3+mainPara.modGr[WL_ARRARY_A].cmp2)*2;

		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//if(cmpTmp<0xa0)
			if(tmp1<0xa0)
			{
				tmp1 = 0xa0;
			}else if(tmp1>0xf0)//else if(cmpTmp>0xf0)
			{
				tmp1 = 0xf0;
			}
		}else
		{

			if(tmp1>0xBF)
			{
				tmp1 = 0xBF;
			}else if(tmp1<0x80)//else if(cmpTmp<0x80)
			{
				tmp1 |= 0x80; 
			}

		}
		//putchar(tmp1);
		wlSet.m_cmdIndex = 2;
	}//else
	else if(WORKINGMODE_MANU==mainPara.modGr[WL_ARRARY_A].mode)
	{
		/*
		if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_A].mode)
		{
			tmp1 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_A].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_A].output2)%3)*2);
			wlSet.m_cmdIndex = 2;
		}else
		{
			tmp1 = 0x80;
		}*/
		tmp1 = (0x80+((MANU_CODE_MAX-mainPara.modGr[WL_ARRARY_A].output1)*3-((3+mainPara.modGr[WL_ARRARY_A].output2)%3))*2);
		wlSet.m_cmdIndex = 2;
	}
	else if(WORKINGMODE_AUTO==mainPara.modGr[WL_ARRARY_A].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp1 = (uartSend.pApert + 0xc1) - uartSend.ISO ;
			tmp1 = ((unsigned int)cmrPara.pApert + (unsigned int)(0xFD-cmrPara.pIso));
			//wlSet.m_cmdIndex = 2;
		}else
		{
			tmp1 = ((unsigned int)(cmrPara.pApert + cmrPara.pShutter) + (unsigned int)(0xC7-cmrPara.pIso));
			//wlSet.m_cmdIndex = 2;
		}
			
		//cmpTmp = tmp1+(wlSet.m_arrPara[WL_ARRARY_A].cmp1*3+wlSet.m_arrPara[WL_ARRARY_A].cmp2)*2;
		tmp1 += (mainPara.modGr[WL_ARRARY_A].a_cmp1*3+mainPara.modGr[WL_ARRARY_A].a_cmp2)*2;
		//if(cmpTmp>0xff)
		if(tmp1>0xff)
		{
			//tmp1 = ((cmpTmp&0xff) + 0x80);//0xff;
			tmp1 = ((tmp1&0xff) + 0x80);
		}/*else
		{
			//tmp1 = cmpTmp;//(unsigned char)cmpTmp;
			
		}*/

		wlSet.m_cmdIndex = 2;
		
	}
	else
	{
		tmp1 = 0x80;
	}
	
	if(WORKINGMODE_TTL==mainPara.modGr[WL_ARRARY_B].mode)
	{
		//tmp2 = (para2>>1) +0x8d;
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp2 = (para2>>1) +0x8d;
			tmp2 = (para2>>1) + (unsigned int)(0xab - cmrPara.pIso);
		}else
		{
			//tmp2 = (para2>>1)+0x50;
			tmp2 = (para2>>1) + (unsigned int)(0x6e - cmrPara.pIso);
			//tmp2 |= 0x80;
		}
		//cmpTmp = tmp2+((wlSet.m_arrPara[WL_ARRARY_B].cmp1-3)*3+wlSet.m_arrPara[WL_ARRARY_B])*2;
		//cmpTmp = tmp2+(wlSet.m_arrPara[WL_ARRARY_B].cmp1*3+wlSet.m_arrPara[WL_ARRARY_B].cmp2)*2;
		tmp2 += (mainPara.modGr[WL_ARRARY_B].cmp1*3+mainPara.modGr[WL_ARRARY_B].cmp2)*2;
		
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//if(cmpTmp<0xa0)
			if(tmp2<0xa0)
			{
				tmp2 = 0xa0;
			}else if(tmp2>0xf0)//else if(cmpTmp>0xf0)
			{
				tmp2 = 0xf0;
			}/*else
			{
				tmp2 = cmpTmp; //(unsigned char)cmpTmp;
			}*/
		}else
		{
			//if(cmpTmp>0xBF)
			if(tmp2>0xBF)
			{
				tmp2 = 0xBF;
			}else if(tmp2<0x80)//else if(cmpTmp<0x80)
			{
				tmp2 |= 0x80; 
			}
		}
		//putchar(tmp2);
		wlSet.m_cmdIndex = 3;
	}//else
	else if(WORKINGMODE_MANU==mainPara.modGr[WL_ARRARY_B].mode)
	{
		tmp2 = (0x80+((MANU_CODE_MAX-mainPara.modGr[WL_ARRARY_B].output1)*3-(3+mainPara.modGr[WL_ARRARY_B].output2)%3)*2);
		wlSet.m_cmdIndex = 3;
		
	}
	else if(WORKINGMODE_AUTO==mainPara.modGr[WL_ARRARY_B].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			tmp2 = ((unsigned int)cmrPara.pApert + (unsigned int)(0xFD-cmrPara.pIso));
			//wlSet.m_cmdIndex = 3;
		}else
		{
			tmp2 = ((unsigned int)(cmrPara.pApert + cmrPara.pShutter) + (unsigned int)(0xC7-cmrPara.pIso));
			//wlSet.m_cmdIndex = 3;
		}

		//cmpTmp = tmp2+(wlSet.m_arrPara[WL_ARRARY_B].cmp1*3+wlSet.m_arrPara[WL_ARRARY_B].cmp2)*2;
		tmp2 += (mainPara.modGr[WL_ARRARY_B].a_cmp1*3+mainPara.modGr[WL_ARRARY_B].a_cmp2)*2;
		//if(cmpTmp>0xff)
		if(tmp2>0xff)
		{
			//tmp2 = 0xff;
			//tmp2 = ((cmpTmp&0xff) + 0x80);
			tmp2 = ((tmp2&0xff) + 0x80);
		}/*else
		{
			//tmp2 = cmpTmp;//(unsigned char)cmpTmp;
		}*/

		wlSet.m_cmdIndex = 3;
	}
	else
	{
		tmp2 = 0x80;
	}
	
	if(WORKINGMODE_TTL==mainPara.modGr[WL_ARRARY_C].mode)
	{
		//tmp3 = (para3>>1) +0x8d;
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp3 = (para3>>1) +0x8d;
			tmp3 = (para3>>1) + (unsigned int)(0xab-cmrPara.pIso);
		}else
		{
			//tmp3 = (para3>>1)+0x50;
			tmp3 = (para3>>1) + (unsigned int)(0x6e-cmrPara.pIso);
			//tmp3 |= 0x80;
		}
		//cmpTmp = tmp3+(wlSet.m_arrPara[WL_ARRARY_C].cmp1*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2)*2;
		tmp3 += (mainPara.modGr[WL_ARRARY_C].cmp1*3+mainPara.modGr[WL_ARRARY_C].cmp2)*2;
		
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//if(cmpTmp<0xa0)
			if(tmp3<0xa0)
			{
				tmp3 = 0xa0;
			}else if(tmp3>0xf0)//else if(cmpTmp>0xf0)
			{
				tmp3 = 0xf0;
			}
		}else
		{
			if(tmp3>0xBF)
			{
				tmp3 = 0xBF;
			}else if(tmp3<0x80)//else if(cmpTmp<0x80)
			{
				tmp3 |= 0x80; 
			}
		}
		wlSet.m_cmdIndex = 4;
	}//else
	else if(WORKINGMODE_MANU==mainPara.modGr[WL_ARRARY_C].mode)
	{
		tmp3 = (0x80+((MANU_CODE_MAX-mainPara.modGr[WL_ARRARY_C].output1)*3-(3+mainPara.modGr[WL_ARRARY_C].output2)%3)*2);
		wlSet.m_cmdIndex = 4;
	}
	else if(WORKINGMODE_AUTO==mainPara.modGr[WL_ARRARY_C].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			tmp3 = ((unsigned int)cmrPara.pApert + (unsigned int)(0xFD-cmrPara.pIso));
			//wlSet.m_cmdIndex = 4;
		}else
		{
			tmp3 = ((unsigned int)(cmrPara.pApert + cmrPara.pShutter) + (unsigned int)(0xC7-cmrPara.pIso));
			//wlSet.m_cmdIndex = 4;
		}

		tmp3 += (mainPara.modGr[WL_ARRARY_C].a_cmp1*3+mainPara.modGr[WL_ARRARY_C].a_cmp2)*2;
		//if(cmpTmp>0xff)
		if(tmp3>0xff)
		{
			//tmp3 = 0xff;
			//tmp3 = ((cmpTmp&0xff) + 0x80);
			tmp3 = ((tmp3&0xff) + 0x80);
		}

		wlSet.m_cmdIndex = 4;
	}
	else
	{
		tmp3 = 0x80;
	}

	if(((para0&0x0f)>=0x09)&&((para0&0x0f)<=0x0c))
	{
		if(0xc0==(para0&0xc0))
		{
			wlSet.m_flaCmd[0] = (0xE0 |(para0&0x0B));
		}else
		{
			wlSet.m_flaCmd[0] = (0xd0 |(para0&0x0B));
		}

		wlSet.m_flaCmd[1]  = tmp1;
		wlSet.m_flaCmd[2]  = tmp2;
		wlSet.m_flaCmd[3]  = tmp3;
		
	}else
	{
		if(0xc0==(para0&0xc0))			//flash lock
		{
			wlSet.m_flaCmd[0] = 0x01;
		}else if(0x80==(para0&0x80))
		{
			wlSet.m_flaCmd[0] = 0x03;
			wlSet.isRear=WLM_ISREAR_REAR;
		}else
		{
			wlSet.m_flaCmd[0] = 0x02;
		}

		wlSet.m_flaCmd[0] <<= 4;
		wlSet.m_flaCmd[0] |= ((tmp1>>4)&0x0f);
		
		wlSet.m_flaCmd[1] = (tmp1&0x0f);
		wlSet.m_flaCmd[1] <<= 4;
		wlSet.m_flaCmd[1] |= ((tmp2>>4)&0x0f);

		wlSet.m_flaCmd[2] = (tmp2&0x0f);
		wlSet.m_flaCmd[2] <<= 4;
		wlSet.m_flaCmd[2] |= ((tmp3>>4)&0x0f);

		wlSet.m_flaCmd[3] = (tmp3&0x0f);
		wlSet.m_flaCmd[3] <<= 4;

		wlSet.m_flaCmd[wlSet.m_cmdIndex-1] &= 0xf0; 
	}

	return wlSet.m_cmdIndex;

	
}

void NikRptFlashProc(void *pdata)
{
	while(1)
	{
		TakeRptFlash(&rptSet.finalExpTimes,rptSet.exp1*3+rptSet.exp2);
		OSTaskSuspend(RptFlash_TASK_Prio);
	}
}

char ExtmIsoIndex(u16 iso)
{
	u8 i=0;
	for(i=0;i<35;i++)
	{
		if(iso<=isoValue[i])
		{
			return i;
		}
	}
	return 0;
}

char ExtmApertIndex(u16 apert)
{
	u8 i=0;
	for(i=0;i<34;i++)
	{
		if(apert<=apertValue[i])
		{
			return i;
		}
	}
	return 0;
}

//void ExtModSet(void)
#define EXTM_DASET_STD 15//6//24//25//28//9
#define EXTM_PA_ST	17
/*ISO 100 APERT 5.6 1/128*/
//void ExtmModSet(signed char  foc, int iso, int apert)
u8 ExtmModSet(signed char  foc, u16 iso, u16 apert,s8 cmp1, s8 cmp2)
{

	//float multi=1;
	//float gn;
	u8 tmp;
	s8 para1=0;
//	char i=0;

	//float tmp=1;

	//char extVal = 0;

	//if(!motoSet.focIndex)

	para1+=EXTM_DASET_STD;

	if(!foc)
	{
		//motoMulti = 1;		
		para1 -= 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		//para1 = (motoSet.focIndex-1)/3;
		//para2 = ((motoSet.focIndex-1)%3);
		//para1 += (motoSet.focIndex-1);
		//para1 += (foc-1);
		//para1 += (foc);
		para1 -= GetMotoIncremental(foc);
	}

	//para1+=EXTM_DASET_STD;

#ifdef EXT_TEST_PRT
	DBG_PRINTF("para1 %d, iso %d, apert %d\r\n",para1,iso,apert);
#endif

	//tmp = ((flaPara.isoCmp>>4)*6)+(((flaPara.isoCmp&0x0F)+2)/3);
	//tmp = ExtmIsoIndex(mainPara.extIso);
	tmp = ExtmIsoIndex(iso);
	//para1 += tmp/3;
	//para2 += tmp%3;
	para1-=tmp;				//ISO

#ifdef EXT_TEST_PRT
	DBG_PRINTF("para1 %d\r\n",para1);
#endif

	//tmp = ExtmApertIndex(mainPara.extApert);
	tmp = ExtmApertIndex(apert);

	//para1 -= tmp/3;
	//para2 -= tmp%3;
	para1+=tmp;				//APERT

#ifdef EXT_TEST_PRT
	DBG_PRINTF("para1 %d\r\n",para1);
#endif

	para1+=(cmp1*3+cmp2);

/*
	if(para1<0)
	{
		para1 = EXT_VAL_LEN;
	}else if(para1>EXT_VAL_LEN)
	{
		para1 = 0;
	}else
	{
		para1 =EXT_VAL_LEN-para1;
	}
*/
	if(para1<0)
	{
		para1 = 0;
	}else if(para1>EXT_VAL_LEN)
	{
		para1 = EXT_VAL_LEN;
	}

#ifdef EXT_TEST_PRT
	DBG_PRINTF("extm %d\r\n",para1);
#endif

	if(para1>EXTM_PA_ST)
	{
		EXT_PA = 1;
#ifdef EXT_TEST_PRT
		DBG_PRINTF("EXT_PA\r\n");
#endif
	}else
	{
		EXT_PA = 0;
	}

	HpExpCodeSend(g_extExpCode[para1]<<4);	

#ifdef EXT_TEST_PRT
	DBG_PRINTF("extm %d\r\n",g_extExpCode[para1]);
#endif

	delay_us(100);
//	while(!PILOT_IN);
	return para1;

}

u8 SlaveAutoFlash(void)
{
	HpSetting hpSet;
	if(g_expTTLPara1>EXTM_PA_ST)
	{
		EXT_PA = 1;
	}else
	{
		EXT_PA = 0;
	}
	HpExpCodeSend(g_extExpCode[g_expTTLPara1]<<4);	

	delay_us(100);

	ENABLE_D30EN_EX();
	if(nikWireless.hpSyn>0)
	{
		HpExpCalc(g_expTTLPara1/3,g_expTTLPara1%3,0,0,0,&hpSet,0);
		HighSpeedTriger(&hpSet,HP_REALFLA_INTERVAL);
	}else
	{
		ExtModFlash(5000);
	}
	DISABLE_D30EN_EX();

	return g_expTTLPara1;
}

