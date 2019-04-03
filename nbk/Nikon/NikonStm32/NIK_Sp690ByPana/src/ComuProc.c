//#include "stc56ad.h"
//#include<intrins.h>
#include "sr101ef51a.h"	
#include "mn101ef52a.h"
#include "uart.h"
#include "nikon.h"
#include "output.h"
#include "init.h"

#define VERI_DATA_LEN 21
#define NIKONNSS_SELF_HANDLE
unsigned char xdata ttl_debugData;
extern unsigned char xdata ttl_pre2_flag;
unsigned char filter;
extern MultiSet xdata multiSet;
unsigned char xdata info_flag=0;
extern unsigned char xdata bitFlag;
extern UartSendFlag uartSendFlag;
extern unsigned char charge_ok;
extern unsigned char xdata tpfRsv;
unsigned char flaLockFlag;

#define HALT_INTERVAL 20
#define LITTLE_END

#define PRE_FLASH_INTERVAL 12

#define WORKINGMODE_WLM WORKINGMODE_MASTER

extern NiWireless xdata nikWireless;

unsigned char WrongCodeFilter=0;

unsigned int code shortShutter[39] = 
{
/*	5,6,8,10,12,16,20,25,33,40,50,66,76,100,125,160,200,250,333,400,500,625,769,1000,1300,
	1600,2000,2500,3000,4000,5000,6000,8000,10000,13000,15000,20000,25000,30000,*/

	30000,25000,20000,15000,13000,10000,8000,6000,5000,4000,3000,2500,2000,1600,1300,1000,
	769,625,500,400,333,250,200,160,125,100,76,66,50,40,33,25,20,16,12,10,8,6,5
};

//shutter code


#ifndef NIKON_D50
/*
unsigned char xdata send_arrary1[] = {
0x80,0x80,0x40,0x80,0x10,0xe0,0x80,0x4c,0x69,0x26,0x30,0x72,
0xa5,0x34,0xc2,0x64,0xc2,0x98
};		//0x85*/
unsigned char xdata send_arrary1[] = {
//0x01,0x01,0x02,0x01,0x08,0x07,0x01,0x32,0x96,0x64,0x0c,0x4e,
//0xa5,0x2c,0x43,0x26,0x43,0x19
//0x01,0x01,0x05,0x02,0x08,0x07,0x81,0x78,0xB4,0x64,0x0A,0x4E,
//0xFF,0x3E,0x90,0x3E,0x90,0x1D 
0x01,0x01,0x02,0x01,0x08,0x07,0x01,0x32,0x96,0x64,0x0c,0x4e,
0xa5,0x2c,0x43,0x26,0x43,0x19
};	
#else
/*unsigned char xdata send_arrary1[] = {
0x80,0x80,0xa0,0x40,0x10,0xe0,0x81,0x1e,0x2d,0x26,0x50,0x72,
0xff,0x7c,0x09,0x7c,0x09,0xb8
};		//0x85	  */
unsigned char xdata send_arrary1[] = {
0x01,0x01,0x05,0x02,0x08,0x07,0x81,0x78,0xb4,0x64,0x0a,0x4e,
0xff,0x3e,0x90,0x3e,0x90,0x1d
};
#endif


#ifdef NIKON_D50
/*//big end
unsigned char xdata send_arrary2[] = {
//0xe0,0x80,0x5d,0x20,0x00,0x00,0x6e,0x2a,0x0c,0xc4,0x50,0x80,
//0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff
0xc0,0x40,0x6d,0x20,0x00,0x00,0x11,0x2a,0x0c,0xc4,0x00,0xff,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xb7,
};		//0x05*/
//little end
/*
unsigned char xdata send_arrary2[] = {
//0xe0,0x80,0x5d,0x20,0x00,0x00,0x6e,0x2a,0x0c,0xc4,0x50,0x80,
//0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff
0x03,0x02,0xb6,0x04,0x00,0x00,0x88,0x54,0x30,0x23,0x00,0xff,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xed,
};*/

unsigned char xdata send_arrary2[] = {
//0xe0,0x80,0x5d,0x20,0x00,0x00,0x6e,0x2a,0x0c,0xc4,0x50,0x80,
//0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff
//0x03,0x02,0xb6,0x04,0x00,0x00,0x88,0x54,0x30,0x23,0x00,0xff,
//0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xed,
0x01,0x02,0xb6,0x04,0x00,0x00,0x88,0x54,0x30,0x23,0x00,0xff,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xeb,
};
/*unsigned char xdata send_arrary2[] = {
//0xe0,0x80,0x5d,0x20,0x00,0x00,0x6e,0x2a,0x0c,0xc4,0x50,0x80,
//0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff
0x03,0x02,0xc0,0x04,0x00,0x00,0x92,0x5e,0x30,0x4b,0x00,0xff,
0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,
};*/
#else
//main paremeter arrary
unsigned char xdata send_arrary2[] = {
0xc0,0x80,0x5d,0x20,0x00,0x00,0xba,0x3a,0x0c,0x62,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4f,
};		//0x05
#endif

//the verifing code is the sum of the previous code


/*
unsigned char xdata send_arrary2[] = {
0xe0,0x60,0x5d,0x20,0x00,0x00,0x6e,0x2a,0x0c,0xc4,0x50,0x80,
0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20
};		//0x05
*/

#ifdef NIKON_D50
/*unsigned char xdata send_arrary3[] = {
0xc0,0xa0,0xb6,0xd9,0x09,0x61,0x01,0x5e,0x4e,0x56,0xa6,0x7a,
0x60,0xae,0x79,0x29,0x51,0x01,0xee,0xf6,0x66,0x06,0x1a,0x60,
0x2e,0x79,0xa9,0xd1,0x01,0x6e,0xf6,0x66,0x06,0x9a,0x20,0xfa,
0x69,0x51,0x41,0x01,0x7e,0x9e,0x8e,0x76,0x56,0x3f,
};*/
unsigned char xdata send_arrary3[] = {
//0x03,0x05,0x6d,0x9b,0x90,0x86,0x80,0x7a,0x72,0x6a,0x65,0x5e,
//0x06,0x75,0x9e,0x94,0x8a,0x80,0x77,0x6f,0x66,0x60,0x58,0x06,
//0x74,0x9e,0x95,0x8b,0x80,0x76,0x6f,0x66,0x60,0x59,0x04,0x5f,
//0x96,0x8a,0x82,0x80,0x7e,0x79,0x71,0x6e,0x6a,0xfc,
0x03,0x05,0x6d,0x9b,0x90,0x86,0x80,0x7a,0x72,0x6a,0x65,0x5e,
0x06,0x75,0x9e,0x94,0x8a,0x80,0x77,0x6f,0x66,0x60,0x58,0x06,
0x74,0x9e,0x95,0x8b,0x80,0x76,0x6f,0x66,0x60,0x59,0x04,0x5f,
0x96,0x8a,0x82,0x80,0x7e,0x79,0x71,0x6e,0x6a,0xfc,
};
#else
unsigned char xdata send_arrary3[] = {
0xc0,0xb0,0x5e,0x35,0x39,0xd1,0x01,0xee,0x36,0x06,0xea,0x72,
0x70,0xe1,0xd5,0xb9,0xb1,0x01,0x2e,0x96,0xfa,0x6a,0x32,0x30,
0x4e,0x39,0x29,0x91,0x01,0x1e,0x86,0x96,0x46,0xd9,0xd0,0xf6,
0x39,0x29,0xd1,0x01,0xee,0x8e,0xe6,0x46,0x3a,0x68,
};		//0x45

#endif

#ifdef LITTLE_END
unsigned char code ISO_CODE[] = {
0x1e,0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,
0x36,0x38,0x3a,0x3c,0x3e,0x40,0x42,
};
#else
unsigned char code ISO_CODE[] = {
0x78,0x04,0x44,0x24,0x64,0x14,0x54,0x34,0x74,0x0c,0x4c,0x2c,
0x6c,0x1c,0x5c,0x3c,0x7c,0x02,0x42,
};
#endif


unsigned int  code ISO_VALUE[] = {
100,125,160,200,250,320,400,500,640,800,1000,1250,1600,2000,
2500,3200,4000,5000,6400
};

#ifdef LITTLE_END
unsigned char code focLen[] ={
0x2d,0x30,0x32,0x31,0x33,0x34,0x36,0x37,0x38,0x39,0x3a,0x3c,0x3d,0x3e,0x40,0x42,0x41,0x43,0x44,0x45,0x46,
0x47,0x8f,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x50,0x52,0x51,0x53,0x55,0x56,
0x57,0x59,0x5a,0x5c,0x5e,0x60,0x62,0x64,0x65,0x66,0x67,0x6a,0x6c,0x6e,0x71,0x73,0x76,0x7a,0x80,0x4f
};
#else
unsigned char code focLen[] = {
0xb4,0x0c,0x4c,0x8c,0xcc,0x2c,0x6c,0xec,0x1c,0x9c,0x5c,0x3c,0xbc,0x7c,0x02,0x42,0x82,0xc2,0x22,0xa2,0x62,
//18,20,   21   21    22   23   24	25   25   26   27  28   29,	30,	   32,  33   33   34,  35,  36   38	
0xe2,0xf1,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0x0a,0x4a,0x8a,0xca,0xaa,0x6a,
//38  40  40   42   44   45   46   48   50   52   52   55   58   60
0xea,0x9a,0x5a,0x3a,0x7a,0x06,0x46,0x26,0xa6,0x66,0xe6,0x56,0x36,0x76,0x8e,0xce,0x6e,0x5e,0x01,0xf2
//62  65   66  70   75   80   85   90   92   95 	98	 105 112  120  130  135  150  170  180  48
};
#endif

unsigned char code focLenVal[] = {
18,20,21,21,22,23,24,25,25,26,27,28,29,30,32,33,33,34,35,36,38,
38,40,40,41,42,44,45,46,48,50,52,52,55,58,60,
62,65,66,70,75,80,85,90,92,95,98,105,112,120,130,135,150,170,180,49
};



/*unsigned char code f_code[] = {
0x50,0x30,0xf0,0x48,0xa8,0x18,0xd8,0x78,0x84,0x24,0x74,0x54,
0xb4,0x0c,0xcc,0x6c,
};*/
/*
unsigned char code aperture[]={
0x38,0x04,0x14,0x18,0x24,0x30,0x44,0x48,0x50,0x54,0x64,0xe4,0xe4,
//50,63,  100, 40,   80,  20,  71,  28,  18,  110, 90,  95, 140,
0x78,0x84,0x0c,0x1c,0x2c,0x3c,0x4c,0x5c,0x6c,0x7c,0x9c,0xa8,
//56, 67, 160, 250, 200, 320, 180, 290, 220, 380, 270,  33,
0x34,0xb8,0xbc,0xcc,0xd8,0xf0,0x68,0x58,0xb4,0x68,0x74
//130,53, 360, 190,  48,  24, 35,   45,  130,38,  140
};*/

unsigned int code apertVal[] = {
8,	10, 12, 14, 16,
18,	20,	22, 24, 25, 28,	32,
33,	35,	38,	40,	42,	45,	48,	50,	53,	56,
63,	67,	71,	80,	90,	95,	100,110,130,130,140,
160,180,190,200,220,250,270,290,320,360,380
};

#ifdef LITTLE_END
unsigned int code aperture[] = {
0x00,0x02,0x04,0x06,0x08,
0x0a,0x0c,0x0e,0x0f,0x10,0x12,0x14,
0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,
0x20,0x21,0x22,0x24,0x26,0x27,0x28,0x2a,0x2c,0x2d,0x2e,
0x30,0x32,0x33,0x34,0x36,0x38,0x39,0x3a,0x3c,0x3d,0x3e,
};
#else
unsigned int code aperture[] = {
0x00,0x40,0x20,0x60,0x10,
0x50,0x30,0x70,0xf0,0x08,0x48,0x28,
0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,
0x04,0x84,0x44,0x24,0x64,0xe4,0x14,0x54,0x34,0xb4,0x74,
0x0c,0x4c,0xcc,0x2c,0x6c,0x1c,0x9c,0x5c,0x3c,0xbc,0x7c,
};
#endif

/*unsigned int code apertVal[]={
50,63,100,40,80,20,71,28,18,110,90,95,140,
56,67,160,250,200,320,180,290,220,380,270,33,
130,53,360,190,48,24,35,45,130,38,140
};*/

unsigned char code clrSta[] = {
0x03,0x01,0xba,0x04,0x00,0x00,0x5d,0x5c,0x30,0x46,0,0,0,0,0,0,0,0,0,0,0,0xf2,
};



/*
unsigned char code onCameraCmp[] = {
0x00,0x40,0x50,0x60,0x10,0x50,0x30,0x
};*/

unsigned char xdata receive_arrary[MAX_RCV_LEN]={0};

extern EXPOSE_PARA xdata flashPara;
extern FLASH_STATUE xdata flashStatue;
extern UART_SEND xdata uartSend;                                                                                    
extern CAMER_SETTING xdata cameraSetting;

extern unsigned char xdata bitChar;
extern Wl_set xdata wlSet;


void GetInfo(unsigned char lead)
{
	char i;
	switch(lead)
	{
		case 0x8d:
		break;

		case 0x0d:
		if(uartSend.pApert!=receive_arrary[RCVDAT_LOCATION_APERTURE])
		{
			uartSend.pApert = receive_arrary[RCVDAT_LOCATION_APERTURE];
			for(i=0;i<sizeof(aperture);i++)
			{
				if(receive_arrary[RCVDAT_LOCATION_APERTURE]==aperture[i])
					break;
			}
			if(i<=sizeof(aperture))
			{
				uartSend.apert = apertVal[i];
				uartSendFlag.imfo_apert = 1;
				//SETBIT(uartSend.sendFlag, UARTFLAG_INDEX_APERT);				
			}
	/*		else
			{
				uartSend.apert = 0x2ff;
			}
			//uartSend.apert = receive_arrary[RCVDAT_LOCATION_APERTURE];
			SETBIT(uartSend.sendFlag, UARTFLAG_INDEX_APERT);
			*/
		}
		if(uartSend.pIso!=receive_arrary[RCVDAT_LOCATION_ISO])
		{
			uartSend.pIso = receive_arrary[RCVDAT_LOCATION_ISO];
			
			for(i=0;i<sizeof(ISO_CODE);i++)
			{
				if( receive_arrary[RCVDAT_LOCATION_ISO]==ISO_CODE[i])
				break;
			}
			if(i<=sizeof(ISO_CODE))
			{
				uartSend.ISO_index = i;
				uartSend.ISO = ISO_VALUE[i];
				uartSendFlag.imfo_iso = 1;
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_ISO);
			}
		}

		if(cameraSetting.pShutter!=(char)receive_arrary[RCVDAT_LOCATION_SHUTTER])
		//if(WORKINGMODE_MULTI==flashPara.workmode)
		{
			cameraSetting.pShutter =  (char)receive_arrary[RCVDAT_LOCATION_SHUTTER];
			//flashPara.fp = LOW_SPEED;
			//Tprintf("%x ",(int)cameraSetting.pShutter);
			if(0x2d==cameraSetting.pShutter)
			{
				cameraSetting.shutter = 5;
				//Tprintf("%d ",(int)cameraSetting.shutter);
			}else if(cameraSetting.pShutter==(char)0x80)		//B
			{
				cameraSetting.shutter = 60000;
			}
			else if((char)cameraSetting.pShutter<(char)0x2d)
			{
				//Tprintf("%x ",(int)(cameraSetting.pShutter + 0x1e)/2);
				cameraSetting.shutter = shortShutter[((int)(cameraSetting.pShutter + 30/*0x1e*/)/2)];
				//putchar(cameraSetting.shutter>>8);
				//putchar(cameraSetting.shutter&0xff);
				//Tprintf("%d ",(int)cameraSetting.shutter);
				/*
				if((int)(1000/multiSet.frequence*multiSet.eTimes)<cameraSetting.shutter)
				{
					multiSet.finalExpTimes = multiSet.eTimes;
				}else
				{
					if((((unsigned long)cameraSetting.shutter*(unsigned long)multiSet.frequence)/1000))
					{
						multiSet.finalExpTimes = (int)((((unsigned long)cameraSetting.shutter*(unsigned long)multiSet.frequence)/1000)+1);
					}else
					{
						multiSet.finalExpTimes = (int)((((unsigned long)cameraSetting.shutter*(unsigned long)multiSet.frequence))/1000);
					}
				}
				*/

				//Tprintf("%d ",multiSet.finalExpTimes);				
			}
/*
			else
			{
				//flashPara.fp = HIGH_SPEED;
				if(!(receive_arrary[RCVDAT_LOCATION_MODE]&0x30))
				{
					flashPara.fp = LOW_SPEED;
				}else
				{
					flashPara.fp = HIGH_SPEED;
				}
			}
*/
		}
		
		

		if(uartSend.pFocLen!=receive_arrary[RCVDAT_LOCATION_FOCLEN])
		{
			uartSend.pFocLen = receive_arrary[RCVDAT_LOCATION_FOCLEN];

			//uartSend.focLen = receive_arrary[RCVDAT_LOCATION_FOCLEN];
			for(i=0;i<sizeof(focLen);i++)
			{
				if(receive_arrary[RCVDAT_LOCATION_FOCLEN]==focLen[i])
					break;
			}

			if(i<=sizeof(focLen))
			{
				uartSend.focLen = focLenVal[i];
				uartSendFlag.imfo_focLen = 1;
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_FOCLEN);
				
			}
			/*
			if(i==sizeof(focLen))
			{
				uartSend.focLen = 0;
			}else{
				uartSend.focLen = focLenVal[i];
			}
			SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_FOCLEN);
			*/
		}

		if(uartSend.mode != receive_arrary[RCVDAT_LOCATION_MODE])
		{
			uartSend.mode = receive_arrary[RCVDAT_LOCATION_MODE];
			uartSendFlag.rearCurtain = 1;
			//if(!(uartSend.mode&0x30))
			//{
			//	flashPara.fp = LOW_SPEED;
			//}else if(cameraSetting.pShutter>0x2d)
			//{
			//	flashPara.fp = HIGH_SPEED;
			//}
			//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_REAR);
		/*	if(GETBIT(uartSend.mode,CAMERA_MODE_REAR))
			{
				SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_REAR);
			}else
			{
				SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_REAR);
			}*/
		}

		if(cameraSetting.rcvCode != receive_arrary[RCVDAT_LOCATION_CAMERACMP])
		{
			//cameraSetting.rcvCode= CodeRecombina(receive_arrary[RCVDAT_LOCATION_CAMERACMP]);
			cameraSetting.rcvCode = receive_arrary[RCVDAT_LOCATION_CAMERACMP];
			if(!cameraSetting.rcvCode)
			{
				cameraSetting.camera_cmp1 = 0;
				cameraSetting.camera_cmp2 = 0;
			}else if(!(cameraSetting.rcvCode&0x80))
			{
				cameraSetting.camera_cmp1 = -1 * ((cameraSetting.rcvCode / 2) / 3);
				cameraSetting.camera_cmp2 = -1 * ((cameraSetting.rcvCode / 2) % 3);
			}else{
				cameraSetting.rcvCode &= 0x1f; 
				cameraSetting.rcvCode = 0x20 - cameraSetting.rcvCode;
				cameraSetting.camera_cmp1 = ((cameraSetting.rcvCode / 2) / 3);
				cameraSetting.camera_cmp2 = ((cameraSetting.rcvCode / 2) % 3);
			}

	#ifdef TTL_DEBUG
			SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_CAMERACMP);
	#endif
			
		}

		if(!uartSend.cls)
		{
			uartSend.cls = 1;
			uartSendFlag.cls = 1;
			uartSendFlag.rearCurtain = 1;
			//uartSend.cls_invalid = 0xff;
			//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CLS);
		}

		//uartSend.cls = 1;
		uartSend.cls_invalid = 200;
		//uartSendFlag.cls = 1;
		//uartSendFlag.imfo_iso = 1;
		//uartSendFlag.imfo_apert = 1;
		//uartSendFlag.imfo_focLen = 1;
		//uartSendFlag.charge = 1;
		break;


	/*	case 0xcb:
			flashPara.ExposeData = receive_arrary[RCVDAT_LOCATION_EXPOSE];
			uartSend.debug = flashPara.ExposeData;
			SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_DEBUG);			
		break;
	*/

	}

	//for(i=0;i<MAX_RCV_LEN;i++)
	//{
		//receive_arrary[i]=0;
	//}
}

void ComuProc()
{
	unsigned char command;
	unsigned char i;
	unsigned char rcvData;
	unsigned char media;
	char exposeCovert;

	while(1)
	{
	//	Delay();
	//	puts("wait\n");	
	/*	if(WORKINGMODE_MULTI==flashPara.workmode)
		{
				continue;
		}*/

		if(READY_SIGNAL_TIMEOUT==READY_FOR_SIGNAL())
		{
			continue;
		}
	//	READY_FOR_SIGNAL();
#ifdef NIKCOM_TEST
		HaltInterval(50);
#endif
		//command = DATA_Receive(1);
		
		if(DATA_SEND_TIMEOUT==DATA_Receive(1,&command))
		{
			continue;
		}
		//while(!NIKONNSS);
		//if(command!=0xd4)
		//{
			//Delay();
			//HaltInterval(15);
			//while(!NIKONNSSI);
			//while(!NIKONNSSI);
			//while(NIKONNSSI);
			/*if((command)&&(MULTI_STFLAG_START==multiSet.startFlag))
			{
				MUL_PCATIMER_STOP
				multiSet.finalExpTimes=0;
				multiSet.startFlag = MULTI_STFLAG_HOLD;
				SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
				SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
				NIKONNSS = 1;
				InitUart();
			}*/
		//}

//		putchar(command);
//		Delay();
//		HaltInterval(20);
//		Tprintf("receive cmd\n");
//		Tprintf(" :%x ",(int)command);
//		Tprintf("CMD: %x	\n",(int)command);


/****************************************
	the following code is about to aviod waitting for
	the NIKONNSS permenantly, so ,set a timer,
	once it overflow, than coutinue for the next 
	command. But I have enbed the code in the 
	receive and send process. so delete here, but
	whether it works should be verified.
****************************************/
/*
#ifdef LITTLE_END
		if((command==0xff)||(command==0xf0))	
#else
		if((command==0xff)||(command==0xf))	
#endif
		{
		//	Tprintf("CMD: %x\n",(int)command);
			continue;
		}
#ifdef LITTLE_END
		if(command!=0xd7)	
#else
		if(command!=0xeb)
#endif
		{
			//Tprintf("%x:\n",(int)command);
			HaltInterval(20);
//			while(NIKONNSS);
			SET_TIMER_OVERFLOW
			while(!TF0)
			{
				if(!NIKONNSS)	
				{
					SET_TIMER_STOP
					break;
				} 
			}
			if(TF0)
			{
				SET_TIMER_STOP
				continue;
			} 
		}
*/
//		putchar(command);
		
		switch(command)
		{
#ifdef LITTLE_END
			case 0xa1:
#else
			case 0x85:
#endif
			while(!NIKONNSSI);

			if(DATA_SEND_TIMEOUT==LIT_PARA_Send(send_arrary1,sizeof(send_arrary1)))
			{
				NIKONMOSI = 1;
			}
/*			for(i=0;i<sizeof(send_arrary1);i++)
			{
				if(DATA_SEND_SUCESS==LIT_PARA_Send(send_arrary1[i]))
				{
				//	Tprintf("%x ",(int)send_arrary1[i]);

					HaltInterval(8);
				//	Delay();
				}else
				{
					break;
				}
			}
*/
			NIKONMOSI = 1;
//			SET_NSS_INPUT;
/*			NIKONNSSO = 1;
			NIKONMOSI = 1;
			HaltInterval(3);
			NIKONNSSO = 0;
			NIKONMOSI = 0;
			HaltInterval(120);
			NIKONMOSI = 1;
			NIKONNSSO = 1;
			HaltInterval(50);
			NIKONMOSI = 0;
			NIKONNSSO = 0;
*/

			break;

			//case 0x05:
#ifdef LITTLE_END
			case 0xa0:
#else
			case 0x05:
#endif

			while(!NIKONNSSI);

#ifdef NIKONNSS_SELF_HANDLE 
			//SET_NSS_OUTPUT;
			//HaltInterval(20);
			//NIKONNSSO = 0;
#endif	
			if(DATA_SEND_TIMEOUT==LIT_PARA_Send(send_arrary2,sizeof(send_arrary2)))
			{
				NIKONMOSI = 1;
			}else 
			{
				NIKONMOSI = 1;
			}
/*			for(i=0;i<sizeof(send_arrary2);i++)
			{
				if(DATA_SEND_SUCESS==LIT_PARA_Send(send_arrary2[i]))
				{
					//HaltInterval(15);
					while(!NIKONNSSI);
					//Tprintf("%x ",(int)send_arrary1[i]);
					//LoadUartDate();
					//HaltInterval(15);
				//	Delay();
				}else
				{
					NIKONMOSI = 1;
					break;
				}
			}
*/
//			NIKONMOSI = 1;
//#ifdef NIKONNSS_SELF_HANDLE
//			NIKONNSS = 1;
//#endif
			//if((receive_arrary[1]!=0xff)&&(info_flag!=0))
			if(receive_arrary[1]!=0xff)
			{
				GetInfo(0x0d);
			}

			//LoadUartDate();
			if(GETBIT(bitChar,BIT_BITCHAR_REDEYEFLAG))
			{
				flashStatue.redEye_ctr++;
				if(flashStatue.redEye_ctr==6)
				{
					NIKONMOSI = 0;
					//TAKE_PREEXPOSE2;
					TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
					Delay();
					NIKONMOSI = 1;
				}else if(flashStatue.redEye_ctr==12)
				{
					NIKONMOSI = 0;
					//TAKE_PREEXPOSE2
					TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
					Delay();
					NIKONMOSI = 1;
					//flashStatue.redEye_ctr=0;
					//CLRBIT(bitChar,BIT_BITCHAR_REDEYEFLAG);
				}
				
			}
			cameraSetting.rcvHandShake = 0;
			//HaltInterval(100);
			//HaltInterval(2);
			NIKONMOSI = 0;
			HaltInterval(50);
			NIKONMOSI = 1;
#ifdef NIKONNSS_SELF_HANDLE
			//NIKONNSSO = 1;
			//SET_NSS_INPUT
#endif
			
			break;

			//case 0x0b:
#ifdef LITTLE_END
			case 0xd0:
#else
			case 0x0b:
#endif

			while(!NIKONNSSI);
			
			for(i=0;i<2;i++)
			{
				//receive_arrary[i] = DATA_Receive(1);
				DATA_Receive(1,&receive_arrary[i]);
				Delay();
				HaltInterval(20);
			}
			if((receive_arrary[0]==0)&&(receive_arrary[1]==0))
			{
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
				if(!flashPara.ledFlag)
				{
					if(AUX_MODE_LED==flashPara.auxLiteSel)
					{
						LED_LITE = 0;
					}else if(AUX_MODE_FLA==flashPara.auxLiteSel)
					{
					/*
						multiSet.mulCnt = 0;
						multiSet.finalExpTimes=0;
						multiSet.mulCtl = MULTI_FLASH_DISABLE;
					*/
						
					}else if(AUX_MODE_AUTO==flashPara.auxLiteSel)
					{
						LED_LITE = 0;
						flashStatue.auxr_light = 0;
					}
				}
#else
				AFCTL = 0;
				flashStatue.auxr_light=0;
#endif
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_AUXRLIT);
			}else
			{
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
				if(!flashPara.ledFlag)				//if LED light is turned on, no af
				{
					if(AUX_MODE_LED==flashPara.auxLiteSel)
					{
						LED_LITE = 1;
					}else if(AUX_MODE_FLA==flashPara.auxLiteSel)
					{
						if(flashStatue.flash_enable==0x01)
						TURN_ON_TRIGER(AF_FLASH_INTERVAL);
						/*
						multiSet.finalExpTimes=100;
						multiSet.lowFrqCnt = 50;	
						multiSet.mulCtl=MULTI_FLASH_ENABLE;
						multiSet.mulCnt = 0;
						multiSet.cMode = MULTI_MODE_CAM;
						multiSet.startFlag = MULTI_STFLAG_START;
						//multiSet.startFlag = MULTI_STFLAG_START;			//once linked, this flag will stop multiply flashing
						*/
					}else if(AUX_MODE_AUTO==flashPara.auxLiteSel)
					{
						if(flashStatue.auxr_light++>10)
						{
							if(flashStatue.flash_enable==0x01)
							TURN_ON_TRIGER(AF_FLASH_INTERVAL);
						}else
						{
							LED_LITE = 1;
						}
					}
				}
#else
				AFCTL = 1;
				flashStatue.auxr_light=1;
#endif
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_AUXRLIT);
			}
			break;
							

			//case 0x45:
#ifdef LITTLE_END
			case 0xa2:
#else
			case 0x45:
#endif
			while(!NIKONNSSI);

			if(DATA_SEND_TIMEOUT==LIT_PARA_Send((send_arrary3),sizeof(send_arrary3)))
			{
				NIKONMOSI = 1;
			}
			/*for(i=0;i<sizeof(send_arrary3);i++)
			{
				if(DATA_SEND_SUCESS==LIT_PARA_Send(send_arrary3[i]))
				{
					//Tprintf("%x ",(int)send_arrary1[i]);
					HaltInterval(10);			
				//	Delay();
				}else 
				{
					break;
				}
			}*/
			#ifndef NIKON_D50
//			HaltInterval(100);
			NIKONMOSI = 1;
//			NIKONNSSO = 1;
			#else
//			HaltInterval(100);
			NIKONMOSI = 1;
//			NIKONNSSO = 1;
			//LoadUartDate();
			//while(!NIKONNSSI);
			//HaltInterval(3);
			//NIKONMOSI = 1;
			//HaltInterval(10);
			//NIKONMOSI = 0;
			//HaltInterval(100);
			//NIKONMOSI=1;
			#endif
			break;

			//case 0x8d:
#ifdef LITTLE_END
			case 0xb1:
#else
			case 0x8d:
#endif

			while(!NIKONNSSI);
			NIKONMOSI = 1;
			for(i=0;i<10;i++)
			{
				//receive_arrary[i] = CMD_Receive();
				//receive_arrary[i] = DATA_Receive(1);
				 DATA_Receive(1,&receive_arrary[i]);
				 while(!NIKONNSSI);
				//Tprintf("%x ",(int)receive_arrary[i]);
				//HaltInterval(15);
			//	Delay();
			}
			//LoadUartDate();
			GetInfo(0x8d);
			NIKONMOSI = 1;
			break;

			//case 0x0d:
#ifdef LITTLE_END
			case 0xb0:
#else
			case 0x0d:
#endif

			while(!NIKONNSSI);
			
			NIKONMOSI = 1;
			
#ifdef NIKONNSS_SELF_HANDLE
			//SET_NSS_OUTPUT;
			//HaltInterval(10);			
			//NIKONNSSO = 0;
#endif
			for(i=0;i<15;i++)
			{
				//receive_arrary[i] = DATA_Receive(1);
				//rcvData = DATA_Receive(1);
				if(DATA_SEND_SUCESS==DATA_Receive(1,&receive_arrary[i]))
				{
					//Delay();
					//HaltInterval(15);
					//while(!NIKONNSSI);
					SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WAIT_SIGNAL_INTERVAL,TIMER_DELAY_ASYN);
					
					while(!(TM7ICR&0X01))
					{
						if(NIKONNSSI)	 
						{
							SET_TIMER_STOP;
							break;
						}
					}

					if(GETBIT(TM7ICR,0))
					{
						SET_TIMER_STOP;
						//CLRBIT(TM7ICR,0);
						disable_irq(); 
						MEMCTR |= 0X04; 
						TM7ICR &= 0xfe;
						MEMCTR &= ~0X04;	
						enable_irq();
						break;
					}
					//Tprintf("%x ",(int)receive_arrary[i]);
#ifdef NIKONNSS_SELF_HANDLE
					//HaltInterval(8);
#endif
					//HaltInterval(15);	
					//if(receive_arrary[i]==0xff)
					//break;
			//		while(NIKONNSS);
				}else
				{
					NIKONMOSI = 1;
					break;
				}
				
			//	Tprintf("%x ",(int)receive_arrary[i]);
			}/*if(receive_arrary[13]==receive_arrary[14])
			{
				receive_arrary[i++] = DATA_Receive(1);
				info_flag = 0;
			}else 
			{
				info_flag = 1;
			}*/
//#ifdef NIKONNSS_SELF_HANDLE
//			NIKONNSS = 1;
//#endif
			if(cameraSetting.rcvHandShake==1)
			{
				AFCTL = 0;
				//cameraSetting.rcv_flag=0;
			}else
			{
				cameraSetting.rcvHandShake = 1;
			}

			//info_flag = 1;
			uartSend.cls_invalid = 0xff;
			//filter = (receive_arrary[1]&0x0f);
			//if((receive_arrary[1]!=0xff)&&(receive_arrary[0]!=0xff))
			//if((filter==0x04)||(filter==0x0c)||(filter==0x06)||(filter==0x0e))
			//if(!(receive_arrary[1]&0x11))
			//if(receive_arrary[1]!=0xff)
			//{
			//	GetInfo(0x0d);
			//}
		//	receive_arrary[i++] = CMD_Receive();
		//	Tprintf("%x ",(int)receive_arrary[i]);
			HaltInterval(2);
			NIKONMOSI = 0;
			//LoadUartDate();
//			NIKONMOSI = 0;
			//HaltInterval(100);
			HaltInterval(50);
//			HaltInterval(50);
			//HaltInterval(100);
#ifdef NIKONNSS_SELF_HANDLE
//			NIKONNSSO = 1;
//			SET_NSS_INPUT;
#endif			
			NIKONMOSI = 1;	
			//SET_SBO_IN;
//			while(!NIKONNSSI);
			break;

			//case 0x03:
#ifdef LITTLE_END
			case 0xc0:
#else
			case 0x03:
#endif
			while(!NIKONNSSI);
			
			NIKONMOSI = 1;
			for(i=0;i<2;i++)
			{
				//receive_arrary[i] = DATA_Receive(1);
				 DATA_Receive(1,&receive_arrary[i] );
				//Delay();
				//Tprintf("%x ",(int)receive_arrary[i]);
				//HaltInterval(20);
				while(!NIKONNSSI);
			//	Delay();
			}
			if(0x00c1==MAKEWORD(receive_arrary[1],receive_arrary[0]))
			{
				LIT_DATA_Send(0X0A);
				//Tprintf("50h\n");
				//HaltInterval(20);
				while(!NIKONNSSI);
				//PARA_Send(0XA0);
				LIT_DATA_Send(0X5E);
				//HaltInterval(20);
				while(!NIKONNSSI);
				//PARA_Send(0X08);
				LIT_DATA_Send(0X69);
				while(!NIKONNSSI);
				//HaltInterval(20);
				if(WORKINGMODE_MULTI==flashPara.workmode)
				{
					multiSet.finalExpTimes=0;
					//multiSet.mulCtl = MULTI_FLASH_DISABLE;
					//MUL_PCATIMER_STOP
					//SETBIT(bitChar,BIT_BITCHAR_ONETIMEFLASH);
					//SETBIT(uartSend.sendFlagEx,UARTFLAGEX_INDEX_ONETIMEFLASH);
					while(!NIKONNSSO);
					//NIKONNSSO = 1;
					//InitUart();
				}
			}
			else{
		       	//PARA_Send(0X50);
		       	LIT_DATA_Send(0X0A);
				//Tprintf("50h\n");
				//HaltInterval(20);
				while(!NIKONNSSI);
				//PARA_Send(0XA0);
				LIT_DATA_Send(0X05);
				//HaltInterval(20);
				while(!NIKONNSSI);
				//PARA_Send(0X08);
				LIT_DATA_Send(0X10);
				//HaltInterval(20);
				while(!NIKONNSSI);
				//LoadUartDate();
			}
			break;

			//case 0x07:
#ifdef LITTLE_END
			case 0xe0:
#else
			case 0x07:
#endif
			while(!NIKONNSSI);
			DATA_Receive(1,&receive_arrary[0]);
			LIT_DATA_Send(0XE0);
			//SET_SBT_IN			
			if(uartSend.cls)
			{
				uartSend.cls = 0;
				uartSendFlag.cls = 1;
				//SETBIT(uartSend.sendFlag,UARTFLAG_INDEX_CLS);
			} 
			break;

			//case 0xcb:
#ifdef LITTLE_END
			case 0xd3:
#else
			case 0xcb:
#endif
			//HaltInterval(15);
#ifdef EXT_LED
			if(flashPara.faceLite)
			{
				LED_LITE = 1;
			}
#endif
			
			disable_irq();
			while(!NIKONNSSI);
			for(i=0;i<2;i++)
			{
				//receive_arrary[i] = DATA_Receive(0);			//where there turn off the interrupt is to avoid flash leaking.
				//receive_arrary[i] = DATA_Receive(1);
				//Tprintf("%x ",(int)receive_arrary[i]);
				//HaltInterval(20);
				DATA_Receive(0,&receive_arrary[i]);
				while(!NIKONNSSI);
				//HaltInterval(15);
				//	Delay();
			}
			if((receive_arrary[0]&0xf0)==0xd0)
			{
				flaLockFlag = 1;
			}
			if(((receive_arrary[0]&0x0f)<=0x0c)&&((receive_arrary[0]&0x0f)>=0x09))
			{
				flashPara.fp = HIGH_SPEED;
			}else
			{
				flashPara.fp = LOW_SPEED;
			}
			//enable_irq();
			//DecodeFlashData(UART_ENCODE_DEBUG,receive_arrary[RCVDAT_LOCATION_EXPOSE],0);
			//exposeCovert = CodeRecombina(receive_arrary[RCVDAT_LOCATION_EXPOSE]);
			exposeCovert = receive_arrary[RCVDAT_LOCATION_EXPOSE];
			ExposeTimeCal(exposeCovert,flashPara.fp);	
			if(flashStatue.flash_enable==0x01)
			{
				SETBIT(bitFlag,BIT_BITFLAG_FLASHLOCK);
				if(WORKINGMODE_MASTER==flashPara.workmode)
				{
					wlSet.m_sendCmdEn = 1;
				}
			}
			
			while(NIKONNSSI);
			//PARA_Send(0xcb);
			LIT_DATA_Send(0xd3);
			//media = 0xd3;
			//LIT_PARA_Send(&media,1);
			//Delay();
			while(!NIKONNSSI);
			enable_irq();
			//SpiInterval();
			//SpiInterval();
			HaltInterval(3);
			NIKONMOSI = 0;
			HaltInterval(25);
			NIKONMOSI = 1;
			break;
			
			
/*			HaltInterval(100);
			NIKONNSS = 1;
			NIKONMOSI = 0;
			Delay();
			Delay();
			NIKONMOSI = 1;*/
//			while(FLASHCTL);
//			while(NIKONMOSI);
//			while(!NIKONMOSI);
//			{
			//	FLASHCTL = 1;
//				NIKONMOSI = 1;
//			}
//			while(!FLASHCTL)
//			{
//				FLASHCTL = 1;
//			}
//			Tprintf("real flash\n");
//			RealFlash();
//			while(1)
//			{
//				NIKONMOSI = 1;
//				FLASHCTL = 1;
//				Tprintf("end\n");	
//			}
//			FLASHCTL = 1;
//			LoadUartDate();
//			break;

			//case 0x8b:			//red eyes
#ifdef LITTLE_END
			case 0xd1:
#else
			case 0x8b:
#endif
			while(!NIKONNSSI);

			//receive_arrary[0] = DATA_Receive(1);
			DATA_Receive(1,&receive_arrary[0]);
			Delay();
			HaltInterval(100);
			//Delay_ms();
			if((receive_arrary[0]==RED_EYE_START)&&(flashPara.workmode!=WORKINGMODE_MASTER)&&(flashPara.workmode!=WORKINGMODE_RTN))
			{
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
				if((flashStatue.flash_enable==0x01)&&(!flashPara.ledFlag))		//when turn on the led light, aviod redeyes
#else
				if(flashStatue.flash_enable==0x01)
#endif
				{
					NIKONMOSI = 0;
					//TAKE_PREEXPOSE2
					TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
					HaltSec(2);
					NIKONMOSI = 1;
					SETBIT(bitChar,BIT_BITCHAR_REDEYEFLAG);
				}else
				{
					Delay();
				}
				//SETBIT(bitChar,BIT_BITCHAR_REDEYEFLAG);
			}else if(receive_arrary[0]==RED_EYE_FINISH)
			{
				flashStatue.redEye_ctr = 0;
				CLRBIT(bitChar,BIT_BITCHAR_REDEYEFLAG);
			}
			//flashStatue.redEye_ctr++;
			break;


			case 0xd2:					//especaily for p7100
			while(!NIKONNSSI);
			DATA_Receive(1,&receive_arrary[0]);
			while(NIKONSCKIN);
			/*
			HaltInterval(3);	
			NIKONMOSI = 0; 
			RELEASE_SBT;
			HaltInterval(10);	
			NIKONSCK = 0;*/
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
			if((flashStatue.flash_enable==0x01)&&(!flashPara.ledFlag))
#else
			if((flashStatue.flash_enable==0x01)/*&&(WORKINGMODE_TTL==flashPara.workmode)*/)	//include mode master take part in flashing
#endif
			{
				//TAKE_PREEXPOSE1
				TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
				SETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE);
				//ES = 0;
			}else
			{
				//Delay();
				HaltInterval(10);
			}
			/*
			HaltInterval(3);
			NIKONSCK = 1;
			//Delay();
			LINKON_SBT;
			HaltInterval(200);
			NIKONMOSI = 1; */
			break;

#if 0
			case 0xd8:
			while(!NIKONNSSI);
			DATA_Receive(1,&receive_arrary[0]);
			while(NIKONSCKIN);
			if((flashStatue.flash_enable==0x01)/*&&(WORKINGMODE_TTL==flashPara.workmode)*/)	//include mode master take part in flashing
			{
				//TAKE_PREEXPOSE1
				TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
				SETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE);
				//ES = 0;
			}else
			{
				//Delay();
				HaltInterval(10);
			}
			break;
#endif


			//case 0xeb:
#ifdef LITTLE_END
			case 0xd7:
#else
			case 0xeb:
#endif
//			NIKONMOSI = 0; 
//			Tprintf("previous flash\n");
			//LoadUartDate();
			//Delay();
			while(!NIKONNSSI);
			HaltInterval(3);	
			NIKONMOSI = 0; 
			RELEASE_SBT;
			HaltInterval(10);	
			NIKONSCK = 0;
#ifdef TTL_DEBUG
			if(GETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE))
			{
				SETBIT(bitChar,BIT_BITCHAR_DEBUGERROR);
			}
#endif
			//if((flashStatue.flash_enable==0x01)&&(WORKINGMODE_TTL==flashPara.workmode))
			//if((flashStatue.flash_enable==0x01)&&(!flashPara.ledFlag)/*&&(WORKINGMODE_TTL==flashPara.workmode)*/)	//include mode master take part in flashing
//#ifdef CANON320EX_COPY
#ifdef EXT_LED
			if((flashStatue.flash_enable==0x01)&&(!flashPara.ledFlag))
#else
			if(flashStatue.flash_enable==0x01)
#endif
			{
				//TAKE_PREEXPOSE1
				TURN_ON_TRIGER(PREFLASH_1_INTERVAL);
				SETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE);
				//ES = 0;
			}else
			{
				//Delay();
				HaltInterval(10);
			}
			ttl_pre2_flag = 0;
			flaLockFlag = 0;
			//Delay();
			HaltInterval(3);
			NIKONSCK = 1;
			//Delay();
			LINKON_SBT;
			HaltInterval(200);
			NIKONMOSI = 1; 
/*			while(!NIKONMOSI);
			while(NIKONMOSI);
			NIKONMOSI = 0;
			Delay(); 
			Delay();
			Delay();
			Delay();
			Delay();
			NIKONMOSI = 1;

			for(i=0;i<2;i++)
			{
				receive_arrary[i] = CMD_Receive();
				HaltInterval(20);
			//	Tprintf("%x ",(int)receive_arrary[i]);
			//	Delay();
			}
			//Tprintf("2nd preflash\n");
			HaltInterval(20);
			NIKONMOSI = 0;
			Delay();
			NIKONSCK = 0;
			//if(flashStatue.flash_enable!=0x01)
			if(GETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE))
			{
				TAKE_PREEXPOSE2
				CLRBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE);
				SETBIT(bitChar,BIT_BITCHAR_PRE2EXPOSE);
				
			}else
			{
				Delay();
			}
			Delay();
			NIKONSCK = 1;
			Delay();
			NIKONMOSI = 1;
			//LoadUartDate();
*/
			break;

#ifdef LITTLE_END
			case 0xd9:
#endif
				while(!NIKONNSSI);
				//receive_arrary[i] = DATA_Receive();
				//BuildWlmFlaCode();
				if(WORKINGMODE_WLM!=flashPara.workmode)
				{
					break;
				}
				
				LIT_DATA_Send(0xd9);
				HaltInterval(8);
				NIKONMOSI = 0;
				HaltInterval(42);
				if((CHARGE_FULL==charge_ok)&&(flashStatue.flash_enable==0X01))
				{		
					wlSet.m_sendCmdEn = 1;
					if(BuildWlmFlaCode())
					{
						WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);				
					}
				}else 
				{
					wlSet.m_sendCmdEn = 0;
				}
				
				NIKONMOSI = 1;
				WrongCodeFilter = HEAD_DATA_RECEIVED;
			break;

			case 0xda:
				//while(1);
				while(!NIKONNSSI);
				
				if(HEAD_DATA_UNRCVD==WrongCodeFilter)
				{
					break;
				}
				HaltInterval(40);
				NIKONMOSI = 0;
				//WLM_SendData(&0x90,1);
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					wlSet.m_flaCmd[0] = 0x90;
					WLM_SendData(&wlSet.m_flaCmd[0],1);
				}
				//HaltInterval(300);
				//HaltInterval(500);
				//HaltInterval(260);				//delay 3ms plus
				SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_BEGINPRE_INTERVAL,TIMER_DELAY_SYN);
				//EA = 0;
				disable_irq(); 
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
					//WLM_TRIGER(WLM_IMPLUS_TIME);
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
#ifdef WLM_CMD_AD
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_PREFLA_INTERVAL);
#else
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME);			//the first implus should be strength
#endif
				}
				//HaltInterval(PRE_FLASH_INTERVAL);
				//WLM_TIMER0_DELAY(WLM_PREFLA_INTERVAL);
				//SET_TIMER7_OVERFLOW(WLM_PREFLA_INTERVAL,TIMER_DELAY_SYN);
#ifndef WLM_CMD_AD
				WLM_CMDSEND_DELAY(WLM_PREFLA_INTERVAL);
#endif
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
					//WLM_TRIGER(WLM_IMPLUS_TIME);
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME);
				}
				//EA = 1;
				//enable_irq();
				RELEASE_SBT;
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_CONFIRM,TIMER_DELAY_SYN);
				WLM_CMDSEND_DELAY(WLM_PREFLA_CONFIRM);
				//HaltInterval(40);
				NIKONSCK = 0;
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_FINISH,TIMER_DELAY_SYN);
				WLM_CMDSEND_DELAY(WLM_PREFLA_FINISH);
				//HaltInterval(35);
				NIKONSCK = 1;
				enable_irq();
				LINKON_SBT;
			break;

			case 0xdb:
				while(!NIKONNSSI);
				
				if(HEAD_DATA_RECEIVED==WrongCodeFilter)
				{				
					HaltInterval(40);
					NIKONMOSI = 0;
					//WLM_SendData(&0xa0,1);
					wlSet.m_flaCmd[0] = 0xa0;
					if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
					{
						WLM_SendData(&wlSet.m_flaCmd[0],1);
					}
					//HaltInterval(300);
					//HaltInterval(500);
					//HaltInterval(260);				//delay 3ms plus
					SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_BEGINPRE_INTERVAL,TIMER_DELAY_SYN);
					//EA = 0;
					disable_irq();
					//if(wlSet.m_sendCmdEn)
					if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
					{
						//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
						//WLM_TRIGER(WLM_IMPLUS_TIME);
						//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
#ifdef WLM_CMD_AD
						WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_PREFLA_INTERVAL);
#else
						WLM_COMMAND_TRIGER(WLM_HEAD_TIME);			//the first implus should be strength
#endif
					}
					//HaltInterval(PRE_FLASH_INTERVAL);
					//WLM_TIMER0_DELAY(WLM_PREFLA_INTERVAL);
					//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_INTERVAL,TIMER_DELAY_SYN);
#ifndef WLM_CMD_AD
					WLM_CMDSEND_DELAY(WLM_PREFLA_INTERVAL);
#endif
					//if(wlSet.m_sendCmdEn)
					if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
					{
						//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
						//WLM_TRIGER(WLM_IMPLUS_TIME);
						//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
						WLM_COMMAND_TRIGER(WLM_HEAD_TIME);
					}
					//EA = 1;
					//enable_irq();
					RELEASE_SBT;
					//HaltInterval(40);
					//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_CONFIRM,TIMER_DELAY_SYN);
					WLM_CMDSEND_DELAY(WLM_PREFLA_CONFIRM);
					NIKONSCK = 0;
					//HaltInterval(35);
					//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_FINISH,TIMER_DELAY_SYN);
					WLM_CMDSEND_DELAY(WLM_PREFLA_FINISH);
					NIKONSCK = 1;
					enable_irq();
					LINKON_SBT
				}
			break;

			case 0xdc:
				while(!NIKONNSSI);
				
				if(HEAD_DATA_UNRCVD==WrongCodeFilter)
				{
					break;
				}
				HaltInterval(40);
				NIKONMOSI = 0;
				//WLM_SendData(&0xb0,1);
				wlSet.m_flaCmd[0] = 0xb0;
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					WLM_SendData(&wlSet.m_flaCmd[0],1);
				}
				//HaltInterval(300);
				//HaltInterval(500);
				//HaltInterval(260);				//delay 3ms plus
				SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_BEGINPRE_INTERVAL,TIMER_DELAY_SYN);
				//EA = 0;
				disable_irq();
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
					//WLM_TRIGER(WLM_IMPLUS_TIME);
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
#ifdef WLM_CMD_AD
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_PREFLA_INTERVAL);
#else
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME);			//the first implus should be strength
#endif
				}
				//HaltInterval(PRE_FLASH_INTERVAL);
				//WLM_TIMER0_DELAY(WLM_PREFLA_INTERVAL);
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_INTERVAL,TIMER_DELAY_SYN);
#ifndef WLM_CMD_AD
				WLM_CMDSEND_DELAY(WLM_PREFLA_INTERVAL);
#endif
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
					//WLM_TRIGER(WLM_IMPLUS_TIME);
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME);
				}
				//EA = 1;
				//enable_irq();
				RELEASE_SBT;
				//HaltInterval(40);
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_CONFIRM,TIMER_DELAY_SYN);
				WLM_CMDSEND_DELAY(WLM_PREFLA_CONFIRM);
				NIKONSCK = 0;
				WLM_CMDSEND_DELAY(WLM_PREFLA_FINISH);
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_FINISH,TIMER_DELAY_SYN);
				//HaltInterval(35);
				NIKONSCK = 1;
				enable_irq();
				LINKON_SBT;
			break;

			case 0xdd:
				while(!NIKONNSSI);
				
				if(HEAD_DATA_UNRCVD==WrongCodeFilter)
				{
					break;
				}
				//HaltInterval(40);
				//HaltInterval(35);
				NIKONMOSI = 0;
				//HaltInterval(50);
				//HaltInterval(45);
				WLM_CMDSEND_DELAY(WLM_PREFLA_CONFIRM);
				//EA = 0;
				disable_irq();
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
					//WLM_TRIGER(WLM_IMPLUS_TIME);
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
#ifdef WLM_CMD_AD
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_PREFLA_INTERVAL);
#else
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME);			//the first implus should be strength
#endif
				}
				//HaltInterval(PRE_FLASH_INTERVAL);
				//WLM_TIMER0_DELAY(WLM_PREFLA_INTERVAL);
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_INTERVAL,TIMER_DELAY_SYN);
#ifndef WLM_CMD_AD
				WLM_CMDSEND_DELAY(WLM_PREFLA_INTERVAL);
#endif
				//if(wlSet.m_sendCmdEn)
				if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
				{
					//TURN_ON_TRIGER(WLM_IMPLUS_TIME);
					//WLM_TRIGER(WLM_IMPLUS_TIME);
					//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
					WLM_COMMAND_TRIGER(WLM_HEAD_TIME);
				}
				//EA = 1;
				//enable_irq();
				RELEASE_SBT;
				//HaltInterval(40);
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_CONFIRM,TIMER_DELAY_SYN);
				WLM_CMDSEND_DELAY(WLM_PREFLA_CONFIRM);
				NIKONSCK = 0;
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_PREFLA_FINISH,TIMER_DELAY_SYN);
				WLM_CMDSEND_DELAY(WLM_PREFLA_FINISH);
				//HaltInterval(35);
				NIKONSCK = 1;
				enable_irq();
				LINKON_SBT;
			break;

			case 0xd4:
				//LIT_PARA_Send(0X10);
				//HaltInterval(10);
				//LIT_PARA_Send(0X81);
				//HaltInterval(10);
				while(!NIKONNSSI);
				for(i=0;i<6;i++)
				{
					//receive_arrary[i] = DATA_Receive(1);
					DATA_Receive(1,&receive_arrary[i] );
					while(!NIKONNSSI);
					//Delay();
					//Tprintf("%x ",(int)receive_arrary[i]);
					//HaltInterval(20);
					//HaltInterval(10);
				}
				media = 0xd4;
				LIT_PARA_Send(&media,1);
				NIKONMOSI = 0;
				HaltInterval(70);
				//BuildRealFlashCmd(receive_arrary[2],receive_arrary[3],receive_arrary[4] );
				/*if(GETBIT(receive_arrary[0],7))
				{
					wlSet.isRear = WLM_ISREAR_REAR;
				}else
				{
					wlSet.isRear = WLM_ISREAR_FRONT;
				}*/
				if(!(receive_arrary[2]&0x0B))
				{
					flashPara.fp = LOW_SPEED;
				}else
				{
					flashPara.fp = HIGH_SPEED;
				}
				
				if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_M].mode)
				{
					//ExposeTimeCal(receive_arrary[1],flashPara.fp);
					ExposeTimeCal(receive_arrary[1],flashPara.fp);
				}
				if(BuildRealFlashCmd(receive_arrary[0],receive_arrary[2],receive_arrary[3],receive_arrary[4],(cameraSetting.pShutter>0x36)))
				//if(BuildRealFlashCmd(0x51,0x51,0x51))
				{
					//if(wlSet.m_sendCmdEn)
					//if((wlSet.m_sendCmdEn!=0)&&(WORKINGMODE_MASTER==flashPara.workmode))
					//if((0x01==flashStatue.flash_enable)&&(WORKINGMODE_MASTER==flashPara.workmode))
					if(wlSet.m_sendCmdEn)
					{
						//wlSet.m_sendCmdEn = 1;
						WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
					}else
					{
						if((0x01==flashStatue.flash_enable)&&(WORKINGMODE_MASTER==flashPara.workmode)&&(charge_ok>AD_CHARGE_RDY))
						{
							wlSet.m_sendCmdEn = 1;
							WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);
						}else if(!WrongCodeFilter)
						{
							if((0x01==flashStatue.flash_enable)&&(charge_ok>AD_CHARGE_RDY))
							{
								wlSet.m_sendCmdEn = 1;
								WLM_SendData(wlSet.m_flaCmd,wlSet.m_cmdIndex);	
							}
							
						}
					}
				}
				HaltInterval(10);
				NIKONMOSI = 1;
			break;
			
			

			//case 0x1b:
#ifdef LITTLE_END
			case 0xd8:
#else
			case 0x1b:
#endif
			while(!NIKONNSSI);
			//if(CMD_Receive()==0x18)
			DATA_Receive(1,&rcvData);
			NIKONMOSI = 1;
			while(!NIKONNSSI);
			HaltInterval(3);
			NIKONMOSI = 0;
			//if(rcvData==0x18)
			{
				NIKONMOSI = 0;
				HaltInterval(20);
				Delay();
				RELEASE_SBT;
				NIKONSCK = 0;
//#ifdef	CANON320EX_COPY
#ifdef EXT_LED
				if(GETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE)&&(!flashPara.ledFlag))
#else
				if(GETBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE))
#endif
				{
					//TAKE_PREEXPOSE2
					TURN_ON_TRIGER(PREFLASH_2_INTERVAL);
					CLRBIT(bitChar,BIT_BITCHAR_PRE1EXPOSE);
					SETBIT(bitChar,BIT_BITCHAR_PRE2EXPOSE);
					ttl_pre2_flag = UART_ENCODE_DEBUG_EXPOSE2;
					tpfRsv = 2;
					
				}
				//Delay();
				HaltInterval(5);
				NIKONSCK = 1;
				LINKON_SBT;
				//Delay();
				HaltInterval(150);
				NIKONMOSI = 1;	
			}
			break;

			//case 0xE0:
			//NIKONNSSO = 1;
			//break;

			case 0x00:
			NIKONNSSO = 1;
			break;
			

			default:
			//LoadUartDate();
			//Tprintf(":%x \n",(int)command);
			NIKONNSSO = 1;
			info_flag = 0;
			break;

		}
	}
}


unsigned char VerCodeCal(const unsigned char *arr)
{
	unsigned char i=0;
	unsigned char verRear=0;

	for (i=0;i<VERI_DATA_LEN;i++) {
		verRear += arr[i];
	}
	
	return verRear+1;
	
}
void HaltInterval(int tick)
{
	while(tick--)
	{
		Delay_10us(1);
	}
}

void Delay_ms()
{
	SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,MS_INTERVAL,TIMER_DELAY_SYN);
	if(WORKINGMODE_RTN==flashPara.workmode)
	{
		Timer7SetCapture();
	}
}

void HaltSec(int tick)
{
	while(tick--)
	{
		Delay_ms();
	}
}


void SpiInterval()
{	
	unsigned char count;
	for (count = 6; count > 0; count--);
}

/*
void Delay(void)
{
	unsigned char i = 0;
   	 for(i=200;i>0;i--)
    //for(i=100;i>0;i--)
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();

	}
}
*/

unsigned char BuildWlmFlaCode()
{

	wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1] = wlSet.m_flaCmd[2] = 0 ;
	wlSet.m_flaCmd[0] = 0x06;
	wlSet.m_flaCmd[0] <<= 4;

	if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x0b;
		wlSet.m_cmdIndex = 1;
	}else if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x09;
		wlSet.m_cmdIndex = 1;
	}
#ifdef MASTER_AUTO_ON
	else if(WORKINGMODE_AUTO==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x0A;
		wlSet.m_cmdIndex = 1;
	}
#endif
	else if(WORKINGMODE_HOLD==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		wlSet.m_flaCmd[0] |= 0x08;
		wlSet.m_cmdIndex = 0;
	}else
	{
		wlSet.m_flaCmd[0] |= 0x08;
		wlSet.m_cmdIndex = 0;
	}

	if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0b;
		wlSet.m_cmdIndex = 2;
	}else if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x09;
		wlSet.m_cmdIndex = 2;
	}
#ifdef MASTER_AUTO_ON
	else if(WORKINGMODE_AUTO==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0A;
		wlSet.m_cmdIndex = 2;
	}
#endif
	else if(WORKINGMODE_HOLD==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		wlSet.m_flaCmd[1] |= 0x08;
	}else
	{
		wlSet.m_flaCmd[1] |= 0x08;
	}

	wlSet.m_flaCmd[1] <<= 4;

	if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0b;
		wlSet.m_cmdIndex = 3;
	}else if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x09;
		wlSet.m_cmdIndex = 3;
	}
#ifdef MASTER_AUTO_ON
	else if(WORKINGMODE_AUTO==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x0A;
		wlSet.m_cmdIndex = 3;
	}
#endif
	else if(WORKINGMODE_HOLD==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		wlSet.m_flaCmd[1] |= 0x00;
	}else
	{
		wlSet.m_flaCmd[1] |= 0x00;
	}	

	if(wlSet.m_cmdIndex)
	{
		return 1;
	}else 
	{
		return 0;
	}
}

char BuildRealFlashCmd(unsigned char para0,unsigned char para1,unsigned char para2,unsigned char para3,char hpLev)
{
	unsigned int tmp1,tmp2,tmp3;
	//unsigned int cmpTmp;

	wlSet.m_cmdIndex = 0;
	wlSet.m_flaCmd[0] = wlSet.m_flaCmd[1] = wlSet.m_flaCmd[2] = wlSet.m_flaCmd[3] = 0;

	//putchar(para1);
	//putchar(para2);
	//putchar(para3);

//when the shutter is lower than 1/400, the max flash value is 0xBF
	if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp1 = (para1>>1) +0x8d;
			tmp1 = (para1>>1) + (unsigned int)(0xab - uartSend.pIso);
		}else
		{
			//tmp1 = (para1>>1)+0x50;
			tmp1 = (para1>>1) + (unsigned int)(0x6e - uartSend.pIso);
			//tmp1 |= 0x80;
			
		}
		//cmpTmp = tmp1+(wlSet.m_arrPara[WL_ARRARY_A].cmp1*3+wlSet.m_arrPara[WL_ARRARY_A].cmp2)*2;
		tmp1 += (wlSet.m_arrPara[WL_ARRARY_A].cmp1*3+wlSet.m_arrPara[WL_ARRARY_A].cmp2)*2;

		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//if(cmpTmp<0xa0)
			if(tmp1<0xa0)
			{
				tmp1 = 0xa0;
			}else if(tmp1>0xf0)//else if(cmpTmp>0xf0)
			{
				tmp1 = 0xf0;
			}/*else
			{
				tmp1 = cmpTmp;//(unsigned char)cmpTmp;
			}*/
		}else
		{
/*
			if(hpLev)
			{
				if(cmpTmp>0xBF)
				{
					tmp1 = 0xBF;
				}else if(cmpTmp<0x80)
				{
					tmp1 |= 0x80;
				}
			}else
			{
				if(cmpTmp>0xff)
				{
					tmp1 = ((cmpTmp & 0xff) | 0x80);
				}
			}
*/
			//if(cmpTmp>0xBF)
			if(tmp1>0xBF)
			{
				tmp1 = 0xBF;
			}else if(tmp1<0x80)//else if(cmpTmp<0x80)
			{
				tmp1 |= 0x80; 
			}
			
/*			
			if(cmpTmp<0x30)
			{
				tmp1 = 0x30;
			}else if(cmpTmp>0xf0)
			{
				tmp1 = 0xf0;
			}else
			{
				tmp1 = cmpTmp;
			}
*/
		}
		//putchar(tmp1);
		wlSet.m_cmdIndex = 2;
	}//else
	else if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		/*
		if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_A].mode)
		{
			tmp1 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_A].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_A].output2)%3)*2);
			wlSet.m_cmdIndex = 2;
		}else
		{
			tmp1 = 0x80;
		}*/
		tmp1 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_A].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_A].output2)%3)*2);
		wlSet.m_cmdIndex = 2;
	}
#ifdef MASTER_AUTO_ON
	else if(WORKINGMODE_AUTO==wlSet.m_arrPara[WL_ARRARY_A].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp1 = (uartSend.pApert + 0xc1) - uartSend.ISO ;
			tmp1 = ((unsigned int)uartSend.pApert + (unsigned int)(0xFD-uartSend.pIso));
			//wlSet.m_cmdIndex = 2;
		}else
		{
			tmp1 = ((unsigned int)(uartSend.pApert + cameraSetting.pShutter) + (unsigned int)(0xC7-uartSend.pIso));
			//wlSet.m_cmdIndex = 2;
		}
			
		//cmpTmp = tmp1+(wlSet.m_arrPara[WL_ARRARY_A].cmp1*3+wlSet.m_arrPara[WL_ARRARY_A].cmp2)*2;
		tmp1 += (wlSet.m_arrPara[WL_ARRARY_A].cmp1*3+wlSet.m_arrPara[WL_ARRARY_A].cmp2)*2;
		//if(cmpTmp>0xff)
		if(tmp1>0xff)
		{
			//tmp1 = ((cmpTmp&0xff) + 0x80);//0xff;
			tmp1 = ((tmp1&0xff) + 0x80);
		}/*else
		{
			//tmp1 = cmpTmp;//(unsigned char)cmpTmp;
			
		}*/

		wlSet.m_cmdIndex = 2;
		
	}
#endif
	else
	{
		tmp1 = 0x80;
	}
	
	if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		//tmp2 = (para2>>1) +0x8d;
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp2 = (para2>>1) +0x8d;
			tmp2 = (para2>>1) + (unsigned int)(0xab - uartSend.pIso);
		}else
		{
			//tmp2 = (para2>>1)+0x50;
			tmp2 = (para2>>1) + (unsigned int)(0x6e - uartSend.pIso);
			//tmp2 |= 0x80;
		}
		//cmpTmp = tmp2+((wlSet.m_arrPara[WL_ARRARY_B].cmp1-3)*3+wlSet.m_arrPara[WL_ARRARY_B])*2;
		//cmpTmp = tmp2+(wlSet.m_arrPara[WL_ARRARY_B].cmp1*3+wlSet.m_arrPara[WL_ARRARY_B].cmp2)*2;
		tmp2 += (wlSet.m_arrPara[WL_ARRARY_B].cmp1*3+wlSet.m_arrPara[WL_ARRARY_B].cmp2)*2;
		
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//if(cmpTmp<0xa0)
			if(tmp2<0xa0)
			{
				tmp2 = 0xa0;
			}else if(tmp2>0xf0)//else if(cmpTmp>0xf0)
			{
				tmp2 = 0xf0;
			}/*else
			{
				tmp2 = cmpTmp; //(unsigned char)cmpTmp;
			}*/
		}else
		{
/*
			if(hpLev)
			{
				if(cmpTmp>0xBF)
				{
					tmp2 = 0xBF;
				}else if(cmpTmp<0x80)
				{
					tmp2 |= 0x80;
				}
			}else
			{
				if(cmpTmp>0xff)
				{
					tmp2 = ((cmpTmp & 0xff) | 0x80);
				}
			}
*/
			//if(cmpTmp>0xBF)
			if(tmp2>0xBF)
			{
				tmp2 = 0xBF;
			}else if(tmp2<0x80)//else if(cmpTmp<0x80)
			{
				tmp2 |= 0x80; 
			}
/*		
			if(cmpTmp<0x30)
			{
				tmp2 = 0x30;
			}else if(cmpTmp>0xf0)
			{
				tmp2 = 0xf0;
			}else
			{
				tmp2 = cmpTmp; //(unsigned char)cmpTmp;
			}
*/
		}
		//putchar(tmp2);
		wlSet.m_cmdIndex = 3;
	}//else
	else if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		/*
		if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_B].mode)
		{
			tmp2 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_B].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_B].output2)%3)*2);
			wlSet.m_cmdIndex = 3;
		}else
		{
			tmp2 = 0x80;
		}*/
		tmp2 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_B].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_B].output2)%3)*2);
		wlSet.m_cmdIndex = 3;
		
	}
#ifdef MASTER_AUTO_ON
	else if(WORKINGMODE_AUTO==wlSet.m_arrPara[WL_ARRARY_B].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			tmp2 = ((unsigned int)uartSend.pApert + (unsigned int)(0xFD-uartSend.pIso));
			//wlSet.m_cmdIndex = 3;
		}else
		{
			tmp2 = ((unsigned int)(uartSend.pApert + cameraSetting.pShutter) + (unsigned int)(0xC7-uartSend.pIso));
			//wlSet.m_cmdIndex = 3;
		}

		//cmpTmp = tmp2+(wlSet.m_arrPara[WL_ARRARY_B].cmp1*3+wlSet.m_arrPara[WL_ARRARY_B].cmp2)*2;
		tmp2 += (wlSet.m_arrPara[WL_ARRARY_B].cmp1*3+wlSet.m_arrPara[WL_ARRARY_B].cmp2)*2;
		//if(cmpTmp>0xff)
		if(tmp2>0xff)
		{
			//tmp2 = 0xff;
			//tmp2 = ((cmpTmp&0xff) + 0x80);
			tmp2 = ((tmp2&0xff) + 0x80);
		}/*else
		{
			//tmp2 = cmpTmp;//(unsigned char)cmpTmp;
		}*/

		wlSet.m_cmdIndex = 3;
	}
#endif
	else
	{
		tmp2 = 0x80;
	}
	
	if(WORKINGMODE_TTL==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		//tmp3 = (para3>>1) +0x8d;
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//tmp3 = (para3>>1) +0x8d;
			tmp3 = (para3>>1) + (unsigned int)(0xab-uartSend.pIso);
		}else
		{
			//tmp3 = (para3>>1)+0x50;
			tmp3 = (para3>>1) + (unsigned int)(0x6e-uartSend.pIso);
			//tmp3 |= 0x80;
		}
		//cmpTmp = tmp3+(wlSet.m_arrPara[WL_ARRARY_C].cmp1*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2)*2;
		tmp3 += (wlSet.m_arrPara[WL_ARRARY_C].cmp1*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2)*2;
		
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			//if(cmpTmp<0xa0)
			if(tmp3<0xa0)
			{
				tmp3 = 0xa0;
			}else if(tmp3>0xf0)//else if(cmpTmp>0xf0)
			{
				tmp3 = 0xf0;
			}/*else
			{
				tmp3 = cmpTmp; //(unsigned char)cmpTmp;
			}*/
		}else
		{
/*
			if(hpLev)
			{
				if(cmpTmp>0xBF)
				{
					tmp3 = 0xBF;
				}else if(cmpTmp<0x80)
				{
					tmp3 |= 0x80;
				}
			}else
			{
				if(cmpTmp>0xff)
				{
					tmp3 = ((cmpTmp & 0xff) | 0x80);
				}
			}
*/
			//if(cmpTmp>0xBF)
			if(tmp3>0xBF)
			{
				tmp3 = 0xBF;
			}else if(tmp3<0x80)//else if(cmpTmp<0x80)
			{
				tmp3 |= 0x80; 
			}
			/*
			if(cmpTmp<0x30)
			{
				tmp3 = 0x30;
			}else if(cmpTmp>0xf0)
			{
				tmp3 = 0xf0;
			}else
			{
				tmp3= cmpTmp; //(unsigned char)cmpTmp;
			}*/
		}
		//putchar(tmp3);
		wlSet.m_cmdIndex = 4;
	}//else
	else if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		/*
		if(WORKINGMODE_MANU==wlSet.m_arrPara[WL_ARRARY_C].mode)
		{
			tmp3 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_C].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_C].output2)%3)*2);
			wlSet.m_cmdIndex = 4;
		}else
		{
			tmp3 = 0x80;
		}*/
		tmp3 = (0x80+((8-wlSet.m_arrPara[WL_ARRARY_C].output1)*3-(3+wlSet.m_arrPara[WL_ARRARY_C].output2)%3)*2);
		wlSet.m_cmdIndex = 4;
	}
#ifdef MASTER_AUTO_ON
	else if(WORKINGMODE_AUTO==wlSet.m_arrPara[WL_ARRARY_C].mode)
	{
		//if(LOW_SPEED==flashPara.fp)
		if(!(para0&0x0B))
		{
			tmp3 = ((unsigned int)uartSend.pApert + (unsigned int)(0xFD-uartSend.pIso));
			//wlSet.m_cmdIndex = 4;
		}else
		{
			tmp3 = ((unsigned int)(uartSend.pApert + cameraSetting.pShutter) + (unsigned int)(0xC7-uartSend.pIso));
			//wlSet.m_cmdIndex = 4;
		}

		//cmpTmp = tmp3+(wlSet.m_arrPara[WL_ARRARY_C].cmp1*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2)*2;
		tmp3 += (wlSet.m_arrPara[WL_ARRARY_C].cmp1*3+wlSet.m_arrPara[WL_ARRARY_C].cmp2)*2;
		//if(cmpTmp>0xff)
		if(tmp3>0xff)
		{
			//tmp3 = 0xff;
			//tmp3 = ((cmpTmp&0xff) + 0x80);
			tmp3 = ((tmp3&0xff) + 0x80);
		}/*else
		{
			tmp3 = cmpTmp;//(unsigned char)cmpTmp;
		}*/

		wlSet.m_cmdIndex = 4;
	}
#endif
	else
	{
		tmp3 = 0x80;
	}

	if(((para0&0x0f)>=0x09)&&((para0&0x0f)<=0x0c))
	{
		if(0xc0==(para0&0xc0))
		{
			wlSet.m_flaCmd[0] = (0xE0 |(para0&0x0B));
		}else
		{
			wlSet.m_flaCmd[0] = (0xd0 |(para0&0x0B));
		}

		wlSet.m_flaCmd[1]  = tmp1;
		wlSet.m_flaCmd[2]  = tmp2;
		wlSet.m_flaCmd[3]  = tmp3;
		
	}else
	{
		if(0xc0==(para0&0xc0))			//flash lock
		{
			wlSet.m_flaCmd[0] = 0x01;
		}else if(0x80==(para0&0x80))
		{
			wlSet.m_flaCmd[0] = 0x03;
			wlSet.isRear=WLM_ISREAR_REAR;
		}else
		{
			wlSet.m_flaCmd[0] = 0x02;
		}

		wlSet.m_flaCmd[0] <<= 4;
		wlSet.m_flaCmd[0] |= ((tmp1>>4)&0x0f);
		
		wlSet.m_flaCmd[1] = (tmp1&0x0f);
		wlSet.m_flaCmd[1] <<= 4;
		wlSet.m_flaCmd[1] |= ((tmp2>>4)&0x0f);

		wlSet.m_flaCmd[2] = (tmp2&0x0f);
		wlSet.m_flaCmd[2] <<= 4;
		wlSet.m_flaCmd[2] |= ((tmp3>>4)&0x0f);

		wlSet.m_flaCmd[3] = (tmp3&0x0f);
		wlSet.m_flaCmd[3] <<= 4;

		wlSet.m_flaCmd[wlSet.m_cmdIndex-1] &= 0xf0; 
	}


/*
	if(LOW_SPEED==flashPara.fp)
	{

		if(0xc0==(para0&0xc0))			//flash lock
		{
			wlSet.m_flaCmd[0] = 0x01;
		}else if(0x80==(para0&0x80))
		{
			wlSet.m_flaCmd[0] = 0x03;
			wlSet.isRear=WLM_ISREAR_REAR;
		}else
		{
			wlSet.m_flaCmd[0] = 0x02;
		}
		
		wlSet.m_flaCmd[0] <<= 4;
		wlSet.m_flaCmd[0] |= ((tmp1>>4)&0x0f);
		
		wlSet.m_flaCmd[1] = (tmp1&0x0f);
		wlSet.m_flaCmd[1] <<= 4;
		wlSet.m_flaCmd[1] |= ((tmp2>>4)&0x0f);

		wlSet.m_flaCmd[2] = (tmp2&0x0f);
		wlSet.m_flaCmd[2] <<= 4;
		wlSet.m_flaCmd[2] |= ((tmp3>>4)&0x0f);

		wlSet.m_flaCmd[3] = (tmp3&0x0f);
		wlSet.m_flaCmd[3] <<= 4;

		wlSet.m_flaCmd[wlSet.m_cmdIndex-1] &= 0xf0; 
		
	}else 
	{
		//if(cameraSetting.shutter>500)
		//if(cameraSetting.pShutter>0x36)
		if(hpLev)
		{
			if(0xC0==(para0&0XC0))
			{
				wlSet.m_flaCmd[0] = 0xEb;
			}else
			{
				wlSet.m_flaCmd[0] = 0xdb;
			}
		}else
		{
			if(0xC0==(para0&0XC0))
			{
				wlSet.m_flaCmd[0] = 0xE8;
			}else
			{
				wlSet.m_flaCmd[0] = 0xd8;
			}
			//wlSet.m_flaCmd[0] = 0xd8;
		}
		wlSet.m_flaCmd[1]  = tmp1;
		wlSet.m_flaCmd[2]  = tmp2;
		wlSet.m_flaCmd[3]  = tmp3;
	}	

*/

	if(wlSet.m_cmdIndex)
	{
		return 1;
	}else 
	{
		return 0;
	}

	//wlSet.m_cmdIndex = 4;
	//wlSet.m_flaCmd[1] |= ((para1>>4)&0x0f);	
	
}


void Nik_AutoShotSet(unsigned char rcvCmd)
{
	switch(rcvCmd&0xf0)
	{
		case 0xf0:
			nikWireless.WL_output1 = 2;
			nikWireless.WL_output2 = (rcvCmd&0x0f)/5;
			//putchar(1);
		break;

		case 0x80:
			nikWireless.WL_output1 = 4;
			nikWireless.WL_output2 = (rcvCmd&0x0f)/5;
			//putchar(2);
		break;

		case 0x90:
			nikWireless.WL_output1 = 6;
			nikWireless.WL_output2 = (rcvCmd&0x0f)/5;
			//putchar(3);
		break;

		default:
			nikWireless.WL_output1 = 4;
			nikWireless.WL_output2 = (rcvCmd&0x0f)/5;
			//putchar(4);
		break;
		
	}
	
}

#ifdef WIRELESS_MASTER
void WLM_SendChanhead(char chan)
{
	//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
#ifdef WLM_CMD_AD
	//WLM_COMMAND_TRIGER(WLM_HEAD_TIME);
	WlmCmdSet();
	switch(chan)
	{
		case WL_CHANNEL_ONE:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN1_WL1,TIMER_DELAY_SYN);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN1_WL1);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN1_WL2);
		break;

		case WL_CHANNEL_TWO:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN2_WL1,TIMER_DELAY_SYN);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN2_WL1);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN2_WL2);
		break;

		case WL_CHANNEL_THREE:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN3_WL1,TIMER_DELAY_SYN);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN3_WL1);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN3_WL2);
		break;

		case WL_CHANNEL_FOUR:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN4_WL1,TIMER_DELAY_SYN);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN4_WL1);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN4_WL2);
		break;

		default:
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN1_WL1);
			WLM_COMMAND_TRIGER(WLM_HEAD_TIME+WLM_CHAN1_WL2);
		break;
	}

	WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME + WLM_BIT1_INTERVAL+100);
	
#else
	WLM_COMMAND_TRIGER(WLM_HEAD_TIME);

	switch(chan)
	{
		case WL_CHANNEL_ONE:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN1_WL1,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN1_WL1);
		break;

		case WL_CHANNEL_TWO:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN2_WL1,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN2_WL1);
		break;

		case WL_CHANNEL_THREE:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN3_WL1,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN3_WL1);
		break;

		case WL_CHANNEL_FOUR:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN4_WL1,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN4_WL1);
		break;

		default:
			WLM_CMDSEND_DELAY(WLM_CHAN1_WL1);
		break;
	}

	WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);

	switch(chan)
	{
		case WL_CHANNEL_ONE:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN1_WL2,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN1_WL2);
		break;

		case WL_CHANNEL_TWO:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN2_WL2,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN2_WL2);
		break;

		case WL_CHANNEL_THREE:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN3_WL2,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN3_WL2);
		break;

		case WL_CHANNEL_FOUR:
			//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_CHAN4_WL2,TIMER_DELAY_SYN);
			WLM_CMDSEND_DELAY(WLM_CHAN4_WL2);
		break;

		default:
			WLM_CMDSEND_DELAY(WLM_CHAN1_WL2);
		break;
	}

	WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME);
#endif

}
#endif

#ifdef WIRELESS_MASTER
void WLM_SendData(unsigned char *Arr, char sendSize)
{
	//char dataIndex=0;
	//char bitIndex=0;
	unsigned int dataIndex=0;
	unsigned int bitIndex=0;
	unsigned int cmdAcc=0;

	if(wlSet.m_sendCmdEn!=1)
	{
		return;
	}

	//EA = 0;
	disable_irq();
	
	WLM_SendChanhead(wlSet.m_chan);
	while(dataIndex<sendSize)
	{
		while(bitIndex<8)
		{
			if((Arr[dataIndex]>>(7-bitIndex))&0x01)
			{
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_BIT1_INTERVAL,TIMER_DELAY_SYN);
				//WLM_CMDSEND_DELAY(WLM_BIT1_INTERVAL-(dataIndex*40+bitIndex*5));
				//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+(dataIndex*40+bitIndex*5));
				//WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME+(dataIndex*50));
#ifdef WLM_CMD_AD
				WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME + WLM_BIT1_INTERVAL);
#else
				WLM_CMDSEND_DELAY(WLM_BIT1_INTERVAL - cmdAcc*40);
				WLM_COMMAND_TRIGER(WLM_IMPLUS_TIME + cmdAcc*40);
#endif
				cmdAcc = 0;
			}else
			{
				//SET_TIMER7_OVERFLOW(TM7SCALE_HIGH,WLM_BIT0_INTERVAL,TIMER_DELAY_SYN);
				WLM_CMDSEND_DELAY(WLM_BIT0_INTERVAL);
#ifndef WLM_CMD_AD
				cmdAcc++;
#endif
			}
			bitIndex++;
		}
		bitIndex = 0;
		dataIndex++;
	}

	//EA = 1; 
	enable_irq();
}
#endif
