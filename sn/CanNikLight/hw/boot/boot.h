#ifndef _BOOT_H_
#define _BOOT_H_

#define RCV_CMD_LEN                   128

#define AM1_IAP_WRITE_PAGE            0x41
#define AM1_IAP_ERASE_DEVICE	        0x42
#define AM1_IAP_REPORT_ERROR          0x43

#define AM1_IAP_CHECK_SUM		          0x44
#define AM1_IAP_READ_STATUS		        0x45
#define AM1_IAP_SECURITYCHECK	        0x46
#define AM1_IAP_READ_64BYTE           0x47 

#define AM1_IAP_JSR_APP               0x48

#define RCV_CMD_LEN                   128//16

//iostm8l152r8 ÈÝÁ¿
#define APP_START_ADDR       0xA000
#define FLASH_AVAILABLE			          (56*1024l)

void BootProc( void );

#endif