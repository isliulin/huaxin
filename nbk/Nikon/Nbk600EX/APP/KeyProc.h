#ifndef _KEYPROC_H_
#define _KEYPROC_H_

#include "Golbal.h"
#include "Display.h"
#include "sys.h"	 
#include "stdlib.h"

typedef struct CMP_POINT
{
	char *cmpStr;
	char pDf;
	char pOf;
}CmpPoint;

#define KEY_VARIANCE	8////3

#if(_HARD_WARE_==HARD_WARE_VER1)

#define KEY0_VALUE_MAX 	(0xD2+KEY_VARIANCE)//482//(uint)(479+3)//227//KEY RIGHT//1030    //用不到
#define KEY0_VALUE_MIN 		(0xD2-KEY_VARIANCE)//(uint)(479-3)//.217//988     //用不到

#define KEY1_VALUE_MAX 	(0xAF+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
#define KEY1_VALUE_MIN 		(0xAF-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364

#define KEY2_VALUE_MAX		(0x8D+KEY_VARIANCE)//(uint)(118+3)//379		//KEY LOCK
#define KEY2_VALUE_MIN		(0x8D-KEY_VARIANCE)//(uint)(118-3)//369

#define KEY3_VALUE_MAX 	(0x66+KEY_VARIANCE)//(uint)(347+3)//93//96		//KEY LEFT	//580   //按键2最大
#define KEY3_VALUE_MIN 		(0x66-KEY_VARIANCE)//(uint)(347-3)//89//86		//500   //按键2最小512

#define KEY4_VALUE_MAX 	(0x3A+KEY_VARIANCE)//(uint)(223+3)//480//483		//KEY  MODE		//932   //按键3最大
#define KEY4_VALUE_MIN 		(0x3A-KEY_VARIANCE)//(uint)(223-3)//476//473		//	//800   //按键3最小892

#define KEY5_VALUE_MAX 	(0x1D2+KEY_VARIANCE)//(uint)(127+3)//385//388//720  KEY SEL //按键4最大
#define KEY5_VALUE_MIN 		(0x1D2-KEY_VARIANCE)//(uint)(127-3)//381//378//600   //按键4最小680

#define KEY6_VALUE_MAX 	(0x1AF+KEY_VARIANCE)//(uint)(91+3)//349//352//720   KEY LED//按键4最大
#define KEY6_VALUE_MIN 		(0x1AF-KEY_VARIANCE)//(uint)(91-3)//345//342//600   //按键4最小680

#define KEY7_VALUE_MAX 	(0x18D+KEY_VARIANCE)//(uint)(170+3)
#define KEY7_VALUE_MIN 		(0x18D-KEY_VARIANCE)//(uint)(170-3)

#define KEY8_VALUE_MAX 	(0x166+KEY_VARIANCE)//(uint)(170+3)
#define KEY8_VALUE_MIN 		(0x166-KEY_VARIANCE)//(uint)(170-3)

#elif(_HARD_WARE_==HARD_WARE_VER2)

#define KEY0_VALUE_MAX 	(0xfd+KEY_VARIANCE)//482//(uint)(479+3)//227//KEY RIGHT//1030    //用不到
#define KEY0_VALUE_MIN 		(0xfd-KEY_VARIANCE)//(uint)(479-3)//.217//988     //用不到

#define KEY1_VALUE_MAX 	(0xd1+KEY_VARIANCE)//(0x46+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
#define KEY1_VALUE_MIN 		(0xd1-KEY_VARIANCE)//(0x46-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364

#define KEY2_VALUE_MAX		(0xad+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(118+3)//379		//KEY LOCK
#define KEY2_VALUE_MIN		(0xad-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(118-3)//369

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

#elif(_HARD_WARE_==HARD_WARE_TSER)

#define KEY0_VALUE_MAX 	(0xfd+KEY_VARIANCE)//482//(uint)(479+3)//227//KEY RIGHT//1030    //用不到
#define KEY0_VALUE_MIN 		(0xfd-KEY_VARIANCE)//(uint)(479-3)//.217//988     //用不到

#define KEY1_VALUE_MAX 	(0xd1+KEY_VARIANCE)//(0x46+KEY_VARIANCE)//(uint)(384+3)//129//132//KEY OK    //404   //按键1最大
#define KEY1_VALUE_MIN 		(0xd1-KEY_VARIANCE)//(0x46-KEY_VARIANCE)//(uint)(384-3)//125//122//324   //按键1最小364

#define KEY2_VALUE_MAX		(0xad+KEY_VARIANCE)//(0x1FF+KEY_VARIANCE)//(uint)(118+3)//379		//KEY LOCK
#define KEY2_VALUE_MIN		(0xad-KEY_VARIANCE)//(0x1FF-KEY_VARIANCE)//(uint)(118-3)//369

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
#define SETTING_SYNMODE		1
#define SETTING_TTLFEB			2
#define WLM_SETTING_RATION	3
#define WLM_SETTING_ARR		4
#define SETTING_ARR_PARA  		5
//#define WLM_SETTING_ARRA		4
#define WLM_SETTING_ARRB		5
#define WLM_SETTING_ARRC		6
#define WLM_SETTING_CHAN		7
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


		

//#define SETTING_MOTO			55

#define WL_CHANNEL_UNSURE 	0xFF
#define WL_CHANNEL_ONE	 	0///1
#define WL_CHANNEL_TWO	  	1///2
#define WL_CHANNEL_THREE  		2///3
#define WL_CHANNEL_FOUR		3///4
#define WL_CHANNEL_MAKESURE 	5

#define SYNC_MODE_FRON  		0
//#define SYNC_MODE_REAR  		2
#define SYNC_MODE_REAR  		1
#ifdef _CANON_
//#define SYNC_MODE_FP			1
#define SYNC_MODE_FP			2
#endif

/*
#define SYS_LOCK		{g_LockMusic |= SYS_SET_LOCK;}
#define SYS_RELEASELOCK		{g_LockMusic &= ~SYS_SET_LOCK;}
#define GET_LOCK		(g_LockMusic &SYS_SET_LOCK)

#define SYS_MSC_ON		{g_LockMusic |= SYS_SET_MSC;}
#define SYS_MSC_OFF		{g_LockMusic &= ~SYS_SET_MSC;}
#define GET_MSC			(g_LockMusic &S)
*/

typedef struct MASTER_PARA
{
	char is_on;
	char mode;
	char para;
}MasterPara;


#define LCD_TTL_ON 		LayModeEttl()//{DisplayEepPicture(0,0,29,16,EEPCHAR_PAGE_MODE_TTL,EEPCHAR_CML_MODE_TTL,DSP_NORMAL);}//{DisplayPicture(0,0,29,16,TTL,0);}
#define LCD_MANU_ON	LayModeManu()//{DisplayEepPicture(0,0,15,16,EEPCHAR_PAGE_MODE_MANU,EEPCHAR_CML_MODE_MANU,DSP_NORMAL);}//{DisplayPicture(0,0,19,16,M,0);}
#define LCD_MULTI_ON	LayModeMulti()//{DisplayEepPicture(0,0,34,16,EEPCHAR_PAGE_MODE_MULTI,EEPCHAR_CML_MODE_MULTI,DSP_NORMAL);}//{DisplayPicture(0,0,34,16,RPT,0);}
#define LCD_Gr_ON		LayModeGr()
#define LCD_MASTER_ON	//{DisplayEepPicture(0,0,65,8,EEPCHAR_PAGE_MODE_MASTE,EEPCHAR_CML_MODE_MASTE,DSP_NORMAL);}//{DisplayPicture(0,0,65,8,master,0);}
#define LCD_RMT_ON		//{DisplayEepPicture(0,0,64,8,EEPCHAR_PAGE_MODE_REMORT,EEPCHAR_CML_MODE_REMORT,DSP_NORMAL);}//{DisplayPicture(0,0,64,8,smallremoter,0);}
#define LCD_RMTF1_ON	//{DisplayEepPicture(0,0,104,16,EEPCHAR_PAGE_MODE_F1,EEPCHAR_CML_MODE_F1,DSP_NORMAL);}//{DisplayPicture(0,0,104,16,REMOTER_F1,0);}
#define LCD_RMTF2_ON	//{DisplayEepPicture(0,0,105,16,EEPCHAR_PAGE_MODE_F2,EEPCHAR_CML_MODE_F2,DSP_NORMAL);}//{DisplayPicture(0,0,105,16,REMOTER_F2,0);}
//#define LCD_SYNMODE_RVS(sm)	{if()}
//#define LCD_FEB_RVS		{DisplayEepPicture(1,)}
//#define LCD_MULTIME_RVS(x) 	{LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1),x/10+'0',FONT_TYPE_MAIN,DSP_SELE);LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(1),x%10+'0',FONT_TYPE_MAIN,DSP_SELE);}//{DisplayEepNumber(2,45,x/10,1,DSP_REVERSE);DisplayEepNumber(2,49,x%10,1,DSP_REVERSE);DisplayEepPicture(2,54,25,8,EEPCHAR_PAGE_TIMERS,EEPCHAR_CML_TIMERS,DSP_REVERSE);}
//#define LCD_MULFRQ_RVS(frq)	 {LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(3)+4,frq/100+'0',FONT_TYPE_MAIN,DSP_SELE);LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(4)+4,(frq%100)/10+'0',FONT_TYPE_MAIN,DSP_SELE);LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(5)+4,frq%10+'0',FONT_TYPE_MAIN,DSP_SELE);LCD_ShowString(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(6)+4,"Hz",FONT_TYPE_MAIN,0,DSP_SELE);}//{DisplayEepNumber(2,83,frq/100,1,DSP_REVERSE);DisplayEepNumber(2,87,(frq%100)/10,1,DSP_REVERSE);DisplayEepNumber(2,91,frq%10,1,DSP_REVERSE);DisplayEepPicture(2,95,11,8,EEPCHAR_PAGE_HZ,EEPCHAR_CML_HZ,DSP_REVERSE);}
#define LCD_MULFT_CLEAR	//{DisplayClearFeild(2,45,(106-45),8);}
//#define LCD_FEB_ON		{}
#define LCD_ADD1_ON	{}
#define LCD_MNS1_ON	{}
#define LCD_CLEAR_GR(page)	//{LcdClearFeild(page,0,LAYOUT_PARAAREA_WT,BIG_FONT_HIGHT(1)+1);}			//"+1" clear frame
//#define LCD_CLEAR_GRALL	{LcdClearFeild(LAYOUT_RATIO_A,0,294,MAIN_FONT_HIGHT(1)+1);LcdClearFeild(LAYOUT_RATIO_A,0,294,MAIN_FONT_HIGHT(1)+1);LcdClearFeild(LAYOUT_RATIO_A,0,294,MAIN_FONT_HIGHT(1)+1);}

//#define LCD_LOCK_ON		{DisplayClearFeild(7,0,131,8);DisplayLine(57,132);DisplayEepPicture(7,4,8,8,EEPCHAR_PAGE_small_lock,EEPCHAR_CML_small_lock,DSP_NORMAL);}
//#define LCD_LOCK_ON		{DisplayClearFeild(7,0,131,8);DisplayLine(57,132);DisplayEepPicture(7,4,8,8,EEPCHAR_PAGE_LOCK,EEPCHAR_CML_LOCK,DSP_NORMAL);}

#define LCD_LOCK_OFF	//{DisplayLine(57,132);DisplayEepPicture(7,0,20,8,EEPCHAR_PAGE_ZOOM,EEPCHAR_CML_ZOOM,DSP_NORMAL);DisplayEepPicture(7,57,15,8,EEPCHAR_PAGE_ZHENSEL,EEPCHAR_CML_ZHENSEL,DSP_NORMAL);DisplayEepPicture(7,118,14,8,EEPCHAR_PAGE_ZHENLED,EEPCHAR_CML_ZHENLED,DSP_NORMAL);}
#define LCD_LOCK_ON		//{DisplayClearFeild(7,0,131,8);DisplayLine(57,132);DisplayEepPicture(7,4,8,8,EEPCHAR_PAGE_LOCK,EEPCHAR_CML_LOCK,DSP_NORMAL);}

#define LCD_TTL_OFF			//{DisplayClearFeild(0,0,29,16);}
#define LCD_MANU_OFF		//{DisplayClearFeild(0,0,15,16);}
#define LCD_MULTI_OFF		//{DisplayClearFeild(0,0,34,16);}
#define LCD_MASTRE_OFF		//{DisplayClearFeild(0,0,65,8);}

#define LCD_REAR_OFF	{}
#define LCD_FEB_OFF		LcdClearFeild(LAYOUT_PARA_AREA0,LOCATION_TTLFEB_CML,FONT_FEB_WT,FONT_FEB_HT);//{LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(7)+FONT_CMP_WT+2,FONT_FEB_WT,FONT_FEB_HT);LcdClearFeild(LOCATION_FEBCNT_PAGE,LOCATION_FEBCNT_CML,BIG_FONT_WIDTH(1),BIG_FONT_HIGHT(1));}

#define LCD_MSTCHAN_CLEAR			//{DisplayClearFeild(0,111,20,16);}


#ifdef _CANON_
#define LCD_TTLPARA_CLEAR			LcdClearFeild(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,LAYOUT_CMPFEB_WT,LAYOUT_PARA_WT);//LcdClearFeild(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,MAIN_FONT_WIDTH(6),MAIN_FONT_HIGHT(1));//{DisplayClearFeild(0,47,60,16);}//{DisplayClearFeild(0,50,57,16);/*LCD_MSTCHAN_CLEAR*/}//{DisplayClearFeild(0,30,80,16);/*LCD_MSTCHAN_CLEAR*/}
#else
#define LCD_TTLPARA_CLEAR			//{DisplayClearFeild(0,50,60,16);LCD_MSTCHAN_CLEAR;}
#endif

#define LCD_MANUPARA_CLEAR		/*Clear field at every time when parameter changed*///{LcdClearFeild(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),130,12);}//{DisplayClearFeild(0,40,67,16);/*DisplayClearFeild(3,0,110,16);*//*DisplayClearFeild(2,0,110,24);*/}//{DisplayClearFeild(0,20,50+69,16);}

#define LCD_MULPARA_CLEAR			LCD_MANUPARA_CLEAR//{DisplayClearFeild(0,35,72,16);DisplayClearFeild(2,45,61,8);}//{DisplayClearFeild(0,35,35+69,16);DisplayClearFeild(2,45,61,8);}
#define LCD_MASTERPARA_CLEAR		//{DisplayClearFeild(2,45,61,8);/*clear iso dsp*/DisplayClearFeild(2,0,27,8);}
#define LCD_RMT_CLEAR				//{DisplayClearFeild(1,0,109,32);DisplayClearFeild(0,0,105,16);DisplayClearFeild(2,0,94,16);}

#define LCD_RMTPARA_CLEAR			//{DisplayClearFeild(LOCATION_RMTRCT_PAGE,LOCATION_RMTRCT_CML,73,16);}
#define LOCATION_MSTCMP_COLL 81
#define LOCATION_MSTPARA_COLL 81

#define MULTI_SET_ETIMES(x,y,tm,sel)			MultiDspNum(x, y, tm, sel)//{LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1),x/10+'0',FONT_TYPE_MAIN,SETTING_MULTI==sel?DSP_SELE:DSP_NORMAL);LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(1),x%10+'0',FONT_TYPE_MAIN,SETTING_MULTI==sel?DSP_SELE:DSP_NORMAL);}//{DisplayEepNumber(2,45,x/10,1,DSP_NORMAL);DisplayEepNumber(2,49,x%10,1,DSP_NORMAL);DisplayEepPicture(2,54,25,8,EEPCHAR_PAGE_TIMERS,EEPCHAR_CML_TIMERS,DSP_NORMAL);}//显示闪光次数
#define MULTI_SET_FRQ(x,y,frq,sel)			MultiDspNum(x,y,frq,sel)//{LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(3)+4,frq/100+'0',FONT_TYPE_MAIN,SETTING_FRQ==sel?DSP_SELE:DSP_NORMAL);LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(4)+4,(frq%100)/10+'0',FONT_TYPE_MAIN,SETTING_FRQ==sel?DSP_SELE:DSP_NORMAL);LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(5)+4,frq%10+'0',FONT_TYPE_MAIN,SETTING_FRQ==sel?DSP_SELE:DSP_NORMAL);LCD_ShowString(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(6)+4,"Hz",FONT_TYPE_MAIN,0,SETTING_FRQ==sel?DSP_SELE:DSP_NORMAL);}//{DisplayEepNumber(2,83,frq/100,1,DSP_NORMAL);DisplayEepNumber(2,87,(frq%100)/10,1,DSP_NORMAL);DisplayEepNumber(2,91,frq%10,1,DSP_NORMAL);DisplayEepPicture(2,95,11,8,EEPCHAR_PAGE_HZ,EEPCHAR_CML_HZ,DSP_NORMAL);}
#define MULTI_SET_LINK(x,y)				SetMultiLink(x,y)//{LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(2)+2,'-',FONT_TYPE_MAIN,DSP_NORMAL);}//LCD_ShowChar(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(4)+FONT_TYPE_MAIN,'-',FONT_TYPE_MAIN,DSP_NORMAL);}//{DisplayEepPicture(2,80,2,8,EEPCHAR_PAGE_GANGGANG,EEPCHAR_CML_GANGGANG,0);}

#ifdef _NIKON_
#define LCD_FP_ON(x)				//{if(x<WORKINGMODE_MULTI){DisplayEepPicture(0,g_fpLoc[x],19,16,EEPCHAR_PAGE_SYNFP,EEPCHAR_CML_SYNFP,DSP_NORMAL);}}//{DisplayEepPicture(0,g_fpLoc[mainPara.workMode],19,16,EEPCHAR_PAGE_SYNFP,EEPCHAR_CML_SYNFP,DSP_NORMAL);}
#define LCD_FP_OFF(x)				//{if(x<WORKINGMODE_MULTI){DisplayClearFeild(0,g_fpLoc[x],19,16);}}//{DisplayClearFeild(0,g_fpLoc[mainPara.workMode],19,16);}
#endif

#ifdef _CANON_
#define LCD_FP_ON(x,dsm)			//{if(x<WORKINGMODE_MULTI){DisplayEepPicture(0,g_fpLoc[x],16,16,EEPCHAR_PAGE_CANSYNFP,EEPCHAR_CML_CANSYNFP,dsm);}}//{DisplayEepPicture(0,g_fpLoc[mainPara.workMode],19,16,EEPCHAR_PAGE_SYNFP,EEPCHAR_CML_SYNFP,DSP_NORMAL);}
#define LCD_RC_ON(x,dsm)			//{if(x<WORKINGMODE_MULTI){DisplayEepPicture(0,g_fpLoc[x],16,16,EEPCHAR_PAGE_CANSYNRC,EEPCHAR_CML_CANSYNRC,dsm);}}//{DisplayEepPicture(0,g_fpLoc[mainPara.workMode],19,16,EEPCHAR_PAGE_SYNFP,EEPCHAR_CML_SYNFP,DSP_NORMAL);}
#define LCD_FP_OFF(x)				//{if(x<WORKINGMODE_MULTI){DisplayClearFeild(0,g_fpLoc[x],16,16);}}//{DisplayClearFeild(0,g_fpLoc[mainPara.workMode],19,16);}
#define LCD_RC_OFF(x)				//LCD_FP_OFF(x)
#define LCD_FEB_ON(x)				DspFebIcon(x)//DisplayPicture(LAYOUT_PARA_AREA0,LOCATION_TTLFEB_CML,FONT_FEB_WT,FONT_FEB_HT,font_feb,x)	//{DisplayEepPicture(0,61,13,16,EEPCHAR_PAGE_TTLFEB,EEPCHAR_CML_TTLFEB,x);}
//#define LCD_FEBRSV_ON				{DisplayEepPicture(1,39,9,8,EEPCHAR_PAGE_FEBRSV,EEPCHAR_CML_FEBRSV,DSP_NORMAL);}
#define LCD_FEB_CLEAR				//(DisplayClearFeild(0,61,13,16);)
#define LCD_CMP_ON(dsm)			DspCmpIcon(dsm)//DisplayPicture(LAYOUT_PARA_AREA1,LAYOUT_CMLPART(1),FONT_CMP_WT,FONT_CMP_HT,font_cmp,dsm)//{DisplayEepPicture(0,47,13,16,EEPCHAR_PAGE_TTLCMP,EEPCHAR_CML_TTLCMP,dsm);}
#define LCD_CMP_CLEAR				LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_CMLPART(1),FONT_CMP_WT,FONT_CMP_HT)//{DisplayClearFeild(0,47,13,16);}
//#define RFGR_SELBAR_CLEAR			{LcdClearFeild(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,8+1,BIG_FONT_HIGHT(3)+LINE_SPACING(2)+1);}
#define RFGR_SELBAR_CLEAR			{LcdClearFeild(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT,48);}
#define LCD_PARA_CLEAR				{LcdClearFeild(LAYOUT_PARA_AREA1,0,LCD_W,LCD_H-LAYOUT_PARA_AREA1);}
#endif

//#define DISPLAY_SYN_FP			{if(flaPara.rear_syn&0x30){LCD_FP_ON(mainPara.workMode);}else{LCD_FP_OFF(mainPara.workMode);}}

//#define LCD_MODE_CLEAR {LCD_TTL_OFF LCD_MANU_OFF LCD_MULTI_OFF LCD_MASTRE_OFF}
#define LCD_MODE_CLEAR 	//LayModeClear()//{DisplayClearFeild(0,0,132,16);}

#ifdef _CANON_
#define LCD_MSTALL_ON(rvs) 	{DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_RATIO,26,8,rationBitmap,DSP_NORMAL);DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_GRMOD,16,8,offBitmap,DSP_NORMAL);}//{DisplayEepPicture(3,0,4,8,EEPCHAR_PAGE_ZUOKUOHAO,EEPCHAR_CML_ZUOKUOHAO,rvs);DisplayEepPicture(3,4,9,8,EEPCHAR_PAGE_MASTER_AFU,EEPCHAR_CML_MASTER_AFU,1-rvs);DisplayEepPicture(3,4+9,9,8,EEPCHAR_PAGE_MASTER_BFU,EEPCHAR_CML_MASTER_BFU,1-rvs);DisplayEepPicture(3,4+9+9,9,8,EEPCHAR_PAGE_MASTER_CFU,EEPCHAR_CML_MASTER_CFU,1-rvs);DisplayEepPicture(3,4+9+9+9,4,8,EEPCHAR_PAGE_YOUKUOHAO,EEPCHAR_CML_YOUKUOHAO,rvs);}
#define LCD_MSTAB_ON(rvs)	{DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_RATIO,26,8,rationBitmap,DSP_NORMAL);DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_GRMOD,14,8,abBitMap,DSP_NORMAL);}//LCD_ShowString(LAYOUT_PARA_AREA3, LAYOUT_CMLPART(1), "RATIO A:B" ,FONT_TYPE_RATIO, 1, DSP_NORMAL)//{DisplayEepPicture(3,0,4,8,EEPCHAR_PAGE_ZUOKUOHAO,EEPCHAR_CML_ZUOKUOHAO,rvs);DisplayEepPicture(3,4,9,8,EEPCHAR_PAGE_MASTER_AFU,EEPCHAR_CML_MASTER_AFU,1-rvs);DisplayEepPicture(3,4+9,4,8,EEPCHAR_PAGE_DALIANGDIAN,EEPCHAR_CML_DALIANGDIAN,rvs);DisplayEepPicture(3,4+9+4,9,8,EEPCHAR_PAGE_MASTER_BFU,EEPCHAR_CML_MASTER_BFU,1-rvs);DisplayEepPicture(3,4+9+4+9,4,8,EEPCHAR_PAGE_YOUKUOHAO,EEPCHAR_CML_YOUKUOHAO,rvs);}
#define LCD_MSTRES_ON(rvs)	{DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_RATIO,26,8,rationBitmap,DSP_NORMAL);DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_GRMOD,22,8,a_bcBitMap,DSP_NORMAL);}//LCD_ShowString(LAYOUT_PARA_AREA3, LAYOUT_CMLPART(1), "RATIO A:B C" ,FONT_TYPE_RATIO, 1, DSP_NORMAL)//{DisplayEepPicture(3,0,4,8,EEPCHAR_PAGE_ZUOKUOHAO,EEPCHAR_CML_ZUOKUOHAO,rvs);DisplayEepPicture(3,4,9,8,EEPCHAR_PAGE_MASTER_AFU,EEPCHAR_CML_MASTER_AFU,1-rvs);DisplayEepPicture(3,4+9,4,8,EEPCHAR_PAGE_DALIANGDIAN,EEPCHAR_CML_DALIANGDIAN,rvs);DisplayEepPicture(3,4+9+4,9,8,EEPCHAR_PAGE_MASTER_BFU,EEPCHAR_CML_MASTER_BFU,1-rvs);DisplayEepPicture(3,4+9+4+9,4,8,EEPCHAR_PAGE_DALIANGDIAN,EEPCHAR_CML_DALIANGDIAN,rvs);DisplayEepPicture(3,4+9+4+9+4,9,8,EEPCHAR_PAGE_MASTER_CFU,EEPCHAR_CML_MASTER_CFU,1-rvs);DisplayEepPicture(3,4+9+4+9+4+9,4,8,EEPCHAR_PAGE_YOUKUOHAO,EEPCHAR_CML_YOUKUOHAO,rvs);}
#define LCD_MSTABC_ON(rvs)	{DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_RATIO,26,8,rationBitmap,DSP_NORMAL);DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_GRMOD,22,8,a_b_cBitMap,DSP_NORMAL);}//LCD_ShowString(LAYOUT_PARA_AREA3, LAYOUT_CMLPART(1), "RATIO A:B:C" ,FONT_TYPE_RATIO, 1, DSP_NORMAL)//{DisplayEepPicture(3,0,4,8,EEPCHAR_PAGE_ZUOKUOHAO,EEPCHAR_CML_ZUOKUOHAO,rvs);DisplayEepPicture(3,4,9,8,EEPCHAR_PAGE_MASTER_AFU,EEPCHAR_CML_MASTER_AFU,1-rvs);DisplayEepPicture(3,4+9,4,8,EEPCHAR_PAGE_DALIANGDIAN,EEPCHAR_CML_DALIANGDIAN,rvs);DisplayEepPicture(3,4+9+4,9,8,EEPCHAR_PAGE_MASTER_BFU,EEPCHAR_CML_MASTER_BFU,1-rvs);DisplayEepPicture(3,4+9+4+9,4,8,EEPCHAR_PAGE_DALIANGDIAN,EEPCHAR_CML_DALIANGDIAN,rvs);DisplayEepPicture(3,4+9+4+9+4,9,8,EEPCHAR_PAGE_MASTER_CFU,EEPCHAR_CML_MASTER_CFU,1-rvs);DisplayEepPicture(3,4+9+4+9+4+9,4,8,EEPCHAR_PAGE_YOUKUOHAO,EEPCHAR_CML_YOUKUOHAO,rvs);}
#define LCD_MSTRES_TTL_ON(rvs)	//{DisplayEepPicture(3,0,4,8,EEPCHAR_PAGE_ZUOKUOHAO,EEPCHAR_CML_ZUOKUOHAO,rvs);DisplayEepPicture(3,4,9,8,EEPCHAR_PAGE_MASTER_AFU,EEPCHAR_CML_MASTER_AFU,1-rvs);DisplayEepPicture(3,4+9,4,8,EEPCHAR_PAGE_DALIANGDIAN,EEPCHAR_CML_DALIANGDIAN,rvs);DisplayEepPicture(3,4+9+4,9,8,EEPCHAR_PAGE_MASTER_BFU,EEPCHAR_CML_MASTER_BFU,1-rvs);DisplayEepPicture(3,4+9+4+9,4,8,EEPCHAR_PAGE_BLOCK,EEPCHAR_CML_BLOCK,rvs);DisplayEepPicture(3,4+9+4+9+4,9,8,EEPCHAR_PAGE_MASTER_CFU,EEPCHAR_CML_MASTER_CFU,1-rvs);DisplayEepPicture(3,4+9+4+9+4+9,4,8,EEPCHAR_PAGE_YOUKUOHAO,EEPCHAR_CML_YOUKUOHAO,rvs);}
#define LCD_CANMST_CLEAR	//{DisplayClearFeild(3,0,110,16);DisplayClearFeild(1,110,21,8);LCD_MSTCHAN_CLEAR;LCD_MSTFLA_OFF}
#define LCD_CANMST_ON		DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST,42,8,mstBitMap,DSP_NORMAL);//LCD_ShowString(LAYOUT_PARA_AREA3, LAYOUT_CMLPART(1)+AUX_FONT_WIDTH(11)+4," MASTER ",FONT_TYPE_RATIO,0,DSP_NORMAL)
#define LCD_CANSLA_ON		{DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+1,41,8,slvBitMap,DSP_NORMAL);}
#define LCD_CANSLA_INDIVIDUAL  {DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_SENSOR_CML_EX+SENSOR_DSP_LEN-1,94,8,font_individualSlave,DSP_NORMAL);}
#define LCD_LSMST_ON(x,y)	{DisplayPicture(x,y,42,8,mstBitMap,DSP_NORMAL);}
#define LCD_LSSLA_ON(x,y)	{DisplayPicture(x,y+1,41,8,slvBitMap,DSP_NORMAL);}
#define LCD_LINKSHOT_FLA()     {DisplayPicture(2,2,68,8,font_sLinkShot,DSP_NORMAL);}
#endif

#define LCD_TTLWL_ON	//{DisplayClearFeild(3,0,36,16);DisplayEepPicture(3,0,29,16,EEPCHAR_PAGE_MODE_TTL,EEPCHAR_CML_MODE_TTL,DSP_NORMAL);}
#define LCD_MANUWL_ON	//{DisplayClearFeild(3,0,36,16);DisplayEepPicture(3,0,15,16,EEPCHAR_PAGE_MODE_MANU,EEPCHAR_CML_MODE_MANU,DSP_NORMAL);}
#define LCD_MULTIWL_ON	//{DisplayClearFeild(3,0,36,16);DisplayEepPicture(3,0,34,16,EEPCHAR_PAGE_MODE_MULTI,EEPCHAR_CML_MODE_MULTI,DSP_NORMAL);}

#ifdef _CANON_
#define LCD_TMPA_ON	//{DisplayEepPicture(4, 110, 10, 24, EEPCHAR_PAGE_wendu, EEPCHAR_CML_wendu, 0);}
//#define LCD_MSC_ON		{DisplayEepPicture(6,109,11,8,EEPCHAR_PAGE_MSC,EEPCHAR_CML_MSC,DSP_NORMAL);}
//#define LCD_MSC_OFF		{DisplayEepPicture(6,109,11,8,EEPCHAR_PAGE_NOMSC,EEPCHAR_CML_NOMSC,DSP_NORMAL);}
#define LCD_MSC_ON		//{DisplayEepPicture(5,121,11,8,EEPCHAR_PAGE_MSC,EEPCHAR_CML_MSC,DSP_NORMAL);}
#define LCD_MSC_OFF		//{DisplayEepPicture(5,121,11,8,EEPCHAR_PAGE_NOMSC,EEPCHAR_CML_NOMSC,DSP_NORMAL);}
#define LCD_MSTFLA_ON	//{DisplayEepPicture(2,114,5,8,EEPCHAR_PAGE_CANMSTFLA,EEPCHAR_CML_CANMSTFLA,DSP_NORMAL);}
#define LCD_MSTFLA_OFF	//{DisplayClearFeild(2,114,5,8);}
#else
#define LCD_TMPA_ON	//{DisplayEepPicture(4, 110, 10, 24, EEPCHAR_PAGE_wendu, EEPCHAR_CML_wendu, 0);}
#define LCD_MSC_ON		//{DisplayEepPicture(5,121,11,8,EEPCHAR_PAGE_MSC,EEPCHAR_CML_MSC,DSP_NORMAL);}
#define LCD_MSC_OFF		//{DisplayEepPicture(5,121,11,8,EEPCHAR_PAGE_NOMSC,EEPCHAR_CML_NOMSC,DSP_NORMAL);}
#endif

#define LCD_MMOTO_ON
#define LCD_MMOTO_OFF

#define LCD_PARA_FOCLEN_OFF		//{DisplayClearFeild(LOCATION_FOCLEN_PAGE,0,70,16);}
#define LCD_PARA_APERT_OFF		//{DisplayClearFeild(5,LOCATION_APERT_COLL,30,16);}

#define LCD_TOOLBAR_ON //{/*DisplayLine(57,132);*/	LCD_LOCK_OFF;}
#define LCD_BAT_ON		//{LCDcls();DisplayEepPicture(3,45,42,16,EEPCHAR_PAGE_LOWBAT,EEPCHAR_CML_LOWBAT,DSP_NORMAL);}
#define LCD_CLS_ON		//{DisplayEepPicture(6,121,11,8,EEPCHAR_PAGE_CAMERA,EEPCHAR_CML_CAMERA,0);}
#define LCD_CLS_OFF		//{DisplayClearFeild(6,121,11,8);}
#define LCD_TMPWARN_ON// {LCDcls();DisplayEepPicture(1,50,32,48,EEPCHAR_PAGE_TMPWARN,EEPCHAR_CML_TMPWARN,DSP_NORMAL);}
#define LCD_AF_ON		//{DisplayEepPicture(2,119,9,8,EEPCHAR_PAGE_AF1,EEPCHAR_CML_AF1,DSP_NORMAL);DisplayEepPicture(3,124,5,8,EEPCHAR_PAGE_AF2,EEPCHAR_CML_AF2,DSP_NORMAL);DisplayEepPicture(3,115,7,8,EEPCHAR_PAGE_AF3,EEPCHAR_CML_AF3,DSP_NORMAL);}
#define LCD_AF_OFF		//{DisplayEepPicture(2,119,9,8,EEPCHAR_PAGE_AF1,EEPCHAR_CML_AF1,DSP_NORMAL);DisplayEepPicture(3,124,5,8,EEPCHAR_PAGE_AF2,EEPCHAR_CML_AF2,DSP_NORMAL);DisplayClearFeild(3,115,7,8);}
#define LCD_AF_CLEAR	//{DisplayClearFeild(2,119,9,8);DisplayClearFeild(3,124,5,8);DisplayClearFeild(3,115,7,8);}

#define LOCATION_FOCLEN_PAGE 5
#define LOCATION_FOCLEN_CMN 23

#define LOCATION_RMTRCT_PAGE   3
#define LOCATION_RMTRCT_CML	36//29

//#define LOCATION_MSTPARA_COLL 81
//#define LOCATION_MSTCMP_COLL 81
#define LOCATION_MSTDISPARA_COLL 44

#define LOCATION_APERT_COLL 71
#define LOCATION_ISO_COLL 0

#define LOCATION_SET_COLL 24

#define LOCATION_CHAN_COLL 	107
#define LOCATION_SIZE_ARRSET 	82

#define LOCATION_MSTHEAD_PAGE 1
#define LOCATION_MSTHEAD_COLL 2

#define LOCATION_RMTRCT_PAGE 3
#define LOCATION_RMTRCT_COLL 0

#define LOCATION_RMTRMT_PAGE 1
#define LOCATION_RMTRMT_COLL 1

#define KEY_DIR_LEFT	1
#define KEY_DIR_RIGHT	2


extern void DisplayTtlCmp(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char  rvs);
extern void DisplayMstOtp(u16 page,u16 cml,u8 mod,unsigned char para1,unsigned char para2,unsigned char rvs);
extern void DisplayOutput(u16 pa, u16 cml,u8 mod,uchar otp,uchar otpOffset,uchar dspRsv);
extern void SetMstHead(char *arr);
extern void SetMstChan(char page, char cml,char ch,char rvc,char frame);
extern void DisplayMstSetting(char arr, unsigned char mode, char para1,char para2);
extern void DisplayPicture(uchar page,uchar column,uchar wide,uchar tall,const char *p,uchar rsv);
extern int DecodeUartData(unsigned char id, unsigned char para1, unsigned char para2);
extern void DisplayCanMstHead(unsigned char sel,char mode,WlSet *wlPara,char rvs);
extern void FuncDsp(void);
extern void DisplayRmt(char arr, char chan,char sel);
extern void DisplayFocLen(uchar motoMode,u16 foc,uchar rsv);
extern void CanSetFeb(char feb1,char feb2, unsigned char rsv);
extern void ParaPreserve(Uchar paraLoc,Uchar para);
extern void SeniorMenuProc(void);
extern void LcdClearFeild(u16 x, u16 y, u16 wt, u16 ht);
extern void KeyMenuSw(void);
extern void LCD_Clear(u16 Color);
extern u16 GetKeyAdc(void);
extern void DisplayTtlCmp_classStyle(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char rvs);
//extern void DisplayTtlCmp_canStyle(u16 page,u16 cml,char bias,CmpPoint cp,signed char cmp1,signed char cmp2,char chWt,char dspLen,char dir,char n,unsigned char rvs);
void DisplayTtlCmp_canStyle(u16 page,u16 cml,
								signed char bias,signed char dBias,signed char rBias,
								CmpPoint cp,
								signed char cmp1,signed char cmp2,
								char chWt,char dspLen,
								//char cellLen,
								float cellLen,
								//float chWt,char dspLen,
								char dir,char n,
								unsigned char rvs);
extern void LcdPicFramed(u16 x, u16 y, u16 wide, u16 tall, u8 rsv);
extern void FillArea(u16 x, u16 y, u16 wt, u16 ht, u8 rsv);
extern void BuildFuncMenu(void);
extern void DspMstModIcon(u8 mod);
extern void BkLiteOn(void);
extern void LcdFillBlank(u16 x, u16 y, u16 wt,u16 ht,u8 rsv);
extern void LoadButtons(void);
extern void LayModeClear(void);
extern void LayModeEttl(void);
extern void LayModeManu(void);
extern void LayModeMulti(void);
extern void LayModeGr(void);
extern void RfDevLinkMark(char mod,WlSet *wlPara);
extern void SetTtlScr(void);
extern void SetManuScr(void);
extern void SetMultiScr(void);
extern void SetRfGrScr(void);
extern void ModPasteScr(signed char mode);
extern void AreaScroll(char *sBuf, char wt,char ht,char aw,char dir,char n);
extern void MultiDspNum(u8 pa, u8 cml,int dat, u8 rvs);
extern void SetLinkshotChan(char page,char cml, char ch, char rvs);
extern void LoadLowBatIcon(void);
extern void DspIndividualSlavePara(char rvs);

typedef struct BK_PARAS
{
	u8 bkState;
	u8 bkFlag;
	u8 bkCout;
}BkParas;

extern BkParas bkParas;

typedef struct ACTIONFUC
{
	void *(*pPressFuc)();
	void *(*pHoldFuc)();
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

extern FuncMenu ttlMenu;
extern FuncMenu mMenu;
extern FuncMenu multiMenu;
extern FuncMenu grMenu;

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


