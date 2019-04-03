/*----------------------------------------------------------------------------
 *  Demo Application for SimpliciTI
 *
 *  L. Friedman
 *  Texas Instruments, Inc.
 *----------------------------------------------------------------------------
 */
/******************************************************************************************

  Copyright 2007-2009 Texas Instruments Incorporated. All rights reserved.

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

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 揂S IS?  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY
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
#define INDICATOR_TIME_OUT 250;

#include <stdio.h>
#include "bsp.h"
#include "mrfi.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "nwk_pll.h"
#include "bsp_leds.h"
#ifdef MRFI_CC430
  #include "uart_intfc_cc430.h"
#else
  #include "uart_intfc.h"
#endif
#include "ZComDef.h"
#include "hal_drivers.h"
#include "OSAL.h"
#include "hal_board.h"
#include "OnBoard.h"

const uint8 tasksCnt = 0;
uint16 *tasksEvents;

unsigned char g_veriBuf[] = 
{
0x2C,0x00,0x24,0x21,0x32,0x02,0x01,0x77,0x26,0x00,0x2C,0x40,0x12,0x34,0x01,0x41,0x36,0x38,0x47,0x2C,0x50,0x01,0x01,
0x02,0x21,0x01,0x08,0x00,0xB7,0x54,0x84,0xD3,0x00,0x00,0x00,0x53,0x42,0x2D,0x35,0x30,0x30,0x30,0x00
};

unsigned char g_mstBuf[] = 
{
0x21,0x1A,0x24,0x21,0x01,0x41,0x36,0x38,0x47,0x00,0x2C,0x40,0x12,0x34,0x25,0x2C,0x55,0x01,0x00,0x02,
0x03,0x03,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x55,0x00,0x00,0x04,0xEC,
};

uint8_t g_rcvPkt = 0;

static void InitUart_test(void)
{
    CLKCONCMD &= ~0x40; // 设置系统时钟源为 32MHZ晶振
    while(CLKCONSTA & 0x40);                     // 等待晶振稳定 
    CLKCONCMD &= ~0x47;                          // 设置系统主时钟频率为 32MHZ

    PERCFG = 0x00;        //位置1 P0口 
    //P0SEL = 0x3c;        //P0_2,P0_3,P0_4,P0_5用作串口,第二功能 
    P0SEL |= 0x0c;
    //P0DIR |= 0x08;
    P2DIR &= ~0XC0;      //P0 优先作为UART0 ，优先级
    //PERCFG &= ~(BV(0)); //选择P0端口uart
    //P0SEL |=BV(5)|BV(4)|BV(3)|BV(2);
    //P2DIR &=~(BV(7)|BV(6));

    U0CSR |= 0x80;       //UART 方式 
    U0GCR |= 11;         //U0GCR与U0BAUD配合     
    U0BAUD |= 216;       // 波特率设为115200 
    UTX0IF = 0;          //UART0 TX 中断标志初始置位1  （收发时候）
    U0CSR |= 0X40;       //允许接收 
   // IEN0 |= 0x84;        // 开总中断，接收中断    

   URX0IF = 0;

  // P1_3 = !P1_3;
}

static void SendData_test(char dat)
{
	U0DBUF = dat;
	while(!UTX0IF);
	UTX0IF = 0; 	
}

static void putsDialog_test(const char *str)
{
	 //UartDecode(str);
	 while(*str != '\0')
	{
        	SendData_test(*str++);
    }
}

static void OAD_test(void)
{
	InitUart_test();	
	 //T1CTL = 0x0d;          
	 //T1STAT= 0x21;         
	 //while(1){POWER_UP_DEBUG();}
	 putsDialog_test("nikon sb5000 project\r\n");
}

/*********************************************************************
 * @fn      zmain_vdd_check
 * @brief   Check if the Vdd is OK to run the processor.
 * @return  Return if Vdd is ok; otherwise, flash LED, then reset
 *********************************************************************/
static void zmain_vdd_check( void )
{
#if 0
  uint8 vdd_passed_count = 0;
  bool toggle = 0;

  // Repeat getting the sample until number of failures or successes hits MAX
  // then based on the count value, determine if the device is ready or not
  while ( vdd_passed_count < MAX_VDD_SAMPLES )
  {
    if ( HalAdcCheckVdd (ZMAIN_VDD_LIMIT) )
    {
      vdd_passed_count++;    // Keep track # times Vdd passes in a row
      MicroWait (10000);     // Wait 10ms to try again
    }
    else
    {
      //cbyi delete for debug //vdd_passed_count = 0;  // Reset passed counter
      MicroWait (50000);     // Wait 50ms
      MicroWait (50000);     // Wait another 50ms to try again
    }

    /* toggle LED1 and LED2 */
    if (vdd_passed_count == 0)
    {
      if ((toggle = !(toggle)))
        HAL_TOGGLE_LED1();
      else
        HAL_TOGGLE_LED2();
    }
  }

  /* turn off LED1 */
  HAL_TURN_OFF_LED1();
  HAL_TURN_OFF_LED2();
#endif
}


void main (void)
{
#if 1
  /* holds length of current message */
  uint8_t len; 
  
  /* the link token */
  linkID_t LinkID = 0;
  
  /* the transmit and receive buffers */
  uint8_t rx[MAX_APP_PAYLOAD], tx[MAX_APP_PAYLOAD];

  /* holds led indicator time out counts */
  uint16_t led_tmr;

  // Turn off interrupts
  osal_int_disable( INTS_ALL );// 关闭所有中断

  // Initialization for board related stuff such as LEDs
  HAL_BOARD_INIT();

  // Make sure supply voltage is high enough to run
  zmain_vdd_check();

  // Initialize board I/O
  InitBoard( OB_COLD );

  // Initialze HAL drivers
  HalDriverInit();

#endif

  BSP_Init( );

  SMPL_Init( NULL );

  //OAD_test();
  
  //uart_intfc_init( );
  DmaUartInit();

  /* turn on the radio so we are always able to receive data asynchronously */
  SMPL_Ioctl( IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_RXON, NULL );
   
  /* turn on LED. */
  BSP_TURN_ON_LED1( );

  tx_send("hello, nikon SB-5000, OK\r\n",sizeof("hello, nikon SB-5000, OK\r\n"));

  NIK_Init();
  
  while(1)
  {
	NIK_LINK();

	tx_send("ran...",sizeof("ran..."));

	HalUARTPoll();
  }
  
#if 0
#ifdef  LINK_TO
  {
    uint8_t cnt = 0;
    tx_send_wait( "Linking to...\r\n", 15 );
    while (SMPL_SUCCESS != SMPL_Link(&LinkID))
      if( cnt++ == 0 )
      {
        /* blink LED until we link successfully */
        BSP_TOGGLE_LED1( );
      }
  }
#else // ifdef LINK_LISTEN
  tx_send_wait( "Listening for Link...\r\n", 23 );
  while (SMPL_SUCCESS != SMPL_LinkListen(&LinkID))
  {
    /* blink LED until we link successfully */
    BSP_TOGGLE_LED1( );
  }
#endif

  tx_send_wait( "Link Established!\r\nReady...\r\n", 29 );
  
  /* turn off the led */
  BSP_TURN_OFF_LED1( );
  
  main_loop:
    /* Check to see if we received any characters from the other radio
     * and if we have send them out the uart and reset indicator timeout.
     * Prioritize on radio received links as that buffer is the most time
     * critical with respect to the UART buffers.
     */
    if( SMPL_Receive( LinkID, tx, &len ) == SMPL_SUCCESS )
    {
      /* blocking call but should be ok if both ends have same uart buad rate */
      tx_send_wait( tx, len );
      led_tmr = INDICATOR_TIME_OUT;   /* update activity time out */
    }

    FHSS_ACTIVE( if( nwk_pllBackgrounder( false ) != false ) );
    {
      /* check to see if the host has sent any characters and if it has
      * then send them over the radio link and reset indicator timeout.
      */
      len = rx_receive( rx, MAX_APP_PAYLOAD );
      if( len != 0 )
      {
        while( SMPL_Send( LinkID, rx, len ) != SMPL_SUCCESS )
          ;
        led_tmr = INDICATOR_TIME_OUT;   /* update activity time out */
        
        /* By forcing a minimum delay between transmissions we guarantee
        * a window for receiving packets.  This is necessary as the radio
        * link is half duplex while the UART is full duplex.  Without this
        * delay mechanism, packets can get lost as both ends may attempt to
        * transmit at the same time which the CCA algorithm fails to handle.
        */
        MRFI_DelayMs( 5 );
      }
    }
    
    /* manage led indicator */
    if( led_tmr != 0 )
    {
      led_tmr--;
      BSP_TURN_ON_LED1( );
    }
    else
      BSP_TURN_OFF_LED1( );

    goto main_loop; /* do it again and again and again and ... */
	
#endif
}


/**************************************************************************************************
 * @fn          MAC_RfFrontendSetup
 *
 * @brief       Setup RF frontend.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)
#else
void MAC_RfFrontendSetup(void)
{
  /* AGCCTRL1 for CC2590 or CC2591 */
  //AGCCTRL1 = 0x16;

  /* CC2591 HGM pin control configuration
   *   P0_7 -> HGM
   */
  //HAL_PA_LNA_RX_HGM();

  /* Select power register value table and RSSI adjustment value table */
  //MAC_SetRadioRegTable(MAC_CC2591_TX_PWR_TABLE_IDX, MAC_CC2591_HGM_RSSI_ADJ_IDX);
}
#endif
