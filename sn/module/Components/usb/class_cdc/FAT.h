/******************************************************************
   ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
			
        ��ӭ�����ҵ�USBר����http://group.ednchina.com/93/
        ��ӭ�����ҵ�blog��   http://www.ednchina.com/blog/computer00
                             http://computer00.21ic.org

        ��лPCB�����̡�������԰�� http://bbs.cepark.com/

FAT.h file

���ߣ�����ȦȦ
��������: 2008.08.19
�޸�����: 2008.08.20
�汾��V1.1
��Ȩ���У�����ؾ���
Copyright(C) ����ȦȦ 2008-2018
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