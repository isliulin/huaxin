#include "lcd.h"
#include"global.h"
#include "..\sys\sys.h"
#include <stdarg.h>

#define LCD_CS(x)  {PE_ODR_ODR6=(x);}
#define LCD_SCK(x)   {PD_ODR_ODR1=(x);}
#define LCD_DATA(x)   {PD_ODR_ODR3=(x);}

#define LCD_RD(x)       {PB_ODR_ODR3=(x);}

#define LCD_DATA_IN()      {PD_DDR_DDR3=0;}
#define LCD_DATA_OUT()   {PD_DDR_DDR3=1;}
#define LCD_RD_PIN                   PD_IDR_IDR3

#define TYPE_CMD   0
#define TYPE_DATA 1

u8 g_beepCnt = 0;
u8 g_beepTms = 0;
u8 g_beepEn = 0;

//u8 fb[16] = {0x00};

void LcdPortInit(void)
{
	// PE6 <--> CS
	// PD1 <--> WR
	// PD3 <--> DATA

	PE_DDR_DDR6 = 1;
	PE_CR1_C16 = 1;
	PE_CR2_C26 = 1;

	PD_DDR_DDR1 = 1;
	PD_CR1_C11 = 1;
	PD_CR2_C21 = 1;

	PD_DDR_DDR3 = 1;
	PD_CR1_C13 = 1;
	//PD_CR2_C23 = 1;

	LCD_CS(1);	
	
}

#define Lcd_delay()     //{ delay_us(2); }

void SendCmdHead(u8 head)
{
	LCD_SCK(0);
	LCD_DATA((head>>2)&0x01);
	Lcd_delay();
	LCD_SCK(1);
	Lcd_delay();
	
	LCD_SCK(0);
	LCD_DATA((head>>1)&0x01);
	Lcd_delay();
	LCD_SCK(1);
	Lcd_delay();
	
	LCD_SCK(0);
	LCD_DATA(head&0x01);
	Lcd_delay();
	LCD_SCK(1);

	Lcd_delay();
}

void LcdRead(u8 addr, u8 * rdBuf, u8 cnt)
{
	u8 i = 0, j = 0;

	LCD_DATA_OUT();
	
	LCD_SCK(1);
	LCD_CS(0);	
	

	SendCmdHead(0x06);

	for (i=0;i<6;i++)
	{
		LCD_SCK(0);
		LCD_DATA((addr&0x20)>0);
		addr <<= 1;
		Lcd_delay();
		LCD_SCK(1);
		Lcd_delay();
	}

	LCD_DATA_IN();

	for (j=0;j<cnt;j++)
	{
		for (i=0;i<4;i++)
		{
			LCD_RD(0);
			Lcd_delay();
			LCD_RD(1);
			Lcd_delay();
			rdBuf[j] |= (LCD_RD_PIN <<  i );
		}
	}

	LCD_CS(1);
	LCD_SCK(1);

	LCD_DATA_OUT();
}

//void LcdWriteCmd(u8 type, u16 data, u8 memAddr)
void LcdWriteCmd(u16 cmd)
{
	u8 i = 0;
	//u8 tmp = 0;
	LCD_DATA_OUT();                       // 2017/08/02
	
	LCD_SCK(1);
	LCD_CS(0);
	
	SendCmdHead(0x04);
	for (i=0;i<9;i++)
	{
		LCD_SCK(0);
		LCD_DATA((cmd&0x100)>0);
		cmd <<= 1;
		Lcd_delay();
		LCD_SCK(1);
		Lcd_delay();
	}

	LCD_CS(1);
	LCD_SCK(1);
}

void LcdWriteData(u8 memAddr, u8 *data, u8 len)
{
	u8 i = 0, j = 0;
	u8 tmp = 0;

	LCD_DATA_OUT();                       // 2017/08/02
	LCD_SCK(1);
	LCD_CS(0);

	SendCmdHead(0x05);
	for (i=0;i<6;i++)
	{
	
		LCD_SCK(0);
		LCD_DATA((memAddr&0x20)>0);
		memAddr <<= 1;
		Lcd_delay();
		LCD_SCK(1);
		Lcd_delay();
	}

	for (j=0;j<len;j++)
	{
		tmp = data[j];
		for (i=0;i<8;i++)
		{
			LCD_SCK(0);
			LCD_DATA(tmp&0x01);
			tmp >>= 1;
			Lcd_delay();
			LCD_SCK(1);
			Lcd_delay();
		}
	}

	LCD_CS(1);
	LCD_SCK(1);
}

void memset(u8 * src, u8 dat, u8 len)
{
	u8 i = 0;
	for (i=0;i<len;i++)
	{
		src[i] = dat;
	}
}

void memcpy(u8 *dest, u8 *src, u8 len)
{
	u8 i = 0;
	for (i=0;i<len;i++)
	{
		dest[i] = src[i];
	}
}


void LcdInit(void)
{
	//u8 rdBuf[16] = {0};
#if 1	
	delay_ms(1);
	LcdPortInit();

	LcdWriteCmd(LCDOFF);

	/*
	LcdWriteCmd(0x02); 
	LcdWriteCmd(0x06);             // lcd on
	LcdWriteCmd(0x52);
	LcdWriteData(0x00,fb,16);	*/

	LcdWriteCmd(BIAS);
	Lcd_delay();
	LcdWriteCmd(RC256);
	Lcd_delay();
	//LcdWriteCmd(SYSDIS);
	//Lcd_delay();
	LcdWriteCmd(WDTDIS);
	Lcd_delay();
	LcdWriteCmd(SYSEN);
	Lcd_delay();
	LcdWriteCmd(LCDON);
	Lcd_delay();



	SetTtlScr();
	//LcdWriteData(0x00,fb,16);
	//Lcd_delay();
	//memset(fb,0xff,16);

	//delay_ms(10);

	LcdWriteData(0x00,fb,16);

	//delay_ms(10);

	//LcdRead(0x00,rdBuf,16);
#endif	

	
}

void BeeperOn(u8 cnt, u16 tms)          // 50 ms
{
	LcdWriteCmd(TONE_4K);	
	LcdWriteCmd(TONEON);	
	g_beepCnt = cnt;
	g_beepTms = tms;
	g_beepEn = 1;
}

void BeeperOff(void)
{
	LcdWriteCmd(TONEOFF);
}





