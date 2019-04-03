#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "menu.h"
#include "string.h"
#include "../HARDWARE/lcd/Lcd.h"
#include "../HARDWARE/lcd/font.h"

signed char *pSnrSet;
signed char *pfnSet;
signed char g_cfnIndex=0;
signed char g_cfnStart=0;
signed char g_pfnIndex=0;
signed char g_pfnStart=0;
extern signed char *g_cmdMode;
//signed char g_cfnSubSel=0;

//char ContrastDsp(char* gBuf, char para,char rvs);
char ContrastDsp(char* gBuf, signed char para,char rvs);
char ContrastSet(char* gBuf, char para);
void MstModUpdate(signed char* para);
void BkLiteSelect(signed char *para);
void BkLiteSw(signed char *para);

SubMenu subCfn0 = {2,"0: m","1: f",NULL,NULL,NULL};
SubMenu subCfn1 = {2,"0: ON","1: OFF",NULL,NULL,NULL};//CFN3 CFN8 CFN9
SubMenu subCfn2 = {4,"0: *","1: &","2: */&","3: OFF",NULL};
SubMenu subCfn4 = {2,"0: 0>->+","1: ->0>+",NULL,NULL,NULL};
SubMenu subCfn5 = {4,"0: ETTL-I/E-TTL","1: TTL","2: Ext.A","3: Ext.M",NULL};
SubMenu subCfn6 = {2,"0: OFF","1: ON",NULL,NULL,NULL}; //CFN20
SubMenu subCfn7 = {2,"0: 1/32","1: 1/1",NULL,NULL,NULL};
SubMenu subCfn10 = {2,"0: 60min","1: 10min",NULL,NULL,NULL};
SubMenu subCfn11 = {2,"0: 8h","1: 1h",NULL,NULL,NULL};
SubMenu subCfn12 = {2,"0: @+#","1: #",NULL,NULL,NULL};
SubMenu subCfn13 = {2,"0: $+%","1: %",NULL,NULL,NULL};
SubMenu subCfn21 = {3,"0: {@","1: =@","2: }@",NULL,NULL};
SubMenu subCfn22 = {3,"0: 12sec","1: OFF","2: ON",NULL,NULL};
SubMenu subCfn23 = {2,"0: ^/&?","1: &?",NULL,NULL,NULL};

SubMenu subPfn0 = {5,NULL,NULL,NULL,NULL,ContrastSet};
SubMenu subPfn1 = {2,"0: GREEN","1: ORANGE",NULL,NULL,NULL};
SubMenu subPfn4 = {2,"0: ORANGE","1: GREEN",NULL,NULL,NULL};
SubMenu subPfn5 = {2,"0: AUTO","1: OFF",NULL,NULL,NULL};
SubMenu subPfn6 = {3,"0: OFF>[>]","1: OFF>[","2: OFF>]",NULL,NULL};
SubMenu subPfn7 = {2,"0: OFF","1: ON",NULL,NULL,NULL};
//SubMenu subCfn2 = {2,"ON","OFF",NULL,NULL};

//char DspFunCfn11(char index, char sub);
//char DspFunCfn21(char index, char sub);
//char DspFunCfn2(char index, char sub);

#if 0
MenuItem const sm_Cfn0 ={NULL,font_cfn0,49,0,NULL,&subCfn0};
MenuItem const sm_Cfn1 ={NULL,font_cfn1,21,0,NULL,&subCfn1};
MenuItem const sm_Cfn2 ={NULL,font_cfn1,21,0,"MODELING",&subCfn2};
MenuItem const sm_Cfn3 ={NULL,font_cfn3,16,0,"AUTO CANCEL",&subCfn2};
MenuItem const sm_Cfn4 ={NULL,font_cfn3,16,0,NULL,&subCfn4};
MenuItem const sm_Cfn5 ={NULL,NULL,0,0,"MODE",&subCfn5};
MenuItem const sm_Cfn6 ={NULL,font_cfn6,23,0,"MODE",&subCfn6};
MenuItem const sm_Cfn7 ={NULL,font_cfn6,23,0,"TEST",&subCfn7};
MenuItem const sm_Cfn8 ={NULL,font_cfn8,23,0,"AF",&subCfn1};
MenuItem const sm_Cfn9 ={NULL,font_cfn9,19,0,NULL,&subCfn1};
MenuItem const sm_Cfn10 ={NULL,font_cfn10,22,0,NULL,&subCfn10};
MenuItem const sm_Cfn11 ={DspFunCfn11,NULL,0,0,NULL,&subCfn11};
MenuItem const sm_Cfn12 ={NULL,font_cfn12,28,0,NULL,&subCfn12};
MenuItem const sm_Cfn13 ={NULL,font_cmp,20,0,NULL,&subCfn13};
MenuItem const sm_Cfn20 ={NULL,font_cfn20,12,0,NULL,&subCfn6};
MenuItem const sm_Cfn21 ={DspFunCfn21,NULL,0,0,NULL,&subCfn21};
MenuItem const sm_Cfn22 ={NULL,font_cfn22,19,0,NULL,&subCfn22};
MenuItem const sm_Cfn23 ={NULL,font_cfn23,19,0,NULL,&subCfn23};

#else

MenuItem const sm_Cfn[] = 
{
	{NULL,font_cfn0,49,16,0,0,0,NULL,&subCfn0,NULL},
	{NULL,font_cfn1,21,24,0,1,0,NULL,&subCfn1,NULL},
	{NULL,font_cfn2,23,24,0,0,0,"MODELING",&subCfn2,NULL},
	{NULL,font_cfn3,16,24,0,0,0,"AUTO CANCEL",&subCfn1,NULL},
	{NULL,font_cfn3,16,24,0,0,0,NULL,&subCfn4,NULL},
	{NULL,NULL,0,0,0,0,0,"MODE",&subCfn5,NULL},
	{NULL,font_cfn6,23,24,0,0,0,"QUICK",&subCfn6,NULL},
	{NULL,font_cfn6,23,24,0,0,0,"TEST",&subCfn7,NULL},
	{NULL,font_cfn8,19,24,0,0,0,"AF",&subCfn1,NULL},
	{NULL,font_cfn9,19,24,0,0,0,NULL,&subCfn1,NULL},
	{NULL,font_cfn10,22,24,0,1,0,NULL,&subCfn10,NULL},
	//{DspFunCfn11,NULL,0,0,0,NULL,&subCfn11},
	{NULL,font_cfn11,61,24,0,1,0,NULL,&subCfn11,NULL},
	{NULL,font_cfn12,28,24,0,0,0,NULL,&subCfn12,NULL},
	{NULL,font_cmp,20,16,0,0,0,NULL,&subCfn13,NULL},
	{NULL,font_cfn20,12,24,6,0,0,NULL,&subCfn6,NULL},
	{NULL,font_cfn21,109,24,0,0,0,NULL,&subCfn21,NULL},	
	//{NULL,font_cfn22,19,24,0,0,0,NULL,&subCfn22,NULL},
	{NULL,font_cfn22,19,24,0,0,0,NULL,&subCfn22,BkLiteSelect},
	//{NULL,font_cfn23,27,16,0,0,0,NULL,&subCfn23},
	{NULL,font_cfn23,27,24,0,0,0,NULL,&subCfn23,NULL},
};


MenuItem const sm_Pfn[] = 
{
	{ContrastDsp,font_pfn1,16,24,0,0,0,NULL,&subPfn0,NULL},
	//{NULL,font_pfn2,39,24,0,0,0,NULL,&subPfn1,NULL},
	{NULL,font_pfn2,39,24,0,0,0,NULL,&subPfn1,BkLiteSw},
	{NULL,font_pfn3,39,24,0,0,0,NULL,&subPfn1,BkLiteSw},
	{NULL,font_pfn4,39,24,0,0,0,NULL,&subPfn4,BkLiteSw},
	{NULL,font_pfn5,20,24,0,0,0,NULL,&subPfn5,NULL},
	//{MstModUpdate,font_pfn6,21,24,0,0,0,NULL,&subPfn6},
	{NULL,font_pfn6,21,24,0,0,0,NULL,&subPfn6,MstModUpdate},
	{NULL,font_cfn6,23,24,0,0,0,"LINKED SHOT",&subPfn7,NULL},

};

#endif

//char g_snrMenuSet[MENU_ITEMS]={0};
//char g_stBuf[10]={'O','F','F',0,0,0,0,0,0,0};
//char g_stByTime=0;
/*
void SetStByDsp(void)
{
	if(pSnrSet[SNRMENU_STBY]>0)
	{
		g_stBuf[0] = pSnrSet[SNRMENU_STBY] + '0';
		g_stBuf[1] = '0';
		g_stBuf[2] = 'm';
		g_stBuf[3] = 0;
	}else
	{
		g_stBuf[0] = 'O';
		g_stBuf[1] = 'F';
		g_stBuf[2] = 'F';
		g_stBuf[3] = 0;
	}
}*/

/*
char SetSleepTime()
{
	//char *tms=&pSnrSet[SNRMENU_STBY];
	pSnrSet[SNRMENU_STBY]++;
	if(pSnrSet[SNRMENU_STBY]>6)
	{
		pSnrSet[SNRMENU_STBY] = 0;
	}

	SetStByDsp();
	
	return 0;	
}
*/
#define CST_BAR_LEN ((172-14-10-10-4*2)/5)
char ContrastDsp(char* gBuf, signed char para,char rvs)
{
	char bias=10;
	char i=0,j=0;
	char n=0;
	for(i=0;i<5;i++)
	{
		for(j=0;j<CST_BAR_LEN;j++)
		{	
			n = j+bias+i*(CST_BAR_LEN+2);
			if(i<=para)
			{
				if(DSP_NORMAL==rvs)
				{
					gBuf[n]|=0x01;
					gBuf[n+PFN_DSP_LEN]|=0xF0;
				}else
				{
					gBuf[n]&=0xFE;
					gBuf[n+PFN_DSP_LEN]&=0x0F;
				}
			}else
			{
				if(DSP_NORMAL==rvs)
				{
					gBuf[n]|=0x01;
					gBuf[n+PFN_DSP_LEN]|=0x10;
					if((!j)||((CST_BAR_LEN-1)==j))
					{
						gBuf[n]|=0x01;
						gBuf[n+PFN_DSP_LEN]|=0xF0;
					}
				}else
				{
					gBuf[n]&=0xFE;
					gBuf[n+PFN_DSP_LEN]&=0xEF;
					if((!j)||((CST_BAR_LEN-1)==j))
					{
						gBuf[n]&=0xfe;
						gBuf[n+PFN_DSP_LEN]&=0x0F;
					}
				}
			}
		}
	}

	if(DSP_NORMAL==rvs)
	{
		gBuf[PFN_DSP_LEN/2-1] |= 0x3C;
		gBuf[PFN_DSP_LEN/2] |= 0x3C;
	}else
	{
		gBuf[PFN_DSP_LEN/2-1] &= 0xC3;
		gBuf[PFN_DSP_LEN/2] &= 0xC3;
	}
	
	return 1;
}

char ContrastSet(char* gBuf, char para)
{
	//memset(g_paraSetBuf,cn==sel?0xFF:0x00,CONTRAST_DSP_LEN*2);
	//g_paraSetBufIndex = 14;	
	char i=0,j=0;
	AreaSet(3, 6, CONTRAST_DSP_LEN, 8);	
	for(i=0;i<CONTRAST_DSP_LEN;i++)
	{
		LCD_WRITE_BYTE(0X80);
	}
	AreaSet(3+5, 6, CONTRAST_DSP_LEN, 8);
	for(i=0;i<CONTRAST_DSP_LEN;i++)
	{
		LCD_WRITE_BYTE(0X01);
	}
	AreaSet(3, 6, 1, 48);
	for(i=0;i<48/8;i++)
	{
		LCD_WRITE_BYTE(0XFF);
	}
	AreaSet(3, 6+CONTRAST_DSP_LEN-1, 1, 48);
	for(i=0;i<48/8;i++)
	{
		LCD_WRITE_BYTE(0XFF);
	}

	memset(g_paraSetBuf,0x00,CONTRAST_BAR_LEN*4);

	for(i=0;i<5;i++)
	{
		for(j=0;j<26;j++)
		{
			if(i<=para)
			{
				g_paraSetBuf[CONTRAST_BAR_LEN+i*(26+2)+j]|=0x03;
				g_paraSetBuf[CONTRAST_BAR_LEN*2+i*(26+2)+j]|=0xE0;
			}else
			{
				if((j==0)||(j==25))
				{
					g_paraSetBuf[CONTRAST_BAR_LEN+i*(26+2)+j]|=0x03;
					g_paraSetBuf[CONTRAST_BAR_LEN*2+i*(26+2)+j]|=0xE0;
				}else
				{
					g_paraSetBuf[CONTRAST_BAR_LEN+i*(26+2)+j]|=0x02;
					g_paraSetBuf[CONTRAST_BAR_LEN*2+i*(26+2)+j]|=0x20;
				}
			}
		}
	}

	g_paraSetBuf[CONTRAST_BAR_LEN/2-1]|=0x03;
	g_paraSetBuf[CONTRAST_BAR_LEN/2]|=0x03;
	g_paraSetBuf[CONTRAST_BAR_LEN/2-1+CONTRAST_BAR_LEN]|=0xF0;
	g_paraSetBuf[CONTRAST_BAR_LEN/2+CONTRAST_BAR_LEN]|=0xF0;

	i=para*(26+2)+9;
	
	RamLoadPicture((char*)font_ctrSet,8,16,g_paraSetBuf+CONTRAST_BAR_LEN*2+i,CONTRAST_BAR_LEN,DSP_NORMAL);

	LcdCopyToRam(4,17, g_paraSetBuf, CONTRAST_BAR_LEN,32);

	SetContrast(para);

	return NULL;
	
}

//char MstModUpdate(char* gBuf, char para,char rvs)
void MstModUpdate(signed char *para)
{
	if(1==pfnSet[PFN_ID_CMDM])
	{
		if((CMD_MODE_LIGH_MST==*g_cmdMode)||(CMD_MODE_LIGH_SLA==*g_cmdMode))
		{
			*g_cmdMode = CMD_MODE_OFF;
		}
	}else if(2==pfnSet[PFN_ID_CMDM])
	{
		if((CMD_MODE_RF_MST==*g_cmdMode)||(CMD_MODE_RF_SLA==*g_cmdMode))
		{
			*g_cmdMode = CMD_MODE_OFF;
		}
	}
}

void BkLiteSw(signed char *para)
{
	BkLiteOn();
}

void BkLiteSelect(signed char *para)
{
	//char tmp = (*para);
	switch(*para)
	{
		case 0:
			BkLiteOn();
		break;

		case 1:
			BK_TURNOFF(g_isHotProtect);
		break;

		case 2:
			BkLiteOn();
		break;
	}
}




