#include "mac_high_level.h"
#include "osal.h"


//#define DEBUG_TRACE
//#define IO_INDEX_TEST
//#define DEBUG_NOTRACE
//#define SEND_OBEY_ZSTACK
//#define CARRY_WAVE_TEST
#ifdef IO_INDEX_TEST
#define FINGER1	P1_3
#define FINGER2  P1_2

void FingerIndex_1(void);
void FingerIndex_2(void);
#endif

//#define LED_TOGGLE_DEBUG

typedef unsigned char       BOOL;

// Data
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

// Unsigned numbers
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned long       UINT32;

// Signed numbers
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed long         INT32;

// Placement controlled by oad.xcl.
#define HAL_OAD_RC_START           0x2000//0x0800//0x1000//0x2000//0x2000//0x0800//0x5000//0x0800
#define HAL_OAD_CRC_ADDR           0x2888//0x0888//0x1088//0x0888
#define HAL_OAD_CRC_OSET          (HAL_OAD_CRC_ADDR - HAL_OAD_RC_START)

#define CPUID_VERI_ADDR             0x1FF0//0x1FF0//0x20188
#define CPUID_VERI_LEN			16

//#define _RT_AP_
#define DEVICE_TOTAL 15

#ifdef _RT_AP_

#define LED_GREEN 	P1_2
#define LED_RED	   	P1_3


#define AP_TXEN      	P1_1
#define AP_RXEN	     	P1_4

#else

//#define LED_GREEN 	P1_4
//#define LED_RED	P1_5
//#define LED_GREEN     P1_2
//#define LED_RED         P1_3

#endif

#define MACEX_SET_POWER_MODE(mode)       st( SLEEPCMD &= ~PMODE; /* clear mode bits */    \
                                                 SLEEPCMD |= mode;   /* set mode bits   */    \
                                                 while (!(STLOAD & LDRDY));                   \
                                                 {                                            \
                                                   halSetSleepMode();                         \
                                                 }                                            \
                                               )

#define min(v1,v2) (((v1)<(v2))?(v1):(v2))
#define halRfWaitTransceiverReady()	{while(MAC_RADIO_TRANS_NOREADY);}

#define MAC_DATAREQ_API
#define DEVICE_DEFINE
#define LINKMODE_DEFINE

#define WAIT_RF_SEND_OK()         {while(!(RFIRQF1 & IRQ_TXDONE) );}

typedef	unsigned int	Word;  

/*
#define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
#define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
#define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
#define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

#define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
#define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
#define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
#define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

#define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
#define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
#define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
#define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

#define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
#define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))
#define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))
#define HAL_STATE_LED4()          HAL_STATE_LED1()
*/

#define LED_GREEN_ON()          st(HAL_TURN_ON_LED1();)
#define LED_GREEN_OFF()        st(HAL_TURN_OFF_LED1();)
#define LED_GREEN_TOGGLE()  st(HAL_TOGGLE_LED1();)

#define LED_RED_ON()          st(HAL_TURN_ON_LED2();)
#define LED_RED_OFF()        st(HAL_TURN_OFF_LED2();)
#define LED_RED_TOGGLE()  st(HAL_TOGGLE_LED2();)

#define LED_INS_LINKED()    st(LED_GREEN_ON();LED_RED_OFF();)
#define LED_INS_UNLINK()    st(LED_GREEN_OFF();LED_RED_ON();)
#define LED_INS_BRIGDE()    st(LED_GREEN_ON();LED_RED_ON();)
#define LED_INS_ALLOFF()    st(LED_GREEN_OFF();LED_RED_OFF();)

#define UART_INT_UP()		st(P0_4=1;)			
#define UART_INT_DN()		st(P0_4=0;)	
//#define UART_INT_SET()		st(UART_INT_DN();UartSendByte(0xFF);UART_INT_UP();)
#define UART_INT_SET()		st(UART_INT_DN();macDelay(1);UART_INT_UP();)

#define	MAKEWORD(v1,v2)	(((Word)(v1)<<8) + (Word)(v2))
#define GETBIT(targetbit,n) 		(((targetbit)>>(n))&0x01)
#define SETBIT(targetbit,n)		((targetbit)|=(0x01<<(n)))
#define CLRBIT(targetbit,n)	 	((targetbit)&=(~(0x01<<(n))))
// (((n) < (m)) ? (n) : (m))

#define BASIC_RF_PACKET_OVERHEAD_SIZE       ((2 + 1 + 2 + 2 + 2) + (2))
#define BASIC_RF_MAX_PAYLOAD_SIZE	        (127 - BASIC_RF_PACKET_OVERHEAD_SIZE - \
    BASIC_RF_AUX_HDR_LENGTH - BASIC_RF_LEN_MIC)
#define BASIC_RF_ACK_PACKET_SIZE	        5
#define BASIC_RF_FOOTER_SIZE                2
#define BASIC_RF_HDR_SIZE                   10

// Auxiliary Security header
#define BASIC_RF_AUX_HDR_LENGTH             5
#define BASIC_RF_LEN_AUTH                   BASIC_RF_PACKET_OVERHEAD_SIZE + \
    BASIC_RF_AUX_HDR_LENGTH - BASIC_RF_FOOTER_SIZE
#define BASIC_RF_SECURITY_M                 2
#define BASIC_RF_LEN_MIC                    8

#define RF_CHAN_AUTO		0
#define RF_CHANNEL               11// 25      // 2.4 GHz RF channel
#define MST_PAN_ID                0x3000//0x5469//0x3000//0x2007
#define LINKED_PAN_ID		0x9000
#define MST_PAN_ID_AUTO	0x0000
#define LINKED_PAN_ID_AUTO 0x6000
#define ADDR                  0x000F//0xFFFF//0x000F//0x2520
#define DSET_ADDR	      0xFFFF	
#define APP_PAYLOAD_LENGTH        127//30//127//30
#define PAN_ID_COMPRESSION 6

#define MAC_UART_EX_INT_IFG	 P0IFG
#define MAC_UART_EX_INT_BIT	BV(4)
#define MAC_KEY_CPU_PORT_0_IF     P0IF

#define MACEX_ROLL_POLING_EVENT 	0x0001
#define MACEX_VERI_SEND_INQURE	0x0002
#define MACEX_LINK_TIMEOUT_EVENT  0x0004
#define MACEX_LED_FLASH			0x0008
#define MACEX_INIT_CHECK			0x0010
#define MACEX_INFO_INQ				0x0020
#define MACEX_SLAVE_PWRDOWN		0x0040

#define MACEX_DEVICE_CHECK_NUM	8

#define PARA_MULTI_A   7
#define PARA_MULTI_B   (PARA_MULTI_A+2)
#define PARA_MULTI_C   (PARA_MULTI_B+2)
#define PARA_MULTI_HZ       (14)
#define PARA_MULTI_TM       (13)

#define PARA_GR_A 8
#define PARA_GR_B (PARA_GR_A+3)
#define PARA_GR_C (PARA_GR_B+3)
#define PARA_GR_D (PARA_GR_C+3)
#define PARA_GR_E (PARA_GR_D+3)


//serial setting
#define MACEX_UART_DEFAULT_BAUDRATE			HAL_UART_BR_115200//HAL_UART_BR_57600
#define MACEX_UART_DEFAULT_OVERFLOW			FALSE
#define MACEX_UART_DEFAULT_THRESHOLD		(128/2)
#define MACEX_UART_DEFAULT_MAX_RX_BUFF		128		
#define MACEX_UART_DEFAULT_MAX_TX_BUFF		128
#define MACEX_UART_DEFAULT_IDLE_TIMEOUT		6

#if defined (ZTOOL_P1)
  #define MACEX_PORT HAL_UART_PORT_0
#elif defined (ZTOOL_P2)
  #define MACEX_PORT HAL_UART_PORT_1
#else
  #undef ZTOOL_PORT
#endif

#define SEND_UNIFIED

//#define UART_ENCODE_RADIO	 0xBB
#define UART_ENCODE_RFMOD					0xE0

#define UART_ENCODE_RFID					0xCF
#define UART_ENCODE_RFCH					0xCE
#define UART_ENCODE_RFADDR				0xCD
#define UART_ENCODE_RFIDEX				0xCC
#define UART_ENCODE_SLAGR					0xCB

#define UART_ENCODE_RADIO					0xBB
#define UART_ENCODE_APERTSHUTER			0xBA
#define UART_ENCODE_ISOCMP				0xB9			

#define UART_ENCODE_RFa					0xB4//0xB8
#define UART_ENCODE_RFb					0xB5//0xB7
#define UART_ENCODE_RFc					0xB6//0xB6
#define UART_ENCODE_RFd					0xB7//0xB5
#define UART_ENCODE_RFe					0xB8//0xB4

#define UART_ENCODE_GRM					0xB4

#define UART_ENCODE_RFPREARR				0xB3
#define UART_ENCODE_RFCOUNTDOWN			0xB2
#define UART_ENCODE_RFRECOVER				0xB1

#define UART_ENCODE_RFCMD					0xB0

#define UART_ENCODE_RFMUL					0xAF
#define UART_ENCODE_PILOT					0xAE
#define UART_ENCODE_CHSCAN				0xAD
#define UART_ENCODE_EXIT					0xAC
#define UART_ENCODE_RFFLA					0xAB

#define UART_ENCODE_SLASLEEP				0xA1
#define UART_ENCODE_SLAPWROFF			0xA2
#define UART_ENCODE_UNMODULATE			0xA0

#define UART_ENCODE_RFCMD_PREFLA			0x01
#define UART_ENCODE_RFCMD_FLA			0x02
#define UART_ENCODE_RFCMD_PILOT			0x03
#define UART_ENCODE_RFCMD_REL				0x04
#define UART_ENCODE_RFCMD_SLEEP			0x05
#define UART_ENCODE_RFCMD_MODEL			0x06
#define UART_ENCODE_RFCMD_TEST			0x07

#define UART_CC_REL							0xC0
#define UART_CC_DEV							0xC1
#define UART_CC_READY						0xC2
#define UART_CC_RTSCAN						0xC3
#define UART_CC_INIT						0xC4
#define UART_CC_SLACMD						0xC5
#define UART_CC_MSTSW						0xC6

#define UART_SLAVE_PRE						0xC7
#define UART_SLAVE_FLA						0xC8
#define UART_SLAVE_MULFLA 					0xC9
#define UART_SLAVE_PREFLA					0xCA
#define UART_SLAVE_LINKED					0xCB

#define UART_CC_REL_FLA					0x01
#define UART_CC_REL_MODEL					0x02

#define UART_CC_SLACMD_TEST				0x03
#define UART_CC_SLACMD_MODEL				0x02


#define RADIO_STATUE_PRE			1
#define RADIO_STATUE_FLACFM		2
#define RADIO_STATUE_FLASH		3
#define RADIO_STATUE_RECOVER		4

#define RF_GROUP_A					0
#define RF_GROUP_B					1
#define RF_GROUP_C					2
#define RF_GROUP_D					3
#define RF_GROUP_E					4

#define WORKINGMODE_TTL 		0
#define WORKINGMODE_MANU 		1
#define WORKINGMODE_MULTI 	2
#define WORKINGMODE_MASTER	3
#define WORKINGMODE_RTN		4
#define WORKINGMODE_RTC 		5
#define WORKINGMODE_GR		6

#define WORKINGMODE_EXTA		4
#define WORKINGMODE_OFF		6
//#define WORKINGMODE_EXTA		8
//#define WORKINGMODE_F1		6
//#define WORKINGMODE_F2		7

//#define WORKINGMODE_OFF		0xff
#define WORKINGMODE_AUTO		2
#define WORKINGMODE_TURNOFF	3


#define GRMOD_TTL				0
#define GRMOD_M					1
#define GRMOD_EXTA				2


#define UART_RCVBUF_LEN			80//128//30//20

#define CMR_ADEPT_OLD		0
#define CMR_ADEPT_NEW		1

#define SYNC_MODE_FRON  		0
#define SYNC_MODE_REAR  		1
#define SYNC_MODE_FP			2

#define HP_FLASHMODE_ON		1
#define HP_FLASHMODE_OFF		0

#define EXIT_BEFORE_SET			       1
#define NOEXIT_BEFORE_SET			0

#define FLASH_PILOT_ON          1
#define FLASH_PILOT_OFF        0

#define ALLOC_SUCCESS		0
#define ALLOC_FULL			1
#define ALLOC_ERROR			2		

typedef struct{
	uint16 mst;
	uint16 sla;
}IpReserve;

typedef struct {
    uint8 seqNumber;
    uint16 srcAddr;
    uint16 srcPanId;
    int8 length;
    uint8* pPayload;
    uint8 ackRequest;
    int8 rssi;
    volatile uint8 isReady;
    uint8 status;
} basicRfRxInfo_t;

typedef struct 
{
//#ifndef SEND_OBEY_ZSTACK
	uint8   packetLength;
//#endif
	uint8   fcf0;           // Frame control field LSB
	uint8   fcf1;           // Frame control field MSB
	uint8   seqNumber;
	uint16  panId;
	uint16  destAddr;
	uint16  srcAddr;
#ifdef SECURITY_CCM
	uint8   securityControl;
	uint8  frameCounter[4];
#endif
} basicRfPktHdr_t;

typedef struct DEVICE_INFO
{
	//uint16 veriFlag;
	uint8 veriFlag;
	uint16 shortAddr;
	uint8 extAddr[8];
	uint8 group;
	uint8 ready;
	uint8 attribute;
	uint8 info;
	//struct DEVICE_INFO* pFront;
	//struct DEVICE_INFO* pNext;
}DeviceInfo;

typedef struct 
{
//#ifndef SEND_OBEY_ZSTACK
	uint8   packetLength;
//#endif
	uint8   fcf0;           // Frame control field LSB
	uint8   fcf1;           // Frame control field MSB
	uint8   seqNumber;
	uint16  panId;
	uint8    extAddr[8];
	uint16  srcPan;
	uint16  srcAddr;
#ifdef SECURITY_CCM
	uint8   securityControl;
	uint8  frameCounter[4];
#endif
} extRfPktHdr_t;

typedef struct 
{
	uint16 myAddr;
	uint16 panId;
	uint8 channel;
	uint8 autoChan;
	uint8 ackRequest;
	uint8 *extAddr;
	uint16 destAddr;
	uint16 frameHead;
	//DeviceInfo * pCurrentDev;
	//DeviceInfo *devHead;
	DeviceInfo devHead[16];
	//DeviceInfo *devRear;
	uint8 totalDev;
	uint8 evtCnt;
	uint8 isRflsCoordinator;
/**********************************
	linked master and slave record
**********************************/
	uint8 mstRcd;
	uint8 slvRcd;
	uint8 arrCnt[5];
	uint8 idInGroup[16];
	uint8 isIdOccupied[16];
	DEVICE_DEFINE uint8 group;
#ifdef SECURITY_CCM
	uint8* securityKey;
	uint8* securityNonce;
#endif
} basicRfCfg_t;

// Tx state
typedef struct 
{
	uint8 txSeqNumber;
	volatile uint8 ackReceived;
	volatile uint8 txDone;
	uint8 receiveOn;
	uint32 frameCounter;
} basicRfTxState_t;

typedef struct
{
	macRx_t	*macPayload;
	osal_event_hdr_t	hdEvt;
}macMsg_t;

typedef struct
{
	uint16 checkTurn;
	uint8 checkCount;
	//DeviceInfo * pChk;
	//uint8 pChk;
}DevCheck;

#define RF_GROUP_TOTAL	5
typedef struct RFGROUP_ARR_PARA
{
	char mode;
	//char output1;
	//char output2;
	unsigned char optCode;
	unsigned char ttlExpVal;
	unsigned char mulOtpVal;
	char extaTmp;
	//char cmp1;
	//char cmp2;
	//char mulOtp;
}RfGroup_ArrPara;

typedef struct RADIO_SETTING
{
	uint8 ratio;
	uint8 adept;
	//uint8 cmdSta;
	unsigned char workMode;
	unsigned char synMod;
	unsigned char isoCmp;
	unsigned char shutter;
	unsigned char apert;
	unsigned char preflashIndex;
	unsigned char mul_times;
	unsigned char mul_frq;
	unsigned char hpFlash;
	unsigned char fPilot;
	RfGroup_ArrPara  rfGr_arrPara[RF_GROUP_TOTAL];
	//unsigned char arrExpVal[5];
}RadioSet;

/*
typedef struct
{
	uint8 pExtAddr[8];
	uint16 shortAddr;
	uint8 verState;
}CurrentVeriDev;
*/

#define MAC_INCOMING_MSG_CMD			0x1A

#define DEVICE_UNVERIFIED		0
#define DEVICE_ONVERI			1
#define DEVICE_RESPOND			2
#define DEVICE_WAITIP			3
#define DEVICE_VERIFIED			4

#define TXSTATE_TXDONE_CLEAR			0
#define TXSTATE_TXDONE_PENDING		1

#define TXSTATE_TXACKDONE_WAIT		0
#define TXSTATE_TXACKDONE_RCVD		1


#define CURRENT_VERISTATUE_UNAUTH 
#define RF_SEND_INT

//#define MSTDEV_MODE_HOLD				0
//#define MSTDEV_MODE_DEV				1//0
//#define MSTDEV_MODE_MST				2///1
//#define MSTDEV_MODE_MLINK				3///2
//#define MSTDEV_MODE_SLINK				4///2

#define CMD_MODE_OFF			0
#define CMD_MODE_RF_MST		1
#define CMD_MODE_RF_SLA		2
#define CMD_MODE_LIGH_MST		3
#define CMD_MODE_LIGH_SLA		4
#define CMD_MODE_RFLS_SLA		5
#define CMD_MODE_RFLS_MST		6

#define BRIDGE_MST_NORMAL				0
#define BRIDGE_MST_HOLD				1

#define RFLS_COORDINATOR_M		0
#define RFLS_COORDINATOR_S			1

#define SEND_MODE_FLUSH			0
#define SEND_MODE_UNSLOTTED		1


/*
*  This macro is for use by other macros to form a fully valid C statement.
*  Without this, the if/else conditionals could show unexpected behavior.
*
*  For example, use...
*    #define SET_REGS()  st( ioreg1 = 0; ioreg2 = 0; )
*  instead of ...
*    #define SET_REGS()  { ioreg1 = 0; ioreg2 = 0; }
*  or
*    #define  SET_REGS()    ioreg1 = 0; ioreg2 = 0;
*  The last macro would not behave as expected in the if/else construct.
*  The second to last macro will cause a compiler error in certain uses
*  of if/else construct
*
*  It is not necessary, or recommended, to use this macro where there is
*  already a valid C statement.  For example, the following is redundant...
*    #define CALL_FUNC()   st(  func();  )
*  This should simply be...
*    #define CALL_FUNC()   func()
*
* (The while condition below evaluates false without generating a
*  constant-controlling-loop type of warning on most compilers.)
*/
//#define st(x)      do { x } while (__LINE__ == -1)

#define UINT16_HTON(x)  st( macRvsBuf((uint8*)&x, sizeof(uint16)); )
#define UINT16_NTOH(x)  st( macRvsBuf((uint8*)&x, sizeof(uint16)); )

#define UINT32_HTON(x)  st( macRvsBuf((uint8*)&x, sizeof(uint32)); )
#define UINT32_NTOH(x)  st( macRvsBuf((uint8*)&x, sizeof(uint32)); )

#define UINT64_HTON(x)  st( macRvsBuf((uint8*)&x, 8); )
#define UINT64_NTOH(x)  st( macRvsBuf((uint8*)&x, 8); )


//uint8 macRfSendPacket(uint16 destAddr, uint8* pPayload, uint8 length,uint8* extAddr,uint8 addrMod);
uint8 macRfSendPacket(uint16 destAddr, uint8* pPayload, uint8 length,uint8* extAddr,uint8 addrMod,uint8 sendMod);
void BuildPreFlashInfoData(uint8 *buf, uint8 *len);
void LoadGrPara(RfGroup_ArrPara *rfg, unsigned char mode, unsigned char para);
void BuildFlashInfoData(uint8 *buf, uint8 *len);
unsigned char RfMcodeCovert(unsigned char src, uint8 hpMod);


