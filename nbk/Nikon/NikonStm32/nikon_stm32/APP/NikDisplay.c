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
//#include "math.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

FuncMenu funcMenu;
unsigned char g_menuId=0;
u16 g_dspUpdateFlag;
extern FuncList *pMenuList;
u8 g_allMyMenuSw = 0;
extern u8 g_isSlaveVoice;
extern u8 g_isReflectPulled;                         //inner reflect board
u8 g_flashMode = 0;
u8 g_cmrFxOrDx = 0;
u8 g_isSoftLightCoverOn=0;
u8 g_dspCmpMod = 0;
extern char g_mstSetRsv;

#define LOCATION_TTLCMP_CML 104

#define SCALE_NUM_MIN  6
#define SCALE_NUM_MAX 200

u8 nikMaxCalc[][14] = {
{90,90,80,70,56,44,36,32,28,24},    // 1/128 , +0.3 +0.7 1/64
{78,78,64,52,40,36,30,28,26,18},            // 1/64
{68,68,56,44,36,32,26,24,22,14},            // 1/32
{60,60,50,40,32,28,24,22,20,12},                        // 1/16
{46,46,30,30,30,26,14,14,10,10},                             // 1/8
{36,36,24,24,24,24,12,12,9,9},                                    // 1/4
{30,30,20,20,20,20,10,10,8,8},
{22,18,14,12,10,10,8,8,6,6},
{16,14,12,10,7,7,6,6,5,5},
{14,12,10,8,6,6,5,5,4,4},
};

const u16 apertValue[] = 
{
	14,16,18,20,22,25,28,32,35,40,
	45,50,56,63,71,80,90,100,110,130,
	140,	160,180,200,220,250,290,320,360,400,
	450,510,570,640
};

const u16 cmrApertVal[] = {
8,	10, 12, 14, 16,	
18,	20,	22, 24, 25, 28,	32,
33,	35,	38,	40,	42,	45,	48,	50,	53,	56,
63,	67,	71,	80,	90,	95,	100,110,130,130,140,
160,180,190,200,220,250,270,290,320,360,380
};

//const u16 isoValue[] = 
const u32 isoValue[] = 
{
	3,4,5,6,8,10,12,16,20,25,
	32,40,50,64,80,100,125,160,200,250,
	320,400,500,640,800,1000,1250,1600,2000,2500,
	3200,4000,5000,6400,8000,
//    the following is not in menu
	10000,12800,16000,20000,25600,
	32000,40000,51200,64000,81200,102400,128000,160000,	204800,250000,
	320000,409600,500000,640000,820000,1000000,1280000,1600000,
	
};


//fx
const u8 gn_min[][8] = 
{
	//ISO 100 APERT 4
	{6,6,6,6,9,12,17,23},           	// 14BA/WP     
	{6,6,6,8,10,15,20,29},         	// 14BA
	{6,6,6,8,11,16,22,31},         	// 14WP
	{6,6,7,10,14,20,28,39},      	// 17
	{6,6,8,11,15,21,29,41},     	// 18
	{6,6,8,11,15,22,30,43},     	// 20
	{6,6,9,12,17,24,34,48},		// 24
	{6,7,10,14,19,27,38,54},	// 28
	{6,8,11,16,22,31,43,61},	// 35
	{7,9,13,18,25,36,50,71},	// 50
	{7,10,14,20,28,39,55,78},	// 70
	{8,11,15,21,29,41,58,82},	// 85
	{8,11,16,22,31,44,62,87},	// 105
	{8,12,16,23,32,45,64,90},	// 120
	{8,12,16,23,32,46,64,91},	// 135
	{9,12,17,23,33,46,65,92},	// 180
	{9,12,17,24,34,47,67,94},	// 200
};

const u8 gn_max[][12] = 
{
	{6,6,8,11,16,22,32,45,65,91,130,180},				// 14BA/WP    ISO 100 APERT 4
	{6,7,9,14,20,28,40,56,80,113,160,200},			// 14BA
	{6,7,10,15,21,30,42,60,85,120,170,200},			// 14WP
	{6,9,13,19,27,38,55,77,110,155,200,200},			// 17
	{7,10,14,20,28,40,57,81,115,162,200,200},			// 18
	{7,10,15,21,30,42,60,84,120,169,200,200},			// 20
	{8,11,16,23,33,47,67,95,135,190,200,200},			// 24
	{9,13,18,26,37,53,75,106,150,200,200,200},		// 28
	{10,15,21,30,42,60,85,120,170,200,200,200},		// 35
	{12,17,25,35,50,70,100,141,200,200,200,200},		// 50
	{13,19,27,38,55,77,110,155,200,200,200,200},		// 70
	{14,20,28,40,57,81,115,162,200,200,200,200},		// 85
	{15,21,30,43,61,86,122,173,200,200,200,200},		// 105
	{15,22,31,44,63,89,126,178,200,200,200,200},		// 120
	{15,22,31,45,63,90,127,180,200,200,200,200},		// 135
	{16,22,32,45,63,90,127,183,200,200,200,200},		// 180
	{16,23,33,46,66,93,132,187,200,200,200,200},		// 200
};

const u16 g_gnByExp[][8] = 
{
	{11,16,22,32,45,65,91,130},				// 14BA/WP    ISO 100 APERT 4
	{14,20,28,40,56,80,113,160},			// 14BA
	{15,21,30,42,60,85,120,170},			// 14WP
	{19,27,38,55,77,110,155,220},			// 17
	{20,28,40,57,81,115,162,230},			// 18
	{21,30,42,60,84,120,169,240},			// 20
	{23,33,47,67,95,135,190,270},			// 24
	{26,37,53,75,106,150,212,300},		// 28
	{30,42,60,85,120,170,240,340},		// 35
	{35,50,70,100,141,200,282,400},		// 50
	{38,55,77,110,155,220,311,440},		// 70
	{40,57,81,115,162,230,325,460},		// 85
	{43,61,86,122,173,245,346,490},		// 105
	{44,63,89,126,178,252,357,505},		// 120
	{45,63,90,127,180,255,360,510},		// 135
	{45,65,91,130,183,260,367,520},		// 180
	{46,66,93,132,187,265,374,530},		// 200
};

const u16 g_gnByExpHp[][8] = 
{
	{4,5,8,11,16,23,33,47},				// 14BA/WP    ISO 100 APERT 4
	{5,7,10,14,20,29,41,58},			// 14BA
	{5,7,10,15,21,31,43,62},			// 14WP
	{7,10,14,20,28,40,57,81},			// 17
	{7,10,15,21,30,42,60,85},			// 18
	{7,11,15,22,31,44,62,88},			// 20
	{8,12,17,25,35,50,70,100},			// 24
	{9,13,19,27,39,55,78,111},		// 28
	{11,15,22,31,44,63,89,126},		// 35
	{13,18,26,37,52,74,104,148},		// 50
	{14,20,28,40,57,81,115,163},		// 70
	{15,21,30,42,60,85,120,170},		// 85
	{15,22,31,45,63,90,127,181},		// 105
	{16,23,33,46,66,93,132,187},		// 120
	{16,23,33,47,66,94,133,189},		// 135
	{16,24,33,48,67,96,135,192},		// 180
	{17,24,34,49,69,98,138,196},		// 200
};

//char const focLenScape[14] =  {17,18,20,24,28,35,50,70,85,105,120,135,180,200};
//extern char const focLenScape[11];
void KeyMenuSw(void);

void *BtnZoom(void);
void *BtnM(void);
void *BtnFno(void);
void *BtnPilot(void);
void *BtnSel(void);
void *BtnMsc(void);
void *BtnCh(void);
void *BtnCmp(void);
void *BtnHead(void);
void *Btnm(void);
void *BtnTimesHz(void);
void *BtnPaup(void);
void *BtnPaDn(void);
void *BtnSnrSet(void);
void *BtnSnrNext(void);
void *BtnMyMenuBack(void);
void *BtnMyMenuSel(void);
void *BtnZoomBack(void);
void *BtnFlashMode(void);
void *BtnFlashWeakOn(void);
void *BtnFlashWeakOff(void);
void *BtnNullBack(void);
//void *BtnFlashWeak(void);
//extern void DspSnrMenu(void);

Button btnNull = {"",0,{NULL,NULL,NULL}};
Button btnNullBack = {"",0,{NULL,BtnNullBack,NULL}};
//Button btnZoom = {(char*)picBtnZoom,25,{BtnZoom,DspSnrMenu}};
Button btnZoom = {(char*)picBtnZoom,25,{NULL,BtnZoom,NULL}};
Button btnZoomBack = {(char*)picBtnZoomBack,26,{NULL,BtnZoomBack,NULL}};
Button btnM = {(char*)picBtn_M,7,{NULL,BtnM,NULL}};
Button btnFno = {(char*)picBtnFno,17,{NULL,BtnFno,NULL}};
Button btnPilot = {(char*)picBtnFlash,7,{NULL,BtnPilot,NULL}};
Button btnModeling = {(char *)picBtnModeling,9,{NULL,BtnPilot,NULL}};
Button btnSel = {(char*)picBtnSel,17,{NULL,BtnSel,NULL}};
Button btnMsc = {(char*)picBtnMsc,9,{NULL,BtnMsc,NULL}}; 
Button btnCh = {(char*)picBtnCh,11,{NULL,BtnCh,NULL}};
Button btnCmp = {(char*)picBtnCmp,16,{NULL,BtnCmp,NULL}};
Button btnHead = {(char*)picBtnHead,13,{NULL,BtnHead,NULL}};
Button btnm = {(char*)picBtn_m,10,{NULL,Btnm,NULL}};
Button btnft = {(char*)picBtn_ft,13,{NULL,Btnm,NULL}};
Button btnTimesHz = {(char*)picBtnTimesHz,35,{NULL,BtnTimesHz,NULL}};
Button btnPaUp = {(char*)picBtnPgUp,7,{NULL,BtnPaup,NULL}};
Button btnPaDn = {(char*)picBtnPgDn,7,{NULL,BtnPaDn,NULL}};
Button btnSnrSet_all = {(char*)picBtnSnrSet_ALL,28,{NULL,BtnSnrSet,NULL}};
Button btnSnrSet_my = {(char*)picBtnSnrSet_My,28,{NULL,BtnSnrSet,NULL}};
Button btnSnrNext = {(char*)picBtnNext,7,{NULL,BtnSnrNext,NULL}};
Button btnMyMenuBack = {(char*)picSnrSubBack,11,{NULL,BtnMyMenuBack,NULL}};
Button btnMyMenuSel = {(char*)picSnrSubMyMenuSel,22,{NULL,BtnMyMenuSel,NULL}};
Button btnFlashMode = {(char*)picBtnFlashMode,13,{NULL,BtnFlashMode,NULL}};
Button btnFlashWeak = {(char*)picBtnFlashWeak,9,{BtnFlashWeakOn,BtnFlashWeakOff,NULL}};

void TtlAutoDrawScale(char isIsoAuto, s8 cmp1, s8 cmp2);
void GrDrawScale(void);
//char WordSearch(u16* srcArr,char size,u16 cd);

void LoadModIcon(u8 page,u8 column,u8 wide,const char *p,u8 rsv)
{
	u16 i=0;
	if(DSP_NORMAL==rsv)
	{
		//memset(g_paraSetBuf,0x00,LAYOUT_RATIO_WIDTH*2);
		memset(g_paraSetBuf,0x00,wide*2);
		memcpy(g_paraSetBuf,p,wide*2);
		for(i=0;i<wide;i++)
		{
			g_paraSetBuf[i]|=0x80;
			g_paraSetBuf[i+wide]|=0x01;
		}
		g_paraSetBuf[0] = g_paraSetBuf[wide] = 0xff;
		g_paraSetBuf[wide-1] = g_paraSetBuf[wide*2-1] = 0xff;
		
	}else
	{
		memset(g_paraSetBuf,0xff,wide*2);
		CalcArrayMask((char*)g_paraSetBuf, (char*)p, wide*2);
	}

/*
	AreaSet(0, 0, wide, 16);

	for(i=0;i<wide*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
*/
	LcdCopyToRam(page,column,g_paraSetBuf,wide,16);
}

void DspResModIcon(s8 mod, u8 y,u8 rsv)
{
	switch(mod)
	{
		case WORKINGMODE_TTL:
			LoadModIcon(0,y, 31, pic_mod_ttl, rsv);
		break;

		case WORKINGMODE_AUTO:
			/*if(cmrPara.cls>0)
			{
				LoadModIcon(0,y, 28, pic_mod_ak, rsv);
			}else
			{
				LoadModIcon(0,y, 17, pic_mod_a, rsv);
			}*/
		break;

		case WORKINGMODE_GN:
			LoadModIcon(0,y, 25, pic_mod_gn, rsv);
		break;

		case WORKINGMODE_MANU:
			LoadModIcon(0,y, 19, pic_mod_m, rsv);
		break;

		case WORKINGMODE_RPT:
			LoadModIcon(0,y, 34, pic_mod_rpt, rsv);
		break;
	}
}

void DspClsFlash(void)
{
	DisplayPicture(0, 0, 15, 16, picLinkFla, DSP_NORMAL);
}

void DspSu4ResModIcon(s8 mod, u8 rsv)
{
	DisplayPicture(0,0,16,16,picMstIcon,DSP_NORMAL);
	DisplayPicture(0,16+1,33,16,fontSu4,DSP_NORMAL);
	switch(mod)
	{
		case WORKINGMODE_TTL:
			LoadModIcon(0,16+1+33+1, 31, pic_mod_ttl, rsv);
		break;

		case WORKINGMODE_AUTO:
			if(cmrPara.cls>0)
			{
				LoadModIcon(0,16+1+33+1, 28, pic_mod_ak, rsv);
			}else
			{
				LoadModIcon(0,16+1+33+1, 17, pic_mod_a, rsv);
			}
		break;

		case WORKINGMODE_GN:
			LoadModIcon(0,16+1+33+1, 25, pic_mod_gn, rsv);
		break;

		case WORKINGMODE_MANU:
			LoadModIcon(0,16+1+33+1, 19, pic_mod_m, rsv);
		break;

		case WORKINGMODE_RPT:
			LoadModIcon(0,16+1+33+1, 34, pic_mod_rpt, rsv);
		break;
	}
}

void DspManuExp1(u8 x,u8 y,s8 exp1, s8 exp2, u8 dspMod,u8 rsv)
{
	u8 i=0;
	u8 dspChar = 128;
	u8 bufIndex=0;
	u8 len = 0;
	
	while(i<exp1)
	{
		dspChar/=2;
		i++;
	}

	memset(g_paraSetBuf,0x00,6*9*2);

	if(!exp1)
	{
		len = 9+8+9*3+2 ;
	}else if(exp1<4)
	{
		len = 9+8+9*2+2 ;
	}else
	{
		len = 9+8+9*1+2;
	}

	bufIndex++;

	memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16],8);
	memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16+8],8);

	bufIndex+=9;

	memcpy(g_paraSetBuf+bufIndex,&font_mExpApart,7);
	memcpy(g_paraSetBuf+bufIndex+len,&font_mExpApart[7],7);

	bufIndex+=8;

	if(!exp1)
	{
		memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16],8);
		memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16+8],8);

		bufIndex+=9;

		memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16*2],8);
		memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16*2+8],8);

		bufIndex+=9;

		memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16*8],8);
		memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16*8+8],8);
		
	}else if(exp1<4)
	{
		memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16*(dspChar/10)],8);
		memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16*(dspChar/10)+8],8);

		bufIndex+=9;

		memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16*(dspChar%10)],8);
		memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16*(dspChar%10)+8],8);
		
	}else
	{
		memcpy(g_paraSetBuf+bufIndex,&font_apertNum[16*dspChar],8);
		memcpy(g_paraSetBuf+bufIndex+len,&font_apertNum[16*dspChar+8],8);
	}

	if(DSP_REVERSE==rsv)
	{
		for(i=0;i<len;i++)
		{
			g_paraSetBuf[i] = ~g_paraSetBuf[i];
			g_paraSetBuf[i+len] = ~g_paraSetBuf[i+len];
		}

		g_paraSetBuf[0] &= 0x7f;
		g_paraSetBuf[len-1] &= 0x7f;
		g_paraSetBuf[len] &= 0xfe;
		g_paraSetBuf[2*len-1] &= 0xfe;
	}

	AreaSet(x, y, len, 16);

	for(i=0;i<len*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}

}

s8 CalcExp2Ret(s8 exp2)
{
	switch(exp2)
	{
		case -1:
		return -3;
		case -2:
		return -7;
		case 1:
		return 3;
		case 2:
		return 7;
		default:
		return 0;
	}
}


void DspManuExp2OrCmp(u8 x,u8 y,s8 exp1, s8 exp2, u8 dspMod,u8 isFlashWeak,u8 rsv)
{
	u8 bufIndex=0;
	u8 i=0;

	if(0==dspMod)
	{
		if((!exp1)&&(!exp2)&&(DSP_NORMAL==rsv))
		{
			return;
		}
	}

	bufIndex+=2;

	memset(g_paraSetBuf,0x00,41*2);

	if((exp1<0)||(exp2<0))
	{
		memcpy(g_paraSetBuf+bufIndex,&picCmpSub[0],5);
		memcpy(g_paraSetBuf+bufIndex+41,&picCmpSub[5],5);
	}else if((exp1>0)||(exp2>0))
	{
		memcpy(g_paraSetBuf+bufIndex,&picCmpAdd[0],5);
		memcpy(g_paraSetBuf+bufIndex+41,&picCmpAdd[5],5);
	}

	bufIndex+=7;

	if(exp1<0)
	{
		exp1 = -1*exp1;
	}

	if(exp2<0)
	{
		exp2 = -1*exp2;
	}

	memcpy(g_paraSetBuf+bufIndex,&font_exp2Num[exp1*12],6);
	memcpy(g_paraSetBuf+bufIndex+41,&font_exp2Num[exp1*12+6],6);

	bufIndex+=7;

	memcpy(g_paraSetBuf+bufIndex,&picExp2Dot[0],2);	
	memcpy(g_paraSetBuf+bufIndex+41,&picExp2Dot[2],2);

	bufIndex+=3;

	memcpy(g_paraSetBuf+bufIndex,&font_exp2Num[exp2*12],6);
	memcpy(g_paraSetBuf+bufIndex+41,&font_exp2Num[exp2*12+6],6);


	if(isFlashWeak>0)
	{
		bufIndex+=9;
		memcpy(g_paraSetBuf+bufIndex,&picBtnFlashWeak[0],9);
		memcpy(g_paraSetBuf+bufIndex+41,&picBtnFlashWeak[9],9);

		for(i=0;i<41;i++)
		{
			g_paraSetBuf[i+41] |= 0x02;
		}
		
	}else
	{
		if(0==dspMod)
		{
			bufIndex+=9;

			memcpy(g_paraSetBuf+bufIndex,&picEv[0],11);
			memcpy(g_paraSetBuf+bufIndex+41,&picEv[11],11);
		}else
		{
			bufIndex+=8;

			memcpy(g_paraSetBuf+bufIndex,&picMstEv[0],9);
			memcpy(g_paraSetBuf+bufIndex+41,&picMstEv[9],9);

		}

		if(DSP_REVERSE==rsv)
		{
			for(i=0;i<41;i++)
			{
				g_paraSetBuf[i] = (~g_paraSetBuf[i])&0x1f;
				g_paraSetBuf[i+41] = (~g_paraSetBuf[i+41])&0xfc;
			}
	/*
			g_paraSetBuf[0] &= 0x0f;
			g_paraSetBuf[41-1] &= 0x0f;
			g_paraSetBuf[41] &= 0xf8;
			g_paraSetBuf[2*41-1] &= 0xf8;
	*/
			BufAngleCut(g_paraSetBuf,41, 0x0f, 0xf8);
		}
	}

/*
	AreaSet(x, y, 41, 16);

	for(i=0;i<41*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
*/
	LcdCopyToRam(x, y, g_paraSetBuf,41, 16);
}


void DspIsoAuto(void)
{
	u8 paraSetBufIndex=0;
	u8 strLen=28+19;
	
	memset(g_paraSetBuf,0x00,strLen*2);
	
	memcpy(g_paraSetBuf,picIso,17);
	memcpy(g_paraSetBuf+strLen,picIso+17,17);

	paraSetBufIndex += 19;

	memcpy(g_paraSetBuf+paraSetBufIndex,picIsoAuto,28);
	memcpy(g_paraSetBuf+paraSetBufIndex+strLen,picIsoAuto+28,28);

	LcdClearFeild(2, 0, strLen, 16);
	LcdCopyToRam(2, 0,g_paraSetBuf, strLen, 16);	

	
	
}

//void DspIsoValue(u16 x, u16 y,int iso,u8 rvs)
//void DspIsoValue(int iso,u8 rvs)
void DspIsoValue(u32 iso,u8 rvs)
{
	int tIso=iso,m=1;
	u8 i=0;
	u8 strLen=0;
	u8 tmp;
	u8 paraSetBufIndex=0;

	memset(g_paraSetBuf,0x00,(7*6+19)*2);
	//paraSetBufIndex = 1;

	while(tIso>0)
	{
		strLen++;
		tIso /= 10;
		m*=10;
	}

	m/=10;
	//strLen*=7+1;
	strLen = (strLen*8)+19;

	if(DSP_NORMAL==rvs)
	{
		memset(g_paraSetBuf,0x00,strLen*2);
	}else
	{
		memset(g_paraSetBuf,0xFF,strLen);
		memset(g_paraSetBuf+strLen,0xFF,strLen);
	}	

	memcpy(g_paraSetBuf,picIso,17);
	memcpy(g_paraSetBuf+strLen,picIso+17,17);

	paraSetBufIndex+=19;

	//tIso = mainPara.extIso;
	tIso = iso;
	//strLen = 5*7+1;
	
	while(m>0)
	{
		tmp = tIso/m;
		if(DSP_NORMAL==rvs)
		{
			memcpy(g_paraSetBuf+paraSetBufIndex,font_isoNum+FONT_WT_ISO*2*tmp,FONT_WT_ISO);
			memcpy(g_paraSetBuf+paraSetBufIndex+strLen,font_isoNum+FONT_WT_ISO*2*tmp+FONT_WT_ISO,FONT_WT_ISO);
			//g_paraSetBufIndex+=7;			
		}else
		{
			for(i=0;i<FONT_WT_ISO;i++)
			{
				g_paraSetBuf[i+paraSetBufIndex] = ~font_isoNum[i+FONT_WT_ISO*2*tmp];
				g_paraSetBuf[i+strLen+paraSetBufIndex] = ~font_isoNum[i+FONT_WT_ISO*2*tmp+FONT_WT_ISO];
			}
			//g_paraSetBufIndex+=7;	
		}
		//tIso/=10;
		tIso%=m;
		m/=10;
		paraSetBufIndex+=(FONT_WT_ISO+1);	
	}

	//LcdCopyToRam(LAYOUT_PARA_AREA2, 60,g_paraSetBuf, strLen, 16);	
	//LcdClearFeild(2, 0, (5*6+19)*2, 16);
	LcdClearFeild(2, 0, (5*6+19), 16);
	LcdCopyToRam(2, 0,g_paraSetBuf, strLen, 16);	
	
}

void DspRptTimesHz(u8 x, u8 y, u8 tms, u8 tmOrHz,u8 rvs)
{
	u8 dspLen = 0;
	u8 bufIndex = 0;
	
	memset(g_paraSetBuf,0x00,43*2);

	if(tmOrHz>0)
	{
		if(tms>=100)
		{
			//bufIndex+=0;
			dspLen = 37+6;
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			dspLen = 37;
		}else
		{
			//bufIndex+=12;
			dspLen = 37-6;
		}
	}else
	{
		if(tms>=100)
		{
			//bufIndex+=0;
			dspLen = 32;
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			dspLen = 32-6;
		}else
		{
			//bufIndex+=12;
			dspLen = 32-12;
		}
	}

	if(rvs>0)
	{
		memset(g_paraSetBuf,0x3f, dspLen);
		memset(g_paraSetBuf+dspLen,0xe0,dspLen);
		/*
		g_paraSetBuf[0] &=0x1f;
		g_paraSetBuf[dspLen-1] &=0x1f;
		g_paraSetBuf[dspLen] &=0xc0;
		g_paraSetBuf[dspLen+dspLen-1] &=0xc0;
		*/
		BufAngleCut(g_paraSetBuf,dspLen,0x1f,0xc0);

		bufIndex+=2;
		if(tms>=100)
		{
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms/100),5);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms/100)+5,5);

			bufIndex+=6;
			tms%=100;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms/10),5);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms/10)+5,5);

			bufIndex+=6;
			tms%=10;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms),5);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms)+5,5);
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms/10),5);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms/10)+5,5);

			bufIndex+=6;
			tms%=10;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms),5);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms)+5,5);
			
		}else
		{
			//bufIndex+=12;
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms),5);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms)+5,5);
		}

		bufIndex+=7;

		if(tmOrHz>0)
		{
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,picMultiTimes,20);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,picMultiTimes+20,20);
		}else
		{
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,picMultiHz,8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,picMultiHz+8,8);
		}
		
	}else
	{
		bufIndex+=2;
		
		if(tms>=100)
		{
			memcpy(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms/100),5);
			memcpy(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms/100)+5,5);

			bufIndex+=6;
			tms%=100;

			memcpy(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms/10),5);
			memcpy(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms/10)+5,5);

			bufIndex+=6;
			tms%=10;

			memcpy(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms),5);
			memcpy(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms)+5,5);
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			
			memcpy(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms/10),5);
			memcpy(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms/10)+5,5);

			bufIndex+=6;
			tms%=10;

			memcpy(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms),5);
			memcpy(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms)+5,5);
			
		}else
		{
			//bufIndex+=12;
			memcpy(g_paraSetBuf+bufIndex,font_isoNum+FONT_WT_ISO*2*(tms),5);
			memcpy(g_paraSetBuf+bufIndex+dspLen,font_isoNum+FONT_WT_ISO*2*(tms)+5,5);
		}

		bufIndex+=7;

		if(tmOrHz>0)
		{
			memcpy(g_paraSetBuf+bufIndex,picMultiTimes,20);
			memcpy(g_paraSetBuf+bufIndex+dspLen,picMultiTimes+20,20);
		}else
		{
			memcpy(g_paraSetBuf+bufIndex,picMultiHz,8);
			memcpy(g_paraSetBuf+bufIndex+dspLen,picMultiHz+8,8);
		}
	}

	if(tmOrHz>0)
	{
		LcdCopyToRam(x,y+43-dspLen,g_paraSetBuf, dspLen, 16);
		//LcdCopyToRam(x,y,g_paraSetBuf, dspLen, 16);
	}else
	{
		LcdCopyToRam(x,y,g_paraSetBuf, dspLen, 16);
	}


}

void DspMstRptTimesHz(u8 x, u8 y, u8 tms, u8 tmOrHz,u8 rvs)
{
	u8 dspLen = 0;
	u8 bufIndex = 0;
	char *pFont=NULL;
	u8 charLen = 0;

	charLen = 6;
	
	//memset(g_paraSetBuf,0x00,43*2);
	memset(g_paraSetBuf,0x00,46*2);

	if(tmOrHz>0)
	{
		if(tms>=100)
		{
			//bufIndex+=0;
			//dspLen = 37+6;
			dspLen = 39+7;
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			//dspLen = 37;
			dspLen = 39;
		}else
		{
			//bufIndex+=12;
			//dspLen = 37-6;
			dspLen = 39-7;
		}
	}else
	{
		if(tms>=100)
		{
			//bufIndex+=0;
			//dspLen = 32;
			dspLen = 35;
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			dspLen = 35-7;
		}else
		{
			//bufIndex+=12;
			dspLen = 35-14;
		}
	}

	pFont  = (char*)font_exp2Num;

	if(rvs>0)
	{
		//memset(g_paraSetBuf,0x3f, dspLen);
		//memset(g_paraSetBuf,0xff, dspLen);
		memset(g_paraSetBuf,0x1f, dspLen);
		memset(g_paraSetBuf+dspLen,0xfc,dspLen);
		BufAngleCut(g_paraSetBuf,dspLen,0x0f,0xf8);

		bufIndex+=2;
		if(tms>=100)
		{
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms/100),charLen);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms/100)+charLen,charLen);

			bufIndex+=(charLen+1);
			tms%=100;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms/10),charLen);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms/10)+charLen,charLen);

			bufIndex+=(charLen+1);
			tms%=10;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms),charLen);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms)+charLen,charLen);
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms/10),charLen);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms/10)+charLen,charLen);

			bufIndex+=(charLen+1);
			tms%=10;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms),charLen);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms)+charLen,charLen);
			
		}else
		{
			//bufIndex+=12;
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms),charLen);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms)+charLen,charLen);
		}

		bufIndex+=(charLen+2);

		if(tmOrHz>0)
		{
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,picMstRptTimes,20);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,picMstRptTimes+20,20);
			
		}else
		{
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,picMstRptHz,8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+dspLen,picMstRptHz+8,8);
		}
		
	}else
	{
		bufIndex+=2;
		
		if(tms>=100)
		{
			memcpy(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms/100),charLen);
			memcpy(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms/100)+charLen,charLen);

			bufIndex+=(charLen+1);
			tms%=100;

			memcpy(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms/10),charLen);
			memcpy(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms/10)+charLen,charLen);

			bufIndex+=(charLen+1);
			tms%=10;

			memcpy(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms),charLen);
			memcpy(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms)+charLen,charLen);
			
		}else if(tms>=10)
		{
			//bufIndex+=6;
			
			memcpy(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms/10),charLen);
			memcpy(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms/10)+charLen,charLen);

			bufIndex+=(charLen+1);
			tms%=10;

			memcpy(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms),charLen);
			memcpy(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms)+charLen,charLen);
			
		}else
		{
			//bufIndex+=12;
			memcpy(g_paraSetBuf+bufIndex,pFont+charLen*2*(tms),charLen);
			memcpy(g_paraSetBuf+bufIndex+dspLen,pFont+charLen*2*(tms)+charLen,charLen);
		}

		bufIndex+=(charLen+2);

		if(tmOrHz>0)
		{
			memcpy(g_paraSetBuf+bufIndex,picMstRptTimes,20);
			memcpy(g_paraSetBuf+bufIndex+dspLen,picMstRptTimes+20,20);
		}else
		{
			memcpy(g_paraSetBuf+bufIndex,picMstRptHz,8);
			memcpy(g_paraSetBuf+bufIndex+dspLen,picMstRptHz+8,8);
		}
	}

	if(tmOrHz>0)
	{
		LcdCopyToRam(x,y+43-dspLen,g_paraSetBuf, dspLen, 16);
		//LcdCopyToRam(x,y,g_paraSetBuf, dspLen, 16);
	}else
	{
		LcdCopyToRam(x,y,g_paraSetBuf, dspLen, 16);
	}


}


void SetTtlScr(u8 isBk)
{
	s8 tmp1,tmp2;
	u8 index = 0;

	if(g_dspCmpMod>0)
	{
		tmp1 = (g_FlashWeakLevels/3);
		tmp2 = (g_FlashWeakLevels%3);

		if(tmp1>=3)
		{
			tmp1 = 3;
			tmp2 = 0;
		}

		tmp1*=(-1);
		tmp2*=(-1);
	}else
	{
		tmp1 = mstPara->cmp1;
		tmp2 = mstPara->cmp2;

		if((tmp2>0)&&(tmp1<0))
		{
			tmp1++;
			tmp2 = -1 * (3-tmp2);
		}
	}
	
	if(cmrPara.cls>0)
	{
		DspClsFlash();
		index +=(15+1);
		DspResModIcon(WORKINGMODE_TTL,index,SETTING_MODE==g_SettingIndex);
		index+=(31+1);
		if(isBk)
		{
			LoadModIcon(0,index,22,pic_mod_bl,SETTING_MODE==g_SettingIndex);
			index+=(22+1);
		}
		if(cmrPara.mode&0x30)
		{
			LoadModIcon(0,index,23,pic_mod_fp,SETTING_MODE==g_SettingIndex);
		}
	}else
	{
		DspResModIcon(WORKINGMODE_TTL,0,SETTING_MODE==g_SettingIndex);
	}
		
	//DspManuExp2OrCmp(0,LOCATION_TTLCMP_CML,mstPara->cmp1,CalcExp2Ret(mstPara->cmp2),0,SETTING_ADDSUB==g_SettingIndex);
	DspManuExp2OrCmp(0,LOCATION_TTLCMP_CML,tmp1,CalcExp2Ret(tmp2),0,g_dspCmpMod,SETTING_ADDSUB==g_SettingIndex);

	tmp1 = mstPara->cmp1+cmrPara.camera_cmp1;                         // 2016/06/13 take camera cmp into consideration
	tmp2 = mstPara->cmp2+cmrPara.camera_cmp2;                         // 2016/06/13 take camera cmp into consideration
	
	//TtlAutoDrawScale(mstPara->cmp1,mstPara->cmp2);
	TtlAutoDrawScale(cmrPara.isAutoIso,tmp1,tmp2);

}

//void SetAutoScr(void)
void SetAutoScr(u8 isSu4)
{
	s8 tmp1,tmp2;
	
	tmp1 = mstPara->a_cmp1;
	tmp2 = mstPara->a_cmp2;

	if((tmp2>0)&&(tmp1<0))
	{
		tmp1++;
		tmp2 = -1*(3-tmp2);
	}
	
	
	if(isSu4>0)
	{
		DspSu4ResModIcon(WORKINGMODE_AUTO,SETTING_MODE==g_SettingIndex);
	}else
	{
		if(cmrPara.cls>0)
		{
			switch(pSnrSet[SNR_APERT])
			{
				case 0:
					DspClsFlash();
					LoadModIcon(0,15+1, 28, pic_mod_ak, SETTING_MODE==g_SettingIndex);
					if(cmrPara.mode&0x30)
					{
						LoadModIcon(0,15+1+28+1,23,pic_mod_fp,SETTING_MODE==g_SettingIndex);
					}
				break;

				case 1:
					LoadModIcon(0,0, 28, pic_mod_ak, SETTING_MODE==g_SettingIndex);
				break;

				case 2:
					DspClsFlash();
					LoadModIcon(0,15+1, 17, pic_mod_a, SETTING_MODE==g_SettingIndex);
					if(cmrPara.mode&0x30)
					{
						LoadModIcon(0,15+1+17+1,23,pic_mod_fp,SETTING_MODE==g_SettingIndex);
					}
				break;

				case 3:
				default:
					LoadModIcon(0,0, 17, pic_mod_a, SETTING_MODE==g_SettingIndex);
				break;
			}
			//DspClsFlash();
			//DspResModIcon(WORKINGMODE_AUTO,15+1,SETTING_MODE==g_SettingIndex);
			//LoadModIcon(0,15+1+28+1,22,pic_mod_bl,SETTING_MODE==g_SettingIndex);
			//LoadModIcon(0,15+1+28+1+22+1,23,pic_mod_fp,SETTING_MODE==g_SettingIndex);
		}else
		{
			//DspResModIcon(WORKINGMODE_AUTO,0,SETTING_MODE==g_SettingIndex);
			LoadModIcon(0,0, 17, pic_mod_a, SETTING_MODE==g_SettingIndex);
		}
	}
	//DspManuExp2OrCmp(0,LOCATION_TTLCMP_CML,mstPara->a_cmp1,CalcExp2Ret(mstPara->a_cmp2),0,SETTING_ADDSUB==g_SettingIndex);

	DspManuExp2OrCmp(0,LOCATION_TTLCMP_CML,tmp1,CalcExp2Ret(tmp2),0,0,SETTING_ADDSUB==g_SettingIndex);

	
	TtlAutoDrawScale(0,mstPara->a_cmp1,mstPara->a_cmp2);
}

//void SetGnScr(void)
void SetGnScr(u8 isSu4)
{
	s8 tmp1,tmp2;

	tmp1 = mstPara->gn_cmp1;
	tmp2 = mstPara->gn_cmp2;

	if((tmp2>0)&&(tmp1<0))
	{
		tmp1++;
		tmp2 = -1*(3-tmp2);
	}
	
	
	if(isSu4>0)
	{
		DspSu4ResModIcon(WORKINGMODE_GN,SETTING_MODE==g_SettingIndex);
	}else
	{
		DspResModIcon(WORKINGMODE_GN,0,SETTING_MODE==g_SettingIndex);
		
		if(cmrPara.cls>0)
		{
			if(cmrPara.mode&0x30)
			{
				LoadModIcon(0,25+1,23,pic_mod_fp,SETTING_MODE==g_SettingIndex);
			}
		}
	}
	DspManuExp2OrCmp(0,LOCATION_TTLCMP_CML,tmp1,CalcExp2Ret(tmp2),0,0,SETTING_ADDSUB==g_SettingIndex);
	GrDrawScale();
}

//void SetManuScr(void)
void SetManuScr(u8 isSu4)
{
	u8 bias = 0;
	if(isSu4>0)
	{
		DspSu4ResModIcon(WORKINGMODE_MANU,SETTING_MODE==g_SettingIndex);
		bias+=(17+34);
	}else
	{
		DspResModIcon(WORKINGMODE_MANU,0,SETTING_MODE==g_SettingIndex);
		if(cmrPara.cls>0)
		{
			if(cmrPara.mode&0x30)
			{
				LoadModIcon(0,19+1,23,pic_mod_fp,SETTING_MODE==g_SettingIndex);
				bias+=(23+1);
			}
		}
	}
	DspManuExp1(0,21+bias,mstPara->output1,mstPara->output2,0,SETTING_ADDSUB==g_SettingIndex);
	if(mstPara->output2!=0)
	{
		//DspManuExp2OrCmp(0, LOCATION_TTLCMP_CML+bias, 0, CalcExp2Ret(mstPara->output2), 0, SETTING_ADDSUB==g_SettingIndex);
		if(isSu4>0)
		{
			DspManuExp2OrCmp(0, LOCATION_TTLCMP_CML+20, 0, CalcExp2Ret(mstPara->output2), 0, 0,SETTING_ADDSUB==g_SettingIndex);
		}else
		{
			DspManuExp2OrCmp(0, LOCATION_TTLCMP_CML, 0, CalcExp2Ret(mstPara->output2), 0, 0,SETTING_ADDSUB==g_SettingIndex);
		}
	}
	
	ManuRptDrawScale();
}

void SetMultiScr(void)
{
	DspResModIcon(WORKINGMODE_RPT,0,SETTING_MODE==g_SettingIndex);
	DspManuExp1(0,36,mstPara->multi_exp1,mstPara->multi_exp2,0,SETTING_ADDSUB==g_SettingIndex);
	if(mstPara->multi_exp2!=0)
	{
		DspManuExp2OrCmp(0, LOCATION_TTLCMP_CML, 0, CalcExp2Ret(mstPara->multi_exp2), 0, 0,SETTING_ADDSUB==g_SettingIndex);
	}
	DspRptTimesHz(2,54,mstPara->rpt_times,1,SETTING_MULTI==g_SettingIndex);

	//DspRptTimesHz(2,54,mstPara->rpt_times,0,SETTING_FRQ==g_SettingIndex);
	//while(1);
	
	DisplayPicture(2, 54+43+1, 3, 16, picMultiTmsHz, DSP_NORMAL);
	DspRptTimesHz(2,54+43+5,mstPara->rpt_hz,0,SETTING_FRQ==g_SettingIndex);
	ManuRptDrawScale();
	//while(1);
}

//void ModPasteScr(signed char mode)
void ModPasteScr(signed char mode, u8 isSu4)
{
	if(isSu4>0)
	{
		switch(mode)
		{
			case MST_SU4_AUTO:
				SetAutoScr(isSu4);
			break;

			case MST_SU4_GN:
				SetGnScr(isSu4);
			break;

			case MST_SU4_MANU:
				SetManuScr(isSu4);
			break;
		}
	}else
	{
		switch(mode)
		{
			case WORKINGMODE_TTL:
				//DBG_PRINTF("TTL\n");
				SetTtlScr(cmrPara.bkMod&0x04);
			break;

			case WORKINGMODE_TTL_POOR:
				SetTtlScr(0);
			break;

			case WORKINGMODE_AUTO:
				//DBG_PRINTF("M\n");
				SetAutoScr(isSu4);
			break;

			case WORKINGMODE_GN:
				//DBG_PRINTF("Multi\n");
				SetGnScr(isSu4);
			break;

			case WORKINGMODE_MANU:
				SetManuScr(isSu4);
			break;

			case WORKINGMODE_RPT:
				SetMultiScr();
			break;

		}
	}

	
}

void DisplayRmtGr(signed char gr,u8 rvs)
{
	//DisplayPicture(3,3,15,16,picRmtGr,DSP_NORMAL);
	u8 bufIndex=0;
	u8 i=0;
	if(rvs>0)
	{
		memset(g_paraSetBuf,0x3f,15);
		memset(g_paraSetBuf+15,0xf8,15);
		BufAngleCut(g_paraSetBuf,15, 0x1f, 0xf0);
		CopyDataMaskToBuf(g_paraSetBuf, picRmtGrMsk, 15*2);
	}else
	{
		memset(g_paraSetBuf,0x00,15*2);
		memcpy(g_paraSetBuf,picRmtGr,15);
		memcpy(g_paraSetBuf+15,picRmtGr+15,15);
	}

	LcdCopyToRam(3, 3, g_paraSetBuf, 15, 16);

	DisplayPicture(3, 3+15+4, 2, 16, picRmtDot, DSP_NORMAL);

	if(rvs>0)
	{
		memset(g_paraSetBuf,0x7f,46);
		memset(g_paraSetBuf+46,0xfe,46);
		BufAngleCut(g_paraSetBuf,46, 0x3f, 0xfc);
		bufIndex+=(3+5);
		for(i=0;i<3;i++)
		{
			if(i==gr)
			{
				//memcpy(g_paraSetBuf+bufIndex,fontRmtGrSel+(i*18),9);
				//memcpy(g_paraSetBuf+bufIndex+46,fontRmtGrSel+(i*18)+9,9);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex,fontRmtGrSel+(i*18),9);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex+46,fontRmtGrSel+(i*18)+9,9);

				bufIndex+=4;
			}else
			{
				//memcpy(g_paraSetBuf+bufIndex,fontRmtGr+(i*10),5);
				//memcpy(g_paraSetBuf+bufIndex+46,fontRmtGr+(i*10)+5,5);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex,fontRmtGr+(i*10),5);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex+46,fontRmtGr+(i*10)+5,5);
			}

			bufIndex+=11;			
		}
		
	}else
	{
		memset(g_paraSetBuf,0x00,46*2);
		memcpy(g_paraSetBuf,picRmtLeft,3);
		memcpy(g_paraSetBuf+46,picRmtLeft+3,3);
		bufIndex+=(3+5);
		for(i=0;i<3;i++)
		{
			if(i==gr)
			{
				memcpy(g_paraSetBuf+bufIndex,fontRmtGrSel+(i*18),9);
				memcpy(g_paraSetBuf+bufIndex+46,fontRmtGrSel+(i*18)+9,9);
				bufIndex+=4;
			}else
			{
				memcpy(g_paraSetBuf+bufIndex,fontRmtGr+(i*10),5);
				memcpy(g_paraSetBuf+bufIndex+46,fontRmtGr+(i*10)+5,5);
			}

			bufIndex+=11;			
		}

		memcpy(g_paraSetBuf+bufIndex-2,picRmtRight,3);
		memcpy(g_paraSetBuf+bufIndex-2+46,picRmtRight+3,3);
		
	}

	LcdCopyToRam(3, 3+15+4+6, g_paraSetBuf, 46, 16);
	
	
}

void DisplayRmtCh(signed char ch,u8 rvs)
{
	u8 bufIndex=0;
	u8 i=0;
	if(rvs>0)
	{
		memset(g_paraSetBuf,0x3f,15);
		memset(g_paraSetBuf+15,0xf8,15);
		BufAngleCut(g_paraSetBuf,15, 0x1f, 0xf0);
		CopyDataMaskToBuf(g_paraSetBuf, picRmtChMsk, 15*2);
	}else
	{
		memset(g_paraSetBuf,0x00,15*2);
		memcpy(g_paraSetBuf,picRmtCh,15);
		memcpy(g_paraSetBuf+15,picRmtCh+15,15);
	}

	LcdCopyToRam(6, 3, g_paraSetBuf, 15, 16);

	DisplayPicture(6, 3+15+4, 2, 16, picRmtDot, DSP_NORMAL);

	if(rvs>0)
	{
		memset(g_paraSetBuf,0x7f,46);
		memset(g_paraSetBuf+46,0xfe,46);
		BufAngleCut(g_paraSetBuf,46, 0x3f, 0xfc);
		bufIndex+=(3+5);
		for(i=0;i<4;i++)
		{
			if(i==ch)
			{
				//memcpy(g_paraSetBuf+bufIndex,fontRmtGrSel+(i*18),9);
				//memcpy(g_paraSetBuf+bufIndex+46,fontRmtGrSel+(i*18)+9,9);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex,font_apertNum+(i*16)+16,8);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex+46,font_apertNum+(i*16)+16+8,8);
				bufIndex+=3;
			}else
			{
				//memcpy(g_paraSetBuf+bufIndex,fontRmtGr+(i*10),5);
				//memcpy(g_paraSetBuf+bufIndex+46,fontRmtGr+(i*10)+5,5);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex,fontRmtCh+(i*10),5);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex+46,fontRmtCh+(i*10)+5,5);
			}

			bufIndex+=8;			
		}
		
	}else
	{
		memset(g_paraSetBuf,0x00,46*2);
		memcpy(g_paraSetBuf,picRmtLeft,3);
		memcpy(g_paraSetBuf+46,picRmtLeft+3,3);
		bufIndex+=(3+5);
		for(i=0;i<4;i++)
		{
			if(i==ch)
			{
				memcpy(g_paraSetBuf+bufIndex,font_apertNum+(i*16)+16,8);
				memcpy(g_paraSetBuf+bufIndex+46,font_apertNum+(i*16)+16+8,8);
				bufIndex+=3;
			}else
			{
				memcpy(g_paraSetBuf+bufIndex,fontRmtCh+(i*10),5);
				memcpy(g_paraSetBuf+bufIndex+46,fontRmtCh+(i*10)+5,5);
			}

			bufIndex+=8;			
		}

		memcpy(g_paraSetBuf+bufIndex,picRmtRight,3);
		memcpy(g_paraSetBuf+bufIndex+46,picRmtRight+3,3);
		
	}

	LcdCopyToRam(6, 3+15+4+6, g_paraSetBuf, 46, 16);
	
	
}

void DisplayRmtSu4Mod(void)
{
	//switch(mstPara->rmt_su4_mode)
	switch(mstPara->mode)
	{
		//case RMT_SU4_M:
		case WORKINGMODE_MANU:
			LoadModIcon(4,5, 19, pic_mod_m, SETTING_MODE==g_SettingIndex);
			DspManuExp1(4,5+21,mstPara->output1,mstPara->output2,0,SETTING_ADDSUB==g_SettingIndex);
			if(mstPara->output2!=0)
			{
				DspManuExp2OrCmp(4, LOCATION_TTLCMP_CML-20, 0, CalcExp2Ret(mstPara->output2), 0, 0,SETTING_ADDSUB==g_SettingIndex);
			}
		break;

		//case RMT_SU4_OFF:
		case WORKINGMODE_OFF:
			LoadModIcon(4,5,44,picRmtSu4ModOff,SETTING_MODE==g_SettingIndex);
		break;

		//case RMT_SU4_AUTO:
		case WORKINGMODE_AUTO:
			LoadModIcon(4,5,36,picRmtSu4ModAuto,SETTING_MODE==g_SettingIndex);
		break;
	}
}

void LightSlaveSU4(void)
{
	DisplayPicture(0,0,16,16,picRmtIcon,DSP_NORMAL);
	DisplayPicture(0,16+1,33,16,fontSu4,DSP_NORMAL);
	DisplayPicture(0,16+1+33+1,66,16,picRmtTitle,DSP_NORMAL);
	DisplayRmtSu4Mod();
	
}

void LightSlaveMultiMod(void)
{
	DisplayPicture(0,0,16,16,picRmtIcon,DSP_NORMAL);
	DisplayPicture(0,16+4,66,16,picRmtTitle,DSP_NORMAL);
	DisplayRmtGr(mstPara->rmt_gr,g_SettingIndex==SETTING_RMT_GR);
	DisplayRmtCh(mstPara->rmt_ch,g_SettingIndex==SETTING_RMT_CH);
}

void LightSlaveScr(void)
{
	if(pSnrSet[SNR_SU4]>0)
	{
		LightSlaveSU4();
	}else
	{
		LightSlaveMultiMod();
	}
}

//void DspMstRspArrHead(u8 arr,u8 rvs)
void DspMstRspArrHead(u8 x, u8 y, u8 arr, u8 rvs)
{
	u8 i=0;
	
	if(rvs>0)
	{
		memset(g_paraSetBuf,0x7f,11);
		memset(g_paraSetBuf+11,0xf8,11);
		BufAngleCut(g_paraSetBuf, 11, 0x3f, 0xf0);
	}else
	{
		memset(g_paraSetBuf,0x00,11*2);
		//memcpy(g_paraSetBuf,picMstFrame,11*2);
	}
	
	switch(arr)
	{
		case 0:
			if(rvs>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf,picMstGrm,11);
				CopyDataMaskToBuf(g_paraSetBuf+11,picMstGrm+11,11);
			}else
			{
				memcpy(g_paraSetBuf,picMstGrm,11);
				memcpy(g_paraSetBuf+11,picMstGrm+11,11);
			}
		break;

		case 1:
			if(rvs>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf,picMstGra,11);
				CopyDataMaskToBuf(g_paraSetBuf+11,picMstGra+11,11);
			}else
			{
				memcpy(g_paraSetBuf,picMstGra,11);
				memcpy(g_paraSetBuf+11,picMstGra+11,11);
			}
		break;

		case 2:
			if(rvs>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf,picMstGrb,11);
				CopyDataMaskToBuf(g_paraSetBuf+11,picMstGrb+11,11);
			}else
			{
				memcpy(g_paraSetBuf,picMstGrb,11);
				memcpy(g_paraSetBuf+11,picMstGrb+11,11);
			}
		break;

		case 3:
			if(rvs>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf,picMstGrc,11);
				CopyDataMaskToBuf(g_paraSetBuf+11,picMstGrc+11,11);
			}else
			{
				memcpy(g_paraSetBuf,picMstGrc,11);
				memcpy(g_paraSetBuf+11,picMstGrc+11,11);
			}
		break;
	}

	if(!rvs)
	{
		for(i=0;i<11*2;i++)
		{
			g_paraSetBuf[i] |= picMstFrame[i];
		}
	}

	if(x%8)
	{
		AreaScroll((u8*)g_paraSetBuf,11,2,11,SCROLL_DOWN,x%8);
	}

	//LcdCopyToRam(2*arr,0, g_paraSetBuf, 11, 16);
	LcdCopyToRam(x/8,y, g_paraSetBuf, 11, 16);

	//DisplayPicture(2*arr, 11+4, 2, 16, picRmtDot, DSP_NORMAL);
	//DisplayPicture(x, y+11+4, 2, 16, picRmtDot, DSP_NORMAL);
	DisplayPictureEx(x, y+11+4, 2, 16, picRmtDot, DSP_NORMAL);
	
}

void DspMstRspArrMod(u8 arr, u8 mode,u8 rsv)
{
	u8 bias = 0;
	if(rsv>0)
	{
		memset(g_paraSetBuf,0xff,27);
		memset(g_paraSetBuf+27,0xfc,27);
		//BufAngleCut(g_paraSetBuf, 27, 0x7f, 0xf8);
	}else
	{
		memset(g_paraSetBuf,0x00,27*2);
	}

	switch(mode)
	{
		case WORKINGMODE_TTL:
			if(rsv>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf,picMstModTtl,27*2);
				
			}else
			{
				memcpy(g_paraSetBuf,picMstModTtl,27*2);
			}
		break;

		case WORKINGMODE_MANU:
			bias = (27-11)/2;
			if(rsv>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf+bias,picMstModM,11);
				CopyDataMaskToBuf(g_paraSetBuf+bias+27,picMstModM+11,11);
			}else
			{
				memcpy(g_paraSetBuf+bias,picMstModM,11);
				memcpy(g_paraSetBuf+bias+27,picMstModM+11,11);
			}
		break;

		case WORKINGMODE_AUTO:
			bias = (27-9)/2;
			if(rsv>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf+bias,picMstModAuto,9);
				CopyDataMaskToBuf(g_paraSetBuf+bias+27,picMstModAuto+9,9);
			}else
			{
				memcpy(g_paraSetBuf+bias,picMstModAuto,9);
				memcpy(g_paraSetBuf+bias+27,picMstModAuto+9,9);
			}
		break;

		default:
			bias = (27-19)/2;
			if(rsv>0)
			{
				CopyDataMaskToBuf(g_paraSetBuf+bias,picMstModOff,19);
				CopyDataMaskToBuf(g_paraSetBuf+bias+27,picMstModOff+19,19);
			}else
			{
				memcpy(g_paraSetBuf+bias,picMstModOff,19);
				memcpy(g_paraSetBuf+bias+27,picMstModOff+19,19);
			}
		break;
		
	}

	LcdCopyToRam(2*arr,11+4+6,g_paraSetBuf,27,16);
}

void DspMstRspArrManuExp(u8 arr, s8 pa1, s8 pa2,u8 rsv)
{
	u8 bufIndex=0;
	u8 dspChar = 128;
	u8 i=0;

	while(i<pa1)
	{
		dspChar/=2;
		i++;
	}
/*	
	if(rsv>0)
	{
		memset(g_paraSetBuf,0xff,76);
		memset(g_paraSetBuf+76,0xfc,76);
		BufAngleCut(g_paraSetBuf, 76, 0x7f,0xf0);
	}else
	{
		memset(g_paraSetBuf,0x00,76*2);
	}
*/
	memset(g_paraSetBuf,0x00,76*2);

	if(rsv>0)
	{
		memset(g_paraSetBuf,0xff,76);
		memset(g_paraSetBuf+76,0xfc,76);
		//BufAngleCut(g_paraSetBuf, 76, 0x7f,0xf0);
	}

	bufIndex+=2;
	if(rsv>0)
	{
		CopyDataMaskToBuf(g_paraSetBuf+bufIndex,picMstExpHead,11);
		CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,picMstExpHead+11,11);

		bufIndex+=11;

		if(!pa1)
		{
			bufIndex+=1;
				
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&fontMstManuExp[16],8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16+8],8);

			//bufIndex+=9;
			bufIndex+=8;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&fontMstManuExp[16*2],8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*2+8],8);

			bufIndex+=9;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&fontMstManuExp[16*8],8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*8+8],8);

			bufIndex+=10;
			
			
		}else if(pa1<4)
		{
			bufIndex+=3;
			
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&fontMstManuExp[16*(dspChar/10)],8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*(dspChar/10)+8],8);

			//bufIndex+=9;
			bufIndex+=9;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&fontMstManuExp[16*(dspChar%10)],8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*(dspChar%10)+8],8);

			//bufIndex+=(9+9);
			if(!pa2)
			{
				bufIndex+=9;
			}else
			{
				bufIndex+=(9+7);
			}
			
		}else
		{
			bufIndex+=3;
			
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&fontMstManuExp[16*dspChar],8);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*dspChar+8],8);

			if(!pa2)
			{
				bufIndex+=9;
			}else
			{
				bufIndex+=(8+9+8);
			}
		}

		
		
	}else
	{
		memcpy(g_paraSetBuf+bufIndex,picMstExpHead,11);
		memcpy(g_paraSetBuf+bufIndex+76,picMstExpHead+11,11);

		//bufIndex+=(11+1);
		bufIndex+=11;

		if(!pa1)
		{
			bufIndex+=1;
			
			memcpy(g_paraSetBuf+bufIndex,&fontMstManuExp[16],8);
			memcpy(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16+8],8);

			//bufIndex+=9;
			bufIndex+=8;

			memcpy(g_paraSetBuf+bufIndex,&fontMstManuExp[16*2],8);
			memcpy(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*2+8],8);

			bufIndex+=9;

			memcpy(g_paraSetBuf+bufIndex,&fontMstManuExp[16*8],8);
			memcpy(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*8+8],8);

			bufIndex+=10;
			
		}else if(pa1<4)
		{
			bufIndex+=3;
			
			memcpy(g_paraSetBuf+bufIndex,&fontMstManuExp[16*(dspChar/10)],8);
			memcpy(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*(dspChar/10)+8],8);

			//bufIndex+=9;
			bufIndex+=9;

			memcpy(g_paraSetBuf+bufIndex,&fontMstManuExp[16*(dspChar%10)],8);
			memcpy(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*(dspChar%10)+8],8);

			if(!pa2)
			{
				bufIndex+=9;
			}else
			{
				bufIndex+=(9+7);
			}
			
		}else
		{
			bufIndex+=3;
			
			memcpy(g_paraSetBuf+bufIndex,&fontMstManuExp[16*dspChar],8);
			memcpy(g_paraSetBuf+bufIndex+76,&fontMstManuExp[16*dspChar+8],8);

			if(!pa2)
			{
				bufIndex+=9;
			}else
			{
				bufIndex+=(8+9+8);
			}
		}
		
	}

	if(pa2!=0)
	{
		if(rsv>0)
		{
			if(pa2<0)
			{
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&picCmpSub[0],5);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&picCmpSub[5],5);
				pa2 = pa2*(-1);
			}else
			{
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&picCmpAdd[0],5);
				CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&picCmpAdd[5],5);				
			}

			bufIndex+=7;
			
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&font_exp2Num[0],6);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&font_exp2Num[6],6);

			bufIndex+=7;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&picExp2Dot[0],2);	
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&picExp2Dot[2],2);

			bufIndex+=3;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&font_exp2Num[pa2*12],6);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&font_exp2Num[pa2*12+6],6);

			//bufIndex+=9;
			bufIndex+=7;

			CopyDataMaskToBuf(g_paraSetBuf+bufIndex,&picMstEv[0],9);
			CopyDataMaskToBuf(g_paraSetBuf+bufIndex+76,&picMstEv[9],9);	

			bufIndex+=13;

			BufAngleCut(g_paraSetBuf,76,0x7f,0xf8);
			
		}else
		{
			if(pa2<0)
			{
				memcpy(g_paraSetBuf+bufIndex,&picCmpSub[0],5);
				memcpy(g_paraSetBuf+bufIndex+76,&picCmpSub[5],5);
				pa2 = pa2*(-1);
			}else
			{
				memcpy(g_paraSetBuf+bufIndex,&picCmpAdd[0],5);
				memcpy(g_paraSetBuf+bufIndex+76,&picCmpAdd[5],5);				
			}

			bufIndex+=7;
			
			memcpy(g_paraSetBuf+bufIndex,&font_exp2Num[0],6);
			memcpy(g_paraSetBuf+bufIndex+76,&font_exp2Num[6],6);

			bufIndex+=7;

			memcpy(g_paraSetBuf+bufIndex,&picExp2Dot[0],2);	
			memcpy(g_paraSetBuf+bufIndex+76,&picExp2Dot[2],2);

			bufIndex+=3;

			memcpy(g_paraSetBuf+bufIndex,&font_exp2Num[pa2*12],6);
			memcpy(g_paraSetBuf+bufIndex+76,&font_exp2Num[pa2*12+6],6);

			//bufIndex+=9;
			bufIndex+=7;

			memcpy(g_paraSetBuf+bufIndex,&picMstEv[0],9);
			memcpy(g_paraSetBuf+bufIndex+76,&picMstEv[9],9);	

			bufIndex+=13;
		}
	}else
	{
		if(rsv>0)
		{
			memset(g_paraSetBuf+bufIndex,0x00,76-bufIndex);
			memset(g_paraSetBuf+bufIndex+76,0x00,76-bufIndex);
			BufAngleCutEx(g_paraSetBuf,0,bufIndex-1,76,0x7f,0xf8);
			//BufAngleCut(g_paraSetBuf,bufIndex,0x7f,0xf8);
			//BufAngleCut(g_paraSetBuf,76,0x7f,0xf8);
		}
	}

	LcdCopyToRam(2*arr,11+4+6+27+1,g_paraSetBuf,76,16);

	//BufAngleCutEx(char * buf, int start, int len, char mask1, char mask2);

}

void DspMstRspArrTtlAutoCmp(u8 arr, s8 pa1, s8 pa2,u8 rsv)
{
	s8 tmp1,tmp2;
	tmp1 = pa1;
	tmp2 = pa2;
	if((tmp2>0)&&(tmp1<0))
	{
		tmp1++;
		tmp2 = -1 * (3-tmp2);
	}
	DspManuExp2OrCmp(2*arr,11+4+6+27+40,tmp1,CalcExp2Ret(tmp2),1,0,rsv);
}

void LightMasterMultiMod(void)
{
	u8 iArr = 0;
//	s8 tmp1,tmp2;
	//tmp1 = mstPara->cmp1;
	//tmp2 = mstPara->cmp2;

	
	for(iArr=0;iArr<4;iArr++)
	{
		DspMstRspArrHead(iArr*2*8,0,iArr,(SETTING_MST_M+iArr)==g_SettingIndex);
		DspMstRspArrMod(iArr,mainPara.modGr[iArr].mode,((SETTING_MST_M+iArr)==g_SettingIndex)&&(g_mstSetting==MST_SET_MODE));
		
		switch(mainPara.modGr[iArr].mode)
		{
			case WORKINGMODE_TTL:
				//DspMstRspArrTtlAutoCmp(iArr,mainPara.modGr[iArr].cmp1,CalcExp2Ret(mainPara.modGr[iArr].cmp2),((SETTING_MST_M+iArr)==g_SettingIndex)&&(g_mstSetting==MST_SET_PARA));
				DspMstRspArrTtlAutoCmp(iArr,mainPara.modGr[iArr].cmp1,mainPara.modGr[iArr].cmp2,((SETTING_MST_M+iArr)==g_SettingIndex)&&(g_mstSetting==MST_SET_PARA));
			break;

			case WORKINGMODE_MANU:
				DspMstRspArrManuExp(iArr,mainPara.modGr[iArr].output1,CalcExp2Ret(mainPara.modGr[iArr].output2),((SETTING_MST_M+iArr)==g_SettingIndex)&&(g_mstSetting==MST_SET_PARA));
			break;

			case WORKINGMODE_AUTO:
				//DspMstRspArrTtlAutoCmp(iArr,mainPara.modGr[iArr].a_cmp1,CalcExp2Ret(mainPara.modGr[iArr].a_cmp2),((SETTING_MST_M+iArr)==g_SettingIndex)&&(g_mstSetting==MST_SET_PARA));
				DspMstRspArrTtlAutoCmp(iArr,mainPara.modGr[iArr].a_cmp1,mainPara.modGr[iArr].a_cmp2,((SETTING_MST_M+iArr)==g_SettingIndex)&&(g_mstSetting==MST_SET_PARA));
			break;

			default:
			break;
		}
	}
}

void MstRptArrOnOff(u8 x, u8 y, u8 isOn, u8 isSel)
{
	if(isSel>0)
	{
		memset(g_paraSetBuf,0x1f,isOn>0?15:21);
		memset(g_paraSetBuf+(isOn>0?15:21),0xfc,isOn>0?15:21);
		BufAngleCut(g_paraSetBuf,isOn>0?15:21,0x0f,0xf8);
		if(isOn>0)
		{
			CopyDataMaskToBuf(g_paraSetBuf,picMstRptOn,15*2);
		}else
		{
			CopyDataMaskToBuf(g_paraSetBuf,picMstRptOff,21*2);
		}
	}else
	{
		memset(g_paraSetBuf,0x00,isOn>0?15*2:21*2);
		if(isOn>0)
		{
			memcpy(g_paraSetBuf,picMstRptOn,15*2);
		}else
		{
			memcpy(g_paraSetBuf,picMstRptOff,21*2);
		}
	}

	LcdCopyToRam(x,y,g_paraSetBuf,isOn>0?15:21,16);
	
}

void MstRptArrPara(void)
{
	u8 iArr = 0;
	for(iArr=0;iArr<4;iArr++)
	{
		//DspMstRspArrHead(4+(iArr/2)*2,(iArr%2)*53+3,iArr,(SETTING_RPT_SET==g_SettingIndex)&&((MSTRPT_SEL_M+iArr)==g_mstSetting));
		DspMstRspArrHead((4+(iArr/2)*2)*8+1,(iArr%2)*53+3,iArr,(SETTING_RPT_SET==g_SettingIndex)&&((MSTRPT_SEL_M+iArr)==g_mstSetting));
		MstRptArrOnOff(4+(iArr/2)*2,(iArr%2)*53+3+11+7+1,mainPara.modGr[iArr].rpt_isOn,(SETTING_RPT_SET==g_SettingIndex)&&((MSTRPT_SEL_M+iArr)==g_mstSetting));
	}
}

void LightMasterRptMod(void)
{
	DspResModIcon(WORKINGMODE_RPT,0,DSP_NORMAL);
	DspManuExp1(0,36,mstPara->multi_exp1,mstPara->multi_exp2,0,(SETTING_RPT_SET==g_SettingIndex)&&(MSTRPT_SEL_EXP==g_mstSetting));
	if(mstPara->multi_exp2!=0)
	{
		DspManuExp2OrCmp(0, LOCATION_TTLCMP_CML, 0, CalcExp2Ret(mstPara->multi_exp2), 0, 0,(SETTING_RPT_SET==g_SettingIndex)&&(MSTRPT_SEL_EXP==g_mstSetting));
	}
	DspMstRptTimesHz(2,2,mstPara->rpt_times,1,(SETTING_RPT_SET==g_SettingIndex)&&(MSTRPT_SEL_TIMES==g_mstSetting));
	DisplayPicture(2, 2+46+1, 3, 16, picMultiTmsHz, DSP_NORMAL);
	DspMstRptTimesHz(2,2+46+5,mstPara->rpt_hz,0,(SETTING_RPT_SET==g_SettingIndex)&&(MSTRPT_SEL_FRQ==g_mstSetting));
	MstRptArrPara();
}

void LightMasterSU4(void)	
{
	//ModPasteScr(mstPara->mst_su4_mode,1);
	ModPasteScr(mstPara->mode,1);
}

void LightMasterScr(void)
{
	if(pSnrSet[SNR_SU4]>0)
	{
		LightMasterSU4();
	}else
	{
		if(pSnrSet[SNR_MSTRPT]>0)
		{
			LightMasterMultiMod();
		}else
		{
			LightMasterRptMod();
		}
	}
}

void DspMotoFxDxM(u8 fxdx, u8 isAuto)
{
	memset(g_paraSetBuf,0x00,22*2);
// 2016/09/03
/*
	if(fxdx>0)
	{
		//memcpy(g_paraSetBuf,picZoomModDx,12);
		memcpy(g_paraSetBuf,picZoomModFx,12);
	}else
	{
		//memcpy(g_paraSetBuf,picZoomModFx,12);
		memcpy(g_paraSetBuf,picZoomModDx,12);
	}
*/

	if(isAuto==0)
	{
		memcpy(g_paraSetBuf+12+1,picZoomModM,9);
	}

	memcpy(g_paraSetBuf+22,picZoomModZoom,22);

	LcdCopyToRam(9, 2,g_paraSetBuf,22,16);
}

void BufFramed(u8 * buf,u8 st,u8 ed,u8 aLen,u8 sLen)
{
	u8 i = 0;
	for(i = st; i<ed;i++)
	{
		buf[i] |= 0x80;
		buf[i+aLen]|=0x01;
	}

	buf[st] |= 0xff;
	buf[ed] |= 0xff;
	buf[aLen+st] |= 0xff;
	buf[aLen+ed] |= 0xff;
}

u8 isMotoMax(u8 zm)
{
#if(defined(REFLACT_EN)&&(REFLACT_EN>0))
	switch(g_flashMode)
	{
		case FLASH_MODE_NORMAL:
		case FLASH_MODE_REFLECT:
		return (zm>=200);

		case FLASH_MODE_PARALLEL:
		return (zm>=135);
	}
	
	return (zm>200);
#else
	return (zm>=200);
#endif
}

u8 isMotoMin(u8 zm)
{
#if(defined(REFLACT_EN)&&(REFLACT_EN>0))
	switch(g_flashMode)
	{
		case FLASH_MODE_NORMAL:
		return (zm<=20);
		
		case FLASH_MODE_PARALLEL:
		return (zm<=20);

		case FLASH_MODE_REFLECT:
		return (zm<=24);
	}
	
	return (zm<=20);
#else
	return (zm<=20);
#endif
}

void DisplayFocLen(uchar motoMode,u16 foc,u8 frame,uchar rsv)
{
	u8 paraSetBufIndex=0;
	u16 focTmp = foc;
	u8 i=0;
	u8 fxorDx = 0;

// 2016/09/01
	if(foc>200)
	{
		foc = 200;
		focTmp = foc;
	}
	
	//DisplayPicture(9, 2, 22, 16, picZoom, DSP_NORMAL);
/*
	if(0==pSnrSet[SNR_FXDX])
	{
		fxorDx = 0;                                    //should refer camer function
	}else if(1==pSnrSet[SNR_FXDX])
	{
		fxorDx = 1;
	}else if(2==pSnrSet[SNR_FXDX])
	{
		fxorDx = 0;
	}
*/
	fxorDx = 0;                                    //should refer camer function
	
	if(MOTO_MODE_AUTO==motoMode)
	{
		//DspMotoFxDxM(fxorDx,(cmrPara.cls>0)&&(MOTO_MODE_AUTO==*motoSet.mode));
		DspMotoFxDxM(g_cmrFxOrDx,(cmrPara.cls>0)&&(MOTO_MODE_AUTO==motoMode));
	}else
	{
		DspMotoFxDxM(fxorDx,(cmrPara.cls>0)&&(MOTO_MODE_AUTO==motoMode));
	}

	memset(g_paraSetBuf,0x00,47*2);

	paraSetBufIndex+=2;

	//if(24==focTmp)
	//if(20==focTmp)
	if(isMotoMin(focTmp))
	{
		//paraSetBufIndex+=2;
		memcpy(g_paraSetBuf+paraSetBufIndex,picZoomMin,6);
	}else if(focTmp/100)
	{
		//paraSetBufIndex+=2;
		memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[(focTmp/100)*16],8);
		memcpy(g_paraSetBuf+paraSetBufIndex+47,&font_apertNum[(focTmp/100)*16+8],8);
		focTmp%=100;
	}

	paraSetBufIndex+=10;

	memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[(focTmp/10)*16],8);
	memcpy(g_paraSetBuf+paraSetBufIndex+47,&font_apertNum[(focTmp/10)*16+8],8);

	paraSetBufIndex+=10;

	memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[(focTmp%10)*16],8);
	memcpy(g_paraSetBuf+paraSetBufIndex+47,&font_apertNum[(focTmp%10)*16+8],8);

	paraSetBufIndex+=12;

	//if(200==foc)
	if(isMotoMax(foc))
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,picZoomMax,6);
	}

	memcpy(g_paraSetBuf+paraSetBufIndex+47,picMm,11);

	if(DSP_REVERSE==rsv)
	{
		if(foc<100)
		{
			focTmp = 10;
		}else
		{
			focTmp = 0;
		}

		for(i=focTmp;i<47;i++)
		{
			g_paraSetBuf[i] = (~g_paraSetBuf[i])&0x7f;
			g_paraSetBuf[i+47] = (~g_paraSetBuf[i+47])&0xfe;
		}

		g_paraSetBuf[focTmp]&=0x3f;
		g_paraSetBuf[47-1]&=0x3f;
		g_paraSetBuf[47+focTmp]&=0xfc;
		g_paraSetBuf[47*2-1]&=0xfc;
		
	}

	if(DSP_NORMAL==rsv)
	{
		if(ZOOM_FRAME_FIXED==frame)
		{
			//memcpy(g_paraSetBuf+12,picZoomFixedFrame,22);
			//memcpy(g_paraSetBuf+47+12,picZoomFixedFrame+22,22);

			for(i=0;i<22;i++)
			{
				g_paraSetBuf[10+i] |= picZoomFixedFrame[i];
				g_paraSetBuf[47+10+i] |= picZoomFixedFrame[22+i];
			}

			
		}else if(ZOOM_FRAME_SETA==frame)
		{
			if(foc>=100)
			{
				BufFramed((u8*)g_paraSetBuf,0,46,47,47);
			}else
			{
				BufFramed((u8*)g_paraSetBuf,10,46,47,37);
			}
		}
	}
	

	LcdClearFeild(9, 2+22+1, 47, 16);
	LcdCopyToRam(9, 2+22+1,g_paraSetBuf, 47, 16);	
	
	
}



void DspApertValue(u16 apert, u8 rsv)
{
	u8 paraSetBufIndex=2;
	u8 i=0;
	u8 len=0;

	if(apert<100)
	{
		if(apert%10)
		{
			len = 37;
		}else
		{
			len = 23;
		}
	}else
	{
		len = 35;
	}
	
	memset(g_paraSetBuf,0x00,len*2);
	
	memcpy(g_paraSetBuf+paraSetBufIndex,picApert_F,7);
	memcpy(g_paraSetBuf+paraSetBufIndex+len,picApert_F+7,7);

	paraSetBufIndex+=11;

	if(apert<100)
	{
		memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[(apert/10)*16],8);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,&font_apertNum[(apert/10)*16+8],8);
		paraSetBufIndex+=10;
		if(apert%10)
		{
			*(g_paraSetBuf+len+paraSetBufIndex)|=0x0c;
			*(g_paraSetBuf+len+paraSetBufIndex+1)|=0x0c;
			paraSetBufIndex+=4;
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[(apert%10)*16],8);
			memcpy(g_paraSetBuf+paraSetBufIndex+len,&font_apertNum[(apert%10)*16+8],8);
			paraSetBufIndex+=10;
		}
	}else{
		memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[(apert/100)*16],8);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,&font_apertNum[(apert/100)*16+8],8);
		paraSetBufIndex+=12;
		memcpy(g_paraSetBuf+paraSetBufIndex,&font_apertNum[((apert%100)/10)*16],8);
		memcpy(g_paraSetBuf+paraSetBufIndex+len,&font_apertNum[((apert%100)/10)*16+8],8);
		paraSetBufIndex+=10;
	}

	if(DSP_REVERSE==rsv)
	{
		for(i=0;i<paraSetBufIndex;i++)
		{
			g_paraSetBuf[i] = (~g_paraSetBuf[i])&0x7f;
			g_paraSetBuf[i+len] = (~g_paraSetBuf[i+len])&0xfe;
		}

		g_paraSetBuf[0]&=0x3f;
		g_paraSetBuf[len-1]&=0x3f;
		g_paraSetBuf[len]&=0xfc;
		g_paraSetBuf[len+paraSetBufIndex-1]&=0xfc;
		
	}else
	{
		for(i=len;i<len*2;i++)
		{
			g_paraSetBuf[i] |=0x01;
		}
	}

	LcdClearFeild(9, 85, 37, 16);
	LcdCopyToRam(9, 85, g_paraSetBuf, paraSetBufIndex,16);	
	
}

void *BtnZoom(void)
{
	u8 motoLevelMin=0, motoLevelMax=0;
	
	if(motoSet.moving != 0)return NULL;
#ifdef MOTO_REFLECT_DETECT
	//if(g_isReflectPulled>0) return NULL;
	if((g_isReflectPulled>0)&&(pSnrSet[SNR_WP]>0)) return NULL;
#endif

#if(defined(REFLACT_EN)&&(REFLACT_EN>0))
	switch(g_flashMode)
	{
		case FLASH_MODE_NORMAL:
			//motoLevelMin = 1;
			motoLevelMin = 2;
			//motoLevelMax = 11;
			motoLevelMax = MOTO_MAX_INDEX-1;
		break;

		case FLASH_MODE_PARALLEL:
			//motoLevelMin = 0;
			motoLevelMin = 2;
			//motoLevelMax = 9;
			motoLevelMax = MOTO_MAX_INDEX-3;
		break;

		case FLASH_MODE_REFLECT:
			//motoLevelMin = 2;
			motoLevelMin = 3;
			//motoLevelMax = 11;
			motoLevelMax = MOTO_MAX_INDEX-1;
		break;
	}
#else
	motoLevelMin = 0;
	motoLevelMax = MOTO_MAX_INDEX-2;
#endif

	if(SETTING_MOTO==g_SettingIndex)
	{
		if(MOTO_MODE_AUTO == *motoSet.mode)
		{
			*motoSet.mode= MOTO_MODE_MANU;
			motoSet.focIndex = GetMotoStdIndex(motoSet.dspLocation);
			
		}else
		{
			motoSet.focIndex++;
		}
		if(motoSet.focIndex>motoLevelMax)
		{
			motoSet.focIndex = motoLevelMin;
		}
		//*motoSet.dest_location = focLenScape[motoSet.focIndex];
		motoSet.dspLocation = focLenScape[motoSet.focIndex];
		//*motoSet.dest_location = GetMotoDest(motoSet.dspLocation);
		//motoSet.destRsv = *motoSet.dest_location;
		motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
		motoSet.manuRsv = motoSet.dspLocation;
	}else
	{
		g_SettingIndex = SETTING_MOTO;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnZoomBack(void)
{
	g_SettingIndex = 0;
	*motoSet.mode = MOTO_MODE_AUTO;
	//motoSet.destRsv = cmrPara.focLen;
	//motoSet.destRsv = focLenScape[GetMotoStdIndex(cmrPara.focLen)];
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void BtnMsetSub(void)
{
	if(SETTING_ADDSUB==g_SettingIndex)
	{
		if(WORKINGMODE_MANU==mstPara->mode)
		{
			if((pSnrSet[SNR_STEP]>0)&&(mstPara->output1==6)&&(mstPara->output2>=0))
			{
				mstPara->output1 = 7;
				mstPara->output2 = 0;
			}else
			{
				if(7==mstPara->output1)
				{
					mstPara->output1 = 0;
				}else
				{
					mstPara->output2++;
					if(mstPara->output2>2)
					{
						mstPara->output1++;
						mstPara->output2-=3;
					}
				}
			}
		}else if(WORKINGMODE_RPT==mstPara->mode)
		{
			if(4==mstPara->multi_exp1)
			{
				mstPara->multi_exp1 = 0;
			}else
			{
				mstPara->multi_exp2++;
				if(mstPara->multi_exp2>2)
				{
					mstPara->multi_exp1++;
					mstPara->multi_exp2-=3;
				}
			}
			MultiMaxTimes(mstPara->rpt_hz,mstPara ->multi_exp1,mstPara ->multi_exp2,rptSet.eTimes,&rptSet.maxTimes);
		}
	}else
	{
		g_SettingIndex = SETTING_ADDSUB;
	}
}


void *BtnM(void)
{
	if(CMD_MODE_OFF==*g_cmdMode)
	{
		BtnMsetSub();
	}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		if(pSnrSet[SNR_SU4]>0)	
		{
			BtnMsetSub();
		}else
		{
			if((g_SettingIndex>SETTING_MST_C)||(g_SettingIndex<SETTING_MST_M))
			{
				return NULL;
			}

			if(MST_SET_PARA==g_mstSetting)
			{
				mainPara.modGr[g_SettingIndex-SETTING_MST_M].multi_exp2++;
				if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].multi_exp2>2)
				{
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].multi_exp1++;
					mainPara.modGr[g_SettingIndex-SETTING_MST_M].multi_exp2-=3;
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].multi_exp1>7)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].multi_exp1 = 0;
					}
				}
			}else
			{
				g_mstSetting = MST_SET_PARA;
			}
		}
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		BtnMsetSub();
	}

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
	return NULL;
}

void *BtnFno(void)
{	
	u8 i=0;
	if(SETTING_EXTM_APERT==g_SettingIndex)
	{
		i = WordSearch(apertValue, 34, mainPara.extApert);
		if(i<34-1)
		{
			mainPara.extApert = apertValue[i+1];
		}else
		{
			mainPara.extApert = apertValue[0];
		}
		
	}else
	{
		g_SettingIndex = SETTING_EXTM_APERT;
		
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnPilot(void)
{
	return NULL;
}

void *BtnSel(void)
{
	switch(*g_cmdMode)
	{
		case CMD_MODE_LIGH_SLA:
			if(SETTING_RMT_GR==g_SettingIndex)
			{
				g_SettingIndex = SETTING_RMT_CH;
			}else if(SETTING_RMT_CH==g_SettingIndex)
			{
				g_SettingIndex = SETTING_RMT_GR;
			}else
			{
				g_SettingIndex = SETTING_RMT_GR;
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		case CMD_MODE_LIGH_MST:
			if(pSnrSet[SNR_MSTRPT]<=0)                       //mst repert
			{
				if(SETTING_RPT_SET==g_SettingIndex)
				{
					g_mstSetting++;
					if(g_mstSetting>6)
					{
						g_mstSetting = 0;
					}
				}else
				{
					g_SettingIndex = SETTING_RPT_SET;
				}
			}else
			{
				//if((g_SettingIndex<SETTING_MST_M)||(g_SettingIndex>SETTING_MST_C))
				if(!g_SettingIndex)
				{
					//g_SettingIndex = SETTING_MST_M;
					if((g_mstSetRsv<SETTING_MST_M)||(g_mstSetRsv>SETTING_MST_C))
					{
						g_SettingIndex = SETTING_MST_M;
						g_mstSetRsv = SETTING_MST_M;
					}else
					{
						 g_SettingIndex = g_mstSetRsv;
					}
					
				}else
				{
					g_SettingIndex++;
					if(g_SettingIndex>SETTING_MST_C)
					{
						g_SettingIndex = SETTING_MST_M;
					}
				}
				g_mstSetting = MST_SET_ARR;
			}
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		break;

		default:
		break;
	}
	return NULL;
}

void *BtnMsc(void)
{
	if(g_isSlaveVoice>0)
	{
		g_isSlaveVoice = 0;
	}else
	{
		g_isSlaveVoice = 1;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnCh(void)
{
	if(SETTING_MST_CHAN==g_SettingIndex)
	{
		mainPara.m_chan++;
		if(mainPara.m_chan>3)
		{
			mainPara.m_chan = 0;
		}
	}else
	{
		g_SettingIndex= SETTING_MST_CHAN;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnFlashMode(void)
{
	g_flashMode++;
	if(g_flashMode>FLASH_MODE_PARALLEL)
	{
		g_flashMode = FLASH_MODE_NORMAL;
	}

#if(defined(REFLACT_EN)&&(REFLACT_EN>0))
	switch(g_flashMode)
	{
		case FLASH_MODE_NORMAL:
			if(motoSet.dspLocation<20)
			{
				motoSet.dspLocation = 20;
			}
		break;

		case FLASH_MODE_PARALLEL:
			if(motoSet.dspLocation>135)
			{
				motoSet.dspLocation = 135;
			}
		break;

		case FLASH_MODE_REFLECT:
			if(motoSet.dspLocation<24)
			{
				motoSet.dspLocation = 24;
			}
		break;
	}

	motoSet.destRsv = GetMotoDest(motoSet.dspLocation);
#else
	//do something
/*
	if(*motoSet.dest_location<=24)
	{
		MotoInvalidMove(50,MOTO_DIR_FORWARD);
		MotoInvalidMove(50,MOTO_DIR_BACKWARD);
	}else
	{
		MotoInvalidMove(50,MOTO_DIR_BACKWARD);
		MotoInvalidMove(50,MOTO_DIR_FORWARD);
	}
*/
#endif

	

	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnFlashWeakOn(void)
{	
	g_dspCmpMod = 1;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

	return NULL;
}

void *BtnFlashWeakOff(void)
{
	g_dspCmpMod = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

	return NULL;
}

void *BtnNullBack(void)
{
	if(g_SettingIndex>0)
	{
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}
	return NULL;
}

void CmpSubAdd(u8 dir,u8 isCircle)
{
	s8 *pCmp1=NULL;
	s8 *pCmp2=NULL;
	
	switch(mstPara->mode)
	{
		case WORKINGMODE_TTL:
		case WORKINGMODE_TTL_POOR:
			pCmp1 = &mstPara->cmp1;
			pCmp2 = &mstPara->cmp2;
		break;

		case WORKINGMODE_AUTO:
			pCmp1 = &mstPara->a_cmp1;
			pCmp2 = &mstPara->a_cmp2;
		break;

		case WORKINGMODE_GN:
			pCmp1 = &mstPara->gn_cmp1;
			pCmp2 = &mstPara->gn_cmp2;
		break;
	}

	if(KEY_DIR_LEFT==dir)
	{
		if((-3==*pCmp1)&&(0==*pCmp2))
		{
			if(isCircle>0)
			{
				(*pCmp1) = 3;
				(*pCmp2) = 0;
			}else
			{
				return;
			}
		}else
		{
			(*pCmp2)--;
			if((*pCmp2)<0)
			{
				(*pCmp1) --;
				(*pCmp2)+=3;
			}
		}
	}else
	{
		if(3==*pCmp1)
		{
			if(isCircle>0)
			{
				*pCmp1 = -3;
				*pCmp2 = 0;
			}else
			{
				return;
			}
		}else
		{
			(*pCmp2)++;
			if((*pCmp2)>2)
			{
				(*pCmp1)++;
				(*pCmp2)-=3;
			}
		}
	}
	
}

void *BtnCmp(void)
{
//	s8 *pCmp1=NULL;
//	s8 *pCmp2=NULL;
	if(CMD_MODE_OFF==*g_cmdMode)
	{
		if(SETTING_ADDSUB==g_SettingIndex)
		{/*
			switch(mstPara->mode)
			{
				case WORKINGMODE_TTL:
					pCmp1 = &mstPara->cmp1;
					pCmp2 = &mstPara->cmp2;
				break;

				case WORKINGMODE_AUTO:
					pCmp1 = &mstPara->a_cmp1;
					pCmp2 = &mstPara->a_cmp2;
				break;

				case WORKINGMODE_GN:
					pCmp1 = &mstPara->gn_cmp1;
					pCmp2 = &mstPara->gn_cmp2;
				break;
			}
			if(3==*pCmp1)
			{
				*pCmp1 = -3;
				*pCmp2 = 0;
			}else
			{
				(*pCmp2)++;
				if((*pCmp2)>2)
				{
					(*pCmp1)++;
					(*pCmp2)-=3;
				}
			}*/
			CmpSubAdd(KEY_DIR_RIGHT,1);
		}else
		{
			g_SettingIndex = SETTING_ADDSUB;
		}
	}else if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		if(pSnrSet[SNR_SU4]>0)
		{
			if(SETTING_ADDSUB==g_SettingIndex)
			{
				CmpSubAdd(KEY_DIR_RIGHT,1);
			}else
			{
				g_SettingIndex = SETTING_ADDSUB;
			}
		}else
		{
			if((g_SettingIndex>SETTING_MST_C)||(g_SettingIndex<SETTING_MST_M))
			{
				return NULL;
			}

			if(MST_SET_PARA==g_mstSetting)
			{
				switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
				{
					case WORKINGMODE_TTL:
					case WORKINGMODE_TTL_POOR:
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1>=3)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1 = -3;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2 = 0;
					}else
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2++;
						if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2>2)
						{
							mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp1++;
							mainPara.modGr[g_SettingIndex-SETTING_MST_M].cmp2-=3;
						}
					}
					break;

					case WORKINGMODE_AUTO:
					if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1>=3)
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1 = -3;
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2 = 0;
					}else
					{
						mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2++;
						if(mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2>2)
						{
							mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp1++;
							mainPara.modGr[g_SettingIndex-SETTING_MST_M].a_cmp2-=3;
						}
					}
					break;
				}
			}else
			{
				g_mstSetting = MST_SET_PARA;
			}
		}
	}
	g_dspCmpMod = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnHead(void)
{
	return NULL;
}

void *Btnm(void)
{
	if(SETTING_GR_M==g_SettingIndex)
	{
		if(mstPara->gn_m>=200)
		{
			mstPara->gn_m = 3;
		}else
		{
			GN_MSubAdd(KEY_DIR_RIGHT);
		}
	}else
	{
		g_SettingIndex = SETTING_GR_M;
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnTimesHz(void)
{
	if((SETTING_MULTI!=g_SettingIndex)&&(SETTING_FRQ!=g_SettingIndex))
	{
		g_SettingIndex = SETTING_MULTI;
	}else
	{
		if(SETTING_MULTI==g_SettingIndex)
		{
			g_SettingIndex = SETTING_FRQ;
		}else
		{
			g_SettingIndex = SETTING_MULTI;

		}
	}
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnPaup(void)
{
	if(g_snrStart>=4)
	{
		g_snrStart -=4;
	}else
	{
		g_snrStart = 16;
	}
	g_snrIndex = g_snrStart;
	DspSnrMenu();
	return NULL;
}

void *BtnPaDn(void)
{
	if(g_snrStart<16)
	{
		g_snrStart +=4;
	}else
	{
		g_snrStart = 0;
	}
	g_snrIndex = g_snrStart;
	DspSnrMenu();
	return NULL;
}

void *BtnSnrSet(void)
{
	if(g_allMyMenuSw>0)
	{
		g_allMyMenuSw = 0;
	}else
	{
		g_allMyMenuSw = 1;
	}
	g_snrIndex = 0;
	g_snrStart = 0;
	DspSnrMenu();
	return NULL;
}

void *BtnSnrNext(void)
{
	g_snrIndex++;
	if(g_snrIndex>=SNR_MENU_CNT)
	{
		g_snrIndex = 0;
		g_snrStart = 0;
	}
	if(g_snrIndex-g_snrStart>3)
	{
		g_snrStart +=4;
	}
	DspSnrMenu();
	return NULL;
}

void *BtnMyMenuBack(void)
{
	if(g_SettingIndex == SETTING_MYMENU)
	{
		g_SettingIndex = SETTING_PFNMENU;
	}
	DspSnrMenu();
	 return NULL;
}

void *BtnMyMenuSel(void)
{
	//u8 i = 0;

	if((g_snrMyMenuBits&(1<<g_snrMyMenuSelIndex))>0)
	{
		g_snrMyMenuBits &= (~(1<<g_snrMyMenuSelIndex));
	}else
	{
		g_snrMyMenuBits |= (1<<g_snrMyMenuSelIndex);
	}

	DspMyMenuSelect();

	return NULL;
}

void AddButtons(Button *btn1, Button *btn2, Button *btn3, Button *btn4,unsigned char listCnt)
{
	//unsigned char btnCnt=0;
	funcMenu.funcList[listCnt].button[0] = btn1;//(Button *)btn1;
	funcMenu.funcList[listCnt].button[1] = btn2;//(Button *)btn2;
	funcMenu.funcList[listCnt].button[2] = btn3;//(Button *)btn3;
	funcMenu.funcList[listCnt].button[3] = btn4;//(Button *)btn4;
	//funcMenu.listCnt++;
}

void BuildFuncMenu(void)
{
	

	Button * pBtnPilot;
	Button *pBtnZoom;
	Button *pBtnMF;

	funcMenu.listCnt = 0;

	if((g_isReflectPulled>0)&&(pSnrSet[SNR_WP]>0))
	{
		pBtnZoom = &btnFlashMode;
	}else
	{
		pBtnZoom = &btnZoom;
	}

	if(!pSnrSet[SNR_MODELING])
	{
		pBtnPilot = &btnPilot;
	}else
	{
		pBtnPilot = &btnModeling;
	}

	if(!pSnrSet[SNR_STD])
	{
		pBtnMF = &btnm;
	}else
	{
		pBtnMF = &btnft;
	}
	
	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			switch(mstPara->mode)
			{
				case WORKINGMODE_TTL:
				case WORKINGMODE_TTL_POOR:
					if(g_isFlashWeak>0)
					{
						AddButtons(pBtnZoom,&btnCmp,&btnFlashWeak,pBtnPilot,funcMenu.listCnt++);
					}else
					{
						AddButtons(pBtnZoom,&btnCmp,&btnNullBack,pBtnPilot,funcMenu.listCnt++);
					}
				break;

				case WORKINGMODE_AUTO:
					if((pSnrSet[SNR_APERT]>1)||(!cmrPara.cls))
					{
						AddButtons(pBtnZoom,&btnCmp,&btnFno,pBtnPilot,funcMenu.listCnt++);					
					}else
					{
						AddButtons(pBtnZoom,&btnCmp,&btnNull,pBtnPilot,funcMenu.listCnt++);		
					}
				break;

				case WORKINGMODE_GN:
					/*if(!pSnrSet[SNR_STD])
					{
						AddButtons(pBtnZoom,&btnCmp,&btnm,pBtnPilot,funcMenu.listCnt++);
					}else
					{
						AddButtons(pBtnZoom,&btnCmp,&btnft,pBtnPilot,funcMenu.listCnt++);
					}*/
					AddButtons(pBtnZoom,&btnCmp,pBtnMF,pBtnPilot,funcMenu.listCnt++);
				break;

				case WORKINGMODE_MANU:
					if(cmrPara.cls>0)
					{
						AddButtons(pBtnZoom,&btnM,&btnNull,pBtnPilot,funcMenu.listCnt++);
					}else
					{
						AddButtons(pBtnZoom,&btnM,&btnFno,pBtnPilot,funcMenu.listCnt++);
					}					
				break;

				case WORKINGMODE_RPT:
					AddButtons(pBtnZoom,&btnM,&btnTimesHz,pBtnPilot,funcMenu.listCnt++);
				break;
			}
		break;

		case CMD_MODE_LIGH_MST:
			if(pSnrSet[SNR_SU4]>0)
			{
				//switch(mstPara->mst_su4_mode)
				switch(mstPara->mode)
				{
					case WORKINGMODE_GN:
						AddButtons(pBtnZoom,&btnCmp,pBtnMF,pBtnPilot,funcMenu.listCnt++);
					break;

					case WORKINGMODE_MANU:
						AddButtons(pBtnZoom,&btnM,&btnFno,pBtnPilot,funcMenu.listCnt++);
					break;

					case WORKINGMODE_AUTO:
						AddButtons(pBtnZoom,&btnCmp,&btnFno,pBtnPilot,funcMenu.listCnt++);
					break;
				}
			}else
			{
				if((g_SettingIndex>SETTING_MST_C)||(g_SettingIndex<SETTING_MST_M))
				{
					AddButtons(pBtnZoom,&btnSel,&btnCh,pBtnPilot,funcMenu.listCnt++);
				}else
				{
					switch(mainPara.modGr[g_SettingIndex-SETTING_MST_M].mode)
					{
						case WORKINGMODE_TTL:
						case WORKINGMODE_AUTO:
							AddButtons(pBtnZoom,&btnSel,&btnCmp,pBtnPilot,funcMenu.listCnt++);
						break;

						case WORKINGMODE_MANU:
							AddButtons(pBtnZoom,&btnSel,&btnM,pBtnPilot,funcMenu.listCnt++);
						break;

						case WORKINGMODE_OFF:
							AddButtons(pBtnZoom,&btnSel,&btnNull,pBtnPilot,funcMenu.listCnt++);
						break;
					}
				}
			}
		break;

		case CMD_MODE_LIGH_SLA:
			if(pSnrSet[SNR_SU4]>0)
			{
				//switch(mstPara->rmt_su4_mode)
				switch(mstPara->mode)
				{
					//case RMT_SU4_AUTO:
					case WORKINGMODE_AUTO:
						AddButtons(pBtnZoom,&btnNull,&btnMsc,pBtnPilot,funcMenu.listCnt++);
					break;

					//case RMT_SU4_M:
					case WORKINGMODE_MANU:
						AddButtons(pBtnZoom,&btnM,&btnMsc,pBtnPilot,funcMenu.listCnt++);
					break;

					//case RMT_SU4_OFF:
					case WORKINGMODE_OFF:
						AddButtons(pBtnZoom,&btnNull,&btnMsc,&btnNull,funcMenu.listCnt++);
					break;
				}
			}else
			{
				AddButtons(pBtnZoom,&btnSel,&btnMsc,pBtnPilot,funcMenu.listCnt++);
			}
		break;
	}
}

void BuildSnrMenu(void)
{

	Button * pBtnPilot;
	funcMenu.listCnt = 0;
	
	if(pSnrSet[SNR_MODELING]>0)
	{
		pBtnPilot = &btnModeling;
	}else
	{
		pBtnPilot = &btnPilot;
	}
	//AddButtons(&btnPaUp,&btnPaDn,&btnSnrSet,&btnPilot,funcMenu.listCnt++);
	if(g_allMyMenuSw>0)
	{
		//AddButtons(&btnPaUp,&btnPaDn,&btnSnrSet_my,&btnSnrNext,funcMenu.listCnt++);
		AddButtons(&btnPaUp,&btnPaDn,&btnSnrSet_my,pBtnPilot,funcMenu.listCnt++);
	}else
	{
		//AddButtons(&btnPaUp,&btnPaDn,&btnSnrSet_all,&btnSnrNext,funcMenu.listCnt++);
		AddButtons(&btnPaUp,&btnPaDn,&btnSnrSet_all,pBtnPilot,funcMenu.listCnt++);
	}
	KeyMenuSw();
}

void BuildSnrSubMenu(void)
{
	Button * pBtnPilot;
	
	funcMenu.listCnt = 0;

	if(pSnrSet[SNR_MODELING]>0)
	{
		pBtnPilot = &btnModeling;
	}else
	{
		pBtnPilot = &btnPilot;
	}
	AddButtons(&btnNull,&btnNull,&btnNull,pBtnPilot,funcMenu.listCnt++);
	KeyMenuSw();
}

void BuildMyMenuSetMenu(void)
{
	funcMenu.listCnt = 0;
	AddButtons(&btnMyMenuBack,&btnMyMenuSel,&btnNull,&btnNull,funcMenu.listCnt++);
	KeyMenuSw();
}

void AreaScroll(u8 *sBuf, char wt,char ht,char aw,char dir,char n)
{
	u32 tmp=0;
	char i,j;

	if(dir>0)
	{
		for(i=0;i<aw;i++)
		{
			tmp = 0;
			for(j=0;j<ht;j++)
			{
				tmp <<= 8;
				tmp|=sBuf[j*wt+i];
			}

			if(SCROLL_UP==dir)
			{
				tmp <<= n;
			}else if(SCROLL_DOWN==dir)
			{
				tmp>>=n;
			}

			for(j=0;j<ht;j++)
			{
				*(sBuf+j*wt+i) = (tmp >> ((ht-j-1)*8))&0xFF;
			}
		}
	}
}


#define BUTTON_LEN (LCD_W/4)

void DisplayButton(u16 x, u8 No, const char *p, u8 len, u8 rsv)
{
//	u16 yLoc;
//	u8 sp;
	//u8 strLen=0;
	u8 i=0;
	//char *pTmp=(char*)p;
//	u8 paraSetBufIndex=0;

	//memset(g_paraSetBuf,0x80,LCD_W);
	memset(g_paraSetBuf,0x00,BUTTON_LEN*2);

	memcpy(g_paraSetBuf+(BUTTON_LEN-len)/2,p,len);
	memcpy(g_paraSetBuf+BUTTON_LEN+(BUTTON_LEN-len)/2,p+len,len);

	for(i=0;i<BUTTON_LEN;i++)
	{
		g_paraSetBuf[i]|=0x80;
	}

	AreaScroll((u8 *)g_paraSetBuf,BUTTON_LEN,16/8,BUTTON_LEN,SCROLL_DOWN,2);

	AreaSet(x, No*BUTTON_LEN, LAYOUT_BUTTON_WIDTH, 16);

	for(i=0;i<(LAYOUT_BUTTON_WIDTH*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}	
/*
	for(i=0;i<4;i++)
	{		
		memcpy(g_paraSetBuf+i*(LCD_W/4)+((LCD_W/4)-len)/2,p,len);
		memcpy(g_paraSetBuf+len+i*(LCD_W/4)+((LCD_W/4)-len)/2,p,len);
	}

	if(!fMod)
	{

		while(*pTmp!='\0')
		{
			strLen+=(CanCharLen((CanFont *)canFontMenu, FONT_MENU_LEN, *pTmp++)+1);
		}

		if(strLen<LAYOUT_BUTTON_WIDTH)
		{
			g_paraSetBufIndex+=((LAYOUT_BUTTON_WIDTH - strLen)/2);
		}

		pTmp = (char*)p;

		while(*pTmp!='\0')
		{
			LoadMenuChar(*pTmp,(g_paraSetBuf+g_paraSetBufIndex),LAYOUT_BUTTON_WIDTH,DSP_REVERSE);
			//g_paraSetBufIndex+=CanCharLen((CanFont *)canFontMenu, FONT_MENU_LEN, *pTmp++);
			g_paraSetBufIndex+=CanCharLen((CanFont *)canFontMenu, FONT_MENU_LEN, *pTmp);
			if(*pTmp!='+')
			{
				g_paraSetBufIndex+=1;
			}
			pTmp++;
		}
	}else
	{
		g_paraSetBufIndex+=((LAYOUT_BUTTON_WIDTH - size)/2);
		
		for(i=0;i<size;i++)
		{
			g_paraSetBuf[i+g_paraSetBufIndex] = ~p[i]&0x1F;
			g_paraSetBuf[i+g_paraSetBufIndex+LAYOUT_BUTTON_WIDTH] = ~p[i+size];
		}
	}

	yLoc = No*(LAYOUT_BUTTON_WIDTH);

	AreaSet(x, yLoc, LAYOUT_BUTTON_WIDTH, 16);

	for(i=0;i<(LAYOUT_BUTTON_WIDTH*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}	
*/	
}


void InsertList(FuncList ls)
{
	DisplayButton(LAYOUT_MENU_PAGE,0,ls.button[0]->dspStr,ls.button[0]->strLen,0);
	DisplayButton(LAYOUT_MENU_PAGE,1,ls.button[1]->dspStr,ls.button[1]->strLen,0);
	DisplayButton(LAYOUT_MENU_PAGE,2,ls.button[2]->dspStr,ls.button[2]->strLen,0);
	DisplayButton(LAYOUT_MENU_PAGE,3,ls.button[3]->dspStr,ls.button[3]->strLen,0);	
}

void KeyMenuSw(void)
{
	InsertList(funcMenu.funcList[g_menuId]);
	pMenuList = &funcMenu.funcList[g_menuId];
}

void LoadButtons(void)
{
	BuildFuncMenu();
	KeyMenuSw();
	//DspMstModIcon(*g_cmdMode);
}

void LoadMotoSetMenu(void)
{
	funcMenu.listCnt = 0;
	if(cmrPara.cls>0)
	{
		AddButtons(&btnZoom,&btnZoomBack,&btnFlashMode,&btnPilot,funcMenu.listCnt++);
	}else
	{
		//AddButtons(&btnZoom,&btnNull,&btnFlashMode,&btnPilot,funcMenu.listCnt++);
		AddButtons(&btnZoom,&btnNullBack,&btnFlashMode,&btnPilot,funcMenu.listCnt++);
	}
	KeyMenuSw();
}

/*
signed char GetMotoStdIndex(u8 dest)
{
	u8 i=0;
	if(dest>=200)
	{
		return MOTO_MAX_INDEX;
	}
	for(i=0;i<sizeof(focLenScape);i++)
	{
		//if(dest==focLenScape[i])
		//{
			//return i-1;
		//}
		//if(dest>=focLenScape[i])
		if((dest>=focLenScape[i])&&(dest<focLenScape[i+1]))
		{
			return i;
		}
	}

	return 0;
}
*/

 char CodeSearch(char *srcArr,char size,const char cd)
{
	char index=0;
	while(index<size)
	{
		if(cd<=srcArr[index])
		{
			return index;
		}else
		{
			index++;
		}
	}
	
	return index;
		
}

char WordSearch(const u16* srcArr,char size,u16 cd)
 {
	char index=0;
	while(index<size)
	{
		if(cd==srcArr[index])
		{
			return index;
		}else
		{
			index++;
		}
		
	}

	return index;
 }

char GetParaStdIndex(const u16* srcArr,char size,u16 cd)
//char GetParaStdIndex(const u16* srcArr,char size,u32 cd)
{
	char index=0;
	while(index<size)
	{
		if(cd<=srcArr[index])
		{
			return index;
		}else
		{
			index++;
		}
		
	}

	return index;
}

char GetIsoStdIndex(const u32* isoArr, char size, u32 cd)
{
	char index=0;
	while(index<size)
	{
		if(cd<=isoArr[index])
		{
			return index;
		}else
		{
			index++;
		}
		
	}

	return index;
}

signed char GetMotoIncremental(signed char iFoc)
{
	//if(iFoc<2)
	if(iFoc<4)
	{
		return 0;
	}else if(iFoc<6)//else if(iFoc<4)
	{
		return iFoc-1;
	}else if(iFoc<7)//else if(iFoc<5)
	{
		return 4;
	}else if(iFoc<9)//else if(iFoc<7)
	{
		//return focLen-1;
		return 5;
	}else
	{
		return 6;
	}
}

#if 0
#define SCALE_MIN 42
#define SCALE_MAX 71
int CalcAvailScale(u8 pFoc, u8 pIso, u8 pApert,u8 exp,s8 cmp)
{
	int sum=34;
	float scale = 6;
	u8 flaMod = 0;
	u8 i=0;

	if(FLASH_MODE_NORMAL==g_flashMode)
	{
		flaMod = 1;
	}else if(FLASH_MODE_PARALLEL==g_flashMode)
	{
		flaMod = 2;
	}else
	{
		flaMod = 0;
	}

	sum += flaMod;
	sum += pFoc;
	sum += pIso;
	sum += exp;
	sum -= cmp;
	sum -= pApert;

	//DBG_PRINTF("para: %d,%d,%d,%d,%d\r\n",pFoc,pIso,exp,cmp,pApert);

	if(sum<SCALE_MIN)
	{
		return 6;
	}else if(sum>SCALE_MAX)
	{
		//return 210;
		return 200;
	}

	//DBG_PRINTF("sum: %d\r\n",sum);

	for(i=0;i<sum-SCALE_MIN;i++)
	{
		scale*=1.1225f;
	}

	//DBG_PRINTF("scale: %d",scale);

	return scale;	
	
}

#else
#define SCALE_PARA_INIT 5
#define SCALE_MIN 3
//start ISO 100(15) , APERT(27) 32 
int CalcAvailScaleMinMax(u8 pFoc, u8 pIso, u8 pApert,u8 isMin,s8 cmp)
{
	//int sum=34;
	s16 x=0;
	s16 y=0;
	u8 y0 = 0;
	float scale = 6;
	s8 flaMod = 0;
	u8 i=0;

	if(FLASH_MODE_NORMAL==g_flashMode)
	{
		flaMod = 0;
	}else if(FLASH_MODE_PARALLEL==g_flashMode)
	{
		flaMod = 1;
	}else
	{
		flaMod = -1;
	}

	x=pFoc;
	//y+=SCALE_PARA_INIT*3;				//the scale table is iso apert 4
	y+=pIso;
	//y+=exp;
	y+=flaMod;
	y-=cmp;
	y-=pApert;

	if(isMin>0)
	{
		if(y<6)			//iso 100 index 15, apert 4 index 9, result is 15-9
		{
			//y = 0;
			scale = gn_min[x][0];
			for (i=0;i<(6-y);i++)
			{
				scale /= 1.1225f; 
			}
			//scale = SCALE_MIN;
		}else if(y<27)
		{
			y-=6;
			y0=y%3;
			y/=3;			
			scale = gn_min[x][y];
			if(1==y0)
			{
				scale*=1.1225f; 
				if(scale>gn_min[x][y+1])
				{
					scale = gn_min[x][y+1];
				}
			}else if(2 == y0)
			{
				scale*=1.26f;
				if(scale>gn_min[x][y+1])
				{
					scale = gn_min[x][y+1];
				}
			}
		}else
		{

			scale = gn_min[x][7];

			y-=27;

			for (i=0;i<y;i++)
			{
				scale*=1.1225f; 
			}

			if (scale>200)
			{
				scale = 200;
			}

		}
		
	}else
	{
		if(y>21)                           //iso 400, apert 1.4
		{
			scale = 200;
		}else if(y>=-12)
		{
			//y-=21;
			y+=12;
			y0=y%3;
			y/=3;
			scale = gn_max[x][y];
			if(1==y0)
			{
				scale*=1.1225f; 
				if(scale>gn_max[x][y+1])
				{
					scale = gn_max[x][y+1];
				}
			}else if(2 == y0)
			{
				scale*=1.26f;
				if(scale>gn_max[x][y+1])
				{
					scale = gn_max[x][y+1];
				}
			}
		}else
		{
			scale = SCALE_MIN;
		}
	}
	return scale;
	
}

int CalcAvailScale(u8 pFoc, u8 pIso, u8 pApert,u8 exp,s8 cmp)
{
	s16 x=0;
	s16 y=0;
	u8 y0 = 0;
	float scale = 6;
	s8 flaMod = 0;
	u8 i=0;

	if(FLASH_MODE_NORMAL==g_flashMode)
	{
		flaMod = 0;
	}else if(FLASH_MODE_PARALLEL==g_flashMode)
	{
		flaMod = 1;
	}else
	{
		flaMod = -1;
	}

	x=pFoc;
	//y+=SCALE_PARA_INIT*3;				//the scale table is iso apert 4
	y+=pIso;
	y+=exp;
	y+=flaMod;
	y-=cmp;
	y-=pApert;
	
	if(y<15)               //ISO 100 APERT 1.0
	{
		scale = g_gnByExp[x][0];
		for(i=0;i<15-y;i++)
		{
			scale /= 1.1225f; 
		}
	}else if(y<=36)
	{
		y-=15;
		y0 = y%3;
		y /= 3;
		scale = g_gnByExp[x][y];
		if(1==y0)
		{
			scale *= 1.1225f; 
		}else if(2==y0)
		{
			scale *= 1.26f;
		}
	}else
	{
		y-=36;
		scale = g_gnByExp[x][7];
		for(i=0;i<y;i++)
		{
			scale *= 1.1225f; 
		}
		
	}

	if(scale<SCALE_MIN)
	{
		return SCALE_MIN;
	}else if(scale>200)
	{
		return 200;
	}else
	{
		return scale;
	}	
	
}

//pshutter 1/500 as standard 0x36
int CalcAvailScaleHp(u8 pFoc, u8 pIso, u8 pApert,u8 exp,u8 pshutte,s8 cmp)
{
	s16 x=0;
	s16 y=0;
	u8 y0 = 0;
	float scale = 6;
	s8 flaMod = 0;
	u8 i=0;

	if(FLASH_MODE_NORMAL==g_flashMode)
	{
		flaMod = 0;
	}else if(FLASH_MODE_PARALLEL==g_flashMode)
	{
		flaMod = 1;
	}else
	{
		flaMod = -1;
	}

	x=pFoc;
	//y+=SCALE_PARA_INIT*3;				//the scale table is iso apert 4
	y+=pIso;
	y+=exp;
	y+=flaMod;
	y-=cmp;
	y-=pApert;
	if(pshutte>0x36)
	{
		y-=(pshutte-0x36)/3;
	}else
	{
		y+=(0x36-pshutte)/3;
	}
	
	if(y<15)               //ISO 100 APERT 1.0
	{
		scale = g_gnByExpHp[x][0];
		for(i=0;i<15-y;i++)
		{
			scale /= 1.1225f; 
		}
	}else if(y<=36)
	{
		y-=15;
		y0 = y%3;
		y /= 3;
		scale = g_gnByExpHp[x][y];
		if(1==y0)
		{
			scale *= 1.1225f; 
		}else if(2==y0)
		{
			scale *= 1.26f;
		}
	}else
	{
		y-=36;
		scale = g_gnByExpHp[x][7];
		for(i=0;i<y;i++)
		{
			scale *= 1.1225f; 
		}
		
	}

	if(scale<SCALE_MIN)
	{
		return SCALE_MIN;
	}else if(scale>200)
	{
		return 200;
	}else
	{
		return scale;
	}	
	
}

#endif

u8 CalcScale(int dst)
{
	u8 tmp=0;
	
	if(dst<6)
	{
		return 0;
	}else if(dst<10)
	{
		tmp = (dst-6)*4;
	}else if(dst<20)
	{
		dst-=10;
		tmp = 15+dst*2;
	}else if(dst<40)
	{
		dst-=20;
		tmp = 35+dst*1;
	}else if(dst<80)
	{
		dst-=40;
		tmp = 55+(dst/2);
	}else if(dst<160)
	{
		dst-=80;
		tmp = 75+(dst/4);
	}else
	{
		dst-=160;
		tmp = 95+(dst/8);
	}

	return tmp;	
	
}

u8 GnCalcScale(int dst)
{
	u8 tmp=0;
	
	if(dst<3)
	{
		return 0;
	}else if(dst<6)
	{
		tmp = (dst-3)*4;
	}else
	{
		tmp = CalcScale(dst) + 8 +4;
	}

	return tmp;
}


#define SCALE_PIC_LEN 108
#define SCALE_DSP_LEN 112
void DrawScaleRuler(u8 mode, u8 para1,u8 para2)
{
	u8 paraSetBufIndex=0;
	u8 i;
	
	memset(g_paraSetBuf,0x00,SCALE_DSP_LEN*2);

	//if(pSnrSet[SNRMENU_STD]>0)
	if(pSnrSet[SNR_STD])
	{
		memcpy(g_paraSetBuf,picScale_ft,SCALE_PIC_LEN);
		memcpy(g_paraSetBuf+SCALE_DSP_LEN,picScale_ft+SCALE_PIC_LEN,SCALE_PIC_LEN);
	}else
	{
		memcpy(g_paraSetBuf,picScale_m,SCALE_PIC_LEN);
		memcpy(g_paraSetBuf+SCALE_DSP_LEN,picScale_m+SCALE_PIC_LEN,SCALE_PIC_LEN);
	}

	switch(mode)
	{
		case WORKINGMODE_MANU:
		case WORKINGMODE_RPT:
		paraSetBufIndex+=(4+para1);
		for(i=0;i<7;i++)
		{
			g_paraSetBuf[paraSetBufIndex+i]|=picCurse[i];
			g_paraSetBuf[paraSetBufIndex+SCALE_DSP_LEN+i]|=picCurse[i+7];
		}
		break;

		case WORKINGMODE_TTL:
		case WORKINGMODE_AUTO:
		for(i=para1;i<para2;i++)
		//for(i=15;i<100;i++)
		{
			g_paraSetBuf[i+7] |= 0x07;
		}
		break;
	}

	AreaSet(7, 10, SCALE_DSP_LEN, 16);

	for(i=0;i<(SCALE_DSP_LEN*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}	
	
	
}


#define GR_SCALE_PIC_LEN 113
#define GR_SCALE_LEN 122//112
void GrDrawScaleRuler(u8 m, u8 para1,u8 para2)
{
	u8 i=0;
	u8 bufIndex=0;
	memset(g_paraSetBuf,0x00,GR_SCALE_LEN*2);

	bufIndex += 3;

	//if(pSnrSet[SNRMENU_STD]>0)
	if(pSnrSet[SNR_STD])
	{
		memcpy(g_paraSetBuf+bufIndex,picGnScale_ft,GR_SCALE_PIC_LEN);
		memcpy(g_paraSetBuf+bufIndex+GR_SCALE_LEN,picGnScale_ft+GR_SCALE_PIC_LEN,GR_SCALE_PIC_LEN);
	}else
	{
		memcpy(g_paraSetBuf+bufIndex,picGnScale_m,GR_SCALE_PIC_LEN);
		memcpy(g_paraSetBuf+bufIndex+GR_SCALE_LEN,picGnScale_m+GR_SCALE_PIC_LEN,GR_SCALE_PIC_LEN);
	}


	if(para2>para1)
	{
		for(i=para1;i<=para2;i++)
		{
			g_paraSetBuf[i+3] |= 0x07;
		}
	}

	for(i=0;i<7;i++)
	{
		g_paraSetBuf[m-3+i]|=picGnFoc[i];
		g_paraSetBuf[m-3+i+GR_SCALE_LEN]|=picGnFoc[i+7];
	}

	for(i=0;i<9;i++)
	{
		g_paraSetBuf[m-4+i]&=~picGnFocMask[i];
		g_paraSetBuf[m-4+i+GR_SCALE_LEN]&=~picGnFocMask[i+9];
	}

	g_paraSetBuf[3]|=0x07;
	g_paraSetBuf[115]|=0x07;
	
	AreaSet(7, 2, GR_SCALE_LEN, 16);

	for(i=0;i<(GR_SCALE_LEN*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}	
	
}

void CopyDataMaskToBuf(char*dest,const char*src,u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
		dest[i] &= (~src[i]);
	}

}

void DspGrScaleNum(u8 x, u8 y, u16 sc, u8 rsv)
{
	//u8 st = (float)sc/1.1225f;
	//u8 se = (float)sc*1.1225f;
	u16 st = 0,se = 0;
//	u8 tmp=0;
	u8 paraSetBufIndex=2;
//	u8 bufAdjSt=0;
	char i=0;

	i = CodeSearch((char*)g_GnScale, 34, sc);
	

	if(i>0)
	{
		st = g_GnScale[i-1];
	}

	if(i<33)
	{
		se = g_GnScale[i+1];
	}

	if(st<3)
	{
		st = 3;
	}

	if(pSnrSet[SNR_STD]>0)
	{
		st = m2feet((int)st);
		sc = m2feet((int)sc);
		se = m2feet((int)se);
	}

	memset(g_paraSetBuf,0x00,112*2);

	if((st>0)&&(sc>st))
	{

		if(st>=100)
		{
			paraSetBufIndex+=2;
			
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(st/100)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(st/100)*12+6],6);

			paraSetBufIndex+=8;

			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(st%100)/10*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(st%100)/10*12+6],6);

			paraSetBufIndex+=6;		
		}else
		{
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(st/10)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(st/10)*12+6],6);

			paraSetBufIndex+=7;

			memcpy(g_paraSetBuf+paraSetBufIndex,&picGnSmDot[0],2);	
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&picGnSmDot[2],2);

			paraSetBufIndex+=3;

			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(st%10)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(st%10)*12+6],6);	

			paraSetBufIndex+=6;	
			
		}
	}else
	{
		paraSetBufIndex+=16;
	}

	paraSetBufIndex+=5;

	if((st>0)&&(sc>st))
	{

		memcpy(g_paraSetBuf+paraSetBufIndex,fontGn_Arrow_Left,3);
		memcpy(g_paraSetBuf+paraSetBufIndex+112,fontGn_Arrow_Left+3,3);
	}

	//AreaScroll(g_paraSetBuf+2, 112, 16, 24, SCROLL_DOWN, 3);
	//paraSetBufIndex+=4;
	paraSetBufIndex+=7;
	if(rsv>0)
	{
		memset(g_paraSetBuf+paraSetBufIndex,0xff,33);
		memset(g_paraSetBuf+paraSetBufIndex+112,0xff,33);
		g_paraSetBuf[paraSetBufIndex] &=0x7f;
		g_paraSetBuf[paraSetBufIndex+32] &=0x7f;
		g_paraSetBuf[paraSetBufIndex+112] &=0xfe;
		g_paraSetBuf[paraSetBufIndex+32+112] &=0xfe;
		
	}
	
	paraSetBufIndex+=3;
	//paraSetBufIndex+=7;

	if(sc<100)
	{
		if(rsv>0)
		{
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/10)],10);
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc/10)]+10,10);

			paraSetBufIndex+=12;

			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex,fontScaleDot,3);
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex+112,fontScaleDot+3,3);

			paraSetBufIndex+=5;

			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc%10)],10);
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc%10)]+10,10);

			paraSetBufIndex+=10;
		}else
		{
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/10)],10);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc/10)]+10,10);

			paraSetBufIndex+=12;

			memcpy(g_paraSetBuf+paraSetBufIndex,fontScaleDot,3);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,fontScaleDot+3,3);

			paraSetBufIndex+=5;

			memcpy(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc%10)],10);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc%10)]+10,10);

			paraSetBufIndex+=10;
		}
		
		
	}else
	{
		if(rsv>0)
		{
			paraSetBufIndex+=4;
			
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/100)],10);
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc/100)]+10,10);

			paraSetBufIndex+=13;

			sc%=100;

			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/10)],10);
			CopyDataMaskToBuf(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc/10)]+10,10);

			paraSetBufIndex+=10;
		}else
		{
			paraSetBufIndex+=4;
			
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/100)],10);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc/100)]+10,10);

			paraSetBufIndex+=13;

			sc%=100;

			memcpy(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/10)],10);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_scaleNum[20*(sc/10)]+10,10);

			paraSetBufIndex+=10;
		}
	}

	paraSetBufIndex+=7;

	//bufAdjSt = paraSetBufIndex;

	if(se>0)
	{

		memcpy(g_paraSetBuf+paraSetBufIndex,fontGn_Arrow_Right,3);
		memcpy(g_paraSetBuf+paraSetBufIndex+112,fontGn_Arrow_Right+3,3);
	}

	paraSetBufIndex+=8;

	if(se>0)
	{
		if(se>=100)
		{
			paraSetBufIndex+=2;
			
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(se/100)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(se/100)*12+6],6);

			paraSetBufIndex+=8;

			se%=100;

			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(se/10)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(se/10)*12+6],6);

			paraSetBufIndex+=6;		
		}else
		{
			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(se/10)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(se/10)*12+6],6);

			paraSetBufIndex+=7;

			memcpy(g_paraSetBuf+paraSetBufIndex,&picGnSmDot[0],2);	
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&picGnSmDot[2],2);

			paraSetBufIndex+=3;

			memcpy(g_paraSetBuf+paraSetBufIndex,&font_gnScaleSmNum[(se%10)*12],6);
			memcpy(g_paraSetBuf+paraSetBufIndex+112,&font_gnScaleSmNum[(se%10)*12+6],6);	

			paraSetBufIndex+=6;			
		}
	}else
	{
		paraSetBufIndex+=16;
	}

	//AreaScroll(g_paraSetBuf+bufAdjSt, 112, 16, paraSetBufIndex-bufAdjSt, SCROLL_DOWN, 3);

	AreaSet(x, y, 112, 16);

	for(i=0;i<(112*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}

	
}

//void DspScaleNum(u8 x, u8 y, u8 sc)
void DspScaleNum(u8 x, u8 y, u16 sc)
{
	u8 paraSetBufIndex=0;
	u8 i;
	
	memset(g_paraSetBuf,0x00,27*2);
	
	if(sc<SCALE_NUM_MIN)
	{
		sc = SCALE_NUM_MIN;
	}

	if(sc<100)
	{
		memcpy(g_paraSetBuf,&font_scaleNum[20*(sc/10)],10);
		memcpy(g_paraSetBuf+27,&font_scaleNum[20*(sc/10)]+10,10);

		paraSetBufIndex+=12;

		memcpy(g_paraSetBuf+paraSetBufIndex,fontScaleDot,3);
		memcpy(g_paraSetBuf+paraSetBufIndex+27,fontScaleDot+3,3);

		paraSetBufIndex+=5;

		memcpy(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc%10)],10);
		memcpy(g_paraSetBuf+paraSetBufIndex+27,&font_scaleNum[20*(sc%10)]+10,10);
	}else
	{
		memcpy(g_paraSetBuf,&font_scaleNum[20*(sc/100)],10);
		memcpy(g_paraSetBuf+27,&font_scaleNum[20*(sc/100)]+10,10);

		paraSetBufIndex+=13;
		sc%=100;

		memcpy(g_paraSetBuf+paraSetBufIndex,&font_scaleNum[20*(sc/10)],10);
		memcpy(g_paraSetBuf+paraSetBufIndex+27,&font_scaleNum[20*(sc/10)]+10,10);
	}

	AreaSet(x, y, 27, 16);

	for(i=0;i<(27*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}	
	
}

void DspScale_m(u8 x, u8 y)
{
	DisplayPicture(x, y, 10, 16, fontScale_m, DSP_NORMAL);
}

void DspScale_ft(u8 x, u8 y)
{
	DisplayPicture(x,y,13,16,fontScale_ft,DSP_NORMAL);
}

//int ParaScaleCalc(u8 exp,s8 cmp,u8 isScale,u8 isMin,u8 ishp)
int ParaScaleCalc(u8 exp,s8 cmp,u8 isScale,u8 isMin)
{
	u8 mFoc=0, mIso=0, mApert=0;
	//u16 iso = 0;
	u32 iso = 0;
	u16 apert = 0;
	u8 isHp = 0;
//	int s1=0,s2=0;

	if(cmrPara.cls>0)
	{
		if((WORKINGMODE_AUTO==mstPara->mode)&&(pSnrSet[SNR_APERT]>1))
		{
			iso = mainPara.extIso;
			apert = mainPara.extApert;
		}else
		{
			iso = cmrPara.iso;
			apert = cmrPara.apert;
		}
		
	}else
	{
		iso = mainPara.extIso;
		apert = mainPara.extApert;
	}

	if((cmrPara.pShutter>0x2d)&&(cmrPara.pShutter<0x80))
	{
		isHp = 1;
	}else
	{
		isHp = 0;
	}

	//mFoc = GetMotoIncremental(GetMotoStdIndex(*motoSet.dest_location));
	if((g_isReflectPulled>0)&&(g_isSoftLightCoverOn>0))
	{
		mFoc = 0;
	}else if(g_isSoftLightCoverOn>0)
	{
		mFoc = 1;
	}else if(g_isReflectPulled>0)
	{
		mFoc = 2;
	}else
	{
		//mFoc = GetMotoStdIndex(*motoSet.dest_location);
		mFoc = GetMotoStdIndex(motoSet.dspLocation);
		mFoc+=3;
	}

	//mIso = WordSearch(isoValue, 35, iso);
	//mIso = GetParaStdIndex(isoValue, 35, iso);
	//mIso = GetParaStdIndex(isoValue, 35, iso);
	mIso = GetIsoStdIndex(isoValue, sizeof(isoValue)/sizeof(u32), iso);

	//mApert = WordSearch(apertValue,34,apert);
	mApert = GetParaStdIndex(apertValue,34,apert);

	if(isScale>0)
	{
		if(isHp>0)
		{
			return CalcAvailScaleHp(mFoc,mIso,mApert,isMin>0?0:21,cmrPara.pShutter,cmp);
			
		}else
		{
			return CalcAvailScaleMinMax(mFoc, mIso, mApert, isMin,cmp);
		}
	}else
	{
		if(isHp>0)
		{
			return CalcAvailScaleHp(mFoc,mIso,mApert,exp,0,cmp);
		}else
		{
			return CalcAvailScale(mFoc,mIso,mApert,exp,cmp);
		}
		//return CalcAvailScale(mFoc,mIso,mApert,exp,cmp);
		//return 0;
	}
	
}

void ManuRptDrawScale(void)
{
	//int a = sqrt(4);
	u8 sc=0;
	u8 mExp = 0;
	int dest=0;

	if(WORKINGMODE_MANU==mstPara->mode)
	{
		mExp = mstPara->output1*3+mstPara->output2;
	}else if(WORKINGMODE_RPT==mstPara->mode)
	{
		mExp = mstPara->multi_exp1*3+mstPara->multi_exp2;
	}

	//dest = CalcAvailScale(mFoc,mIso,mApert,mExp,0);
	//dest  = ParaScaleCalc(mExp,0);
	//dest = ParaScaleCalc(mExp, 0, 0, 0,0);
	dest = ParaScaleCalc(mExp, 0, 0, 0);

	if(dest<6)
	{
		dest = 6;
	}

	sc = CalcScale(dest);

	//DrawScaleRuler(sc);
	DrawScaleRuler(WORKINGMODE_MANU,sc,0);

	if(!pSnrSet[SNR_STD])
	{
		DspScaleNum(5,46+4,dest);
	}else
	{
		DspScaleNum(5,46+4,m2feet(dest));
	}

	if(dest<=6)
	{
		DisplayPicture(5,2+4,8,16,picScaleUnder,DSP_NORMAL);
	}else if(dest>=200)
	{
		DisplayPicture(5,83+4,8,16,picScaleOver,DSP_NORMAL);
	}

	if(!pSnrSet[SNR_STD])
	{
		DspScale_m(5,94+4);
	}else
	{
		DspScale_ft(5,94+4);
	}

	
}

int m2feet(int m)
{
	int feet = m*328;
	return (feet+50)/100;
}


void TtlAutoDrawScale(char isIsoAuto, s8 cmp1, s8 cmp2)
{
	int  st=0, dest=0;                        //cause convert error

	s8 cmp = cmp1*3+cmp2;

	//st = ParaScaleCalc(0,cmp)-2;
	//dest = ParaScaleCalc(7*3,cmp);
	//st = ParaScaleCalc(0, cmp, 1, 1,0);
	//dest  = ParaScaleCalc(0, cmp, 1, 0,0);
	if(isIsoAuto>0)
	{
		st = 6;
		dest = 200;
	}else
	{
		st = ParaScaleCalc(0, cmp, 1, 1);
		dest  = ParaScaleCalc(0, cmp, 1, 0);
	}

/*
	if(st>cmp)
	{
		st-=cmp;
	}else
	{
		st = SCALE_NUM_MIN;
	}	

	if(dest>cmp)
	{
		dest-=cmp;
	}else
	{
		dest= SCALE_NUM_MAX;
	}

	if(st>SCALE_NUM_MAX)
	{
		st = SCALE_NUM_MAX;
	}

	if(dest>SCALE_NUM_MAX)
	{
		dest = SCALE_NUM_MAX;
	}
*/
	if(st<6)
	{
		st = 6;
	}
	if(dest<6)
	{
		dest = 6;
	}

	DrawScaleRuler(WORKINGMODE_TTL,CalcScale(st),CalcScale(dest));

	if(st<=6)
	{
		DisplayPicture(5,2+4,8,16,picScaleUnder,DSP_NORMAL);
	}

	if(!pSnrSet[SNR_STD])
	{
		DspScaleNum(5,13+4,st);
	}else
	{
		DspScaleNum(5,13+4,m2feet(st));
	}

	DisplayPicture(5, 13+4+27+3, 4, 16, fontScale_div, DSP_NORMAL);

	if(!pSnrSet[SNR_STD])
	{
		DspScaleNum(5,13+4+27+3+8,dest);
	}else
	{
		DspScaleNum(5,13+4+27+3+8,m2feet(dest));
	}

	if(dest>=200)
	{
		DisplayPicture(5,83+4,8,16,picScaleOver,DSP_NORMAL);
	}

	if(!pSnrSet[SNR_STD])
	{
		DspScale_m(5,94+4);
	}else
	{
		DspScale_ft(5,94+4);
	}
	

}

void GrDrawScale(void)
{
	//s8  st=0, dest=0;
	//u8 i = 0;
	float set = 0;
	s8 cmp = mstPara->gn_cmp1*3 + mstPara->gn_cmp2;
	//i = CodeSearch((char*)g_GnScale,34, mstPara->gn_m);
	set = GnCalcScale(mstPara->gn_m)+3;
	/*
	if(i>=33)
	{
		set = 115;
	}else
	{
		set = (i*3.3)+3;
	}*/
	//s8 gn_m=0;
	//flashPara.gnSt = ParaScaleCalc(0,cmp)-2;
	//flashPara.gnDst = ParaScaleCalc(7*3,cmp);

	//flashPara.gnSt = ParaScaleCalc(0,cmp,1,1,0);
	//flashPara.gnDst = ParaScaleCalc(0,cmp,1,0,0);

	flashPara.gnSt = ParaScaleCalc(0,cmp,1,1);
	flashPara.gnDst = ParaScaleCalc(0,cmp,1,0);

	
	//gn_m = ParaScaleCalc(mstPara->gn_m);
	//GrDrawScaleRuler(set,CalcScale(flashPara.gnSt),CalcScale(flashPara.gnDst));
	GrDrawScaleRuler(set,GnCalcScale(flashPara.gnSt),GnCalcScale(flashPara.gnDst));
	DspGrScaleNum(5, 2,mstPara->gn_m, SETTING_GR_M==g_SettingIndex);
	if(pSnrSet[SNR_STD]>0)
	{
		DspScale_ft(5,100);
	}else
	{
		DspScale_m(5,100);
	}
}

void DspMstChan(u8 ch, u8 rvs)
{
	u8 i = 0;
	if(ch>3)
	{
		return;
	}
	if(rvs>0)
	{
		memcpy(g_paraSetBuf,picChFrameMask,28*2);
		CopyDataMaskToBuf(g_paraSetBuf+18, &fontMstCh[ch*12], 6);
		CopyDataMaskToBuf(g_paraSetBuf+18+28, &fontMstCh[ch*12+6], 6);
	}else
	{
		memcpy(g_paraSetBuf,picChFrame,28*2);
		for(i=0;i<6;i++)
		{
			g_paraSetBuf[i+18] |= fontMstCh[(ch*12)+i];
			g_paraSetBuf[i+18+28] |= fontMstCh[(ch*12)+i+6];
		}
	}
	LcdCopyToRam(0,144,g_paraSetBuf,28,16);
}


//void DspLightBody(void)
void DspLightBody(u8 isAf, u8 isBk)
{
	if(FLASH_MODE_REFLECT==g_flashMode)
	{
		DisplayPicture(3,140,7,32,picLightBody_fr_reflect,DSP_NORMAL);
	}else if(FLASH_MODE_PARALLEL==g_flashMode)
	{
		DisplayPicture(4,139,8,16,picLightBody_fr_parallel,DSP_NORMAL);
	}
	
	DisplayPicture(4,139+8+1,21,16,picLightBody_up,DSP_NORMAL);
	
	if(SENSOR_UPDN_UP==g_upDnSensor)
	{
		DisplayPicture(4,139+8+1+5,7,16,picHeadUp,DSP_NORMAL);
	}else if(SENSOR_UPDN_DN==g_upDnSensor)
	{
		DisplayPicture(4,139+8+1+5,7,16,picHeadDn,DSP_NORMAL);
	}
	
	if(isAf>0)
	{
		DisplayPicture(6,139+8,11,8,picLightBody_af,DSP_NORMAL);
	}
	if(isBk>0)
	{
		DisplayPicture(6,139+8+1+19+3,2,16,picLightBody_bk,DSP_NORMAL);
	}
	DisplayPicture(6,139+8+11+1,10,16,picLightBody_un,DSP_NORMAL);
}

void DspTempWarning(void)
{
	DisplayPictureEx(58,137,15,32,picTempratureWarn,DSP_NORMAL);
}

void DspNikonCls(u8 cls)
{
	LcdClearFeild(64, 156, 15, 32);
	if(cls>0)
	{
		DisplayPictureEx(64,156,15,24,picLinkCls,DSP_NORMAL);
	}
}

void DspVoiceOff(void)
{
	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		if(g_isSlaveVoice>0)
		{
			DisplayPicture(8,137+15+4,13,16,picVoiceOn,DSP_NORMAL);
		}else
		{
			DisplayPicture(8,137+15+3,16,16,picVoiceOff,DSP_NORMAL);
		}
	}
}

void DspFunctions(void)
{
	DspTempWarning();//in oder to avioding being coverd
	DspLightBody((!pSnrSet[SNR_AF])&&(*g_cmdMode!=CMD_MODE_LIGH_SLA),!pSnrSet[SNR_BKLIGHT]);
	//DspTempWarning();
	DspVoiceOff();
	DspNikonCls(cmrPara.cls);
}

#define LOWBAT_ICON_HT		24
#define LOWBAT_ICON_WT	       44
void LoadLowBatIcon(void)
{
	CHARGE_LED_OFF();
	DISABLE_KEY_INT();
	ADC_DeInit(ADC1);
	SPI_I2S_DeInit(SPI1);
	DISABLE_CHARGE();

	LCD_Clear(0);
	DisplayPicture((12-(LOWBAT_ICON_HT/8))/2, (LCD_W-LOWBAT_ICON_WT)/2, LOWBAT_ICON_WT, 
		LOWBAT_ICON_HT, picLowBat, DSP_NORMAL);
}


void BuildRfOnlyMenu(void)
{
	funcMenu.listCnt = 0;
	AddButtons(&btnNull,&btnNull,&btnNull,&btnNull,funcMenu.listCnt++);
	KeyMenuSw();
}


void BuildStbyMenu(void)
{
	funcMenu.listCnt = 0;
	AddButtons(&btnNull,&btnNull,&btnNull,&btnMyMenuBack,funcMenu.listCnt++);
	KeyMenuSw();
}

void BuildKeyLockMenu(void)
{
	funcMenu.listCnt = 0;
	AddButtons(&btnNull,&btnNull,&btnNull,&btnPilot,funcMenu.listCnt++);
	KeyMenuSw();
}


void MultiMaxTimes(s8 frq,s8 exp1,s8 exp2,s8 *eTm,u8 *maxTm)
{
#if 1
//	unsigned char frqTmp;
	u8 arrNum;
	u8 rowNum;
	u8 tMax;

/*
	if(frq>10)
	{
		frqTmp = 11;
	}else
	{
		frqTmp = frq;
	}

	if(frq<=3)
	{
		arrNum = 0;
	}else
	{
		arrNum = frqTmp - 3;
	}
*/
	if(frq<3)
	{
		arrNum = 0;
	}else if(frq<11)
	{
		arrNum = frq-2;
	}else 
	{
		arrNum = 9;
	}

	rowNum = exp1*3+exp2;

	if(rowNum<=3)
	{
		rowNum = 0;
	}else if(rowNum<=12)
	{
		rowNum = rowNum - 3;
	}else
	{
		rowNum = 9;
	}
	
#if 1
	tMax =  nikMaxCalc[rowNum][arrNum];	
	
	*maxTm = tMax;

	if(*eTm>tMax)
	{
		//tMax = eTm;
		*eTm = tMax;
	}
#endif
#endif	
}




