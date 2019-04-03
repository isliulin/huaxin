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

#ifdef _220x176_
#define CLR_SUBMENU_FILD				{LcdClearFeild(SNR_FONT_HIGHT(2),SUBMENU_COLUMN,SNR_TITLE_WIDTH+1,SNR_FONT_HIGHT(7)+3);}
#endif

#define MAINMENU_COLUMN 		0
#define SUBMENU_COLUMN		(SNR_TITLE_WIDTH+SNR_TITLE_WIDTH/4)//100

#define	MENU_ITEMS				9//8//9//8//6//7//8//7//9//10//12
#define   AF_ITEMS					2
#define	SELECT_ITEMS				2
#define   SLEEP_ITEMS				7///4
#define   SOUND_ITEMS				2
#define   STANDBY_ITEMS				4
//#define   AF_ITEMS					4
#define   MFT_ITEMS					2
#define	RST_ITEMS					1


//the order of senior menu
//#define SNRMENU_BKLITE		0
#define SNRMENU_MODELING	0
#define SNRMENU_PENCLR		1
#define SNRMENU_BKCLR		2
#define SNRMENU_SOUND		3
//#define SNRMENU_BKLITE		2
#define SNRMENU_AF			4
//#define SNRMENU_AF			5
#define SNRMENU_BKLITE		5
#define SNRMENU_STBY		6
#define SNRMENU_ATSAVE		7

#define SNRMENU_TMEPWARN 	8
//#define SNRMENU_RST		5
#define SNRMENU_VER		9

#define NULL  0
typedef struct MENUITEM                         //结构体名称
{ 	
	//unsigned char 	MenuCount;         //当前层节点数、每层菜单能显示的条目数
    	char 	*displayString;    //指向菜单标题的字符串
	char        *cnMenuString;
	char 	(*mFunc)();         //指向当前状态应该执行的功能函数
	char		(*subFuc)();
	struct MenuItem 	*childrenMenus;//指向当前菜单的下级菜单
	struct MenuItem 	*parentMenus;  //指向当前菜单的上级菜单
	char subCnt;
}MenuItem; 

typedef struct SENIORMENU
{
	MenuItem *munuItem;
	char menuCnt;
	signed char subSel;
	char userSel;
	char dspStart;
	char subStart;
	//char Option; 
	//struct SnrMenu *cldMenus;
	//struct SnrMenu *prtMenus;
             
}SnrMenu;

//extern SnrMenu snrMenu;
//extern MenuItem MeunouItem;
//extern MenuItem const MainMenu[MENU_ITEMS];
extern SnrMenu snrMenu;
//extern void SetStByDsp(void);
//extern char g_snrMenuSet[MENU_ITEMS];
//extern MenuItem const BACKNEXTMENU[BACKLIGHT_NEXT_MENU];
extern void DspSnrFucStr(u16 x, u16 y, const char *str, u8 rvs);
extern void	ShowSnrMenu(char start, char pa, char sel ,char sel_coll,MenuItem *menu,char total);

#endif

