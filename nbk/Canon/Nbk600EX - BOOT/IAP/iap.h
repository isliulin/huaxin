#ifndef __IAP_H__
#define __IAP_H__
//#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IAP ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////	    
typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.   
#define FLASH_APP1_ADDR		0x08005000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)	

//#define FLASH_VER_ADDR		0x0801E000		   //�汾��
#define FLASH_APPBASE_ADDR      0x08000000
#define FLASH_VER_ADDR			(FLASH_APPBASE_ADDR+127*1024)//0x0801FC00
#define FLASH_SERNUM_ADDR        (FLASH_VER_ADDR+16)
//#define FLASH_SERNUM_ADDR	       (FLASH_VER_ADDR+4)		// ���кź�Ӳ����ַ
//#define FLASH_VERI_SN_ADDR (FLASH_APP1_ADDR+0x1000)
//#define FLASH_VERI_FLAG_ADDR (FLASH_VER_ADDR+16+16)
//#define FLASH_VERI_SN_ADDR         (FLASH_VERI_FLAG_ADDR+4)
//#define FLASH_USER_ADDR		(FLASH_SERNUM_ADDR+16)
//#define FLASH_FACTORY_ADDR	(FLASH_USER_ADDR+128)
//#define FLASH_USER_ADDR		(0x0801E000+1024)
//#define FLASH_USER_ADDR		(FLASH_SERNUM_ADDR+1024)
#define FLASH_USER_ADDR             (FLASH_VER_ADDR+512)
#define FLASH_PARA_SAVE_ADDR   (FLASH_USER_ADDR+256)


#define CPU_ID_BASE				(0x1FFFF7E8)
//#define FLASH_AVAILABLE			(100*1024)
#define FLASH_AVAILABLE			(106*1024)

#define FLASH_VERI_FLAG_ADDR      (FLASH_APPBASE_ADDR+126*1024)//0x0801F800  

											//����0X08000000~0X08004FFF�Ŀռ�ΪBootloaderʹ��	   
void iap_load_app(u32 appxaddr);			//��ת��APP����ִ��
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//��ָ����ַ��ʼ,д��bin
#endif







































