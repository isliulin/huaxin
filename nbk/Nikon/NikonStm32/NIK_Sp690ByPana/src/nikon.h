#ifndef _NIKON_H_
#define _NIKON_H_

//#define TTL_DEBUG
#define TTL_DEBUG_FULLTIME
#define NIKON_D50
#define NIKON_WIRELESS_VERSION
//#define WIRELESS_MEASURE
//#define WIRELESS_DEBUG
#define WIRELESS_MASTER
#define NIKON_PARA
#define UART_SENDMODE_ASYC
#define _MULTIPLY_FLASH_
#define _NIKON_WIRELESS_
#define UART_SENDDATA_CONTINUE
//#define NIKCOM_TEST
#define MASTER_AUTO_ON						//this indicate the master mode whether include the auto mode
//#define CANON320EX_COPY						//

#define AUX_MODE_OFF	3
#define AUX_MODE_LED	0
#define AUX_MODE_FLA	1
#define AUX_MODE_AUTO	2

#define _SP330_ 		1
#define _SP690II_ 	2
#define _SP700_ 		3

//#define CANON_TTL_DEBUG
//#define WIRELESS_DEBUG
#define NEW_DSP_SCHEME _SP700_

#ifdef WIRELESS_MEASURE
#define WLBUF_LEN 45
#else
#define WLBUF_LEN 8///4
#define WLMBUF_LEN 4
#endif

#define HIGH_SPEED	1
#define LOW_SPEED	0
#define HIGH_SPEED_LEVEL1 1
#define HIGH_SPEED_LEVEL2 2


//#define WORD unsigned it
//#define BYTE unsigned char

typedef unsigned int WORD;
typedef unsigned char BYTE;

#define TM7SCALE_LOW	0
#define TM7SCALE_HIGH	1

#define xdata 
#define code const


#ifdef NIKON_PARA

#define AF_EN_TEST
#define AF_EN PAOUT7

#define HP_SYN_MODE_NO	0
#define HP_SYN_MODE_SW	1
#define HP_SYN_MODE_HW	2

#define TRIGER_MODE_TIME	0
#define TRIGER_MODE_SW		1
#define TRIGER_MODE_HW	2

//#ifdef CANON320EX_COPY
#if(NEW_DSP_SCHEME==_SP330_)
#define IGBT_CONTROL PAOUT1
#define POWER P0OUT2//P2OUT2
#define FLASH_TRIGER P0OUT1
#define LED_LITE	P2OUT5
#define CHARGE_ENABLE PAOUT4
#define FLASH_PWM	PAOUT6
#define WLM_CMD_AD
//#define HIGH_SPEED_SYNCHRONIC
#define _HP_SYN_
#define _ADD_COIL_
#define EXT_LED
#define _HP_SYN_MODE_	  HP_SYN_MODE_SW
#define _TRIGER_MODE_	  TRIGER_MODE_SW
#define WAKE_UP      	P2OUT3
#define WAKE_UP_IN  P2IN3
#define SET_WAKEUP_OUT {P2DIR|=0x08;} 
#define SET_WAKEUP_IN    {P2DIR&=0xF7;}
#define FLASH_TRIGER_TIME		200
#define HIGH_SPEED_OVERFLOW 2000
#elif(NEW_DSP_SCHEME==_SP690II_)
#define MOTO_VER
#define IGBT_CONTROL P0OUT0
#define POWER PAOUT5
#define FLASH_TRIGER PAOUT6
#define EXTERN_CHARGE P2OUT5
#define CHARGE_ENABLE PAOUT0
#define _HP_SYN_MODE_	HP_SYN_MODE_NO
#define _TRIGER_MODE_	TRIGER_MODE_TIME
#define FLASH_TRIGER_TIME		200
#elif(NEW_DSP_SCHEME==_SP700_)
#define MOTO_VER
#define EXT_CHARGE
#define WLM_CMD_HWC
#define _HP_SYN_
#define WLM_CMD_AD
#define _HP_SYN_MODE_	HP_SYN_MODE_HW
#define _TRIGER_MODE_	TRIGER_MODE_HW
#define EXTERN_CHARGE 	P2OUT5
#define IGBT_CONTROL 	P0OUT0
#define POWER 			PAOUT5//P2OUT2
#define FLASH_TRIGER 	P2OUT6//P0OUT1
#define CHARGE_ENABLE 	PAOUT0//PAOUT4
#define FLASH_PWM		PAOUT6
//#define FLASH_READY 	P0OUT0
#define AF_INPUT		PAIN7
#define HC_CON2			PAOUT4
#define HC_CON3			P3OUT0
#define HC_CON4			P0OUT2//PAOUT4
#define HP_I2C_SCK		P0OUT4
#define HP_I2C_SDA		P0OUT3
#define HP_I2C_DAIN		P0IN4
#define _HP_SYN_MODE_	HP_SYN_MODE_HW
#define WAKE_UP      		P2OUT3//P2OUT0
#define WAKE_UP_IN  	P2IN3//P2IN0
#define HP_MNT_PIN		P2IN3
#define SET_WAKEUP_OUT {P2DIR|=0x08;}//{P2DIR|=0x01;} 
#define SET_WAKEUP_IN    {P2DIR&=0xF7;}//{P2DIR&=0xFE;}
#define FLASH_TRIGER_TIME		400
#define HIGH_SPEED_OVERFLOW 0xFFFF
#endif

#ifdef _HP_SYN_
#if(_HP_SYN_MODE_==HP_SYN_MODE_SW)
#define HP_PREFLASH_INTERVAL	150
#define HP_REALFLA_INTERVAL	2900//2200
#define WLHP_LEVEL1				2000
#define WLHP_LEVEL2				2500
#else
#define HP_PREFLASH_INTERVAL	4500//450
#define HP_REALFLA_INTERVAL	0xffff//65500//9000//2200
#define WLHP_LEVEL1				0xFFFF
#define WLHP_LEVEL2				0xFFFF
#endif
#define HP_FLASHMODE_ON		1
#define HP_FLASHMODE_OFF		0
#endif

//#define POWER PAOUT5
//#define CHARGE_ENABLE PAOUT0
//#define FLASH_OUT P0OUT6

//#define FLASH_TRIGER PAOUT6
//#define WAKE_UP      	P2OUT3
//#define WAKE_UP_IN  P2IN3

//#define TEST_OUT	 PAOUT0
#define WIRELESS_IN		P2IN2
//#define WEAKUP_PIN	P2OUT3

#define NIKONSCK 	P0OUT7//P0OUT3
#define NIKONSCKIN	P0IN7
#define NIKONSCKI	P0IN7//P0IN3
#define NIKONMOSI 	P0OUT6//P0OUT2
#define NIKONMISO 	P0IN6//P0IN2
#define NIKONNSSI 	P0IN5//P0IN4
#define NIKONNSSO 	P0OUT5//P0OUT4
#define AFCTL		AF_EN
#define FLASHCTL 	P2IN1
//#define EXTERN_CHARGE P2OUT5
#endif

#define MULTI_FLASH_ENABLE		1
#define MULTI_FLASH_DISABLE	0

#define WL_RCVTIME_OUT 220
#define WL_PF_JUDGE    75

#define TIMER_DELAY_SYN 	1
#define TIMER_DELAY_ASYN	0

#define SET_TIMER_STOP {TM7MD1 &= 0XEF;}

#define WAIT_SIGNAL_INTERVAL 	40000//40000//0X56B8
#define PREFLASH_2_INTERVAL	900
#define PREFLASH_1_INTERVAL	600
#define MS_INTERVAL				10000
#define TENUS_INTERVAL			99
#define AF_FLASH_INTERVAL		300

#define MAX_RCV_LEN 20

#define TEST_PILOT_ON	1
#define TEST_PILOT_OFF	0

//#define SET_WAKEUP_OUT {P2DIR|=0x08;} 
//#define SET_WAKEUP_IN    {P2DIR&=0xF7;}

#define ERR 0XFF
//#define	MAX_UART_DATA_LEN	4
#define	MAX_UART_DATA_LEN	5
#define UART_QUE_LEN		32
#define CHARGE_OK_LEVEL  	0xA3		 //2.1V ADC_RES = 256 * VIN/VCC ==> VIN = VCC*ADC_RES / 256
#define CHARGE_WLM_LEVEL	0xBD//0xB5		
#define TEMPER_WARNING_LEVEL 0xFF
#define CHARGE_OK           1
#define CHARGE_UNDER		0
#define CHARGE_FULL		2

/*****************************************
	AD SIMPLE
*****************************************/
#define VOL_MAX		   2000		  //260V
#define VOL_AD_FLALEVEL	165//180//160 //178//155//190 //200//190 //178//155//620
//#define VOL_MIN		   2000
#define VOL_FULL	   2451
#define VOL_AD_FULL   		190//180//200//180//190//178 //200//210//200//178//713

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

									 
#define GETBIT(targetbit,n) ((targetbit>>n)&0x01)
#define SETBIT(targetbit,n)	(targetbit|=(0x01<<n))
#define CLRBIT(targetbit,n)	 (targetbit&=(~(0x01<<n)))

#define WLM_SETARR_OPPARA(x,y) (0x2a-((x*3+y)*2-6))

#define START_TIMER0		{TM0MD |= 0x08;}
#define STOP_TIMER0			{TM0MD &= ~0x08;}
#define UPDATA_TIMER0(x)	{STOP_TIMER0;TM0OC=(x-1);START_TIMER0}
#define CLR_WLN_BUF {nikWireless.WL_buf[0]=nikWireless.WL_buf[1]=nikWireless.WL_buf[2]=nikWireless.WL_buf[3]=nikWireless.WL_buf[4]=nikWireless.WL_buf[5]=nikWireless.WL_buf[6]=nikWireless.WL_buf[7]=0;nikWireless.WLBufIndex=0;}
#define STOP_TIMER7		{TM7MD1 &= 0XEF;}

//#ifdef CANON320EX_COPY
//#if(NEW_DSP_SCHEME==_SP330_)
#ifdef EXT_LED
#define TURNOFF_FOLLOW_INDEX {LED_LITE=0;}//{AFCTL = 0;}
#else
#define TURNOFF_FOLLOW_INDEX {AFCTL = 0;}//{LED_LITE=0;}
#endif

#ifdef _NIKON_WIRELESS_
	#define SET_WL_FLASH_TIMEOUT {SETBIT(bitFlag,BIT_BITFLAG_WLFLASHTO);nikWireless.WL_timeout=0;TURNOFF_FOLLOW_INDEX/*AFCTL = 0;*/}
	#define CLR_WL_FLASH_TIMEOUT {CLRBIT(bitFlag,BIT_BITFLAG_WLFLASHTO);nikWireless.WL_timeout=0;}
#endif

//unsigned char bitChar
#define BIT_BITCHAR_S2SETFLAG 0
#define BIT_BITCHAR_MOTOAUTO 1
#define BIT_BITCHAR_PRE1EXPOSE 2
#define BIT_BITCHAR_PRE2EXPOSE 3
#define BIT_BITCHAR_LOWBATWARN 4
#define BIT_BITCHAR_REDEYEFLAG 5
#define BIT_BITCHAR_ONETIMEFLASH 6
#define BIT_BITCHAR_DEBUGERROR	7

#define BIT_BITFLAG_WLTIMEOUT 0
#define BIT_BITFLAG_WLFLASHTO 1
#define BIT_BITFLAG_FLASHLOCK  2

#define BIT_bitIndex_PROTECT 		 		0

#ifdef UART_SENDDATA_CONTINUE
#define BIT_bitIndex_CHARGE 		 		1
#define BIT_bitIndex_CHARGEFULL  			    2
#else
#define BIT_bitIndex_CHARGE 		 		1
#define BIT_bitIndex_CHARGEFULL  			2
#endif
#define BIT_bitIndex_PREFLASH  				3
#define BIT_bitIndex_REALFLASH 				4
#define BIT_bitIndex_S2COUNT 				5
#define BIT_bitIndex_PILOTFLAG  			6
#define BIT_bitIndex_TTLTIMECALC  			7

#define CAMERA_MODE_REAR	0//7			the little endian ande big endian concerned the rear is different
#define CAMERA_MODE_REDEYE  6
#define CAMERA_MODE_LED		5

#define RED_EYE_START		0x80//0x01		little endian and big endian
#define RED_EYE_FINISH		0x00

#define NIKWL_MODE_TTL  0
#define NIKWL_MODE_MANU 1

#define NIKWL_STATUE_HOLD 0
#define NIKWL_STATUE_ENSURECHAN 1
#define NIKWL_STATUE_RUN	2
#define NIKWL_STATUE_START 3
#define NIKWL_STATUE_STOP  4
#define NIKWL_STATUE_PLUSMEASURE 5
#define NIKWL_STATUE_PREFLASH	6
#define NIKWL_STATUE_FLASH		7

#define WLN_WIZARD_PRE	1
#define WLN_WIZARD_FLA	2
#define WLN_WIZARD_HOLD	0

#define WLN_PFCOUNT_RDY	0X00
#define WLN_PFCOUNT_ONE 	0x01
#define WLN_PFCOUNT_TWO 	0x02

#define WL_FLASH_STATUE_HOLD  	0	//before receive light signal
#define WL_FLASH_STATUE_START 	1	//after ensure the channel
#define WL_FLASH_STATUE_BFRPF	2	//before take previous flash
#define WL_FLASH_STATUE_AFTPF 	3	//before take previous flash
#define WL_FLASH_STATUE_FLASH	4	//before take real flash

#define WL_CHANNEL_UNSURE 	0
#define WL_CHANNEL_ONE	 	1
#define WL_CHANNEL_TWO	  	2
#define WL_CHANNEL_THREE  		3
#define WL_CHANNEL_FOUR		4
#define WL_CHANNEL_MAKESURE 	5

#define WL_ARRARY_M	0
#define WL_ARRARY_A	1
#define WL_ARRARY_B 	2
#define WL_ARRARY_C 	3
#define WL_ARRARY_NULL 4

#define WLM_ARRAY_M	0
#define WLM_ARRAY_A	1
#define WLM_ARRAY_B	2
#define WLM_ARRAY_C	3

#define WL_WIZARD_PRE	1
#define WL_WIZARD_FLA	2
#define WL_WIZARD_HOLD	0

#define MULTI_MODE_CAM		1
#define MULTI_MODE_UNCAM	0

#define CANWL_STATUE_HOLD 0
#define CANWL_STATUE_ENSURECHAN 1
#define CANWL_STATUE_RUN	2
#define CANWL_STATUE_START 3
#define CANWL_STATUE_STOP  4
#define CANWL_STATUE_PLUSMEASURE 5
#define CANWL_STATUE_PREFLASH	6
#define CANWL_STATUE_FLASH		7

#define WL_CHAN_SCALE1_GND			140//164//210//230
#define WL_CHAN_SCALE1_ROOF			230//340//300

#define WL_CHAN_SCALE2_GND			230//250//370//380
#define WL_CHAN_SCALE2_ROOF			339//500//450

#define WL_CHAN_SCALE3_GND			230//340//350
#define WL_CHAN_SCALE3_ROOF			339//500//420

#define WL_CHAN_SCALE4_GND			380//407//600//630
#define WL_CHAN_SCALE4_ROOF			488//720//700

#define WL_MASTER_PREFLA_WIDTH		4000//0x1800


#define CHANL_CONDITION_1(x)	((WL_CHAN_SCALE1_GND<x)&&(x<WL_CHAN_SCALE1_ROOF))
#define CHANL_CONDITION_2(x)	((WL_CHAN_SCALE2_GND<x)&&(x<WL_CHAN_SCALE2_ROOF))
#define CHANL_CONDITION_3(x)	((WL_CHAN_SCALE3_GND<x)&&(x<WL_CHAN_SCALE3_ROOF))
#define CHANL_CONDITION_4(x)	((WL_CHAN_SCALE4_GND<x)&&(x<WL_CHAN_SCALE4_ROOF))


#define BIT_bitWireless_mode			0
#define BIT_bitWireless_TtlExp		1
#define BIT_bitWireless_Moutp		2
#define BIT_bitWireless_chanAndArr	3

//#define WL_BITJUDGE_CODE0	450
//#define WL_BITJUDGE_CODE1	750
//#define WL_BITJUDGE_CODE2	1050
//#define WL_BITJUDGE_CODE3	1300
//#define WL_BITJUDGE_CODE4	1550
//#define WL_BITJUDGE_CODE5	1800
//#define WL_BITJUDGE_CODE6	2050
//#define WL_BITJUDGE_CODE7   2300

//#define WL_BITJUDGE_CODE0	450
//#define WL_BITJUDGE_CODE1	750
//#define WL_BITJUDGE_CODE2	1050
//#define WL_BITJUDGE_CODE3	1300
//#define WL_BITJUDGE_CODE4	1550
//#define WL_BITJUDGE_CODE5	1800
//#define WL_BITJUDGE_CODE6	2050
//#define WL_BITJUDGE_CODE7   2300
//#define WL_BITJUDGE_CODE8	2543

#define WL_PREWIDTH_GND	200//360
#define WL_PREWIDTH_ROOF 	400//460

//#define BLFLAG_START 0x60
//#define BLFLAG_PRE1 0X90
//#define BLFLAG_PRE2 0XA0
//#define BLFLAG_PRE3 0XB0
//#define BLFLAG_READY 0X20

#define BLFLAG_START 0x60
#define BLFLAG_PRE1 0X90
#define BLFLAG_PRE2 0XA0
#define BLFLAG_PRE3 0XB0
#define BLFLAG_READY_FRONT 0X20
#define BLFLAG_READY_REAR	0X30
#define PREFLASH_LOCK		0X10
#define HPSYN_FLASH_LOCK1	0xEB
#define HPSYN_FLASH_LOCK2	0xE8

#define PREFLASH_LOCK_ON		1
#define PREFLASH_LOCK_OFF		0

#define WL_PFCOUNT_RDY 0X00
#define WL_PFCOUNT_ONE 0x01
#define WL_PFCOUNT_TWO 0x02

#define HEAD_DATA_UNRCVD	 0
#define HEAD_DATA_RECEIVED 1

#define DATA_SEND_SUCESS 		0
#define DATA_SEND_TIMEOUT		1

#define READY_SIGNAL_TIMEOUT   1
#define READY_SIGNAL_ARRIVAL    0

#define SET_NSS_OUTPUT		{	P0DIR |= 0X20;	}
#define SET_NSS_INPUT		{	P0DIR &= 0XDF;	}
#define ENABLE_CLKSYN		{    SC4MD1 |= 0X30;	}
#define DISABLE_CLKSYN		{    SC4MD1 &= 0xCF;   }
#define SET_SBO_OUT			{	P0DIR |= 0X40;	}
#define SET_SBO_IN			{	P0DIR &= 0XBF;	}
#define RELEASE_SBT			{	SC4MD1	&= 0XBF;	}
#define LINKON_SBT			{	SC4MD1 |= 0X40;	}
#define SET_SC4SBIS_IN		{	SC4MD1|= 0X20;	}
#define SET_SBT_OUT			{	P0DIR |= 0X80;	}
#define SET_SBT_IN			{	P0DIR &= 0X7F;	}

#define SET_FOLLOWFLASH_OPEN   {disable_irq();MEMCTR    |=  0x04;	IRQ2ICR = 0X82;	IRQ1ICR = 0x00; MEMCTR   &= ~0x04;enable_irq();}
#define SET_FOLLOWFLASH_CLOSE  {disable_irq();MEMCTR    |=  0x04;	IRQ2ICR = 0X80;	IRQ1ICR = 0x82; MEMCTR   &= ~0x04;enable_irq();}

#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 

#define err 1
#define SLEEP_MODE_ON		1
#define SLEEP_MODE_OFF		0

#define F2FLASH_LABEL_SET	1
#define F2FLASH_LABEL_CLR	0
									
typedef struct EXPOSEPARA
{
	char workmode;
	char output1;
	char output2;
	char TTL_LEVEL1;
	char TTL_LEVEL2;
	char hp_ttl1;
	char hp_ttl2;
	char SyncMode;		//0:front  1:back  2:fast
	char ttlcomp1;
	char ttlcomp2;
	char camera_comp1;
	char camera_comp2;	
	char ttlFeb1;
	char ttlFeb2;
	char surrand_state;
	char ExposeData;
	char ExposeState;
	char music_set;
	char moto_mode;
	char moto_location;
	char sleep_flag;
	char fp;
	char auxLiteSel;
	char faceLite;
	char rmtIndex;
	char ledFlag;
	char rmtShut;
}EXPOSE_PARA;

typedef struct CAMERASETTING
{
	char ISO;
	unsigned int shutter;
	unsigned char pShutter;
	char aperture;
	char focLen;
	char cls;
	char rcvCode;
	char camera_cmp1;
	char camera_cmp2;
	char rcvHandShake;
	unsigned char wl_chan;
	char wl_arrary;
}CAMER_SETTING;

typedef struct FLASHSTATUE
{
	 char flash_enable;
	 char camera_state;					//0 single              1 CLS 
	 char sleep;
	 char full_flash;
	 char s2_count;
	 char auxr_light;
	 char redEye_ctr;
}FLASH_STATUE;

typedef struct UARTSEND
{
	unsigned char sendFlag;
	unsigned char sendFlagEx;	
	char chargeOK;
	int	ISO;
	unsigned char pIso;
	unsigned char ISO_index;
	unsigned int apert; 					//sendFlag:	0 chargeOK	1 ISO  2 apert 3 temperature alarm
	unsigned char pApert;
	char tempAlarm;
	char cls;
	char cls_invalid;
	char pFocLen;
	char focLen;
	char debug;
	unsigned char mode;
}UART_SEND;

#define WLM_ARRA_MODE 0
#define WLM_ARRB_MODE 1
#define WLM_ARRC_MODE 2
#define WLM_FLASH_OFF 	0
#define WLM_FLASH_ON	1

#define WLM_ISREAR_FRONT 0
#define WLM_ISREAR_REAR   1
typedef struct WL_ARR_PARA
{
	char mode;
	char output1;
	char output2;
	char cmp1;
	char cmp2;
}WL_ArrPara;

typedef struct WIRELESS
{
	unsigned char s_chan;
	unsigned char s_arrary;
	unsigned char s_wlMode;
	unsigned char s_flashStatue;
#ifdef WIRELESS_MASTER
	unsigned char m_chan;
	//unsigned char chan;
	unsigned char m_array;
	unsigned char m_op[WLMBUF_LEN];
	unsigned char m_flaOn;
	//unsigned char m_ma;
	//unsigned char m_mb;
	//unsigned char m_mc;
	unsigned char m_arrMode[4];
	//unsigned char m_charge_statue;
	unsigned char m_sendCmdEn;
	unsigned char isRear;
//	unsigned char m_slpara[3];
	unsigned char m_slpara[4];
//	WL_ArrPara  m_arrPara[3];
	WL_ArrPara m_arrPara[4];
	unsigned char m_flaCmd[8];
	char m_cmdIndex;
	unsigned int testCnt;
	unsigned char testPilot;
#endif
}Wl_set;


typedef struct NIKON_WIRELESS
{
	unsigned char WL_mode;
	unsigned char WL_opCode;
	unsigned char WL_output1;
	unsigned char WL_output2;
	unsigned char WL_Ttl;
	unsigned char WL_statue;
	unsigned int WL_timeout;
	unsigned char RearReserve;
#ifdef WIRELESS_MEASURE
	unsigned int WL_buf[WLBUF_LEN];
#else
	unsigned char WL_buf[WLBUF_LEN];
#endif
	unsigned char WLBufIndex;
	unsigned int PCA_timer_record;
	unsigned char WL_Channel;
	unsigned char bitLoadIndex;
	unsigned char preArr;
	unsigned char preTotal;
	unsigned char preTurn;
	unsigned char flash_wizard;
#ifdef WIRELESS_DEBUG
	unsigned char wl_debug[4];
#endif
	unsigned char pilotFalg;
	unsigned char preflashLock;
	unsigned char preflashLockModeReserve[4];
	unsigned char fp;
	
}NiWireless;

#define MULTI_STFLAG_START 1
#define MULTI_STFLAG_HOLD	 0
typedef struct MULTI_Set
{
	unsigned char expose;
	int  eTimes;
	int frequence;
	int finalExpTimes;
	unsigned char frqMode;
	int lowFrqCnt;
	unsigned char cMode;
	int mulCnt;
	unsigned char startFlag;
	unsigned char mulCtl;
}MultiSet;

#if(_HP_SYN_MODE_==HP_SYN_MODE_SW)
typedef struct HP_SETTING
{
	unsigned int head;
	unsigned int dt;
	unsigned int val;
	unsigned int step;
	unsigned int adVal;
	unsigned int coef;
	unsigned int startVal;
	unsigned int realVal;
	unsigned int sdt;
	unsigned int hDelay;
	unsigned int initLev;
	unsigned char accFlag;
	int mvScope;
	
}HpSetting;
#else
typedef unsigned int HpSetting;
#endif

//keypad handle
//#define WORKINGMODE_TTL 0
//#define WORKINGMODE_MANU 1
//#define WORKINGMODE_MULTI 2
//#define WORKINGMODE_S1 (2+1)
//#define WORKINGMODE_S2 (3+1)
//#define WORKINGMODE_SIGN 0xcc
//#define WORKINGMODE_TURNOFF 5


//#ifdef MASTER_AUTO_ON
//#define WORKINGMODE_HOLD 3
//#define WORKINGMODE_AUTO  2
//#else
//#define WORKINGMODE_HOLD 2
//#define WORKINGMODE_AUTO  2
//#endif

#define WORKINGMODE_TTL 		0
#define WORKINGMODE_MANU 		1
#define WORKINGMODE_MULTI 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC 		5
#define WORKINGMODE_F1		6
#define WORKINGMODE_F2		7
#define WORKINGMODE_OFF		0xff
#define WORKINGMODE_AUTO		2

#define WORKINGMODE_TURNOFF   3
#define WORKINGMODE_HOLD 		3




//flash state 
#define FLASH_STATE_CHARGEOK 0x01
#define FLASH_STATE_INCHARGE 0xfe

#define FLASH_STATE_TEMPALARM 0x2


//received data location
#define RCVDAT_LOCATION_APERTURE 0x05
#define RCVDAT_LOCATION_ISO 0x03
#define RCVDAT_LOCATION_FOCLEN 0x06
#define RCVDAT_LOCATION_MODE 0x01
#define RCVDAT_LOCATION_EXPOSE 0x01
#define RCVDAT_LOCATION_CAMERACMP 0x07
#define RCVDAT_LOCATION_SHUTTER	0x04

#define LOCATION_PARAMETER_MODE 0x01
#define LOCATION_PARAMETER_SIX 0x05
#define LOCATION_PARAMETER_REAR 21
#define LOCATION_PARAMETER_ZERO 0
#define LOCATION_PARAMETER_CMP  5
#define LOCATION_PARAMETER_OUTPUT 5
#define LOCATION_PARAMETER_MULTIMES 11
#define LOCATION_PARAMETER_MULFRQ	10



#define WLM_LOC_MASTSET	1
#define WLM_LOC_MASTPAR	5
#define WLM_LOC_MSL_A		16
#define WLM_LOC_MSL_BC		17
#define WLM_LOC_SLPARA_A 	18
#define WLM_LOC_SLPARA_B 	19
#define WLM_LOC_SLPARA_C 	20


#define WLM_SLAMODE_MANU 		0X06
#define WLM_SLAMODE_TTL		0X01
#define WLM_SLAMODE_AUTO 		0X03
#define WLM_SLAMODE_TURNOFF 	0X00


#define PARAMETER_MODE_MANU 0x06
#define PARAMETER_MODE_TTLS 0x02
#define PARAMETER_MODE_TTLA 0x01
#define PARAMETER_MODE_MULTI 0x07

#define WL_REARFLASH_COMMAND 0x70
#define WL_REARFLASH_COMMANDex 0x78
#define WL_PILOT_CMD 0xf1



void INT_System_init(void);
char DATA_Receive(char flag,unsigned char *pRcv);
//unsigned char DATA_Receive(char flag);
//void PARA_Send (char SPI_byte);
char LIT_PARA_Send (unsigned char *SPI_byte,char size);
char LIT_DATA_Send(unsigned char dat);
void SpiInterval();
void Delay (void);
void Delay_ms ();
void RealFlash();
void HaltSec(int tick);
void HaltInterval(int tick);
void Delay(void);
void LoadUartDate();
void TURN_ON_TRIGER(unsigned int time);
//extern void BuildWlmFlaCode();
extern unsigned char BuildWlmFlaCode();
extern int OutputSwitch(unsigned char mode);
extern void SET_TIMER7_OVERFLOW(unsigned scaleSel,unsigned int tm, char mode);
extern void WLM_COMMAND_TRIGER(unsigned int time);
extern void SetTimerLinked(unsigned int st);

extern unsigned char xdata send_arrary2[];

extern void Delay_10us();

#ifdef WIRELESS_MASTER		/*20us*/

//#ifdef  CANON320EX_COPY
//#if(NEW_DSP_SCHEME==_SP330_)
#if(_TRIGER_MODE_==TRIGER_MODE_SW)

#define WLM_HEAD_TIME		400

#ifdef WLM_CMD_AD
#define WLM_IMPLUS_TIME	400
#define WLM_BIT1_INTERVAL	700
#else
#define WLM_IMPLUS_TIME	120//180
#define WLM_BIT1_INTERVAL	700//750//900//800//950//1020
#endif


#define WLM_CHAN1_WL1		900//740//750//950//950//900//750//800//930//1100//0xff35//(0xff35-0x10)//(0xff35)//(0xff35+0x28)//(0xff35-0x28)
#define WLM_CHAN2_WL1		1600//1420//1450//1650//1650//1600//1450//1500//1670//1850//0xfeab//(0xfeab-0x10)//(0xfeab)//(0xfeab+0x28)//(0xfeab-0x28)
#define WLM_CHAN3_WL1		1800//1420//1650//1850//1850//1800//1450//1500//1670//2010//(0xfe8c)//(0xfe8c)//(0xfe8c+0x28)//(0xfeb4-0x28)
#define WLM_CHAN4_WL1		900//740//750//950//950//900//800//850//930//1100//0xff35//(0xff35-0x10)//(0xff35)//(0xff35+0x28)//(0xff35-0x28)

#define WLM_CHAN1_WL2		1600//1360//1800//1870//1600//1450//1500//1650//1810//0xfeb2//(0xfeb2-0x10)//(0xfeb2)//(0xfeb2+0x28)//(0xfeb2-0x28)
#define WLM_CHAN2_WL2		900//740//1100//1170//900//800//850//930//1150//0xff2c//(0xff2c-0x10)//(0xff2c)//(0xff2c+0x28)//(0xff2c-0x28)
#define WLM_CHAN3_WL2		1800//1420//2000//2070//1800//1450//1500//1670//2010//0xfe8c//(0xfe8c)//(0xfe8c)//(0xfe8c+0x28)//(0xfeb4-0x28)
#define WLM_CHAN4_WL2		3000//2750//3250//3320//3000//2800//2850//2900//3300//0xfd9e//(0xfd9e-0x10)//(0xfd9e)//(0xfd9e+0x28)//(0xfd9e-0x28)

#define WLM_BIT0_INTERVAL	1350//1600
//#define WLM_BIT1_INTERVAL	700//750//900//800//950//1020

#define WLM_PREFLA_INTERVAL 1500//1400//1600//1750//0xfebd
#define WLM_PREFLA_CONFIRM  4200//4400
#define WLM_PREFLA_FINISH	3500
#define WLM_BEGINPRE_INTERVAL 35000

//#elif(NEW_DSP_SCHEME==_SP690II_)
#else

/************************************************************
//   not for 320ex
*************************************************************/
#define WLM_HEAD_TIME		600
#ifdef WLM_CMD_AD
#define WLM_IMPLUS_TIME  	600
#else
#define WLM_IMPLUS_TIME  	400//450//500//450//500//450//500//450//500//450//500//0xffa4//0xffb6//0xffbf//0xffc9//0xffc9//0xffdb//0xffc9//0xffa4//0xffc9//0xffa4//0xffc9//0xffa4//0xffc9//0xffc0
#endif

#define WLM_CHAN1_WL1		750//950//900//750//800//930//1100//0xff35//(0xff35-0x10)//(0xff35)//(0xff35+0x28)//(0xff35-0x28)
#define WLM_CHAN2_WL1		1450//1650//1600//1450//1500//1670//1850//0xfeab//(0xfeab-0x10)//(0xfeab)//(0xfeab+0x28)//(0xfeab-0x28)
#define WLM_CHAN3_WL1		1650//1850//1800//1450//1500//1670//2010//(0xfe8c)//(0xfe8c)//(0xfe8c+0x28)//(0xfeb4-0x28)
#define WLM_CHAN4_WL1		750//950//900//800//850//930//1100//0xff35//(0xff35-0x10)//(0xff35)//(0xff35+0x28)//(0xff35-0x28)

#define WLM_CHAN1_WL2		1650//1600//1450//1500//1650//1810//0xfeb2//(0xfeb2-0x10)//(0xfeb2)//(0xfeb2+0x28)//(0xfeb2-0x28)
#define WLM_CHAN2_WL2		950//900//800//850//930//1150//0xff2c//(0xff2c-0x10)//(0xff2c)//(0xff2c+0x28)//(0xff2c-0x28)
#define WLM_CHAN3_WL2		1850//1800//1450//1500//1670//2010//0xfe8c//(0xfe8c)//(0xfe8c)//(0xfe8c+0x28)//(0xfeb4-0x28)
#define WLM_CHAN4_WL2		3100//3000//2800//2850//2900//3300//0xfd9e//(0xfd9e-0x10)//(0xfd9e)//(0xfd9e+0x28)//(0xfd9e-0x28)

//#define WLM_BIT0_INTERVAL	0xfef4//0xfefe			//140us
#define WLM_BIT0_INTERVAL	1380//1400//0xfefe//0xfeec//0xff12//0xff02//0xfefe	//138us
//#define WLM_BIT0_INTERVAL  0xfef5			//145
//#define WLM_BIT0_INTERVAL	0xfeec			//delay 150us
//#define WLM_BIT0_INTERVAL	0xff11			//delay 130
//#define WLM_BIT1_INTERVAL	0xff35//0xff23//0xff35			//110us
//#define WLM_BIT1_INTERVAL  0xff48			//impluse 30us intervel 100us
//#define WLM_BIT1_INTERVAL  0xff23			//impluse 30us intervel 110us
//#define WLM_BIT1_INTERVAL  0xff36
//#define WLM_BIT1_INTERVAL 0xff5a
//#define WLM_BIT1_INTERVAL  0xff2b//0xff35			//110us
//#define WLM_BIT1_INTERVAL  0xff2c			//115us
#define WLM_BIT1_INTERVAL 900//800//750//800//0xff40//0xff2e//0xff25//0xff39//0xff35			//110us
#define WLM_PREFLA_INTERVAL 1400//1600//1750//0xfebd
#define WLM_PREFLA_CONFIRM 4200//4400
#define WLM_PREFLA_FINISH	3500
#define WLM_BEGINPRE_INTERVAL 35000
#endif



void WLM_SendData(unsigned char *Arr, char sendSize);
void ExposeTimeCal(const unsigned char camCode,unsigned char hp);
char BuildRealFlashCmd(unsigned char para1,unsigned char para1,unsigned char para2,unsigned char para3,char hpLev);
void Nik_AutoShotSet(unsigned char rcvCmd);

#endif
#endif


