#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	
#include "includes.h"	 
#include "../HARDWARE/spi/spi.h"
#include "Nikon.h"
#include "../hardware/Lcd/font.h"
#include "keyproc.h"
#include "menu.h"

#define SNRSUB_CML 80
#define SNRSUB_ADJ(x)  ((172-7-70-(x))/2+70)

#define SNRSUB_PAGE_START_2    54//50
#define SNRSUB_PAGE_START_3    47//43
#define SNRSUB_PAGE_START_6    8

s8 g_snrIndex=0;
s8 g_snrSubIndex=0;
s8 g_snrStart=0;
s8 g_snrMyMenuSelStart=0;
s8 g_snrMyMenuSelIndex=0;
//u32 g_snrMyMenuBits = 0x0007AAA3;
u32 g_snrMyMenuBits = 0xFFFFFFFF;
extern FuncMenu funcMenu;

void BufAngleCut(char *buf, int len, char mask1, char mask2);
extern void BuildMyMenuSetMenu(void);
extern void ShiftMyMenubits(s8* sft, u8 dir,u8 cnt);
extern u8 GeMyMenuDspIndex(u8 num);

#define ISSUB_SEL(x,msk)  ((msk&0x80)&&(msk&(1<<x)))

void SubDsp_Apert(u8 sel, u8 org, u8 rsvId);
void SubDsp_2(u8 sel, u8 org, u8 rsvId);
void SubDsp_3(u8 sel, u8 org, u8 rsvId);
void SubDsp_SU4(u8 sel, u8 org, u8 rsvId);
void SubDsp_5(u8 sel, u8 org, u8 rsvId);
void SubDsp_6(u8 sel, u8 org, u8 rsvId);
void SubDsp_7(u8 sel, u8 org, u8 rsvId);
void SubDsp_ZoomManu(u8 sel, u8 org, u8 rsvId);
void SubDsp_9(u8 sel, u8 org, u8 rsvId);
void SubDsp_10(u8 sel, u8 org, u8 rsvId);
void SubDsp_Iso(u8 sel, u8 org, u8 rsvId);
void SubDsp_12(u8 sel, u8 org, u8 rsvId);
void SubDsp_13(u8 sel, u8 org, u8 rsvId);
void SubDsp_14(u8 sel, u8 org, u8 rsvId);
void SubDsp_15(u8 sel, u8 org, u8 rsvId);
void SubDsp_16(u8 sel, u8 org, u8 rsvId);
void SubDsp_17(u8 sel, u8 org, u8 rsvId);
void SubDsp_18(u8 sel, u8 org, u8 rsvId);
void SubDsp_Reset(u8 sel, u8 org, u8 rsvId);

void SubSetProc_Apert(u8 sel);
void SubSetProc_2(u8 sel);
void SubSetProc_3(u8 sel);
void SubSetProc_4(u8 sel);
void SubSetProc_5(u8 sel);
void SubSetProc_6(u8 sel);
void SubSetProc_7(u8 sel);
void SubSetProc_8(u8 sel);
void SubSetProc_Af(u8 sel);
void SubSetProc_10(u8 sel);
void SubSetProc_Iso(u8 sel);
void SubSetProc_12(u8 sel);
void SubSetProc_13(u8 sel);
void SubSetProc_14(u8 sel);
void SubSetProc_15(u8 sel);
void SubSetProc_16(u8 sel);
void SubSetProc_17(u8 sel);
void SubSetProc_18(u8 sel);
void SubSetProc_Reset(u8 sel);

void SmHandle_ReflectPad(u8 sel);

SubMenu snrSub1 = {4,&SubDsp_Apert,SubSetProc_Apert}; 
SubMenu snrSub2 = {2,&SubDsp_2,SubSetProc_2}; 
SubMenu snrSub3 = {2,&SubDsp_3,SubSetProc_3}; 
SubMenu snrSub4 = {2,&SubDsp_SU4,SubSetProc_4}; 

SubMenu snrSub5 = {2,&SubDsp_5,SubSetProc_5}; 
SubMenu snrSub6 = {3,&SubDsp_6,SubSetProc_6}; 
SubMenu snrSub7 = {3,&SubDsp_7,SubSetProc_7}; 
SubMenu snrSub8 = {2,&SubDsp_ZoomManu,SubSetProc_8}; 

SubMenu snrSub9 = {3,&SubDsp_9,SubSetProc_Af}; 
SubMenu snrSub10 = {6,&SubDsp_10,SubSetProc_10}; 
SubMenu snrSubIso = {35,&SubDsp_Iso,SubSetProc_Iso}; 
SubMenu snrSub12 = {3,&SubDsp_12,SubSetProc_12}; 

SubMenu snrSub13 = {2,&SubDsp_13,SubSetProc_13}; 
SubMenu snrSub14 = {9,&SubDsp_14,SubSetProc_14}; 
SubMenu snrSub15 = {2,&SubDsp_15,SubSetProc_15}; 
SubMenu snrSub16 = {2,&SubDsp_16,SubSetProc_16};

SubMenu snrSub17 = {3,&SubDsp_17,SubSetProc_17}; 
SubMenu snrSub18 = {1,&SubDsp_18,SubSetProc_18}; 
SubMenu snrSub19 = {2,&SubDsp_Reset,SubSetProc_Reset}; 

const char g_subMenuCnt[SNR_MENU_CNT] =
{
4,2,2,2,2,3,/*3,*/2,3,/*6,*/35,3,2,9,2,2,3,1,2,
};

const char g_subMenuIsLoop[SNR_MENU_CNT] = 
{
//0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,
   1,1,1,1,1,1,/*1,*/1,1,/*1,*/1,1,1,0,1,1,1,1,1, 
};

/*
SubMenu snrSub1 = {4,&SubDsp_1,NULL}; 
SubMenu snrSub2 = {2,&SubDsp_2,NULL}; 
SubMenu snrSub3 = {2,&SubDsp_3,NULL}; 
SubMenu snrSub4 = {2,&SubDsp_4,NULL}; 
SubMenu snrSub5 = {2,&SubDsp_5,NULL}; 
SubMenu snrSub6 = {3,&SubDsp_6,NULL}; 
SubMenu snrSub7 = {3,&SubDsp_7,NULL}; 
SubMenu snrSub8 = {2,&SubDsp_8,NULL}; 
SubMenu snrSub9 = {3,&SubDsp_9,NULL}; 
SubMenu snrSub10 = {6,&SubDsp_10,NULL}; 
SubMenu snrSub11 = {35,&SubDsp_11,NULL}; 
SubMenu snrSub12 = {3,&SubDsp_12,NULL}; 
SubMenu snrSub13 = {2,&SubDsp_13,NULL}; 
SubMenu snrSub14 = {9,&SubDsp_14,NULL}; 
SubMenu snrSub15 = {2,&SubDsp_15,NULL}; 
SubMenu snrSub16 = {2,&SubDsp_16,NULL}; 
SubMenu snrSub17 = {3,&SubDsp_17,NULL}; 
SubMenu snrSub18 = {1,&SubDsp_18,NULL}; 
SubMenu snrSub19 = {2,&SubDsp_19,NULL}; 
*/

const MenuItem g_snrMenu[SNR_MENU_CNT] = 
{
	//{picSnrNO8,27,0,&snrSub8,NULL},
	//{picSnrNO9,37,0,&snrSub9,NULL},
	
	{picSnrNO1,15,0,&snrSub1,NULL},
	{picSnrNO2,45,1,&snrSub2,NULL},
	{picSnrNO3,23,0,&snrSub3,NULL},
	{picSnrNO4,54,1,&snrSub4,NULL},
	{picSnrNO5,26,0,&snrSub5,NULL},
	{picSnrNO6,45,0,&snrSub6,NULL},
	//{picSnrNO7,31,0,&snrSub7,NULL},
	//{picSnrNO7,31,0,&snrSub7,NULL},
	{picSnrNO8,27,0,&snrSub8,NULL},
	{picSnrNO9,37,0,&snrSub9,NULL},
	//{picSnrNO10,45,0,&snrSub10,NULL},
	{picSnrNO11,17,0,&snrSubIso,NULL},
	{picSnrNO12,47,1,&snrSub12,NULL},
	{picSnrNO13,47,0,&snrSub13,NULL},
	{picSnrNO14,41,0,&snrSub14,NULL},
	{picSnrNO15,32,0,&snrSub15,NULL},
	{picSnrNO16,39,1,&snrSub16,NULL},
	{picSnrNO17,13,0,&snrSub17,NULL},
	{picSnrNO18,19,0,&snrSub18,NULL},
	{picSnrNO19,29,0,&snrSub19,NULL},			
};

void CopyToRamCenter(u8 *buf, u8 *src, char wt, char swt)
{
	u8 i=0;
	for(i=0;i<swt;i++)
	{
		buf[(wt-swt)/2+i] = src[i];
		buf[(wt-swt)/2+i+wt] = src[i+swt];
	}
}

void SnrMenuSetFrame(u8 *buf)
{
	u8 i=0;
	for(i=0;i<SNR_FRAGMENT_LEN*3;i++)
	{
		buf[i] |= picSnrMask[i];
	}
}

void DspSnrCalcBufMask(u8 *buf, int size)
{
	int i=0;
	for(i=0;i<3*size;i++)
	{
		buf[i] = ~buf[i];
	}
	for(i=2*size;i<3*size;i++)
	{
		buf[i] &= 0xE0;
	}
}

#define DIR_UP   1
#define DIR_DN   2
#define DIR_NULL 0

/*
void CopyToDest(char *dest, char *src, int len, char bias, char dir)
{
	int i = 0;
	for(i=0;i<len;i++)
	{
		if(DIR_UP==dir)
		{
			dest[i] |= (src[i]<<bias);
		}else if(DIR_DN==dir)
		{
			dest[i] |= (src[i]>>bias);
		}else
		{
			dest[i] = src[i];
		}
	}
}
*/

char CopyToDest(u8 *dest, u8 *src, char bias, u8* step)
{
	int i=0;
//	u8 lines=0;
	for(i=0;i<SNR_FRAGMENT_LEN;i++)
	{
		if(bias<5)
		{
			dest[i] |= (src[i]>>bias);
			dest[SNR_FRAGMENT_LEN+i] |= (src[i]<<(8-bias));
			dest[SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN]>>bias);
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN]<<(8-bias));
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN]>>bias);
			*step = 2;
		}else if(5==bias)
		{
			dest[i] |= (src[i]>>bias);
			dest[SNR_FRAGMENT_LEN+i] |= (src[i]<<(8-bias));
			dest[SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN]>>bias);
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN]<<(8-bias));
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN]>>bias);
			*step = 3;
		}else
		{
			dest[i] |= (src[i]>>bias);
			dest[SNR_FRAGMENT_LEN+i] |= (src[i]<<(8-bias));
			dest[SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN]>>bias);
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN]<<(8-bias));
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN]>>bias);
			dest[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN+i] |= (src[i+SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN]<<(8-bias));
			*step = 3;
		}		
	}
	return ((19+bias)%8);//(19-16-bias)
}

void DspMyMenuSelIcon(u8 x, u8 y, u16 isSel, u8 rsv)
{
	u8 i = 0;
	
	memset(g_paraSetBuf,0x00,13*3);
	if(isSel>0)
	{
		memcpy(g_paraSetBuf,picSnrSubMyMenu_sel,26);
	}else
	{
		memcpy(g_paraSetBuf,picSnrSubMyMenu_unsel,26);
	}

	if((DSP_REVERSE==rsv))
	{
		for(i=0;i<13;i++)
		{
			g_paraSetBuf[i] = ~g_paraSetBuf[i];
			g_paraSetBuf[13+i] = ~g_paraSetBuf[13+i];
			g_paraSetBuf[13+i]&=0xf8;
		}
	}

	BufAngleCut(g_paraSetBuf,13,0x7f,0xf0);

	BufferScroll((u8*)g_paraSetBuf, 13, 3, SCROLL_DOWN, x%8);
	//LcdCopyToRam(0, 1, g_paraSetBuf, SNR_FRAGMENT_LEN, 80);
	LcdCopyToRam(x/8, y, g_paraSetBuf, 13, 24);
	
}

void DspMyMenuSelect(void)
{
	u8 *pTmpBuf = (u8*)(g_paraSetBuf+STM_SECTOR_SIZE);
	u8 bias = 4;
	int paraSetBufIndex=0;
//	int tmpBufIndex=0;
	u8 step=0;
	int i=0;

//	g_SettingIndex = SETTING_PFNMENU;

//	LCD_Clear(0);
	SubSnrClear();
	
	//memset(g_paraSetBuf,0x00,SNR_FRAGMENT_LEN*10);
	memset(g_paraSetBuf,0x00,SNR_FRAGMENT_LEN*11);

	for(i=0;(i<4)&&(g_snrMyMenuSelStart+i<SNR_MENU_CNT);i++)
	{

		memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);

		CopyToRamCenter(pTmpBuf,(u8*)g_snrMenu[g_snrMyMenuSelStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrMyMenuSelStart+i].picWt);

		AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 4, SNR_FRAGMENT_LEN, SCROLL_DOWN, 1);

		//DisplayPicture(i*3, 1, SNR_FRAGMENT_LEN, 24, pTmpBuf, DSP_NORMAL);
		if(g_snrMenu[g_snrMyMenuSelStart+i].isMask>0)
		{
			SnrMenuSetFrame(pTmpBuf);
		}


		if((g_snrMyMenuSelStart+i)==g_snrMyMenuSelIndex)
		{
#ifdef PRINT_DEBUG
		if((g_snrStart+i)==g_snrIndex)
		{
			DBG_PRINTF("NO %d\r\n",g_snrStart);
			printf_buf(pTmpBuf, SNR_FRAGMENT_LEN*3, SNR_FRAGMENT_LEN);
		}
#endif
			DspSnrCalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN);
#ifdef PRINT_DEBUG
			printf_buf(pTmpBuf, SNR_FRAGMENT_LEN*3, SNR_FRAGMENT_LEN);
#endif
		}

		//DisplayPicture(i*3, 1, SNR_FRAGMENT_LEN, 24, pTmpBuf, DSP_NORMAL);

		//AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 5);

		//DBG_PRINTF("index %d, bias %d\r\n",paraSetBufIndex,bias);

		bias = CopyToDest((u8*)g_paraSetBuf+paraSetBufIndex,pTmpBuf,bias,&step);

		paraSetBufIndex+=(step*SNR_FRAGMENT_LEN);


	}

	for(i=0;i<SNR_FRAGMENT_LEN;i++)
	{
		g_paraSetBuf[i] |= 0x10;
		g_paraSetBuf[i+SNR_FRAGMENT_LEN*2] |=0x02;
		g_paraSetBuf[i+SNR_FRAGMENT_LEN*5] |=0x40;
		g_paraSetBuf[i+SNR_FRAGMENT_LEN*7] |=0x08;
		g_paraSetBuf[i+SNR_FRAGMENT_LEN*9] |=0x01;
	}

	g_paraSetBuf[0] |= 0x1f;
	g_paraSetBuf[SNR_FRAGMENT_LEN-1] |= 0x1f;

	for(i=1;i<10;i++)
	{
		g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= 0xff;
		g_paraSetBuf[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN*i-1] |=0xff;
	}


	BufferScroll((u8*)g_paraSetBuf, SNR_FRAGMENT_LEN, 11, SCROLL_DOWN, 4);
	//LcdCopyToRam(0, 1, g_paraSetBuf, SNR_FRAGMENT_LEN, 80);
	LcdCopyToRam(0, (LCD_W-SNR_FRAGMENT_LEN-2), g_paraSetBuf, SNR_FRAGMENT_LEN, 88);


	for(i=0;i<4;i++)
	{
		DspMyMenuSelIcon(i*19+8+3,SNR_FRAGMENT_LEN+15,g_snrMyMenuBits&(1<<(g_snrMyMenuSelStart+i)),(SETTING_MYMENU==g_SettingIndex)&&(g_snrMyMenuSelIndex==(g_snrMyMenuSelStart+i)));
	}

	BuildMyMenuSetMenu();

}

//void*DspSnrMenu(void)


void *DspMySnrMenu(void)
{
	u8 *pTmpBuf = (u8*)(g_paraSetBuf+STM_SECTOR_SIZE);
	u8 bias = 4;
	int paraSetBufIndex=0;
//	int tmpBufIndex=0;
	u8 step=0;
	u8 fragment = 0;
	int i=0;
	u8 rowDots = 0;
	s8 mdt = 0;
 	//u16 dspTurn=0;

	g_SettingIndex = SETTING_PFNMENU;

	LCD_Clear(0);
	
	//memset(g_paraSetBuf,0x00,SNR_FRAGMENT_LEN*10);
	memset(g_paraSetBuf,0x00,SNR_FRAGMENT_LEN*11);

	mdt = g_snrStart;
	
	//for(i=0;(i<4)&&(g_snrStart+i<SNR_MENU_CNT);i++)
	//while((g_snrStart+i<SNR_MENU_CNT)&&(fragment<4))
	while((mdt<SNR_MENU_CNT)&&(fragment<4))
	{

		if(g_allMyMenuSw>0)
		{
			if(!(g_snrMyMenuBits&(1<<(mdt))))
			{
				mdt++;
				continue;
			}
			
		}

		//ShiftMyMenubits(&mdt,KEY_DIR_RIGHT,1);
		
		memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);

		//CopyToRamCenter(pTmpBuf,(u8*)g_snrMenu[g_snrStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrStart+i].picWt);
		CopyToRamCenter(pTmpBuf,(u8*)g_snrMenu[mdt].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[mdt].picWt);

		AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 4, SNR_FRAGMENT_LEN, SCROLL_DOWN, 1);

		//DisplayPicture(i*3, 1, SNR_FRAGMENT_LEN, 24, pTmpBuf, DSP_NORMAL);
		//if(g_snrMenu[g_snrStart+i].isMask>0)
		if(g_snrMenu[mdt].isMask>0)
		{
			SnrMenuSetFrame(pTmpBuf);
		}


		//if((g_snrStart+i)==g_snrIndex)
		if(mdt==g_snrIndex)
		{
#ifdef PRINT_DEBUG
		if(mdt==g_snrIndex)
		{
			DBG_PRINTF("NO %d\r\n",g_snrStart);
			printf_buf(pTmpBuf, SNR_FRAGMENT_LEN*3, SNR_FRAGMENT_LEN);
		}
#endif
			DspSnrCalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN);
#ifdef PRINT_DEBUG
			printf_buf(pTmpBuf, SNR_FRAGMENT_LEN*3, SNR_FRAGMENT_LEN);
#endif
		}

		bias = CopyToDest((u8*)g_paraSetBuf+paraSetBufIndex,pTmpBuf,bias,&step);

		paraSetBufIndex+=(step*SNR_FRAGMENT_LEN);

		fragment++;
		mdt++;
		
	}

	for(i=0;i<SNR_FRAGMENT_LEN;i++)
	{
		g_paraSetBuf[i] |= 0x10;
		g_paraSetBuf[i+SNR_FRAGMENT_LEN*2] |=0x02;
		if(fragment>1)
		{
			g_paraSetBuf[i+SNR_FRAGMENT_LEN*5] |=0x40;
		}
		if(fragment>2)
		{
			g_paraSetBuf[i+SNR_FRAGMENT_LEN*7] |=0x08;
		}
		if(fragment>3)
		{
			g_paraSetBuf[i+SNR_FRAGMENT_LEN*9] |=0x01;
		}
	}

	g_paraSetBuf[0] |= 0x1f;
	g_paraSetBuf[SNR_FRAGMENT_LEN-1] |= 0x1f;

	rowDots = fragment*19;

	for(i=1;i<(rowDots+4)/8;i++)
	{
		g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= 0xff;
		g_paraSetBuf[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN*i-1] |=0xff;
	}

	if((rowDots+4)%8)
	{
		g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= (0xff<<(8-((rowDots+4)%8)));
		g_paraSetBuf[SNR_FRAGMENT_LEN*i+SNR_FRAGMENT_LEN-1] |= (0xff<<(8-((rowDots+4)%8)));
	}

/*
	if(g_snrStart<16)
	{
		for(i=1;i<10;i++)
		{
			g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= 0xff;
			g_paraSetBuf[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN*i-1] |=0xff;
		}
	}else
	{
		for(i=1;i<7;i++)
		{
			g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= 0xff;
			g_paraSetBuf[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN*i-1] |=0xff;
		}

		g_paraSetBuf[SNR_FRAGMENT_LEN*7] |= 0xF8;
		g_paraSetBuf[SNR_FRAGMENT_LEN*8-1] |= 0xF8;
	}
*/

	BufferScroll((u8*)g_paraSetBuf, SNR_FRAGMENT_LEN, 11, SCROLL_DOWN, 4);
	//LcdCopyToRam(0, 1, g_paraSetBuf, SNR_FRAGMENT_LEN, 80);
	LcdCopyToRam(0, 1, g_paraSetBuf, SNR_FRAGMENT_LEN, 88);

 	//g_snrMenu[g_snrIndex].subMenu->subDsp(pSnrSet[g_snrIndex],pSnrSet[g_snrIndex],0);
 	DspSubMenu(pSnrSet[g_snrIndex],DSP_NORMAL);

	BuildSnrMenu();	

	return NULL;
}

void DspSubMenu(u8 subSel,u8 rsvId)
{
	switch(g_snrIndex)
	{
		case SNR_APERT:
			//pSnrSet[g_snrIndex] = subSel;
			SubDsp_Apert(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_MSTRPT:
			SubDsp_2(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_STEP:
			SubDsp_3(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_SU4:
			SubDsp_SU4(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_MODELING:
			SubDsp_5(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_PILOT:
			SubDsp_6(subSel,pSnrSet[g_snrIndex],rsvId);
		break;
/*
		case SNR_FXDX:
			SubDsp_7(subSel,pSnrSet[g_snrIndex],rsvId);
		break;
*/

		case SNR_ZOOM:
			SubDsp_ZoomManu(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_AF:
			SubDsp_9(subSel,pSnrSet[g_snrIndex],rsvId);
		break;
/*
		case SNR_STBY:
			SubDsp_10(subSel,pSnrSet[g_snrIndex],rsvId);
		break;
*/

		case SNR_ISO:
			SubDsp_Iso(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_REAR:
			SubDsp_12(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_BKLIGHT:
			SubDsp_13(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_CONTRAST:
			pSnrSet[g_snrIndex] = subSel;
			SubDsp_14(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_STD:
			SubDsp_15(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_WP:
			SubDsp_16(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_MYMENU:
			SubDsp_17(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_VER:
			SubDsp_18(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		case SNR_RESET:
			SubDsp_Reset(subSel,pSnrSet[g_snrIndex],rsvId);
		break;

		default:
		break;


	}
}

void SubMenuHandle(void)
{
	switch(g_snrIndex)
	{
		case SNR_APERT:
			if(CMD_MODE_OFF==*g_cmdMode)
			{
				SetSpiComModeOff(mstPara->mode,pSnrSet[SNR_APERT]);
				macData2[COM_DAT_REAR] = VerCodeCal(macData2);
			}
		break;
	
		case SNR_WP:
			SmHandle_ReflectPad(g_snrSubIndex);
		break;

		//case 7:
		case SNR_ZOOM:
			//pSnrSet[SNR_ZOOM] = g_snrSubIndex;
			//if(!pSnrSet[SNR_ZOOM])
			if(!g_snrSubIndex)
			{
				*motoSet.mode = MOTO_MODE_MANU;
				motoSet.destRsv = motoSet.manuRsv;
			}else
			{
				*motoSet.mode = MOTO_MODE_AUTO;
				//motoSet.manuRsv = *motoSet.dest_location;
				if(cmrPara.cls>0)
				{
					motoSet.destRsv = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
				}
			}
		break;

		case SNR_REAR:
			if(2==g_snrSubIndex)
			{
				CHARGE_LED_OFF();	
			}else if(g_chargeOk>0)
			{
				CHARGE_LED_OK();
			}
		break;

		case SNR_STEP:
			if(((6==mstPara->output1)&&(mstPara->output2>0))||(7==mstPara->output1))
			{
				mstPara->output2 = 0;
			}
		break;

		case SNR_CONTRAST:
			SetContrast(pSnrSet[SNR_CONTRAST]);
		break;

		default:
		break;
	}
}

void SubMenuSetProc(void)
{
	switch(g_snrIndex)
	{
		case SNR_APERT:
			SubSetProc_Apert(g_snrSubIndex);
		break;

		case SNR_MSTRPT:
			SubSetProc_2(g_snrSubIndex);
		break;

		case SNR_STEP:
			SubSetProc_3(g_snrSubIndex);
		break;

		case SNR_SU4:
			SubSetProc_4(g_snrSubIndex);
		break;

		case SNR_MODELING:
			SubSetProc_5(g_snrSubIndex);
		break;

		case SNR_PILOT:
			SubSetProc_6(g_snrSubIndex);
		break;
/*
		case SNR_FXDX:
			SubSetProc_7(g_snrSubIndex);
		break;
*/
		case SNR_ZOOM:
			SubSetProc_8(g_snrSubIndex);
		break;

		case SNR_AF:
			SubSetProc_Af(g_snrSubIndex);
		break;
/*
		case SNR_STBY:
			SubSetProc_10(g_snrSubIndex);
		break;
*/
		case SNR_ISO:
			SubSetProc_Iso(g_snrSubIndex);
		break;

		case SNR_REAR:
			SubSetProc_12(g_snrSubIndex);
		break;

		case SNR_BKLIGHT:
			SubSetProc_13(g_snrSubIndex);
		break;

		case SNR_CONTRAST:
			SubSetProc_14(g_snrSubIndex);
		break;

		case SNR_STD:
			SubSetProc_15(g_snrSubIndex);
		break;

		case SNR_WP:
			SubSetProc_16(g_snrSubIndex);
		break;

		case SNR_MYMENU:
			SubSetProc_17(g_snrSubIndex);
		break;

		case SNR_VER:
			SubSetProc_18(g_snrSubIndex);
		break;

		case SNR_RESET:
			SubSetProc_Reset(g_snrSubIndex);
		break;
	}
}

void *DspAllSnrMenu(void)
{
//	u32 tmp=0;
//	int frgSt[4] = {0,SNR_FRAGMENT_LEN*3,SNR_FRAGMENT_LEN*5,SNR_FRAGMENT_LEN*7};
//	s8 set=g_snrStart;
	u8 *pTmpBuf = (u8*)(g_paraSetBuf+STM_SECTOR_SIZE);
	u8 bias = 4;
	int paraSetBufIndex=0;
//	int tmpBufIndex=0;
	u8 step=0;
//	u8 fragment = 0;
	u16 i=0;
 	//u16 dspTurn=0;

	g_SettingIndex = SETTING_PFNMENU;

	LCD_Clear(0);
	
	//memset(g_paraSetBuf,0x00,SNR_FRAGMENT_LEN*10);
	memset(g_paraSetBuf,0x00,SNR_FRAGMENT_LEN*11);

	for(i=0;(i<4)&&(g_snrStart+i<SNR_MENU_CNT);i++)
	//while((g_snrStart+i<SNR_MENU_CNT)&&(fragment<4))
	{
/*
		if(g_allMyMenuSw>0)
		{
			if(!(g_snrMyMenuBits&(1<<(g_snrStart+i))))
			{
				i++;
				continue;
			}
			
		}
*/

		memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);

		CopyToRamCenter(pTmpBuf,(u8*)g_snrMenu[g_snrStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrStart+i].picWt);

		AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 4, SNR_FRAGMENT_LEN, SCROLL_DOWN, 1);

		//DisplayPicture(i*3, 1, SNR_FRAGMENT_LEN, 24, pTmpBuf, DSP_NORMAL);
		if(g_snrMenu[g_snrStart+i].isMask>0)
		{
			SnrMenuSetFrame(pTmpBuf);
		}


		if((g_snrStart+i)==g_snrIndex)
		{
#ifdef PRINT_DEBUG
		if((g_snrStart+i)==g_snrIndex)
		{
			DBG_PRINTF("NO %d\r\n",g_snrStart);
			printf_buf(pTmpBuf, SNR_FRAGMENT_LEN*3, SNR_FRAGMENT_LEN);
		}
#endif
			DspSnrCalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN);
#ifdef PRINT_DEBUG
			printf_buf(pTmpBuf, SNR_FRAGMENT_LEN*3, SNR_FRAGMENT_LEN);
#endif
		}

		//DisplayPicture(i*3, 1, SNR_FRAGMENT_LEN, 24, pTmpBuf, DSP_NORMAL);

		//AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 5);

		//DBG_PRINTF("index %d, bias %d\r\n",paraSetBufIndex,bias);

		bias = CopyToDest((u8*)g_paraSetBuf+paraSetBufIndex,pTmpBuf,bias,&step);

		paraSetBufIndex+=(step*SNR_FRAGMENT_LEN);
/*
		if(!bias)
		{
			paraSetBufIndex+=3*SNR_FRAGMENT_LEN;
		}else
		{
			paraSetBufIndex+=2*SNR_FRAGMENT_LEN;
		}*/
	//	dspTurn++;

	}


/*

	paraSetBufIndex+=SNR_FRAGMENT_LEN*3;

	i++;
	memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);
	CopyToRamCenter(pTmpBuf,(char*)g_snrMenu[g_snrStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrStart+i].picWt);
	AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 2);
	if(g_snrMenu[g_snrStart+i].isMask>0)
	{
		SnrMenuSetFrame(pTmpBuf);
	}

	if((g_snrStart+i)==g_snrIndex)
	{
		CalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN);
	}

	//AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 0);

	CopyToDest(g_paraSetBuf+paraSetBufIndex,pTmpBuf,SNR_FRAGMENT_LEN*3,0,DIR_NULL);

	paraSetBufIndex+=SNR_FRAGMENT_LEN*2;

	i++;

	memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);
	CopyToRamCenter(pTmpBuf,(char*)g_snrMenu[g_snrStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrStart+i].picWt);
	AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 2);
	if(g_snrMenu[g_snrStart+i].isMask>0)
	{
		SnrMenuSetFrame(pTmpBuf);
	}

	if((g_snrStart+i)==g_snrIndex)
	{
		CalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN);
	}

	//AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 0);

	CopyToDest(g_paraSetBuf+paraSetBufIndex,pTmpBuf,SNR_FRAGMENT_LEN*3,0,DIR_NULL);
*/

/*
	for(i=0;i<4;i++)
	{	
		memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);

		CopyToRamCenter(pTmpBuf,(char*)g_snrMenu[g_snrStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrStart+i].picWt);
		AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 2);
		if(g_snrMenu[g_snrStart+i].isMask>0)
		{
			SnrMenuSetFrame(pTmpBuf);
		}

		if((g_snrStart+i)==g_snrIndex)
		{
			CalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN);
		}

		CopyToDest(g_paraSetBuf+paraSetBufIndex,pTmpBuf,SNR_FRAGMENT_LEN,bias,DIR_DN);
		bias = 8 -bias;
		paraSetBufIndex+=SNR_FRAGMENT_LEN;
		CopyToDest(g_paraSetBuf++paraSetBufIndex, pTmpBuf, SNR_FRAGMENT_LEN, bias, DIR_UP);
		bias = 8 -bias;
		CopyToDest(g_paraSetBuf++paraSetBufIndex, pTmpBuf, SNR_FRAGMENT_LEN, 0, DIR_NULL);
	}
*/	
	
/*
	for(i=0;i<4;i++)
	{
		memset(pTmpBuf,0x00,SNR_FRAGMENT_LEN*3);

		CopyToRamCenter(pTmpBuf,(char*)g_snrMenu[g_snrStart+i].pIcon,SNR_FRAGMENT_LEN,g_snrMenu[g_snrStart+i].picWt);

		//AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 4);

		if(g_snrMenu[g_snrStart+i].isMask>0)
		{
			SnrMenuSetFrame(pTmpBuf);
		}

		if((g_snrStart+i)==g_snrIndex)
		{
			CalcBufMask(pTmpBuf,SNR_FRAGMENT_LEN*3);
		}

		AreaScroll(pTmpBuf, SNR_FRAGMENT_LEN, 3, SNR_FRAGMENT_LEN, SCROLL_DOWN, 4); 

		CopyToDest(g_paraSetBuf+frgSt[i],pTmpBuf,SNR_FRAGMENT_LEN*3);
	}
*/

	for(i=0;i<SNR_FRAGMENT_LEN;i++)
	{
		g_paraSetBuf[i] |= 0x10;
		g_paraSetBuf[i+SNR_FRAGMENT_LEN*2] |=0x02;
		//g_paraSetBuf[i+SNR_FRAGMENT_LEN*5] |=0x40;
		//g_paraSetBuf[i+SNR_FRAGMENT_LEN*7] |=0x08;
		if(g_snrStart<16)
		{
			g_paraSetBuf[i+SNR_FRAGMENT_LEN*5] |=0x40;
			g_paraSetBuf[i+SNR_FRAGMENT_LEN*7] |=0x08;
			g_paraSetBuf[i+SNR_FRAGMENT_LEN*9] |=0x01;
		}
	}

	g_paraSetBuf[0] |= 0x1f;
	g_paraSetBuf[SNR_FRAGMENT_LEN-1] |= 0x1f;

	if(g_snrStart<16)
	{
		for(i=1;i<10;i++)
		{
			g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= 0xff;
			g_paraSetBuf[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN*i-1] |=0xff;
		}
	}else
	{
		//for(i=1;i<7;i++)
		{
			g_paraSetBuf[SNR_FRAGMENT_LEN*i] |= 0xff;
			g_paraSetBuf[SNR_FRAGMENT_LEN+SNR_FRAGMENT_LEN*i-1] |=0xff;
		}

		//g_paraSetBuf[SNR_FRAGMENT_LEN*7] |= 0xF8;
		//g_paraSetBuf[SNR_FRAGMENT_LEN*8-1] |= 0xF8;
	}

	BufferScroll((u8*)g_paraSetBuf, SNR_FRAGMENT_LEN, 11, SCROLL_DOWN, 4);
	//LcdCopyToRam(0, 1, g_paraSetBuf, SNR_FRAGMENT_LEN, 80);
	LcdCopyToRam(0, 1, g_paraSetBuf, SNR_FRAGMENT_LEN, 88);

	DBG_PRINTF("subDsp %x\r\n",g_snrMenu[g_snrIndex].subMenu->subDsp);
	//DBG_PRINTF("subDsp %x\r\n",g_snrMenu[g_snrIndex].subMenu->subDsp);

	//if(g_snrMenu[g_snrIndex].subMenu->subDsp!=NULL)
	//{
 	//	g_snrMenu[g_snrIndex].subMenu->subDsp(pSnrSet[g_snrIndex],pSnrSet[g_snrIndex],0);
	//}

	DspSubMenu(pSnrSet[g_snrIndex],DSP_NORMAL);

	BuildSnrMenu();	

	return NULL;
	
}

u8 GetMyMenuNum(void)
{
	u8 i=0,j=0;

	u32 shadow = g_snrMyMenuBits;

	for(i=0;i<19;i++)
	{
		if(shadow&0x01)
		{
			j++;
		}
		shadow>>=1;
	}

	return j;
}

void *DspSnrMenu(void)
{
#if 0
	if(SETTING_PFNMENU==g_SettingIndex)
	{
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}else
	{
		if(g_allMyMenuSw>0)
		{
			DspMySnrMenu();

			SnrProcessBar(1, LCD_W-7,GeMyMenuDspIndex(g_snrIndex)/4, GetMyMenuNum());
		}else
		{
			DspAllSnrMenu();

			SnrProcessBar(1, LCD_W-7,g_snrIndex/4,19);
		}
	}
#else
	if(g_allMyMenuSw>0)
	{
		DspMySnrMenu();

		SnrProcessBar(1, LCD_W-7,GeMyMenuDspIndex(g_snrIndex)/4, GetMyMenuNum());
	}else
	{
		DspAllSnrMenu();

		//SnrProcessBar(1, LCD_W-7,g_snrIndex/4,19);
		SnrProcessBar(1, LCD_W-7,g_snrIndex/4,SNR_MENU_CNT);
	}
#endif

	return NULL;
}

char SnrSubCharGetIndex(char ch)
{
	//PfnFont *pfont=(PfnFont*)pfnFont;
	char i=0;
	for(i=0;i<FONT_SNRSUB_LEN;i++)
	{
		if(ch == fontSnrSubChar[i].ch)
		{
			return i;
		}
	}

	return 0;
}

void CalcBufMask(char *buf, int size)
{
	int i=0;
	for(i=0;i<size;i++)
	{
		buf[i] = ~buf[i];	
	}
}

void BufXOR(char *buf, char mask, int size)
{
	int i=0;
	for(i=0;i<size;i++)
	{
		buf[i] &= mask;
	}
}

void BufAngleCut(char *buf, int len, char mask1, char mask2)
{
	buf[0] &=mask1;
	buf[len-1] &= mask1;
	buf[len] &= mask2;
	buf[2*len-1] &=mask2;
}

void BufAngleCutEx(char *buf, int start,int end,int len, char mask1, char mask2)
{
	buf[start] &=mask1;
	//buf[len-1] &= mask1;
	buf[end] &=mask1;
	buf[len+start] &= mask2;
	//buf[2*len-1] &=mask2;
	buf[len+end] &=mask2;
}


void SnrSubMaskBuf(u8 *buf,u8 len,u8 bias)
{
	u8 i=0;
	
	//if(bias>3)
	if(bias>5)
	{
		for(i=0;i<len;i++)
		{
			buf[i] |= (0xff>>bias);
			buf[i+len] |= 0xff;
			//buf[i+len*2] |= (0xff<<(11-bias));
			buf[i+len*2] |= (0xff<<(13-bias));
		}

		buf[0] &= ~(0x80>>bias);
		buf[len-1] &= ~(0x80>>bias);
		//buf[len*2] &= ~(1<<(11-bias));
		//buf[len*3-1] &= ~(1<<(11-bias));
		buf[len*2] &= ~(1<<(13-bias));
		buf[len*3-1] &= ~(1<<(13-bias));
		
	}else
	{
		for(i=0;i<len;i++)
		{
			buf[i] |= (0xff>>bias);
			//buf[i+len] |= (0xff<<(3-bias));
			buf[i+len] |= (0xff<<(5-bias));
		}

		buf[0] &= ~(0x80>>bias);
		buf[len-1] &= ~(0x80>>bias);
		//buf[len] &= ~(1<<(3-bias));
		//buf[len*2-1] &= ~(1<<(3-bias));
		buf[len] &= ~(1<<(5-bias));
		buf[len*2-1] &= ~(1<<(5-bias));
	}

	
}


void SnrSubStrCopyToBuf(u8 *buf, u8 *src, u8 len, u8 chLen,u8 bias, u8 rsv)
{
	u8 i=0;
	
	if(bias>3)
	{
		if(DSP_NORMAL==rsv)
		{
			for(i=0;i<chLen;i++)
			{
				buf[i] |= (src[i]>>bias);
				buf[i+len] |= (src[i] <<(8-bias));
				buf[i+len] |= (src[i+chLen]>>bias);
				//buf[i+len*2] |= (src[i+chLen]<<(8-bias));
				buf[i+len*2] |= (src[i+chLen]<<(11-bias));
			}
		}else
		{
			for(i=0;i<chLen;i++)
			{
				//buf[i] &= ((~src[i])>>bias)&(0xff>>bias);
				buf[i] &= (((~src[i])>>bias)|(0xff<<(8-bias)));
				//buf[i+len] &= ((~src[i]) <<(8-bias))&(0xff<<(8-bias));
				buf[i+len] &= (((~src[i]) <<(8-bias))|(0xff>>bias));
				//buf[i+len] &= ((~src[i+chLen])>>bias)&(0xff>>bias);
				buf[i+len] &= (((~src[i+chLen])>>bias)|(0xff<<(8-bias)));
				//buf[i+len*2] &= (src[i+chLen]<<(8-bias))&(0xff<<(8-bias));
				//buf[i+len*2] &= ((~src[i+chLen])<<(11-bias))&(0xff<<(11-bias));
				buf[i+len*2] &= (((~src[i+chLen])<<(11-bias))|(0xff>>bias));
			}
		}

	}else
	{
		if(DSP_NORMAL==rsv)
		{
			for(i=0;i<chLen;i++)
			{
				buf[i] |= src[i]>>bias;
				buf[i+len] |= (src[i] <<(8-bias));
				buf[i+len] |= (src[i+chLen]>>(bias));
			}
		}else
		{
			for(i=0;i<chLen;i++)
			{
				buf[i] &= (((~(src[i])>>bias))|(0xff<<(8-bias)));
				buf[i+len] &= ((~(src[i]) <<(8-bias))|(0xff>>bias));
				buf[i+len] &= (((~src[i+chLen])>>bias)|(0xff<<(8-bias)));
			}
		}
	}
	
}

void BufferScroll(u8 * buf, u8 len, u8 ht, u8 dir, u8 n)
{
	u8 i=0;
	s8 j=0;
//	u8 tmp = 0;
	if(SCROLL_UP==dir)
	{
		for(i=0;i<len;i++)
		{
			buf[i]<<=n;
			for(j=1;j<ht;j++)
			{
				buf[(j-1)*len+i] |= buf[j*len+i]>>(8-n);
				buf[j*len+i]<<=n;
			}
		}
	}else if(SCROLL_DOWN==dir)
	{
		for(i=0;i<len;i++)
		{
			buf[(ht-1)*len+i]>>=n;
			for(j=ht-2;j>=0;j--)
			{
				buf[(j+1)*len+i] |= buf[j*len+i]<<(8-n);
				buf[j*len+i]>>=n;
			}
		}
	}
}


void DspSnrSubStr(u8 x, u8 y, u8 cnt, u8 len, u8 * str, u8 isSel, u8 set, u8 rsv)
{
	int paraSetBufIndex=0;
	u8 lineIndex = 0;
	char index=0;
	char bias =( x%8 );
	char selCnt=0;

	lineIndex += 3;
	
	//memset(g_paraSetBuf,0x00,(len*cnt*13+8)/8);
	//memset(g_paraSetBuf,0x00,(len*cnt*13+8)/8);
	memset(g_paraSetBuf,0x00,STM_SECTOR_SIZE);

	//if(isSel==0)
	if(isSel==cnt)
	{
		//memcpy(g_paraSetBuf+paraSetBufIndex+lineIndex,picSnrSubArrow,4);
		//memcpy(g_paraSetBuf+paraSetBufIndex+lineIndex+len,picSnrSubArrow+4,4);
		if(DSP_REVERSE==rsv)
		{
			SnrSubMaskBuf((u8*)(g_paraSetBuf+paraSetBufIndex),len,bias);
		}
		//SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+lineIndex),(u8*)picSnrSubArrow,len,4,bias,rsv);
	}

	//if(set==0)
	if(set==cnt)
	{
		//SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+lineIndex),(u8*)picSnrSubArrow,len,4,bias,rsv);
		//SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+lineIndex),(u8*)picSnrSubArrow,len,4,bias,rsv&&(0==isSel));
		SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+lineIndex),(u8*)picSnrSubArrow,len,4,bias,rsv&&(cnt==isSel));
	}

	lineIndex += 7;

	while(*str!='\0')
	{
		index = SnrSubCharGetIndex(*str);
		//memcpy(g_paraSetBuf+paraSetBufIndex,fontSnrSubChar[index].buf,fontSnrSubChar[index].wt);
		//memcpy(g_paraSetBuf+paraSetBufIndex+len,fontSnrSubChar[index].buf+fontSnrSubChar[index].wt,fontSnrSubChar[index].wt);
		SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+lineIndex),(u8*)fontSnrSubChar[index].buf,len,fontSnrSubChar[index].wt,bias,((selCnt+cnt)==isSel)&&(rsv>0));
		//paraSetBufIndex+=(fontSnrSubChar[index].wt+1);
		lineIndex+=(fontSnrSubChar[index].wt+1);
		str++;
		if(*str==';')
		{
			str++;
			if(bias<3)
			{
				paraSetBufIndex += len;
			}else
			{
				paraSetBufIndex += (len*2);
			}
			bias = (bias+13)%8;	
			selCnt++;
			if((selCnt+cnt)==isSel)
			{
				if(DSP_REVERSE==rsv)
				{
					SnrSubMaskBuf((u8*)(g_paraSetBuf+paraSetBufIndex),len,bias);
				}
				//SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+3),(u8*)picSnrSubArrow,len,4,bias,rsv);
			}

			if((selCnt+cnt) == set)
			{
				SnrSubStrCopyToBuf((u8*)(g_paraSetBuf+paraSetBufIndex+3),(u8*)picSnrSubArrow,len,4,bias,rsv&&((selCnt+cnt)==isSel));
			}
			lineIndex = 10;
		}
	}

	selCnt++;

	//AreaScroll(g_paraSetBuf, len, 16/8, len, SCROLL_DOWN, (x%8));
	//BufferScroll(g_paraSetBuf,len,(cnt*13+7)/8,SCROLL_DOWN, (x%8));

	//LcdCopyToRam(x/8,y,g_paraSetBuf,len,((cnt+1)*13+7));	
	LcdCopyToRam(x/8,y,g_paraSetBuf,len,((selCnt+1)*13+7));	

/*
	paraSetBufIndex += 3;
	
	if(isSel>0)
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSubArrow,4);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,picSnrSubArrow+4,4);
	}

	paraSetBufIndex+=7;

	while(*str!='\0')
	{
		index = SnrSubCharGetIndex(*str);
		memcpy(g_paraSetBuf+paraSetBufIndex,fontSnrSubChar[index].buf,fontSnrSubChar[index].wt);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,fontSnrSubChar[index].buf+fontSnrSubChar[index].wt,fontSnrSubChar[index].wt);
		paraSetBufIndex+=(fontSnrSubChar[index].wt+1);
		str++;
	}

	if(DSP_REVERSE==rsv)
	{
		CalcBufMask(g_paraSetBuf,len*2);
		BufXOR(g_paraSetBuf,0x1f,len);
		BufXOR(g_paraSetBuf+len,0xfc,len);
		BufAngleCut(g_paraSetBuf,len,0x0f,0xf8);
	}

	AreaScroll(g_paraSetBuf, len, 16/8, len, SCROLL_DOWN, (x%8));

	LcdCopyToRam(x/8,y,g_paraSetBuf,len,16);	
*/
	
}

/*
void DspSnrSubStr(u8 x, u8 y, u8 len, u8 * str, u8 isSel, u8 rsv)
{
	u8 paraSetBufIndex=0;
	char index=0;
	memset(g_paraSetBuf,0x00,len*2);

	paraSetBufIndex += 3;
	
	if(isSel>0)
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSubArrow,4);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,picSnrSubArrow+4,4);
	}

	paraSetBufIndex+=7;

	while(*str!='\0')
	{
		index = SnrSubCharGetIndex(*str);
		memcpy(g_paraSetBuf+paraSetBufIndex,fontSnrSubChar[index].buf,fontSnrSubChar[index].wt);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,fontSnrSubChar[index].buf+fontSnrSubChar[index].wt,fontSnrSubChar[index].wt);
		paraSetBufIndex+=(fontSnrSubChar[index].wt+1);
		str++;
	}

	if(DSP_REVERSE==rsv)
	{
		CalcBufMask(g_paraSetBuf,len*2);
		BufXOR(g_paraSetBuf,0x7f,len);
		BufXOR(g_paraSetBuf+len,0xfe,len);
		BufAngleCut(g_paraSetBuf,len,0x3f,0xfc);
	}

	LcdCopyToRam(x,y,g_paraSetBuf,len,16);	
	
}
*/

void DspSnrSubStrEx(u8 x, u8 y, u8 len, u8 * str, u8 isSel, u8 rsv)
{
	u8 paraSetBufIndex=0;
	char index=0;
	memset(g_paraSetBuf,0x00,len*2);

	paraSetBufIndex += 3;
	
	if(isSel>0)
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSubArrowEx,4);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,picSnrSubArrowEx+4,4);
	}

	

	paraSetBufIndex+=7;

	while(*str!='\0')
	{
		index = SnrSubCharGetIndex(*str);
		memcpy(g_paraSetBuf+paraSetBufIndex,fontSnrSubChar[index].buf,fontSnrSubChar[index].wt);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,fontSnrSubChar[index].buf+fontSnrSubChar[index].wt,fontSnrSubChar[index].wt);
		paraSetBufIndex+=(fontSnrSubChar[index].wt+1);
		str++;
	}

	if(rsv>0)
	{
		CalcBufMask(g_paraSetBuf,len*2);
		BufXOR(g_paraSetBuf,0x7f,len);
		BufXOR(g_paraSetBuf+len,0xfe,len);
		BufAngleCut(g_paraSetBuf,len,0x3f,0xfc);
	}

	LcdCopyToRam(x,y,g_paraSetBuf,len,16);	
	
}


void SubSnrClear(void)
{
	LcdClearFeild(0,70,LCD_W-70,88);
}

void SubDsp_Apert(u8 sel, u8 org, u8 rsvId)
{
	//SubSnrClear();
	DspSnrSubStrEx(2,/*SNRSUB_CML*/SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,"!*@",org==0,(sel==0)&&(rsvId>0)/*ISSUB_SEL(0,rsvId)*/);
	DspSnrSubStrEx(4,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,"*@",org==1,(sel==1)&&(rsvId>0)/*ISSUB_SEL(1,rsvId)*/);
	DspSnrSubStrEx(6,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,"!@",org==2,(sel==2)&&(rsvId>0)/*ISSUB_SEL(2,rsvId)*/);
	DspSnrSubStrEx(8,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,"@",org==3,(sel==3)&&(rsvId>0)/*ISSUB_SEL(3,rsvId)*/);
}



void SubDsp_2(u8 sel, u8 org, u8 rsvId)
{
	//SubSnrClear();

	if(1==sel)
	{
		DisplayPicture(1, SNRSUB_ADJ(60), 60, 40, picSnrSub_2_bk, DSP_NORMAL);
	}else
	{
		memset(g_paraSetBuf,0x00,60*5);
		memcpy(g_paraSetBuf,picSnrSub_2_bk,60*5);
		memcpy(g_paraSetBuf+12,picSnrSub_2_msk,33);
		memcpy(g_paraSetBuf+60+12,picSnrSub_2_msk+33,33);
		LcdCopyToRam(1, SNRSUB_ADJ(60),g_paraSetBuf,60,40);		
	}
	//DspSnrSubStr(7,SNRSUB_ADJ(37),37,"ON",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(37),37,"OFF",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(37),0,37,"ON;OFF",sel,org,rsvId);
}

void SubDsp_3(u8 sel, u8 org, u8 rsvId)
{
	//SubSnrClear();

	DisplayPicture(1, SNRSUB_ADJ(33), 33, 16, picSnrSub_3_1, DSP_NORMAL);
	DisplayPicture(4, SNRSUB_ADJ(33), 33, 16, picSnrSub_3_2, DSP_NORMAL);

	if(0==sel)
	{
		DisplayPicture(2, SNRSUB_ADJ(33)+33+4, 5, 32, picSnrSub_3_3, DSP_NORMAL);
		//DisplayPictureEx(12, SNRSUB_ADJ(33)+33+4, 5, 32, picSnrSub_3_3, DSP_NORMAL);
	}else
	{
		DisplayPicture(2, SNRSUB_ADJ(33)+33+4, 5, 32, picSnrSub_3_4, DSP_NORMAL);
		//DisplayPictureEx(12, SNRSUB_ADJ(33)+33+4, 5, 32, picSnrSub_3_4, DSP_NORMAL);
	}

	//DspSnrSubStr(7,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,"1/3  EV",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,"   1 EV",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(SNR_SUBSTR_LEN),0,SNR_SUBSTR_LEN,"1/3 EV;  1 EV",sel,org,rsvId);
}

void SubDsp_SU4(u8 sel, u8 org, u8 rsvId)
{
	memset(g_paraSetBuf,0x00,60*5);
	memcpy(g_paraSetBuf,picSnrSub_2_bk,60*5);
	if(1==sel)
	{
		BufXOR(g_paraSetBuf+2*60, 0x00, 8);
		BufXOR(g_paraSetBuf+3*60, 0x00, 8);
		BufXOR(g_paraSetBuf+3*60-10, 0x00, 8);
		BufXOR(g_paraSetBuf+4*60-10, 0x00, 8);
	}
	LcdCopyToRam(1, SNRSUB_ADJ(60),g_paraSetBuf,60,40);	
	//DspSnrSubStr(7,SNRSUB_ADJ(56),56,"Advanced",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(56),56,"SU-4",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(56),0,56,"Advanced;SU-4",sel,org,rsvId);
}

void SubDsp_5(u8 sel, u8 org, u8 rsvId)
{
	if(0==sel)
	{
		DisplayPicture(2,SNRSUB_ADJ(15)/*(172-7-SNRSUB_CML-15)/2+SNRSUB_CML*/,15,32,picSnrSub_5_1,DSP_NORMAL);
	}else
	{
		DisplayPicture(2,SNRSUB_ADJ(24)/*(172-7-SNRSUB_CML-24)/2+SNRSUB_CML*/,24,32,picSnrSub_5_2,DSP_NORMAL);
	}
	//DspSnrSubStr(7,SNRSUB_ADJ(67),67,"FLASH",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(67),67,"MODELING",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(67),0,67,"FLASH;MODELING",sel,org,rsvId);
}

void SubDsp_6(u8 sel, u8 org, u8 rsvId)
{
	//DspSnrSubStr(5,SNRSUB_ADJ(56),56,"M 1/128",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(7,SNRSUB_ADJ(56),56,"M 1/32",org==1,ISSUB_SEL(1,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(56),56,"M 1/1",org==2,ISSUB_SEL(2,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_3,SNRSUB_ADJ(56),0,56,"M 1/128;M 1/32;M 1/1",sel,org,rsvId);
}

void SubDsp_7(u8 sel, u8 org, u8 rsvId)
{
	DisplayPicture(3, SNRSUB_ADJ(27)/*(172-7-27-SNRSUB_CML)/2+SNRSUB_CML*/, 27, 16, picSnrSub_7_1, DSP_NORMAL);

	if(1==sel)
	{
		DisplayPicture(2,SNRSUB_ADJ(27)/*(172-7-27-SNRSUB_CML)/2+SNRSUB_CML*/,15,16,picSnrFx,DSP_NORMAL);
	}else if(2==sel)
	{
		DisplayPicture(2,SNRSUB_ADJ(27)/*(172-7-27-SNRSUB_CML)/2+SNRSUB_CML*/,15,16,picSnrDx,DSP_NORMAL);
	}

	//DspSnrSubStr(5,SNRSUB_ADJ(57),57,"FX_DX",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(7,SNRSUB_ADJ(57),57,"FX",org==1,ISSUB_SEL(1,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(57),57,"DX",org==2,ISSUB_SEL(2,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_3, SNRSUB_ADJ(57), 0, 57, "FX_DX;FX;DX", sel, org,rsvId);
}

void SubDsp_ZoomManu(u8 sel, u8 org, u8 rsvId)
{
	DBG_PRINTF("SubDsp_8\r\n");
#if 1
	DisplayPicture(3, SNRSUB_ADJ(27)/*(172-7-27-SNRSUB_CML)/2+SNRSUB_CML*/, 27, 16, picSnrSub_7_1, DSP_NORMAL);
	if(0==sel)
	{
		DisplayPicture(2,SNRSUB_ADJ(27)+14/*(172-7-27-SNRSUB_CML)/2+SNRSUB_CML+14*/,13,16,picSnrSub_8_mask,DSP_NORMAL);
	}else
	{
		LcdClearFeild(3, SNRSUB_ADJ(27)/*(172-7-27-SNRSUB_CML)/2+SNRSUB_CML*/, 27, 8);
	}

	//DspSnrSubStr(7,SNRSUB_ADJ(37),37,"ON",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(37),37,"OFF",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(48,SNRSUB_ADJ(37),0,37,"ON;OFF",sel,org,rsvId);
#endif
}

void SubDsp_9(u8 sel, u8 org, u8 rsvId)
{
	if(0==sel)
	{	
		DisplayPicture(1,SNRSUB_ADJ(21),21,32,lightHead,DSP_NORMAL);
		DisplayPicture(3,SNRSUB_ADJ(21)-1,11,16,picSnrSub_9_af,DSP_NORMAL);
	}else if(1==sel)
	{
		DisplayPicture(1,SNRSUB_ADJ(21),21,32,lightHead,DSP_NORMAL);
	}else
	{
		DisplayPicture(1,SNRSUB_ADJ(21),25,32,picSnrSub_9_3,DSP_NORMAL);
	}                       
	//DspSnrSubStr(5,SNRSUB_ADJ(56),56,"ON",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(7,SNRSUB_ADJ(56),56,"OFF",org==1,ISSUB_SEL(1,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(56),56,"AF ONLY",org==2,ISSUB_SEL(2,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_3,SNRSUB_ADJ(56),0,56,"ON;OFF;AF ONLY",sel,org,rsvId);
}

void SubDsp_10(u8 sel, u8 org, u8 rsvId)
{
	//DspSnrSubStr(0,SNRSUB_ADJ(43),43,"AUTO",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(2,SNRSUB_ADJ(43),43," 40",org==1,ISSUB_SEL(1,rsvId));
	//DspSnrSubStr(4,SNRSUB_ADJ(43),43," 80",org==2,ISSUB_SEL(2,rsvId));
	//DspSnrSubStr(6,SNRSUB_ADJ(43),43,"160",org==1,ISSUB_SEL(3,rsvId));
	//DspSnrSubStr(8,SNRSUB_ADJ(43),43,"300",org==2,ISSUB_SEL(4,rsvId));
	//DspSnrSub",sel,rsvId);
	DspSnrSubStr(SNRSUB_PAGE_START_6,SNRSUB_ADJ(43),0,43,"AUTO;##40;##80;#160;#300;#---",sel,org,rsvId);
}

//void SnrSubIsoDsp(int iso, u8 *buf,u8 rsv)
void SnrSubIsoDsp(u16 iso, u8 *buf,u8 rsv)
{
	memset(buf,'#',5);
	if(iso>=1000)
	{
		buf[0] = iso/1000+'0';
		iso %= 1000;
		buf[1] = iso/100 +'0';
		iso %= 100;
		buf[2] = iso/10 + '0';
		buf[3] = iso%10 + '0';
	}else if(iso>=100)
	{
		buf[1] = iso/100 +'0';
		iso %= 100;
		buf[2] = iso/10 + '0';
		buf[3] = iso%10 + '0';
	}else if(iso>=10)
	{
		buf[2] = iso/10 + '0';
		buf[3] = iso%10+'0';
	}else
	{
		buf[3] = iso+'0';
	}
	
}

void SubDsp_Iso(u8 sel, u8 org, u8 rsvId)
{
	//u8 buf[5] = {0};
	u8 mIso=0;
	u16 bufIndex=0;
	u8 i = 0;
	u8 dsStart=0;
	u8 *pIsoStr = (u8*)(g_paraSetBuf+1024);
	//mIso = WordSearch((int*)isoValue, 35, mainPara.extIso);
	mIso = sel;

	memset(pIsoStr,0x00,1024);
	//memset(g_paraSetBuf,0x00,9*43);//8+1
	//memcpy(pIsoStr+19,picSnrSub_isoUP,5);
	DisplayPicture(1, SNRSUB_ADJ(43)+19,5, 8, picSnrSub_isoUP, DSP_NORMAL);

	if(mIso>32)
	{
		dsStart = mIso-2;
		if(org<2)
		{
			org+=35;
		}
		//sel+=35;
		
		for(i=mIso-2;i<35;i++)
		{
			SnrSubIsoDsp(isoValue[i],pIsoStr+bufIndex,0);
			bufIndex+=5;
			pIsoStr[bufIndex++] = ';';
			
			//DspSnrSubStr(1+(i-(mIso-2)*2),SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(rows,SNRSUB_ADJ(43),6,43,buf,2,rsvId);
			//rows+=13;
		}
		for(i=0;i<mIso-32;i++)
		{
			SnrSubIsoDsp(isoValue[i],pIsoStr+bufIndex,0);
			bufIndex+=5;
			pIsoStr[bufIndex++] = ';';
			//DspSnrSubStr(7-(mIso-32)*2*i,SNRSUB_CML,SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(i*2+(9-(mIso-32)*2),SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(rows,SNRSUB_ADJ(43),6,43,buf,2,rsvId);
			//rows+=13;
			
		}		
	}else if(mIso<2)
	{
		dsStart = 33+mIso;
		if(org<3)
		{
			org+=35;
		}
		sel+=35;
		
		for(i=33+mIso;i<35;i++)
		{
			SnrSubIsoDsp(isoValue[i],pIsoStr+bufIndex,0);
			bufIndex+=5;
			pIsoStr[bufIndex++] = ';';
			//SnrSubIsoDsp(isoValue[i],buf,0);
			//DspSnrSubStr(7-(mIso-32)*2*i,SNRSUB_CML,SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr((i-(33+mIso))*2+1,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(rows,SNRSUB_ADJ(43),6,43,buf,2,rsvId);
			//rows+=13;
		}
		for(i=0;i<mIso+3;i++)
		{
			SnrSubIsoDsp(isoValue[i],pIsoStr+bufIndex,0);
			bufIndex+=5;
			pIsoStr[bufIndex++] = ';';
			//SnrSubIsoDsp(isoValue[i],buf,0);
			//DspSnrSubStr(7-(mIso-32)*2*i,SNRSUB_CML,SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(i*2+(5-mIso*2),SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(rows,SNRSUB_ADJ(43),6,43,buf,2,rsvId);
			//rows+=13;
		}
	}else
	{
		dsStart = mIso-2;
		
		for(i=mIso-2;i<mIso+3;i++)
		{
			//SnrSubIsoDsp(isoValue[i],buf,0);
			//DspSnrSubStr((i-(mIso-2))*2,SNRSUB_ADJ(SNR_SUBSTR_LEN),SNR_SUBSTR_LEN,buf,sel==2,0);
			//DspSnrSubStr(rows,SNRSUB_ADJ(43),6,43,buf,2,rsvId);
			//rows+=13;
			SnrSubIsoDsp(isoValue[i],pIsoStr+bufIndex,0);
			bufIndex+=5;
			pIsoStr[bufIndex++] = ';';
		}
	}

	//memcpy(pIsoStr+9*43+19,picSnrSub_isoDN,5);

	pIsoStr[bufIndex-1] = '\0';

	DspSnrSubStr(16,SNRSUB_ADJ(43),dsStart,43,pIsoStr,sel,org,rsvId);

	DisplayPicture(10, SNRSUB_ADJ(43)+19,5, 8, picSnrSub_isoDN, DSP_NORMAL);
	
}


void SubDsp_12(u8 sel, u8 org, u8 rsvId)
{
	DisplayPicture(1,SNRSUB_ADJ(21),21,32,lightHead,DSP_NORMAL);
	//DspSnrSubStr(5,SNRSUB_ADJ(71),71,"REAR.FRONT",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(7,SNRSUB_ADJ(71),71,"REAR",org==1,ISSUB_SEL(1,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(71),71,"FRONT",org==2,ISSUB_SEL(2,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_3,SNRSUB_ADJ(71),0,71,"REAR.FRONT;REAR;FRONT",sel,org,rsvId);
}

void SubDsp_13(u8 sel, u8 org, u8 rsvId)
{
	DisplayPicture(1,SNRSUB_ADJ(21),21,32,lightHead,DSP_NORMAL);
	if(0==sel)
	{
		DisplayPicture(3,SNRSUB_ADJ(21)+21+1,2,16,picSnrSub_13_bk,DSP_NORMAL);
	}
	//DspSnrSubStr(7,SNRSUB_ADJ(37),37,"ON",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(37),37,"OFF",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(37),0,37,"ON;OFF",sel,org,rsvId);
}

#define CONTRAST_BAR_LEN 26

void SubDsp_14(u8 sel, u8 org, u8 rsvId)
{
	u8 row=0,line=0;
	u8 bias=0;
	u8 paraSetBufIndex = 0;
	u8 pfont[26];
	
	memset(g_paraSetBuf,0x00,CONTRAST_BAR_LEN*9);
	memcpy(g_paraSetBuf+12,picSnrSub_13_1,5);
	paraSetBufIndex+=CONTRAST_BAR_LEN;

	for(row=0;row<9;row++)
	{
		if(row<(8-sel))
		{
			//pfont = (u8*)picSnrSub_13_2;
			memcpy(pfont,picSnrSub_13_2,26);
		}else if(row>(8-sel))
		{
			//pfont = (u8*)picSnrSub_13_3;
			memcpy(pfont,picSnrSub_13_3,26);
		}else
		{
			//pfont = (u8*)picSnrSub_13_4; 	
			//if(rsvId&0x80)
			if(rsvId>0)
			{
				memcpy(pfont,picSnrSub_13_selected,26);
			}else
			{
				memcpy(pfont,picSnrSub_13_3,26);
			}
		}

		if(row == (8-org))
		{
			memcpy(pfont,picSnrSub_13_4,4);
		}
		
		if(bias>2)
		{
			for(line=0;line<CONTRAST_BAR_LEN;line++)
			{
				g_paraSetBuf[paraSetBufIndex+line] |= (pfont[line]>>bias);
			}

			paraSetBufIndex+=CONTRAST_BAR_LEN;

			for(line=0;line<CONTRAST_BAR_LEN;line++)
			{
				g_paraSetBuf[paraSetBufIndex+line] |= (pfont[line]<<(8-bias));
			}

			bias-=2;
			
		}else
		{
			for(line=0;line<CONTRAST_BAR_LEN;line++)
			{
				g_paraSetBuf[paraSetBufIndex+line] |= (pfont[line]>>bias);
			}

			bias+=6;

			if(bias>=8)
			{
				bias-=8;
				paraSetBufIndex+=CONTRAST_BAR_LEN;
			}
		}		

	}

	for(line=0;line<5;line++)
	{
		g_paraSetBuf[8*CONTRAST_BAR_LEN+12+line]|=0x20;
	}

	LcdCopyToRam(1, SNRSUB_ADJ(CONTRAST_BAR_LEN), g_paraSetBuf, CONTRAST_BAR_LEN, 72);

	//SetContrast(sel);
	
}

void SubDsp_15(u8 sel, u8 org, u8 rsvId)
{
	u8 paraSetBufIndex=3;
	memset(g_paraSetBuf,0x00,37*2);

	if(0==sel)
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSubArrowEx,4);
		memcpy(g_paraSetBuf+paraSetBufIndex+37,picSnrSubArrowEx+4,4);
	}

	paraSetBufIndex+=7;

	memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSub_14_1,10);
	memcpy(g_paraSetBuf+paraSetBufIndex+37,picSnrSub_14_1+10,10);

	//if(1==rsvId)
	//if(ISSUB_SEL(0,rsvId))
	if((sel==0)&&(rsvId>0))
	{
		CalcBufMask(g_paraSetBuf,37*2);
		BufXOR(g_paraSetBuf,0x7f,37);
		BufXOR(g_paraSetBuf+37,0xfc,37);
		BufAngleCut(g_paraSetBuf,37,0x3f,0xf8);
	}

	LcdCopyToRam(7, SNRSUB_ADJ(37), g_paraSetBuf, 37, 16);

	memset(g_paraSetBuf,0x00,37*2);
	paraSetBufIndex=3;	

	if(1==sel)
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSubArrowEx,4);
		memcpy(g_paraSetBuf+paraSetBufIndex+37,picSnrSubArrowEx+4,4);
	}

	paraSetBufIndex+=7;

	memcpy(g_paraSetBuf+paraSetBufIndex,picSnrSub_14_2,13);
	memcpy(g_paraSetBuf+paraSetBufIndex+37,picSnrSub_14_2+13,13);

	//if(1==rsvId)
	//if(ISSUB_SEL(1,rsvId))
	if((sel==1)&&(rsvId>0))
	{
		CalcBufMask(g_paraSetBuf,37*2);
		BufXOR(g_paraSetBuf,0x7f,37);
		BufXOR(g_paraSetBuf+37,0xfc,37);
		BufAngleCut(g_paraSetBuf,37,0x3f,0xf8);
	}

	LcdCopyToRam(9, SNRSUB_ADJ(37), g_paraSetBuf, 37, 16);

}

void SubDsp_16(u8 sel, u8 org, u8 rsvId)
{
	if(0==sel)
	{
		DisplayPicture(4, SNRSUB_ADJ(62), 20, 8, picSnrSub_16_zoom, DSP_NORMAL);
		DisplayPicture(3, SNRSUB_ADJ(62)+24, 38, 16, picSnrSub_16_wpon, DSP_NORMAL);
	}else
	{
		DisplayPicture(3, SNRSUB_ADJ(62)+13, 7, 8, picSnrSub_16_M, DSP_NORMAL);
		DisplayPicture(4, SNRSUB_ADJ(62), 20, 8, picSnrSub_16_zoom, DSP_NORMAL);
		DisplayPicture(3, SNRSUB_ADJ(62)+24,35, 16, picSnrSub_16_wpoff, DSP_NORMAL);
	}

	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(37),0,37,"ON;OFF",sel,org,rsvId);
}

void SubDsp_17(u8 sel, u8 org, u8 rsvId)
{
	switch(sel)
	{
		case 0:
			DisplayPicture(1,SNRSUB_ADJ(16),16,24,picSnrSub_17_1,DSP_NORMAL);
		break;

		case 1:
			DisplayPicture(1,SNRSUB_ADJ(16),20,24,picSnrSub_17_2,DSP_NORMAL);
		break;

		case 2:
			DisplayPicture(2,SNRSUB_ADJ(13),13,16,picSnrSub_17_3,DSP_NORMAL);
		break;
	}
	
	//DspSnrSubStr(5,SNRSUB_ADJ(61),61,"FULL",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(7,SNRSUB_ADJ(61),61,"MY MENU",org==1,ISSUB_SEL(1,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(61),61,"SET UP",org==2,ISSUB_SEL(2,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_3,SNRSUB_ADJ(61),0,61,"FULL;MY MENU;SET UP",sel,org,rsvId);
}

void SubDsp_18(u8 sel, u8 org, u8 rsvId)
{
	//DspSnrSubStr(5,SNRSUB_ADJ(44),44,"7.001",org==1,0);
	DspSnrSubStr(SNRSUB_PAGE_START_3,SNRSUB_ADJ(44),0,44,"7.001",1,org,rsvId);
}

void SubDsp_Reset(u8 sel, u8 org, u8 rsvId)
{
	//DspSnrSubStr(7,SNRSUB_ADJ(37),37,"YES",org==0,ISSUB_SEL(0,rsvId));
	//DspSnrSubStr(9,SNRSUB_ADJ(37),37,"NO",org==1,ISSUB_SEL(1,rsvId));
	DspSnrSubStr(SNRSUB_PAGE_START_2,SNRSUB_ADJ(37),0,37,"YES;NO",sel,3,rsvId);
}

void SmHandle_ReflectPad(u8 sel)
{
	if(0==sel)
	{
		*motoSet.dest_location = g_flectDestRsv;
		motoSet.destRsv = *motoSet.dest_location;
	}else
	{
		g_flectDestRsv = *motoSet.dest_location;
		*motoSet.dest_location = 14;
		motoSet.destRsv = *motoSet.dest_location;
	}
}

void SnrProcessBar(u8 pa, u8 cl,u8 foc, u8 cnt)
{
	u8 bias =0;
	u8 i=0,y=0,x=0;
	u8 dots = 0;

//	u8 *pBuf = (u8*)(g_paraSetBuf+6*11);

//	DBG_PRINTF("foc: %d, cnt: %d\r\n", foc, cnt);
	
	memset(g_paraSetBuf,0x00,6*13);
//	memset(pBuf,0x00,6*11);

/*	
	for(i=0;i<6;i++)
	{
		g_paraSetBuf[i] |= (0x80);
		//g_paraSetBuf[i+72] |= 0x08;
	}

	g_paraSetBuf[0] |= (0xff);
	g_paraSetBuf[5] |= (0xff);

	for(i=1;i<10;i++)
	{
		g_paraSetBuf[i*6] |= 0xff;
		g_paraSetBuf[i*6+5] |= 0xff;
	}
*/

	dots = 76/((cnt+3)/4);
	x = dots*foc;
	y = (x/8)*6;

//	DBG_PRINTF("dots: %d\r\n",dots);

	while(dots/8)
	{
		for(i=0;i<6;i++)
		{
			g_paraSetBuf[y+i] |= 0xff;
		}
		y+=6;
		dots-=8;
	}

	if(dots>0)
	{
		for(i=0;i<6;i++)
		{
			g_paraSetBuf[y+i] |= (0xff<<(8-dots));
		}
	}

//	DBG_PRINTF("foc*dots %d\r\n",x);

	BufferScroll((u8*)g_paraSetBuf, 6, 11, SCROLL_DOWN,x%8);

	for(i=0;i<6*11;i++)
	{
		//DBG_PRINTF("%x ",pBuf[i]);
		if(((i+1)%6)==0)
		{
			//DBG_PRINTF("\r\n");
		}
		g_paraSetBuf[i] |= picSnrFrame[i];
	}

/*

	for(i=0;i<6*11;i++)
	{
		DBG_PRINTF("%x ",pBuf[i]);
		if(((i+1)%6)==0)
		{
			DBG_PRINTF("\r\n");
		}
		g_paraSetBuf[i] |= pBuf[i];
	}

	for(i=0;i<6;i++)
	{
		//g_paraSetBuf[i] |= (0x80);
		g_paraSetBuf[i+72] |= 0x08;
	}
*/

	BufferScroll((u8*)g_paraSetBuf,6,11,SCROLL_DOWN,bias);

	LcdCopyToRam(pa,cl,g_paraSetBuf,6,80);
	
}

void SubSetProcExit(void)
{
	g_SettingIndex = SETTING_PFNMENU;
	pSnrSet[g_snrIndex] = g_snrSubIndex;
	//DspSnrMenu();
	//DspSnrMenu();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
}

void SubSetProc_Apert(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_2(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_3(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_4(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_5(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_6(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_7(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_8(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_Af(u8 sel)
{
	//SET_UPDATE_EVT(DSP_UPDATE_MODE);
	SetSpiComData();
	SubSetProcExit();
}

void SubSetProc_10(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_Iso(u8 sel)
{
	mainPara.extIso = isoValue[pSnrSet[SNR_ISO]];        //set iso value
	SubSetProcExit();
}

void SubSetProc_12(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_13(u8 sel)
{
	SubSetProcExit();
	BkLiteOn();
}

void SubSetProc_14(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_15(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_16(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_17(u8 sel)
{
	if(2==sel)
	{
		g_SettingIndex = SETTING_MYMENU;
		DspMyMenuSelect();
		//SET_UPDATE_EVT(DSP_UPDATE_MODE);
		
	}else
	{
		SubSetProcExit();
	}
}

void SubSetProc_18(u8 sel)
{
	SubSetProcExit();
}

void SubSetProc_Reset(u8 sel)
{
	if(!pSnrSet[SNR_RESET])
	{
		DisplayPicture(4, 40, 93, 32,picReset, DSP_NORMAL);
		memset(pSnrSet,0x00,19);
		g_snrMyMenuBits = 0xffffffff;
		delay_ms(1000);
		//delay_ms(1000);
		SET_UPDATE_EVT(DSP_NORMAL);
		pSnrSet[SNR_RESET] = 1;
	}
	SubSetProcExit();
}

