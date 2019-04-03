#ifndef __USART_H
#define __USART_H
//#include <stm32f10x_lib.h>
#include "stm32f10x.h"
#include "stdio.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/27
//�汾��V1.3
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
////////////////////////////////////////////////////////////////////////////////// 	  
 
	  	
extern u8 USART_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_STA;         //����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
//#define EN_USART1_RX //ʹ�ܴ���1����
void uart1_init(u32 pclk2,u32 bound);
void uart2_init(u32 pclk2,u32 bound);

//#define GR_EXT_CODE(x) (0x64-menuPara.rfGrTmp[x])

#define EXIT_BEFORE_SET			       1
#define NOEXIT_BEFORE_SET			0

#define UART_ENCODE_RFMOD				0xE0

#define UART_ENCODE_RFID					0xCF
#define UART_ENCODE_RFCH					0xCE
#define UART_ENCODE_RFADDR				0xCD
#define UART_ENCODE_RFIDEX				0xCC


#define UART_ENCODE_RADIO					0xBB
#define UART_ENCODE_APERTSHUTER			0xBA
#define UART_ENCODE_ISOCMP				0xB9



//#define UART_ENCODE_RFa					0xB4
#define UART_ENCODE_GRM					0xB4
#define UART_ENCODE_RFb					0xB5
#define UART_ENCODE_RFc					0xB6
#define UART_ENCODE_RFd					0xB7
#define UART_ENCODE_RFE					0xB8

#define UART_ENCODE_RFPREARR				0xB3
#define UART_ENCODE_RFCOUNTDOWN			0xB2
#define UART_ENCODE_RFRECOVER				0xB1

#define UART_ENCODE_RFCMD					0xB0

#define UART_ENCODE_RFMUL					0xAF

#define UART_ENCODE_PILOT					0xAE
#define UART_ENCODE_CHSCAN				0xAD
#define UART_ENCODE_EXIT					0xAC
#define UART_ENCODE_RFFLA					0xAB
#define UART_ENCODE_UNMODULATE			0xA0

#define UART_ENCODE_RFCMD_PREFLA			0x01
#define UART_ENCODE_RFCMD_FLA			0x02
#define UART_ENCODE_RFCMD_PILOT			0x03
#define UART_ENCODE_RFCMD_REL				0x04
#define UART_ENCODE_RFCMD_SLEEP			0x05


#define UART_CC_REL							0xC0
#define UART_CC_DEV							0xC1
#define UART_CC_READY						0xC2
#define UART_CC_RTSCAN						0xC3
#define UART_CC_INIT						0xC4
#define UART_CC_SLACMD						0xC5
#define UART_CC_MSTSW						0xC6

#define UART_CC_REL_FLA					0x01
#define UART_CC_REL_MODEL					0x02

#define UART_CC_SLACMD_TEST				0x03
#define UART_CC_SLACMD_MODEL				0x02

extern unsigned char LoadUartSendBuf(unsigned char id, unsigned char pa1, unsigned char pa2);
extern unsigned char LoadUartArray(u8 id, u8 *pBuf, u8 len);
extern void TrigerUartSend(u8 mod);
extern void RfPreFlashInfo(u8 tms);
extern void RfRecoverInfo(void);
extern void RfCountDownInfo(int tick);
extern void RfFlashConfirm(u8 tms,u8 fPilot);
unsigned char UartSctSend(unsigned char id, unsigned char pa1, unsigned char pa2);
extern unsigned char RfMulTimeRev(void);
extern unsigned char WlmMulFrqRev(void);
extern u8 CalcSum(u8 *rcv, u8 len);


#endif	   
















