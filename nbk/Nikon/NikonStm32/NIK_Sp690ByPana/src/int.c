#include "sr101ef51a.h"
#include "mn101ef52a.h"
//#include "oloong.h"
#include "init.h"
//#include "Golbal.h"
#include "output.h"
#include "nikon.h"
#include "uart.h"

#ifdef _CANON_WIRELESS_
	#define WL_BITJUDGE_CODE0	163//(188-41)//188//150
	#define WL_BITJUDGE_CODE1	300//(335-41)//344//275
	#define WL_BITJUDGE_CODE2	413//(436-41)//500//400
	#define WL_BITJUDGE_CODE3	538//(580-41)//656//525
	#define WL_BITJUDGE_CODE4	663//(700-41)//813//650
	#define WL_BITJUDGE_CODE5	788//(820-41)//906//725
	#define WL_BITJUDGE_CODE6	913//(940-41)//1063//850
	#define WL_BITJUDGE_CODE7   1038//(1060-41)//1125//900
#endif

#ifdef _NIKON_WIRELESS_
	#define WL_BITJUDGE_CODE0	275//304//450
	#define WL_BITJUDGE_CODE1	463//506//750
	#define WL_BITJUDGE_CODE2	650//708//1050
	#define WL_BITJUDGE_CODE3	838//878//1300
	#define WL_BITJUDGE_CODE4	1025//1047//1550
	#define WL_BITJUDGE_CODE5	1213//1215//1800
	#define WL_BITJUDGE_CODE6	1400//1384//2050
	#define WL_BITJUDGE_CODE7  1588//1553//2300
	#define WL_BITJUDGE_CODE8	1775//1723
#endif

unsigned int pTime;
//extern char sleep_debug;
#ifdef _CANON_WIRELESS_
unsigned int wl_buf[WL_BUF_SIZE];
int wlBufSize=0;
#endif

extern unsigned char bitFlag;
extern unsigned char bitWireless;
extern unsigned char bitIndex;
extern unsigned char g_flagAftFlash;
extern UartSendFlag uartSendFlag;
unsigned char g_f2FlashLabel=0;
//unsigned char g_f2FlashCnt=0;
extern HpSetting hpSetting;
extern unsigned char xdata tpfRsv;
extern unsigned char flaLockFlag;

#ifdef WIRELESS_DEBUG
	unsigned char wlDbgPrt=0;
#endif


const unsigned char wl_opArr[] = {0x03,0x05,0x06,0x07,0x09,0x0a,0x0b,0x0d,0x0e,0x0f,0x11,0x12,0x13,0x15,0x16,0x17,0x19,0x1a,0x1b,0x1d,0x1e,0x1f,};


void Delay(void);

extern char com_statue;
unsigned char charge_ok=0;
#ifdef _CANON_WIRELESS_
CaWireless canWireless;
#endif
#ifdef _NIKON_WIRELESS_
extern NiWireless nikWireless;
extern FLASH_STATUE xdata flashStatue;
extern unsigned char xdata ttl_pre2_flag;
extern EXPOSE_PARA xdata flashPara;
extern unsigned char xdata bitChar;
extern UART_SEND xdata uartSend;
extern MultiSet xdata multiSet;
extern unsigned char WrongCodeFilter;
extern unsigned int xdata lowBatCount;
extern CAMER_SETTING xdata cameraSetting;
#endif
extern Wl_set wlSet;
extern char HpExpCalc(char exp1,char exp2,char cmp1,char cmp2,unsigned int piso,HpSetting *hpSt,unsigned char ttlAccFlag);



#pragma	_interrupt int_Ext0_SCKINT
#pragma _interrupt int_Ext1_FLASHINT
#pragma _interrupt int_Ext2_FlwFlash
#pragma _interrupt int_Ext3_WakeUp



void int_Ext0_SCKINT(void)
{
	if(SLEEP_MODE_ON==flashPara.sleep_flag)
	{
		//flashPara.sleep_flag = SLEEP_MODE_OFF;
		//NIKONNSSO = 1;
		SET_WAKEUP_OUT;
		flashPara.sleep_flag = SLEEP_MODE_OFF;
		SET_SBT_OUT;
		WAKE_UP = 0;
		SET_WAKEUP_IN;
		/*
		disable_irq();	
		MEMCTR   |=  0x04;
		IRQ0ICR = 0X00;
		IRQ1ICR = 0X82;
		IRQ3ICR = 0X00;
		MEMCTR   &= ~0x04;
		enable_irq();
		*/
	}
}

void int_Ext1_FLASHINT(void)
{
	if(SLEEP_MODE_ON==flashPara.sleep_flag)
	{
		SET_WAKEUP_OUT;
		flashPara.sleep_flag = SLEEP_MODE_OFF;
		SET_SBT_OUT;
		WAKE_UP = 0;
		SET_WAKEUP_IN;
		return;
		/*
		disable_irq();	
		MEMCTR   |=  0x04;
		IRQ0ICR = 0X00;
		IRQ1ICR = 0X82;
		IRQ3ICR = 0X00;
		MEMCTR   &= ~0x04;
		enable_irq();
		*/
	}

	if(flashPara.ledFlag)
	{
		return;
	}
		
	if(flashPara.workmode==WORKINGMODE_MANU)
	{
		if(FLASHCTL!=0)
		{
			return;
		}
		if(flashStatue.flash_enable!=0x01)
		{
			//DecodeFlashData(0xbb,flashStatue.flash_enable,flashStatue.flash_enable);
			return;
		}

		//flashTime = OutputSwitch(WORKINGMODE_MANU);
		//uartSend.ISO = flashTime;
		//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ISO);
		if(LOW_SPEED==flashPara.fp)
		{
			HaltInterval(12);
			//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_MANU))
			pTime = OutputSwitch(WORKINGMODE_MANU);
			TURN_ON_TRIGER(pTime);
			//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
			//g_flagAftFlash = 1;
	//		while(NIKONMISO);
	//		while(!NIKONMISO);
		}else
		{
			//HighSpeedTriger(100);
//#ifdef HIGH_SPEED_SYNCHRONIC
#ifdef _HP_SYN_
			//hpSetting= HpExpCalc(flashPara.output1,flashPara.output2);
			HpExpCalc(flashPara.output1,flashPara.output2,0,0,0,&hpSetting,ttl_pre2_flag);
			HighSpeedTriger(&hpSetting,HIGH_SPEED_OVERFLOW);
//#else
#else
			HaltInterval(12);
			//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_MANU))
			pTime = OutputSwitch(WORKINGMODE_MANU);
			TURN_ON_TRIGER(pTime);
#endif
		}
		flashPara.ExposeState = 0;
		uartSendFlag.flashWarn = 1;
		uartSendFlag.clear = 1;
		//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);	
		//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
//			SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
		//InitUart();

	}else if(flashPara.workmode==WORKINGMODE_TTL)
	{
		if(FLASHCTL!=0)
		{
			return;
		}  
		if((!GETBIT(bitChar,BIT_BITCHAR_PRE2EXPOSE))&&(!GETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE))&&(!GETBIT(bitFlag,BIT_BITFLAG_FLASHLOCK)))
		{
			return;	  
		}  
		if(flashStatue.flash_enable!=0x01)
		{
			//DecodeFlashData(0xbb,flashStatue.flash_enable,flashStatue.flash_enable);
			return;
		} 
		if(flaLockFlag)
		{
			ttl_pre2_flag = tpfRsv;
		}
		//flashTime = OutputSwitch(WORKINGMODE_TTL);
		if(LOW_SPEED==flashPara.fp)
		{
			HaltInterval(12);
			//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_TTL))
			pTime = OutputSwitch(WORKINGMODE_TTL);
			TURN_ON_TRIGER(pTime);
		}
		else
		{
//#ifdef HIGH_SPEED_SYNCHRONIC
#if(_HP_SYN_MODE_==HP_SYN_MODE_SW)
			//pTime = HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2);
			HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,flashPara.ttlcomp1,flashPara.ttlcomp2,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
			HighSpeedTriger(&hpSetting,HIGH_SPEED_OVERFLOW);
//#else
#elif(_HP_SYN_MODE_==HP_SYN_MODE_NO)
			HaltInterval(12);
			//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_TTL))
			pTime = OutputSwitch(WORKINGMODE_TTL);
			TURN_ON_TRIGER(pTime);
#elif(_HP_SYN_MODE_==HP_SYN_MODE_HW)
			HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,flashPara.ttlcomp1,flashPara.ttlcomp2,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
			HighSpeedTriger(&hpSetting,HIGH_SPEED_OVERFLOW);
#endif
		}
		//CLRBIT(bitChar,BIT_BITCHAR_PRE2EXPOSE);
		flashPara.ExposeState = 0;
		//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
		CLRBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE);
		CLRBIT(bitChar,BIT_BITCHAR_PRE2EXPOSE);
		CLRBIT(bitFlag,BIT_BITFLAG_FLASHLOCK);
		uartSendFlag.clear = 1;
		uartSendFlag.flashWarn = 1;
		//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
		//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
//		SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
		ttl_pre2_flag = UART_ENCODE_DEBUG_EXPOSE;
//		g_flagAftFlash = 1;
//		while(NIKONMISO);
//		while(!NIKONMISO);
		//InitUart();
		//ES = 1;
#ifdef TTL_DEBUG
		CLRBIT(bitChar,BIT_BITCHAR_DEBUGERROR);
#endif
	}else if( WORKINGMODE_MULTI==flashPara.workmode)
	{

		if(FLASHCTL!=0)
		{
			return;
		}

		if(flashStatue.flash_enable!=0x01)
		{
			//DecodeFlashData(0xbb,flashStatue.flash_enable,flashStatue.flash_enable);
			return;
		} 
		

		if(!uartSend.cls)
		{
			multiSet.finalExpTimes = multiSet.eTimes;
			multiSet.cMode = MULTI_MODE_UNCAM;
		}else
		{
			if((unsigned long)((1000/multiSet.frequence)*multiSet.eTimes)<(unsigned long)cameraSetting.shutter)
			{
				multiSet.finalExpTimes = multiSet.eTimes;
				//putchar(multiSet.finalExpTimes);
			}else
			{
				if((((unsigned long)cameraSetting.shutter*(unsigned long)multiSet.frequence)/1000))
				{
					multiSet.finalExpTimes = (int)((((unsigned long)cameraSetting.shutter*(unsigned long)multiSet.frequence)/1000));
				}else
				{
					multiSet.finalExpTimes = (int)(((((unsigned long)cameraSetting.shutter*(unsigned long)multiSet.frequence))/1000)-1);
				}
			}
			//multiSet.finalExpTimes = multiSet.eTimes;
			multiSet.cMode = MULTI_MODE_CAM;
		}

		



		//Tprintf("%d %d",(int)(1000/multiSet.frequence*multiSet.eTimes),cameraSetting.shutter);
		//Tprintf("%d ",multiSet.finalExpTimes);

		//putchar(multiSet.finalExpTimes>>8);
		//putchar(multiSet.finalExpTimes&0xff);
		
		//UPDATA_TIMER0(0xf5,0x36);
		//START_TIMER0;
		/*
		if(multiSet.finalExpTimes>30)
		{
			multiSet.frqMode = MULTI_FRQMODE_HIGH;
			UPDATA_TIMER0((1845000/multiSet.frequence)>>8,(1845000/multiSet.frequence)&0xff);
			START_TIMER0;
		else
		{
			multiSet.frqMode = MULTI_FRQMODE_LOW;
			multiSet.lowFrqCnt = (1000/multiSet.frequence);
			UPDATA_TIMER0(0XF8,0XCB);
			START_TIMER0;
		}
		*/

		//multiSet.finalExpTimes = multiSet.eTimes;
		
		pTime = OutputSwitch(WORKINGMODE_MULTI);
		TURN_ON_TRIGER(pTime);
		multiSet.finalExpTimes--;
		multiSet.lowFrqCnt = (1000/multiSet.frequence);	
		//UPDATA_TIMER0(0XF8,0XCB);
		//STOP_PCA_TIMER
		multiSet.mulCtl=MULTI_FLASH_ENABLE;
		//mulTmp = MAKEWORD(CL,CH);
		//mulTmp += CHANNEL1_1MS;
		//START_PCA_TIMER;
		//MUL_PCATIMER_SETVALUE(mulTmp>>8, mulTmp&0XFF);
		multiSet.mulCnt = 0;
		//MUL_PCATIMER_START;
		multiSet.startFlag = MULTI_STFLAG_START;
		if(multiSet.finalExpTimes<=0)
		{
			//MUL_PCATIMER_STOP
			multiSet.mulCtl=MULTI_FLASH_DISABLE;
			multiSet.finalExpTimes=0;
			multiSet.startFlag = MULTI_STFLAG_HOLD;
			uartSendFlag.clear = 1;
			uartSendFlag.flashWarn = 1;
			//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
			//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
			//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
			//InitUart();
		}
		//g_flagAftFlash = 1;
		//putchar(multiSet.finalExpTimes>>8);
		//putchar(multiSet.finalExpTimes&0xff);

		//START_TIMER0;

		//putchar(multiSet.lowFrqCnt>>8);
		//putchar(multiSet.lowFrqCnt&0xff);
		//putchar(multiSet.finalExpTimes);
		//putchar(multiSet.lowFrqCnt);
	}else if(WORKINGMODE_MASTER==flashPara.workmode)
	{
		if(wlSet.m_sendCmdEn)
		{

			if(WLM_ISREAR_REAR==wlSet.isRear)
			{
				wlSet.m_flaCmd[0] = 0x70;	
				WLM_SendData(&wlSet.m_flaCmd[0],1);
			}else
			{

				//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
				switch(wlSet.m_arrPara[WL_ARRARY_M].mode)
				{
					case WORKINGMODE_TTL:
					case WORKINGMODE_AUTO:
						if(LOW_SPEED==flashPara.fp)
						{
							HaltInterval(12);
							//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_TTL))
							pTime = OutputSwitch(WORKINGMODE_TTL);
							TURN_ON_TRIGER(pTime);
							//TrigerMasterOn(pTime);
							//TURN_ON_TRIGER(pTime);
						}
						else
						{
//#ifdef HIGH_SPEED_SYNCHRONIC
#ifdef _HP_SYN_
							//pTime = HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2);
							//HpExpCalc(flashPara.output1,flashPara.output2,&hpSetting);
							//HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,flashPara.ttlcomp1,flashPara.ttlcomp2,uartSend.ISO_index,&hpSetting);
							//HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,flashPara.ttlcomp1,flashPara.ttlcomp2,uartSend.ISO_index,&hpSetting);
							TrigerMasterOn(0);
							HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,flashPara.ttlcomp1,flashPara.ttlcomp2,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
							HighSpeedTriger(&hpSetting,HIGH_SPEED_OVERFLOW);
#else
#endif
						}
					break;

					case WORKINGMODE_MANU:
						if(LOW_SPEED==flashPara.fp)
						{
							HaltInterval(12);
							//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_MANU))
							pTime = OutputSwitch(WORKINGMODE_MANU);
							//TURN_ON_TRIGER(pTime);
							TrigerMasterOn(pTime);
						}else
						{
//#ifdef HIGH_SPEED_SYNCHRONIC
#ifdef _HP_SYN_
							//HighSpeedTriger(100);
							//pTime = HpExpCalc(flashPara.output1,flashPara.output2);
							//HpExpCalc(flashPara.output1,flashPara.output2,&hpSetting);
							//HpExpCalc(wlSet.m_arrPara[WL_ARRARY_M].output1,wlSet.m_arrPara[WL_ARRARY_M].output2,0,0,0,&hpSetting);
							//HighSpeedTriger(&hpSetting,HIGH_SPEED_OVERFLOW);
							TrigerMasterOn(0);
							HpExpCalc(wlSet.m_arrPara[WL_ARRARY_M].output1,wlSet.m_arrPara[WL_ARRARY_M].output2,0,0,0,&hpSetting,ttl_pre2_flag);
							HighSpeedTriger(&hpSetting,HIGH_SPEED_OVERFLOW);
#else
#endif
						}						
					break;

					case WORKINGMODE_TURNOFF:
						//TURN_ON_TRIGER(WLM_HEAD_TIME+WLM_HEAD_TIME);
						//WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_HEAD_TIME);
						TrigerMasterOn(0);
					break;
					
				}
				//TURN_ON_TRIGER(WLM_HEAD_TIME);
			}
			SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
			uartSendFlag.clear = 1;
			uartSendFlag.flashWarn= 1;
			wlSet.m_sendCmdEn = 0;
			//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
			//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
		}
		WrongCodeFilter = HEAD_DATA_UNRCVD;
	}
}

void int_Ext2_FlwFlash(void)
{
#ifdef _NIKON_WIRELESS_
//	static unsigned int tmrRcd;
	static unsigned int WL_chan1=0,WL_chan2=0,plusWidth=0;
	static unsigned char wl_preIndex=0;


	if(WORKINGMODE_RTN==flashPara.workmode)
	{

		if(nikWireless.WL_statue == NIKWL_STATUE_HOLD)
		{
			
			//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
			nikWireless.PCA_timer_record = TM7IC;
			
#ifdef WIRELESS_MEASURE
			nikWireless.WL_statue = NIKWL_STATUE_PLUSMEASURE;
#else
			nikWireless.WL_statue = NIKWL_STATUE_ENSURECHAN;
#endif
			
			SET_WL_FLASH_TIMEOUT;

			 return;

		}else if(nikWireless.WL_statue == NIKWL_STATUE_ENSURECHAN)
		{
			//UPDATA_TIMER0(0xf8,0xcf);
			if(nikWireless.WL_Channel==WL_CHANNEL_UNSURE)
			{
//				STOP_PCA_TIMER
//				WL_chan1 = (unsigned int)(MAKEWORD(CL,CH) - nikWireless.PCA_timer_record);
//				START_PCA_TIMER

				WL_chan1 = TM7IC - nikWireless.PCA_timer_record;
				if(WL_chan1>WL_MASTER_PREFLA_WIDTH)
				{
					nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
					nikWireless.PCA_timer_record = TM7IC;
					//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
					SET_WL_FLASH_TIMEOUT
				}else
				{
					nikWireless.WL_Channel = WL_CHANNEL_MAKESURE;
					nikWireless.PCA_timer_record = TM7IC;
					//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
				}

				//nikWireless.WL_Channel = WL_CHANNEL_MAKESURE;

				//nikWireless.PCA_timer_record = TM7IC;
				
				//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
				//SETBIT(bitFlag,BIT_BITFLAG_WLTIMEOUT);
				//nikWireless.WL_timeout = 0;
				return;
			}else if(nikWireless.WL_Channel == WL_CHANNEL_MAKESURE)
			{
//				WL_chan2 = (unsigned int)((MAKEWORD(CL,CH) - nikWireless.PCA_timer_record));
				WL_chan2 = TM7IC - nikWireless.PCA_timer_record;
				nikWireless.PCA_timer_record = TM7IC;

				if(nikWireless.preTurn== WL_PFCOUNT_TWO)
				{
					nikWireless.preTurn=WL_PFCOUNT_RDY;
					nikWireless.preTotal --;
					if(nikWireless.preTotal==0)
					{
						wlSet.s_flashStatue = WL_FLASH_STATUE_AFTPF;					
					}
				}
								
				//nikWireless.WL_Channel = MAKEWORD(WL_chan1,WL_chan2);
				//nikWireless.WL_buf[nikWireless.WLBufIndex++] = WL_chan1;
				//nikWireless.WL_buf[nikWireless.WLBufIndex++] = WL_chan2;

				//STOP_PCA_TIMER
				//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
				//START_PCA_TIMER

				if((WL_CHAN_SCALE1_GND<WL_chan1)&&(WL_chan1<WL_CHAN_SCALE1_ROOF))
				{
					if((WL_CHAN_SCALE3_GND<WL_chan2)&&(WL_chan2<WL_CHAN_SCALE3_ROOF))
					{
						nikWireless.WL_Channel = WL_CHANNEL_ONE;
					}else if((WL_CHAN_SCALE4_GND<WL_chan2)&&(WL_chan2<WL_CHAN_SCALE4_ROOF))
					{
						nikWireless.WL_Channel = WL_CHANNEL_FOUR;
					}

				}else if((WL_CHAN_SCALE2_GND<WL_chan1)&&(WL_chan1<WL_CHAN_SCALE2_ROOF))
				{
					if((WL_CHAN_SCALE1_GND<WL_chan2)&&(WL_chan2<WL_CHAN_SCALE1_ROOF))
					{
						nikWireless.WL_Channel = WL_CHANNEL_TWO;
					}else if((WL_CHAN_SCALE3_GND<WL_chan2)&&(WL_chan2<WL_CHAN_SCALE3_ROOF))
					{
						nikWireless.WL_Channel = WL_CHANNEL_THREE;
					}
				}
					
				//if(cameraSetting.wl_chan==nikWireless.WL_Channel)
				if(wlSet.s_chan==nikWireless.WL_Channel)
				{
					nikWireless.WL_statue = NIKWL_STATUE_RUN;
					if(wlSet.s_flashStatue==WL_FLASH_STATUE_HOLD)
					{
						wlSet.s_flashStatue = WL_FLASH_STATUE_START;
					}
					nikWireless.bitLoadIndex = 0;
					nikWireless.WLBufIndex = 0;
					//UPDATA_TIMER0(0xf5,0x36);
					UPDATA_TIMER0(WL_RCVTIME_OUT);
					START_TIMER0

					WL_chan1 = WL_chan2 = 0;
					//if(wlSet.s_flashStatue==WL_FLASH_STATUE_AFTPF)
					//{
						//WLDEBUG_BREAK_POINT	
					//}
				}else
				{
/*				
#ifdef WIRELESS_DEBUG
					Debug_c1 = wlSet.s_chan;
					Debug_c2 = nikWireless.WL_Channel;
					Debug_chan1 = WL_chan1;
					Debug_chan2 = WL_chan2;
#endif
*/
					
					nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
					nikWireless.WL_statue = NIKWL_STATUE_HOLD;
					wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
					nikWireless.bitLoadIndex = 0;
					nikWireless.WLBufIndex = 0;
					WL_chan1 = WL_chan2 = 0;
				}	

				//CLRBIT(bitFlag,BIT_BITFLAG_WLTIMEOUT);
				//nikWireless.WL_timeout = 0;
				//return;
				//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
				//nikWireless.WL_statue = NIKWL_STATUE_START;			
			}
			//nikWireless.WL_buf[nikWireless.WLBufIndex++] = ( CH<<8 | CL ) - nikWireless.PCA_timer_record;
		}
		else if(nikWireless.WL_statue== NIKWL_STATUE_START)
		{
			//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
			nikWireless.PCA_timer_record = TM7IC;
			nikWireless.WL_statue = NIKWL_STATUE_RUN;
			nikWireless.bitLoadIndex = 0;
			nikWireless.WLBufIndex = 0;
			//UPDATA_TIMER0(0xf5,0x36);
			UPDATA_TIMER0(180);
			START_TIMER0
			//return;
		}else if(nikWireless.WL_statue==NIKWL_STATUE_RUN)
		{
			
		//	nikWireless.WL_buf[nikWireless.WLBufIndex++] = MAKEWORD(CL,CH)- nikWireless.PCA_timer_record;
		//	STOP_PCA_TIMER
		//	plusWidth = MAKEWORD(CL,CH) - nikWireless.PCA_timer_record;
		//	START_PCA_TIMER

			plusWidth = TM7IC - nikWireless.PCA_timer_record;

#ifdef _TRACE_
			if(TraTimeIndex<TRACE_SIZE)
			{
				TraTime[TraTimeIndex++] = plusWidth;
			}
#endif

			if(plusWidth<WL_BITJUDGE_CODE0)
			{
				nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(7-nikWireless.bitLoadIndex));
				nikWireless.bitLoadIndex++;
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
				TraArr[TraIndex++] = 0x11;
#endif
				if(nikWireless.bitLoadIndex>=8)
				{
					nikWireless.bitLoadIndex=0;
					nikWireless.WLBufIndex++;
				}
				
/*				nikWireless.WL_buf[nikWireless.WLBufIndex] <<= 1;
				nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
				if(++nikWireless.bitLoadIndex>=8)
				{
					nikWireless.bitLoadIndex = 0;
					nikWireless.WLBufIndex++;
				}*/

			}else if(plusWidth<WL_BITJUDGE_CODE1)
			{

				if(nikWireless.bitLoadIndex>6)
				{
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (2-(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));	
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x20;
#endif
					
				}else if(nikWireless.bitLoadIndex==6)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(6-nikWireless.bitLoadIndex));
					nikWireless.bitLoadIndex+=2;
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x21;
#endif
				}
			
/*				if(nikWireless.bitLoadIndex>6)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= (8-nikWireless.bitLoadIndex);
					nikWireless.WLBufIndex ++;
					//nikWireless.WL_buf[nikWireless.WLBufIndex] |= (0x02>>(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(7-nikWireless.bitLoadIndex));
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (2-(8-nikWireless.bitLoadIndex));
				}else 
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= 2;
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01;
					nikWireless.bitLoadIndex+=2;
				}*/
			}else if(plusWidth<WL_BITJUDGE_CODE2)
			{
				if(nikWireless.bitLoadIndex>5)
				{
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (3-(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x30;
#endif
				}else if(nikWireless.bitLoadIndex==5)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(5-nikWireless.bitLoadIndex));
					nikWireless.bitLoadIndex+=3;
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x31;
#endif
				}
/*				if(nikWireless.bitLoadIndex>5)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= (8-nikWireless.bitLoadIndex);
					//nikWireless.WL_buf[nikWireless.WLBufIndex] |= (0x04>>(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(7-nikWireless.bitLoadIndex));
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (3-(8-nikWireless.bitLoadIndex));
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= 3;
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x04;
					nikWireless.bitLoadIndex+=3;
				}*/
			}else if(plusWidth<WL_BITJUDGE_CODE3)
			{
				if(nikWireless.bitLoadIndex>4)
				{
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (4-(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x40;
#endif
				}else if(nikWireless.bitLoadIndex==4)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(4-nikWireless.bitLoadIndex));
					nikWireless.bitLoadIndex+=4;
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x41;
#endif
				}
				
/*				if(nikWireless.bitLoadIndex>4)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= (8-nikWireless.bitLoadIndex);
					//nikWireless.WL_buf[nikWireless.WLBufIndex] |= (0x08>>(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(7-nikWireless.bitLoadIndex));
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (4-(8-nikWireless.bitLoadIndex));
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= 4;
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x08;
					nikWireless.bitLoadIndex+=4;
				}*/
			}else if(plusWidth<WL_BITJUDGE_CODE4)
			{
				if(nikWireless.bitLoadIndex>3)
				{
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (5-(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x50;
#endif
				}else if(nikWireless.bitLoadIndex==3)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(3-nikWireless.bitLoadIndex));
					nikWireless.bitLoadIndex+=5;
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x51;
#endif
				}
				
/*				if(nikWireless.bitLoadIndex>3)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= (8-nikWireless.bitLoadIndex);
					//nikWireless.WL_buf[nikWireless.WLBufIndex] |= (0x10>>(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(7-nikWireless.bitLoadIndex));
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (4-(8-nikWireless.bitLoadIndex));

				}else{
					nikWireless.WL_buf[nikWireless.WLBufIndex] <<= 5;
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x10;
					nikWireless.bitLoadIndex+=5;
				}*/
			}else if(plusWidth<WL_BITJUDGE_CODE5)
			{
				if(nikWireless.bitLoadIndex>2)
				{
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (6-(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x50;
#endif
				}else if(nikWireless.bitLoadIndex==2)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(2-nikWireless.bitLoadIndex));
					nikWireless.bitLoadIndex+=6;
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x51;
#endif
				}
			}else if(plusWidth<WL_BITJUDGE_CODE6)
			{
				if(nikWireless.bitLoadIndex>1)
				{
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = (7-(8-nikWireless.bitLoadIndex));
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x50;
#endif
				}else if(nikWireless.bitLoadIndex==1)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}else
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(1-nikWireless.bitLoadIndex));
					nikWireless.bitLoadIndex+=7;
#ifdef _TRACE_
					if(TraIndex<TRACE_SIZE)
						TraArr[TraIndex++] = 0x51;
#endif
				}
			}else if(plusWidth<WL_BITJUDGE_CODE7)
			{
				if(nikWireless.bitLoadIndex>0)
				{
					nikWireless.WLBufIndex++;
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= (1<<(8-nikWireless.bitLoadIndex));
				}else if(nikWireless.bitLoadIndex==0)
				{
					nikWireless.WL_buf[nikWireless.WLBufIndex] |= 0x01; 
					nikWireless.WLBufIndex++;
					nikWireless.bitLoadIndex = 0;
				}
				
			}else if(plusWidth<WL_BITJUDGE_CODE8)
			{
				nikWireless.WLBufIndex++;
				nikWireless.WL_buf[nikWireless.WLBufIndex] = (1<<(7-nikWireless.bitLoadIndex));
			}else 
			{
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
				nikWireless.bitLoadIndex = 0;
				nikWireless.WLBufIndex = 0;
			}
			
//			STOP_PCA_TIMER;
//			nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
//			START_PCA_TIMER;

			nikWireless.PCA_timer_record = TM7IC;
			
//			UPDATA_TIMER0(0xf5,0x36);//
			UPDATA_TIMER0(WL_RCVTIME_OUT);
			//return;
		}else if(nikWireless.WL_statue==NIKWL_STATUE_PLUSMEASURE)
		{
			//STOP_PCA_TIMER
			//nikWireless.WL_buf[nikWireless.WLBufIndex++] = MAKEWORD(CL,CH)- nikWireless.PCA_timer_record;
			//START_PCA_TIMER
			nikWireless.WL_buf[nikWireless.WLBufIndex++] = TM7IC - nikWireless.PCA_timer_record;

			if(nikWireless.WLBufIndex>=WLBUF_LEN)
			{
				//nikWireless.WL_statue = NIKWL_STATUE_START;
				nikWireless.WL_statue = NIKWL_STATUE_STOP;
				nikWireless.PCA_timer_record = 0;
				//WLReady = 1;
			}

			//STOP_PCA_TIMER
			//nikWireless.PCA_timer_record = ( CH<<8 | CL );
			//START_PCA_TIMER

			nikWireless.PCA_timer_record = TM7IC;
			
			//UPDATA_TIMER0(0xf5,0x36);
			UPDATA_TIMER0(WL_RCVTIME_OUT);
			
		}else if(nikWireless.WL_statue==NIKWL_STATUE_PREFLASH)
		{			
			if(nikWireless.preTurn == WL_PFCOUNT_ONE)
			{

				if(!wl_preIndex)
				{
					//STOP_PCA_TIMER
					//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
					//START_PCA_TIMER
					nikWireless.PCA_timer_record = TM7IC;
					wl_preIndex = 1;

				}else
				{

					//STOP_PCA_TIMER
					//plusWidth = MAKEWORD(CL,CH) - nikWireless.PCA_timer_record;
					//START_PCA_TIMER
					plusWidth = TM7IC - nikWireless.PCA_timer_record;
					nikWireless.PCA_timer_record = TM7IC;
					wl_preIndex = 0;
//					Debug_preWidth = plusWidth;
//					WLDEBUG_BREAK_POINT
					if((WL_PREWIDTH_GND<=plusWidth)&&(plusWidth<=WL_PREWIDTH_ROOF))
					{
					
						//HaltInterval(42);
						Delay_10us(20);				//delay for 450us		//some problem here
						//Delay_10us(20);	
						//HaltInterval(20);
						
						//TURN_ON_TRIGER(OUTPUT_30);

						if(nikWireless.preArr==wlSet.s_arrary)
						{
							//puts("pre1 \n");
							//if(flashStatue.flash_enable==0x01)
							//if((charge_ok!=AD_CHARGE_LOW)&&(GETBIT(bitIndex,BIT_bitIndex_PROTECT)==0))
							if(flashStatue.flash_enable==0x01)
							{
								//TAKE_PREEXPOSE1
								TURN_ON_TRIGER(400-1);		//40us
								nikWireless.flash_wizard = WL_WIZARD_PRE;
								//ttl_pre2_flag = 0;
								//HaltInterval(50);
							#if 0
								Delay_10us(50);
								disable_irq();	
								MEMCTR   |=  0x04;
								CLRBIT(IRQ2ICR,0);
								MEMCTR   &= ~0x04;
								enable_irq();
							#endif 
								//IE0 = 0;
							}
							ttl_pre2_flag = 0;
							tpfRsv = 0;
							
						}
//delay, aviod intervence
						Delay_10us(50);
						disable_irq();	
						MEMCTR   |=  0x04;
						//CLRBIT(IRQ2ICR,0);
						IRQ2ICR = 0X82;
						MEMCTR   &= ~0x04;
						enable_irq();
//after a previous flash, it should handle respectively. Sometimes it followed a secondely previous flash. So it should record
//the time at this point
						//STOP_PCA_TIMER
						//nikWireless.PCA_timer_record= (unsigned int)(MAKEWORD(CL,CH) - nikWireless.PCA_timer_record);
						//START_PCA_TIMER
						nikWireless.PCA_timer_record = TM7IC;// - nikWireless.PCA_timer_record;
						nikWireless.preTurn = WL_PFCOUNT_TWO;
						//ttl_pre2_flag = 0;
						//tpfRsv = 0;
						//STOP_TIMER0
					}else
					{
						//nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
						//nikWireless.WL_statue = NIKWL_STATUE_HOLD;
						//wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
						CLR_WL_FLASH_TIMEOUT;
						nikWireless.bitLoadIndex = 0;
						nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
						nikWireless.WL_statue = NIKWL_STATUE_HOLD;
						wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;	
						nikWireless.WLBufIndex = 0;
						nikWireless.flash_wizard = WL_WIZARD_HOLD;
						wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						nikWireless.preArr = WL_ARRARY_NULL;
						nikWireless.preTurn=WL_PFCOUNT_RDY;
						CLR_WLN_BUF
					}
				}
			}else if(nikWireless.preTurn==WL_PFCOUNT_TWO)
			{	

				if(!wl_preIndex)
				{
					//STOP_PCA_TIMER
					//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
					//START_PCA_TIMER
					nikWireless.PCA_timer_record = TM7IC; 
					wl_preIndex = 1;

/*				
					STOP_PCA_TIMER
					//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
					WL_chan1 = MAKEWORD(CL,CH) - nikWireless.PCA_timer_record;
					nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
					STOP_PCA_TIMER
//after the first previous flash, it is a critical problem to judge whether it is the signal of the second previous flash
//or a new data change, so it set the timer, if the timer overflow, it judged as a second previous flash, otherwise data transefer.
//the timer overflow set is 480us
					nikWireless.WL_statue = NIKWL_STATUE_ENSURECHAN;
					nikWireless.WL_Channel = WL_CHANNEL_MAKESURE;
					UPDATA_TIMER0(0XFC,0X8E); 
					START_TIMER0
					wl_preIndex = 1;
*/
				}else
				{
					//STOP_PCA_TIMER
					//plusWidth = MAKEWORD(CL,CH) - nikWireless.PCA_timer_record;
					//nikWireless.PCA_timer_record = MAKEWORD(CL,CH);
					//WL_chan1 = plusWidth;
					//START_PCA_TIMER
					plusWidth = TM7IC - nikWireless.PCA_timer_record;
					nikWireless.PCA_timer_record = TM7IC;
					WL_chan1 = plusWidth;
					wl_preIndex = 0;
					STOP_TIMER0;

					//if((WL_PREWIDTH_GND<=plusWidth)&&(plusWidth<=WL_PREWIDTH_ROOF))
					//{
//in this point, it is need to judge whether the next occured is a secondly previous flash or a next communication
//the handle method is start the timer0, if the timer0 overflows, it prove to be a previous flash, otherwise, record the
//plus width as the channel judger.
					nikWireless.WL_statue = NIKWL_STATUE_ENSURECHAN;
					nikWireless.WL_Channel = WL_CHANNEL_MAKESURE;
					nikWireless.WLBufIndex = 0;
					nikWireless.bitLoadIndex = 0;
					//UPDATA_TIMER0(0XFC,0X8E);
					UPDATA_TIMER0(WL_PF_JUDGE);//0.8us*400//470us in order to judge whether is the seconde preflash
					START_TIMER0
					//}else 
					//{
					//	nikWireless.WL_statue = NIKWL_STATUE_HOLD;
					//	nikWireless.WL_Channel = WL_CHANNEL_MAKESURE;
					//	nikWireless.bitLoadIndex = 0;
					//	nikWireless.WLBufIndex = 0;
					//}	wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
					
						
#if 0
					if((WL_PREWIDTH_GND<=plusWidth)&&(plusWidth<=WL_PREWIDTH_ROOF))
					{				
							HaltInterval(42);

						if(nikWireless.preArr==wlSet.s_arrary)
						{
							if(nikWireless.flash_wizard == WL_WIZARD_PRE)
							{
								TAKE_PREEXPOSE2
								nikWireless.flash_wizard = WL_WIZARD_FLA;
							}
							ttl_pre2_flag=UART_ENCODE_DEBUG_EXPOSE2;
							HaltInterval(50);
							IE0 = 0;
//							puts("pre2 \n");
						}
						nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
						nikWireless.WL_statue = NIKWL_STATUE_HOLD;
						nikWireless.preTotal --;
						if(nikWireless.preTotal==0)
						{
							wlSet.s_flashStatue = WL_FLASH_STATUE_AFTPF;	
							
						}
						nikWireless.preTurn = WL_PFCOUNT_RDY;
						
					}else
					{

						nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
						nikWireless.WL_statue = NIKWL_STATUE_HOLD;
						wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
					}
#endif
				}
			}
			
		}else if(nikWireless.WL_statue==NIKWL_STATUE_FLASH)
		{
			//HaltInterval(12);
			if((0x01==flashStatue.flash_enable)&&(!flashPara.ledFlag))
			{
				if(PREFLASH_LOCK_OFF==nikWireless.preflashLock)						//if it with the case of flash lock, never display the mode
				{
					if(wlSet.s_wlMode==WORKINGMODE_TURNOFF)
					{
						HaltInterval(10);
					}else
					{
						//flashPara.output1 = nikWireless.WL_output1;
						//flashPara.output2 = nikWireless.WL_output2;
						if(LOW_SPEED==nikWireless.fp)
						{
							pTime = OutputSwitch(wlSet.s_wlMode);
							TURN_ON_TRIGER(pTime);
						}else
						{
//#ifdef HIGH_SPEED_SYNCHRONIC
#ifdef _HP_SYN_
							if(HIGH_SPEED_LEVEL1==nikWireless.fp)
							{
								switch(wlSet.s_wlMode)
								{
									case WORKINGMODE_TTL:
									case WORKINGMODE_AUTO:
									//pTime = HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2);
									//HpExpCalc(flashPara.output1,flashPara.output2,0,0,&hpSetting);
									HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,0,0,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
									HighSpeedTriger(&hpSetting,WLHP_LEVEL1);
									break;

									case WORKINGMODE_MANU:
									//pTime = HpExpCalc(nikWireless.WL_output1,nikWireless.WL_output2);
									HpExpCalc(nikWireless.WL_output1,nikWireless.WL_output2,0,0,0,&hpSetting,ttl_pre2_flag);
									HighSpeedTriger(&hpSetting,WLHP_LEVEL1);
									break;
								}
								
							}else if(HIGH_SPEED_LEVEL2==nikWireless.fp)
							{
								switch(wlSet.s_wlMode)
								{
									case WORKINGMODE_TTL:
									case WORKINGMODE_AUTO:
									//pTime = HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2);
									HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,0,0,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
									HighSpeedTriger(&hpSetting,WLHP_LEVEL2);
									break;

									case WORKINGMODE_MANU:
									//pTime = HpExpCalc(nikWireless.WL_output1,nikWireless.WL_output2);
									HpExpCalc(nikWireless.WL_output1,nikWireless.WL_output2,0,0,0,&hpSetting,ttl_pre2_flag);
									HighSpeedTriger(&hpSetting,WLHP_LEVEL2);
									break;
								}
							}
#endif
						}
						flashPara.ExposeState = 0;
						nikWireless.flash_wizard = WL_WIZARD_HOLD;
						//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
						//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
						uartSendFlag.clear = 1;
						uartSendFlag.flashWarn= 1;
						//SETBIT(bitWireless,BIT_bitWireless_mode);
						//SETBIT(bitWireless,BIT_bitWireless_chanAndArr);
						//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLMODE);
						//debug 11.13
						uartSendFlag.wlm_mode = 1;
						/*
						if(WORKINGMODE_MANU==wlSet.s_wlMode)
						{
							SETBIT(bitWireless,BIT_bitWireless_Moutp);
							SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLEXP);
						}*/
					}
				}else
				{
					//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					//putchar('b');
					//flashPara.output1 = nikWireless.WL_output1;
					//flashPara.output2 = nikWireless.WL_output2;
					ttl_pre2_flag = tpfRsv;
					if(LOW_SPEED==nikWireless.fp)
					{
						if(wlSet.s_wlMode!=WORKINGMODE_TURNOFF)
						{
							pTime = OutputSwitch(wlSet.s_wlMode);
							TURN_ON_TRIGER(pTime);						
							//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
							uartSendFlag.clear = 1;
							uartSendFlag.flashWarn= 1;					
							//SETBIT(bitWireless,BIT_bitWireless_mode);
							//uartSendFlag.wlm_mode = 1;
						}
						flashPara.ExposeState = 0;
						nikWireless.flash_wizard = WL_WIZARD_HOLD;
					}else
					{
//#ifdef HIGH_SPEED_SYNCHRONIC
#ifdef _HP_SYN_
						if(HIGH_SPEED_LEVEL1==nikWireless.fp)
						{
							switch(wlSet.s_wlMode)
							{
								case WORKINGMODE_TTL:
								case WORKINGMODE_AUTO:
								//pTime = HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2);
								//HpExpCalc(flashPara.output1,flashPara.output2,&hpSetting);
								HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,0,0,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
								HighSpeedTriger(&hpSetting,WLHP_LEVEL1);
								uartSendFlag.clear = 1;
								uartSendFlag.flashWarn= 1;
								//uartSendFlag.wlm_mode = 1;
								break;

								case WORKINGMODE_MANU:
								//pTime = HpExpCalc(nikWireless.WL_output1,nikWireless.WL_output2);
								//HpExpCalc(flashPara.output1,flashPara.output2,&hpSetting);
								HpExpCalc(flashPara.output1,flashPara.output2,0,0,0,&hpSetting,ttl_pre2_flag);
								HighSpeedTriger(&hpSetting,WLHP_LEVEL1);
								uartSendFlag.clear = 1;
								uartSendFlag.flashWarn= 1;
								//uartSendFlag.wlm_mode = 1;
								break;
							}
							
						}else if(HIGH_SPEED_LEVEL2==nikWireless.fp)
						{
							switch(wlSet.s_wlMode)
							{
								case WORKINGMODE_TTL:
								case WORKINGMODE_AUTO:
								//pTime = HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2);
								//HpExpCalc(flashPara.output1,flashPara.output2,&hpSetting);
								HpExpCalc(flashPara.hp_ttl1,flashPara.hp_ttl2,0,0,uartSend.ISO_index,&hpSetting,ttl_pre2_flag);
								HighSpeedTriger(&hpSetting,WLHP_LEVEL2);
								uartSendFlag.clear = 1;
								uartSendFlag.flashWarn= 1;
								//uartSendFlag.wlm_mode = 1;
								break;

								case WORKINGMODE_MANU:
								//pTime = HpExpCalc(nikWireless.WL_output1,nikWireless.WL_output2);
								HpExpCalc(flashPara.output1,flashPara.output2,0,0,0,&hpSetting,ttl_pre2_flag);
								HighSpeedTriger(&hpSetting,WLHP_LEVEL2);
								uartSendFlag.clear = 1;
								uartSendFlag.flashWarn= 1;
								//uartSendFlag.wlm_mode = 1;
								break;
							}
						}
#endif
					}
				}
			}
/*
			main_parameter.output1 = nikWireless.WL_output1;
			main_parameter.output2 = nikWireless.WL_output2;
			pTime = OutputSwitch(wlSet.s_wlMode);
			TURN_ON_TRIGER(pTime);
			main_parameter.ExposeState = 0;
			nikWireless.flash_wizard = WL_WIZARD_HOLD;
			//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
			SETBIT(paraBit,BIT_paraBit_ONE_TIME_FLASH);
			SETBIT(bitPara3,BIT_bitPara3_FLASH_OK);
			Timer7SetCapture();
			CLR_WL_FLASH_TIMEOUT;*/
/*
			if(wlSet.s_wlMode==WORKINGMODE_MANU)
			{
				flashPara.output1= nikWireless.WL_output1;
				flashPara.output2= nikWireless.WL_output2;
			}
			//if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==0)&&(charge_ok!=AD_CHARGE_LOW)&&(wlSet.s_wlMode!=WORKINGMODE_TURNOFF))
			if(flashStatue.flash_enable==0x01)
			{
				pTime = OutputSwitch(wlSet.s_wlMode);
				TURN_ON_TRIGER(pTime);
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
				//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
				//SETBIT(paraBit,BIT_paraBit_ONE_TIME_FLASH);
				//SETBIT(bitPara3,BIT_bitPara3_FLASH_OK);
				//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
				//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
				Timer7SetCapture();
				flashPara.ExposeState = 0;
				nikWireless.flash_wizard = WL_WIZARD_HOLD;
				uartSendFlag.wlm_mode = 1;
				uartSendFlag.flashWarn = 1;
				//SETBIT(bitWireless,BIT_bitWireless_mode);
				//SETBIT(bitWireless,BIT_bitWireless_chanAndArr);
				//if(WORKINGMODE_MANU==wlSet.s_wlMode)
				//{
				//	SETBIT(bitWireless,BIT_bitWireless_Moutp);
				//}else if(WORKINGMODE_TTL==wlSet.s_wlMode)
				//{
				//	SETBIT(bitWireless,BIT_bitWireless_TtlExp);
				//}
			}
*/

			STOP_TIMER0;
			CLR_WL_FLASH_TIMEOUT;
			nikWireless.bitLoadIndex = 0;
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;	
			nikWireless.WLBufIndex = 0;
			nikWireless.flash_wizard = WL_WIZARD_HOLD;
			//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
			nikWireless.preArr = WL_ARRARY_NULL;
			nikWireless.preTurn=WL_PFCOUNT_RDY;
			ttl_pre2_flag = 0;
			//nikWireless.preflashLock = PREFLASH_LOCK_OFF;
			CLR_WLN_BUF;
			/*
			nikWireless.WLBufIndex = 0;
			nikWireless.bitLoadIndex = 0;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			ttl_pre2_flag = 0;
			*/
			//WLReady = 1;

			//HaltInterval(50);
			//Delay_10us(50);
			HaltInterval(50);
			disable_irq();	
			MEMCTR   |=  0x04;
			CLRBIT(IRQ2ICR,0);
			MEMCTR   &= ~0x04;
			enable_irq();
			//IE0 = 0;
			//nikWireless.WL_statue = NIKWL_STATUE_STOP;
			//WLDEBUG_BREAK_POINT
			
		}
	}else if(WORKINGMODE_F1==flashPara.workmode)
	{
//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
		if((flashStatue.flash_enable!=0x01)||(flashPara.ledFlag)||(flashPara.rmtShut))
#else
		if(flashStatue.flash_enable!=0x01)
#endif
		{
			return;
		}
		HaltInterval(12);
		//TURN_ON_TRIGER(OutputSwitch(WORKINGMODE_MANU))
		pTime = OutputSwitch(WORKINGMODE_MANU);
		TURN_ON_TRIGER(pTime);
		flashPara.ExposeState = 0;
		uartSendFlag.clear = 1;
		uartSendFlag.flashWarn = 1;
		//Delay_ms(5);
		HaltInterval(500);
		disable_irq();
		MEMCTR    |=  0x04;	
		IRQ2ICR &= 0XFE;
		MEMCTR   &= ~0x04;
		enable_irq();
	}else if(WORKINGMODE_F2==flashPara.workmode)
	{
		if(g_f2FlashLabel==F2FLASH_LABEL_CLR)
		{
			g_f2FlashLabel = F2FLASH_LABEL_SET;
			//Delay_ms(20);
			//Delay_ms(60);
			HaltInterval(500);

			disable_irq();
			MEMCTR    |=  0x04;	
			IRQ2ICR &= 0XFE;
			MEMCTR   &= ~0x04;
			enable_irq();
			return;
		}else
		{
//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
			if((flashStatue.flash_enable!=0x01)||(flashPara.ledFlag)||(flashPara.rmtShut))
#else
			if(flashStatue.flash_enable!=0x01)
#endif
			{
				return;
			}
			pTime = OutputSwitch(WORKINGMODE_MANU);
			TURN_ON_TRIGER(pTime);
			uartSendFlag.clear = 1;
			uartSendFlag.flashWarn = 1;
			g_f2FlashLabel = F2FLASH_LABEL_CLR;
			//Delay_ms(5);
			HaltInterval(500);

			disable_irq();
			MEMCTR    |=  0x04;	
			IRQ2ICR &= 0XFE;
			MEMCTR   &= ~0x04;
			enable_irq();
			return;			
		}
	}
		
		
#endif
#if 0
#ifdef _CANON_WIRELESS_
	static unsigned int tmrRcd;
	static unsigned int WL_chan1=0,WL_chan2=0,plusWidth=0;

	if(canWireless.WL_statue==CANWL_STATUE_HOLD)
	{
		tmrRcd = TM7IC;
		TM7MD1 &= 0XEF;	
#ifdef WIRELESS_MEASURE
			canWireless.WL_statue = CANWL_STATUE_PLUSMEASURE;
#else
			canWireless.WL_statue = CANWL_STATUE_ENSURECHAN;
#endif
		SET_WL_FLASH_TIMEOUT
		while(!WIRELESS_IN);
		TM7MD1 |= 0X10;	
			
	}else if(canWireless.WL_statue==CANWL_STATUE_ENSURECHAN)
	{
		WL_chan1 = TM7IC - tmrRcd;	
		TM7MD1 &= 0XEF;

		tmrRcd = TM7IC;
				

		if((WL_chan1>WL_CHAN_SCALE1_GND)&&(WL_chan1<WL_CHAN_SCALE1_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_ONE;
		}else if((WL_chan1>WL_CHAN_SCALE2_GND)&&(WL_chan1<WL_CHAN_SCALE2_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_TWO;
		}else if((WL_chan1>WL_CHAN_SCALE3_GND)&&(WL_chan1<WL_CHAN_SCALE3_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_THREE;
		}else if((WL_chan1>WL_CHAN_SCALE4_GND)&&(WL_chan1<WL_CHAN_SCALE4_ROOF))
		{
			canWireless.WL_Channel = WL_CHANNEL_FOUR;
		}else
		{
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			canWireless.WL_statue = CANWL_STATUE_HOLD;
		}

		if(wlSet.s_chan == canWireless.WL_Channel)
		{
			canWireless.WL_statue = CANWL_STATUE_RUN;
			canWireless.bitLoadIndex = 0;
			canWireless.WLBufIndex = 0;
			if(wlSet.s_flashStatue==WL_FLASH_STATUE_HOLD)
			{
				wlSet.s_flashStatue = WL_FLASH_STATUE_START;
			}
			while(!WIRELESS_IN);
			TM7MD1 |= 0X10;	
			START_TIMER0
		}else
		{
			//should be handle there
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			canWireless.bitLoadIndex = 0;
			canWireless.WLBufIndex = 0;
			TM7MD1 |= 0X10;	
			disable_irq();	
			MEMCTR   |=  0x04;
			CLRBIT(IRQ2ICR,0);
			MEMCTR   &= ~0x04;
			enable_irq();
		}
	//	tmrRcd = TM7IC;
		
	}else if(canWireless.WL_statue==CANWL_STATUE_RUN)
	{
		plusWidth = TM7IC - tmrRcd;
		TM7MD1 &= 0XEF;	
				
		
#ifdef WIRELESS_DEBUG
		wl_buf[wlBufSize++] = plusWidth;
#endif
		
		tmrRcd = TM7IC;

		UPDATA_TIMER0(180)

		if(plusWidth<WL_BITJUDGE_CODE0)
		{
			canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<canWireless.bitLoadIndex);
			canWireless.bitLoadIndex++;
#ifdef _TRACE_
			if(TraIndex<TRACE_SIZE)
			TraArr[TraIndex++] = 0x11;
#endif
			if(canWireless.bitLoadIndex>=8)
			{
				canWireless.bitLoadIndex=0;
				canWireless.WLBufIndex++;
			}
		}else if(plusWidth<WL_BITJUDGE_CODE1)
		{

			if(canWireless.bitLoadIndex>6)
			{
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = (2-(8-canWireless.bitLoadIndex));	
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x20;
#endif
				
			}else if(canWireless.bitLoadIndex==6)
			{
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<7); 
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = 0;
			}else
			{
				canWireless.bitLoadIndex+=2;
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x21;
#endif
			}
		}else if(plusWidth<WL_BITJUDGE_CODE2)
		{

			if(canWireless.bitLoadIndex>5)
			{	
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = (3-(8-canWireless.bitLoadIndex));
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x20;
#endif
				
			}else if(canWireless.bitLoadIndex==5)
			{
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<7); 
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = 0;
			}else
			{
				canWireless.bitLoadIndex+=3;
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x21;
#endif
			}
		}else if(plusWidth<WL_BITJUDGE_CODE3)
		{

			if(canWireless.bitLoadIndex>4)
			{	
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = (4-(8-canWireless.bitLoadIndex));
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
				
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x20;
#endif
				
			}else if(canWireless.bitLoadIndex==4)
			{
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<7); 
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = 0;
			}else
			{
				canWireless.bitLoadIndex+=4;
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
			
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x21;
#endif
			}
		}else if(plusWidth<WL_BITJUDGE_CODE4)
		{

			if(canWireless.bitLoadIndex>3)
			{
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = (5-(8-canWireless.bitLoadIndex));	
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x20;
#endif
				
			}else if(canWireless.bitLoadIndex==3)
			{
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<7); 
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = 0;
			}else
			{
				canWireless.bitLoadIndex+=5;
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
				
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x21;
#endif
			}
		}else if(plusWidth<WL_BITJUDGE_CODE5)
		{

			if(canWireless.bitLoadIndex>2)
			{
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = (6-(8-canWireless.bitLoadIndex));	
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x20;
#endif
				
			}else if(canWireless.bitLoadIndex==2)
			{
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<7); 
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = 0;
			}else
			{				
				canWireless.bitLoadIndex+=6;
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x21;
#endif
			}
		}else if(plusWidth<WL_BITJUDGE_CODE6)
		{

			if(canWireless.bitLoadIndex>1)
			{
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = (7-(8-canWireless.bitLoadIndex));
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x20;
#endif
				
			}else if(canWireless.bitLoadIndex==1)
			{
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<7); 
				canWireless.WLBufIndex++;
				canWireless.bitLoadIndex = 0;
			}else
			{
				canWireless.bitLoadIndex+=7;
				canWireless.WL_buf[canWireless.WLBufIndex] |= (1<<(canWireless.bitLoadIndex-1));
#ifdef _TRACE_
				if(TraIndex<TRACE_SIZE)
					TraArr[TraIndex++] = 0x21;
#endif
			}
		}else if(plusWidth<WL_BITJUDGE_CODE7)
		{
			canWireless.WL_buf[canWireless.WLBufIndex] = (1<<canWireless.bitLoadIndex);
			canWireless.WLBufIndex++;
		}else 
		{
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			canWireless.bitLoadIndex = 0;
			canWireless.WLBufIndex = 0;
			STOP_TIMER0
		}		

		while(!WIRELESS_IN);
		TM7MD1 |= 0X10;	
		
		
	}else if(canWireless.WL_statue==CANWL_STATUE_PREFLASH)
	{
		if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==0)&&(charge_ok!=AD_CHARGE_LOW)&&(canWireless.activeArr==1))
		{
			TURN_ON_TRIGER(OUTPUT_30);
			canWireless.activeArr=0;
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			Delay_10us(10);
			Timer7SetCapture();
			disable_irq();	
			MEMCTR   |=  0x04;
			CLRBIT(IRQ2ICR,0);
			MEMCTR   &= ~0x04;
			enable_irq();
		}else
		{
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;	
			Delay_10us(10);
			Timer7SetCapture();
			disable_irq();	
			MEMCTR   |=  0x04;
			CLRBIT(IRQ2ICR,0);
			MEMCTR   &= ~0x04;
			enable_irq();
		}
		
	}else if(canWireless.WL_statue==CANWL_STATUE_FLASH)
	{
		if(wlSet.s_wlMode==WORKINGMODE_MANU)
		{
			main_parameter.VIP_output1 = canWireless.WL_output1;
			main_parameter.VIP_output2 = canWireless.WL_output2;
		}
		if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==0)&&(charge_ok!=AD_CHARGE_LOW)&&(canWireless.activeArr==1))
		{
			pTime = OutputSwitch(wlSet.s_wlMode);
			TURN_ON_TRIGER(pTime);
			//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
			SETBIT(paraBit,BIT_paraBit_ONE_TIME_FLASH);
			SETBIT(bitPara3,BIT_bitPara3_FLASH_OK);
			Timer7SetCapture();
			SETBIT(bitWireless,BIT_bitWireless_mode);
			SETBIT(bitWireless,BIT_bitWireless_chanAndArr);
			if(WORKINGMODE_MANU==wlSet.s_wlMode)
			{
				SETBIT(bitWireless,BIT_bitWireless_Moutp);
			}else if(WORKINGMODE_TTL==wlSet.s_wlMode)
			{
				SETBIT(bitWireless,BIT_bitWireless_TtlExp);
			}
			
#ifdef WIRELESS_DEBUG
			for(wlBufSize=0;wlBufSize<WL_BUF_SIZE;wlBufSize++)
			{
				wl_buf[wlBufSize] = 0;
			}
			wlBufSize = 0;

			wlDbgPrt = 1;

#endif

		}
			canWireless.WL_mode = WORKINGMODE_TURNOFF;

#ifdef _TRACE_
			while(TraTimeIndex)
			{
				TraTime[TraTimeIndex--] = 0;
			}
			TraTime[0] = 0;

#endif

		disable_irq();	
		MEMCTR   |=  0x04;
		CLRBIT(IRQ2ICR,0);
		MEMCTR   &= ~0x04;
		enable_irq();

		canWireless.activeArr=0;		
		CLR_WL_FLASH_TIMEOUT		
		canWireless.WLBufIndex = 0;
		canWireless.bitLoadIndex = 0;
		canWireless.WL_statue = CANWL_STATUE_HOLD;
		canWireless.WL_Channel = WL_CHANNEL_UNSURE;
		wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
		CLR_WL_BUF
				

	}else if(canWireless.WL_statue==CANWL_STATUE_PLUSMEASURE)
	{
		TM7MD1 &= 0XEF;
		if(wlBufSize<WL_BUF_SIZE)
		{
			wl_buf[wlBufSize++] = TM7IC-tmrRcd;
			tmrRcd = TM7IC;
		}
		while(!WIRELESS_IN);
		TM7MD1 |= 0X10;	
		
	}


/*
	Delay();
	if(FFLA_IN)
	{
		return;
	}
*/
#endif
#endif
#if 0
	if(main_parameter.VIP_workmode!=WORKINGMODE_MASTER && main_parameter.VIP_workmode!=WORKINGMODE_S2)
	    	return;
	#ifdef UART_SENDDATA_CONTINUE
		if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==1)||(charge_ok!=AD_CHARGE_LOW))
	#else

		if((GETBIT(bitIndex,BIT_bitIndex_PROTECT)==1)||(GETBIT(bitIndex,BIT_bitIndex_CHARGE)==0))
	#endif
			return;

	if(main_parameter.VIP_workmode==WORKINGMODE_MASTER)
	{
		SETBIT(paraBit,BIT_paraBit_FOLLOW_FLASH);
		pTime = OutputSwitch(WORKINGMODE_MANU);
		Delay();
		TURN_ON_TRIGER(pTime);	
		SETBIT(bitPara3,BIT_bitPara3_FLASH_OK);
		SETBIT(paraBit,BIT_paraBit_ONE_TIME_FLASH);
	}else
	{
		if(GETBIT(bitIndex,BIT_bitIndex_S2COUNT)==0)
		{
			main_parameter.s2_count=0xff;
			SETBIT(bitIndex,BIT_bitIndex_S2COUNT);
			Delay_10us(1000);
		}else
		{
			SETBIT(paraBit,BIT_paraBit_FOLLOW_FLASH);
			pTime = OutputSwitch(WORKINGMODE_MANU);
			TURN_ON_TRIGER(pTime);
			SETBIT(bitPara3,BIT_bitPara3_FLASH_OK);
			SETBIT(paraBit,BIT_paraBit_ONE_TIME_FLASH);
			Delay();
		}

	}
#endif
}

void int_Ext3_WakeUp(void)
{
	if(SLEEP_MODE_ON==flashPara.sleep_flag)
	{
		//flashPara.sleep_flag = SLEEP_MODE_OFF;
		//NIKONNSSO = 1;
		SET_WAKEUP_OUT;
		SET_SBT_OUT;
		WAKE_UP = 0;
		/*
		disable_irq();	
		MEMCTR   |=  0x04;
		IRQ0ICR = 0X00;
		IRQ1ICR = 0X82;
		IRQ3ICR = 0X00;
		MEMCTR   &= ~0x04;
		enable_irq();
		*/
	}
}



