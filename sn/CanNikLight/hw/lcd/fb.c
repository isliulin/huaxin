
#include"global.h"
#include <stdarg.h>
#include "..\adc\adc.h"
#include "lcd.h"
#include "..\flash\flash.h"
#include "..\a7105\a7105.h"
#include "..\uart\uart.h"

u8 fb[16] = {0x00};
FuncList *pMenuList=0;
FuncMenu funcMenu;
u8 g_SettingIndex = 0;
u32 gWSleepCounter=0;
u8 g_shadowKey=0;
extern u8 g_keyDir;
u8 g_toggleTimer = 0;
u8 s_togFlag = 0;
u16 s_togCnt = 0;
u8 g_rfTtl = 0;
u16 s_slaveLedTimer = 0;
extern u16 g_rfLinkTmr;
u8 g_keyLock = 0;
extern CmrPara cmrPara;


u8 g_fnIndex = 0;
u8 *g_cfnOption;

void SetScrRespect(u8 mode);

extern const u8 focLenScape[10];

const u8 cfnCmrIndex[5] = {1,3,4,8,14};

#define KEY_DIR_1		PE_IDR_IDR1
#define KEY_DIR_2		PE_IDR_IDR2

//#define SET_KEYINT_RTIG()   {EXTI_CR1 &= 0xCF;	EXTI_CR1 |= 0x20;}
//#define SET_KEYINT_FTIG()   {EXTI_CR1 &= 0xCF;	EXTI_CR1 |= 0x10;}
#define SET_KEYINT_RTIG()   {EXTI_CR1 &= 0xCF;	EXTI_CR1 |= 0x10;}
#define SET_KEYINT_FTIG()   {EXTI_CR1 &= 0xCF;	EXTI_CR1 |= 0x20;}
#define DISABLE_KEY_INT()   {PE_CR2_C22 = 0;}
#define ENABLE_KEY_INT()     {PE_CR2_C22 = 1;}
#define CLEAR_KEYINT()


const u8 charFine[] = {0x00,0x03, 0x07};
const u8 charFeatures[] = {0xfa,0x60,0xd6,0xf4,0x6c,0xbc,0xbe,0xe0,0xfe,0xfc};
const u8 fineTune[] = {0,3,7};
const u8 output_table[22]={0x00,0x02,0x05,0x08,0x0a,0x0d,
									0x10,0x12,0x15,0x18,0x1a,0x1d,
									0x20,0x22,0x25,0x28,0x2a,0x2d,
									0x30,0x32,0x35,0x38,};

unsigned char canMaxCalc[][14] = {
{100,100,100,100,100,90,80,70,70,60,50,40,40,30},        	// 1/128
{90,90,90,80,80,70,60,50,40,40,35,30,20,15},			// 1/64
{60,60,60,50,50,40,30,20,20,20,18,16,12,10},			// 1/32
{30,30,30,20,20,20,10,8,8,8,8,8,8,8},					// 1/16
{14,14,12,10,8,6,5,4,4,4,4,4,4,4},						// 1/8
{7,6,5,4,4,3,3,2,2,2,2,2,2,2},                                           	// 1/4
};

void EnableKeyInt(void)
{
	ENABLE_KEY_INT();
}

void ResetDailInt(void)
{
	SET_KEYINT_FTIG();
	ENABLE_KEY_INT();
}

void SetCmpFeb(s8 cmp1, s8 cmp2, s8 feb1, s8 feb2, u8 isCmp, u8 sel)
{
	//u8 i=0;
	s8 value1=0,value2=0;

	value1 = cmp1;
	value2 = cmp2;

#if 1
	
	if (!g_cfnOption[CFN_FEB_TURN])
	{
		switch (flaPara.FEB_count)
		{
			case 0:
				value1= cmp1;
				value2= cmp2;
			break;

			case 1:
				value1= cmp1 - feb1;
				value2= cmp2 - feb2;
			break;

			case 2:
				value1= cmp1 + feb1;
				value2= cmp2 + feb2;
			break;
		}
	}else
	{
		switch (flaPara.FEB_count)
		{
			case 1:
				value1= cmp1;
				value2= cmp2;
			break;

			case 0:
				value1= cmp1 - feb1;
				value2= cmp2 - feb2;
			break;

			case 2:
				value1= cmp1 + feb1;
				value2= cmp2 + feb2;
			break;
		}
	}


	if(value2<0) 
	{
		value1--;
		value2 = 3 + value2;	
	}
	else if(value2>2)
	{
		value1++;
		value2 = value2 - 3;
	}
	
#endif

	if (SETTING_TTLFEB == sel)
	{
		fb[5] |= 0x80;                    // T26  feb icon
		fb[2] |= charFeatures[feb1];
		fb[3] |= 0x01;
		fb[3] |= charFeatures[fineTune[feb2]];
	}else if (SETTING_ADDSUB == sel)
	{
		if(value1 < 0)
		{
			fb[1] |= 0x01;                          //  T14
			if (value2 > 0)
			{
				value1 ++;
				value2 = 3-value2;
			}
			value1 = -value1;
		}else 
		{
			fb[1] |= 0x01;                          //  T14
			fb[4] |= 0x10;
		}
		
		fb[2] |= charFeatures[value1];
		fb[3] |= 0x01;
		fb[3] |= charFeatures[charFine[value2]];
		fb[5] |= 0x08;                                   // cmp icon T18
	}/*else if (feb1 || feb2)
	{
		//fb [6] = charFeatures[flaPara.FEB_count+1];

		fb[5] |= 0x80;                    // T26  feb icon
		
		if(value1 < 0)
		{
			fb[1] |= 0x01;                          //  T14
			if (value2 > 0)
			{
				value1 ++;
				value2 = 3-value2;
			}
			value1 = -value1;
		}else
		{
			fb[1] |= 0x01;                          //  T14
			fb[4] |= 0x10;
		}
		
		fb[2] |= charFeatures[value1];
		fb[3] |= 0x01;
		fb[3] |= charFeatures[charFine[value2]];
		fb[5] |= 0x08;                                   // cmp icon T18
		
	}*/else //if(SETTING_ADDSUB == sel)
	{
		if ((feb1 > 0) || (feb2 > 0))
		{
			fb[5] |= 0x80;                    // T26

		}
		
		if ((!value1)&&(!value2)/*&&(!feb1)&(!feb2)*/)
		{
			return;
		}
		
		if(value1 < 0)
		{
			fb[1] |= 0x01;                          //  T14
			if (value2 > 0)
			{
				value1 ++;
				value2 = 3-value2;
			}
			value1 = -value1;
		}else 
		{
			fb[1] |= 0x01;                          //  T14
			fb[4] |= 0x10;
		}
		
		fb[2] |= charFeatures[value1];
		fb[3] |= 0x01;
		fb[3] |= charFeatures[charFine[value2]];
		fb[5] |= 0x08;                                   // cmp icon T18
	}
	
	
	
}

void SetSyn(s8 syn)
{
	switch (syn)
	{
		case SYNC_MODE_FRON:
		break;

		case SYNC_MODE_REAR:
			fb[5] |= 0x10;
		break;
	}

}

void SetTtlScr(void)
{
	s8 cmp1=0,cmp2=0;
	if (CMR_LINK_CAN==g_cmrLinked) /*仅代表sck 处于上拉状态*/
	{
	// T4: SIG13 COM1
		if (g_canCmrLink > 0)
		{
			fb[4] |= 0x06;
		}
	}else if(CMR_LINK_NIK==g_cmrLinked)/*仅代表nss 处于上拉状态*/
	{
		if (cmrPara.cls > 0)
		{
			fb[4] |= 0x0a; 
		}
	}
	fb[4] |= 0x01; 

	if (mstPara->cmp1||mstPara->cmp2)
	{
		cmp1 = mstPara->cmp1;
		cmp2 = mstPara->cmp2;
	}//else if ((flaPara.cmrCmp1) || (flaPara.cmrCmp2))
	else if (CMR_LINK_CAN == g_cmrLinked)
	{
		cmp1 = flaPara.cmrCmp1;
		cmp2 = flaPara.cmrCmp2;
	}

	if (CMR_LINK_NIK==g_cmrLinked)
	{
		SetCmpFeb(cmp1,cmp2,0,0,0,g_SettingIndex);
	}else
	{
		SetCmpFeb(cmp1,cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0,g_SettingIndex);
	}
	//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,0,g_SettingIndex);
	
	SetSyn(mainPara.synMode);
	
}

u8 Power(u8 n)
{
	u8 ret = 1;
	while(n--)
	{
		ret *= 2;
	}
	return ret;
}

void SetManuPara(s8 output1,s8 output2)
{
	u8 chDsp;
	fb[7] |= 0x01;                        // T27 
	chDsp = Power(7-output1);
	
	if (chDsp >= 100)
	{
		fb [6] |= 0x01;                // T28
	}

	if(chDsp >= 10)
	{
		fb [6] |= charFeatures[(chDsp%100)/10];
	}

	fb [7] |= charFeatures[chDsp%10];

	if (output2 > 0)
	{
		fb[4] |= 0x10;                    // T13
		fb[1] |= 0x01;                     // T14
		fb[2] |= charFeatures[0];
		//fb[3] |= 0x01;
		fb[3] |= (charFeatures[fineTune[output2]] |0x01) ;
		
	}
}

void SetManuScr(void)
{
	fb[4] |= 0x80;
	SetManuPara(mstPara->output1,mstPara->output2);	
	SetSyn(mainPara.synMode);
}

void SetMultiTmFrq(u8 mulTimes, u16 mulFrq)
{
	if (mulFrq >= 200)
	{
		mulFrq = 199;
	}
	if (mulTimes >= 100)
	{
		fb[0] |= 0x01;
	}
	if (mulTimes>=10)
	{
		fb [0] |= charFeatures[((mulTimes%100) /10)];
	}

	if (!mulTimes)
	{
		// 20171225 set multi time zero as "--"
		fb[0] |= 0x04;
		fb[1] |= 0x04;
	}else
	{
		fb [1] |= charFeatures[mulTimes%10];
	}

	
	fb [1] |= 0x01;

	if (mulFrq >= 100)
	{
		fb [2] |= 0x01;
	}

	if (mulFrq >= 10)
	{
		fb[2] |= charFeatures[(mulFrq%100)/10];
	}

	fb [3] |= charFeatures[mulFrq%10]; 
	
 }

void SetMultiScr(void)
{
	fb[4] |= 0xc0;
	SetMultiTmFrq(*multiSet.eTimes,*multiSet.frequence);
	//SetManuPara(*multiSet.expose>>4,*multiSet.expose&0x0f);	
	SetManuPara(*multiSet.expose>>4,0);	
}

void SetRfGr(u8 gr)
{
	switch (gr)
	{
		case 0:
			fb[13] |= 0x01;
		break;

		case 1:
			fb[13] |= 0x02;
		break;

		case 2:
			fb[13] |= 0x04;
		break;
	}

}

void SetRfChannal(u8 ch)
{
	fb[13] |= 0x08;                                // T37
	if (ch >= 10)
	{
		fb[11] |= 0x01;                        // T38
	}
	ch %= 10;

	fb[11] |= charFeatures[ch];
}

void SetRfCanNik(void)
{
	if (RF_MODE_CAN == *g_rfMode)
	{
		fb[4] |= 0x06; 
	}else if (RF_MODE_NIK == *g_rfMode)
	{
		fb[4] |= 0x0a; 
	}
}

void SetRfScr(void)
{
	fb[8] |= 0x02;                       // T19
	fb[5] |= 0x03;                       // T24,T25
	fb[5] |= 0x60;

/*
	if (g_rfTtl > 0)
	{
		fb[4] |= 0x01; 
	}
*/
	SetScrRespect(mainPara.workMode);

	SetRfGr(mainPara.s_array);
	SetRfChannal(mainPara.s_chan+1);

	SetRfCanNik();

	//A7105_Init();
}

void SetS1Scr(void)
{
	//fb[13] |= 0x40;                    // T20
	fb[14] |= 0x04;                    // T20
	fb[5] |= 0x03;                       // T24,T25
	fb[5] |= 0x60;

	SetManuPara(mstPara->output1,mstPara->output2);
}

void SetS2Scr(void)
{
	//fb[13] |= 0x80;                     // T11
	fb[14] |= 0x08;                    // T20
	fb[5] |= 0x03;                       // T24,T25
	fb[5] |= 0x60;

	SetManuPara(mstPara->output1,mstPara->output2);	
}

void SetScrRespect(u8 mode)
{
	//RF_LED_OFF();
	
	switch(mode)
	{
		case WORKINGMODE_TTL:
			SetTtlScr();
		break;

		case WORKINGMODE_MANU:
			SetManuScr();
		break;

		case WORKINGMODE_MULTI:
			SetMultiScr();
		break;
	}
}

void SetCfnScr(void)
{
	fb[0] = 0x8E;                         // F
	fb[1] = 0x27;                         // n

	//fb[2] = charFeatures[cfnCmrIndex[g_fnIndex]/10];
	//fb[3] = charFeatures[cfnCmrIndex[g_fnIndex]%10];
	fb[2] = charFeatures[0];
	fb[3] = charFeatures[g_fnIndex+1];

	fb[6] = charFeatures[0];
	fb[7] = charFeatures[g_cfnOption[g_fnIndex]];

	LoadButtons();

}


void FuncDsp(void)
{
	if (g_cfnOption[4] > 0)
	{
		fb [9] |= 0x01;
	}

	if(CMD_MODE_OFF == *g_cmdMode)
	{/*
		if (CMR_LINK_NULL!=g_cmrLinked) 
		{
			fb[14] |= 0x02;                    // T31
		}*/
		if (CMR_LINK_CAN == g_cmrLinked)
		{
 			if (g_canCmrLink > 0)
 			{
				fb[14] |= 0x02;                    // T31
			}
 		}else if (CMR_LINK_NIK == g_cmrLinked)
 		{
 			if (cmrPara.cls > 0)
			{
				fb[14] |= 0x02;                    // T31
			}
		}
	}

	if (g_lowbatDsp>0)
	{
		fb[10] |= 0x01;		                    // T9 lowbat icon
	}else
	{
		fb[10] &= ~0x01;
	}
	
}

void RepaintScr(void)
{
#ifdef CAN_COM_DBG
	printf("scr\n");
#endif
	memset(fb, 0x00, 16);

	if (SETTING_CFN == g_SettingIndex)
	{
		SetCfnScr();
	}else
	{	
		switch (*g_cmdMode)
		{
			case CMD_MODE_OFF:
				SetScrRespect(mainPara.workMode);
				//RF_LED_OFF();
			break;

			case CMD_MODE_RF:
				SetRfScr();
				//RF_LED_READY();
			break;

			case CMD_MODE_LIGH_F1:
				SetS1Scr();
				//RF_LED_OFF();
			break;

			case CMD_MODE_LIGH_F2:
				SetS2Scr();
				//RF_LED_OFF();
			break;
		}

		DisplayFocLen(*motoSet.mode,*motoSet.dest_location);

		if (!g_keyLock)
		{
			LoadButtons();
		}

		FuncDsp();
		
	}

	//LcdWriteData(0x00,fb,16);
	LcdWriteData(0x00,fb,16);
#ifdef CAN_COM_DBG
	printf("exit\n");
#endif
	
}

void FillScr(void)
{
	memset(fb, 0xff, 16);
	LcdWriteData(0x00,fb,16);
}

void KeyModeProc(void)
{
	if (SETTING_CFN == g_SettingIndex)
	{
		return;
	}
	
	switch (*g_cmdMode)
	{
		case CMD_MODE_OFF:
			(*g_lightMode) ++;
			if (*g_lightMode > WORKINGMODE_MULTI)
			{
				*g_lightMode = WORKINGMODE_TTL;
			}
		break;

		case CMD_MODE_RF:
			if (RF_MODE_CAN == *g_rfMode)
			{
				*g_rfMode = RF_MODE_NIK;
				g_cmrLinked = CMR_LINK_NIK;
			}else
			{
				*g_rfMode = RF_MODE_CAN;
				g_cmrLinked = CMR_LINK_CAN;
			}
		break;

		case CMD_MODE_LIGH_F1:
		break;

		default:
		return;
	}
	if (CMR_LINK_NIK==g_cmrLinked)
	{
		SetSpiComModeOff(*g_lightMode,0);
	}else if (WORKINGMODE_MULTI == *g_lightMode)
	{
		mainPara.synMode = SYNC_MODE_FRON;	
	}
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	//RepaintScr();
}

void RfHardWareCheck(void)
{
	if (*g_cmdMode == CMD_MODE_RF)
	{
		if (RF_MODE_CAN == *g_rfMode) 
		{
			g_cmrLinked = CMR_LINK_CAN;
		}else
		{
			g_cmrLinked = CMR_LINK_NIK;
		}
		A7105_RX_AF = 1;                                             // af
#if (BOARD_VER == __BOARD_10_)
		A7105_TX_AF = 0;       //
#endif
		*g_lightMode = WORKINGMODE_TTL;
		RF_LED_READY();
		A7105_Init();
		RfIntPortSet(ENABLE);
		flaPara.ttl_FEB1 = flaPara.ttl_FEB2 =0;
		menuPara.otpCod[MASTER] = 0xff;
		menuPara.WlmMul[MASTER] = 0xff;
	}else
	{
		A7105_RX_AF = 0;
#if (BOARD_VER == __BOARD_10_)
		A7105_TX_AF = 0;
#endif
		*g_lightMode = WORKINGMODE_TTL;
		//g_cmrLinked = CMR_LINK_NULL;
		RF_LED_OFF();
		A7105Halt();
		RfIntPortSet(DISABLE);
	}

	if ((CMD_MODE_LIGH_F1 == *g_cmdMode) || (CMD_MODE_LIGH_F2 == *g_cmdMode))
	{
		PB_DDR &= ~0x02;
		PB_CR1 |= 0x02;
		PB_CR2 |= 0x02;
	}else
	{
		PB_CR2 &= ~0x02;
	}
}

void KeyRfSwProc(void)
{
	if (SETTING_CFN == g_SettingIndex)
	{
		return;
	}

	// the camera is been linked, do not change the mode
#ifdef NIK_COM_DBG
	printf("link %d, can : %d, nik : %d\n", g_cmrLinked, g_canCmrLink,cmrPara.cls);
#endif

	if (CMR_LINK_CAN==g_cmrLinked)
	{
		if (g_canCmrLink > 0)
		{
			return;
		}
	}else if (CMR_LINK_NIK == g_cmrLinked)
	{
		if (cmrPara.cls > 0)
		{
			return;
		}
	}/*
	if ((g_canCmrLink > 0) ||(cmrPara.cls > 0))           
	{
		return;
	}*/
/*
	if (CMD_MODE_RF == *g_cmdMode)         // RF
	{
		A7105_RX_AF = 1;
		A7105_TX_AF = 0;
	}else
	{
		A7105_RX_AF = 0;
		A7105_TX_AF = 0;
		A7105_Write_Reg(0x00,0x00);
	}
*/
	
	(*g_cmdMode) ++;
	if (*g_cmdMode > CMD_MODE_LIGH_F2)
	{
		*g_cmdMode = CMD_MODE_OFF;
	}
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	AF_OFF();                                             // TURN OFF AF, IN CASE SLAVE TOGGLE KEEPS AF ON
	// in rf mode, set linked flag as the mode default

	RfHardWareCheck();

	g_cmrLinked = CMR_LINK_NULL;
	g_canComStatue = COM_STATUE_IDLE; 
	g_nikComStatue = COM_STATUE_IDLE;
	//RepaintScr();
}

void KeyFunc1Proc(void)
{
	if(pMenuList->button[0]->actionFuc.pPressFuc!=NULL)
	{
		pMenuList->button[0]->actionFuc.pPressFuc();
	}
}

void KeyFunc1Long(void)
{
	if(pMenuList->button[0]->actionFuc.pHoldFuc!=NULL)
	{
		pMenuList->button[0]->actionFuc.pHoldFuc();
	}
}

void KeyFunc2Proc(void)
{
	if(pMenuList->button[1]->actionFuc.pPressFuc!=NULL)
	{
		pMenuList->button[1]->actionFuc.pPressFuc();
	}
}

void KeyFunc3Proc(void)
{
	if(pMenuList->button[2]->actionFuc.pPressFuc!=NULL)
	{
		pMenuList->button[2]->actionFuc.pPressFuc();
	}
}

void KeyFuncSwProc(void)
{
	if(pMenuList->button[3]->actionFuc.pPressFuc!=NULL)
	{
		pMenuList->button[3]->actionFuc.pPressFuc();
	}
}

void CfnSelect(void)
{
	if (g_cfnOption[g_fnIndex] > 0)
	{
		g_cfnOption[g_fnIndex] = 0;
	}else
	{
		g_cfnOption[g_fnIndex] = 1;
	}
}

void KeyOkProc(void)
{
	if (SETTING_CFN == g_SettingIndex)
	{
		CfnSelect();
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}else
	{
		if (SETTING_IDLE!=g_SettingIndex)
		{
			g_SettingIndex = 0;
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
	}
}

void MultiMaxTimes(u16 frq,u8 exp,u16 *eTm,u8 *maxTm)
{
//	unsigned char frqTmp;
	u8 arrNum;
	u8 tMax;

/*
	if(frq>10)
	{
		frqTmp = 11;
	}else
	{
		frqTmp = frq;
	}

	if(frq<=3)
	{
		arrNum = 0;
	}else
	{
		arrNum = frqTmp - 3;
	}
*/
	if(frq<6)
	{
		arrNum = frq - 1;
	}else if(frq<8)
	{
		arrNum = 5;
	}else if(frq<10)
	{
		arrNum = 6;
	}else if(frq<11)
	{
		arrNum = 7;
	}else if(frq<12)
	{
		arrNum = 8;
	}else if(frq<15)
	{
		arrNum = 9;
	}else if(frq<20)
	{
		arrNum = 10;
	}else if(frq<60)
	{
		arrNum = 11;
	}else if(frq<250)
	{
		arrNum = 12;
	}else
	{
		arrNum = 13;
	}

//#ifdef _CANON_
//	tMax =  canMaxCalc[exp][arrNum];
//#else
	tMax =  canMaxCalc[exp>>4][arrNum];
//#endif
	
	
	*maxTm = tMax;

	if(*eTm>tMax)
	{
		//tMax = eTm;
		*eTm = tMax;
	}
	
}

void ExposeAddSub(u8 dir, u8 mode)
{
	//switch(*g_lightMode)
	switch (mode)
	{
		case WORKINGMODE_TTL:
			if(KEY_DIR_LEFT==dir)
			{
				if((-3==mstPara->cmp1)&&(0==mstPara->cmp2))
				{
					return;
				}
				mstPara->cmp2--;
				if(mstPara->cmp2<0)
				{
					mstPara->cmp2+=3;					
					mstPara->cmp1--;
				}
			}else
			{
				if(3==mstPara->cmp1)
				{
					return;
				}
				mstPara->cmp2++;
				if(mstPara->cmp2>2)
				{
					mstPara->cmp1++;
					mstPara->cmp2 = 0;
				}				
			}
			//menuPara.mstTtl = compensation_table[(mstPara->cmp1+6)*3+mstPara->cmp2];
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case WORKINGMODE_MANU:
			if(KEY_DIR_LEFT==dir)
			{
				if((0==mstPara->output1)&&(0==mstPara->output2))
				{
					//mstPara->output1 = 7;
					//mstPara->output2 = 0;
					return;
				}else
				{
					mstPara->output2--;
					if(mstPara->output2<0)
					{
						mstPara->output2+=3;
						mstPara->output1--;
					}			
				}
			}else
			{
				if(7==mstPara->output1)
				{
					//mstPara->output1 = 0;
					//mstPara->output2 = 0;
					return;
				}else
				{
					mstPara->output2++;
					if(mstPara->output2>2)
					{
						mstPara->output1++;
						mstPara->output2=0;
					}
					
				}
			}
			//menuPara.otpCod[MASTER] = output_table[(7-mstPara->output1)*3-mstPara->output2];
		//SetManuPara(mstPara->output1,mstPara->output2,DSP_SELE);
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case WORKINGMODE_MULTI:
			if(KEY_DIR_LEFT==dir)
		{
			if(0x00==*multiSet.expose)
			{
				//*multiSet.expose = 0x40;
				//MultiCalMaxTime();
				//MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
				return;
			}else
			{/*
				if(0==(*multiSet.expose&0x0f))
				{
					(*multiSet.expose) -=0x10;
					(*multiSet.expose) += 2;
				}else
				{
					(*multiSet.expose) --;
				}*/
				(*multiSet.expose) -=0x10;
			}
		}else
		{
			if(0x50<=*multiSet.expose)
			{
				//*multiSet.expose = 0x00;
				return;
			}else
			{/*
				if(2==(*multiSet.expose&0x0f))
				{
					(*multiSet.expose) +=0x10;
					(*multiSet.expose) &= 0xf0;
				}else
				{
					(*multiSet.expose) ++;
				}*/
				(*multiSet.expose) +=0x10;
			}
			//MultiCalMaxTime();
			MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
		}
		//multiSet.WlmMul[MASTER]=output_table[(7-*multiSet.expose)*3];
		//menuPara.WlmMul[MASTER]=output_table[(7-(*multiSet.expose>>4))*3];
		//SetMultiPara(*multiSet.expose,mainPara.multi_times,mainPara.multi_Frq,DSP_SELE);
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;
	}
}

void MotoShift(u8 dir)
{
		//printf ("motoshift\r\n");
		if(motoSet.moving != 0)
		{
			//printf ("moving\r\n");
			return;
		}
		if(g_isReflectPulled>0) 
		{
			//printf ("pulled\r\n");
			return;
		}
		
		if(KEY_DIR_LEFT==dir)
		{
			motoSet.focIndex--;
		}else
		{
			if(MOTO_MODE_AUTO==*motoSet.mode)
			{
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.focIndex = 0;
			}else
			{
				motoSet.focIndex++;
			}
		}
		
		//if(motoSet.focIndex<=0)
		if(motoSet.focIndex<0)
		{
			motoSet.focIndex = 0;
			//*motoSet.dest_location = focLenScape[0];				//delete 2015/06/14
			//*motoSet.dest_location = 35;
			//motoSet.destRsv = *motoSet.dest_location;				//delete 2015/06/14
			//MotoReset();
			//LCD_MMOTO_OFF;
			*motoSet.mode = MOTO_MODE_AUTO;
			menuPara.motoCod = 0x01;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_AUTO;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_AUTO &  motoSet.focIndex);
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//g_paraChange = 1;
			//SetFocLen(0,0);
			*motoSet.dest_location = motoSet.autoDest;
			motoSet.destRsv = *motoSet.dest_location;
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location); 
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}//else if(motoSet.focIndex>11)
		//else if(motoSet.focIndex>10)
		else if(motoSet.focIndex>9)
		{
			//motoSet.focIndex=11;
			//motoSet.focIndex = 10;
			motoSet.focIndex = 9;
			motoSet.focLen = 0x45;
			menuPara.motoCod = 0x17;
			//MotoReset();
		}
		else
		{
			//LCD_MMOTO_ON;
			*motoSet.mode = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = MOTO_MODE_MANU;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = motoSet.focIndex;
			//g_paraBakBuf[EEPROM_INDEX_MOTOMODE] = (MASK_MOTOMODE_MANU | motoSet.focIndex);
			//ParaPreserve(EEPROM_INDEX_MOTOMODE,(MASK_MOTOMODE_MANU|motoSet.focIndex));
			//*motoSet.dest_location = focLenScape[motoSet.focIndex];
			//*motoSet.dest_location = focLenScape[motoSet.focIndex-1];
			*motoSet.dest_location = focLenScape[motoSet.focIndex];
			motoSet.destRsv = *motoSet.dest_location;
			//motoSet.focLen = focLenCode[motoSet.focIndex];
			// 该参数对应0xD8，随意改变的后果，耽误一整天时间
			motoSet.focLen = focLenCode[motoSet.focIndex];                                                
			//menuPara.motoCod = (0x11+motoSet.focIndex)>0x17?0x17:(0x11+motoSet.focIndex);
			menuPara.motoCod = motoSet.focIndex>0?((0x10+motoSet.focIndex)>0x17?0x17:(0x11+motoSet.focIndex)):0x11;
			//g_paraBakBuf[EEPROM_INDEX_LOCATION] = *motoSet.dest_location;
			//ParaPreserve(EEPROM_INDEX_LOCATION,*motoSet.dest_location);
			//DisplayFocLen(*motoSet.mode,*motoSet.dest_location); 
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}

		//if(g_cmrLinked>0)
		/*
		// refresh the scale ruler
		if((g_cmrPressed>0)||(WORKINGMODE_EXTM==mainPara.workMode))
		{
			//SET_UPDATE_EVT(DSP_UPDATE_LINK);
			SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
		}*/
}

void MultiTimesShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
                if (!mainPara.multi_times)
                {
                  return;
                }
		if(mainPara.multi_times<=20)
		{
			mainPara.multi_times--;
		}else if(mainPara.multi_times<=50)
		{
			mainPara.multi_times-=5;
		}else
		{
			mainPara.multi_times-=10;
		}
/*
		if(mainPara.multi_times<0)
		{
			mainPara.multi_times = 0;
		}*/
	}else
	{
		if(mainPara.multi_times<20)
		{
			mainPara.multi_times++;
		}else if(mainPara.multi_times<50)
		{
			mainPara.multi_times+=5;
		}else
		{
			mainPara.multi_times+=10;
		}
	}
	//MultiCalMaxTime();
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
	
	//LCD_MULTIME_RVS(mainPara.multi_times);
	//MULTI_SET_ETIMES(mainPara.multi_times, g_SettingIndex)
	//MULTI_SET_FRQ(mainPara.multi_Frq,g_SettingIndex);
	//SetMultiTmFrq(mainPara.multi_times,mainPara.multi_Frq,g_SettingIndex);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void MultiFrqShift(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		if(mainPara.multi_Frq<=20)
		{
			mainPara.multi_Frq--;
		}else if(mainPara.multi_Frq<=50)
		{
			mainPara.multi_Frq-=5;
		}else if(mainPara.multi_Frq<=200)
		{
			mainPara.multi_Frq-=10;
		}else if(mainPara.multi_Frq<=500)
		{
			mainPara.multi_Frq-=50;
		}
		
		if(mainPara.multi_Frq<1)
		{
			mainPara.multi_Frq = 1;
		}
	}else
	{
		if(mainPara.multi_Frq<20)
		{
			mainPara.multi_Frq++;
		}else if(mainPara.multi_Frq<50)
		{
			mainPara.multi_Frq+=5;
		}else if(mainPara.multi_Frq<200)
		{
			mainPara.multi_Frq+=10;
			//if(mainPara.multi_Frq==200)
				//mainPara.multi_Frq = 199;
		}/*else if(mainPara.multi_Frq<500)
		{
			mainPara.multi_Frq+=50;
		} */else
		{
			mainPara.multi_Frq = 200;
		}
		//if(mainPara.multi_Frq>199)

	}
	//MultiCalMaxTime();
	MultiMaxTimes(*multiSet.frequence,*multiSet.expose,multiSet.eTimes, &multiSet.maxTimes);
	//SetMultiTmFrq(mainPara.multi_times,mainPara.multi_Frq,g_SettingIndex);
	//MULTI_SET_ETIMES(mainPara.multi_times,g_SettingIndex);
	//MULTI_SET_FRQ(mainPara.multi_Frq, g_SettingIndex)
	//LCD_MULFRQ_RVS(mainPara.multi_Frq);	
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void ChannalAddSub(u8 dir)
{
	if (KEY_DIR_LEFT == dir)
	{
		if (mainPara.s_chan > 0)
		{
			mainPara.s_chan --;
		}
	}else
	{
		if (mainPara.s_chan < (15 - 1))
		{
			mainPara.s_chan ++;
		}
	}
	RfSetChan(mainPara.s_chan);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void CfnAddSub(u8 dir)
{
	if (KEY_DIR_LEFT == dir)
	{
		if (g_fnIndex > 0)
		{
			g_fnIndex --;
		}
		
	}else
	{
		if (g_fnIndex < MENU_ITEMS - 1)
		{
			g_fnIndex ++;
		}
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void TtlFebAddSub(u8 dir)
{
	if(KEY_DIR_LEFT==dir)
	{
		if((!flaPara.ttl_FEB2)&&(!flaPara.ttl_FEB1))
			return;

		if (flaPara.ttl_FEB2 > 0)
		{
			flaPara.ttl_FEB2 --;
		}else
		{
			flaPara.ttl_FEB1--;
			flaPara.ttl_FEB2+=2;
		}

	}else
	{
		if(flaPara.ttl_FEB1>=3)
		{
			return;
		}
		flaPara.ttl_FEB2++;
		if(flaPara.ttl_FEB2>=3)
		{
			flaPara.ttl_FEB1++;
			flaPara.ttl_FEB2-=3;
			
		}
	}		
	//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_REVERSE);
	/*
	if(WORKINGMODE_TTL==mainPara.workMode)
	{
		SetTtlScr();
	}else
	{
		CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
	}*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}


void KeySubAdd(u8 dir)
{
	switch (g_SettingIndex)
	{/*
		case SETTING_SYNMODE:
			if (SYNC_MODE_FRON == mainPara.synMode)
			{
				mainPara.synMode = SYNC_MODE_REAR;
			}else
			{
				mainPara.synMode = SYNC_MODE_FRON;
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;*/

		case SETTING_ADDSUB:
			switch (*g_cmdMode)
			{
				case CMD_MODE_OFF:
					ExposeAddSub(dir, *g_lightMode);
				break;

				case CMD_MODE_RF:
					if (WORKINGMODE_TTL == *g_lightMode )
					{
						ExposeAddSub(dir, *g_lightMode);
					}
				break;

				case CMD_MODE_LIGH_F1:
				case CMD_MODE_LIGH_F2:
					ExposeAddSub(dir, WORKINGMODE_MANU);
				break;
			}
		break;

		case SETTING_MOTO:
			MotoShift(dir);
		break;

		case SETTING_MULTI:
			MultiTimesShift(dir);
		break;

		case SETTING_FRQ:
			MultiFrqShift(dir);
		break;

		case SETTING_CH:
			ChannalAddSub(dir);
		break;

		case SETTING_CFN:
			CfnAddSub(dir);
		break;

		case SETTING_TTLFEB:
			TtlFebAddSub(dir);
		break;
	}
}

void KeyLeftProc(void)
{
	KeySubAdd(KEY_DIR_LEFT);
	EnableKeyInt();
}

void KeyRightProc(void)
{
	KeySubAdd(KEY_DIR_RIGHT);
	EnableKeyInt();
}

void KeyPilotProc(void)
{
	if (g_chargeOk == AD_CHARGE_LOW)
	{
		return;
	}

	//g_flashTime = OutputSwitch(mstPara->output1,mstPara->output2);
	g_flashEnable = 1;
	
}

void KeyEnterFn(void)
{
	if (SETTING_CFN == g_SettingIndex)
	{
		g_SettingIndex = 0;
	}else
	{
		g_SettingIndex = SETTING_CFN;
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void KeyLockProc(void)
{
#ifdef DETAIL_DBG
	//printf("lock\r\n");
#endif
	if (!g_keyLock)
	{
		g_keyLock = 1;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
}

void KeyUnlockProc(void)
{
#ifdef DETAIL_DBG
	//printf("unlock\r\n");
#endif
	if (g_keyLock)
	{
		g_keyLock = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
}


void KeyEventLoop( void )
{
	u8 key;
	if(KEY_NULL == ParseKey(&key))
	{
		return;
	}

	BkLightOn();

	if (g_keyLock)
	{
		if ((key!=KEY_LOCK)&&(key!=KEY_LOCK_DEBOUNCE))
		{
			KeyUnlockProc();
		}
	}


	g_sysSleepCnt = 0;

	switch (key)
	{
		case KEY_MODE:
			KeyModeProc();
		break;

		case KEY_RF_SW:
			KeyRfSwProc();
		break;

		//case KEY_FUNC1:
		case KEY_FUNC1_DEBOUNCE:
			KeyFunc1Proc();
		break;

		case KEY_FUNC2:
			KeyFunc2Proc();
		break;

		case KEY_FUNC3:
			KeyFunc3Proc();
		break;

		case KEY_MENU_SW:
			KeyFuncSwProc();
		break;

		case KEY_OK:
			KeyOkProc();
		break;

		case KEY_LEFT:
			KeyLeftProc();
		break;

		case KEY_RIGHT:
			KeyRightProc();
		break;

		case KEY_PILOT:
			KeyPilotProc();
			//FillScr();
		break;

		case KEY_FUNC1_LONG:
			KeyFunc1Long();
		break;

		case KEY_LOCK:
			KeyLockProc();
		break;

		case KEY_LOCK_DEBOUNCE:
			KeyUnlockProc();
		break;
	}
	
}

void SetDailInt(void)
{
	PE_DDR &= 0xf9;                                  // PE1, PE2
	//EXTI_CONF2 |= 0x20;	                        // port G as interrupt
	EXTI_CR1 &= 0xCF;
	EXTI_CR1 |= 0x20;                               // Portx bit 3 external interrupt Falling edge only
	
	PE_CR1_C12 = 1;                 			//PG4 输入pull up
	PE_CR2_C22 = 1;
	
}

// 拨盘中断
// PG2, PG3
#pragma vector= EXTI2_vector
__interrupt void DialPad_IRQHandler(void)
{
#ifdef SLEEP_DBG
	//printf("dial\r\n");
#endif
	
	if(EXTI_SR1 & 0x04) 
	{
		EXTI_SR1 |= 0x04; 

		if(!g_shadowKey)
		{
			if(!KEY_DIR_1)
			{
				//g_keyDir = KEY_DIR_LEFT;
				g_shadowKey = KEY_DIR_LEFT;
			}else
			{
				//g_keyDir = KEY_DIR_RIGHT;
				g_shadowKey = KEY_DIR_RIGHT;
			}
			//Ex_NVIC_Config(GPIO_B,10,RTIR);
			SET_KEYINT_RTIG();
		}else
		{
			if((KEY_DIR_LEFT==g_shadowKey)&&(KEY_DIR_1>0))
			{
				g_keyDir = KEY_DIR_LEFT;                  // 20171230
				//g_keyDir = KEY_DIR_RIGHT;
				SET_KEYINT_FTIG();
				DISABLE_KEY_INT();
			}else if((KEY_DIR_RIGHT==g_shadowKey)&&(!KEY_DIR_1))
			{
				g_keyDir = KEY_DIR_RIGHT;               // 20171230
				//g_keyDir = KEY_DIR_LEFT;
				SET_KEYINT_FTIG();
				DISABLE_KEY_INT();
			}else
			{
				SET_KEYINT_FTIG();
				ENABLE_KEY_INT();
				//FLAOK_TOGGLE();
			}

			g_shadowKey = 0;
			//DISABLE_KEY_INT();
		}
		
		//g_keyDir = 1;
		//Ex_NVIC_Disable(1);
		//ENABLE_KEY_INT();
		//DISABLE_KEY_INT();
		//EXTI->PR=1<<1;  //清除LINE0上的中断标志位  
		//EXTI->PR=1<<10;
		CLEAR_KEYINT();
	}
}


void FoclenRefresh(void)
{
//	u8 i;

	if(MOTO_MODE_AUTO==*motoSet.mode)
	{
		//if(menuPara.motoLoc!=motoSet.destRsv)
		//if(menuPara.motoLoc!=motoSet.focLen)
		if ((menuPara.motoLoc!=motoSet.autoRsv)
			&& (menuPara.motoLoc >= 20))                                   // 20180105
		{
#ifdef MOTO_DBG
			printf ("auto: %d\r\n",menuPara.motoLoc);
#endif
			motoSet.autoRsv = menuPara.motoLoc;
			/*
			if(menuPara.motoLoc>=focLenScape[motoSet.focIndex+1])
			{
				motoSet.destRsv = focLenScape[motoSet.focIndex+1];
				
			}else if(menuPara.motoLoc<=focLenScape[motoSet.focIndex-1])
			{
				motoSet.destRsv = focLenScape[motoSet.focIndex-1];
			}
			*/
			motoSet.focIndex = GetMotoStdIndex(menuPara.motoLoc);
			motoSet.destRsv = focLenScape[motoSet.focIndex];
			//motoSet.destRsv = focLenScape[GetMotoStdIndex(menuPara.motoLoc)];
			motoSet.autoDest = motoSet.destRsv;
			
		}
	}

	/*
	if(motoSet.destRsv!=*motoSet.dest_location)
	{
		*motoSet.dest_location = motoSet.destRsv;
		for(i=0;i<sizeof(focLenScape);i++)
		{ 
			//if((focLenScape[i]<*motoSet.dest_location)&&(*motoSet.dest_location<=focLenScape[i+1]))
			if((focLenScape[i]<=*motoSet.dest_location)&&(*motoSet.dest_location<focLenScape[i+1]))
			{
				//motoSet.focIndex = i+1;
				motoSet.focIndex = i;
				break;
			}
		}
	}
	*/
	//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,0); 
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}


void AfEventLoop(void)
{
	//if(!pSnrSet[SNRMENU_AF])
	//if(g_cmrLinked>0)
	//static u8 afInRsv=0;
	//if(g_cmrPressed>0)
	if(CMR_LINK_CAN == g_cmrLinked)
	{
		if (CMD_MODE_OFF == *g_cmdMode)
		{
			//if((!pSnrSet[SNRMENU_AF])&&(!g_shutterFlag))
			//AF_OUT = AF_IN;
			if (CFN_ON == g_cfnOption[CFN_AF_ON])
			{
				if (!AF_IN)
				{
					AF_ON();
				}else
				{
					AF_OFF();
				}
			}
	        }
	}

	//if ((COM_STATUE_IDLE == g_comStatue) && (SCK_IN))
	// 佳能sck 中断处理放在主循环中，此次设置状态，会导致
	// 状态机错误
	/*
	if ((COM_STATUE_IDLE==g_canComStatue) && (SCK_IN))
	{
		SetSpiTrans();
		g_cmrLinked = CMR_LINK_CAN;
	}*/
}

void DspUpdateEventLoop(void)
{
	if(g_dspUpdateFlag&DSP_UPDATE_MODE)
	{
		RepaintScr();
		g_dspUpdateFlag ^= DSP_UPDATE_MODE;
	}

	if(g_dspUpdateFlag&DSP_UPDATE_MOTO)
	{
		FoclenRefresh();
		g_dspUpdateFlag^=DSP_UPDATE_MOTO;
		//SET_UPDATE_EVT(DSP_UPDATE_LINK);
	}
/*
	if(g_dspUpdateFlag&DSP_UPDATE_LINK)
	{
		if (!g_cmrLinked)
		{
			flaPara.cmrCmp1 = 0;
			flaPara.cmrCmp2 = 0;
			flaPara.cmrCmpCode = 0;
		}

		if (*g_cmdMode != CMD_MODE_OFF)
		{
			*g_cmdMode = CMD_MODE_OFF;
			*g_lightMode = WORKINGMODE_TTL;
		}

		gWSleepCounter = 0;
		
		g_dspUpdateFlag^=DSP_UPDATE_LINK;

		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
*/
	
}

void DisplayFocLen(u8 motoMode,u16 foc)
{
	if ( MOTO_MODE_AUTO == motoMode)
	{
		fb [8] |= 0x04;                                    // T7
	}else
	{
		fb [8] |= 0x08;                                   // T8
	}

	if (foc == 200)
	{
		fb [8] |= 0xc0;
		foc %= 100;
	}else if( foc >= 100)
	{
		fb [8] |= 0xa0;
		foc %= 100;
	}

	fb[8] |= 0x10;

	fb[9] |= charFeatures[foc/10];
	fb[10] |= charFeatures[foc%10];

	
}

void DspSettingToggle(void)
{
	if (s_togFlag > 0)
	{
		s_togFlag = 0;
		RepaintScr();
	}else
	{
		s_togFlag = 1;
		switch (g_SettingIndex)
		{
			case SETTING_ADDSUB:
				if ((CMD_MODE_OFF==*g_cmdMode)&&(WORKINGMODE_TTL==*g_lightMode))
				{
					fb[1] &= ~0x01;
					fb[4] &= ~0x10;
					fb[2] &= 0x00;
					fb[3] &= 0x00;
					fb[5] &= ~0x08;
					//fb[2] &= 0xfe;
				}else if ((CMD_MODE_OFF==*g_cmdMode)&&(WORKINGMODE_MULTI==*g_lightMode))
				{
					fb[6] &= 0x01;
					fb[7] &= 0x01;
					fb[7] &= ~0x01;                        // T27 
					fb[6] &= ~0x01;                // T28
				}else
				{
					fb[6] &= 0x01;
					fb[7] &= 0x01;
					fb[7] &= ~0x01;                        // T27 
					fb[6] &= ~0x01;                // T28

					fb[1] &= ~0x01;
					fb[4] &= ~0x10;
					fb[2] &= 0x01;
					fb[3] &= 0x00;
				}
			break;

			case SETTING_CH:
				fb[13] &= ~0x08;                                // T37
				fb[11] = 0x00;                        // T38
			break;

			case SETTING_FRQ:
				fb[2] &= 0x00;
				fb[3] &= 0x01;
			break;

			case SETTING_MOTO:
				fb [8] &= ~0x04;                                    // T7
				fb [8] &= ~0x08;                                   // T8
				fb [8] &= 0x03;
				fb [9] &= 0x01;
				fb [10] &= 0x01;
			break;

			case SETTING_MULTI:
				fb[0] &= 0x00;
				fb[1] &= 0x01;
			break;

			case SETTING_TTLFEB:
				fb[5] &= ~0x80;
				fb[2] &= 0x01;
				fb[3] &= 0x00;
			break;

		}
		LcdWriteData(0x00,fb,16);
	}
}

void DspToggle(void)
{
	s_togCnt ++;
	//if(s_togCnt > 300)
	if(s_togCnt > 6)
	{
		s_togCnt = 0;
		if (g_SettingIndex > 0)
		{
			DspSettingToggle();
		}else if (s_togFlag > 0)
		{
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
		if(g_lowbat > 0)
		{
			g_lowbatDsp = !g_lowbatDsp;
		}
	}
}

#define LED_HOLD_TIME 1
void SlaveLedToggle(void)
{
	if ((CMD_MODE_RF == *g_cmdMode)||(CMD_MODE_LIGH_F1== *g_cmdMode)||(CMD_MODE_LIGH_F2== *g_cmdMode))
	{
		s_slaveLedTimer++;
		if (0 == s_slaveLedTimer)
		//if (s_slaveLedTimer < 15)
		{
			AF_ON();
		}else if((0+LED_HOLD_TIME) == s_slaveLedTimer)
		//else if(s_slaveLedTimer < 16)
		{
			AF_OFF();
		}else if (20 == s_slaveLedTimer)
		{
			AF_ON();
		}else if ((20+LED_HOLD_TIME) == s_slaveLedTimer)
		{
			AF_OFF();
		}else if (200 == s_slaveLedTimer)
		{
			AF_ON();
		}else if ((200+LED_HOLD_TIME) == s_slaveLedTimer)
		{
			AF_OFF();
		}else if (1000 <= s_slaveLedTimer)
		{
			s_slaveLedTimer = 0;
			AF_ON();
		}
	}
}

void SlaveFlashEnableCounterClear()
{
	if (g_slaveFlashCounter > 0)
	{
		g_slaveFlashCounter --;
	}
}

void SlaveEventToggle(void)
{
	/*if ((CMD_MODE_RF == *g_cmdMode)||(CMD_MODE_LIGH_F1== *g_cmdMode)||(CMD_MODE_LIGH_F2== *g_cmdMode))
	{
		s_slaveLedTimer++;
		//if (s_slaveLedTimer < 750)
		if (s_slaveLedTimer < 15)
		{
			AF_OFF();
		}//else if(s_slaveLedTimer < 800)
		else if(s_slaveLedTimer < 16)
		{
			AF_ON();
		}else
		{
			s_slaveLedTimer = 0;
		}
	}*/

	if (g_sRcvState > 0)
	{
		g_sRcvState--;
		if (!g_sRcvState)
		{
			RfSetId(mainPara.s_chan,(u8*)g_a7105_id2);
			g_sRcvCnt = 12;
			g_sFlashTig = 0;
			g_rfStatue = RF_STATUES_IDLE;
		}
	}

	if (g_rfLinkTmr > 0)
	{
		g_rfLinkTmr --;
		if (g_rfLinkTmr <= 0)
		{
			RF_LED_READY();
		}
	}
}

/*
void DspToggleEventLoop(void)
{
	u16 crtTime;
	crtTime = get_counter();
	if(crtTime - g_toggleTimer > 10000L)
	{
		g_toggleTimer = crtTime;
		s_togCnt ++;
		if(s_togCnt > 300)
		{
			s_togCnt = 0;
			if (g_SettingIndex > 0)
			{
				DspSettingToggle();
			}else if (s_togFlag > 0)
			{
				SET_UPDATE_EVT(DSP_UPDATE_MODE);
			}
		}
		
	}
}*/

