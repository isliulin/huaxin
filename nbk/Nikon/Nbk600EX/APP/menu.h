#ifndef	_Menu_H_
#define _Menu_H_

#define MAXDISPLAYLINE				5          //LCD最大显示行数
#define SNR_FONT_TYPE					FONT_TYPE_MAIN
#define SNR_TITLE_WIDTH				((80*LCD_W)/220)//80
#define SNR_FONT_WIDTH(x)				MAIN_FONT_WIDTH(x)
#define SNR_FONT_HIGHT(x)				MAIN_FONT_HIGHT(x)

#ifdef _320x240_
//#define CLR_SUBMENU_FILD				{LcdClearFeild(SNR_FONT_HIGHT(2),SUBMENU_COLUMN,SNR_TITLE_WIDTH+1,LCD_H-24-SNR_FONT_HIGHT(2));}
#define CLR_SUBMENU_FILD				{LcdClearFeild(SNR_FONT_HIGHT(2),SNR_TITLE_WIDTH+2,SNR_TITLE_WIDTH+50,LCD_H-24-SNR_FONT_HIGHT(2));}
#endif

#ifdef _172x104_
#define CLR_SUBMENU_FILD
#endif

#define MAINMENU_COLUMN 		0
#define SUBMENU_COLUMN		(SNR_TITLE_WIDTH+SNR_TITLE_WIDTH/4)//100

#define	MENU_ITEMS				25//9//8//6//7//8//7//9//10//12
#define   AF_ITEMS					2
#define	SELECT_ITEMS				2
#define   SLEEP_ITEMS				7///4
#define   SOUND_ITEMS				2
#define   STANDBY_ITEMS				4
//#define   AF_ITEMS					4
#define   MFT_ITEMS					2
#define	RST_ITEMS					1

#define SNR_START_CML			9
#define SNR_TCHAR_CML			34
#define SNR_SUB_CML				14

#define CONTRAST_DSP_LEN	159
#define CONTRAST_BAR_LEN	138

//the order of senior menu
//#define SNRMENU_BKLITE		0
#define SNRMENU_STD		0
#define SNRMENU_STBY		1
#define SNRMENU_MODELING	2
#define SNRMENU_FEBCANCEL	3
#define SNRMENU_FEBTURN	4
#define SNRMENU_MODE		5
#define SNRMENU_QUICK		6
#define SNRMENU_TEST		7
#define SNRMENU_AF			8
#define SNRMENU_SCREEN		9
#define SNRMENU_SSTD		10
#define SNRMENU_MSTD		11
#define SNRMENU_EXTPWR	12
#define SNRMENU_CMP_IDEL	13
#define SNRMENU_VOICE		14
#define SNRMENU_ANGLE		15
#define SNRMENU_BKLITE		16
#define SNRMENU_SBK		17

#define SNRMENU_TMEPWARN 	7
//#define SNRMENU_RST		5
#define SNRMENU_VER		8

#define PFN_ID_CTS			0
#define PFN_ID_BKCLR1		1
#define PFN_ID_BKCLR2		2
#define PFN_ID_BKCLR3		3
#define PFN_ID_DETEC		4
#define PFN_ID_CMDM		5
#define PFN_ID_LSSW			6

#define NULL  0

typedef struct SUBMENU
{
	char cnt;
	char *pStr1;
	char *pStr2;
	char *pStr3;
	char *pStr4;	
	//char (*dspSub)(char* gBuf, char para,char rvs);
	char (*dspSub)(char* gBuf, char para);
}SubMenu;

typedef struct MENUITEM                         //结构体名称
{ 	
	//unsigned char 	MenuCount;         //当前层节点数、每层菜单能显示的条目数
	//const char *dspPic;
	//const char 	*displayString;    //指向菜单标题的字符串
	char        (*dspFunc)(char* gBuf, signed char para,char rvs);
	const char *pIcon;
	char picWt;
	char picHt;
	char bias;
	char dn;				//主页面图标下挪位置
	char up;				//细节选项图标上升
	const char 	*displayString;
	const SubMenu *subMenu;
	void (*excFunc)(signed char *para);
    	//char 	(*mFunc)();         //指向当前状态应该执行的功能函数
	//char		(*subFuc)();
	//struct MenuItem 	*childrenMenus;//指向当前菜单的下级菜单
	//struct MenuItem 	*parentMenus;  //指向当前菜单的上级菜单
	//char subCnt;
}MenuItem; 

typedef struct SENIORMENU
{
	MenuItem *munuItem;
	char menuCnt;
	char subSel;
	char userSel;
	char dspStart;
	char subStart;
	//char Option; 
	//struct SnrMenu *cldMenus;
	//struct SnrMenu *prtMenus;
             
}SnrMenu;
extern signed char g_cfnIndex;
extern signed char g_cfnStart;
extern MenuItem const sm_Cfn[];
extern MenuItem const sm_Pfn[];
extern signed char *pfnSet;
//extern SnrMenu snrMenu;
//extern MenuItem MeunouItem;
//extern MenuItem const MainMenu[MENU_ITEMS];
//extern SnrMenu snrMenu;
//extern void SetStByDsp(void);
//extern char g_snrMenuSet[MENU_ITEMS];
//extern MenuItem const BACKNEXTMENU[BACKLIGHT_NEXT_MENU];
extern void DspSnrFucStr(u16 x, u16 y, const char *str, u8 rvs);
extern void	ShowSnrMenu(char start, char pa, char sel ,char sel_coll,MenuItem *menu,char total);

#endif

