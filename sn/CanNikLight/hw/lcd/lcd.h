#ifndef _LCD_H_
#define _LCD_H_

#include "type.h"

extern u8 fb[16];

typedef struct ACTIONFUC
{
	void (*pPressFuc)();
	void (*pHoldFuc)();
	void  (*pDspFuc)();
}ActionFuc;

typedef struct BUTTON
{
	unsigned char dspMod;
	char *dspStr;
	unsigned char strLen;
	ActionFuc actionFuc;
	//unsigned char (*func)();
	//void (*func)();
}Button;

typedef struct FUNCLIST
{
	//Button *pBut;
	Button *button[4];
	struct FuncList *mNext;
	//Button button2;
	//Button button3;
	//Button button4;
}FuncList;

typedef struct FUNCMENU
{
	FuncList funcList[4];
	unsigned char listCnt;
	//FuncList funcList2;
	//FuncList funcList3;
	//FuncList funcList4;
}FuncMenu;

void LoadButtons(void);
void LcdWriteCmd(u16 cmd);
void LcdWriteData(u8 memAddr, u8 *data, u8 len);
void LcdInit(void);
void DisplayFocLen(u8 motoMode,u16 foc);

void BeeperOn(u8 cnt, u16 tms);
void BeeperOff(void);

extern FuncList *pMenuList;
extern u8 g_beepCnt;
extern u8 g_beepTms;
extern u8 g_beepEn;

//#define BIAS 0x52        //0b1000 0101 0010 1/3duty 4com
#define BIAS 0x52        //0b1000 0101 0010 1/3duty 4com
#define SYSDIS 0X00      //0b1000 0000 0000 ����ϵͳ������LCDƫѹ������
#define SYSEN 0X02      //0b1000 0000 0010 ��ϵͳ����

#define LCDOFF 0X04    //0b1000 0000 0100 ��LCDƫѹ
#define LCDON 0X06     //0b1000 0000 0110 ��LCDƫѹ
#define XTAL 0x28      //0b1000 0010 1000 �ⲿ��ʱ��
#define RC256 0X30     //0b1000 0011 0000 �ڲ�ʱ��
#define TONEON 0X12    //0b1000 0001 0010 ���������
#define TONEOFF 0X10   //0b1000 0001 0000 �ر��������
#define WDTDIS 0X0A    //0b1000 0000 1010 ��ֹ���Ź�

#define TONE_4K 0x80
#define TONE_2K 0xC0


#endif

