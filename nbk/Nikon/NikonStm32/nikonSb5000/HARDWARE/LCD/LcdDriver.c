//#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"
#include "delay.h"	 
#include "includes.h"
#include "../../APP/Nikon.h"
#include "../../APP/KeyProc.h"
//#include "../../USER/Display.h"
//#include "../../USER/KeyProc.h"
//#include "../../USER/canon.h"
#include "../STMFLASH/stmflash.h"
//#include "../../USER/menu.h"
//#include "../DMA/DMA.h"
//#include "../../USER/FlashPara.h"
#define DISP_ON           0XAF
#define DISP_OFF         0XAE

#define LCD_WIDTH       172
#define EV_ST75256        300

#define SEG_START	84 
#define EV_LSB		 (EV_ST75256&0X3F)
#define EV_MSB		 (((EV_ST75256>>1)&0XE0)>>5)
#define EV_MAX		330
//#define EV_MIN		300
#define EV_MIN           295

#if 1


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
}


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

void LcdPortInit(void)
{

	//RCC->APB1RSTR |=(1<<14);			//reset spi2
	//RCC->APB1RSTR &=~(1<<14);
	//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	//delay_ms(1);
	//LCD_RST = 0;
	//delay_ms(10);
	//BACKLIGHT_ON(BACKLIGHT_DELAY_NORMAL);
	

	RCC->APB1ENR|=1<<14;      //SPI2时钟使能
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能 	 
	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	RCC->APB2ENR|=1<<5;	//PORTD时钟使能
 		   
	//GPIOB->CRH&=0X0F0F0FFF; 
	//GPIOB->CRH|=0XB0B03000;//PB13.14.15复用 	    
	//GPIOB->ODR|=0XF<<12;   //PB13.14.15上拉
	GPIOC->CRH&=0xFFFFF0FF;
	GPIOC->CRH|=0x00000300;

	LCD_RST = 1;
	delay_ms(5);

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
	//SetContrast(pSnrSet[SNR_CONTRAST]);
	//SetGrayLevel(0x08,0x08);
	//DBG_PRINTF("start2");
}

void DisplayOff(void)
{
	LCD_WRITE_CMD(DISP_OFF);	
}


void DisplayPicture(u8 page,u8 column,u8 wide,u8 tall,const char *p,u8 rsv)
{
	 u8 i=0,j=0;

	//AreaSet(page, column, wide, tall);
	AreaSet(page, column, wide, tall%8?(tall+8)&0xF8:tall);
	
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

void DisplayPictureEx(u8 x,u8 y,u8 wide,u8 tall,const char *p,u8 rsv)
{

	u8 i=0,j=0;

	if(rsv>0)
	{
		memset(g_paraSetBuf,0xff,(tall/8+1)*wide);
		
			for(j=0;j<(tall+7)/8;tall++)
			{
				for(i=0;i<wide;i++)
				{
					g_paraSetBuf[j*wide+i] = ~p[j*wide+i];
				}
			}
		//memcpy(g_paraSetBuf,p,tall/8*wide);
	}else
	{
		memset(g_paraSetBuf,0x00,(tall/8+1)*wide);
		memcpy(g_paraSetBuf,p,(tall/8+1)*wide);
	}

	if(x%8)
	{
		AreaScroll((u8*)g_paraSetBuf,wide,tall/8,wide,SCROLL_DOWN,x%8);
	}

	//AreaSet(x/8,y,wide,tall);
	LcdCopyToRam(x/8, y, g_paraSetBuf, wide, tall);

	

}

void CalcArrayMask(char* dest, char *src, u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
		*dest++ = ~*src++;
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

void SetContrast(char lev)
{
	int val;
	val = ((EV_MAX- EV_MIN)/9)*lev+EV_MIN;
	LCD_WRITE_CMD(0x30);   	// Extension Command 2
	LCD_WRITE_CMD(0x81);   	// Extension Command 2
	
	LCD_WRITE_BYTE(val&0X3F);  	// VPR[5-0]	  ((EV>>1)&0XE0)>>5
	LCD_WRITE_BYTE(((val>>1)&0Xe0)>>5);   // VPR[8-6]  vop=(10-3.6)/0.04=160=1010 0000
}

//char (*pTest)[5] = {test_ttl,test_master,test_rpt,test_m,test_sttl};
void Lcd_test(void)
{
//	u8 i=0;	
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
//		DspIsoValue(LAYOUT_PARA_AREA2, 60,isoValue[i++],DSP_NORMAL);

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


void DspClearAllPara(void)
{

	LcdClearFeild(0, 0, LCD_W, LCD_H-16);	
	
}

void LcdLoadPicture(u16 x, u16 y, u16 wide, u16 tall, const char* pBit,u8 rsv)
{
	DisplayPicture(x,y,wide,tall,pBit,rsv);
}

void DspParaNum(u8 ft,u8 x, u8 y,u8 num,u8 rsv)
{
	u8 fwt=0;
	u16 i=0;
	char *pFont=NULL;

	switch(ft)
	{
		case FONT_STYLE_ISO:
			fwt = FONT_WT_ISO;
			pFont = (char*)(font_isoNum+num*fwt*2);
		break;

		case FONT_STYLE_APERT:
			fwt = FONT_WT_APERT;
			pFont= (char*)(font_apertNum+num*fwt*2);
		break;

		case FONT_STYLE_SCALE:
			fwt = FONT_WT_SCALE;
			pFont = (char*)(font_scaleNum+num*fwt*2);
		break;
	}
	
	AreaSet(x, y, fwt, FONT_HT_N);

	for(i=0;i<fwt*(FONT_HT_N/8);i++)
	{
		LCD_WRITE_BYTE(DSP_NORMAL==rsv?*pFont++:~*pFont++);
		//LCD_WRITE_BYTE(DSP_NORMAL==rsv?font_scaleNum[i]:~font_scaleNum[i]);
		//LCD_WRITE_BYTE(test[i]);
	}
	
}	



#endif







