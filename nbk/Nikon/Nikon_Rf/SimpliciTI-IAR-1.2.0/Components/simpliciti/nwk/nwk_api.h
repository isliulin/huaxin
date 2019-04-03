/**************************************************************************************************
  Filename:       nwk_api.h
  Revised:        $Date: 2011-10-26 15:44:58 -0700 (Wed, 26 Oct 2011) $
  Revision:       $Revision: 28059 $
  Author:         $Author: jnoxon $

  Description:    This header file supports the SimpliciTI appliction layer API.

  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights granted under
  the terms of a software license agreement between the user who downloaded the software,
  his/her employer (which must be your employer) and Texas Instruments Incorporated (the
  "License"). You may not use this Software unless you agree to abide by the terms of the
  License. The License limits your use, and you acknowledge, that the Software may not be
  modified, copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio frequency
  transceiver, which is integrated into your product. Other than for the foregoing purpose,
  you may not use, reproduce, copy, prepare derivative works of, modify, distribute,
  perform, display or sell this Software and/or its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS”
  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY
  WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
  IN NO EVENT SHALL TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE
  THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY
  INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST
  DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY
  THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef NWK_API_H  
#define NWK_API_H

#include "hal_types.h"

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
#define UART_INT_SET()		st(UART_INT_DN();macDelay(2);UART_INT_UP();)
#define UART_INT_TOGGLE()    st( if (P0_4) { P0_4 = 0; } else { P0_4 = 1;} )


typedef	unsigned int	Word;  

#define	MAKEWORD(v1,v2)	(((Word)(v1)<<8) + (Word)(v2))


#define RF_GROUP_TOTAL	6

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


/* Tx options (bit map) */
#define  SMPL_TXOPTION_NONE       ((txOpt_t)0x00)
#define  SMPL_TXOPTION_ACKREQ     ((txOpt_t)0x01)

extern unsigned char g_veriBuf[];
extern unsigned char g_mstBuf[];

smplStatus_t SMPL_Init(uint8_t (*)(linkID_t));
smplStatus_t SMPL_Link(linkID_t *);
smplStatus_t SMPL_LinkListen(linkID_t *);
smplStatus_t SMPL_Send(linkID_t lid, uint8_t *msg, uint8_t len);
smplStatus_t SMPL_SendOpt(linkID_t lid, uint8_t *msg, uint8_t len, txOpt_t);
smplStatus_t SMPL_Receive(linkID_t lid, uint8_t *msg, uint8_t *len);
smplStatus_t SMPL_Ioctl(ioctlObject_t, ioctlAction_t, void *);
void NIK_Init(void);
void NIK_LINK(void);
#ifdef EXTENDED_API
smplStatus_t SMPL_Ping(linkID_t);
smplStatus_t SMPL_Unlink(linkID_t);
smplStatus_t SMPL_Commission(addr_t *, uint8_t, uint8_t, linkID_t *);
#endif  /* EXTENDED_API */

#endif
