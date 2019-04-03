#ifndef _A7105_H_
#define _A7105_H_
#include "type.h"
#include "global.h"

/*a7105 con*/
#define A7105_SCK PD_ODR_ODR1
#define A7105_CS   PE_ODR_ODR5
#define A7105_ODR   PD_ODR_ODR2
#define A7105_IDR    PD_IDR_IDR3
#define A7105_DDR   PD_DDR_DDR3
#define A7105_IRQ    PB_IDR_IDR0
#define A7105_RX_AF    PE_ODR_ODR3
#if (BOARD_VER == __BOARD_10_)
#define A7105_TX_AF    PE_ODR_ODR4
#endif

//#define RF_LED_GREEN      PC_ODR_ODR0
//#define RF_LED_RED          PC_ODR_ODR1
#define RF_LED_GREEN       PC_ODR_ODR6
#define RF_LED_RED            PC_ODR_ODR5

#define RF_MODE_CAN      0
#define RF_MODE_NIK       1

typedef enum 
{
	RF_STATUES_IDLE,
	RF_STATUES_PRE_FLASH,
	RF_STATUES_FLASH,
}RF_STATUES;

#define RF_LED_LINK()        {RF_LED_RED=0;RF_LED_GREEN=1;}
#define RF_LED_READY()        {RF_LED_RED=1;RF_LED_GREEN=0;}
#define RF_LED_OFF()           {RF_LED_RED=0;RF_LED_GREEN=0;}

void A7105_Init(void);
void A7105Halt(void);
void A7105_Read_Ram(u8 addr, u8 * ram, u8 len);
void A7105_strobe_cmd(u8 cmd);
void RfSetChan(u8 chan);
void RfSetId(u8 ch, u8* id);
void RfHandleProc(void);


#endif

