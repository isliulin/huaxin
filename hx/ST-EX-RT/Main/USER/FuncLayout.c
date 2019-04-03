#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "../HARDWARE/LCD/LCD.h"
#include "../HARDWARE/LCD/Font.h"


/*
void LayoutClear(u16 x, u16 y, u16 wt, u16 ht)
{
	LcdClearFeild(x,y,wt,ht);
}

void LayModeClear(void)
{
	LayoutClear(0,0,LAYOUT_MODE_WT,LAYOUT_MODE_HT);
}
*/

void LayModeEttl(void)
{
	LcdLoadPicture(0,LAYOUT_ETTL_X,LAY_WT_NOR,LAY_HT,ettl_EBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_ETTL_X+LAY_WT_NOR,LAY_WT_NOR,LAY_HT,ettl_TBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_ETTL_X+LAY_WT_NOR*2,LAY_WT_NOR,LAY_HT,ettl_TBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_ETTL_X+LAY_WT_NOR*3,LAY_WT_NOR,LAY_HT,ettl_LBitmap,DSP_NORMAL);	
}

void LayModeManu(void)
{
	LcdLoadPicture(0,LAYOUT_MANU_X,LAY_WT_M,LAY_HT,mBitmap,DSP_NORMAL);	
}

void LayModeMulti(void)
{
	LcdLoadPicture(0,LAYOUT_MULTI_X,LAY_WT_M,LAY_HT,mBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M,LAY_WT_NOR,LAY_HT,uBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+LAY_WT_NOR,LAY_WT_NOR,LAY_HT,ettl_LBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+LAY_WT_NOR*2,LAY_WT_NOR,LAY_HT,ettl_TBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+LAY_WT_NOR*3,LAY_WT_I,LAY_HT,iBitmap,DSP_NORMAL);
}

void LayModeGr(void)
{
	LCD_MODE_CLEAR;
	LcdLoadPicture(0,LAYOUT_Gr_X,LAY_WT_NOR,LAY_HT,gBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_Gr_X+LAY_WT_NOR,LAY_WT_NOR,LAY_HT,rBitmap,DSP_NORMAL);
}




