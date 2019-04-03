/**
  ******************************************************************************
  * @file    main.c
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   ������.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usbio.h"
//#include "../sys/sys.h"
#include "../iap/iap.h"
#include "../Enc_Dec/encrypto.h"
#include "../stmflash/stmflash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t USB_Received_Flag;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#define USBHID_REC_LEN	128
#define RCV_CMD_LEN 16

#define UPDATE_FAST

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define CHARGE_LED1 		PDout(14)
#define CHARGE_LED2 		PDout(15)
#define D3V3_EN				PBout(12)
#define CC2530_RESET      PDout(7)
#define CC2530_PWR         PBout(6)
//#define RF_RESET			PBout(6)
//#define CHARGE_LED 		PAout(15)
#define CHARGE_LED1_ON()			{CHARGE_LED1=0;}
#define CHARGE_LED1_OFF()		{CHARGE_LED1=1;}

#define CHARGE_LED2_ON()			{CHARGE_LED2=0;}
#define CHARGE_LED2_OFF()		{CHARGE_LED2=1;}

#define STM32_IAP_WRITE_PAGE            0x41
#define STM32_IAP_ERASE_DEVICE	      0x42

#define STM32_IAP_CHECK_SUM		       0x44
#define STM32_IAP_READ_STATUS		       0x45
#define STM32_IAP_SECURITYCHECK	       0x46
#define STM32_IAP_READ_64BYTE            0x47 

#define STM32_IAP_WRITE_OK		0x48
#define STM32_IAP_VER			0x49
#define STM32_IAP_SERNUM		0x4A
#define STM32_IAP_USER			0x4B
#define STM32_IAP_START		0x4C

#define STM32_IAP_VERIRQ			0x50
#define STM32_IAP_CC2530			0x51

#define VALID_INIT_SN				(978137684)
#define VALID_START_SN				(260636838)

#define UART1_TRACE_DEBUG
//7E FF 47 00 02 FF FF 46
//u8 usbHidRxBuf[USBHID_REC_LEN] __attribute__ ((at(0X20001000)));
//0x1FFFF7E8
u8 usbHidRxBuf[USBHID_REC_LEN];
//char *g_decKey = "WorkForYourDream";
char *g_decKey = "MakeTomorrowBest";			//NBK�����

char g_cupIdKey[16] = {0x24,0x98,0xb7,0xA0,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};
//char g_cupIdRst[16] = {0xE8,0xF7,0xff,0x1f,0xCB,0xDF,0xE6,0x5D,0xD1,0x4B,0xF7,0x51,0xA6,0x01,0x98,0x22};
char g_cupIdRst[16] = {0x08,0x3f,0x37,0x9f,0xb6,0xe6,0x5d,0x1f,0x3d,0xb9,0xdb,0x9c,0x4f,0xf3,0x6f,0xf0};

unsigned char start_flag=0;

#define ucLDR_Message (*(STCOMMANDER *)usbHidRxBuf)

char *g_bootVer = "A001";

/**
  * @brief  ���ڴ�ӡ���
  * @param  None
  * @retval None
  */
typedef struct U_STCOMMANDER
{
	unsigned char head;
	unsigned char cmd_no;
	unsigned char cmd;
	unsigned char rsv;                        // use as start address
	unsigned char data_len;
 	char buf[RCV_CMD_LEN];
	unsigned char sum;
	//unsigned char sum;
}STCOMMANDER;

void PortInit(void)
{
	RCC->APB2ENR |= 1<<2;
	RCC->APB2ENR|=1<<3;//��ʹ������PORTBʱ��
	RCC->APB2ENR |=1<<5;		//enable port D clock
	//GPIOA->CRL&=0xFFFFFFF0; 			//PA1
	//GPIOA->CRL|=0x00000008;	
	GPIOB->CRH&=0xFFF0FFFF;
	GPIOB->CRH|=0x00030000;
	GPIOB->ODR&=~(1<<12); 		//D3V3_EN

	GPIOB->CRL &= 0xF0FFFFFF;
	GPIOB->CRL|=  0x03000000;

	GPIOD->CRL&=0x0FFFFFFF;		//COM_AUX
	GPIOD->CRL|=0x30000000;
	//GPIOD->ODR&=~(1<<7);
	
	GPIOA->CRL&=0xFFFFFF0F;
	GPIOA->CRL|=0x00000080;
	GPIOA->ODR|=0x02; 

	GPIOD->CRH&=0x00FFFFFF;		//COM_AUX
	GPIOD->CRH|=0x33000000;
	GPIOD->ODR |= 0xC000;

	//CHARGE_LED_OFF();
	//GPIOA->CRH&=0x0FFFFFFF; 
	//GPIOA->CRH|=0x30000000;//PAOUT15 CHARGE_LED 
	//CHARGE_LED_OFF();
	CHARGE_LED1_OFF();
	CC2530_PWR = 1;	
	//RF_RESET = 1;
	
	//GPIOC->CRL&=0xFF0FFFFF;	
	//GPIOC->CRL|=0x00300000;
	//GPIOC->ODR|=0x20;		//CC2530 RESET UP
}

static unsigned char uartRcvData(void)
{
	unsigned int count1=0;
	unsigned int count2=0;

	//while( !URX0IF )
	//while(!(USART1->SR&(1<<5)))
	while(!(USART2->SR&(1<<5)))
	{
     		count1++;
     		if(count1>=5000)
     		{
			count1=0;
			count2++;
		}
		if((count2>=5000)&&(start_flag==1))
		{
			start_flag=0;
			count2 = 0;
			return 0;
		}
	}
	//URX0IF = 0;
	//printf("rcv,%x", USART1->DR);
	//return (USART1->DR);
	return (USART2->DR);
}

//static unsigned char Message_Rcv(void)
static unsigned char Message_Rcv(unsigned char *errCod)
{
	unsigned char	ucdata;
	unsigned char rcvdat;
	unsigned char	uci;

	//RCC->APB2RSTR|=1<<14;   //��λ����1
	//RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	
	
	start_flag=1;

	//P1_3 = !P1_3;
	
    ucdata=uartRcvData();
	if(ucdata!=0x7E)
	{
		//*errCod = ucdata;
		*errCod = ucdata;
		//RCC->APB2RSTR|=1<<14;   //��λ����1
		//RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ		
		return 1;
	}
	start_flag=1;
	ucdata=uartRcvData();
	if(start_flag==0)
	{
		return 2;
	}
	ucLDR_Message.cmd_no = ucdata;
	ucLDR_Message.sum=ucdata;
	
      ucdata=uartRcvData();
	if(start_flag==0)
	{
		return 3;
	}
	ucLDR_Message.cmd=  ucdata;
	ucLDR_Message.sum+=ucdata;	
	
	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 4;
	}
	ucLDR_Message.rsv=  ucdata;
	ucLDR_Message.sum+=ucdata;	
	
	ucdata=uartRcvData();
	if(start_flag==0)
	{
		 return 5;
	}
	ucLDR_Message.data_len=  ucdata;
	ucLDR_Message.sum+=ucdata;
	
	for( uci=0 ; uci<ucLDR_Message.data_len ; uci++ )
	{
		ucdata = uartRcvData();
		if(start_flag==0)
		{
			 return (6+uci);
		}
		ucLDR_Message.buf[uci] = ucdata;
		ucLDR_Message.sum+=ucdata;
	}
	ucdata = uartRcvData();
	rcvdat = ucLDR_Message.sum;
	if(start_flag==0)
	{
		 return (6+ucLDR_Message.data_len);
	}
	//if(ucLDR_Message.msg_cks==ucdata)
	if(rcvdat==ucdata)
	{
      		return 0;
	}
	else
	{
		//*errCod = ucdata;
		return (6+ucLDR_Message.data_len+1);	
	}

}


int RcvDataParse(STCOMMANDER *stCom)
{
	u8 i,sum=0;
	
	if(stCom->head!=0x7E)	return	1;
	//if(stCom->data_len!=0x10)	return	2;
	
	sum+= stCom->cmd_no+stCom->cmd+stCom->rsv+stCom->data_len;
	
	for(i=0;i<stCom->data_len;i++)
	{
		sum += stCom->buf[i];
	}
	if(sum!=stCom->sum)	
	{
		//for(i=0;i<stCom->data_len;i++)
		//{
		//	printf("%x ",stCom->buf[i]);
		//}
		//printf("\r\n");
		//printf("%x, %x\r\n", sum, stCom->sum);
		return 3;
	}
	
	return 0;

}

unsigned char CalcSum(unsigned char *buf, unsigned char len)
{
	unsigned char sum=0, i;
	for(i=0;i<len;i++)
	{
		sum+=buf[i];
	}
	return sum;
}

extern u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�

int main(void)
{
//	uint8_t data[64];
	//uint32_t i=0,ret=0;
	u8 ret=0;
	u32 pAddrInc=0;
	u32 pReadInc=0;
	u32 pUserInc=0;
	//u16 pVerInc = 0;
	u8 errCod[2]={0};
	STCOMMANDER *pStCom;
	u8 *pRcvBuf=usbHidRxBuf;
	u8 veriBuf[16];
	u32 *pVeriFlag;
	u32 *cpuAddr;
	//u16 veriFlag=0;

	//u8 rst[16];
#ifdef UART1_TRACE_DEBUG
//	u8 input[16];
#endif
	u8 i;
	
//	Set_System();//ϵͳʱ�ӳ�ʼ��
	Stm32_Clock_Init(9);
	delay_init(72);
#ifdef UART1_TRACE_DEBUG
	uart1_init(72,115200);
	//printf("start\r\n");
#endif
	uart2_init(36,115200);
//	USART_Configuration();//����1��ʼ��
//	printf("\x0c\0");printf("\x0c\0");//�����ն�����
//	printf("\033[1;40;32m");//���ó����ն˱���Ϊ��ɫ���ַ�Ϊ��ɫ
//	printf("\r\n*******************************************************************************");
//	printf("\r\n************************ Copyright 2009-2012, ViewTool ************************");
//	printf("\r\n*************************** http://www.viewtool.com ***************************");
//	printf("\r\n***************************** All Rights Reserved *****************************");
//	printf("\r\n*******************************************************************************");
//	printf("\r\n");

	//USB_Interrupts_Config();
	//Set_USBClock();
	//USB_Init();
	PortInit();
//#ifdef UART1_TRACE_DEBUG
//	uart1_init(72,115200);
//#endif

//verify the cpu id in order to protect from copying
#if 1
	STMFLASH_Read(FLASH_VERI_FLAG_ADDR,(u16*)veriBuf,8);
#ifdef UART1_TRACE_DEBUG
		for(i=0;i<16;i++)
		{
			printf("%x ",veriBuf[i]);
		}
		printf("\r\n");
#endif
	Boot_Decrypt(veriBuf,g_decKey,veriBuf,16);

#ifdef UART1_TRACE_DEBUG
		for(i=0;i<16;i++)
		{
			printf("%x ",veriBuf[i]);
		}
		printf("\r\n");
#endif

	Boot_Decrypt(g_cupIdRst,g_cupIdKey,g_cupIdRst,16);

	cpuAddr = (u32*)g_cupIdRst;

#ifdef UART1_TRACE_DEBUG
	printf("Cpu id %x\r\n",*cpuAddr);
#endif
	
	pVeriFlag = (u32*)veriBuf;
	//if(veriFlag!=0xBCDE)			//first start
	if(VALID_INIT_SN==*pVeriFlag)   //first start
	{
#ifdef UART1_TRACE_DEBUG
		printf("first start\r\n");
#endif
		//STMFLASH_Read(CPU_ID_BASE,(u16*)(veriBuf+4),6);			//read cpu id
		STMFLASH_Read(*cpuAddr,(u16*)(veriBuf+4),6);
		//STMFLASH_Write(FLASH_VERI_SN_ADDR,(u16*)pRcvBuf,6);   //write verified id
#ifdef UART1_TRACE_DEBUG
		for(i=0;i<16;i++)
		{
			printf("%x ",veriBuf[i]);
		}
		printf("\r\n");
#endif
		*pVeriFlag = VALID_START_SN;
#ifdef UART1_TRACE_DEBUG
		for(i=0;i<16;i++)
		{
			printf("%x ",veriBuf[i]);
		}
		printf("\r\n");
#endif
		Boot_Encrypt(veriBuf,g_decKey,veriBuf,16);
#ifdef UART1_TRACE_DEBUG
		for(i=0;i<16;i++)
		{
			printf("%x ",veriBuf[i]);
		}
		printf("\r\n");
#endif
		STMFLASH_Write(FLASH_VERI_FLAG_ADDR,(u16*)&veriBuf,16/2);
	}else if(VALID_START_SN==*pVeriFlag)
	{
		//STMFLASH_Read(FLASH_VERI_SN_ADDR,(u16*)pRcvBuf,6);
		//STMFLASH_Read(CPU_ID_BASE,(u16*)(pRcvBuf+12),6);
		//STMFLASH_Read(CPU_ID_BASE,(u16*)pRcvBuf,6);
		STMFLASH_Read(*cpuAddr,(u16*)pRcvBuf,6);
#ifdef UART1_TRACE_DEBUG
		for(i=0;i<12;i++)
		{
			printf("%x ",pRcvBuf[i]);
		}
		printf("\r\n");
		for(i=0;i<12;i++)
		{
			printf("%x ",(pRcvBuf+12)[i]);
		}
		printf("\r\n");
#endif
		if(memcmp(veriBuf+4,pRcvBuf, 12)!=0)
		{
			CHARGE_LED1_ON();
			delay_ms(500);
			CHARGE_LED1_OFF();
			delay_ms(500);
			iap_load_app(0x08000000);
		}
	}else
	{
#ifdef UART1_TRACE_DEBUG
		printf("Invalid data %x\r\n",*pVeriFlag);
#endif
		CHARGE_LED2_ON();
		delay_ms(500);
		CHARGE_LED2_OFF();
		delay_ms(500);
		iap_load_app(0x08000000);
	}
#endif

	//USB_Init();

//#ifndef UART1_TRACE_DEBUG
	//if(PAin(0))
	if(PAin(1))
	{
		iap_load_app(FLASH_APP1_ADDR);
	}

	CC2530_RESET = 1;

	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	
//#endif

	//printf("boot\r\n");
	uartSendData("boot\r\n",sizeof("boot\r\n"));

#ifdef UART1_TRACE_DEBUG
	printf("AES BOOT\r\n");
#endif

	delay_ms(500);

	while(USART2->SR&(1<<5))
	{
		ret = USART2->DR;
	}
	
	while(1)
	{
		if(USB_Received_Flag)
		{
			USB_Received_Flag=0;
			ret = USB_GetData(pRcvBuf,USBHID_REC_LEN);
			pStCom = (STCOMMANDER*)pRcvBuf;
			ret = RcvDataParse(pStCom);
			if(!ret)
			{
				switch(pStCom->cmd)
				{
					case STM32_IAP_WRITE_PAGE: 
					//iap_write_appbin(FLASH_APP1_ADDR+pAddrInc,pStCom->buf,RCV_CMD_LEN);//����FLASH����   
					Boot_Decrypt(pStCom->buf,g_decKey,pStCom->buf,pStCom->data_len);
#if 0
					for(i=0;i<16;i++)
					printf("%x",pStCom->buf[i]);
#endif
					//ret = DES_Decrypt(pStCom->buf+8,g_decKey,pStCom->buf+8);
#ifdef UPDATE_FAST
					//if(pAddrInc<(107*1024))
					//if(pAddrInc<(100*1024))			//the encoded verifing cannot delete
					if(pAddrInc<FLASH_AVAILABLE)
					{
						//memcpy(STMFLASH_BUF+(pAddrInc&(STM_SECTOR_SIZE-1)), pStCom->buf, RCV_CMD_LEN);
						STMFLASH_Unlock();
						//if(!(pAddrInc&(STM_SECTOR_SIZE-1)))
						if(!(pAddrInc%STM_SECTOR_SIZE))
						{
							STMFLASH_ErasePage(FLASH_APP1_ADDR+pAddrInc);
						}
						STMFLASH_Write_NoCheck(FLASH_APP1_ADDR+pAddrInc,(u16*)pStCom->buf,RCV_CMD_LEN/2);
						STMFLASH_Lock();
						pAddrInc+=RCV_CMD_LEN;	 
					}else
					{
						ret = 3;
					}
#else
					STMFLASH_Write(FLASH_APP1_ADDR+pAddrInc,(u16*)pStCom->buf,(RCV_CMD_LEN/2));
					pAddrInc+=RCV_CMD_LEN;
#endif
					
					//USB_SendData(pRcvBuf,65);
					USB_SendData(&ret,1);
					break;

					case STM32_IAP_WRITE_OK:
					//DES_Decrypt(pStCom->buf,g_decKey,pStCom->buf,pStCom->data_len);
					//ret = DES_Decrypt(pStCom->buf+8,g_decKey,pStCom->buf+8);
					//STMFLASH_Write(FLASH_APP1_ADDR+pAddrInc,(u16*)pStCom->buf,RCV_CMD_LEN/2);
					USB_SendData(&ret,1);
					delay_ms(5);
					iap_load_app(FLASH_APP1_ADDR);
					//USB_SendData(pRcvBuf,65);
					//pAddrInc+=RCV_CMD_LEN;
					//printf("finished\r\n");
					//iap_load_app(FLASH_APP1_ADDR);//
					break;

					case STM32_IAP_VER:
					if ( pStCom->rsv * 16 < 512)
					{
						STMFLASH_Write(FLASH_VER_ADDR + (pStCom->rsv * 16) , (u16*)(pStCom->buf), pStCom->data_len);	
						/*
						if(FLASH_GetReadOutProtectionStatus() != SET)
						{
							FLASH_ReadOutProtection(ENABLE);
						}
						*/
						USB_SendData(&ret,1);
					}else
					{
						ret = 0xff;
						USB_SendData(&ret,1);
					}
					//printf("finished\r\n");
					break;

					case STM32_IAP_SERNUM:
						/*
					STMFLASH_Write(FLASH_SERNUM_ADDR,(u16*)pStCom->buf,RCV_CMD_LEN/2);
					*/
					USB_SendData(&ret,1);	
					break;

					case STM32_IAP_USER:
						/*
					STMFLASH_Write(FLASH_USER_ADDR+pUserInc,(u16*)pStCom->buf,RCV_CMD_LEN/2);
					//STMFLASH_Write(FLASH_FACTORY_ADDR+pUserInc,(u16*)pStCom->buf,RCV_CMD_LEN/2);
					pUserInc+=RCV_CMD_LEN;
					*/
					break;


					case STM32_IAP_READ_64BYTE:
						/*
					//printf("read\r\n");
					STMFLASH_Read(FLASH_APP1_ADDR+pReadInc,(u16*)pRcvBuf,64);
					USB_SendData(pRcvBuf,65);
					pReadInc+=64;
					*/
					break;

					case STM32_IAP_VERIRQ:
					if ((pStCom->buf[0] * 16) > 512)
					{
						memcpy((u8*)(pRcvBuf+2), g_bootVer, sizeof(g_bootVer));
						USB_SendData(pRcvBuf,2+16);
					}else
					{
						STMFLASH_Read(FLASH_VER_ADDR +  (pStCom->buf[0] * 16) , (u16*)pRcvBuf , pStCom->buf[1]*16);	
						//STMFLASH_Read(FLASH_SERNUM_ADDR,(u16*)(pRcvBuf+2),16/2);
						//memcpy((u8*)(pRcvBuf+2), g_bootVer, sizeof(g_bootVer));
						USB_SendData(pRcvBuf,pStCom->buf[2]);
						//printf("%02x,%02x\r\n",pRcvBuf[0],pRcvBuf[1]);
					}
					break;

					/******************************************************************
						reset cc2530 and clear the point
					*******************************************************************/
					case STM32_IAP_START:
					pAddrInc = 0;
					//RF_RESET = 0;
					//delay_ms(5);
					//RF_RESET = 1;
					pStCom->cmd = 0x48;
					pStCom->sum = CalcSum(pRcvBuf+1,4);
					uartSendData(pRcvBuf,6);
					errCod[0]= Message_Rcv(&errCod[1]);
					//USB_SendData(errCod,2);
					//ret = 0;
					USB_SendData(&ret,1);
					break;

					//case 0x51:
					//USB_SendData(pStCom->buf,65);
					//break;

					case STM32_IAP_CC2530:
					//Boot_Decrypt(pStCom->buf,g_decKey,pStCom->buf,pStCom->data_len);
					pStCom->cmd = 0x41;			//cc2530 burn cmd
					pStCom->sum = CalcSum(pRcvBuf+1,4+RCV_CMD_LEN);
					//printf("%x,\r\n",pStCom->sum);
					uartSendData(pRcvBuf,6+RCV_CMD_LEN);
					errCod[0]= Message_Rcv(&errCod[1]);
					//errCod[1] = 0x7E;
					
/*					
					if(!ret)
					{
						//ret = 0;
						printf("ret ok\r\n");
					}else
					{
						//ret = 1;
						//printf("no return or err");
						printf("ret err\r\n");
					}
*/
					//USB_SendData(&ret,1);
					USB_SendData(errCod,2);
					//USB_SendData(pRcvBuf,6+RCV_CMD_LEN);
					break;

					
				}
			}else
			{
					//printf("parse error: %x\r\n",ret);
					USB_SendData(&ret,1);
			}
		}
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  �����ڼ�������������ʱ��Դ�ļ����ʹ�������
  * @param  file Դ�ļ���
  * @param  line ������������ 
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* �û����������Լ��Ĵ������ڱ��������ļ�������������,
       ���磺printf("�������ֵ: �ļ��� %s �� %d��\r\n", file, line) */

    /* ����ѭ�� */
    while (1)
    {
    }
}
#endif

/*********************************END OF FILE**********************************/
