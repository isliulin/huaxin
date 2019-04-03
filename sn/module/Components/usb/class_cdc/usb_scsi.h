//-----------------------------------------------------------------------------
// F34x_MSD_Scsi.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file with function prototypes relevant to F34x_Scsi.c
//
//
// FID:            34X000052
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
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef _SCSI_H_
#define _SCSI_H_

//#include "F34x_MSD_USB_Main.h"
#include "Hal_types.h"
//#include "bsp.h"

//#define code CODE
//#define xdata  XDATA

typedef unsigned char BYTE;
typedef unsigned long DWORD;

#define SCSI_PASSED 		0
#define SCSI_FAILED 		1
#define SCSI_PHASE_ERROR 	2

#define  EP1_PACKET_SIZE         0x0040      // Can range 0 - 1024 depending on data and transfer type
#define  EP1_PACKET_SIZE_LE      0x4000      // IMPORTANT- this should be Little-Endian version of EP1_PACKET_SIZE
#define  EP2_PACKET_SIZE         0x0040      // Can range 0 - 1024 depending on data and transfer type
#define  EP2_PACKET_SIZE_LE      0x4000      // IMPORTANT- this should be Little-Endian version of EP2_PACKET_SIZE

#define MSD_READY 					0x00
#define MSD_COMMAND_TRANSPORT 		0x01
#define MSD_DATA_IN					0x02
#define MSD_DATA_OUT				0x03
#define MSD_STATUS_TRANSPORT		0x04
#define MSD_DATA					0x05
#define MSD_DO_RESET				0xFF

#define CBW_SIGNATURE 0x55534243
#define CSW_SIGNATURE 0x55534253

#define FW_OAD_LOCA_START (32768)//(128*1024)
#define FW_OAD_LOCA_END	(65536)//(256*1024)

#define ntohl(x)  (x)
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Scsi_Rx(void);

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define msb(x) (((x)>>8)&0x000000FFul)
#define lsb(x) ((x)&0x000000FFul)

extern BYTE XDATA Scsi_Status;
extern DWORD XDATA Scsi_Residue;


#define PHYSICAL_BLOCK_SIZE     512
#define Sect_Block_Size() ((unsigned)PHYSICAL_BLOCK_SIZE)


typedef struct _CBW {
	DWORD dCBWSignature;
	DWORD dCBWTag;
	DWORD dCBWDataTransferLength;
	BYTE  bmCBWFlags;
	BYTE  bCBWLUN;
	BYTE  bCBWCBLength;
	BYTE  CBWCB[16];
} CBW;

typedef struct _CSW {
	DWORD dCSWSignature;
	DWORD dCSWTag;
	DWORD dCSWDataResidue;
	BYTE  bCSWStatus;
} CSW;

extern CBW XDATA cbw;
extern CSW XDATA csw;



typedef unsigned char       BOOL;

// Data
//typedef unsigned char       BYTE;
typedef unsigned short      WORD;
//typedef unsigned long       DWORD;

// Unsigned numbers
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned long       UINT32;

// Signed numbers
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed long         INT32;


//AES 
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


// Macro for setting the mode of the AES operation
#define AES_SETMODE(mode) do { ENCCS &= ~0x70; ENCCS |= mode; } while (0)

// Macro for starting the AES module for either encryption, decryption,
// key or initialisation vector loading.
#define AES_START()     ENCCS |= 0x01

#define DELAY 0x0F

#endif
