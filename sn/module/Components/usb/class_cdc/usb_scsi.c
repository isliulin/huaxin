//-----------------------------------------------------------------------------
// F34x_MSD_Scsi.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file contains functions which responses to requests from USB device
//
//
//
// How To Test:    See Readme.txt
//
//
// FID:            34X000051
// Target:         C8051F34x
// Tool chain:     Keil
// Command Line:   See Readme.txt
// Project Name:   F34x_USB_MSD
//
// Release 1.3
//    -All changes by ES
//    -18 DEC 2009
//    -No changes; incremented revision number to match project revision
//
// Release 1.2
//    -All changes by BD and PD
//    -5 OCT 2007
//    -No changes; incremented revision number to match project revision
//
// Release 1.1
//    -All changes by PKC
//    -09 JUN 2006
//    -No changes; incremented revision number to match project revision
//
// Release 1.0
//    -Initial Release
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "usb_scsi.h"
#include "FAT.h"
//#include "bsp.h"
//#include "F34x_MSD_Msd.h"
//#include "F34x_MSD_Util.h"
//#include "F34x_MSD_Sect_Serv.h"
#include <stdio.h>
#include <ioCC2530.h>
#include "comdef.h"
#include "hal_board_cfg.h"
#include "hal_dma.h"
#include "hal_flash.h"
#include "hal_oad.h"
#include "hal_types.h"
#include "hal_aes.h"

#define SEND_PACKET_LEN 64

#define SCSI_TEST_UNIT_READY 				0x00
#define SCSI_REQUEST_SENSE 					0x03
#define SCSI_FORMAT_UNIT					0x04
#define SCSI_SEND_DIAGNOSTIC				0x10
#define SCSI_INQUIRY						0x12
#define SCSI_MODE_SELECT_6					0x15
#define SCSI_MODE_SENSE_6					0x1A
#define SCSI_START_STOP_UNIT				0x1B
#define SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL 	0x1E
#define SCSI_READ_CAPACITY_10				0x25
#define SCSI_READ_CAPACITY_16				0x9E
#define SCSI_READ_6							0x08
#define SCSI_READ_10						0x28
#define SCSI_READ_16						0x88
#define SCSI_WRITE_10						0x2A
#define SCSI_VERIFY_10						0x2F
#define SCSI_READ_FORMAT_CAPACITIES 		0x23

BYTE  Scsi_Status;
DWORD Scsi_Residue;
CBW cbw;
CSW csw;
unsigned char g_outDataReady=0;
char g_modBoot=0;
extern unsigned char g_deviceLinked;
//BYTE xdata Msd_State=MSD_READY;

BYTE outPacket[EP1_PACKET_SIZE];

extern uint8 usbOutDataRcv(uint8* rBuf);
static void halAesEncrDecr(BYTE *pDataIn, UINT16 length, BYTE *pDataOut, /*BYTE *pInitVector, */BOOL decr);
static void halAesLoadKeyOrInitVector(BYTE* pData, BOOL key);
extern void DevUpdateUartSend(char *sbuf, int len);

// Buffer for read/write transfers:
//BYTE Scratch[PHYSICAL_BLOCK_SIZE]={0};

#if 0
const BYTE Scsi_Standard_Inquiry_Data[36]= {
0x00,0x80,0x02,0x02,0x20,0x00,0x00,0x00,0x53,0x54,0x4d,0x20,
0x20,0x20,0x20,0x20,0x53,0x44,0x20,0x46,0x6c,0x61,0x73,0x68,
0x20,0x44,0x69,0x73,0x6b,0x20,0x20,0x20,0x31,0x2e,0x30,0x20,
/*
//BYTE Scsi_Standard_Inquiry_Data[28]= {
  0x00, // Peripheral qualifier & peripheral device type
  0x80, // Removable medium
  0x05, // Version of the standard (2=obsolete, 5=SPC-3)
  0x02, // No NormACA, No HiSup, response data format=2
  0x1F, // No extra parameters
  0x00, // No flags
  0x80, // 0x80 => BQue => Basic Task Management supported
  0x00, // No flags
  'S','i','L','a','b','s',' ',' ', // Requested by Dekimo via www.t10.org
  'M','a','s','s',' ','S','t','o','r','a','g','e'
*/
};
#else

const BYTE Scsi_Standard_Inquiry_Data[36]=
{
 0x00, //磁盘设备
 0x80, //其中最高位D7为RMB。RMB=0，表示不可移除设备。如果RMB=1，则为可移除设备。
 0x00, //各种版本号0
 0x01, //数据响应格式
 0x1F, //附加数据长度，为31字节
 0x00, //保留
 0x00, //保留
 0x00, //保留
 //0xB5,0xE7,0XC4,0xD4,0xC8,0xA6,0xC8,0xA6, //厂商标识，为字符串“电脑圈圈”
 0xC1,0xbd,0xb0,0xD1,0xb2,0xcb,0xb5,0xb6,
 
 //产品标识，为字符串“自己做的假U盘”
 //0xD7,0xD4,0xBC,0xBA,0xD7,0xF6,0xB5,0xC4,0xBC,0xD9,0x55,0xC5,0xCC,0x00,0x00,0x00,
//0x31,0x2E,0x30,0x31 //产品版本号，为1.01
0xC9,0XE8,0XB1,0XB8,0XC9,0XFD,0XBC,0XB6,0X00,0X00,0X00,0X00,0X00,0x00,0x00,0x00,
0x31,0x2E,0x30,0x31
};
#endif

const uint8 MaximumCapacity[12]=
{

	0x00, 0x00, 0x00, //保留
	0x08,  //容量列表长度
	0x01, 0x00, 0x00, 0x00,  //块数(最大支持8GB)
	0x03, //描述符代码为3，表示最大支持的格式化容量
	0x00, 0x02, 0x00 //每块大小为512字节
/*
	0x00,0x00,0x00,0x08,0x00,0x6c,0xe0,0x00,0x02,0x00,0x02,0x00
*/

};

const uint8 DiskCapacity[8]=
{
//0x00,0x6c,0xdf,0xff,0x00,0x00,0x02,0x00,
0x00,0x03,0xFF,0xFF, //能够访问的最大逻辑块地址
0x00,0x00,0x02,0x00  //块的长度
 //所以该磁盘的容量为
 //(0x3FFFF+1)*0x200 = 0x8000000 = 128*1024*1024 = 128MB.
 //0x00,0x6c,0xdf,0xff,0x00,0x00,0x02,0x00,
};


const BYTE Scsi_Mode_Sense_6[4]= { 0x03,0,0,0 }; // No mode sense parameter

//----------------------------------------------------------------------------
// Scsi_Send
//----------------------------------------------------------------------------
//
// This function sends defined numbers of bytes via USB
//
// Parameters   : ptr - poiter to sending bytes
//                count - number of sending bytes
// Return Value :
//----------------------------------------------------------------------------
extern uint16 halUartBufferedWrite(const uint8* buf, uint16 length);
extern void usbInProcess(void);
extern void UsbSendBlock(uint8 *ps, uint32 len);

static void Scsi_Send(uint8* ptr,uint32 count)
{

	Scsi_Residue-=count;
	UsbSendBlock(ptr,count);

}

//----------------------------------------------------------------------------
// Scsi_Inquiry
//----------------------------------------------------------------------------
//
// This function responses to inquiry from other USB device
//
// Parameters   :
// Return Value :
//----------------------------------------------------------------------------

static void Scsi_Inquiry(void)
{
	uint8 *sp;
	Scsi_Status=SCSI_PASSED;
	sp = (uint8 *)Scsi_Standard_Inquiry_Data;
	Scsi_Send(sp,sizeof(Scsi_Standard_Inquiry_Data));
}

//----------------------------------------------------------------------------
// Scsi_Write10
//----------------------------------------------------------------------------
//
// This function responses to write command
//
// Parameters   :
// Return Value :
//----------------------------------------------------------------------------
extern halDMADesc_t dmaCh0;
char *g_decKey = "WorkForYourDream";
static void Scsi_Write10(void)
{
	uint32 rcvTotal=0;
	uint8 rcvBlockSize=0;
	static uint32 addr = FW_OAD_LOCA_START;
  	//uint16 oset;
  	//uint16 page=FW_OAD_LOCA_START/HAL_FLASH_PAGE_SIZE;
	uint32 rtAddr=(((uint32)cbw.CBWCB[2]<<24)|((uint32)cbw.CBWCB[3]<<16)|((uint32)cbw.CBWCB[4]<<8)|((uint32)cbw.CBWCB[5]));

	if(g_deviceLinked)
	{
		while(rcvTotal<cbw.dCBWDataTransferLength)
		{
			rcvBlockSize=usbOutDataRcv(outPacket);
			rcvTotal+=rcvBlockSize;
			if((0x00!=rtAddr)&&(0x01!=rtAddr)&&(0x41!=rtAddr)&&(0x21!=rtAddr))
			{
				DevUpdateUartSend(outPacket,rcvBlockSize);
			}
		}

	}else
	{
		HAL_DMA_SET_ADDR_DESC0( &dmaCh0 );
	  	HalFlashInit();
		AES_SETMODE(ECB);
		halAesLoadKeyOrInitVector(g_decKey, TRUE);
		
		while(rcvTotal<cbw.dCBWDataTransferLength)
		{
			rcvBlockSize=usbOutDataRcv(outPacket);
			rcvTotal+=rcvBlockSize;
			halAesEncrDecr((BYTE *)outPacket, EP1_PACKET_SIZE, (BYTE *)outPacket, DECRYPT);
			if((0x00!=rtAddr)&&(0x01!=rtAddr)&&(0x41!=rtAddr)&&(0x21!=rtAddr)&&(addr<(FW_OAD_LOCA_END)))
			{
				if ((addr % (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)) == 0)
				{
					HalFlashErase(addr / (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE));
				}
				HalFlashWrite(addr, outPacket, rcvBlockSize/4);
				addr += (rcvBlockSize/4);
			}
		}
	}
	
	Scsi_Status=SCSI_PASSED;
}

//----------------------------------------------------------------------------
// Scsi_Mode_Sense6
//----------------------------------------------------------------------------
//
// This function responses to mode sense information request
//
// Parameters   :
// Return Value :
//----------------------------------------------------------------------------

static void Scsi_Mode_Sense6(void)
{
	uint8 *sp;
	Scsi_Status=SCSI_PASSED;
	sp = (uint8 *)Scsi_Mode_Sense_6;
	Scsi_Send(sp,sizeof(Scsi_Mode_Sense_6));
}


static void Scsi_Read10(void)
{

	uint32 index=0;
	uint32 byteAddrBlock = (((uint32)cbw.CBWCB[2]<<24)|((uint32)cbw.CBWCB[3]<<16)|((uint32)cbw.CBWCB[4]<<8)|((uint32)cbw.CBWCB[5]));
	uint32 byteAddr=byteAddrBlock*512;

	uint8* sp;
	while(index<cbw.dCBWDataTransferLength)
	{
		 if(byteAddr<512) 
		 {
		 	sp=(uint8*)(Dbr+index); //返回DBR
		 }else if(byteAddr==512) 
		{
			sp=(uint8*)Fat; //返回FAT
		 }else if((byteAddr>=576)&&(byteAddr<16896)) 
		 {
		 	sp=(uint8*)Zeros;
		}else if(byteAddr==16896)
		{
			sp=(uint8*)Fat; //返回FAT（备份FAT）
		}else if((byteAddr>=16960)&&(byteAddr<33280)) 
		{
			sp=(uint8*)Zeros;
		}else if(byteAddr==33280) 
		{
			sp=(uint8*)RootDir; //返回根目录
		}else if((byteAddr>=33344)&&(byteAddr<49664)) 
		{
			sp=(uint8*)Zeros;
		}
#ifdef INCLUDE_TEST_FILE
		else if((byteAddr>=49664) &&(byteAddr<(49664+512)))
		 {
			sp=(uint8*)(TestFileData+index); //返回文件数据
		 }
#endif
		else
		 {
		 	sp=(uint8*)Zeros;
		 }

		 if((index+SEND_PACKET_LEN)<cbw.dCBWDataTransferLength)
		 {
			Scsi_Send(sp,SEND_PACKET_LEN);
			index += SEND_PACKET_LEN;
		 }else
		 {
			Scsi_Send(sp,cbw.dCBWDataTransferLength-index);
			break;
		 }

		byteAddr+=SEND_PACKET_LEN;
	}
	
	Scsi_Status=SCSI_PASSED;
}


static void Scsi_Read_Capacity10(void)
{
	uint8 *sp;
	DWORD d_len = ntohl(cbw.dCBWDataTransferLength);
	Scsi_Status=SCSI_PASSED;
	if(d_len>sizeof(DiskCapacity))
	{
		d_len = sizeof(DiskCapacity);
	}
	sp = (uint8 *)DiskCapacity;
	Scsi_Send(sp,d_len);
}


static void SCSI_ReadFormatCapacity(void)
{
	uint8 *sp;
	DWORD d_len = ntohl(cbw.dCBWDataTransferLength);

	Scsi_Status=SCSI_PASSED;
	if(d_len>sizeof(MaximumCapacity))
	{
		d_len = sizeof(MaximumCapacity);
	}
	sp = (uint8 *)MaximumCapacity;
	Scsi_Send(sp,d_len);
}

static void SCSI_RequestSense(void)
{
	
}

//----------------------------------------------------------------------------
// Scsi_Rx
//----------------------------------------------------------------------------
//
// This function answers to requests from USB
//
// Parameters   :
// Return Value :
//----------------------------------------------------------------------------

void Scsi_Rx()
{

	Scsi_Status=SCSI_FAILED;
	Scsi_Residue=ntohl(cbw.dCBWDataTransferLength);

	if(g_outDataReady)
	{
		UsbSendBlock((BYTE*)&csw,sizeof(CSW));
		g_outDataReady = 0;
	}

	if(!cbw.bCBWCBLength)
	return;

	switch(cbw.CBWCB[0]) 
	{ // SCSI Operation code
		case SCSI_TEST_UNIT_READY:
			Scsi_Status=SCSI_PASSED;
		break;
		
		case SCSI_INQUIRY:
			Scsi_Inquiry();
		break;
		
		case SCSI_MODE_SENSE_6:
			Scsi_Mode_Sense6();
		break;
		
		case SCSI_READ_CAPACITY_10:
			Scsi_Read_Capacity10();
		break;
		
		case SCSI_READ_10:
			Scsi_Read10();
		break;
		
		case SCSI_WRITE_10:
			Scsi_Write10();
		break;
		
		case SCSI_VERIFY_10:
			Scsi_Residue=0;
			Scsi_Status=SCSI_PASSED;
		break;
		
		case SCSI_START_STOP_UNIT:
			Scsi_Status=SCSI_PASSED;
		break;

		case SCSI_READ_FORMAT_CAPACITIES:
			SCSI_ReadFormatCapacity();
		break;

		case SCSI_REQUEST_SENSE:
			SCSI_RequestSense();
		break;
			
		case SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL:
			Scsi_Status=SCSI_PASSED;
		break;
		
		default:
		//printf("Unknown SCSI Cmd (0x%02X).\n",(int)cbw.CBWCB[0]);
			Scsi_Status=SCSI_PASSED;
			//Scsi_Read10();
		break;
	}

	//if(SCSI_WRITE_10!=cbw.CBWCB[0])
	{
	  	csw.dCSWSignature=CSW_SIGNATURE;
	  	csw.dCSWTag=cbw.dCBWTag;
	  	csw.bCSWStatus=Scsi_Status;
	  	csw.dCSWDataResidue=ntohl(Scsi_Residue);
	  	UsbSendBlock((BYTE*)&csw,sizeof(CSW));
	}

      //USB_In((BYTE*)&csw,sizeof(CSW));

	  
}

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





