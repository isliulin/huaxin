#ifndef _CBYICANON_
#define _CBYICANON_

#define _CANON_WIRELESS_
#define CANON_RF
#define _MULTIPLY_FLASH_
#define _HP_SYN_
#define RF_MODE_GROUP

#define LIGH_CMD_REBUILD

//#define _1DX_DEBUG_

typedef unsigned int WORD;
typedef unsigned char BYTE;

#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
#define GETBIT(targetbit,n) ((targetbit&(1<<n)))
//#define SETBIT(targetbit,n)	(targetbit|=(n))
#define CLRBIT(targetbit,n)	 (targetbit&=(~(n)))


//indicate the three mode after canon 2012
/*
#define CMD_MODE_OFF			0
#define CMD_MODE_LIGH			1
#define CMD_MODE_RF			2
*/

#define COM_STATUE_START 0x01
#define COM_STATUE_IDLE 0x00
#define COM_STATUE_PREFLASH 0X02
#define COM_STATUE_REALFLASH 0X03
#define COM_STATUE_SLEEP    0X04
#define COM_STATUE_WLMPRE 0X05
#define COM_STATUE_MULTIFLASH 0x06
#define COM_STATUE_MODEL	0x07

#define WL_ARRARY_A			0
#define WL_ARRARY_B 			1
#define WL_ARRARY_C 			2
#define WL_ARRARY_NULL 		3

#define WL_SCHEDULE_ALL		0X01
#define WL_SCHEDULE_RES		0X07
#define WL_SCHEDULE_AB		0X03
#define WL_HPSYN_ON		1
#define WL_HPSYN_OFF		0

#define WLM_MODE_ALL			0
#define WLM_MODE_AB			1
#define WLM_MODE_RES			2

#define WORKINGMODE_TTL 		0
#define WORKINGMODE_MANU 		1
#define WORKINGMODE_MULTI 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC 		5
#define WORKINGMODE_GR		6
#define WORKINGMODE_F1		7
#define WORKINGMODE_F2		8

#define WORKINGMODE_EXTA		4
#define WORKINGMODE_OFF		6

//#define WORKINGMODE_OFF		0xff
#define WORKINGMODE_AUTO		2
#define WORKINGMODE_TURNOFF	3

#define WIRELESS_MASTER_ON 1
#define MST_MODE_RADIO		0
#define MST_MODE_LIGHT		1

#define WLM_ON 			1
#define WLM_OFF 		0

#define WLM_CMDSEND_PREFLA_ALL	3
#define WLM_CMDSEND_PREFLA_CHA	4
#define WLM_CMDSEND_PREFLA_CHB	5
#define WLM_CMDSEND_PREFLA_CHC	6
#define WLM_CMDSEND_FLAIMF		7

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

#define WLM_CMDFLASH_CONFIRM		1
#define WLM_CMDFLASH_DENY			0

//#define WLM_CMDPROC_ON 1
#define WLM_CMDPROC_OFF 0
#define WLM_CMDPROC_DAT 1
#define WLM_CMDPROC_FLA 2
#define WLM_CMDPROC_PRF 3	

#ifdef _HP_SYN_
#define HP_PREFLASH_INTERVAL	150
#define HP_REALFLA_INTERVAL	2200
#define HP_FLASHMODE_ON		1
#define HP_FLASHMODE_OFF		0
#endif

#define RADIO_STATUE_PRE			1
#define RADIO_STATUE_FLACFM		2
#define RADIO_STATUE_FLASH		3
#define RADIO_STATUE_RECOVER		4
#define RADIO_STATUE_PRECFM		5
#define RADIO_STATUE_MODEL		6
#define RADIO_STATUE_PRE_FLA		7
#define RADIO_STATUE_PRE_EX		8//7
#define RADIO_STATUE_FLAINFO		12

#define SYNMODE_HEAD 0
#define SYNMODE_REAR 1
#define SYNMODE_HP	   2

#define MOTO_MODE_AUTO 0x00
#define MOTO_MODE_MANU 0x01

#define WLM_MASTON_BIT	1
#define WLM_WLSWTH_BIT	0

#define WLM_MASTER_ON 		1
#define WLM_MASTER_OFF 		0

#define RF_INFOBIT_MODE	0
#define RF_INFOBIT_OUTPUT	1

#define DSP_UPDATE_MODE	0x0001
#define DSP_UPDATE_MOTO	0x0002
#define DSP_UPDATE_RFCHAN		0x0004
#define DSP_UPDATE_LINK	0x0008
#define DSP_UPDATE_LKDEV	0x0010
#define DSP_UPDATE_RFSW	0x0020
#define DSP_UPDATE_SHOT	0x0040
#define DSP_UPDATE_CMDMOD 0x0080
#define DSP_SLACMD_TEST	0x0100
#define DSP_SLACMD_MODEL	0x0200
#define DSP_SLACMD_REL	       0x0400
#define DSP_UPDATE_RFID      0x0800
#define DSP_UPDATE_LINKM2S  0x1000
#define DSP_UPDATE_UARTINT 0x2000
#define DSP_UPDATE_PWON     0x4000
#define DSP_UPDATE_RFINFO     0x8000
#ifdef _1DX_DEBUG_
#define DSP_UPDATE_DEBUG	0x2000
#endif
//#define DSP_UPDATE_MULCAL	0x0010	

#define RF_OFF				0
#define RF_SLAVE			1
#define RF_MASTER			2
#define RF_LKSHOT_MASTER	3
#define RF_LKSHOT_SLAVE	4



typedef struct arrary_para
{
	unsigned char isMasterOn;
	unsigned char arrMode;
	unsigned char ABRation;
#ifdef CANON_RF
	unsigned char otpCode[3];
#else 
	unsigned char otpCode[5];
#endif
	//unsigned char AotpCode;
	//unsigned char BotpCode;
	//unsigned char CotpCode;
	unsigned char Aeps;
	unsigned char Beps;
	unsigned char Ceps;
	unsigned char Acmp;
	unsigned char Bcmp;
	unsigned char Ccmp;
	unsigned char ResArrCcmp;
}ArrPara;

/*
typedef struct RADIO_SETTING
{
	//unsigned char ratio;				//0==> All	1==> A:B		2==>A:B C
	unsigned char cmdReady;
	unsigned char cmdSta;
	unsigned char devLinked;
	//unsigned char arrExpVal[5];	
	//unsigned char grMods[RF_GROUP_TOTAL];
	unsigned char chan;
	unsigned int id;
}RadioSet;
*/

/*
#define RF_GROUP_TOTAL	5
typedef struct RFGROUP_ARR_PARA
{
	char mode;
	char output1;
	char output2;
	unsigned char optCode;
	unsigned char ttlExpVal;
	char cmp1;
	char cmp2;
	char mulOtp;
}RfGroup_ArrPara;
*/

//RfGroup_ArrPara  rfGr_arrPara[RF_GROUP_TOTAL]={0};
/*
typedef struct RFGROUP_PARA
{
	unsigned char isoCmp;
	unsigned char shutter;
	unsigned char preflashIndex;
	unsigned char rfGrInfoFlag[5];					//uart info flag
	RfGroup_ArrPara  rfGr_arrPara[RF_GROUP_TOTAL];
}RfGroupPara;
*/

//extern RfGroupPara rfGrPara;

#if 0
typedef struct	 vip_para
{
	char VIP_workmode;
	char VIP_output1;
	char VIP_output2;
	char TTL_LEVEL1;
	char TTL_LEVEL2;
//	char VIP_fbmode;
	char pre_flash_count;
	char statue_sleep;
	char VIP_lowBatWarn;
	char commu_flag;
	char SyncMode;		//0:front  1:back  2:fast
	char outputChangeCode;
	char flashCompensation;
//	char flash_surrand;
	char flash_FEB1;
	char flash_FEB2;
	unsigned char surrand_state;
	char VIP_ttlcomp1;
	char VIP_ttlcomp2;
	char camera_comp1;
	char camera_comp2;
	unsigned char s2_count;
	char full_flash;
	int VIP_ttlTime;
	int quickTime;	
	//unsigned char focLen; 	
	//unsigned char moto_location;
	//unsigned char moto_mode;
	unsigned char apert;
	unsigned char hpFlash;
	
}VIP_PARA;
#endif

/*
typedef struct MOTO_SET
{
	unsigned char focLen;
	unsigned char moto_location;
	unsigned char moto_mode;
}MotoSet;
*/

typedef struct WLM_AB_RATION
{
	unsigned char para1;
	unsigned char para2;
}WLM_abRation_fillwords;



typedef struct MULTI_Set
{
	char *expose;
	int  *eTimes;
	int *frequence;
	int finalExpTimes;
	unsigned char cmdCode;
	unsigned char mulCtl;
	//unsigned char WlmMul[3];
	//unsigned char *wlmMulExp;
//	unsigned char frqMode;
	int lowFrqCnt;
	unsigned char sCode;
	unsigned char maxTimes;
//	unsigned char cMode;
}MultiSet;


#define WLM_IMPLUS_TIME		38
#define WLM_CHAN1_WIDTH		(45+20)//350//500//5
#define WLM_CHAN2_WIDTH		(90+20)//520//1300//9
#define WLM_CHAN3_WIDTH		(140+20)//830//1700//16
#define WLM_CHAN4_WIDTH		(200+10)//1100//2200//22
#define WLM_BIT1_INTERVAL  		(62)//(37+20)//600//5//600
#define WLM_BIT0_INTERVAL		(100)

#define WLM_BYTE_DELAY			(50)



#endif


