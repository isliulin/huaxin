//#include <stm32f10x_lib.h>		 
#include "stm32f10x.h"
#include "sys.h"
#include "../../APP/golbal.h" 
#include "../../HARDWARE/STMFLASH/stmflash.h"
#include "usart.h"
#include "../../APP/canon.h" 



//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����
//CHECK OK
//091207
typedef  void (*iapfun)(void);
u8 g_sleepFlag=0;
extern unsigned char uart_tran_flag;
extern void RfParaSend(void);
extern void BkLiteAutoOff(void);
extern void EXTIx_Init(u8 GPIOx,u8 TirEdge);
extern void SPIx_Init(void);
extern unsigned char g_comStatue;
extern char g_SettingIndex;
extern u16 g_dspUpdateFlag;
extern signed char *g_cmdMode;

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
//void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u32 NVIC_Channel,u8 NVIC_Group)	 
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
 	//if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
	//if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
	//not active rasing eadge and falling eadge at the same time
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
	
#ifdef NBK600EX_DEBUG
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0); 
#else
	MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x5000);	   
	//MY_NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);	
#endif

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
//�ر������ж�
__asm void INTX_DISABLE(void)
{
	CPSID I;		  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE I;		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
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

void Sys_StopMode()
{
	//g_sleepFlag = 1;
	SCB->SCR|=1<<2;
	RCC->APB1ENR|=1<<28;
	PWR->CR |= 1<<2;
	PWR->CR &= 0xFD;
	PWR->CR |= 0x01;		//��ͣ��ģʽ�µ�ѹ��ѹ�����ڵ͹���ģʽ
	WFI_SET();
} 

#if 1//delete in oder to simplicity
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
#endif

/*****************************************************************************************************************
����Ҫ��Ϊ��
1������IO�ܽţ��������״̬�˿��Ǹߵ�ƽ��������������룬�������״̬�ǵ͵�ƽ������������룬����������м�״̬�����ģ�����롣����ܶ��˶��ᵽ��������ġ���Ϊ����ھ����ˣ��������������ʲô������һ��Ҫ�䣬Ӳ���ϼ��������Ϳ�����
2���������������Ҫremap����ͨIO������ʹ���ڲ�����
3��pwr��ʱ��Ҫʹ�ܣ���RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);���Ҳ�൱��Ҫ
4���ر�jtag�ڣ��������ͨIO��
5��ע�������������������ע���·���֮���Ƿ��ˮ��������
�����⼸��Ҫ�㣬�����ж�ʲô�Ķ��У����������徲�ˣ�������ȫ���ܺġ�
*******************************************************************************************************************/

void SetPwrOnExti(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
       EXTI_InitTypeDef EXTI_InitStructure;
       NVIC_InitTypeDef NVIC_InitStructure;
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);//        ������ش򿪣ǣУɣϣ��ͣ��ƣɣ�ʱ�ӣ�����

        // �������������ⲿ�жϣ��൱�ڵ�Ƭ�����ж����á�

       GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource13);//                �������ã��ڵã�����Ϊ�ж�
       EXTI_ClearITPendingBit(EXTI_Line13);//                ��������жϣ�ò�Ʋ���Ҳû�����⣬���Ǳ���һ�£�
 
       EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//                �����ⲿ�ж�
       EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//                �����½��ش���
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
void LowPow_Init(u8 wkEn)
{
	//set wakeup pin
	//g_sleepFlag = 1;
//	GPIO_InitTypeDef GPIO_InitStructure;
//       EXTI_InitTypeDef EXTI_InitStructure;
//     NVIC_InitTypeDef NVIC_InitStructure;
	
//	ADC1->CR2 &= ~1;			//turn off adc
	//ADC_DeInit(ADC1);
	//USART_DeInit(USART1);
	//USART_DeInit(USART2);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);


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

	//EXTI_INITIAL(ENABLE);//���¶˿ڵ���������Ӱ��
	//GPIO_PIN_INITIAL(GPIOA,GPIO_Pin_All,GPIO_Mode_AIN,0); //�˴�û���ⲿ���ѡ������ڲ��ԡ�EXTI��������GPIO��Ӱ�����
	//GPIO_PIN_INITIAL(GPIOB,GPIO_Pin_All,GPIO_Mode_AIN,0); 
	//GPIO_PIN_INITIAL(GPIOC,GPIO_Pin_All,GPIO_Mode_AIN,0); 
	//GPIO_PIN_INITIAL(GPIOD,GPIO_Pin_All,GPIO_Mode_AIN,0);

	//EXTI_INITIAL(ENABLE);//���¶˿ڵ���������Ӱ��
	//GPIO_PIN_INITIAL(GPIOA,GPIO_Pin_All,GPIO_Mode_AIN,0); //�˴�û���ⲿ���ѡ������ڲ��ԡ�EXTI��������GPIO��Ӱ�����
	//GPIO_PIN_INITIAL(GPIOB,GPIO_Pin_All,GPIO_Mode_AIN,0); 
	//GPIO_PIN_INITIAL(GPIOC,GPIO_Pin_All,GPIO_Mode_AIN,0); 
	//GPIO_PIN_INITIAL(GPIOD,GPIO_Pin_All,GPIO_Mode_AIN,0);
	//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);//STOPģʽ
	//PWR_EnterSTANDBYMode();//standbyģʽ 
	//RCC->CR &= ~(1<<0);
	//RCC->CR &=~(1<<16);
	//RCC->CR &=~(1<<24);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,DISABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,DISABLE);
	//BK_LIGHT = 0;
#if 1
	//RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	//RCC->APB2ENR|=1<<5;		 //ʹ��PORTDʱ��

	//JTAG_Set(JTAG_SWD_DISABLE);//�ر�JTAG��SWD   

	//UartSctSend(UART_ENCODE_RFCMD,UART_ENCODE_RFCMD_SLEEP,0);
	//while(uart_tran_flag);

	g_comStatue = COM_STATUE_PWRDN;

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
	CC2530_PWR = 0;		//FOR NEW board 2015/12/04

#if 0
	GPIOA->CRL&=0XFFFFFFF0;//PA0���ó�����	  
	GPIOA->CRL|=0X00000008;   
	//GPIOA->CRH&=0X0F0FFFFF;//PA13,15���ó�����	  
	//GPIOA->CRH|=0X80800000; 				   
	//GPIOA->ODR|=1<<13;	   //PA13����,PA0Ĭ������
	//GPIOA->ODR|=1<<15;	   //PA15����

	//GPIOA->CRL&=0XFF0FFFFF;//PA0���ó�����	  
	//GPIOA->CRL|=0X00800000;
	//GPIOA->ODR|=1<<5;


	Ex_NVIC_Config(GPIO_A,0,RTIR); //�����ش���
	//Ex_NVIC_Config(GPIO_A,13,FTIR);//�½��ش���
	//Ex_NVIC_Config(GPIO_A,15,FTIR);//�½��ش���
	//Ex_NVIC_Config(GPIO_A,5,FTIR);
	
	//MY_NVIC_Init(2,1,EXTI9_5_IRQChannel,2); 
	MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //��ռ2�������ȼ�2����2
	//MY_NVIC_Init(2,1,EXTI15_10_IRQChannel,2);//��ռ2�������ȼ�1����2	   

#else
#if 0
	GPIOA->CRH&=0XFF0FFFFF;
	GPIOA->CRH|=0X00800000;
	GPIOA->ODR|=(1<<13);

	Ex_NVIC_Config(GPIO_A,13,FTIR);
	
	MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);
#endif

	if(wkEn>0)
	{
/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       GPIO_Init(GPIOA, &GPIO_InitStructure);

       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);//        ������ش򿪣ǣУɣϣ��ͣ��ƣɣ�ʱ�ӣ�����

        // �������������ⲿ�жϣ��൱�ڵ�Ƭ�����ж����á�

       GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource13);//                �������ã��ڵã�����Ϊ�ж�
       EXTI_ClearITPendingBit(EXTI_Line13);//                ��������жϣ�ò�Ʋ���Ҳû�����⣬���Ǳ���һ�£�
 
       EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//                �����ⲿ�ж�
       EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//                �����½��ش���
       EXTI_InitStructure.EXTI_Line = EXTI_Line13;//
       EXTI_InitStructure.EXTI_LineCmd = ENABLE;//        
       EXTI_Init(&EXTI_InitStructure);

	//���Ĳ������ãΣ֣ɣã����Ҳ�ǣӣԣͣ�������ɫ�ɡ�

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
        NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;//                         ����ͨ���������ڣ���������֮�䣬���Ǽ����ǹ��õģ�
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//                ����ռ�����ȼ���
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//                         �����������ȼ���
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//
        NVIC_Init(&NVIC_InitStructure);
*/
	SetPwrOnExti();

	
#endif

	}


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

	//ADC1->CR2|=(1<<0);

	//BkLiteOn();

	
#endif


	//Sys_Standby();
	//Sys_StopMode();
	//WFI_SET();

 	
	
}

void SleepPortUninit(u32 *confReg,u16 *datReg)
{
	confReg[0] = GPIOA->CRL;
	confReg[1] = GPIOA->CRH;
	confReg[2] = GPIOB->CRL;
	confReg[3] = GPIOB->CRH;
	confReg[4] = GPIOC->CRL;
	confReg[5] = GPIOC->CRH;

	confReg[6] = GPIOD->CRL;
	confReg[7] = GPIOD->CRH;

	confReg[8] = GPIOE->CRL;
	confReg[9] = GPIOE->CRH;

	datReg[0] = GPIOA->ODR;
	datReg[1] = GPIOB->ODR; 
	datReg[2] = GPIOC->ODR;

	datReg[3] = GPIOD->ODR;
	datReg[4] = GPIOE->ODR;

#if 1
	PortUninit();			//sleep2

#else						//sleep3
	GPIOA->CRL = 0x88888888;
	GPIOA->CRH = 0x88888888;
	GPIOA->ODR =0x6013;


	GPIOB->CRL = 0x88888888;			//RF RESET
	GPIOB->CRH &= 0x00000000;
	GPIOB->CRH |= 0x88838888;
//	GPIOB->CRH = 0x88838888;
//	GPIOB->CRH = 0x88838888;
/*	GPIOB->CRH = 0x88838888;			//30V 3V3
*/	GPIOB->ODR =0x8000;


	GPIOC->CRL = 0x88888888;	
//	GPIOC->CRL &= 0x00000FFF;
//	GPIOC->CRL |= 0x88888000;
	GPIOC->CRH = 0x88888888;			
	GPIOC->ODR =0x0000;


	GPIOD->CRL = 0x88888888;			
	GPIOD->CRH = 0x88888388;			
	GPIOD->ODR =0x0000;

	GPIOE->CRL = 0x88888888;			
	GPIOE->CRH = 0x88888888;			
	GPIOE->ODR =0x0000;
#endif
	
}

void SleepWakeIntSet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//       EXTI_InitTypeDef EXTI_InitStructure;
//       NVIC_InitTypeDef NVIC_InitStructure;
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//                �������ãУ����������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
       GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //pull down

	GPIO_Init(GPIOA, &GPIO_InitStructure);

/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //pull down

	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
*/

//	GPIO_Write(GPIOA, 0X6003);
}

void SlaveSleepWakeIntSet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
//       EXTI_InitTypeDef EXTI_InitStructure;
//       NVIC_InitTypeDef NVIC_InitStructure;
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA0
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PA1
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
       //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
       GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //pull down

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //pull down

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//                �������ãУ������������� 
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //pull down

	GPIO_Init(GPIOC, &GPIO_InitStructure);

	Ex_NVIC_Config(GPIO_A,0,FTIR);
	Ex_NVIC_Config(GPIO_A,1,FTIR);
	//Ex_NVIC_Config(GPIO_B,5,FTIR);
	Ex_NVIC_Config(GPIO_A,15,RTIR);	

	if(CMD_MODE_LIGH_SLA==*g_cmdMode)
	{
		Ex_NVIC_Config(GPIO_C,6,FTIR);
	}else if(CMD_MODE_RF_SLA==*g_cmdMode)
	{
		Ex_NVIC_Config(GPIO_B,5,FTIR);
	}

	//EXTIx_Init(GPIO_A,RTIR);
	//Ex_NVIC_Config(GPIO_A,5,RTIR);

	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);
	//MY_NVIC_Init(2,2,EXTI1_IRQChannel,1);
	//MY_NVIC_Init(0,0,EXTI9_5_IRQChannel,2);
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
	MY_NVIC_Init(2,1,EXTI0_IRQn,1);
	MY_NVIC_Init(2,2,EXTI1_IRQn,1);
	MY_NVIC_Init(0,0,EXTI9_5_IRQn,2);
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,1);

	SetPwrOnExti();

}

void SlaveSleepWakeIntClear(void)
{
	Ex_NVIC_Disable(0);
	Ex_NVIC_Disable(1);
	Ex_NVIC_Disable(13);
	Ex_NVIC_Disable(15);
	Ex_NVIC_Disable(6);
	Ex_NVIC_Disable(5);
}


void SleepPortInit(u32 *confReg,u16 *datReg)
{

	GPIOA->CRL = confReg[0];
	GPIOA->CRH = confReg[1];

	GPIOB->CRL = confReg[2];
	GPIOB->CRH = confReg[3];

	GPIOC->CRL = confReg[4];
	GPIOC->CRH = confReg[5];

	GPIOD->CRL = confReg[6];
	GPIOD->CRH = confReg[7];
	GPIOE->CRL = confReg[8];
	GPIOE->CRH = confReg[9];	


	GPIOA->ODR = datReg[0];
	GPIOB->ODR = datReg[1];
	GPIOC->ODR = datReg[2];

	GPIOD->ODR = datReg[3];
	GPIOE->ODR = datReg[4];
	

}

#ifdef SLEEP_PWR_OFF
void Sleep_Init(void)
{
	u32 confRegRsv[10]={0};
	u16 datRegRsv[5] = {0};
	
	//BkLiteAutoOff();					//SleepPortUninit
	//BK_TURNOFF(0);
	//KEY_BK_OFF();
	
	
	//DISABLE_CHARGE();				//SleepPortUninit
	//LCD_Clear(0);
	//LCD_WRITE_CMD(0XAF);
	DisplayOff();

	//PortUninit();	

	//PortUninit();
	DISABLE_KEY_INT();
#ifdef SLEEP_PWR_OFF
	DISABLE_FLASH_INT();
	DISABLE_SYN_INT();
#endif
	//Ex_NVIC_Disable(5);
	//Ex_NVIC_Disable(4);
	g_SettingIndex = 0;
	g_chargeOk = 0;						//2015/08/10  to avoid twinkle when waked up
	
	//CHARGE_LED1_OFF;
	//CHARGE_LED2_OFF;
	CHARGE_LED_OFF();
	//FLAOK_OFF();
	
	//BOOST_EN = 1;
	//D3V3_EN = 0;

	//g_comStatue = COM_STATUE_SLEEP;
	//LCD_RST = 0;					//SleepPortUninit

//for test
//	BOOST_EN = 0;
//	D3V3_EN = 1;
/**************************************
for test enable RF module to sleep
***************************************/
	DISABLE_RF_MODULE();			//SleepPortUninit
/**************************************
end
***************************************/
#ifdef SLEEP_PWR_OFF
//	ADC1->CR2 &= ~(1<<0);			//turn off adc
//	g_keyAdcEnable = 0;
//	ADC_DeInit(ADC1);
	KeyAdcDisable();
#else
//	g_keyAdcEnable = 0;
//	ADC1->CR2 &= ~(1<<0);			//turn off adc
	KeyAdcDisable();
#endif
/***************************************************
2015/08/21 set portUninit and turn on 5v and 3v3
****************************************************/
	//SleepPortUninit();
	//SPI_I2S_DeInit(SPI2);
//	SPIx_Disable();
	SleepPortUninit(confRegRsv,datRegRsv);
#ifdef SLEEP_PWR_OFF
	D3V3_EN = 1;
	BOOST_EN = 0;
	CC2530_PWR = 0;			//FOR NEW board 2015/12/04
//this is where should pay special attention, after ADC_DeInit,  OSTimeDlyHMSM will cause task switched, than 
//the keyprocess and voltage check will excuted, but the ADC is closed, so the task will cause wait perminately
	delay_ms(10);		
	//OSTimeDlyHMSM(0,0,0,10);
	//OSTimeDlyHMSM(0,0,0,10);
#endif	
/**************************************************/

	SleepWakeIntSet();				//sleep1
	//GPIOA->CRL&=0XFF0FFF00;//PA0���ó�����	  
	//GPIOA->CRL|=0X00800088; 

	//GPIOC->CRL&=0xFFF0FFFF;
	//GPIOC->CRL|=0x00030000;

	Ex_NVIC_Config(GPIO_A,0,FTIR);
	Ex_NVIC_Config(GPIO_A,1,FTIR);
#ifdef SLEEP_PWR_OFF
	Ex_NVIC_Config(GPIO_A,15,RTIR);
#endif
	//EXTIx_Init(GPIO_A,RTIR);
	//Ex_NVIC_Config(GPIO_A,5,RTIR);

	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);
	//MY_NVIC_Init(2,2,EXTI1_IRQChannel,1);
	MY_NVIC_Init(2,1,EXTI0_IRQn,1);
	MY_NVIC_Init(2,2,EXTI1_IRQn,1);
#ifdef SLEEP_PWR_OFF
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
	MY_NVIC_Init(2,3,EXTI15_10_IRQn,1);
#endif

	SetPwrOnExti();
	//SleepWakeIntSet();
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);
	delay_ms(10);	
	
	g_comStatue = COM_STATUE_SLEEP;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	RCC_Configuration();

	//while(!PILOT_IN);				//avoid unexpected flash

	//HaltSec(100);

	Ex_NVIC_Disable(0);
	Ex_NVIC_Disable(1);
	Ex_NVIC_Disable(13);
#ifdef SLEEP_PWR_OFF
	Ex_NVIC_Disable(15);
	ENABLE_FLASH_INT();
	ENABLE_SYN_INT();
#endif

//	Adc_Init();

	//if(KEY_POW_OFF>0)
	if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
/***************************************************
2015/08/21 set portUninit and turn on 5v and 3v3
****************************************************/
		//SleepPortUninit();
		//SleepPortUninit(confRegRsv,datRegRsv);
#ifdef SLEEP_PWR_OFF
//		D3V3_EN = 0;
//		BOOST_EN = 1;
#endif	
/**************************************************/
	
		ENABLE_KEY_INT();

	/*******************************************************/
	//	port initialize
	/*******************************************************/
	//	PortInit();				//2015/08/21
		SleepPortInit(confRegRsv,datRegRsv);

		//OSTimeDlyHMSM(0,0,0,1);
		delay_ms(1);

		//if(COM_STATUE_START==g_comStatue)
		if(CLK_IN>0)
		{
			EXTIx_Init(GPIO_A,FTIR);
			//OSTimeDlyHMSM(0,0,0,1);
			delay_ms(1);
			SPIx_Init();							//it is pratically the EXTIx_Init() should place before the SPIx_Init(),   why?	//because the pin5 set				
			g_cmrLinked = 1;
			g_comStatue = COM_STATUE_START;
		}else
		{
			EXTIx_Init(GPIO_A,RTIR);
			SPI_MISO_AUX  = 0;
			COM_AUX = 0;
			g_cmrLinked = 0;
			g_comStatue = COM_STATUE_IDLE;
		}
	/*******************************************************/

	//for test
	//	BOOST_EN = 1;
	//	D3V3_EN = 0;

/**********************************************************/
#ifdef SLEEP_PWR_OFF
	//	ADC1->CR2|=(1<<0);
	//	g_keyAdcEnable = 1;
	//	Adc_Init();
	KeyAdcEnable();
#else
	//	g_keyAdcEnable = 1;
	//	ADC1->CR2|=(1<<0);
	KeyAdcEnable();
#endif
/**********************************************************/

		LCD_Init();

		//SET_UPDATE_EVT(DSP_UPDATE_MODE);
		Mode();

#ifdef RF_LOWPWR_SLEEP
		CC2530_RESET = 0;
		delay_ms(400);
		CC2530_RESET = 1;
#else
/**************************************
for test enable RF module to sleep
***************************************/
		DISABLE_RF_MODULE();			//SleepPortUninit
/**************************************
end
***************************************/
		delay_ms(400);
#endif

		while(!PILOT_IN);	

		//if(KEY_POW_OFF>0)
		//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
		{
			RfMoudleSet();
		}
	}
	//OSTaskSuspend(CMDSEND_TASK_Prio);

	//BOOST_EN = 1;
	//BOOST_EN = 0;
	//D3V3_EN = 0;
	g_expAccHotWarnValue = 0;
	

	//PILOT_IN    PA1 PA0 PA5
	//EXTIx_Init(GPIO_A,RTIR);				//PA5 SPI_CLK
	//Ex_NVIC_Config(GPIO_A,0,RTIR); //�����ش���	
	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //��ռ2�������ȼ�2����2
	
}

#else
void Sleep_Init(void)
{
	u32 confRegRsv[10]={0};
	u16 datRegRsv[5] = {0};
	
	//BkLiteAutoOff();					//SleepPortUninit
	BK_TURNOFF(0);
	KEY_BK_OFF();
	
	
	DISABLE_CHARGE();				//SleepPortUninit
	//LCD_Clear(0);
	//LCD_WRITE_CMD(0XAF);
	//DisplayOff();

	//PortUninit();	

	//PortUninit();
	DISABLE_KEY_INT();
#ifdef SLEEP_PWR_OFF
	DISABLE_FLASH_INT();
	DISABLE_SYN_INT();
#endif
	//Ex_NVIC_Disable(5);
	//Ex_NVIC_Disable(4);
	g_SettingIndex = 0;
	g_chargeOk = 0;						//2015/08/10  to avoid twinkle when waked up
	
	//CHARGE_LED1_OFF;
	//CHARGE_LED2_OFF;
	CHARGE_LED_OFF();
	FLAOK_OFF();
	
	//BOOST_EN = 1;
	//D3V3_EN = 0;

	//g_comStatue = COM_STATUE_SLEEP;
	LCD_RST = 0;					//SleepPortUninit

//for test
//	BOOST_EN = 0;
//	D3V3_EN = 1;
/**************************************
for test enable RF module to sleep
***************************************/
//	DISABLE_RF_MODULE();			//SleepPortUninit
/**************************************
end
***************************************/
#ifdef SLEEP_PWR_OFF
//	ADC1->CR2 &= ~(1<<0);			//turn off adc
//	g_keyAdcEnable = 0;
//	ADC_DeInit(ADC1);
//	ADC1->CR2 &= ~(1<<0);
	KeyAdcDisable();
#else
//	g_keyAdcEnable = 0;
//	ADC1->CR2 &= ~(1<<0);			//turn off adc
	KeyAdcDisable();
#endif
/***************************************************
2015/08/21 set portUninit and turn on 5v and 3v3
****************************************************/
	//SleepPortUninit();
	SleepPortUninit(confRegRsv,datRegRsv);
#ifdef SLEEP_PWR_OFF
	D3V3_EN = 1;
	BOOST_EN = 0;
	OSTimeDlyHMSM(0,0,0,10);
	//OSTimeDlyHMSM(0,0,0,10);
#endif	
/**************************************************/

	//GPIOA->CRL&=0XFF0FFF00;//PA0���ó�����	  
	//GPIOA->CRL|=0X00800088; 

	//GPIOC->CRL&=0xFFF0FFFF;
	//GPIOC->CRL|=0x00030000;

	Ex_NVIC_Config(GPIO_A,0,FTIR);
	Ex_NVIC_Config(GPIO_A,1,FTIR);
#ifdef SLEEP_PWR_OFF
	Ex_NVIC_Config(GPIO_A,15,RTIR);
#endif
	//EXTIx_Init(GPIO_A,RTIR);
	//Ex_NVIC_Config(GPIO_A,5,RTIR);

	MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);
	MY_NVIC_Init(2,2,EXTI1_IRQChannel,1);
#ifdef SLEEP_PWR_OFF
	MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,1);
#endif

	SetPwrOnExti();
	//MY_NVIC_Init(2,3,EXTI15_10_IRQChannel,3);
	
	g_comStatue = COM_STATUE_SLEEP;
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	RCC_Configuration();

	while(!PILOT_IN);				//avoid unexpected flash

	//HaltSec(100);

	Ex_NVIC_Disable(0);
	Ex_NVIC_Disable(1);
	Ex_NVIC_Disable(13);
#ifdef SLEEP_PWR_OFF
	Ex_NVIC_Disable(15);
	ENABLE_FLASH_INT();
	ENABLE_SYN_INT();
#endif

	//if(KEY_POW_OFF>0)
	if((!KEY_LOCK_EX)||(!KEY_POW_ON))
	{
/***************************************************
2015/08/21 set portUninit and turn on 5v and 3v3
****************************************************/
		//SleepPortUninit();
		//SleepPortUninit(confRegRsv,datRegRsv);
#ifdef SLEEP_PWR_OFF
		D3V3_EN = 0;
		BOOST_EN = 1;
#endif	
/**************************************************/
	
		ENABLE_KEY_INT();

	/*******************************************************/
	//	port initialize
	/*******************************************************/
	//	PortInit();				//2015/08/21
	//	SleepPortInit(confRegRsv,datRegRsv);
	/*******************************************************/

	//for test
	//	BOOST_EN = 1;
	//	D3V3_EN = 0;

/**********************************************************/
#ifdef SLEEP_PWR_OFF
		//ADC1->CR2|=(1<<0);
		//g_keyAdcEnable = 1;
		//Adc_Init();
		KeyAdcEnable();
#else
		//g_keyAdcEnable = 1;
		//ADC1->CR2|=(1<<0);
		KeyAdcEnable();
#endif
/**********************************************************/

		LCD_Init();

		//SET_UPDATE_EVT(DSP_UPDATE_MODE);
		Mode();

#ifdef RF_LOWPWR_SLEEP
		CC2530_RESET = 0;
		delay_ms(400);
		CC2530_RESET = 1;
#else
/**************************************
for test enable RF module to sleep
***************************************/
		DISABLE_RF_MODULE();			//SleepPortUninit
/**************************************
end
***************************************/
		delay_ms(400);
#endif

		while(!PILOT_IN);	

		//if(KEY_POW_OFF>0)
		//if((!KEY_LOCK_EX)&&(!KEY_POW_ON))
		{
			RfMoudleSet();
		}
	}
	//OSTaskSuspend(CMDSEND_TASK_Prio);

	//BOOST_EN = 1;
	//BOOST_EN = 0;
	//D3V3_EN = 0;
	
	

	//PILOT_IN    PA1 PA0 PA5
	//EXTIx_Init(GPIO_A,RTIR);				//PA5 SPI_CLK
	//Ex_NVIC_Config(GPIO_A,0,RTIR); //�����ش���	
	//MY_NVIC_Init(2,1,EXTI0_IRQChannel,1);    //��ռ2�������ȼ�2����2
	
}
#endif


void EXTI0_IRQHandler(void)
{
#ifdef SLAVE_POWER_DOWN
	if(COM_STATUE_SLEEP==g_comStatue)
	{
		 Ex_NVIC_Disable(0);				//2015/11/04
		g_comStatue = COM_STATUE_IDLE;
	}//2015/10/31 for slave sleep
#endif
	EXTI->PR=1<<0;  //���LINE0�ϵ��жϱ�־λ  
}


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




























