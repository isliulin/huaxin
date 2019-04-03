#ifndef _CBYIOLOONG_
#define _CBYIOLOONG_
#define OLO_PANAS

#define _SP330_ 		1
#define _SP690II_ 	2
#define _SP700_ 		3

//#define CANON_TTL_DEBUG
//#define WIRELESS_DEBUG
#define NEW_DSP_SCHEME 3

/*************************************
//the following macro determined the fuction
**************************************/
#define _CANON_WIRELESS_
//#define _NIKON_WIRELESS_
//#define _MULTIPLY_FLASH_
//#define WIRELESS_MASTER_ON 1
#define HP_SYN_MODE_NO	0
#define HP_SYN_MODE_SW	1
#define HP_SYN_MODE_HW	2

//#ifdef _SP330_
#if(NEW_DSP_SCHEME==_SP330_)
#define _MULTIPLY_FLASH_
#define _HP_SYN_
#define WIRELESS_MASTER_ON 1
//#define _HP_TRIGER_MODE_
#define WLM_MASTER_FLA
//#define _LED_AUX_
#define _AF_AUTO_
#define WLM_CMD_AD
#define _ADD_COIL_
#define EXT_LED
#define _HP_SYN_MODE_	HP_SYN_MODE_SW
#define WLM_ADVAL 40
#define FLASH_TRIGER_TIME		200
#define SCREEN_LCD_DOT
#define WLM_CHAN1_WIDTH		450//600//450//350//500//5
#define WLM_CHAN2_WIDTH		900//1050//900//520//1300//9
#define WLM_CHAN3_WIDTH		1400//1550//1400//830//1700//16
#define WLM_CHAN4_WIDTH		2000//2150//2000//1100//2200//22
#define WLM_BIT1_INTERVAL  		230//300//300//600//5//600
#define WLM_BIT0_INTERVAL		830//850
//#endif
//#ifdef _SP690II_
#elif(NEW_DSP_SCHEME==_SP690II_)
#define _MULTIPLY_FLASH_
#define WIRELESS_MASTER_ON 1
#define _OLD_TRIGER_MODE_
#define EXT_CHARGE
#define _HP_SYN_MODE_	HP_SYN_MODE_NO
#define WLM_CHAN1_WIDTH		450//350//500//5
#define WLM_CHAN2_WIDTH		900//520//1300//9
#define WLM_CHAN3_WIDTH		1400//830//1700//16
#define WLM_CHAN4_WIDTH		2000//1100//2200//22
#define WLM_BIT1_INTERVAL  		370//600//5//600
#define WLM_BIT0_INTERVAL		850
#elif(NEW_DSP_SCHEME==_SP700_)
#define _MULTIPLY_FLASH_
#define _HP_SYN_
//#define _HP_SYN_HARDWARE_
//#define _HP_SYN_MODE_	2
#define WIRELESS_MASTER_ON 1
#define WLM_MASTER_FLA
#define WLM_CMD_AD
#define EXT_CHARGE
#define _ADD_COIL_
#define _AF_AUTO_
#define _HP_SYN_MODE_	HP_SYN_MODE_HW
//#define AD_SIMPLE_START {ANCTR2 |= 0x80;}
#define WLM_ADVAL 40
#define FLASH_TRIGER_TIME		500//200//250//400//600//200
#define SCREEN_LCD_DOT 
#define WLM_CHAN1_WIDTH		(450+200)//350//500//5
#define WLM_CHAN2_WIDTH		(900+200)//520//1300//9
#define WLM_CHAN3_WIDTH		(1400+200)//830//1700//16
#define WLM_CHAN4_WIDTH		(2000+100)//1100//2200//22
#define WLM_BIT1_INTERVAL  		(370+200)//600//5//600
#define WLM_BIT0_INTERVAL		(850)
#endif


#ifdef _HP_SYN_
#if(_HP_SYN_MODE_==HP_SYN_MODE_SW)
#define HP_PREFLASH_INTERVAL	150
#define HP_REALFLA_INTERVAL	2900//2200
#else
#define HP_PREFLASH_INTERVAL	4500//450
#define HP_REALFLA_INTERVAL	0xffff//65500//9000//2200
#endif
#define HP_FLASHMODE_ON		1
#define HP_FLASHMODE_OFF		0
#endif

//#define CANON600D_DEBUG
//#define CANON_NOMOTO_VERSION
//#define WIRELESS_MEASURE
#ifdef WIRELESS_MEASURE
#define WLBUF_LEN 45
#else
#define WLBUF_LEN 8
#endif

//#define _TRACE_

#define HIGH_TRIGER
#define UART_SENDDATA_CONTINUE
#define UART_SENDMODE_ASYC

#define SYSCLK          24500000    // Internal oscillator frequency in Hz
//#define SYSCLK          12250000 
#define BAUDRATE        9600//9600           // Baud rate of UART in bps

#define MAX_BUFFER_SIZE    10           // Maximum buffer Master will send
#define SEND_MAX_SIZE 20
#define  PARA_SIZE 20
#define	MAX_UART_DATA_LEN	5


#define VOL_MAX		   2000		  //260V
#define VOL_AD_FLALEVEL	   155//620
//#define VOL_MIN		   2000
#define VOL_FULL	   2451
#define VOL_AD_FULL    190//178//713

#define LOW_BAT		   1470
#define NOR_BAT		   1635		  //5v

#define AD_CHARGE_LOW 0
#define AD_CHARGE_RDY 1
#define AD_CHARGE_FUL 2

#define PILOT_FLAG_ON				1
#define PILOT_FLAG_OFF				0

#define TEST_PILOT_ON	1
#define TEST_PILOT_OFF	0

#define err 1

typedef unsigned int WORD;
typedef unsigned char BYTE;

#define LENOFTIMETABLE 40

#define SYNMODE_HEAD 0
#define SYNMODE_REAR 1
#define SYNMODE_HP	2

#define AUX_MODE_OFF	3
#define AUX_MODE_LED	0
#define AUX_MODE_FLA	1
#define AUX_MODE_AUTO	2

#define F2FLASH_LABEL_SET	1
#define F2FLASH_LABEL_CLR	0

//keypad handle
//#define WORKINGMODE_TTL 0																					                               
//#define WORKINGMODE_MANU 1
//#define WORKINGMODE_F1 (2+1)
//#define WORKINGMODE_F2 (3+1)
//#define WORKINGMODE_AUTO (4+1)
//#define WORKINGMODE_SIGN 0xcc
//#define WORKINGMODE_TURNOFF (5+1)
//#define WORKINGMODE_MULTI 2

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
#define WORKINGMODE_TURNOFF	3

//communication statue
#define COM_STATUE_START 0x01
#define COM_STATUE_HOLD 0x00
#define COM_STATUE_PREFLASH 0X02
#define COM_STATUE_REALFLASH 0X03
#define COM_STATUE_SLEEP    0X04
#define COM_STATUE_WLMPRE 0X05
#define COM_STATUE_MULTIFLASH 0x06

#define FLASH_STATUE_READY 				0
#define FLASH_STATUE_PREFLASH_OK			1
#define FLASH_STATUE_PREFLASH_FAILED		2


#ifdef HIGH_TRIGER 	  
//#define TURN_ON_TRIGER(time) {disable_irq(); MEMCTR |= 0X04; TM7MD1 &= 0XEF; TM7ICR &= 0XFC; CHARGE_ENABLE=1; TM7PR1 = time; FLASH_TRIGER=1;TM7MD1 |= 0X10;	while(!(TM7ICR&0X01));FLASH_TRIGER=0;TM7MD1 &= 0XEF;TM7ICR &= 0XFE;	CHARGE_ENABLE=0;MEMCTR &= ~0X04;enable_irq();}

#define PREFLASH_TRIGER	  	{CHARGE_ENABLE=1;FLASH_TRIGER=1;}
#define TRUNOFF_PREFLASH	{FLASH_TRIGER=0;CHARGE_ENABLE=0;}	
#else
//#define TURN_ON_TRIGER
#define TRUN_OFF_TRIGER		
#endif

#define SET_FOLLOWFLASH_OPEN   {disable_irq();MEMCTR    |=  0x04;	IRQ2ICR = 0X82;	MEMCTR   &= ~0x04;enable_irq();}
#define SET_FOLLOWFLASH_CLOSE  {disable_irq();MEMCTR    |=  0x04;	IRQ2ICR = 0X80;	MEMCTR   &= ~0x04;enable_irq();}

#define SET_AFCTL_INPUT 	{PADIR&=0X7F;}
#define SET_AFCTL_OUTPUT	{PADIR|=0X80;}

#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 

#define START_TIMER0	{TM0MD |= 0x08;}
#define STOP_TIMER0		{TM0MD &= ~0x08;}
#define UPDATA_TIMER0(x)	{STOP_TIMER0;TM0OC=(x-1);START_TIMER0}

//new version uart communication define
#define UARTFLAG_INDEX_CHARGE 0
#define UARTFLAG_INDEX_ISO    1
#define UARTFLAG_INDEX_APERT  2
#define UARTFLAG_INDEX_TEMPALARM 3
#define UARTFLAG_INDEX_CLS     4
#define UARTFLAG_INDEX_FOCLEN  5

#define UART_ENCODE_MODE   				0xf0
#define UART_ENCODE_OUTPUT 				0xf1
#define UART_ENCODE_TTLCOM 				0xf2
#define UART_ENCODE_TTLFEB				0xf3
//#define UART_ENCODE_CMD					0xf4
#define UART_ENCODE_COMPENSATION   		0xf5
#define UART_ENCODE_CMD					0xf9
#define UART_ENCODE_FUNC					0xfd
#define UART_ENCODE_ISO  					0xfe
#define UART_ENCODE_APERT 					0xf6//0xfc
#define UART_ENCODE_FOCLEN 				0xfb
#define UART_ENCODE_MODECHANGE  			0xfa
#define UART_ENCODE_WIRELESS				0xf8
#define UART_ENCODE_WLMOUT				0xef
#define UART_ENCODE_WLTTLEXP				0xee

#define UART_ENCODE_WLMOPEN				0xec
#define UART_ENCODE_WLMPARA				0xeb
#define UART_ENCODE_WLMPRESEV			0xe8

#define UART_ENCODE_WLMOPEN_MSTFLA		0x03


#define UART_ENCODE_MULTIPARA				0xea
#define UART_ENCODE_MULTIP				0xe9
#define UART_ENCODE_MULWLM				0xe7
#define UART_ENCODE_EXITSLAVE				0xe6

#define UART_ENCODE_MULTIP_EXP			0x01

#define UART_ENCODE_WIRELESS_MODE		0x00
#define UART_ENCODE_WIRELESS_CHANANDARR 0x01

#define UART_ENCODE_WLMPRESEV_ABRATIO 0x01
#define UART_ENCODE_WLMPRESEV_CCMP	    0x02
#define UART_ENCODE_WLMPRESEV_AOPT	    0x03
#define UART_ENCODE_WLMPRESEV_BOPT	    0x04
#define UART_ENCODE_WLMPRESEV_COPT	    0x05




#define UART_ENCODE_CMD_PILOT 			0x01
#define UART_ENCODE_CMD_ONETIMEFLASH	0x03
#define UART_ENCODE_CMD_SINGLE			0x02
#define UART_ENCODE_CMD_COMPATIBLE		0x04
#define UART_ENCODE_CMD_FULL25TIME		0x05
#define UART_ENCODE_CMD_S2COUNT			0x06
#define UART_ENCODE_CMD_READY			0x07




#define UART_ENCODE_FUNC_CHARGE 0x01
#define UART_ENCODE_FUNC_TEMP	0X02
#define UART_ENCODE_FUNC_CLS    0x03
#define UART_ENCODE_FUNC_LOWBAT 0x04
#define UART_ENCODE_FUNC_STATUE 0x05
#define UART_ENCODE_FUNC_S2		0x06

#define UART_ENCODE_FUNC_STATUE_LINK 0x01
#define UART_ENCODE_FUNC_LOWBAT_WARNING 0X01
#define UART_ENCODE_FUNC_LOWBAT_CANCEL  0X00
#define UART_ENCODE_FUNC_CHARGE_OK 0x01
#define UART_ENCODE_FUNC_CHARGE_ING 0x00
#define UART_ENCODE_FUNC_TEMP_ALARM 0x00
#define UART_ENCODE_FUNC_CLS_COM 0x00
#define UART_ENCODE_FUNC_CLS_UNCOM 0x01


#define UART_DECODE_MODE   				0xf0
#define UART_DECODE_OUTPUT 				0xf1
#define UART_DECODE_TTLCOM 				0xf2
#define UART_DECODE_TTLFEB				0xf3
#define UART_DECODE_CMD					0xf4
//#define UART_DECODE_COMPENSATION   		0xf5
#define UART_DECODE_REFLECT				0xF5
#define UART_DECODE_FOCLEN 				0xfb  
#define UART_DECODE_FUNC				0xfd
#define UART_DECODE_PRDIMFO 			0xfc
#define UART_DECODE_WIRELESS 			0xf8

#define UART_DECODE_WLMARR			0xec
#define UART_DECODE_WLMSET			0xeb

#define UART_DECODE_WLMSET_OPEN		0x04
#define UART_DECODE_WLMSET_MSTFLA	0x05


#ifdef _MULTIPLY_FLASH_
#define UART_DECODE_MULTI				0xea
#define UART_DECODE_MULTMFRQ			0xe9

#define UART_DECODE_MULTI_EXPOSE		0X00
#endif

#define UART_DECODE_WIRELESS_CHAN 0X01
#define UART_DECODE_WIRELESS_ARR  0X02

#define UART_DECODE_PRDIMFO_AUTHOR 0x00
#define UART_DECODE_PRDIMFO_UPDATE 0x01



#define UART_DECODE_KEY 		0xf6
#define UART_DECODE_PARA		0xf7

#define UART_DECODE_FUNC_SYNC   0x01
#define UART_DECODE_FUNC_SYNC_REAR 0x01
#define UART_DECODE_FUNC_SYNC_FRON 0x00
#define UART_DECODE_FUNC_SYNC_HP	    0x02	


#define UART_DECODE_KEY_PILOT	0x00
#define UART_DECODE_KEY_MUSIC	0x07
#define UART_DECODE_KEY_LOCK	0x0f
#define UART_DECODE_KEY_LIGHT	0x04
#define UART_DECODE_KEY_UP		0x08
#define UART_DECODE_KEY_DOWN	0x05
#define UART_DECODE_KEY_LEFT	0x06
#define UART_DECODE_KEY_RIGHT	0x09
#define UART_DECODE_KEY_MODE	0x0e


#define UART_DECODE_CMD_PILOT 0x01
#define UART_DECODE_CMD_SLEEP 0x02
#define UART_DECODE_CMD_HOTWARN 0X03
#define UART_DECODE_CMD_CHARGE_ENABLE  0X04
#define UART_DECODE_CMD_CHARGE_DISABLE 0X05
#define UART_DECODE_CMD_INIT		0X07
#define UART_DECODE_CMD_LOWBAT		0X09
#define UART_DECODE_CMD_LED    0x0A
#define UART_DECODE_CMD_FACELITE	0x0B
#define UART_DECODE_CMD_AFLITE 0x0C


#define UART_DECODE_CMD_MUSIC 0x06
#define UART_DECODE_CMD_MUSIC_ON  0X01
#define UART_DECODE_CMD_MUSIC_OFF 0X00
#define UART_DECODE_CMD_LED_ON	0x01
#define UART_DECODE_CMD_LED_OFF	0x01

#define UART_DECODE_PARA_CHARGE 0x00

#define WLM_ON 			1
#define WLM_OFF 		0

//#define WLM_MASTER_ON 0x08
//#define WLM_MASTER_OFF 0X00

#define WLM_MASTER_ON 		1
#define WLM_MASTER_OFF 		0

#define WLM_SET_MASTERON 1
#define WLM_SET_MASTEROFF 0

#define WLM_IMPLUS_TIME		380

#if 0
#ifdef WLM_CMD_AD
#define WLM_CHAN1_WIDTH		450//600//450//350//500//5
#define WLM_CHAN2_WIDTH		900//1050//900//520//1300//9
#define WLM_CHAN3_WIDTH		1400//1550//1400//830//1700//16
#define WLM_CHAN4_WIDTH		2000//2150//2000//1100//2200//22
#define WLM_BIT1_INTERVAL  		230//300//300//600//5//600
#define WLM_BIT0_INTERVAL		830//850
#else
#define WLM_CHAN1_WIDTH		450//350//500//5
#define WLM_CHAN2_WIDTH		900//520//1300//9
#define WLM_CHAN3_WIDTH		1400//830//1700//16
#define WLM_CHAN4_WIDTH		2000//1100//2200//22
#define WLM_BIT1_INTERVAL  		370//600//5//600
#define WLM_BIT0_INTERVAL		850
#endif
#endif

//#define WLM_BIT0_INTERVAL	850//1000//11//1000
//#define WLM_BIT1_INTERVAL  370//600//5//600

#define WLC_RCVDAT_OVERFLOW 162//125




//unsigned char bitIndex=0;
//unsigned char paraBit=0;
//unsigned char bitPara3=0;

//char protect=0;
//char charge_ok=0;
//char mode_sigle=0;
//char statue_sleep=0;


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

#define BIT_paraBit_CAMERLINK 				0
#define BIT_paraBit_WORKMODE_VARY 			1
#define BIT_paraBit_OUTPUT_CHANGE 			2
#define BIT_paraBit_TTL_COMPENSATION_VARY 	3
#define BIT_paraBit_ONE_TIME_FULLFLASH 		4
#define BIT_paraBit_ONE_TIME_FLASH 			5
#define BIT_paraBit_FOLLOW_FLASH 			6
//#define BIT_paraBit_COMPENSATION_SET 		7

#define BIT_bitWireless_mode			0
#define BIT_bitWireless_TtlExp		1
#define BIT_bitWireless_Moutp		2
#define BIT_bitWireless_chanAndArr	3
#define BIT_bitMaster_Switch			4
#define BIT_bitMaster_Para			5

#define BIT_bitMultip_TmFrq			0
#define BIT_bitMultip_Exp				1
#define BIT_bitMultip_WLMA			2
#define BIT_bitMultip_WLMB			3
#define BIT_bitMultip_WLMC			4
#define BIT_bitShutter				5


#define BIT_bitWLM_Acmp			0
#define BIT_bitWLM_abRatio			1
#define BIT_bitWLM_Ccmp				2
#define BIT_bitWLM_Aotp				3
#define BIT_bitWLM_Botp				4
#define BIT_bitWLM_Cotp				5
#define BIT_bitWLM_Mfla				6



//bit protection_en=0;			 bitIndex.1
//bit pre_flash=0;				 bitIndex.3
//bit real_flash=0;				 bitIndex.5
//bit charge_ok = 0;			  bitIndex.2
//bit charge_full = 0;			  bitIndex.3
//bit s2_count=0;				 bitIndex.6
//bit pilot_flag=0;				 bitIndex.7
//bit ttl_time_calculate=0;		 bitIndex.8
//bit check_bat_en=0;	
//bit mode_single = 0;
//bit flash_ok = 0;
//bit camera_linked									paraBit.0
//bit workmode_vary									paraBit.1
//bit output_change									paraBit.2
//bit ttl_flash_compensation_vary           		paraBit.3
//bit one_time_full_flash							paraBit.4
//bit one_time_flash								paraBit.5
//bit follow_flash									paraBit.6
//bit compensation_set								paraBit.7  

#define BIT_bitPara3_CHECK_BAT 				0
#define BIT_bitPara3_MODE_SINGLE			1		//this bit indicating a compatible camera has been link, so bypass the s1 and s2 mode
//#define BIT_bitPara3_FLASH_OK 				2		
#define BIT_bitPara3_PRE_TO_REAL 			3
#define BIT_bitPara3_FLASH_SURRAND_INDEX 	4
#define BIT_bitPara3_PRE_FLASH_FAIL			5
#define BIT_bitPara3_LOWBATWARNING			7


#define BIT_bitTerminate_LOWBATCHECK		0
#define BIT_bitTerminate_LOWBATWARN		1
#define BIT_bitTerminata_FOCCHANGE			2
#define BIT_bitTerminate_A5FLAG				3
#define BIT_bitTerminate_STATUELINK			4
#define BIT_bitTerminate_SLEEP				5
#define BIT_bitTerminate_TTLFEB				6
#define BIT_bitTerminate_APERT				7

#define BIT_bitExtra_A8index				0
#define BIT_bitExtra_Shutter				1
#define BIT_BitExtra_ExitSlave			2

//bit check_bata_en									bitPara3.0 
//bit mode_single									bitPara3.1
//bit flash_ok										bitPara3.2
//bit pre_to_real									bitPara3.3
//bit flash_surrand_index							bitPara3.4
//bit previous_flash_failed						    bitPara3.5
//bit workmode_flag									bitPara3.6




/*
// Instruction Set
#define  SLAVE_LED_ON      0x01        // Turn the Slave LED on
#define  SLAVE_LED_OFF     0x02        // Turn the Slave LED off
#define  SPI_WRITE         0x04        // Send a byte from the Master to the
                                       // Slave
#define  SPI_READ          0x08        // Send a byte from the Slave to the
                                       // Master
#define  SPI_WRITE_BUFFER  0x10        // Send a series of bytes from the
                                       // Master to the Slave
#define  SPI_READ_BUFFER   0x20        // Send a series of bytes from the Slave
                                       // to the Master
#define  ERROR_OCCURRED    0x40        // Indicator for the Slave to tell the
                                       // Master an error occurred

//#define SYSCLK             24500000/8  // SYSCLK in Hz (24.5 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define TIMER_PRESCALER            12  // Based on Timer2 CKCON and TMR2CN
                                       // settings

#define LED_TOGGLE_RATE            50  // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

// There are SYSCLK/TIMER_PRESCALER timer ticks per second, so
// SYSCLK/TIMER_PRESCALER/1000 timer ticks per millisecond.
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

// Note: LED_TOGGLE_RATE*TIMER_TICKS_PER_MS should not exceed 65535 (0xFFFF)
// for the 16-bit timer

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1

#define TIMER2_RELOAD            AUX2  // Reload value for Timer2

#define TIMER2_RELOAD_HIGH       AUX2  // Reload value for Timer2 high byte
#define TIMER2_RELOAD_LOW        AUX2  // Reload value for Timer2 low byte

#define TCOUNT_MIN  
*/
/////////////////////////////////////

//////////////////////////
//parameter location defination
#define PARA_MODE		0
#define PARA_SYNC1	 	0
#define PARA_SYNC2	 	1
#define PARA_SYNC3	 	2
#define PARA_FOCIND		3
#define PARA_FOCLEN		4 
#define PARA_VOUT		5
#define PARA_SRD		6
#define PARA_WIRELESS	7
#define PARA_FREQ		8
#define PARA_MOUT		9
#define PARA_A5			10
#define PARA_COMP       11
#define PARA_WLM_MODE       12

//canon wireless macro
#define CANWL_STATUE_HOLD 0
#define CANWL_STATUE_ENSURECHAN 1
#define CANWL_STATUE_RUN	2
#define CANWL_STATUE_START 3
#define CANWL_STATUE_STOP  4
#define CANWL_STATUE_PLUSMEASURE 5
#define CANWL_STATUE_PREFLASH	6
#define CANWL_STATUE_FLASH		7

#define WL_FLASH_STATUE_HOLD  	0	//before receive light signal
#define WL_FLASH_STATUE_START 	1	//after ensure the channel
#define WL_FLASH_STATUE_BFRPF		2	//before take previous flash
#define WL_FLASH_STATUE_AFTPF 	3	//before take previous flash
#define WL_FLASH_STATUE_FLASH		4	//before take real flash

#define WL_CHANNEL_UNSURE 	0
#define WL_CHANNEL_ONE	 	1
#define WL_CHANNEL_TWO	  	2
#define WL_CHANNEL_THREE  		3
#define WL_CHANNEL_FOUR		4
#define WL_CHANNEL_MAKESURE 	5

#define WL_ARRARY_A	0
#define WL_ARRARY_B 1
#define WL_ARRARY_C 2
#define WL_ARRARY_NULL 3

#define WLN_WIZARD_PRE	1
#define WLN_WIZARD_FLA	2
#define WLN_WIZARD_HOLD	0

#define BIT_BITFLAG_WLTIMEOUT 0
#define BIT_BITFLAG_WLFLASHTO 1


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
	unsigned char hpLev;			//for master
	
}HpSetting;
#else
typedef unsigned int HpSetting;
#endif

#ifdef _CANON_WIRELESS_
	#define WLC_CHAN_SCALE1_GND			70//80//90//80//230
	#define WLC_CHAN_SCALE1_ROOF			140//120//130//140//120//300

	#define WLC_CHAN_SCALE2_GND			140//130//140//150//120//380
	#define WLC_CHAN_SCALE2_ROOF			190//180//190//200//160//450

	#define WLC_CHAN_SCALE3_GND			190//190//200//160//350
	#define WLC_CHAN_SCALE3_ROOF			260//240//240//250//200//420

	#define WLC_CHAN_SCALE4_GND			260//250//260//250//200//630
	#define WLC_CHAN_SCALE4_ROOF			350//310//300//310//300//240//700

	#define WLC_CHAN_VERI_GND				550//440
	#define WLC_CHAN_VERI_ROOF			600//480
#endif

#ifdef _NIKON_WIRELESS_
	#define BLFLAG_START 	0x60
	#define BLFLAG_PRE1 	0X90
	#define BLFLAG_PRE2 	0XA0
	#define BLFLAG_PRE3 	0XB0
	#define BLFLAG_READY 	0X20

	#define WLN_PFCOUNT_RDY		0X00
	#define WLN_PFCOUNT_ONE 	0x01
	#define WLN_PFCOUNT_TWO 	0x02

	#define WLN_CHAN_SCALE1_GND			140//210//230
	#define WLN_CHAN_SCALE1_ROOF			210//310//300

	#define WLN_CHAN_SCALE2_GND			250//370//380
	#define WLN_CHAN_SCALE2_ROOF			320//470//450

	#define WLN_CHAN_SCALE3_GND			230//340//350
	#define WLN_CHAN_SCALE3_ROOF			300//440//420

	#define WLN_CHAN_SCALE4_GND			420//620//630
	#define WLN_CHAN_SCALE4_ROOF			490//720//700

	#define NIKWL_STATUE_HOLD 0
	#define NIKWL_STATUE_ENSURECHAN 1
	#define NIKWL_STATUE_RUN	2
	#define NIKWL_STATUE_START 3
	#define NIKWL_STATUE_STOP  4
	#define NIKWL_STATUE_PLUSMEASURE 5
	#define NIKWL_STATUE_PREFLASH	6
	#define NIKWL_STATUE_FLASH		7

	#define WLN_PREWIDTH_GND	250//500//220//360
	#define WLN_PREWIDTH_ROOF 350//600//320//460

	#define TTL_TIMECAL_POINT0 	0x00//0x00
	#define TTL_TIMECAL_POINT1 	0x05//0x20
	#define TTL_TIMECAL_POINT2 	0x0a//0x28
	#define TTL_TIMECAL_POINT3 	0x10//0x30
	#define TTL_TIMECAL_POINT4 	0x15//0x38
	#define TTL_TIMECAL_POINT5 	0x1a//0x40
	#define TTL_TIMECAL_POINT6 	0x20//0x43
	#define TTL_TIMECAL_POINT7 	0x30//0x45
	#define TTL_TIMECAL_POINT8 	0x33//0x48
	#define TTL_TIMECAL_POINT9 	0x36//0x4a
	#define TTL_TIMECAL_POINT10 0x3a//0x4c
	#define TTL_TIMECAL_POINT11 0x40//0x50
	#define TTL_TIMECAL_POINT12 0x45//0x53
	#define TTL_TIMECAL_POINT13 0x4a//0x56
	#define TTL_TIMECAL_POINT14 0x4d//0x5a
	#define TTL_TIMECAL_POINT15 0x50//0x5d
	#define TTL_TIMECAL_POINT16 0x56//0x60
	#define TTL_TIMECAL_POINT17 0x5a//0x63
	#define TTL_TIMECAL_POINT18 0x60//0x66
	#define TTL_TIMECAL_POINT19 0x64//0x6a
	#define TTL_TIMECAL_POINT20 0x68//0x6d
	#define TTL_TIMECAL_POINT21 0x6c//0x70
	#define TTL_TIMECAL_POINT22 0x71//0x73
	#define TTL_TIMECAL_POINT23 0x76//0x76
	#define TTL_TIMECAL_POINT24 0x7a//0x7a
	#define TTL_TIMECAL_POINT25 0x7d//0x7d
	#define TTL_TIMECAL_POINT26 0x82//0x80
	#define TTL_TIMECAL_POINT27 0x85//0x85
	#define TTL_TIMECAL_POINT28 0x8a//0x8a
	#define TTL_TIMECAL_POINT29 0x90//0x90
	#define TTL_TIMECAL_POINT30 0x93//0x95

	#define WL_MASTER_PREFLA_WIDTH		0x1042


#define WLS_TTL_TP0				0xa0	
#define WLS_TTL_TP1				0xa2
#define WLS_TTL_TP2				0xa4
#define WLS_TTL_TP3				0xa6
#define WLS_TTL_TP4				0xa8
#define WLS_TTL_TP5				0xaa
#define WLS_TTL_TP6				0xac
#define WLS_TTL_TP7				0xae

#define WLS_TTL_TP8				0xb0
#define WLS_TTL_TP9				0xb2
#define WLS_TTL_TP10			0xb4
#define WLS_TTL_TP11			0xb6
#define WLS_TTL_TP12			0xb8
#define WLS_TTL_TP13			0xba
#define WLS_TTL_TP14			0xbc
#define WLS_TTL_TP15			0xbe

#define WLS_TTL_TP16			0xc0
#define WLS_TTL_TP17			0xc2
#define WLS_TTL_TP18			0xc4
#define WLS_TTL_TP19			0xc6
#define WLS_TTL_TP20			0xc8
#define WLS_TTL_TP21			0xca
#define WLS_TTL_TP22			0xcc
#define WLS_TTL_TP23			0xce

#define WLS_TTL_TP24			0xd0
#define WLS_TTL_TP25			0xd2
#define WLS_TTL_TP26			0xd4
#define WLS_TTL_TP27			0xd6
#define WLS_TTL_TP28			0xd8
#define WLS_TTL_TP29			0xda
#define WLS_TTL_TP30			0xdc
#define WLS_TTL_TP31			0xde

	typedef struct	 EXPOSEPARA
	{
		char workmode;
		char output1;
		char output2;
		char TTL_LEVEL1;
		char TTL_LEVEL2;
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
	}EXPOSE_PARA;
#endif

#define WL_SCHEDULE_ALL		0X01
#define WL_SCHEDULE_RES		0X07
#define WL_SCHEDULE_AB		0X03
#define WL_HPSYN_ON		1
#define WL_HPSYN_OFF		0

#define CALC_FP_SWHEAD		0
#define CALC_FP_SWWL		1

#define WLM_MODE_ALL			0
#define WLM_MODE_AB			1
#define WLM_MODE_RES			2

#ifdef _CANON_WIRELESS_
#define CLR_WLC_BUF {canWireless.WL_buf[0]=canWireless.WL_buf[1]=canWireless.WL_buf[2]=canWireless.WL_buf[3]=canWireless.WL_buf[4]=canWireless.WL_buf[5]=canWireless.WL_buf[6]=canWireless.WL_buf[7]=0;canWireless.WLBufIndex=0;}
#endif

#ifdef _NIKON_WIRELESS_
#define CLR_WLN_BUF {nikWireless.WL_buf[0]=nikWireless.WL_buf[1]=nikWireless.WL_buf[2]=nikWireless.WL_buf[3]=nikWireless.WL_buf[4]=nikWireless.WL_buf[5]=nikWireless.WL_buf[6]=nikWireless.WL_buf[7]=0;nikWireless.WLBufIndex=0;}
#endif
 

//full flash protection 
#define MAX_FULL_FLASH 20

#define SPI_PARA_MAX 11

//#define GETTTLTIME(value) ((36*value*value-7600*value+440000)/1000)
#define GETBIT(targetbit,n) ((targetbit>>n)&0x01)
#define SETBIT(targetbit,n)	(targetbit|=(0x01<<n))
#define CLRBIT(targetbit,n)	 (targetbit&=(~(0x01<<n)))

//unsigned char pre_flash = 0;
//-----------------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------------

typedef struct TIMEINDEX{	
	unsigned char index;
	int ms;
}timeTable;

typedef struct arrary_para
{
	unsigned char isMasterOn;
	unsigned char arrMode;
	unsigned char ABRation;
	unsigned char AotpCode;
	unsigned char BotpCode;
	unsigned char CotpCode;
	unsigned char Aeps;
	unsigned char Beps;
	unsigned char Ceps;
	unsigned char Acmp;
	unsigned char Bcmp;
	unsigned char Ccmp;
	unsigned char ResArrCcmp;
}ArrPara;

//typedef struct wireless_set
//{
//	unsigned char chan;
//	ArrPara arrPara;	 
//}WlSet;

typedef struct WLM_AB_RATION
{
	unsigned char para1;
	unsigned char para2;
}WLM_abRation_fillwords;

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
	unsigned char focLen; 	
	unsigned char moto_location;
	unsigned char moto_mode;
	unsigned char apert;
	unsigned char hpFlash;
	//unsigned char ttl_expCodeRsv;
	
}VIP_PARA;

typedef struct SENIOR_MENU
{
	unsigned char rmtIndex;
	unsigned char ledFlag;
	unsigned char faceLite;
}SnrMenuSet;

typedef struct AFSETTING
{
	unsigned char afSel;
	unsigned char afStart;
	unsigned int afcnt;
	unsigned char afFlash;
}AfSet;

typedef struct spi_index
{
	unsigned char SPIIndex ;
	unsigned char sendIndex ;
	char receiveIndex ;
	unsigned char SPI_Receive_Index;
	unsigned char WORKMODE ;
	unsigned char workmode_index;	
}SPI_INDEX;

#ifdef _MULTIPLY_FLASH_
#define MULTI_FLASH_ENABLE		1
#define MULTI_FLASH_DISABLE	0
typedef struct MULTI_Set
{
	unsigned char expose;
	int  eTimes;
	int frequence;
	int finalExpTimes;
	unsigned char cmdCode;
	unsigned char mulCtl;
	unsigned char WlmMul[3];
	unsigned char wlmMulExp[3];
//	unsigned char frqMode;
	int lowFrqCnt;
	unsigned char sCode;
	unsigned char maxTimes;
//	unsigned char cMode;
}MultiSet;
#endif

#define WLM_CMDSEND_PREFLA_ALL	3
#define WLM_CMDSEND_PREFLA_CHA	4
#define WLM_CMDSEND_PREFLA_CHB	5
#define WLM_CMDSEND_PREFLA_CHC	6
#define WLM_CMDSEND_FLAIMF		7

#define WLM_CMDPROC_ON 1
#define WLM_CMDPROC_OFF 0

#define WLM_CMDFLASH_CONFIRM		1
#define WLM_CMDFLASH_DENY			0


typedef struct WIRELESS
{
	unsigned char s_chan;
	unsigned char s_arrary;
	unsigned char s_wlMode;
	unsigned char s_flashStatue;
	unsigned char s_extCode;

	unsigned char m_mode;
	unsigned char m_on;
	unsigned char m_chan;
	unsigned char m_preIndex;
	ArrPara m_arrPara;
	unsigned char m_flaCmd[8];
	unsigned char m_isMasterFla;
	char m_cmdIndex;
	unsigned char m_ttlCmd[3];
	unsigned char m_cmdSendId;
	unsigned char m_cmdProcOn;
	unsigned char pullFlag;
	unsigned int muitl_realTimes;
	unsigned int testCnt;
	unsigned char testPilot;

	unsigned char m_flaConfirm;
	
	
}Wl_set;

typedef struct CANON_WIRELESS
{
	unsigned char WL_mode;
	unsigned char WL_opCode;
	unsigned char WL_output1;
	unsigned char WL_output2;
	unsigned char WL_Ttl;
	unsigned char WL_statue;
	unsigned int WL_timeout;
#ifdef WIRELESS_MEASURE
	unsigned int WL_buf[WLBUF_LEN];
#else
	unsigned char WL_buf[WLBUF_LEN];
#endif
	unsigned char ttl_buf[3];
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
	unsigned char activeArr;
	unsigned char wlMul_frq;
	unsigned char wlMul_time;
	unsigned char wlMul_expose;

#ifdef _HP_SYN_
	unsigned char HpSyn;
#endif

	unsigned char pilotFalg;
	unsigned char preflashLock;
	
}CaWireless;

#ifdef _NIKON_WIRELESS_

	typedef struct NIKON_WIRELESS
	{
		unsigned char WL_mode;
		unsigned char WL_opCode;
		unsigned char WL_output1;
		unsigned char WL_output2;
		unsigned char WL_Ttl;
		unsigned char WL_statue;
		unsigned char WL_timeout;
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
	}NiWireless;

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
		char mode;
	}UART_SEND;

	typedef struct CAMERASETTING
	{
		char ISO;
		char shutter;
		char aperture;
		char focLen;
		char cls;
		char rcvCode;
		char camera_cmp1;
		char camera_cmp2;
		char rcvHandShake;
		unsigned char wl_chan;
		char wl_arrary;
		//char prefla_com;
	}CAMER_SETTING;

#endif

#if 0

sbit OUTFLAG = P0^6;  
//sbit SPIINDEX = P1^5;
sbit TESTFLAG = P2^0;  
sbit INT0SRC1 = P1^0;	  //TTL sck interrupt input
sbit INT0SRC2 = P1^1;	  //real flash int input
sbit INT0SRC3 = P1^2;	   //s1, s2 mode interrupt input
sbit FLASH_PWM = P0^7;
//bit FLASH_PWM ;
sbit POWER = P1^3;	   
sbit AF_EN = P1^6;  
sbit MISO = P0^1;
sbit SPISCK = P0^0;
sbit CHARGE_ENABLE=P1^5;
sbit IGBT_CONTROL=P1^7;
//sbit TIMERFLAG = P1^7;

#endif

#endif

