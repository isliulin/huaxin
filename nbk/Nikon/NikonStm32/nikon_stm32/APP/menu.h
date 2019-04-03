#ifndef _NIKON_MENU_H
#define _NIKON_MENU_H

#ifndef NULL
#define NULL ((void *)0)
#endif


#define SNR_FRAGMENT_LEN 69
#define SNR_SUBSTR_LEN 54

typedef struct SUBMENU
{
	char cnt;
	//char *pStr1;
	//char *pStr2;
	//char *pStr3;
	//char *pStr4;	
	void (*subDsp)(u8 sel, u8 org, u8 rsvId);
	void (*subSetProc)(u8 sel);
	//char (*dspSub)(char* gBuf, char para,char rvs);
	//char (*dspSub)(char* gBuf, char para);
}SubMenu;


typedef struct MENUITEM                         //结构体名称
{ 	
	//unsigned char 	MenuCount;         //当前层节点数、每层菜单能显示的条目数
	//const char *dspPic;
	//const char 	*displayString;    //指向菜单标题的字符串
	//char        (*dspFunc)(char* gBuf, signed char para,char rvs);
	const char *pIcon;
	char picWt;
	//char picHt;
	//char bias;
	char isMask;
	//char dn;				//主页面图标下挪位置
	//char up;				//细节选项图标上升
	//const char 	*displayString;
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

extern const MenuItem g_snrMenu[SNR_MENU_CNT];


#endif


