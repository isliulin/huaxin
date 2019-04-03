#include "sys.h"
#include"global.h"

#pragma optimize=none
void Delayus(void)   
{    
    asm("nop"); //一个asm("nop")函数经过示波器测试代表100ns   
    asm("nop");   
    asm("nop");   
    asm("nop");    
}   

void delay_us(u16 us)
{
    while (us--)
    {
        Delayus();
    }
}
  
//---- 毫秒级延时程序-----------------------   
void delay_ms(u32 time)   
{   
    u32 i;   
    while(time--)     
    for(i=900;i>0;i--)   
    Delayus();    
}
