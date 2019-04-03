#ifndef __FONT_H
#define __FONT_H 	   
//常用ASCII表
//偏移量32
//ASCII字符集
//偏移量32
//大小:12*6
//extern const unsigned char asc2_1206[95][12];

#include "../APP/Golbal.h"
//#ifndef _220x176_
//#define _220x176_
//#endif

#define FONT_PARA_LEN 150
#define FONT_MENU_LEN 150
#define FONT_CMPFEB_LEN 150
#define FONT_TITLE_LEN 150
#define FONT_SCALE_LEN 150
#define FONT_CFN_LEN 150

typedef struct CAN_FONT
{
	char ch;
	char wt;
	char buf[14];
}CanFont;

typedef struct TITLE_FONT
{
	char ch;
	char wt;
	char buf[24];
}TitleFont;

typedef struct SCALE_FONT
{
	char ch;
	char wt;
	char buf[6];
}ScaleFont;

typedef struct PFN_FONT
{
	char ch;
	char wt;
	const char buf[18];
}PfnFont;

#if 1
typedef struct PFNSUB_FONT
{
	char ch;
	char wt;
	const char buf[38];
}PfnSubFont;
#endif

#ifdef _320x240_
//extern const char asc2_3216[95][64];
//extern const char _16x32Number[][64];
//extern const char asc2_1608B[95][16] ;
#endif

//extern const char asc2_1608[95][16];
//extern const char asc2_2412[95][36];
//extern const unsigned char _16x32Number[][36];


//extern const char REMOTER_F1[208];
extern const char ettl_EBitmap[];
extern const char ettl_TBitmap[];
extern const char ettl_LBitmap[];
extern const char mBitmap[];
extern const char iBitmap[];
extern const char uBitmap[];
extern const char gBitmap[];
extern const char rBitmap[];
extern const char multi_ltBitmap[];
extern const char ext_xBitmap[];
extern const char ext_tBitmap[];
extern const char ext_dotBitmap[];
extern const char ext_aBitmap[];
extern const char rfBitmap[];
extern const char Id_Dbitmap[];
//extern const char liteBitmap[];
extern const char synRear[];
extern const char synFront[];
extern const char synFp[];
extern const char font_cmp[];
extern const char font_feb[];
extern const char f_menuCmp[];
extern const char rationBitmap[];
extern const char offBitmap[];
extern const char abBitMap[];
extern const char a_bcBitMap[];
extern const char a_b_cBitMap[];
extern const char mstBitMap[];
extern const char pfnBitMap[];
extern const char cfnBitMap[];
extern const char liteBitmap[];
extern const char chAuto[];
extern const char font_amoto[];
extern const char font_mmoto[];
extern const char font_zoom[];
extern const char sensor_head[];
extern const char sensor_head_1[];
extern const char sensor_head_unknown[];
extern const char sensor_flaOn[];
extern const char sensor_flaOn_1[];
extern const char sensor_up[];
extern const char sensor_down[];
extern const char sensor_rgb[];
extern const char sensor_mst[];
extern const char sensor_sla[];
extern const char font_mm[];
extern const char font_flaSw[];
extern const ScaleFont scaleFont[];
extern const char scaleNum[];
extern const char font_cfn0[];
extern const char font_cfn1[];
extern const char font_cfn2[];
extern const char font_cfn3[];
extern const char font_cfn6[];
extern const char font_cfn8[];
extern const char font_cfn9[];
extern const char font_cfn10[];
extern const char font_cfn11[];
extern const char font_cfn12[];
extern const char font_cfn20[];
extern const char font_cfn21[];
extern const char font_cfn22[];
extern const char font_cfn23[];
extern const PfnFont pfnFont[];
extern const PfnSubFont pfnSubFont[];
extern const char f_menuBack[];
extern const char font_ctrSet[];
extern const char slvBitMap[];

extern const char font_pfn1[];
extern const char font_pfn2[];
extern const char font_pfn3[];
extern const char font_pfn4[];
extern const char font_pfn5[];
extern const char font_pfn6[];
extern const char fontChSan[];
extern const char fontGood[];
extern const char picArrow[];
extern const char scanRstScale[];
extern const char fontIso[];
extern const char fontIsoIcon[];
extern const char f_cfn[];
extern const char f_pfn[];
extern const char cfnNum1[];
extern const char font_locked[];
extern const char font_rgbWarn[];
extern const char font_motoWarn[];
extern const char font_motoWp[];
extern const char font_motoBlank[];
extern const char font_speak[];
extern const char font_sLinkShot[];
extern const char font_sAuto[];
extern const char font_chdot[];
extern const char font_lowBat[];
extern const char font_window[];
extern const char font_clear[];
extern const char font_cfnClr1[];
extern const char font_cfnClr2[];
extern const char font_pfnClr1[];
extern const char font_wenhao[];
extern const char hot_warn[];
	
//extern const char font_focLen[];
//extern const char febDf[];
//extern const char febOf[];
//extern const char febLab[];
//extern const char head[];
//extern const char mMoto[];
//extern const char aMoto[];
//extern const char speaker[];
extern const char LkDevMark[];
//extern const char mstSw[234];
//extern const char nBitmapDot[] ;
//extern const char nBitmapDot_1[];
//extern const char test_ttl[];
//extern const char test_m[];
//extern const char test_master[];
//extern const char test_rpt[];
//extern const char test_sttl[];
extern const char picScan_1[];
extern const char picScan_2[];
extern const char picScan_3[];
extern const CanFont canFontPara[];
extern const CanFont canFontMenu[];
extern const TitleFont titleFont[];
extern const CanFont canFontCmpFeb[];
extern const char font_amoto[];
extern const char font_individualSlave[];
//extern const char test_rmt[];
#ifdef _220x176_
extern const char asc2_1206[95][12];
extern const char asc2_20x10[95][30];
#endif

#define FONT_16x08		1
#define FONT_24x12		2
#define FONT_32x16		3
#define FONT_16x08B		4
#define FONT_12x06		5
#define FONT_20x10		6

#define FONT_PARA		1
#define FONT_MENU		2

#define PFN_DSP_LEN 158


#define BIG_FONT				1
#define SMALL_FONT				0
#define _DSP_FONT_OPTION_ 		SMALL_FONT


#ifdef _172x104_


#define FONT_TYPE_MAIN			FONT_24x12
#define FONT_TYPE_AUX			FONT_16x08
#define FONT_TYPE_GRHEAD		FONT_24x12
#define FONT_TYPE_BIG			FONT_24x12

#define MAIN_FONT_HIGHT(x)		(24*(x))
#define MAIN_FONT_WIDTH(x)		(12*(x))
#define AUX_FONT_HIGHT(x)		(16*(x))
#define AUX_FONT_WIDTH(x)		(8*(x))
#define FONT_INTERVAL(x)		(12*(x))
#define LINE_SPACING(x)			(8*(x))
//#define BIG_FONT_HIGHT(x)		(24*(x))
//#define BIG_FONT_WIDTH(x)		(12*(x))
#define BIG_FONT_HIGHT(x)		(MAIN_FONT_HIGHT(x))
#define BIG_FONT_WIDTH(x)		(MAIN_FONT_WIDTH(x))
#define LAYOUT_MAST_SPACING(x)	(6*(x))

#define LAYOUT_PARA_AREA0 2
#define LAYOUT_PARA_AREA1  LAYOUT_PARA_AREA0
//#define LAYOUT_PARA_AREA1	(LAYOUT_MODE_HT)
#define LAYOUT_PARA_AREA2	3//3(LAYOUT_PARA_AREA1+0)
#define LAYOUT_PARA_AREA3	4//(LAYOUT_PARA_AREA2+32)

#define LAYOUT_RATIO_A		5//(LAYOUT_PARA_AREA3+24+8)
#define LAYOUT_RATIO_B		(LAYOUT_RATIO_A+2)//(LAYOUT_RATIO_A+24+8)
#define LAYOUT_RATIO_C		(LAYOUT_RATIO_B+2)//(LAYOUT_RATIO_B+24+8)
#define LAYOUT_ROW_MOTO	55//(LAYOUT_MODE_WT+4)

#define LAYOUT_PAGEX_PARA		5

#define LAYOUT_CMLPART(x)	((32*x)-1)//((LCD_W>>3)*x)

#define LAYOUT_PAGE_MOTO	0
#define LAYOUT_MOTO_ADJUST	0
#define FONT_MOTO_WT		13//25
#define FONT_MOTO_HT		16//24
#define LAYOUT_DOTAREA_HT		8

#define FONT_TYPE_MENU		FONT_24x12
#define FONT_TYPE_MAST		FONT_16x08

#define FONT_DOT_SIZE		4
#define FONT_DOT_OFFSET	2

#define FEB_ARROW_SIZE		8

#define LAYOUT_MENU_PAGE	11//(LCD_H-24)
#define LAYOUT_SELBAR_CML	161//295
#define LAYOUT_PARAAREA_WT	295

#define LOCATION_TTLCMP_PAGE	LAYOUT_PARA_AREA1
//#define LOCATION_TTLCMP_CML	(LAYOUT_CMLPART(1)+FONT_CMP_WT+2)//(LAYOUT_PARA_LINE+36+8)
#define LOCATION_TTLCMP_CML 31 
#define LAYOUT_CMPFEB_WT	92
#define PARA_CMPFEB_WT  20

#define LAYOUT_PARA_WT 16

#define LOCATION_TTLFEB_CML     83//72//58

#define LOCATION_FEBCNT_PAGE LOCATION_TTLCMP_PAGE
#define LOCATION_FEBCNT_CML   (LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(7)+FONT_CMP_WT+FONT_FEB_WT+2)

#define LOCATION_SPEAK_CML (LAYOUT_ROW_MOTO+25+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+MAIN_FONT_WIDTH(3)+4+AUX_FONT_WIDTH(2)+4)
#define LOCATION_MSTMODE_CML	(LOCATION_SPEAK_CML+13+8)

#define FONT_SYN_WT	14//24
#define FONT_SYN_HT	16//24

#define FONT_SPEAKER_WT	9
#define FONT_SPEAKER_HT	16

#define LAYOUT_SPEAKER_PAGE	0
#define LAYOUT_SPEAKER_CML	(LAYOUT_MSTMODE_CML-10)

#define FONT_RF_WT			36
#define FONT_RF_HT			36

#define FONT_HEAD_WT		32
#define FONT_HEAD_HT		32

#define DSP_OUTPUT_PAGE 		LAYOUT_RATIO_A
#define LAYOUT_MSTMODE_PAGE	0//8
#define LAYOUT_MSTMODE_CML	152//(LCD_W-36)
#define FONT_MSTMOD_SIZE	19//36
#define LAYOUT_IDSET_CML		75
#define FONT_MSTMOD_HT 16

//#define LAYOUT_CHAN_LEN	27
#define LAYOUT_CHAN_LEN     29
//#define LAYOUT_CHAN_CML	143
#define LAYOUT_CHAN_CML	142

#define LAYOUT_LKDEV_WT	9
#define LAYOUT_LKDEV_HT	16

#define LAYOUT_SENSOR_PAGE 2
#define LAYOUT_SENSOR_CML   2//3

#define LAYOUT_SENSOR_PAGE_EX 3
#define LAYOUT_SENSOR_CML_EX   4

#define DSP_BLOCK_LEN 26  //104 dot one line
#define DSP_2DOT_LEN (4+5+4)
#define DSP_PARA_1LINE (DSP_BLOCK_LEN+DSP_2DOT_LEN+DSP_BLOCK_LEN+DSP_2DOT_LEN+DSP_BLOCK_LEN)
#define DSP_CENTER (12)

#define DSP_BLKEX_LEN 22
#define DSP_2DOTEX_LEN (4+3+4)
#define DSP_PARAMEX_1LINE (DSP_BLKEX_LEN+DSP_2DOTEX_LEN+DSP_BLKEX_LEN+DSP_2DOTEX_LEN+DSP_BLKEX_LEN)


#define DSP_PARAEX_1LINE 86

#define DSP_CMPFEB_LEN	(FONT_CMP_WT+8+PARA_CMPFEB_WT+4+FONT_FEB_WT+2+7)
#define DSP_BIAS_LNK (FONT_CMP_WT+1)
#define DSP_BIAS_FEB (FONT_CMP_WT+8+PARA_CMPFEB_WT+4)

#define LAYOUT_PFN_CML (LAYOUT_PARAEX_COLUMN+DSP_PARAEX_1LINE+2)

#endif


#endif




/*###################ASCII#############################
  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
*/
