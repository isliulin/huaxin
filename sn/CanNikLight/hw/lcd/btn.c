#include"global.h"
#include <stdarg.h>
#include "..\adc\adc.h"
#include "btn.h"
#include "lcd.h"

extern FuncMenu funcMenu;
u8 g_menuId=0;
u8 g_isVoiceOn = 0;

void BtnZoom(void);
void BtnSnrMenu(void);
void BtnAddSub(void);
void BtnMulti(void);
void BtnFrq(void);
void BtnGrSel(void);
void BtnChSet(void);
void BtnSynSet(void);
void BtnTtlFeb(void);

void BtnDspZoom(void);
void BtnDspAddSub(void);
void BtnDspMulti(void);
void BtnDspGrSel(void);
void BtnDspChSet(void);
void BtnDspSynSet(void);
void BtnDspFrq(void);
void BtnDspFeb(void);

const Button mNull = {0,"",0,{NULL,NULL}};
const Button cfn={0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1),{BtnZoom,BtnSnrMenu,BtnDspZoom}};
const Button addSub = {1,"+/-",10,{BtnAddSub,NULL,BtnDspAddSub}};
const Button mMulti = {0,"MULTI",(sizeof("MULTI")-1),{BtnMulti,NULL,BtnDspMulti}};
const Button mGr = {0,"Gr",(sizeof("Gr")-1),{BtnGrSel,NULL,BtnDspGrSel}};
const Button mCh = {0,"CH",(sizeof("CH")-1),{BtnChSet,NULL,BtnDspChSet}};
const Button mSync = {0,"SYNC",(sizeof("SYNC")-1),{BtnSynSet,NULL,BtnDspSynSet}};
const Button mFrq = {0,"Hz",(sizeof("Hz")-1),{BtnFrq,NULL,BtnDspFrq}};
const Button ttlFeb = {0,"FEB",(sizeof("FEB")-1),{BtnTtlFeb,NULL,BtnDspFeb}};

void BtnZoom(void)
{
	if (SETTING_CFN == g_SettingIndex)
	{
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		return;
	}
	
	if(!g_isReflectPulled)
	{
		if(SETTING_MOTO!=g_SettingIndex)
		{
			g_SettingIndex = SETTING_MOTO;
		}else
		{
			g_SettingIndex = 0;
		}
	}else
	{
		g_SettingIndex = 0;
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnSnrMenu(void)
{/*
	if (g_isVoiceOn > 0)
	{
		g_isVoiceOn = 0;
	}else
	{
		g_isVoiceOn = 1;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	*/
	if (SETTING_CFN == g_SettingIndex)
	{
		g_SettingIndex = 0;
	}else
	{
		g_SettingIndex = SETTING_CFN;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnAddSub(void)
{
	if(SETTING_ADDSUB!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_ADDSUB;
	}else
	{
		g_SettingIndex = 0;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnMulti(void)
{
	if(SETTING_MULTI!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_MULTI;
	}else
	{
		g_SettingIndex = 0;
	}
	//MULTI_SET_ETIMES(*multiSet.eTimes, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnFrq(void)
{
	if(SETTING_FRQ!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_FRQ;
	}else
	{
		g_SettingIndex = 0;
	}
	//MULTI_SET_FRQ(*multiSet.frequence, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnGrSel(void)
{
	mainPara.s_array ++;
	if (mainPara.s_array > 2)
	{
		mainPara.s_array = 0;
	}
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnChSet(void)
{
	if (SETTING_CH != g_SettingIndex)
	{
		g_SettingIndex = SETTING_CH;
	}else
	{
		g_SettingIndex = 0;
	}
}

void BtnSynSet(void)
{/*
	if (SETTING_SYNMODE != g_SettingIndex)
	{
		g_SettingIndex = SETTING_SYNMODE;
	}else
	{
		g_SettingIndex = 0;
	}*/
	if (SYNC_MODE_FRON == mainPara.synMode)
	{
		mainPara.synMode = SYNC_MODE_REAR;
	}else
	{
		mainPara.synMode = SYNC_MODE_FRON;
	}
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnTtlFeb(void)
{
	if(SETTING_TTLFEB!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_TTLFEB;
	}else
	{
		g_SettingIndex = 0;
	}
	//MULTI_SET_FRQ(*multiSet.frequence, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
}

void BtnDspZoom(void)
{
	fb [12] |= 0x01;
}


void BtnDspAddSub(void)
{
	fb [12] |= 0x10;
}


void BtnDspMulti(void)
{
	fb [12] |= 0x20;
}

void BtnDspGrSel(void)
{
	fb [12] |= 0x80;
}

void BtnDspChSet(void)
{
	fb [12] |= 0x08;
}

void BtnDspSynSet(void)
{
	fb [12] |= 0x02;
}

void BtnDspFrq(void)
{
	fb [12] |= 0x04;
}

void BtnDspFeb(void)
{
	fb [12] |= 0x40;
}

void AddButtons(const Button *btn1, const Button *btn2, const Button *btn3, const Button *btn4,u8 listCnt)
{
	//unsigned char btnCnt=0;
	funcMenu.funcList[listCnt].button[0] = (Button *)btn1;//(Button *)btn1;
	funcMenu.funcList[listCnt].button[1] = (Button *)btn2;//(Button *)btn2;
	funcMenu.funcList[listCnt].button[2] = (Button *)btn3;//(Button *)btn3;
	funcMenu.funcList[listCnt].button[3] = (Button *)btn4;//(Button *)btn4;
	//funcMenu.listCnt++;
}

void BuildFuncMenu(void)
{
	funcMenu.listCnt = 0;

	if (SETTING_CFN == g_SettingIndex)
	{
		AddButtons(&cfn,&mNull,&mNull,&mNull,funcMenu.listCnt++);
		
	}else
	{
	
		switch(*g_cmdMode)
		{
			case CMD_MODE_OFF:
				switch(*g_lightMode)
				{
					case WORKINGMODE_TTL:
						if(CMR_LINK_NIK==g_cmrLinked)
						{
							AddButtons(&cfn,&addSub,&mNull,&mSync,funcMenu.listCnt++);
						}else
						{
							AddButtons(&cfn,&addSub,&ttlFeb,&mSync,funcMenu.listCnt++);
						}
					break;

					case WORKINGMODE_MANU:
						AddButtons(&cfn,&addSub,&mNull,&mSync,funcMenu.listCnt++);
					break;

					case WORKINGMODE_MULTI:
						AddButtons(&cfn,&addSub,&mMulti,&mFrq,funcMenu.listCnt++);
					break;
				}
			break;

			case CMD_MODE_RF:
				//AddButtons(&cfn,&addSub,&mGr,&mCh,funcMenu.listCnt++);
				if (WORKINGMODE_TTL == *g_lightMode)
				{
					AddButtons(&cfn,&addSub,&mGr,&mCh,funcMenu.listCnt++);
				}else
				{
					AddButtons(&cfn,&mNull,&mGr,&mCh,funcMenu.listCnt++);
				}
			break;

			case CMD_MODE_LIGH_F1:
				AddButtons(&cfn,&addSub,&mNull,&mNull,funcMenu.listCnt++);
			break;

			case CMD_MODE_LIGH_F2:
				AddButtons(&cfn,&addSub,&mNull,&mNull,funcMenu.listCnt++);
			break;
		}
	}
}

void InsertList(FuncList *ls)
{
	
}

void KeyMenuSw(void)
{
	u8 i = 0;
	//Button *btn;
	for(i=0;i<4;i++)
	{
		//btn = funcMenu.funcList[g_menuId].button[i];
		if (NULL != funcMenu.funcList[g_menuId].button[i]->actionFuc.pDspFuc)
		{
			 funcMenu.funcList[g_menuId].button[i]->actionFuc.pDspFuc();
		}
	}
	
	pMenuList = &funcMenu.funcList[g_menuId];
}

void LoadButtons(void)
{
	BuildFuncMenu();
	KeyMenuSw();
}


