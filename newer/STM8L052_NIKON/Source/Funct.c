#include "includes.h"
#include "stdbool.h"    	//BOOL 类型头文件
#include "intrinsics.h" 	//总中断头文件   

_SysInfo   SysInfo;
_Beep_Flag BeepFlag;
COMM_STATE comm_state;

extern uint16_t uhADCxConvertedValue[5];

_Sys_Mode_Typedef               Sys_Mode_Typedef         = Sys_Mode_TTL;
_Key_SET_Function_Typedef       Key_SET_Function_Typedef = Key_SET_None;
_Flash_Output_Typedef           Flash_Output_Typedef     = Flash_Output_128;
_Slave_Output_Typedef           Slave_Output_Typedef     = Slave_Output_128;
_Multi_Output_Typedef           Multi_Output_Typedef     = Multi_Output_128;
_TTL_Ev_TypeDef                 TTL_Ev_TypeDef           = TTL_Ev_0;
_M_Zoom_TypeDef                 M_Zoom_TypeDef           = M_Zoom_AUTO;
_Multi_Freq_TypeDef             Multi_Freq_TypeDef       = Multi_Freq_1;
_Multi_Times_TypeDef            Multi_Times_TypeDef      = Multi_Times_2;
_Beep_Mode_TypeDef              Beep_Mode_TypeDef        = Beep_Mode_None;
_Lamp_Temp_TypeDef              Lamp_Temp_TypeDef        = Lamp_Temp_level_0;

_M_Zoom_TypeDef    T_Zoom_Group[10] ={
    M_Zoom_AUTO,
    M_Zoom_24mm,    
    M_Zoom_28mm,
    M_Zoom_35mm,
    M_Zoom_50mm,
    M_Zoom_70mm,
    M_Zoom_105mm,
    M_Zoom_135mm,
    M_Zoom_180mm,
    M_Zoom_200mm
};

//_Multi_Freq_TypeDef   T_Multi_Freq[19] ={
//    Multi_Freq_1,       Multi_Freq_2,
//    Multi_Freq_3,       Multi_Freq_4,
//    Multi_Freq_5,       Multi_Freq_6,
//    Multi_Freq_7,       Multi_Freq_8,
//    Multi_Freq_9,       Multi_Freq_10,
//    Multi_Freq_20,      Multi_Freq_30,
//    Multi_Freq_40,      Multi_Freq_50,
//    Multi_Freq_60,      Multi_Freq_70,
//    Multi_Freq_80,      Multi_Freq_90,
//    Multi_Freq_100,
//};

_Multi_Freq_TypeDef   T_Multi_Freq[41] ={
    Multi_Freq_1  ,    Multi_Freq_2  ,
    Multi_Freq_3  ,    Multi_Freq_4  ,
    Multi_Freq_5  ,    Multi_Freq_6  ,
    Multi_Freq_7  ,    Multi_Freq_8  ,
    Multi_Freq_9  ,    Multi_Freq_10 ,
    Multi_Freq_11 ,    Multi_Freq_12 ,
    Multi_Freq_13 ,    Multi_Freq_14 ,
    Multi_Freq_15 ,    Multi_Freq_16 ,
    Multi_Freq_17 ,    Multi_Freq_18 ,
    Multi_Freq_19 ,    Multi_Freq_20 ,
    Multi_Freq_25 ,    Multi_Freq_30 ,
    Multi_Freq_35 ,    Multi_Freq_40 ,
    Multi_Freq_45 ,    Multi_Freq_50 ,
    Multi_Freq_60 ,    Multi_Freq_70 ,
    Multi_Freq_80 ,    Multi_Freq_90 ,
    Multi_Freq_100,    Multi_Freq_110,
    Multi_Freq_120,    Multi_Freq_130,
    Multi_Freq_140,    Multi_Freq_150,
    Multi_Freq_160,    Multi_Freq_170,
    Multi_Freq_180,    Multi_Freq_190,
    Multi_Freq_199,
};

//_Multi_Times_TypeDef  T_Multi_Times[23] ={
//    Multi_Times_0,      Multi_Times_2,  
//    Multi_Times_3,      Multi_Times_4,  
//    Multi_Times_5,      Multi_Times_6,  
//    Multi_Times_7,      Multi_Times_8,  
//    Multi_Times_9,      Multi_Times_10, 
//    Multi_Times_15,     Multi_Times_20, 
//    Multi_Times_25,     Multi_Times_30, 
//    Multi_Times_35,     Multi_Times_40, 
//    Multi_Times_45,     Multi_Times_50, 
//    Multi_Times_60,     Multi_Times_70, 
//    Multi_Times_80,     Multi_Times_90, 
//    Multi_Times_99,    
//};

_Multi_Times_TypeDef  T_Multi_Times[31] ={
    Multi_Times_0 ,    Multi_Times_1 ,
    Multi_Times_2 ,    Multi_Times_3 ,
    Multi_Times_4 ,    Multi_Times_5 ,
    Multi_Times_6 ,    Multi_Times_7 ,
    Multi_Times_8 ,    Multi_Times_9 ,
    Multi_Times_10,    Multi_Times_11,
    Multi_Times_12,    Multi_Times_13,
    Multi_Times_14,    Multi_Times_15,
    Multi_Times_16,    Multi_Times_17,
    Multi_Times_18,    Multi_Times_19,
    Multi_Times_20,    Multi_Times_25,
    Multi_Times_30,    Multi_Times_35,
    Multi_Times_40,    Multi_Times_45,
    Multi_Times_50,    Multi_Times_60,
    Multi_Times_70,    Multi_Times_80,
    Multi_Times_90,    
};

volatile __eeprom __no_init char  EEPROM_DATA1;
volatile __eeprom __no_init char  EEPROM_DATA2;
volatile __eeprom __no_init char  EEPROM_DATA3;
volatile __eeprom __no_init char  EEPROM_DATA4;
volatile __eeprom __no_init char  EEPROM_DATA5;
volatile __eeprom __no_init char  EEPROM_DATA6;
volatile __eeprom __no_init char  EEPROM_DATA7;
volatile __eeprom __no_init char  EEPROM_DATA8;
volatile __eeprom __no_init char  EEPROM_DATA9;


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

/**************************************************************************************
* FunctionName   : __eeprom_write_8
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void __eeprom_write_8(unsigned short addr_eep, char data)
{
    FLASH_WaitForLastOperation(FLASH_MemType_Data);  
    FLASH_Unlock(FLASH_MemType_Data);
    
    FLASH_ProgramByte(addr_eep, data); 
    
    FLASH_WaitForLastOperation(FLASH_MemType_Data); 
    FLASH_Lock(FLASH_MemType_Data);  
}
///**************************************************************************************
//* FunctionName   : __eeprom_write_16
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
//void __eeprom_write_16(unsigned short addr_eep,unsigned short data)
//{
//    FLASH_WaitForLastOperation(FLASH_MemType_Data);  
//    FLASH_Unlock(FLASH_MemType_Data);
//    
//    FLASH_ProgramByte(addr_eep,   data/256); 
//    FLASH_WaitForLastOperation(FLASH_MemType_Data); 
//    
//    FLASH_ProgramByte(addr_eep+1, data%256);
//    FLASH_WaitForLastOperation(FLASH_MemType_Data); 
//    
//    FLASH_Lock(FLASH_MemType_Data);  
//}
///**************************************************************************************
//* FunctionName   : __eeprom_write_32
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
//void __eeprom_write_32(unsigned short addr_eep,unsigned long data)
//{
//    FLASH_WaitForLastOperation(FLASH_MemType_Data);  
//    FLASH_Unlock(FLASH_MemType_Data);
//    
//    FLASH_ProgramByte(addr_eep,   (unsigned char)(data>>24)); 
//    FLASH_WaitForLastOperation(FLASH_MemType_Data); 
//    
//    FLASH_ProgramByte(addr_eep+1, (unsigned char)(data>>16));
//    FLASH_WaitForLastOperation(FLASH_MemType_Data); 
//    
//    FLASH_ProgramByte(addr_eep+2,   (unsigned char)(data>>8)); 
//    FLASH_WaitForLastOperation(FLASH_MemType_Data); 
//    
//    FLASH_ProgramByte(addr_eep+3, (unsigned char)(data>>0));
//    FLASH_WaitForLastOperation(FLASH_MemType_Data);
//    
//    FLASH_Lock(FLASH_MemType_Data);  
//}
///**************************************************************************************
//* FunctionName   : __eeprom_write_many
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
//void __eeprom_write_many(unsigned short addr_eep,unsigned short size,unsigned short dummy,unsigned short addr_ram)
//{    
//    FLASH_WaitForLastOperation(FLASH_MemType_Data);  
//    FLASH_Unlock(FLASH_MemType_Data);
//    
//    for(unsigned short i=0;i<size;i++)
//    {
//        FLASH_ProgramByte(addr_eep+i,   *((unsigned char *)(addr_ram)+i)); 
//        FLASH_WaitForLastOperation(FLASH_MemType_Data); 
//    }    
//    FLASH_Lock(FLASH_MemType_Data);      
//}
///**************************************************************************************
//* FunctionName   : delay_time
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void delay_time(void)
{
    uint16_t delay_time;
    delay_time = 20000;
    while(delay_time--);
}
///**************************************************************************************
//* FunctionName   : Save_EepromData
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void Save_EepromData(void)
{
    disableInterrupts();
    EEPROM_DATA1 = (uint8_t)Sys_Mode_Typedef;
    delay_time();
    EEPROM_DATA2 = (int8_t)Flash_Output_Typedef;
    delay_time();
    EEPROM_DATA3 = (int8_t)Slave_Output_Typedef;
    delay_time();
    EEPROM_DATA4 = (int8_t)Multi_Output_Typedef;
    delay_time();
    EEPROM_DATA5 = (int8_t)TTL_Ev_TypeDef;
    delay_time();
    EEPROM_DATA6 = (uint8_t)M_Zoom_TypeDef;
    delay_time();
    EEPROM_DATA7 = (uint8_t)Multi_Freq_TypeDef;
    delay_time();
    EEPROM_DATA8 = (uint8_t)Multi_Times_TypeDef;
    delay_time();
    EEPROM_DATA9 = (uint8_t)SysInfo.Beep_En_flag;
    delay_time();
    enableInterrupts();
}
///**************************************************************************************
//* FunctionName   : Read_EepromData
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void Read_EepromData(void)
{
    Sys_Mode_Typedef     = (_Sys_Mode_Typedef)     EEPROM_DATA1;
    Flash_Output_Typedef = (_Flash_Output_Typedef) EEPROM_DATA2;
    Slave_Output_Typedef = (_Slave_Output_Typedef) EEPROM_DATA3;
    Multi_Output_Typedef = (_Multi_Output_Typedef) EEPROM_DATA4;
    TTL_Ev_TypeDef       = (_TTL_Ev_TypeDef)       EEPROM_DATA5;
    M_Zoom_TypeDef       = (_M_Zoom_TypeDef)       EEPROM_DATA6;
    Multi_Freq_TypeDef	 = (_Multi_Freq_TypeDef)   EEPROM_DATA7;
    Multi_Times_TypeDef	 = (_Multi_Times_TypeDef)  EEPROM_DATA8;
    SysInfo.Beep_En_flag =                         EEPROM_DATA9;
}


uint8_t Slave_S2_Count=0;
uint8_t Slave_Flag;
uint8_t Slave_time_count;
void GPIO_Pin_1_Callback(void)
{
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_S1:
        {
            uint32_t delay_time;
            GPIOC->CR2 &= (uint8_t)(~(GPIO_Pin_1));
            Formalfrash(Flash_Time_Group[getVolLevel()][(Flash_Output_Typedef+9)+6]);
            delay_time = 0x1000;
            while(delay_time--);
            GPIOC->CR2 |= GPIO_Pin_1;            
        }break;
      case Sys_Mode_S2:
        {
            GPIOC->CR2 &= (uint8_t)(~(GPIO_Pin_1));
            Slave_S2_Count++;
            if(Slave_Flag==0)
            {
                Slave_Flag = 1;
            }
            if(Slave_S2_Count == 2)
            {
                Formalfrash(Flash_Time_Group[getVolLevel()][(Flash_Output_Typedef+9)+6]);
            }
        }break;
    }
}
void Slave_S2_TimeOut(void)
{
    if(Slave_Flag)
    {
        Slave_time_count++;
        if(Slave_time_count == 30)
        {
            GPIOC->CR2 |= GPIO_Pin_1;
        }
        else if(Slave_time_count == 120)
        {
            Slave_Flag = 0;
            Slave_time_count = 0;
            Slave_S2_Count = 0;
            GPIOC->CR2 |= GPIO_Pin_1;
        }
    }
}
///**************************************************************************************
//* FunctionName   : Beep_Mode_Handle
//* Description    : 
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
//void Beep_Mode_Handle(void)
//{
//    static uint8_t Beep_Num;
//    
//    if(SysInfo.Beep_En_flag)
//    {       
//        if(Beep_Mode_TypeDef != Beep_Mode_None)
//        {
//            Beep_Num++;  
//        }
//        else
//        {
//            Beep_Num = 0;
//        }
//        
//        if(BeepFlag.Beep_Key_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_1;
//            switch(Beep_Num)
//            {
//              case 1 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 6 : Beep_Num = 0;BeepFlag.Beep_Key_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
//            }
//        }
//        else if(BeepFlag.Beep_CapFull_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_2;
//            switch(Beep_Num)
//            {
//              case 1 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 16 : Beep_Num = 0;BeepFlag.Beep_CapFull_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
//            }
//        }
//        else if(BeepFlag.Beep_TrueFlash_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_3;
//            switch(Beep_Num)
//            {
//              case 1 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 5 : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 8 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 12: Beep_Num = 0;BeepFlag.Beep_TrueFlash_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
//            }
//        }
//        else if(BeepFlag.Beep_FailFlash_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_4;
//            switch(Beep_Num)
//            {
//              case 1  : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 11 : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 21 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 31 : Beep_Num = 0;BeepFlag.Beep_FailFlash_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
//            }
//        }
//        else if(BeepFlag.Beep_PowerOff_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_5;
//            switch(Beep_Num)
//            {
//              case 1  : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 6  : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 11 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 16 : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 21 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 26 : Beep_Num = 0;BeepFlag.Beep_PowerOff_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
//            }
//        }
//        else if(BeepFlag.Beep_BattLow_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_6;
//            switch(Beep_Num)
//            {
//              case 1  : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 6  : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 11 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 16 : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 21 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 26 : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 31 : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 36 : Beep_Num = 0;BeepFlag.Beep_BattLow_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
//            }
//        }
//        else if(BeepFlag.Beep_LampTemp_Flag)
//        {
//            Beep_Mode_TypeDef = Beep_Mode_7;
//            BeepFlag.Beep_LampTemp_Cnt++;
//            switch(Beep_Num)
//            {
//              case 1  : TIM3_CtrlPWMOutputs(ENABLE);break;
//              case 6  : TIM3_CtrlPWMOutputs(DISABLE);break;
//              case 11 : Beep_Num = 0;break; 
//            }
//            if(BeepFlag.Beep_LampTemp_Cnt >= 1500)
//            {
//                BeepFlag.Beep_LampTemp_Cnt = 0;
//                BeepFlag.Beep_LampTemp_Flag = 0;
//            }
//        }
//        else
//        {
//            BeepFlag.Beep_Key_Flag       = 0;
//            BeepFlag.Beep_CapFull_Flag   = 0;
//            BeepFlag.Beep_TrueFlash_Flag = 0;
//            BeepFlag.Beep_FailFlash_Flag = 0;
//            BeepFlag.Beep_PowerOff_Flag  = 0;
//            BeepFlag.Beep_BattLow_Flag   = 0;
//            BeepFlag.Beep_LampTemp_Flag  = 0;
//            Beep_Mode_TypeDef = Beep_Mode_None;
//            TIM3_CtrlPWMOutputs(DISABLE);
//        }
//    }
//    else
//    {
//        BeepFlag.Beep_Key_Flag       = 0;
//        BeepFlag.Beep_CapFull_Flag   = 0;
//        BeepFlag.Beep_TrueFlash_Flag = 0;
//        BeepFlag.Beep_FailFlash_Flag = 0;
//        BeepFlag.Beep_PowerOff_Flag  = 0;
//        BeepFlag.Beep_BattLow_Flag   = 0;
//        BeepFlag.Beep_LampTemp_Flag  = 0;
//        TIM3_CtrlPWMOutputs(DISABLE);
//    }
//    
//    if(Beep_Num>36)
//    {
//        Beep_Num = 0;
//        Beep_Mode_TypeDef = Beep_Mode_None;
//        TIM3_CtrlPWMOutputs(DISABLE);
//    }
//}

void Beep_Mode_Handle(void)
{
    static uint8_t Beep_Num = 0;
    
    if(SysInfo.Beep_En_flag)
    {   
        if(!BeepFlag.Beep_Start)
        {
            if(BeepFlag.Beep_Key_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_1;
            }
            else if(BeepFlag.Beep_CapFull_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_2;
            }
            else if(BeepFlag.Beep_TrueFlash_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_3;
            }
            else if(BeepFlag.Beep_FailFlash_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_4;
            }
            else if(BeepFlag.Beep_PowerOff_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_5;
            }
            else if(BeepFlag.Beep_BattLow_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_6;
            }
            else if(BeepFlag.Beep_LampTemp_Flag)
            {
                Beep_Mode_TypeDef = Beep_Mode_7;
            }
            else
            {
                Beep_Mode_TypeDef = Beep_Mode_None;
                TIM3_CtrlPWMOutputs(DISABLE);
            }
        }
        
        if(Beep_Mode_TypeDef != Beep_Mode_None)
        {
            Beep_Num++;            

            switch(Beep_Mode_TypeDef)
            {
              case Beep_Mode_1:
                {
                    switch(Beep_Num)
                    {
                      case 1 : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 6 : BeepFlag.Beep_Start = 0;Beep_Num = 0;BeepFlag.Beep_Key_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
                    }
                }break;
              case Beep_Mode_2:
                {
                    switch(Beep_Num)
                    {
                      case 1 : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 16: BeepFlag.Beep_Start = 0;Beep_Num = 0;BeepFlag.Beep_CapFull_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
                    }
                }break;
              case Beep_Mode_3:
                {
                    switch(Beep_Num)
                    {
                      case 1 : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 5 : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 8 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 12: BeepFlag.Beep_Start = 0;Beep_Num = 0;BeepFlag.Beep_TrueFlash_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
                    }
                }break;
              case Beep_Mode_4:
                {
                    switch(Beep_Num)
                    {
                      case 1  : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 11 : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 21 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 31 : BeepFlag.Beep_Start = 0;Beep_Num = 0;BeepFlag.Beep_FailFlash_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
                    } 
                }break;
              case Beep_Mode_5:
                {
                    switch(Beep_Num)
                    {
                      case 1  : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 6  : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 11 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 16 : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 21 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 26 : BeepFlag.Beep_Start = 0;Beep_Num = 0;BeepFlag.Beep_PowerOff_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
                    }
                }break;
              case Beep_Mode_6:
                {
                    switch(Beep_Num)
                    {
                      case 1  : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 6  : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 11 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 16 : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 21 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 26 : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 31 : TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 36 : BeepFlag.Beep_Start = 0;Beep_Num = 0;BeepFlag.Beep_BattLow_Flag = 0;TIM3_CtrlPWMOutputs(DISABLE);break; 
                    }
                }break;
              case Beep_Mode_7:
                {
                    BeepFlag.Beep_LampTemp_Cnt++;
                    switch(Beep_Num)
                    {
                      case 1  : BeepFlag.Beep_Start = 1;TIM3_CtrlPWMOutputs(ENABLE);break;
                      case 6  : TIM3_CtrlPWMOutputs(DISABLE);break;
                      case 11 : Beep_Num = 0;break; 
                    }
                    if(BeepFlag.Beep_LampTemp_Cnt >= 1500)
                    {
                        BeepFlag.Beep_LampTemp_Cnt = 0;
                        BeepFlag.Beep_LampTemp_Flag = 0;
                        BeepFlag.Beep_Start = 0;
                    }
                }break;
            }             
        }
        else
        {
            Beep_Num = 0;
        }        
    }
    else
    {
        BeepFlag.Beep_Key_Flag       = 0;
        BeepFlag.Beep_CapFull_Flag   = 0;
        BeepFlag.Beep_TrueFlash_Flag = 0;
        BeepFlag.Beep_FailFlash_Flag = 0;
        BeepFlag.Beep_PowerOff_Flag  = 0;
        BeepFlag.Beep_BattLow_Flag   = 0;
        BeepFlag.Beep_LampTemp_Flag  = 0;
        TIM3_CtrlPWMOutputs(DISABLE);
    }
}

void Nikon_To_Camera(void)
{
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:mainPara.workMode = WORKINGMODE_TTL;  break;
      case Sys_Mode_M:  mainPara.workMode = WORKINGMODE_MANU; break;
      case Sys_Mode_RPT:mainPara.workMode = WORKINGMODE_MULTI;break;
      default:mainPara.workMode = WORKINGMODE_MANU; break;
    }
}