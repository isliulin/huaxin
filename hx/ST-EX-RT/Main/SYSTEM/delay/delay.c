//#include <stm32f10x_lib.h>
#include "stm32f10x.h"
#include "delay.h"
#include "../../user/golbal.h"
#include "../sys/sys.h"

static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������
//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 

void delay_ms(u16 nms)
{	 		  	  
	u32 temp;
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	//rsv = SysTick->CTRL;
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL=0x01 ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
	//SysTick->VAL =0X00;       //��ռ�����	  	    
}   
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
}

#if(_HARD_WARE_!=HARD_WARE_RFONLY)

void WLM_COMMAND_TRIGER(u16 time)
{
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	CHARGE_ENABLE=1; 		//disable charge
	HC_CON1 = 0;
	FLASH_PWM = 1;
	//Delay();
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			FLASH_PWM = 0;
			HC_CON1 = 1;
			CHARGE_ENABLE=0;
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
	
	FLASH_PWM = 0;
	HC_CON1 = 1;
	CHARGE_ENABLE=0;

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
	
}

void WLM_SHOT_ONCE(u16 time)
{
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	CHARGE_ENABLE=1; 		//disable charge
	HC_CON1 = 0;
	FLASH_PWM = 1;
	//Delay();
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_PWM = 0;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			break;
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
	
	FLASH_PWM = 0;
	HC_CON1 = 1;
	CHARGE_ENABLE=0;

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
}

#endif


































