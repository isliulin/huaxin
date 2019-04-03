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


typedef struct MENUITEM                         //�ṹ������
{ 	
	//unsigned char 	MenuCount;         //��ǰ��ڵ�����ÿ��˵�����ʾ����Ŀ��
	//const char *dspPic;
	//const char 	*displayString;    //ָ��˵�������ַ���
	//char        (*dspFunc)(char* gBuf, signed char para,char rvs);
	const char *pIcon;
	char picWt;
	//char picHt;
	//char bias;
	char isMask;
	//char dn;				//��ҳ��ͼ����Ųλ��
	//char up;				//ϸ��ѡ��ͼ������
	//const char 	*displayString;
	const SubMenu *subMenu;
	void (*excFunc)(signed char *para);
    	//char 	(*mFunc)();         //ָ��ǰ״̬Ӧ��ִ�еĹ��ܺ���
	//char		(*subFuc)();
	//struct MenuItem 	*childrenMenus;//ָ��ǰ�˵����¼��˵�
	//struct MenuItem 	*parentMenus;  //ָ��ǰ�˵����ϼ��˵�
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


