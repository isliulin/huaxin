#include "dma.h"

#ifdef DRV_DMA_TEST
//u16 DMA1_MEM_LEN;	    
/********************************************
//下面是配置DMA通道x的过程(x代表通道号)： 
//1. 在DMA_CPARx寄存器中设置外设寄存器的地址。
	发生外设数据传输请求时，这个地址将是数据传输的源或目标。
//2. 在DMA_CMARx寄存器中设置数据存储器的地址。
	发生外设数据传输请求时，传输的数据将从这个地址读出或写入这个地址。
//3. 在DMA_CNDTRx寄存器中设置要传输的数据量。
	在每个数据传输后，这个数值递减。
//4. 在DMA_CCRx寄存器的PL[1:0]位中设置通道的优先级。 
//5. 在DMA_CCRx寄存器中设置数据传输的方向、
	循环模式、外设和存储器的增量模式、外设和存储器的数据宽度、
	传输一半产生中断或传输完成产生中断。 
//6. 设置DMA_CCRx寄存器的ENABLE位，启动该通道。
*************************************************/

void SpiDMAInit(DMA_Channel_TypeDef* DMA_CHx)
{
	//u32 DR_Base;  //做缓冲用,不知道为什么.非要不可
	RCC->AHBENR|=1<<0;//开启DMA1时钟
	//DR_Base=cpar;
	//DMA_CHx->CPAR=DR_Base; 	 //DMA1 外设地址 
	//DMA_CHx->CMAR=(u32)cmar; //DMA1,存储器地址
	//DMA1_MEM_LEN=cndtr;      //保存DMA传输数据量
	//DMA_CHx->CNDTR=cndtr;    //DMA1,传输数据量
	DMA_CHx->CCR=0X00000000;//复位
	DMA_CHx->CCR|=1<<4;  //从存储器读
	DMA_CHx->CCR|=0<<5;  //普通模式
	DMA_CHx->CCR|=0<<6;  //外设地址非增量模式
	DMA_CHx->CCR|=1<<7;  //存储器增量模式
	DMA_CHx->CCR|=0<<8;  //外设数据宽度为8位
	DMA_CHx->CCR|=0<<10; //存储器数据宽度8位
	DMA_CHx->CCR|=1<<12; //中等优先级
	DMA_CHx->CCR|=0<<14; //非存储器到存储器模式	
}

void SpiDMASend(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	u32 DR_Base;  //做缓冲用,不知道为什么.非要不可
	
	DR_Base=cpar;
	DMA_CHx->CPAR=DR_Base; 	 //DMA1 外设地址 
	DMA_CHx->CMAR=cmar; //DMA1,存储器地址
	//DMA1_MEM_LEN=cndtr;      //保存DMA传输数据量
	DMA_CHx->CNDTR=cndtr;    //DMA1,传输数据量

	DMA_CHx->CCR|=(1<<0);          //开启DMA传输

	while(!(DMA1->ISR&(1<<17)));
	DMA1->IFCR|=(1<<17);
	//while(SPI2->SR|0x80);

	DMA_CHx->CCR&=~(1<<0); 
	
} 

#endif
/*
//开启一次DMA传输
void SpiDMAEnable(DMA_Channel_TypeDef*DMA_CHx, u16 tsLen)
{
	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
	DMA_CHx->CNDTR=tsLen; 	//DMA1,传输数据量 
	DMA_CHx->CCR|=1<<0;          //开启DMA传输
}	  
*/

 

























