/**************************************************************************************************
  Filename:       _hal_oad.c
  Revised:        $Date: 2008-01-17 12:32:06 -0800 (Thu, 17 Jan 2008) $
  Revision:       $Revision: 16224 $

  Description:    This module contains optionally-compiled Boot Code to support OAD.
                  The rest of the functionality is the H/W specific drivers to read/write
                  the flash/NV containing the ACTIVE and the DOWNLOADED images.
  Notes:          This version targets the Texas Instruments CC2x3x family of processors.


  Copyright 2008-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include <ioCC2530.h>
#include "comdef.h"
#include "hal_board_cfg.h"
#include "hal_dma.h"
#include "hal_flash.h"
#include "hal_oad.h"
#include "hal_types.h"
//#include "stdio.h"
//#include "hal_aes.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                          Constants
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                          Typedefs
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                       Global Variables
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                       Global Functions
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                       Local Variables
 * ------------------------------------------------------------------------------------------------
 */

#if HAL_OAD_BOOT_CODE
halDMADesc_t dmaCh0;
#endif

/* ------------------------------------------------------------------------------------------------
 *                                       Local Functions
 * ------------------------------------------------------------------------------------------------
 */

static uint16 runPoly(uint16 crc, uint8 val);

#define	 ucLDR_Security_F	(*(unsigned char *)0x100)	
#define	 start_flag	(*(unsigned char *)0x101)	
#define	 ucLDR_Ret	(*(unsigned char *)0x102)
#define	 ucLDR_Message	(*(Uart_message *)0x110)
#define	 stCommand	(*(union  u_stCommand *)0x1E0)	
#define	 ucLDR_WorkRam	(*(unsigned char *)0x200)
//#define 	 dmaCh0		(*(halDMADesc_t*)0x201)

#define LED_1 P1_3
#define LED_2 P1_4

#define OAD_AES_VERI

// _mode_ is one of
#define CBC         0x00
#define CFB         0x10
#define OFB         0x20
#define CTR         0x30
#define ECB         0x40
#define CBC_MAC     0x50

#define AES_BUSY    0x08
#define ENCRYPT     0x00
#define DECRYPT     0x01

// Macro for starting or stopping encryption or decryption
#define AES_SET_ENCR_DECR_KEY_IV(mode) \
   do {                                \
    ENCCS = (ENCCS & ~0x07) | mode     \
   } while(0)

// Where _mode_ is one of
#define AES_ENCRYPT     0x00;
#define AES_DECRYPT     0x02;
#define AES_LOAD_KEY    0x04;
#define AES_LOAD_IV     0x06;

#define LENGTH_IV  16
#define LENGTH_KEY 16

#define STRING_LENGTH       16

#define VALID_INIT_SN				(978137684)
#define VALID_START_SN				(260636838)


// Macro for setting the mode of the AES operation
#define AES_SETMODE(mode) do { ENCCS &= ~0x70; ENCCS |= mode; } while (0)

// Macro for starting the AES module for either encryption, decryption,
// key or initialisation vector loading.
#define AES_START()     ENCCS |= 0x01

#define DELAY 0x0F

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

typedef volatile unsigned char XDATA macRam_t;
#define TI_EXTADDR  PXREG( 0x780C )  /* Pointer to Start of Flash Information Page          */

#if HAL_OAD_XNV_IS_SPI
static void HalSPIRead(uint32 addr, uint8 *pBuf, uint16 len);
static void HalSPIWrite(uint32 addr, uint8 *pBuf, uint16 len);
static unsigned char RcvData(void);
static unsigned char Message_Rcv(void);
static void Message_Send(void);
static void SendData(char dat);
static void delay(unsigned int de);
static void MemReadRam(macRam_t * pRam, uint8 * pData, uint8 len);
static uint8 OAD_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );
static void putsDialog(const char *str);
//static void halAesEncrDecr(BYTE *pDataIn, UINT16 length, BYTE *pDataOut, /*BYTE *pInitVector, */BOOL decr);
//static void halAesLoadKeyOrInitVector(BYTE* pData, BOOL key);
#ifdef OAD_AES_VERI
static void halAesLoadKeyOrInitVector(BYTE* pData, BOOL key);
static void halAesEncrDecr(BYTE *pDataIn, UINT16 length, BYTE *pDataOut, /*BYTE *pInitVector, */BOOL decr);
#endif

#endif

#if 0
const uint8 crcTest[] = {
0x02,0x1C,0x0E,0x02,0x11,0xE2,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x02,0x12,0xA8,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x2B,0xD2,0xFF,0xFF,
0xFF,0xFF,0xFF,0x02,0x11,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x12,0xFD,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x19,0x07,0xFF,0xFF,
0xFF,0xFF,0xFF,0x02,0x11,0x22,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x19,0x16,0xFF,0xFF,
0xFF,0xFF,0xFF,0x02,0x19,0x22,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x2B,0x5B,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x02,0x11,0x70,0xFF,0xFF,0x00,0xF0,0x44,0x7B,0x0F,0xFE,0x00,0x8F,
0x01,0x00,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,
0x00,0x05,0x00,0x0A,0x00,0x14,0x00,0x28,0x00,0x4F,0x00,0x9E,0x00,0x00,0x00,0x59,
0x21,0x59,0x21,0x59,0x21,0x2D,0x20,0x27,0x20,0x59,0x21,0x67,0x2C,0x00,0x00,0x00,
0x00,0x2B,0x2D,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x0C,0x00,0x9C,0x04,0xFF,
0x01,0x64,0x00,0x02,0x02,0x02,0x07,0x1E,0x02,0x05,0x1E,0x02,0x0A,0x03,0xB8,0x0B,
0x02,0xF0,0xFF,0x02,0x01,0x17,0x00,0xFF,0x47,0x75,0x30,0x60,0x04,0x01,0xA0,0x04,
0x01,0xA0,0x04,0x01,0xA0,0x04,0x01,0x00,0x00,0x01,0x00,0x00,0x0F,0x00,0xFF,0x48
};
#endif

#if HAL_OAD_BOOT_CODE
static void dl2rc(void);
static uint16 crcCalc(void);
static void InitUart(void);
//static void Uart_Send_String(char *Data,int len);
#endif

#ifdef _FORMAT_PRINT_
static int Printf(const char *fmt, ...);
#endif

/**************************************************************************************************
 * @fn          main
 *
 * @brief       ISR for the reset vector.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
 
//#pragma location="RAM_CODE_XDATA"

//unsigned char ucLDR_Security_F;
//unsigned char start_flag;
//unsigned char ucLDR_Ret;
//Uart_message ucLDR_Message;
//unsigned char ucLDR_WorkRam;

 //char *g_decKey = "WorkForYourDream";
 //BYTE  IV[LENGTH_IV ] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#pragma location="NEAR_CODE"

void main(void)
{
  uint16 crc[2];
  uint8 veriBuf[16];
  //uint8 g_extAddr[8] = {0x38,0x15,0xC6,0xFE,0xFF,0x17,0x87,0x88};
 // uint16 addr = HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE;
 uint16 seriAddr = HAL_OAD_CODE_START/HAL_FLASH_WORD_SIZE;
  uint16 oset;
  //uint8 page=HAL_OAD_CODE_START/HAL_FLASH_PAGE_SIZE;
  uint8 page=HAL_OAD_RC_START/HAL_FLASH_PAGE_SIZE;
  char *decKey = "WorkForYourDream";
  uint32 *pVeriFlag;
  //preamble_t *tstPre;
  //uint8 test[12] = {0X0F,0XFE,0XAA,0XBB,0XCC,0XDD,0X00,0X03,0X00,0X03,0X00,0X03};
  //uint8 page=HAL_OAD_RC_START/HAL_FLASH_PAGE_SIZE;
  //uint8 buf[16];
 // char *g_decKey = "WorkForYourDream";
 //BYTE  IV[LENGTH_IV ] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  //uint8 buf[8] = {0};  

  HAL_BOARD_INIT();
#if HAL_OAD_XNV_IS_SPI
  //XNV_SPI_INIT(); 
#endif
  /* This is in place of calling HalDmaInit() which would require init of the other 4 DMA
   * descriptors in addition to just Channel 0.
   */
  HAL_DMA_SET_ADDR_DESC0( &dmaCh0 );
  HalFlashInit();

#if 0
  HalFlashRead(HAL_OAD_CRC_ADDR / HAL_FLASH_PAGE_SIZE,
               HAL_OAD_CRC_ADDR % HAL_FLASH_PAGE_SIZE,
               (uint8 *)crc, sizeof(crc));


  if (crc[0] != crc[1])
  {
    // If the CRC is erased or the RC code fails a sanity check, instantiate the DL code (again).
    if ((crc[0] == 0) || (crc[0] != crcCalc()))
    {
      dl2rc();

      /* If dl2rc() fails, a flawed image is allowed to run - 
       * maybe the damage is not fatal to OTA ops?
       */
    }
    else
    {
      crc[1] = crc[0];
      HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);
    }
  }
#endif

  //InitUart();
  //Uart_Send_String("boot",4);
  
  //P1DIR &=( ~0x04);
  //P1DIR |= 0x08;				//P1_3
  //P1DIR |= 0x08;
  //P1DIR |= 0x10;
  //P1DIR |= 0x18;
  //P1_3 = 0;
  //P1_4 = 0;
  P0SEL &= ~0x30;
  P0DIR &= 0xDF;
  //P0DIR |= 0x11;
  P0DIR |= 0x10;
  P0DIR |= 0x80;
  //P1DIR |= 0x82;
  P0_4 = 0;
  P1_7 = 1;
  //P1_0 = 0;
  //P1_1 = 0;//led
  //P0_0 = 0;//led
  //P0 &= ~0x20;
  //P0DIR |= 0x10;
  //P0_4 = 1;
  //P1DIR |= 0x80;
  //P0DIR |= 0x20;					//P05
  InitUart();
#ifdef OAD_AES_VERI
  AES_SETMODE(ECB);
#endif

  HAL_TURN_OFF_LED2();
  HAL_TURN_OFF_LED1();
  
#ifdef OAD_AES_VERI
  halAesLoadKeyOrInitVector(decKey, TRUE);
  
  HalFlashRead(CPUID_VERI_ADDR / HAL_FLASH_PAGE_SIZE,
               CPUID_VERI_ADDR % HAL_FLASH_PAGE_SIZE,
               veriBuf, sizeof(veriBuf));

  halAesEncrDecr((BYTE *)veriBuf, CPUID_VERI_LEN, (BYTE *)veriBuf/*, ucLDR_Message.com_data*/, DECRYPT); 
  
  pVeriFlag = (uint32*)veriBuf;
  if(VALID_INIT_SN==*pVeriFlag)
  {
  	putsDialog("Initial\r\n");

	
	//2015/09/13 what's wrong
	//dl2rc();
	//HalFlashErase(HAL_OAD_RC_START / HAL_FLASH_PAGE_SIZE );
	//HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);		//����������¼
	
  	*pVeriFlag = VALID_START_SN;						//first start 
  	MemReadRam((macRam_t *)TI_EXTADDR, (veriBuf+4), 8);
	halAesEncrDecr((BYTE *)veriBuf, CPUID_VERI_LEN, (BYTE *)veriBuf/*, ucLDR_Message.com_data*/, ENCRYPT); 
	HalFlashErase(CPUID_VERI_ADDR / HAL_FLASH_PAGE_SIZE);
	HalFlashWrite((CPUID_VERI_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)veriBuf, CPUID_VERI_LEN/4);	
  }else if(VALID_START_SN==*pVeriFlag)
  {
  	MemReadRam((macRam_t *)TI_EXTADDR, (veriBuf+12), 8);		//read mac iD
	if(FALSE==OAD_memcmp((veriBuf+4), (veriBuf+12), 8))
	{
		putsDialog("Verified Error");
		while(1);
		//asm("LJMP 0x1000\n");
	}
	putsDialog("Start\r\n");
  }else
  {
  	putsDialog("Verified Error");
	while(1);
  }
#endif
 //tstPre = (preamble_t*)test;
  //SendData(tstPre->len>>24);
 // SendData(tstPre->len>>16);
  //SendData(tstPre->len>>8);
  //SendData(tstPre->len);

  //HalFlashWrite((HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE), (uint8 *)crcTest, sizeof(crcTest)/4);
  
 //if(!P1_2)
  if(!P0_5)
  {

	//Uart_Send_String("boot",4);
	//putsDialog("boot\r\n");
        //P1DIR |= 0x18;
        //P1_4 = 0;
	
	while(1)
	{
		if(Message_Rcv())
		{	
			//P1_3 = !P1_3;
			ucLDR_Security_F = 0;
			ucLDR_Ret = 0x00;
			//Uart_Send_String("rcv",3);
#if 1
			switch(ucLDR_Message.protocol_command )
			{

				case AM1_IAP_READ_64BYTE:
#if 0
					//P1_3 = !P1_3;
                                   //     P1_4 = !P1_4;
				  	for (oset = 0; oset < HAL_FLASH_PAGE_SIZE; oset += 64)
				  	{
						HalFlashRead(page,oset,ucLDR_Message.com_data,64);
						ucLDR_Message.data_length=64;
						Message_Send();		
						//Uart_Send_String(buf,4);
					
				  	}
					page++;
#endif
				break;
/*
				case BOOT_FIRMWARE_VERSION:
					ucLDR_Message.com_data[0]=1;
					ucLDR_Message.com_data[1]=0;      
					ucLDR_Message.com_data[12]=5;	//for AM1_IAP
					ucLDR_Message.data_length=20;
					Message_Send();
					break;
					
				case AM1_IAP_READ_64BYTE:
					stCommand.Program_Lib.ram_adr	= &ucLDR_WorkRam;
					stCommand.Program_Lib.dst_adr	+= 64;
					if( stCommand.Program_Lib.dst_adr == 0x00 )
					stCommand.Program_Lib.dst_bank++;	
					stCommand.Program_Lib.bc		=  LDR_WRITE_SIZE;
					stCommand.Program_Lib.src_adr  =   &ucLDR_Message.com_data[0];
					ucLDR_Read_64Byte_Ram();
					ucLDR_Message.data_length=64;
					Message_Send();
					break;

						
				case AM1_IAP_READ_STATUS:
					stCommand.ReadStatus_Lib.ram_adr = (unsigned char*)&ucLDR_WorkRam;
					ucLDR_Ret=0;//test
					ucLDR_ReadStatus_Ram();//test
					ucLDR_Message.com_data[0]=ucLDR_Ret;// No Key	
					ucLDR_Message.data_length=2;
					Message_Send();
					stCommand.Program_Lib.dst_adr	= (unsigned char*)(LDR_WRITE_ADDRESS-64);	   
					stCommand.Program_Lib.dst_bank	= 0;	
					break;

					  
				case AM1_IAP_SECURITYCHECK:
		 			stCommand.Security_Lib.ram_adr	= (unsigned char*)&ucLDR_WorkRam;
			 		stCommand.Security_Lib.key_adr = (unsigned char*)&ucLDR_Message.com_data[0];
					ucLDR_Ret = 0x00;
					ucLDR_SecurityCheck_Ram();
					ucLDR_Message.com_data[0]=ucLDR_Ret;// No Key	
					ucLDR_Message.data_length=2;		
					Message_Send();
					stCommand.Program_Lib.dst_adr	= (unsigned char*)(LDR_WRITE_ADDRESS-64);	   
					stCommand.Program_Lib.dst_bank	= 0;	
				      break;
*/
						
	//=================================================================================
				case AM1_IAP_WRITE_PAGE:	
/*
					stCommand.Program_Lib.ram_adr	= &ucLDR_WorkRam;
					stCommand.Program_Lib.dst_adr	+= 64;
					if( stCommand.Program_Lib.dst_adr == 0x00 )
					stCommand.Program_Lib.dst_bank++;	
					stCommand.Program_Lib.bc		=  LDR_WRITE_SIZE;
					stCommand.Program_Lib.src_adr  =   &ucLDR_Message.com_data[0];
					ucLDR_Ret=0x00;//test	
					ucLDR_Program_Ram();//test
*/
					halAesEncrDecr((BYTE *)ucLDR_Message.com_data, STRING_LENGTH, (BYTE *)ucLDR_Message.com_data/*, ucLDR_Message.com_data*/, DECRYPT);
					//Uart_Send_String((BYTE *)buf,16);
					if ((seriAddr % (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)) == 0)
					{
						HalFlashErase(seriAddr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
					}
					//HalFlashWrite(addr++, ucLDR_Message.com_data, 1);
					HalFlashWrite(seriAddr, ucLDR_Message.com_data, ucLDR_Message.data_length/4);
					seriAddr+=(ucLDR_Message.data_length/4);
					delay(1000);
				    //ucLDR_Read_64Byte_Ram();
					ucLDR_Message.com_data[0]=0x00;
					ucLDR_Message.data_length=2;
					Message_Send();					
					break;

				case AM1_IAP_ERASE_DEVICE:
					ucLDR_Message.com_data[0]=0x55;
					ucLDR_Message.com_data[1]=0x00;
					ucLDR_Message.data_length=3;
					Message_Send();	
				break;
	//=================================================================================
/*
				case AM1_IAP_ERASE_DEVICE:		
					ucLDR_Security_F = 0;
					stCommand.ReadStatus_Lib.ram_adr = &ucLDR_WorkRam;
					ucLDR_Ret=0;//test
					ucLDR_ReadStatus_Ram();//test
					if( (ucLDR_Ret&0x01) == 0x01 )
						{
						ucLDR_Message.com_data[0]=0x55;// No Key	
						ucLDR_Security_F=1;
					     }
					else
						{
						ucLDR_Ret = 0x00;
			 			stCommand.Security_Lib.ram_adr	= &ucLDR_WorkRam;
			 			stCommand.Security_Lib.key_adr = (unsigned char*)&ucLDR_Message.com_data[0];
						ucLDR_SecurityCheck_Ram();
						if( ucLDR_Ret != 0x00 )
							{
							ucLDR_Message.com_data[0]=0xEE;   //Key Verify error
			
							}
						else
							{
							 ucLDR_Message.com_data[0]=0xaa ;//Key Verify Pass
		                                 ucLDR_Security_F=1;
						       }
						}

						 if(ucLDR_Security_F==1)
							{
								stCommand.Erase_Lib.ram_adr	= &ucLDR_WorkRam;
								ucLDR_Ret=0x00;//test
								ucLDR_Erase_Ram();//test
								ucLDR_Message.com_data[1]= ucLDR_Ret;
								#if 0
								if(ucLDR_Ret==0)
								{
								stCommand.Secter_Lib.ram_adr	= (unsigned char*)&ucLDR_WorkRam[0];
								ucLDR_Ret=0x00;//test
								ucLDR_BankCheck_Ram();  //test 
								ucLDR_Message.com_data[2]=ucLDR_Ret;
								}
								#endif
							}
					delay(60000);		
					stCommand.Program_Lib.dst_adr	= (unsigned char*)(0x4000-64);	   
					stCommand.Program_Lib.dst_bank	= 0;
					Message_Send();	
					break;
*/
//=================================================================================
				  default:
					//Message_Send();		
					break;
				}
#endif
		}
		//start_flag = 0;
		//InitUart();
		//Uart_Send_String("test",4);
		//P1_3 = !P1_3;

	}
  }else
  {

#if 1
  	HalFlashRead(HAL_OAD_CRC_ADDR / HAL_FLASH_PAGE_SIZE,
               HAL_OAD_CRC_ADDR % HAL_FLASH_PAGE_SIZE,
               (uint8 *)crc, sizeof(crc));

	if (crc[0] != crc[1])
	{
		SendData(crc[1]>>8);
		SendData(crc[1]&0xFF);
		if( crc[1]== crcCalc())
		{
			//putsDialog("CRC OK\r\n");
			dl2rc();
			//putsDialog("FW OK\r\n");
			crc[0] = crc[1];
			//HalFlashErase(HAL_OAD_RC_START / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
			//HalFlashErase(HAL_OAD_RC_START / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
			HalFlashErase(HAL_OAD_RC_START / HAL_FLASH_PAGE_SIZE );
			HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);
		}else
		{
			//Add code to avoiding repeat crc process
			crc[0] = crc[1];
			HalFlashErase(HAL_OAD_RC_START / HAL_FLASH_PAGE_SIZE );
			HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);
			//putsDialog("CRC error\r\n");
			//SendData(crc[0]>>8);
			//SendData(crc[0]&0xff);
			//SendData(crc[1]>>8);
			//SendData(crc[1]&0xff);
		}
	}

	HAL_TURN_ON_LED2();
	HAL_TURN_OFF_LED1();
#if 0
	  if (crc[0] != crc[1])
	{
		// If the CRC is erased or the RC code fails a sanity check, instantiate the DL code (again).
		if ((crc[0] == 0) || (crc[0] != crcCalc()))
		{
			dl2rc();

			/* If dl2rc() fails, a flawed image is allowed to run - 
			* maybe the damage is not fatal to OTA ops?
			*/
		}
		else
		{
			crc[1] = crc[0];
			HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);
		}
	}
#endif
#endif

	// Simulate a reset for the Application code by an absolute jump to location 0x0800.
	//asm("LJMP 0x800\n");
	//Uart_Send_String("jump",4);
	//asm("LJMP 0x2000\n");
	//putsDialog("jump");
	//asm("LJMP 0x1000\n");
	asm("LJMP 0x2000\n");
	//asm("LJMP 0x2000\n");
	//asm("LJMP 0x1000\n");
	//asm("LJMP 0x5000\n");
  }
  
}

static void MemReadRam(macRam_t * pRam, uint8 * pData, uint8 len)
{
  while (len)
  {
    len--;
    *pData = *pRam;
    pRam++;
    pData++;
  }
}

static uint8 OAD_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len )
{
  const uint8 GENERIC *pSrc1;
  const uint8 GENERIC *pSrc2;

  pSrc1 = src1;
  pSrc2 = src2;

  while ( len-- )
  {
    if( *pSrc1++ != *pSrc2++ )
      return FALSE;
  }
  return TRUE;
}

static void InitUart(void)
{
    //CLKCONCMD &= ~0x40; // ����ϵͳʱ��ԴΪ 32MHZ����
    //while(CLKCONSTA & 0x40);                     // �ȴ������ȶ� 
    //CLKCONCMD &= ~0x47;                          // ����ϵͳ��ʱ��Ƶ��Ϊ 32MHZ

    PERCFG &= ~0x01;        //λ��1 P0�� 
    P0SEL |= 0x0c;        //P0_2,P0_3,P0_4,P0_5��������,�ڶ����� 
    //P0DIR |= 0x04;
   // P0DIR &= ~0x08;
    //P0DIR |= 0x08;
    //P2DIR &= ~0XC0;      //P0 ������ΪUART0 �����ȼ�
    //PERCFG &= ~(BV(0)); //ѡ��P0�˿�uart
    //P0SEL |=BV(5)|BV(4)|BV(3)|BV(2);
    //P2DIR &=~(BV(7)|BV(6));

    U0CSR |= 0x80;       //UART ��ʽ 
    U0GCR = 11;         //U0GCR��U0BAUD���     
    U0BAUD = 216;       // ��������Ϊ115200 
    UTX0IF = 0;          //UART0 TX �жϱ�־��ʼ��λ1  ���շ�ʱ��
    U0CSR |= 0X40;       //������� 
   // IEN0 |= 0x84;        // �����жϣ������ж�    

   URX0IF = 0;

  // P1_3 = !P1_3;
}

static void SendData(char dat)
{
	U0DBUF = dat;
	while(!UTX0IF);
	UTX0IF = 0; 	
}

#if 1
static void putsDialog(const char *str)
{
	 //UartDecode(str);
	 while(*str != '\0')
	{
        	SendData(*str++);
    }
}
#endif

#ifdef _FORMAT_PRINT_


static char *_itoa(long n, int base)
 {
  	char *p;
 	int minus;
 	static char buf[16];

//	puts_("enter _itoa\n");
//	put_hex(n);

 	p = &buf[16];
 	*--p =  0;
 	if (n < 0) 
	{
  		minus = 1;
  		n = -n;
 	}
 	else
  		minus = 0;


 	if (n == 0)
	{
  		*--p = '0';
	}
 	else
  		while (n > 0) 
		{  
	//		puts_("_itoa ");
   			*--p = "0123456789abcdef"[n % base];
	//		putchar(*p);
   			n /= base;
  		}

 	if (minus)
  		*--p = '-';

 	return p;
}
/*
int printfDig( long n, int base )
{
	return Tprintf( _itoa( n, base ) );
	put_byte(0x0D);
    put_byte(0x0A);	
}
*/
static int Printf(const char *fmt, ...)
{
	//int i;
	const char *s;
 	int d;
	char buf[16];
	char *p = buf;
//	char buf[16];
//	char *p = buf;
 	va_list ap;

    va_start(ap, fmt);
	//i = 16;

    while (*fmt) 
	{
        if (*fmt != '%') 
		{
            SendData(*fmt++);
            continue;
        }
		
		switch(*++fmt)
		{
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
				{
                    SendData(*s);
                }
                break;
            case 'd':			
                d = va_arg(ap, int);
				//d>>=8;
                p = _itoa(d, 10);
                for (s = p; *s; s++) 
				{
                    SendData(*s);
                }

				break;

            case 'x':
			case 'X':
                d = va_arg(ap, int);
                p = _itoa(d, 16);
                for (s = p; *s; s++) 
				{
                    SendData(*s);
                }
                break;
             //Add other specifiers here...              
            default:  
                putsDialog(fmt);
                break;
        } 
        fmt++;
    }
    va_end(ap);

    return 1;   /* Dummy return value */
} 
#endif

static void delay(unsigned int de)
{
	//while(de--);
	 uint16 i,j;
	 for(i=de;i>0;i--)
	   for(j=587;j>0;j--);
}

static unsigned char RcvData(void)
{
	unsigned char count1=0;
	unsigned char count2=0;

	while( !URX0IF )
	{
		count1++;
		if(count1==252)
		{
		count1=0;
		count2++;
		}
		if((count2==252)&&(start_flag==1))
		{
			start_flag=0;
			
			return 0;
		}
	}
	URX0IF = 0;
	return U0DBUF;
}


static unsigned char Message_Rcv(void)
{
	unsigned char	ucdata;
	unsigned char 	rcvdat;
	unsigned char	uci;
	start_flag=0;

	//P1_3 = !P1_3;
	
     ucdata=RcvData();
	if(ucdata!=0x7E)
	   return 0;
	start_flag=1;
	ucdata=RcvData();
	ucLDR_Message.com_number=  ucdata;
	ucLDR_Message.msg_cks=ucdata;
	
      ucdata=RcvData();
	if(start_flag==0)
		return 0;
	ucLDR_Message.protocol_command=  ucdata;
	ucLDR_Message.msg_cks+=ucdata;	
	
	ucdata=RcvData();
	if(start_flag==0)
		 return 0;
	ucLDR_Message.no_used=  ucdata;
	ucLDR_Message.msg_cks+=ucdata;	
	
	ucdata=RcvData();
	if(start_flag==0)
		 return 0;
	ucLDR_Message.data_length=  ucdata;
	ucLDR_Message.msg_cks+=ucdata;
	
	for( uci=0 ; uci<ucLDR_Message.data_length ; uci++ )
	{
		ucdata = RcvData();
		if(start_flag==0)
			 return 0;
		ucLDR_Message.com_data[uci] = ucdata;
		ucLDR_Message.msg_cks+=ucdata;
	}
	ucdata = RcvData();
	rcvdat = ucLDR_Message.msg_cks;
	if(start_flag==0)
		 return 0;
	//if(ucLDR_Message.msg_cks==ucdata)
	if(rcvdat==ucdata)
      return 1;
	else
	return 0;	

}


/*********************************************************************
 * @fn      dl2rc
 *
 * @brief   Copy the DL image to the RC image location.
 *
 *  NOTE:   Assumes that DL image ends on a flash word boundary.
 *
 * @param   None.
 *
 * @return  None.
 *********************************************************************/
#if 1
static void Message_Send(void)
{
	unsigned char  uci;
	
	SendData(0x7E);
    
	ucLDR_Message.msg_cks =ucLDR_Message.com_number;
    SendData(ucLDR_Message.com_number);
    
    ucLDR_Message.msg_cks+=ucLDR_Message.protocol_command;
	SendData(ucLDR_Message.protocol_command);  
	
    ucLDR_Message.msg_cks+=ucLDR_Message.no_used;
	SendData(ucLDR_Message.no_used); 	
		
    ucLDR_Message.msg_cks+=ucLDR_Message.data_length;
	SendData(ucLDR_Message.data_length); 
		

    for(uci =0;uci<ucLDR_Message.data_length;uci++)
    {
       ucLDR_Message.msg_cks+=ucLDR_Message.com_data[uci];  
       SendData(ucLDR_Message.com_data[uci]); 	
	 }
     ucLDR_Message.msg_cks+=0;
	 SendData(ucLDR_Message.msg_cks); 		
}

#endif

#ifdef OAD_AES_VERI
static void halAesLoadKeyOrInitVector(BYTE* pData, BOOL key)
{
   UINT8 i;

   // Checking whether to load a key or an initialisation vector.
   if(key){
      AES_SET_ENCR_DECR_KEY_IV(AES_LOAD_KEY);
   }
   else {
      AES_SET_ENCR_DECR_KEY_IV(AES_LOAD_IV);
   }
   // Starting loading of key or vector.
   AES_START();

   // loading the data (key or vector)
   for(i = 0; i < 16; i++){
      ENCDI = pData[i];
   }
}


static void halAesEncrDecr(BYTE *pDataIn, UINT16 length, BYTE *pDataOut, /*BYTE *pInitVector, */BOOL decr)
{
   UINT16  i;
   UINT8   j, k;
   BYTE    mode;
   UINT16  nbrOfBlocks;
   UINT16  convertedBlock;
   UINT8   delay;

   nbrOfBlocks = length / 16;
   mode = 0;

   if( (length % 16) != 0){
      // length not multiplum of 16, convert one block extra with zeropadding
      nbrOfBlocks++;
   }

   // Loading the IV.
   //halAesLoadKeyOrInitVector(pInitVector, FALSE);

   // Starting either encryption or decryption
   if(decr){
      AES_SET_ENCR_DECR_KEY_IV(AES_DECRYPT);
   } else {
      AES_SET_ENCR_DECR_KEY_IV(AES_ENCRYPT);
   }

   // Getting the operation mode.
   mode = ENCCS & 0x70;

   for(convertedBlock = 0; convertedBlock < nbrOfBlocks; convertedBlock++){
      // Starting the conversion.
      AES_START();

      i = convertedBlock * 16;
      // Counter, Output Feedback and Cipher Feedback operates on 4 bytes and not 16 bytes.
      if((mode == CFB) || (mode == OFB) || (mode == CTR))
	 {
/*
         for(j = 0; j < 4; j++){
            // Writing the input data
            // Zeropadding the remainder of the block
            for(k = 0; k < 4; k++){
               ENCDI = ((i + 4*j + k < length) ? pDataIn[i + 4*j + k] : 0x00 );
            }
            // wait for data ready
            delay = DELAY;
            while(delay--);
            // Read out data for every 4th byte
            for(k = 0; k < 4; k++){
               pDataOut[i + 4*j + k] = ENCDO;
            }
         }
*/
      }
      else if(mode == CBC_MAC){/*
         // Writing the input data
         // Zeropadding the remainder of the block
         for(j = 0; j < 16; j++){
            ENCDI = ((i + j < length) ? pDataIn[i + j] : 0x00 );
         }
         // The last block of the CBC-MAC is computed by using CBC mode.
         if(convertedBlock == nbrOfBlocks - 2){
            AES_SETMODE(CBC);
            delay = DELAY;
            while(delay--);
         }
         // The CBC-MAC does not produce an output on the n-1 first blocks
         // only the last block is read out
         else if(convertedBlock == nbrOfBlocks - 1){
            // wait for data ready
            delay = DELAY;
            while(delay--);
            for(j = 0; j < 16; j++){
               pDataOut[j] = ENCDO;
            }
         }*/
      }
      else{
         // Writing the input data
         // Zeropadding the remainder of the block
         for(j = 0; j < 16; j++){
            ENCDI = ((i+j < length) ? pDataIn[i+j] : 0x00 );
         }
         // wait for data ready
         delay = DELAY;
         while(delay--);
         // Read out data
         for(j = 0; j < 16; j++){
            pDataOut[i+j] = ENCDO;
         }
      }
   }
}




#endif

 #ifdef _USER_OAD_FLASH_WRITE_
static void dl2rc(void)
{
  preamble_t preamble;
  uint32 oset;
  //uint16 addr = HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE;				//0x0800/4		user code start address
  uint16 addr = HAL_OAD_CODE_START / HAL_FLASH_WORD_SIZE;
  uint8 buf[4];

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_DL);
  //putsDialog("total len %x\r\n",preamble.len);
  //SendData(preamble.len>>24);
  //SendData((preamble.len>>16)&0xff);
  //SendData((preamble.len>>8)&0xff);
  //SendData(preamble.len&0xff);
  //preamble.len = 65536;

  for (oset = 0; oset < preamble.len; oset += HAL_FLASH_WORD_SIZE)
  {
    HalOADRead(oset, buf, HAL_FLASH_WORD_SIZE, HAL_OAD_DL);
    if ((addr % (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)) == 0)
    {
      HalFlashErase(addr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
      HAL_TOGGLE_LED1();
    }
    HalFlashWrite(addr++, buf, 1);
  }
  HAL_TURN_OFF_LED1();
}  

/*********************************************************************
 * @fn      crcCalc
 *
 * @brief   Run the CRC16 Polynomial calculation over the RC image.
 *
 * @param   None.
 *
 * @return  The CRC16 calculated.
 */
static uint16 crcCalc(void)
{
  preamble_t preamble;
  uint32 oset;
  uint16 crc = 0;
  uint8 tmp;

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);

  if(preamble.len>HAL_OAD_RC_START)
  {
	return 0;
  }

  // Run the CRC calculation over the active body of code.
  for (oset = 0; oset < preamble.len; oset++)
  {
 #if 1
    if ((oset < HAL_OAD_CRC_OSET) || (oset >= HAL_OAD_CRC_OSET+4))
    {
      uint8 buf;
      HalOADRead(oset, &buf, 1, HAL_OAD_RC);
      crc = runPoly(crc, buf);
    }
    if(0==(oset%HAL_FLASH_PAGE_SIZE))
    {
	 HAL_TOGGLE_LED2();
    }
    //HAL_TURN_OFF_LED2();
#else
	uint8 buf;
	HalOADRead(oset, &buf, 1, HAL_OAD_RC);
	SendData(buf);
#endif

  }

  // IAR note explains that poly must be run with value zero for each byte of crc.
  crc = runPoly(crc, 0);
  crc = runPoly(crc, 0);

  SendData(crc>>8);
  SendData(crc&0xff);

  HAL_TURN_OFF_LED2();

  return crc;
}
#endif
//#endif

/*********************************************************************
 * @fn      runPoly
 *
 * @brief   Run the CRC16 Polynomial calculation over the byte parameter.
 *
 * @param   crc - Running CRC calculated so far.
 * @param   val - Value on which to run the CRC16.
 *
 * @return  crc - Updated for the run.
 */
#if _USER_OAD_FLASH_WRITE_
static uint16 runPoly(uint16 crc, uint8 val)
{
  const uint16 poly = 0x1021;
  uint8 cnt;

  for (cnt = 0; cnt < 8; cnt++, val <<= 1)
  {
    uint8 msb = (crc & 0x8000) ? 1 : 0;

    crc <<= 1;
    if (val & 0x80)  crc |= 0x0001;
    if (msb)         crc ^= poly;
  }

  return crc;
}

/*********************************************************************
 * @fn      HalOADChkDL
 *
 * @brief   Run the CRC16 Polynomial calculation over the DL image.
 *
 * @param   dlImagePreambleOffset - Offset into the monolithic DL image to read the preamble.
 *
 * @return  SUCCESS or FAILURE.
 *********************************************************************/
 #if 0
uint8 HalOADChkDL(uint8 dlImagePreambleOffset)
{
  preamble_t preamble;
  uint32 oset;
  uint16 crc = 0, crc2;

  HalOADRead(dlImagePreambleOffset, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_DL);

  // Run the CRC calculation over the downloaded image.
  for (oset = 0; oset < preamble.len; oset++)
  {
    if ((oset < HAL_OAD_CRC_OSET) || (oset >= HAL_OAD_CRC_OSET+4))
    {
      uint8 buf;
      HalOADRead(oset, &buf, 1, HAL_OAD_DL);
      crc = runPoly(crc, buf);
    }
  }

  // IAR note explains that poly must be run with value zero for each byte of crc.
  crc = runPoly(crc, 0);
  crc = runPoly(crc, 0);

  HalOADRead(HAL_OAD_CRC_OSET, (uint8 *)&crc2, sizeof(crc2), HAL_OAD_DL);
  return (crc2 == crc) ? SUCCESS : FAILURE;
}
 #endif

/*********************************************************************
 * @fn      HalOADInvRC
 *
 * @brief   Invalidate the active image so that the boot code will instantiate the DL image on the
 *          next reset.
 *
 * @param   None.
 *
 * @return  None.
 *********************************************************************/
void HalOADInvRC(void)
{
  uint16 crc[2] = {0,0xFFFF};
  HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);
}

/*********************************************************************
 * @fn      HalOADRead
 *
 * @brief   Read from the storage medium according to image type.
 *
 * @param   oset - Offset into the monolithic image.
 * @param   pBuf - Pointer to the buffer in which to copy the bytes read.
 * @param   len - Number of bytes to read.
 * @param   type - Which image: HAL_OAD_RC or HAL_OAD_DL.
 *
 * @return  None.
 *********************************************************************/
void HalOADRead(uint32 oset, uint8 *pBuf, uint16 len, image_t type)
{
#if 0
  if (HAL_OAD_RC != type)
  {
#if HAL_OAD_XNV_IS_INT
    preamble_t preamble;

    HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);
    //oset += HAL_OAD_RC_START + preamble.len;
    oset += HAL_OAD_RC_START + HAL_OAD_DL_OSET;
#elif HAL_OAD_XNV_IS_SPI
    oset += HAL_OAD_DL_OSET;
    HalSPIRead(oset, pBuf, len);
    return;
#endif
  }
  else
  {
    oset += HAL_OAD_RC_START;
  }
 #endif

  oset += HAL_OAD_RC_START;
  HalFlashRead(oset / HAL_FLASH_PAGE_SIZE, oset % HAL_FLASH_PAGE_SIZE, pBuf, len);
}

/*********************************************************************
 * @fn      HalOADWrite
 *
 * @brief   Write to the storage medium according to the image type.
 *
 *  NOTE:   Destructive write on page boundary! When writing to the first flash word
 *          of a page boundary, the page is erased without saving/restoring the bytes not written.
 *          Writes anywhere else on a page assume that the location written to has been erased.
 *
 * @param   oset - Offset into the monolithic image, aligned to HAL_FLASH_WORD_SIZE.
 * @param   pBuf - Pointer to the buffer in from which to write.
 * @param   len - Number of bytes to write. If not an even multiple of HAL_FLASH_WORD_SIZE,
 *                remainder bytes are overwritten with garbage.
 * @param   type - Which image: HAL_OAD_RC or HAL_OAD_DL.
 *
 * @return  None.
 *********************************************************************/
void HalOADWrite(uint32 oset, uint8 *pBuf, uint16 len, image_t type)
{
  if (HAL_OAD_RC != type)
  {
#if HAL_OAD_XNV_IS_INT
    preamble_t preamble;

    HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);
    //oset += HAL_OAD_RC_START + preamble.len;
    oset += HAL_OAD_RC_START + HAL_OAD_DL_OSET;
#elif HAL_OAD_XNV_IS_SPI
    oset += HAL_OAD_DL_OSET;
    HalSPIWrite(oset, pBuf, len);
    return;
#endif
  }
  else
  {
    oset += HAL_OAD_RC_START;
  }

  if ((oset % HAL_FLASH_PAGE_SIZE) == 0)
  {
    HalFlashErase(oset / HAL_FLASH_PAGE_SIZE);
  }

  HalFlashWrite(oset / HAL_FLASH_WORD_SIZE, pBuf, len / HAL_FLASH_WORD_SIZE);
}

#if HAL_OAD_XNV_IS_INT
/*********************************************************************
 * @fn      HalOADAvail
 *
 * @brief   Determine the space available for downloading an image.
 *
 * @param   None.
 *
 * @return  Number of bytes available for storing an OAD image.
 *********************************************************************/
uint32 HalOADAvail(void)
{
  /*
  preamble_t preamble;

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);
  return HAL_OAD_DL_MAX - preamble.len;
   */
  return HAL_OAD_DL_MAX - HAL_OAD_DL_OSET;
}

#elif HAL_OAD_XNV_IS_SPI

/*********************************************************************
 * CONSTANTS
 */

#define XNV_STAT_CMD  0x05
#define XNV_WREN_CMD  0x06
#define XNV_WRPG_CMD  0x0A
#define XNV_READ_CMD  0x0B

#define XNV_STAT_WIP  0x01

/*********************************************************************
 * @fn      xnvSPIWrite
 *
 * @brief   SPI write sequence for code size savings.
 *
 * @param   ch - The byte to write to the SPI.
 *
 * @return  None.
 *********************************************************************/
static void xnvSPIWrite(uint8 ch);
static void xnvSPIWrite(uint8 ch)
{
  XNV_SPI_TX(ch);
  XNV_SPI_WAIT_RXRDY();
}

/*********************************************************************
 * @fn      HalOADAvail
 *
 * @brief   Determine the space available for downloading an image.
 *
 * @param   None.
 *
 * @return  Number of bytes available for storing an OAD image.
 *********************************************************************/
uint32 HalOADAvail(void)
{
  return HAL_OAD_DL_MAX - HAL_OAD_DL_OSET;
}

/*********************************************************************
 * @fn      HalSPIRead
 *
 * @brief   Read from the external NV storage via SPI.
 *
 * @param   addr - Offset into the external NV.
 * @param   pBuf - Pointer to the buffer in which to copy the bytes read from external NV.
 * @param   len - Number of bytes to read from external NV.
 *
 * @return  None.
 *********************************************************************/
 #if 0
static void HalSPIRead(uint32 addr, uint8 *pBuf, uint16 len)
{
#if !HAL_OAD_BOOT_CODE
  uint8 shdw = P1DIR;
  halIntState_t his;
  HAL_ENTER_CRITICAL_SECTION(his);
  P1DIR |= BV(3);
#endif

  XNV_SPI_BEGIN();
  do {
    xnvSPIWrite(XNV_STAT_CMD);
  } while (XNV_SPI_RX() & XNV_STAT_WIP);
  XNV_SPI_END();
  asm("NOP"); asm("NOP");

  XNV_SPI_BEGIN();
  xnvSPIWrite(XNV_READ_CMD);
  xnvSPIWrite(addr >> 16);
  xnvSPIWrite(addr >> 8);
  xnvSPIWrite(addr);
  xnvSPIWrite(0);

  while (len--)
  {
    xnvSPIWrite(0);
    *pBuf++ = XNV_SPI_RX();
  }
  XNV_SPI_END();

#if !HAL_OAD_BOOT_CODE
  P1DIR = shdw;
  HAL_EXIT_CRITICAL_SECTION(his);
#endif
}
 #endif

/*********************************************************************
 * @fn      HalSPIWrite
 *
 * @brief   Write to the external NV storage via SPI.
 *
 * @param   addr - Offset into the external NV.
 * @param   pBuf - Pointer to the buffer in from which to write bytes to external NV.
 * @param   len - Number of bytes to write to external NV.
 *
 * @return  None.
 *********************************************************************/
static void HalSPIWrite(uint32 addr, uint8 *pBuf, uint16 len)
{
  uint8 cnt;
#if !HAL_OAD_BOOT_CODE
  uint8 shdw = P1DIR;
  halIntState_t his;
  HAL_ENTER_CRITICAL_SECTION(his);
  P1DIR |= BV(3);
#endif

  while (len)
  {
    XNV_SPI_BEGIN();
    do {
      xnvSPIWrite(XNV_STAT_CMD);
    } while (XNV_SPI_RX() & XNV_STAT_WIP);
    XNV_SPI_END();
    asm("NOP"); asm("NOP");

    XNV_SPI_BEGIN();
    xnvSPIWrite(XNV_WREN_CMD);
    XNV_SPI_END();
    asm("NOP"); asm("NOP");

    XNV_SPI_BEGIN();
    xnvSPIWrite(XNV_WRPG_CMD);
    xnvSPIWrite(addr >> 16);
    xnvSPIWrite(addr >> 8);
    xnvSPIWrite(addr);

    // Can only write within any one page boundary, so prepare for next page write if bytes remain.
    cnt = 0 - (uint8)addr;
    if (cnt)
    {
      addr += cnt;
    }
    else
    {
      addr += 256;
    }

    do
    {
      xnvSPIWrite(*pBuf++);
      cnt--;
      len--;
    } while (len && cnt);
    XNV_SPI_END();
  }

#if !HAL_OAD_BOOT_CODE
  P1DIR = shdw;
  HAL_EXIT_CRITICAL_SECTION(his);
#endif
}
#else
#error Invalid Xtra-NV for OAD.
#endif

#endif

/**************************************************************************************************
*/
