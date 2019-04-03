#include "dma.h"

#ifdef DRV_DMA_TEST
//u16 DMA1_MEM_LEN;	    
/********************************************
//����������DMAͨ��x�Ĺ���(x����ͨ����)�� 
//1. ��DMA_CPARx�Ĵ�������������Ĵ����ĵ�ַ��
	�����������ݴ�������ʱ�������ַ�������ݴ����Դ��Ŀ�ꡣ
//2. ��DMA_CMARx�Ĵ������������ݴ洢���ĵ�ַ��
	�����������ݴ�������ʱ����������ݽ��������ַ������д�������ַ��
//3. ��DMA_CNDTRx�Ĵ���������Ҫ�������������
	��ÿ�����ݴ���������ֵ�ݼ���
//4. ��DMA_CCRx�Ĵ�����PL[1:0]λ������ͨ�������ȼ��� 
//5. ��DMA_CCRx�Ĵ������������ݴ���ķ���
	ѭ��ģʽ������ʹ洢��������ģʽ������ʹ洢�������ݿ�ȡ�
	����һ������жϻ�����ɲ����жϡ� 
//6. ����DMA_CCRx�Ĵ�����ENABLEλ��������ͨ����
*************************************************/

void SpiDMAInit(DMA_Channel_TypeDef* DMA_CHx)
{
	//u32 DR_Base;  //��������,��֪��Ϊʲô.��Ҫ����
	RCC->AHBENR|=1<<0;//����DMA1ʱ��
	//DR_Base=cpar;
	//DMA_CHx->CPAR=DR_Base; 	 //DMA1 �����ַ 
	//DMA_CHx->CMAR=(u32)cmar; //DMA1,�洢����ַ
	//DMA1_MEM_LEN=cndtr;      //����DMA����������
	//DMA_CHx->CNDTR=cndtr;    //DMA1,����������
	DMA_CHx->CCR=0X00000000;//��λ
	DMA_CHx->CCR|=1<<4;  //�Ӵ洢����
	DMA_CHx->CCR|=0<<5;  //��ͨģʽ
	DMA_CHx->CCR|=0<<6;  //�����ַ������ģʽ
	DMA_CHx->CCR|=1<<7;  //�洢������ģʽ
	DMA_CHx->CCR|=0<<8;  //�������ݿ��Ϊ8λ
	DMA_CHx->CCR|=0<<10; //�洢�����ݿ��8λ
	DMA_CHx->CCR|=1<<12; //�е����ȼ�
	DMA_CHx->CCR|=0<<14; //�Ǵ洢�����洢��ģʽ	
}

void SpiDMASend(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	u32 DR_Base;  //��������,��֪��Ϊʲô.��Ҫ����
	
	DR_Base=cpar;
	DMA_CHx->CPAR=DR_Base; 	 //DMA1 �����ַ 
	DMA_CHx->CMAR=cmar; //DMA1,�洢����ַ
	//DMA1_MEM_LEN=cndtr;      //����DMA����������
	DMA_CHx->CNDTR=cndtr;    //DMA1,����������

	DMA_CHx->CCR|=(1<<0);          //����DMA����

	while(!(DMA1->ISR&(1<<17)));
	DMA1->IFCR|=(1<<17);
	//while(SPI2->SR|0x80);

	DMA_CHx->CCR&=~(1<<0); 
	
} 

#endif
/*
//����һ��DMA����
void SpiDMAEnable(DMA_Channel_TypeDef*DMA_CHx, u16 tsLen)
{
	DMA_CHx->CCR&=~(1<<0);       //�ر�DMA���� 
	DMA_CHx->CNDTR=tsLen; 	//DMA1,���������� 
	DMA_CHx->CCR|=1<<0;          //����DMA����
}	  
*/

 

























