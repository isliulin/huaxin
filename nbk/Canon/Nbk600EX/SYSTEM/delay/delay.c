//#include <stm32f10x_lib.h>
#include "stm32f10x.h"
#include "delay.h"
#include "../../APP/golbal.h"
#include "../../APP/canon.h"
#include "../sys/sys.h"
#include "../../APP/FlashPara.h"

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
	u32 temp=0;
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

void WLM_COMMAND_TRIGER(u16 time)
{

#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;

	//if(g_isHotProtect>0)
	//{
	//	return;
	//}
	
	DISABLE_CHARGE();//disable charge
	HC_CON3 = 0;
	FLASH_PWM = 1;
	//delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	//FLASH_TIG = 0;
	delay_us(20);
	FLASH_TIG = 0;                                         //move here to add triger time    2016/07/15
	//delay_us(125);
	//Delay();
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_TIG = 0;
			FLASH_PWM = 0;
			HC_CON3 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			ENABLE_CHARGE();
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	HC_CON3 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
	
}

void WLM_HEAD_TRIGER(u16 time)
{

#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;

	//if(g_isHotProtect>0)
	//{
	//	return;
	//}
	
	DISABLE_CHARGE();//disable charge
	HC_CON3 = 0;
	FLASH_PWM = 1;
	//delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	//FLASH_TIG = 0;
	delay_us(20);
	FLASH_TIG = 0;                                                  //move here to add triger time    2016/07/15
	//delay_us(125);
	//Delay();
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_TIG = 0;
			FLASH_PWM = 0;
			HC_CON3 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			ENABLE_CHARGE();
			break;
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   

	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 

	do
	{
		temp=SysTick->CTRL;
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��  
	
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	HC_CON3 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif

}

#ifdef FLASH_BASE_FIBER
void FlashBaseFiber(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;

	//if(g_isHotProtect>0)
	//{
	//	return;
	//}
	
	DISABLE_CHARGE();//disable charge
	//HC_CON3 = 0;
	FLASH_PWM = 1;
	//delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	delay_us(20);                // 
	FLASH_TIG = 0;                //move here to add triger time    2016/07/15
	//delay_us(125);
	//Delay();
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
		if(!CMD_THRESHOLD_INT)
		{
			//FLASH_TIG = 0;
			//FLASH_PWM = 0;
			//HC_CON3 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			//ENABLE_CHARGE();
			break;
			
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON3 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
}

#endif


void WlmCmdOneShot(u16 time)
{
	if(!g_isHotProtect)
	{
		HC_CON2 = 1;		//2015/11/12
		HC_CON1 = 0;
		WLM_COMMAND_TRIGER(time);
	}
}


/*
void WlmCmdOneShot(u16 time)
{
	if(time<WLM_IMPLUS_TIME)
	{
		return;
	}
	WlmCmdHeadSend(time-WLM_IMPLUS_TIME);
}
*/

void WlmCmdHeadSend(u16 time)
{
	if(!g_isHotProtect)
	{
		HC_CON2 = 1;		//2015/11/12
		HC_CON1 = 0;
		WLM_HEAD_TRIGER(time);
	}
}

#ifdef FLASH_BASE_FIBER

void WLM_SHOT_ONCE(u16 time)
{
	HC_CON2 = 1;		//2015/11/12
	HC_CON1 = 0;
	FlashBaseFiber(time);
	HC_CON3 = 1;
}

void HP_SHOT_ONCE(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	//HC_CON1 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	//delay_us(20);
	FLASH_TIG = 1;
	delay_us(20);              //move here to add triger time    2016/07/15
	delay_us(5);
	FLASH_TIG = 0;

	//Delay();
	
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;		
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 

	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON1 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
}

#else
void WLM_SHOT_ONCE(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	//HC_CON1 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	//delay_us(20);
	FLASH_TIG = 1;
	delay_us(20);                      //move here to add triger time    2016/07/15
	delay_us(5);
	FLASH_TIG = 0;

	//Delay();
	
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;		
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 

	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON1 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
}
#endif

void TakePreFlash(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	//HC_CON1 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	//delay_us(20);
	FLASH_TIG = 1;
	delay_us(20);                    //move here to add triger time    2016/07/15
	delay_us(5);
	FLASH_TIG = 0;

	delay_us(50);

	SPI_MISO_AUX = 0;

	//Delay();
	
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;		
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 

	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON1 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SPI_MISO_AUX  = 1;

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
}

void ExtModFlash(u16 time)
{
#if 1
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	DISABLE_CHARGE();//disable charge
	HC_CON4 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	FLASH_TIG = 1;
	delay_us(5);
	delay_us(20);                               //move here to add triger time    2016/07/15
	FLASH_TIG = 0;
	//delay_us(125);
	delay_us(40);				//2016/02/17 in rf slave mode, when mst flash , its flash cannot be seen, delay for a while to release the flash
	//Delay();
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
		if(!EXT_MODE_DETEC)
		{
			//FLASH_TIG = 0;
			FLASH_PWM = 0;
			HC_CON4 = 1;
			//HC_CON1 = 1;
			//CHARGE_ENABLE=0;
			ENABLE_CHARGE();
		}
		
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 
	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	HC_CON4 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;
#endif
	
}

void TTLModFlash(u16 time)
{
	u32 temp;	    	 
	u32 loadRsv = SysTick->LOAD;
	u8 ctrlRsv = SysTick->CTRL;	
	u8 valRsv = SysTick->VAL;
	
	HC_CON1 = 1;
	//HC_CON4 = 1;
	HC_CON3 = 1;
	HC_CON2 = 0;
	
	DISABLE_CHARGE();//disable charge
	//HC_CON1 = 0;
	FLASH_PWM = 1;
	delay_us(5);
	//delay_us(20);
	FLASH_TIG = 1;
	delay_us(20);                    //move here to add triger time    2016/07/15      
	delay_us(5);
	FLASH_TIG = 0;

	//Delay();
	
	SysTick->LOAD=time*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;	
		if(BP_IN>0)
		{
			break;
		}
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	//SysTick->CTRL=0x00;       //�رռ�����
	//SysTick->CTRL = rsv;
	//SysTick->VAL =0X00;       //��ռ�����	 

	//FLASH_TIG = 0;
	FLASH_PWM = 0;
	//HC_CON1 = 1;
	//CHARGE_ENABLE=0;
	ENABLE_CHARGE();

	SysTick->CTRL=ctrlRsv;
	SysTick->LOAD=loadRsv;
	SysTick->VAL = valRsv;

}


void delay_seconds(u16 ns)
{
	while(ns--)
	{
		delay_ms(1000);
	}
}




































