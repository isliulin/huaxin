#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "usart.h"
#include "canon.h"

unsigned int g_Key;
extern unsigned char g_keyDir;
u8 g_shadowKey=0;

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
#if 1
void  Adc_Init(void)
{    
	//�ȳ�ʼ��IO��
 	RCC->APB2ENR|=1<<4;    //ʹ��PORTC��ʱ�� 
	//GPIOA->CRL&=0XFFFFF00F;//PA1 2 anolog����
	GPIOC->CRL&=0xFFFFF000;
	//ͨ��10/11����			 
	RCC->APB2ENR|=1<<9;    //ADC1ʱ��ʹ��	  
	RCC->APB2RSTR|=1<<9;   //ADC1��λ
	RCC->APB2RSTR&=~(1<<9);//��λ����	    
	RCC->CFGR&=~(3<<14);   //��Ƶ��������	
	//SYSCLK/DIV2=12M ADCʱ������Ϊ12M,ADC���ʱ�Ӳ��ܳ���14M!
	//���򽫵���ADC׼ȷ���½�! 
	RCC->CFGR|=2<<14;      	 

	ADC1->CR1&=0XF0FFFF;   //����ģʽ����
	ADC1->CR1|=0<<16;      //��������ģʽ  
	ADC1->CR1&=~(1<<8);    //��ɨ��ģʽ	  
	ADC1->CR2&=~(1<<1);    //����ת��ģʽ
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //�������ת��  
	ADC1->CR2|=1<<20;      //ʹ�����ⲿ����(SWSTART)!!!	����ʹ��һ���¼�������
	ADC1->CR2&=~(1<<11);   //�Ҷ���	 
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;     //1��ת���ڹ��������� Ҳ����ֻת����������1 			   
	//����ͨ��0~3�Ĳ���ʱ��
	//ADC1->SMPR2&=0XFFFFF000;//ͨ��0,1,2,3����ʱ�����	  
	//ADC1->SMPR2|=7<<9;      //ͨ��3  239.5����,��߲���ʱ�������߾�ȷ��	 
	//ADC1->SMPR2&=0XFFFFFE07;
	//ADC1->SMPR2|=7<<6;      //ͨ��2  239.5����,��߲���ʱ�������߾�ȷ��	 
	//ADC1->SMPR2|=7<<3;      //ͨ��1  239.5����,��߲���ʱ�������߾�ȷ��	 
	//ADC1->SMPR2|=7<<0;      //ͨ��0  239.5����,��߲���ʱ�������߾�ȷ��	 
	ADC1->SMPR1 |= 7<<0;		//channal 10 ,11 PC0, PC1
	ADC1->SMPR1 |= 7<<3;
	
	ADC1->CR2|=1<<0;	    //����ADת����
	g_keyAdcEnable = 1;
	
	ADC1->CR2|=1<<3;        //ʹ�ܸ�λУ׼  
	while(ADC1->CR2&1<<3);  //�ȴ�У׼���� 			 
    //��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ��������� 		 
	ADC1->CR2|=1<<2;        //����ADУ׼	   
	while(ADC1->CR2&1<<2);  //�ȴ�У׼����
	//��λ����������Կ�ʼУ׼������У׼����ʱ��Ӳ�����  
}
#else
void Adc_Init(void)
{

    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable ADC1 and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12MHZ 
    
  /* Configure PA.1 (ADC Channel) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  //GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
    
   ADC_DeInit(ADC1);//�����︴λ����ע�͵��ˡ�����Ϊʲô���Ҵ����˵
    
      /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//�����Ӧ�������������ã�������Ͳ�����ע����
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
    
    /* Enable ADC1 *///֪����ΪɶҪ���������Ѵ��Ҫ������ķ�ʽ�����˰ɡ���Ϊ�����ע�Ͷ�����Ӣ�ĵ�
  //�벻Ҫ��Ϊ��װ�ƣ�������������ԭ��ġ���ԭ���Ҵ����˵����Ҳ������������ı���Ϊ������д

  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

#endif

void KeyAdcEnable(void)
{
	g_keyAdcEnable = 1;
	Adc_Init();
}

void KeyAdcDisable(void)
{
	g_keyAdcEnable = 0;
	ADC_DeInit(ADC1);
}

//���ADCֵ
//ch:ͨ��ֵ 0~3
#if(_HARD_WARE_==HARD_WARE_VER1)
u16 GetKeyAdc(void)   
{
	unsigned int key=0;
		//����ת������	  		 
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch
	ADC1->SQR3|=AD_CHANNEL_KEY2;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));
	key = ADC1->DR;
	ADC1->SQR3&=0XFFFFFFE0;
	ADC1->SQR3|=AD_CHANNEL_KEY1;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));
	return (key>>4) | ((ADC1->DR<<4)&0xff00);
}
#elif(_HARD_WARE_==HARD_WARE_VER2)
u16 GetKeyAdc(void)
{
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch
	ADC1->SQR3|=11;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));
	return ADC1->DR>>4;
}
#elif(_HARD_WARE_==HARD_WARE_TSER)
u16 GetKeyAdc(void)
{
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch
	ADC1->SQR3|=AD_CHANNEL_KEY;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&(1<<1)));
	//delay_us(50);
	return ADC1->DR>>4;
}
#endif

u8 GetBatteryAdc(void)
{
//	u8 zoom=0;
	
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch
	ADC1->SQR3|=AD_CHANNEL_VOL;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));

	return (ADC1->DR>>4);
}


void EXTI15_10_IRQHandler(void)
{
/*
	if(COM_STATUE_PWRDN==g_comStatue)
	{
		g_comStatue = COM_STATUE_IDLE;
		CLEAR_PWINT();
		return;
	}
*/

	//static u8 g_shadowKey=0;

	if(EXTI_GetITStatus(EXTI_Line13) == SET)//                 �����ж��Ƿ��ǣ����ŵ��ж�
       {
#ifdef SLAVE_POWER_DOWN
		Ex_NVIC_Disable(13);	
       	 g_comStatue = COM_STATUE_IDLE;					//2015/10/31 for slave sleep
#endif
               EXTI_ClearITPendingBit(EXTI_Line13);//                ��������жϱ�־λ��
               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            ���������̣ţ�            
        }

#ifdef SLEEP_PWR_OFF
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		//SPI_MISO_AUX  = 1;
		//OSTimeDlyHMSM(0,0,0,1);
		//EXTIx_Init(FTIR);
		//D3V3_EN = 0;
		//BOOST_EN = 1;
		//EXTIx_Init(GPIO_A,FTIR);
		//SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
		//g_cmrLinked = 1;
		//g_comStatue = COM_STATUE_START;
#ifdef SLAVE_POWER_DOWN
		Ex_NVIC_Disable(15);	
		g_comStatue = COM_STATUE_IDLE;					//2015/10/31 for slave sleep
#endif
		EXTI_ClearITPendingBit(EXTI_Line15);
		return;
	}
#endif

	if(!g_shadowKey)
	{
		if(!KEY_DIR_1)
		{
			//g_keyDir = KEY_DIR_LEFT;
			g_shadowKey = KEY_DIR_LEFT;
		}else
		{
			//g_keyDir = KEY_DIR_RIGHT;
			g_shadowKey = KEY_DIR_RIGHT;
		}
		//Ex_NVIC_Config(GPIO_B,10,RTIR);
		SET_KEYINT_RTIG();
	}else
	{
		if((KEY_DIR_LEFT==g_shadowKey)&&(KEY_DIR_1>0))
		{
			//g_keyDir = KEY_DIR_LEFT;
			g_keyDir = KEY_DIR_RIGHT;
			SET_KEYINT_FTIG();
			DISABLE_KEY_INT();
		}else if((KEY_DIR_RIGHT==g_shadowKey)&&(!KEY_DIR_1))
		{
			//g_keyDir = KEY_DIR_RIGHT;
			g_keyDir = KEY_DIR_LEFT;
			SET_KEYINT_FTIG();
			DISABLE_KEY_INT();
		}else
		{
			SET_KEYINT_FTIG();
			ENABLE_KEY_INT();
			//FLAOK_TOGGLE();
		}

		g_shadowKey = 0;
		//DISABLE_KEY_INT();
	}
	
	//g_keyDir = 1;
	//Ex_NVIC_Disable(1);
	//ENABLE_KEY_INT();
	//DISABLE_KEY_INT();
	//EXTI->PR=1<<1;  //���LINE0�ϵ��жϱ�־λ  
	//EXTI->PR=1<<10;
	CLEAR_KEYINT();
}

