#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "menu.h"
#include "string.h"
#include "../HARDWARE/lcd/Lcd.h"
#include "../HARDWARE/lcd/font.h"

//char g_snrMenuSet[MENU_ITEMS]={0};
char *pSnrSet;
MenuItem MainMenu[MENU_ITEMS];
extern char g_ver[10];
char RePaint(void);
u16 penSetArr[3]={WHITE,BLUE,GREEN};
u16 bkSetArr[3]={BLACK,GRAY,YELLOW};
extern u16 g_bkPwmDuty;
extern u8 g_dspChinese;
extern void DspLogo(void);
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

char BkLightSet(void)
{
	//pSnrSet[] = 
	g_bkPwmDuty = (BK_PWM_ARR/5)*pSnrSet[SNRMENU_BKLITE];
	BkLiteOn();
	return 1;
}

	
void NULLSubs()
{
}

MenuItem code selMenu[SELECT_ITEMS] =	   //系统菜单
{
	{"OFF","关",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"ON","开",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
};

MenuItem code bkLite[2] = 
{
	{"OFF","关",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"ON", "开",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
};

MenuItem code sleepMenu[SLEEP_ITEMS]=
{
	{"OFF","关",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"10min","10min",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"20min","20min",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"30min","30min",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"40min","40min",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"50min","50min",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"60min","60min",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	//{"LONG",NULL,NULL,(struct MenuItem*)MainMenu,0},
	//{"SHORT",NULL,NULL,(struct MenuItem*)MainMenu,0},
	//{"OFF",NULL,NULL,(struct MenuItem*)MainMenu,0},		
};

MenuItem code AfMenu[SELECT_ITEMS]=
{
	{"ON","开",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"OFF","关",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},	
};


MenuItem code verMenu[1]=
{
	{g_ver,"2.2",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
};

MenuItem code aboutMenu[1]=
{
	{"ST-EX-RT",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
};


MenuItem code focLenDsp[2]={
	{"PUL","开",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"FOC","开",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},	
};

MenuItem code penClr[3] = {
	{"WHITE","白",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	//{"BULE","蓝",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"BLUE","蓝",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"GREEN","绿",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},		
};

MenuItem code bkLight[5] = {
	{"100%","100%",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"80%","80%",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"60%","60%",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"40%","40%",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"20%","20%",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
};

MenuItem code bkClr[3] = {
	{"BLACK","黑",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"GRAY","灰",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},
	{"YELLOW","黄",NULL,NULL,NULL,(struct MenuItem*)MainMenu,0},		
};

MenuItem MainMenu[MENU_ITEMS] =      //结构体主菜单
{		
	{"MODELING","造型闪光",NULL,NULL,(struct MenuItem*)selMenu,NULL,SELECT_ITEMS},
	{"PENCLR","字体色",RePaint,NULL,(struct MenuItem*)penClr,NULL,3},
	{"BKCLR","背景",RePaint,NULL,(struct MenuItem*)bkClr,NULL,3},
	{"SOUND","声音",NULL,NULL,(struct MenuItem*)selMenu,NULL,SELECT_ITEMS},
	//{"BKLIGHT","亮度",BkLightSet,NULL,(struct MenuItem*)bkLight,NULL,5},
	{"AF","对焦灯",NULL,NULL,(struct MenuItem*)AfMenu,NULL,SELECT_ITEMS},
	//{"PENCLR","字体色",RePaint,NULL,(struct MenuItem*)penClr,NULL,3},
	//{"BKCLR","背景",RePaint,NULL,(struct MenuItem*)bkClr,NULL,3},
	//{"AF","对焦灯",NULL,NULL,(struct MenuItem*)AfMenu,NULL,SELECT_ITEMS},
	{"BKLIGHT","亮度",BkLightSet,NULL,(struct MenuItem*)bkLight,NULL,5},
	{"STBY","待机",NULL,NULL,(struct MenuItem*)sleepMenu,NULL,SLEEP_ITEMS},
	{"ATSAVE","自动保存",NULL,NULL,(struct MenuItem*)AfMenu,NULL,SELECT_ITEMS},
	//{"TEMP",NULL,NULL,(struct MenuItem*)selMenu,NULL,SELECT_ITEMS},
	//{"VER","版本",DspLogo,NULL,(struct MenuItem*)verMenu,NULL,1},
	{"VER","版本",NULL,NULL,(struct MenuItem*)verMenu,NULL,1},
};


SnrMenu snrMenu = 
{
	MainMenu,MENU_ITEMS,0,0,0,0
};


void DspSnrFucStr(u16 x, u16 y, const char *str, u8 rvs)
{
	u8 cnInterval=0;
	u8 htCmp=0;
	if(g_dspChinese>0)
	{
		cnInterval = 2*x;
		htCmp = 2;
	}
	//LcdClearFeild(SNR_FONT_HIGHT(x)+SNR_DSP_DOWN+cnInterval, y, SNR_TITLE_WIDTH, SNR_FONT_HIGHT(1)+htCmp);
	//LcdDspAtCenter(SNR_FONT_HIGHT(x)+SNR_DSP_DOWN+cnInterval+1, y,str,SNR_FONT_TYPE,rvs,SNR_TITLE_WIDTH,SNR_FONT_WIDTH(strlen(str)-1+g_dspChinese));
	LcdDspSnrString(SNR_FONT_HIGHT(x)+SNR_DSP_DOWN+cnInterval+1, y,str,SNR_FONT_TYPE,rvs,SNR_TITLE_WIDTH,SNR_FONT_WIDTH(strlen(str)-1+g_dspChinese));
	LcdPicFramed(SNR_FONT_HIGHT(x)+SNR_DSP_DOWN+cnInterval, y, SNR_TITLE_WIDTH, SNR_FONT_HIGHT(1)+htCmp,DSP_NORMAL);
}

void	ShowSnrMenu(char start, char pa, char sel ,char sel_coll,MenuItem *menu,char total)
{

	unsigned char i = 0;
	//unsigned char maxmCount; 
	//maxmCount=MENU_ITEMS;//snrMenu.munuItem[0].MenuCount;
	//DisplayEepPicture(0,0,99,16,EEPCHAR_PAGE_SENIOR_LOGO,EEPCHAR_CML_SENIOR_LOGO,0);
	
	if((start+MAXDISPLAYLINE-1)<total)
	{
		for(i=0;i<MAXDISPLAYLINE;i++)
		{
			//DisplayEepSeniorSTR(i+2,sel_coll,snrMenu.munuItem[start+i].displayString,0);
			//DisplayClearFeild(i+2,0,132,8);
			if(!g_dspChinese)
			{
				DspSnrFucStr(i+2+pa,sel_coll,menu[start+i].displayString,DSP_NORMAL);
			}else
			{
				DspSnrFucStr(i+2+pa,sel_coll,menu[start+i].cnMenuString,DSP_NORMAL);
			}
		}
	}else
	{
		for(i=start;i<total;i++)//i<maxmCount
		{
			//DisplayEepSeniorSTR(i-start+2,sel_coll,snrMenu.munuItem[i].displayString,0);
			//DisplayClearFeild(i-start+2,0,132,8);
			if(!g_dspChinese)
			{
				DspSnrFucStr(i-start+2+pa,sel_coll,menu[i].displayString,DSP_NORMAL);
			}else
			{
				DspSnrFucStr(i-start+2+pa,sel_coll,menu[i].cnMenuString,DSP_NORMAL);
			}
		}
	}

	//DisplayEepSeniorSTR(sel-start+2,sel_coll,snrMenu.munuItem[sel].displayString,1);
	if(!g_dspChinese)
	{
		DspSnrFucStr(sel-start+2+pa,sel_coll,menu[sel].displayString,DSP_SELE); 
	}else
	{
		DspSnrFucStr(sel-start+2+pa,sel_coll,menu[sel].cnMenuString,DSP_SELE); 
	}

	
}

#if 0
void	SeniorMenuProc(void)
{		
	unsigned char GetKeyNum;
	unsigned char userSel=0;				//menu items selected
	unsigned char dspStart=0,subStart=0;
	unsigned char subSel=0;				//submenu 's select
	MenuItem *subItems;
	//unsigned char selcoll;
	//unsigned char maxmCount; 
	//unsigned char saveusechoose,savedisstart;
	subSel = g_snrMenuSet[0];	//avoid the select error
	LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	LCD_ShowString(0,LAYOUT_ETTL_X,"C.Fn Menu",SNR_FONT_TYPE, 0, DSP_NORMAL);
	//maxmCount=MENU_ITEMS;//snrManu.munuItem[0].MenuCount;
      ShowMenu(0,0,0,snrMenu.munuItem,MENU_ITEMS);
	subItems = snrMenu.munuItem[0].childrenMenus;
	DspSnrFucStr(0+2,SUBMENU_COLUMN,subItems[g_snrMenuSet[0]].displayString,0); 
	while(g_snrCnt)
   	{	

		GetKeyNum = ReadKey();
		if (GetKeyNum != 0)
		{         	
			BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
			BUZZER_ON(BUZZER_TIME_NORMAL,1);
			gWSleepCounter = 0;
			switch (GetKeyNum)
			{
				case KEY_RIGHT:
				subSel++;
				if(subSel>=(snrMenu.munuItem[userSel].subCnt))
				//if(g_snrMenuSet[userSel]>=(snrMenu.munuItem[userSel].subCnt))
				{
					subSel = 0;
					//g_snrMenuSet[userSel] = 0;
					subStart = 0;
				}
				if(subStart+4<subSel)
				//if(subStart+4<g_snrMenuSet[userSel])
				{
					subStart = subSel;
					//subStart = g_snrMenuSet[userSel];
					CLR_SUBMENU_FILD;
					//DisplayClearFeild(2,SUBMENU_COLUMN,132-SUBMENU_COLUMN,41);
				}
				CLR_SUBMENU_FILD;
				ShowMenu(subStart,subSel,SUBMENU_COLUMN,snrMenu.munuItem[userSel].childrenMenus,snrMenu.munuItem[userSel].subCnt);
				//ShowMenu(subStart,g_snrMenuSet[userSel]++,SUBMENU_COLUMN,snrMenu.munuItem[userSel].childrenMenus,snrMenu.munuItem[userSel].subCnt);
				break;
				
				case KEY_SEL:
				//maxmCount=MENU_ITEMS;//snrManu.munuItem[0].MenuCount;
				userSel++;
				if(userSel==MENU_ITEMS)
				{
					userSel=0;
					dspStart=0;
					DisplayClearFeild(2,0,132,41);
				}
				if(dspStart+4<userSel)
				{
					dspStart = userSel;
					DisplayClearFeild(2,0,132,41);
				}
				CLR_SUBMENU_FILD;
				ShowMenu(dspStart,userSel,MAINMENU_COLUMN,snrMenu.munuItem,MENU_ITEMS);
				subItems = snrMenu.munuItem[userSel].childrenMenus;
				//DisplayEepSeniorSTR(userSel-dspStart+2,SUBMENU_COLUMN,subItems[g_snrMenuSet[userSel]].displayString,0); 
				subSel = g_snrMenuSet[userSel];
				DisplayEepSeniorSTR(userSel-dspStart+2,SUBMENU_COLUMN,subItems[subSel].displayString,0); 
				//DisplayEepSeniorSTR(userSel-dspStart+2,SUBMENU_COLUMN,snrMenu.munuItem[userSel].childrenMenus[subSel].displayString,0); 
				break;
				
				case KEY_OK:
				/*
				if ((snrMenu.munuItem[userSel].Subs) != NULLSubs)
				{
					(*snrMenu.munuItem[userSel].Subs)(); 
				}*/
				CLR_SUBMENU_FILD;
				g_snrMenuSet[userSel] = subSel;
				subItems = snrMenu.munuItem[userSel].childrenMenus;
				//DisplayEepSeniorSTR(userSel-dspStart+2,SUBMENU_COLUMN,snrMenu.munuItem[userSel].childrenMenus[subSel].displayString,0); 
				DisplayEepSeniorSTR(userSel-dspStart+2,SUBMENU_COLUMN,subItems[g_snrMenuSet[userSel]].displayString,0); 
				WriteEeprom(EEP_SETTING_PAGE,0,g_snrMenuSet,MENU_ITEMS);
				if(subItems[subSel].Subs!=NULL)
				{
					(*subItems[subSel].Subs)();
				}
                 		break;
				
				case KEY_LEFT:
				CLR_SUBMENU_FILD;
				subSel = g_snrMenuSet[userSel];
				ShowMenu(dspStart,userSel,MAINMENU_COLUMN,snrMenu.munuItem,MENU_ITEMS);
				DisplayEepSeniorSTR(userSel-dspStart+2,SUBMENU_COLUMN,subItems[g_snrMenuSet[userSel]].displayString,0); 
				
				/*
			        DisplayClearFeild(2,60,70,40);                          
      			      if (snrManu.munuItem[userSel].parentMenus != NULL)
	   	             { 				 
				snrManu.munuItem= snrManu.munuItem[userSel].parentMenus ; 			 
				userSel = saveusechoose; 
				dspStart = savedisstart; 
				selcoll=0;
				}*/
				break;

				case KEY_SENIOR_MENU:
				/*
				if (snrManu.munuItem[userSel].parentMenus != NULL)
	   	             { 				 
				snrManu.munuItem= snrManu.munuItem[userSel].parentMenus ; 			 
				userSel = 0; 
				dspStart = 0; 
				selcoll=0;
				} */
					//goto exit;
				g_snrCnt = 0;
				break;

				default:
					//HaltSec(50);
				break;
			}

		}
		HaltSec(50);
   	}
	
}
#endif

