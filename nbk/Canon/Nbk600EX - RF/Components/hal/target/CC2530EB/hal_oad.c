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
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
#include <stdio.h>
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

#define VALID_INIT_SN				(978137684l)
#define VALID_START_SN				(260636838l)

typedef volatile unsigned char XDATA macRam_t;
#define TI_EXTADDR  PXREG( 0x780C )  /* Pointer to Start of Flash Information Page          */

// Where _mode_ is one of
#define AES_ENCRYPT     0x00;
#define AES_DECRYPT     0x02;
#define AES_LOAD_KEY    0x04;
#define AES_LOAD_IV     0x06;

#define LENGTH_IV  16
#define LENGTH_KEY 16

#define STRING_LENGTH       16


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

#if HAL_OAD_XNV_IS_SPI
//static void HalSPIRead(uint32 addr, uint8 *pBuf, uint16 len);
//static void HalSPIWrite(uint32 addr, uint8 *pBuf, uint16 len);
static unsigned char RcvData(void);
static unsigned char Message_Rcv(void);
static void Message_Send(void);
static void SendData(char dat);
static void delay(unsigned int de);
static void halAesLoadKeyOrInitVector(BYTE* pData, BOOL key);
static void halAesEncrDecr(BYTE *pDataIn, UINT16 length, BYTE *pDataOut, /*BYTE *pInitVector, */BOOL decr);
static void MemReadRam(macRam_t * pRam, uint8 * pData, uint8 len);
static uint8 OAD_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );
static void putsDialog(const char *str);
#endif

//char g_cupIdRst[16] = {0x08,0x3f,0x37,0x9f,0xb6,0xe6,0x5d,0x1f,0x3d,0xb9,0xdb,0x9c,0x4f,0xf3,0x6f,0xf0};

//#define DEBUG_TRACE

#if HAL_OAD_BOOT_CODE
static void dl2rc(void);
static uint16 crcCalc(void);
static void InitUart(void);
static void Uart_Send_String(char *Data,int len);
#endif

#ifdef DEBUG_TRACE
static void PutChar(const char Tbyte);
static void putsStr(const char *str);
static char *mac_itoa(long n, int base);
int Printf(const char *fmt, ...);
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

// char *g_decKey = "WorkForYourDream";
//    char *g_decKey = "MakeTomorrowBest";
 //BYTE  IV[LENGTH_IV ] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/****************

:101FF000e560f3070687996742fdfbc684bdc655B9
:100FF000e560f3070687996742fdfbc684bdc655C9
e5 60 f3 07 06 87 99 67 42 fd fb c6 84 bd c6 55

938
*****************/

/********************************************/
//F0 02 2D 76 AA 9C C7 D7 2A B3 24 53 98 B9 62 93
//:101FF000F0022D76AA9CC7D72AB3245398B96293CE
//
/********************************************/


#pragma location="NEAR_CODE"
//char g_cupIdRst[16] = {0xE8,0xF7,0x0C,0x78,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};

void main(void)
{
  //uint16 crc[2];
  //uint8 g_extAddr[8] = {0x38,0x15,0xC6,0xFE,0xFF,0x17,0x87,0x88};
  static char cpuIdKey[16] = {0x24,0x98,0xb7,0xA0,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};
  //char cpuIdRst[16] = {0xE8,0xF7,0x0C,0x78,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};
  char cpuIdRst[16]  = {0x3f ,0xdd ,0xfb ,0xb7 ,0x20 ,0xb1 ,0x95 ,0x89 ,0x75 ,0xd6 ,0x6e ,0x92 ,0x3f ,0xf7 ,0xf0 ,0xad};
  //uint8 veriBuf[16];
  uint8 veriBuf[20];
  uint16 addr = HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE;
  uint16 oset;
  uint8 page=HAL_OAD_RC_START/HAL_FLASH_PAGE_SIZE;
  //char *decKey = "WorkForYourDream";
  //char *g_decKey = "MakeTomorrowBest";
  char *decKey = "MakeTomorrowBest";
  uint8 buf[16];
  uint32 *pVeriFlag;
  uint8 i = 0;
  uint16 *cpuIdAddr;
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

#ifdef _USER_OAD_FLASH_WRITE_
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
  P0DIR &= 0xDF;
  P0SEL&=~(1<<5);//设置P0_5为一般IO口功能
  P0DIR&=~(1<<5);//设置P0_5为输入功能
  P0INP&=~(1<<5);//设置为上拉
  //P0DIR |= 0x20;					//P05
  InitUart();
  AES_SETMODE(ECB);
  //halAesLoadKeyOrInitVector(g_decKey, TRUE);
  //halAesLoadKeyOrInitVector(decKey, TRUE);

  putsDialog("BOOT\r\n");

#ifdef OAD_AES_VERI
  //halAesLoadKeyOrInitVector(decKey, TRUE);
#if 1
//****************************************************************
// encrpto CPUID
//****************************************************************
  halAesLoadKeyOrInitVector(cpuIdKey, TRUE);
  //halAesEncrDecr(cpuIdRst, 16, cpuIdRst, ENCRYPT);
  //putsDialog("key enc\r\n");
  //for(i=0;i<16;i++)
  //{
  //    Printf("%x ",cpuIdRst[i]);
  //}
  //putsDialog("\r\nkey dec\r\n");
  halAesEncrDecr(cpuIdRst, 16, cpuIdRst, DECRYPT);
  //for(i=0;i<16;i++)
  //{
      //Printf("%x ",cpuIdRst[i]);
  //}
  //putsDialog("\r\n");

  cpuIdAddr = (uint16*)(cpuIdRst+2);

  //Printf("cpuIdAddr %x\r\n",*cpuIdAddr);
#endif
//****************************************************************
// end
//****************************************************************
// resume the decrypto key
  halAesLoadKeyOrInitVector(decKey, TRUE);
//****************************************************************
// end
//****************************************************************

  MemReadRam((macRam_t *)TI_EXTADDR, veriBuf, 8);                             // initialize the veriBuf
  
  HalFlashRead(CPUID_VERI_ADDR / HAL_FLASH_PAGE_SIZE,
               CPUID_VERI_ADDR % HAL_FLASH_PAGE_SIZE,
               veriBuf, sizeof(veriBuf));

  halAesEncrDecr((BYTE *)veriBuf, CPUID_VERI_LEN, (BYTE *)veriBuf/*, ucLDR_Message.com_data*/, DECRYPT); 
  
  pVeriFlag = (uint32*)veriBuf;
  if(VALID_INIT_SN==*pVeriFlag)
  {
  	putsDialog("Initial\r\n");

	//dl2rc();
	//HalFlashErase(HAL_OAD_RC_START / HAL_FLASH_PAGE_SIZE );
	//HalFlashWrite((HAL_OAD_CRC_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)crc, 1);		//不能批量烧录
	
  	*pVeriFlag = VALID_START_SN;						//first start 
  	//MemReadRam((macRam_t *)TI_EXTADDR, (veriBuf+4), 8);
  	MemReadRam((macRam_t *)(PXREG(*cpuIdAddr)),(veriBuf+4), 8);
//	 test for read cpuid
//	putsDialog("cpu id\r\n");
//	for(i=0;i<8;i++)
//	{
//	    Printf("%x ",veriBuf[i]);
//	}
//	putsDialog("\r\n");
//
	halAesEncrDecr((BYTE *)veriBuf, CPUID_VERI_LEN, (BYTE *)veriBuf/*, ucLDR_Message.com_data*/, ENCRYPT); 
	HalFlashErase(CPUID_VERI_ADDR / HAL_FLASH_PAGE_SIZE);
	HalFlashWrite((CPUID_VERI_ADDR / HAL_FLASH_WORD_SIZE), (uint8 *)veriBuf, CPUID_VERI_LEN/4);	
  }else if(VALID_START_SN==*pVeriFlag)
  {
  	//MemReadRam((macRam_t *)TI_EXTADDR, (veriBuf+12), 8);		//read mac iD
  	MemReadRam((macRam_t *)(PXREG(*cpuIdAddr)),(veriBuf+12), 8);
	if(FALSE==OAD_memcmp((veriBuf+4), (veriBuf+12), 8))
	{
		//putsDialog("Verified Error");
		//while(1);
		for(oset=0;oset<1000;oset++)
		{
			delay(1000);
		}
		asm("LJMP 0x0000\n");
	}
	putsDialog("Start\r\n");
  }else
  {
  	//putsDialog("Verified Error");
	//while(1);
	for(oset=0;oset<1000;oset++)
	{
		delay(1000);
	}
	asm("LJMP 0x0000\n");
  }

#endif
  
 //if(!P1_2)
  if(!P0_5)
  {

	Uart_Send_String("boot",4);
        P1DIR |= 0x18;
        P1_4 = 0;
	
	while(1)
	{
                //LED_RED_TOGGLE();
		if(Message_Rcv())
		{	
			//P1_3 = !P1_3;
			LED_RED_TOGGLE();
			ucLDR_Security_F = 0;
			ucLDR_Ret = 0x00;
			//Uart_Send_String("rcv",3);
#if 1
			switch(ucLDR_Message.protocol_command )
			{

				case AM1_IAP_READ_64BYTE:
#if 1
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
					if(addr>=(HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE))
					{
						if ((addr % (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)) == 0)
						{
							HalFlashErase(addr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
						}
						//HalFlashWrite(addr++, ucLDR_Message.com_data, 1);
						HalFlashWrite(addr, ucLDR_Message.com_data, ucLDR_Message.data_length/4);
						addr+=(ucLDR_Message.data_length/4);
						delay(1000);
					    //ucLDR_Read_64Byte_Ram();
						ucLDR_Message.com_data[0]=0x00;
					}else
					{
						ucLDR_Message.com_data[0]=0x01;
					}
					ucLDR_Message.data_length=2;
					Message_Send();					
					break;

				case AM1_IAP_RST_POINT:
					addr = HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE;
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
  	Uart_Send_String("jump",4);
	asm("LJMP 0x2000\n");
	//asm("LJMP 0x800\n");
	//asm("LJMP 0x2000\n");
	//asm("LJMP 0x1000\n");
	//asm("LJMP 0x5000\n");
  }
  
  //Uart_Send_String("boot",4);
  /*
  for(page=0;page<10;page++)
  {
  	for (oset = 0; oset < 512; oset += HAL_FLASH_WORD_SIZE)
  	{
		HalFlashRead(page,oset,buf,4);
		Uart_Send_String(buf,4);
	
  	}
  }*/
  //HalFlashInit();
  //HalFlashWrite(addr++, buf, 1);

  //HalFlashErase(addr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
  //HalFlashWrite(addr, g_extAddr, 1);
  //HalFlashWrite(addr+1, g_extAddr+4, 2);
  //HalFlashRead(addr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE),0,g_extAddr,8);
  //Uart_Send_String(g_extAddr,8);
  
  // Simulate a reset for the Application code by an absolute jump to location 0x0800.
  //asm("LJMP 0x800\n");
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

static void SendData(char dat)
{
	U0DBUF = dat;
	while(!UTX0IF);
	UTX0IF = 0; 	
}

static void putsDialog(const char *str)
{
	 //UartDecode(str);
	 while(*str != '\0')
	{
        	SendData(*str++);
    }
}

static void Uart_Send_String(char *Data,int len) 
{
	  int j; 
	  for(j=0;j<len;j++) 
	  { 
		    U0DBUF = *Data++; 
		    while(UTX0IF == 0); //发送完成标志位
		    UTX0IF = 0; 
	  } 
}

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
     start_flag=1;
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


 #ifdef _USER_OAD_FLASH_WRITE_
static void dl2rc(void)
{
  preamble_t preamble;
  uint32 oset;
  uint16 addr = HAL_OAD_RC_START / HAL_FLASH_WORD_SIZE;				//0x0800/4		user code start address
  uint8 buf[4];

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_DL);
  //preamble.len = 65536;

  for (oset = 0; oset < preamble.len; oset += HAL_FLASH_WORD_SIZE)
  {
    HalOADRead(oset, buf, HAL_FLASH_WORD_SIZE, HAL_OAD_DL);
    if ((addr % (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)) == 0)
    {
      HalFlashErase(addr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
    }
    HalFlashWrite(addr++, buf, 1);
  }
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

  HalOADRead(PREAMBLE_OFFSET, (uint8 *)&preamble, sizeof(preamble_t), HAL_OAD_RC);

  // Run the CRC calculation over the active body of code.
  for (oset = 0; oset < preamble.len; oset++)
  {
    if ((oset < HAL_OAD_CRC_OSET) || (oset >= HAL_OAD_CRC_OSET+4))
    {
      uint8 buf;
      HalOADRead(oset, &buf, 1, HAL_OAD_RC);
      crc = runPoly(crc, buf);
    }
  }

  // IAR note explains that poly must be run with value zero for each byte of crc.
  crc = runPoly(crc, 0);
  crc = runPoly(crc, 0);

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

#if defined HAL_PA_LNA || defined HAL_PA_LNA_CC2590
void MAC_RfFrontendSetup(void)
{}
#endif

#ifdef DEBUG_TRACE
static void PutChar(const char Tbyte)
{
	U0DBUF = Tbyte; 
	while(UTX0IF == 0);   //发送完成标志位
	UTX0IF = 0;  
}

static void putsStr(const char *str)
{
	while(*str!='0')
		PutChar(*str++);
	//HalUARTWrite(0, (unsigned char*)str++, 1);
}

static char *mac_itoa(long n, int base)
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



int Printf(const char *fmt, ...)
{
	int i;
	const char *s;
 	int d;
	char buf[128];
	char *p = buf;
//	char buf[16];
//	char *p = buf;
 	va_list ap;

    va_start(ap, fmt);
	i = 16;

    while (*fmt) 
	{
        if (*fmt != '%') 
		{
            PutChar(*fmt++);
            continue;
        }
		
		switch(*++fmt)
		{
            case 's':
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
		 {
                   PutChar(*s);
                }
                break;
            case 'd':			
                d = va_arg(ap, int);
				//d>>=8;
                p = mac_itoa(d, 10);
                for (s = p; *s; s++) 
				{
                    PutChar(*s);
                }

				break;

            case 'x':
			case 'X':
                d = va_arg(ap, int);
                p = mac_itoa(d, 16);
                for (s = p; *s; s++) 
				{
                    PutChar(*s);
                }
                break;
             //Add other specifiers here...              
            default:  
                putsStr(fmt);
                break;
        } 
        fmt++;
    }
    va_end(ap);

    return 1;   /* Dummy return value */
}
#endif

/**************************************************************************************************
*/
