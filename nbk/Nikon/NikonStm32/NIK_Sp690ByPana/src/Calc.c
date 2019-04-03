#include "sr101ef51a.h"	
#include "mn101ef52a.h"
#include "uart.h"
#include "nikon.h"
#include "output.h"
#include "init.h"

extern EXPOSE_PARA xdata flashPara;
extern CAMER_SETTING xdata cameraSetting;
extern unsigned char xdata ttl_pre2_flag;
extern UART_SEND xdata uartSend;
extern MultiSet xdata multiSet;
extern FLASH_STATUE xdata flashStatue;
extern UartSendFlag uartSendFlag;
extern NiWireless xdata nikWireless;
extern Wl_set xdata wlSet;

void WLS_ExposeTimeCal(const unsigned char i_camCode,unsigned char hp)
{
	unsigned char camTmp;
	unsigned char camCode=0;
	if(LOW_SPEED==hp)
	{
		//if(WLS_TTL_TP0<camCode)
		camCode = i_camCode-6;
		
		if(camCode < WLS_TTL_TP1)
		{
			flashPara.TTL_LEVEL1 = -1;
			flashPara.TTL_LEVEL2 = 2;
		}else if((WLS_TTL_TP1<=camCode)&&(camCode<WLS_TTL_TP2))
		{
			flashPara.TTL_LEVEL1 = 0;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP2<=camCode)&&(camCode<WLS_TTL_TP3))
		{
			flashPara.TTL_LEVEL1 = 0;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP3<=camCode)&&(camCode<WLS_TTL_TP4))
		{
			flashPara.TTL_LEVEL1 = 0;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP4<=camCode)&&(camCode<WLS_TTL_TP5))
		{
			flashPara.TTL_LEVEL1 = 1;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP5<=camCode)&&(camCode<WLS_TTL_TP6))
		{
			flashPara.TTL_LEVEL1 = 1;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP6<=camCode)&&(camCode<WLS_TTL_TP7))
		{
			flashPara.TTL_LEVEL1 = 1;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP7<=camCode)&&(camCode<WLS_TTL_TP8))
		{
			flashPara.TTL_LEVEL1 = 2;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP8<=camCode)&&(camCode<WLS_TTL_TP9))
		{
			flashPara.TTL_LEVEL1 = 2;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP9<=camCode)&&(camCode<WLS_TTL_TP10))
		{
			flashPara.TTL_LEVEL1 = 2;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP10<=camCode)&&(camCode<WLS_TTL_TP11))
		{
			flashPara.TTL_LEVEL1 = 3;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP11<=camCode)&&(camCode<WLS_TTL_TP12))
		{
			flashPara.TTL_LEVEL1 = 3;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP12<=camCode)&&(camCode<WLS_TTL_TP13))
		{
			flashPara.TTL_LEVEL1 = 3;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP13<=camCode)&&(camCode<WLS_TTL_TP14))
		{
			flashPara.TTL_LEVEL1 = 4;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP14<=camCode)&&(camCode<WLS_TTL_TP15))
		{
			flashPara.TTL_LEVEL1 = 4;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP15<=camCode)&&(camCode<WLS_TTL_TP16))
		{
			flashPara.TTL_LEVEL1 = 4;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP16<=camCode)&&(camCode<WLS_TTL_TP17))
		{
			flashPara.TTL_LEVEL1 = 5;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP17<=camCode)&&(camCode<WLS_TTL_TP18))
		{
			flashPara.TTL_LEVEL1 = 5;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP18<=camCode)&&(camCode<WLS_TTL_TP19))
		{
			flashPara.TTL_LEVEL1 = 5;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP19<=camCode)&&(camCode<WLS_TTL_TP20))
		{
			flashPara.TTL_LEVEL1 = 6;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP20<=camCode)&&(camCode<WLS_TTL_TP21))
		{
			flashPara.TTL_LEVEL1 = 6;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP21<=camCode)&&(camCode<WLS_TTL_TP22))
		{
			flashPara.TTL_LEVEL1 = 6;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP22<=camCode)&&(camCode<WLS_TTL_TP23))
		{
			flashPara.TTL_LEVEL1 = 7;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP23<=camCode)&&(camCode<WLS_TTL_TP24))
		{
			flashPara.TTL_LEVEL1 = 7;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((WLS_TTL_TP24<=camCode)&&(camCode<WLS_TTL_TP25))
		{
			flashPara.TTL_LEVEL1 = 7;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((WLS_TTL_TP25<=camCode)&&(camCode<WLS_TTL_TP26))
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP26<=camCode)&&(camCode<WLS_TTL_TP27))
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP27<=camCode)&&(camCode<WLS_TTL_TP28))
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP28<=camCode)&&(camCode<WLS_TTL_TP29))
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP29<=camCode)&&(camCode<WLS_TTL_TP30))
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((WLS_TTL_TP30<=camCode)&&(camCode<WLS_TTL_TP31))
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if(camCode>=WLS_TTL_TP31)
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
	}else
	{
	/*
		if(HIGH_SPEED_LEVEL2==hp)					//the shutter is less than 1/500(included)
		{
			if(camCode<0xaa)
			{
				camTmp = 0xff;
			}else
			{
				camTmp = camCode;
			}
		}else
		{
			if(camCode>0xbf)
			{
				camTmp = (camCode&0x7f);
			}else
			{
				camTmp = camCode;
			}
		}
	*/
		camCode = i_camCode+18;
	
		if(camCode>0xbf)
		{
			camTmp = (camCode&0x7f);
		}else
		{
			camTmp = camCode;
		}
		
		if(camTmp<WLS_HPTTL_TP11)
		{
			flashPara.hp_ttl1= 1;
			flashPara.hp_ttl2= 0;
		}
		else if((WLS_HPTTL_TP11<=camTmp)&&(camTmp<WLS_HPTTL_TP12))
		{
			flashPara.hp_ttl1= 1;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP12<=camTmp)&&(camTmp<WLS_HPTTL_TP13))
		{
			flashPara.hp_ttl1= 1;
			flashPara.hp_ttl2= 2;
		}
		else if((WLS_HPTTL_TP13<=camTmp)&&(camTmp<WLS_HPTTL_TP14))
		{
			flashPara.hp_ttl1= 2;
			flashPara.hp_ttl2= 0;
		}
		else if((WLS_HPTTL_TP14<=camTmp)&&(camTmp<WLS_HPTTL_TP15))
		{
			flashPara.hp_ttl1= 2;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP15<=camTmp)&&(camTmp<WLS_HPTTL_TP16))
		{
			flashPara.hp_ttl1= 2;
			flashPara.hp_ttl2= 2;
		}
		else if((WLS_HPTTL_TP16<=camTmp)&&(camTmp<WLS_HPTTL_TP17))
		{
			flashPara.hp_ttl1= 3;
			flashPara.hp_ttl2= 0;
		}
		else if((WLS_HPTTL_TP17<=camTmp)&&(camTmp<WLS_HPTTL_TP18))
		{
			flashPara.hp_ttl1= 3;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP18<=camTmp)&&(camTmp<WLS_HPTTL_TP19))
		{
			flashPara.hp_ttl1= 3;
			flashPara.hp_ttl2= 2;
		}
		else if((WLS_HPTTL_TP19<=camTmp)&&(camTmp<WLS_HPTTL_TP20))
		{
			flashPara.hp_ttl1= 4;
			flashPara.hp_ttl2= 0;
		}
		else if((WLS_HPTTL_TP20<=camTmp)&&(camTmp<WLS_HPTTL_TP21))
		{
			flashPara.hp_ttl1= 4;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP21<=camTmp)&&(camTmp<WLS_HPTTL_TP22))
		{
			flashPara.hp_ttl1= 4;
			flashPara.hp_ttl2= 2;
		}
		else if((WLS_HPTTL_TP22<=camTmp)&&(camTmp<WLS_HPTTL_TP23))
		{
			flashPara.hp_ttl1= 5;
			flashPara.hp_ttl2= 0;
		}
		else if((WLS_HPTTL_TP23<=camTmp)&&(camTmp<WLS_HPTTL_TP24))
		{
			flashPara.hp_ttl1= 5;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP24<=camTmp)&&(camTmp<WLS_HPTTL_TP25))
		{
			flashPara.hp_ttl1= 5;
			flashPara.hp_ttl2= 2;
		}
		else if((WLS_HPTTL_TP25<=camTmp)&&(camTmp<WLS_HPTTL_TP26))
		{
			flashPara.hp_ttl1= 6;
			flashPara.hp_ttl2= 0;
		}else if((WLS_HPTTL_TP26<=camTmp)&&(camTmp<WLS_HPTTL_TP27))
		{
			flashPara.hp_ttl1= 6;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP27<=camTmp)&&(camTmp<WLS_HPTTL_TP28))
		{
			flashPara.hp_ttl1= 6;
			flashPara.hp_ttl2= 2;
		}
		else if((WLS_HPTTL_TP28<=camTmp)&&(camTmp<WLS_HPTTL_TP29))
		{
			flashPara.hp_ttl1= 7;
			flashPara.hp_ttl2= 0;
		}
		else if((WLS_HPTTL_TP29<=camTmp)&&(camTmp<WLS_HPTTL_TP30))
		{
			flashPara.hp_ttl1= 7;
			flashPara.hp_ttl2= 1;
		}
		else if((WLS_HPTTL_TP30<=camTmp)&&(camTmp<WLS_HPTTL_TP31))
		{
			flashPara.hp_ttl1= 7;
			flashPara.hp_ttl2= 2;
		}
		else if(camTmp>=WLS_HPTTL_TP31)
		{
			flashPara.hp_ttl1= 8;
			flashPara.hp_ttl2= 0;
		}
	}

	flashPara.ExposeState = 1;	
}

void ExposeTimeCal(const unsigned char i_camCode,unsigned char hp)
{
	unsigned char camCode;
	//camCode = i_camCode - 8;
	if(LOW_SPEED==hp)
	{
		camCode = i_camCode - 8;
		
		if((TTL_TIMECAL_POINT0<=camCode)&&(camCode<TTL_TIMECAL_POINT1))
		{
			flashPara.TTL_LEVEL1 = -2;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT1<=camCode)&&(camCode<TTL_TIMECAL_POINT2))
		{
			flashPara.TTL_LEVEL1 = -2;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT2<=camCode)&&(camCode<TTL_TIMECAL_POINT3))
		{
			flashPara.TTL_LEVEL1 = -2;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT3<=camCode)&&(camCode<TTL_TIMECAL_POINT4))
		{
			flashPara.TTL_LEVEL1 = -1;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT4<=camCode)&&(camCode<TTL_TIMECAL_POINT5))
		{
			flashPara.TTL_LEVEL1 = -1;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT5<=camCode)&&(camCode<TTL_TIMECAL_POINT6))
		{
			flashPara.TTL_LEVEL1 = -1;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT6<=camCode)&&(camCode<TTL_TIMECAL_POINT7))
		{
			flashPara.TTL_LEVEL1 = 0;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT7<=camCode)&&(camCode<TTL_TIMECAL_POINT8))
		{
			flashPara.TTL_LEVEL1 = 0;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT8<=camCode)&&(camCode<TTL_TIMECAL_POINT9))
		{
			flashPara.TTL_LEVEL1 = 0;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT9<=camCode)&&(camCode<TTL_TIMECAL_POINT10))
		{
			flashPara.TTL_LEVEL1 = 1;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT10<=camCode)&&(camCode<TTL_TIMECAL_POINT11))
		{
			flashPara.TTL_LEVEL1 = 1;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT11<=camCode)&&(camCode<TTL_TIMECAL_POINT12))
		{
			flashPara.TTL_LEVEL1 = 1;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT12<=camCode)&&(camCode<TTL_TIMECAL_POINT13))
		{
			flashPara.TTL_LEVEL1 = 2;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT13<=camCode)&&(camCode<TTL_TIMECAL_POINT14))
		{
			flashPara.TTL_LEVEL1 = 2;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT14<=camCode)&&(camCode<TTL_TIMECAL_POINT15))
		{
			flashPara.TTL_LEVEL1 = 2;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT15<=camCode)&&(camCode<TTL_TIMECAL_POINT16))
		{
			flashPara.TTL_LEVEL1 = 3;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT16<=camCode)&&(camCode<TTL_TIMECAL_POINT17))
		{
			flashPara.TTL_LEVEL1 = 3;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT17<=camCode)&&(camCode<TTL_TIMECAL_POINT18))
		{
			flashPara.TTL_LEVEL1 = 3;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT18<=camCode)&&(camCode<TTL_TIMECAL_POINT19))
		{
			flashPara.TTL_LEVEL1 = 4;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT19<=camCode)&&(camCode<TTL_TIMECAL_POINT20))
		{
			flashPara.TTL_LEVEL1 = 4;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT20<=camCode)&&(camCode<TTL_TIMECAL_POINT21))
		{
			flashPara.TTL_LEVEL1 = 4;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT21<=camCode)&&(camCode<TTL_TIMECAL_POINT22))
		{
			flashPara.TTL_LEVEL1 = 5;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT22<=camCode)&&(camCode<TTL_TIMECAL_POINT23))
		{
			flashPara.TTL_LEVEL1 = 5;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT23<=camCode)&&(camCode<TTL_TIMECAL_POINT24))
		{
			flashPara.TTL_LEVEL1 = 5;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT24<=camCode)&&(camCode<TTL_TIMECAL_POINT25))
		{
			flashPara.TTL_LEVEL1 = 6;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT25<=camCode)&&(camCode<TTL_TIMECAL_POINT26))
		{
			flashPara.TTL_LEVEL1 = 6;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT26<=camCode)&&(camCode<TTL_TIMECAL_POINT27))
		{
			flashPara.TTL_LEVEL1 = 6;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if((TTL_TIMECAL_POINT27<=camCode)&&(camCode<TTL_TIMECAL_POINT28))
		{
			flashPara.TTL_LEVEL1 = 7;
			flashPara.TTL_LEVEL2 = 0;
		}
		else if((TTL_TIMECAL_POINT28<=camCode)&&(camCode<TTL_TIMECAL_POINT29))
		{
			flashPara.TTL_LEVEL1 = 7;
			flashPara.TTL_LEVEL2 = 1;
		}
		else if((TTL_TIMECAL_POINT29<=camCode)&&(camCode<TTL_TIMECAL_POINT30))
		{
			flashPara.TTL_LEVEL1 = 7;
			flashPara.TTL_LEVEL2 = 2;
		}
		else if(camCode>=TTL_TIMECAL_POINT30)
		{
			flashPara.TTL_LEVEL1 = 8;
			flashPara.TTL_LEVEL2 = 0;
		}
	}else
	{
		camCode = i_camCode+10;
		
		if(camCode<TTL_HPTIME_POINT8)
		{
			flashPara.hp_ttl1 = 1;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT8<=camCode)&&(camCode<TTL_HPTIME_POINT9))
		{
			flashPara.hp_ttl1 = 1;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT9<=camCode)&&(camCode<TTL_HPTIME_POINT10))
		{
			flashPara.hp_ttl1 = 1;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT10<=camCode)&&(camCode<TTL_HPTIME_POINT11))
		{
			flashPara.hp_ttl1 = 1;
			flashPara.hp_ttl2 = 2;
		}
		else if((TTL_HPTIME_POINT11<=camCode)&&(camCode<TTL_HPTIME_POINT12))
		{
			flashPara.hp_ttl1 = 2;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT12<=camCode)&&(camCode<TTL_HPTIME_POINT13))
		{
			flashPara.hp_ttl1 = 2;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT13<=camCode)&&(camCode<TTL_HPTIME_POINT14))
		{
			flashPara.hp_ttl1 = 2;
			flashPara.hp_ttl2 = 2;
		}
		else if((TTL_HPTIME_POINT14<=camCode)&&(camCode<TTL_HPTIME_POINT15))
		{
			flashPara.hp_ttl1 = 3;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT15<=camCode)&&(camCode<TTL_HPTIME_POINT16))
		{
			flashPara.hp_ttl1 = 3;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT16<=camCode)&&(camCode<TTL_HPTIME_POINT17))
		{
			flashPara.hp_ttl1 = 3;
			flashPara.hp_ttl2 = 2;
		}
		else if((TTL_HPTIME_POINT17<=camCode)&&(camCode<TTL_HPTIME_POINT18))
		{
			flashPara.hp_ttl1 = 4;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT18<=camCode)&&(camCode<TTL_HPTIME_POINT19))
		{
			flashPara.hp_ttl1 = 4;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT19<=camCode)&&(camCode<TTL_HPTIME_POINT20))
		{
			flashPara.hp_ttl1 = 4;
			flashPara.hp_ttl2 = 2;
		}
		else if((TTL_HPTIME_POINT20<=camCode)&&(camCode<TTL_HPTIME_POINT21))
		{
			flashPara.hp_ttl1 = 5;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT21<=camCode)&&(camCode<TTL_HPTIME_POINT22))
		{
			flashPara.hp_ttl1 = 5;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT22<=camCode)&&(camCode<TTL_HPTIME_POINT23))
		{
			flashPara.hp_ttl1 = 5;
			flashPara.hp_ttl2 = 2;
		}
		else if((TTL_HPTIME_POINT23<=camCode)&&(camCode<TTL_HPTIME_POINT24))
		{
			flashPara.hp_ttl1 = 6;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT24<=camCode)&&(camCode<TTL_HPTIME_POINT25))
		{
			flashPara.hp_ttl1 = 6;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT25<=camCode)&&(camCode<TTL_HPTIME_POINT26))
		{
			flashPara.hp_ttl1 = 6;
			flashPara.hp_ttl2 = 2;
		}
		else if((TTL_HPTIME_POINT26<=camCode)&&(camCode<TTL_HPTIME_POINT27))
		{
			flashPara.hp_ttl1 = 7;
			flashPara.hp_ttl2 = 0;
		}
		else if((TTL_HPTIME_POINT27<=camCode)&&(camCode<TTL_HPTIME_POINT28))
		{
			flashPara.hp_ttl1 = 7;
			flashPara.hp_ttl2 = 1;
		}
		else if((TTL_HPTIME_POINT26<=camCode)&&(camCode<TTL_HPTIME_POINT27))
		{
			flashPara.hp_ttl1 = 7;
			flashPara.hp_ttl2 = 2;
		}
		else if(camCode>TTL_HPTIME_POINT27)
		{
			flashPara.hp_ttl1 = 8;
			flashPara.hp_ttl2 = 0;
		}
		
	}

	flashPara.ExposeState = 1;		
}

#if(_HP_SYN_MODE_==HP_SYN_MODE_SW)
char HpExpCalc(char exp1,char exp2,char cmp1,char cmp2,unsigned int piso,HpSetting *hpSt,unsigned char ttlAccFlag)
{
	char xdata value1=0, value2=0;
	//unsigned int xdata TMR2=0;
	//HpSetting hpSt;

	value1 = exp1+cmp1;
	value2 = exp2+cmp2;

	if(piso)
	{
		value1 -= (piso / 3);
		value2 -= (piso % 3);
	}

	if(0x02==ttlAccFlag)
	{
		value1+=2;
	}

	if(value2>2)
	{
		value1 ++;
		value2 -= 3;
	}else if(value2<0)
	{
		value2 += 3;
		value1 --;
	}

	
	if(value1>=8)
	{
		value1 = 8;
		value2 = 0;
	}else if(value1<1)
	{
		value1 = 1;
		value2 = 0;
	}

	if(value1<4)
	{
		hpSt->accFlag = 1;
	}else
	{
		hpSt->accFlag = 0;
	}
	
	switch(value1)
	{
		case 0x01:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_10;
			hpSt->head = HP_HEAD_10;
			hpSt->val = HP_VAL_10;
			hpSt->step = HP_STEP_10;
			hpSt->adVal = HP_ADVAL_10;
			hpSt->coef = HP_COEF_10;
			hpSt->startVal = HP_SVAL_10;
			hpSt->realVal = HP_RVAL_10;
			hpSt->sdt = HP_SDT_10;
			hpSt->hDelay = HP_HDELAY_10;
			hpSt->initLev = HP_INITLEV_10;
			break;
			case 1:
			hpSt->dt=HP_EXP_11;
			hpSt->head = HP_HEAD_11;
			hpSt->val = HP_VAL_11;
			hpSt->step = HP_STEP_11;
			hpSt->adVal = HP_ADVAL_11;
			hpSt->coef = HP_COEF_11;
			hpSt->startVal = HP_SVAL_11;
			hpSt->realVal = HP_RVAL_11;
			hpSt->sdt = HP_SDT_11;
			hpSt->hDelay = HP_HDELAY_11;
			hpSt->initLev = HP_INITLEV_11;
			break;
			case 2:
			hpSt->dt=HP_EXP_12;
			hpSt->head = HP_HEAD_12;
			hpSt->val = HP_VAL_12;
			hpSt->step = HP_STEP_12;
			hpSt->adVal = HP_ADVAL_12;
			hpSt->coef = HP_COEF_12;
			hpSt->startVal = HP_SVAL_12;
			hpSt->realVal = HP_RVAL_12;
			hpSt->sdt = HP_SDT_12;
			hpSt->hDelay = HP_HDELAY_12;
			hpSt->initLev = HP_INITLEV_12;
			break;
		}
		break;

		case 0x02:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_20;
			hpSt->head = HP_HEAD_20;
			hpSt->val = HP_VAL_20;
			hpSt->step = HP_STEP_20;
			hpSt->adVal = HP_ADVAL_20;
			hpSt->coef = HP_COEF_20;
			hpSt->startVal = HP_SVAL_20;
			hpSt->realVal = HP_RVAL_20;
			hpSt->sdt = HP_SDT_20;
			hpSt->hDelay = HP_HDELAY_20;
			hpSt->initLev = HP_INITLEV_20;
			break;
			case 1:
			hpSt->dt=HP_EXP_21;
			hpSt->head = HP_HEAD_21;
			hpSt->val = HP_VAL_21;
			hpSt->step = HP_STEP_21;
			hpSt->adVal = HP_ADVAL_21;
			hpSt->coef = HP_COEF_21;
			hpSt->startVal = HP_SVAL_21;
			hpSt->realVal = HP_RVAL_21;
			hpSt->sdt = HP_SDT_21;
			hpSt->hDelay = HP_HDELAY_21;
			hpSt->initLev = HP_INITLEV_21;
			break;
			case 2:
			hpSt->dt=HP_EXP_22;
			hpSt->head = HP_HEAD_22;
			hpSt->val = HP_VAL_22;
			hpSt->step = HP_STEP_22;
			hpSt->adVal = HP_ADVAL_22;
			hpSt->coef = HP_COEF_22;
			hpSt->startVal = HP_SVAL_22;
			hpSt->realVal = HP_RVAL_22;
			hpSt->sdt = HP_SDT_22;
			hpSt->hDelay = HP_HDELAY_22;
			hpSt->initLev = HP_INITLEV_22;
			break;
		}
		break;

		case 0x03:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_30;
			hpSt->head = HP_HEAD_30;
			hpSt->val = HP_VAL_30;
			hpSt->step = HP_STEP_30;
			hpSt->adVal = HP_ADVAL_30;
			hpSt->coef = HP_COEF_30;
			hpSt->startVal = HP_SVAL_30;
			hpSt->realVal = HP_RVAL_30;
			hpSt->sdt = HP_SDT_30;
			hpSt->hDelay = HP_HDELAY_30;
			hpSt->initLev = HP_INITLEV_30;
			break;
			case 1:
			hpSt->dt=HP_EXP_31;
			hpSt->head = HP_HEAD_31;
			hpSt->val = HP_VAL_31;
			hpSt->step = HP_STEP_31;
			hpSt->adVal = HP_ADVAL_31;
			hpSt->coef = HP_COEF_31;
			hpSt->startVal = HP_SVAL_31;
			hpSt->realVal = HP_RVAL_31;
			hpSt->sdt = HP_SDT_31;
			hpSt->hDelay = HP_HDELAY_31;
			hpSt->initLev = HP_INITLEV_31;
			break;
			case 2:
			hpSt->dt=HP_EXP_32;
			hpSt->head = HP_HEAD_32;
			hpSt->val = HP_VAL_32;
			hpSt->step = HP_STEP_32;
			hpSt->adVal = HP_ADVAL_32;
			hpSt->coef = HP_COEF_32;
			hpSt->startVal = HP_SVAL_32;
			hpSt->realVal = HP_RVAL_32;
			hpSt->sdt = HP_SDT_32;
			hpSt->hDelay = HP_HDELAY_32;
			hpSt->initLev = HP_INITLEV_32;
			break;
		}
		break;

		case 0x04:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_40;
			hpSt->head = HP_HEAD_40;
			hpSt->val = HP_VAL_40;
			hpSt->step = HP_STEP_40;
			hpSt->adVal = HP_ADVAL_40;
			hpSt->coef = HP_COEF_40;
			hpSt->startVal = HP_SVAL_40;
			hpSt->realVal = HP_RVAL_40;
			hpSt->sdt = HP_SDT_40;
			hpSt->hDelay = HP_HDELAY_40;
			hpSt->initLev = HP_INITLEV_40;
			break;
			case 1:
			hpSt->dt=HP_EXP_41;
			hpSt->head = HP_HEAD_41;
			hpSt->val = HP_VAL_41;
			hpSt->step = HP_STEP_41;
			hpSt->adVal = HP_ADVAL_41;
			hpSt->coef = HP_COEF_41;
			hpSt->startVal = HP_SVAL_41;
			hpSt->realVal = HP_RVAL_41;
			hpSt->sdt = HP_SDT_41;
			hpSt->hDelay = HP_HDELAY_41;
			hpSt->initLev = HP_INITLEV_41;
			break;
			case 2:
			hpSt->dt=HP_EXP_42;
			hpSt->head = HP_HEAD_42;
			hpSt->val = HP_VAL_42;
			hpSt->step = HP_STEP_42;
			hpSt->adVal = HP_ADVAL_42;
			hpSt->coef = HP_COEF_42;
			hpSt->startVal = HP_SVAL_42;
			hpSt->realVal = HP_RVAL_42;
			hpSt->sdt = HP_SDT_42;
			hpSt->hDelay = HP_HDELAY_42;
			hpSt->initLev = HP_INITLEV_42;
			break;
		}
		break;

		case 0x05:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_50;
			hpSt->head = HP_HEAD_50;
			hpSt->val = HP_VAL_50;
			hpSt->step = HP_STEP_50;
			hpSt->adVal = HP_ADVAL_50;
			hpSt->coef = HP_COEF_50;
			hpSt->startVal = HP_SVAL_50;
			hpSt->realVal = HP_RVAL_50;
			hpSt->sdt = HP_SDT_50;
			hpSt->hDelay = HP_HDELAY_50;
			hpSt->initLev = HP_INITLEV_50;
			break;
			case 1:
			hpSt->dt=HP_EXP_51;
			hpSt->head = HP_HEAD_51;
			hpSt->val = HP_VAL_51;
			hpSt->step = HP_STEP_51;
			hpSt->adVal = HP_ADVAL_51;
			hpSt->coef = HP_COEF_51;
			hpSt->startVal = HP_SVAL_51;
			hpSt->realVal = HP_RVAL_51;
			hpSt->sdt = HP_SDT_51;
			hpSt->hDelay = HP_HDELAY_51;
			hpSt->initLev = HP_INITLEV_51;
			break;
			case 2:
			hpSt->dt=HP_EXP_52;
			hpSt->head = HP_HEAD_52;
			hpSt->val = HP_VAL_52;
			hpSt->step = HP_STEP_52;
			hpSt->adVal = HP_ADVAL_52;
			hpSt->coef = HP_COEF_52;
			hpSt->startVal = HP_SVAL_52;
			hpSt->realVal = HP_RVAL_52;
			hpSt->sdt = HP_SDT_52;
			hpSt->hDelay = HP_HDELAY_52;
			hpSt->initLev = HP_INITLEV_52;
			break;
		}
		break;

		case 0x06:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_60;
			hpSt->head = HP_HEAD_60;
			hpSt->val = HP_VAL_60;
			hpSt->step = HP_STEP_60;
			hpSt->adVal = HP_ADVAL_60;
			hpSt->coef = HP_COEF_60;
			hpSt->startVal = HP_SVAL_60;
			hpSt->realVal = HP_RVAL_60;
			hpSt->sdt = HP_SDT_60;
			hpSt->hDelay = HP_HDELAY_60;
			hpSt->initLev = HP_INITLEV_60;
			break;
			case 1:
			hpSt->dt=HP_EXP_61;
			hpSt->head = HP_HEAD_61;
			hpSt->val = HP_VAL_61;
			hpSt->step = HP_STEP_61;
			hpSt->adVal = HP_ADVAL_61;
			hpSt->coef = HP_COEF_61;
			hpSt->startVal = HP_SVAL_61;
			hpSt->realVal = HP_RVAL_61;
			hpSt->sdt = HP_SDT_61;
			hpSt->hDelay = HP_HDELAY_61;
			hpSt->initLev = HP_INITLEV_61;
			break;
			case 2:
			hpSt->dt=HP_EXP_62;
			hpSt->head = HP_HEAD_62;
			hpSt->val = HP_VAL_62;
			hpSt->step = HP_STEP_62;
			hpSt->adVal = HP_ADVAL_62;
			hpSt->coef = HP_COEF_62;
			hpSt->startVal = HP_SVAL_62;
			hpSt->realVal = HP_RVAL_62;
			hpSt->sdt = HP_SDT_62;
			hpSt->hDelay = HP_HDELAY_62;
			hpSt->initLev = HP_INITLEV_62;
			break;
		}
		break;

		case 0x07:
		switch(value2)
		{
			case 0:
			hpSt->dt=HP_EXP_70;
			hpSt->head = HP_HEAD_70;
			hpSt->val = HP_VAL_70;
			hpSt->step = HP_STEP_70;
			hpSt->adVal = HP_ADVAL_70;
			hpSt->coef = HP_COEF_70;
			hpSt->startVal = HP_SVAL_70;
			hpSt->realVal = HP_RVAL_70;
			hpSt->sdt = HP_SDT_70;
			hpSt->hDelay = HP_HDELAY_70;
			hpSt->initLev = HP_INITLEV_70;
			break;
			case 1:
			hpSt->dt=HP_EXP_71;
			hpSt->head = HP_HEAD_71;
			hpSt->val = HP_VAL_71;
			hpSt->step = HP_STEP_71;
			hpSt->adVal = HP_ADVAL_71;
			hpSt->coef = HP_COEF_71;
			hpSt->startVal = HP_SVAL_71;
			hpSt->realVal = HP_RVAL_71;
			hpSt->sdt = HP_SDT_71;
			hpSt->hDelay = HP_HDELAY_71;
			hpSt->initLev = HP_INITLEV_71;
			break;
			case 2:
			hpSt->dt=HP_EXP_72;
			hpSt->head = HP_HEAD_72;
			hpSt->val = HP_VAL_72;
			hpSt->step = HP_STEP_72;
			hpSt->adVal = HP_ADVAL_72;
			hpSt->coef = HP_COEF_72;
			hpSt->startVal = HP_SVAL_72;
			hpSt->realVal = HP_RVAL_72;
			hpSt->sdt = HP_SDT_72;
			hpSt->hDelay = HP_HDELAY_72;
			hpSt->initLev = HP_INITLEV_72;
			break;								          
		}
		break;

		case 0x08:
		hpSt->dt=HP_EXP_80;	
		hpSt->head = HP_HEAD_80;
		hpSt->val = HP_VAL_80;
		hpSt->step = HP_STEP_80;
		hpSt->adVal = HP_ADVAL_80;
		hpSt->coef = HP_COEF_80;
		hpSt->startVal = HP_SVAL_80;
		hpSt->realVal = HP_RVAL_80;
		hpSt->sdt = HP_SDT_80;
		hpSt->hDelay = HP_HDELAY_80;
		hpSt->initLev = HP_INITLEV_80;
#ifdef TTL_DEBUG
#ifdef TTL_DEBUG_FULLTIME

		SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_FULLTIMEDEBUG);
		flashStatue.full_flash++;
#endif
#endif
		if(flashStatue.full_flash++>25)
		{
			uartSend.tempAlarm = 0x01;
			//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_TEMPALARM);
			uartSendFlag.tempCheck = 1;
			flashStatue.flash_enable |= FLASH_STATE_TEMPALARM;
		}
		break;

		default:
		hpSt->dt=HP_EXP_40;
		hpSt->head = HP_HEAD_40;
		hpSt->val = HP_VAL_40;
		hpSt->step = HP_STEP_40;
		hpSt->adVal = HP_ADVAL_40;
		hpSt->coef = HP_COEF_40;
		hpSt->startVal = HP_SVAL_40;
		hpSt->realVal = HP_RVAL_40;
		hpSt->sdt = HP_SDT_40;
		hpSt->hDelay = HP_HDELAY_40;
		hpSt->initLev = HP_INITLEV_40;
		break;

	}

	return 1;
}
#elif(_HP_SYN_MODE_==HP_SYN_MODE_HW)
char HpExpCalc(char exp1,char exp2,char cmp1,char cmp2,unsigned int piso,HpSetting *hpSt,unsigned char ttlAccFlag)
{
	char xdata value1, value2;
	//unsigned int xdata TMR2=0;
	//HpSetting hpSt;

	value1 = exp1+cmp1;
	value2 = exp2+cmp2;

	if(0x02==ttlAccFlag)
	{
		value1+=2;
	}

	if(value2>2)
	{
		value1 ++;
		value2 -= 3;
	}
	if(value1>=8)
	{
		value1 = 8;
		value2 = 0;
	}else if(value1<1)
	{
		value1 = 1;
		value2 = 0;
	}

	switch(value1)
	{
		case 0x01:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_10;
			break;
			
			case 1:
				*hpSt = HP_EXP_11;
			break;
			
			case 2:
				*hpSt = HP_EXP_12;
			break;
		}
		break;

		case 0x02:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_20;
			break;
			
			case 1:
				*hpSt = HP_EXP_21;
			break;
			
			case 2:
				*hpSt = HP_EXP_22;
			break;
		}
		break;

		case 0x03:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_30;
			break;
			case 1:
				*hpSt = HP_EXP_31;
			break;
			case 2:
				*hpSt = HP_EXP_32;
			break;
		}
		break;

		case 0x04:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_40;
			break;
			case 1:
				*hpSt = HP_EXP_41;
			break;
			case 2:
				*hpSt = HP_EXP_42;
			break;
		}
		break;

		case 0x05:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_50;
			break;
			case 1:
				*hpSt = HP_EXP_51;
			break;
			case 2:
				*hpSt = HP_EXP_52;
			break;
		}
		break;

		case 0x06:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_60;
			break;
			case 1:
				*hpSt = HP_EXP_61;
			break;
			case 2:
				*hpSt = HP_EXP_62;
			break;
		}
		break;

		case 0x07:
		switch(value2)
		{
			case 0:
				*hpSt = HP_EXP_70;
			break;
			case 1:
				*hpSt = HP_EXP_71;
			break;
				
			case 2:
				*hpSt = HP_EXP_72;
			break;								          
		}
		break;

		case 0x08:
				*hpSt = HP_EXP_80;
		break;

		default:
				*hpSt = HP_EXP_50;
		break;
		}

		//HpExpCodeSend(((*hpSt)>>8)&0xff,(*hpSt&)0xff);
		HpExpCodeSend(*hpSt);
}
#endif


int OutputSwitch(unsigned char mode)
{
	char xdata value1, value2;
	char xdata surtmp1,surtmp2;
	unsigned int xdata TMR2=0;

	if(mode==WORKINGMODE_TTL)
	{

	/*	if((flashPara.ttlcomp1==0)	 && (flashPara.ttlcomp2==0))
		{
			value1 =  flashPara.TTL_LEVEL1 + flashPara.camera_comp1;
			value2 =  flashPara.TTL_LEVEL2 + flashPara.camera_comp2;
		}
		else
		{
			value1 = flashPara.TTL_LEVEL1+(flashPara.ttlcomp1);
			value2 = flashPara.TTL_LEVEL2+flashPara.ttlcomp2;
		}*/
		if(WORKINGMODE_TTL==flashPara.workmode)
		{
			value1 = flashPara.TTL_LEVEL1 + cameraSetting.camera_cmp1 + flashPara.ttlcomp1;
			value2 = flashPara.TTL_LEVEL2 + cameraSetting.camera_cmp2 + flashPara.ttlcomp2;

		    if((!flashPara.ttlFeb1)||(!flashPara.ttlFeb2))
			{
				surtmp1 = flashPara.ttlFeb1;
				surtmp2 = flashPara.ttlFeb2;

				 switch(flashPara.surrand_state)
				 {
				 	case 0:
					flashPara.surrand_state ++;
					break;
					case 1:
					value1 -= surtmp1;
					value2 -= surtmp2;
					flashPara.surrand_state ++;
					break;
					case 2:
					value1 += surtmp1;
					value2 += surtmp2;
					flashPara.surrand_state = 0; 
	//				flashPara.flash_surrand = 0;
					break;	 
				 }
			}
		}else if(WORKINGMODE_RTN==flashPara.workmode)
		{
			value1 = flashPara.TTL_LEVEL1;
			value2 = flashPara.TTL_LEVEL2;
		}else if(WORKINGMODE_MASTER==flashPara.workmode)
		{
			value1 = flashPara.TTL_LEVEL1+wlSet.m_arrPara[WL_ARRARY_M].cmp1;
			value2 = flashPara.TTL_LEVEL2+wlSet.m_arrPara[WL_ARRARY_M].cmp2;
		}
		
		if(ttl_pre2_flag==UART_ENCODE_DEBUG_EXPOSE2)
		{
			value1 += 2;
		}

		if(uartSend.ISO_index)
		{
			value1 -= (uartSend.ISO_index / 3);
			value2 -= (uartSend.ISO_index % 3);
		}
		
		
		if(value2<0) {
			value1--;
			value2 = 3 + value2;		
		}
		else if(value2>2)
		{
			value1++;
			value2 = value2 - 3;
		}
		if(value1<-2) 
		{
			value1 = -2;
			value2 = 0;
		}
		
		else if(value1>8)  value1 = 8;

		

	}else if(WORKINGMODE_MULTI==mode)
	{
		value1 = (multiSet.expose>>4);
		value2 = (multiSet.expose&0x0f);
		//putchar(multiSet.expose);
		
	}
	else if(WORKINGMODE_MANU==mode)
	{
		if(WORKINGMODE_RTN==flashPara.workmode)
		{
			value1 = nikWireless.WL_output1;
			value2 = nikWireless.WL_output2;
		}else if(WORKINGMODE_MASTER==flashPara.workmode)
		{
			value1 = wlSet.m_arrPara[WL_ARRARY_M].output1;
			value2 = wlSet.m_arrPara[WL_ARRARY_M].output2;
		}
		else
		{
	 		value1 = flashPara.output1;
			value2 = flashPara.output2;
		}
	}

//	printf("vp:%d,%d",main_parameter.VIP_output1,main_parameter.VIP_output2);

	switch(value1)
		{
		   	case -2:
			switch(value2)
			{
				case 0:
				TMR2=TTLTRA0;
				break;
				case 1:
				TMR2=TTLTRA1;
				break;
				case 2:
				TMR2=TTLTRA2;
				break;
			}
			case -1:
			switch(value2)
			{
				case 0:
				TMR2=TTLTRA3;
				break;
				case 1:
				TMR2=TTLTRA4;
				break;
				case 2:
				TMR2=TTLTRA5;
				break;
			}
			break;

			case 0x00:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_00;
				break;
				case 1:
				TMR2=OUTPUT_01;
				break;
				case 2:
				TMR2=OUTPUT_02;
				break;
			}
			break;

			case 0x01:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_10;
				break;
				case 1:
				TMR2=OUTPUT_11;
				break;
				case 2:
				TMR2=OUTPUT_12;
				break;
			}
			break;

			case 0x02:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_20;
				break;
				case 1:
				TMR2=OUTPUT_21;
				break;
				case 2:
				TMR2=OUTPUT_22;
				break;
			}
			break;

			case 0x03:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_30;
				break;
				case 1:
				TMR2=OUTPUT_31;
				break;
				case 2:
				TMR2=OUTPUT_32;
				break;
			}
			break;

			case 0x04:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_40;
				break;
				case 1:
				TMR2=OUTPUT_41;
				break;
				case 2:
				TMR2=OUTPUT_42;
				break;
			}
			break;

			case 0x05:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_50;
				break;
				case 1:
				TMR2=OUTPUT_51;
				break;
				case 2:
				TMR2=OUTPUT_52;
				break;
			}
			break;

			case 0x06:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_60;
				break;
				case 1:
				TMR2=OUTPUT_61;
				break;
				case 2:
				TMR2=OUTPUT_62;
				break;
			}
			break;

			case 0x07:
			switch(value2)
			{
				case 0:
				TMR2=OUTPUT_70;
				break;
				case 1:
				TMR2=OUTPUT_71;
				break;
				case 2:
				TMR2=OUTPUT_72;
				break;								          
			}
			break;

			case 0x08:
			TMR2=OUTPUT_80;	
#ifdef TTL_DEBUG
#ifdef TTL_DEBUG_FULLTIME

			SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_FULLTIMEDEBUG);
			flashStatue.full_flash++;
#endif
#endif
			if(flashStatue.full_flash++>25)
			{
				uartSend.tempAlarm = 0x01;
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_TEMPALARM);
				uartSendFlag.tempCheck = 1;
				flashStatue.flash_enable |= FLASH_STATE_TEMPALARM;
			}
			break;

			default:
			TMR2=OUTPUT_40;
			break;

	}
			return TMR2;
}




