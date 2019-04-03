#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"
#include "delay.h"	 
#include "includes.h"
#include "../../APP/Golbal.h"
#include "../../APP/Display.h"
#include "../../APP/KeyProc.h"
#include "../../APP/canon.h"
#include "../STMFLASH/stmflash.h"
#include "../../APP/menu.h"
#include "../DMA/DMA.h"
#include "../../APP/FlashPara.h"

#define DISP_ON           0XAF
#define DISP_OFF         0XAE

//u16 g_penColor = BLACK;//BACK_COLOR = LIGHTGREEN;  
//u16 DeviceCode;	 
//u16 rdt;
extern unsigned char g_dspStyle;
extern char g_SettingIndex;
extern unsigned char uart_tran_flag;
//extern unsigned char g_wlCmdMod;
extern unsigned char *g_cmdMode;
extern unsigned char g_menuId;
extern unsigned char g_menuIdRsv;
extern MultiSet multiSet;
extern signed char g_grSel;
extern RadioSet* radioSet;
extern char g_idSetIndex;
extern SnMac g_snMac;
extern char *pSnrSet;
extern signed char g_pfnIndex;
extern signed char g_pfnStart;
extern signed char g_ArrSel;
extern RadioStatue radioStatue;
extern unsigned int g_rfIdSet;
signed char g_chSan=0;
char g_isChScan=0;
extern u8 g_isRgbFilter;
extern u8 g_upDnSensor;
extern CanWireless canWireless;
extern unsigned char g_moto_error;
extern u8 g_expTTLPara3;
extern char g_RgbCheckTurn;
extern u8 g_chargeflag;

#define GR_DSP_LEN	17
#define ABr_DSP_LEN	15
#define PAGE_MAX 13

/************************************************************************/
//ST75256 MACRO
//======================================
//#define OFF 0
//#define ON 1 
//#define HIGH   1
//#define LOW    0
//#define REVERSE 1
//#define NORMAL  0
//#define P6800  1
//#define P8080  2
//#define SPI4   3 
//#define DATA_IO P1
//==========================
//#define MODE 	    SPI4
//#define UP_START    0  	 //0-7 page	  
//#define UP_END      6	  
//#define DOWN_START  7	 //0-7 page
//#define DOWN_END    12
#define LCD_WIDTH       172
#define SEG_START	84 //
//#define LINE1		0	//adjust vop screen	(EV)
//#define LINE2		2	//adjust vop screen	(VOP=**.**V)
#define EV_ST75256        300//280//250//200//150//0x55//360//323  //VOP=3.6+EV*0.04 (5-335) 
//#define EV_MIN        300
//#define EV_MAX       330
//#define EV_LSB	 	0x20 		//0x00-0x3f D5-D0
//#define EV_MSB	    0x02		//0x00-0x07 D2-D0
//#define BOOSTER_EFFICIENCY 1  //0: 3KHz   1: 6KHz
							  //2: 12KHz  3: 24KHz 
//#define BIAS 	    2	  //    BIAS        BIAS  
                          //0:  1/14	3:  1/11
						  //1:  1/13    4:  1/10
						  //2:  1/12    5:  1/9	
//#define CLD 	0	      //0: NOT DIVIDE 1:  1:2 DIVISION
//#define DUTY 	103	  //real=DUTY+1						 					     
//#define BOOSTER  	  1   // 0:X8  1:X10
//#define TA		  0 //temp=TA-40	 10
//#define TB		  20 //temp=TB-40	 20
//#define TC		  40 //temp=TC-40	 30
//#define FREQUENCYA     0X1f	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 -30 TO TA
//#define FREQUENCYB     0X1F	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 TA TO TB
//#define FREQUENCYC     0X1F	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 TB TO TC
//#define FREQUENCYD     0X1F	//Frame Rate:[D4-D0] (* * * D4 D3 D2 D1 D0)	 TC TO 90
//#define VB  ON		 //internal booster
//#define VR  ON		 //internal regulator
//#define VF  ON		 //internal follower
//#define HPM  HIGH //HIGH:HIGH POWEWR MODE  LOW:NORMAL MODE
//#define CL   0 		  //0:in the column direction 1:in the page direction  *9.2.9*
//#define MX  REVERSE   // SEG DIRECTION: NORMAL:0->131  REVERSE: 131->0
//#define MY  NORMAL	  // COM_DIRECTIONN: ORMAL:0->63   REVERSE: 63->0
//#define DISP_MODE  	 0 	//0: MONO   1: 4GRAY SCALE MODE
//#define ALL_PIXEL  OFF//ALL PIXEL ON/OFF
//#define DISP           NORMAL	 //正显or反显  
//#define VOP_COMPENSATE  -0.03   //VOP补偿	  
//=======================================================
//#define DISP_ON           0XAE|ON
//#define DISP_OFF          0XAE|OFF
#define EV_LSB		 (EV_ST75256&0X3F)
#define EV_MSB		 (((EV_ST75256>>1)&0XE0)>>5)
#define EV_MAX		330
#define EV_MIN		300

/************************************************************************/

//char g_paraSetBuf[208];

unsigned char g_paraSetBufIndex=0;
extern u8 g_isIndividualSlave;
//extern signed char g_cfnSubSel;

//#define LCD_WRITE_DELAY()   //{delay_us(1);}  //the flag of spi transcent accomplished is useless, so use this to wait for data transcent ok


const char cmpArr[][4] = 
{
	//"0.0","0.3","0.7","1.0","1.3","1.7","2.0","2.3","2.7","3.0","3.3","3.7","4.0","4.3","4.7","5.0","5.3","5.7","6.0",
	"0","0*3","0*7","1","1*3","1*7","2","2*3","2*7","3","3*3","3*7","4","4*3","4*7","5","5*3","5*7","6",
};

const char cmpArr_5[][4] = 
{
	"0","0*5","1","1*5","2","2*5","3","3*5","4","4*5","5","5*5","6",
};

const char scaleArr[][4] = 
{
	"0.5","1","2","4","9","18",
};

const char scaleArrEx[][4] =
{
	"1.7","4","7","15","30","60",
};

const char scopeNum[][4] = 
{
	"0.5","0.7","1.0","1.5","2.0","3.0","4.0","6.0","9.0","13","18",
};

const char scopeNumEx[][4] = 
{
	"1.7","2.3","4","5.0","7.0","10","15","20","30","40","60",
};

const char outputString[] ="1/128. .1/64. .1/32. .1|/16. .1|/|8. .1|/|4. .1|/|2. .1|/|1..";
const char otpStrIndex[] = {0,5,7,8,12,14,15,19,21,22,27,29,30,35,37,38,43,45,46,51,53,54};
//const char outputString[] = "||1/128||. .||||1/64|||. .||||1/32||||. .||||1|/16||||. . |1|/|8 . . |1|/|4||||. . |1|/|2 . . |1|/|1| ";
//const char otpStrIndex[] = {3,5,6,9,11,12,15,17,18,21,23,24,26,28,29,31,33,34,36,38,39,41};
const char cmpString[] = "-3..2..1..;..1..2.+3";
const char cmpString_5[] = "-3lr2lr1lr;lr1lr2lk3";
const char cmpStringEx[] = "s3..2..1..;..1..2.a3";
const char cmpStringEx_5[] = "c3.2.1.;.1.2b3";
//const char abRatio[] = "8:1 . 4:1 . 2:1 . 1:1 . 1:2 . 1:4 . 1:8";
const char abRatio[] = "8:1.4:1.2:1.1:1.1:2.1:4.1:8.";
const char m_dotSetLoc[] = {11,26,35,50,65,74,89};
const char m_dotSetLocEx[] = {9,22,29,42,55,62,75};
const char multi_dotSetLoc[] = {11,26,35,50,89,89,89};
const char multi_dotSetLocEx[] = {9,22,29,42,75,75,75};
const float gnFocMul[] = {1.74,1.07,1.07,1.2,1.16,1.19,1.05,1.09,1.01,1.02,1.01};
/*
float v_gn_up[] =
{
	15,26,28,30,36,42,50,53,58,59,60
};

float v_gn_dn[] =
{
	1.3,2.3,2.5,2.7,3.2,3.7,4.4,4.7,5.1,5.2,5.3,
};

float v_gn_m[] = 				//should be verified
{
	 1.3,2.3,2.5,2.7,3.2,3.7,4.4,4.7,5.1,5.2,5.3,
};
*/

const int isoValue[] = 
{
	25,32,40,50,64,80,100,125,160,200,250,320,400,500,640,800,1000,1250,1600,2000,
	2500,3200,4000,5000,6400,8000,10000,12800,16000,20000,25600,32000,40000,51200,
};

const int apertValue[] = 
{
	10,11,12,14,16,18,20,22,25,28,32,35,40,45,50,56,63,71,80,90,100,110,130,140,
	160,180,200,220,250,290,320,360,400,450,510,570,640,720,810,910,
};

const float scaleStep[] = {0,0.7,1,1.5,2,3,4,6,9,13,18};

void *BtnAddSub(void);
void *BtnTtlFeb(void);
void *BtnRatio(void);
void *BtnMenu(void);
void *BtnZoon(void);
void *BtnMulti(void);
void *BtnFrq(void);

void *BtnGrSel(void);
void *BtnMstSw(void);
void *BtnIdSet(void);
void *BtnIdSel(void);
void *BtnChSet(void);
void *BtnMemory(void);
void *BtnReset(void);
void *BtnRel(void);
void *BtnSnrMenu(void);
void *BtnSmSel(void);
void *BtnSmSub(void);
void *BtnHome(void);
void *BtnChScan(void);
void *BtnSynSet(void);
void *BtnPfnMenu(void);
void *BtnGrOnOff(void);
void *BtnGrModRes(void);
void *BtnGrParaRes(void);
void *BtnArrParaRes(void);
void *BtnExtmIso(void);
void *BtnExtmApert(void);
void *BtnSave(void);
void *BtnLoad(void);
void *BtnCfnClear(void);
void *BtnCfnClearCancel(void);
void *BtnSlaveModel(void);
void *BtnSlaveTest(void);
void* LoadCfnAllClear(void);
#ifdef FIBER_ADJUST_PROC
void *BtnFbAdj(void);
void *BtnFbAddSub(void);
void *BtnRgbCheck(void);
#endif
//void BtnRfGrMod(void);
extern void KeyOk(void);
//void AddButtons(Button *btn1, Button *btn2, Button *btn3, Button *btn4,unsigned char listCnt);
void AddButtons(Button *btn1, Button *btn2, Button *btn3, Button *btn4,unsigned char listCnt);
extern void CanSynMode(/*signed char mode,*/signed char* sm,char mDsp);
char LoadCanParaChar(char ch,char *gBuf,char aLen,unsigned char rvs);
//u16 DisplayCanOtpString(u8 dspLen,u8 blkLen,/*u16 page,u16 cml,*/u16 wt,char * str,unsigned char rvs);
u16 DisplayCanOtpString(u8 dspLen,u8 spLen,u8 blkLen,/*u16 page,u16 cml,*/u16 wt,char * str,unsigned char rvs);
u16 DisplayCanCmpString(u16 wt,char * str,char dspLen,unsigned char rvs);
void DisplayMstOtp_canStyle(u16 page,u16 cml,char mod,char dspLen,char blockLen,unsigned char para1,unsigned char para2,u8 spLen,char *pDot,char dir,char n,unsigned char rvs);
char CanCharGetIndex(CanFont *font, char fLen,char ch);
char LoadCmpFebChar(char ch,char *gBuf,char dspLen,unsigned char rvs);
char Lcd_ShowCmpFebChar(u16 page,u16 cml,char ch,u8 rsv);
void DspCmpFeb(u16 x,u16 y,const char *p,u8 rvs);
u8 CanCharLen(CanFont *font, char fLen,char ch);
u8 TitleLen(TitleFont *tFont, char tLen, char ch);
void DspMstRatioHead(u16 x, u16 y, const char* p, u8 rsv,u8 sz);
void DisplayABRation(signed char ABr1,signed char ABr2,unsigned char rsv);
char LoadMenuChar(char ch,char *gBuf,char dspLen,unsigned char rvs);
void DspSensors(char page, char cml, char fla, char rgbFilter,char upDn,char mstSla);
void DspSensorsEx(char page, char cml, char fla, char rgbFilter,char upDn,char mstSla);
//void DrawScale(s8 mode,u8 para,char on);
void DrawScale(signed char mode,unsigned char st,unsigned char isHp,u8 para,char on);
char RamLoadPicture(char *p,char wt, char ht ,char *gBuf, char dspLen, unsigned char rvs);
//void DspSlavePara(u8 rvs);
void DspSlavePara(char mod, char para1, char para2, char arr, char rvs);
void DspIsoValue(u16 x, u16 y,int iso,u8 rvs);
void DspApertValue(int apert, u8 rvs);
void CalcAvailScale(signed char mode, u8 mIn,signed char *up, signed char *dn);
char TitleGetIndex(TitleFont *font, char fLen,char ch);
void SetContrast(char lev);
void CalcAvailScaleEx(signed char *up, signed char *dn);
void CalcAvailScaleHp(signed char mode, u8 mIn, unsigned char st,signed char * up, signed char * dn);
void DspSensorRgbWarn(char page, char cml);
#ifdef FIBER_ADJUST_PROC
void FiberAdjustProc(void);
#endif
void LoadUncompetibleIcon(void);
void ReStdPara(signed char *p1, signed char *p2);

Button mNull = {0,"",0,{NULL,NULL}};
#ifdef _ST_EX_
Button cfn={0,"C.Fn",(sizeof("C.Fn")-1)*FONT_BUTTON_WT-6,{NULL,BtnSnrMenu}};
#define mstFlaSw mNull
#else
Button cfn={0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*FONT_BUTTON_WT-6,{BtnZoon,BtnSnrMenu}};
Button cfnNul = {0,"C.Fn",(sizeof("Zm/C.Fn")-1)*FONT_BUTTON_WT-6,{NULL,BtnSnrMenu}};
Button mstFlaSw = {1,(char*)font_flaSw,35,{BtnMstSw,NULL}};
#endif
Button addSub = {1,(char*)f_menuCmp,10,{BtnAddSub,NULL}};
//Button addSub = {0,"+-",10,{BtnAddSub,NULL}};
Button ttlFeb = {0,"FEB",(sizeof("FEB")-1)*FONT_BUTTON_WT,{BtnTtlFeb,NULL}};
Button menu1 = {0,"MENU 1",(sizeof("MENU1")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button menu2 = {0,"MENU 2",(sizeof("MENU2")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button menu3 = {0,"MENU 3",(sizeof("MENU3")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button menu4 = {0,"MENU 4",(sizeof("MENU4")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button mRatio = {0,"RATIO",(sizeof("RATIO")-1)*FONT_BUTTON_WT,{BtnRatio,NULL}};
//Button mGr = {0,"GR",(sizeof("GR")-1)*FONT_BUTTON_WT,{BtnGrSel,NULL}};
Button mGr = {0,"Gr",(sizeof("Gr")-1)*FONT_BUTTON_WT,{BtnGrSel,NULL}};
Button mCh = {0,"CH",(sizeof("CH")-1)*FONT_BUTTON_WT,{BtnChSet,NULL}};
Button mId = {0,"ID",(sizeof("ID")-1)*FONT_BUTTON_WT,{BtnIdSel,NULL}};
Button mScan = {0,"SCAN",(sizeof("SCAN")-1)*FONT_BUTTON_WT,{BtnChScan,NULL}};
Button mSync = {0,"SYNC",(sizeof("SYNC")-1)*FONT_BUTTON_WT,{BtnSynSet,NULL}};
Button mMemory = {0,"MEMORY",(sizeof("MEMORY")-1)*FONT_BUTTON_WT,{BtnMemory,NULL}};
Button mMulti = {0,"MULTI",(sizeof("MULTI")-1)*FONT_BUTTON_WT,{BtnMulti,NULL}};
Button mFrq = {0,"Hz",(sizeof("Hz")-1)*FONT_BUTTON_WT,{BtnFrq,NULL}};
//Button mCancel = {0,"HOME",(sizeof("HOME")-1)*FONT_BUTTON_WT,{BtnHome,NULL}};
Button mCancel = {0,"CANCEL",(sizeof("CANCEL")-1)*FONT_BUTTON_WT,{BtnCfnClearCancel,NULL}};
Button mOK = {0,"OK",(sizeof("OK")-1)*FONT_BUTTON_WT,{BtnCfnClear,NULL}};
Button mIdSel = {0, "SEL",(sizeof("SEL")-1)*FONT_BUTTON_WT,{BtnIdSet,NULL}};
Button mReset = {0, "",(sizeof("Reset")-1)*FONT_BUTTON_WT,{BtnReset,NULL}};
Button mRel = {0,"REL",(sizeof("Rel")-1)*FONT_BUTTON_WT,{BtnRel,NULL}};//linked shot mode to triger the shutter;
Button mSmSel = {0,"SEL",(sizeof("SEL")-1)*FONT_BUTTON_WT,{BtnSmSel,NULL}};
Button mSmSubSel = {0,"SUB",(sizeof("SUB")-1)*FONT_BUTTON_WT,{BtnSmSub,NULL}};
Button cfnEx={0,"C.Fn",(sizeof("C.Fn")-1)*FONT_BUTTON_WT-6,{BtnSnrMenu,NULL}};
Button mPfn={0,"P.Fn",(sizeof("P.Fn")-1)*FONT_BUTTON_WT-6,{BtnPfnMenu,NULL}};
Button mClear={0,"CLEAR",(sizeof("CLEAR")-1)*FONT_BUTTON_WT-6,{LoadCfnAllClear,NULL}};
Button mGrIsOff={0,"ON/OFF",(sizeof("ON/OFF")-1)*FONT_BUTTON_WT-6,{BtnGrOnOff,NULL}};
Button mGrResMod = {0,NULL,0,{BtnGrModRes,NULL}};
Button mGrResPara = {0,NULL,0,{BtnGrParaRes,NULL}};
Button mArrResPara = {0,NULL,0,{BtnArrParaRes,NULL}};
Button mBack = {1,(char*)f_menuBack,11,{BtnHome,NULL}};
Button mModel={0,"MODEL",(sizeof("MODEL")-1)*FONT_BUTTON_WT-6,{BtnSlaveModel,NULL}};
Button mTest={0,"TEST",(sizeof("TEST")-1)*FONT_BUTTON_WT-6,{BtnSlaveTest,NULL}};
Button mIso={0,"ISO",(sizeof("ISO")-1)*FONT_BUTTON_WT-6,{BtnExtmIso,NULL}};
Button mApert={0,"F",(sizeof("F")-1)*FONT_BUTTON_WT-6,{BtnExtmApert,NULL}};
Button mSave={0,"SAVE",0,{BtnSave,NULL}};
Button mLoad = {0,"LOAD",0,{BtnLoad,NULL}};
#ifdef FIBER_ADJUST_PROC
Button mFbAdj = {0,"ADJ",0,{BtnFbAdj,NULL}};
Button mFbAddSub = {1,(char*)f_menuCmp,10,{BtnFbAddSub,NULL}};
Button mRgbChk1 = {0,"RGB1",0,{BtnRgbCheck,NULL}};
Button mRgbChk2 = {0,"RGB2",0,{BtnRgbCheck,NULL}};
Button mRgbChk3 = {0,"RGB3",0,{BtnRgbCheck,NULL}};
Button mRgbChk4 = {0,"RGB4",0,{BtnRgbCheck,NULL}};
#endif
//Button mr={0,"REL",(sizeof("REL")-1)*FONT_BUTTON_WT-6,{NULL,NULL}};


char DisplayTitleChar(char ch,char *gBuf,u8 dspLen,unsigned char rvs);

//FuncList subFuncList;
FuncMenu funcMenu;

void LcdDelay()
{
	return;
}

#ifdef LCD_DRIVER_TSERIAL

void LCD_Writ_Bus(char a)
{
	SPI_NSS_L;
	SPI2->DR = a;
	//delay_us(3);
	while(!(SPI2->SR&0x02));
	while((SPI2->SR&0x80));
	SPI_NSS_H;	
	//return SPI2->DR;
}

void LCD_WRITE_CMD(char cmd)
{
	//LCD_CD = 0;
	//LCD_Writ_Bus(cmd);
	SPI_NSS_L;
	LCD_CD = 0;
	SPI2->DR = cmd;
	//LCD_WRITE_DELAY();
	while(!(SPI2->SR&0x02));
	while((SPI2->SR&0x80));
	//while((SPI2->SR&0x02));
	SPI_NSS_H;	
}

void LCD_WRITE_BYTE(char dat)
{
	//LCD_CD = 1;
	//LCD_Writ_Bus(dat);
	SPI_NSS_L;
	LCD_CD = 1;
	SPI2->DR = dat;
	//LCD_WRITE_DELAY();
	while(!(SPI2->SR&0x02));
	while((SPI2->SR&0x80));
	//while((SPI2->SR&0x02));
	SPI_NSS_H;	
}

/*
void LCD_WRITE_DATA(int a)
{
	LCD_CD = 1;
	LCD_Writ_Bus(a>>8);
	LCD_Writ_Bus(a);
}
*/

#if 0
u16 LCD_WR_DATA(int dat)
{
	u16 rdt;
	SPI2->DR=(dat>>8);
	while(!(SPI2->SR&0x01));
	rdt = SPI2->DR;
	SPI2->DR=(dat&0xff);
	while(!(SPI2->SR&0x01));
	rdt = SPI2->DR;
	return rdt;
}
#endif

#define LCD_WR_DATA(dat)\
{\
SPI2->DR=(dat>>8);\
while(!(SPI2->SR&0x02));\
SPI2->DR=(dat&0xff);\
while(!(SPI2->SR&0x02));\
}


void AreaSet(u16 page,u16 cml,u16 wt,u16 ht)
{
	LCD_WRITE_CMD(0x30);
	LCD_WRITE_CMD(0x75); //set page address
	LCD_WRITE_BYTE(page);
	LCD_WRITE_BYTE(page+(ht/8)-1);
	LCD_WRITE_CMD(0x15); //set column address
	LCD_WRITE_BYTE(SEG_START+cml);
	LCD_WRITE_BYTE(SEG_START+cml+wt-1); 
	LCD_WRITE_CMD(0x5c);
#if 0
	LCD_WRITE_CMD(0x2a); 	//x轴   
	LCD_WRITE_DATA(x1);
	LCD_WRITE_DATA(x2);
	LCD_WRITE_CMD(0x2b); 	//y轴   
	LCD_WRITE_DATA(y1);
	LCD_WRITE_DATA(y2);
	LCD_WRITE_CMD(0x2c);
#endif
}

#if 1
void ST75256_Init(void)
{
/*
    	LCD_RESET=1;
	delay_ms(2);
    	LCD_RESET=0;
    	delay_ms(2);
    	LCD_RESET=1;
    	delay_ms(200);
*/
	//int evTmp=EV_MIN;
	//if((pfnSet[0]<0)||(pfnSet[0]>4))
	//{
	//	pfnSet[0] = 0;
	//}
	//evTmp = ((EV_MAX-EV_MIN)/5)*pfnSet[0]+EV_MIN;
    		
	LCD_WRITE_CMD(0x30);   	// Extension Command 1

	LCD_WRITE_CMD(0x94);   	// Sleep out
	//LCD_WRITE_CMD(DISP_OFF);    	//Display off
	LCD_WRITE_CMD(0XAE); 
	delay_ms(50);
		
	LCD_WRITE_CMD(0x20);   	// Power control
	//LCD_WRITE_BYTE((VB<<3)|(VF<<1)|VR);  // D0=regulator ; D1=follower ; D2=booster,  on:1 off:0
	LCD_WRITE_BYTE(0X0B); 

	LCD_WRITE_CMD(0x81);   	// EV control	
	
	LCD_WRITE_BYTE(EV_LSB);  	// VPR[5-0]
	LCD_WRITE_BYTE(EV_MSB);   // VPR[8-6]  vop=(10-3.6)/0.04=160=1010 0000
	//LCD_WRITE_BYTE(evTmp&0x3F); 
	//LCD_WRITE_BYTE(((evTmp>>1)&0XE0)>>5); 
	//SetContrast(pfnSet[0]);
	
	LCD_WRITE_CMD(0x31);   	// Extension Command 2
	LCD_WRITE_CMD(0x32);   	// analog circuit set
	LCD_WRITE_BYTE(0x00); 	//
	//LCD_WRITE_BYTE(BOOSTER_EFFICIENCY);   // Frequency on booster capacitors 6khz
	LCD_WRITE_BYTE(0X01);
	LCD_WRITE_BYTE(0X02);//LCD_WRITE_BYTE(BIAS);	//1/9 Bias	0X05
	
	LCD_WRITE_CMD(0x51);   	// Booster Level x10
	LCD_WRITE_BYTE(0XFB);//LCD_WRITE_BYTE(0XFA|BOOSTER);
	
	LCD_WRITE_CMD(0x30);   	// Extension Command 1
//	LCD_WRITE_CMD(0xee);   	//disable read modify write   *****

	LCD_WRITE_CMD(0xF0);   	// Gray or mono
	LCD_WRITE_BYTE(0X10);//LCD_WRITE_BYTE(0X10|DISP_MODE);    //

//	LCD_WRITE_CMD(0x76);  // disable icon

	LCD_WRITE_CMD(0xCA);   	// Display Control
	LCD_WRITE_BYTE(0X00);//LCD_WRITE_BYTE(CLD<<2);   // 1/2 division  0X04
	LCD_WRITE_BYTE(103);//LCD_WRITE_BYTE(DUTY);	// 1/64duty	 
	LCD_WRITE_BYTE(0x00);      	//
	
	LCD_WRITE_CMD(0xBC);   	// data scan direction -----------
	LCD_WRITE_BYTE(0X02);//LCD_WRITE_BYTE((CL<<2)|(MX<<1)|MY);   	// Data scam direction
	//LCD_WRITE_BYTE(0x01); 

	LCD_WRITE_CMD(0XA6);//LCD_WRITE_CMD(0XA6|DISP);   	// Normal display
	
	LCD_WRITE_CMD(0x31);   	// Extension Command 2
	LCD_WRITE_CMD(0x40);   	// Internal Power Supply 
	LCD_WRITE_CMD(0x49);//LCD_WRITE_CMD(0x48|HPM);//high power mode set 

	LCD_WRITE_CMD(0xf2);	// set frequency of frame rate
	LCD_WRITE_BYTE(0);//LCD_WRITE_BYTE(TA);	 //0
	LCD_WRITE_BYTE(20);//LCD_WRITE_BYTE(TB);	 //30
	LCD_WRITE_BYTE(40);//LCD_WRITE_BYTE(TC);	 //60
	 
	LCD_WRITE_CMD(0xf0);	// set frequency of frame rate
	LCD_WRITE_BYTE(0X1F);//LCD_WRITE_BYTE(FREQUENCYA);	 //the Frame Rate in temp. -30~TA
	LCD_WRITE_BYTE(0X1F);//LCD_WRITE_BYTE(FREQUENCYB);	 //the Frame Rate in temp. TA~TB
	LCD_WRITE_BYTE(0X1F);//LCD_WRITE_BYTE(FREQUENCYC);	 //the Frame Rate in temp. TB~TC
	LCD_WRITE_BYTE(0X1F);//LCD_WRITE_BYTE(FREQUENCYD);	 //the Frame Rate in temp. TC~90
//	temp_compensation();

	LCD_WRITE_CMD(0x30);
	//if(ALL_PIXEL)
	//LCD_WRITE_CMD(0X23); //all on	
	//LCD_WRITE_CMD(0XAF); //LCD_WRITE_CMD(DISP_ON);    	//Display on
	LCD_WRITE_CMD(DISP_ON);
	
}

#else

void ST75256_Init(void)
{
	LCD_WRITE_CMD(0x30);
	// Extension Command 1
	LCD_WRITE_CMD(0x6E);
	//Enable Master
	LCD_WRITE_CMD(0x31);
	// Extension Command 2
	LCD_WRITE_CMD(0xD7);
	// Disable Auto Read
	LCD_WRITE_BYTE(0x9F);
	LCD_WRITE_CMD(0xE0);
	// Enable OTP Read
	LCD_WRITE_BYTE(0x00);
	delay_ms(10);
	LCD_WRITE_CMD(0xE3);
	// OTP Up-Load
	delay_ms(20);
	LCD_WRITE_CMD(0xE1);
	// OTP Control Out
	LCD_WRITE_CMD(0x30);
	// Extension Command 1
	LCD_WRITE_CMD(0x94);
	// Sleep Out
	LCD_WRITE_CMD(0xAE);
	// Display OFF
	delay_ms(50);
	LCD_WRITE_CMD(0x20);
	// Power Control
	LCD_WRITE_BYTE(0x0B);
	// VB, VR, VF All ON
	LCD_WRITE_CMD(0x81);
	// Set Vop = 16V
	LCD_WRITE_BYTE(0x36);
	LCD_WRITE_BYTE(0x04);
	LCD_WRITE_CMD(0x31);
	// Extension Command 2
	LCD_WRITE_CMD(0x20);
	// Set Gray Mode
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x17);
	//Light Gray Level Setting
	LCD_WRITE_BYTE(0x17);
	//Light Gray Level Setting
	LCD_WRITE_BYTE(0x17);
	//Light Gray Level Setting
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x1D);
	//Dark Gray Level Setting
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x00);			

	LCD_WRITE_BYTE(0x1D);
	//Dark Gray Level Setting
	LCD_WRITE_BYTE(0x1D);
	//Dark Gray Level Setting
	LCD_WRITE_BYTE(0x1D);
	//Dark Gray Level Setting
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_CMD(0x32);
	// Analog Circuit Set
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_BYTE(0x01);
	// Booster Efficiency =6KHz
	LCD_WRITE_BYTE(0x02);
	//Bias=1/12
	LCD_WRITE_CMD(0x51);
	// Booster Level x10
	LCD_WRITE_BYTE(0xFB);
	LCD_WRITE_CMD(0x30);
	// Extension Command 1
	LCD_WRITE_CMD(0XF0);
	// Display Mode
	LCD_WRITE_BYTE(0x11);
	// 4G Mode
	LCD_WRITE_CMD(0xCA);
	// Display Control
	LCD_WRITE_BYTE(0x00);
	// CL Dividing Ratio劤 Not Divide
	LCD_WRITE_BYTE(0XA1);
	//Duty Set 劤162 Duty
	LCD_WRITE_BYTE(0x00);
	//Frame Inversion
	LCD_WRITE_CMD(0xBC);
	// Data Scan Direction
	LCD_WRITE_BYTE(0x00);
	LCD_WRITE_CMD(0xA6);
	// Normal Display
	LCD_WRITE_CMD(0x31);
	// Extension Command 2
	LCD_WRITE_CMD(0x40);
	// Internal Power Supply
	LCD_WRITE_CMD(0x30);
	// Extension Command 1
	LCD_WRITE_CMD(0x77);
	// Enable ICON RAM
	LCD_WRITE_CMD(0x15);
	// Column Address Setting
	LCD_WRITE_BYTE(0x00);
	// SEG0 -> SEG255
	LCD_WRITE_BYTE( 0xFF);
	//Clear_ICON_DDRAM();
	//Clear Whole ICON DDRAM by ¨0〃
	LCD_WRITE_CMD(0x76);
	// Disable ICON RAM
	LCD_WRITE_CMD(0x75);
	// Page Address Setting
	LCD_WRITE_BYTE(0x00);
	// COM0 -> COM161
	LCD_WRITE_BYTE( 0x28);
	LCD_WRITE_CMD(0x30);
	// Extension Command 1
	LCD_WRITE_CMD(0x15);
	// Column Address Setting
	LCD_WRITE_BYTE(0x00);
	// SEG0 -> SEG255
	LCD_WRITE_BYTE(0xFF);
	LCD_WRITE_CMD(0x75);
	// Page Address Setting
	LCD_WRITE_BYTE( 0x00);
	// COM0 -> COM161
	LCD_WRITE_BYTE(0x28);
	//Disp_Image();
	// Fill the DDRAM Data by Panel Resolution
	LCD_WRITE_CMD(0xAF);
	// Display ON
}
#endif

//void SetGrayLevel(char gl,char gd)
void SetContrast(char lev)
{
	int val;
	val = ((EV_MAX- EV_MIN)/5)*lev+EV_MIN;
	LCD_WRITE_CMD(0x30);   	// Extension Command 2
	LCD_WRITE_CMD(0x81);   	// Extension Command 2
	
	LCD_WRITE_BYTE(val&0X3F);  	// VPR[5-0]	  ((EV>>1)&0XE0)>>5
	LCD_WRITE_BYTE(((val>>1)&0Xe0)>>5);   // VPR[8-6]  vop=(10-3.6)/0.04=160=1010 0000
}

void LcdPortInit(void)
{

	//RCC->APB1RSTR |=(1<<14);			//reset spi2
	//RCC->APB1RSTR &=~(1<<14);
	//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	//delay_ms(1);
	//LCD_RST = 0;
	//delay_ms(10);
	LCD_RST = 1;
	delay_ms(5);

	//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	

	RCC->APB1ENR|=1<<14;      //SPI2时钟使能
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能 	 
	RCC->APB2ENR|=1<<5;	//PORTD时钟使能
 		   
	//GPIOB->CRH&=0X0F0F0FFF; 
	//GPIOB->CRH|=0XB0B03000;//PB13.14.15复用 	    
	//GPIOB->ODR|=0XF<<12;   //PB13.14.15上拉

	GPIOB->CRH&=0X0F0FFFFF; 
	GPIOB->CRH|=0XB0B00000;//PB13.14.15复用 	    
	//GPIOB->CRH&=0XFFFFFFFF; 
	//GPIOB->CRH|=0X00000000;//PB13.14.15复用 
	//GPIOB->ODR|=0X0E<<12;   //PB13.14.15上拉
	GPIOB->ODR|=(1<<13);
	GPIOB->ODR|=(1<<15);

	GPIOD->CRH&=0XFFFFFF00; 
	GPIOD->CRH|=0X00000033;
	GPIOD->ODR|=0X3<<8;
		
	SPI2->CR1|=0<<10;//全双工模式	
	SPI2->CR1|=1<<9; //软件nss管理
	SPI2->CR1|=1<<8;  

	SPI2->CR1|=1<<2; //SPI主机
	SPI2->CR1|=0<<11;//8bit数据格式	
	SPI2->CR1|=0<<1; //空闲模式下SCK为1 CPOL=1
	SPI2->CR1|=0<<0; //数据采样从第二个时间边沿开始,CPHA=1  
	SPI2->CR1|=0<<3; //Fsck=Fcpu/2
	//SPI2->CR1|=1<<3; //Fsck=Fcpu/4
	//SPI2->CR1|=2<<3; //Fsck=Fcpu/8
	//SPI2->CR1|=3<<3; //Fsck=Fcpu/16
	//SPI2->CR1|=4<<3; //Fsck=Fcpu/32
	//SPI2->CR1|=5<<3; //Fsck=Fcpu/64
	//SPI2->CR1|=6<<3; //Fsck=Fcpu/128
	//SPI2->CR1|=7<<3; //Fsck=Fcpu/256
	
	
	
	SPI2->CR1|=0<<7; //MSBfirst   

	//SPI2->CR2|=1<<2;

#ifdef DRV_DMA_TEST
	
	SPI2->CR2|=(1<<1);
#endif

	
	SPI2->CR1|=1<<6; //SPI设备使能
//	LCD_Writ_Bus(0xFF);
	//SPIx_ReadWriteByte(0xff);//启动传输	

	
}

void LCD_Init(void)
{
#ifdef DRV_DMA_TEST
	SpiDMAInit(DMA1_Channel5);
#endif
	LcdPortInit();
	//LCD_Reset();
	ST75256_Init();	
	//SetGrayLevel(0x08,0x08);
	//printf("start2");
}

void DisplayOff(void)
{
	LCD_WRITE_CMD(DISP_OFF);	
}


void DisplayPicture(uchar page,uchar column,uchar wide,uchar tall,const char *p,uchar rsv)
{
	 uchar i=0,j=0;

	AreaSet(page, column, wide, tall);
	for(i=0;i<(tall+7)/8;i++)
	{
		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+column);
		//LCD_WRITE_BYTE(SEG_START+wide); 
		//LCD_WRITE_CMD(0x5c);
		for(j=0;j<wide;j++)
		{			
			if(rsv>0)
			{
				LCD_WRITE_BYTE(~(*p++));
			}else
			{
				LCD_WRITE_BYTE(*p++);
			}
		}
	}

}

void LcdCopyToRam(u16 page, u16 cml,char *gBuf,u16 wt,u16 ht) 
{
	//char i,j;

	u16 i=0;
	
	AreaSet(page,cml,wt,ht);

	for(i=0;i<wt*(ht/8);i++)
	{
		//LCD_WRITE_BYTE(g_paraSetBuf[i]);
		LCD_WRITE_BYTE(gBuf[i]);
	}

	//for(j=0;j<ht/8;j++)
	//{
	//	AreaSet(page++,cml,wt,8);
	//	for(i=0;i<wt;i++)
	//	{
			//LCD_WRITE_BYTE(g_paraSetBuf[i]);
	//		LCD_WRITE_BYTE(g_paraSetBuf[j*wt+i]);
			//LCD_WRITE_BYTE(g_paraSetBuf[wt+i]);
	//	}
	//}
}



//char (*pTest)[5] = {test_ttl,test_master,test_rpt,test_m,test_sttl};
void Lcd_test(void)
{
	u8 i=0;	
//	u8 index=0;
//	u8 abr=0;
//	u8 otp1=0,otp2=0;
//	signed char cmp1=-3,cmp2=0;
//	u8 fcLen=0;
//	char ch =0;
	//float a;
	//u8 b;
	//a=10.08;
	//b=(int)a;
	//a=a-b;
	//flaPara.ttl_FEB2 = 1;
	//flaPara.ttl_FEB1 = 0;
	//LCD_Clear(0);
	//DisplayCanParaChar(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,'4',DSP_NORMAL);
	//DisplayCanParaChar(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA+6,'3',DSP_NORMAL);
	//DisplayCanParaChar(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA+6+5,'2',DSP_NORMAL);
	//DisplayCanParaChar(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA+6+5+5,'1',DSP_NORMAL);
	//DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,0,0,DSP_NORMAL);
	//DisplayCanParaString(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,DSP_PARA_1LINE,(char*)outputString,DSP_NORMAL);
	//DisplayCanParaChar(LAYOUT_RATIO_A,0x51,'.',DSP_NORMAL);
	//AreaSet(LAYOUT_RATIO_A-1, LAYOUT_RATIO_STPARA, 1, 8);
	//LCD_WRITE_BYTE(0xFF);
	//DisplayMstOtp_canStyle(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,0,0,DSP_NORMAL);
	//DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,1,2,DSP_NORMAL);
	//DisplayMstOtp_canStyle(LAYOUT_RATIO_A+2,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,2,2,DSP_NORMAL);
	//DisplayMstOtp(LAYOUT_RATIO_A+4,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,3,2,DSP_NORMAL);
	//DisplayMstOtp_canStyle(LAYOUT_RATIO_A+4,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,7,0,DSP_NORMAL);
	//memset(g_paraSetBuf,0x00,208);
	//DisplayCanParaString(LAYOUT_RATIO_A+2,LAYOUT_RATIO_STPARA,DSP_PARA_1LINE,(char*)(&outputString[otpStrIndex[2]]),DSP_NORMAL);
	//LcdCopyToRam(LAYOUT_RATIO_A+2,LAYOUT_RATIO_STPARA, DSP_PARA_1LINE);
	//mainPara.workMode = WORKINGMODE_TTL;
	//SetCmpFeb(-1,2,2,1,DSP_NORMAL);
	//DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_CMP_WT+1,1,2,DSP_REVERSE);
	//DspCmpFeb
	//Lcd_ShowMenuChar(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_CMP_WT+1,'0',DSP_NORMAL);
	//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",DSP_REVERSE,0);

	//DisplayABRation(10,DSP_NORMAL);

	//DisplayMstOtp_canStyle(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,7,0,DSP_REVERSE);
	//DisplayCanOtpString(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,DSP_PARA_1LINE)
	//LoadButtons();
	//DspSensors(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML,1,1,1,1);
	//	SETTING_ADDSUB==g_SettingIndex?DSP_REVERSE:DSP_NORMAL);

	while(1)
	{

		//KeyProcess();
		DspIsoValue(LAYOUT_PARA_AREA2, 60,isoValue[i++],DSP_NORMAL);

		delay_ms(500);
		
		

		//delay_ms(5);

		//CHARGE_LED2 = !CHARGE_LED2;
		//CHARGE_LED1 = !CHARGE_LED2;
		//delay_ms(200);
#if 0
		DisplayABRation(abr,DSP_REVERSE);
		abr++;
		if(abr>12)
		{
			abr = 0;
		}
		delay_ms(1000);
#endif
		//SetTtlScr();

		//DspCmpFeb(LOCATION_TTLCMP_PAGE, LOCATION_TTLCMP_CML+FONT_CMP_WT+1,cmpArr[cmp1*3+cmp2],DSP_NORMAL);

		//SetCmpFeb(-1,2,2,1,DSP_NORMAL);
		//DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_CMP_WT+1,cmp1,cmp2,DSP_SELE);

		//CanSynMode(0,SYNC_MODE_FP,DSP_NORMAL);
		//delay_ms(500);
		//CanSynMode(0,SYNC_MODE_REAR,DSP_NORMAL);
		//delay_ms(500);
		//Lcd_ShowMenuChar(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_CMP_WT+1,'0',DSP_NORMAL);
		//DisplayFocLen(MOTO_MODE_MANU,fcLen,DSP_REVERSE);
		//delay_ms(300);
		//fcLen++;
		//if(fcLen>200)
		//fcLen=0;
#if 0
		SetMstChan(ch,DSP_NORMAL);
		delay_ms(1000);
		ch++;
		if(ch>15)
		{
			ch = 0;
		}
#endif
#if 0	
		flaPara.ttl_FEB2++;
		if(flaPara.ttl_FEB1>=3)
		{
			flaPara.ttl_FEB1 = 0;
			flaPara.ttl_FEB2 = 0;
		}
		if(flaPara.ttl_FEB2>>2)
		{
			flaPara.ttl_FEB1++;
			flaPara.ttl_FEB2-=3;
		}

		delay_ms(1000);
		delay_ms(1000);
#endif
#if 0
		cmp2++;
		if(cmp1>=3)
		{
			cmp1 = -3;
			cmp2 = 0;
		}
		if(cmp2>2)
		{
			cmp1++;
			cmp2-=3;
		}

		delay_ms(1000);
		//delay_ms(300);
#endif
#if 0
		SetManuPara(otp1,otp2,DSP_NORMAL);
		otp2++;
		if(otp1>=7)
		{
			otp1 =0;
			otp2 =0;
		}
		if(otp2>2)
		{
			otp1++;
			otp2-=3;
			//if(otp1>=7)
		}
		delay_ms(500);
		//delay_ms(1000);
		//delay_ms(1000);
#endif
		
#if 0
		DisplayPicture(5,74,24,24,picScan_1,0);
		delay_ms(100);
		DisplayPicture(5,74,24,24,picScan_2,0);
		delay_ms(100);
		DisplayPicture(5,74,24,24,picScan_3,0);
		delay_ms(400);
#endif

	}
	//LCD_WRITE_BYTE(0x00);
	//LCD_WRITE_BYTE(0x00);
	//LCD_WRITE_BYTE(0xFF);
	//LCD_WRITE_BYTE(0xFF);
	//LCD_WRITE_BYTE(0xFF);
	
}

#endif

#if 0
void LCD_DrawPoint(u16 x,u16 y, u8 rvs)
{
} 
#endif

#if 0
//清屏函数
//Color:要清屏的填充色
void LCD_Clear(u16 Color)
{
	u32 index=0;	        
	LCD_SetCursor(0x00,0x0000);//设置光标位置 
	LCD_WriteRAM_Prepare();     //开始写入GRAM	 	  
	for(index=0;index<76800;index++)
	{
		LCD_WR_DATA(Color);   	  
	}
}  
//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
#ifdef USE_HORIZONTAL
	xlen=yend-ysta+1;	   
	for(i=xsta;i<=xend;i++)
	{
	 	LCD_SetCursor(i,ysta);      //设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM	  
		for(j=0;j<xlen;j++)LCD_WR_DATA(color);//设置光标位置 	  
	}
#else
	xlen=xend-xsta+1;	   
	for(i=ysta;i<=yend;i++)
	{
	 	LCD_SetCursor(xsta,i);      //设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM	  
		for(j=0;j<xlen;j++)LCD_WR_DATA(color);//设置光标位置 	    
	}
#endif						  	    
}  
#endif

void LCD_Clear(u16 Color)
{
	char i,j;
	for(i=0;i<PAGE_MAX;i++)
	{
		LCD_WRITE_CMD(0x30);
		LCD_WRITE_CMD(0x75); //set page address
		LCD_WRITE_BYTE(i);
		LCD_WRITE_BYTE(i);
		LCD_WRITE_CMD(0x15); //set column address
		LCD_WRITE_BYTE(SEG_START);
		LCD_WRITE_BYTE(SEG_START+LCD_WIDTH-1); 
		LCD_WRITE_CMD(0x5c);
		for(j=0;j<+LCD_WIDTH;j++)
		{
			LCD_WRITE_BYTE(0x00);
		}
	}
}


void LcdClearFeild(u16 x, u16 y, u16 wt, u16 ht)
{
#if 0
	//LCD_Fill(y, x, y+wt, y+ht, BACK_COLOR);
	u16 i=0;//, y0=y;
	u8 j=0;
	//u8 k=0;
	//g_penColor = BACK_COLOR;
	g_penColor = bkSetArr[pSnrSet[SNRMENU_BKCLR]];
	//AreaSet((LCD_HEND-x-ht+1),y+1,(LCD_HEND-x),y+wt);
	AreaSet(x, y, x+ht-1, y+wt-1);
	SET_CD_DATA;
	SET_CS_ENABLE;
	//AddrSet(,y,(LCD_WEND-x),y+wt-1);
	for(i=0;i<wt;i++)
	{
		for(j=0;j<ht;j++)
		{
			//g_penColor = BACK_COLOR;
			//LCD_DrawPoint(x++,y+i);
			LCD_WR_DATA(g_penColor);
			//LCD_WR_DATA8(VH,VL);
		}
		//x = x0;
	}
	SET_CS_DISABLE;
#endif
	u8 i,j;
	AreaSet(x, y, wt, ht);
	for(i=0;i<ht/8;i++)
	{
		for(j=0;j<wt;j++)
		{
			LCD_WRITE_BYTE(0x00);
		}
	}
}

void LcdGrAllClear(void)
{
	LCD_CLEAR_GR(LAYOUT_RATIO_A);
	LCD_CLEAR_GR(LAYOUT_RATIO_B);
	LCD_CLEAR_GR(LAYOUT_RATIO_C);
}

void DspClearAllPara(void)
{
#if 0
	LcdClearFeild(0,0,LAYOUT_MODE_WT,LAYOUT_MODE_HT);		//clear mode dsp fieldy
	LcdClearFeild(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,MAIN_FONT_WIDTH(11),BIG_FONT_HIGHT(1));	//ttl cmp
	LcdClearFeild(LAYOUT_PARA_AREA2,LAYOUT_CMLPART(1),MAIN_FONT_WIDTH(10),MAIN_FONT_HIGHT(1)); //master arr and multi TIMES and FRQ
	LcdGrAllClear();											//clear All master groups parameter	
	LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-16,18+10,24);		//clear channal
	LcdClearFeild(8,LAYOUT_MODE_WT,BIG_FONT_WIDTH(5)+MAIN_FONT_WIDTH(7),BIG_FONT_HIGHT(1));	//Linked shot dsp
	LcdClearFeild(LAYOUT_PARA_AREA3, LAYOUT_CMLPART(1),MAIN_FONT_WIDTH(20),MAIN_FONT_HIGHT(1));	//ratio
	LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT);
	LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_CMLPART(1),FONT_CMP_WT,FONT_CMP_HT);				//ttl cmp
	RFGR_SELBAR_CLEAR;
#endif
	LcdClearFeild(0, 0, LCD_W, LCD_H-16);
	
	
}

#if 0

void FillArea(u16 x, u16 y, u16 wt, u16 ht, u8 rsv)
{
	u16 i=0,j=0;
	//g_penColor = rsv?BACK_COLOR:PEN_COLOR;
	//g_penColor = rsv?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
	for(j=0;j<wt;j++)
	{
		for(i=0;i<ht;i++)
		{
			LCD_DrawPoint(x+i,y+j,rsv);
		}		
	}
}


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol,DSP_NORMAL);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	g_penColor = PEN_COLOR;
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,DSP_NORMAL);             //3           
		LCD_DrawPoint(x0+b,y0-a,DSP_NORMAL);             //0           
		LCD_DrawPoint(x0-a,y0+b,DSP_NORMAL);             //1       
		LCD_DrawPoint(x0-b,y0-a,DSP_NORMAL);             //7           
		LCD_DrawPoint(x0-a,y0-b,DSP_NORMAL);             //2             
		LCD_DrawPoint(x0+b,y0+a,DSP_NORMAL);             //4               
		LCD_DrawPoint(x0+a,y0-b,DSP_NORMAL);             //5
		LCD_DrawPoint(x0+a,y0+b,DSP_NORMAL);             //6 
		LCD_DrawPoint(x0-b,y0+a,DSP_NORMAL);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b,DSP_NORMAL);
	}
} 
#endif


#if 0
void LCD_ShowString(u16 x,u16 y,const char *p, u8 font, u8 dotCpr,u8 rsvMod)
{         
#if 0
	unsigned char pageWid=0;

/*
	if(FONT_16x08==font)
	{
		pageWid = 8; 
	}else if(FONT_24x12==font)
	{
		pageWid = 12;
	}
*/

	switch(font)
	{
		case FONT_16x08:
		case FONT_16x08B:
			pageWid = 8; 
		break;

		case FONT_24x12:
			pageWid = 12; 
		break;

		case FONT_32x16:
			pageWid = 16; 
		break;

		case FONT_12x06:
			pageWid = 6;
		break;

		case FONT_20x10:
			pageWid = 10;
		break;

	
	}

	while(*p!='\0')
	{       
		//if(x>MAX_CHAR_POSX){x=0;y+=16;}
		//if(x>MAX_CHAR_POSX){x=0;y+=24;}
		// if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear(WHITE);}
		if(y+pageWid>319)
		{
			x+=pageWid*2;
			y = 0;
		}
		LCD_ShowChar(x,y,*p,font,rsvMod);
		//y+=8;
		if((*p=='.')&&(dotCpr))
		{
			y+=pageWid/2;
		}/*else if(*p==';')
		{
#ifdef _220x176_
			y+=(pageWid+1);
#else
			y+=pageWid;
#endif
		}*/else
		{
			y+=pageWid;
		}
		p++;
	}  
#endif
}
#endif

#if 0
void LcdPutBuf(u16 x,u16 y,const u8 *p, u8 font, u8 sz,u8 dotCpr,u8 rsvMod)
{
#if 0
	unsigned char pageWid=0;
	unsigned char i=0;
	
	switch(font)
	{
		case FONT_16x08:
			pageWid = 8; 
		break;

		case FONT_24x12:
			pageWid = 12; 
		break;

		case FONT_32x16:
			pageWid = 16; 
		break;

		case FONT_12x06:
			pageWid = 6; 
		break;
	
	}

	//while(*p!='\0')
	while(i++<sz)
	{       
		//if(x>MAX_CHAR_POSX){x=0;y+=16;}
		//if(x>MAX_CHAR_POSX){x=0;y+=24;}
		// if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear(WHITE);}
		if(y+pageWid>319)
		{
			x+=pageWid*2;
			y = 0;
		}
		LCD_ShowChar(x,y,*p,font,rsvMod);
		//y+=8;
		if((*p=='.')&&(dotCpr))
		{
			y+=pageWid/2;
		}else
		{
			y+=pageWid;
		}
		p++;
	}
#endif
}		
#endif

void LcdLoadPicture(u16 x, u16 y, u16 wide, u16 tall, const char* pBit,u8 rsv)
{
	DisplayPicture(x,y,wide,tall,pBit,rsv);
}


void DspTest(void)
{
#if 0
	static u16 x=0,y=0;

	if((x==0)&&(y==0))
	{
		LCD_Clear(BLACK);
	}

	LcdLoadPicture(x,y,32,32,head,DSP_NORMAL);

	y+=40;

	if(y>(319-32))
	{
		x += 40;
		y=0;
		if(x>(239-32))
		{
			x = 0;
			y = 0;			
		}
	}
#endif

}


#if 0
void LcdPicFramed(u16 x, u16 y, u16 wide, u16 tall, u8 rsv)
{
	u16 i;
	if(x>LCD_H||y>LCD_W)		return;
	//g_penColor = rsv? SEL_COLOR:PEN_COLOR;
	//g_penColor =(rsv&DSP_SELE)?SEL_COLOR:PEN_COLOR;
	//g_penColor = (rsv&DSP_SELE)?SEL_COLOR:penSetArr[pSnrSet[SNRMENU_PENCLR]];
	for(i=0;i<=wide;i++)
	{
		LCD_DrawPoint(x,y+i,rsv);
		LCD_DrawPoint(x+tall,y+i,rsv);
	}
	for(i=0;i<=tall;i++)
	{
		LCD_DrawPoint(x+i,y,rsv);
		LCD_DrawPoint(x+i,y+wide,rsv);
	}
}
#endif

#if 0
void LcdDspAtCenter(u16 x, u16 y, const char* p,u8 font, u8 rsv, u16 wt,u16 sz)
{
	u16 ys=0;
	if(sz>wt) return;
	ys = y+((wt -sz)>>1);
	LCD_ShowString(x,ys,p,font,1,rsv);//(u16 x, u16 y, const u8 * p, u8 font, u8 rsvMod)	
}
#endif

void *BtnAddSub(void)
{
	if(SETTING_ADDSUB!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_ADDSUB;
	}else
	{
		g_SettingIndex = 0;
	}
	//if((WORKINGMODE_TTL==mainPara.workMode)&&(WLM_MODE_ALL==*wlSet.ttl_Ratio))
	//{
		//g_ArrSel = 0;
		//g_SettingIndex = SETTING_ARR_PARA;
		//(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1)
	//}
//	Mode(mainPara.workMode);
	Mode();
/*
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			SetTtlScr();
			//SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);	
		break;

		case WORKINGMODE_MANU:
			//DisplayOutput(LAYOUT_RATIO_A,LOCATION_TTLCMP_CML,WORKINGMODE_MANU,mstPara->output1,mstPara->output2,DSP_SELE);
			SetManuScr();
		break;

		case WORKINGMODE_MULTI:
			//DisplayOutput(DSP_OUTPUT_PAGE,LAYOUT_CMLPART(2),WORKINGMODE_MULTI,(*multiSet.expose>>4),0,DSP_SELE);
			SetMultiScr();
		break;

		case WORKINGMODE_GR:
			//SetCmpFeb(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,g_SettingIndex);
			SetRfGrScr();
		break;
	}
*/
	return NULL;
}

void *BtnTtlFeb(void)
{
	if(SETTING_TTLFEB!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_TTLFEB;
	}else
	{
		g_SettingIndex = 0;
	}
	//SetTtlPara(mstPara->cmp1,mstPara->cmp2,flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);	
	//CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
	//SetTtlScr();
	/*
	if(WORKINGMODE_TTL==mainPara.workMode)
	{
		SetTtlScr();
	}else if(WORKINGMODE_GR==mainPara.workMode)
	{
		//CanSetFeb(flaPara.ttl_FEB1,flaPara.ttl_FEB2,DSP_SELE);
		SetRfGrScr();
	}*/
	//Mode(mainPara.workMode);
	Mode();
	return NULL;
}

void *BtnRatio()
{
	if(WORKINGMODE_TTL==mainPara.workMode)
	{
		(*wlSet.ttl_Ratio)++;
		if((*wlSet.ttl_Ratio)>WLM_MODE_RES)
		{
			(*wlSet.ttl_Ratio) = WLM_MODE_ALL;
		}
	}else
	{
		(*wlSet.arr_mode)++;
		if((*wlSet.arr_mode)>WLM_MODE_RES)
		{
			(*wlSet.arr_mode) = WLM_MODE_ALL;
		}
	}
	g_SettingIndex = 0;
	g_ArrSel = 0;			//2015/08/09
	//DisplayCanMstHead(1,struMode.mode,wlSet.arr_mode);
	//DisplayCanMstHead(0,mainPara.workMode,&wlSet);
	//DisplayCanMstHead(1,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
	Mode();
	//LoadButtons();
	BuildFuncMenu();
	KeyMenuSw();
	//ModPasteScr(mainPara.workMode);
	return NULL;

}

void *BtnMulti(void)
{
	if(SETTING_MULTI!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_MULTI;
	}else
	{
		g_SettingIndex = 0;
	}
	//MULTI_SET_ETIMES(*multiSet.eTimes, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
	return NULL;
}

void *BtnFrq(void)
{
	if(SETTING_FRQ!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_FRQ;
	}else
	{
		g_SettingIndex = 0;
	}
	//MULTI_SET_FRQ(*multiSet.frequence, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);
	return NULL;
}

void RfArrSelMenu(void)
{
	switch(g_ArrSel)
	{
		case 0:
			if(WORKINGMODE_TTL==mainPara.workMode)
			{
				//mArrResPara.dspStr = (WORKINGMODE_TTL==mainPara.workMode)?"A:B +-":((WLM_MODE_ALL==*wlSet->arr_mode)?"+-":"A +-");
				mArrResPara.dspStr = "A:B +-";
			}else
			{
				if(WLM_MODE_ALL==*wlSet.arr_mode)
				{
					mArrResPara.dspStr = "+-";
				}else
				{
					mArrResPara.dspStr = "A +-";
				}
			}
		break;

		case 1:
			mArrResPara.dspStr = (WORKINGMODE_TTL==mainPara.workMode)?"C +-":"B +-";
		break;

		case 2:
			//mArrResPara.dspStr = (WORKINGMODE_TTL==mainPara.workMode)?"C +-":"B +-";
			mArrResPara.dspStr = "C +-";
		break;
	}

	g_menuId = 0;
	funcMenu.listCnt = 0;
	AddButtons(&mNull,&mNull,&mArrResPara,&mBack,funcMenu.listCnt++);
	KeyMenuSw();

}


void RfgrSelMenu(void)
{
	if(radioSet->rfGr[g_grSel].isOff>0)
	{
		funcMenu.listCnt = 0;
		AddButtons(&mGrIsOff,&mNull,&mNull,&mBack,funcMenu.listCnt++);
	}else
	{
		switch(g_grSel)
		{
			case 0:
				mGrResMod.dspStr = "A MODE";
				mGrResPara.dspStr = "A +-";
			break;

			case 1:
				mGrResMod.dspStr = "B MODE";
				mGrResPara.dspStr = "B +-";
			break;

			case 2:
				mGrResMod.dspStr = "C MODE";
				mGrResPara.dspStr = "C +-";
			break;

			case 3:
				mGrResMod.dspStr = "D MODE";
				mGrResPara.dspStr = "D +-";
			break;

			case 4:
				mGrResMod.dspStr = "E MODE";
				mGrResPara.dspStr = "E +-";
			break;
		}
		funcMenu.listCnt = 0;
		AddButtons(&mGrIsOff,&mGrResMod,&mGrResPara,&mBack,funcMenu.listCnt++);
	}	
	KeyMenuSw();
}

void *BtnGrSel(void)
{
	if(WORKINGMODE_GR==mainPara.workMode)
	{
		if(g_SettingIndex>0)
		{
			g_SettingIndex = 0;
			Mode();
		}
		g_SettingIndex = SETTING_RFGR_SEL;
		if(g_grSel<1)
		{
			g_grDspStart = 0;
		}else if(g_grSel<4)
		{
			g_grDspStart = g_grSel-1;
		}else
		{
			g_grDspStart = 2;
		}

		//g_grDspStart = 0;
/*
		g_grSel++;
		if(g_grDspStart>g_grSel-1)
		{
			g_grDspStart = g_grSel-1;
		}
		if(g_grSel-g_grDspStart>3)
		{
			g_grDspStart++;
		}
		if(g_grSel>5)
		{
			g_grSel = 0;
			g_grDspStart=0;
			g_SettingIndex = 0;
			RFGR_SELBAR_CLEAR;
		}
*/
		//RfGrDspPara(g_grSel);	
		//SetRfGrScr();
		g_menuIdRsv = g_menuId;
		RfGrDspPara();
		RfgrSelMenu();

		RFGR_SELBAR_CLEAR;
		GrDrawSelBar(g_grSel);
		
/*
		if(g_grSel)
		{
			GrDrawSelBar(g_grSel-1);
		}
*/
		//GrDspPara(LAYOUT_RATIO_A+(g_grSel-1-g_grDspStart)*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_TTL,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,DSP_REVERSE);
	}else if(CMD_MODE_RF_SLA==*g_cmdMode)
	{
		//mainPara.s_array++;
		//if(mainPara.s_array>WL_ARRARY_E)
		//{
		//	mainPara.s_array = WL_ARRARY_A;
		//}
		(*wlSlave.s_array)++;
		if(*wlSlave.s_array>WL_ARRARY_E)
		{
			*wlSlave.s_array = WL_ARRARY_A;
		}
		//DspSlavePara(DSP_NORMAL);
		//DspRfSlavePara(DSP_NORMAL);
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		//UartSctSend(UART_ENCODE_SLAGR, mainPara.s_array, 0);
		UartSctSend(UART_ENCODE_SLAGR, *wlSlave.s_array,0);
		
	}else if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		//mainPara.s_array++;
		//if(mainPara.s_array>WL_ARRARY_C)
		//{
		//	mainPara.s_array = WL_ARRARY_A;
		//}
		(*wlSlave.s_array)++;
		if(*wlSlave.s_array>WL_ARRARY_C)
		{
			*wlSlave.s_array = WL_ARRARY_A;
		}
		//DspSlavePara(DSP_NORMAL);
		//UartSctSend(UART_ENCODE_SLAGR, mainPara.s_array, 0);
		//DspLightSlavePara(DSP_NORMAL);
		g_SettingIndex = 0;
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
	}else
	{
/*
		if(g_SettingIndex<WLM_SETTING_ARRA)
		{
			g_SettingIndex = WLM_SETTING_ARRA;
		}else if(g_SettingIndex>WLM_SETTING_ARRC)
		{
			g_SettingIndex = WLM_SETTING_ARRA;
		}else
		{
			g_SettingIndex++;
			if(g_SettingIndex>WLM_SETTING_ARRC)
			{
				g_SettingIndex = WLM_SETTING_ARRA;
			}		
		}
*/
		//g_SettingIndex++;
		if(g_SettingIndex>0)
		{
			g_SettingIndex = 0;
			Mode();
		}
		g_SettingIndex = WLM_SETTING_ARR;
		g_menuIdRsv = g_menuId;
/*
		if(g_SettingIndex<WLM_SETTING_ARRA)
		{
			g_SettingIndex = WLM_SETTING_ARRA;
		}
		if((g_SettingIndex-WLM_SETTING_ARRA)>(*wlSet.arr_mode))
		{
			g_SettingIndex = 0;
		}
*/
		ModPasteScr(mainPara.workMode);
		RfArrSelMenu();
		//Mode(mainPara.workMode);
		//DisplayCanMstHead(g_SettingIndex-2,mainPara.workMode,&wlSet);	
	}
	return NULL;
}

void *BtnMenu()
{
	g_menuId ++;
	if(g_menuId>=funcMenu.listCnt)
	{
		g_menuId = 0;
	}
	//KeyMenuSw();
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnZoon(void)
{
	//if(!REF_BOARD_DETECT)
	if(!g_isReflectPulled)
	{
		if(SETTING_MOTO!=g_SettingIndex)
		{
			g_SettingIndex = SETTING_MOTO;
		}else
		{
			g_SettingIndex = 0;
		}
	}else
	{
		g_SettingIndex = 0;
	}
	//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE);
//	Mode(mainPara.workMode);
	Mode();
	return NULL;
}

void *BtnMstSw(void)
{
	//g_SettingIndex = SETTING_MSTSW;
	if(WLM_MSTFLA_ON==*wlSet.is_masterOn)
	{
		*wlSet.is_masterOn = WLM_MSTFLA_OFF;
		LCD_MSTFLA_OFF;
		g_chargeflag = 0;
	}else
	{
		*wlSet.is_masterOn = WLM_MSTFLA_ON;
		LCD_MSTFLA_ON;
	}
	FuncDsp();
	TurnOnChargeLed(1);
	return NULL;
}

void DspRfIdChar(u16 x,u16 y,u8 num,u8 rvs)
{
	TitleFont *pFont;
	//char i=0,j=0;
	char i=0;

	AreaSet(x,y,11,16);
	pFont = (TitleFont*)&titleFont[TitleGetIndex((TitleFont*)titleFont, FONT_TITLE_LEN, num+'0')];
	for(i=0;i<(pFont->wt*2);i++)
	{
		if(DSP_NORMAL==rvs)
		{
			LCD_WRITE_BYTE(pFont->buf[i]);
		}else
		{
			LCD_WRITE_BYTE(~pFont->buf[i]);
		}
	}
}


void DspID(u8 sel,u8 rvs,unsigned int rfId)
{
	u8 i;
	//LcdLoadPicture(5,LAYOUT_ID_CML,3,LAY_HT,multi_ltBitmap,DSP_NORMAL);
	LcdLoadPicture(5,LAYOUT_ID_CML,19,LAY_HT,Id_Dbitmap,DSP_NORMAL);
	//DspRfIdChar(5,LAYOUT_IDNUM_CML,(radioSet->id)/1000,((sel==0)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	//DspRfIdChar(5,LAYOUT_IDNUM_CML+11,(radioSet->id)%1000/100,((sel==1)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	//DspRfIdChar(5,LAYOUT_IDNUM_CML+11+11,(radioSet->id)%100/10,((sel==2)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	//DspRfIdChar(5,LAYOUT_IDNUM_CML+11+11+11,(radioSet->id)%10,((sel==3)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	DspRfIdChar(5,LAYOUT_IDNUM_CML,rfId/1000,((sel==0)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	DspRfIdChar(5,LAYOUT_IDNUM_CML+11,rfId%1000/100,((sel==1)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	DspRfIdChar(5,LAYOUT_IDNUM_CML+11+11,rfId%100/10,((sel==2)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);
	DspRfIdChar(5,LAYOUT_IDNUM_CML+11+11+11,rfId%10,((sel==3)&&(DSP_SELE==rvs))?DSP_SELE:DSP_NORMAL);

	LcdClearFeild(7,LAYOUT_IDNUM_CML,11*4,8);

	AreaSet(7, LAYOUT_IDNUM_CML+sel*11+1, 9, 8);
	for(i=0;i<9;i++)
	{
		LCD_WRITE_BYTE(0x60);
	}
	//LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML, 'I', FONT_TYPE_BIG, DSP_NORMAL);
	//LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(1), 'D', FONT_TYPE_BIG, DSP_NORMAL);
	//LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(3), (radioSet->id)/1000+'0', FONT_TYPE_BIG, (sel==4)?DSP_SELE:DSP_NORMAL);
	//LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(4), (radioSet->id)%1000/100+'0', FONT_TYPE_BIG, (sel==3)?DSP_SELE:DSP_NORMAL);
	//LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(5), (radioSet->id)%100/10+'0', FONT_TYPE_BIG, (sel==2)?DSP_SELE:DSP_NORMAL);
	//LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(6), (radioSet->id)%10+'0', FONT_TYPE_BIG, (sel==1)?DSP_SELE:DSP_NORMAL);
	
#if 0

	for(i=0;i<8;i++)
	{
		LCD_ShowHexChar(LAYOUT_RATIO_B,60+MAIN_FONT_WIDTH(2*i),(g_snMac.mac[7-i]>>4),FONT_TYPE_MAIN,DSP_NORMAL);
		LCD_ShowHexChar(LAYOUT_RATIO_B,60+MAIN_FONT_WIDTH(2*i+1),(g_snMac.mac[7-i]&0x0f),FONT_TYPE_MAIN,DSP_NORMAL);
	}

	for(i=0;i<8;i++)
	{
		LCD_ShowHexChar(LAYOUT_RATIO_C,60+MAIN_FONT_WIDTH(2*i),g_snMac.sn[7-i]>>4,FONT_TYPE_MAIN,DSP_NORMAL);
		LCD_ShowHexChar(LAYOUT_RATIO_C,60+MAIN_FONT_WIDTH(2*i+1),g_snMac.sn[7-i]&0x0f,FONT_TYPE_MAIN,DSP_NORMAL);
		
	}
#endif	
	//if(sel)
	//LcdPicFramed( LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(2+sel),BIG_FONT_WIDTH(1),BIG_FONT_HIGHT(1),DSP_SELE);
	
}

void *BtnIdSel(void)
{
	//LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	DspClearAllPara();
	funcMenu.listCnt = 0;
	g_menuIdRsv = g_menuId;
	g_menuId=0;
	//AddButtons(&mNull,&mNull,&mIdSel,&mBack,funcMenu.listCnt++);
	AddButtons(&mNull,&mNull,&mNull,&mBack,funcMenu.listCnt++);
	KeyMenuSw();
	g_SettingIndex = SETTING_ID_SEL;
	g_rfIdSet = radioSet->id;
	g_idSetIndex = 0;
	DspID(g_idSetIndex,DSP_NORMAL,radioSet->id);
	return NULL;
}

void *BtnIdSet(void)
{
	g_SettingIndex = SETTING_ID_SET;
	//g_idSetIndex++;
	//if(g_idSetIndex>3)
	//{
	//	g_idSetIndex = 0;
	//}
	DspClearAllPara();
	DspID(g_idSetIndex,DSP_SELE,radioSet->id);
	return NULL;	
}

void *BtnChSet(void)
{	
	if(WLM_SETTING_CHAN!=g_SettingIndex)
	{
		g_SettingIndex = WLM_SETTING_CHAN;
	}else
	{
		g_SettingIndex = 0;
	}
	Mode();
	//ModPasteScr(mainPara.workMode);
	//Mode(mainPara.workMode);
	/*
	if(CMD_MODE_LIGH_MST==*g_cmdMode)
	{
		SetMstChan(*wlSet.m_chan+1,DSP_SELE);
	}else if(CMD_MODE_RF_MST==*g_cmdMode)
	{
		SetMstChan(radioSet->rfChan,DSP_SELE);
	}*/
	/*
	switch(*g_cmdMode)
	{
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,*wlSet.m_chan+1,DSP_SELE,0);
		break;

		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
		case CMD_MODE_RF_MST:
		case CMD_MODE_RF_SLA:
			SetMstChan(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML,radioSet->rfChan,DSP_SELE,0);
		break;
	}*/
	return NULL;
}

void *BtnReset(void)
{
	//memset((u8*)&mainPara,0,sizeof(mainPara));
	memset((u8*)&mainPara,0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));
	return NULL;
}

void *BtnRel(void)
{
	//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
	UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, *g_cmdMode);
	return NULL;
}

void *BtnMemory(void)
{
	//mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.sumVeri)-(char*)(&mainPara.workMode)+2)/2);
	if(g_SettingIndex>0)
	{
		g_SettingIndex = 0;
		Mode();
	}
	funcMenu.listCnt=0;
	g_menuIdRsv = g_menuId;
	g_menuId = 0;
	AddButtons(&mSave,&mLoad,&mNull,&mBack,funcMenu.listCnt++);
	KeyMenuSw();

	g_menuId = g_menuIdRsv;
	
	return NULL;
}	

void *BtnSmSel(void)
{
#if 0
	MenuItem *subItem;
	snrMenu.userSel++;
	CLR_SUBMENU_FILD;
	if(snrMenu.userSel>=MENU_ITEMS)
	{
		snrMenu.userSel=0;//userSel=0;
		snrMenu.dspStart=0;//dspStart=0;
		LcdClearFeild(SNR_FONT_HIGHT(2), 0, LCD_W, SNR_FONT_HIGHT(6));
	}
	if(snrMenu.dspStart+MAXDISPLAYLINE-1<snrMenu.userSel)
	{
		snrMenu.dspStart = snrMenu.userSel;
		LcdClearFeild(SNR_FONT_HIGHT(2), 0, LCD_W, SNR_FONT_HIGHT(6));
	}
	subItem = (MenuItem *)snrMenu.munuItem[snrMenu.userSel].childrenMenus;
	ShowSnrMenu(snrMenu.dspStart,0,snrMenu.userSel,MAINMENU_COLUMN,snrMenu.munuItem,MENU_ITEMS);
	LcdClearFeild(SNR_FONT_HIGHT(2),SNR_TITLE_WIDTH+2,8,SNR_FONT_HIGHT(MAXDISPLAYLINE));
	LCD_ShowChar(SNR_FONT_HIGHT(snrMenu.userSel-snrMenu.dspStart+2),SNR_TITLE_WIDTH+5,'!',SNR_FONT_TYPE,DSP_SELE);
	DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].displayString,DSP_SELE);
#endif

	return NULL;
}

void *BtnSmSub()
{
#if 0
	//struct MenuItem 	*pNow;
	snrMenu.subSel++;
	if(snrMenu.subSel>=snrMenu.munuItem[snrMenu.userSel].subCnt)
	{
		snrMenu.subSel = 0;
		snrMenu.subStart = 0;
	}
	if(snrMenu.subStart+MAXDISPLAYLINE-1<snrMenu.subSel)
	{
		snrMenu.subStart = snrMenu.subSel;
		//subStart = g_snrMenuSet[userSel];
		//CLR_SUBMENU_FILD;
		//DisplayClearFeild(2,SUBMENU_COLUMN,132-SUBMENU_COLUMN,41);
	}
	CLR_SUBMENU_FILD;
	//pNow = snrMenu.munuItem[snrMenu.userSel].childrenMenus;
	if(snrMenu.munuItem[snrMenu.userSel].subFuc!=NULL)
	{
		snrMenu.munuItem[snrMenu.userSel].subFuc();
		//CHARGE_LED =!CHARGE_LED;
	}
	ShowSnrMenu(snrMenu.subStart,snrMenu.userSel%MAXDISPLAYLINE,snrMenu.subSel,SUBMENU_COLUMN,(MenuItem *)(snrMenu.munuItem[snrMenu.userSel].childrenMenus),snrMenu.munuItem[snrMenu.userSel].subCnt);
#endif
	return NULL;
}

void *BtnHome(void)
{
	
#if 1
	switch(g_SettingIndex)
	{
		

		case SETTING_SNRMENU:
		case SETTING_PFNMENU:
			SavePara(FLASH_USER_ADDR);
			g_SettingIndex = 0;
			KeyOk();
		break;
		
		case WLM_SETTING_ARR:
		case SETTING_RFGR_SEL:
			g_SettingIndex = 0;
			g_menuId = g_menuIdRsv;
			KeyOk();
		break;
		
		case SETTING_ID_SEL:
		case SETTING_ID_SET:
		//default:
			g_rfIdSet = radioSet->id;
			SavePara(FLASH_USER_ADDR);
			g_SettingIndex = 0;
			g_menuId = g_menuIdRsv;
			KeyOk();
			//Mode(mainPara.workMode);
			//FuncDsp();
			//LoadButtons();
		break;

		default:
			KeyOk();
		break;

	}

	//g_SettingIndex = 0;
	//DspClearAllPara();
	//FuncDsp();
	//KeyOk();
#endif
	return NULL;
}

void *BtnSynSet(void)
{
	switch(mainPara.synMode)
	{
		case SYNC_MODE_FRON:
			mainPara.synMode = SYNC_MODE_FP;
		break;

		case SYNC_MODE_REAR:
			mainPara.synMode = SYNC_MODE_FRON;
		break;

		case SYNC_MODE_FP:
			mainPara.synMode = SYNC_MODE_REAR;
		break;
	}

	//mainPara.synMode++;
	//if(mainPara.synMode>SYNC_MODE_FP)
	//if(mainPara.synMode>SYNC_MODE_REAR)
	//{
		//mainPara.synMode = SYNC_MODE_FRON;
	//}
	//if((SYNC_MODE_REAR==mainPara.synMode)&&(CMD_MODE_OFF!=*g_cmdMode))
	if(SYNC_MODE_REAR==mainPara.synMode)
	{
		if((CMD_MODE_RF_MST==*g_cmdMode)||(CMD_MODE_LIGH_MST==*g_cmdMode))
		{
			mainPara.synMode=SYNC_MODE_FRON;
		}
	}
	//CanSynMode(mainPara.workMode,mainPara.synMode,DSP_NORMAL);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	
	return NULL;
}

char PfnCharGetIndex(char ch)
{
	//PfnFont *pfont=(PfnFont*)pfnFont;
	char i=0;
	for(i=0;i<FONT_CFN_LEN;i++)
	{
		if(ch == pfnFont[i].ch)
		{
			return i;
		}
	}

	return 0;
}

char LoadPfnChar(char ch,char *gBuf,char dspLen,unsigned char rvs)
{
	PfnFont *pFont;
	//char i=0,j=0;
	char i=0;

	pFont = (PfnFont*)&pfnFont[PfnCharGetIndex(ch)];
	//pFont = (canFontPara+CanCharGetIndex(canFontPara, FONT_PARA_LEN, ch));

		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+cml);
		//LCD_WRITE_BYTE(SEG_START+cml+pFont->wt); 
		//LCD_WRITE_CMD(0x5c);
	for(i=0;i<pFont->wt;i++)
	{
		if(DSP_NORMAL==rvs)
		{
			//LCD_WRITE_BYTE(pFont->buf[j+i*pFont->wt]); 
			//g_paraSetBuf[g_paraSetBufIndex] = pFont->buf[i];
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i] = pFont->buf[i];
			//gBuf[i+PARA_CMPFEB_WT] = pFont->buf[i+pFont->wt];
			gBuf[i+dspLen] = pFont->buf[i+pFont->wt];
			//g_paraSetBufIndex++;
		}else
		{

			//LCD_WRITE_BYTE(~(pFont->buf[j+pFont->wt])); 
			//g_paraSetBuf[g_paraSetBufIndex++] = ~(pFont->buf[j+pFont->wt]);
			//g_paraSetBuf[g_paraSetBufIndex] = (~(pFont->buf[i]))&0x3f;
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			//g_paraSetBufIndex++;
			gBuf[i] = (~(pFont->buf[i]));;
			//gBuf[i+PARA_CMPFEB_WT] = (~(pFont->buf[i+pFont->wt]))&0xFE;
			gBuf[i+dspLen] = ~(pFont->buf[i+pFont->wt]);

		}
	}
	return pFont->wt;
}



void LoadPfnString(char *str,char *gBuf,char dspLen,unsigned char rvs)
{
	char len=0;
	while(*str!='\0')
	{
		LoadPfnChar(*str,gBuf+len,dspLen,rvs);
		len+=pfnFont[PfnCharGetIndex(*str)].wt+1;
		str++;
	}
}

char PfnSubCharGetIndex(char ch)
{
	//PfnFont *pfont=(PfnFont*)pfnFont;
	char i=0;
	for(i=0;i<FONT_CFN_LEN;i++)
	{
		if(ch == pfnSubFont[i].ch)
		{
			return i;
		}
	}

	return 0;
}

void DspSnrSubStr(char* str,char *gBuf, char dspLen,char rvs)
{
	char i=0;
	char index=0;
	char strLen =0 ;
	while(*str!=0)
	{
		index = PfnSubCharGetIndex(*str);
		for(i=0;i<pfnSubFont[index].wt;i++)
		{
			if(DSP_NORMAL==rvs)
			{
				gBuf[i+strLen] = pfnSubFont[index].buf[i];
				gBuf[i+dspLen+strLen] = pfnSubFont[index].buf[i+pfnSubFont[index].wt];
			}else
			{
				gBuf[i+strLen] = ~pfnSubFont[index].buf[i];
				gBuf[i+dspLen+strLen] = ~pfnSubFont[index].buf[i+pfnSubFont[index].wt];
			}
		}
		strLen+=(pfnSubFont[index].wt+1);
		str++;
	}
}

//161//(LCD_W-4-7)

//void DspSnrMenu(u16 page,char index, MenuItem *pSnr,char sel,char sub,char rvs)
//void DspSnrMenu(u16 page,char index, MenuItem *pSnr,char sel,char sub,char bias,char rvs)
void DspSnrMenu(u16 page,char index, MenuItem *pSnr,signed char sel,char sub,char bias,char rvs)
{
	u8 i;
//	char tmp[PFN_DSP_LEN*3];
	char* tmp = g_paraSetBuf+1024;
	//LCD_Clear(0);
	if(DSP_NORMAL==rvs)
	{
		memset(g_paraSetBuf,0x00,PFN_DSP_LEN*5);
	}else
	{
		memset(g_paraSetBuf,0x7F,PFN_DSP_LEN);
		memset(g_paraSetBuf+PFN_DSP_LEN,0xFF,PFN_DSP_LEN*4);
	}

	g_paraSetBufIndex = 0;
	g_paraSetBufIndex+= SNR_START_CML;
	g_paraSetBufIndex+= pSnr[index].bias;

	//if(NULL==pSnr[index].dspFunc)
	//{
	memcpy(tmp,pSnr[index].pIcon,pSnr[index].picWt*(pSnr[index].picHt/8));
	
	//if(pSnr[index].dn>0)
	if(bias>0)
	{
		AreaScroll(tmp,pSnr[index].picWt, (pSnr[index].picHt/8),pSnr[index].picWt,SCROLL_DOWN,pSnr[index].dn);
	}
	
	if(pSnr[index].picHt>16)
	{
		//RamLoadPicture((char*)pSnr[index].pIcon,pSnr[index].picWt,pSnr[index].picHt,
		//					g_paraSetBuf+g_paraSetBufIndex,
		//					PFN_DSP_LEN,rvs);
		RamLoadPicture(tmp,pSnr[index].picWt,pSnr[index].picHt,
							g_paraSetBuf+g_paraSetBufIndex,
							PFN_DSP_LEN,rvs);
	}else
	{
		//RamLoadPicture((char*)pSnr[index].pIcon,pSnr[index].picWt,pSnr[index].picHt,
		//					g_paraSetBuf+PFN_DSP_LEN+g_paraSetBufIndex,
		//					PFN_DSP_LEN,rvs);
		RamLoadPicture(tmp,pSnr[index].picWt,pSnr[index].picHt,
							g_paraSetBuf+PFN_DSP_LEN+g_paraSetBufIndex,
							PFN_DSP_LEN,rvs);
	}
	//}else
	//{
		//pSnr[index].dspFunc(index,pSnrSet[index]);
		//pSnr[index].dspFunc(g_paraSetBuf,pSnrSet[index],rvs);
		//pSnr[index].dspFunc(g_paraSetBuf+PFN_DSP_LEN*3,sel,rvs);
		//LCD_Clear(0);
	//}

	//LCD_Clear(0);

	if(pSnr[index].pIcon!=NULL)
	{
		g_paraSetBufIndex = SNR_TCHAR_CML;	
	}else
	{
		g_paraSetBufIndex = SNR_START_CML;
	}
	

	if(NULL!=pSnr[index].displayString)
	{
		LoadPfnString((char*)pSnr[index].displayString,g_paraSetBuf+PFN_DSP_LEN+g_paraSetBufIndex,PFN_DSP_LEN,rvs);
	}

	if(sub>0)
	{

		g_paraSetBufIndex = SNR_SUB_CML;

		//switch(pSnrSet[index])
		//if(pSnr[index].subMenu->cnt>0)
		if(NULL==pSnr[index].dspFunc)
		{
			switch(sel)
			{
				case 0:
					DspSnrSubStr(pSnr[index].subMenu->pStr1,g_paraSetBuf+3*PFN_DSP_LEN+g_paraSetBufIndex,PFN_DSP_LEN,rvs);
				break;

				case 1:
					DspSnrSubStr(pSnr[index].subMenu->pStr2,g_paraSetBuf+3*PFN_DSP_LEN+g_paraSetBufIndex,PFN_DSP_LEN,rvs);
				break;

				case 2:
					DspSnrSubStr(pSnr[index].subMenu->pStr3,g_paraSetBuf+3*PFN_DSP_LEN+g_paraSetBufIndex,PFN_DSP_LEN,rvs);
				break;

				default:
				case 3:
					DspSnrSubStr(pSnr[index].subMenu->pStr4,g_paraSetBuf+3*PFN_DSP_LEN+g_paraSetBufIndex,PFN_DSP_LEN,rvs);
				break;
			}
		}else
		{
			//if(NULL!=pSnr[index].dspFunc)
			{
				pSnr[index].dspFunc(g_paraSetBuf+PFN_DSP_LEN*3,sel,rvs);
			}
		}		

		//if(pSnr[index].dn>0)
		//{
		//	AreaScroll(g_paraSetBuf,PFN_DSP_LEN, 3,pSnr[index].picWt,SCROLL_DOWN,pSnr[index].dn);
		//}

		if(DSP_REVERSE==rvs)
		{
			//memset(g_paraSetBuf,0x7F,PFN_DSP_LEN);
			for(i=0;i<PFN_DSP_LEN;i++)
			{
				g_paraSetBuf[i] &= 0x7F;
			}
		}else
		{
			for(i=0;i<PFN_DSP_LEN;i++)
			{
				g_paraSetBuf[i] |= 0x40;
				g_paraSetBuf[i+PFN_DSP_LEN*4] |= 0x01;
			}
			g_paraSetBuf[0] |= 0x7F;
			g_paraSetBuf[PFN_DSP_LEN-1] |= 0x7F;
			for(i=1;i<5;i++)
			{
				g_paraSetBuf[PFN_DSP_LEN*i] = 0xFF;
				g_paraSetBuf[PFN_DSP_LEN*(i+1)-1] = 0xFF;
				
			}
			//g_paraSetBuf[PFN_DSP_LEN] = g_paraSetBuf[2*PFN_DSP_LEN] = g_paraSetBuf[3*PFN_DSP_LEN] = g_paraSetBuf[4*PFN_DSP_LEN]=0xff;
			//g_paraSetBuf[5*PFN_DSP_LEN-1] = g_paraSetBuf[2*PFN_DSP_LEN-1] = g_paraSetBuf[3*PFN_DSP_LEN-1] = g_paraSetBuf[4*PFN_DSP_LEN-1]=0xff;
		}
	}
	
	LcdCopyToRam(page, 4, g_paraSetBuf,PFN_DSP_LEN, 40);

}

void DspSnrBar(char sel,char nDot ,char total)
{
	char i=0;
	char pa=0,ba=0;
	u32 tmp=0;
	memset(g_paraSetBuf,0x00,5*10);
	for(i=0;i<5;i++)
	{
		g_paraSetBuf[i] |= 0x40; 
		g_paraSetBuf[i+5*9] |= 0x01; 
	}

	for(i=0;i<10;i++)
	{
		g_paraSetBuf[i*5] |= 0xFF;
		g_paraSetBuf[i*5+4] |= 0xFF;
	}

	pa = (80/total)*sel/8;
	//pa = (nDot*sel)/8;
	ba = (80/total)*sel%8+2+(total>7?sel/4:0);
	//ba = (nDot*sel)%8;

	//for(i=0;i<5;i++)
	//{
	//	g_paraSetBuf[i+5*4] |= 0xff;
	//}
	//pa /=8;
	//ba = 80%total;
#if 0
	for(i=0;i<5;i++)
	{
		g_paraSetBuf[5*pa+i] |= 0xFF;
	}
#else

	tmp = (0xffffffff>>ba)&(0xffffffff<<(32-ba-nDot));
	
	for(i=0;i<5;i++)
	{
		g_paraSetBuf[i+5*pa] |= (tmp>>24)&0xFF;
		g_paraSetBuf[i+5*(pa+1)] |= (tmp>>16)&0xFF;
		g_paraSetBuf[i+5*(pa+2)] |= (tmp>>8)&0xFF;
		g_paraSetBuf[i+5*(pa+3)] |= tmp&0xFF;
	}
#endif
	
/*
	if((nDot+ba)>8)
	{
		for(i=0;i<5;i++)
		{
			g_paraSetBuf[i+5*pa]|=(0xff>>ba);
			g_paraSetBuf[i+5*(pa+1)]|=(0xff<<(nDot-8+ba));
		}
	}else
	{
		for(i=0;i<5;i++)
		{
			g_paraSetBuf[i+5*pa] |= (((1<<nDot)-1)<<(7-ba));
		}
	}
*/
	
	for(i=0;i<5;i++)
	{
		g_paraSetBuf[i] &= 0x7f;
	}
	//g_paraSetBuf[0] &= 0x7f;
	//g_paraSetBuf[4] &= 0x7f;

	LcdCopyToRam(1, 164, g_paraSetBuf, 5, 80);

	//tmp = (1<<nDot)-1;
	

	

	
	
}

void DspSnrTitle(char *str,char num)
{
	memset(g_paraSetBuf,0x00,40);
	g_paraSetBufIndex = 0;
	memcpy(g_paraSetBuf,str,21);
	g_paraSetBufIndex+=25;
	memcpy(g_paraSetBuf+g_paraSetBufIndex,cfnNum1+(num/10)*6,6);
	g_paraSetBufIndex+=7;
	memcpy(g_paraSetBuf+g_paraSetBufIndex,cfnNum1+(num%10)*6,6);
	LcdCopyToRam(0, 130, g_paraSetBuf, 40, 8);
	
}

void DspCfnMenu(void)
{
	//printf("DspCfnMenu, %d, %d\r\n",g_cfnIndex,g_cfnStart);
	u8 cn;
	if(g_cfnIndex<14)
	{
		cn = g_cfnIndex;
	}else
	{
		cn = 20+(g_cfnIndex-14);
	}

	DspSnrTitle((char*)f_cfn,cn);
	
	if(g_cfnIndex==g_cfnStart)
	{
		//DspSnrMenu(1,g_cfnIndex,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex],1,DSP_SELE);
		//DspSnrMenu(1+5,g_cfnIndex+1,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex+1],1,DSP_NORMAL);
		DspSnrMenu(1,g_cfnIndex,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex],1,sm_Cfn[g_cfnIndex].dn>0,DSP_SELE);
		DspSnrMenu(1+5,g_cfnIndex+1,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex+1],1,sm_Cfn[g_cfnIndex+1].dn>0,DSP_NORMAL);
	}else
	{
		//DspSnrMenu(1,g_cfnIndex-1,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex-1],1,DSP_NORMAL);
		//DspSnrMenu(1+5,g_cfnIndex,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex],1,DSP_SELE);
		DspSnrMenu(1,g_cfnIndex-1,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex-1],1,sm_Cfn[g_cfnIndex-1].dn>0,DSP_NORMAL);
		DspSnrMenu(1+5,g_cfnIndex,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex],1,sm_Cfn[g_cfnIndex].dn>0,DSP_SELE);
	}

	DspSnrBar(g_cfnIndex,5,18);

	funcMenu.listCnt=0;
	g_menuId = 0;
	AddButtons(&mPfn,&mClear,&mNull,&mBack,funcMenu.listCnt++);
	KeyMenuSw();
}

void DspPfnMenu(void)
{
	DspSnrTitle((char*)f_pfn,g_pfnIndex+1);
	
	if(g_pfnIndex==g_pfnStart)
	{
		//DspSnrMenu(1,g_pfnIndex,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex],1,DSP_SELE);
		//DspSnrMenu(1+5,g_pfnIndex+1,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex+1],1,DSP_NORMAL);
		DspSnrMenu(1,g_pfnIndex,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex],1,0,DSP_SELE);
		DspSnrMenu(1+5,g_pfnIndex+1,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex+1],1,0,DSP_NORMAL);
	}else
	{
		//DspSnrMenu(1,g_pfnIndex-1,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex-1],1,DSP_NORMAL);
		//DspSnrMenu(1+5,g_pfnIndex,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex],1,DSP_SELE);
		DspSnrMenu(1,g_pfnIndex-1,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex-1],1,0,DSP_NORMAL);
		DspSnrMenu(1+5,g_pfnIndex,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex],1,0,DSP_SELE);
	}

	DspSnrBar(g_pfnIndex,11,7);

	funcMenu.listCnt=0;
	g_menuId = 0;
	AddButtons(&cfnEx,&mClear,&mNull,&mBack,funcMenu.listCnt++);
	KeyMenuSw();
	
}

#define DSP_SNRSUB_LEN 159

void DspSubMenu(MenuItem *pSnr,char index,char sel)
{
	char *pStr;
	char cn=0,i=0;

	//if(pSnr[index].subMenu->cnt>0)
	if(NULL==pSnr[index].subMenu->dspSub)
	{
		for(cn=0;cn<pSnr[index].subMenu->cnt;cn++)
		{
			memset(g_paraSetBuf,cn==sel?0xFF:0x00,DSP_SNRSUB_LEN*2);
			g_paraSetBufIndex = 14;
			switch(cn)
			{

				case 0:
					pStr = pSnr[index].subMenu->pStr1;
				break;

				case 1:
					pStr = pSnr[index].subMenu->pStr2;
				break;

				case 2:
					pStr = pSnr[index].subMenu->pStr3;
				break;

				case 3:
					pStr = pSnr[index].subMenu->pStr4;
				break;

				default:
					pStr = pSnr[index].subMenu->pStr1;
				break;
				
			}
			//pStr = sm_Cfn[g_cfnIndex].subMenu->pStr#(cn+1+'0');
			DspSnrSubStr(pStr,g_paraSetBuf+g_paraSetBufIndex,DSP_SNRSUB_LEN,cn==sel?DSP_REVERSE:DSP_NORMAL);
			
			g_paraSetBuf[0] = g_paraSetBuf[DSP_SNRSUB_LEN-1] = 0xff;
			g_paraSetBuf[DSP_SNRSUB_LEN] = g_paraSetBuf[2*DSP_SNRSUB_LEN-1] = 0xff;
			
			if(0==cn)
			{
				for(i=0;i<DSP_SNRSUB_LEN;i++)
				{
					g_paraSetBuf[i] |= 0x80;
				}
			}else if((pSnr[index].subMenu->cnt-1)==cn)
			{
				for(i=0;i<DSP_SNRSUB_LEN;i++)
				{
					g_paraSetBuf[i+DSP_SNRSUB_LEN] |= 0x01;
				}
			}
			LcdCopyToRam(3+2*cn, 6,  g_paraSetBuf, DSP_SNRSUB_LEN, 16);
			
		}
	}else
	{
		//if(NULL!=pSnr[index].subMenu->dspSub)
		{
			pSnr[index].subMenu->dspSub(g_paraSetBuf,sel);
		}
	}
	

}

void EnterCfnSubMenu(char sel)
{
	char cn=0;
	//char *pStr;

	LCD_Clear(0);
	
	DspSnrMenu(0,g_cfnIndex,(MenuItem*)sm_Cfn,pSnrSet[g_cfnIndex],0,0,DSP_NORMAL);
	memset(g_paraSetBuf,0x00,100);
	g_paraSetBufIndex = 0;

	LoadPfnString("C.Fn ",g_paraSetBuf,39,DSP_NORMAL);
	
	g_paraSetBufIndex+=26;

	if(g_cfnIndex<14)
	{
		cn = g_cfnIndex;
	}else
	{
		cn = 20+(g_cfnIndex-14);
	}

	LoadPfnChar(cn/10+'0',g_paraSetBuf+g_paraSetBufIndex, 39, DSP_NORMAL);
	g_paraSetBufIndex+=7;
	LoadPfnChar(cn%10+'0',g_paraSetBuf+g_paraSetBufIndex, 39, DSP_NORMAL);

	LcdCopyToRam(1, 127,  g_paraSetBuf, 39, 16);

	//DspSubMenu(sel);
	DspSubMenu((MenuItem*)sm_Cfn,g_cfnIndex,sel);

	funcMenu.listCnt=0;
	AddButtons(&mNull,&mNull,&mNull,&mBack,funcMenu.listCnt++);
	KeyMenuSw();
	
	//g_paraSetBufIndex+=26;
	
}


void EnterPfnSubMenu(char sel)
{
	char cn=0;
	//char *pStr;

	LCD_Clear(0);

	DspSnrMenu(0,g_pfnIndex,(MenuItem*)sm_Pfn,pfnSet[g_pfnIndex],0,0,DSP_NORMAL);
	memset(g_paraSetBuf,0x00,100);
	g_paraSetBufIndex = 0;

	LoadPfnString("P.Fn ",g_paraSetBuf,39,DSP_NORMAL);

	g_paraSetBufIndex+=26;

	cn = g_pfnIndex+1;

	LoadPfnChar(cn/10+'0',g_paraSetBuf+g_paraSetBufIndex, 39, DSP_NORMAL);
	g_paraSetBufIndex+=7;
	LoadPfnChar(cn%10+'0',g_paraSetBuf+g_paraSetBufIndex, 39, DSP_NORMAL);

	LcdCopyToRam(1, 127,  g_paraSetBuf, 39, 16);

	DspSubMenu((MenuItem*)sm_Pfn,g_pfnIndex,sel);

	funcMenu.listCnt=0;
	AddButtons(&mNull,&mNull,&mNull,&mBack,funcMenu.listCnt++);
	KeyMenuSw();


}

void *BtnSnrMenu(void)
{
	if(g_cmrLinked>0)	return NULL;

	g_SettingIndex = SETTING_SNRMENU;

	//CHARGE_LED_OFF;

	LCD_Clear(0);
	
	DspCfnMenu();

	//CHARGE_LED_ON;
	
#if 0
//	unsigned char subSel=0;				//submenu 's select
	MenuItem *subItem;
//	subSel = g_snrMenuSet[0];			//avoid the select error
	g_SettingIndex = SETTING_SNRMENU;
	snrMenu.subSel = 0;
	snrMenu.userSel = 0;
	snrMenu.dspStart = 0;
	LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	LCD_ShowString(0,LAYOUT_ETTL_X,"C.Fn Menu",SNR_FONT_TYPE, 0, DSP_NORMAL);
	LCD_ShowChar(SNR_FONT_HIGHT(snrMenu.userSel-snrMenu.dspStart+2),SNR_TITLE_WIDTH+5,'!',SNR_FONT_TYPE,DSP_SELE);
	subItem = (MenuItem *)snrMenu.munuItem[snrMenu.userSel].childrenMenus;
	DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].displayString,DSP_SELE);
	ShowSnrMenu(0,0,0,0,snrMenu.munuItem,MENU_ITEMS);
	funcMenu.listCnt=0;
	AddButtons(&mNull,&mSmSel,&mSmSubSel,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();
#endif
	return NULL;
}

void *BtnPfnMenu(void)
{
	g_SettingIndex = SETTING_PFNMENU;

	LCD_Clear(0);
	DspPfnMenu();

	return NULL;
}

void *BtnGrOnOff(void)
{
	
	radioSet->rfGr[g_grSel].isOff = !radioSet->rfGr[g_grSel].isOff;
	
	RfGrDspPara();
	RfgrSelMenu();

	TurnOnChargeLed(0);
	
	return NULL;
}

void *BtnGrParaRes(void)
{
	if(SETTING_GR_RES==g_SettingIndex)
	{
		g_SettingIndex = SETTING_RFGR_SEL;
		
		RfGrDspPara();
		RfgrSelMenu();
		
		GrDrawSelBar(g_grSel);
		
	}else
	{
		if(!radioSet->rfGr[g_grSel].isOff)
		{
			g_SettingIndex = SETTING_GR_RES;
			RfGrDspPara();
			funcMenu.listCnt = 0;
			AddButtons(&mNull,&mNull,&mGrResPara,&mBack,funcMenu.listCnt++);
			KeyMenuSw();
		}
	}
	return NULL;
}

void *BtnArrParaRes(void)
{
	if(SETTING_ARR_PARA==g_SettingIndex)
	{
		g_SettingIndex = WLM_SETTING_ARR;
		ModPasteScr(mainPara.workMode);
		RfArrSelMenu();
	}else
	{
		g_SettingIndex = SETTING_ARR_PARA;
		DisplayCanMstHead(g_ArrSel,mainPara.workMode,&wlSet,(WLM_SETTING_ARR==g_SettingIndex)|((SETTING_ARR_PARA==g_SettingIndex)<<1));
	}
	return NULL;
}

void *BtnExtmIso(void)
{
	if(SETTING_EXTM_ISO!=g_SettingIndex)
	{
		g_SettingIndex = SETTING_EXTM_ISO;
	}else
	{
		g_SettingIndex = 0;
	}
	//DspIsoValue(mainPara.extIso,DSP_REVERSE);
	//SetExtmScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);

	return NULL;
}

void *BtnExtmApert(void)
{
	if(SETTING_EXTM_APERT!=g_SettingIndex)
	{
		g_SettingIndex  = SETTING_EXTM_APERT;
	}else
	{
		g_SettingIndex = 0;
	}
	//DspApertValue(mainPara.extApert,DSP_REVERSE);
	//SetExtmScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODENOCLR);

	return NULL;
	
}

void *BtnGrModRes(void)
{
	//g_SettingIndex = SETTING_GR_RES;
	radioSet->rfGr[g_grSel].mode++;
	if(radioSet->rfGr[g_grSel].mode > GRMOD_EXTA)
	{
		radioSet->rfGr[g_grSel].mode = GRMOD_TTL;
	}
	RfGrDspPara();
	return NULL;
}


void *BtnSave(void)
{
	//mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
	//SavePara();
	SavePara(FLASH_PARA_SAVE_ADDR);
	//g_menuId=3;
	g_menuId = g_menuIdRsv;
	KeyOk();
	return NULL;
}

void *BtnLoad(void)
{
	//UserInit(0);
	g_menuId =0;
	UserInit(FLASH_PARA_SAVE_ADDR,0);
	UserScreenOn();
	//KeyOk();
	return NULL;
}

#ifdef FIBER_ADJUST_PROC
void *BtnFbAdj(void)
{
	g_SettingIndex = SETTING_FB_ADJ;
	FiberAdjustProc();
	return NULL;
}

void *BtnFbAddSub(void)
{
	g_SettingIndex = SETTING_FB_EXP;
	FiberAdjustProc();
	return NULL;
}

void *BtnRgbCheck(void)
{
/*
	if(g_RgbCheckTurn>0)
	{
		g_RgbCheckTurn = 0;
	}else
	{
		g_RgbCheckTurn = 1;
	}
*/
	g_RgbCheckTurn++;
	if(g_RgbCheckTurn>3)
	{
		g_RgbCheckTurn = 0;
	}
	FiberAdjustProc();

	return NULL;
}

#endif

void *BtnCfnClear(void)
{
	if(SETTING_SNRMENU==g_SettingIndex)
	{
		memset(pSnrSet,0x00,18);
		LCD_Clear(0);	
		DspCfnMenu();
	}else if(SETTING_PFNMENU==g_SettingIndex)
	{
		LCD_Clear(0);
		memset(pfnSet,0x00,7);
		pfnSet[PFN_ID_CTS]=2;
		DspPfnMenu();
	}
	return NULL;
}

void *BtnCfnClearCancel(void)
{
	if(SETTING_SNRMENU==g_SettingIndex)
	{
		LCD_Clear(0);	
		DspCfnMenu();
	}else if(SETTING_PFNMENU==g_SettingIndex)
	{
		LCD_Clear(0);
		DspPfnMenu();
	}
	return NULL;
}


void *BtnSlaveModel(void)
{
	UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_MODEL, *g_cmdMode);
	return NULL;
}

void *BtnSlaveTest(void)
{
	UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_TEST, *g_cmdMode);
	return NULL;
}

void DspScanResult(void)
{
	u8 i=0,j=0,k=0;
	u8 rssi;
	LCD_Clear(0);
	DisplayPicture(1,6,28,16,fontGood,0);
	//memset(g_paraSetBuf,0x00,9*6);
	for(i=0;i<16;i++)
	{
		memset(g_paraSetBuf,0x00,9*6);
		if(0==i)
		{
			memcpy(g_paraSetBuf,picArrow,9);
			for(j=1;j<5;j++)
			{
				g_paraSetBuf[9*j+3]=0xFF;
			} 
			g_paraSetBuf[9*5+3]=0xFC;
			for(k=4;k<9;k++)
			{
				g_paraSetBuf[9*5+k]=0x04;
			}
			
		}else
		{
			rssi=radioStatue.rssi[i-1]/25;
			for(j=0;j<5;j++)
			{
				//rssi=radioStatue.rssi[i-1]/25;
				if(rssi>(5-j)*2)
				{
					memset(g_paraSetBuf+j*9,0x77,5);
				}else if(rssi>(5-j)*2-1)
				{
					memset(g_paraSetBuf+j*9,0x07,5);
				}
			}
			memset(g_paraSetBuf+5*9,rssi>0?0x74:0x04,5);
			memset(g_paraSetBuf+5*9+5,0x04,4);
/*
			if(rssi>0)
			{
				memset(g_paraSetBuf+5*9,0x74,9);
			}else
			{
				memset(g_paraSetBuf+5*9,0x74,9);
			}
*/
			
		}
		//AreaSet(3, 13+9*i, 9, 6*8);
		LcdCopyToRam(3, 13+9*i,  g_paraSetBuf, 9, 6*8);		
		
	}

	memset(g_paraSetBuf,0x00,148);
	g_paraSetBufIndex=1;
	for(i=1;i<=15;i++)
	{
		if((1==i)||(5==i)||(10==i)||(15==i))
		{
			g_paraSetBufIndex--;
			memcpy(g_paraSetBuf+g_paraSetBufIndex,scanRstScale+(i/5)*10,10);
			g_paraSetBufIndex+=10;
		}else
		{
			g_paraSetBuf[g_paraSetBufIndex+3] =0x07;
			g_paraSetBuf[g_paraSetBufIndex+4] =0x07;
			g_paraSetBuf[g_paraSetBufIndex+5] =0x07;
			g_paraSetBufIndex+=9;
		}
	}
	g_paraSetBufIndex+=3;
	memcpy(g_paraSetBuf+g_paraSetBufIndex,scanRstScale+40,10);

	LcdCopyToRam(9, 19,  g_paraSetBuf, 148, 8);	

	g_chSan = radioSet->rfChan;
	
	//AreaSet(10, 22+9*(g_chSan>0?g_chSan-1:0), 5, 8);
/*
	AreaSet(10, 22+9*(g_chSan-1), 5, 8);
	for(i=0;i<5;i++)
	{
		LCD_WRITE_BYTE(0X78);
	}
*/
	memset(g_paraSetBuf,0x00,135);
	memset(g_paraSetBuf+9*(g_chSan-1),0x78,5);
	LcdCopyToRam(10,22,g_paraSetBuf,135,8);
	//g_chSan = radioSet->rfChan;
	SetMstChan(1, 128,g_chSan,DSP_NORMAL,1);


}


void DrawFrame(void)
{
	u8 i=0;
//*************************************************
//	frame
//*************************************************//

	AreaSet(1,15,142,8);
	for(i=0;i<142;i++)
	{
		LCD_WRITE_BYTE(0X04);
	}

	//AreaSet(9,15,142,8);
	AreaSet(10,15,142,8);
	for(i=0;i<142;i++)
	{
		LCD_WRITE_BYTE(0X20);
	}

	AreaSet(1,15,1,80);
	for(i=0;i<10;i++)
	{
		if(0==i)
		{
			LCD_WRITE_BYTE(0X07);
		}else if(9==i)
		{
			LCD_WRITE_BYTE(0XE0);
		}else
		{
			LCD_WRITE_BYTE(0XFF);
		}
	}

	AreaSet(1,156,1,80);
	for(i=0;i<10;i++)
	{
		if(0==i)
		{
			LCD_WRITE_BYTE(0X07);
		}else if(9==i)
		{
			LCD_WRITE_BYTE(0XE0);
		}else
		{
			LCD_WRITE_BYTE(0XFF);
		}
	}
}

//13,15,142,65,
void *BtnChScan(void)
{
//	u8 fillPr=0;
//	u8 i=0;
	u16 scanCount=0;

	g_SettingIndex = SETTING_RFSCAN;

	UartSctSend(UART_ENCODE_CHSCAN,0x01,0);

	LCD_Clear(0);

//*************************************************
//	frame
//*************************************************//
	DrawFrame();
#if 0
	AreaSet(1,15,142,8);
	for(i=0;i<142;i++)
	{
		LCD_WRITE_BYTE(0X04);
	}

	//AreaSet(9,15,142,8);
	AreaSet(10,15,142,8);
	for(i=0;i<142;i++)
	{
		LCD_WRITE_BYTE(0X20);
	}

	AreaSet(1,15,1,80);
	for(i=0;i<10;i++)
	{
		if(0==i)
		{
			LCD_WRITE_BYTE(0X07);
		}else if(9==i)
		{
			LCD_WRITE_BYTE(0XE0);
		}else
		{
			LCD_WRITE_BYTE(0XFF);
		}
	}

	AreaSet(1,156,1,80);
	for(i=0;i<10;i++)
	{
		if(0==i)
		{
			LCD_WRITE_BYTE(0X07);
		}else if(9==i)
		{
			LCD_WRITE_BYTE(0XE0);
		}else
		{
			LCD_WRITE_BYTE(0XFF);
		}
	}
#endif

	DisplayPicture(3,55,64,16,fontChSan,0);	
	

	g_isChScan = 1;
	//while(scanCount++<30)
	while(g_isChScan)
	{
		//scanCount++;
		DisplayPicture(6,74,24,24,picScan_1,0);
		//delay_ms(300);
		HaltSec(300);
		DisplayPicture(6,74,24,24,picScan_2,0);
		//delay_ms(300);
		HaltSec(300);
		DisplayPicture(6,74,24,24,picScan_3,0);
		//delay_ms(400);
		HaltSec(400);

		if(scanCount++>24)
		{
			DISABLE_RF_MODULE();	
			break;
		}
		//scanCount++;
	}

	//Mode(mainPara.workMode);

	//KeyOk();
	//if(KEY_LOCK_EX)	
	//{
	//	LoadButtons();
	//}
	
#ifdef FOR_TEST	
	for(i=1;i<16;i++)
	{
		radioStatue.rssi[i] = i*25;
	}
#endif

	//radioStatue.rssi[14] = 250;

	if(scanCount>24)
	{
		g_SettingIndex = 0;
		BtnHome();
		ENABLE_RF_MODULE();
	}else
	{
		if(radioSet->rfChan>0)
		{
			g_SettingIndex = SETTING_SCAN_SET;
			DspScanResult();
			funcMenu.listCnt = 0;
			g_menuId = 0;
			AddButtons(&mNull,&mNull,&mNull,&mBack,funcMenu.listCnt++);
			KeyMenuSw();
		}else
		{
			g_SettingIndex = 0;
			KeyOk();
		}
	}

	

	//FuncDsp();
	

//	BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	
	return NULL;
	
}


char RePaint(void)
{
#if 0
	//LCD_Clear(bkSetArr[pSnrSet[SNRMENU_BKCLR]]);	
	u8 i;
	LcdClearFeild(0, 0, LCD_W, LCD_H);
	LCD_ShowString(0,LAYOUT_ETTL_X,"C.Fn Menu",SNR_FONT_TYPE, 0, DSP_NORMAL);
	for(i=0;i<4;i++)
	{
		LcdFillBlank(
			LAYOUT_MENU_PAGE,
			i*(LAYOUT_BUTTON_WIDTH+LAYOUT_BUTTON_INTERVAL),
			LAYOUT_BUTTON_WIDTH,
			LAYOUT_BUTTON_HIGHT,
			DSP_NORMAL);			//menu fill just once
	}
	snrMenu.userSel--;
	BtnSmSel();
	funcMenu.listCnt=0;
	AddButtons(&mNull,&mSmSel,&mSmSubSel,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();
#endif
	return 1;
}


void DspMstRatioHead(u16 x, u16 y, const char* p, u8 rsv,u8 sz)
{
	u8 strLen=0;
	//u8 lenTmp=0;
	u8 dspStart = 0;
	u8 i=0;
	char *pTmp=(char *)p;
	u16 y0=y;
	
	//printf("DspMstRatioHead\r\n");
	//puts(pTmp);

	if(DSP_NORMAL==rsv)
	{
		//memset(g_paraSetBuf,0x00,LAYOUT_RATIO_WIDTH*2);
		memset(g_paraSetBuf,0x00,sz*2);
	}else
	{
		//memset(g_paraSetBuf,0x3f,LAYOUT_RATIO_WIDTH);
		//memset(g_paraSetBuf+LAYOUT_RATIO_WIDTH,0xff,LAYOUT_RATIO_WIDTH);
		memset(g_paraSetBuf,0x3f,sz);
		memset(g_paraSetBuf+sz,0xff,sz);
	}
	g_paraSetBufIndex=0;

	while(*pTmp!='\0')
	{
		strLen += TitleLen((TitleFont*)titleFont, FONT_TITLE_LEN, *pTmp++);		
	}

	dspStart = (sz-strLen)/2;
	y0+=dspStart;
	g_paraSetBufIndex+=dspStart;

	pTmp = (char*)p;

	//printf("%d, %d\r\n",strLen,dspStart);

	while(*pTmp!='\0')
	{
		DisplayTitleChar(*pTmp, g_paraSetBuf+g_paraSetBufIndex,sz,rsv);
		strLen = TitleLen((TitleFont*)titleFont, FONT_TITLE_LEN, *pTmp);
		g_paraSetBufIndex+=strLen;
		y0+=strLen;
		pTmp++;
		//printf("%c, %d\r\n",*(pTmp-1),strLen);
	}	

	//printf("DspMstRatioHead\r\n");

	for(i=0;i<sz;i++)
	{
		g_paraSetBuf[i] |= 0x20;
		g_paraSetBuf[i+sz] |= 0x01;
	}//frame

	g_paraSetBuf[0] = g_paraSetBuf[sz-1] = 0x3f;
	g_paraSetBuf[sz] = g_paraSetBuf[2*sz-1] = 0xff;

	AreaSet(x, y, sz, 16);

	for(i=0;i<sz*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	
	
//#if(_DSP_FONT_OPTION_==BIG_FONT)
//	LcdDspAtCenter(x, y,p,FONT_TYPE_GRHEAD,rsv,LAYOUT_RATIO_WIDTH,sz);
//	LcdPicFramed(x, y, LAYOUT_RATIO_WIDTH, BIG_FONT_HIGHT(1),rsv);
//#elif(_DSP_FONT_OPTION_==SMALL_FONT)
//	LcdDspAtCenter(x, y,p,FONT_TYPE_GRHEAD,rsv,LAYOUT_RATIO_WIDTH,sz);
//	LcdPicFramed(x, y, LAYOUT_RATIO_WIDTH, BIG_FONT_HIGHT(1),rsv);
//#endif
}

void DspGrMode(u16 x, u16 y, const char* p, u8 rsv,u8 sz)
{
	u8 strLen=0;
	//u8 lenTmp=0;
	u8 dspStart = 0;
	u8 i=0;
	char *pTmp=(char *)p;
	u16 y0=y;
	
	//printf("DspGrMode\r\n");
	//puts(pTmp);

	if(DSP_NORMAL==rsv)
	{
		//memset(g_paraSetBuf,0x00,LAYOUT_RATIO_WIDTH*2);
		memset(g_paraSetBuf,0x00,sz*2);
	}else
	{
		//memset(g_paraSetBuf,0x3f,LAYOUT_RATIO_WIDTH);
		//memset(g_paraSetBuf+LAYOUT_RATIO_WIDTH,0xff,LAYOUT_RATIO_WIDTH);
		memset(g_paraSetBuf+1,0x3f,sz-1);
		memset(g_paraSetBuf+sz+1,0xff,sz-1);

		g_paraSetBuf[0] = g_paraSetBuf[sz-1] = 0x0f;
		g_paraSetBuf[sz] = g_paraSetBuf[2*sz-1] = 0xfc;

		g_paraSetBuf[1] = g_paraSetBuf[sz-2] = 0x1F;
		g_paraSetBuf[sz+1] = g_paraSetBuf[2*sz-2] = 0xFE;
	}
	g_paraSetBufIndex=0;

	while(*pTmp!='\0')
	{
		strLen += TitleLen((TitleFont*)titleFont, FONT_TITLE_LEN, *pTmp++);		
	}

	dspStart = (sz-strLen)/2;
	y0+=dspStart;
	g_paraSetBufIndex+=dspStart;

	pTmp = (char*)p;

	//printf("%d, %d\r\n",strLen,dspStart);
	//puts(pTmp);
	//printf("\r\n");

	while(*pTmp!='\0')
	{
		DisplayTitleChar(*pTmp, g_paraSetBuf+g_paraSetBufIndex,sz,rsv);
		strLen = TitleLen((TitleFont*)titleFont, FONT_TITLE_LEN, *pTmp);
		g_paraSetBufIndex+=strLen;
		y0+=strLen;
		pTmp++;
		//printf("%c, %d\r\n",*(pTmp-1),strLen);
	}	

	//printf("DspMstRatioHead\r\n");

	if(DSP_NORMAL==rsv)
	{

		for(i=2;i<sz-2;i++)
		{
			g_paraSetBuf[i] |= 0x20;
			g_paraSetBuf[i+sz] |= 0x01;
		}//frame

		g_paraSetBuf[0] = g_paraSetBuf[sz-1] = 0x0f;
		g_paraSetBuf[sz] = g_paraSetBuf[2*sz-1] = 0xfc;

		g_paraSetBuf[1] = g_paraSetBuf[sz-2] = 0x10;
		g_paraSetBuf[sz+1] = g_paraSetBuf[2*sz-2] = 0x02;

	}

	AreaSet(x, y, sz, 16);

	for(i=0;i<sz*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}

}





#ifdef _320x240_
void LcdDsp32Num(u16 x, u16 y, u8 num,u8 rsvMod)
{

}
#endif

#ifdef _320x240_
void LcdDspOutput(u16 x, u16 y, const u8 *p, u8 rsvMod)
{
	 while(*p!='\0')
    	{       
	        LcdDsp32Num(x,y,*p,rsvMod);
	        //y+=12;
	        y+=16;
	        p++;
    	}  
}
#endif

void LcdFillBlank(u16 x, u16 y, u16 wt,u16 ht,u8 rsv)
{
/*
	u16 i,j;
	//g_penColor = (rsv?BACK_COLOR:PEN_COLOR);
	g_penColor = (rsv?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]]);
	//g_penColor = (rsv?SEL_COLOR:PEN_COLOR);
	AreaSet(x,y,x+ht-1,y+wt-1);
	SET_CD_DATA;
	SET_CS_ENABLE;
	for(i=0;i<ht;i++)
	{
		for(j=0;j<wt;j++)
			LCD_WR_DATA(g_penColor);
	}
	SET_CS_DISABLE;
	//for(i=0;i<wt;i++)
	//{	
	//	for(j=0;j<ht;j++)
	//	{
	//		LCD_DrawPoint(x++,y+i);
	//	}
	//	x = x0;
	//}
*/
}


void DisplayButton(u16 x, u8 No, const char *p, u8 Font, u8 size, u8 fMod)
{
	u16 yLoc;
//	u8 sp;
	u8 strLen=0;
	u8 i=0;
	char *pTmp=(char*)p;

	memset(g_paraSetBuf,0x1F,LAYOUT_BUTTON_WIDTH);
	memset(g_paraSetBuf+LAYOUT_BUTTON_WIDTH,0xFF,LAYOUT_BUTTON_WIDTH);

	if(!No)
	{
		g_paraSetBufIndex=0;
	}else
	{
		g_paraSetBufIndex=1;
		g_paraSetBuf[0] = 0;
		g_paraSetBuf[LAYOUT_BUTTON_WIDTH] = 0;
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
	
	//yLoc = No*(LAYOUT_BUTTON_WIDTH+LAYOUT_BUTTON_INTERVAL);

	//LcdClearFeild(x,yLoc,80,24);

	//LcdFillBlank(LAYOUT_MENU_PAGE,yLoc,LAYOUT_BUTTON_WIDTH,LAYOUT_BUTTON_HIGHT,DSP_NORMAL);//(u16 x, u16 y, u16 wt, u16 ht, u8 rsv)
	//for(i=0;i<pa;i++)
	//{
	//	LcdFillBlank(x+(i<<3),yLoc,78,DSP_NORMAL);
	//}
	//LcdFillBlank(x,yLoc,78);
	//LcdFillBlank(x+8,yLoc,78);
	//if(size>LAYOUT_BUTTON_WIDTH)
	//{
	//	size = LAYOUT_BUTTON_WIDTH; 
	//	sp = 0;
	//}else
	//{
	//	sp = (LAYOUT_BUTTON_WIDTH - size)>>1;
	//}
#if 0
	if(!fMod)
	{
#ifdef _ST_EX_
		LCD_ShowString(x,yLoc+sp,p,Font,0,DSP_REVERSE);
#else
		LCD_ShowString(x,yLoc+sp,p,Font,1,DSP_REVERSE);
#endif
	}else
	{
		LcdLoadPicture(x,yLoc+sp,size,24,p,DSP_REVERSE);
	}
#endif	
}

void InsertList(FuncList ls)
{
	DisplayButton(LAYOUT_MENU_PAGE,0,ls.button[0]->dspStr,FONT_TYPE_MENU,ls.button[0]->strLen,ls.button[0]->dspMod);
	DisplayButton(LAYOUT_MENU_PAGE,1,ls.button[1]->dspStr,FONT_TYPE_MENU,ls.button[1]->strLen,ls.button[1]->dspMod);
	DisplayButton(LAYOUT_MENU_PAGE,2,ls.button[2]->dspStr,FONT_TYPE_MENU,ls.button[2]->strLen,ls.button[2]->dspMod);
	DisplayButton(LAYOUT_MENU_PAGE,3,ls.button[3]->dspStr,FONT_TYPE_MENU,ls.button[3]->strLen,ls.button[3]->dspMod);	
}

void DisplayMstSetDot(u16 x, u16 y,u8 rsv)
{
#if 0
	u8 i,j;
	u16 x0=x;
	//g_penColor = ((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);
	g_penColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
	//AreaSet(u16 x1, u16 y1, u16 x2, u16 y2);
	for(i=0;i<FONT_DOT_SIZE;i++)
	{	
		for(j=0;j<FONT_DOT_SIZE;j++)
		{
			LCD_DrawPoint(2+x++,2+y+i,DSP_NORMAL);
		}
		x = x0;
	}
#endif
}


void DisplayTtlCmp_classStyle(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char rvs)
{
	char tmp1,tmp2;
	if((!cmp1)&&(!cmp2))
	{
		if(DSP_SELE==rvs)
		{
			Lcd_ShowCmpFebChar(page,cml,'%',DSP_NORMAL);
			cml+=CanCharLen((CanFont *)canFontCmpFeb, FONT_CMPFEB_LEN, '%');
		}
		tmp1 = cmp1;
		tmp2 = cmp2;
	}else
	{
		if(cmp1<0)
		{
			tmp2 = cmp2;
		//#if(_DSP_FONT_OPTION_==BIG_FONT)
		//		LCD_ShowChar(page,cml,'-',FONT_TYPE_MAIN,rvs);
		//#elif(_DSP_FONT_OPTION_==SMALL_FONT)
		//		LCD_ShowChar(page,cml,'-',FONT_TYPE_MAIN,rvs);
		//#endif
			Lcd_ShowCmpFebChar(page,cml,'-',DSP_NORMAL);
			cml+=CanCharLen((CanFont *)canFontCmpFeb, FONT_CMPFEB_LEN, '-');

			if(0==cmp2)
			{
				tmp1 = (-1)*cmp1;
			}else
			{
				tmp1 = (-1)*(cmp1 + 1);
				tmp2 = 3 - tmp2;
			}
		}else 
		{
			tmp1 = cmp1;
			tmp2 = cmp2;
		//#if(_DSP_FONT_OPTION_==BIG_FONT)		
		//		LCD_ShowChar(page,cml,'+',FONT_TYPE_MAIN,rvs);
		//#elif(_DSP_FONT_OPTION_==SMALL_FONT)
		//		LCD_ShowChar(page,cml,'+',FONT_TYPE_MAIN,rvs);
		//#endif
			Lcd_ShowCmpFebChar(page,cml,'+',DSP_NORMAL);
			cml+=CanCharLen((CanFont *)canFontCmpFeb, FONT_CMPFEB_LEN, '+');
		}
	}

	DspCmpFeb(page, ++cml,cmpArr[tmp1*3+tmp2],rvs);

	//LCD_ShowString(page,cml+12,(u8*)(cmpArr+tmp1*3+tmp2),FONT_24x12,DSP_NORMAL);
//#if(_DSP_FONT_OPTION_==BIG_FONT)
//	LCD_ShowString(page,cml+MAIN_FONT_WIDTH(1),cmpArr[tmp1*3+tmp2],FONT_TYPE_MAIN,0,rvs);	
//	LCD_ShowString(page,cml+FONT_INTERVAL(1)+MAIN_FONT_WIDTH(3),"Ev",FONT_TYPE_MAIN,0,rvs);
//#elif(_DSP_FONT_OPTION_==SMALL_FONT)
//	LCD_ShowString(page,cml+MAIN_FONT_WIDTH(1),cmpArr[tmp1*3+tmp2],FONT_TYPE_MAIN,0,rvs);	
//	LCD_ShowString(page,cml+FONT_INTERVAL(1)+MAIN_FONT_WIDTH(3),"Ev",FONT_TYPE_MAIN,0,rvs);
//#endif
}

void DisplayTtlScale(u16 page, u16 cml, u16 org, u16 radius, unsigned char rvs)
{
	DisplayMstSetDot(page,cml+AUX_FONT_WIDTH(org)+AUX_FONT_WIDTH(1), rvs);		//"+8" should take the '-' take into consideration
	if(org<radius)
	{
		//DisplayPicture(page, cml, FEB_ARROW_SIZE, FEB_ARROW_SIZE, febDf,rvs);
	}else
	{
		//DisplayPicture(page, cml+AUX_FONT_WIDTH(org-radius)+AUX_FONT_WIDTH(1), FEB_ARROW_SIZE, FEB_ARROW_SIZE, febLab,rvs);
	}
	
	if(org+radius>18)
	{
		//DisplayPicture(page, cml+AUX_FONT_WIDTH(20), FEB_ARROW_SIZE, FEB_ARROW_SIZE,febOf,rvs);
	}else
	{
		//DisplayPicture(page, cml+AUX_FONT_WIDTH(org+radius)+AUX_FONT_WIDTH(1), FEB_ARROW_SIZE, FEB_ARROW_SIZE, febLab,rvs);
	}
	
}

//AreaScroll(char * sBuf, char wt, char ht, char dir, char n);
void DisplayTtlCmp_canStyle(u16 page,u16 cml,
								signed char bias,signed char dBias,signed char rBias,
								CmpPoint cp,
								signed char cmp1,signed char cmp2,
								char chWt,char dspLen,
								//char cellLen,
								float cellLen,
								//float chWt,char dspLen,
								char dir,char n,
								unsigned char rvs)
{
	u16 dspSt=0;
	u16 rds=0;
//	u16 tmp=0;
	u8 i=0;
	//dspSt =( cmp1 + 3)*3 + cmp2;
	if((WORKINGMODE_TTL==mainPara.workMode)
		||(WORKINGMODE_EXTA==mainPara.workMode))
	{
		if(g_stepLength>0)
		{
			dspSt =( cmp1 + 3)*2 + cmp2;
			rds = flaPara.ttl_FEB1*2+flaPara.ttl_FEB2;
		}else
		{
			dspSt =( cmp1 + 3)*3 + cmp2;
			rds = flaPara.ttl_FEB1*3+flaPara.ttl_FEB2;
		}
	}else
	{
		if(g_stepLength>0)
		{
			dspSt =( cmp1 + 3)*2 + cmp2;
		}else
		{
			dspSt =( cmp1 + 3)*3 + cmp2;
		}
	}

	//LcdClearFeild(page, cml, 24, 160);
	//LcdClearFeild(page, cml, 160, 24);
	//LCD_ShowString(page,cml,cmpString,FONT_TYPE_AUX,0,rvs);
	if(DSP_NORMAL==rvs)
	{
		//memset(g_paraSetBuf,0x00,208);
		memset(g_paraSetBuf,0x00,dspLen*2);
	}else
	{
		//memset(g_paraSetBuf,0x3f,104);
		//memset(g_paraSetBuf+104,0xFF,104);
		memset(g_paraSetBuf,0x3f,dspLen);
		memset(g_paraSetBuf+dspLen,0xFF,dspLen);
	}
	g_paraSetBufIndex= bias;
	//DisplayCanCmpString(chWt,(char*)cmpString,dspLen,rvs);
	DisplayCanCmpString(chWt,cp.cmpStr,dspLen,rvs);
	
	for(i=0;i<4;i++)
	{
		if(rvs==DSP_NORMAL)
		{
			//g_paraSetBuf[DSP_PARA_1LINE+(dspSt+1)*5+i] |= 0x1E;
			//g_paraSetBuf[dspLen+(dspSt+1)*chWt+i+bias] |= 0x1E;
			//g_paraSetBuf[dspLen+((dspSt+1)*chWt)+i+bias] |= 0x1E;
			g_paraSetBuf[dspLen+(char)((dspSt+1)*cellLen)+i+bias+dBias] |= 0x1E;
			
		}else
		{
			//g_paraSetBuf[dspLen+(dspSt+1)*chWt+i+bias] &= ~0x1E;
			//g_paraSetBuf[dspLen+((dspSt+1)*chWt)+i+bias] &= ~0x1E;
			g_paraSetBuf[dspLen+(char)((dspSt+1)*cellLen)+i+bias+dBias] &= ~0x1E;
		}
	}

	if(rds>0)
	{
		if(rds>dspSt)
		{
			for(i=0;i<4;i++)
			{
				if(rvs==DSP_NORMAL)
				{
					//g_paraSetBuf[dspLen+i] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'<')].buf[i+4];
					//g_paraSetBuf[dspLen+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'<')].buf[i+4];
					g_paraSetBuf[dspLen+i+bias+rBias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pDf)].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt+rds+1)*5+i] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt+rds+1)*chWt+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(char)((dspSt+rds+1)*chWt)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt+rds+1)*cellLen)+i+bias+dBias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
				}else
				{
					//g_paraSetBuf[dspLen+i] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'<')].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt+rds+1)*5+i] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'<')].buf[i+4];
					g_paraSetBuf[dspLen+i+bias+rBias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pDf)].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt+rds+1)*chWt+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(char)((dspSt+rds+1)*chWt)+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt+rds+1)*cellLen)+i+bias+dBias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
				}
			}
		}//else if(rds+dspSt>18)
		else if(rds+dspSt>((g_stepLength>0)?12:18))
		{
			for(i=0;i<4;i++)
			{
				if(rvs==DSP_NORMAL)
				{	
					//g_paraSetBuf[dspLen+100+i] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'>')].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt-rds+1)*5+i] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+chWt*20+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'>')].buf[i+4];
					//g_paraSetBuf[dspLen+chWt*20+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(char)(chWt*20)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(char)(cellLen*20)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(char)(cellLen*20)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					g_paraSetBuf[dspLen+dspLen-4+i] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt-rds+1)*chWt+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(char)((dspSt-rds+1)*chWt)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt-rds+1)*cellLen)+i+bias+dBias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
				}else
				{
					//g_paraSetBuf[dspLen+chWt*20+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'>')].buf[i+4];
					//g_paraSetBuf[dspLen+chWt*20+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(char)(chWt*20)+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(char)(cellLen*20)+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					g_paraSetBuf[dspLen+dspLen-4+i] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,cp.pOf)].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt-rds+1)*chWt+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(char)((dspSt-rds+1)*chWt)+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt-rds+1)*cellLen)+i+bias+dBias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
				}
			}
		}else
		{
			for(i=0;i<4;i++)
			{
				if(rvs==DSP_NORMAL)
				{	
					//g_paraSetBuf[dspLen+(dspSt+rds+1)*chWt+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt-rds+1)*chWt+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+((dspSt+rds+1)*chWt)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt+rds+1)*cellLen)+i+bias+dBias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+((dspSt-rds+1)*chWt)+i+bias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt-rds+1)*cellLen)+i+bias+dBias] |= canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
				}else
				{
					//g_paraSetBuf[dspLen+(dspSt+rds+1)*chWt+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+(dspSt-rds+1)*chWt+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+((dspSt+rds+1)*chWt)+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt+rds+1)*cellLen)+i+bias+dBias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					//g_paraSetBuf[dspLen+((dspSt-rds+1)*chWt)+i+bias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
					g_paraSetBuf[dspLen+(char)((dspSt-rds+1)*cellLen)+i+bias+dBias] &= ~canFontPara[CanCharGetIndex((CanFont*)canFontPara,FONT_PARA_LEN,'f')].buf[i+4];
				}
			}
		}

		
	}

	AreaScroll(g_paraSetBuf,dspLen,16/8,dspLen,dir,n);
	
	AreaSet(page,cml,dspLen,16);
	  
	//for(i=0;i<sizeof(g_paraSetBuf);i++)
	for(i=0;i<dspLen*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	//DisplayCanParaString(page ,cml,cmp);
	//LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);		//clear the field or highlight
	/*Clear field at every time when parameter changed*/
	//LcdClearFeild(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN),LAYOUT_DOTAREA_HT);
	//LcdClearFeild(page+AUX_FONT_HIGHT(1), cml, AUX_FONT_WIDTH(sizeof(cmpString)),BIG_FONT_HIGHT(1)-AUX_FONT_HIGHT(1));
	//if(!rds)
	//{
		//DisplayMstSetDot(page+AUX_FONT_HIGHT(1),cml+AUX_FONT_WIDTH(dspSt)+AUX_FONT_WIDTH(1), rvs);		//"+8" should take the '-' take into consideration
	//}
	//else
	//{
		//DisplayTtlScale(page+AUX_FONT_HIGHT(1),cml,dspSt,rds,rvs);
	//}
}

void DisplayTtlCmp(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char rvs)
{
	CmpPoint cmpp={0};
	//cmpp.cmpStr = (char*)cmpString;
	//cmpp.pDf = '<';
	//cmpp.pOf = '>';

	if(g_stepLength>0)
	{
		cmpp.cmpStr = (char*)cmpString_5;
		cmpp.pDf = '<';
		cmpp.pOf = '>';
		DisplayTtlCmp_canStyle(page,cml,0,-2,0,cmpp,cmp1,cmp2,5,DSP_PARA_1LINE,7.5f,0,0,rvs);
	}else
	{
		cmpp.cmpStr = (char*)cmpString;
		//cmpp.pDf = '<';
		//cmpp.pOf = '>';
		cmpp.pDf = '<';
		cmpp.pOf = '>';
		DisplayTtlCmp_canStyle(page,cml,0,0,0,cmpp,cmp1,cmp2,5,DSP_PARA_1LINE,5,0,0,rvs);
	}

#if 0
	if(DSP_STYLE_CANON==g_dspStyle)
	{
		
		DisplayTtlCmp_canStyle(page,cml,0,cmpp,cmp1,cmp2,5,DSP_PARA_1LINE,0,0,rvs);
	}else if(DSP_STYLE_CLASSIC==g_dspStyle)
	{
		DisplayTtlCmp_classStyle(page,cml,cmp1,cmp2,rvs);
	}
#endif
}

#if 0
void DisplayMstOtp_classStyle(u16 page,u16 cml,unsigned char para1,unsigned char para2,unsigned char rvs)
{
	char *str=NULL;
	u16 sz=0;

	//LcdClearFeild(page, cml, MAIN_FONT_HIGHT(1), 160);
	//LcdClearFeild(page, cml, 160,MAIN_FONT_HIGHT(1));
	
	switch(para1+1)
	{
		case 1:
			str = "1/128";	
			sz= 5;
		break;

		case 2:
			str = "1/64";
			sz= 4;
		break;

		case 3:
			str = "1/32";
			sz= 4;
		break;

		case 4:
			str = "1/16";	
			sz= 4;
		break;

		case 5:
			str = "1/8";	
			sz= 3;
		break;

		case 6:
			str = "1/4";	
			sz= 3;
		break;

		case 7:
			str = "1/2";	
			sz= 3;
		break;

		case 8:
			str = "1/1";	
			sz= 3;
		break;
	}

#if(_DSP_FONT_OPTION_==BIG_FONT)
	LCD_ShowString(page, cml, str,FONT_32x16,0,rvs);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	LCD_ShowString(page, cml, str,FONT_24x12,0,rvs);
#endif
	
	switch(para2)
	{
		case 0:
			str = "";
		break;

		case 1:
			str = "+0.3 Ev";
		break;

		case 2:
			str = "+0.7 Ev";
		break;		
	}
	
#if(_DSP_FONT_OPTION_==BIG_FONT)
	LCD_ShowString(page+AUX_FONT_WIDTH(1), sz*12+cml+16, str,FONT_16x08,0,rvs);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	LCD_ShowString(page+AUX_FONT_WIDTH(1), sz*12+cml+16, str,FONT_16x08,0,rvs);
#endif
}
#endif

u8 CanCharLen(CanFont *font, char fLen,char ch)
{
	char i=0;
	for(i=0;i<fLen;i++)
	{
		if(ch == font[i].ch)
		{
			return font[i].wt;
		}
	}
	return 0;
}

u8 TitleLen(TitleFont *tFont, char tLen, char ch)
{
	char i=0;
	for(i=0;i<tLen;i++)
	{
		if(ch==tFont[i].ch)
		{
			return tFont[i].wt;
		}
	}
	return 0;
}

char TitleGetIndex(TitleFont *font, char fLen,char ch)
{
	char i=0;
	for(i=0;i<fLen;i++)
	{
		if(ch == font[i].ch)
		{
			return i;
		}
	}

	return 0;
}

char CanCharGetIndex(CanFont *font, char fLen,char ch)
{
	char i=0;
	for(i=0;i<fLen;i++)
	{
		if(ch == font[i].ch)
		{
			return i;
		}
	}

	return 0;
}

char Lcd_ShowCmpFebChar(u16 page,u16 cml,char ch,u8 rsv)
{
	CanFont *pFont;
	char i;

	pFont = (CanFont*)&canFontCmpFeb[CanCharGetIndex((CanFont*)canFontCmpFeb, FONT_CMPFEB_LEN, ch)];


	AreaSet(page, cml, pFont->wt, 16);


	for(i=0;i<(pFont->wt)*2;i++)
	{
		LCD_WRITE_BYTE(pFont->buf[i]);	
	}

	return NULL;	
}

char DisplayTitleChar(char ch,char *gBuf,u8 dspLen,unsigned char rvs)
{
	TitleFont *pFont;
	//char i=0,j=0;
	char i=0;


	pFont = (TitleFont*)&titleFont[TitleGetIndex((TitleFont*)titleFont, FONT_TITLE_LEN, ch)];
	//pFont = (canFontPara+CanCharGetIndex(canFontPara, FONT_PARA_LEN, ch));
		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+cml);
		//LCD_WRITE_BYTE(SEG_START+cml+pFont->wt); 
		//LCD_WRITE_CMD(0x5c);
	for(i=0;i<pFont->wt;i++)
	{
		if(DSP_NORMAL==rvs)
		{
			//LCD_WRITE_BYTE(pFont->buf[j+i*pFont->wt]); 
			//g_paraSetBuf[g_paraSetBufIndex] = pFont->buf[i];
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i] = pFont->buf[i];
			//gBuf[i+LAYOUT_RATIO_WIDTH] = pFont->buf[i+pFont->wt];
			gBuf[i+dspLen] = pFont->buf[i+pFont->wt];
			//g_paraSetBufIndex++;
		}else
		{

			//LCD_WRITE_BYTE(~(pFont->buf[j+pFont->wt])); 
			//g_paraSetBuf[g_paraSetBufIndex++] = ~(pFont->buf[j+pFont->wt]);
			//g_paraSetBuf[g_paraSetBufIndex] = (~(pFont->buf[i]))&0x3f;
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			//g_paraSetBufIndex++;
			gBuf[i] = (~(pFont->buf[i]))&0x3F;;
			//gBuf[i+LAYOUT_RATIO_WIDTH] = (~(pFont->buf[i+pFont->wt]));
			gBuf[i+dspLen] = (~(pFont->buf[i+pFont->wt]));

		}
	}

	return pFont->wt;
}

void DspApertValue(int apert, u8 rvs)
{
	//DisplayPicture(uchar page, uchar column, uchar wide, uchar tall, const char * p, uchar rsv);
	u8 i;
	u8 ds1,ds2;
	memset(g_paraSetBuf,0x00,27*2);
	memcpy(g_paraSetBuf,fontIso+12*10,6);
	memcpy(g_paraSetBuf+27,fontIso+12*10+6,6);		//'F'
	//g_paraSetBufIndex = 7;
	

	if(DSP_REVERSE==rvs)
	{
		memset(g_paraSetBuf+7,0xff,20);
		memset(g_paraSetBuf+7+27,0xff,20);
	}

	//g_paraSetBufIndex++;

	if(apert<100)
	{
		ds1 = apert/10;
		ds2 = apert%10;
		if(DSP_NORMAL==rvs)
		{
			for(i=0;i<6;i++)
			{
				g_paraSetBuf[8+i] = fontIso[(ds1)*12+i];
				g_paraSetBuf[8+i+27] = fontIso[(ds1)*12+i+6];
				g_paraSetBuf[20+i] = fontIso[(ds2)*12+i];
				g_paraSetBuf[20+i+27] = fontIso[(ds2)*12+i+6];
			}
			g_paraSetBuf[16+27] = 0x18;
			g_paraSetBuf[17+27] = 0x18;
		}else
		{
			for(i=0;i<6;i++)
			{
				g_paraSetBuf[8+i] = ~fontIso[(ds1)*12+i];
				g_paraSetBuf[8+i+27] = ~fontIso[(ds1)*12+i+6];
				g_paraSetBuf[20+i] = ~fontIso[(ds2)*12+i];
				g_paraSetBuf[20+i+27] = ~fontIso[(ds2)*12+i+6];
			}
			g_paraSetBuf[16+27] = ~0x18;
			g_paraSetBuf[17+27] = ~0x18;
		}

		
	}else
	{
		ds1 = apert/100; 
		ds2 = (apert%100)/10;
		
		if(DSP_NORMAL==rvs)
		{
			for(i=0;i<6;i++)
			{
				g_paraSetBuf[10+i] = fontIso[(ds1)*12+i];
				g_paraSetBuf[10+i+27] = fontIso[(ds1)*12+i+6];
				g_paraSetBuf[18+i] = fontIso[(ds2)*12+i];
				g_paraSetBuf[18+i+27] = fontIso[(ds2)*12+i+6];
			}
		}else
		{
			for(i=0;i<6;i++)
			{
				g_paraSetBuf[10+i] = ~fontIso[(ds1)*12+i];
				g_paraSetBuf[10+i+27] = ~fontIso[(ds1)*12+i+6];
				g_paraSetBuf[18+i] = ~fontIso[(ds2)*12+i];
				g_paraSetBuf[18+i+27] = ~fontIso[(ds2)*12+i+6];
			}
		}
	}

	for(i=0;i<27;i++)
	{
		g_paraSetBuf[i] &=0x3F;
		g_paraSetBuf[i+27] &=0xFC;
	}

	LcdCopyToRam(7, 141, g_paraSetBuf, 27, 16);
	
}

void DspIsoValue(u16 x, u16 y,int iso,u8 rvs)
{
	int tIso=iso,m=1;
	u8 i=0;
	u8 strLen=0;
	u8 tmp;
/*
	if(DSP_NORMAL==rvs)
	{
		memset(g_paraSetBuf,0x00,10);
	}else
	{
		memset(g_paraSetBuf,0xFF,10);
	}
*/
	memset(g_paraSetBuf,0x00,(5*8)*2);
	g_paraSetBufIndex = 1;

	while(tIso>0)
	{
		strLen++;
		tIso /= 10;
		m*=10;
	}

	m/=10;
	//strLen*=7+1;
	strLen = (strLen*8);

	if(DSP_NORMAL==rvs)
	{
		memset(g_paraSetBuf,0x00,strLen*2);
	}else
	{
		memset(g_paraSetBuf,0xFF,strLen);
		memset(g_paraSetBuf+strLen,0xFF,strLen);
	}	

	//tIso = mainPara.extIso;
	tIso = iso;
	//strLen = 5*7+1;
	
	while(m>0)
	{
		tmp = tIso/m;
		if(DSP_NORMAL==rvs)
		{
			memcpy(g_paraSetBuf+g_paraSetBufIndex,fontIso+12*tmp,6);
			memcpy(g_paraSetBuf+g_paraSetBufIndex+strLen,fontIso+12*tmp+6,6);
			//g_paraSetBufIndex+=7;			
		}else
		{
			for(i=0;i<6;i++)
			{
				g_paraSetBuf[i+g_paraSetBufIndex] = ~fontIso[i+12*tmp];
				g_paraSetBuf[i+strLen+g_paraSetBufIndex] = ~fontIso[i+12*tmp+6];
			}
			//g_paraSetBufIndex+=7;	
		}
		//tIso/=10;
		tIso%=m;
		m/=10;
		g_paraSetBufIndex+=8;	
	}

	//LcdCopyToRam(LAYOUT_PARA_AREA2, 60,g_paraSetBuf, strLen, 16);	
	LcdClearFeild(x, y, (5*8)*2, 16);
	LcdCopyToRam(x, y,g_paraSetBuf, strLen, 16);	
	
}

char LoadZoomChar(char ch,char *gBuf,u8 dspLen,unsigned char rvs)
{
	TitleFont *pFont;
	//char i=0,j=0;
	char i=0;


	pFont = (TitleFont*)&titleFont[TitleGetIndex((TitleFont*)titleFont, FONT_TITLE_LEN, ch)];
	//pFont = (canFontPara+CanCharGetIndex(canFontPara, FONT_PARA_LEN, ch));

		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+cml);
		//LCD_WRITE_BYTE(SEG_START+cml+pFont->wt); 
		//LCD_WRITE_CMD(0x5c);
	for(i=0;i<pFont->wt;i++)
	{
		if(DSP_NORMAL==rvs)
		{
			//LCD_WRITE_BYTE(pFont->buf[j+i*pFont->wt]); 
			//g_paraSetBuf[g_paraSetBufIndex] = pFont->buf[i];
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i] = pFont->buf[i];
			//gBuf[i+LAYOUT_RATIO_WIDTH] = pFont->buf[i+pFont->wt];
			gBuf[i+dspLen] = pFont->buf[i+pFont->wt];
			//g_paraSetBufIndex++;
		}else
		{

			//LCD_WRITE_BYTE(~(pFont->buf[j+pFont->wt])); 
			//g_paraSetBuf[g_paraSetBufIndex++] = ~(pFont->buf[j+pFont->wt]);
			//g_paraSetBuf[g_paraSetBufIndex] = (~(pFont->buf[i]))&0x3f;
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			//g_paraSetBufIndex++;
			gBuf[i] = (~(pFont->buf[i]));;
			//gBuf[i+LAYOUT_RATIO_WIDTH] = (~(pFont->buf[i+pFont->wt]));
			gBuf[i+dspLen] = (~(pFont->buf[i+pFont->wt]));

		}
	}

	return pFont->wt;
}


char LoadMenuChar(char ch,char *gBuf,char dspLen,unsigned char rvs)
{
	CanFont *pFont;
	//char i=0,j=0;
	char i=0;

	pFont = (CanFont*)&canFontMenu[CanCharGetIndex((CanFont*)canFontMenu, FONT_MENU_LEN, ch)];
	//pFont = (canFontPara+CanCharGetIndex(canFontPara, FONT_PARA_LEN, ch));

		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+cml);
		//LCD_WRITE_BYTE(SEG_START+cml+pFont->wt); 
		//LCD_WRITE_CMD(0x5c);
	for(i=0;i<pFont->wt;i++)
	{
		if(DSP_NORMAL==rvs)
		{
			//LCD_WRITE_BYTE(pFont->buf[j+i*pFont->wt]); 
			//g_paraSetBuf[g_paraSetBufIndex] = pFont->buf[i];
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i] = pFont->buf[i];
			//gBuf[i+PARA_CMPFEB_WT] = pFont->buf[i+pFont->wt];
			gBuf[i+dspLen] = pFont->buf[i+pFont->wt];
			//g_paraSetBufIndex++;
		}else
		{

			//LCD_WRITE_BYTE(~(pFont->buf[j+pFont->wt])); 
			//g_paraSetBuf[g_paraSetBufIndex++] = ~(pFont->buf[j+pFont->wt]);
			//g_paraSetBuf[g_paraSetBufIndex] = (~(pFont->buf[i]))&0x3f;
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			//g_paraSetBufIndex++;
			gBuf[i] = (~(pFont->buf[i]))&0x1F;;
			//gBuf[i+PARA_CMPFEB_WT] = (~(pFont->buf[i+pFont->wt]))&0xFE;
			gBuf[i+dspLen] = (~(pFont->buf[i+pFont->wt]));

		}
	}

	return pFont->wt;
}

char RamLoadPicture(char *p,char wt, char ht ,char *gBuf, char dspLen, unsigned char rvs)
{
	char i=0,j=0;
	for(j=0;j<ht/8;j++)
	{
		for(i=0;i<wt;i++)
		{
			if(DSP_NORMAL==rvs)
			{
				//gBuf[j*dspLen+i] = p[i+j*wt];
				gBuf[j*dspLen+i] |= p[i+j*wt];
			}else
			{
				//gBuf[j*dspLen+i] = ~p[i+j*wt];
				gBuf[j*dspLen+i] &= ~p[i+j*wt];
			}
		}
	}

	return 1;
}

#if 1
char LoadCmpFebChar(char ch,char *gBuf,char dspLen,unsigned char rvs)
{
	CanFont *pFont;
	//char i=0,j=0;
	char i=0;

	pFont = (CanFont*)&canFontCmpFeb[CanCharGetIndex((CanFont*)canFontCmpFeb, FONT_CMPFEB_LEN, ch)];
	//pFont = (canFontPara+CanCharGetIndex(canFontPara, FONT_PARA_LEN, ch));

		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+cml);
		//LCD_WRITE_BYTE(SEG_START+cml+pFont->wt); 
		//LCD_WRITE_CMD(0x5c);
	for(i=0;i<pFont->wt;i++)
	{
		if(DSP_NORMAL==rvs)
		{
			//LCD_WRITE_BYTE(pFont->buf[j+i*pFont->wt]); 
			//g_paraSetBuf[g_paraSetBufIndex] = pFont->buf[i];
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i] = pFont->buf[i];
			//gBuf[i+PARA_CMPFEB_WT] = pFont->buf[i+pFont->wt];
			gBuf[i+dspLen] = pFont->buf[i+pFont->wt];
			//g_paraSetBufIndex++;
		}else
		{

			//LCD_WRITE_BYTE(~(pFont->buf[j+pFont->wt])); 
			//g_paraSetBuf[g_paraSetBufIndex++] = ~(pFont->buf[j+pFont->wt]);
			//g_paraSetBuf[g_paraSetBufIndex] = (~(pFont->buf[i]))&0x3f;
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			//g_paraSetBufIndex++;
			gBuf[i] = (~(pFont->buf[i]))&0x7F;;
			//gBuf[i+PARA_CMPFEB_WT] = (~(pFont->buf[i+pFont->wt]))&0xFE;
			gBuf[i+dspLen] = (~(pFont->buf[i+pFont->wt]))&0xFE;

		}
	}
	return pFont->wt;
}
#endif

char LoadCanParaChar(char ch,char *gBuf,char aLen,unsigned char rvs)
{
	CanFont *pFont;
	//char i=0,j=0;
	char i=0;

	pFont = (CanFont*)&canFontPara[CanCharGetIndex((CanFont*)canFontPara, FONT_PARA_LEN, ch)];
	//pFont = (canFontPara+CanCharGetIndex(canFontPara, FONT_PARA_LEN, ch));
		//LCD_WRITE_CMD(0x30);
		//LCD_WRITE_CMD(0x75); //set page address
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_BYTE(page+i);
		//LCD_WRITE_CMD(0x15); //set column address
		//LCD_WRITE_BYTE(SEG_START+cml);
		//LCD_WRITE_BYTE(SEG_START+cml+pFont->wt); 
		//LCD_WRITE_CMD(0x5c);
	for(i=0;i<pFont->wt;i++)
	{
		if(DSP_NORMAL==rvs)
		{
			//LCD_WRITE_BYTE(pFont->buf[j+i*pFont->wt]); 
			//g_paraSetBuf[g_paraSetBufIndex] = pFont->buf[i];
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i] = pFont->buf[i];
			//gBuf[i+DSP_PARA_1LINE] = pFont->buf[i+pFont->wt];
			gBuf[i+aLen] = pFont->buf[i+pFont->wt];
			//g_paraSetBufIndex++;
		}else
		{

			//LCD_WRITE_BYTE(~(pFont->buf[j+pFont->wt])); 
			//g_paraSetBuf[g_paraSetBufIndex++] = ~(pFont->buf[j+pFont->wt]);
			//g_paraSetBuf[g_paraSetBufIndex] = (~(pFont->buf[i]))&0x3f;
			//g_paraSetBuf[g_paraSetBufIndex+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			//g_paraSetBufIndex++;
			gBuf[i] = (~(pFont->buf[i]))&0x3f;;
			//gBuf[i+DSP_PARA_1LINE] = ~(pFont->buf[i+pFont->wt]);
			gBuf[i+aLen] = ~(pFont->buf[i+pFont->wt]);

		}
	}

	return pFont->wt;
}

u16 DisplayCanCmpString(u16 wt,char * str,char dspLen,unsigned char rvs)
{
	while(*str!='\0')
	{
		LoadCanParaChar(*str++,g_paraSetBuf+g_paraSetBufIndex,dspLen,rvs);
	//	cml+=5;
		g_paraSetBufIndex+=wt;
	}

	return 1;
}

u16 DisplayCanOtpString(u8 dspLen,u8 spLen,u8 blkLen,/*u16 page,u16 cml,*/u16 wt,char * str,unsigned char rvs)
{
	u16 charLen=0;
	u16 strLen=0;
	char *pStr=str;
	//u16 tCml=cml;
	u8 tmp=0;
	u8 chSum=0;
	//u8 i;
	//memset(g_paraSetBuf,0x00,208);

	while(strLen<wt)
	{

//		printf("%c\r\n",*str);
		if(*str=='.')
		{
			LoadCanParaChar(*str,g_paraSetBuf+g_paraSetBufIndex,dspLen,rvs);
			//LoadCanParaChar(*str,g_paraSetBuf+g_paraSetBufIndex,wt,rvs);
			strLen += 4;
			//cml+=4;
			str++;
			g_paraSetBufIndex+=4;

		}else if(*str==' ')
		{
			str++;
			//cml+=5;
			//strLen+=5;
			strLen+=spLen;
			//g_paraSetBufIndex+=5;
			g_paraSetBufIndex+=spLen;

		}else
		{
			pStr = str;
			//tCml = cml;
			charLen = 0;
			while(*pStr!='.')
			{
				//charLen += DisplayCanParaChar(page,cml+charLen,*str++,rvs);
				charLen += CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *pStr);
				pStr++;
			}

			
			//tCml += ((DSP_BLOCK_LEN - charLen) / 2);
			//chSum = ((DSP_BLOCK_LEN - charLen) / 2);
			chSum = ((blkLen- charLen) / 2);

			tmp = 0;
			//chSum = tCml;
			
			//while((*str!='.')&&((strLen+CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *str))<wt))
			while((*str!='.')&&(*str!='\0'))
			{
				if((strLen+chSum+CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *str))>=wt)
				{
					break;
				}
				LoadCanParaChar(*str,g_paraSetBuf+(g_paraSetBufIndex+chSum),dspLen,rvs);
				
				tmp = CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *str);
				//tCml+=tmp;
				chSum+=tmp;
				//g_paraSetBufIndex+=tmp;
				str++;

			}

			//strLen += DSP_BLOCK_LEN;
			strLen += blkLen;
			//cml += DSP_BLOCK_LEN;
			//tCml = cml;
			//g_paraSetBufIndex+=DSP_BLOCK_LEN;
			g_paraSetBufIndex+=blkLen;
			//delay_ms(1000);
			
		}

	}

	return strLen;
}

void DisplayParaCmlSkip(u16 page,u16 cml,u8 nSkip,unsigned char rvs)
{
	char i;
	if(rvs==DSP_REVERSE)
	{
		LCD_WRITE_CMD(0x30);
		LCD_WRITE_CMD(0x75); //set page address
		LCD_WRITE_BYTE(page);
		LCD_WRITE_BYTE(page+1);
		LCD_WRITE_CMD(0x15); //set column address
		LCD_WRITE_BYTE(SEG_START+cml);
		LCD_WRITE_BYTE(SEG_START+cml+nSkip); 
		LCD_WRITE_CMD(0x5c);
       	for(i=0;i<nSkip*2;i++)
       	{
       		if(i<nSkip)
       		{
				LCD_WRITE_BYTE(0xff&0x3f); 
       		}else
       		{
				LCD_WRITE_BYTE(0xff); 
       		}
       	}
	}
}

#if 0
char CanParaStringLen(char *pStr, char dir)
{
	u8 len=0;
	while(*pStr!='.')
	{
		len += CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *pStr);
		if(dir>0)
		{
			pStr--;
		}else
		{
			pStr++;
		}
	}
	return len;
}
#endif

void DisplayMstOtp_canStyle(u16 page,u16 cml,
							     char mod,char dspLen,char blockLen,
							     unsigned char para1,unsigned char para2,
							     u8 spLen,char *pDot,
							     char dir,char n,
							     unsigned char rvs)
{
	u8 dspSt;
	char *pStart=NULL;
	char *pTmp;
	u8 bLen=0;
//	u8 tLen=0;
	//u8 strLen;
	u8 tmp=0;
	u16 i=0;
	u8 para = (para1)*3+para2;
	u16 tCml=cml;
	u16 dotSet=0;
	u8 critical;

	//return;
	if(WORKINGMODE_MULTI==mod)
	{
		critical = 15;
	}else
	{
		critical = 19;
	}
	
	if(DSP_NORMAL==rvs)
	{
		memset(g_paraSetBuf,0x00,dspLen*2);
	}else
	{
		memset(g_paraSetBuf,0x3f,dspLen);
		memset(g_paraSetBuf+dspLen,0xFF,dspLen);
	}

	if(para<3)
	{
		pStart = (char *)outputString;
		//cml+=2;
		//dotSet = dotSetLoc[para];
		dotSet = pDot[para];
		g_paraSetBufIndex = 0;
	}else if(para<critical)//else if(para<19)
	{
		//dotSet = dotSetLoc[3];
		dotSet = pDot[3];
		//dspSt = otpStrIndex[(para1)*3+para2];
		if(!para2)
		{
			//para -= 3;
			dspSt = otpStrIndex[para-3];
			pStart = (char*)(outputString+dspSt);
			g_paraSetBufIndex = 0;
		}else if(1==para2)
		{
			g_paraSetBufIndex = 0;			
			//para -= 3;//50-26-4-5-4 = 11
			dspSt = otpStrIndex[para-3];
			pStart = (char*)(outputString+dspSt-1);
			if(1==para1)
			{
				tCml+=4;
				g_paraSetBufIndex+=4;
				//DisplayCanParaChar(page,cml,'8',g_paraSetBuf+g_paraSetBufIndex,rvs);
			}
			else if(5==para1)
			{
				tCml+=1;
				g_paraSetBufIndex=1;
				//DisplayCanParaChar(page,cml,'8',g_paraSetBuf+g_paraSetBufIndex,rvs);
			}else
			{
				tCml+=2;
				g_paraSetBufIndex=2;
				//DisplayCanParaChar(page,cml,'8',g_paraSetBuf+g_paraSetBufIndex,rvs);
			}
				
			tmp = LoadCanParaChar(*pStart,g_paraSetBuf+g_paraSetBufIndex,dspLen,rvs);
			//tmp=CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *pStart);
			g_paraSetBufIndex+=tmp;
			tCml+=tmp;
			pStart++;

			//LcdCopyToRam(page,cml,DSP_PARA_1LINE);

			//delay_ms(1000);

			pTmp = (char*)(outputString+otpStrIndex[para-4]);
			while(*pTmp!='.')
			{
				bLen += CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *pTmp);
				pTmp++;
			}
			tCml += (blockLen-bLen)/2;
			g_paraSetBufIndex += (blockLen-bLen)/2;
			
			
		}else if(2==para2)
		{
			//para -=4;
			dspSt = otpStrIndex[para-4];
			pStart = (char*)(outputString+dspSt);
			tCml+=2;
			g_paraSetBufIndex=2;
		}
		//puts(pStart);

	}else
	{
		//dotSet = dotSetLoc[para-19+4];
		//dotSet = pDot[para-19+4];
		dotSet = pDot[para-critical+4];
		g_paraSetBufIndex = 0;
		if(WORKINGMODE_MULTI==mod)
		{
			pStart = (char*)(outputString+22);
		}else
		{
			pStart = (char*)(outputString+38);
		}
	}

	//puts(pStart);
	//cml = (CanCharLen(canFontPara, FONT_PARA_LEN, *pStart)-(bLen-tLen));
	//memset(g_paraSetBuf,0x00,208);
	//g_paraSetBufIndex=tLen;
	//puts(pStart);
	DisplayCanOtpString(dspLen,spLen,blockLen,/*page,tCml,*/(dspLen-g_paraSetBufIndex),pStart,rvs);	
	
	for(i=0;i<4;i++)
	{
		if(rvs==DSP_NORMAL)
		{
			g_paraSetBuf[dspLen+dotSet+i] |= 0x1E;
		}else
		{
			g_paraSetBuf[dspLen+dotSet+i] &= ~0x1E;
		}
	}

	AreaScroll(g_paraSetBuf,dspLen,16/8,dspLen,dir,n);

	AreaSet(page,cml,dspLen,16);

	for(i=0;i<dspLen*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	
		//DisplayCanParaString(page, cml+tLen,outputString+dspSt,rvs);
	
/*	
	dspSt = otpStrIndex[(para1)*3+para2];

	if(WORKINGMODE_MULTI==mod)
	{
		strLen = 32;
	}else
	{
		strLen = 42;
	}
	
	if(dspSt<8)
	{
		dsHead = 0;
	}else if(dspSt<(strLen-8))
	{
		dsHead = dspSt-8;
	}else
	{
		dsHead = (strLen-GR_DSP_LEN+1);
	}
*/
/*
	if(DSP_REVERSE==rvs)
	{
		LcdFillBlank(page+16,cml,GR_DSP_LEN*8, DSP_NORMAL);
	}
*/	
	//LcdClearFeild(page, cml, AUX_FONT_WIDTH(17), 24);
	//LcdClearFeild(page, cml, AUX_FONT_WIDTH(17), 24);

//	LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);

//	pStart = (unsigned char*)outputString+dsHead;
	
	/*
	if(dspSt<8)
	{
		pStart = (unsigned char *)outputString; 
		DisplayMstSetDot(page+16,cml+outputString[dspSt],rvs);
	}else if(dspSt<(42-GR_DSP_LEN))
	{
		pStart = (unsigned char *)outputString+(dspSt-8);
		DisplayMstSetDot(page+16,cml+8*8,rvs);
	}else
	{
		pStart = (unsigned char *)outputString+(42-GR_DSP_LEN);
		DisplayMstSetDot(page+16,cml+(outputString[dspSt]-(42-GR_DSP_LEN)),rvs);
	}*/
	
//#if(_DSP_FONT_OPTION_==BIG_FONT)
//	LcdPutBuf(page,cml,pStart,FONT_TYPE_MAST,GR_DSP_LEN,0,rvs);
//	LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);
//	DisplayMstSetDot(page+AUX_FONT_HIGHT(1),cml+(dspSt-dsHead)*AUX_FONT_WIDTH(1),rvs);	
//#elif(_DSP_FONT_OPTION_==SMALL_FONT)
//	LcdPutBuf(page,cml,pStart,FONT_TYPE_MAST,GR_DSP_LEN,0,rvs);
//	LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);
//	DisplayMstSetDot(page+AUX_FONT_HIGHT(1),cml+(dspSt-dsHead)*AUX_FONT_WIDTH(1),rvs);	
//#endif
	
	//LcdFillBlank(page+16,cml,GR_DSP_LEN*8,rvs);
}

//u8 g_abrLoc[5] = {10,30,50,70,90}

void DisplayABRation(signed char ABr1,signed char ABr2,unsigned char rsv)		//0~~12
{
	u8 dspSt;
//	u8 dsHead;
	//u8 cml=0;
	u8 dotSet=0;
	u8 tmp=0;
	u8 cnt=0;
	u8 i=0;
	unsigned char *pStart;
	unsigned char *ps;
	//signed char ABr=(ABr1+3)*3+ABr2;
	signed char ABr = ABr1;
	dspSt = (ABr*2+(ABr%2));
	if(dspSt<=4)
	{
		//dsHead = 0;
		pStart = (unsigned char*)abRatio;
		dotSet = ABr*20+10;
		
	}//else if(dspSt<19)
	else if(dspSt<21)
	{
		//dsHead = dspSt-7;
		dspSt = ((ABr-2)*2+((ABr-2)%2));
		pStart = (unsigned char*)(abRatio + dspSt);
		dotSet = 50;
	}else
	{
		//dsHead = (39 -ABr_DSP_LEN);
		pStart = (unsigned char*)(abRatio + 16);
		//dotSet = (ABr-9)*20+70;
		dotSet = (ABr-11)*20+70;
	}
	//pStart = (unsigned char*)abRatio+dsHead;
	//puts(pStart);

	if(DSP_NORMAL==rsv)
	{
		memset(g_paraSetBuf,0x00,208);
	}else
	{
		memset(g_paraSetBuf,0x3f,104);
		memset(g_paraSetBuf+104,0xFF,104);
	}

	g_paraSetBufIndex=0;

	if(*pStart=='.')
	{
		//cml += 10;
		g_paraSetBufIndex+=10;
	}
	else
	{
		//cml += 5;
		g_paraSetBufIndex+=5;
	}

	ps = pStart;
	while(cnt<5)
	{
		if(*ps=='.')
		{
			LoadCanParaChar(*ps,g_paraSetBuf+g_paraSetBufIndex,DSP_PARA_1LINE,rsv);
			g_paraSetBufIndex+=4;
			if(CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *(ps+1))>5)
			{
				g_paraSetBufIndex+=10;
			}else
			{
				g_paraSetBufIndex+=11;
			}
			ps++;
			cnt++;
		}else
		{
		
			while(*ps!='.')
			{
				LoadCanParaChar(*ps,g_paraSetBuf+g_paraSetBufIndex,DSP_PARA_1LINE,rsv);
				tmp = CanCharLen((CanFont *)canFontPara, FONT_PARA_LEN, *ps);
				if(tmp>5)
				{
					g_paraSetBufIndex+=tmp;
					i++;
				}else
				{
					if(*ps==':')
					{
						g_paraSetBufIndex+=4;
					}else
					{
						g_paraSetBufIndex+=5;
					}
				}				
				ps++;
			}
			cnt++;
			g_paraSetBufIndex+=11;
			g_paraSetBufIndex-=i;
		}
	}

	for(i=0;i<4;i++)
	{
		if(DSP_NORMAL==rsv)
		{
			g_paraSetBuf[DSP_PARA_1LINE+dotSet+i] |= 0x1E;
		}else
		{
			g_paraSetBuf[DSP_PARA_1LINE+dotSet+i] &= ~0x1E;
		}
	}

	AreaSet(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA, DSP_PARA_1LINE, 16);
	for(i=0;i<(DSP_PARA_1LINE*2);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	//LcdClearFeild(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA, 24,AUX_FONT_WIDTH(17));
	//LcdClearFeild(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,AUX_FONT_WIDTH(17), 24);
	//LcdPutBuf(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(3),pStart,FONT_TYPE_MAST,ABr_DSP_LEN,0,rsv);
	//LcdFillBlank(LAYOUT_RATIO_A+AUX_FONT_HIGHT(1),LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(3),AUX_FONT_WIDTH(GR_DSP_LEN),LAYOUT_DOTAREA_HT,(rsv==DSP_SELE)?DSP_SELE:DSP_REVERSE);
	//DisplayMstSetDot(LAYOUT_RATIO_A+AUX_FONT_HIGHT(1),LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(dspSt-dsHead+3),rsv);
	
}

void DisplayMstOtp(u16 page,u16 cml,u8 mod,unsigned char para1,unsigned char para2,unsigned char rvs)
{
	//if(DSP_STYLE_CANON==g_dspStyle)
	//{
		DisplayMstOtp_canStyle(page,cml,mod,DSP_PARA_1LINE,DSP_BLOCK_LEN,
			para1,para2,5,(WORKINGMODE_MULTI==mod)?(char*)multi_dotSetLoc:(char*)m_dotSetLoc,
			0,0,
			rvs);
	//}else if(DSP_STYLE_CLASSIC==g_dspStyle)
	//{
	//	DisplayMstOtp_classStyle(page,cml,para1,para2,rvs);
	//}
}

void DisplayOutput(u16 pa, u16 cml,u8 mod,uchar otp,uchar otpOffset,uchar dspRsv)
{
	//if(DSP_STYLE_CANON==g_dspStyle)
	//{
		DisplayMstOtp_canStyle(pa,cml,mod,DSP_PARA_1LINE,DSP_BLOCK_LEN,otp,otpOffset,
								5,(WORKINGMODE_MULTI==mod)?(char*)multi_dotSetLoc:(char*)m_dotSetLoc,
								0,0,
								dspRsv);
	//}else if(DSP_STYLE_CLASSIC==g_dspStyle)
	//{
	//	DisplayMstOtp_classStyle(pa,cml,otp,otpOffset,dspRsv);
	//}
}

void DspMstModIcon(u8 mod)
{
	
	switch(mod)
	{
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
			//ENABLE_CHARGE();
#if 1
			DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_HT,liteBitmap,DSP_NORMAL);
#endif
		break;

		//case CMD_MODE_RF_MST:
		case CMD_MODE_RFLS_MST:
		//case CMD_MODE_RF_SLA:
		case CMD_MODE_RFLS_SLA:
			//if(pfnSet[PFN_ID_LSSW]>0)
			//{
			//	ENABLE_CHARGE();
			//}else
			//{
			//	DISABLE_CHARGE();
			//}
#if 1
			DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_HT,rfBitmap,DSP_NORMAL);
#endif
		break;

		case CMD_MODE_RF_MST:
			//if(*wlSet.is_masterOn>0)
			//{
			//	ENABLE_CHARGE();
			//}else
			//{
			//	DISABLE_CHARGE();
			//}
			DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_HT,rfBitmap,DSP_NORMAL);
		break;

		case CMD_MODE_RF_SLA:
			//ENABLE_CHARGE();
			DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_HT,rfBitmap,DSP_NORMAL);
		break;

		case CMD_MODE_OFF:
			//ENABLE_CHARGE();
			LcdClearFeild(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_HT);
		break;
	}
}

void TakeOnCharge(u8 mod)
{
	switch(mod)
	{
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
			//ENABLE_CHARGE();
			SetChargeEnable();
		break;

		//case CMD_MODE_RF_MST:
		case CMD_MODE_RFLS_MST:
		//case CMD_MODE_RF_SLA:
		case CMD_MODE_RFLS_SLA:
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				//ENABLE_CHARGE();
				SetChargeEnable();
			}else
			{
				//DISABLE_CHARGE();
				SetChargeDisable();
			}
		break;

		case CMD_MODE_RF_MST:
			if(WORKINGMODE_GR==mainPara.workMode)
			{
				if(radioSet->rfGr[0].isOff>0)
				{
					SetChargeDisable();
				}else
				{
					SetChargeEnable();
				}	
			}else
			{
				if(*wlSet.is_masterOn>0)
				{
					//ENABLE_CHARGE();
					SetChargeEnable();
				}else
				{
					//DISABLE_CHARGE();
					SetChargeDisable();
				}
			}
		break;

		case CMD_MODE_RF_SLA:
			//ENABLE_CHARGE();
			SetChargeEnable();
		break;

		case CMD_MODE_OFF:
			//ENABLE_CHARGE();
			SetChargeEnable();
		break;
	}
}

void SetMstHead(char *arr)
{
}

void SetLinkshotChan(char page,char cml, char ch, char rvs)
{
	u8 i=0;
	
	g_paraSetBufIndex = 0;
	
	memset(g_paraSetBuf,0x00,27);
	

	if(!ch)
	{
		g_paraSetBufIndex+=1;
		memcpy(g_paraSetBuf+g_paraSetBufIndex,font_sAuto,25);
	}else
	{
		memcpy(g_paraSetBuf,font_chdot,11);
		g_paraSetBufIndex+=11;
		if(ch>9)
		{
			g_paraSetBufIndex+=2;//(27-11-12)/2
			memcpy(g_paraSetBuf+g_paraSetBufIndex,cfnNum1+(ch/10)*6,6);
			g_paraSetBufIndex+=7;
			memcpy(g_paraSetBuf+g_paraSetBufIndex,cfnNum1+(ch%10)*6,6);
		}
		else
		{
			g_paraSetBufIndex+=5;//(27-11-6)/2;
			memcpy(g_paraSetBuf+g_paraSetBufIndex,cfnNum1+ch*6,6);
		}
		
	}

	if(DSP_REVERSE==rvs)
	{
		for(i=0;i<27;i++)
		{
			g_paraSetBuf[i] = ~g_paraSetBuf[i];
		}
	}

	LcdCopyToRam(page,cml,g_paraSetBuf,27,8);

}

//void SetMstChan(char ch,char rvc)
void SetMstChan(char page,char cml,char ch,char rvc,char frame)
{
	u8 i=0;
	if(DSP_NORMAL==rvc)
	{
		memset(g_paraSetBuf,0x00,LAYOUT_CHAN_LEN*2);
	}else
	{
		memset(g_paraSetBuf,0x7F,LAYOUT_CHAN_LEN);
		memset(g_paraSetBuf+LAYOUT_CHAN_LEN,0xFE,LAYOUT_CHAN_LEN);
	}

	g_paraSetBufIndex = 0;

	if(!ch)
	{
		for(i=0;i<LAYOUT_CHAN_LEN;i++)
		{
			g_paraSetBuf[i+g_paraSetBufIndex] = (DSP_NORMAL==rvc)?chAuto[i]:(~chAuto[i]&0x7f);
			g_paraSetBuf[i+LAYOUT_CHAN_LEN+g_paraSetBufIndex] = (DSP_NORMAL==rvc)?chAuto[i+LAYOUT_CHAN_LEN]:(~chAuto[i+LAYOUT_CHAN_LEN]&0xfe);
		}
		
	}else
	{
		g_paraSetBufIndex+=1;
		LoadCmpFebChar('c',g_paraSetBuf+g_paraSetBufIndex,LAYOUT_CHAN_LEN,rvc);
		g_paraSetBufIndex+=4;
		LoadCmpFebChar('h',g_paraSetBuf+g_paraSetBufIndex,LAYOUT_CHAN_LEN,rvc);
		g_paraSetBufIndex+=5;
		LoadCmpFebChar('.',g_paraSetBuf+g_paraSetBufIndex,LAYOUT_CHAN_LEN,rvc);
		g_paraSetBufIndex+=3;

		if(ch<10)
		{
			g_paraSetBufIndex += 3;
			LoadCmpFebChar(ch+'0',g_paraSetBuf+g_paraSetBufIndex,LAYOUT_CHAN_LEN,rvc);
			//g_paraSetBufIndex+=7;
		}else
		{
			LoadCmpFebChar(ch/10+'0',g_paraSetBuf+g_paraSetBufIndex,LAYOUT_CHAN_LEN,rvc);
			g_paraSetBufIndex+=7;
			LoadCmpFebChar(ch%10+'0',g_paraSetBuf+g_paraSetBufIndex,LAYOUT_CHAN_LEN,rvc);
		}		
		
	}

	
	if(frame>0)
	{
		for(i=1;i<LAYOUT_CHAN_LEN-1;i++)
		{
			g_paraSetBuf[i] |= 0x80;
			g_paraSetBuf[i+LAYOUT_CHAN_LEN] |= 0x01;
		}
		g_paraSetBuf[0] = g_paraSetBuf[LAYOUT_CHAN_LEN]  = 0xFF;
		g_paraSetBuf[LAYOUT_CHAN_LEN-1] = g_paraSetBuf[2*LAYOUT_CHAN_LEN-1]  = 0xFF;
	}else
	{
		//LcdCopyToRam(page,cml, g_paraSetBuf+1, LAYOUT_CHAN_LEN-2,16);
		g_paraSetBuf[0] = g_paraSetBuf[LAYOUT_CHAN_LEN]  = 0;
		g_paraSetBuf[LAYOUT_CHAN_LEN-1] = g_paraSetBuf[2*LAYOUT_CHAN_LEN-1]  = 0;
	}
	LcdCopyToRam(page,cml, g_paraSetBuf, LAYOUT_CHAN_LEN,16);
	//AreaSet(LAYOUT_PARA_AREA0, LAYOUT_CHAN_CML, LAYOUT_CHAN_LEN, 16);

	//for(i=0;i<LAYOUT_CHAN_LEN*2;i++)
	//{
	//	LCD_WRITE_BYTE(g_paraSetBuf[i]);
	//}

	
#if 0
	LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,rvc);
	LcdClearFeild(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), BIG_FONT_WIDTH(2), BIG_FONT_HIGHT(1));

	if(!ch)
	{
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), 'A', FONT_TYPE_BIG,rvc);
	}else if(ch<10)
	{
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), ch+'0', FONT_TYPE_BIG,rvc);
	}else
	{
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), ch/10+'0', FONT_TYPE_BIG,rvc);
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3)+BIG_FONT_WIDTH(1), ch%10+'0', FONT_TYPE_BIG,rvc);
	}
	//LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-16,24,18+10);	
#endif
}

void DisplayMstSetting(char arr, unsigned char mode, char para1,char para2)
{
}



void DisplayCanMstHead(unsigned char sel,char mode,WlSet *wlPara,char rvs)
{
// Note: the parameter sel is not the sel index, it indicates the master setting;
//1: Ratio 2: A 3: B 4: C 5: chan
//	char dsp = 0;
//	if(1==sel)	dsp = 1;			//this determined the selected option, if it is, it should be highlight
	signed char cmp1=0,cmp2=0;
	
	LCD_CANMST_CLEAR;
	if(WLM_MSTFLA_ON==*(wlPara->is_masterOn))
	{
		LCD_MSTFLA_ON;
	}else
	{
		LCD_MSTFLA_OFF;
	}

/*
	if(5==sel)
	{
		SetMstChan(*wlPara->m_chan,DSP_SELE);
	}else
	{
		SetMstChan(*wlPara->m_chan,DSP_NORMAL);
	}
*/

	/*Clear field at every time when parameter changed*/
	//LcdGrAllClear();
	
	switch(mode)
	{
		case WORKINGMODE_TTL:
			//LCD_CLEAR_GR(LAYOUT_RATIO_A);
			//LCD_CLEAR_GR(LAYOUT_RATIO_A);
			if((wlPara->m_arrPara[WL_ARRARY_A].cmp1||wlPara->m_arrPara[WL_ARRARY_A].cmp2))
			{
				cmp1 = wlPara->m_arrPara[WL_ARRARY_A].cmp1;
				cmp2 = wlPara->m_arrPara[WL_ARRARY_A].cmp2;
			}else if(wlPara->cmrCmp1||wlPara->cmrCmp2)
			{
				cmp1 = wlPara->cmrCmp1;
				cmp2 = wlPara->cmrCmp2;
				
			}
			switch(*wlPara->ttl_Ratio)
			{
				case WLM_MODE_ALL:
				LCD_MSTALL_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/								
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("ALL")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2,((2==sel)?DSP_SELE:DSP_NORMAL));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2,((0==sel)&&(rvs&DSP_SUB_SELE)?DSP_SELE:DSP_NORMAL));
				//DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2,((0==sel)&&(rvs&DSP_SUB_SELE)?DSP_SELE:DSP_NORMAL));
				//DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2,((0==sel)&&(rvs&DSP_SUBADD)?DSP_SELE:DSP_NORMAL));
				DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,cmp1,cmp2,((0==sel)&&(rvs&DSP_SUBADD)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_AB:
				LCD_MSTAB_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A:B")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2);
				DisplayABRation(wlPara->m_arrPara[WL_ARRARY_B].cmp1,wlPara->m_arrPara[WL_ARRARY_B].cmp2,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_RES:
				LCD_MSTRES_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A:B")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"C",((4==sel)?DSP_SELE:DSP_NORMAL),(sizeof("C")-1)*12);
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"C",(((1==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"C",(((1==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DisplayABRation(wlPara->m_arrPara[WL_ARRARY_B].cmp1, wlPara->m_arrPara[WL_ARRARY_B].cmp2,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayTtlCmp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_C].cmp1,wlPara->m_arrPara[WL_ARRARY_C].cmp2,((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL);
				break;
			}
		break;

		case WORKINGMODE_MANU:
			switch(*wlPara->arr_mode)
			{
				case WLM_MODE_ALL:
				LCD_MSTALL_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("ALL")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_A].output1,wlPara->m_arrPara[WL_ARRARY_A].output2,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_AB:
				LCD_MSTAB_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_A].output1,wlPara->m_arrPara[WL_ARRARY_A].output2,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);	
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);	
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_B].output1,wlPara->m_arrPara[WL_ARRARY_B].output2,(((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_RES:
				LCD_MSTABC_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_A].output1,wlPara->m_arrPara[WL_ARRARY_A].output2,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);	
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);	
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_B].output1,wlPara->m_arrPara[WL_ARRARY_B].output2,(((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				//if(4==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				//DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",((4==sel)?DSP_SELE:DSP_NORMAL),(sizeof("C")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",(((2==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",(((2==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DisplayMstOtp(LAYOUT_RATIO_C,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_C].output1,wlPara->m_arrPara[WL_ARRARY_C].output2,(((2==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;
			}
		break;

		case WORKINGMODE_MULTI:
			switch(*wlPara->arr_mode)
			{
				case WLM_MODE_ALL:
				LCD_MSTALL_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("ALL")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp>>4,0,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp,0,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_AB:
				LCD_MSTAB_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp>>4,0,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp,0,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = 0;
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);		
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);		
				//DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_B].multi_exp>>4,0,(((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_B].multi_exp,0,(((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_RES:
				LCD_MSTABC_ON(rvs);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",(((0==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp>>4,0,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp,0,(((0==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				//DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);	
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",(((1==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);	
				//DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_B].multi_exp>>4,0,(((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_B].multi_exp,0,(((1==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				//if(4==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				//DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",((4==sel)?DSP_SELE:DSP_NORMAL),(sizeof("C")-1)*BIG_FONT_WIDTH(1));
				//DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",(((2==sel)&&(rvs>=DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",(((2==sel)&&(rvs&DSP_SELE))?DSP_SELE:DSP_NORMAL),LAYOUT_RATIO_WIDTH);
				//DisplayMstOtp(LAYOUT_RATIO_C,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_C].multi_exp>>4,0,(((2==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				DisplayMstOtp(LAYOUT_RATIO_C,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_C].multi_exp,0,(((2==sel)&&(DSP_SUB_SELE&rvs))?DSP_SELE:DSP_NORMAL));
				break;
			}
		break;

		case WORKINGMODE_GR:
			switch(*wlPara->arr_mode)
			{
				case WLM_MODE_ALL:
				break;

				case WLM_MODE_AB:
				break;

				case WLM_MODE_RES:
				break;
			}
		break;
	}
	
}

//void DspSlavePara(u8 rvs)
void DspSlavePara(char mod, char para1, char para2, char arr, char rvs)
{
	char sTemp[2]={0,0};
	//if((mainPara.s_array>WL_ARRARY_C)&&(CMD_MODE_LIGH_SLA==*g_cmdMode))
	//{
	//	mainPara.s_array = WL_ARRARY_A;
	//}

	//sTemp[0] = mainPara.s_array+'A';
	sTemp[0] = arr + 'A';
	
	DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,sTemp,DSP_NORMAL,LAYOUT_RATIO_WIDTH);

	//switch(mainPara.workMode)
	switch(mod)
	{
		case WORKINGMODE_TTL:
			//DisplayTtlCmp(LAYOUT_RATIO_B, LAYOUT_RATIO_STPARA,mstPara->cmp1,mstPara->cmp2,rvs);
			DisplayTtlCmp(LAYOUT_RATIO_B, LAYOUT_RATIO_STPARA,para1,para2,rvs);
		break;

		case WORKINGMODE_MANU:
			//DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,mstPara->output1,mstPara->output2,rvs);
			DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,para1,para2,rvs);
		break;

		case WORKINGMODE_MULTI:
			//DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,mstPara->multi_exp>>4,0,rvs);
			DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,para1,para2,rvs);
		break;
	}	
	
}

void DspIndividualSlavePara(char rvs)
{
	switch(mainPara.workMode)
	{
		case WORKINGMODE_MANU:
			//DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,mstPara->output1,mstPara->output2,rvs);
			DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,mstPara->output1,mstPara->output2,rvs);
		break;

		case WORKINGMODE_MULTI:
			//DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,mstPara->multi_exp>>4,0,rvs);
			DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,*multiSet.expose,0,rvs);
		break;

		default:
		break;
	}
}

void DspLightSlavePara(char rvs)
{
	//if(mainPara.s_array>WL_ARRARY_C)
	//{
	//	mainPara.s_array = WL_ARRARY_A;
	//}

	if(*wlSlave.s_array>WL_ARRARY_C)
	{
		*wlSlave.s_array = WL_ARRARY_A;
	}

	if(g_expTTLPara3>0)
	{
		DisplayPicture(LAYOUT_PARA_AREA0,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFp,DSP_NORMAL);
	}
	
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			//DspSlavePara(WORKINGMODE_TTL,mstPara->cmp1,mstPara->cmp2,mainPara.s_array,rvs);
			DspSlavePara(WORKINGMODE_TTL,mstPara->cmp1,mstPara->cmp2,*wlSlave.s_array,rvs);
		break;

		case WORKINGMODE_MANU:
			//DspSlavePara(WORKINGMODE_MANU,canWireless.WL_output1,canWireless.WL_output2,mainPara.s_array,rvs);
			DspSlavePara(WORKINGMODE_MANU,canWireless.WL_output1,canWireless.WL_output2,*wlSlave.s_array,rvs);
		break;

		case WORKINGMODE_MULTI:
			//DspSlavePara(WORKINGMODE_MULTI,canWireless.wlMul_expose,0,mainPara.s_array,rvs);
			DspSlavePara(WORKINGMODE_MULTI,canWireless.wlMul_exp1,0,*wlSlave.s_array,rvs);
			//SetMultiTmFrq(*multiSet.eTimes,canWireless.wlMul_frq,canWireless.wlMul_time);
			//SetMultiTmFrq(canWireless.wlMul_time,canWireless.wlMul_frq,DSP_NORMAL);
			SetMultiTmFrq(0,canWireless.wlMul_frq,DSP_NORMAL);
			//SetMultiTmFrq(canWireless.wlMul_time,canWireless.wlMul_frq,DSP_NORMAL);
		break;

		case WORKINGMODE_EXTA:
			DspSlavePara(WORKINGMODE_TTL,0,0,*wlSlave.s_array,rvs);
		break;
	}
}

//extern u8 slvTmsDebug;
void DspRfSlavePara(char rvs)
{

/*	if(g_expTTLPara3>0)
	{
		DisplayPicture(LAYOUT_PARA_AREA0,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFp,DSP_NORMAL);
	}
*/

	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			//DspSlavePara(WORKINGMODE_TTL,mstPara->cmp1,mstPara->cmp2,mainPara.s_array,rvs);
			DspSlavePara(WORKINGMODE_TTL,mstPara->cmp1,mstPara->cmp2,*wlSlave.s_array,rvs);
			if(g_expTTLPara3>0)
			{
				DisplayPicture(LAYOUT_PARA_AREA0,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFp,DSP_NORMAL);
			}
#ifdef TTL_DEBUG	
			//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,g_rfSlaveFlashCode,DSP_NORMAL);
#endif
		break;

		case WORKINGMODE_MANU:
			//DspSlavePara(WORKINGMODE_MANU,canWireless.WL_output1,canWireless.WL_output2,mainPara.s_array,rvs);
			DspSlavePara(WORKINGMODE_MANU,canWireless.WL_output1,canWireless.WL_output2,*wlSlave.s_array,rvs);
			if(g_expTTLPara3>0)
			{
				DisplayPicture(LAYOUT_PARA_AREA0,LAYOUT_SYNMODE_LINE,FONT_SYN_WT,FONT_SYN_HT,synFp,DSP_NORMAL);
			}
		break;

		case WORKINGMODE_MULTI:
			//DspSlavePara(WORKINGMODE_MULTI,canWireless.wlMul_expose,0,mainPara.s_array,rvs);
			DspSlavePara(WORKINGMODE_MULTI,canWireless.wlMul_exp1,0,*wlSlave.s_array,rvs);
			//SetMultiTmFrq(canWireless.wlMul_time,canWireless.wlMul_frq,DSP_NORMAL);
			SetMultiTmFrq(0,canWireless.wlMul_frq,DSP_NORMAL);
			//SetMultiTmFrq(slvTmsDebug,canWireless.wlMul_frq,DSP_NORMAL);
		break;

		case WORKINGMODE_EXTA:
			DspSlavePara(WORKINGMODE_TTL,0,0,*wlSlave.s_array,rvs);
		break;
	}
}

void DspFnIcon(u8 x, u8 y)
{
	u8 i,rt=0;
	rt= 0;
	for(i=0;i<18;i++)
	{
		if(pSnrSet[i]>0)
		{
			rt = 1;
			break;
		}
	}
	if((WORKINGMODE_EXTA==mainPara.workMode)||(WORKINGMODE_EXTM==mainPara.workMode))
	{
		rt = 1;
	}
	if(rt>0)
	{
		//DisplayPicture(x,y,21,8,pfnBitMap,DSP_REVERSE);
		DisplayPicture(x,y+21+1,21,8,cfnBitMap,DSP_REVERSE);
	}
	rt = 0;
	if(2!=pfnSet[PFN_ID_CTS])
	{
		rt = 1;
	}else
	{
		for(i=1;i<7;i++)
		{
			if(pfnSet[i]>0)
			{
				rt = 1; 
				break;
			}
		}
	}
	if(rt>0)
	{
		//DisplayPicture(x,y+21+1,21,8,cfnBitMap,DSP_REVERSE);
		DisplayPicture(x,y,21,8,pfnBitMap,DSP_REVERSE);
	}
}

void FuncDsp(void)
{

	switch(*g_cmdMode)
	{
		case CMD_MODE_OFF:
			if((!g_isRgbFilter)&&(!g_rgbStyle)&&(!pfnSet[PFN_ID_DETEC]))
			{
				DspSensorRgbWarn(LAYOUT_SENSOR_PAGE_EX, LAYOUT_SENSOR_CML_EX);
			}else
			{
				DspSensorsEx(LAYOUT_SENSOR_PAGE_EX, LAYOUT_SENSOR_CML_EX, 1, !g_isRgbFilter,g_upDnSensor,0);
			}
			if(!g_menuLock)
			{
				//DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML,21,8,pfnBitMap,DSP_REVERSE);
				//DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML+21+1,21,8,cfnBitMap,DSP_REVERSE);
				DspFnIcon(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML);
			}
			//if(((SENSOR_UPDN_UP==g_upDnSensor)&&(REF_BOARD_DETECT>0))||(g_moto_error>0))
			if(((SENSOR_UPDN_UP==g_upDnSensor)&&(g_isReflectPulled>0))||(g_moto_error>0))
			{
				DspMotoWarn();
			}else
			{
				DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
				//g_ref_flag=!REF_BOARD_DETECT;
				//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
			}
		break;

		case CMD_MODE_RF_MST:
		case CMD_MODE_LIGH_MST:
			if((!g_isRgbFilter)&&(!g_rgbStyle)&&(!pfnSet[PFN_ID_DETEC]))
			{
				DspSensorRgbWarn(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML);
			}else
			{
				DspSensors(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML,mainPara.m_isMstFla,!g_isRgbFilter,g_upDnSensor,1);
			}
			//DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1,21,8,pfnBitMap,DSP_REVERSE);
			//DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1+21+1,21,8,cfnBitMap,DSP_REVERSE);
			DspFnIcon(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1);
		//	if(((SENSOR_UPDN_UP==g_upDnSensor)&&(REF_BOARD_DETECT>0))||(g_moto_error>0))
			if(((SENSOR_UPDN_UP==g_upDnSensor)&&(g_isReflectPulled>0))||(g_moto_error>0))
			{
				DspMotoWarn();
			}else
			{
				DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
				//g_ref_flag=!REF_BOARD_DETECT;
				//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
			}
		break;

		case CMD_MODE_RF_SLA:
		case CMD_MODE_LIGH_SLA:
			//DspSensors(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML,1,0,0,2);
			if((!g_isRgbFilter)&&(!g_rgbStyle)&&(!pfnSet[PFN_ID_DETEC]))
			{
				DspSensorRgbWarn(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML);
			}else
			{
				DspSensors(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML,1,!g_isRgbFilter,g_upDnSensor,2);
			}
			//DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1,21,8,pfnBitMap,DSP_REVERSE);
			//DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1+21+1,21,8,cfnBitMap,DSP_REVERSE);
			DspFnIcon(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1);
			//if(((SENSOR_UPDN_UP==g_upDnSensor)&&(REF_BOARD_DETECT>0))||(g_moto_error>0))
			if(((SENSOR_UPDN_UP==g_upDnSensor)&&(g_isReflectPulled>0))||(g_moto_error>0))
			{
				DspMotoWarn();
			}else
			{
				DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
				//g_ref_flag=!REF_BOARD_DETECT;
				//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
			}
		break;

		case CMD_MODE_RFLS_SLA:
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				if((!g_isRgbFilter)&&(!g_rgbStyle)&&(!pfnSet[PFN_ID_DETEC]))
				{
					DspSensorRgbWarn(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML);
				}else
				{
					DspSensors(LAYOUT_SENSOR_PAGE_EX, LAYOUT_SENSOR_CML_EX,1,!g_isRgbFilter,g_upDnSensor,2);
				}
			//	if(((SENSOR_UPDN_UP==g_upDnSensor)&&(REF_BOARD_DETECT>0))||(g_moto_error>0))
				if(((SENSOR_UPDN_UP==g_upDnSensor)&&(g_isReflectPulled>0))||(g_moto_error>0))
				{
					DspMotoWarn();
				}else
				{
					DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
					//g_ref_flag=!REF_BOARD_DETECT;
					//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
				}
			}/*else
			{
				DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML,21,8,pfnBitMap,DSP_REVERSE);
				DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML+21+1,21,8,cfnBitMap,DSP_REVERSE);
			}*/
			DspFnIcon(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML);
		break;
		
		case CMD_MODE_RFLS_MST:
			if(pfnSet[PFN_ID_LSSW]>0)
			{
				if((!g_isRgbFilter)&&(!g_rgbStyle)&&(!pfnSet[PFN_ID_DETEC]))
				{
					DspSensorRgbWarn(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML);
				}else
				{
					DspSensors(LAYOUT_SENSOR_PAGE_EX, LAYOUT_SENSOR_CML_EX,1,!g_isRgbFilter,g_upDnSensor,1);
				}
				//if(((SENSOR_UPDN_UP==g_upDnSensor)&&(REF_BOARD_DETECT>0))||(g_moto_error>0))
				if(((SENSOR_UPDN_UP==g_upDnSensor)&&(g_isReflectPulled>0))||(g_moto_error>0))
				{
					DspMotoWarn();
				}else
				{
					DisplayFocLen(*motoSet.mode,*motoSet.dest_location,SETTING_MOTO==g_SettingIndex);
					//g_ref_flag=!REF_BOARD_DETECT;
					//DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_NORMAL);
				}
			}/*else
			{
				DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML,21,8,pfnBitMap,DSP_REVERSE);
				DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML+21+1,21,8,cfnBitMap,DSP_REVERSE);
			}*/
			DspFnIcon(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML);
		break;

	}
	
/*
	if(CMD_MODE_OFF==*g_cmdMode)
	{
		DspSensorsEx(LAYOUT_SENSOR_PAGE_EX, LAYOUT_SENSOR_CML_EX, 1, 1, 1,0);
		DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML,21,8,pfnBitMap,DSP_REVERSE);
		DisplayPicture(LAYOUT_PARA_AREA3+1,LAYOUT_PFN_CML+21+1,21,8,cfnBitMap,DSP_REVERSE);
	}else 
	{
		DspSensors(LAYOUT_SENSOR_PAGE,LAYOUT_SENSOR_CML,mainPara.m_isMstFla,0,0,1);
		DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1,21,8,pfnBitMap,DSP_REVERSE);
		DisplayPicture(LAYOUT_PARA_AREA3,LAYOUT_FUNC_MST+42+1+21+1,21,8,cfnBitMap,DSP_REVERSE);
	}
*/

	if((CMD_MODE_OFF==*g_cmdMode)||
		((CMD_MODE_RFLS_MST==*g_cmdMode)&&(pfnSet[PFN_ID_LSSW]>0))||
		((CMD_MODE_RFLS_SLA==*g_cmdMode)&&(pfnSet[PFN_ID_LSSW]>0)))
	{

		if(MOTO_MODE_AUTO==*motoSet.mode)
		{
			if((g_isFullFrame>0)&&(!pSnrSet[SNRMENU_SCREEN]))	
			{
				LoadUncompetibleIcon();
			}
		}
		
		switch(mainPara.workMode)
		{
			case WORKINGMODE_TTL:
			case WORKINGMODE_GR:
				DrawScale(mainPara.workMode,flaPara.shutter,(SYNC_MODE_FP==mainPara.synMode)&&(flaPara.shutter>0x75),
							0,g_cmrPressed);
			break;

			case WORKINGMODE_MANU:
				DrawScale(mainPara.workMode,flaPara.shutter,(SYNC_MODE_FP==mainPara.synMode)&&(flaPara.shutter>0x75),
							(mstPara->output1)*3+mstPara->output2,g_cmrPressed);
			break;

			case WORKINGMODE_MULTI:
				DrawScale(mainPara.workMode,flaPara.shutter,0,	*multiSet.expose*3,g_cmrPressed);
			break;

			case WORKINGMODE_EXTA:
				DrawScale(mainPara.workMode,0,0,0,g_cmrPressed);
			break;

			case WORKINGMODE_EXTM:
				DrawScale(mainPara.workMode,flaPara.shutter,0,0,1);
			break;
		}

		//if(g_cmrLinked>0)
		if(WORKINGMODE_EXTM!=mainPara.workMode)
		{
			if(g_cmrPressed>0)
			{
				DspApertValue(flaPara.apert,DSP_NORMAL);
			}else
			{
				LcdClearFeild(7, 141, 27, 16);
			}	
		}


#if 0
		if(WORKINGMODE_EXTM==mainPara.workMode)
		{
			DrawScale(mainPara.workMode,flaPara.shutter,0,0,1);
		}else if(WORKINGMODE_MANU==mainPara.workMode)
		{
			//DrawScale(mainPara.workMode,(mstPara->output1-1)*3+mstPara->output2,g_cmrLinked);
			//DrawScale(mainPara.workMode,(mstPara->output1)*3+mstPara->output2,g_cmrPressed);
			/*
			if((flaPara.shutter>0x75)&&(SYNC_MODE_FP==mainPara.synMode))
			{
				DrawScaleHp(mainPara.workMode, flaPara.shutter, (mstPara->output1)*3+mstPara->output2,,g_cmrPressed);
			}else
			{
				DrawScale(mainPara.workMode,(mstPara->output1)*3+mstPara->output2,g_cmrPressed);
			}*/
			DrawScale(mainPara.workMode,flaPara.shutter,(SYNC_MODE_FP==mainPara.synMode)&&(flaPara.shutter>0x75),
			(mstPara->output1)*3+mstPara->output2,g_cmrPressed);
		}else if(WORKINGMODE_MULTI==mainPara.workMode)
		{
			//DrawScale(mainPara.workMode,((*multiSet.expose>>4)-1)*3,g_cmrLinked);
			//DrawScale(mainPara.workMode,((*multiSet.expose>>4)*3,g_cmrPressed);
			DrawScale(mainPara.workMode,*multiSet.expose*3,g_cmrPressed);
		}else
		{
			//DrawScale(mainPara.workMode,0,g_cmrLinked);
			DrawScale(mainPara.workMode,0,g_cmrPressed);
		}
#endif
	}
	
	if(pSnrSet[SNRMENU_VOICE])
	{
		DisplayPicture(LAYOUT_SPEAKER_PAGE,LAYOUT_SPEAKER_CML,FONT_SPEAKER_WT,FONT_SPEAKER_HT,font_speak,DSP_NORMAL);
	}
}

void DisplayRmt(char arr, char chan,char sel)
{
}

void DspSensors(char page, char cml, char fla, char rgbFilter,char upDn,char mstSla)
{
	u8 i=0;
	memset(g_paraSetBuf,0x00,SENSOR_DSP_LEN*3);
	g_paraSetBufIndex=0;
	
	if(fla>0)
	{
		if((!rgbFilter)&&(!upDn))
		{
			g_paraSetBufIndex += 4;
		}
		
		for(i=0;i<5;i++)
		{
			g_paraSetBuf[i+g_paraSetBufIndex]|=sensor_flaOn[i+(pSnrSet[SNRMENU_ANGLE]*15)];
			g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN] |= sensor_flaOn[i+(pSnrSet[SNRMENU_ANGLE]*15)+5];
			g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_flaOn[i+(pSnrSet[SNRMENU_ANGLE]*15)+5*2];
		}

	}else
	{
		//g_paraSetBufIndex+=4;
	}

	g_paraSetBufIndex+=5;

	if((upDn>0)||(rgbFilter>0))
	{
		if(upDn>0)
		{
			if(SENSOR_UPDN_DOWN==upDn)
			{
				for(i=0;i<7;i++)
				{
					g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_down[i];
				}
			}else
			{
				for(i=0;i<7;i++)
				{
					g_paraSetBuf[i+g_paraSetBufIndex] |= sensor_up[i];
				}
			}
			
		}

		g_paraSetBufIndex+=2;

		if(rgbFilter>0)
		{
			//g_paraSetBufIndex+=2;
			g_paraSetBuf[SENSOR_DSP_LEN+g_paraSetBufIndex] |= 0xFF;
			g_paraSetBuf[SENSOR_DSP_LEN+g_paraSetBufIndex+1] |= 0xFF;
		}

		g_paraSetBufIndex+=4;
	}else
	{
		if(!fla)
		{
			g_paraSetBufIndex+=4;
		}
		g_paraSetBufIndex+=2;
	}

	for(i=0;i<17;i++)
	{
		g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN] |= sensor_head[i];
		g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_head[i+17];
	}

	g_paraSetBufIndex+=1;

	if(mstSla>0)
	{
		if(SENSOR_MS_MST==mstSla)
		{
			for(i=0;i<10;i++)
			{
				g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_mst[i];
			}
		}else
		{
			for(i=0;i<10;i++)
			{
				g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_sla[i];
			}
		}
	}	

	if(g_isHotProtect>0)
	{
		for(i=0;i<5;i++)
		{
			g_paraSetBuf[i+15] |= hot_warn[i];
		}
	}

	AreaSet(page,cml,SENSOR_DSP_LEN,24);

	for(i=0;i<SENSOR_DSP_LEN*3;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	
}

void AreaScroll(char *sBuf, char wt,char ht,char aw,char dir,char n)
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

void DspSensorRgbWarn(char page, char cml)
{
	u8 i=0;
	
	AreaSet(page,cml,SENSOR_DSP_LEN,24);

	for(i=0;i<SENSOR_DSP_LEN*3;i++)
	{
		LCD_WRITE_BYTE(font_rgbWarn[i]);
	}
}


void DspSensorsEx(char page, char cml, char fla, char rgbFilter,char upDn,char mstSla)
{

	u8 i=0;
	memset(g_paraSetBuf,0x00,SENSOR_DSP_LEN*3);
	g_paraSetBufIndex=0;
	
	if(fla>0)
	{
		if((!rgbFilter)&&(!upDn))
		{
			g_paraSetBufIndex += 4;
		}
		
		for(i=0;i<5;i++)
		{
			g_paraSetBuf[i+g_paraSetBufIndex]|=sensor_flaOn_1[i+(pSnrSet[SNRMENU_ANGLE]*15)];
			g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN] |= sensor_flaOn_1[i+(pSnrSet[SNRMENU_ANGLE]*15)+5];
			g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_flaOn_1[i+(pSnrSet[SNRMENU_ANGLE]*15)+5*2];
		}

	}

	g_paraSetBufIndex+=4;
	g_paraSetBufIndex+=1;

	if((upDn>0)||(rgbFilter>0))
	{
		if(upDn>0)
		{
			//g_paraSetBufIndex+=1;
			
			if(SENSOR_UPDN_DOWN==upDn)
			{
				for(i=0;i<7;i++)
				{
					g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_down[i];
				}
			}else
			{
				for(i=0;i<7;i++)
				{
					g_paraSetBuf[i+g_paraSetBufIndex] |= sensor_up[i];
				}
			}

			//g_paraSetBufIndex+=2;
			
		}

		g_paraSetBufIndex+=2;

		if(rgbFilter>0)
		{
			//g_paraSetBufIndex+=2;
			g_paraSetBuf[SENSOR_DSP_LEN+g_paraSetBufIndex] |= 0xFF;
			g_paraSetBuf[SENSOR_DSP_LEN+g_paraSetBufIndex+1] |= 0xFF;
		}

		//if((rgbFilter>0)||(upDn>0))
		//{
		//	g_paraSetBufIndex+=4;
		//}

		g_paraSetBufIndex+=4;
	}else
	{
		if(!fla)
		{
			g_paraSetBufIndex+=4;
		}
		g_paraSetBufIndex+=2;
	}

	for(i=0;i<17;i++)
	{
		g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN] |= sensor_head_1[i];
		g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_head_1[i+17];
	}

	g_paraSetBufIndex+=1;

	if(mstSla>0)
	{
		if(SENSOR_MS_MST==mstSla)
		{
			for(i=0;i<10;i++)
			{
				g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_mst[i];
			}
		}else
		{
			for(i=0;i<10;i++)
			{
				g_paraSetBuf[i+g_paraSetBufIndex+SENSOR_DSP_LEN*2] |= sensor_sla[i];
			}
		}
	}	

	if(g_isHotProtect>0)
	{
		for(i=0;i<5;i++)
		{
			g_paraSetBuf[i+15] |= hot_warn[i];
		}
	}

	AreaScroll(g_paraSetBuf,SENSOR_DSP_LEN,3,SENSOR_DSP_LEN,SCROLL_UP,1);

	AreaSet(page,cml,SENSOR_DSP_LEN,24);

	for(i=0;i<SENSOR_DSP_LEN*3;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	
}

void DspMotoWarn(void)
{
	//memset(g_paraSetBuf,0x00,83*2);
	LcdClearFeild(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO, 83, 16);
	DisplayPicture(LAYOUT_PAGE_MOTO,LAYOUT_ROW_MOTO,16,16,font_motoWarn,DSP_NORMAL);
	DisplayPicture(LAYOUT_PAGE_MOTO,LAYOUT_ROW_MOTO+16+6,22,16,font_motoWp,DSP_NORMAL);
	DisplayPicture(LAYOUT_PAGE_MOTO,LAYOUT_ROW_MOTO+16+6+22+5,11,16,font_motoBlank,DSP_NORMAL);
	DisplayPicture(LAYOUT_PAGE_MOTO,LAYOUT_ROW_MOTO+16+6+22+5+11,11,16,font_motoBlank,DSP_NORMAL);
	DisplayPicture(LAYOUT_PAGE_MOTO+1,LAYOUT_ROW_MOTO+16+6+22+5+11+11,12,8,font_mm,DSP_NORMAL);
	
}

void DisplayFocLen(uchar motoMode,u16 foc,uchar rsv)
{
	uchar tmp1=0,tmp2=0,tmp3=0;
	uchar noDsp=0;
	if(MOTO_MODE_AUTO==motoMode)
	{
#if 1
	//	if(!g_upDnSensor)
		if(SENSOR_UPDN_UP==g_upDnSensor)
		{
			noDsp = 1;
			//DisplayPicture(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO,FONT_MOTO_WT,FONT_MOTO_HT,font_amoto,DSP_REVERSE);
			LcdClearFeild(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO,FONT_MOTO_WT,FONT_MOTO_HT);
		}else
		{
			//noDsp = 1;
			DisplayPicture(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO,FONT_MOTO_WT,FONT_MOTO_HT,font_amoto,DSP_REVERSE);
		}
#endif
		
	}else if(MOTO_MODE_MANU==motoMode)
	{
#if 1
		DisplayPicture(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO,FONT_MOTO_WT,FONT_MOTO_HT,font_mmoto,DSP_REVERSE);
#endif
	}

//	if(noDsp>0)
//	{
//		tmp1 = 13;
//		tmp2 = 13;
//		tmp3 = 13;
//	}else
//	{
		tmp1 = foc /100;
		tmp2 = (foc %100)/10;
		tmp3 = foc%10;
//	}
	//LCD_ShowString(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST,LAYOUT_ROW_MOTO+FONT_MOTO_WT+2,"Zoom",FONT_TYPE_MAIN,0,DSP_NORMAL);
	DisplayPicture(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO+FONT_MOTO_WT+2, 22, 16, font_zoom, DSP_NORMAL);
	if(DSP_NORMAL==rsv)
	{
		memset(g_paraSetBuf,0x00,11*3*2);
	}else
	{
		memset(g_paraSetBuf,0xFF,11*3*2);
	}
	g_paraSetBufIndex = 0;

	if(noDsp>0)
	{
		LoadZoomChar(' ', g_paraSetBuf+g_paraSetBufIndex,11*3,rsv);
		g_paraSetBufIndex += 11;
		LoadZoomChar(' ', g_paraSetBuf+g_paraSetBufIndex,11*3,rsv);
		g_paraSetBufIndex += 11;
		LoadZoomChar(' ', g_paraSetBuf+g_paraSetBufIndex,11*3,rsv);
		
	}else
	{
		//LcdClearFeild(LAYOUT_PAGE_MOTO,LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1),BIG_FONT_WIDTH(3),BIG_FONT_HIGHT(1));
		if(tmp1)
		{
			//LCD_ShowChar(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+4, tmp1+'0', FONT_TYPE_BIG,rsv);
			LoadZoomChar(tmp1+'0', g_paraSetBuf+g_paraSetBufIndex,11*3,rsv);
		}

		g_paraSetBufIndex += 11;

		LoadZoomChar(tmp2+'0', g_paraSetBuf+g_paraSetBufIndex,11*3,rsv);
		g_paraSetBufIndex += 11;
		LoadZoomChar(tmp3+'0', g_paraSetBuf+g_paraSetBufIndex,11*3,rsv);
		g_paraSetBufIndex += 11;
	}

	AreaSet(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+22+1, 11*3, 16);


	for(tmp1=0;tmp1<11*3*2;tmp1++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[tmp1]);
	}

	DisplayPicture(LAYOUT_PAGE_MOTO+1,LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+22+1+33,12,8,font_mm,DSP_NORMAL);
	
	//LCD_ShowChar(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO+FONT_MOTO_HT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(1)+4, tmp2+'0', FONT_TYPE_BIG,rsv);
	//LCD_ShowChar(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO+FONT_MOTO_HT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(2)+4, tmp3+'0', FONT_TYPE_BIG,rsv);
	//LCD_ShowString(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST,LAYOUT_ROW_MOTO+FONT_MOTO_HT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(3)+4+4,"mm",FONT_TYPE_MAIN,0,DSP_NORMAL);
	
}

void LoadMaskData(char *gBuf, char dspLen, char mask1, char mask2,char n)
{
	char i=0;
	for(i=0;i<n;i++)
	{
		gBuf[i] = mask1;
		gBuf[i+dspLen] = mask2;
	}
}

void LoadCmpFebData(char *gBuf, const char *p, u8 rvs)
{
	u8 strLen=0;
	char *pTmp=(char*)p;
	u16 dspStart=0;
	u8 index=0;
	
	while(*pTmp!='\0')
	{
		strLen += CanCharLen((CanFont *)canFontCmpFeb, FONT_CMPFEB_LEN, *pTmp++);
	}

	dspStart = (PARA_CMPFEB_WT-strLen)/2;
	pTmp = (char*)p;
	while(*pTmp!='\0')
	{
		LoadCmpFebChar(*pTmp,gBuf+dspStart+index,DSP_CMPFEB_LEN,rvs);
		index+=CanCharLen((CanFont*)canFontCmpFeb,FONT_CMPFEB_LEN,*pTmp++);
	}
	
}

void DspCmpFeb(u16 x,u16 y,const char *p,u8 rvs)
{
	u8 strLen=0;
	u8 tmp=0;
	char *pTmp=(char*)p;
	u16 dspStart=0;
	u16 i=0;
	u16 y0=y;

	
	if(DSP_NORMAL==rvs)
	{
		memset(g_paraSetBuf,0x00,PARA_CMPFEB_WT*2);
	}else
	{
		memset(g_paraSetBuf,0x7f,PARA_CMPFEB_WT);
		memset(g_paraSetBuf+PARA_CMPFEB_WT,0xFE,PARA_CMPFEB_WT);
	}

	g_paraSetBufIndex = 0;		//skip a coulmn
	//y++;

	//puts(p);

	while(*pTmp!='\0')
	{
		strLen += CanCharLen((CanFont *)canFontCmpFeb, FONT_CMPFEB_LEN, *pTmp++);
	}

	dspStart = (PARA_CMPFEB_WT-strLen)/2;
	y+=dspStart;
	g_paraSetBufIndex+=dspStart;

	pTmp = (char*)p;

	while(*pTmp!='\0')
	{
		LoadCmpFebChar(*pTmp,g_paraSetBuf+g_paraSetBufIndex,PARA_CMPFEB_WT,rvs);
		tmp=CanCharLen((CanFont*)canFontCmpFeb,FONT_CMPFEB_LEN,*pTmp++);
		//y+=5;
		//pTmp++;
		y+=tmp;
		g_paraSetBufIndex+=tmp;
	}

	AreaSet(x,y0,PARA_CMPFEB_WT,16);
	  
	for(i=0;i<(PARA_CMPFEB_WT*16/8);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	
	
}

#if 0
void CanSetFeb(char feb1,char feb2, unsigned char rsv)
{
	//LcdClearFeild(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,LAYOUT_CMPFEB_WT,LAYOUT_PARA_WT);
	//LCD_FEB_ON(rsv);
	//LCD_FEB_ON(DSP_REVERSE);
	LCD_FEB_ON(DSP_NORMAL);
	if(CMD_MODE_OFF==*g_cmdMode)
	{
		DspCmpFeb(LAYOUT_PARA_AREA2,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1+7),cmpArr[feb1*3+feb2],rsv);
	}else
	{
		DspCmpFeb(LOCATION_TTLCMP_PAGE,(LOCATION_TTLCMP_CML+FONT_CMP_WT+1),cmpArr[feb1*3+feb2],rsv);
	}
	//DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,feb1,feb2,rsv);
	//LCD_ShowString(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+MAIN_FONT_WIDTH(1),cmpArr[feb1*3+feb2],FONT_TYPE_MAIN,0,rsv);	
	//LCD_ShowString(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_INTERVAL(1)+MAIN_FONT_WIDTH(3),"Ev",FONT_TYPE_MAIN,0,rsv);
}
#endif

void ParaPreserve(Uchar paraLoc,Uchar para)
{
}

void SeniorMenuProc()
{
}


/*
FuncMenu ttlMenu = 
{
	{
	{{{0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*12-6,NULL},{0,"+/-",(sizeof("+/-")-1)*12,BtnTtlCmp},{0,"FEB",(sizeof("FEB")-1)*12,BtnTtlFeb},{0,"MENU1",(sizeof("MENU1")-1)*12,NULL}}},
	{{{1,(char*)mstSw,78,NULL},{0,"RATIO",(sizeof("RATIO")-1)*12,BtnRatio},{0,"GR",(sizeof("GR")-1)*12,NULL},{0,"MENU2",(sizeof("MUNU2")-1)*12,NULL}}},
	{{{0,"CH",(sizeof("CH")-1)*12,NULL},{0,"ID",(sizeof("ID")-1)*12,NULL},{0,"SCAN",(sizeof("SCAN")-1)*12,NULL},{0,"MENU3",(sizeof("MENU3")-1)*12,NULL}}},
	{{{0,"",0,NULL},{0,"SYNC",(sizeof("SYNC")-1)*12,NULL},{0,"MEMORY",(sizeof("MEMORY")-1)*12,NULL},{0,"MENU4",(sizeof("MENU4")-1)*12,NULL}}},
	}
};

FuncMenu mMenu=
{
	{
	{{{0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*12-6,NULL},{0,"RATIO",(sizeof("RATIO")-1)*12,NULL},{0,"GR",(sizeof("GR")-1)*12,NULL},{0,"MENU1",(sizeof("MENU1")-1)*12,NULL}}},
	{{{1,(char*)mstSw,78,NULL},{0,"SYNC",(sizeof("SYNC")-1)*12,NULL},{0,"",0,NULL},{0,"MENU2",(sizeof("MUNU2")-1)*12,NULL}}},
	{{{0,"CH",(sizeof("CH")-1)*12,NULL},{0,"ID",(sizeof("ID")-1)*12,NULL},{0,"SCAN",(sizeof("SCAN")-1)*12,NULL},{0,"MENU3",(sizeof("MENU3")-1)*12,NULL}}},
	{{{0,"",0,NULL},{0,"",0,NULL},{0,"MEMORY",(sizeof("MEMORY")-1)*12,NULL},{0,"MENU4",(sizeof("MENU4")-1)*12,NULL}}},
	}
};

FuncMenu multiMenu=
{
	{
	{{{0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*12-6,NULL},{0,"MULTI",(sizeof("MULTI")-1)*12,NULL},{0,"Hz",(sizeof("Hz")-1)*12,NULL},{0,"MENU1",(sizeof("MENU1")-1)*12,NULL}}},
	{{{1,(char*)mstSw,78,NULL},{0,"RATIO",(sizeof("RATIO")-1)*12,NULL},{0,"GR",(sizeof("GR")-1)*12,NULL},{0,"MENU2",(sizeof("MUNU2")-1)*12,NULL}}},
	{{{0,"CH",(sizeof("CH")-1)*12,NULL},{0,"ID",(sizeof("ID")-1)*12,NULL},{0,"SCAN",(sizeof("SCAN")-1)*12,NULL},{0,"MENU3",(sizeof("MENU3")-1)*12,NULL}}},
	{{{0,"",0,NULL},{0,"",0,NULL},{0,"MEMORY",(sizeof("MEMORY")-1)*12,NULL},{0,"MENU4",(sizeof("MENU4")-1)*12,NULL}}},
	}
};

FuncMenu grMenu=
{
	{
	{{{0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*12-6,NULL},{0,"+/-",(sizeof("+/-")-1)*12,NULL},{0,"Gr",(sizeof("GR")-1)*12,NULL},{0,"MENU1",(sizeof("MUNU1")-1)*12,NULL}}},
	{{{1,(char*)mstSw,78,NULL},{0,"FEB",(sizeof("FEB")-1)*12,NULL},{0,"SYNC",(sizeof("SYNC")-1)*12,NULL},{0,"MENU2",(sizeof("MUNU2")-1)*12,NULL}}},
	{{{0,"CH",(sizeof("CH")-1)*12,NULL},{0,"ID",(sizeof("ID")-1)*12,NULL},{0,"SCAN",(sizeof("SCAN")-1)*12,NULL},{0,"MENU3",(sizeof("MENU3")-1)*12,NULL}}},
	{{{0,"",0,NULL},{0,"",0,NULL},{0,"MEMORY",(sizeof("MEMORY")-1)*12,NULL},{0,"MENU4",(sizeof("MENU4")-1)*12,NULL}}},
	}
};


Button mNull = {0,"",0,NULL};
Button cfn={0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*12-6,NULL};
Button addSub = {0,"+/-",(sizeof("+/-")-1)*12,BtnAddSub};
Button ttlFeb = {0,"FEB",(sizeof("FEB")-1)*12,BtnTtlFeb};
Button menu1 = {0,"MENU1",(sizeof("MENU1")-1)*12,NULL};
Button menu2 = {0,"MENU2",(sizeof("MENU2")-1)*12,NULL};
Button menu3 = {0,"MENU3",(sizeof("MENU3")-1)*12,NULL};
Button menu4 = {0,"MENU4",(sizeof("MENU4")-1)*12,NULL};
Button mstFlaSw = {1,(char*)mstSw,78,NULL};
Button mRatio = {0,"RATIO",(sizeof("RATIO")-1)*12,BtnRatio};
Button mGr = {0,"GR",(sizeof("GR")-1)*12,NULL};
Button mCh = {0,"CH",(sizeof("CH")-1)*12,NULL};
Button mId = {0,"ID",(sizeof("ID")-1)*12,NULL};
Button mScan = {0,"SCAN",(sizeof("SCAN")-1)*12,NULL};
Button mSync = {0,"SYNC",(sizeof("SYNC")-1)*12,NULL};
Button mMemory = {0,"MEMORY",(sizeof("MEMORY")-1)*12,NULL};
Button mMulti = {0,"MULTI",(sizeof("MULTI")-1)*12,NULL};
Button mFrq = {0,"Hz",(sizeof("Hz")-1)*12,NULL};

*/


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
	funcMenu.listCnt = 0;
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&ttlFeb,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mRatio,(*wlSet.ttl_Ratio==WLM_MODE_ALL)?&mNull:&mGr,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mSync,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_MST:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&ttlFeb,&menu1,funcMenu.listCnt++);
					//AddButtons(&mstFlaSw,&mRatio,&mGr,&menu2,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mRatio,(*wlSet.ttl_Ratio==WLM_MODE_ALL)?&mNull:&mGr,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
					//AddButtons(&mReset,&mSync,&mMemory,&menu4,funcMenu.listCnt++);
					AddButtons(&mNull,&mSync,&mMemory,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_OFF:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&ttlFeb,&mSync,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_SLA:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mRel,&mModel,&mTest,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_LIGH_SLA:
					AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mCh,&mNull,&mMemory,&menu2,funcMenu.listCnt++);
				break;

				case CMD_MODE_RFLS_MST:
					//funcMenu.listCnt = 0;
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfn,&addSub,&mNull,&menu3,funcMenu.listCnt++);
						AddButtons(&mSync,&ttlFeb,&mNull,&menu4,funcMenu.listCnt++);
					}else
					{
						//AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
						AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfnNul,&mNull,&mNull,&menu3,funcMenu.listCnt++);
						//AddButtons(&mSync,&ttlFeb,&mNull,&menu4,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfn,&addSub,&mNull,&menu3,funcMenu.listCnt++);
						AddButtons(&mSync,&ttlFeb,&mNull,&menu4,funcMenu.listCnt++);
					}else
					{
						//AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
						AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfnNul,&mNull,&mNull,&menu3,funcMenu.listCnt++);
					}
					//AddButtons(&mNull,&mCh,&mId,&mScan,funcMenu.listCnt++);
				break;
			}
		break;

		case WORKINGMODE_MANU:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&mRatio,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mSync,&mNull,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mNull,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_MST:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&mRatio,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mSync,&mNull,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_SLA:
					AddButtons(&cfn,&mNull,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mRel,&mModel,&mTest,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_OFF:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&mNull,&mSync,funcMenu.listCnt++);
				break;

				case CMD_MODE_RFLS_MST:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfn,&addSub,&mSync,&menu3,funcMenu.listCnt++);
						//AddButtons(&mSync,&ttlFeb,&mNull,&menu4,funcMenu.listCnt++);
					}else
					{
						//AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
						AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfnNul,&mNull,&mNull,&menu3,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_LIGH_SLA:
					if(g_isIndividualSlave>0)
					{
						AddButtons(&cfn,&addSub,&mNull,&menu1,funcMenu.listCnt++);
						AddButtons(&mCh,&mNull,&mMemory,&menu2,funcMenu.listCnt++);
					}else
					{
						AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
						AddButtons(&mCh,&mNull,&mMemory,&menu2,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfn,&addSub,&mNull,&menu3,funcMenu.listCnt++);
						AddButtons(&mSync,&ttlFeb,&mNull,&menu4,funcMenu.listCnt++);
					}else
					{
						//AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
						AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfnNul,&mNull,&mNull,&menu3,funcMenu.listCnt++);
					}
					//AddButtons(&mNull,&mCh,&mId,&mScan,funcMenu.listCnt++);
				break;
			}
		break;

		case WORKINGMODE_MULTI:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&mMulti,&mFrq,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mRatio,&mGr,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mNull,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&mMulti,&mFrq,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mRatio,&mGr,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
					AddButtons(&mNull,&mSync,&mMemory,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_OFF:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&mMulti,&mFrq,funcMenu.listCnt++);					
				break;

				case CMD_MODE_RF_SLA:
					AddButtons(&cfn,&mNull,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mRel,&mModel,&mTest,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RFLS_MST:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfn,&addSub,&mNull,&menu3,funcMenu.listCnt++);
						AddButtons(&mMulti,&mFrq,&mNull,&menu4,funcMenu.listCnt++);
					}else
					{
						//AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
						AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfnNul,&mNull,&mNull,&menu3,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_LIGH_SLA:
					if(g_isIndividualSlave>0)
					{
						AddButtons(&cfn,&mNull,&mNull,&menu1,funcMenu.listCnt++);
						AddButtons(&addSub,&mMulti,&mFrq,&menu2,funcMenu.listCnt++);
						AddButtons(&mCh,&mNull,&mMemory,&menu3,funcMenu.listCnt++);
					}else
					{
						AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
						AddButtons(&mCh,&mNull,&mMemory,&menu2,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_RFLS_SLA:
					if(pfnSet[PFN_ID_LSSW]>0)
					{
						AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfn,&addSub,&mNull,&menu3,funcMenu.listCnt++);
						AddButtons(&mMulti,&mFrq,&mNull,&menu4,funcMenu.listCnt++);
					}else
					{
						AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
						AddButtons(&cfnNul,&mNull,&mNull,&menu3,funcMenu.listCnt++);
					}
					//AddButtons(&mNull,&mCh,&mId,&mScan,funcMenu.listCnt++);
				break;
			}
		break;

		case WORKINGMODE_GR:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					funcMenu.listCnt = 0;
				break;

				case CMD_MODE_RF_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&ttlFeb,&mSync,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_OFF:
					funcMenu.listCnt = 0;
				break;

				case CMD_MODE_RFLS_MST:
					AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
				break;

				case CMD_MODE_RFLS_SLA:
					AddButtons(&mNull,&mCh,&mId,&mScan,funcMenu.listCnt++);
				break;
			}
		break;

		case WORKINGMODE_EXTA:
			switch(*g_cmdMode)
			{
				case CMD_MODE_RFLS_MST:
					AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
					AddButtons(&cfn,&addSub,&ttlFeb,&menu3,funcMenu.listCnt++);
					//AddButtons(&mSync,&ttlFeb,&mNull,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_RFLS_SLA:
					AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
					AddButtons(&cfn,&addSub,&ttlFeb,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_SLA:
					//funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
					AddButtons(&mRel,&mModel,&mTest,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				default:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&ttlFeb,&mNull,funcMenu.listCnt++);
				break;
			}
			//funcMenu.listCnt = 0;
			//AddButtons(&cfn,&addSub,&ttlFeb,&mNull,funcMenu.listCnt++);
		break;

		case WORKINGMODE_EXTM:
			switch(*g_cmdMode)
			{
				case CMD_MODE_RFLS_MST:
					AddButtons(&mRel,&mCh,&mId,&menu1,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
					AddButtons(&cfn,&mIso,&mApert,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RFLS_SLA:
					AddButtons(&mNull,&mCh,&mId,&menu1,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mScan,&menu2,funcMenu.listCnt++);
					AddButtons(&cfn,&mIso,&mApert,&menu3,funcMenu.listCnt++);
				break;

				default:
				funcMenu.listCnt = 0;
				//AddButtons(&cfn,&mMemory,&mIso,&mApert,funcMenu.listCnt++);
				AddButtons(&cfn,&mNull,&mIso,&mApert,funcMenu.listCnt++);
				break;
			}
		break;
/*
		case WORKINGMODE_RTC:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
				break;

				case CMD_MODE_RF_MST:
				break;

				case CMD_MODE_OFF:
				break;
			}
		break;
*/
	}
}

void BuildLinkedShotMenu(u8 ms)
{
	switch(ms)
	{
		//case RF_LKSHOT_MASTER:
		case CMD_MODE_RFLS_MST:
			funcMenu.listCnt = 0;
			AddButtons(&mRel,&mCh,&mId,&mScan,funcMenu.listCnt++);
		break;

		//case RF_LKSHOT_SLAVE:
		case CMD_MODE_RFLS_SLA:
			funcMenu.listCnt = 0;
			AddButtons(&mNull,&mCh,&mId,&mScan,funcMenu.listCnt++);
		break;
	}
	KeyMenuSw();
}

u8 ScaleNumLen(char ch)
{
	u8 i;
	for(i=0;i<FONT_SCALE_LEN;i++)
	{
		if(ch==scaleFont[i].ch)
		{
			return scaleFont[i].wt;
		}
	}
	return 0;
}

u8 ScaleStrLen(char *str)
{
	u8 i=0;
	u8 strLen=0;
	while(*str!='\0')
	{
		for(i=0;i<FONT_SCALE_LEN;i++)
		{
			if(*str==scaleFont[i].ch)
			{
				strLen += scaleFont[i].wt;
				break;
			}
		}
		str++;
	}
	return strLen;
}

u8 ResScaleFont(char ch)
{
	u8 i;
	for(i=0;i<FONT_SCALE_LEN;i++)
	{
		if(ch==scaleFont[i].ch)
		{
			//return &scaleFont[i];
			return i;
		}
	}
	return 0;
}

#define DSP_SCOPE_LEN (10+6+10+6+13+10+6+10+6)

void DrawScopeNum(char *ps, char *pe,u8 mod, char mft)
{
	u8 i=0,index=0;
	
	memset(g_paraSetBuf,0x00,DSP_SCOPE_LEN*2);

	if(!mod)
	{
		g_paraSetBufIndex = 22;
	}else
	{
		g_paraSetBufIndex = 0;
	}

	while(*ps!='\0')
	{
		if(*ps=='.')
		{
			g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+2] |= 0x0c;
			g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+3] |= 0x0c;
			g_paraSetBufIndex+=6;
		}else if((*ps>='0')&&(*ps<='9'))
		{
			for(i=0;i<10;i++)
			{
				g_paraSetBuf[g_paraSetBufIndex+i] = scaleNum[(*ps-'0')*20+i];
				g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+i] = scaleNum[(*ps-'0')*20+10+i];
			}
			g_paraSetBufIndex+=10;
		}
		ps++;
	}

	//index = ResScaleFont('m');
	index = ResScaleFont(mft);

	for(i=0;i<scaleFont[index].wt;i++)
	{
		g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+i] = scaleFont[index].buf[i];
	}

	if(mod>0)
	{
	
		g_paraSetBufIndex+=6;

		g_paraSetBufIndex+=4;

		for(i=0;i<6;i++)
		{
			g_paraSetBuf[g_paraSetBufIndex+i] = 0x01;
			g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+i] = 0x80;
		}

		g_paraSetBufIndex+=6;
		g_paraSetBufIndex+=3;

		while(*pe!='\0')
		{
			if(*pe=='.')
			{
				g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+2] |= 0x0c;
				g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+3] |= 0x0c;
				g_paraSetBufIndex+=6;
			}else if((*pe>='0')&&(*pe<='9'))
			{
				for(i=0;i<10;i++)
				{
					g_paraSetBuf[g_paraSetBufIndex+i] = scaleNum[(*pe-'0')*20+i];
					g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+i] = scaleNum[(*pe-'0')*20+10+i];
				}
				g_paraSetBufIndex+=10;
			}
			pe++;
		}

		//index = ResScaleFont('m');
		index = ResScaleFont(mft);

		for(i=0;i<scaleFont[index].wt;i++)
		{
			g_paraSetBuf[DSP_SCOPE_LEN+g_paraSetBufIndex+i] = scaleFont[index].buf[i];
		}
		
	}

	AreaSet(7,45,DSP_SCOPE_LEN,16);

	for(i=0;i<DSP_SCOPE_LEN*2;i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}	
	
}

#define SCALE_START_CML 13
#define SCALE_INTERVAL	29
#define SCALE_PAGE		10
//void DrawScaleNum(void)
void DrawScaleNum(char mft)
{
	u8 i=0;
	char *p=NULL;
	u8 cml=0;
	//ScaleFont *pFont;
	u8 index;
	u8 tmp;
	u8 j=0;
	for(i=0;i<6;i++)
	{
		//p = (char*)scaleArr[i];
		if('m'==mft)
		{
			p = (char*)scaleArr[i];
		}else if('f'==mft)
		{
			p = (char*)scaleArrEx[i];
		}
		tmp = ScaleStrLen(p);
		
		cml = (SCALE_START_CML+SCALE_INTERVAL*i)-((tmp+1)/2);
		//cml = (SCALE_START_CML+SCALE_INTERVAL*i);
		
		//AreaSet(SCALE_PAGE, cml, ScaleStrLen(p), 8);
		AreaSet(SCALE_PAGE, cml, tmp, 8);
		while(*p!='\0')
		{
			//pFont = ResScaleFont(*p);
			index = ResScaleFont(*p);
			for(j=0;j<scaleFont[index].wt;j++)
			{
				LCD_WRITE_BYTE(scaleFont[index].buf[j]);
			}
			p++;
		}
	}
	AreaSet(SCALE_PAGE, 164, 6, 8);
	//index = ResScaleFont('m');
	index = ResScaleFont(mft);
	for(j=0;j<scaleFont[index].wt;j++)
	{
		LCD_WRITE_BYTE(scaleFont[index].buf[j]>>2);
	}
	
}

void DrawScaleRuler(char on, signed char up, signed char low)
{
	u8 i;
	u8 ns=0,ne=0;
	u8 isScape=1;
	memset(g_paraSetBuf,0x00,SCALE_INTERVAL*5+1+16);
	g_paraSetBufIndex = 0;

	if(on>0)
	{
		if(up==low)
		{
			if(low<0)
			{
				memcpy(g_paraSetBuf,scaleFont[ResScaleFont('<')].buf,6);
				//on = 0;
				low = 0;
				isScape = 0;
			}
		}else
		{
			if((low<=0)&&(up<=0))				//two dark, no scale to diaplay
			{
				low = 0;
				memcpy(g_paraSetBuf,scaleFont[ResScaleFont('<')].buf,6);
				//on = 0;
				isScape = 0;
				//return;
			}else if(low<0)
			{
				low = 0;
			}
		}
		g_paraSetBufIndex+=7;

		if(up>10)							//
		{
			memcpy(g_paraSetBuf+(SCALE_INTERVAL*5+1+2)+g_paraSetBufIndex,scaleFont[ResScaleFont('>')].buf,6);
			up = 10;
			if(up==low)
			{
				isScape = 0;
			}
		}
		//AreaSet(SCALE_PAGE-1, SCALE_START_CML, SCALE_INTERVAL*5+1, 8);
		
		for(i=0;i<SCALE_INTERVAL*5+1;i++)
		{
			if(i%SCALE_INTERVAL)
			{
				//LCD_WRITE_BYTE(0x04);
				g_paraSetBuf[i+g_paraSetBufIndex] |= 0x04;
			}else
			{
				//LCD_WRITE_BYTE(0x06);
				g_paraSetBuf[i+g_paraSetBufIndex] |= 0x06;
			}
		}

		//if(on>0)
		if(isScape>0)
		{
			if(up==low)
			{
				if(up<=0)
				{
					ne = 4;
					ns = 0;
				}else if(up>=10)
				{
					ne = 10*15;
					ns = ne-4;
				}else
				{
					ns = (up/2)*29+(up%2)*15-2;
					ne = ns+4;
				}
				
			}else
			{
				ne = (up/2)*29+(up%2)*15;
				ns = (low/2)*29+(low%2)*15;
				if(ne>SCALE_INTERVAL*5+1)
				{
					ne = SCALE_INTERVAL*5+1;
				}
			}
			
			for(i=ns;i<=ne;i++)
			{
				g_paraSetBuf[i+g_paraSetBufIndex] |= 0xF0;
			}
		}
	}else
	{
		g_paraSetBufIndex+=7;
		
		for(i=0;i<SCALE_INTERVAL*5+1;i++)
		{
			if(i%SCALE_INTERVAL)
			{
				//LCD_WRITE_BYTE(0x04);
				g_paraSetBuf[i+g_paraSetBufIndex] |= 0x04;
			}else
			{
				//LCD_WRITE_BYTE(0x06);
				g_paraSetBuf[i+g_paraSetBufIndex] |= 0x06;
			}
		}
	}

	AreaSet(SCALE_PAGE-1, SCALE_START_CML-8, SCALE_INTERVAL*5+1+16, 8);

	for(i=0;i<(SCALE_INTERVAL*5+1+16);i++)
	{
		LCD_WRITE_BYTE(g_paraSetBuf[i]);
	}
	
}

#if 0
void DrawScaleHp(signed char mode, unsigned char st,u8 para, char on)
{
	signed char up=0,dn=0;
	
	DrawScaleNum();								//draw ruler
	if(on>0)
	{
		if(WORKINGMODE_TTL==mode)
		{
			
		}else(WORKINGMODE_MANU==mode)
		{

		}
	}else
	{
		DrawScaleRuler(0,up,dn);
		AreaSet(7,45,DSP_SCOPE_LEN,16);

		for(i=0;i<DSP_SCOPE_LEN*2;i++)
		{
			LCD_WRITE_BYTE(0x00);
		}
	}
}
#endif

void DrawScale(signed char mode,unsigned char st,unsigned char isHp,u8 para,char on)
{
	signed char up=0,dn=0;
	u8 i;
	if(pSnrSet[SNRMENU_STD]>0)
	{
		DrawScaleNum('f');								//draw ruler
	}else
	{
		DrawScaleNum('m');
	}
	if(on>0)
	{
		if(mode==WORKINGMODE_EXTM)
		{
			CalcAvailScaleEx(&up,&dn);
		}else
		{
			if(isHp>0)
			{
				CalcAvailScaleHp(mode, para, st, &up, &dn);
			}else
			{
				CalcAvailScale(mode,para,&up,&dn);
			}
		}
		DrawScaleRuler(1,up,dn);
		if(dn<0)
		{
			dn=0;
		}else if(dn>10)
		{
			dn = 10;
		}
		if(up<0)
		{
			up = 0;
		}else if(up>10)
		{
			up = 10;
		}
		if(pSnrSet[SNRMENU_STD]>0)
		{
			DrawScopeNum((char*)(scopeNumEx+dn),(char*)(scopeNumEx+up),up>dn,'f');
		}else
		{
			DrawScopeNum((char*)(scopeNum+dn),(char*)(scopeNum+up),up>dn,'m');
		}
	}else
	{
		DrawScaleRuler(0,up,dn);
		AreaSet(7,45,DSP_SCOPE_LEN,16);

		for(i=0;i<DSP_SCOPE_LEN*2;i++)
		{
			LCD_WRITE_BYTE(0x00);
		}
	}
	
}

float calcSqrt(float sr)
{
	int i=100;
	float x = sr/2;
	while (i--)
	{
		x=(x+sr/x)/2;
	}
	return x;
}

signed char SearchScaleIndex(float gn)
{
	u8 i=0;

	if(gn<0.5)
	{
		return 0;
	}
	
	for(i=0;i<12;i++)
	{
		if((gn<=scaleStep[i]+0.05)&&(gn>scaleStep[i]-0.05))
		{
			//gn = i;
			//break;
			return i;
		}else if(gn<scaleStep[i])
		{
			return i-1;
		}
	}

	return 12;
}

signed char GetMotoIncremental(signed char focLen)
{
	if(focLen<2)
	{
		return 0;
	}else if(focLen<4)
	{
		return focLen-1;
	}else if(focLen<5)
	{
		return 4;
	}else if(focLen<7)
	{
		//return focLen-1;
		return 5;
	}else
	{
		return 6;
	}
}


#define SCALE_STANDARD 10//12

void CalcAvailScale(signed char mode, u8 mIn,signed char *up, signed char *dn)
{
	//float multi=1;
	//float gn0,gn1;
	//u8 motoMulti=0;
	u8 tmp;
	s8 para1=0,para2=0;
//	u8 i=0;
	signed char focIndex=0;

	focIndex = GetMotoStdIndex(*motoSet.dest_location);

	tmp = GetMotoIncremental(focIndex);

	if(MOTO_MODE_AUTO==*motoSet.mode)			//2015/08/16
	{
		tmp++;
	}

/*
	if(!focIndex)
	{
		para1 += 0;
		para2 += 0;
	}else
	{
		para1 += (focIndex/3);
		para2 += (focIndex%3);
	}
*/
	para1 += (tmp/3);
	para2 += (tmp%3);
/*
	if(!motoSet.focIndex)
	{
		//motoMulti = 1;		
		para1 = 0;para2 = 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		para1 = (motoSet.focIndex-1)/3;
		para2 = ((motoSet.focIndex-1)%3);
	}
*/

	tmp = ((flaPara.isoCmp>>4)*6)+(((flaPara.isoCmp&0x0F)+2)/3);

	para1+=( tmp/3);
	para2+=((tmp+3)%3);

	ReStdPara(&para1,&para2);

	tmp = ((menuPara.cApert>>4)*6)+(((menuPara.cApert&0x0F)+2)/3);

	para1-=(tmp/3);
	para2 -= ((tmp+3)%3);

	ReStdPara(&para1,&para2);

	para1-=(SCALE_STANDARD/3);
	para2-=(SCALE_STANDARD%3);

/*
	if(para2>2)
	{
		para1+=1;
		para2-=3;
	}else if(para2<0)
	{
		para1--;
		para2+=3;
	}
*/
	ReStdPara(&para1,&para2);

	if((WORKINGMODE_TTL==mode)||(WORKINGMODE_EXTA==mode))
	{
		*up = para1+7;
		*dn = para1+1;

		if(*up>0)
		{
			//if((motoSet.focIndex>3)&&(*dn<1))
			//if((motoSet.focIndex>2)&&(*dn<1))
			if((focIndex>2)&&(*dn<1))
			{
				*dn = 1;
			}
		}
		
	}else
	{
		para1 += (mIn/3);
		para2 +=((mIn+3)%3);

		if(para2>3)
		{
			para1+=1;
			para2-=3;
		}

		*up = *dn = para1;

		//if((motoSet.focIndex>3)&&(*dn==0))
		//if((motoSet.focIndex>2)&&(*dn==0))
		if((focIndex>2)&&(*dn==0))
		{
			*dn=1;
			*up=1;
		}
	}

	
	
	//for(i=0;i<motoSet.focIndex;i++)
	//{
	//	multi *=  gnFocMul[i];//foclen 
	//}

	//multi *= calcSqrt(mainPara.extIso)/10;//iso
	//tmp1 = menuPara.cApert>>4;
	//tmp2 = menuPara.cApert&0x0F;
	//tmp1 = flaPara.isoCmp>>4;
	//tmp2 = flaPara.isoCmp&0x0F;

/*
	if(tmp1<4)
	{
		for(i=tmp1;i<4;i++)
		{
			multi/=2;
		}
	}else
	{
		for(i=4;i<tmp1;i++)
		{
			multi*=2;
		}
	}
*/

/*
	if(tmp2>8)
	{
		multi*=1.4f;
	}	

	multi *=10;

	multi/=flaPara.apert;
*/
/*
	if((WORKINGMODE_TTL==mode)||(WORKINGMODE_EXTA==mode))
	{

		gn0 = v_gn_dn[0]*multi*1.4;
		gn1 = v_gn_up[0]*multi;

		*up = SearchScaleIndex(gn1);
		*dn = SearchScaleIndex(gn0);

		if((*dn>0)&&(*up-*dn<6))
		{
			*up = *up+1;;
		}

		
	}else
	{
		*up = *dn = v_gn_m[mIn];
	}
*/
}

#define SCALE_EXTM_STD 11 //3//(-3)
//void CalcAvailScaleEx(signed char mode, u8 mIn,signed char *up, signed char *dn)
void CalcAvailScaleEx(signed char *up, signed char *dn)
{
	//float multi=1;
	//float gn0,gn1;
	u8 tmp;
	s8 para1=0,para2=0;
	signed char focIndex=0;

	para1 += (SCALE_EXTM_STD/3);
	para2 += (SCALE_EXTM_STD%3);

	focIndex = GetMotoStdIndex(*motoSet.dest_location);

	tmp = GetMotoIncremental(focIndex);

	if(MOTO_MODE_AUTO==*motoSet.mode)			//2015/08/16
	{
		tmp++;
	}

	para1 += (tmp/3);
	para2 += (tmp%3);
	//u8 motoMulti=0;
//	u8 i=0;

	//if(!motoSet.focIndex)
	//{
	//	motoMulti = 3;
	//}else
	//{
	//	motoMulti = motoSet.focIndex - 1;
	//}
/*
	if(!motoSet.focIndex)
	{
		//motoMulti = 1;		
		para1 = 0;para2 = 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		para1 = (motoSet.focIndex-1)/3;
		para2 = ((motoSet.focIndex-1)%3);
	}
*/
/*
	if(!focIndex)
	{
		para1 += 0;
		para2 += 0;
	}else
	{
		para1 += (focIndex/3);
		para2 += (focIndex%3);
	}
*/
	

	ReStdPara(&para1,&para2);

	//tmp = ((flaPara.isoCmp>>4)*6)+(((flaPara.isoCmp&0x0F)+2)/3);
	tmp = ExtmIsoIndex(mainPara.extIso);
	para1 += tmp/3;
	para2 += tmp%3;

	ReStdPara(&para1,&para2);

	tmp = ExtmApertIndex(mainPara.extApert);

	para1 -= tmp/3;
	para2 -= tmp%3;

	ReStdPara(&para1,&para2);

	//para1+=SCALE_EXTM_STD;
/*
	if(para2<0)
	{
		para1--;
		para2+=3;
	}else if(para2>2)
	{
		para1+=1;
		para2-=3;
	}
*/

	*dn = para1;
	*up = para1+6;

	//if((motoSet.focIndex>3)&&(*up>0)&&(*dn<1))
	//if((motoSet.focIndex>2)&&(*up>0)&&(*dn<1))
	if((focIndex>2)&&(*up>0)&&(*dn<1))
	{
		*dn = 1;
	}
	
/*	
	for(i=0;i<motoSet.focIndex;i++)
	{
		multi *=  gnFocMul[i];//foclen 
	}

	multi *= calcSqrt(mainPara.extIso)/10;//iso

	multi *=10;

	multi/=mainPara.extApert;

	//if((WORKINGMODE_TTL==mode)||(WORKINGMODE_EXTA==mode)||(WORKINGMODE_EXTM==mode))
	//{

	gn0 = v_gn_dn[0]*multi*1.4;
	gn1 = v_gn_up[0]*multi;

	*up = SearchScaleIndex(gn1);
	*dn = SearchScaleIndex(gn0);

	if((*dn>0)&&(*up-*dn<6))
	{
		*up = *up+1;;
	}
*/
		
	//}else
	//{
	//	*up = *dn = v_gn_m[mIn];
	//}
	

/*

	for(i=0;i<12;i++)
	{
		if(gn0<=scaleStep[i])
		{
			*dn = i;
			break;
		}
	}

	for(i=0;i<12;i++)
	{
		if(gn1<=scaleStep[i])
		{
			*up = i;
			break;
		}
	}
*/

}

#define SCALE_STANDARD_HPA	10
#define SCALE_STANDARD_HPM	16//10

void CalcAvailScaleHp(signed char mode, u8 mIn, unsigned char st,signed char * up, signed char * dn)
{
//	u8 motoMulti=0;
	u8 tmp1;
	s8 para1=0,para2=0;
//	u8 i=0;
	signed char focIndex=0;

	focIndex = GetMotoStdIndex(*motoSet.dest_location);

	tmp1 = GetMotoIncremental(focIndex);

	if(MOTO_MODE_AUTO==*motoSet.mode)			//2015/08/16
	{
		tmp1++;
	}
/*
	if(!motoSet.focIndex)
	{
		//motoMulti = 1;		
		para1 = 0;para2 = 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		para1 = (motoSet.focIndex-1)/3;
		para2 = ((motoSet.focIndex-1)%3);
	}
*/
/*
	if(!focIndex)
	{
		//motoMulti = 1;		
		para1 = 0;para2 = 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		para1 = (focIndex/3);
		para2 = (focIndex%3);
	}
*/
	para1 += (tmp1/3);
	para2 += (tmp1%3);

/*
	if(WORKINGMODE_TTL==mode)
	{
		tmp1 = ((flaPara.isoCmp>>4)*6)+(((flaPara.isoCmp&0x0F)+2)/3);

		para1+=( tmp1/3);
		para2+=((tmp1+3)%3);
	}else 
	{
		para1+=(mIn/3);
		para2+=(mIn%3);
	}
*/

	tmp1 = ((flaPara.isoCmp>>4)*6)+(((flaPara.isoCmp&0x0F)+2)/3);

	para1+=( tmp1/3);
	para2+=((tmp1+3)%3);

	if(WORKINGMODE_MANU==mode)
	{
		ReStdPara(&para1,&para2);
		
		para1+=(mIn/3);
		para2+=(mIn%3);
	}

	ReStdPara(&para1,&para2);

	tmp1 = ((menuPara.cApert>>4)*6)+(((menuPara.cApert&0x0F)+2)/3);

	para1-=(tmp1/3);
	para2 -= ((tmp1+3)%3);

	ReStdPara(&para1,&para2);

	tmp1 = (((st-0x78)>>4)*6)+((((st-0x78)&0x0F)+2)/3);
	para1-=( tmp1/3);
	para2-=((tmp1+3)%3);

	ReStdPara(&para1,&para2);

//	para1-=(SCALE_STANDARD/3);
//	para2-=(SCALE_STANDARD%3);

//	ReStdPara(&para1,&para2);

/*
	if(para2<-3)
	{
		para1-=2;
		para2+=6;
	}else if(para2<0)
	{
		para1--;
		para2+=3;
	}else if(para2>2)
	{
		para1+=1;
		para2-=3;
	}
*/

	if((WORKINGMODE_TTL==mode)||(WORKINGMODE_EXTA==mode))
	{
		//*up = para1+7;
		//*dn = para1+1;
		para1-=(SCALE_STANDARD_HPA/3);
		para2-=(SCALE_STANDARD_HPA%3);
		
		ReStdPara(&para1,&para2);
		
		//*up = para1+6;
		//*dn = para1+1;

		*up= para1+5;
		*dn = para1;

		//if((*up>0)&&(*dn<1)&&(motoSet.focIndex>3))
		if((*up>0)&&(*dn<1)&&(focIndex>2))
		{
			*dn = 1;
		}
		
		
	}else
	{
		//para1 += (mIn/3);
		//para2 +=((mIn+3)%3);		
/*
		if(para2>3)
		{
			para1+=1;
			para2-=3;
		}
*/
		para1 -= (SCALE_STANDARD_HPM/3);
		para2 -= (SCALE_STANDARD_HPM%3);

		ReStdPara(&para1,&para2);
		
		*up = *dn = para1;

		if((!*up)&&(focIndex>2))
		{
			*up = *dn = 1;
		}
	}

}

void ReStdPara(signed char *p1, signed char *p2)
{
	if((*p2)<0)
	{
		(*p1)--;
		(*p2)+=3;
	}else if((*p2)>2)
	{
		(*p1)+=1;
		(*p2)-=3;
	}
}

extern const u16 g_hpExpCode[];
#define EXT_VAL_LEN		27
#define EXTA_DASET_STD 19//22//19//15//18
#define EXTA_PA_ST	5

//void ExtaModSet(signed char foc, unsigned char isoCmp, unsigned char apert,
u8 ExtaModSet(signed char foc, unsigned char isoCmp, unsigned char apert,
					signed char cmp1, signed char cmp2, 
					signed char feb1, signed char feb2, unsigned char febCnt)
{
	u8 tmp;
	s8 para1=0,para2=0;

#ifdef EXT_TEST_PRT
	//printf("foc %d,iso %x, apert %x\r\n",foc,isoCmp,apert);
#endif


	para1+=(EXTA_DASET_STD/3);
	para2+=(EXTA_DASET_STD%3);

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

	//GetMotoIncremental(foc);

	if(foc>0)
	{
		//para1 -= (foc-1)/3;
		//para2 -= (foc-1)%3;
		//para1 -= (foc)/3;
		//para2 -= (foc)%3;
		para1 -= (GetMotoIncremental(foc)/3);
		para2 -= (GetMotoIncremental(foc)%3);
	}

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif
/*
	if(!foc)
	{
		//motoMulti = 1;		
		para1 = 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		//para1 = (motoSet.focIndex-1)/3;
		//para2 = ((motoSet.focIndex-1)%3);
		//para1 += (motoSet.focIndex-1);
		para1 += (foc-1);
	}
*/
	
	//para1+=(EXTA_DASET_STD);

	//tmp = ((menuPara.cApert>>4)*6)+(((menuPara.cApert&0x0F)+2)/3);
	tmp = ((apert>>4)*6)+(((apert&0x0F)+2)/3);

	para1+=(tmp/3);
	para2 += ((tmp+3)%3);

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

	tmp = ((isoCmp>>4)*6)+(((isoCmp&0x0F)+2)/3);

	para1-=( tmp/3);
	para2-=((tmp+3)%3);
	

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

/*
	if(para2<0)
	{
		para1--;
		para2+=3;
	}else if(para2>2)
	{
		para1+=1;
		para2-=3;
	}
*/
	ReStdPara(&para1,&para2);


	para1+=cmp1;
	para2+=cmp2;

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	printf("cmp %d, %d\r\n",para1,para2);
#endif

	switch(febCnt)
	{
		case 0:
		if(pSnrSet[SNRMENU_FEBTURN]>0)
		{
			para1-= feb1;
			para2-= feb2;
		}
		break;

		case 1:
		if(!pSnrSet[SNRMENU_FEBTURN])
		{
			para1-= feb1;
			para2-= feb2;
		}
		break;

		case 2:
		para1+=feb1;
		para2+=feb2;
		break;
	}

	ReStdPara(&para1,&para2);

#ifdef EXT_TEST_PRT
	printf("feb %d, %d\r\n",para1,para2);
#endif

	if(para1<0)
	{
		para1 = 0;
		para2 = 0;
	}else if(para1>=9)
	{
		para1 = 9;
		para2 = 0;
	}

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

	if(para1>EXTA_PA_ST)
	{
		EXT_PA = 1;
#ifdef EXT_TEST_PRT
		//printf("EXTA_PA\r\n");
#endif
	}else
	{
		EXT_PA = 0;
	}

#ifdef EXT_TEST_PRT
	//printf("para %d, %d\r\n",para1,para2);
#endif

	//HpExpCodeSend(g_hpExpCode[para1*3+para2]);	
	HpExpCodeSend((g_extExpCode[para1*3+para2]<<4));	

	//DspIsoValue(LAYOUT_PARA_AREA2, 60,g_hpExpCode[para1*3+para2],DSP_NORMAL);
	//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(3*8)+8,g_hpExpCode[para1*3+para2]>>8,DSP_NORMAL);
	//TtlDebugDspDat(3,LAYOUT_PARAEX_COLUMN-1+(5*8)+8,g_hpExpCode[para1*3+para2]&0xff,DSP_NORMAL);
#ifdef EXT_TEST_PRT
	printf("DA %d\r\n",g_extExpCode[para1*3+para2]);
#endif

	delay_us(100);

	return (para1*3+para2);
	

}

//void ExtModSet(void)
#define EXTM_DASET_STD 24//25//28//9
#define EXTM_PA_ST	17
/*ISO 100 APERT 5.6 1/128*/
//void ExtmModSet(signed char  foc, int iso, int apert)
u8 ExtmModSet(signed char  foc, int iso, int apert)
{

	//float multi=1;
	//float gn;
	u8 tmp;
	s8 para1=0;
//	char i=0;

	//float tmp=1;

	//char extVal = 0;

	//if(!motoSet.focIndex)

	para1+=EXTM_DASET_STD;

	if(!foc)
	{
		//motoMulti = 1;		
		para1 += 0;
	}else
	{
		//motoMulti = motoSet.focIndex - 1;
		//para1 = (motoSet.focIndex-1)/3;
		//para2 = ((motoSet.focIndex-1)%3);
		//para1 += (motoSet.focIndex-1);
		//para1 += (foc-1);
		//para1 += (foc);
		para1 += GetMotoIncremental(foc);
	}

	//para1+=EXTM_DASET_STD;

#ifdef EXT_TEST_PRT
	printf("para1 %d, iso %d, apert %d\r\n",para1,iso,apert);
#endif

	//tmp = ((flaPara.isoCmp>>4)*6)+(((flaPara.isoCmp&0x0F)+2)/3);
	//tmp = ExtmIsoIndex(mainPara.extIso);
	tmp = ExtmIsoIndex(iso);
	//para1 += tmp/3;
	//para2 += tmp%3;
	para1+=tmp;				//ISO

#ifdef EXT_TEST_PRT
	printf("para1 %d\r\n",para1);
#endif

	//tmp = ExtmApertIndex(mainPara.extApert);
	tmp = ExtmApertIndex(apert);

	//para1 -= tmp/3;
	//para2 -= tmp%3;
	para1-=tmp;				//APERT

#ifdef EXT_TEST_PRT
	printf("para1 %d\r\n",para1);
#endif

	//para1+=EXTM_DASET_STD;

//#ifdef EXT_TEST_PRT
//	printf("extm %d\r\n",para1);
//#endif

	if(para1<0)
	{
		para1 = EXT_VAL_LEN;
	}else if(para1>EXT_VAL_LEN)
	{
		para1 = 0;
	}else
	{
		para1 =EXT_VAL_LEN-para1;
	}

#ifdef EXT_TEST_PRT
	printf("extm %d\r\n",para1);
#endif

	if(para1>EXTM_PA_ST)
	{
		EXT_PA = 1;
#ifdef EXT_TEST_PRT
		printf("EXT_PA\r\n");
#endif
	}else
	{
		EXT_PA = 0;
	}

/*	

	for(i=0;i<motoSet.focIndex;i++)
	{
		multi *=  gnFocMul[i];//foclen 
	}

	multi *= calcSqrt(mainPara.extIso)/10;//iso

	multi *=10;

	multi/=mainPara.extApert;			//apert

	//ISO 100, APERT 5, FOC 1

	multi*=40;

	if(multi<2)
	{
		extVal = 0;
	}else
	{
		multi /=2 ;
		
		while(tmp<multi)
		{
			//tmp*=1.4;
			tmp*=1.12;
			extVal++;
		}
		
	}

	//g_hpExpCode
	if(extVal>21)
	{
		extVal = 0;
	}else
	{
		extVal = (21-extVal);
	}
*/
//	FLAOK_TOGGLE();

//	LCD_Clear(0);

//	DspIsoValue(LAYOUT_PARA_AREA2, 60,g_hpExpCode[para1],DSP_NORMAL);

	//HpExpCodeSend(g_hpExpCode[para1]);	
	HpExpCodeSend(g_extExpCode[para1]<<4);	

#ifdef EXT_TEST_PRT
	printf("extm %d\r\n",g_extExpCode[para1]);
#endif

	delay_us(100);
//	while(!PILOT_IN);
	return para1;

}

#ifdef EXT_TEST_PRT
void ExtCodeSetPrt(void)
{
	int i,j;
	for(i=0;i<34;i++)
	{
		for(j=0;j<40;j++)
		{
			ExtmModSet(0,isoValue[i],apertValue[j]);
		}
	}
}
#endif

#define LOWBAT_ICON_HT		32
#define LOWBAT_ICON_WT	35
void LoadLowBatIcon(void)
{
	DISABLE_RF_MODULE();
	CHARGE_LED_OFF();
	FLAOK_OFF();
	DISABLE_KEY_INT();
	ADC_DeInit(ADC1);
	SPI_I2S_DeInit(SPI1);			//2015/09/13
	DISABLE_CHARGE();				//2015/11/27
	
	//BK_TURNOFF(0);
	LCD_Clear(0);
	DisplayPicture((12-(LOWBAT_ICON_HT/8))/2, (LCD_W-LOWBAT_ICON_WT)/2, LOWBAT_ICON_WT, 
		LOWBAT_ICON_HT, font_lowBat, DSP_NORMAL);
}

void LoadUncompetibleIcon(void)
{
	DisplayPicture(LAYOUT_PARA_AREA2,LAYOUT_PFN_CML+21+1+3,14,16,font_window,DSP_NORMAL);
}

void* LoadCfnAllClear(void)
{
	DspClearAllPara();
	if(SETTING_SNRMENU==g_SettingIndex)
	{
		DisplayPicture(5,37,21,16,font_cfnClr1,DSP_NORMAL);
	}else if(SETTING_PFNMENU==g_SettingIndex)
	{
		DisplayPicture(5,37,21,16,font_pfnClr1,DSP_NORMAL);
	}
	DisplayPicture(5,37+21+4,21,16,font_cfnClr2,DSP_NORMAL);
	DisplayPicture(5,37+21+4+21+4,47,16,font_clear,DSP_REVERSE);
	DisplayPicture(5,37+21+4+21+4+47+1,5,16,font_wenhao,DSP_NORMAL);
	funcMenu.listCnt=0;
	g_menuId = 0;
	AddButtons(&mOK,&mNull,&mNull,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();

	return NULL;

}

#ifdef FIBER_ADJUST_PROC
extern void SetManuPara(unsigned char  output1,unsigned char output2,char rvs);
void FiberAdjustProc(void)
{
	LCD_Clear(0);
	
	DspSensorsEx(LAYOUT_SENSOR_PAGE_EX, LAYOUT_SENSOR_CML_EX, 1, !g_isRgbFilter,g_upDnSensor,0);
	SetManuPara(mstPara->output1,mstPara->output2,
				SETTING_FB_EXP==g_SettingIndex?DSP_SELE:DSP_NORMAL);
	DisplayTtlCmp(LAYOUT_RATIO_B, LAYOUT_PARAEX_COLUMN-3,
					mainPara.fiberCoe/3-3,mainPara.fiberCoe%3,
					(g_SettingIndex==SETTING_FB_ADJ?DSP_SELE:DSP_NORMAL));

	funcMenu.listCnt=0;
	g_menuId = 0;

	/*
	if(g_RgbCheckTurn>0)
	{
		AddButtons(&mRgbChk2,&mFbAdj,&mFbAddSub,&mBack,funcMenu.listCnt++);
	}else
	{
		AddButtons(&mRgbChk1,&mFbAdj,&mFbAddSub,&mBack,funcMenu.listCnt++);
	}*/
	switch(g_RgbCheckTurn)
	{
		case 0:
			AddButtons(&mRgbChk1,&mFbAdj,&mFbAddSub,&mBack,funcMenu.listCnt++);
		break;

		case 1:
			AddButtons(&mRgbChk2,&mFbAdj,&mFbAddSub,&mBack,funcMenu.listCnt++);
		break;

		case 2:
			AddButtons(&mRgbChk3,&mFbAdj,&mFbAddSub,&mBack,funcMenu.listCnt++);
		break;

		case 3:
			AddButtons(&mRgbChk4,&mFbAdj,&mFbAddSub,&mBack,funcMenu.listCnt++);
		break;

	}
	KeyMenuSw();
	
}
#endif









