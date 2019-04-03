#ifndef _KEYPROC_H_
#define _KEYPROC_H_

#define NIKON_STYLE

#include "sys.h"	 
#include "stdlib.h"

#define KEY_VARIANCE	8////3

#define KEY0_VALUE_MAX 	(0xfd+KEY_VARIANCE)//482//(uint)(479+3)//227//KEY RIGHT//1030    //用不到
#define KEY0_VALUE_MIN 		(0xfd-KEY_VARIANCE)//(uint)(479-3)//.217//988     //用不到

#define KEY1_VALUE_MAX 	(0xd1+KEY_VARIANCE)//(0x46+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
#define KEY1_VALUE_MIN 		(0xd1-KEY_VARIANCE)//(0x46-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364

#define KEY2_VALUE_MAX		(0xad+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(118+3)//379		//KEY LOCK
#define KEY2_VALUE_MIN		(0xad-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(118-3)//369

#ifdef NIKON_STYLE

#define KEY3_VALUE_MAX 	(0x8c+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(347+3)//93//96		//KEY LEFT	//580   //按键2最大
#define KEY3_VALUE_MIN 		(0x8c-KEY_VARIANCE)

#define KEY4_VALUE_MAX 	(0x66+KEY_VARIANCE)//(uint)(223+3)//480//483		//KEY  MODE		//932   //按键3最大
#define KEY4_VALUE_MIN 		(0x66-KEY_VARIANCE)

#define KEY5_VALUE_MAX 	(0x21+KEY_VARIANCE)//(uint)(127+3)//385//388//720  KEY SEL //按键4最大
#define KEY5_VALUE_MIN 		(0x21-KEY_VARIANCE)

#define KEY6_VALUE_MAX 	(0x45+KEY_VARIANCE)//(uint)(91+3)//349//352//720   KEY LED//按键4最大
#define KEY6_VALUE_MIN 		(0x45-KEY_VARIANCE)//(uint)(91-3)//345//342//600   //按键4最小680

#define KEY7_VALUE_MAX        (0xC5+KEY_VARIANCE)
#define KEY7_VALUE_MIN        (0xC5-KEY_VARIANCE)  //LOCK

#define KEY8_VALUE_MAX        (0xBC+KEY_VARIANCE)
#define KEY8_VALUE_MIN        (0xBC-KEY_VARIANCE)  //CLEAR

#else

#define KEY3_VALUE_MAX 	(0x8c+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(347+3)//93//96		//KEY LEFT	//580   //按键2最大
#define KEY3_VALUE_MIN 		(0x8c-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(347-3)//89//86		//500   //按键2最小512

#define KEY4_VALUE_MAX 	(0x66+KEY_VARIANCE)//(uint)(223+3)//480//483		//KEY  MODE		//932   //按键3最大
#define KEY4_VALUE_MIN 		(0x66-KEY_VARIANCE)//(uint)(223-3)//476//473		//	//800   //按键3最小892

#define KEY5_VALUE_MAX 	(0x45+KEY_VARIANCE)//(uint)(127+3)//385//388//720  KEY SEL //按键4最大
#define KEY5_VALUE_MIN 		(0x45-KEY_VARIANCE)//(uint)(127-3)//381//378//600   //按键4最小680

#define KEY6_VALUE_MAX 	(0x1FE+KEY_VARIANCE)//(uint)(91+3)//349//352//720   KEY LED//按键4最大
#define KEY6_VALUE_MIN 		(0x1FE-KEY_VARIANCE)//(uint)(91-3)//345//342//600   //按键4最小680

#define KEY7_VALUE_MAX 	(0x20+KEY_VARIANCE)//(uint)(170+3)
#define KEY7_VALUE_MIN 		(0x20-KEY_VARIANCE)//(uint)(170-3)

#define KEY8_VALUE_MAX 	(0x1AF+KEY_VARIANCE)//(uint)(170+3)
#define KEY8_VALUE_MIN 		(0x1AF-KEY_VARIANCE)//(uint)(170-3)
#endif

#ifdef _172x104_

#define FONT_TYPE_RATIO	FONT_12x06

#define LAYOUT_MODE_WT	54//75
#define LAYOUT_MODE_HT		16//32
#define LAYOUT_ETTL_X		5
#define LAYOUT_HEAD_X		(5+16)
#define LAYOUT_MANU_X		19
#define LAYOUT_MULTI_X		2
#define LAYOUT_Gr_X			17
#define LAYOUT_ID_CML		47
#define LAYOUT_IDNUM_CML		(LAYOUT_ID_CML+7+11+11)

#define LAY_WT_NOR		16
#define LAY_WT_M			15
#define LAY_WT_I			7

#define LAY_HT	16

#define LAYOUT_RATIO_COLUMN	(3)
#define LAYOUT_RATIO_WIDTH	52//60//72//70
#define LAYOUT_RATIO_STPARA	(55+1)//(LAYOUT_RATIO_WIDTH+24)
#define LAYOUT_GRAE_WIDTH		15
#define LAYOUT_GRMOD_WIDTH     35
#define LAYOUT_GRMOD_COLUMN   20

#define LAYOUT_PARAEX_COLUMN  37

#define LAYOUT_PARA_LINE			48
#define LAYOUT_SYNMODE_LINE		109//146//130

#define LAYOUT_BUTTON_WIDTH		(172/4)//54
#define LAYOUT_BUTTON_HIGHT		16
#define LAYOUT_BUTTON_INTERVAL	1

#define LAYOUT_FUNC_RATIO	31
#define LAYOUT_FUNC_MST	83//84
#define LAYOUT_FUNC_GRMOD 59		

#define FONT_BUTTON_WT			8


#define FONT_CMP_WT	20//24			
#define FONT_CMP_HT	16

#define FONT_FEB_WT	13//16			
#define FONT_FEB_HT		16

#define LAYOUT_FEB_OFFSET		40
#endif


#define KEY_RF_SW	1
#define KEY_FUNC1	2
#define KEY_FUNC2	3
#define KEY_FUNC3	4
#define KEY_MENU_SW		5
#define KEY_MODE	6
#define KEY_LEFT		7
#define KEY_OK		8
#define KEY_RIGHT	9
#define KEY_CLEAR	10
#define KEY_LOCK	11
#define KEY_PILOT	12
#define KEY_MENU      13
#define KEY_WAKE	14

#define KEY_RF_SW_LONG		(0x10|KEY_RF_SW)
#define KEY_FUNC1_LONG		(0x10|KEY_FUNC1)
#define KEY_FUNC2_LONG		(0x10|KEY_FUNC2)
#define KEY_FUNC3_LONG		(0x10|KEY_FUNC3)
#define KEY_SELE_LONG		(0x10|KEY_MENU_SW)
#define KEY_MODE_LONG		(0x10|KEY_MODE)
#define KEY_LEFT_LONG		(0x10|KEY_LEFT)
#define KEY_OK_LONG			(0x10|KEY_OK)
#define KEY_RIGHT_LONG		(0x10|KEY_RIGHT)
#define KEY_SENIOR_MENU	(0x10|KEY_FUNC1_LONG)
#define KEY_CLEAR_LONG		(0x10|KEY_CLEAR)
#define KEY_MENU_LONG          (0x10|KEY_MENU)
#define KEY_LOCK_LONG          (0x10|KEY_LOCK)

#define KEY_DIR_LEFT	1
#define KEY_DIR_RIGHT	2
#define KEY_ERROR		3

#define sample_time 10// 采样时间
#define LONG_KEYPRESS_TIME 20

#define WLM_ON 					1
#define WLM_OFF 				0

#define WLM_MSTFLA_ON			1
#define WLM_MSTFLA_OFF		0

#ifdef _NIKON_
#define WLM_ARR_MASTER		0
#define WLM_ARR_A				1
#define WLM_ARR_B				2
#define WLM_ARR_C				3
#endif

#define DSP_NORMAL				0
#define DSP_REVERSE				1
//#define DSP_SELE				2
#define DSP_SELE				DSP_REVERSE
#define DSP_SUB_SELE			2
#define DSP_SUBADD				4


#define MOTO_ENABLE			0X01

#define MOTO_DIR_FORWARD 0x00
#define MOTO_DIR_BACKWARD 0X01

#define MOTO_READY       0X01
#define MOTO_MODE_AUTO      0x00
#define MOTO_MODE_MANU      0x01


#define SYS_LOCK	7//0x80
#define SYS_MSC		6//0X40


#define SETTING_IDLE			0
#define SETTING_MODE     		1
#define SETTING_TTLFEB			2
//#define WLM_SETTING_RATION	3
//#define WLM_SETTING_ARR		4
//#define SETTING_ARR_PARA  		5
//#define WLM_SETTING_ARRA		4
//#define WLM_SETTING_ARRB		5
//#define WLM_SETTING_ARRC		6
#define SETTING_MST_M			3
#define SETTING_MST_A                  4
#define SETTING_MST_B                  5
#define SETTING_MST_C                  6
#define SETTING_MST_CHAN		7
#define SETTING_MULTI			8
#define SETTING_FRQ				9
#define SETTING_RFGR_SEL		10
//#define SETTING_ADDSUB			0
#define SETTING_MSTSW			11
#define SETTING_ID_SEL			12		//ModeNoExit
#define SETTING_SNRMENU		13		//ModeNoExit
#define SETTING_SNRSUB			15		//ModeNoExit
#define SETTING_ADDSUB			14
#define SETTING_PFNMENU		16		//ModeNoExit
#define SETTING_PFNSUB			17		//ModeNoExit
#define SETTING_GR_RES			18
#define SETTING_SCAN_SET		19		//ModeNoExit
#define SETTING_EXTM_ISO		20
#define SETTING_EXTM_APERT		21
#define SETTING_ID_SET			22		//ModeNoExit
#define SETTING_RFSCAN			23		//ModeNoExit
#define SETTING_MOTO			24
#ifdef FIBER_ADJUST_PROC
#define SETTING_FB_EXP			25
#define SETTING_FB_ADJ			26
#define SETTING_FIBER			27
#endif
#define SETTING_MYMENU			28
#define SETTING_GR_M			29
#define SETTING_RMT_GR                30
#define SETTING_RMT_CH		       31
#define SETTING_RPT_SET               32

#define MST_SET_ARR			0
#define MST_SET_MODE             1
#define MST_SET_PARA              2

typedef struct ACTIONFUC
{
	void *(*pPressFuc)();
	void *(*pReleaseFun)();
	void *(*pHoldFuc)();
}ActionFuc;

typedef struct BUTTON
{
	//unsigned char dspMod;
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

typedef struct BK_PARAS
{
	u8 bkState;
	u8 bkFlag;
	u8 bkCout;
}BkParas;
		

//#define SETTING_MOTO			55

#define SYNC_MODE_FRON  		0
//#define SYNC_MODE_REAR  		2
#define SYNC_MODE_REAR  		1
#ifdef _CANON_
//#define SYNC_MODE_FP			1
#define SYNC_MODE_FP			2
#endif

/*
闪光灯指数Guide Numbe
基本公式：闪光灯指数=闪光灯距被摄物距离x拍摄光圈（GN=DxF）
衍生公式：闪光灯与被摄物应有距离=闪光灯指数/拍摄光圈（D=GN/F）
拍摄光圈=闪光灯指数/闪光灯与被摄物距离（F=GN/D）
不同ISO的换算

因为公式计算是以ISO100的前提下进行，所以如果使用的感光度不是ISO100，则需进行换算：
闪光灯变动指数 = 原始闪光灯指数 ×（（新iso感光值/100）^0.5））；
　　注：（新iso感光值/100）^0.5意思是（新iso感光值/100）的平方根；
*/
#endif


