#include "sr101ef51a.h"
#include "mn101ef52a.h"
//#include "oloong.h"
#include "init.h"
//#include "Golbal.h"
#include "output.h"
#include "nikon.h"
#include "uart.h"


extern unsigned int pTime;
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
extern unsigned char charge_ok;
extern unsigned char g_f2FlashLabel;
extern unsigned char xdata tpfRsv;

#pragma	_interrupt int_TM0
#pragma	_interrupt int_TM1
//#pragma	_interrupt int_TM7
#pragma	_interrupt int_AD

extern void WLS_ExposeTimeCal(const unsigned char camCode,unsigned char hp);

/* ------------------------------------End ---------------------------------

 ===========================================================================

 函数名   :  void int_TM0(void)
 
 功能  :     定时器特卸携
 
 描叙:       2MS定时查询IRQ4端口状态
 
 ===========================================================================*/
void int_TM0(void)
{
//here indicate a communication with the camera timeout, so reset the com port
//clear the interrupt request flag first
	char temp;

	if(flashPara.workmode==WORKINGMODE_RTN)
	{
#ifdef 	_NIKON_WIRELESS_
//		nikWireless.WLBufIndex++;
//		nikWireless.bitLoadIndex = 0;	//
//		STOP_TIMER0
#endif

#ifdef _CANON_WIRELESS_
//		canWireless.WLBufIndex++;
//		canWireless.bitLoadIndex = 0;	//
//		STOP_TIMER0
#endif
		nikWireless.WLBufIndex++;
		nikWireless.bitLoadIndex = 0;	//
		STOP_TIMER0

//#ifdef _NIKON_WIRELESS_
	if(wlSet.s_flashStatue == WL_FLASH_STATUE_START)
	{
		if((nikWireless.WL_buf[0]==WL_REARFLASH_COMMAND)||(nikWireless.WL_buf[0]==WL_REARFLASH_COMMANDex)/*&&(nikWireless.WLBufIndex==1)*/)   //in order to competible with rear synchronic
		{
			if(0x01==flashStatue.flash_enable)
			{
				//flashPara.output1 = nikWireless.WL_output1;
				//flashPara.output2 = nikWireless.WL_output2;
				if(WORKINGMODE_TTL==wlSet.s_wlMode)
				{
					WLS_ExposeTimeCal(nikWireless.RearReserve,nikWireless.fp);
				/*	putchar(0x01);
					putchar(nikWireless.RearReserve);
					putchar(flashPara.TTL_LEVEL1);
					putchar(flashPara.TTL_LEVEL2);*/
				}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
				{
					Nik_AutoShotSet(nikWireless.WL_opCode);
				}
				pTime = OutputSwitch(wlSet.s_wlMode);
				//EX0 = 0;
				//disable_irq();
				TURN_ON_TRIGER(pTime);
				flashPara.ExposeState = 0;
/**********************************************************************/
//				the following code in order to clear the receive process
/**********************************************************************/
				CLR_WL_FLASH_TIMEOUT;
				nikWireless.preArr = WL_ARRARY_NULL;
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
				nikWireless.bitLoadIndex = 0;
				nikWireless.WLBufIndex = 0;
				nikWireless.flash_wizard = WL_WIZARD_HOLD;
				//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
				nikWireless.preTurn=WL_PFCOUNT_RDY;
				CLR_WLN_BUF;
/**********************************************************************/
				//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
				//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
				//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
				uartSendFlag.flashWarn = 1;
				//SETBIT(bitWireless,BIT_bitWireless_mode);
				uartSendFlag.wlm_mode = 1;
				//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLMODE);
				//if(WORKINGMODE_MANU==wlSet.s_wlMode)
				//{
					//SETBIT(bitWireless,BIT_bitWireless_Moutp);
					//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLEXP);
				//}
				//EX0 = 1;
				//enable_irq();
				return;
				
			}
		}else if(BLFLAG_READY_FRONT==(nikWireless.WL_buf[0]&0xf0))//this is work for preflash lock
		{ 
				//nikWireless.preTotal = 0;
				//putchar('p');
				//nikWireless.preflashLock = PREFLASH_LOCK_ON;
				//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_A]);
				//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_B]);
				//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_C]);
				nikWireless.fp = LOW_SPEED;
				nikWireless.WL_opCode= ((nikWireless.WL_buf[wlSet.s_arrary-1]<<4) | (nikWireless.WL_buf[wlSet.s_arrary]>>4));
				if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
				{
					switch(nikWireless.preflashLockModeReserve[wlSet.s_arrary])
					{
						case WORKINGMODE_TTL:
							wlSet.s_wlMode = WORKINGMODE_TTL;
							WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
						break;

						case WORKINGMODE_MANU:
							wlSet.s_wlMode = WORKINGMODE_MANU;	
							nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
							nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
							//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
							if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
							{
								nikWireless.WL_output1 = 5;
								nikWireless.WL_output2 = 0;
							}
							if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
							{
								nikWireless.WL_output2 = 0;
							}
						break;

						case WORKINGMODE_AUTO:
							wlSet.s_wlMode = WORKINGMODE_AUTO;	
							Nik_AutoShotSet(nikWireless.WL_opCode);
						break;

						default:
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						break;						
					}
				}else
				{
					wlSet.s_wlMode = WORKINGMODE_TURNOFF;
				}
				nikWireless.WL_statue = NIKWL_STATUE_FLASH;
				nikWireless.bitLoadIndex = 0;
				nikWireless.WLBufIndex = 0;
				CLR_WLN_BUF;

#if 0 				
				switch(wlSet.s_arrary)
				{
					case WL_ARRARY_A:
						//putchar('a');
						//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_A]);
						nikWireless.WL_opCode= ((nikWireless.WL_buf[0]<<4) | (nikWireless.WL_buf[1]>>4));						
						nikWireless.WL_statue = NIKWL_STATUE_FLASH;
						if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
						{
							switch(nikWireless.preflashLockModeReserve[WL_ARRARY_A])
							{
								case WORKINGMODE_TTL:
									wlSet.s_wlMode = WORKINGMODE_TTL;
									WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
									//putchar(0xa1);
								break;
								
								case WORKINGMODE_MANU:
									wlSet.s_wlMode = WORKINGMODE_MANU;	
									nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
									nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
									//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
									if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
									{
										nikWireless.WL_output1 = 5;
										nikWireless.WL_output2 = 0;
									}
									if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
									{
										nikWireless.WL_output2 = 0;
									}
									//putchar(0xa2);
								break;
								
								case WORKINGMODE_AUTO:
									wlSet.s_wlMode = WORKINGMODE_AUTO;	
									Nik_AutoShotSet(nikWireless.WL_opCode);
									//putchar(0xa3);
								break;

								default:
									wlSet.s_wlMode = WORKINGMODE_TURNOFF;
								break;
							}
						}else
						{
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						}
						CLR_WLN_BUF;
					break;

					case WL_ARRARY_B:
						nikWireless.WL_opCode= ((nikWireless.WL_buf[1]<<4) | (nikWireless.WL_buf[2]>>4));
						if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
						{
							switch(nikWireless.preflashLockModeReserve[WL_ARRARY_B])
							{
								case WORKINGMODE_TTL:
									wlSet.s_wlMode = WORKINGMODE_TTL;
									WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
									//putchar(0xb1);
								break;
								
								case WORKINGMODE_MANU:
									wlSet.s_wlMode = WORKINGMODE_MANU;	
									nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
									nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
									//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
									if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
									{
										nikWireless.WL_output1 = 5;
										nikWireless.WL_output2 = 0;
									}
									if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
									{
										nikWireless.WL_output2 = 0;
									}
									//putchar(0xb2);
								break;
								
								case WORKINGMODE_AUTO:
									wlSet.s_wlMode = WORKINGMODE_AUTO;	
									Nik_AutoShotSet(nikWireless.WL_opCode);
									//putchar(0xb3);
								break;

								default:
									wlSet.s_wlMode = WORKINGMODE_TURNOFF;	
								break;
							}
						}else
						{
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;	
						}
						nikWireless.WL_statue = NIKWL_STATUE_FLASH;
						CLR_WLN_BUF;
						//CLR_WL_BUF
					break;

					case WL_ARRARY_C:
						nikWireless.WL_opCode= ((nikWireless.WL_buf[2]<<4) | (nikWireless.WL_buf[3]>>4));
						if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
						{
							switch(nikWireless.preflashLockModeReserve[WL_ARRARY_C])
							{
								case WORKINGMODE_TTL:
									wlSet.s_wlMode = WORKINGMODE_TTL;
									WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
									//putchar(0xc1);
								break;
								
								case WORKINGMODE_MANU:
									wlSet.s_wlMode = WORKINGMODE_MANU;	
									nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
									nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
									//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
									if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
									{
										nikWireless.WL_output1 = 5;
										nikWireless.WL_output2 = 0;
									}
									if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
									{
										nikWireless.WL_output2 = 0;
									}
									//putchar(0xc2);
								break;
								
								case WORKINGMODE_AUTO:
									wlSet.s_wlMode = WORKINGMODE_AUTO;	
									Nik_AutoShotSet(nikWireless.WL_opCode);
									//putchar(0xc3);
								break;

								default:
									wlSet.s_wlMode = WORKINGMODE_TURNOFF;	
								break;
							}
						}else
						{
								wlSet.s_wlMode = WORKINGMODE_TURNOFF;	
						}
						nikWireless.WL_statue = NIKWL_STATUE_FLASH;
						nikWireless.bitLoadIndex = 0;
						nikWireless.WLBufIndex = 0;
						CLR_WLN_BUF;
						//CLR_WL_BUF
					break;
				}
#endif
				return;
				//putchar(EX0);
		}
		//else if((0xDB==nikWireless.WL_buf[0])||(0xD8==nikWireless.WL_buf[0])||(0xDC==nikWireless.WL_buf[0]))			//high speed shutter preflash lock
		else if((0xd8<=nikWireless.WL_buf[0])&&(0xdc>=nikWireless.WL_buf[0]))
		{
			if(0xDB==nikWireless.WL_buf[0]) 
			{
				nikWireless.fp = HIGH_SPEED_LEVEL2;
			}else
			{
				nikWireless.fp = HIGH_SPEED_LEVEL1;
			}

			nikWireless.WL_opCode = nikWireless.WL_buf[wlSet.s_arrary];

			if(PREFLASH_LOCK_ON==nikWireless.preflashLock)
			{
				switch(nikWireless.preflashLockModeReserve[wlSet.s_arrary])
				{
					case WORKINGMODE_TTL:
						wlSet.s_wlMode = WORKINGMODE_TTL;
						WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
					break;

					case WORKINGMODE_MANU:
						wlSet.s_wlMode = WORKINGMODE_MANU;	
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					break;

					case WORKINGMODE_AUTO:
						wlSet.s_wlMode = WORKINGMODE_TURNOFF;	
					break;

					default:
						wlSet.s_wlMode = WORKINGMODE_TURNOFF;	
					break;					
				}

			}else
			{
				wlSet.s_wlMode = WORKINGMODE_TURNOFF;
			}
			nikWireless.WL_statue = NIKWL_STATUE_FLASH;
			nikWireless.bitLoadIndex = 0;
			nikWireless.WLBufIndex = 0;
			CLR_WLN_BUF;
			return;
			
			/*
			nikWireless.WL_opCode = nikWireless.WL_buf[wlSet.s_arrary];
			if(wlSet.s_wlMode==WORKINGMODE_MANU)
			{
				nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
				nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
				//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
				if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
				{
					nikWireless.WL_output1 = 5;
					nikWireless.WL_output2 = 0;
				}
				if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
				{
					nikWireless.WL_output2 = 0;
				}
			}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
			{
				WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
			}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
			{
				Nik_AutoShotSet(nikWireless.WL_opCode);
			}
			if(0xDB==nikWireless.WL_buf[0])
			{
				nikWireless.fp = HIGH_SPEED_LEVEL2;
			}else
			{
				nikWireless.fp = HIGH_SPEED_LEVEL1;
			}
			
			nikWireless.WL_statue = NIKWL_STATUE_FLASH;
			CLR_WLN_BUF;
			return;
			*/
			
		}
		
		else if(nikWireless.WL_buf[0]==WL_PILOT_CMD)	//receive a test flash command
		{
			switch(wlSet.s_arrary)
			{
				case WL_ARRARY_A:
					wlSet.testCnt = 1000;
					wlSet.testPilot = TEST_PILOT_ON;
				break;

				case WL_ARRARY_B:
					wlSet.testCnt = 2000;
					wlSet.testPilot = TEST_PILOT_ON;						
				break;

				case WL_ARRARY_C:
					wlSet.testCnt = 3000;
					wlSet.testPilot = TEST_PILOT_ON;
				break;
			}
		}
		
		if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_START)
		{
			
			switch(wlSet.s_arrary)
			{
				case WL_ARRARY_A:
					switch(nikWireless.WL_buf[0]&0x0f)
					{
						case 0x0B:
							wlSet.s_wlMode = WORKINGMODE_MANU;
						break;

						//case 0x01:
						case 0x08:
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						break;
 
						case 0x09:
							wlSet.s_wlMode = WORKINGMODE_TTL;
						break;

						case 0x0a:
							wlSet.s_wlMode = WORKINGMODE_AUTO;
						break;
					}
				break;

				case WL_ARRARY_B:
					switch(nikWireless.WL_buf[1]&0xf0)
					{
						case 0xB0:
							wlSet.s_wlMode = WORKINGMODE_MANU;
						break;

						//case 0x10:
						case 0x80:
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						break;

						case 0x90:
							wlSet.s_wlMode = WORKINGMODE_TTL;
						break;

						case 0x00:
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						break;

						case 0x0a:
							wlSet.s_wlMode = WORKINGMODE_AUTO;
						break;
					}
				break;

				case WL_ARRARY_C:
					switch(nikWireless.WL_buf[1]&0x0f)
					{
						case 0x0B:
							wlSet.s_wlMode = WORKINGMODE_MANU;
						break;

						//case 0x01:
						case 0x08:							
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						break;

						case 0x09:
							wlSet.s_wlMode = WORKINGMODE_TTL;
						break;

						case 0x00:
							wlSet.s_wlMode = WORKINGMODE_TURNOFF;
						break;

						case 0x0a:
							wlSet.s_wlMode = WORKINGMODE_AUTO;
						break;
							
						
					}
				break;

			}

/********************************************************************************************************
			the following code is aim at preserver for preflash locking
********************************************************************************************************/
			switch(nikWireless.WL_buf[0]&0x0f)
			{
				case 0x0B:
					//wlSet.s_wlMode = WORKINGMODE_MANU;
					nikWireless.preflashLockModeReserve[WL_ARRARY_A] = WORKINGMODE_MANU;
				break;

				case 0x08:
					//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					nikWireless.preflashLockModeReserve[WL_ARRARY_A] = WORKINGMODE_TURNOFF;
				break;

				case 0x09:
					//wlSet.s_wlMode = WORKINGMODE_TTL;
					nikWireless.preflashLockModeReserve[WL_ARRARY_A] = WORKINGMODE_TTL;
				break;

				case 0x0a:
					//wlSet.s_wlMode = WORKINGMODE_AUTO;
					nikWireless.preflashLockModeReserve[WL_ARRARY_A] = WORKINGMODE_AUTO;
				break;
			}

			switch(nikWireless.WL_buf[1]&0xf0)
			{
				case 0xB0:
					//wlSet.s_wlMode = WORKINGMODE_MANU;
					nikWireless.preflashLockModeReserve[WL_ARRARY_B] = WORKINGMODE_MANU;
				break;

				case 0x80:
					//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					nikWireless.preflashLockModeReserve[WL_ARRARY_B] = WORKINGMODE_TURNOFF;
				break;

				case 0x90:
					//wlSet.s_wlMode = WORKINGMODE_TTL;
					nikWireless.preflashLockModeReserve[WL_ARRARY_B] = WORKINGMODE_TTL;
				break;

				case 0x00:
					//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					nikWireless.preflashLockModeReserve[WL_ARRARY_B] = WORKINGMODE_TURNOFF;
				break;

				case 0x0a:
					//wlSet.s_wlMode = WORKINGMODE_AUTO;
					nikWireless.preflashLockModeReserve[WL_ARRARY_B] = WORKINGMODE_AUTO;
				break;
			}

			switch(nikWireless.WL_buf[1]&0x0f)
			{
				case 0x0B:
					//wlSet.s_wlMode = WORKINGMODE_MANU;
					nikWireless.preflashLockModeReserve[WL_ARRARY_C] = WORKINGMODE_MANU;
				break;

				case 0x08:
					//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					nikWireless.preflashLockModeReserve[WL_ARRARY_C] = WORKINGMODE_TURNOFF;
				break;

				case 0x09:
					//wlSet.s_wlMode = WORKINGMODE_TTL;
					nikWireless.preflashLockModeReserve[WL_ARRARY_C] = WORKINGMODE_TTL;
				break;

				case 0x00:
					//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
					nikWireless.preflashLockModeReserve[WL_ARRARY_C] = WORKINGMODE_TURNOFF;
				break;

				case 0x0a:
					//wlSet.s_wlMode = WORKINGMODE_AUTO;
					nikWireless.preflashLockModeReserve[WL_ARRARY_C] = WORKINGMODE_AUTO;
				break;
			}

			//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_A]);
			//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_B]);
			//putchar(nikWireless.preflashLockModeReserve[WL_ARRARY_C]);
/******************************************************************************************************
*******************************************************************************************************/
			
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
				wlSet.s_flashStatue = WL_FLASH_STATUE_BFRPF;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WLBufIndex= 0;
				nikWireless.preflashLock = PREFLASH_LOCK_OFF;
				CLR_WLN_BUF
					
			}else if(((nikWireless.WL_buf[0]&0x0f)==0x0b)||((nikWireless.WL_buf[1]&0xf0)==0xb0)||((nikWireless.WL_buf[1]&0x0f)==0x0b))
			{
				//SET_WL_FLASH_TIMEOUT
				wlSet.s_flashStatue = WL_FLASH_STATUE_AFTPF;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WLBufIndex= 0;
				CLR_WLN_BUF
			}else
			{
				CLR_WL_FLASH_TIMEOUT
				nikWireless.bitLoadIndex = 0;
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;	
				nikWireless.WLBufIndex = 0;
				nikWireless.flash_wizard = WL_WIZARD_HOLD;
				wlSet.s_wlMode = WORKINGMODE_TURNOFF;
				nikWireless.preArr = WL_ARRARY_NULL;
				nikWireless.preTurn=WL_PFCOUNT_RDY;				
				//nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				//nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				//wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
				//nikWireless.bitLoadIndex = 0;
				//nikWireless.WLBufIndex = 0;
				//nikWireless.flash_wizard = WLN_WIZARD_HOLD;
				CLR_WLN_BUF
			}
		}else
		{
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			nikWireless.bitLoadIndex = 0;
			nikWireless.WLBufIndex = 0;
			nikWireless.flash_wizard = WLN_WIZARD_HOLD;
			CLR_WLN_BUF
		}
	}else if(wlSet.s_flashStatue == WL_FLASH_STATUE_BFRPF)
	{
#ifdef WIRELESS_DEBUG
			nikWireless.wl_debug[0] = nikWireless.WL_buf[0];
			nikWireless.wl_debug[1] = nikWireless.WL_buf[1];
			nikWireless.wl_debug[2] = nikWireless.WL_buf[2];
			nikWireless.wl_debug[3] = nikWireless.WL_buf[3];

#endif
		if(nikWireless.preTurn==WLN_PFCOUNT_TWO)
		{
			nikWireless.preTurn = WLN_PFCOUNT_RDY;
			if(nikWireless.preArr==wlSet.s_arrary)
			{
				if(nikWireless.flash_wizard==WLN_WIZARD_PRE)
				{
					//EX0=0;//disable timer2 in order to generate interrupt
					//TAKE_PREEXPOSE2
					TURN_ON_TRIGER(750-1);//the seconde previous flash
					/*
					disable_irq();
					MEMCTR   |=  0x04;
					CLRBIT(IRQ2ICR,1);
					MEMCTR   &= ~0x04;
					enable_irq();	*/		//Clear the light sensitive interrupt flag
					nikWireless.flash_wizard = WLN_WIZARD_FLA;
					Timer7SetCapture();
					ttl_pre2_flag=0x02;
					tpfRsv = 2;
				}					
				

			}
/********************************************************/
//delay, avoid intervence
/*********************************************************/
			disable_irq();
			Delay_10us(50);
			MEMCTR   |=  0x04;
			IRQ2ICR = 0X82;
			MEMCTR   &= ~0x04;
			enable_irq();
			
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			nikWireless.preTotal --;
			if(nikWireless.preTotal==0)
			{
				wlSet.s_flashStatue = WL_FLASH_STATUE_AFTPF;					
			}
			
		}else
		{
			//here handle arrays previose previous flash respectively
			if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE1)
			{
				nikWireless.preArr= WL_ARRARY_A;
				nikWireless.preTurn = WLN_PFCOUNT_ONE;
				nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
#ifdef WIRELESS_DEBUG
				nikWireless.wl_debug[0] = nikWireless.WL_buf[0];
#endif
				CLR_WLN_BUF
			}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE2)
			{
				nikWireless.preArr = WL_ARRARY_B;
				nikWireless.preTurn = WLN_PFCOUNT_ONE;
				nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
#ifdef WIRELESS_DEBUG
				nikWireless.wl_debug[1] = nikWireless.WL_buf[0];
#endif
				CLR_WLN_BUF
			}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_PRE3)
			{
				nikWireless.preArr = WL_ARRARY_C;
				nikWireless.preTurn = WLN_PFCOUNT_ONE;
				nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
#ifdef WIRELESS_DEBUG
				nikWireless.wl_debug[2] = nikWireless.WL_buf[0];
#endif
				CLR_WLN_BUF
			}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_READY_FRONT)
			{
				nikWireless.preTotal = 0;
				nikWireless.fp = LOW_SPEED;
				switch(wlSet.s_arrary)
				{
				case WL_ARRARY_A:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[0]<<4) | (nikWireless.WL_buf[1]>>4));
					/*
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					//CLR_WL_BUF
					//nikWireless.WL_opCode&= 0x7f;
					CLR_WLN_BUF
					*/
				break;

				case WL_ARRARY_B:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[1]<<4) | (nikWireless.WL_buf[2]>>4));
					/*
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
					*/
				break;

				case WL_ARRARY_C:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[2]<<4) | (nikWireless.WL_buf[3]>>4));
					/*
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}
					else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
					*/
				break;
				if(wlSet.s_wlMode==WORKINGMODE_MANU)
				{
					nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
					nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
					if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
					{
						nikWireless.WL_output1 = 5;
						nikWireless.WL_output2 = 0;
					}
					if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
					{
						nikWireless.WL_output2 = 0;
					}
				}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
				{
					WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
				}
				else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
				{
					Nik_AutoShotSet(nikWireless.WL_opCode);
				}
				nikWireless.WL_statue = NIKWL_STATUE_FLASH;
				CLR_WLN_BUF
				//CLR_WL_BUF
				}
			}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_READY_REAR)
			{
				nikWireless.preTotal = 0;
				nikWireless.fp = LOW_SPEED;
				switch(wlSet.s_arrary)
				{
				case WL_ARRARY_A:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[0]<<4) | (nikWireless.WL_buf[1]>>4));
				/*
					nikWireless.RearReserve = nikWireless.WL_opCode;
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					//CLR_WL_BUF
					//nikWireless.WL_opCode&= 0x7f;
					CLR_WLN_BUF
				*/
				break;

				case WL_ARRARY_B:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[1]<<4) | (nikWireless.WL_buf[2]>>4));
				/*
					nikWireless.RearReserve = nikWireless.WL_opCode;
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				*/
				break;

				case WL_ARRARY_C:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[2]<<4) | (nikWireless.WL_buf[3]>>4));
				/*
					nikWireless.RearReserve = nikWireless.WL_opCode;
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				*/
				break;

				}
				nikWireless.RearReserve = nikWireless.WL_opCode;
				if(wlSet.s_wlMode==WORKINGMODE_MANU)
				{
					nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
					nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
					if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
					{
						nikWireless.WL_output1 = 5;
						nikWireless.WL_output2 = 0;
					}
					if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
					{
						nikWireless.WL_output2 = 0;
					}
				}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
				{
					WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
				}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
				{
					Nik_AutoShotSet(nikWireless.WL_opCode);
				}
				nikWireless.WL_statue = NIKWL_STATUE_FLASH;
				CLR_WLN_BUF
				//CLR_WL_BUF
			}else
			{
				 //nikWireless.wl_debug = nikWireless.WL_buf[0];
				//nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				//nikWireless.WL_statue = CANWL_STATUE_HOLD;
				//wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
				//nikWireless.bitLoadIndex = 0;
				//nikWireless.WLBufIndex = 0;
				CLR_WL_FLASH_TIMEOUT;
				nikWireless.preArr = WL_ARRARY_NULL;
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
				nikWireless.bitLoadIndex = 0;
				nikWireless.WLBufIndex = 0;
				nikWireless.flash_wizard = WL_WIZARD_HOLD;
				wlSet.s_wlMode = WORKINGMODE_TURNOFF;
				nikWireless.preTurn=WL_PFCOUNT_RDY;
				CLR_WLN_BUF
			}
			nikWireless.WL_statue = NIKWL_STATUE_PREFLASH;
		}
				
		
	}else if(wlSet.s_flashStatue == WL_FLASH_STATUE_AFTPF)
	{
		if((nikWireless.WL_buf[0]==WL_REARFLASH_COMMAND)||(nikWireless.WL_buf[0]==WL_REARFLASH_COMMANDex))
		{
			if(0x01==flashStatue.flash_enable)
			{
				//flashPara.output1 = nikWireless.WL_output1;
				//flashPara.output2 = nikWireless.WL_output2;
				if(WORKINGMODE_TTL==wlSet.s_wlMode)
				{
					WLS_ExposeTimeCal(nikWireless.RearReserve,nikWireless.fp);
				}
				//Tprintf("2:%x \n",nikWireless.RearReserve);
				//putchar(0x02);
				//putchar(nikWireless.RearReserve);
				pTime = OutputSwitch(wlSet.s_wlMode);
				//disable_irq();
				TURN_ON_TRIGER(pTime);
				flashPara.ExposeState = 0;
				//nikWireless.flash_wizard = WL_WIZARD_HOLD;
/**********************************************************************/
//				the following code in order to clear the receive process
/**********************************************************************/
				CLR_WL_FLASH_TIMEOUT;
				nikWireless.preArr = WL_ARRARY_NULL;
				nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
				nikWireless.WL_statue = NIKWL_STATUE_HOLD;
				wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
				nikWireless.bitLoadIndex = 0;
				nikWireless.WLBufIndex = 0;
				nikWireless.flash_wizard = WL_WIZARD_HOLD;
				//wlSet.s_wlMode = WORKINGMODE_TURNOFF;
				nikWireless.preTurn=WL_PFCOUNT_RDY;
				CLR_WLN_BUF;
/**********************************************************************/
				//DecodeFlashData(UART_ENCODE_CMD,UART_ENCODE_CMD_ONETIMEFLASH,0);
				//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
				//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
				uartSendFlag.flashWarn= 1;
				//SETBIT(bitWireless,BIT_bitWireless_mode);
				//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLMODE);
				uartSendFlag.wlm_mode = 1;
				//if(WORKINGMODE_MANU==wlSet.s_wlMode)
				//{
				//	SETBIT(bitWireless,BIT_bitWireless_Moutp);
				//	SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_WLEXP);
				//}
				//enable_irq();
				//EX0 = 1;
				return;
				
			}
		}
		
		if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_READY_FRONT)		//head at front synchronic
		{
#ifdef WIRELESS_DEBUG
			nikWireless.wl_debug[0] = nikWireless.WL_buf[0];
			nikWireless.wl_debug[1] = nikWireless.WL_buf[1];
			nikWireless.wl_debug[2] = nikWireless.WL_buf[2];
			nikWireless.wl_debug[3] = nikWireless.WL_buf[3];
#endif

			nikWireless.fp = LOW_SPEED;

			switch(wlSet.s_arrary)
			{
				case WL_ARRARY_A:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[0]<<4) | (nikWireless.WL_buf[1]>>4));
				/*
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					//CLR_WL_BUF
					//nikWireless.WL_opCode&= 0x7f;
					CLR_WLN_BUF
				*/
				break;

				case WL_ARRARY_B:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[1]<<4) | (nikWireless.WL_buf[2]>>4));
				/*
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				*/
				break;

				case WL_ARRARY_C:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[2]<<4) | (nikWireless.WL_buf[3]>>4));
				/*
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}
					else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				*/
				break;

			}
			if(wlSet.s_wlMode==WORKINGMODE_MANU)
			{
				nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
				nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
				if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
				{
					nikWireless.WL_output1 = 5;
					nikWireless.WL_output2 = 0;
				}
				if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
				{
					nikWireless.WL_output2 = 0;
				}
			}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
			{
				WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
			}
			else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
			{
				Nik_AutoShotSet(nikWireless.WL_opCode);
			}
			nikWireless.WL_statue = NIKWL_STATUE_FLASH;
			CLR_WLN_BUF
					//CLR_WL_BUF
			//WLDEBUG_BREAK_POINT
			//CLR_WL_BUF

		}else if((nikWireless.WL_buf[0]&0xf0)==BLFLAG_READY_REAR)//head at rear synchronic
		{

			nikWireless.fp = LOW_SPEED;
			
			switch(wlSet.s_arrary)
			{
				case WL_ARRARY_A:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[0]<<4) | (nikWireless.WL_buf[1]>>4));
				/*
					nikWireless.RearReserve = nikWireless.WL_opCode;
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					//CLR_WL_BUF
					//nikWireless.WL_opCode&= 0x7f;
					CLR_WLN_BUF
				*/
				break;

				case WL_ARRARY_B:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[1]<<4) | (nikWireless.WL_buf[2]>>4));
				/*
					nikWireless.RearReserve = nikWireless.WL_opCode;
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				*/
				break;

				case WL_ARRARY_C:
					nikWireless.WL_opCode= ((nikWireless.WL_buf[2]<<4) | (nikWireless.WL_buf[3]>>4));
				/*
					nikWireless.RearReserve = nikWireless.WL_opCode;
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				*/
				break;

			}
			nikWireless.RearReserve = nikWireless.WL_opCode;
			if(wlSet.s_wlMode==WORKINGMODE_MANU)
			{
				nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
				nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
				if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
				{
					nikWireless.WL_output1 = 5;
					nikWireless.WL_output2 = 0;
				}
				if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
				{
					nikWireless.WL_output2 = 0;
				}
			}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
			{
				WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
			}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
			{
				Nik_AutoShotSet(nikWireless.WL_opCode);
			}
			nikWireless.WL_statue = NIKWL_STATUE_FLASH;
			CLR_WLN_BUF
			//CLR_WL_BUF
		}else if(PREFLASH_LOCK==(nikWireless.WL_buf[0]&0xf0))
		{
			nikWireless.preflashLock = PREFLASH_LOCK_ON;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
		}/*else if((HPSYN_FLASH_LOCK1==nikWireless.WL_buf[0])||(HPSYN_FLASH_LOCK2==nikWireless.WL_buf[0]))
		{
			nikWireless.preflashLock = PREFLASH_LOCK_ON;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
		}*/
		/*
		else if((0xDB==nikWireless.WL_buf[0])||(0xD8==nikWireless.WL_buf[0])||(0xDC==nikWireless.WL_buf[0])\
			||(0xDA==nikWireless.WL_buf[0]))		*/
		else if((0xd8<=nikWireless.WL_buf[0])&&(0xdc>=nikWireless.WL_buf[0]))//high speed shutter
		{
			nikWireless.WL_opCode = nikWireless.WL_buf[wlSet.s_arrary];
			if(wlSet.s_wlMode==WORKINGMODE_MANU)
			{
				nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
				nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
				//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
				if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
				{
					nikWireless.WL_output1 = 5;
					nikWireless.WL_output2 = 0;
				}
				if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
				{
					nikWireless.WL_output2 = 0;
				}
			}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
			{
				WLS_ExposeTimeCal(nikWireless.WL_opCode,nikWireless.fp);
			}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
			{
				Nik_AutoShotSet(nikWireless.WL_opCode);
			}
			if(0xDB==nikWireless.WL_buf[0])
			{
				nikWireless.fp = HIGH_SPEED_LEVEL2;
			}else
			{
				nikWireless.fp = HIGH_SPEED_LEVEL1;
			}
			
			nikWireless.WL_statue = NIKWL_STATUE_FLASH;
			CLR_WLN_BUF;
			
/*
			switch(wlSet.s_arrary)
			{
				case WL_ARRARY_A:
					nikWireless.WL_opCode= nikWireless.WL_buf[1];
					//if(wlSet.s_wlMode==WORKINGMODE_MANU)
					//if((wlSet.s_wlMode==WORKINGMODE_MANU)||(wlSet.s_wlMode==WORKINGMODE_TTL))
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						//nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					//CLR_WL_BUF
					//nikWireless.WL_opCode&= 0x7f;
					CLR_WLN_BUF;
				break;

				case WL_ARRARY_B:
					nikWireless.WL_opCode= nikWireless.WL_buf[2];
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					//if((wlSet.s_wlMode==WORKINGMODE_MANU)||(wlSet.s_wlMode==WORKINGMODE_TTL))
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				break;

				case WL_ARRARY_C:
					nikWireless.WL_opCode= nikWireless.WL_buf[2];
					//if((wlSet.s_wlMode==WORKINGMODE_MANU)||(wlSet.s_wlMode==WORKINGMODE_TTL))
					if(wlSet.s_wlMode==WORKINGMODE_MANU)
					{
						nikWireless.WL_output1= (8-((((nikWireless.WL_opCode&0x7f)>>1)+2)/3));
						nikWireless.WL_output2= (2-((((nikWireless.WL_opCode&0x7f)>>1)+2)%3));
						if(!((nikWireless.WL_output1>0)&&(nikWireless.WL_output1<=8)))
						{
							nikWireless.WL_output1 = 5;
							nikWireless.WL_output2 = 0;
						}
						if(!((nikWireless.WL_output2>=0)&&(nikWireless.WL_output2<=2)))
						{
							nikWireless.WL_output2 = 0;
						}
					}else if(wlSet.s_wlMode==WORKINGMODE_TTL)
					{
						WLS_ExposeTimeCal(nikWireless.WL_opCode);
					}else if(WORKINGMODE_AUTO==wlSet.s_wlMode)
					{
						Nik_AutoShotSet(nikWireless.WL_opCode);
					}
					nikWireless.WL_statue = NIKWL_STATUE_FLASH;
					CLR_WLN_BUF
					//CLR_WL_BUF
				break;
			}//this is observed in the D90and sp900version
*/
	}else if((0xE8==nikWireless.WL_buf[0])||(0xEB==nikWireless.WL_buf[0]))
	{
		nikWireless.preflashLock = PREFLASH_LOCK_ON;
		nikWireless.WL_statue = NIKWL_STATUE_HOLD;
	}/*else if(0xD8==nikWireless.WL_buf[0])
	{
		
	}*/else
		{
			//puts("E6");
			CLR_WL_FLASH_TIMEOUT;
			nikWireless.preArr = WL_ARRARY_NULL;
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			nikWireless.WL_statue = NIKWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			nikWireless.bitLoadIndex = 0;
			nikWireless.WLBufIndex = 0;
			nikWireless.flash_wizard = WL_WIZARD_HOLD;
			wlSet.s_wlMode = WORKINGMODE_TURNOFF;
			nikWireless.preTurn=WL_PFCOUNT_RDY;
			ttl_pre2_flag = 0;
			CLR_WLN_BUF;

			
		}
	}
	else if(wlSet.s_flashStatue ==WL_FLASH_STATUE_FLASH)
	{}

	//FLASHCTL = ~FLASHCTL;
	//UPDATA_TIMER0(0xf8,0xcf);

	//nikWireless.WL_statue = NIKWL_STATUE_STOP;

	STOP_TIMER0
	CLR_WLN_BUF
	}
}



void int_TM1(void)
{
	static unsigned char counter;
	static int  mulCnt=0;
	static char serialCnt=0;
	static char handleCnt=0;
	static unsigned char xdata head_timeOut=0;
	static int cntTimes=0;
	static unsigned char twikleLit=0;
	static unsigned char f2FlashCnt=0;
	
	ANCTR2 |= 0x80;		//AD simpling

	if(GETBIT(bitFlag,BIT_BITFLAG_WLFLASHTO))
	{
		
#ifdef _CANON_WIRELESS_
		if(canWireless.WL_timeout++>500)
		{
			//CLRBIT(bitFlag,BIT_BITFLAG_WLTIMEOUT);
			CLR_WL_FLASH_TIMEOUT
			canWireless.bitLoadIndex = 0;
			canWireless.WL_Channel = WL_CHANNEL_UNSURE;
			canWireless.WL_statue = CANWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;	
			canWireless.WLBufIndex = 0;
#ifdef WIRELESS_DEBUG
			for(wlBufSize=0;wlBufSize<WL_BUF_SIZE;wlBufSize++)
			{
				wl_buf[wlBufSize] = 0;
			}
			wlBufSize = 0;
#endif
		}
#endif
#ifdef _NIKON_WIRELESS_

		if(nikWireless.WL_timeout++>600)
		{
			//CLRBIT(bitFlag,BIT_BITFLAG_WLTIMEOUT);
			CLR_WL_FLASH_TIMEOUT
			nikWireless.bitLoadIndex = 0;
			nikWireless.WL_Channel = WL_CHANNEL_UNSURE;
			nikWireless.WL_statue = CANWL_STATUE_HOLD;
			wlSet.s_flashStatue = WL_FLASH_STATUE_HOLD;
			nikWireless.flash_wizard = WL_WIZARD_HOLD;
			nikWireless.WLBufIndex = 0;
		}
		
		cntTimes = 0;
		twikleLit = 0;		
#endif
	}else
	{
	
		//if(WORKINGMODE_RTN==flashPara.workmode)
		if(WORKINGMODE_MASTER<flashPara.workmode)
		{
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
			if(++twikleLit>=4)
			{
				LoadUartDate();
				//cntTimes ++;
				twikleLit = 0;
			}

			cntTimes++;
			if(cntTimes==50)
			{
				//LoadUartDate();
				if(flashPara.rmtIndex)
				{
					LED_LITE = 1;
				}
			}else if(cntTimes==51)
			{
				if(flashPara.rmtIndex)
				{
					LED_LITE = 0;
				}
			}else if(cntTimes>1000)
			{
				cntTimes = 0;
			}
#else
			if(cntTimes++>50)
			{
				LoadUartDate();
				if(++twikleLit>40)
				{
					AFCTL= 1;
				}else if(twikleLit>50)
				{
					AFCTL= 0;
					twikleLit = 0;
				}
				cntTimes = 0;
			}
#endif
		}else
		{
			if(++serialCnt>50)
			{
				//uartSendFlag.imfo_iso = 1;
				//uartSendFlag.imfo_apert = 1;
				//uartSendFlag.cls = 1;
				//uartSendFlag.imfo_focLen = 1;
				//uartSendFlag.charge = 1;
				//uartSendFlag.rearCurtain = 1;
				LoadUartDate();
				serialCnt = 0;
			}
		}
	}

	if(wlSet.testPilot)
	{
		if(!wlSet.testCnt--)
		{
			TURN_ON_TRIGER(PILOT_EXP);
			wlSet.testPilot = TEST_PILOT_OFF;
			uartSendFlag.flashWarn = 1;
		}
		
	}

	if(F2FLASH_LABEL_SET==g_f2FlashLabel)
	{
		if(f2FlashCnt++>200)
		{
			f2FlashCnt = 0;
			g_f2FlashLabel = F2FLASH_LABEL_CLR;
		}
	}else if(f2FlashCnt)
	{
		f2FlashCnt = 0;
	}
	

#ifdef _MULTIPLY_FLASH_

	//if((WORKINGMODE_MULTI==flashPara.workmode)&&(MULTI_FLASH_ENABLE==multiSet.mulCtl))
	if(MULTI_FLASH_ENABLE==multiSet.mulCtl)
	{
			//putchar(multiSet.finalExpTimes);
			if(MULTI_MODE_UNCAM==multiSet.cMode)
			{
				if(FLASHCTL!=0)
				{
					//MUL_PCATIMER_STOP
					multiSet.mulCtl = MULTI_FLASH_DISABLE;
					multiSet.finalExpTimes=0;
					multiSet.mulCnt = 0;
					multiSet.startFlag = MULTI_STFLAG_HOLD;
					uartSendFlag.flashWarn= 1;
					//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
					//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
					//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
				}
			}else
			{
				if(charge_ok==AD_CHARGE_LOW)
				{
					multiSet.finalExpTimes=0;
					//mulCnt = 0;
					//multiSet.mulCtl = MULTI_FLASH_DISABLE;
					if(WORKINGMODE_RTN==flashPara.workmode)
					{
						Timer7SetCapture();
					}
					//SETBIT(paraBit,BIT_paraBit_ONE_TIME_FLASH);
				}
				if(multiSet.finalExpTimes>0)
				{
					//putchar(mulCnt);
					
					multiSet.mulCnt++;
					if(multiSet.mulCnt>=multiSet.lowFrqCnt)
					{
						pTime = OutputSwitch(WORKINGMODE_MULTI);
						TURN_ON_TRIGER(pTime);
						
						multiSet.mulCnt = 0;

						//Tprintf("%d ",multiSet.finalExpTimes);
						//putchar(multiSet.finalExpTimes>>8);
						//putchar(multiSet.finalExpTimes&0xff);

						//if((--multiSet.finalExpTimes<=0)||(!NIKONMOSI))
						//if(--multiSet.finalExpTimes<=0)
						//if((FLASHCTL!=0)||(--multiSet.finalExpTimes<=0))
						if(--multiSet.finalExpTimes<=0)
						{
							//MUL_PCATIMER_STOP
							multiSet.mulCtl = MULTI_FLASH_DISABLE;
							multiSet.finalExpTimes=0;
							multiSet.startFlag = MULTI_STFLAG_HOLD;
							uartSendFlag.flashWarn= 1;
							//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ONEFLASH);
							//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
							//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
							//NIKONNSSO= 1;
							while(!NIKONNSSO);
					//		InitUart();
					//		if(!NIKONMOSI)
					//		{
					//			NIKONMOSI = 1;
					//		}
							//puts("fin\n");
							//NIKONMOSI = 1;
							//NIKONNSS = 0;
						}
					}			
				
				}else 
				{
					multiSet.mulCnt = 0;
					multiSet.finalExpTimes=0;
					multiSet.mulCtl = MULTI_FLASH_DISABLE;
					//MUL_PCATIMER_STOP
						
				}
			}
		}
#endif

	if(++handleCnt>10)
	{
		if(WrongCodeFilter)
		{
			if(++head_timeOut>100)
			{
				head_timeOut = 0;
				WrongCodeFilter = HEAD_DATA_UNRCVD;
			}
		}else
		{
			head_timeOut = 0;
		}

		if(GETBIT(bitChar,BIT_BITCHAR_S2SETFLAG))
		{
			if(!flashStatue.s2_count--)
			{
				CLRBIT(bitChar,BIT_BITCHAR_S2SETFLAG);
				flashStatue.s2_count = 0;
			}
		}

		if(lowBatCount++>8000)
		{
			if(!GETBIT(bitChar,BIT_BITCHAR_LOWBATWARN))
			{
				SETBIT(bitChar,BIT_BITCHAR_LOWBATWARN);
				SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_LOWBAT);
			}
			if(lowBatCount>9000)
			{
				lowBatCount = 0;
			}
			//lowBatCount = 0;
		}else
		{
			CLRBIT(bitChar,BIT_BITCHAR_LOWBATWARN);
		}

//		if(counter++>50)
//		{
//			uartSend.sendFlag |= 0xff;
//			counter = 0;
//		}

		if(uartSend.cls)
		{
			if(!uartSend.cls_invalid--)
			{
				uartSend.cls = 0;
				uartSendFlag.cls = 1;
				NIKONSCK = 0;
				//NIKONNSSO = 0;
			}
		}else {
			if(counter++>50)
			{
				//uartSendFlag.charge = 1;
				counter = 0;
			}
		}
	}
	
//	CHARGE_ENABLE = ~CHARGE_ENABLE;
//	TEST_OUT = ~TEST_OUT;
}


void int_TM7(void)
{
//	TEST_OUT = ~TEST_OUT;
}

#if 0
void int_AD(void)
{
   static unsigned long accumulator = 0;     // accumulator for averaging
   static unsigned int measurements = 256;  // measurement counter
   unsigned long result=0;
//   static unsigned char adc_indicator;

   //unsigned long mV;                         // measured voltage in mV

//   AD0INT = 0;                               // clear ADC0 conv. complete flag

//   buf1 = ANBUF1;
//   buf0 = ANBUF0;
//   accumulator +=(((buf1<<2)+(buf0>>6)));
   accumulator += ANBUF1;
//   measurements--;

//   if(measurements == 0)
    if(!(measurements--))
   {  
      measurements = 256;
      result = accumulator >> 8;
      accumulator=0;

      // The 10-bit ADC value is averaged across 2048 measurements.  
      // The measured voltage applied to P1.4 is then:
      //
      //                           Vref (mV)
      //   measurement (mV) =   --------------- * result (bits) 
      //                       (2^10)-1 (bits)

  //   mV =  result * 2440 / 1023;   


	  

		if(result<(unsigned long)VOL_AD_FLALEVEL)
		{
			
			IGBT_CONTROL = 0;	
#ifdef AF_EN_TEST
			AF_EN = 0;
#endif

#ifdef UART_SENDDATA_CONTINUE
			if(charge_ok!=AD_CHARGE_LOW)
			{
				SETBIT(bitIndex,BIT_bitIndex_CHARGE);
				SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
			}
			charge_ok = AD_CHARGE_LOW;
			flashStatue.flash_enable = 0x01;
#else
			SETBIT(bitIndex,BIT_bitIndex_CHARGE);
#endif
		}
		else if(result<(unsigned long)VOL_AD_FULL)
		{
		//	StopCharge(); 
		//	charge_ok=1;
			IGBT_CONTROL = 1;
#ifdef AF_EN_TEST

			AF_EN = 1;
#endif
#ifdef UART_SENDDATA_CONTINUE
			if(charge_ok!=AD_CHARGE_RDY)
			{
				SETBIT(bitIndex,BIT_bitIndex_CHARGE);
				SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
			}
			charge_ok = AD_CHARGE_RDY;
			flashStatue.flash_enable = 0x01;
#else
			SETBIT(bitIndex,BIT_bitIndex_CHARGE);
#endif
		}else{
			IGBT_CONTROL = 1;
#ifdef AF_EN_TEST

			AF_EN = 1;
#endif
			if(charge_ok!=AD_CHARGE_FUL)
			{
				SETBIT(bitIndex,BIT_bitIndex_CHARGE);
				SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CHARGE);
			}
			charge_ok = AD_CHARGE_FUL;
			flashStatue.flash_enable = 0x01;

		}
	}

}
#endif

void int_AD(void)
{
   static unsigned long accumulator = 0;     // accumulator for averaging
   //static unsigned int measurements = 256;  // measurement counter
   static unsigned int measurements = 16;  // measurement counter
   unsigned long result=0;
//   static unsigned char adc_indicator;

   //unsigned long mV;                         // measured voltage in mV

//   AD0INT = 0;                               // clear ADC0 conv. complete flag

//   buf1 = ANBUF1;
//   buf0 = ANBUF0;
//   accumulator +=(((buf1<<2)+(buf0>>6)));
   accumulator += ANBUF1;
//   measurements--;

//   if(measurements == 0)
    if(!(measurements--))
   {  
      //measurements = 256;
      measurements = 32;
      //result = accumulator >> 8;
      result = accumulator >> 5;
      accumulator=0;

      // The 10-bit ADC value is averaged across 2048 measurements.  
      // The measured voltage applied to P1.4 is then:
      //
      //                           Vref (mV)
      //   measurement (mV) =   --------------- * result (bits) 
      //                       (2^10)-1 (bits)

  //   mV =  result * 2440 / 1023;   


	  

		if(result<(unsigned long)VOL_AD_FLALEVEL)
		{

			if(charge_ok!=AD_CHARGE_LOW)
			{
				//IGBT_CONTROL = 0;	
				charge_ok = AD_CHARGE_LOW;
				//wlSet.m_charge_statue = CHARGE_UNDER;
				flashStatue.flash_enable &= FLASH_STATE_INCHARGE;
				uartSendFlag.charge = 1;
			}
		}
		else if(result<(unsigned long)VOL_AD_FULL)
		{
			if(charge_ok!=AD_CHARGE_RDY)
			{				
				charge_ok = AD_CHARGE_RDY;
				flashStatue.flash_enable |= FLASH_STATE_CHARGEOK;
				//wlSet.m_charge_statue = CHARGE_OK;
				//IGBT_CONTROL = 1;
				uartSendFlag.charge = 1;
			}
			
		}else{
			if(charge_ok!=AD_CHARGE_FUL)
			{
				charge_ok = AD_CHARGE_FUL;
				flashStatue.flash_enable |= FLASH_STATE_CHARGEOK;
				//wlSet.m_charge_statue = CHARGE_FULL;
				//IGBT_CONTROL = 1;
				uartSendFlag.charge = 1;
			}
		}
	}

}





void Delay(void)
{
	int a;
	for(a=0;a<3;a++);
}

