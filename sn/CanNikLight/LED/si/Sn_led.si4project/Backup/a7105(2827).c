#include "a7105.h"
#include"global.h"
#include "..\uart\uart.h"
#include "..\sys\sys.h"

//const u8 g_a7105_id1[4] = {0x35,0x99,0x9a,0x5a};
//u8 g_a7105_id1[4] = {0x35,0x99,0x9a,0x5a};
const u8 g_a7105_id1[4] = {0xA4,0x11,0x66,0xf4};
const u8 g_a7105_id2[4] = {0xC2,0x4D,0xDE,0xE4};
const u8 g_a7105_id3[4] = {0xCD,0xB3,0xAD,0x9B};            // PreFLASH

extern u8 g_rfTtl;
u8 g_rfStatue = 0;
u8 g_rfLinkTag = 0;
u16 g_rfLinkTmr = 0;
u8 g_rfRcvSetTag = 0;
extern u8 ttl_pre_dbg;

void RfTest(void);

void Spi_Send_byte(u8 dat)
{
	u8 i = 0;
	//A7105_CS = 0;
	for (i=0;i<8;i++)
	{	
		if (dat & 0x80)
		{
			A7105_ODR = 1;
		}else
		{
			A7105_ODR = 0;
		}
		dat <<= 1;
		A7105_SCK = 1;
		A7105_SCK = 0;
	}
	//A7105_CS = 1;
}

u8 Spi_Rcv_byte(void)
{
	u8 i = 0;
	u8 rcvDat = 0;
	for (i=8;i>0;i--)
	{
		/*if (A7105_IDR)
		{
			rcvDat |= (1 << i);
		}*/
		rcvDat |= (A7105_IDR << (i-1));
		A7105_SCK = 1;
		A7105_SCK = 0;
	}
	return rcvDat;
}

void A7105_strobe_cmd(u8 cmd)
{
	A7105_SCK = 0;
	A7105_CS = 0;
	Spi_Send_byte(cmd);
	A7105_CS = 1;
}

void A7105_Write_Reg(u8 addr, u8 val)
{
	A7105_SCK = 0;
	A7105_CS = 0;
	Spi_Send_byte(addr);
	Spi_Send_byte(val);
	A7105_CS = 1;
}

u8 A7105_Read_Reg(u8 addr)
{
	 u8 val;
	 A7105_SCK = 0;
	A7105_CS = 0;
	//PD_CR2_C23 = 0;
	A7105_DDR = 0;
	Spi_Send_byte(addr|0x40);
	val = Spi_Rcv_byte();
	A7105_CS = 1;
	return val;
}

void A7105_Write_Ram(u8 addr, const u8 * ram, u8 len)
{
	u8 i = 0;
	A7105_SCK = 0;
	A7105_CS = 0;
	Spi_Send_byte(addr);
	for (i=0;i<len;i++)
	{
		Spi_Send_byte(ram[i]);
	}
	A7105_CS = 1;
}

void A7105_Read_Ram(u8 addr, u8 * ram, u8 len)
{
	u8 i = 0;
	//PD_CR2_C23 = 0;
	A7105_DDR = 0;
	A7105_SCK = 0;
	A7105_CS = 0;
	Spi_Send_byte(0x40|addr);
	for (i=0;i<len;i++)
	{
		ram[i] = Spi_Rcv_byte();
	}
	A7105_CS = 1;
}

void A7105_Probe(void)
{
	

}

u8 memcmp(u8* src, u8 *dest, u8 len)
{
	u8 i = 0;
	for (i=0;i<len;i++)
	{
		if (src[i]!=dest[i])
		{
			return (i+1);
		}
	}

	return 0;
}

void A7105_Init(void)
{
	//u8 rdBuf[32];
	u8 idBuf[4];
	u8 tm = 0;
	//u8 i;

	A7105_RX_AF = 1;

	g_sRcvCnt = 12;

	A7105_Write_Reg(0x00, 0x00);
	A7105_Write_Reg(0x01, 0x62);
	A7105_Write_Reg(0x02, 0x00);
	A7105_Write_Reg(0x03, 0x0b);
	A7105_Write_Reg(0x04, 0x00);
	A7105_Write_Reg(0x07, 0x00);
	A7105_Write_Reg(0x08, 0x00);
	A7105_Write_Reg(0x09, 0x00);
	A7105_Write_Reg(0x0a, 0x00);
	A7105_Write_Reg(0x0b, 0x01);
	A7105_Write_Reg(0x0c, 0x01);
	A7105_Write_Reg(0x0d, 0x05);
	A7105_Write_Reg(0x0e, 0x00);
	A7105_Write_Reg(0x0f, 0x50);
	A7105_Write_Reg(0x10, 0x9e);
	A7105_Write_Reg(0x11, 0x4b);
	A7105_Write_Reg(0x12, 0x00);
	A7105_Write_Reg(0x13, 0x02);
	A7105_Write_Reg(0x14, 0x16);

	A7105_Write_Reg(0x15, 0x2b);
	A7105_Write_Reg(0x16, 0x12);
	A7105_Write_Reg(0x17, 0x00);
	A7105_Write_Reg(0x18, 0x62);
	A7105_Write_Reg(0x19, 0x80);
	A7105_Write_Reg(0x1a, 0x80);
	A7105_Write_Reg(0x1b, 0x00);
	A7105_Write_Reg(0x1c, 0x0a);
	A7105_Write_Reg(0x1d, 0x32);
	A7105_Write_Reg(0x1e, 0xc3);
	A7105_Write_Reg(0x1f, 0x05);
	A7105_Write_Reg(0x20, 0x16);
	A7105_Write_Reg(0x21, 0x00);
	A7105_Write_Reg(0x22, 0x00);
	A7105_Write_Reg(0x24, 0x00);
	A7105_Write_Reg(0x25, 0x00);
	A7105_Write_Reg(0x26, 0x3b);
	A7105_Write_Reg(0x27, 0x00);
	A7105_Write_Reg(0x28, 0x1f);

	A7105_Write_Reg(0x29, 0x47);
	A7105_Write_Reg(0x2a, 0x80);
	A7105_Write_Reg(0x2b, 0x03);
	A7105_Write_Reg(0x2c, 0x01);
	A7105_Write_Reg(0x2d, 0x45);
	A7105_Write_Reg(0x2e, 0x18);
	A7105_Write_Reg(0x2f, 0x00);
	A7105_Write_Reg(0x30, 0x01);
	A7105_Write_Reg(0x31, 0x0f);

	A7105_Write_Reg(0x0b, 0x19);

	A7105_strobe_cmd(0xa0);	

	A7105_Write_Reg(0x02,0x01);	

	for (tm=0;tm<10;tm++)
	{
		if (0 == (0x01 & A7105_Read_Reg(0x02)))
		{
			break;
		}else
		{
			delay_ms(10);
		}
	}

	A7105_Read_Reg(0x22);
	A7105_Write_Reg(0x24,0x13);
	A7105_Write_Reg(0x26,0x3b);
	A7105_Write_Reg(0x0f,0x00);
	A7105_Write_Reg(0x02,0x02);

#if 0
	while(0x02 & A7105_Read_Reg(0x02))
	{
		delay_ms(10);
	}
#else
	for (tm=0;tm<100;tm++)
	{
		if (0 == (0x02 & A7105_Read_Reg(0x02)))
		{
			break;
		}else
		{
			delay_ms(10);
		}
	}
#endif

	A7105_Read_Reg(0x25);
	A7105_Write_Reg(0x0f,0xa0);
	A7105_Write_Reg(0x02,0x02);
/*
	while(0x02 & A7105_Read_Reg(0x02))
	{
		delay_ms(10)
	}
*/
	for (tm=0;tm<10;tm++)
	{
		if (0 == (0x02 & A7105_Read_Reg(0x02)))
		{
			break;
		}else
		{
			delay_ms(10);
		}
	}
	
	A7105_Read_Reg(0x25);
	
	
	A7105_Write_Ram(0x06, g_a7105_id1,4);
	A7105_Read_Ram(0x06, idBuf,4);

	//A7105_strobe_cmd(0xC0);
	//A7105_Write_Reg(0x0f,0x1e);

	//A7105_Write_Ram(0x06, g_a7105_id2,4);
	//A7105_Write_Reg(0x03,0x0B);

	RfSetChan(mainPara.s_chan);

	A7105_strobe_cmd(0xF0);
	A7105_strobe_cmd(0xC0);

	//A7105_Write_Reg(0x0f,0x1e);
	//RfSetChan(mainPara.s_chan);
	//A7105_Write_Reg(0x0f,0x20);

	A7105_Read_Ram(0x06, idBuf,4);

#ifdef _A7105_DBG_
	if(!memcmp((u8*)g_a7105_id2,idBuf,4))
	{
		RF_LED_READY();// A7105 ready
		printf("a105 ok\r\n");
		//g_rfTtl = 1;
	}
#endif
	

}

void A7105Halt(void)
{
	A7105_Write_Reg(0x00, 0x00);
	A7105_strobe_cmd(0x80);
}

void CanParseRfData(u8 *rcv, u8 len)
{
	u8 mod = 0;
	//switch(rcv[0])
	switch (rcv[0] & 0x0f)
	{
		//case 0x7f:                      // mode information
		case 0x0f:
		if (g_sRcvState > 0)
		{
			return;
		}

		if (0xF6 == rcv[1])
		{
			RfTest();
			g_rfLinkTmr = 100;
			RF_LED_LINK();
			return;
		}

		
		if (rcv[2]&~0x17)
		{
			return;
		}

		mod = rcv[2]&0x0f;
		
		//if (*g_lightMode != (rcv[2]&0x07))
		if (mod != *g_lightMode)
		{
			if ((mod != WORKINGMODE_TTL) && (mod != WORKINGMODE_MANU) && (mod != WORKINGMODE_MULTI))
			{
				return;
			}
			*g_lightMode = mod;
			//*g_lightMode = rcv[2]&0x07;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
			printf("mode\r\n");
#endif
		}

		if (mainPara.synMode != ((rcv[2]>>3)&0x03))
		{
			mainPara.synMode = (rcv[2]>>3)&0x03;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
			printf("syn\r\n");
#endif
		}
		
		if (*multiSet.frequence != rcv[4])
		{
			*multiSet.frequence = rcv[4];
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
			printf("hz\r\n");
#endif
		}
		if (*multiSet.eTimes != (rcv[5] & 0x7f))
		{
			*multiSet.eTimes = rcv[5] & 0x7f;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
			printf("tms\r\n");
#endif
		}

		if (WORKINGMODE_MANU == *g_lightMode)
		{
			if (menuPara.otpCod[MASTER] != rcv[8+mainPara.s_array])
			{
				menuPara.otpCod[MASTER] = rcv[8+mainPara.s_array];
				mstPara->output1 =EXPH_COV(menuPara.otpCod[MASTER]&0x7F);
				mstPara->output2 = EXPL_COV(menuPara.otpCod[MASTER]&0x7F);
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
				printf("otp\r\n");
#endif
			}
		}else if (WORKINGMODE_MULTI == *g_lightMode)
		{
			if (menuPara.WlmMul[MASTER]!=rcv[8+mainPara.s_array])
			{
				menuPara.WlmMul[MASTER] = rcv[8+mainPara.s_array];
				mstPara->multi_exp = EXPH_COV(menuPara.WlmMul[MASTER]&0x7F)<<4;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}

		if (menuPara.motoCod != rcv[6])
		{
			if ((!motoSet.moving)&&(motoSet.moto_ready))
			{
				menuPara.motoCod = rcv[6];
				// like 580ex, it have no 20mm
				if (menuPara.motoCod > 0x10)
				{
					motoSet.destRsv = focLenScape[menuPara.motoCod-0x10];
					*motoSet.mode = MOTO_MODE_MANU;
				}else
				{
					*motoSet.mode = MOTO_MODE_AUTO;
					motoSet.autoRsv = 0;                               // 强制刷新一次
					SET_UPDATE_EVT(DSP_UPDATE_MOTO);
				}
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
#ifdef _A7105_DBG_
			printf("moto\r\n");
#endif
		}		
		g_rfLinkTmr = 100;
		RF_LED_LINK();
		//RF_LED_LINK();
		//g_rfLinkTmr = 20;
		//SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		//case 0x77:
		case 0x07:
		if (g_sRcvState > 0)
		{
			return;
		}/*canon moto*/
		if (rcv[1] != menuPara.motoLoc)
		{
			if ((!motoSet.moving)&&(motoSet.moto_ready))
			{
				menuPara.motoLoc = rcv[1];
				if (MOTO_MODE_AUTO == *motoSet.mode)
				{
					SET_UPDATE_EVT(DSP_UPDATE_MOTO);
				}
			}
		}
		break;

		//case 0x79:
		case 0x09:                                               // 预闪
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id3);
			A7105_Write_Reg(0x03,0x01);
			g_sRcvCnt = 2;
			if ((rcv[0] >> 4) & (1 << mainPara.s_array))
			{
				g_sFlashEn = 1;
			}else
			{
				g_sFlashEn = 0;
			}
			g_sRcvState = 40;
			//g_rfStatue = RF_STATUES_PRE_FLASH;
		break;

		case 0x0A:                                            // 正闪
			//flaPara.s_ttlCode = rcv[8+mainPara.s_array];
			ttl_expCodeRsv = rcv[8+mainPara.s_array];
			flaPara.s_flashEn = ((rcv[0]>>4)&(1<<mainPara.s_array));
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id3);
			A7105_Write_Reg(0x03,0x01);
			g_sRcvCnt = 2;
			g_sRcvState = 40;
		break;

		case 0x01:
			if(0x01 == rcv[1])
			{
				if (g_sFlashTig > 0)
				{
					if (flaPara.s_flashEn > 0)
					{
						g_flashEnable = 0x01; 
					}					
					g_sFlashTig = 0;
					g_sRcvCnt = 12;
				}else
				{
					g_sFlashTig = 1;
					g_sRcvCnt = 2;
				}// sck 拉下
			}else if(0x00 == rcv[1])
			{
				if (g_sFlashEn > 0)
				{
					//g_flashEnable = 0x02;
					CanPreFlash();
				}
				//A7105_Write_Ram(0x06,g_a7105_id2,4);
				RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				A7105_Write_Reg(0x03,0x0B);
				g_sRcvCnt = 12;
			}
			//g_sRcvCnt = 12;
		break;

		case 0x00:                                            // 大触点拉下
			if(rcv[1] > 0)
			{				
				if (g_sFlashTig > 0)
				{
					if (flaPara.s_flashEn > 0)
					{
						g_flashEnable = 0x01; 
					}
					g_sFlashTig = 0;
					g_sRcvCnt = 12;
				}else
				{
					g_sFlashTig = 1;
					g_sRcvCnt = 2;
				}
			}
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			//A7105_Write_Reg(0x03,0x0B);
		break;

		case 0x0B:                                               // manu
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id3);
			A7105_Write_Reg(0x03,0x01);
			flaPara.s_flashEn = ((rcv[0]>>4)&(1<<mainPara.s_array));
			g_sRcvState = 40;
		break;
		
	}

	
}

void RfTest(void)
{
#ifdef _A7105_DBG_
	printf("Rf test\r\n");
#endif
	if (WORKINGMODE_TTL==*g_lightMode)
	{
		ttl_expCodeRsv = 0x7A;
	}
	g_flashEnable = 0x02;
}

void NikParseRfData(u8 *rcv, u8 len) 
{
	u8 mod = 0;
	switch (rcv[0] & 0x0f)
	{
		//case 0x7f:                      // mode information
		case 0x0f:
		//if ((rcv[0] & 0xf0) == (mainPara.s_array + 1))
		if(((rcv[0]>>4)&0x0f) & (1<< mainPara.s_array))
		{
			if (0xF6 == rcv[1])                                 //测试闪光
			{
				RfTest();
				g_rfLinkTmr = 100;
				RF_LED_LINK();
				return;
			}else if (0x86 == rcv[1])                      // 造影
			{
				g_nikComStatue = COM_STATUE_FOCUSDEPTH;
				g_rfLinkTmr = 100;
				RF_LED_LINK();
				return;
			}

			mod = rcv[2]&0x0f;
			
			if (mod != *g_lightMode)
			{
				if ((mod != WORKINGMODE_TTL) && (mod != WORKINGMODE_MANU) && (mod != WORKINGMODE_MULTI))
				{
					return;
				}
				*g_lightMode = mod;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
                              printf("mod %d\r\n",*g_lightMode);
#endif
			}

			if (WORKINGMODE_MANU == *g_lightMode)
			{
				if (menuPara.otpCod[MASTER] != rcv[8+mainPara.s_array])
				{
					menuPara.otpCod[MASTER] = rcv[8+mainPara.s_array];
					mstPara->output1 =EXPH_COV(menuPara.otpCod[MASTER]&0x7F);
					mstPara->output2 = EXPL_COV(menuPara.otpCod[MASTER]&0x7F);
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
                                        printf("otp %d\r\n",menuPara.otpCod[MASTER]);
#endif
				}
			}else if (WORKINGMODE_MULTI == *g_lightMode)
			{
				if (menuPara.WlmMul[MASTER]!=rcv[8+mainPara.s_array])
				{
					menuPara.WlmMul[MASTER] = rcv[8+mainPara.s_array];
					mstPara->multi_exp = EXPH_COV(menuPara.WlmMul[MASTER]&0x7F)<<4;
					SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
                                        printf("mul %d\r\n",menuPara.WlmMul[MASTER]);
#endif
				}
			}

			if (*multiSet.frequence != rcv[4])
			{
				*multiSet.frequence = rcv[4];
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
				printf("hz %d\r\n",*multiSet.frequence);
#endif
			}
			if (*multiSet.eTimes != (rcv[5] & 0x7f))
			{
				*multiSet.eTimes = rcv[5] & 0x7f;
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
#ifdef _A7105_DBG_
                            printf("hz %d\r\n",*multiSet.eTimes);
#endif
				//printf("tms\r\n");
			}
		}
		g_rfLinkTmr = 100;
		RF_LED_LINK();
		g_sRcvCnt = 12;
		A7105_Write_Reg(0x03,0x0B);
		break;

		//case 0x77:
		case 0x07:
			if (rcv[1] != menuPara.motoLoc)
			{
				menuPara.motoLoc = rcv[1];
				if (MOTO_MODE_AUTO == *motoSet.mode)
				{
					SET_UPDATE_EVT(DSP_UPDATE_MOTO);
				}
#ifdef _A7105_DBG_
				printf("loc %d\r\n",menuPara.motoLoc);
#endif
			}
			g_sRcvCnt = 12;
			A7105_Write_Reg(0x03,0x0B);
		break;

		case 0x05:
			if (*motoSet.mode != rcv[4+mainPara.s_array])
			{
				*motoSet.mode = rcv[4+mainPara.s_array];
			}
			if (MOTO_MODE_MANU == *motoSet.mode)
			{
				if (motoSet.destRsv != rcv[1+mainPara.s_array])
				{
					motoSet.destRsv = rcv[1+mainPara.s_array];
#ifdef _A7105_DBG_
					printf("moto %d\r\n",motoSet.destRsv);
#endif
				}
				
			}
			g_sRcvCnt = 12;
			A7105_Write_Reg(0x03,0x0B);
		break;

/*
		//case 0x79:
		case 0x09:
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id3);
			A7105_Write_Reg(0x03,0x01);
			g_sRcvCnt = 2;
			if ((rcv[0] >> 4) & (1 << mainPara.s_array))
			{
				g_sFlashEn = 1;
			}else
			{
				g_sFlashEn = 0;
			}
			g_sRcvState = 40;
		break;
*/

		case 0x0A:
			//flaPara.s_ttlCode = rcv[8+mainPara.s_array];
			ttl_expCodeRsv = rcv[8+mainPara.s_array];
			flaPara.s_flashEn = ((rcv[0]>>4)&(1<<mainPara.s_array));
			/*
			if ( !ttl_expCodeRsv )
			{
				flaPara.s_flashEn = 0;
			}else
			{
				flaPara.s_flashEn = 1;
			}*/
			// @20171116 以第八字节作为判断第一次和第二次预闪
			// 实际情况:   第一次接收到0x75，第二次接收到0x8E
			/*if ( rcv[8] & 0x80)
			{
				ttl_pre2_flag = 1;
			}else
			{
				ttl_pre2_flag = 0;
			}*/
			//printf("flag=%x\r\n",rcv[0]>>4);
			//printf("en = %d\r\n",flaPara.s_flashEn);
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id3);
			A7105_Write_Reg(0x03,0x01);
			g_sRcvState = 40;
			g_sRcvCnt = 2;
		break;

		case 0x01:
			if(0x01 == rcv[1])
			{/*
				if (g_sFlashTig > 0)
				{
					if (flaPara.s_flashEn > 0)
					{
						g_flashEnable = 0x01; 
					}					
					g_sFlashTig = 0;
					g_sRcvCnt = 12;
				}else
				{
					g_sFlashTig = 1;
					g_sRcvCnt = 2;
				}*/// sck 拉下
				if ( g_sRearFlag > 0)
				{
					/* @20171115*/
					g_flashEnable = 0x01;          // rear
					g_sRearFlag = 0;
					//ttl_pre2_flag = 0;
				}
				//g_sRcvCnt = 2;
				//A7105_Write_Reg(0x03,0x01);
			}else if(0x00 == rcv[1])
			{
				//if (g_sFlashEn > 0)
				if (flaPara.s_flashEn > 0)
				{
					//g_flashEnable = 0x02;
					//g_flashEnable = 0x04;
					NikPreFlash();
					/* add @20171115*/
					ttl_pre_dbg++;
					if (!ttl_pre2_flag)
					{
						ttl_pre2_flag = 1;
					}else
					{
						ttl_pre2_flag = 0;
					}
				}
				//A7105_Write_Ram(0x06,g_a7105_id2,4);
				RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				A7105_Write_Reg(0x03,0x0B);
			}
			//g_sRcvCnt = 12;
		break;

		case 0x00:                                            // 大触点拉下
/*			if(rcv[1] > 0)
			{				
				if (g_sFlashTig > 0)
				{
					if (flaPara.s_flashEn > 0)
					{
						g_flashEnable = 0x01; 
					}
					g_sFlashTig = 0;
					g_sRcvCnt = 12;
				}else
				{
					g_sFlashTig = 1;
					g_sRcvCnt = 2;
				}
			}*/
			if (0x73 == rcv[1])
			{
				if (flaPara.s_flashEn > 0)
				{
					//g_flashEnable = 0x02; 
					//g_flashEnable = 0x04; 
					NikPreFlash();
					/* add @20171115*/
					ttl_pre_dbg++;
					if (!ttl_pre2_flag)
					{
						ttl_pre2_flag = 1;
					}else
					{
						ttl_pre2_flag = 0;
					}
				}
				RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				g_sRcvCnt = 12;
				A7105_Write_Reg(0x03,0x0B);               // 设定阈值
#ifdef _A7105_DBG_
				printf("pre\r\n");
#endif
				//g_sRcvCnt = 12;
				//RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				//A7105_strobe_cmd(0xA0);
				//A7105_strobe_cmd(0xE0);
				
			}/*else if (0x24 == rcv[1])
			{
				if (flaPara.s_flashEn > 0)
				{
					g_flashEnable = 0x01; 
				}
#ifdef _A7105_DBG_
				printf("fla\r\n");
#endif
				//RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				//g_sRcvCnt = 12;
			}*/else if (0x00 == rcv[1] )
			{
				if (flaPara.s_flashEn > 0)
				{
					if (ttl_expCodeRsv > 0)
					{
						g_flashEnable = 0x01; 
					}
					/* @20171115*/
					//ttl_pre2_flag = 0;
				}
				RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				g_sRcvCnt = 12;
				A7105_Write_Reg(0x03,0x0B);               // 设定阈值
#ifdef _A7105_DBG_
				printf("fla\r\n");
#endif
			}else if (0x20==(rcv[1]&0xf0))
			{
				if (flaPara.s_flashEn > 0)
				{
					if (ttl_expCodeRsv > 0)
					{
						g_flashEnable = 0x01; 
					}
					/* @20171115*/
					//ttl_pre2_flag = 0;
				}
				RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				g_sRcvCnt = 12;
				A7105_Write_Reg(0x03,0x0B);               // 设定阈值
			}else if (0x38 == rcv[1])
			{
				g_sRcvCnt = 2;
				g_sRearFlag = 1;
				A7105_Write_Reg(0x03,0x01);
			}else
			{
				if (flaPara.s_flashEn > 0)
				{
					if (ttl_expCodeRsv > 0)
					{
						g_flashEnable = 0x01; 
					}
					/* @20171115*/
					//ttl_pre2_flag = 0;
				}
				RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
				g_sRcvCnt = 12;
				A7105_Write_Reg(0x03,0x0B);               // 设定阈值
				printf("fla err %x\r\n",rcv[1]);
			}
			//RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
			//g_sRcvCnt = 12;
			//A7105_Write_Reg(0x03,0x0B);               // 设定阈值
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			//A7105_Write_Reg(0x03,0x0B);
		break;

		case 0x0B:                                               // manu
			//A7105_Write_Ram(0x06,g_a7105_id3,4);
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id3);
			A7105_Write_Reg(0x03,0x01);
			flaPara.s_flashEn = ((rcv[0]>>4)&(1<<mainPara.s_array));
			g_sRcvState = 40;
		break;

		default:
			g_sRcvCnt = 12;
			A7105_Write_Reg(0x03,0x0B); 
		break;
		
	}

}

void ParseRfData(u8 *rcv, u8 len)
{
	if (RF_MODE_CAN == *g_rfMode)
	{
		CanParseRfData(rcv,len);
	}else if (RF_MODE_NIK == *g_rfMode)
	{
		NikParseRfData(rcv,len);
	}

}

void RfPreFlash(u8 *rcv, u8 len)
{
	if (RF_MODE_CAN == *g_rfMode)
	{
		//delay_us(260);
		if (g_sFlashEn > 0)
		{
			CanPreFlash();
			/*delay_us(30);
			CanPreFlash();
			delay_us(30);
			CanPreFlash();*/
		}
		A7105_Read_Ram(0x05,rcv,len);
		RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
		A7105_Write_Reg(0x03,0x0B);
		g_sRcvCnt = 12;
		g_rfStatue = RF_STATUES_IDLE;
	}
}

void RfHandleProc(void)
{
	u8 rfBuf[12];
#ifdef _A7105_DBG_     
	u8 i;
#endif
	switch (g_rfStatue)
	{
		case RF_STATUES_IDLE:
			A7105_Read_Ram(0x05,rfBuf,g_sRcvCnt);
#ifdef _A7105_DBG_
			ParseRfData(rfBuf,g_sRcvCnt);
			if ((rfBuf[0] == 0x09) || (rfBuf[0] == 0x0a) || (rfBuf[0] == 0x0b)
				||(rfBuf[0] == 0x00)||(rfBuf[0] == 0x01))
			{
				for (i=0;i<g_sRcvCnt;i++)
				{
					printf("%x ",rfBuf[i]);
				}
				printf("\r\n");
			}
			
#else
			ParseRfData(rfBuf,g_sRcvCnt);
#endif
		break;

		case RF_STATUES_PRE_FLASH:
			RfPreFlash(rfBuf,g_sRcvCnt);
		break;

		case RF_STATUES_FLASH:
		break;
	}
}

void RfSetId(u8 ch, u8* id)
{
	//u8 i = 0;
	u8 tmpBuf[4];
	tmpBuf[0] = ch*2 + id[0];
	tmpBuf[1] = ch*4 + id[1];
	tmpBuf[2] = ch*8 + id[2];
	tmpBuf[3] = ch*16 + id[3];
	
	A7105_Write_Ram(0x06,tmpBuf,4);
}

void RfSetChan(u8 chan)
{
	A7105_Write_Reg(0x0f, chan*2+0x1E);
	RfSetId(chan,(u8*)g_a7105_id2);
}

u16 g_slaveTimer = 0;
void RfSlaveEventLoop(void)
{
	u16 crtTime; 
	//if((motoSet.moto_en!=MOTO_ENABLE)||(g_moto_error!=0))
	//if((MOTO_ENABLE!=motoSet.moto_en)||g_moto_error)
	//{
	//	return;
	//}
	if (CMD_MODE_RF == *g_cmdMode)
	{/*
		if (!A7105_IRQ)
		{
			
			RfHandleProc();
			//g_rfRcvSetTag = 1;
			// Reset A7105 receive
			A7105_strobe_cmd(0xF0);
			A7105_strobe_cmd(0xC0);
		}*/	
		return;
	}else if (CMD_MODE_LIGH_F2 == *g_cmdMode)
	{
		if (g_s2Flag > 0)
		{
			 crtTime = get_counter();              // get time
       	 	if ( crtTime - g_slaveTimer > 1000L)              // key timer overflow 
       	 	{
				g_slaveTimer = crtTime;
				g_s2Timer++;
				if (g_s2Timer > 5000L)
				{
					g_s2Flag = 0;
					g_s2Timer = 0;
				}
			}
		}
	}

/*
		u8 buf[16];
		u8 i = 0;

		if (!A7105_IRQ)
		{
			A7105_Read_Ram(0x05, buf, 12);
			printf("\r\n");
			for (i=0;i<12;i++)
			{
				printf("%x ",buf[i]);
			}
			A7105_strobe_cmd(0xF0);
			A7105_strobe_cmd(0xC0);
		}
*/
	
}

void RfIntPortSet(u8 en)
{
	//asm("sim");
	PB_DDR &= ~0x01;             // input
	PB_CR1 |= 0x01;                // pull up
	if (en == ENABLE)
	{
		PB_CR2 |= 0x01;                // int enalbe
	}else
	{
		PB_CR2 &= ~0x01;                // int enalbe
	}
	//EXTI_CR2 &= 0x3F;
	//EXTI_CR2 |= 0x80;
	//asm("rim");
}


#if 0
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
#endif


