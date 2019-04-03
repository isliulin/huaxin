#include "flash.h"
#include"global.h"
#include <stdarg.h>
#include "..\lcd\lcd.h"
#include "..\spi\spi.h"
#include "..\sys\sys.h"
#include "..\uart\uart.h"
#include "..\a7105\a7105.h"

u16 g_flashTime = 0;
u16 g_multiTimer = 0;
u16 g_multiCnt = 0;
u8 g_flashEnable = 0;
u8 g_sFlashTig=0;
u8 g_sRcvCnt = 0;
u8 g_sRearFlag = 0;
u8 g_sRcvState = 0;
u8 g_sFlashEn = 0;
u8 g_multiFlashEnable = 0;
s8 g_expTTLPara1=0;
s8 g_expTTLPara2=0;
u16 g_s2Timer = 0;
u8 g_s2Flag = 0;
u16 g_nikPreTmr1 = 0;
u16 g_nikPreTmr2 = 0;
extern CmrPara cmrPara;
extern const u32 isoValue[];
extern u8 ttl_pre_dbg;
u16 g_slaveFlashCounter = 0;

void ReadyForFlash(void);
void CanExposeTimeCal(const u8 i_camCode,u8 fpMod);
void NikExposeTimeCal(const unsigned char i_camCode,unsigned char hp,s8 cmp1, s8 cmp2);

/*
const u16 g_expTime[31] = {TTLTRA0,TTLTRA1,TTLTRA2,TTLTRA3,TTLTRA4,TTLTRA5,
					      OUTPUT_00,OUTPUT_01,OUTPUT_02,OUTPUT_10,OUTPUT_11,OUTPUT_12,
					      OUTPUT_20,OUTPUT_21,OUTPUT_22,OUTPUT_30,OUTPUT_31,OUTPUT_32,
					      OUTPUT_40,OUTPUT_41,OUTPUT_42,OUTPUT_50,OUTPUT_51,OUTPUT_52,
					      OUTPUT_60,OUTPUT_61,OUTPUT_62,OUTPUT_70,OUTPUT_71,OUTPUT_72,
					      OUTPUT_80};
					      
*/
//const u16 g_preTimeSet[6] = {V320_CAN_PRE,V310_CAN_PRE,V300_CAN_PRE,V290_CAN_PRE,V280_CAN_PRE,V270_CAN_PRE};

const u16 g_expTime[6][31] = 
{
#if 0
	{
	V330_1024_0,V330_1024_1,V330_1024_2,V330_512_0,V330_512_1,V330_512_2,
	V330_256_0, V330_256_1, V330_256_2, V330_128_0,V330_128_1,V330_128_2,
	V330_64_0,  V330_64_1,  V330_64_2,  V330_32_0, V330_32_1, V330_32_2,
	V330_16_0,  V330_16_1,  V330_16_2,  V330_8_0,  V330_8_1,  V330_8_2,
	V330_4_0,   V330_4_1,   V330_4_2,   V330_2_0,  V330_2_1,  V330_2_2,
	V330_1_0
	},
#endif
	{
	V320_1024_0,V320_1024_1,V320_1024_2,V320_512_0,V320_512_1,V320_512_2,
	V320_256_0, V320_256_1, V320_256_2, V320_128_0,V320_128_1,V320_128_2,
	V320_64_0,  V320_64_1,  V320_64_2,  V320_32_0, V320_32_1, V320_32_2,
	V320_16_0,  V320_16_1,  V320_16_2,  V320_8_0,  V320_8_1,  V320_8_2,
	V320_4_0,   V320_4_1,   V320_4_2,   V320_2_0,  V320_2_1,  V320_2_2,
	V320_1_0
	},
	{
	V310_1024_0,V310_1024_1,V310_1024_2,V310_512_0,V310_512_1,V310_512_2,
	V310_256_0, V310_256_1, V310_256_2, V310_128_0,V310_128_1,V310_128_2,
	V310_64_0,  V310_64_1,  V310_64_2,  V310_32_0, V310_32_1, V310_32_2,
	V310_16_0,  V310_16_1,  V310_16_2,  V310_8_0,  V310_8_1,  V310_8_2,
	V310_4_0,   V310_4_1,   V310_4_2,   V310_2_0,  V310_2_1,  V310_2_2,
	V310_1_0
	},
	{
	V300_1024_0,V300_1024_1,V300_1024_2,V300_512_0,V300_512_1,V300_512_2,
	V300_256_0, V300_256_1, V300_256_2, V300_128_0,V300_128_1,V300_128_2,
	V300_64_0,  V300_64_1,  V300_64_2,  V300_32_0, V300_32_1, V300_32_2,
	V300_16_0,  V300_16_1,  V300_16_2,  V300_8_0,  V300_8_1,  V300_8_2,
	V300_4_0,   V300_4_1,   V300_4_2,   V300_2_0,  V300_2_1,  V300_2_2,
	V300_1_0
	},
	{
	V290_1024_0,V290_1024_1,V290_1024_2,V290_512_0,V290_512_1,V290_512_2,
	V290_256_0, V290_256_1, V290_256_2, V290_128_0,V290_128_1,V290_128_2,
	V290_64_0,  V290_64_1,  V290_64_2,  V290_32_0, V290_32_1, V290_32_2,
	V290_16_0,  V290_16_1,  V290_16_2,  V290_8_0,  V290_8_1,  V290_8_2,
	V290_4_0,   V290_4_1,   V290_4_2,   V290_2_0,  V290_2_1,  V290_2_2,
	V290_1_0
	},
	{
	V280_1024_0,V280_1024_1,V280_1024_2,V280_512_0,V280_512_1,V280_512_2,
	V280_256_0, V280_256_1, V280_256_2, V280_128_0,V280_128_1,V280_128_2,
	V280_64_0,  V280_64_1,  V280_64_2,  V280_32_0, V280_32_1, V280_32_2,
	V280_16_0,  V280_16_1,  V280_16_2,  V280_8_0,  V280_8_1,  V280_8_2,
	V280_4_0,   V280_4_1,   V280_4_2,   V280_2_0,  V280_2_1,  V280_2_2,
	V280_1_0
	},
	{
	V270_1024_0,V270_1024_1,V270_1024_2,V270_512_0,V270_512_1,V270_512_2,
	V270_256_0, V270_256_1, V270_256_2, V270_128_0,V270_128_1,V270_128_2,
	V270_64_0,  V270_64_1,  V270_64_2,  V270_32_0, V270_32_1, V270_32_2,
	V270_16_0,  V270_16_1,  V270_16_2,  V270_8_0,  V270_8_1,  V270_8_2,
	V270_4_0,   V270_4_1,   V270_4_2,   V270_2_0,  V270_2_1,  V270_2_2,
	V270_1_0
	},
};


void TURN_ON_TRIGER(u16 time)
{
	u16 crtTime = 0;

	DISABLE_CHARGE();                                  // 关闭充电
	
	FLASH_TRIGER = 1;
	delay_us(5);
	FLASH_PWM = 1;
	delay_us(20);
	FLASH_TRIGER = 0;

	if (time < 20)
	{
		ENABLE_CHARGE();
		FLASH_PWM = 0;
		return;
	}else
	{
		time -= 20;
	}

	crtTime = get_counter();

	while (get_counter() - crtTime < time);

	FLASH_PWM = 0;

	ENABLE_CHARGE();					// 打开充电

}

u8 VolLevel(u8 vol)
{/*
	if (vol<157)
	{
		return 6;
	}else if(vol<193)
	{
		return 6-((vol-157)/6);
	}else
	{
		return 0;
	}*//*
	if (vol < 169)
	{
		return 5;               // <280
	}else if (vol < 175)
	{
		return 4;		// <290
	}else if (vol < 181)
	{
		return 3;		// <300
	}else if (vol < 187)
	{
		return 2;		// <310
	}else if (vol < 193)
	{
		return 1;		// <320
	}else 
	{
		return 0;		// >=320
	}*/
	if (vol < 169)
	{
		return 4;               // <280
	}else if (vol < 175)
	{
		return 3;		// <290
	}else if (vol < 181)
	{
		return 2;		// <300
	}else if (vol < 187)
	{
		return 1;		// <310
	}else if (vol < 193)
	{
		return 0;		// <320
	}else 
	{
		return 0;		// >=320
	}
	
}

u16 OutputSwitch(s8 value1,s8 value2)
{
	//char value1, value2;
	//char surtmp1=0,surtmp2=0;
	u16 TMR2=0;

	/*check voltage before flash*/
	//g_volRef = CheckVolBeforeFlash();
	//g_volRef = 193;                                        // for test

	//if((value1<=7)&&(value2<=2))
	if(value1<=7)
	{
		//TMR2 = g_expTime[(value1+2)*3+value2];
		TMR2 = g_expTime[VolLevel(g_volRef)][(value1+3)*3+value2];
	}else
	{
		//TMR2 = OUTPUT_80;
		TMR2 =  g_expTime[VolLevel(g_volRef)][30];
	}

	return TMR2;
	
}

void FlashFinishProc(void)
{
	if ((COM_STATUE_REALFLASH == g_canComStatue)&&(WORKINGMODE_TTL == *g_lightMode))
	{
		if ((flaPara.ttl_FEB1)||(flaPara.ttl_FEB2))
		{
			flaPara.FEB_count ++;
			if (flaPara.FEB_count >= 3)
			{
				flaPara.FEB_count = 0;
				if (!g_cfnOption[CFN_FEB_OFF])
				{
					flaPara.ttl_FEB1 = 0;
					flaPara.ttl_FEB2 = 0;
				}
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);

		}
	}
}

void NikPreFlash(void)
{
	if (ttl_pre2_flag > 0)
	{
		//g_flashTime = OUTPUT_20;
               //g_flashTime = g_expTime[VolLevel(g_volRef)][(2+2)*3+0];
               g_flashTime = g_expTime[VolLevel(g_volRef)][NIK_PRE2_LEV];
		g_nikPreTmr1 = g_flashTime;
	}else
	{
		//g_flashTime = OUTPUT_10;
               //g_flashTime = g_expTime[VolLevel(g_volRef)][(1+2)*3+0];
               g_flashTime = g_expTime[VolLevel(g_volRef)][NIK_PRE1_LEV];
		g_nikPreTmr2 = g_flashTime;
	}

	if (g_chargeOk > 0)
	{
		TURN_ON_TRIGER(g_flashTime);
	}
}

u16 AdToVol(u16 ad)
{
	if (ad < 157)
	{
		return 260;
	}else if(ad < 163)
	{
		return 270;
	}else if(ad < 169)
	{
		return 280;
	}else if(ad < 175)
	{
		return 290;
	}else if(ad < 181)
	{
		return 300;
	}else if(ad < 187)
	{
		return 310;
	}else if(ad < 193)
	{
		return 320;
	}else
	{
		return 330;
	}
	
	//return (330l - (((193l-ad)*10)/6));
}

#pragma optimize=none
void CanPreFlash(void)
{
	//g_flashTime = OUTPUT_10;
       //g_flashTime = g_expTime[VolLevel(g_volRef)][(1+2)*3+0];
       //g_flashTime = g_expTime[VolLevel(g_volRef)][CAN_PRE_LEV];
       /*
       g_flashTime = g_preTimeSet[VolLevel(g_volRef)];
	if (g_chargeOk > 0)
	{
		TURN_ON_TRIGER(g_flashTime);
	}*/
	/*驱动波形为第一个脉冲32us，后面15个小脉冲间隔14us，触发9us*/
	//u16 crtTime = 0;
	u8 plusCnt = 0;

	DISABLE_CHARGE();                                  // 关闭充电
	
	FLASH_TRIGER = 1;
	delay_us(5);
	FLASH_PWM = 1;
	delay_us(20);
	FLASH_TRIGER = 0;

	//crtTime = get_counter();

	//while (get_counter() - crtTime < 32);
	delay_us(1);

	FLASH_PWM = 0;

	for (plusCnt = 0; plusCnt < 15; plusCnt ++)
	{
		delay_us(8);
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		FLASH_PWM = 1;
		delay_us(7);
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		FLASH_PWM = 0;
	}	

	FLASH_PWM = 0;

	ENABLE_CHARGE();					// 打开充电
	
}

void FlashEventLoop(void)
{
	if (1 == g_flashEnable)
	{
		ReadyForFlash();

		if (g_chargeOk > 0)
		{
			TURN_ON_TRIGER(g_flashTime);
		}
		g_flashEnable = 0;

#ifdef FLASH_SYN_DBG
		printf("ttl: %x, %x, %x, %d\r\n",g_expTTLPara1,g_expTTLPara2,ttl_expCodeRsv,g_flashTime);
#endif
		printf("pra %d: %d %d\r\n",ttl_pre_dbg,g_nikPreTmr1, g_nikPreTmr2);
		printf("ad: %d, vol: %d,  iso: %d\r\n", g_volRef, AdToVol(g_volRef), (cmrPara.pIso -0x18)/2);
		printf("ttl: %x, %x, %x, %d\r\n",g_expTTLPara1,g_expTTLPara2,ttl_expCodeRsv,g_flashTime);
		//printf("iso: %d, piso: %x\r\n",cmrPara.iso, cmrPara.pIso);
		//printf("ttl: %x, %x, %x, %d\r\n",g_expTTLPara1,g_expTTLPara2,ttl_expCodeRsv,g_flashTime);
		g_nikPreTmr1 = 0;
		g_nikPreTmr2 = 0;
/*
		if (CMR_LINK_CAN == g_cmrLinked)
		{
			FlashFinishProc();
			SetSpiTrans();                                                                      // do not change the statue after flash 2017/08/15
		}else if(CMR_LINK_NIK == g_cmrLinked)
		{
			//SetSBOInt();                                                                       // do not change the statue after flash 2017/08/15
			g_nikComStatue = COM_STATUE_IDLE;
			g_nikSuspend = 1;
			//g_comStatue = COM_STATUE_IDLE;
		}
*/

		if (CMD_MODE_RF == *g_cmdMode)
		{
			//A7105_Write_Ram(0x06,g_a7105_id2,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
			A7105_Write_Reg(0x03,0x0B);
			g_sRcvState = 0;
		}else if(CMD_MODE_OFF == *g_cmdMode)
		{
			if (CMR_LINK_CAN == g_cmrLinked)
			{
				FlashFinishProc();
				SetSpiTrans();                                                                      // do not change the statue after flash 2017/08/15
			}else if(CMR_LINK_NIK == g_cmrLinked)
			{
				//SetSBOInt();                                                                       // do not change the statue after flash 2017/08/15
				g_nikComStatue = COM_STATUE_IDLE;
				g_nikSuspend = 1;
				//g_comStatue = COM_STATUE_IDLE;
			}
		}else if (CMD_MODE_LIGH_F1 == *g_cmdMode)
		{
			g_slaveFlashCounter = 10;      // 延时10ms			
		}else if(CMD_MODE_LIGH_F2==*g_cmdMode)
		{
			g_slaveFlashCounter = 10;      // 延时10ms	
		}

		ttl_pre_dbg = 0;

		//if (CFN_ON == g_cfnOption[CFN_BEEP])
		/*
		if (g_cfnOption[CFN_BEEP] > 0)
		{
			BeeperOn(8,1);
		}*/

		g_sysSleepCnt = 0;
		
	}else if (2 == g_flashEnable)          // preflash
	{
		if (CMR_LINK_CAN == g_cmrLinked)
		{
			//g_flashTime = OUTPUT_10;
                      //g_flashTime = g_expTime[VolLevel(g_volRef)][(2+2)*3+1];      //  1/64
                      //g_flashTime = g_expTime[VolLevel(g_volRef)][CAN_PRE_LEV]; 
                      //g_flashTime = g_preTimeSet[VolLevel(g_volRef)];
			g_getTtlFlashCode = 1;
			
			SET_SPIO_OFF( );
			SPI_MISO_AUX(0);	
			if (g_chargeOk > 0)
			{
				//TURN_ON_TRIGER(g_flashTime);
				CanPreFlash();
			}else
			{
				delay_us(100);
			}
			
			//delay_us(10);
			SPI_MISO_AUX(1);
			SetSpiTrans();
		}else if (CMR_LINK_NIK == g_cmrLinked)
		{
			
			if (ttl_pre2_flag > 0)
			{
				//g_flashTime = OUTPUT_20;
                             //g_flashTime = g_expTime[VolLevel(g_volRef)][(2+2)*3+0];
                             g_flashTime = g_expTime[VolLevel(g_volRef)][NIK_PRE2_LEV];
				g_nikPreTmr2 = g_flashTime;
				ttl_pre_dbg = 2;
			}else
			{
				//g_flashTime = OUTPUT_10;
                             //g_flashTime = g_expTime[VolLevel(g_volRef)][(1+2)*3+2];
                             g_flashTime = g_expTime[VolLevel(g_volRef)][NIK_PRE1_LEV];
				g_nikPreTmr1 = g_flashTime;
				ttl_pre_dbg = 1;
			}
#if (BOARD_VER == __BOARD_20_)
			NIK_ODR_ON();
#endif
			PULL_MOSI_LOW();
			delay_us(400);
			SET_SCK_LOW();
			if (g_chargeOk > 0)
			{
				TURN_ON_TRIGER(g_flashTime);
			}
			SET_SCK_HIGH();
			delay_us(400);
			PULL_MOSI_HIGH();
#if (BOARD_VER == __BOARD_20_)
			NIK_ODR_OFF();
#endif

			ClearComRstTime();              // this should be set, otherwise it will cause problem

#ifdef _A7105_DBG_
			printf("pre\r\n");
#endif

			/*
			SetSBOInt();
			*/

			

			g_nikComStatue = COM_STATUE_IDLE;
			
			
		}else
		{
			//g_flashTime = OUTPUT_10;
                        g_flashTime = g_expTime[VolLevel(g_volRef)][(1+2)*3+0];
			if (g_chargeOk > 0)
			{
				TURN_ON_TRIGER(g_flashTime);
			}
		}
		g_flashEnable = 0;
	}else if ( 3 == g_flashEnable )
	{
		if (g_chargeOk > 0)
		{
			TURN_ON_TRIGER(g_flashTime);
		}
		g_flashEnable = 0;
	}else if ( 4 == g_flashEnable )
	{
		NikPreFlash();
		g_flashEnable = 0;
	}

}

void MultiFlashEventLoop(void)
{
	u16 crtTime; 
	//if((motoSet.moto_en!=MOTO_ENABLE)||(g_moto_error!=0))
	//if((MOTO_ENABLE!=motoSet.moto_en)||g_moto_error)
	//{
	//	return;
	//}
        if (g_multiFlashEnable > 0)
        {
        	crtTime = get_counter();              // get time
	        if (crtTime - g_multiTimer > 1000L)              // key timer overflow
	        {
			g_multiTimer = crtTime;

			g_multiCnt ++;

#ifdef _RF_MULTI_DBG_
			printf("tms: %d\r\n",multiSet.finalExpTimes);
#endif

			if (g_multiCnt >= multiSet.lowFrqCnt)
			{
#ifdef _RF_MULTI_DBG_
				printf("mel: %d\r\n",multiSet.finalExpTimes);
#endif

				g_multiCnt = 0;
				if (multiSet.finalExpTimes > 0)
				{
					g_flashTime = OutputSwitch(mstPara->multi_exp>>4, mstPara->multi_exp&0x0f);
					TURN_ON_TRIGER(g_flashTime);
					multiSet.finalExpTimes --;
#ifdef _RF_MULTI_DBG_
					printf("fla: %d\r\n",multiSet.finalExpTimes);
#endif	
				}

				if (!multiSet.finalExpTimes)
				{
					g_multiCnt = 0;
					g_multiFlashEnable = 0;
				}
/*处理佳能和尼康的频闪此数20171215*/
				if (CMD_MODE_OFF == *g_cmdMode)
				{
					if (CMR_LINK_CAN == g_cmrLinked)
					{
						if ((FLASH_IN>0)&&(PILOT_PIN))
						{
							g_multiCnt = 0;
							g_multiFlashEnable = 0;
						}
					}
				}
			}

			

		}
        }
}

void FLASHIntProc(void)
{/*
	switch(g_comStatue)
	{
		case COM_STATUE_IDLE:
			g_flashTime = OutputSwitch(mstPara->output1,mstPara->output2);
			g_flashEnable = 1;
		break;

		//case COM_STATUE_PREFLASH:
		case COM_STATUE_REALFLASH:
			g_flashEnable = 1;
			if (CMR_LINK_CAN == g_cmrLinked)
			{
				if(!SCK_IN)                                             // canon
				{
					g_flashEnable = 1;
				}
			}else if(CMR_LINK_NIK == g_cmrLinked)
			{
				g_flashEnable = 1;
			}
		break;
	}*/
	//RF_LED_GREEN ^= 1;
#ifdef NIK_COM_DBG
	printf("fi %d,%d\r\n",g_canComStatue,g_nikComStatue);
#endif
	
	if ((COM_STATUE_IDLE == g_canComStatue) && (COM_STATUE_IDLE == g_nikComStatue))
	{
		g_flashTime = OutputSwitch(mstPara->output1,mstPara->output2);
		g_flashEnable = 1;
	}else if(COM_STATUE_REALFLASH == g_canComStatue)
	{
		if(!SCK_IN)                                             // canon
		{
			g_flashEnable = 1;
		}
	}else if (COM_STATUE_REALFLASH == g_nikComStatue)
	{
		g_flashEnable = 1;
	}
}

void SetMultiFlashTimer(void)
{
	
	if(multiSet.finalExpTimes>0)
	{
		//g_tmHandleFlag = TM_HANDLE_MULTIFLASH;	
		//Timer4_Init(multiSet.lowFrqCnt, 7200);
		//Timer4_Init(100,72);
		g_multiFlashEnable = 1;
	}else
	{
		g_multiFlashEnable = 0;
	}
	g_multiTimer = get_counter();

#ifdef _RF_MULTI_DBG_
	printf("multi: t %d, c %d\r\n",multiSet.finalExpTimes,multiSet.lowFrqCnt);
#endif
}

int MultiZero(unsigned char mExp)
{
	int ret;
	switch(mExp)
	{
		case 0:
		case 1:
			ret = 40;
		break;

		case 2:
			ret = 20;
		break;

		case 3:
			ret = 12;
		break;

		case 4:
			ret = 8;
		break;

		case 5:
			ret = 4;
		break;

		case 6:
			ret = 2;
		break;

		default:
			ret = 1;
		break;
	}

	return ret;

}

void CalcRptFlashTimes(void)
{
	u16 tmp;

	if (CMD_MODE_OFF == *g_cmdMode)
	{
		if(!*multiSet.eTimes)
		{
			tmp = MultiZero((*multiSet.expose)>>4);
		}else
		{
			tmp = *multiSet.eTimes;
		}
		if((1000/(*multiSet.frequence)+1)>(cmrPara.shutter/tmp))
		{
			multiSet.finalExpTimes= cmrPara.shutter*(*multiSet.frequence)/1000;
		}else
		{
			multiSet.finalExpTimes = tmp;
		}
	}else
	{
		multiSet.finalExpTimes = *multiSet.eTimes;
	}
}


void MultiFlashPrepear(void)
{
	if (CMR_LINK_CAN == g_cmrLinked)
	{
		if(!*multiSet.eTimes)
		{
	//#ifdef _CANON_
			//multiSet.finalExpTimes = MultiZero(*multiSet.expose);
	//#else
			multiSet.finalExpTimes = MultiZero((*multiSet.expose)>>4);
	//#endif
		}else
		{
			multiSet.finalExpTimes = *multiSet.eTimes;
		}

		multiSet.finalExpTimes--;

		//multiSet.lowFrqCnt = (10000L/(*multiSet.frequence));
		//multiSet.lowFrqCnt = (1000L/(*multiSet.frequence));
	}else
	{
		CalcRptFlashTimes();
		if (multiSet.finalExpTimes > 0)
		{
			multiSet.finalExpTimes--;
		}
	}

	multiSet.lowFrqCnt = (1000L/(*multiSet.frequence));
					
	//multiSet.mulCtl=MULTI_FLASH_ENABLE;
	//multiSet.finalExpTimes--;
}

void NikFlashCmp(s8 *c1, s8 *c2)
{
	s8 tmp1, tmp2;
	tmp1 = mstPara->cmp1+cmrPara.camera_cmp1;
	tmp2 = mstPara->cmp2+cmrPara.camera_cmp2;
	if(tmp2>2)
	{
		tmp1 ++;
		tmp2 -= 3;
	}else if(tmp2<-2)
	{
		tmp1--;
		tmp2 += 3;
	}
	*c1 = tmp1;
	*c2 = tmp2;
}

void ReadyForFlash(void)
{
	s8 cmp1, cmp2;
	
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
		//case CMD_MODE_RF:
			switch (*g_lightMode)
			{
				case WORKINGMODE_TTL:
					if (CMR_LINK_CAN == g_cmrLinked)//if (RF_MODE_CAN == *g_rfMode)
					{
						CanExposeTimeCal(ttl_expCodeRsv,0/*,mstPara->cmp1,mstPara->cmp2*/);
						g_getTtlFlashCode = 0;
					}else if (CMR_LINK_NIK == g_cmrLinked)
					{
						NikFlashCmp(&cmp1, &cmp2);
						NikExposeTimeCal(ttl_expCodeRsv,0,cmp1,cmp2);
					}else
					{
						CanExposeTimeCal(ttl_expCodeRsv,0/*,mstPara->cmp1,mstPara->cmp2*/);
					}
					g_flashTime = OutputSwitch(g_expTTLPara1,g_expTTLPara2);
				break;

				case WORKINGMODE_MANU:
					g_flashTime = OutputSwitch(mstPara->output1, mstPara->output2);
				break;

				case WORKINGMODE_MULTI:
#ifdef _A7105_DBG_
					printf("multi\r\n");
#endif
					MultiFlashPrepear();
					g_flashTime = OutputSwitch(mstPara->multi_exp>>4, mstPara->multi_exp&0x0f);
					SetMultiFlashTimer();					
				break;
			}
		break;

		case CMD_MODE_RF:
			switch (*g_lightMode)
			{
				case WORKINGMODE_TTL:
					if (RF_MODE_CAN == *g_rfMode)
					{
						CanExposeTimeCal(ttl_expCodeRsv,0/*,mstPara->cmp1,mstPara->cmp2*/);
						g_getTtlFlashCode = 0;
					}else if (RF_MODE_NIK == *g_rfMode)
					{
						//NikFlashCmp(&cmp1, &cmp2);
						// 2017/11/14 尼康无线模式亮度降低
						//CanExposeTimeCal(ttl_expCodeRsv-(3*(7+4)),0/*,mstPara->cmp1,mstPara->cmp2*/);
						// @20171115 修改尼康无线预闪
						if (1 == ttl_pre_dbg)
						{
							CanExposeTimeCal(ttl_expCodeRsv+7,0/*,mstPara->cmp1,mstPara->cmp2*/);
						}else if (2 == ttl_pre_dbg )
						{
							CanExposeTimeCal(ttl_expCodeRsv+7,0/*,mstPara->cmp1,mstPara->cmp2*/);
						}
						//CanExposeTimeCal(ttl_expCodeRsv-9,0/*,mstPara->cmp1,mstPara->cmp2*/);
						//NikExposeTimeCal(ttl_expCodeRsv,0,cmp1,cmp2);
						//ttl_pre_dbg = ttl_pre2_flag;
						ttl_pre2_flag = 0;
						g_getTtlFlashCode = 0;
					}else
					{
						CanExposeTimeCal(ttl_expCodeRsv,0/*,mstPara->cmp1,mstPara->cmp2*/);
						g_getTtlFlashCode = 0;
					}
					g_flashTime = OutputSwitch(g_expTTLPara1,g_expTTLPara2);
				break;

				case WORKINGMODE_MANU:
					g_flashTime = OutputSwitch(mstPara->output1, mstPara->output2);
				break;

				case WORKINGMODE_MULTI:
#ifdef _A7105_DBG_
					printf("multi\r\n");
#endif
					MultiFlashPrepear();
					g_flashTime = OutputSwitch(mstPara->multi_exp>>4, mstPara->multi_exp&0x0f);
					SetMultiFlashTimer();					
				break;
			}
		break;

		case CMD_MODE_LIGH_F1:
		break;

		case CMD_MODE_LIGH_F2:
		break;
	}
}

#define TTL_CALC_MIN      0x68//0x78//0x70//0x68//0x70//0X78
//#define TTL_CALC_MAX     ((7+2)*3*3+TTL_CALC_MIN)
#define TTL_CALC_MAX     ((7+3)*3*3+TTL_CALC_MIN)
#define TTL_HPCALC_MIN   0X70
#define TTL_HPCALC_MAX   (7*3*3+TTL_HPCALC_MIN)

void CanExpCalSub(const u8 i_code)
{
	if (i_code < 110)
	{
		g_expTTLPara1 = -3;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 113)
	{
		g_expTTLPara1 = -3;
		g_expTTLPara2 = 1;
	}else if (i_code < 116)
	{
		g_expTTLPara1 = -3;
		g_expTTLPara2 = 2;
	}else if (i_code < 119)
	{
		g_expTTLPara1 = -2;
		g_expTTLPara2 = 0;
	}else if (i_code < 122)
	{
		g_expTTLPara1 = -2;
		g_expTTLPara2 = 1;
	}else if (i_code < 125)
	{
		g_expTTLPara1 = -2;
		g_expTTLPara2 = 2;
	}else if (i_code < 128)
	{
		g_expTTLPara1 = -1;
		g_expTTLPara2 = 0;
	}else if (i_code < 131)
	{
		g_expTTLPara1 = -1;
		g_expTTLPara2 = 1;
	}else if (i_code < 134)
	{
		g_expTTLPara1 = -1;
		g_expTTLPara2 = 2;
	}else if (i_code < 137)
	{
		g_expTTLPara1 = 0;
		g_expTTLPara2 = 0;
	}else if (i_code < 140)
	{
		g_expTTLPara1 = 0;
		g_expTTLPara2 = 1;
	}else if (i_code < 143)
	{
		g_expTTLPara1 = 0;
		g_expTTLPara2 = 2;
	}else if (i_code < 145)
	{
		g_expTTLPara1 = 1;
		g_expTTLPara2 = 0;
	}else if (i_code < 147)
	{
		g_expTTLPara1 = 1;
		g_expTTLPara2 = 1;
	}else if (i_code < 149)
	{
		g_expTTLPara1 = 1;
		g_expTTLPara2 = 2;
	}else if (i_code < 152)
	{
		g_expTTLPara1 = 2;
		g_expTTLPara2 = 0;
	}else if (i_code < 155)
	{
		g_expTTLPara1 = 2;
		g_expTTLPara2 = 1;
	}else if (i_code < 158)
	{
		g_expTTLPara1 = 2;
		g_expTTLPara2 = 2;
	}else if (i_code < 161)
	{
		g_expTTLPara1 = 3;
		g_expTTLPara2 = 0;
	}else if (i_code < 163)
	{
		g_expTTLPara1 = 3;
		g_expTTLPara2 = 1;
	}else if (i_code < 165)
	{
		g_expTTLPara1 = 3;
		g_expTTLPara2 = 2;
	}else if (i_code < 168)
	{
		g_expTTLPara1 = 4;
		g_expTTLPara2 = 0;
	}else if (i_code < 170)
	{
		g_expTTLPara1 = 4;
		g_expTTLPara2 = 1;
	}else if (i_code < 173)
	{
		g_expTTLPara1 = 4;
		g_expTTLPara2 = 2;
	}else if (i_code < 176)
	{
		g_expTTLPara1 = 5;
		g_expTTLPara2 = 0;
	}else if (i_code < 179)
	{
		g_expTTLPara1 = 5;
		g_expTTLPara2 = 1;
	}else if (i_code < 182)
	{
		g_expTTLPara1 = 5;
		g_expTTLPara2 = 2;
	}else if (i_code < 185)
	{
		g_expTTLPara1 = 6;
		g_expTTLPara2 = 0;
	}else if (i_code < 188)
	{
		g_expTTLPara1 = 6;
		g_expTTLPara2 = 1;
	}else if (i_code < 191)
	{
		g_expTTLPara1 = 6;
		g_expTTLPara2 = 2;
	}else
	{
		g_expTTLPara1 = 7;
		g_expTTLPara2 = 0;
	}

	if ((g_expTTLPara1 >= 0) && (g_expTTLPara1< 7))
	{
		g_expTTLPara2+=1;
		//g_expTTLPara2+=2;
		if (g_expTTLPara2 > 2)
		{
			g_expTTLPara1 ++;
			g_expTTLPara2 -= 3;
		}
	}

	
}


void CanExposeTimeCal(const u8 i_camCode,u8 fpMod)
{
	u8 camCode=0;
	u8 o_camCode;
//	u8 tmp1;
//	u8 tmp2;

	//tmp1 = menuPara.cApert>>4;
	//tmp2 = menuPara.cApert&0x0F;	
	//flaPara.apert = CannonApertVal[tmp1*6+(tmp2+2)/3];
	//o_camCode = i_camCode + (3*3*2);     // fine tune
	//o_camCode = i_camCode + 3*2;
	o_camCode = i_camCode + 3*3;                // 20171026

	if(LOW_SPEED==fpMod)
	{
		//camCode = o_camCode-30;
		//camCode = o_camCode;
		CanExpCalSub(o_camCode);
#if 0
		//if(o_camCode<=0x60)
		//if(o_camCode<=0x70)
		if(o_camCode<TTL_CALC_MIN)
		{
			//g_expTTLPara1 = -2;
			//g_expTTLPara2 = 0;
			g_expTTLPara1 = -3;                          // 20171026
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
			//g_expTTLPara1 = (-2)+camCode/3;
			g_expTTLPara1 = (-3)+camCode/3;
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
#endif

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
	}

}

#define TTL_VAL_MIN 45//0x32//0x26//0x23//0x1d//0x32//0x54
#define TTL_VAL_MIN_HP 0x52//0x5A//0x57//0x66//0x4E//0x60
#define VAL_STEP 4//6(When 0.5Ev step)
#define TTL_VAL_MAX (TTL_VAL_MIN+(9*3*VAL_STEP))
#define TTL_VAL_MAX_HP (TTL_VAL_MIN_HP+(7*3*VAL_STEP))

void NikExpCalSub(const u8 i_code)
{

	/*if (i_code < TTL_VAL_MIN)
	{
		g_expTTLPara1 = -3;                          // 20171026
		g_expTTLPara2 = 0;
	}
	else if (i_code < (TTL_VAL_MIN+3))
	{
		g_expTTLPara1 = -3;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < (TTL_VAL_MIN+6))
	{
		g_expTTLPara1 = -3;                          // 20171026
		g_expTTLPara2 = 2;
	}else*/ if(i_code < 45)
	{
		g_expTTLPara1 = -2;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 47)
	{
		g_expTTLPara1 = -2;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 49)
	{
		g_expTTLPara1 = -2;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 50)
	{
		g_expTTLPara1 = -1;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 53)
	{
		g_expTTLPara1 = -1;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 56)
	{
		g_expTTLPara1 = -1;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 59)
	{
		g_expTTLPara1 = 0;                          // 20171026
		g_expTTLPara2 = 0;
	}
	else if (i_code < 64)
	{
		g_expTTLPara1 = 0;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 69)
	{
		g_expTTLPara1 = 0;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 74)
	{
		g_expTTLPara1 = 1;                          // 20171026
		g_expTTLPara2 = 0;
	}
	else if (i_code < 78)
	{
		g_expTTLPara1 = 1;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 82)
	{
		g_expTTLPara1 = 1;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 86)
	{
		g_expTTLPara1 = 2;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 89)
	{
		g_expTTLPara1 = 2;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 92)
	{
		g_expTTLPara1 = 2;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 95)
	{
		g_expTTLPara1 = 3;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 99)
	{
		g_expTTLPara1 = 3;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 103)
	{
		g_expTTLPara1 = 3;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 107)
	{
		g_expTTLPara1 = 4;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 110)
	{
		g_expTTLPara1 = 4;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 113)
	{
		g_expTTLPara1 = 4;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 116)
	{
		g_expTTLPara1 = 5;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 120)
	{
		g_expTTLPara1 = 5;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 124)
	{
		g_expTTLPara1 = 5;                          // 20171026
		g_expTTLPara2 = 2;
	}else if (i_code < 128)
	{
		g_expTTLPara1 = 6;                          // 20171026
		g_expTTLPara2 = 0;
	}else if (i_code < 132)
	{
		g_expTTLPara1 = 6;                          // 20171026
		g_expTTLPara2 = 1;
	}else if (i_code < 136)
	{
		g_expTTLPara1 = 6;                          // 20171026
		g_expTTLPara2 = 2;
	}else
	{
		g_expTTLPara1 = 7;                          // 20171026
		g_expTTLPara2 = 0;
	}
	
}

void NikExposeTimeCal(const unsigned char i_camCode,unsigned char hp,s8 cmp1, s8 cmp2)
{
	u16 camCode;
//	signed char tmp = 0;
//	u8 i = 0;
//	s8 cmpTmp = 0;
	u8 isoIndex = 0;
	//camCode = i_camCode - 8;
	//camCode = i_camCode + 12;
	//camCode = i_camCode;                         // 2017/11/9
	//camCode = i_camCode - VAL_STEP;                         // 2017/11/9
	camCode = i_camCode+6+(3*VAL_STEP); 

	if(ttl_pre2_flag > 0)
	{
		//camCode += (6*VAL_STEP);
		camCode += (3*VAL_STEP);            /*20171212*/
	}

	//cmrPara.isoIndex = GetParaStdIndex(isoValue, 35, cmrPara.iso);
/*
	if(cmrPara.iso<50)
	{
		isoIndex = 0;
	}else
	{
		isoIndex = (GetIsoStdIndex(isoValue, 58, cmrPara.iso) - 12);          // sub iso50 index
	}
*/

	if (cmrPara.pIso > 0x18)
	{
		isoIndex = (cmrPara.pIso -0x18)/2;
	}else
	{
		isoIndex = 0;
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
	
	//g_isFlashWeak = 0;
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

		//NikExpCalSub(camCode);

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

		if (camCode > (isoIndex*VAL_STEP))
		{
			camCode -= (isoIndex*VAL_STEP);
		}else
		{
			camCode = 0;
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
		//g_isFlashWeak = 1;
		//g_FlashWeakLevels = (g_expTTLPara1*3)+g_expTTLPara2-21;
		//g_expTTLPara1 = 7;
		g_expTTLPara2 = 0;
	}

}

#pragma vector= EXTI1_vector
__interrupt void EXTI1_IRQHandler(void)
{
#ifdef SLEEP_DBG
	//printf("dial\r\n");
#endif

	if (EXTI_SR1 & 0x02)
	{
		EXTI_SR1 |= 0x02;                   // Light slave mode
		if (LIGHT_SLAVE_IRQ)
		{
			return;
		}

		// add at 20180708
		if (g_slaveFlashCounter > 0)
		{
			return;
		}
		
		if (CMD_MODE_LIGH_F1 == *g_cmdMode)
		{
			g_flashTime = OutputSwitch(mstPara->output1,mstPara->output2);
			g_flashEnable = 1;			
		}else if (CMD_MODE_LIGH_F2 == *g_cmdMode)
		{
			if (g_s2Flag > 0)
			{
				if (g_s2Timer > 50)                        // delay 50 ms, then follow flash
				{
					g_flashTime = OutputSwitch(mstPara->output1,mstPara->output2);
					g_flashEnable = 1;	
					g_s2Flag = 0;
                                        g_s2Timer = 0;
				}
			}else
			{
				g_s2Flag = 1;
			}
		}
	}
}

#pragma vector= EXTI0_vector
__interrupt void EXTI0_IRQHandler(void)
{
#ifdef SLEEP_DBG
	//printf("pilot int\r\n");
#endif
	if (EXTI_SR1 & 0x01)
	{
		EXTI_SR1 |= 0x01; 
		//SleepPortResume(g_sleepRegDdr,g_sleepRegOdr,g_sleepRegCr1,g_sleepRegCr2);
		// recover the port setting
		if (CMD_MODE_RF == *g_cmdMode)
		{
			if (!A7105_IRQ)
			{
				
				RfHandleProc();
				//g_rfRcvSetTag = 1;
				// Reset A7105 receive
				A7105_strobe_cmd(0xF0);
				A7105_strobe_cmd(0xC0);
			}	
		}
	}
}

#if (BOARD_VER == __BOARD_20_)
#pragma vector= EXTI3_vector
__interrupt void EXTI3_IRQHandler(void)
{
	if (EXTI_SR1 & 0x08)
	{
		EXTI_SR1 |= 0x08;
		if (!FLASH_IN)
		{	
			//TEST_PIN ^= 1;
			FLASHIntProc();
		}
	}
}
#endif



