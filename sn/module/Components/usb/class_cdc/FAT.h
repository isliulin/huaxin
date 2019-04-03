/******************************************************************
   本程序只供学习使用，未经作者许可，不得用于其它任何用途
			
        欢迎访问我的USB专区：http://group.ednchina.com/93/
        欢迎访问我的blog：   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        感谢PCB赞助商——电子园： http://bbs.cepark.com/

FAT.h file

作者：电脑圈圈
建立日期: 2008.08.19
修改日期: 2008.08.20
版本：V1.1
版权所有，盗版必究。
Copyright(C) 电脑圈圈 2008-2018
All rights reserved            
*******************************************************************/

#ifndef __FAT_H__
#define __FAT_H__

#include "Hal_types.h"
//#include "bsp.h"

#define INCLUDE_TEST_FILE

#if 0
#define FLASH_PAGE_SIZE         512
#define STORAGE_BUFFER_SIZE     512             /* Data transfer buffer size in 512 bytes alignment */
#define MASS_BUFFER_SIZE        48              /* Mass Storage command buffer size */
#define FAT_SECTORS  u8FormatData[14]
extern const uint8_t u8FormatData[62];
extern const uint8_t u8RootDirData[96];

extern uint32_t MassBlock[MASS_BUFFER_SIZE / 4];
extern uint32_t Storage_Block[STORAGE_BUFFER_SIZE / 4];

void FMC_ReadPage(uint32_t u32startAddr, uint32_t * u32buff);

#else

extern const  uint8 Dbr[];
extern const  uint8 Fat[];
extern const  uint8 Zeros[];
extern const  uint8 TestFileData[];
extern const  uint8 RootDir[];
#endif

#endif