#ifndef __FONT_H
#define __FONT_H 	   
//常用ASCII表
//偏移量32
//ASCII字符集
//偏移量32
//大小:12*6
//extern const unsigned char asc2_1206[95][12];

#include "../user/Golbal.h"
//#ifndef _220x176_
//#define _220x176_
//#endif
#ifdef _320x240_
extern const char asc2_3216[95][64];
extern const char _16x32Number[][64];
extern const char asc2_1608B[95][16] ;
#endif

#define CN_CHAR_TOTAL 85
extern const char asc2_1608[95][16];
extern const char asc2_2412[95][36];
extern const char cn_1608_index[CN_CHAR_TOTAL][2];
extern const char cn_1608[CN_CHAR_TOTAL][32];
//extern const unsigned char _16x32Number[][36];
#define FONT_CHINESE_SIZE 32

#ifdef _TUHAO_START_
extern const char tuhao_1[640];
extern const char tuhao_2[282];
extern const char tuhao_3[160];
extern const char tuhao_4[84];
#else
#ifdef _SN_DEBUG_
extern const char Sn_logo[16*67+8];
extern const char Sn_logo_wz[30*16];
#else
extern const char samhero_logo[600];
extern const char samhero_char[800];
#endif
#endif

extern const char REMOTER_F1[208];
extern const char ettl_EBitmap[];
extern const char ettl_TBitmap[];
extern const char ettl_LBitmap[];
extern const char mBitmap[];
extern const char iBitmap[];
extern const char uBitmap[];
extern const char gBitmap[];
extern const char rBitmap[];
extern const char rfBitmap[];
extern const char liteBitmap[];
extern const char synRear[];
extern const char synFront[];
extern const char synFp[];
extern const char cmp[];
extern const char feb[];
extern const char febDf[];
extern const char febOf[];
extern const char febLab[];
extern const char head[];
extern const char mMoto[];
extern const char aMoto[];
extern const char speaker[];
extern const char LkDevMark[];
extern const char mstSw[];
extern const char chScArrow[];
extern const char picScan_1[];
extern const char picScan_2[];
extern const char picScan_3[];

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


#define BIG_FONT				1
#define SMALL_FONT				0
#define _DSP_FONT_OPTION_ 		SMALL_FONT


#ifdef _320x240_
#if(_DSP_FONT_OPTION_==BIG_FONT)
#define FONT_TYPE_MAIN			FONT_24x12
#define FONT_TYPE_AUX			FONT_16x08
#define FONT_TYPE_GRHEAD		FONT_24x12
#define FONT_TYPE_BIG			FONT_24x12

#define MAIN_FONT_HIGHT(x)		(32*(x))
#define MAIN_FONT_WIDTH(x)		(16*(x))
#define AUX_FONT_HIGHT(x)		(16*(x))
#define AUX_FONT_WIDTH(x)		(8*(x))
//#define BIG_FONT_HIGHT(x)		(24*(x))
#define BIG_FONT_WIDTH(x)		(12*(x))
#define FONT_INTERVAL(x)		(12*(x))
#define LINE_SPACING(x)			(8*(x))
#define LAYOUT_MAST_SPACING(x)	(6*(x))

#define BIG_FONT_HIGHT(x)		(MAIN_FONT_HIGHT(x))
#define BIG_FONT_WIDTH(x)		(MAIN_FONT_WIDTH(x))

#define LAYOUT_PARA_AREA1	(LAYOUT_MODE_HT)
#define LAYOUT_PARA_AREA2	(LAYOUT_PARA_AREA1+16)

#define LAYOUT_PARA_AREA3	(LAYOUT_PARA_AREA2+16)

#define LAYOUT_RATIO_A		(LAYOUT_PARA_AREA3+16)
#define LAYOUT_RATIO_B		(LAYOUT_RATIO_A+32+8)
#define LAYOUT_RATIO_C		(LAYOUT_RATIO_B+32+8)
#define LAYOUT_ROW_MOTO	(120)
#define LAYOUT_PAGE_MOTO	8
#define LAYOUT_MOTO_ADJUST	8
#define FONT_MOTO_WT		25
#define FONT_MOTO_HT		24

#define FONT_DOT_SIZE		4
#define FONT_DOT_OFFSET	2

#define LAYOUT_DOTAREA_HT		8

#define LAYOUT_MENU_PAGE	(LCD_H-24)
#define LAYOUT_SELBAR_CML	295

#define LAYOUT_PARAAREA_WT	295

#define LOCATION_TTLCMP_PAGE	LAYOUT_PARA_AREA1
#define LOCATION_TTLCMP_CML	(LAYOUT_PARA_LINE+36+8)

#define LOCATION_SPEAK_CML (LAYOUT_ROW_MOTO+25+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+MAIN_FONT_WIDTH(3)+4+AUX_FONT_WIDTH(2)+4)
#define LOCATION_MSTMODE_CML	(LOCATION_SPEAK_CML+13+8)

#define FONT_TYPE_MENU		FONT_24x12
#define FONT_TYPE_MAST		FONT_16x08

#elif(_DSP_FONT_OPTION_==SMALL_FONT)

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

#define LAYOUT_PARA_AREA1	(LAYOUT_MODE_HT)
#define LAYOUT_PARA_AREA2	(LAYOUT_PARA_AREA1+0)
#define LAYOUT_PARA_AREA3	(LAYOUT_PARA_AREA2+32)

#define LAYOUT_RATIO_A		(LAYOUT_PARA_AREA3+24+8)
#define LAYOUT_RATIO_B		(LAYOUT_RATIO_A+24+8)
#define LAYOUT_RATIO_C		(LAYOUT_RATIO_B+24+8)
#define LAYOUT_ROW_MOTO	(LAYOUT_MODE_WT+4)

#define LAYOUT_CMLPART(x)	((LCD_W>>3)*x)

#define LAYOUT_PAGE_MOTO	16
#define LAYOUT_MOTO_ADJUST	8
#define FONT_MOTO_WT		25
#define FONT_MOTO_HT		24
#define LAYOUT_DOTAREA_HT		8

#define FONT_TYPE_MENU		FONT_24x12
#define FONT_TYPE_MAST		FONT_16x08

#define FONT_DOT_SIZE		4
#define FONT_DOT_OFFSET	2

#define FEB_ARROW_SIZE		8

#define LAYOUT_MENU_PAGE	(LCD_H-24)
#define LAYOUT_SELBAR_CML	295
#define LAYOUT_PARAAREA_WT	295

#define LOCATION_TTLCMP_PAGE	LAYOUT_PARA_AREA1
#define LOCATION_TTLCMP_CML	(LAYOUT_CMLPART(1)+FONT_CMP_WT+2)//(LAYOUT_PARA_LINE+36+8)

#define LOCATION_FEBCNT_PAGE LOCATION_TTLCMP_PAGE
#define LOCATION_FEBCNT_CML   (LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(7)+FONT_CMP_WT+FONT_FEB_WT+2)

#define LOCATION_SPEAK_CML (LAYOUT_ROW_MOTO+25+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+MAIN_FONT_WIDTH(3)+4+AUX_FONT_WIDTH(2)+4)
#define LOCATION_MSTMODE_CML	(LOCATION_SPEAK_CML+13+8)

#define FONT_SYN_WT	24
#define FONT_SYN_HT	24

#define FONT_SPEAKER_WT	13
#define FONT_SPEAKER_HT	24

#define LAYOUT_SPEAKER_PAGE	8
#define LAYOUT_SPEAKER_CML	265

#define FONT_RF_WT			36
#define FONT_RF_HT			36

#define FONT_HEAD_WT		32
#define FONT_HEAD_HT		32

#define DSP_OUTPUT_PAGE 		LAYOUT_RATIO_A
#define LAYOUT_MSTMODE_PAGE	0//8
#define LAYOUT_MSTMODE_CML	(LCD_W-36)
#define FONT_MSTMOD_SIZE	36
#define LAYOUT_IDSET_CML		75

#define LAYOUT_LKDEV_WT	16
#define LAYOUT_LKDEV_HT	24

#define LAYOUT_LKDEV_VER	0

#define LAYOUT_SCANRST_X    (LCD_H-36-48)
#define LAYOUT_SCANRST_Y    (25)

#define SCANRST_DOT_WT	10
#define SCANRST_DOT_HT		6
#define SCANRST_DOT_VAL	2
#define SCANRST_BAR_VAL	6

#define SCANRST_ARROW_HT	16
#define SCANRST_ARROW_WT	16

#endif
#endif

#ifdef _220x176_

#define FONT_TYPE_MAIN			FONT_16x08
#define FONT_TYPE_AUX			FONT_12x06
#define FONT_TYPE_MENU			FONT_16x08
#define FONT_TYPE_MAST			FONT_12x06
#define FONT_TYPE_GRHEAD		FONT_20x10
#define FONT_TYPE_BIG			FONT_20x10

#define MAIN_FONT_HIGHT(x)		(16*(x))
#define MAIN_FONT_WIDTH(x)		(8*(x))
#define AUX_FONT_HIGHT(x)		(12*(x))
#define AUX_FONT_WIDTH(x)		(6*(x))
#define FONT_INTERVAL(x)		(8*(x))
#define LINE_SPACING(x)			(4*(x))
#define BIG_FONT_HIGHT(x)		(20*(x))
#define BIG_FONT_WIDTH(x)		(10*(x))
#define LAYOUT_MAST_SPACING(x) 	(3*(x))

#define LAYOUT_DOTAREA_HT		8

#define LAYOUT_PARA_AREA1	(LAYOUT_MODE_HT+4)
#define LAYOUT_PARA_AREA2	(LAYOUT_PARA_AREA1+8)
#define LAYOUT_PARA_AREA3	(LAYOUT_PARA_AREA2+16)
#define LAYOUT_MENU_PAGE	(176-16)

#define LAYOUT_RATIO_A		(LAYOUT_PARA_AREA3+20)
#define LAYOUT_RATIO_B		(LAYOUT_RATIO_A+20+4)
#define LAYOUT_RATIO_C		(LAYOUT_RATIO_B+20+4)
#define LAYOUT_ROW_MOTO	(LAYOUT_MODE_WT+2)
#define LAYOUT_PAGE_MOTO	8
#define LAYOUT_MOTO_ADJUST	4

#define LAYOUT_GR_A		(LAYOUT_RATIO_A-20-4-20-4+4)
#define LAYOUT_GR_B			(LAYOUT_GR_A+20+4)
#define LAYOUT_GR_C			(LAYOUT_GR_B+20+4)
#define LAYOUT_GR_D		(LAYOUT_GR_C+20+4)
#define LAYOUT_GR_E			(LAYOUT_GR_D+20+4)

#define LOCATION_TTLCMP_PAGE	LAYOUT_PARA_AREA1
#define LOCATION_TTLCMP_CML	(LAYOUT_CMLPART(1)+FONT_CMP_WT+2)

#define LAYOUT_MULTIME_PAGE	(LAYOUT_PARA_AREA2)

#define LOCATION_SPEAK_CML (LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(3)+4+AUX_FONT_WIDTH(2)+10)
#define LOCATION_MSTMODE_CML	(LOCATION_SPEAK_CML+13)
#define LAYOUT_CMLPART(x)	((220>>3)*x)

#define LOCATION_FEBCNT_PAGE LOCATION_TTLCMP_PAGE
#define LOCATION_FEBCNT_CML   (LAYOUT_CMLPART(1)+MAIN_FONT_WIDTH(7)+FONT_CMP_WT+FONT_FEB_WT+2)

#define LAYOUT_SELBAR_CML	208
#define LAYOUT_PARAAREA_WT	(LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(20)+1)

#define LAYOUT_MSTMODE_PAGE	4//8
#define LAYOUT_MSTMODE_CML	194//196

#define LAYOUT_IDSET_CML		75

#define DSP_OUTPUT_PAGE 		LAYOUT_RATIO_A//0
#define DSP_OUTPUT_CMN	  		48//35

#define FONT_DOT_SIZE		4//2//

#define FONT_MOTO_WT		16
#define FONT_MOTO_HT		16

#define FONT_SYN_WT	16
#define FONT_SYN_HT	16

#define FONT_MSTMOD_SIZE	24

#define FONT_SPEAKER_WT	10
#define FONT_SPEAKER_HT	16

#define FONT_RF_WT			24
#define FONT_RF_HT			24

#define FONT_HEAD_WT		24
#define FONT_HEAD_HT		24

#define LAYOUT_LKDEV_WT	8
#define LAYOUT_LKDEV_HT	16

#define LAYOUT_LKDEV_VER	2

#define LAYOUT_SCANRST_X    (LCD_H-24-36)
#define LAYOUT_SCANRST_Y    (18)

#define SCANRST_DOT_WT	7
#define SCANRST_DOT_HT		4
#define SCANRST_DOT_VAL	1
#define SCANRST_BAR_VAL	4

#define SCANRST_ARROW_HT	16//13
#define SCANRST_ARROW_WT	16//13

#define LAYOUT_SPEAKER_PAGE	6
#define LAYOUT_SPEAKER_CML	180

#define FEB_ARROW_SIZE		8

#endif


#endif




/*###################ASCII#############################
  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
*/
