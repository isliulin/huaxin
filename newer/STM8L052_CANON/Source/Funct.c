#include "includes.h"
#include "stdbool.h"    	//BOOL 类型头文件
#include "intrinsics.h" 	//总中断头文件   

_SysInfo   SysInfo;
_Beep_Flag BeepFlag;

extern uint16_t uhADCxConvertedValue[5];

_Sys_Mode_Typedef               Sys_Mode_Typedef         = Sys_Mode_TTL;
_Key_SET_Function_Typedef       Key_SET_Function_Typedef = Key_SET_None;
_Flash_Output_Typedef           Flash_Output_Typedef     = Flash_Output_128;
//_Slave_Output_Typedef           Slave_Output_Typedef     = Slave_Output_128;
_Multi_Output_Typedef           Multi_Output_Typedef     = Multi_Output_128;
_TTL_Ev_TypeDef                 TTL_Ev_TypeDef           = TTL_Ev_0;
_M_Zoom_TypeDef                 M_Zoom_TypeDef           = M_Zoom_AUTO;
_Multi_Freq_TypeDef             Multi_Freq_TypeDef       = Multi_Freq_1;
_Multi_Times_TypeDef            Multi_Times_TypeDef      = Multi_Times_2;
_Beep_Mode_TypeDef              Beep_Mode_TypeDef        = Beep_Mode_None;
_Lamp_Temp_TypeDef              Lamp_Temp_TypeDef        = Lamp_Temp_level_0;

_M_Zoom_TypeDef    T_Zoom_Group[11] ={
    M_Zoom_AUTO,
    M_Zoom_24mm,    
    M_Zoom_28mm,
    M_Zoom_35mm,
    M_Zoom_50mm,
    M_Zoom_70mm,
    M_Zoom_80mm,
    M_Zoom_105mm,
    M_Zoom_135mm,
    M_Zoom_180mm,
    M_Zoom_200mm
};

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
    EEPROM_DATA1 = (uint8_t)Sys_Mode_Typedef;           delay_time();
    EEPROM_DATA2 = (int8_t)Flash_Output_Typedef;        delay_time();
//    EEPROM_DATA3 = (int8_t)Slave_Output_Typedef;    delay_time();
    EEPROM_DATA4 = (int8_t)Multi_Output_Typedef;        delay_time();
    EEPROM_DATA5 = (int8_t)TTL_Ev_TypeDef;              delay_time();
    EEPROM_DATA6 = (uint8_t)M_Zoom_TypeDef;             delay_time();
    EEPROM_DATA7 = (uint8_t)Multi_Freq_TypeDef;         delay_time();
    EEPROM_DATA8 = (uint8_t)Multi_Times_TypeDef;        delay_time();
    EEPROM_DATA9 = (uint8_t)SysInfo.Beep_En_flag;       delay_time();
    //enableInterrupts();
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
//    Slave_Output_Typedef = (_Slave_Output_Typedef) EEPROM_DATA3;
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
            //Formalfrash(Flash_Time_Group[(Flash_Output_Typedef+9)+6]);
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


void Canon_Link_Camera(void)      //灯数据同步到相机
{
    if(Sys_Mode_Typedef < Sys_Mode_S1)
    {
        mainPara.workMode = Sys_Mode_Typedef-1;
    }
    
    switch(Multi_Output_Typedef)
    {
      case Multi_Output_128: *multiSet.expose = 0 ; break;
      case Multi_Output_64 : *multiSet.expose = 16; break;
      case Multi_Output_32 : *multiSet.expose = 32; break;
      case Multi_Output_16 : *multiSet.expose = 48; break;
      case Multi_Output_8  : *multiSet.expose = 64; break;	  
      case Multi_Output_4  : *multiSet.expose = 80; break;	
    }
    *multiSet.frequence = Multi_Freq_TypeDef;
    *multiSet.eTimes    = Multi_Times_TypeDef;
    
    switch(TTL_Ev_TypeDef)
    {
      case TTL_Ev_P3s0: mstPara->cmp1 = -3; mstPara->cmp2 = 0;break; 
      case TTL_Ev_P2s7: mstPara->cmp1 = -3; mstPara->cmp2 = 1;break; 
      case TTL_Ev_P2s3: mstPara->cmp1 = -3; mstPara->cmp2 = 2;break; 
      case TTL_Ev_P2s0: mstPara->cmp1 = -2; mstPara->cmp2 = 0;break; 
      case TTL_Ev_P1s7: mstPara->cmp1 = -2; mstPara->cmp2 = 1;break; 
      case TTL_Ev_P1s3: mstPara->cmp1 = -2; mstPara->cmp2 = 2;break; 
      case TTL_Ev_P1s0: mstPara->cmp1 = -1; mstPara->cmp2 = 0;break; 
      case TTL_Ev_P0s7: mstPara->cmp1 = -1; mstPara->cmp2 = 1;break; 
      case TTL_Ev_P0s3: mstPara->cmp1 = -1; mstPara->cmp2 = 2;break; 
      case TTL_Ev_0   : mstPara->cmp1 = 0;  mstPara->cmp2 = 0;break; 
      case TTL_Ev_N0s3: mstPara->cmp1 = 0;  mstPara->cmp2 = 1;break; 
      case TTL_Ev_N0s7: mstPara->cmp1 = 0;  mstPara->cmp2 = 2;break; 
      case TTL_Ev_N1s0: mstPara->cmp1 = 1;  mstPara->cmp2 = 0;break; 
      case TTL_Ev_N1s3: mstPara->cmp1 = 1;  mstPara->cmp2 = 1;break; 
      case TTL_Ev_N1s7: mstPara->cmp1 = 1;  mstPara->cmp2 = 2;break; 
      case TTL_Ev_N2s0: mstPara->cmp1 = 2;  mstPara->cmp2 = 0;break; 
      case TTL_Ev_N2s3: mstPara->cmp1 = 2;  mstPara->cmp2 = 1;break; 
      case TTL_Ev_N2s7: mstPara->cmp1 = 2;  mstPara->cmp2 = 2;break; 
      case TTL_Ev_N3s0: mstPara->cmp1 = 3;  mstPara->cmp2 = 0;break;       
    }
    
    switch(Flash_Output_Typedef)
    {
      case Flash_Output_128  : mstPara->output1 = 0;mstPara->output2 = 0;break;
      case Flash_Output_128s3: mstPara->output1 = 0;mstPara->output2 = 1;break;
      case Flash_Output_128s7: mstPara->output1 = 0;mstPara->output2 = 2;break;
      case Flash_Output_64   : mstPara->output1 = 1;mstPara->output2 = 0;break;
      case Flash_Output_64s3 : mstPara->output1 = 1;mstPara->output2 = 1;break;
      case Flash_Output_64s7 : mstPara->output1 = 1;mstPara->output2 = 2;break;
      case Flash_Output_32   : mstPara->output1 = 2;mstPara->output2 = 0;break;
      case Flash_Output_32s3 : mstPara->output1 = 2;mstPara->output2 = 1;break;
      case Flash_Output_32s7 : mstPara->output1 = 2;mstPara->output2 = 2;break;
      case Flash_Output_16   : mstPara->output1 = 3;mstPara->output2 = 0;break;
      case Flash_Output_16s3 : mstPara->output1 = 3;mstPara->output2 = 1;break;
      case Flash_Output_16s7 : mstPara->output1 = 3;mstPara->output2 = 2;break;
      case Flash_Output_8    : mstPara->output1 = 4;mstPara->output2 = 0;break;
      case Flash_Output_8s3  : mstPara->output1 = 4;mstPara->output2 = 1;break;
      case Flash_Output_8s7  : mstPara->output1 = 4;mstPara->output2 = 2;break;
      case Flash_Output_4    : mstPara->output1 = 5;mstPara->output2 = 0;break;
      case Flash_Output_4s3  : mstPara->output1 = 5;mstPara->output2 = 1;break;
      case Flash_Output_4s7  : mstPara->output1 = 5;mstPara->output2 = 2;break;
      case Flash_Output_2    : mstPara->output1 = 6;mstPara->output2 = 0;break;
      case Flash_Output_2s3  : mstPara->output1 = 6;mstPara->output2 = 1;break;
      case Flash_Output_2s7  : mstPara->output1 = 6;mstPara->output2 = 2;break;
      case Flash_Output_1    : mstPara->output1 = 7;mstPara->output2 = 0;break;  
    }
    
    
    switch(M_Zoom_TypeDef)  
    {
      case  M_Zoom_AUTO :*motoSet.mode = MOTO_MODE_AUTO;motoSet.destRsv = 0;menuPara.motoCod = 0x01 ;break;
      case  M_Zoom_24mm :*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 24;menuPara.motoCod = 0x11 ;motoSet.focIndex=1;break;
      case  M_Zoom_28mm :*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 28;menuPara.motoCod = 0x12 ;motoSet.focIndex=2;break;
      case  M_Zoom_35mm :*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 35;menuPara.motoCod = 0x13 ;motoSet.focIndex=3;break;
      case  M_Zoom_50mm :*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 50;menuPara.motoCod = 0x14 ;motoSet.focIndex=4;break;
      case  M_Zoom_70mm :*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 70;menuPara.motoCod = 0x15 ;motoSet.focIndex=5;break;
      case  M_Zoom_80mm :*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 80;menuPara.motoCod = 0x16 ;motoSet.focIndex=6;break;
      case  M_Zoom_105mm:*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 105;menuPara.motoCod = 0x17; motoSet.focIndex=7;break;
      case  M_Zoom_135mm:*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 135;menuPara.motoCod = 0x17; motoSet.focIndex=8;break;
      case  M_Zoom_180mm:*motoSet.mode = MOTO_MODE_MANU;motoSet.destRsv = 180;menuPara.motoCod = 0x17; motoSet.focIndex=9;break;
      default:menuPara.motoCod = 0x17;break;
    }      
    
//    switch(M_Zoom_TypeDef)  
//    {
//      case  M_Zoom_AUTO : menuPara.motoCod = 0x01;break;
//      case  M_Zoom_24mm : menuPara.motoCod = 0x11;break; 
//      case  M_Zoom_28mm : menuPara.motoCod = 0x12;break; 
//      case  M_Zoom_35mm : menuPara.motoCod = 0x13;break; 
//      case  M_Zoom_50mm : menuPara.motoCod = 0x14;break; 
//      case  M_Zoom_70mm : menuPara.motoCod = 0x15;break; 
//      case  M_Zoom_80mm : menuPara.motoCod = 0x16;break; 
//      case  M_Zoom_105mm: menuPara.motoCod = 0x17;break; 
//      case  M_Zoom_135mm: menuPara.motoCod = 0x17;break; 
//      case  M_Zoom_180mm: menuPara.motoCod = 0x17;break;//广角扩散板是0x70
//      default:menuPara.motoCod = 0x17;break;

}
void Camera_Link_Canon(void)    //相机数据同步到灯
{
    if(Sys_Mode_Typedef < Sys_Mode_S1)
    {
        Sys_Mode_Typedef = (_Sys_Mode_Typedef)(mainPara.workMode+1);
    }
    switch(*multiSet.expose)
    {
      case 0 : Multi_Output_Typedef = Multi_Output_128;break;
      case 16: Multi_Output_Typedef = Multi_Output_64; break;
      case 32: Multi_Output_Typedef = Multi_Output_32; break;
      case 48: Multi_Output_Typedef = Multi_Output_16; break;
      case 64: Multi_Output_Typedef = Multi_Output_8;  break;	  
      case 80: Multi_Output_Typedef = Multi_Output_4;  break;	
    }
    
    Multi_Freq_TypeDef   = (_Multi_Freq_TypeDef)(*multiSet.frequence);
    Multi_Times_TypeDef  = (_Multi_Times_TypeDef)(*multiSet.eTimes);
    
    switch(mstPara->output1)    //同步档位
    {
      case 0:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_128  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_128s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_128s7; break;
            }
        }break;
      case 1:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_64  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_64s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_64s7; break;
            }
        }break;
      case 2:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_32  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_32s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_32s7; break;
            }
        }break;
      case 3:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_16  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_16s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_16s7; break;
            }
        }break;
      case 4:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_8  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_8s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_8s7; break;
            }
        }break;
      case 5:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_4  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_4s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_4s7; break;
            }
        }break;
      case 6:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_2  ; break;
              case 1: Flash_Output_Typedef = Flash_Output_2s3; break;
              case 2: Flash_Output_Typedef = Flash_Output_2s7; break;
            }
        }break;
      case 7:
        {
            switch(mstPara->output2)
            {
              case 0: Flash_Output_Typedef = Flash_Output_1  ; break;
            }
        }break;   
    }
    
    if(*motoSet.mode == MOTO_MODE_MANU)
    {
        switch(motoSet.destRsv)
        {
          case 24: M_Zoom_TypeDef = M_Zoom_24mm;break;
          case 28: M_Zoom_TypeDef = M_Zoom_28mm;break;
          case 35: M_Zoom_TypeDef = M_Zoom_35mm;break;
          case 50: M_Zoom_TypeDef = M_Zoom_50mm;break;
          case 70: M_Zoom_TypeDef = M_Zoom_70mm;break;
          case 80: M_Zoom_TypeDef = M_Zoom_80mm;break;
          case 105: M_Zoom_TypeDef = M_Zoom_105mm;break; 
          case 135: M_Zoom_TypeDef = M_Zoom_135mm;break;   
        }
    }
    else
    {
        M_Zoom_TypeDef = M_Zoom_AUTO;
    }
    
//    if(menuPara.motoCod & 0xf0)
//    {
//        switch(menuPara.motoCod & 0x0f)
//        {
//          case 0x01: M_Zoom_TypeDef = M_Zoom_24mm;break;
//          case 0x02: M_Zoom_TypeDef = M_Zoom_28mm;break;
//          case 0x03: M_Zoom_TypeDef = M_Zoom_35mm;break;
//          case 0x04: M_Zoom_TypeDef = M_Zoom_50mm;break;
//          case 0x05: M_Zoom_TypeDef = M_Zoom_70mm;break;
//          case 0x06: M_Zoom_TypeDef = M_Zoom_80mm;break;
//          case 0x07: M_Zoom_TypeDef = M_Zoom_105mm;break;                                  
//        }
//    }
//    else
//    {
//        M_Zoom_TypeDef = M_Zoom_AUTO;
//    }
}



