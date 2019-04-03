#include "includes.h"


_Key_Flag_Struct        Key_Flag;
static struct_KeyInfo g_KeyInfo1 = {0, 0, 0, 0, 0, 0, KeyDownCallBack,KeyUpCallBack}; 
extern uint16_t uhADCxConvertedValue[5];
uint8_t Multi_Output_MAX;

/**************************************************************************************
* FunctionName   : Key_Scan 
* Description    : 按键扫描
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/ 
uint8_t Key_Scan(void)
{
    uint16_t KEY_Value;
    uint8_t  key_set = 0;   
    
    //ADC_VALUE.Channel_KEY_VALUE = filter(0);
    KEY_Value =  ADC_VALUE.Channel_KEY_VALUE;
    
    if((KEY_Value > KEY_S2_MIN_Value)&&(KEY_Value < KEY_S2_MAX_Value))
    {
        key_set = KEY_S2;
    }    
    else if((KEY_Value > KEY_S3_MIN_Value)&&(KEY_Value < KEY_S3_MAX_Value))
    {
        key_set = KEY_S3;
    }
    else if((KEY_Value > KEY_S4_MIN_Value)&&(KEY_Value < KEY_S4_MAX_Value))
    {
        key_set = KEY_S4;
    }
    else if((KEY_Value > KEY_S5_MIN_Value)&&(KEY_Value < KEY_S5_MAX_Value))
    {
        key_set = KEY_S5;
    }
    else if((KEY_Value > KEY_S6_MIN_Value)&&(KEY_Value < KEY_S6_MAX_Value))
    {
        key_set = KEY_S6;
    }
    else if((KEY_Value > KEY_S7_MIN_Value)&&(KEY_Value < KEY_S7_MAX_Value))
    {
        key_set = KEY_S7;
    }
    else if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))
    {
        key_set = KEY_PILOT;
    }
    else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))
    {
        key_set = KEY_POWER;
    }
    else
    {
        key_set = 0;
    } 
    
    return key_set;
}
/**************************************************************************************
* FunctionName   : KeyDownCallBack 
* Description    : 键确认按下的回调函数
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/ 
uint8_t KeyDownCallBack(uint16_t Key, uint16_t Times) 
{ 
    switch(Key) 
    {        
      case KEY_S2: 
        if(Times < 50)              
        { 
            return _REENTER;        //2s内允许长按 
        } 
        break;
      case KEY_S3: 
        if(Times < 50)              
        { 
            return _REENTER;        //2s内允许长按 
        } 
        break; 
      case KEY_S4: 
        if(Times < 50)               
        { 
            return _REENTER;        //2s内允许长按 
        } 
        break;
      case KEY_S5: 
        if(Times < 50)              
        { 
            return _REENTER;        //2s内允许长按 
        } 
        break;
      case KEY_S7: 
        if(Times < 100)              
        { 
            return _REENTER;        //2s内允许长按 
        } 
        break;
      case KEY_POWER: 
        if(Times < 100)              
        { 
            return _REENTER;        //2s内允许长按 
        } 
        break;
      default:break; 
    }    
    //    g_DownKey = Key;           //输出按键信息, 给主循环处理. 这个回调函数是由定时中断中的代码调用的.
    return _NO_REENTER;             //其余键, 不允许长按 
} 
/**************************************************************************************
* FunctionName   : KeyUpCallBack 
* Description    : 键抬起的回调函数
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/ 
void KeyUpCallBack(uint16_t Key)
{
    Key_Flag.Key_Short = 0;
    Key_Flag.Key_Long  = 0; 
}
/**************************************************************************************
* FunctionName   : DitherlessKey(struct_KeyInfo* pInfo) 
* Description    : 定时消抖的按键处理函数, 通常在定时中断中调用,
* EntryParameter : 回调函数的返回值决定了是否允许出现长按的情况 .
* ReturnValue    : None
**************************************************************************************/ 
void DitherlessKey(struct_KeyInfo* pInfo) 
{ 
    switch(pInfo->KeyState) 
    { 
      case _HAS_NO_KEY: 
        {
            pInfo->SameKeyCntr = 0; 
            if(pInfo->CurKey != _KEY_NONE) 
            { 
                pInfo->KeyState = _HAS_KEY_DOWN;                           //进入有键按下状态 
            }            
        }break;
        
      case _HAS_KEY_DOWN: 
        {
            if(pInfo->PreDownKey == pInfo->CurKey) 
            { 
                pInfo->KeyState = _HAS_KEY_SURE;                           //确认键已按下状态 
            } 
            else 
            { 
                pInfo->KeyState = _HAS_NO_KEY;                             //回到无键状态 
            }
        }break;
        
      case _HAS_KEY_SURE:
        {
            if(pInfo->CurKey == pInfo->PreDownKey) 
            { 
                pInfo->KeyState = _HAS_KEY_WAITUP; 
                if(pInfo->KeyDownCallBack) 
                { 
                    if(_REENTER == pInfo->KeyDownCallBack(pInfo->CurKey, pInfo->SameKeyCntr)) //这里回调函数
                    { 
                        pInfo->KeyState = _HAS_KEY_SURE; 
                        ++pInfo->SameKeyCntr; 
                    } 
                } 
            } 
            else 
            { 
                pInfo->KeyState = _KEY_NONE; 
            }  
        }break;
        
      case _HAS_KEY_WAITUP: 
        {
            if(pInfo->CurKey != pInfo->PreDownKey) 
            { 
                pInfo->KeyState = _HAS_NO_KEY; 
                
                if(pInfo->KeyUpCallBack) 
                { 
                    pInfo->KeyUpCallBack(pInfo->PreDownKey); 
                } 
            } 
        }break;
        
      default: break; 
    }
    
    pInfo->PreDownKey = pInfo->CurKey;                                        //保存上次按键值
    
    return; 
} 
/**************************************************************************************
* FunctionName   : Key_Stroke()
* Description    : 按键处理，长按键在这里处理
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_Stroke(void)
{  
    if(g_KeyInfo1.KeyState == _HAS_KEY_SURE)     //短按键处理,按下触发
    {
        switch(g_KeyInfo1.CurKey)
        {         
          case KEY_S6: Key_S6_Str();break;
          case KEY_PILOT:Key_Pilot_Str();break;
          default: break; 
        }
        
        if(Key_Flag.Key_Short == 0)
        {
            Key_Flag.Key_Short = 1;
            Key_Flag.Key_Short_Set = 1;
            g_KeyInfo1.LastKey = g_KeyInfo1.CurKey;
        }        
    }
    else if(g_KeyInfo1.KeyState == _HAS_NO_KEY) //短按键处理,松手触发
    {
        Key_Flag.Key_Short = 0;
        Key_Flag.Key_Long  = 0;
        g_KeyInfo1.Long_Key_t = 0;
        if(Key_Flag.Key_Short_Set)
        {
            Key_Flag.Key_Short_Set = 0;
            
            switch(g_KeyInfo1.LastKey) 
            {
              case KEY_S2: Key_S2_Str();break;                
              case KEY_S3: Key_S3_Str();break;
              case KEY_S4: Key_S4_Str();break;
              case KEY_S5: Key_S5_Str();break;
              case KEY_S7: Key_S7_Str();break;
              case KEY_POWER:Key_Power_Str();break;
              default: break; 
            }
            g_KeyInfo1.LastKey = 0;
        }
    }    
    else if(g_KeyInfo1.KeyState == _HAS_KEY_WAITUP) //长按键处理        
    {
        if(Key_Flag.Key_Long == 0)                       //长按键只触发一次
        {
            Key_Flag.Key_Long = 1;
            Key_Flag.Key_Short_Set = 0;
            
            switch(g_KeyInfo1.CurKey)
            {
              case KEY_S2: Key_S2_Long_Str();break;
              case KEY_S3: Key_S3_Long_Str();break;
              case KEY_S4: Key_S4_Long_Str();break;
              case KEY_S5: Key_S5_Long_Str();break;
              case KEY_S7: Key_S7_Long_Str();break;  
              case KEY_POWER:Key_Power_Long_Str();break;
              default: break;
            }
        }
        
        g_KeyInfo1.Long_Key_t++;                   //长按键触发后开始连续触发计时
        
        if(g_KeyInfo1.Long_Key_t>10)               //长按键连续触发
        {
            g_KeyInfo1.Long_Key_t = 0;
            
            switch(g_KeyInfo1.LastKey) 
            {
              case KEY_S2: Key_S2_Str();break;                
              case KEY_S3: Key_S3_Str();break;
              case KEY_S4: Key_S4_Str();break;
              case KEY_S5: Key_S5_Str();break;
              default: break; 
            } 
        }
    }
}
/**************************************************************************************
* FunctionName   : Key_Stroke()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_Proc(void)
{ 
    uint16_t temp;    
    temp = Key_Scan();     
    g_KeyInfo1.CurKey = temp & 0x00FF;                 
    DitherlessKey(&g_KeyInfo1);
    Key_Stroke();
}
void Key_Beep_Srt(void)
{
    SysInfo.Dis_Glitter_flag_en = 1;
    
    SysInfo.KEY_AUTO_SET_Count = 0;
    
    SysInfo.BK_Light_Count = 0;
    
    if(SysInfo.Beep_En_flag)
    {
        BeepFlag.Beep_Key_Flag = 1;   //蜂鸣器使能
    }
    
    SysInfo.KEY_SET_Change_flag = SET;
}
/**************************************************************************************
* FunctionName   : Key_S2_Str()
* Description    : UP
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_S2_Str(void)//UP
{
//    SysInfo.KEY_UP_DOWN_Flag = 0;
    
//    Key_SET_Function_Typedef = Key_SET_None;
        
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:
        {
            if(TTL_Ev_TypeDef < TTL_Ev_N3s0)
            {
                TTL_Ev_TypeDef+=1;
                Key_Beep_Srt();
            }
            if(TTL_Ev_TypeDef > TTL_Ev_N3s0)
            {
                TTL_Ev_TypeDef = TTL_Ev_N3s0;
            }            
        }break;
      case Sys_Mode_M:
      case Sys_Mode_S1:
      case Sys_Mode_S2:
        {
            switch(abs(Flash_Output_Typedef%3))
            {
              case 0: SysInfo.KEY_UP_DOWN_Flag_Change = 1;break;
              case 1: SysInfo.KEY_UP_DOWN_Flag_Change = 0;break;
              case 2: SysInfo.KEY_UP_DOWN_Flag_Change = 0;break;
            }
            if(SysInfo.KEY_UP_DOWN_Flag_Change)
            {
                SysInfo.KEY_UP_DOWN_Flag_Change = 0;
                SysInfo.KEY_UP_DOWN_Flag = 0;
            }
            if(Flash_Output_Typedef < Flash_Output_1)
            {
                Flash_Output_Typedef+=1;
                Key_Beep_Srt();
            }
        }break;
      case Sys_Mode_RPT:break;
//      case Sys_Mode_S1:
//      case Sys_Mode_S2:
//        {
//            if(Slave_Output_Typedef < Slave_Output_1)
//            {
//                Slave_Output_Typedef+=1;
//                Key_Beep_Srt();
//            }
//        }break;
      default:break;
    }
}
void Key_S2_Long_Str(void)
{
    ;
}
/**************************************************************************************
* FunctionName   : Key_S4_Str()
* Description    : DOWN
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_S4_Str(void)//DOWN
{
    
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:
        {
            if(TTL_Ev_TypeDef > TTL_Ev_P3s0)
            {
                TTL_Ev_TypeDef-=1;
                Key_Beep_Srt();
            }
            if(TTL_Ev_TypeDef < TTL_Ev_P3s0)
            {
                TTL_Ev_TypeDef = TTL_Ev_P3s0;
            }  
        }break;
      case Sys_Mode_M:
      case Sys_Mode_S1:
      case Sys_Mode_S2:
        {
            switch(abs(Flash_Output_Typedef%3))
            {
              case 0: SysInfo.KEY_UP_DOWN_Flag_Change = 1;break;
              case 1: SysInfo.KEY_UP_DOWN_Flag_Change = 0;break;
              case 2: SysInfo.KEY_UP_DOWN_Flag_Change = 0;break;
            }
            if(SysInfo.KEY_UP_DOWN_Flag_Change)
            {
                SysInfo.KEY_UP_DOWN_Flag_Change = 0;
                SysInfo.KEY_UP_DOWN_Flag = 1;
            }
            if(Flash_Output_Typedef > Flash_Output_128)
            {
                Flash_Output_Typedef-=1;
                Key_Beep_Srt();
            }
        }break;
      case Sys_Mode_RPT:break;
//      case Sys_Mode_S1:
//      case Sys_Mode_S2:
//        {
//            if(Slave_Output_Typedef > Slave_Output_128)
//            {
//                Slave_Output_Typedef-=1;
//                Key_Beep_Srt();
//            }
//            if(Slave_Output_Typedef < Slave_Output_128)
//            {
//                Slave_Output_Typedef = Slave_Output_128;
//            } 
//        }break;
      default:break;
    }
}
void Key_S4_Long_Str(void)
{
    ;
}
/**************************************************************************************
* FunctionName   : Key_S5_Str()
* Description    : RIGHT
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_S5_Str(void)//RIGHT
{
    if(Key_SET_Function_Typedef == Key_SET_Zoom)
    {
        if(M_Zoom_TypeDef < M_Zoom_180mm)
        {            
            uint8_t count;
            count = sizeof(T_Zoom_Group);                    
            while(--count){if(M_Zoom_TypeDef >= T_Zoom_Group[count])break;}
            count++;
            Key_Beep_Srt();
            M_Zoom_TypeDef = T_Zoom_Group[count];
        }
    } 
    else
    {
        switch(Sys_Mode_Typedef)
        {
          case Sys_Mode_TTL:
            {
                if(TTL_Ev_TypeDef < TTL_Ev_N3s0)
                {
                    TTL_Ev_TypeDef+=3;
                    Key_Beep_Srt();
                }
                if(TTL_Ev_TypeDef > TTL_Ev_N3s0)
                {
                    TTL_Ev_TypeDef = TTL_Ev_N3s0;
                } 
            }break;
          case Sys_Mode_M:
          case Sys_Mode_S1:
          case Sys_Mode_S2:
            {
                if(Flash_Output_Typedef < Flash_Output_1)
                {
                    Flash_Output_Typedef+=3;
                    Key_Beep_Srt();
                }
                if(Flash_Output_Typedef > Flash_Output_1)
                {
                    Flash_Output_Typedef = Flash_Output_1;
                }
            }break;
          case Sys_Mode_RPT:
            {
                switch(Key_SET_Function_Typedef)
                {
                  case Key_SET_Position:
                    {
                        if(Multi_Output_Typedef < Multi_Output_4)
                        {
                            Multi_Output_Typedef+=3;
                            Key_Beep_Srt();
                        }
//                        if(Multi_Output_Typedef > Multi_Output_4)
//                        {
//                            Multi_Output_Typedef = Multi_Output_4;
//                        } 
                        switch(Multi_Output_Typedef)
                        {
                          case Multi_Output_128:if(Multi_Times_TypeDef>40) {Multi_Times_TypeDef = Multi_Times_40;};break;
                          case Multi_Output_64: if(Multi_Times_TypeDef>20) {Multi_Times_TypeDef = Multi_Times_20;};break;
                          case Multi_Output_32: if(Multi_Times_TypeDef>12) {Multi_Times_TypeDef = Multi_Times_12;};break;
                          case Multi_Output_16: if(Multi_Times_TypeDef>8 ) {Multi_Times_TypeDef = Multi_Times_8 ;};break;
                          case Multi_Output_8:  if(Multi_Times_TypeDef>4 ) {Multi_Times_TypeDef = Multi_Times_4 ;};break;
                          case Multi_Output_4:  if(Multi_Times_TypeDef>2 ) {Multi_Times_TypeDef = Multi_Times_2 ;};break;
                          default:break;
                        }
                    }break;
                  case Key_SET_Times:
                    {
                        uint8_t count;
                        
                        switch(Multi_Output_Typedef)
                        {
                          case Multi_Output_128:Multi_Output_MAX = 40; break;
                          case Multi_Output_64: Multi_Output_MAX = 20;break;
                          case Multi_Output_32: Multi_Output_MAX = 12;break;
                          case Multi_Output_16: Multi_Output_MAX = 8 ;break;
                          case Multi_Output_8:  Multi_Output_MAX = 4 ;break;
                          case Multi_Output_4:  Multi_Output_MAX = 2 ;break;
                          default:break;
                        }
                        
                        multiSet.maxTimes = Multi_Output_MAX;
                        
                        if(Multi_Times_TypeDef < Multi_Output_MAX)
                        {
                            
                            count = sizeof(T_Multi_Times);                    
                            while(--count){if(Multi_Times_TypeDef >= T_Multi_Times[count])break;}
                            if(count < (sizeof(T_Multi_Times)-1))
                            {
                                count++;
                                Key_Beep_Srt();
                                Multi_Times_TypeDef = T_Multi_Times[count];
                            }                            
                        }                        
                    }break;
                  case Key_SET_Freq:
                    {
                        uint8_t count;
                        count = sizeof(T_Multi_Freq);                    
                        while(--count){if(Multi_Freq_TypeDef >= T_Multi_Freq[count])break;}
                        if(count < (sizeof(T_Multi_Freq)-1))
                        {
                            count++;
                            Key_Beep_Srt();
                            Multi_Freq_TypeDef = T_Multi_Freq[count];
                        }
                    }break;
                }
            }break;
//          case Sys_Mode_S1:
//          case Sys_Mode_S2:
//            {
//                if(Slave_Output_Typedef < Slave_Output_1)
//                {
//                    Slave_Output_Typedef+=3;
//                    Key_Beep_Srt();
//                }
//                if(Slave_Output_Typedef > Slave_Output_1)
//                {
//                    Slave_Output_Typedef = Slave_Output_1;
//                }
//            }break;
          default:break;
        }
    }
}
void Key_S5_Long_Str(void)
{
    ;
}
/**************************************************************************************
* FunctionName   : Key_S3_Str()
* Description    : LEFT
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_S3_Str(void)//LEFT
{
    if(Key_SET_Function_Typedef == Key_SET_Zoom)
    {
        if(M_Zoom_TypeDef > M_Zoom_AUTO)
        {            
            uint8_t count;
            count = sizeof(T_Zoom_Group);                    
            while(--count){if(M_Zoom_TypeDef >= T_Zoom_Group[count])break;}
            count--;
            Key_Beep_Srt();
            M_Zoom_TypeDef = T_Zoom_Group[count];
        }
    }
    else
    {
        switch(Sys_Mode_Typedef)
        {
          case Sys_Mode_TTL:
            {
                if(TTL_Ev_TypeDef > TTL_Ev_P3s0)
                {
                    TTL_Ev_TypeDef-=3;
                    Key_Beep_Srt();
                }
                if(TTL_Ev_TypeDef < TTL_Ev_P3s0)
                {
                    TTL_Ev_TypeDef = TTL_Ev_P3s0;
                }
            }break;
          case Sys_Mode_M:
          case Sys_Mode_S1:
          case Sys_Mode_S2:
            {
                if(Flash_Output_Typedef > Flash_Output_128)
                {
                    Flash_Output_Typedef-=3;
                    Key_Beep_Srt();
                }
                if(Flash_Output_Typedef < Flash_Output_128)
                {
                    Flash_Output_Typedef = Flash_Output_128;
                } 
            }break;
          case Sys_Mode_RPT:
            {
                switch(Key_SET_Function_Typedef)
                {
                  case Key_SET_Position:
                    {                        
                        if(Multi_Output_Typedef > Multi_Output_128)
                        {
                            Multi_Output_Typedef-=3;
                            Key_Beep_Srt();
                        }
                        if(Multi_Output_Typedef < Multi_Output_128)
                        {
                            Multi_Output_Typedef = Multi_Output_128;
                        } 
                    }break;
                  case Key_SET_Times:
                    {
                        uint8_t count;
                        count = sizeof(T_Multi_Times);                    
                        while(--count){if(Multi_Times_TypeDef >= T_Multi_Times[count])break;}
                        if(count > 0)
                        {
                            count--;
                            Key_Beep_Srt();
                            Multi_Times_TypeDef = T_Multi_Times[count];
                        }
                    }break;
                  case Key_SET_Freq:
                    {
                        uint8_t count;
                        count = sizeof(T_Multi_Freq);                    
                        while(--count){if(Multi_Freq_TypeDef >= T_Multi_Freq[count])break;}
                        if(count > 0)
                        {
                            count--;
                            Key_Beep_Srt();
                            Multi_Freq_TypeDef = T_Multi_Freq[count];
                        }
                    }break;
                }
            }break;
//          case Sys_Mode_S1:
//          case Sys_Mode_S2:
//            {
//                if(Slave_Output_Typedef > Slave_Output_128)
//                {
//                    Slave_Output_Typedef-=3;
//                    Key_Beep_Srt();
//                }
//                if(Slave_Output_Typedef < Slave_Output_128)
//                {
//                    Slave_Output_Typedef = Slave_Output_128;
//                } 
//            }break;
          default:break;
        }
    }
}
void Key_S3_Long_Str(void)
{
    ;
}
/**************************************************************************************
* FunctionName   : Key_S6_Str()
* Description    : MODE
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_S6_Str(void)//MODE
{
    Key_Beep_Srt();
    Key_SET_Function_Typedef = Key_SET_None;
    Sys_Mode_Typedef++;    
    if(Sys_Mode_Typedef > Sys_Mode_S2)
    {
        Sys_Mode_Typedef = Sys_Mode_TTL;
    }
    if((Sys_Mode_Typedef == Sys_Mode_S1) | (Sys_Mode_Typedef == Sys_Mode_S2))
    {
        GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_PU_IT);           //PHOTOSEN
    }
    else
    {
        GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_In_FL_No_IT);        //PHOTOSEN
    }
    
    SysInfo.KEY_SET_Change_flag = SET;
}
/**************************************************************************************
* FunctionName   : Key_S7_Str()
* Description    : SET
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_S7_Str(void)//SET
{
    Key_Beep_Srt();
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:
      case Sys_Mode_M:
        {
            if(Key_SET_Function_Typedef == Key_SET_None)
            {
                Key_SET_Function_Typedef = Key_SET_Zoom;
            }
            else
            {
                Key_SET_Function_Typedef = Key_SET_None;
            }
        }break;
      case Sys_Mode_RPT:
        {
            Key_SET_Function_Typedef++;
            if(Key_SET_Function_Typedef > Key_SET_Zoom)
            {
                Key_SET_Function_Typedef = Key_SET_None;
            }
        }break;
      case Sys_Mode_S1:
      case Sys_Mode_S2:
        {
            if(Key_SET_Function_Typedef == Key_SET_None)
            {
                Key_SET_Function_Typedef = Key_SET_Zoom;
            }
            else
            {
                Key_SET_Function_Typedef = Key_SET_None;
            }
        }break;
      default:break;
    }
}
void Key_S7_Long_Str(void)
{
    SysInfo.Beep_En_flag = !SysInfo.Beep_En_flag;
    Key_Beep_Srt();
}
/**************************************************************************************
* FunctionName   : Key_Pilot_Str()
* Description    : SET
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_Pilot_Str(void)
{
    SysInfo.Dis_Glitter_flag_en = 1;    
    SysInfo.BK_Light_Count = 0;
    Key_SET_Function_Typedef = Key_SET_None;
    
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:
      case Sys_Mode_M:
        {
            Formalfrash(Flash_Time_Group[getVolLevel()][(Flash_Output_Typedef+9)+6]);
        }break;
      case Sys_Mode_RPT:
        {
            Multiflash();
        }break;
      case Sys_Mode_S1:
      case Sys_Mode_S2:
        {
            Formalfrash(Flash_Time_Group[getVolLevel()][(Flash_Output_Typedef+9)+6]);
        }
      default:break;
    }
}
/**************************************************************************************
* FunctionName   : Key_Power_Str()
* Description    : SET
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Key_Power_Str(void)
{
    SysInfo.BK_Light_Flag = !SysInfo.BK_Light_Flag;   
}
void Key_Power_Long_Str(void)
{
    SysInfo.Sys_Power_Flag = !SysInfo.Sys_Power_Flag;
    
    if(SysInfo.Sys_Power_Flag)
    {
        SysInfo.Sys_Power_Flag_On = 1;
        Read_EepromData();
        BeepFlag.Beep_TrueFlash_Flag = 1;
        TM1621_Init();
        MOTOR_RESET_EN;
    }
    else
    {
        SysInfo.Sys_Power_Flag_Off = 1;
        Save_EepromData();
        BeepFlag.Beep_PowerOff_Flag = 1;
        Disable_Flash_IN_Int;
        EXTI_ClearITPendingBit(EXTI_IT_Pin1);
        EXTI_ClearITPendingBit(EXTI_IT_Pin4);
        EXTI_ClearITPendingBit(EXTI_IT_Pin5);
        disableInterrupts();
    }
}