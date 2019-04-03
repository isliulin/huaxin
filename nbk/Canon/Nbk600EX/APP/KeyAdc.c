#include "Golbal.h"
#include "display.h"
#include "KeyProc.h"
#include "usart.h"
#include "canon.h"

unsigned int g_Key;
extern unsigned char g_keyDir;
u8 g_shadowKey=0;

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
#if 1
void  Adc_Init(void)
{    
	//先初始化IO口
 	RCC->APB2ENR|=1<<4;    //使能PORTC口时钟 
	//GPIOA->CRL&=0XFFFFF00F;//PA1 2 anolog输入
	GPIOC->CRL&=0xFFFFF000;
	//通道10/11设置			 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 

	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
	ADC1->CR1&=~(1<<8);    //非扫描模式	  
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐	 
	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道0~3的采样时间
	//ADC1->SMPR2&=0XFFFFF000;//通道0,1,2,3采样时间清空	  
	//ADC1->SMPR2|=7<<9;      //通道3  239.5周期,提高采样时间可以提高精确度	 
	//ADC1->SMPR2&=0XFFFFFE07;
	//ADC1->SMPR2|=7<<6;      //通道2  239.5周期,提高采样时间可以提高精确度	 
	//ADC1->SMPR2|=7<<3;      //通道1  239.5周期,提高采样时间可以提高精确度	 
	//ADC1->SMPR2|=7<<0;      //通道0  239.5周期,提高采样时间可以提高精确度	 
	ADC1->SMPR1 |= 7<<0;		//channal 10 ,11 PC0, PC1
	ADC1->SMPR1 |= 7<<3;
	
	ADC1->CR2|=1<<0;	    //开启AD转换器
	g_keyAdcEnable = 1;
	
	ADC1->CR2|=1<<3;        //使能复位校准  
	while(ADC1->CR2&1<<3);  //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  
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
    
   ADC_DeInit(ADC1);//在这里复位被我注释掉了、至于为什么，我待会会说
    
      /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//这里对应上面所讲的配置，在这里就不给出注释了
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
    
    /* Enable ADC1 *///知道我为啥要在上面提醒大家要用老外的方式来看了吧、因为这里的注释都是用英文的
  //请不要以为我装逼，我这样做是有原因的、、原因我待会会说、你也会明白我最初的标题为何那样写

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

//获得ADC值
//ch:通道值 0~3
#if(_HARD_WARE_==HARD_WARE_VER1)
u16 GetKeyAdc(void)   
{
	unsigned int key=0;
		//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
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
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=11;
	ADC1->CR2|=1<<22;
	while(!(ADC1->SR&1<<1));
	return ADC1->DR>>4;
}
#elif(_HARD_WARE_==HARD_WARE_TSER)
u16 GetKeyAdc(void)
{
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
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
	
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
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

	if(EXTI_GetITStatus(EXTI_Line13) == SET)//                 ／／判断是否是１３脚的中断
       {
#ifdef SLAVE_POWER_DOWN
		Ex_NVIC_Disable(13);	
       	 g_comStatue = COM_STATUE_IDLE;					//2015/10/31 for slave sleep
#endif
               EXTI_ClearITPendingBit(EXTI_Line13);//                ／／清除中断标志位！
               return;//GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);//            ／／点亮ＬＥＤ            
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
	//EXTI->PR=1<<1;  //清除LINE0上的中断标志位  
	//EXTI->PR=1<<10;
	CLEAR_KEYINT();
}

