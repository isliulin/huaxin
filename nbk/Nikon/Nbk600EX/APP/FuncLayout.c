#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "../HARDWARE/LCD/LCD.h"
#include "../HARDWARE/LCD/Font.h"

extern signed char *pSnrSet;

void LayoutClear(u16 x, u16 y, u16 wt, u16 ht)
{
	LcdClearFeild(x,y,wt,ht);
}

void LayModeClear(void)
{
	LayoutClear(0,0,LAYOUT_MODE_WT,LAYOUT_MODE_HT);
}

void LayModeEttl(void)
{
	if(1==pSnrSet[SNRMENU_MODE])
	{
		LcdLoadPicture(0,LAYOUT_ETTL_X+4+1,11,LAY_HT,ettl_TBitmap,DSP_NORMAL);
		LcdLoadPicture(0,LAYOUT_ETTL_X+4+1+11+1,11,LAY_HT,ettl_TBitmap,DSP_NORMAL);
		LcdLoadPicture(0,LAYOUT_ETTL_X+4+1+11+1+11+1,9,LAY_HT,ettl_LBitmap,DSP_NORMAL);	
	}else
	{
		LcdLoadPicture(0,LAYOUT_ETTL_X,9,LAY_HT,ettl_EBitmap,DSP_NORMAL);
		LcdLoadPicture(0,LAYOUT_ETTL_X+9+1,11,LAY_HT,ettl_TBitmap,DSP_NORMAL);
		LcdLoadPicture(0,LAYOUT_ETTL_X+9+1+11+1,11,LAY_HT,ettl_TBitmap,DSP_NORMAL);
		LcdLoadPicture(0,LAYOUT_ETTL_X+9+1+11+1+11+1,9,LAY_HT,ettl_LBitmap,DSP_NORMAL);	
	}
}

void LayModeManu(void)
{
	LcdLoadPicture(0,LAYOUT_MANU_X,LAY_WT_M,LAY_HT,mBitmap,DSP_NORMAL);	
}

void LayModeMulti(void)
{
	LcdLoadPicture(0,LAYOUT_MULTI_X,LAY_WT_M,LAY_HT,mBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+1,11,LAY_HT,uBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+1+11+1,18,LAY_HT,multi_ltBitmap,DSP_NORMAL);
	//LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+LAY_WT_NOR*2,LAY_WT_NOR,LAY_HT,ettl_TBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_MULTI_X+LAY_WT_M+1+11+1+18+1,3,LAY_HT,iBitmap,DSP_NORMAL);
}

void LayModeGr(void)
{
	//LCD_MODE_CLEAR;
	LcdLoadPicture(0,LAYOUT_Gr_X,12,LAY_HT,gBitmap,DSP_NORMAL);
	LcdLoadPicture(0,LAYOUT_Gr_X+12+2,9,16,rBitmap,DSP_NORMAL);
}

void LayModeExtA(void)
{
	LcdLoadPicture(0,2,9,LAY_HT,ettl_EBitmap,DSP_NORMAL);
	LcdLoadPicture(0,2+9+1,9,LAY_HT,ext_xBitmap,DSP_NORMAL);
	LcdLoadPicture(0,2+9+1+9+1,7,LAY_HT,ext_tBitmap,DSP_NORMAL);
	LcdLoadPicture(1,2+9+1+9+1+7+1,3,8,ext_dotBitmap,DSP_NORMAL);
	LcdLoadPicture(0,2+9+1+9+1+7+1+3+1,17,LAY_HT,ext_aBitmap,DSP_NORMAL);
}

void LayModeExtM(void)
{
	LcdLoadPicture(0,2,9,LAY_HT,ettl_EBitmap,DSP_NORMAL);
	LcdLoadPicture(0,2+9+1,9,LAY_HT,ext_xBitmap,DSP_NORMAL);
	LcdLoadPicture(0,2+9+1+9+1,7,LAY_HT,ext_tBitmap,DSP_NORMAL);
	LcdLoadPicture(1,2+9+1+9+1+7+1,3,8,ext_dotBitmap,DSP_NORMAL);
	LcdLoadPicture(0,2+9+1+9+1+7+1+3+2,15,LAY_HT,mBitmap,DSP_NORMAL);
}




