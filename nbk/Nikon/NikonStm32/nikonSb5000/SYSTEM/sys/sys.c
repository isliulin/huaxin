//#include <stm32f10x_lib.h>
#include "stm32f10x.h"
#include "sys.h"
#include "../../APP/Nikon.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//ϵͳʱ�ӳ�ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/27
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.4�޸�˵��
//��NVIC KO��,û��ʹ���κο��ļ�!
//������JTAG_Set����
////////////////////////////////////////////////////////////////////////////////// 	  

//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����
//CHECK OK
//091207
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 
  	//�������Ϸ���
	assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
	assert_param(IS_NVIC_OFFSET(Offset));  	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
	//���ڱ�ʶ����������CODE��������RAM��
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 
//CHECK OK
//091209
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����
//CHECK OK
//100329
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,s32 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	u8 IPRADDR=NVIC_Channel/4;  //ÿ��ֻ�ܴ�4��,�õ����ַ 
	u8 IPROFFSET=NVIC_Channel%4;//�����ڵ�ƫ��
	IPROFFSET=IPROFFSET*8+4;    //�õ�ƫ�Ƶ�ȷ��λ��
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;//ȡ����λ

	if(NVIC_Channel<32)NVIC->ISER[0]|=1<<NVIC_Channel;//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK)
	else NVIC->ISER[1]|=1<<(NVIC_Channel-32);    
	//NVIC->IPR[IPRADDR]|=temp<<IPROFFSET;//������Ӧ���ȼ����������ȼ�   	 
	NVIC->IP[IPRADDR]|=temp<<IPROFFSET;//������Ӧ���ȼ����������ȼ�   		
}

//�ⲿ�ж����ú���
//ֻ���GPIOA~G;������PVD,RTC��USB����������
//����:GPIOx:0~6,����GPIOA~G;BITx:��Ҫʹ�ܵ�λ;TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   
//������...
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//�õ��жϼĴ�����ı��
	EXTOFFSET=(BITx%4)*4;

	RCC->APB2ENR|=0x01;//ʹ��io����ʱ��

	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx
	
	//�Զ�����
	//EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
	//EXTI->EMR|=1<<BITx;//������line BITx�ϵ��¼� (������������,��Ӳ�����ǿ��Ե�,��������������ʱ���޷������ж�!)
 	if(DU_TIR==(TRIM&0x03))
	{
		EXTI->FTSR|=1<<BITx;
		EXTI->RTSR|=1<<BITx;
	}else if(TRIM&0x01)
	{
		EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
		EXTI->RTSR&=~(1<<BITx);
 	}else if(TRIM&0x02)
	{
		EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
		EXTI->FTSR&=~(1<<BITx);
	}

	EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
} 

void Ex_NVIC_Enable(u8 BITx)
{
	EXTI->IMR |= 1<<BITx;
}

void Ex_NVIC_Disable(u8 BITx)
{
	EXTI->IMR &= ~(1<<BITx);
}


//����������ִ���������踴λ!�����������𴮿ڲ�����.		    
//������ʱ�ӼĴ�����λ
//CHECK OK
//091209
void MYRCC_DeInit(void)
{										  					   
	RCC->APB1RSTR = 0x00000000;//��λ����			 
	RCC->APB2RSTR = 0x00000000; 
	  
  	RCC->AHBENR = 0x00000014;  //˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.	  
  	RCC->APB2ENR = 0x00000000; //����ʱ�ӹر�.			   
  	RCC->APB1ENR = 0x00000000;   
	RCC->CR |= 0x00000001;     //ʹ���ڲ�����ʱ��HSION	 															 
	RCC->CFGR &= 0xF8FF0000;   //��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR &= 0xFEF6FFFF;     //��λHSEON,CSSON,PLLON
	RCC->CR &= 0xFFFBFFFF;     //��λHSEBYP	   	  
	RCC->CFGR &= 0xFF80FFFF;   //��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR = 0x00000000;     //�ر������ж�
	//����������				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else   
	//MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x5000);
#endif
}
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI
//CHECK OK
//091209
__asm void WFI_SET(void)
{
	WFI;    
}
//�������ģʽ	 
//check ok 
//091202
void Sys_Standby(void)
{
	SCB->SCR|=1<<2;//ʹ��SLEEPDEEPλ (SYS->CTRL)	   
  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
 	PWR->CSR|=1<<8;          //����WKUP���ڻ���
	PWR->CR|=1<<2;           //���Wake-up ��־
	PWR->CR|=1<<1;           //PDDS��λ		  
	WFI_SET();				 //ִ��WFIָ��		 
}	  
//�󱸼Ĵ���д�����
//reg:�Ĵ������
//reg:Ҫд�����ֵ 
////check ok
////091202
//void BKP_Write(u8 reg,u16 dat)
//{
//  RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
//	RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��	    
//	PWR->CR|=1<<8;           //ȡ��������д���� 
//	switch(reg)
//	{
//		case 1:
//			BKP->DR1=dat;
//			break;
//		case 2:
//			BKP->DR2=dat;
//			break;
//		case 3:
//			BKP->DR3=dat;
//			break; 
//		case 4:
//			BKP->DR4=dat;
//			break;
//		case 5:
//			BKP->DR5=dat;
//			break;
//		case 6:
//			BKP->DR6=dat;
//			break;
//		case 7:
//			BKP->DR7=dat;
//			break;
//		case 8:
//			BKP->DR8=dat;
//			break;
//		case 9:
//			BKP->DR9=dat;
//			break;
//		case 10:
//			BKP->DR10=dat;
//			break;
//	} 
//}	    
//ϵͳ��λ
//CHECK OK
//091209
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 

//JTAGģʽ����,��������JTAG��ģʽ
//mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;
//CHECK OK	
//100818		  
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ
} 
//ϵͳʱ�ӳ�ʼ������
//pll:ѡ��ı�Ƶ������2��ʼ�����ֵΪ16	
//CHECK OK
//091209
void Stm32_Clock_Init(u8 PLL)
{
	unsigned char temp=0;   
	MYRCC_DeInit();		  //��λ������������
	RCC->CR|=0x00010000;  //�ⲿ����ʱ��ʹ��HSEON
	while(!(RCC->CR>>17));//�ȴ��ⲿʱ�Ӿ���
	RCC->CFGR=0X00000400; //APB1=DIV2;APB2=DIV1;AHB=DIV1;
	PLL-=2;//����2����λ
	RCC->CFGR|=PLL<<18;   //����PLLֵ 2~16
	RCC->CFGR|=5<<11;
	RCC->CFGR|=1<<16;	  //PLLSRC ON 
	FLASH->ACR|=0x32;	  //FLASH 2����ʱ����

	RCC->CR|=0x01000000;  //PLLON
	while(!(RCC->CR>>25));//�ȴ�PLL����
	RCC->CFGR|=0x00000002;//PLL��Ϊϵͳʱ��	 
	while(temp!=0x02)     //�ȴ�PLL��Ϊϵͳʱ�����óɹ�
	{   
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}    
}

void RCC_Configuration(void)
{
	/* RCC system reset(for debug purpose) */
	/* ��λ���е�RCC��Χ�豸�Ĵ��������ı��ڲ�������������λ��HSITRIM[4..0]���Լ�
	��������ƼĴ�����RCC_BDCR��,����״̬�Ĵ���RCC_CSR */
	RCC_DeInit();

	/* Enable HSE */
	/* ����HSE���� */
	/* ��������
	RCC_HSE_ON ����
	RCC_HSE_OFF �ر�
	RCC_HSE_BYPASS ʹ���ⲿʱ������
	*/
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	/* HSEStartUpStatusΪö�����ͱ�����2��ȡֵ��0ΪERROR����0ΪSUCCESS
	�ȴ�HSE׼���ã�����ʱʱ�䵽���˳�*/
	//HSEStartUpStatus = RCC_WaitForHSEStartUp();

	/* ��HSE׼����������ȶ��� */
	if(RCC_WaitForHSEStartUp()== SUCCESS)
	{
	/* HCLK = SYSCLK */
	/* ����AHBʱ�ӣ����ʱ�Ӵ�SYSCLK��Ƶ����
	��Ƶϵ����1��2��4��8��16��64��128��256��512
	*/
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* PCLK2 = HCLK */
	/* ���õ���APB2ʱ�ӣ����ʱ�Ӵ�AHBʱ�ӷ�Ƶ����
	��Ƶϵ��Ϊ1��2��4��8��16 */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* PCLK1 = HCLK/2 */
	/* ���õ���APB1ʱ�ӣ����ʱ�Ӵ�AHBʱ�ӷ�Ƶ����
	��Ƶϵ��Ϊ1��2��4��8��16 */
	RCC_PCLK1Config(RCC_HCLK_Div2);

	/* Flash 2 wait state */
	FLASH_SetLatency(FLASH_Latency_2);
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	/* PLLCLK = 8MHz * 9 = 72 MHz */
	/* ����PLL��ʱ��Դ�ͳ˷�����
	��һ����ڲ���Ϊʱ��Դ������3��
	RCC_PLLSource_HSI_Div2
	RCC_PLLSource_HSE_Div1
	RCC_PLLSource_HSE_Div2
	�˷�����RCC_PLLMul_2~RCC_PLLMul_16��֮���������
	*/
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	/* Enable PLL */
	/* �������ΪENABLE��DISABLE */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source */
	/*ѡ��ϵͳʱ��Դ����������
	RCC_SYSCLKSource_HSI �ڲ���������
	RCC_SYSCLKSource_HSE �ⲿ��������
	RCC_SYSCLKSource_PLLCLK PLLʱ��
	*/
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source */
	/* ����ϵͳ��ǰ��ʱ��Դ
	����ֵ��3��
	0x00 HSI�ǵ�ǰʱ��Դ
	0x04 HSE�ǵ�ǰʱ��Դ
	0x08 PLL�ǵ�ǰʱ��Դ
	*/
	while(RCC_GetSYSCLKSource() != 0x08)
	{
	}
	}

	/* Enable GPIOB, GPIOC and AFIO clocks */
	/* ʹ�ܵ�������APB2�ϵ���Χ�豸
	�������IO�˿�B��ʱ��
	AFIO����ӳ�书�ܵ�ʱ�ӣ�һ��δʹ��REMAP����ʱ����ʱ�Ӳ��ؿ���
	���������UM0472 PAGE 426
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

void SetMstPwrOnExti(u8 triMod)
{
	GPIO_InitTypeDef GPIO_InitStructure;
       EXTI_InitTypeDef EXTI_InitStructure;
       NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(EXTI_Trigger_Falling == triMod)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
	}else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	}

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//        ������ش򿪣ǣУɣϣ��ͣ��ƣɣ�ʱ�ӣ�����

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);//                �������ã��ڵã�����Ϊ�ж�
       EXTI_ClearITPendingBit(EXTI_Line3);//                ��������жϣ�ò�Ʋ���Ҳû�����⣬���Ǳ���һ�£�
 
       EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//                �����ⲿ�ж�
       //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//                �����½��ش���
       if(EXTI_Trigger_Falling == triMod)
       {
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	}else if(EXTI_Trigger_Rising == triMod)
	{
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	}else
	{
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	}
       
       EXTI_InitStructure.EXTI_Line = EXTI_Line3;//
       EXTI_InitStructure.EXTI_LineCmd = ENABLE;//        
       EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	//NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//                ����ռ�����ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//                         �����������ȼ���
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
	NVIC_Init(&NVIC_InitStructure);
	
	
}

void SleepWakeIntSet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
       EXTI_InitTypeDef EXTI_InitStructure;
       NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//        ������ش򿪣ǣУɣϣ��ͣ��ƣɣ�ʱ�ӣ�����

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);//                �������ã��ڵã�����Ϊ�ж�
       EXTI_ClearITPendingBit(EXTI_Line1);//                ��������жϣ�ò�Ʋ���Ҳû�����⣬���Ǳ���һ�£�
 
       EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//                �����ⲿ�ж�
       //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//                �����½��ش���
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
       
       EXTI_InitStructure.EXTI_Line = EXTI_Line1;//
       EXTI_InitStructure.EXTI_LineCmd = ENABLE;//        
       EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	//NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//                ����ռ�����ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//                         �����������ȼ���
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //��������

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	EXTI_ClearITPendingBit(EXTI_Line15);

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//                �����ⲿ�ж�
       //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//                �����½��ش���
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
       
       EXTI_InitStructure.EXTI_Line = EXTI_Line15;//
       EXTI_InitStructure.EXTI_LineCmd = ENABLE;//        
       EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	//NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//                ����ռ�����ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//                         �����������ȼ���
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
	NVIC_Init(&NVIC_InitStructure);
	

}

//void SetPwrOnExti(void)
void SetPwrOnExti(u8 triMod)
{
	GPIO_InitTypeDef GPIO_InitStructure;
       EXTI_InitTypeDef EXTI_InitStructure;
       NVIC_InitTypeDef NVIC_InitStructure;
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(EXTI_Trigger_Falling == triMod)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
	}else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
	}
       
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);//        ������ش򿪣ǣУɣϣ��ͣ��ƣɣ�ʱ�ӣ�����

        // �������������ⲿ�жϣ��൱�ڵ�Ƭ�����ж����á�

       GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource13);//                �������ã��ڵã�����Ϊ�ж�
       EXTI_ClearITPendingBit(EXTI_Line13);//                ��������жϣ�ò�Ʋ���Ҳû�����⣬���Ǳ���һ�£�
 
       EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//                �����ⲿ�ж�
       //EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//                �����½��ش���
       if(EXTI_Trigger_Falling == triMod)
       {
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	}else if(EXTI_Trigger_Rising == triMod)
	{
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	}else
	{
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	}
       
       EXTI_InitStructure.EXTI_Line = EXTI_Line13;//
       EXTI_InitStructure.EXTI_LineCmd = ENABLE;//        
       EXTI_Init(&EXTI_InitStructure);

	//���Ĳ������ãΣ֣ɣã����Ҳ�ǣӣԣͣ�������ɫ�ɡ�

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
        //NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
				NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//                ����ռ�����ȼ���
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//                         �����������ȼ���
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
        NVIC_Init(&NVIC_InitStructure);
}


//void LowPow_Init(void)
void LowPow_Init(u8 wkEn,u8 tMod,u8 wkSrc)
{


	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  //��ȫʧ��JTAG/SWD ���ص���Ҳ����Ӱ��
	GPIO_PinRemapConfig( GPIO_Remap_PD01 , ENABLE );//����ΪGPIO�����ص���Ҳ����Ӱ��
	RCC_LSEConfig(RCC_LSE_OFF);//�ر�RTC�� ���ص���Ҳ����Ҳ��Ӱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	//ADC_CONFIG(DISABLE);
	//g_keyAdcEnable = 0;
	//ADC_DeInit(ADC1);
	KeyAdcDisable();		//disable keyAdc
	SPI_I2S_DeInit(SPI1);
	
	//RCC_DeInit();
	USART_DeInit(USART1);
	USART_DeInit(USART2);
	//USART_DeInit(USART3);
	//USART_DeInit(UART5);
#if 1
	//RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	//RCC->APB2ENR|=1<<5;		 //ʹ��PORTDʱ��

	//JTAG_Set(JTAG_SWD_DISABLE);//�ر�JTAG��SWD   

	//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_SLEEP,0);
	//while(uart_tran_flag);

	//g_comStatue = COM_STATUE_PWRDN;

	//BkLiteAutoOff();	
	//DISABLE_CHARGE();
	//CHARGE_LED1_OFF;
	//CHARGE_LED2_OFF;
	//ADC1->CR2&=~(1<<0);

	//POW_ON = 0;		//turn off the power supply
	//BOOST_EN = 0;
	//DISABLE_CHARGE;
	LCD_RST = 0;
	BOOST_EN = 0;
	D3V3_EN = 1;
//	CC2530_PWR = 0;		//FOR NEW board 2015/12/04

	if(wkEn>0)
	{
		if(wkSrc>0)
		{
			SetMstPwrOnExti(tMod);
		}else
		{
			SetPwrOnExti(tMod);
		}
	}

	delay_ms(5);

	g_comStatue = COM_STATUE_PWRDN;


#if 1			//delete in oder to simplicity
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	RCC_Configuration();
	
#endif


#if 0
	Ex_NVIC_Disable(0);
#else
	Ex_NVIC_Disable(13);
#endif

/******************************************************************************************************
//	send rf parameter
******************************************************************************************************/
	//RfParaSend();
	//delay_ms(50);

	//POW_ON = 1;	
	BOOST_EN = 1;
	//BOOST_EN = 0;
	D3V3_EN = 0;

	g_comStatue = COM_STATUE_IDLE;

	g_SettingIndex = 0;
	
#endif

 	
	
}


void EXTI3_IRQHandler(void)
{
	EXTI->PR=1<<3;
	Ex_NVIC_Disable(13);
}


























