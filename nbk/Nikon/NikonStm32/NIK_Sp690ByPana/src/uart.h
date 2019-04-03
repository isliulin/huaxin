#ifndef _UART_
#define _UART_
#define _FORMAT_PRINT_

//#define HP_SAMPLE_TEST

#ifdef HP_SAMPLE_TEST
extern unsigned int adSample;
#endif

#define AD_SIMPLE_START {ANCTR2 |= 0x80;}

extern unsigned char uart_tran_flag;

#define UARTFLAG_INDEX_CHARGE 7
#define UARTFLAG_INDEX_ISO    1
#define UARTFLAG_INDEX_APERT  2
#define UARTFLAG_INDEX_TEMPALARM 3
#define UARTFLAG_INDEX_CLS     4
#define UARTFLAG_INDEX_FOCLEN  5
#define UARTFLAG_INDEX_DEBUG   6
#define UARTFLAG_INDEX_ONEFLASH 6
#define UARTFLAG_INDEX_AUXRLIT 0

#define UARTFLAGEX_INDEX_REAR	   0
#define UARTFLAGEX_INDEX_LOWBAT  1
#define UARTFLAGEX_INDEX_ONETIMEFLASH 2
#define UARTFLAGEX_INDEX_DEBUGDATA 3
#define UARTFLAGEX_INDEX_FULLTIMEDEBUG 4
#define UARTFLAGEX_INDEX_CAMERACMP	5
//#define UARTFLAGEX_INDEX_DEBUG	6
#define UARTFLAGEX_INDEX_WLMODE		6
#define UARTFLAGEX_INDEX_WLEXP		7

#define UART_DECODE_MODE   0xf0
#define UART_DECODE_OUTPUT 0xf1
#define UART_DECODE_TTLCOM 0xf2
#define UART_DECODE_TTLFEB 0xf3
#define UART_DECODE_CMD 0xf4
#define UART_DECODE_FOCLEN 0xfb
#define UART_DECODE_PRDIMFO 0xfc
#define UART_DECODE_WIRELESS 0xf8
#define UART_DECODE_WLMDEBUG 0xf7

#define UART_DECODE_WLMSET			0xeb

#define UART_DECODE_MULTI				0xea
#define UART_DECODE_MULTMFRQ			0xe9

#define UART_DECODE_MULTI_EXPOSE		0X00


#define UART_DECODE_WLMSET_ARRM		0x00
#define UART_DECODE_WLMSET_ARRA		0x01
#define UART_DECODE_WLMSET_ARRB		0x02
#define UART_DECODE_WLMSET_ARRC		0x03
#define UART_DECODE_WLMSET_CHAN		0x04

#define UART_DECODE_WIRELESS_CHAN 0X01
#define UART_DECODE_WIRELESS_ARR  0X02


#define UART_DECODE_CMD_PILOT 0x01
#define UART_DECODE_CMD_SLEEP 0x02
#define UART_DECODE_CMD_HOTWARN 0X03
#define UART_DECODE_CMD_CHARGE_ENABLE  0X04
#define UART_DECODE_CMD_CHARGE_DISABLE 0X05
#define UART_DECODE_CMD_INIT  0x07
#define UART_DECODE_CMD_MUSIC 0x06
#define UART_DECODE_CMD_LED    0x0A
#define UART_DECODE_CMD_FACELITE 0X0B
#define UART_DECODE_CMD_AFLITE	0x0C
#define UART_DECODE_CMD_LOWBAT 0x09
#define UART_DECODE_CMD_RMTSHUT	0x0d
#define UART_DECODE_CMD_RMTOPEN 0x0e

#define UART_DECODE_PRDIMFO_AUTHOR 0x00
#define UART_DECODE_PRDIMFO_UPDATE 0x01
#define UART_DECODE_PRDIMFO_UARTHOLD 0x02


#define UART_DECODE_CMD_MUSIC_ON  0X01
#define UART_DECODE_CMD_MUSIC_OFF 0X00
#define UART_DECODE_CMD_LED_ON	0x01
#define UART_DECODE_CMD_LED_OFF	0x01


#define UART_ENCODE_FUNC 0xfd
#define UART_ENCODE_ISO  0xfe
#define UART_ENCODE_APERT 0xf6//0xfc
#define UART_ENCODE_FOCLEN 0xfb
#define UART_ENCODE_DEBUG 0xfa
#define UART_ENCODE_CMD	0xf9
#define UART_ENCODE_WIRELESS 0xf8

#define UART_ENCODE_WLMOUT					0xef
#define UART_ENCODE_WLTTLEXP					0xee
#define UART_ENCODE_WLCHANDARR				0xed
#define UART_ENCODE_WLMARR					0xec

#ifdef HP_SAMPLE_TEST
#define UART_ENCODE_HPSTEST					0xdd
#endif

#define UART_ENCODE_WIRELESS_MODE			0x00
#define UART_ENCODE_WIRELESS_CHANANDARR 	0x01

#ifdef NIKON_STC_SCHEME
#define UART_ENCODE_INIT 0xf8

#define UART_ENCODE_INIT_MODE				0
#define UART_ENCODE_INIT_TTLCOMP1			1
#define UART_ENCODE_INIT_TTLCOMP2			2
#define UART_ENCODE_INIT_OUTPUT1			3
#define UART_ENCODE_INIT_OUTPUT2			4
#define UART_ENCODE_INIT_MOTOMODE			5
#define UART_ENCODE_INIT_LOCATION			6
#define UART_ENCODE_INIT_MUSIC				7
#endif


#define UART_ENCODE_FUNC_CHARGE 0x01
#define UART_ENCODE_FUNC_TEMP	0X02
#define UART_ENCODE_FUNC_CLS    0x03
#define UART_ENCODE_FUNC_LOWBAT 0x04
//#define UART_ENCODE_FUNC_REAR	0x05
#define UART_ENCODE_FUNC_MODE	0x05
#define UART_ENCODE_FUNC_REAR_ON	0x01
#define UART_ENCODE_FUNC_REAR_OFF	0x00

#define UART_ENCODE_CMD_CHARGE  		0X01
#define UART_ENCODE_CMD_AUXRLIT 		0X02
#define UART_ENCODE_CMD_ONETIMEFLASH 	0x03
#define UART_ENCODE_CMD_MOTRESET		0x04

#define UART_ENCODE_CMD_AUXRLIT_ON  0x01
#define UART_ENCODE_CMD_AUXRLIT_OFF 0x00


#define UART_ENCODE_DEBUG_EXPOSE 0x00
#define UART_ENCODE_DEBUG_ERROR  0x01
#define UART_ENCODE_DEBUG_EXPOSE2 0x02

#define UART_ENCODE_FUNC_LOWBAT_WARNING 0X01
#define UART_ENCODE_FUNC_LOWBAT_CANCEL  0X00
#define UART_ENCODE_FUNC_CHARGE_OK 0x01
#define UART_ENCODE_FUNC_CHARGE_ING 0x00
#define UART_ENCODE_FUNC_TEMP_ALARM 0x00
#define UART_ENCODE_FUNC_CLS_COM 0x00
#define UART_ENCODE_FUNC_CLS_UNCOM 0x01

typedef struct UART_ARRARY
{
	unsigned char rtRcvBuf[4];
	char bufIndex;
}UARTARRARY;

typedef struct UARTSENDFLAG
{
	unsigned char tempCheck;
	unsigned char imfo_iso;
	unsigned char imfo_apert;
	unsigned char imfo_focLen;
	unsigned char charge;
	unsigned char rearCurtain;
	unsigned char flashWarn;
	unsigned char wlm_mode;
	unsigned char wlm_exp;
	unsigned char cls;	
	unsigned char lowBat;
	unsigned char lowBatFlag;
	unsigned char index;
	unsigned char clear;
#ifdef HP_SAMPLE_TEST
	unsigned char hpsFlag;
#endif
}UartSendFlag;


void putsDialog(const char *str);
int DecodeUartData(unsigned char id, unsigned char para1, unsigned char para2);
void com_start(void);
void SerialSync();

#ifdef _FORMAT_PRINT_
int Printf(const char *fmt, ...);
#endif

#endif
 

