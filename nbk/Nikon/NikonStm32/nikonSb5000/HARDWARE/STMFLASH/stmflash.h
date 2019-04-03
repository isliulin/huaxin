#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  
//#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 128//512 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1200//1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif	

#define FLASH_APP1_ADDR		0x08005000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)	
//#define FLASH_VER_ADDR			0x0801E000		   //�汾��

//#define FLASH_VER_ADDR			0x0801FC00
#define FLASH_APPBASE_ADDR      0x08000000
#define FLASH_VER_ADDR			(FLASH_APPBASE_ADDR+127*1024)//0x0801FC00
#define FLASH_SERNUM_ADDR		(FLASH_VER_ADDR+16)		// ���кź�Ӳ����ַ
//#define FLASH_USER_ADDR		(FLASH_SERNUM_ADDR+16)
//#define FLASH_USER_ADDR		(0x0801E000+1024)
#define FLASH_USER_ADDR             (FLASH_VER_ADDR+512)
#define FLASH_PARA_SAVE_ADDR   (FLASH_USER_ADDR+256)

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB
void STMFLASH_Unlock(void);					  //FLASH����
void STMFLASH_Lock(void);					  //FLASH����
u8 STMFLASH_GetStatus(void);				  //���״̬
u8 STMFLASH_WaitDone(u16 time);				  //�ȴ���������
u8 STMFLASH_ErasePage(u32 paddr);			  //����ҳ
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);//д�����
u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif

















