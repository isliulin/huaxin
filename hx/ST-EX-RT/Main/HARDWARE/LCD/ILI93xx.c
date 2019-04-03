#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"
#include "delay.h"	 
#include "includes.h"
#include "../../USER/Golbal.h"
#include "../../USER/Display.h"
#include "../../USER/KeyProc.h"
#include "../../USER/canon.h"
#include "../STMFLASH/stmflash.h"
#include "../../USER/menu.h"
#include <string.h>

//u16 g_penColor = BLACK;//BACK_COLOR = LIGHTGREEN;  
u16 DeviceCode;	 
extern unsigned char g_dspStyle;
extern char g_SettingIndex;
extern unsigned char uart_tran_flag;
//extern unsigned char g_wlCmdMod;
extern unsigned char *g_cmdMode;
extern unsigned char g_menuId;
extern MultiSet multiSet;
extern unsigned char g_grSel;
extern RadioSet* radioSet;
extern char g_idSetIndex;
extern SnMac g_snMac;
extern char *pSnrSet;
char g_isChScan=0;
u8 g_isSmExpand =0;
extern RadioStatue radioStatue;
extern unsigned char g_grSelRsv;
extern char g_SettingRsv;
extern unsigned char g_grStyle;

extern signed char g_rfChanSet;
extern signed char g_liteChanSet;
extern unsigned int g_rfIdSet;
extern u8 g_dspChinese;
extern char g_cmrLinked;
u16 g_hGpiob;

#define GR_DSP_LEN	17
#define ABr_DSP_LEN	15

u8 g_fbMem[220*176/8];
u8 g_fbSelMem[220*176/8];

u16 g_horiStart = 0;
u16 g_horiEnd = 0;
u16 g_vertiStart = 0;
u16 g_vertiEnd = 0;

const char cmpArr[][4] = 
{
	"0.0","0.3","0.7","1.0","1.3","1.7","2.0","2.3","2.7","3.0","3.3","3.7","4.0","4.3","4.7","5.0","5.3","5.7","6.0",
};

const char outputString[] ="1/128..1/64..1/32..1/16..1/8..1/4..1/2..1/1";
const char otpStrIndex[] = {3,5,6,9,11,12,15,17,18,21,23,24,26,28,29,31,33,34,36,38,39,41};
const char cmpString[] = "?3..2..1..;..1..2.'3";
const char abRatio[] = "8:1 . 4:1 . 2:1 . 1:1 . 1:2 . 1:4 . 1:8";

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
void *BtnSynSet(void);
void *BtnChScan(void);
void *BtnGrStyle(void);
//void BtnRfGrMod(void);
extern void KeyOk(void);
void AddButtons(Button *btn1, Button *btn2, Button *btn3, Button *btn4,unsigned char listCnt);
extern void CanSynMode(signed char mode,signed char sm,char mDsp);
void DisplayMstSetDot(u16 x, u16 y,u8 rsv);



Button mNull = {0,"","",0,0,{NULL,NULL}};
#ifdef _ST_EX_
//Button cfn={0,"C.Fn",(sizeof("C.Fn")-1)*FONT_BUTTON_WT-6,{NULL,BtnSnrMenu}};
Button cfn={0,"C.Fn","自定义",(sizeof("C.Fn")-1)*FONT_BUTTON_WT-6,(sizeof("自定义")-1)*FONT_BUTTON_WT,{NULL,BtnSnrMenu}};
#define mstFlaSw mNull
#else
Button cfn={0,"Zm/C.Fn",(sizeof("Zm/C.Fn")-1)*FONT_BUTTON_WT-6,{BtnZoon,NULL}};
Button mstFlaSw = {1,(char*)mstSw,78,{BtnMstSw,NULL}};
#endif
Button addSub = {0,"+/-","补偿",(sizeof("+/-")-1)*FONT_BUTTON_WT,(sizeof("增减")-1)*FONT_BUTTON_WT,{BtnAddSub,NULL}};
Button ttlFeb = {0,"FEB","包围",(sizeof("FEB")-1)*FONT_BUTTON_WT,(sizeof("包围")-1)*FONT_BUTTON_WT,{BtnTtlFeb,NULL}};
Button menu1 = {0,"MENU1","菜单一",(sizeof("MENU1")-1)*FONT_BUTTON_WT,(sizeof("菜单一")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button menu2 = {0,"MENU2","菜单二",(sizeof("MENU2")-1)*FONT_BUTTON_WT,(sizeof("菜单二")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button menu3 = {0,"MENU3","菜单三",(sizeof("MENU3")-1)*FONT_BUTTON_WT,(sizeof("菜单三")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button menu4 = {0,"MENU4","菜单四",(sizeof("MENU4")-1)*FONT_BUTTON_WT,(sizeof("菜单四")-1)*FONT_BUTTON_WT,{BtnMenu,NULL}};
Button mRatio = {0,"RATIO","分组",(sizeof("RATIO")-1)*FONT_BUTTON_WT,(sizeof("分组")-1)*FONT_BUTTON_WT,{BtnRatio,NULL}};
Button mGr = {0,"GR","选择",(sizeof("GR")-1)*FONT_BUTTON_WT,(sizeof("选择")-1)*FONT_BUTTON_WT,{BtnGrSel,NULL}};
Button mCh = {0,"CH","频道",(sizeof("CH")-1)*FONT_BUTTON_WT,(sizeof("频道")-1)*FONT_BUTTON_WT,{BtnChSet,NULL}};
Button mId = {0,"ID","ID",(sizeof("ID")-1)*FONT_BUTTON_WT,(sizeof("艾第")-1)*FONT_BUTTON_WT,{BtnIdSet,NULL}};
Button mScan = {0,"SCAN","搜索",(sizeof("SCAN")-1)*FONT_BUTTON_WT,(sizeof("搜索")-1)*FONT_BUTTON_WT,{BtnChScan,NULL}};
//Button mSync = {0,"SYNC","同步",(sizeof("SYNC")-1)*FONT_BUTTON_WT,(sizeof("同步")-1)*FONT_BUTTON_WT,{BtnSynSet,NULL}};
Button mSync_fr = {0,"SYNC","后帘",(sizeof("SYNC")-1)*FONT_BUTTON_WT,(sizeof("后帘")-1)*FONT_BUTTON_WT,{BtnSynSet,NULL}};
Button mSync_fh = {0,"SYNC","高速",(sizeof("SYNC")-1)*FONT_BUTTON_WT,(sizeof("高速")-1)*FONT_BUTTON_WT,{BtnSynSet,NULL}};
Button mSync_rh = {0,"SYNC","高速",(sizeof("SYNC")-1)*FONT_BUTTON_WT,(sizeof("高速")-1)*FONT_BUTTON_WT,{BtnSynSet,NULL}};
Button mSync_hp = {0,"SYNC","前帘",(sizeof("SYNC")-1)*FONT_BUTTON_WT,(sizeof("前帘")-1)*FONT_BUTTON_WT,{BtnSynSet,NULL}};
Button mMemory = {0,"MEMORY","保存",(sizeof("MEMORY")-1)*FONT_BUTTON_WT,(sizeof("保存")-1)*FONT_BUTTON_WT,{BtnMemory,NULL}};
Button mMulti = {0,"MULTI","次数",(sizeof("MULTI")-1)*FONT_BUTTON_WT,(sizeof("次数")-1)*FONT_BUTTON_WT,{BtnMulti,NULL}};
Button mFrq = {0,"Hz","频率",(sizeof("Hz")-1)*FONT_BUTTON_WT,(sizeof("频率")-1)*FONT_BUTTON_WT,{BtnFrq,NULL}};
Button mCancel = {0,"HOME","返回",(sizeof("HOME")-1)*FONT_BUTTON_WT,(sizeof("返回")-1)*FONT_BUTTON_WT,{BtnHome,NULL}};
Button mIdSel = {0, "SEL","设置",(sizeof("SEL")-1)*FONT_BUTTON_WT,(sizeof("设置")-1)*FONT_BUTTON_WT,{BtnIdSel,NULL}};
Button mReset = {0, "Reset","",(sizeof("Reset")-1)*FONT_BUTTON_WT,(sizeof("")-1)*FONT_BUTTON_WT,{BtnReset,NULL}};
Button mRel = {0,"Rel","释放",(sizeof("Rel")-1)*FONT_BUTTON_WT,(sizeof("释放")-1)*FONT_BUTTON_WT,{BtnRel,NULL}};//linked shot mode to triger the shutter;
Button mSmSel = {0,"SEL","设置",(sizeof("SEL")-1)*FONT_BUTTON_WT,(sizeof("设置")-1)*FONT_BUTTON_WT,{BtnSmSel,NULL}};
Button mSmSubSel = {0,"SUB","子选项",(sizeof("SUB")-1)*FONT_BUTTON_WT,(sizeof("子选项")-1)*FONT_BUTTON_WT,{BtnSmSub,NULL}};
Button mGrStyle = {0,"STYLE","风格",(sizeof("STYLE")-1)*FONT_BUTTON_WT,(sizeof("风格")-1)*FONT_BUTTON_WT,{BtnGrStyle,NULL}};


FuncMenu funcMenu;

void LcdDelay()
{
	return;
}

#define LCD_WR_DATA(dat)	\
{\
	LCD_RS_SET;\
	DATAOUT(dat>>8);\
   	LCD_WR_CLR;\
	LCD_WR_SET;\
	DATAOUT(dat&0xff);\
	LCD_WR_CLR;\
	LCD_WR_SET;\
}

#ifdef LCD_DRIVER_SUMSUNG

void LCD_Writ_Bus(u8 VH,u8 VL)   //并行数据写入函数
{	
    	DATAOUT(VH);	
   	LCD_WR_CLR;//LCD_WR=0;
	LCD_WR_SET;//LCD_WR=1;
	DATAOUT(VL);		
	LCD_WR_CLR;//LCD_WR=0;
	LCD_WR_SET;//LCD_WR=1;
}

void LCD_WR_DATA8(u8 VH,u8 VL) //发送数据-8位参数
{
	LCD_RS_SET;//LCD_RS=1;
	LCD_Writ_Bus(VH,VL);
}

//void LCD_WR_DATA(u16 da)
//{
//    	LCD_RS_SET;//LCD_RS=1;
//	LCD_Writ_Bus(da>>8,da);
//}

void LCD_WR_REG(u16 da)	 
{	
	SET_LCDCS_VALID();
    	LCD_RS_CLR;//LCD_RS=0;
	LCD_Writ_Bus(da>>8,da);
	SET_LCDCS_INVALID();
}


//写寄存器
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_WORD(LCD_RegValue);
}	 

void AddrSet(u16 x,u16 y)
{ 
 
	//LCD_WR_REG(0x0037);LCD_WR_DATA(x1); // windows address 
	//LCD_WR_REG(0x0036);LCD_WR_DATA(x2);
	//LCD_WR_REG(0x0039);LCD_WR_DATA(y1);    
	//LCD_WR_REG(0x0038);LCD_WR_DATA(y2);	
	LCD_WR_REG(0x0020); 	//HS
	LCD_WR_WORD(x); 	
	LCD_WR_REG(0x0021);  //VS
	LCD_WR_WORD(y);	
	LCD_WR_REG(0x0022);  // write ram
}

void AreaSet(u16 x1,u16 y1,u16 x2,u16 y2)
{
/*
	LCD_WR_REG(0x0037);LCD_WR_DATA(LCD_HEND-x2); // windows address 
	LCD_WR_REG(0x0036);LCD_WR_DATA(LCD_HEND-x1);
	LCD_WR_REG(0x0039);LCD_WR_DATA(y1);    
	LCD_WR_REG(0x0038);LCD_WR_DATA(y2);
	AddrSet(LCD_HEND-x2,y1);
*/
	LCD_WR_REG(0x37);
       LCD_WR_WORD(x1); // windows address 
	LCD_WR_REG(0x36);
	LCD_WR_WORD(x2);
	LCD_WR_REG(0x39);
	LCD_WR_WORD(y1);   
	LCD_WR_REG(0x38);
	LCD_WR_WORD(y2);
	AddrSet(x1,y1);
}

#endif

#ifdef LCD_DRIVER_ILI9320
/*
#define LCD_WRITE_DATA(Data)  \	
{\
	LCD_CS = 0;\
	LCD_RS = 1;\
	LCD_RD = 1;\	
  	DATAOUT(Data);\
	LCD_WR = 0;\
	LCD_WR = 1;\
	LCD_CS = 1;\
	LCD_RS = 0;\
}
*/
void LCD_Writ_Bus(u8 VH,u8 VL)   //并行数据写入函数
{	
    	DATAOUT(VH);	
   	LCD_WR_CLR;//LCD_WR=0;
	LCD_WR_SET;//LCD_WR=1;
	DATAOUT(VL);		
	LCD_WR_CLR;//LCD_WR=0;
	LCD_WR_SET;//LCD_WR=1;
}

void LCD_SET_REG(Uchar index)		   	//写命令到TFT
{
	LCD_CS_CLR;//LCD_CS = 0;
	LCD_RS_CLR;//LCD_RS = 0;
	LCD_RD_SET;//LCD_RD = 1;
	
	//DATA   = index;
	DATAOUT(0x00);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;	

	DATAOUT(index);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;	

	LCD_RS_SET;//LCD_RS = 1;	
	//LCD_CS_SET;//LCD_CS = 1;

}

/*
void LCD_WR_REG(u8 reg, u16 dat)
{
	LCD_SET_REG(reg);
	LCD_WR_DATA(dat);
}
*/

void LCD_WR_REG(u8 reg, u16 dat)
{
	LCD_CS_CLR;//LCD_CS = 0;
	LCD_RS_CLR;//LCD_RS = 0;
	LCD_RD_SET;//LCD_RD = 1;

	DATAOUT(0x00);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;

	DATAOUT(reg);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;

	LCD_RS_SET;

	DATAOUT(dat>>8);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;

	DATAOUT(dat&0xff);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;

	LCD_CS_SET;
	
}

void LCD_RD_REG(u8 reg,u16* rDat)
{
	
	LCD_CS_CLR;//LCD_CS = 0;
	LCD_RS_CLR;//LCD_RS = 0;
	LCD_RD_SET;//LCD_RD = 1;

	DATAOUT(0x00);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;

	DATAOUT(reg);

	LCD_WR_CLR;//LCD_WR = 0;
	LCD_WR_SET;//LCD_WR = 1;

	LCD_RS_SET;

	LCD_RD_CLR;

	GPIOB->CRL = 0x88888888;

	*rDat = (DATAIN<<8)&0xFF00;

	LCD_RD_SET;
	LCD_RD_CLR;	

	*rDat |= (DATAIN&0xFF);

	LCD_RD_SET;
	LCD_CS_SET;
	
	GPIOB->CRL = 0x33333333;
	

	

}

void AddrSet(u16 x,u16 y)
{ 
	//LCD_CS_SET;
	LCD_WR_REG(0x20,x);
	LCD_WR_REG(0x21,y);
	LCD_SET_REG(0x22);
}

void AreaSet(u16 x1,u16 y1,u16 x2,u16 y2)
{
/*
	LCD_WR_REG(0x0037);LCD_WR_DATA(LCD_HEND-x2); // windows address 
	LCD_WR_REG(0x0036);LCD_WR_DATA(LCD_HEND-x1);
	LCD_WR_REG(0x0039);LCD_WR_DATA(y1);    
	LCD_WR_REG(0x0038);LCD_WR_DATA(y2);
	AddrSet(LCD_HEND-x2,y1);
	
	LCD_WR_REG(0x0037);LCD_WR_DATA(x1); // windows address 
	LCD_WR_REG(0x0036);LCD_WR_DATA(x2);
	LCD_WR_REG(0x0039);LCD_WR_DATA(y1);    
	LCD_WR_REG(0x0038);LCD_WR_DATA(y2);

	LCD_WR_REG(0x0050,0x00);//水平 GRAM起始位置
	LCD_WR_REG(0x0051,239);//水平GRAM终止位置
	LCD_WR_REG(0x0052,0x00);//垂直GRAM起始位置
	LCD_WR_REG(0x0053,319);//垂直GRAM终止位置   
*/
/*
	LCD_WR_REG(0x50);LCD_WR_DATA(x1);
	LCD_WR_REG(0x51);LCD_WR_DATA(x2);
	LCD_WR_REG(0x52);LCD_WR_DATA(y1);
	LCD_WR_REG(0x53);LCD_WR_DATA(y2);	
*/
	LCD_WR_REG(0x50,x1);//LCD_WR_DATA(x1);
	LCD_WR_REG(0x51,x2);//LCD_WR_DATA(x2);
	LCD_WR_REG(0x52,y1);//LCD_WR_DATA(y1);
	LCD_WR_REG(0x53,y2);//LCD_WR_DATA(y2);	
	
	AddrSet(x1,y1);
}

#endif


#ifdef LCD_DRIVER_SUMSUNG

void LCD_Init(void)
{
	
	//RCC->APB2ENR|=1<<2;//先使能外设PORTA时钟
 	//RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	//RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	//RCC->APB2ENR|=1<<0;    //开启辅助时钟
	//JTAG_Set(SWD_ENABLE);  //开启SWD
#if(_HARD_WARE_==HARD_WARE_RFONLY)
	JTAG_Set(JTAG_SWD_DISABLE);
	//JTAG_Set(SWD_ENABLE);
	//RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
#else
	JTAG_Set(SWD_ENABLE);
#endif
	
	//GPIOC->CRH&=0X00000FFF;
	//GPIOC->CRH|=0X33333000; 
	
	GPIOC->ODR|=0XF800; 

	//PORTB 推挽输出 	
	//GPIOB->CRH=0X33333333;
	GPIOB->CRL = 0X33333333; 	 
	//GPIOB->ODR|=0XFF;

	GPIOC->CRH &= 0x0000FFFF;
	GPIOC->CRH |= 0x33330000;

	//GPIOB->CRH &= 0xFFFFFF0F; 
	//GPIOB->CRH |= 0x00000030;
	//turn off back light first
	//BK_LIGHT
	//GPIOB->ODR&= 0xFDFF;	

#ifdef LCD_RESET_MANU
   	LCD_CS_SET;//LCD_CS =1;
   	
	LCD_RESET_SET;//LCD_REST=1;
	delay_ms(100);	
	LCD_RESET_CLR;//LCD_REST=0;
	delay_ms(100);
	LCD_RESET_SET;//LCD_REST=1;
#endif

#if(_HARD_WARE_==HARD_WARE_RFONLY)
	GPIOB->CRH &= 0xFFFFFFF0; 
	GPIOB->CRH |= 0x00000003;
	delay_ms(100);
	LCD_RST = 1;
#endif
	g_hGpiob = DATAIN&0xff00;

	LCD_CS_SET;//LCD_CS=1;
	LCD_RD_SET;//LCD_RD=1;
	LCD_WR_SET;//LCD_WR=1;
	delay_ms(5);
	//LCD_CS_CLR;//LCD_CS =0;  //打开片选使能
	
	LCD_WR_REG(0x0011);LCD_WR_WORD(0x001A);
	LCD_WR_REG(0x0012);LCD_WR_WORD(0x3121);
	LCD_WR_REG(0x0013);LCD_WR_WORD(0x006C);
	LCD_WR_REG(0x0014);LCD_WR_WORD(0x4245);
	LCD_WR_REG(0x0010);LCD_WR_WORD(0x0800);
	delay_ms(10);
	LCD_WR_REG(0x0011);LCD_WR_WORD(0x011A);
	delay_ms(10);
	LCD_WR_REG(0x0011);LCD_WR_WORD(0x031A);
	delay_ms(10);
	LCD_WR_REG(0x0011);LCD_WR_WORD(0x071A);
	delay_ms(10);
	LCD_WR_REG(0x0011);LCD_WR_WORD(0x0F1A);
	delay_ms(20);
	LCD_WR_REG(0x0011);LCD_WR_WORD(0x0F3A);
	delay_ms(30);
	/*Initialization set sequence*/
	LCD_WR_REG(0x0001);LCD_WR_WORD(0x031C);//LCD_WR_DATA(0x011C);
	LCD_WR_REG(0x0002);LCD_WR_WORD(0x0100);
	LCD_WR_REG(0x0003);LCD_WR_WORD(0x1030);
	LCD_WR_REG(0x0007);LCD_WR_WORD(0x0000);
	LCD_WR_REG(0x0008);LCD_WR_WORD(0x0808);
	LCD_WR_REG(0x000B);LCD_WR_WORD(0x1100);
	LCD_WR_REG(0x000C);LCD_WR_WORD(0x0000);
	LCD_WR_REG(0x000F);LCD_WR_WORD(0x1401);
	LCD_WR_REG(0x0015);LCD_WR_WORD(0x0000);
	LCD_WR_REG(0x0020);LCD_WR_WORD(0x0000);//RAM Address Register AD7-AD0
	LCD_WR_REG(0x0021);LCD_WR_WORD(0x0000);//RAM Address Register AD15-AD8

	LCD_WR_REG(0x0038);LCD_WR_WORD(0x00DB);
	LCD_WR_REG(0x0039);LCD_WR_WORD(0x0000);
	LCD_WR_REG(0x0050);LCD_WR_WORD(0x0001);//Gamma
	LCD_WR_REG(0x0051);LCD_WR_WORD(0x020B);
	LCD_WR_REG(0x0052);LCD_WR_WORD(0x0805);
	LCD_WR_REG(0x0053);LCD_WR_WORD(0x0404);
	LCD_WR_REG(0x0054);LCD_WR_WORD(0x0C0C);
	LCD_WR_REG(0x0055);LCD_WR_WORD(0x000C);
	LCD_WR_REG(0x0056);LCD_WR_WORD(0x0101);
	LCD_WR_REG(0x0057);LCD_WR_WORD(0x0400);
	LCD_WR_REG(0x0058);LCD_WR_WORD(0x1108);
	LCD_WR_REG(0x0059);LCD_WR_WORD(0x050C);

	LCD_WR_REG(0x0036);LCD_WR_WORD(0x00AF);//H end
	LCD_WR_REG(0x0037);LCD_WR_WORD(0x0000);//H start
	LCD_WR_REG(0x0038);LCD_WR_WORD(0x00DB);//V end
	LCD_WR_REG(0x0039);LCD_WR_WORD(0x0000);//V start
	//LCD_WR_REG(0x0036);LCD_WR_DATA(0x0000);//H end
	//LCD_WR_REG(0x0037);LCD_WR_DATA(0x00AF);//H start
	//LCD_WR_REG(0x0038);LCD_WR_DATA(0x00DB);//V end
	//LCD_WR_REG(0x0039);LCD_WR_DATA(0x0000);//V start

	LCD_WR_REG(0x000F);LCD_WR_WORD(0x0B01);
	LCD_WR_REG(0x0007);LCD_WR_WORD(0x0016);
	delay_ms(2);
	LCD_WR_REG(0x0007);LCD_WR_WORD(0x0017);
	LCD_WR_REG(0x0022);
	delay_ms(10);
}

void LCD_StandBy(void)
{
	LCD_WR_REG(0x0015);LCD_WR_WORD(0x0000);
	LCD_WR_REG(0x0007);LCD_WR_WORD(0x0012);
	delay_ms(2);
	LCD_WR_REG(0x0007);LCD_WR_WORD(0x0000);
	delay_ms(1);
	LCD_WR_REG(0x0010);LCD_WR_WORD(0x0002);
	LCD_CS_SET;	

}

void LCD_StandByExit(void)
{
	LCD_CS_CLR;	
	delay_us(200);
	LCD_CS_SET;	
	delay_us(200);

	LCD_CS_CLR;	
	delay_us(200);
	LCD_CS_SET;	
	delay_us(200);

	LCD_CS_CLR;	
	delay_us(200);
	LCD_CS_SET;	
	delay_us(200);

	LCD_CS_CLR;	
	delay_us(200);
	LCD_CS_SET;	
	delay_us(200);

	LCD_CS_CLR;	
	delay_us(200);
	LCD_CS_SET;	
	delay_us(200);

	LCD_CS_CLR;	
	delay_us(200);
	LCD_CS_SET;	
	delay_us(200);

	//LCD_CS_CLR;	
	
	//LCD_WR_REG(0x000F);LCD_WR_DATA(0x0B01);
	//LCD_WR_REG(0x0007);LCD_WR_DATA(0x0012);
	//delay_ms(2);
	//LCD_WR_REG(0x0007);LCD_WR_DATA(0x0017);

	LCD_Init();
	
}
#endif

#ifdef LCD_DRIVER_ILI9320

void LCD_Init(void)
{
	JTAG_Set(SWD_ENABLE);
	
	GPIOB->CRL = 0X33333333;

	GPIOC->CRH &= 0x000FFFFF;
	GPIOC->CRH |= 0x33300000;

	GPIOE->CRL &= 0xF0FFFFFF;
	GPIOE->CRL |= 0x03000000;

	LCD_WR_REG(0x00,0x0001);
	delay_ms(20);

	LCD_WR_REG(0x00,0x0000);
	LCD_WR_REG(0x01,0x0100);	//Driver Output Contral.
	LCD_WR_REG(0x02,0x0700);	//LCD Driver Waveform Contral.
	LCD_WR_REG(0x03,0x1030);//Entry Mode Set.
	//LCD_WR_REG(0x03,0x1018);	//Entry Mode Set.

	LCD_WR_REG(0x04,0x0000);	//Scalling Contral.
	LCD_WR_REG(0x08,0x0202);	//Display Contral 2.(0x0207)
	LCD_WR_REG(0x09,0x0000);	//Display Contral 3.(0x0000)
	LCD_WR_REG(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
	LCD_WR_REG(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
	LCD_WR_REG(0x0d,0x0000);	//Frame Maker Position.
	LCD_WR_REG(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
	delay_ms(50); 
	LCD_WR_REG(0x07,0x0101);	//Display Contral.
	delay_ms(50); 								  
	LCD_WR_REG(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
	LCD_WR_REG(0x11,0x0007);								//Power Control 2.(0x0001)
	LCD_WR_REG(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
	LCD_WR_REG(0x13,0x0b00);								//Power Control 4.
	LCD_WR_REG(0x29,0x0000);								//Power Control 7.

	LCD_WR_REG(0x2b,(1<<14)|(1<<4));	    
	LCD_WR_REG(0x50,0);	//Set X Star
	//水平GRAM终止位置Set X End.
	LCD_WR_REG(0x51,239);	//Set Y Star
	LCD_WR_REG(0x52,0);	//Set Y End.t.
	LCD_WR_REG(0x53,319);	//

	LCD_WR_REG(0x60,0xA700);	//Driver Output Control.
	LCD_WR_REG(0x61,0x0001);	//Driver Output Control.
	LCD_WR_REG(0x6a,0x0000);	//Vertical Srcoll Control.

	LCD_WR_REG(0x80,0x0000);	//Display Position? Partial Display 1.
	LCD_WR_REG(0x81,0x0000);	//RAM Address Start? Partial Display 1.
	LCD_WR_REG(0x82,0x0000);	//RAM Address End-Partial Display 1.
	LCD_WR_REG(0x83,0x0000);	//Displsy Position? Partial Display 2.
	LCD_WR_REG(0x84,0x0000);	//RAM Address Start? Partial Display 2.
	LCD_WR_REG(0x85,0x0000);	//RAM Address End? Partial Display 2.

	LCD_WR_REG(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
	LCD_WR_REG(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
	LCD_WR_REG(0x93,0x0001);	//Panel Interface Contral 3.
	LCD_WR_REG(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
	LCD_WR_REG(0x97,(0<<8));	//
	LCD_WR_REG(0x98,0x0000);	//Frame Cycle Contral.	   
	LCD_WR_REG(0x07,0x0173);	//(0x0173)

}

#endif


//void LCD_DrawPoint(u16 x,u16 y,)
void LCD_DrawPoint(u16 x,u16 y, u8 rvs)
{
	u16 dspColor;
	//AddrSet(LCD_H-x,y,LCD_H-x,y);//设置光标位置 
	//LCD_WR_REG(0x0020); 	//HS
	//LCD_WR_DATA(LCD_H-x); 		 
	//LCD_WR_REG(0x0021);  //VS
	//LCD_WR_DATA(y); 
	//LCD_WR_REG(0x0022);
	//AddrSet(LCD_H-x,y);
	//AddrSet(x,y);
	dspColor = (rvs&DSP_SELE)?SEL_COLOR:(rvs&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
	AreaSet(x,y,x,y);
	//LCD_WR_DATA(g_penColor); 	    
	LCD_WR_WORD(dspColor); 
} 

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
	//u8 VH,VL;
	u16 i,j;
	//VH=Color>>8;
	//VL=Color;	
	g_hGpiob = DATAIN&0xff00;
	AreaSet(0,0,LCD_H-1,LCD_W-1);
	//AddrSet(30,30,80,80);
	SET_LCDCS_VALID();
    	for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   {
        	 //LCD_WR_DATA8(VH,VL);
        	 LCD_WR_DATA(Color);
		//delay_ms(1);
	    }

	  }
	SET_LCDCS_INVALID();
}


void LcdClearFeild(u16 x, u16 y, u16 wt, u16 ht)
{
	//LCD_Fill(y, x, y+wt, y+ht, BACK_COLOR);
	u16 i=0;//, y0=y;
	u8 j=0;
	u16 dspColor;
	//u8 k=0;
	//g_penColor = BACK_COLOR;
	//g_penColor = bkSetArr[pSnrSet[SNRMENU_BKCLR]];
	dspColor = bkSetArr[pSnrSet[SNRMENU_BKCLR]];
	g_hGpiob = DATAIN&0xff00;;
	//AreaSet((LCD_HEND-x-ht+1),y+1,(LCD_HEND-x),y+wt);
	AreaSet(x, y, x+ht-1, y+wt-1);
	SET_LCDCS_VALID();
	//AddrSet(,y,(LCD_WEND-x),y+wt-1);
	for(i=0;i<wt;i++)
	{
		for(j=0;j<ht;j++)
		{
			//g_penColor = BACK_COLOR;
			//LCD_DrawPoint(x++,y+i);
			//LCD_WR_DATA(g_penColor);
			LCD_WR_DATA(dspColor);
			//LCD_WR_DATA8(VH,VL);
		}
		//x = x0;
	}
	SET_LCDCS_INVALID();
}

void LcdGrAllClear(void)
{
	LCD_CLEAR_GR(LAYOUT_RATIO_A);
	LCD_CLEAR_GR(LAYOUT_RATIO_B);
	LCD_CLEAR_GR(LAYOUT_RATIO_C);
}

void ClearRfGrSelBar(void)
{
	if(g_grStyle>0)
	{
		LcdClearFeild(LAYOUT_GR_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT+1,BIG_FONT_HIGHT(5)+LINE_SPACING(4)+1);
	}else
	{
		LcdClearFeild(LAYOUT_RATIO_A,LAYOUT_SELBAR_CML,LAYOUT_LKDEV_WT+1,BIG_FONT_HIGHT(3)+LINE_SPACING(2)+1);
	}
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
	if(g_dspChinese>0)
	{
		LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE-2);
	}else
	{
		LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	}
	//LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	
	
}



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
	//g_penColor = PEN_COLOR;
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

#if 0
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  
	u8 temp;
	u8 pos,t;
	u16 x0=x;
	//u16 colortemp=g_penColor;      
    if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	    
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)g_penColor=PEN_COLOR;
				else g_penColor=BACK_COLOR;
				LCD_DrawPoint(x,y);	
				temp>>=1; 
				x++;
		    }
			x=x0;
			y++;
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点     
		        temp>>=1; 
		    }
		}
	}
	g_penColor=PEN_COLOR;	    	   	 	  
}   
#endif

#if 0
void LCD_ShowChar(u16 x,u16 y,u8 num, u8 font,u8 rsvMod)
{
	u8 temp;
	u8 pos,t;
	u8 fontWidth;
	u8 pageEnd=0,paEle=0;//一页的宽带和换页的递增数
	u16 x0=x,y0=y;   
	//u8 (*pFont)[36];
	u8 *pFont=NULL;
	if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY) return;	
	num = num - ' ';
	//for(pos=0;pos<16;pos++)
	switch(font)
	{
		case FONT_16x08:
			fontWidth = 16;
			pFont = (u8*)(&asc2_1608[num][0]);
			pageEnd = 7;
			paEle = 8;
		break;

#if	0
		case FONT_16x08B:
			fontWidth = 16;
			pFont = (u8*)(&asc2_1608B[num][0]);
			pageEnd = 7;
			paEle = 8;
		break;
#endif

#ifdef _320x240_
		case FONT_24x12:
			fontWidth = 36;
			pFont = (u8*)(&asc2_2412[num][0]);
			pageEnd = 11;
			paEle = 12;
		break;
#endif

#ifdef _320x240_
		case FONT_32x16:
			fontWidth = 64;
			pFont = (u8*)(&asc2_3216[num][0]);
			pageEnd = 15;
			paEle = 16;
		break;
#endif

#ifdef _220x176_
		case FONT_12x06:
			fontWidth = 12;
			pFont = (u8*)(&asc2_1206[num][0]);
			pageEnd = 5;
			paEle = 6;
		break;

		case FONT_20x10:
			fontWidth = 30;
			pFont = (u8*)(&asc2_20x10[num][0]);
			pageEnd = 9;
			paEle = 10;
		break;
#endif
	}

/*
	if(FONT_16x08==font)
	{
		fontWidth = 16;
		pFont = (u8*)(&asc2_1608[num][0]);
		pageEnd = 7;
		paEle = 8;
	}else if(FONT_24x12==font)
	{
		fontWidth = 36;
		pFont = (u8*)(&asc2_2412[num][0]);
		pageEnd = 11;
		paEle = 12;
	}else if(FONT_32x16==font)
	{
		fontWidth = 64;
		pFont = (u8*)(&asc2_1632[num][0]);
		pageEnd = 15;
		paEle = 16;
	}
*/
	//for(pos=0;pos<36;pos++)
	
	for(pos=0;pos<fontWidth;pos++)
	{
		//temp=asc2_2412[num][pos];
		//temp = pFont[num][pos];
		temp = pFont[pos];
		//AreaSet();
		/*
		if(FONT_16x08==font)
		{
			temp=asc2_1608[num][pos];
		}else if(FONT_24x12==font)
		{
			temp=asc2_2412[num][pos];
		}*/
		//AreaSet(x,y,x+8,y+);
		for(t=0;t<8;t++)
		{
			if(temp&0x01)
			{
				g_penColor=(rsvMod)? BACK_COLOR:PEN_COLOR;//PEN_COLOR:BACK_COLOR;
				//LCD_DrawPoint(x,y);	
			}else
			{
				g_penColor=(rsvMod)? PEN_COLOR:BACK_COLOR;//BACK_COLOR:PEN_COLOR;
			}
			LCD_DrawPoint(x,y);	
			//LCD_WR_DATA(g_penColor);
			temp>>=1; 
			x++;
		}
		x=x0;
		y++;
		//if(7==pos)
		//if((11==pos)||(23==pos))
		if(pos==pageEnd)
		{
			pageEnd+=paEle;
			x0+=8;
			x = x0;
			y = y0;
		}
	}
}
#endif

u8 CnCharCmp(u8* cnChar, const char *cnArr)
{
	if((cnChar[0]==cnArr[0])&&(cnChar[1]==cnArr[1]))
	{
		return 0;
	}else
	{
		return 1;
	}
}

u8 SearchCnIndex(u8* cnChar)
{
	u8 cnIndex;
	for(cnIndex=0;cnIndex<CN_CHAR_TOTAL;cnIndex++)
	{
		if(!CnCharCmp(cnChar,cn_1608_index[cnIndex]))
		{
			return cnIndex;
		}
	}
	return 0xFF;
}

void LCD_ShowCnChar(u16 x, u16 y, u8* cnChar,u8 rvs)
{
	u8 sIndex;
	u8 temp;
	u8 pos,t,paEle=0;
	u16 dspColor;
	sIndex = SearchCnIndex(cnChar);
	if(0xFF==sIndex)
	{
		return;
	}
	//AreaSet(x,y,x+8-1,y+16-1);
	//SET_LCDCS_VALID();
	g_hGpiob = DATAIN&0xff00;;
	for(paEle=0;paEle<2;paEle++)
	{
		AreaSet(x+paEle*8,y,x+paEle*8+8-1,y+16-1);
		SET_LCDCS_VALID();
		for(pos=0;pos<16;pos++)
		{
			temp = cn_1608[sIndex][pos+paEle*16];
			for(t=0;t<8;t++)
				{
					if(temp&0x01)
					{
						//g_penColor=(rsvMod&(DSP_SELE))? SEL_COLOR:((rsvMod&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//PEN_COLOR:BACK_COLOR;
						dspColor=(rvs&(DSP_SELE))? SEL_COLOR:((rvs&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]]);
						//g_penColor=(rsvMod)? SEL_COLOR:PEN_COLOR;
						//LCD_DrawPoint(x,y);	
					}else
					{
						//g_penColor=(rsvMod&(DSP_REVERSE))? PEN_COLOR:BACK_COLOR;//BACK_COLOR:PEN_COLOR;
						dspColor=(rvs&(DSP_REVERSE))? penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
						//g_penColor = BACK_COLOR;
					}
					LCD_WR_DATA(dspColor);
					temp>>=1; 
				}
			
		}
		SET_LCDCS_INVALID();
	}
	
	
}

#if 1
void LCD_ShowChar(u16 x,u16 y,u8 num, u8 font,u8 rsvMod)
{
	u8 temp=0;
	u8 pos,t;
	u8 fontWidth=0;
	u16 dspColor;
	u8 pageEnd=0,paEle=0;//一页的宽带和换页的递增数   
	//u8 (*pFont)[36];
	u8 *pFont=NULL;
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY) return;	
	num = num - ' ';
	//for(pos=0;pos<16;pos++)
	switch(font)
	{
		case FONT_16x08:
			fontWidth = 8;
			pFont = (u8*)(&asc2_1608[num][0]);
			pageEnd = 2;
			//paEle = 8;
		break;

#if	0
		case FONT_16x08B:
			fontWidth = 16;
			pFont = (u8*)(&asc2_1608B[num][0]);
			pageEnd = 7;
			paEle = 8;
		break;
#endif

#ifdef _320x240_
		case FONT_24x12:
			fontWidth = 12;
			pFont = (u8*)(&asc2_2412[num][0]);
			pageEnd = 3;
			//paEle = 12;
		break;
#endif

#ifdef _320x240_
		case FONT_32x16:
			fontWidth = 16;
			pFont = (u8*)(&asc2_3216[num][0]);
			pageEnd = 4;
			//paEle = 16;
		break;
#endif

#ifdef _220x176_
		case FONT_12x06:
			fontWidth = 6;
			pFont = (u8*)(&asc2_1206[num][0]);
			pageEnd = 2;
			//paEle = 6;
		break;

		case FONT_20x10:
			fontWidth = 10;
			pFont = (u8*)(&asc2_20x10[num][0]);
			pageEnd = 3;
			//paEle = 10;
		break;
#endif
	}
	//t = 0;

	g_hGpiob = DATAIN&0xff00;;
	for(paEle=0;paEle<pageEnd;paEle++)
	{
		AreaSet(x,y,x+8-1,y+fontWidth-1);
		SET_LCDCS_VALID();
		for(pos=0;pos<fontWidth;pos++)
		{
			//temp = pFont[t++];
			//temp = pFont[pos+pageEnd*fontWidth];
			temp = *pFont++;
			for(t=0;t<8;t++)
			{
				if(temp&0x01)
				{
					//g_penColor=(rsvMod&(DSP_SELE))? SEL_COLOR:((rsvMod&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//PEN_COLOR:BACK_COLOR;
					dspColor=(rsvMod&(DSP_SELE))? SEL_COLOR:((rsvMod&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]]);
					//g_penColor=(rsvMod)? SEL_COLOR:PEN_COLOR;
					//LCD_DrawPoint(x,y);	
				}else
				{
					//g_penColor=(rsvMod&(DSP_REVERSE))? PEN_COLOR:BACK_COLOR;//BACK_COLOR:PEN_COLOR;
					dspColor=(rsvMod&(DSP_REVERSE))? penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
					//g_penColor = BACK_COLOR;
				}
				LCD_WR_DATA(dspColor);
				temp>>=1; 
			}
		}
		SET_LCDCS_INVALID();
		x+=8;
	}
	
}
#endif

void LCD_ShowHexChar(u16 x,u16 y,u8 num, u8 font,u8 rsvMod)
{
	if(num<10)
	{
		LCD_ShowChar(x,y,num+'0',font,rsvMod);
	}else
	{
		LCD_ShowChar(x,y,num-10+'A',font,rsvMod);
	}
}


u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}


void LCD_ShowCnString(u16 x,u16 y,const char *p, u8 rsvMod)
{
	u8* pFont;
	pFont = (u8*)p;
	while(*pFont!='\0')
	{
		if(pFont[0]>0x80)
		{
			LCD_ShowCnChar(x, y, pFont, rsvMod);
			y+=16;
			pFont+=2;
		}else
		{
			LCD_ShowChar(x, y, *pFont, FONT_TYPE_MAIN,rsvMod);
			y+=8;
			pFont+=1;
		}
	}
}


void LCD_ShowString(u16 x,u16 y,const char *p, u8 font, u8 dotCpr,u8 rsvMod)
{         
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
		if(y+pageWid>=LCD_W)
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
}


void LcdPutBuf(u16 x,u16 y,const u8 *p, u8 font, u8 sz,u8 dotCpr,u8 rsvMod)
{
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
	
}

#if 0
void LcdLoadPicture(u16 x, u16 y, u16 wide, u16 tall, const char* pBit,u8 rsv)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t;
	u16 pos,pa;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;
	AreaSet(x,y,x+wide-1,y+tall-1);
	//fCode = *temp++;
	for(pos=0;pos<wide;pos++)
	{
		for(pa=0;pa<tall;pa++)
		{	
			fCode = *temp++;
			for(t=0;t<8;t++)
			{
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					g_penColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	g_penColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
			        	//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(g_penColor);
				fCode>>=1; 	
			}
		}
	}

}
#endif

#if 1
void LcdLoadPicture(u16 x, u16 y, u16 wide, u16 tall, const char* pBit,u8 rsv)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;
	
	g_hGpiob = DATAIN&0xff00;;

	for(pa=0;pa<tall/8;pa++)
	{
		AreaSet(x,y,x+8-1,y+wide);
		SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					dspColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	dspColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
			        	//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		SET_LCDCS_INVALID();
		x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}
#endif


#ifdef _TUHAO_START_
void SamheroLogoDsp(u16 x, u16 y, u16 wide, u16 tall, const char* pBit, u16 color)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;
	g_hGpiob = DATAIN&0xff00;;

	for(pa=0;pa<tall/8+((tall%8)>0);pa++)
	{
		AreaSet(x,y,x+8-1,y+wide);
		SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					//dspColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					dspColor = color;
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	//dspColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
					dspColor = BLACK;
					//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		SET_LCDCS_INVALID();
		x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}

void TuhaoDrawPoint(u16 x,u16 y, u16 color)
{
	//u16 dspColor;
	//AddrSet(LCD_H-x,y,LCD_H-x,y);//设置光标位置 
	//LCD_WR_REG(0x0020); 	//HS
	//LCD_WR_DATA(LCD_H-x); 		 
	//LCD_WR_REG(0x0021);  //VS
	//LCD_WR_DATA(y); 
	//LCD_WR_REG(0x0022);
	//AddrSet(LCD_H-x,y);
	//AddrSet(x,y);
	//dspColor = (rvs&DSP_SELE)?SEL_COLOR:(rvs&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
	g_hGpiob = DATAIN&0xff00;;
	AreaSet(x,y,x,y);
	//LCD_WR_DATA(g_penColor); 	    
	LCD_WR_WORD(color); 
}

void LcdTuhaoLogo(u16 x, u16 y, u16 wide, u16 tall, const char* pBit,u16 dspColor)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 sta;
	//u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;

	for(pa=0;pa<tall/8+((tall%8)>0);pa++)
	{
		//AreaSet(x,y,x+8-1,y+wide);
		//SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					TuhaoDrawPoint(x+pa*8+t,y+pos,dspColor);
			        }
				//LCD_DrawPoint(x,y);	
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		//SET_LCDCS_INVALID();
		//x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}


void DspLogo(void)
{
	LCD_Clear(BLACK);
	SamheroLogoDsp(40+20-12,30-2,42,10,tuhao_4,WHITE);
	SamheroLogoDsp(40+20,30,160,26,tuhao_1,WHITE);
	LcdTuhaoLogo(40-35+20,30+103,47,46,tuhao_2,ORANGE);
	SamheroLogoDsp(80+20,119,80,16,tuhao_3,WHITE);
	
}

#else
#ifdef _SN_DEBUG_
void SamheroLogoDsp(u16 x, u16 y, u16 wide, u16 tall, const char* pBit, u16 color)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;

	g_hGpiob = DATAIN&0xff00;;

	for(pa=0;pa<tall/8;pa++)
	{
		AreaSet(x,y,x+8-1,y+wide);
		SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					//dspColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					dspColor = color;
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	//dspColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
					dspColor = BLACK;
					//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		SET_LCDCS_INVALID();
		x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}

void LcdSnDspLogo(u16 x, u16 y, u16 wide, u16 tall, const char* pBit)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 sta;
	u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;
	g_hGpiob = DATAIN&0xff00;;

	for(pa=0;pa<tall/8;pa++)
	{
		AreaSet(x,y,x+8-1,y+wide);
		SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					//dspColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					sta = pa*8+t;
					if((pos>35)&&(pos<84)&&(sta>16)&&(sta<56))
					{
						dspColor = ORANGE;
					}
					else
					{
						dspColor = WHITE;
					}
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	//dspColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
			        	dspColor = BLACK;
			        	//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		SET_LCDCS_INVALID();
		x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}

void LcdSnDspLogoWz(u16 x, u16 y, u16 wide, u16 tall, const char* pBit)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;
	g_hGpiob = DATAIN&0xff00;;

	for(pa=0;pa<tall/8;pa++)
	{
		AreaSet(x,y,x+8-1,y+wide);
		SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					//dspColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					dspColor = WHITE;
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	//dspColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
					dspColor = BLACK;
					//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		SET_LCDCS_INVALID();
		x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}

void DspLogo(void)
{
	LCD_Clear(BLACK);
	LcdSnDspLogo(30,50,120,72,Sn_logo);  
	LcdSnDspLogoWz(104,30,160,24,Sn_logo_wz);
}


#else

void SamheroLogoDsp(u16 x, u16 y, u16 wide, u16 tall, const char* pBit, u16 color)
{
	u8 *temp,fCode;
	//u8 pos,t,pa;
	u8 t,pa;
	u16 pos;
	u16 dspColor;
	//u8 *pIndex;
	//u16 x0=x,y0=y;
	//u16 colortemp=g_penColor;      
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY)return;	

	//pIndex = (u8*)pBit;
	temp = (u8*)pBit;
	g_hGpiob = DATAIN&0xff00;;

	for(pa=0;pa<tall/8;pa++)
	{
		AreaSet(x,y,x+8-1,y+wide);
		SET_LCDCS_VALID();
		for(pos=0;pos<wide;pos++)
		{
			//temp = pBit;
			fCode = *temp++;
			for(t=0;t<8;t++)
		    	{                 
			        if(fCode&0x01)
				{
					//rsv?g_penColor=PEN_COLOR:g_penColor=BACK_COLOR;
					//g_penColor=((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);//(rsv?PEN_COLOR:BACK_COLOR);
					//dspColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
					dspColor = color;
					//g_penColor = rsv?SEL_COLOR:PEN_COLOR;
			        }else 
			        {
			        	//rsv? g_penColor=BACK_COLOR:g_penColor=PEN_COLOR;
			        	//g_penColor=((rsv&DSP_REVERSE)?PEN_COLOR:BACK_COLOR);//(rsv?BACK_COLOR:PEN_COLOR);
			        	//dspColor = (rsv&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
					dspColor = BLACK;
					//g_penColor = BACK_COLOR;
			        }
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				fCode>>=1; 
				//x++;
		    	}
			//temp ++;
			//x=x0;
			//y++;
		}
		SET_LCDCS_INVALID();
		x+=8;
		//x0+=8;
		//x = x0;
		//y = y0;		
	}
}

void DspLogo(void)
{
	LCD_Clear(BLACK);
	SamheroLogoDsp(40,55,100,41,samhero_logo,GOLDEN);
	//SamheroLogoDsp(40,80,100,41,samhero_logo,GOLDEN);
	SamheroLogoDsp(86,30,160,40,samhero_char,GOLDEN);
}


#endif
#endif

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

void LcdDspAtCenter(u16 x, u16 y, const char* p,u8 font, u8 rsv, u16 wt,u16 sz)
{
	u16 ys=0;
	if(sz>wt) return;
	ys = y+((wt -sz)>>1);

	if((g_dspChinese>0)&&(p[0]>0x80))
	{
		LCD_ShowCnString(x, ys, p,rsv);
	}else
	{
		LCD_ShowString(x,ys,p,font,1,rsv);
	}
}

void LcdDspSnrString(u16 x, u16 y, const char* p,u8 font, u8 rsv, u16 wt,u16 sz)
{
	u16 ys=0;
	if(sz>wt) return;
	ys = y+((wt -sz)>>1);

	if((g_dspChinese>0)&&(p[0]>0x80))
	{
		LCD_ShowCnString(x, ys, p,rsv);
	}else
	{
		LCD_ShowString(x,ys,p,font,0,rsv);
	}
}

void *BtnAddSub(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	
	if(SETTING_ADDSUB==g_SettingIndex)
	{
		//g_SettingIndex = 0;
		return;
	}else
	{
		g_SettingIndex = SETTING_ADDSUB;
	}
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
			g_grSel = 0;
			SetRfGrScr();
		break;
	}*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnTtlFeb(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	
	if(SETTING_TTLFEB==g_SettingIndex)
	{
		g_SettingIndex = 0;
	}else
	{
		g_SettingIndex = SETTING_TTLFEB;
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
		g_grSel = 0;
		SetRfGrScr();
	}*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnRatio()
{
	(*wlSet.arr_mode)++;
	if((*wlSet.arr_mode)>WLM_MODE_RES)
	{
		(*wlSet.arr_mode) = WLM_MODE_ALL;
	}
	//DisplayCanMstHead(1,struMode.mode,wlSet.arr_mode);
	//DisplayCanMstHead(0,mainPara.workMode,&wlSet);
	//Mode(mainPara.workMode);
	//ModPasteScr(mainPara.workMode);
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;

}

void *BtnMulti(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	g_SettingIndex = SETTING_MULTI;
	//MULTI_SET_ETIMES(*multiSet.eTimes, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnFrq(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	g_SettingIndex = SETTING_FRQ;
	//MULTI_SET_FRQ(*multiSet.frequence, g_SettingIndex);
	//SetMultiScr();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnGrSel(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	
	if(WORKINGMODE_GR==mainPara.workMode)
	{
		g_SettingIndex = SETTING_RFGR_SEL;
		//g_grDspStart = 0;
		if(0==g_grSel)
		{
			g_grSel = g_grSelRsv;
		}else
		{
			g_grSel++;
		}
		if(0==g_grSel)
		{
			g_grSel++;
			g_grSelRsv = g_grSel;
		}
		if(!g_grStyle)
		{
			if(g_grDspStart>g_grSel-1)
			{
				g_grDspStart = g_grSel-1;
			}
			if(g_grSel-g_grDspStart>3)
			{
				g_grDspStart++;
			}
		}else
		{
			g_grDspStart = 0;
		}
		
		if(g_grSel>5)
		{
			g_grSel = 0;
			g_grSelRsv = 0;
			g_grDspStart=0;
			g_SettingIndex = 0;
			RFGR_SELBAR_CLEAR;
			//RfDevLinkMark(WORKINGMODE_GR,&wlSet);
		}
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
		//RfGrDspPara(g_grSel);	
		//SetRfGrScr();
		/*
		if((g_grSel)&&(!g_grStyle))
		{
			GrDrawSelBar(g_grSel-1);
		}*/
		//GrDspPara(LAYOUT_RATIO_A+(g_grSel-1-g_grDspStart)*(BIG_FONT_HIGHT(1)+LINE_SPACING(1)),gr+g_grDspStart,WORKINGMODE_TTL,radioSet->rfGr[gr+g_grDspStart].ttl_cmp1,radioSet->rfGr[gr+g_grDspStart].ttl_cmp2,DSP_REVERSE);
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
		if((g_SettingIndex-WLM_SETTING_ARRA)>(*wlSet.arr_mode))
		{
			g_SettingIndex = 0;
		}
		
		if(0==g_SettingIndex)
		{
			g_SettingIndex = g_SettingRsv;
		}else 
		{
			g_SettingIndex++;
		}
		
		if((WORKINGMODE_TTL==mainPara.workMode)&&(*wlSet.arr_mode>0))
		{
			if(g_SettingIndex<WLM_SETTING_ARRB)
			{
				g_SettingIndex = WLM_SETTING_ARRB;
			}
		}else if(g_SettingIndex<WLM_SETTING_ARRA)
		{
			g_SettingIndex = WLM_SETTING_ARRA;
		}
		
		if((g_SettingIndex-WLM_SETTING_ARRA)>(*wlSet.arr_mode))
		{
			g_SettingIndex = 0;
			g_SettingRsv = 0;
		}
		//ModPasteScr(mainPara.workMode);
		//Mode(mainPara.workMode);
		//DisplayCanMstHead(g_SettingIndex-2,mainPara.workMode,&wlSet);	
		SET_UPDATE_EVT(DSP_UPDATE_MODE);
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
	KeyMenuSw();
	return NULL;
}

void *BtnZoon(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	g_SettingIndex = SETTING_MOTO;
	DisplayFocLen(*motoSet.mode,*motoSet.dest_location,DSP_SELE);
	return NULL;
}

void *BtnMstSw(void)
{
	g_SettingIndex = SETTING_MSTSW;
	if(WLM_MSTFLA_ON==*wlSet.is_masterOn)
	{
		*wlSet.is_masterOn = WLM_MSTFLA_OFF;
		LCD_MSTFLA_OFF;
	}else
	{
		*wlSet.is_masterOn = WLM_MSTFLA_ON;
		LCD_MSTFLA_ON;
	}
	return NULL;
}

void DspID(u8 sel,unsigned int rfId)
{
//	u8 i;
	DspClearAllPara();
	LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML, 'I', FONT_TYPE_BIG, DSP_NORMAL);
	LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(1), 'D', FONT_TYPE_BIG, DSP_NORMAL);
	LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(3), (rfId)/1000+'0', FONT_TYPE_BIG, (sel==4)?DSP_SELE:DSP_NORMAL);
	LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(4), (rfId)%1000/100+'0', FONT_TYPE_BIG, (sel==3)?DSP_SELE:DSP_NORMAL);
	LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(5), (rfId)%100/10+'0', FONT_TYPE_BIG, (sel==2)?DSP_SELE:DSP_NORMAL);
	LCD_ShowChar(LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(6), (rfId)%10+'0', FONT_TYPE_BIG, (sel==1)?DSP_SELE:DSP_NORMAL);

/*
	for(i=0;i<8;i++)
	{
		LCD_ShowHexChar(LAYOUT_RATIO_B,60+MAIN_FONT_WIDTH(2*i),(g_snMac.mac[7-i]>>4),FONT_TYPE_MAIN,DSP_NORMAL);
		LCD_ShowHexChar(LAYOUT_RATIO_B,60+MAIN_FONT_WIDTH(2*i+1),(g_snMac.mac[7-i]&0x0f),FONT_TYPE_MAIN,DSP_NORMAL);
	}
*/
#ifndef _NO_SN_
	//for(i=0;i<8;i++)
	for(i=0;i<12;i++)
	{
		//LCD_ShowHexChar(LAYOUT_RATIO_B,MAIN_FONT_WIDTH(1)+60+MAIN_FONT_WIDTH(2*i),g_snMac.sn[7-i]>>4,FONT_TYPE_MAIN,DSP_NORMAL);
		//LCD_ShowHexChar(LAYOUT_RATIO_B,MAIN_FONT_WIDTH(1)+60+MAIN_FONT_WIDTH(2*i+1),g_snMac.sn[7-i]&0x0f,FONT_TYPE_MAIN,DSP_NORMAL);
		LCD_ShowChar(LAYOUT_RATIO_B,MAIN_FONT_WIDTH(1)+60+MAIN_FONT_WIDTH(i),g_snMac.sn[i],FONT_TYPE_MAIN,DSP_NORMAL);
	}
#endif	
	//if(sel)
	//LcdPicFramed( LAYOUT_RATIO_A, LAYOUT_IDSET_CML+BIG_FONT_WIDTH(2+sel),BIG_FONT_WIDTH(1),BIG_FONT_HIGHT(1),DSP_SELE);

	funcMenu.listCnt = 0;
	g_menuId=0;
	AddButtons(&mNull,&mNull,&mIdSel,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();
	
}

#ifdef _1DX_DEBUG_
void DspUnknownData(u16 x,u16 dat)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		LCD_ShowHexChar(x*18+4,100-MAIN_FONT_WIDTH(i),dat&0x0f,FONT_TYPE_MAIN,DSP_NORMAL);
		dat>>=4;
	}
	
}
#endif

void *BtnIdSet(void)
{
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	
	//LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	//funcMenu.listCnt = 0;
	//g_menuId=0;
	//AddButtons(&mNull,&mNull,&mIdSel,&mCancel,funcMenu.listCnt++);
	//KeyMenuSw();
	g_SettingIndex = SETTING_ID;
	g_rfIdSet = radioSet->id;
	//DspID(0,radioSet->id);
	//DspID(0,g_rfIdSet);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void ScanFillProc(u16 x, u16 y,u16 wt,u16 ht,u8 pr)
{
	u16 wFill=wt/4/4;
	u16 wVal=wt/12;
	u16 sp=y+(wt/4)*pr;
	LcdFillBlank(x,sp,wFill,ht,DSP_NORMAL);
	LcdFillBlank(x,sp+wVal,wFill,ht,DSP_NORMAL);
	LcdFillBlank(x,sp+2*wVal,wFill,ht,DSP_NORMAL);	
}

void DrawScanRstBar(u16 y, u8 rssi)
{
	u8 i=0;
	u8 sd=rssi/25;
	u16 xa=LAYOUT_SCANRST_X;

	for(i=0;i<sd;i++)
	{
		xa-=SCANRST_DOT_VAL;
		xa-=SCANRST_DOT_HT;
		LcdFillBlank(xa, y, SCANRST_DOT_WT, SCANRST_DOT_HT, DSP_NORMAL);
	}
	
}

void DspScanResult(void)
{
	u8 chIndex=0;
	u16 sy=LAYOUT_SCANRST_Y+SCANRST_DOT_WT;
	LcdClearFeild(0, 0, LCD_W, LCD_H);
	LCD_DrawLine(LAYOUT_SCANRST_X-11*(SCANRST_DOT_HT+SCANRST_DOT_VAL), LAYOUT_SCANRST_Y,LAYOUT_SCANRST_X,LAYOUT_SCANRST_Y);
	LcdLoadPicture(LAYOUT_SCANRST_X-11*(SCANRST_DOT_HT+SCANRST_DOT_VAL)-SCANRST_ARROW_HT, LAYOUT_SCANRST_Y-SCANRST_ARROW_HT/2, SCANRST_ARROW_WT, SCANRST_ARROW_HT,chScArrow, DSP_NORMAL);
	LCD_ShowString(LAYOUT_SCANRST_X-11*(SCANRST_DOT_HT+SCANRST_DOT_VAL)-SCANRST_ARROW_HT-AUX_FONT_HIGHT(1), LAYOUT_SCANRST_Y-AUX_FONT_WIDTH(2), "GOOD", FONT_TYPE_AUX, 0, DSP_NORMAL);
	LCD_DrawLine(LAYOUT_SCANRST_X, LAYOUT_SCANRST_Y,LAYOUT_SCANRST_X,LAYOUT_SCANRST_Y+16*(SCANRST_BAR_VAL+SCANRST_DOT_WT));

#ifdef UART1_TRACE_DEBUG
	for(chIndex=0;chIndex<15;chIndex++)
	{
		printf("%x ", radioStatue.rssi[chIndex]);
	}
	printf("\r\n");
#endif
	
	for(chIndex=0;chIndex<15;chIndex++)
	{
		sy+=SCANRST_BAR_VAL;
		//radioStatue.rssi[chIndex]
		DrawScanRstBar(sy,radioStatue.rssi[chIndex]);
		if((!chIndex)||(!((chIndex+1)%5)))
		{
			if(chIndex<9)
			{
				LCD_ShowChar(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL, sy, chIndex+1+'0', FONT_TYPE_AUX, DSP_NORMAL);
			}else
			{
				LCD_ShowChar(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL, sy-2, (chIndex+1)/10+'0', FONT_TYPE_AUX, DSP_NORMAL);
				LCD_ShowChar(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL, sy+AUX_FONT_WIDTH(1)-2,((chIndex+1)%10+'0'), FONT_TYPE_AUX, DSP_NORMAL);
			}
		}else
		{
			DisplayMstSetDot(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL+SCANRST_DOT_HT, sy+1, DSP_NORMAL);
		}
		sy+=SCANRST_DOT_WT;
	}

	//g_rfChanSet = radioSet->rfChan;
	SetMstChan(g_rfChanSet,DSP_NORMAL);
	DisplayMstSetDot(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL+SCANRST_DOT_HT+8, 5+LAYOUT_SCANRST_Y+SCANRST_DOT_WT+(g_rfChanSet-1)*(SCANRST_BAR_VAL+SCANRST_DOT_WT), DSP_NORMAL);

	sy+=SCANRST_DOT_WT+SCANRST_DOT_WT;
	LCD_ShowString(LAYOUT_SCANRST_X+2*SCANRST_DOT_VAL, sy, "Ch", FONT_TYPE_AUX, 0, DSP_NORMAL);

	funcMenu.listCnt = 0;
	g_menuId=0;
	AddButtons(&mNull,&mNull,&mNull,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();
	
	
}

void *BtnChScan(void)
{
//	u8 fillPr=0;
	u16 scanCount=0;
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	g_isChScan = 1;
	g_SettingIndex = SETTING_RFSCAN;
	LcdClearFeild(0, 0, LCD_W, LCD_H);
	LcdPicFramed(LCD_H/8, LCD_W/16,(LCD_W*7)/8,(LCD_H*3)/4,DSP_NORMAL);
	LCD_ShowString(LCD_H/4,LCD_W/2-BIG_FONT_WIDTH(4),"Ch.SCAN",FONT_TYPE_BIG,0,DSP_NORMAL);
	UartSctSend(UART_ENCODE_CHSCAN,0x01,0);
	//DISABLE_RF_MODULE();	
#if 0
	while(g_isChScan)
	{
		LcdClearFeild(LCD_H/4+BIG_FONT_HIGHT(2)-1, LCD_W/2-BIG_FONT_WIDTH(6)-1,BIG_FONT_WIDTH(12)+2,BIG_FONT_HIGHT(1)+2);
		LcdPicFramed(LCD_H/4+BIG_FONT_HIGHT(2), LCD_W/2-BIG_FONT_WIDTH(6),BIG_FONT_WIDTH(12),BIG_FONT_HIGHT(1),DSP_NORMAL);
		ScanFillProc(LCD_H/4+BIG_FONT_HIGHT(2), LCD_W/2-BIG_FONT_WIDTH(6),BIG_FONT_WIDTH(12),BIG_FONT_HIGHT(1),fillPr++);
		if(fillPr>3) fillPr = 0;
		if(scanCount++>24)
		{
			DISABLE_RF_MODULE();	
			break;
		}
		OSTimeDlyHMSM(0,0,0,300);
		
	}
#else
	while(g_isChScan)
	{
		//LcdClearFeild(LCD_H/4+BIG_FONT_HIGHT(2)-1, LCD_W/2-BIG_FONT_WIDTH(6)-1,BIG_FONT_WIDTH(12)+2,BIG_FONT_HIGHT(1)+2);
		//LcdPicFramed(LCD_H/4+BIG_FONT_HIGHT(2), LCD_W/2-BIG_FONT_WIDTH(6),BIG_FONT_WIDTH(12),BIG_FONT_HIGHT(1),DSP_NORMAL);
		//ScanFillProc(LCD_H/4+BIG_FONT_HIGHT(2), LCD_W/2-BIG_FONT_WIDTH(6),BIG_FONT_WIDTH(12),BIG_FONT_HIGHT(1),fillPr++);
		DisplayPicture(63+15,85,48,48,picScan_1,DSP_NORMAL);
		//delay_ms(300);
		OSTimeDlyHMSM(0,0,0,300);
		DisplayPicture(63+15,85,48,48,picScan_2,DSP_NORMAL);
		//delay_ms(300);
		OSTimeDlyHMSM(0,0,0,300);
		DisplayPicture(63+15,85,48,48,picScan_3,DSP_NORMAL);
		//delay_ms(400);
		OSTimeDlyHMSM(0,0,0,400);
		//if(fillPr>3) fillPr = 0;
		if(scanCount++>24)
		{
			DISABLE_RF_MODULE();	
			break;
		}
		//OSTimeDlyHMSM(0,0,0,300);
		
	}

#endif

	if(scanCount>24)
	{
		g_SettingIndex = 0;
		BtnHome();
		ENABLE_RF_MODULE();	
	}else
	{

		if(RF_CHAN_AUTO==radioSet->rfChan)
		{
			g_SettingIndex = 0;
			BtnHome();
			
		}else
		{
			g_rfChanSet = radioSet->rfChan;
			//DspScanResult();
			//funcMenu.listCnt = 0;
			g_menuId=0;
			//AddButtons(&mNull,&mNull,&mNull,&mCancel,funcMenu.listCnt++);
			//KeyMenuSw();	
			SET_UPDATE_EVT(DSP_UPDATE_MODE);
		}
	}

	BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	
	return NULL;
	
}

void *BtnGrStyle(void)
{
	if(!g_grStyle)	
	{
		g_grStyle = 1;
	}else
	{
		g_grStyle = 0;
	}
	/*
	DspClearAllPara();
	DspMstModIcon(*g_cmdMode);
	SetRfGrScr();
	//RfGrDspPara(g_grSel);
	LoadButtons();	
	RfDevLinkMark(WORKINGMODE_GR,&wlSet);*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);

	return NULL;
}
	

void *BtnIdSel(void)
{
	g_idSetIndex++;
	if(g_idSetIndex>4)
	{
		g_idSetIndex = 1;
	}
	//DspID(g_idSetIndex,g_rfIdSet);
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;	
}

void *BtnChSet(void)
{	
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	
	g_SettingIndex = WLM_SETTING_CHAN;
	g_rfChanSet = radioSet->rfChan;
	g_liteChanSet = *wlSet.m_chan;
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
			SetMstChan(g_liteChanSet+1,DSP_SELE);
			ModPasteScr(mainPara.workMode);
		break;

		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RFLS_SLA:
			SetMstChan(g_rfChanSet,DSP_SELE);
		break;

		case CMD_MODE_RF_MST:
		case CMD_MODE_RF_SLA:
			SetMstChan(g_rfChanSet,DSP_SELE);
			ModPasteScr(mainPara.workMode);
		break;
	}*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnReset(void)
{
	//memset((u8*)&mainPara,0,sizeof(mainPara));
	memset((u8*)&(mainPara.workMode),0,((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode))+1);
	STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
	//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));
	return NULL;
}

void *BtnRel(void)
{
	//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL, 0);
	UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_REL,*g_cmdMode);
	return NULL;
}

void *BtnMemory(void)
{
	/*
	u8 index;
	char *pMain;
	mainPara.sumVeri = 0;
	pMain = (char *)&mainPara;
	//for(index=0;index<sizeof(mainPara)-1;index++)
	for(index=0;index<((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode));index++)
	{
		mainPara.sumVeri += pMain[index];
	}*/
	mainPara.sumVeri=CalcSum((u8*)&(mainPara.workMode),((char*)&(mainPara.sumVeri)-(char*)&(mainPara.workMode)));
	STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,((char*)(&mainPara.rsv)-(char*)(&mainPara.workMode)+2)/2);
	//STMFLASH_Write(FLASH_USER_ADDR,(u16*)&mainPara,(sizeof(mainPara)/2));
	return NULL;
}	

void DspSnrMenu(void)
{
	MenuItem *subItem;
	u8 cnCmp=0;
	u8 clrCmp=0;

	//CLR_SUBMENU_FILD;
	DspClearAllPara();
	
	if(!g_dspChinese)
	{
		LCD_ShowString(0,LAYOUT_ETTL_X,"C.Fn Menu",SNR_FONT_TYPE, 0, DSP_NORMAL);
		//cnCmp = snrMenu.userSel*2+1;
	}else
	{
		LCD_ShowCnString(0,LAYOUT_ETTL_X,"自定义菜单", DSP_NORMAL);
	}
		
	if(snrMenu.userSel>=MENU_ITEMS)
	{
		snrMenu.userSel=0;//userSel=0;
		snrMenu.dspStart=0;//dspStart=0;
		//LcdClearFeild(SNR_FONT_HIGHT(2), 0, LCD_W, SNR_FONT_HIGHT(7));
	}
	if(snrMenu.dspStart+MAXDISPLAYLINE-1<snrMenu.userSel)
	{
		snrMenu.dspStart = snrMenu.userSel;
		//LcdClearFeild(SNR_FONT_HIGHT(2), 0, LCD_W, SNR_FONT_HIGHT(7));
	}
	if(snrMenu.userSel<snrMenu.dspStart)
	{
		snrMenu.dspStart = snrMenu.userSel;
	}
	if(g_dspChinese>0)
	{
		cnCmp = (snrMenu.userSel-snrMenu.dspStart)*2+1;
		clrCmp = 2*MAXDISPLAYLINE;
	}
	subItem = (MenuItem *)snrMenu.munuItem[snrMenu.userSel].childrenMenus;
	ShowSnrMenu(snrMenu.dspStart,0,snrMenu.userSel,MAINMENU_COLUMN,snrMenu.munuItem,MENU_ITEMS);
	//LcdClearFeild(SNR_FONT_HIGHT(2),SNR_TITLE_WIDTH+5,8,SNR_FONT_HIGHT(MAXDISPLAYLINE)+clrCmp);

	LCD_ShowChar(SNR_FONT_HIGHT(snrMenu.userSel-snrMenu.dspStart+2)+cnCmp,SNR_TITLE_WIDTH+5,'|',SNR_FONT_TYPE,DSP_SELE);
	
	if (g_isSmExpand > 0)
	{
		SmSubExpand();
	}
	else
	{
		if(!g_dspChinese)
		{
			DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].displayString,DSP_SELE);
		}else
		{
			DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].cnMenuString,DSP_SELE);
		}
	}

	funcMenu.listCnt=0;
	AddButtons(&mNull,&mSmSel,&mSmSubSel,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();
}

void *BtnSmSel(void)
{
	//MenuItem *subItem;
	snrMenu.userSel++;

/*	
	if(snrMenu.userSel>=MENU_ITEMS)
	{
		snrMenu.userSel=0;//userSel=0;
		snrMenu.dspStart=0;//dspStart=0;
		//LcdClearFeild(SNR_FONT_HIGHT(2), 0, LCD_W, SNR_FONT_HIGHT(6));
	}
	if(snrMenu.dspStart+MAXDISPLAYLINE-1<snrMenu.userSel)
	{
		snrMenu.dspStart = snrMenu.userSel;
		//LcdClearFeild(SNR_FONT_HIGHT(2), 0, LCD_W, SNR_FONT_HIGHT(6));
	}*/

	g_isSmExpand = 0;
	
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	//DspSnrMenu();
	/*
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
	LcdClearFeild(SNR_FONT_HIGHT(2),SNR_TITLE_WIDTH+5,8,SNR_FONT_HIGHT(MAXDISPLAYLINE));
	LCD_ShowChar(SNR_FONT_HIGHT(snrMenu.userSel-snrMenu.dspStart+2),SNR_TITLE_WIDTH+5,'|',SNR_FONT_TYPE,DSP_SELE);
	DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].displayString,DSP_SELE);
	*/
	return NULL;
}

void SmSubExpand(void)
{
	if(snrMenu.subSel>=snrMenu.munuItem[snrMenu.userSel].subCnt)
	{
		snrMenu.subSel = 0;
		snrMenu.subStart = 0;
	}

	if(snrMenu.subSel<0)
	{
		snrMenu.subSel = snrMenu.munuItem[snrMenu.userSel].subCnt-1;
		if(snrMenu.munuItem[snrMenu.userSel].subCnt>MAXDISPLAYLINE)
		{
			snrMenu.subStart = snrMenu.munuItem[snrMenu.userSel].subCnt-MAXDISPLAYLINE;
		}else
		{
			snrMenu.subStart = 0;
		}
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
}

void *BtnSmSub()
{
	//struct MenuItem 	*pNow;
	g_isSmExpand = 1;
	snrMenu.subSel++;
	/*
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
	}*/
	if(snrMenu.munuItem[snrMenu.userSel].subFuc!=NULL)
	{
		snrMenu.munuItem[snrMenu.userSel].subFuc();
		//CHARGE_LED =!CHARGE_LED;
	}
#if 0
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
	//SmSubExpand();
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnHome(void)
{
/*
	DspClearAllPara();
	FuncDsp();
	g_SettingIndex = 0xff;
	KeyOk();
	//g_SettingIndex = 0;
*/
	g_SettingIndex = 0;
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

void *BtnSynSet(void)
{
	mainPara.synMode++;
	if(mainPara.synMode>SYNC_MODE_FP)
	{
		mainPara.synMode = SYNC_MODE_FRON;
	}
	if((SYNC_MODE_REAR==mainPara.synMode)&&(CMD_MODE_OFF!=*g_cmdMode))
	{
		mainPara.synMode=SYNC_MODE_FP;
	}
	CanSynMode(mainPara.workMode,mainPara.synMode,DSP_NORMAL);
	//BuildFuncMenu();
	LoadButtons();
	return NULL;
}

void *BtnSnrMenu(void)
{
//	unsigned char subSel=0;				//submenu 's select
//	MenuItem *subItem;
//	u8 cnCmp=0;
//	subSel = g_snrMenuSet[0];			//avoid the select error
	//if(g_cmrLinked>0)		//no edite when linked
	//return;
	g_SettingIndex = SETTING_SNRMENU;
	snrMenu.subSel = 0;
	snrMenu.userSel = 0;
	snrMenu.dspStart = 0;
/*
	LcdClearFeild(0, 0, LCD_W, LAYOUT_MENU_PAGE);
	if(!g_dspChinese)
	{
		LCD_ShowString(0,LAYOUT_ETTL_X,"C.Fn Menu",SNR_FONT_TYPE, 0, DSP_NORMAL);
		cnCmp = snrMenu.userSel*2+1;
	}else
	{
		LCD_ShowCnString(0,LAYOUT_ETTL_X,"自定义菜单", DSP_NORMAL);
	}
	LCD_ShowChar(SNR_FONT_HIGHT(snrMenu.userSel-snrMenu.dspStart+2)+cnCmp,SNR_TITLE_WIDTH+5,'|',SNR_FONT_TYPE,DSP_SELE);
	subItem = (MenuItem *)snrMenu.munuItem[snrMenu.userSel].childrenMenus;
	if(!g_dspChinese)
	{
		DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].displayString,DSP_SELE);
	}else
	{
		DspSnrFucStr(snrMenu.userSel-snrMenu.dspStart+2,SUBMENU_COLUMN,subItem[pSnrSet[snrMenu.userSel]].cnMenuString,DSP_SELE);
	}
	ShowSnrMenu(0,0,0,0,snrMenu.munuItem,MENU_ITEMS);
	funcMenu.listCnt=0;
	AddButtons(&mNull,&mSmSel,&mSmSubSel,&mCancel,funcMenu.listCnt++);
	KeyMenuSw();
*/
	SET_UPDATE_EVT(DSP_UPDATE_MODE);
	return NULL;
}

char RePaint(void)
{
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
	return 1;
}


void DspMstRatioHead(u16 x, u16 y, const char* p, u8 rsv,u8 sz)
{
#if(_DSP_FONT_OPTION_==BIG_FONT)
	LcdDspAtCenter(x, y,p,FONT_TYPE_GRHEAD,rsv,LAYOUT_RATIO_WIDTH,sz);
	LcdPicFramed(x, y, LAYOUT_RATIO_WIDTH, BIG_FONT_HIGHT(1),rsv);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	LcdDspAtCenter(x, y,p,FONT_TYPE_GRHEAD,rsv,LAYOUT_RATIO_WIDTH,sz);
	LcdPicFramed(x, y, LAYOUT_RATIO_WIDTH, BIG_FONT_HIGHT(1),rsv);
#endif
}

#ifdef _320x240_
void LcdDsp32Num(u16 x, u16 y, u8 num,u8 rsvMod)
{
	u8 temp;
	u8 pos,t,pa=0; 
	u16 dspColor;
	//if(x>MAX_CHAR_POSX||y>MAX_CHAR_POSY) return;	
	if(num<'+'||num>';')	return;
	num = num - '+';
	g_hGpiob = DATAIN&0xff00;;
	//for(pos=0;pos<36;pos++)
	for(pa=0;pa<4;pa++)
	{
		AreaSet(x,y,x+8-1,y+16-1);
		SET_LCDCS_VALID();
		for(pos=0;pos<16;pos++)
		{			
			temp=_16x32Number[num][pos];
			for(t=0;t<8;t++)
			{
				if(temp&0x01)
				{
					//g_penColor=(rsvMod&DSP_SELE)? BACK_COLOR:PEN_COLOR;//PEN_COLOR:BACK_COLOR;
					dspColor= (rsvMod&DSP_SELE)? bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
				}else
				{
					//g_penColor=(rsvMod&DSP_REVERSE)? PEN_COLOR:BACK_COLOR;//BACK_COLOR:PEN_COLOR;
					dspColor=(rsvMod&DSP_REVERSE)?penSetArr[pSnrSet[SNRMENU_PENCLR]]:bkSetArr[pSnrSet[SNRMENU_BKCLR]];
				}
				//LCD_DrawPoint(x,y);	
				LCD_WR_DATA(dspColor);
				temp>>=1; 
				//x++;
			}
			/*
			x=x0;
			y++;
			//if((11==pos)||(23==pos))
			//if((15==pos)||(31==pos)||(47==pos))
			if(pos==pEnd)
			{
				pEnd+=16;
				x0+=8;
				x = x0;
				y = y0;
			}*/
		}
		SET_LCDCS_INVALID();
		x+=8;
	}
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
	u16 i,j;
	u16 dspColor;
	//g_penColor = (rsv?BACK_COLOR:PEN_COLOR);
	dspColor= (rsv?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]]);
	g_hGpiob = DATAIN&0xff00;;
	//g_penColor = (rsv?SEL_COLOR:PEN_COLOR);
	AreaSet(x,y,x+ht-1,y+wt-1);
	SET_LCDCS_VALID();
	for(i=0;i<ht;i++)
	{
		for(j=0;j<wt;j++)
		{
			LCD_WR_DATA(dspColor);
		}
	}
	SET_LCDCS_INVALID();		
	//for(i=0;i<wt;i++)
	//{	
	//	for(j=0;j<ht;j++)
	//	{
	//		LCD_DrawPoint(x++,y+i);
	//	}
	//	x = x0;
	//}
}

//void DisplayButton(u16 x, u8 No, const char *p, u8 Font, u8 size, u8 fMod)
void DisplayButton(u16 x, u8 No,Button *btn,u8 fMod)
{
	u16 yLoc;
	u8 sp;
	yLoc = No*(LAYOUT_BUTTON_WIDTH+LAYOUT_BUTTON_INTERVAL);

	if(g_dspChinese>0)
	{
		LcdFillBlank(LAYOUT_MENU_PAGE-2,yLoc,LAYOUT_BUTTON_WIDTH,LAYOUT_BUTTON_HIGHT+2,DSP_NORMAL);
		x-=1;
	}else
	{
		LcdFillBlank(LAYOUT_MENU_PAGE,yLoc,LAYOUT_BUTTON_WIDTH,LAYOUT_BUTTON_HIGHT,DSP_NORMAL);//(u16 x, u16 y, u16 wt, u16 ht, u8 rsv)
	}

	if((g_dspChinese>0)&&(btn->dspCnStr[0]>0x80))
	{
		if(btn->cnStrLen>LAYOUT_BUTTON_WIDTH)
		{
			btn->cnStrLen = LAYOUT_BUTTON_WIDTH; 
			sp = 0;
		}else
		{
			sp = (LAYOUT_BUTTON_WIDTH - btn->cnStrLen)>>1;
		}
	}else
	{
		if(btn->strLen>LAYOUT_BUTTON_WIDTH)
		{
			btn->strLen = LAYOUT_BUTTON_WIDTH; 
			sp = 0;
		}else
		{
			sp = (LAYOUT_BUTTON_WIDTH - btn->strLen)>>1;
		}
	}

	if(!fMod)
	{
#ifdef _ST_EX_
		if(!g_dspChinese)
		{
			LCD_ShowString(x,yLoc+sp,btn->dspStr,FONT_TYPE_MENU,0,DSP_REVERSE);
		}else
		{
			LCD_ShowCnString(x,yLoc+sp,btn->dspCnStr,DSP_REVERSE);
		}
#else
		LCD_ShowString(x,yLoc+sp,btn->dspStr,FONT_TYPE_MENU,1,DSP_REVERSE);
#endif
	}else
	{
		LcdLoadPicture(x,yLoc+sp,btn->strLen,24,btn->dspStr,DSP_REVERSE);
	}
	
}

#if 0
void DisplayButton(u16 x, u8 No, const char *p, u8 Font, u8 size, u8 fMod)
{
	u16 yLoc;
	u8 sp;
	yLoc = No*(LAYOUT_BUTTON_WIDTH+LAYOUT_BUTTON_INTERVAL);

/*
	switch(Font)
	{
		case FONT_16x08:
			pa = 2;
		break;

		case FONT_24x12:
			pa = 3;
		break;

		case FONT_32x16:
			pa = 4;
		break;

		case FONT_16x08B:
			
		break;
	
	}
*/
	//LcdClearFeild(x,yLoc,80,24);

	LcdFillBlank(LAYOUT_MENU_PAGE,yLoc,LAYOUT_BUTTON_WIDTH,LAYOUT_BUTTON_HIGHT,DSP_NORMAL);//(u16 x, u16 y, u16 wt, u16 ht, u8 rsv)
	
	//for(i=0;i<pa;i++)
	//{
	//	LcdFillBlank(x+(i<<3),yLoc,78,DSP_NORMAL);
	//}
	//LcdFillBlank(x,yLoc,78);
	//LcdFillBlank(x+8,yLoc,78);
	if(size>LAYOUT_BUTTON_WIDTH)
	{
		size = LAYOUT_BUTTON_WIDTH; 
		sp = 0;
	}else
	{
		sp = (LAYOUT_BUTTON_WIDTH - size)>>1;
	}

	if(!fMod)
	{
#ifdef _ST_EX_
		if(!g_dspChinese)
		{
			LCD_ShowString(x,yLoc+sp,p,Font,0,DSP_REVERSE);
		}else
		{
			LCD_ShowCnString(x,yLoc+sp,p,DSP_REVERSE);
		}
#else
		LCD_ShowString(x,yLoc+sp,p,Font,1,DSP_REVERSE);
#endif
	}else
	{
		LcdLoadPicture(x,yLoc+sp,size,24,p,DSP_REVERSE);
	}
	
}
#endif

void InsertList(FuncList ls)
{
#if 0
	if(!g_dspChinese)
	{
		DisplayButton(LAYOUT_MENU_PAGE,0,ls.button[0]->dspStr,FONT_TYPE_MENU,ls.button[0]->strLen,ls.button[0]->dspMod);
		DisplayButton(LAYOUT_MENU_PAGE,1,ls.button[1]->dspStr,FONT_TYPE_MENU,ls.button[1]->strLen,ls.button[1]->dspMod);
		DisplayButton(LAYOUT_MENU_PAGE,2,ls.button[2]->dspStr,FONT_TYPE_MENU,ls.button[2]->strLen,ls.button[2]->dspMod);
		DisplayButton(LAYOUT_MENU_PAGE,3,ls.button[3]->dspStr,FONT_TYPE_MENU,ls.button[3]->strLen,ls.button[3]->dspMod);	
	}else
	{
		DisplayButton(LAYOUT_MENU_PAGE,0,ls.button[0]->dspCnStr,FONT_TYPE_MENU,ls.button[0]->cnStrLen,ls.button[0]->dspMod);
		DisplayButton(LAYOUT_MENU_PAGE,1,ls.button[1]->dspCnStr,FONT_TYPE_MENU,ls.button[1]->cnStrLen,ls.button[1]->dspMod);
		DisplayButton(LAYOUT_MENU_PAGE,2,ls.button[2]->dspCnStr,FONT_TYPE_MENU,ls.button[2]->cnStrLen,ls.button[2]->dspMod);
		DisplayButton(LAYOUT_MENU_PAGE,3,ls.button[3]->dspCnStr,FONT_TYPE_MENU,ls.button[3]->cnStrLen,ls.button[3]->dspMod);	
	}
#endif
	DisplayButton(LAYOUT_MENU_PAGE,0,ls.button[0],ls.button[0]->dspMod);
	DisplayButton(LAYOUT_MENU_PAGE,1,ls.button[1],ls.button[1]->dspMod);
	DisplayButton(LAYOUT_MENU_PAGE,2,ls.button[2],ls.button[2]->dspMod);
	DisplayButton(LAYOUT_MENU_PAGE,3,ls.button[3],ls.button[3]->dspMod);
	
}

void DisplayMstSetDot(u16 x, u16 y,u8 rsv)
{
	u8 i,j;
	u16 x0=x;
	//g_penColor = ((rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?BACK_COLOR:PEN_COLOR);
	//g_penColor = (rsv&DSP_SELE)?SEL_COLOR:(rsv&DSP_REVERSE)?bkSetArr[pSnrSet[SNRMENU_BKCLR]]:penSetArr[pSnrSet[SNRMENU_PENCLR]];
	//AreaSet(u16 x1, u16 y1, u16 x2, u16 y2);
	for(i=0;i<FONT_DOT_SIZE;i++)
	{	
		for(j=0;j<FONT_DOT_SIZE;j++)
		{
			//LCD_DrawPoint(2+x++,2+y+i,rsv);
			LCD_DrawPoint(1+x++,1+y+i,rsv);
		}
		x = x0;
	}
}

void DisplayTtlCmp_classStyle(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char rvs)
{
	char tmp1,tmp2;
	if(cmp1<0)
	{
		tmp2 = cmp2;
#if(_DSP_FONT_OPTION_==BIG_FONT)
		LCD_ShowChar(page,cml,'-',FONT_TYPE_MAIN,rvs);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
		LCD_ShowChar(page,cml,'-',FONT_TYPE_MAIN,rvs);
#endif

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
#if(_DSP_FONT_OPTION_==BIG_FONT)		
		LCD_ShowChar(page,cml,'+',FONT_TYPE_MAIN,rvs);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
		LCD_ShowChar(page,cml,'+',FONT_TYPE_MAIN,rvs);
#endif
	}

	//LCD_ShowString(page,cml+12,(u8*)(cmpArr+tmp1*3+tmp2),FONT_24x12,DSP_NORMAL);
#if(_DSP_FONT_OPTION_==BIG_FONT)
	LCD_ShowString(page,cml+MAIN_FONT_WIDTH(1),cmpArr[tmp1*3+tmp2],FONT_TYPE_MAIN,0,rvs);	
	LCD_ShowString(page,cml+FONT_INTERVAL(1)+MAIN_FONT_WIDTH(3),"Ev",FONT_TYPE_MAIN,0,rvs);
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	LCD_ShowString(page,cml+MAIN_FONT_WIDTH(1),cmpArr[tmp1*3+tmp2],FONT_TYPE_MAIN,0,rvs);	
	LCD_ShowString(page,cml+FONT_INTERVAL(1)+MAIN_FONT_WIDTH(3),"Ev",FONT_TYPE_MAIN,0,rvs);
#endif
}

void DisplayTtlScale(u16 page, u16 cml, u16 org, u16 radius, unsigned char rvs)
{
	DisplayMstSetDot(page,cml+AUX_FONT_WIDTH(org)+AUX_FONT_WIDTH(1), rvs);		//"+8" should take the '-' take into consideration
	if(org<radius)
	{
		DisplayPicture(page, cml, FEB_ARROW_SIZE, FEB_ARROW_SIZE, febDf,rvs);
	}else
	{
		DisplayPicture(page, cml+AUX_FONT_WIDTH(org-radius)+AUX_FONT_WIDTH(1), FEB_ARROW_SIZE, FEB_ARROW_SIZE, febLab,rvs);
	}
	
	if(org+radius>18)
	{
		DisplayPicture(page, cml+AUX_FONT_WIDTH(20), FEB_ARROW_SIZE, FEB_ARROW_SIZE,febOf,rvs);
	}else
	{
		DisplayPicture(page, cml+AUX_FONT_WIDTH(org+radius)+AUX_FONT_WIDTH(1), FEB_ARROW_SIZE, FEB_ARROW_SIZE, febLab,rvs);
	}
	
}

void DisplayTtlCmp_canStyle(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char rvs)
{
	u16 dspSt=0;
	u16 rds=0;
	dspSt =( cmp1 + 3)*3 + cmp2;
	if(WORKINGMODE_TTL==mainPara.workMode)
	rds = flaPara.ttl_FEB1*3+flaPara.ttl_FEB2;
	//LcdClearFeild(page, cml, 24, 160);
	//LcdClearFeild(page, cml, 160, 24);
#if(_DSP_FONT_OPTION_==BIG_FONT)
	LCD_ShowString(page,cml,cmpString,FONT_TYPE_AUX,0,rvs);
	//LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN),LAYOUT_DOTAREA_HT ,1-rvs);		//clear the field or highlight
	/*Clear field at every time when parameter changed*///LcdClearFeild(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN),LAYOUT_DOTAREA_HT);
	DisplayMstSetDot(page+AUX_FONT_WIDTH(1),cml+AUX_FONT_WIDTH(dspSt)+AUX_FONT_WIDTH(1), rvs);		//"+8" should take the '-' take into consideration
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	LCD_ShowString(page,cml,cmpString,FONT_TYPE_AUX,0,rvs);
	//LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);		//clear the field or highlight
	/*Clear field at every time when parameter changed*/
	//LcdClearFeild(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN),LAYOUT_DOTAREA_HT);
	LcdClearFeild(page+AUX_FONT_HIGHT(1), cml, AUX_FONT_WIDTH(sizeof(cmpString))+1,BIG_FONT_HIGHT(1)-AUX_FONT_HIGHT(1));
	if(!rds)
	DisplayMstSetDot(page+AUX_FONT_HIGHT(1),cml+AUX_FONT_WIDTH(dspSt)+AUX_FONT_WIDTH(1), rvs);		//"+8" should take the '-' take into consideration
	else
	DisplayTtlScale(page+AUX_FONT_HIGHT(1),cml,dspSt,rds,rvs);
#endif	
}

void DisplayTtlCmp(u16 page,u16 cml,signed char cmp1,signed char cmp2,unsigned char rvs)
{
	if(DSP_STYLE_CANON==g_dspStyle)
	{
		DisplayTtlCmp_canStyle(page,cml,cmp1,cmp2,rvs);
	}else if(DSP_STYLE_CLASSIC==g_dspStyle)
	{
		DisplayTtlCmp_classStyle(page,cml,cmp1,cmp2,rvs);
	}
}

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

void DisplayMstOtp_canStyle(u16 page,u16 cml,u8 mod,unsigned char para1,unsigned char para2,unsigned char rvs)
{
	u8 dspSt,dsHead;
	unsigned char *pStart;
	u8 strLen;
	
	//if(para1<0)	return;
	//dspSt = otpStrIndex[(para1-1)*3+para2];
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

/*
	if(DSP_REVERSE==rvs)
	{
		LcdFillBlank(page+16,cml,GR_DSP_LEN*8, DSP_NORMAL);
	}
*/	
	//LcdClearFeild(page, cml, AUX_FONT_WIDTH(17), 24);
	//LcdClearFeild(page, cml, AUX_FONT_WIDTH(17), 24);

//	LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);

	pStart = (unsigned char*)outputString+dsHead;
	
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
	
#if(_DSP_FONT_OPTION_==BIG_FONT)
	LcdPutBuf(page,cml,pStart,FONT_TYPE_MAST,GR_DSP_LEN,0,rvs);
	LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);
	DisplayMstSetDot(page+AUX_FONT_HIGHT(1),cml+(dspSt-dsHead)*AUX_FONT_WIDTH(1),rvs);	
#elif(_DSP_FONT_OPTION_==SMALL_FONT)
	LcdPutBuf(page,cml,pStart,FONT_TYPE_MAST,GR_DSP_LEN,0,rvs);
	LcdFillBlank(page+AUX_FONT_HIGHT(1),cml,AUX_FONT_WIDTH(GR_DSP_LEN), LAYOUT_DOTAREA_HT,1-rvs);
	DisplayMstSetDot(page+AUX_FONT_HIGHT(1),cml+(dspSt-dsHead)*AUX_FONT_WIDTH(1),rvs);	
#endif
	
	//LcdFillBlank(page+16,cml,GR_DSP_LEN*8,rvs);
}

void DisplayABRation(signed char ABr, unsigned char rsv)		//0~~12
{
	u8 dspSt,dsHead;
	unsigned char *pStart;
	dspSt = (ABr*3+1);
	if(dspSt<7)
	{
		dsHead = 0;
	}else if(dspSt<(39-7))
	{
		dsHead = dspSt-7;
	}else
	{
		dsHead = (39 -ABr_DSP_LEN);
	}
	pStart = (unsigned char*)abRatio+dsHead;
	//LcdClearFeild(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA, 24,AUX_FONT_WIDTH(17));
	//LcdClearFeild(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,AUX_FONT_WIDTH(17), 24);
	LcdPutBuf(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(3),pStart,FONT_TYPE_MAST,ABr_DSP_LEN,0,rsv);
	LcdFillBlank(LAYOUT_RATIO_A+AUX_FONT_HIGHT(1),LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(3),AUX_FONT_WIDTH(GR_DSP_LEN),LAYOUT_DOTAREA_HT,(rsv==DSP_SELE)?DSP_SELE:DSP_REVERSE);
	DisplayMstSetDot(LAYOUT_RATIO_A+AUX_FONT_HIGHT(1),LAYOUT_RATIO_STPARA+AUX_FONT_WIDTH(dspSt-dsHead+3),rsv);
	
}

void DisplayMstOtp(u16 page,u16 cml,u8 mod,unsigned char para1,unsigned char para2,unsigned char rvs)
{
	if(DSP_STYLE_CANON==g_dspStyle)
	{
		DisplayMstOtp_canStyle(page,cml,mod,para1,para2,rvs);
	}else if(DSP_STYLE_CLASSIC==g_dspStyle)
	{
		DisplayMstOtp_classStyle(page,cml,para1,para2,rvs);
	}
}

void DisplayOutput(u16 pa, u16 cml,u8 mod,uchar otp,uchar otpOffset,uchar dspRsv)
{
	if(DSP_STYLE_CANON==g_dspStyle)
	{
		DisplayMstOtp_canStyle(pa,cml,mod,otp,otpOffset,dspRsv);
	}else if(DSP_STYLE_CLASSIC==g_dspStyle)
	{
		DisplayMstOtp_classStyle(pa,cml,otp,otpOffset,dspRsv);
	}
}

void DspMstModIcon(u8 mod)
{
	//printf("lite or rf\r\n");
	
	switch(mod)
	{
		case CMD_MODE_LIGH_MST:
		case CMD_MODE_LIGH_SLA:
			//printf("lite\r\n");
			ENABLE_CHARGE;
			DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_SIZE,liteBitmap,DSP_NORMAL);
		break;

		case CMD_MODE_RF_MST:
		case CMD_MODE_RFLS_MST:
		case CMD_MODE_RF_SLA:
		case CMD_MODE_RFLS_SLA:
			DISABLE_CHARGE;
			//printf("rf\r\n");
			DisplayPicture(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_SIZE,rfBitmap,DSP_NORMAL);
		break;

		case CMD_MODE_OFF:
			ENABLE_CHARGE;
			//printf("off\r\n");
			LcdClearFeild(LAYOUT_MSTMODE_PAGE,LAYOUT_MSTMODE_CML,FONT_MSTMOD_SIZE,FONT_MSTMOD_SIZE);
		break;
	}
}

void SetMstHead(char *arr)
{
}

void SetScanChan(char ch)
{
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
	LcdClearFeild(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16, BIG_FONT_WIDTH(2)+AUX_FONT_WIDTH(3)+2, BIG_FONT_HIGHT(1));
#else
	LcdClearFeild(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16, BIG_FONT_WIDTH(2)+AUX_FONT_WIDTH(3), BIG_FONT_HIGHT(1));
#endif

	if(!ch)
	{
		//LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), 'A', FONT_TYPE_BIG,rvc);
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
		LCD_ShowString(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-8,"AUTO",FONT_TYPE_MAIN,0,DSP_NORMAL);
#else
		LCD_ShowString(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-16,"AUTO",FONT_TYPE_MAIN,0,DSP_NORMAL);
#endif
	}else if(ch<10)
	{
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,DSP_NORMAL);
#else
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,DSP_NORMAL);
#endif
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), ch+'0', FONT_TYPE_BIG,DSP_NORMAL);
	}else
	{
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,DSP_NORMAL);
#else
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,DSP_NORMAL);
#endif
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), ch/10+'0', FONT_TYPE_BIG,DSP_NORMAL);
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3)+BIG_FONT_WIDTH(1), ch%10+'0', FONT_TYPE_BIG,DSP_NORMAL);
	}

}

void SetMstChan(char ch,char rvc)
//void SetMstChan(void)
{/*
	u8 ch;
	u8 rvc;
	if (WLM_SETTING_CHAN==g_SettingIndex)
	{
		ch = g_rfChanSet;
		rvc = DSP_SELE;
	}else
	{
		ch = radioSet->rfChan;
		rvc = DSP_NORMAL;
	}*/
	//LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,rvc);
	//LcdClearFeild(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), BIG_FONT_WIDTH(2), BIG_FONT_HIGHT(1));
	if((WORKINGMODE_GR==mainPara.workMode)&&(g_grStyle>0))
	{
		LcdClearFeild(LAYOUT_MSTMODE_PAGE+4, LAYOUT_SYNMODE_LINE-16, BIG_FONT_WIDTH(2)+AUX_FONT_WIDTH(3)+2, BIG_FONT_HIGHT(1));
		if(!ch)
		{
			LCD_ShowString(LAYOUT_MSTMODE_PAGE+4,LAYOUT_SYNMODE_LINE-8,"AUTO",FONT_TYPE_MAIN,0,rvc);
		}else if(ch<10)
		{
			LCD_ShowString(LAYOUT_MSTMODE_PAGE+4+8,LAYOUT_SYNMODE_LINE-16,"Ch.",FONT_TYPE_AUX,0,rvc);
			LCD_ShowChar(LAYOUT_MSTMODE_PAGE+4, LAYOUT_SYNMODE_LINE-16+AUX_FONT_WIDTH(3), ch+'0', FONT_TYPE_BIG,rvc);
		}else
		{
			LCD_ShowString(LAYOUT_MSTMODE_PAGE+4+8,LAYOUT_SYNMODE_LINE-16,"Ch.",FONT_TYPE_AUX,0,rvc);
			LCD_ShowChar(LAYOUT_MSTMODE_PAGE+4, LAYOUT_SYNMODE_LINE-16+AUX_FONT_WIDTH(3), ch/10+'0', FONT_TYPE_BIG,rvc);
			LCD_ShowChar(LAYOUT_MSTMODE_PAGE+4, LAYOUT_SYNMODE_LINE-16+AUX_FONT_WIDTH(3)+BIG_FONT_WIDTH(1), ch%10+'0', FONT_TYPE_BIG,rvc);
		}
		
	}else
	{
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
	LcdClearFeild(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16, BIG_FONT_WIDTH(2)+AUX_FONT_WIDTH(3)+2, BIG_FONT_HIGHT(1));
#else
	LcdClearFeild(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16, BIG_FONT_WIDTH(2)+AUX_FONT_WIDTH(3), BIG_FONT_HIGHT(1));
#endif

	if(!ch)
	{
		//LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), 'A', FONT_TYPE_BIG,rvc);
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
		LCD_ShowString(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-8,"AUTO",FONT_TYPE_MAIN,0,rvc);
#else
		LCD_ShowString(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-16,"AUTO",FONT_TYPE_MAIN,0,rvc);
#endif
	}else if(ch<10)
	{
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,rvc);
#else
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,rvc);
#endif
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), ch+'0', FONT_TYPE_BIG,rvc);
	}else
	{
#if((_HARD_WARE_==HARD_WARE_VER1)||(_HARD_WARE_==HARD_WARE_VER3))
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,rvc);
#else
		LCD_ShowString(LAYOUT_PARA_AREA1+8,LAYOUT_MSTMODE_CML-16,"Ch.",FONT_TYPE_AUX,0,rvc);
#endif
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3), ch/10+'0', FONT_TYPE_BIG,rvc);
		LCD_ShowChar(LAYOUT_PARA_AREA1, LAYOUT_MSTMODE_CML-16+AUX_FONT_WIDTH(3)+BIG_FONT_WIDTH(1), ch%10+'0', FONT_TYPE_BIG,rvc);
	}
	}
	//LcdClearFeild(LAYOUT_PARA_AREA1,LAYOUT_MSTMODE_CML-16,24,18+10);	
}

void DisplayMstSetting(char arr, unsigned char mode, char para1,char para2)
{
}

void DisplayPicture(u16 plat,u16 column,uchar wide,uchar tall,const char *p,uchar rsv)
{
	LcdLoadPicture(plat,column,wide,tall,p,rsv);
}


void DisplayCanMstHead(unsigned char sel,char mode,WlSet *wlPara)
{
// Note: the parameter sel is not the sel index, it indicates the master setting;
//1: Ratio 2: A 3: B 4: C 5: chan
//	char dsp = 0;
//	if(1==sel)	dsp = 1;			//this determined the selected option, if it is, it should be highlight
	
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
			switch(*wlPara->arr_mode)
			{
				case WLM_MODE_ALL:
				LCD_MSTALL_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/								
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("ALL")-1)*BIG_FONT_WIDTH(1));
				DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2,((2==sel)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_AB:
				LCD_MSTAB_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A:B")-1)*BIG_FONT_WIDTH(1));
				//DisplayTtlCmp(LAYOUT_RATIO_A, LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_A].cmp1,wlPara->m_arrPara[WL_ARRARY_A].cmp2);
				DisplayABRation(wlPara->m_arrPara[WL_ARRARY_B].cmp1,((3==sel)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_RES:
				LCD_MSTRES_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A:B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A:B")-1)*BIG_FONT_WIDTH(1));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"C",((4==sel)?DSP_SELE:DSP_NORMAL),(sizeof("C")-1)*12);
				DisplayABRation(wlPara->m_arrPara[WL_ARRARY_B].cmp1, ((3==sel)?DSP_SELE:DSP_NORMAL));
				DisplayTtlCmp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,wlPara->m_arrPara[WL_ARRARY_C].cmp1,wlPara->m_arrPara[WL_ARRARY_C].cmp2,(4==sel)?DSP_SELE:DSP_NORMAL);
				break;
			}
		break;

		case WORKINGMODE_MANU:
			switch(*wlPara->arr_mode)
			{
				case WLM_MODE_ALL:
				LCD_MSTALL_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("ALL")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_A].output1,wlPara->m_arrPara[WL_ARRARY_A].output2,((2==sel)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_AB:
				LCD_MSTAB_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_A].output1,wlPara->m_arrPara[WL_ARRARY_A].output2,((2==sel)?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_B].output1,wlPara->m_arrPara[WL_ARRARY_B].output2,((3==sel)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_RES:
				LCD_MSTABC_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_A].output1,wlPara->m_arrPara[WL_ARRARY_A].output2,((2==sel)?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_B].output1,wlPara->m_arrPara[WL_ARRARY_B].output2,((3==sel)?DSP_SELE:DSP_NORMAL));
				//if(4==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",((4==sel)?DSP_SELE:DSP_NORMAL),(sizeof("C")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_C,LAYOUT_RATIO_STPARA,WORKINGMODE_MANU,wlPara->m_arrPara[WL_ARRARY_C].output1,wlPara->m_arrPara[WL_ARRARY_C].output2,((4==sel)?DSP_SELE:DSP_NORMAL));
				break;
			}
		break;

		case WORKINGMODE_MULTI:
			switch(*wlPara->arr_mode)
			{
				case WLM_MODE_ALL:
				LCD_MSTALL_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"ALL",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("ALL")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp>>4,0,((2==sel)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_AB:
				LCD_MSTAB_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp>>4,0,((2==sel)?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = 0;
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));		
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_B].multi_exp>>4,0,((3==sel)?DSP_SELE:DSP_NORMAL));
				break;

				case WLM_MODE_RES:
				LCD_MSTABC_ON(dsp);
				/*
				if(2==sel)
				{
					dsp = DSP_SELE;
				}else
				{
					dsp = DSP_NORMAL;
				}*/
				DspMstRatioHead(LAYOUT_RATIO_A, LAYOUT_RATIO_COLUMN,"A",((2==sel)?DSP_SELE:DSP_NORMAL),(sizeof("A")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_A,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_A].multi_exp>>4,0,((2==sel)?DSP_SELE:DSP_NORMAL));
				//if(3==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				DspMstRatioHead(LAYOUT_RATIO_B, LAYOUT_RATIO_COLUMN,"B",((3==sel)?DSP_SELE:DSP_NORMAL),(sizeof("B")-1)*BIG_FONT_WIDTH(1));	
				DisplayMstOtp(LAYOUT_RATIO_B,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_B].multi_exp>>4,0,((3==sel)?DSP_SELE:DSP_NORMAL));
				//if(4==sel)	dsp = DSP_SELE;
				//else			dsp = DSP_NORMAL;
				DspMstRatioHead(LAYOUT_RATIO_C, LAYOUT_RATIO_COLUMN,"C",((4==sel)?DSP_SELE:DSP_NORMAL),(sizeof("C")-1)*BIG_FONT_WIDTH(1));
				DisplayMstOtp(LAYOUT_RATIO_C,LAYOUT_RATIO_STPARA,WORKINGMODE_MULTI,wlPara->m_arrPara[WL_ARRARY_C].multi_exp>>4,0,((4==sel)?DSP_SELE:DSP_NORMAL));
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

void FuncDsp()
{
	if(pSnrSet[SNRMENU_SOUND])
	{
		DisplayPicture(LAYOUT_SPEAKER_PAGE,LAYOUT_SPEAKER_CML,FONT_SPEAKER_WT,FONT_SPEAKER_HT,speaker,DSP_NORMAL);
	}
}

void DisplayRmt(char arr, char chan,char sel)
{
}




void DisplayFocLen(uchar motoMode,uchar foc,uchar rsv)
{
	uchar tmp1,tmp2,tmp3;
	if(MOTO_MODE_AUTO==motoMode)
	{
		DisplayPicture(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO,FONT_MOTO_WT,FONT_MOTO_HT,aMoto,DSP_REVERSE);
		
	}else if(MOTO_MODE_MANU==motoMode)
	{
		DisplayPicture(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST, LAYOUT_ROW_MOTO,FONT_MOTO_WT,FONT_MOTO_HT,mMoto,DSP_REVERSE);
	}
	
	tmp1 = foc /100;
	tmp2 = (foc %100)/10;
	tmp3 = foc%10;
	LCD_ShowString(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST,LAYOUT_ROW_MOTO+FONT_MOTO_WT+2,"Zoom",FONT_TYPE_MAIN,0,DSP_NORMAL);
	LcdClearFeild(LAYOUT_PAGE_MOTO,LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1),BIG_FONT_WIDTH(3),BIG_FONT_HIGHT(1));
	if(tmp1)
	{
		LCD_ShowChar(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO+FONT_MOTO_WT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+4, tmp1+'0', FONT_TYPE_BIG,rsv);
	}
	
	LCD_ShowChar(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO+FONT_MOTO_HT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(1)+4, tmp2+'0', FONT_TYPE_BIG,rsv);
	LCD_ShowChar(LAYOUT_PAGE_MOTO, LAYOUT_ROW_MOTO+FONT_MOTO_HT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(2)+4, tmp3+'0', FONT_TYPE_BIG,rsv);
	LCD_ShowString(LAYOUT_PAGE_MOTO+LAYOUT_MOTO_ADJUST,LAYOUT_ROW_MOTO+FONT_MOTO_HT+2+MAIN_FONT_WIDTH(sizeof("Zoom")-1)+BIG_FONT_WIDTH(3)+4+4,"mm",FONT_TYPE_MAIN,0,DSP_NORMAL);
	
}

void CanSetFeb(char feb1,char feb2, unsigned char rsv)
{
	LCD_FEB_ON(rsv);
	LcdClearFeild(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,MAIN_FONT_WIDTH(7),MAIN_FONT_HIGHT(1));
	//DisplayTtlCmp_classStyle(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML,feb1,feb2,rsv);
	LCD_ShowString(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+MAIN_FONT_WIDTH(1),cmpArr[feb1*3+feb2],FONT_TYPE_MAIN,0,rsv);	
	LCD_ShowString(LOCATION_TTLCMP_PAGE,LOCATION_TTLCMP_CML+FONT_INTERVAL(1)+MAIN_FONT_WIDTH(3),"Ev",FONT_TYPE_MAIN,0,rsv);
}

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
	funcMenu.funcList[listCnt].button[0] = btn1;
	funcMenu.funcList[listCnt].button[1] = btn2;
	funcMenu.funcList[listCnt].button[2] = btn3;
	funcMenu.funcList[listCnt].button[3] = btn4;
	//funcMenu.listCnt++;
}

void BuildFuncMenu(void)
{
	
	switch(mainPara.workMode)
	{
		case WORKINGMODE_TTL:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&ttlFeb,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mRatio,&mGr,&menu2,funcMenu.listCnt++);
					if(SYNMODE_HEAD==mainPara.synMode)
					{
						AddButtons(&mCh,&mSync_fh,&mMemory,&menu3,funcMenu.listCnt++);
					}else if(SYNMODE_HP==mainPara.synMode)
					{
						AddButtons(&mCh,&mSync_hp,&mMemory,&menu3,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_RF_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&ttlFeb,&menu1,funcMenu.listCnt++);
					AddButtons(&mstFlaSw,&mRatio,&mGr,&menu2,funcMenu.listCnt++);
					AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
					//AddButtons(&mReset,&mSync,&mMemory,&menu4,funcMenu.listCnt++);
					//AddButtons(&mNull,&mSync,&mMemory,&menu4,funcMenu.listCnt++);
					if(SYNMODE_HEAD==mainPara.synMode)
					{
						AddButtons(&mNull,&mSync_fh,&mMemory,&menu4,funcMenu.listCnt++);
					}else if(SYNMODE_HP==mainPara.synMode)
					{
						AddButtons(&mNull,&mSync_hp,&mMemory,&menu4,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_OFF:
					funcMenu.listCnt = 0;
					if(SYNMODE_HEAD==mainPara.synMode)
					{
						AddButtons(&cfn,&addSub,&ttlFeb,&mSync_fr,funcMenu.listCnt++);
					}else if(SYNC_MODE_FP==mainPara.synMode)
					{
						AddButtons(&cfn,&addSub,&ttlFeb,&mSync_hp,funcMenu.listCnt++);
					}else
					{
						AddButtons(&cfn,&addSub,&ttlFeb,&mSync_rh,funcMenu.listCnt++);
					}
				break;
			}
		break;

		case WORKINGMODE_MANU:
			//switch(g_wlCmdMod)
			switch(*g_cmdMode)
			{
				case CMD_MODE_LIGH_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&mRatio,&mGr,&menu1,funcMenu.listCnt++);
					//AddButtons(&mstFlaSw,&mSync,&mNull,&menu2,funcMenu.listCnt++);
					if(SYNMODE_HEAD==mainPara.synMode)
					{
						AddButtons(&mstFlaSw,&mSync_fh,&mNull,&menu2,funcMenu.listCnt++);
					}else if(SYNMODE_HP==mainPara.synMode)
					{
						AddButtons(&mstFlaSw,&mSync_hp,&mNull,&menu2,funcMenu.listCnt++);
					}
					AddButtons(&mCh,&mNull,&mMemory,&menu3,funcMenu.listCnt++);
				break;

				case CMD_MODE_RF_MST:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&mRatio,&mGr,&menu1,funcMenu.listCnt++);
					//AddButtons(&mstFlaSw,&mSync,&mNull,&menu2,funcMenu.listCnt++);
					if(SYNMODE_HEAD==mainPara.synMode)
					{
						AddButtons(&mstFlaSw,&mSync_fh,&mNull,&menu2,funcMenu.listCnt++);
					}else if(SYNMODE_HP==mainPara.synMode)
					{
						AddButtons(&mstFlaSw,&mSync_hp,&mNull,&menu2,funcMenu.listCnt++);
					}
					AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
					AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_OFF:
					funcMenu.listCnt = 0;
					//AddButtons(&cfn,&addSub,&mNull,&mSync,funcMenu.listCnt++);
					if(SYNMODE_HEAD==mainPara.synMode)
					{
						AddButtons(&cfn,&addSub,&mNull,&mSync_fr,funcMenu.listCnt++);
					}else if(SYNC_MODE_FP==mainPara.synMode)
					{
						AddButtons(&cfn,&addSub,&mNull,&mSync_hp,funcMenu.listCnt++);
					}else
					{
						AddButtons(&cfn,&addSub,&mNull,&mSync_rh,funcMenu.listCnt++);
					}
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
					AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
				break;

				case CMD_MODE_OFF:
					funcMenu.listCnt = 0;
					AddButtons(&cfn,&addSub,&mMulti,&mFrq,funcMenu.listCnt++);					
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
					if(!g_grStyle)
					{
						AddButtons(&cfn,&addSub,&mGr,&menu1,funcMenu.listCnt++);
						//AddButtons(&mstFlaSw,&ttlFeb,&mSync,&menu2,funcMenu.listCnt++);
						if(SYNMODE_HEAD==mainPara.synMode)
						{
							AddButtons(&mstFlaSw,&ttlFeb,&mSync_fh,&menu2,funcMenu.listCnt++);
						}else if(SYNMODE_HP==mainPara.synMode)
						{
							AddButtons(&mstFlaSw,&ttlFeb,&mSync_hp,&menu2,funcMenu.listCnt++);
						}
						AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
						//AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
						AddButtons(&mGrStyle,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
					}else
					{
						AddButtons(&cfn,&mNull,&mGr,&menu1,funcMenu.listCnt++);
						AddButtons(&mGrStyle,&mNull,&mNull,&menu2,funcMenu.listCnt++);
						AddButtons(&mCh,&mId,&mScan,&menu3,funcMenu.listCnt++);
						//AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
						AddButtons(&mNull,&mNull,&mMemory,&menu4,funcMenu.listCnt++);
					}
				break;

				case CMD_MODE_OFF:
					funcMenu.listCnt = 0;
				break;
			}
		break;

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















