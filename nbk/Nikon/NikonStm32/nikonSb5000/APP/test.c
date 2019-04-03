#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"	
#include "includes.h"	 
#include "../HARDWARE/spi/spi.h"
#include "canon.h"
//Mini STM32��������չʵ�� 8
//ucos ʵ��
//����ԭ��@ALIENTEK
//����֧�֣�www.openedv.com

 
//���������ջ��С

#define STACK_COMPROC_SIZE     128
#define LED1_STK_SIZE    128
#define START_STK_SIZE   256

//�����������ȼ�

//�����ջ
OS_STK  TASK_LED1_STK[LED1_STK_SIZE];
//OS_STK  TASK_LED_STK[LED_STK_SIZE];
OS_STK TASK_COMPROC_STK[STACK_COMPROC_SIZE];
OS_STK  TASK_START_STK[START_STK_SIZE];
//��������
void TaskStart(void *pdata);
void TaskLed1(void *pdata);
extern void NikComProc(void *pdata);

u8 test_data[] = {
0x01,0x01,0x05,0x02,0x08,0x07,0x81,0x78,0xb4,0x64,0x0a,0x4e,
0xff,0x3e,0x90,0x3e,0x90,0x1d
};

//ϵͳʱ�����ú���
void SysTick_Configuration(void);

unsigned char g_comStatue=0;

	 
//Mini STM32������ucos����
//����ucos2.52�ں�.
//����ԭ��@ALIENTEK
//www.openedv.com
//2010.12.15			 
int main(void)
{			 	   
	Stm32_Clock_Init(9); //ϵͳʱ������
	delay_init(72);	     //��ʱ��ʼ�� 
	uart_init(72,115200);	 //���ڳ�ʼ��Ϊ9600
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ�  
	//PortInit();
	printf("Nikon APP\n");
	SPIx_Init();
	//ClkSynSend(0xEA);
	//PortInit();
	//ClkSynSend(0xEA);
	//printf("os start");
	//EXTIx_Init();
	SysTick_Configuration();
	OSInit();
	OSTaskCreate( TaskStart,	//task pointer
					(void *)0,	//parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],	//task stack top pointer
					START_TASK_Prio );	//task priority
	//printf("os start");	
	OSStart();
	return 0;	   
}
//��ʼ����
void TaskStart(void * pdata)
{
	pdata = pdata; 
	OS_ENTER_CRITICAL();   
	//OSTaskCreate(NikComProc, (void * )0, (OS_STK *)&TASK_COMPROC_STK[STACK_COMPROC_SIZE-1], COMPROC_TASK_PRIO);
	OSTaskCreate(TaskLed1, (void * )0, (OS_STK *)&TASK_LED1_STK[LED1_STK_SIZE-1], LED1_TASK_Prio);
	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
}


void TaskLed1(void *pdata)
{
	u8 i=0;
	//SPIx_Enable();
	while(1)
	{
		//TEST_SIGNAL = !TEST_SIGNAL;
		LED1=!LED1;

		//SPIx_Enable();
		//OSTimeDly(10);
		
		//ClkSynSend(0xEA);
		//OSTimeDly(10);
		//SPIx_ReadWriteByte(0xEA);
		//SPIx_Disable();
		for(i=0;i<sizeof(test_data);i++)
		{
			test_data[i] = SPIx_ReceiveByte();
			OSTimeDly(1);
			//delay_us(50);
			
		}
		//LiteBlockSend(test_data,sizeof(test_data));
		//LiteBlockSend("spi test",sizeof("spi test"));
		//SPI_MISO_AUX = !SPI_MISO_AUX;

		//OSTimeDlyHMSM(0,0,0,200);
		//OSTimeDly(100);
		OSTimeDly(100);
	}
}	  
//ϵͳʱ���жϷ�����
void SysTick_Handler(void)
{
	OS_ENTER_CRITICAL(); /* Tell uC/OS-II that we are starting an ISR  */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();        /* Call uC/OS-II's OSTimeTick()               */

    OSIntExit();         /* Tell uC/OS-II that we are leaving the ISR  */
}
//ϵͳʱ�����ã����1ms����һ���ж�
void SysTick_Configuration(void)
{
 	SysTick->CTRL&=~(1<<2);//SYSTICKʹ���ⲿʱ��Դ
	SysTick->CTRL|=1<<1;   //����SYSTICK�ж�
	SysTick->LOAD=9000;    //����1ms�ж�
	//bit2���,ѡ���ⲿʱ��  HCLK/8
	MY_NVIC_Init(3,3,SystemHandler_SysTick,2);//��2��������ȼ� 
	SysTick->CTRL|=1<<0;   //����SYSTICK
}




















