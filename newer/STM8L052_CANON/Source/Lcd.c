#include "includes.h"

uint8_t LCD_Data[16]={0};//LCD缓存

/*************************************************************************************
* FunctionName   : TM1621Wr_Data()
* Description    : TM1621 数据  
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void TM1621Wr_Data(unsigned char Data,unsigned char cnt)
{
    unsigned char i;
    for (i=0;i<cnt;i++)
    {
        TM1621_WR_LOW;
        if((Data & 0x80) == 0x80)
        {
            TM1621_DAT_HIGH;
        }
        else
        {
            TM1621_DAT_LOW;
        }
        _Nop();        
        TM1621_WR_HIGH;
        Data<<=1;
        _Nop();    
    }
}
/*************************************************************************************
* FunctionName   : TM1621WrCmd()
* Description    : TM1621 命令  
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621WrCmd(unsigned char Cmd)
{
    TM1621_CS_LOW;
    _Nop();
    TM1621Wr_Data(0x80,3);       //写入命令标志100
    TM1621Wr_Data(Cmd,8);        //写入命令数据    
    TM1621_DAT_LOW;              //写X位，不关心
    TM1621_WR_LOW;
    _Nop();
    TM1621_WR_HIGH;
    _Nop();
    TM1621_CS_HIGH;    
}
/*************************************************************************************
* FunctionName   : TM1621WrAllData()
* Description    : TM1621 写缓存  
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621WrAllData(unsigned char Addr,unsigned char *p,unsigned char cnt)
{
    unsigned char i;
    TM1621_CS_LOW;
    TM1621Wr_Data(0xA0,3);       //写入数据标志101
    TM1621Wr_Data(Addr<<2,6);    //写入地址数据
    for(i=0;i<cnt;i++)
    {
        TM1621Wr_Data(*p,8);     //写入数据
        p++;
    }
    TM1621_CS_HIGH;
}
/*************************************************************************************
* FunctionName   : TM1621_Init()
* Description    : TM1621 初始化配置
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_Init(void)
{  
    uint16_t delay_time;
    delay_time = 0x1000;
    while(delay_time--);    
    TM1621WrCmd(LCDOFF);
    TM1621WrCmd(BIAS1_3);
    TM1621WrCmd(RC256K);                 //使用内部振荡器
    TM1621WrCmd(SYSDIS);
    TM1621WrCmd(WDTDIS);
    TM1621WrCmd(SYSEN);
    TM1621WrCmd(LCDON);
    TM1621WrAllData(0,LCD_Data,16);     //清除1621寄存器数据，清屏
}
/*************************************************************************************
* FunctionName   : LCD_Dis_All_Clear()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void LCD_Dis_All_Clear(void)
{
    memset(LCD_Data,0,sizeof(LCD_Data));    
}
/*************************************************************************************
* FunctionName   : LCD_Display_SLEEP()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void LCD_Display_SLEEP(void)
{
    LCD_Dis_All_Clear();
//    LCD_Dis_22_ON;
    TM1621WrCmd(SYSEN);    
    TM1621WrCmd(LCDON);
    TM1621WrAllData(0,LCD_Data,16);
    //TM1621WrCmd(SYSDIS);
}
/*************************************************************************************
* FunctionName   : LCD_Display()
* Description    : 刷新显示
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void LCD_Display(void)
{
    LCD_Handler();
    TM1621WrAllData(0,LCD_Data,16); 
}
/*************************************************************************************
* FunctionName   : BuzLock_Hndler()
* Description    : 蜂鸣器显示
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void BuzLock_Hndler(void)
{
    if(SysInfo.Beep_En_flag)
    {
        LCD_Dis_S06_ON;
    }
}
/**************************************************************************************
* FunctionName   : Multi_Handler()
* Description    : 频闪档显示
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void Multi_Handler(void)
{
    LCD_Dis_S11_ON;
    LCD_Dis_S19_ON;
    Display_Multi_Hz(Multi_Freq_TypeDef);
    Display_Multi_Times(Multi_Times_TypeDef);
    
    if(Key_SET_Function_Typedef == Key_SET_Freq)
    {
        if(SysInfo.Dis_Glitter_flag)
        {
            
            LCD_Dis_S18_OFF;
            LCD_Digital5_OFF;
            LCD_Digital6_OFF;
        }
    }
    else if(Key_SET_Function_Typedef == Key_SET_Times)
    {
        if(SysInfo.Dis_Glitter_flag)
        {
            LCD_Digital3_OFF;
            LCD_Digital4_OFF;
        }
    }
    else if(Key_SET_Function_Typedef ==Key_SET_Position)
    {
        if(SysInfo.Dis_Glitter_flag)
        {
            LCD_Dis_S07_OFF;
            LCD_Dis_S08_OFF;
            LCD_Dis_S09_OFF;
            LCD_Digital1_OFF;
            LCD_Digital2_OFF;
        }
    }
}
/**************************************************************************************
* FunctionName   : Zoom_Handler()
* Description    : 焦距显示
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void Zoom_Handler(void)
{
    LCD_Dis_S23_ON;
    
   
//    if(Motor_Manger_TypeDef.Motor_State == Motor_Reset_Error)
//    {
//        if(SysInfo.Dis_Glitter_flag)
//        {
//            LCD_Dis_55_OFF;//如果马达3S没复位，闪烁zoom图标
//        }
//    }
//    
    if(M_Zoom_TypeDef != M_Zoom_AUTO)
    {
        LCD_Dis_S25_ON;  //M
        Display_Zoom(M_Zoom_TypeDef); 
    }
    else
    {
        LCD_Dis_S26_ON;  //A
        
        if(comm_state.link)
        {
            Display_Zoom(Motor_Contorl_TypeDef.Motor_Value);
        }
        else  //没通信前显示--
        {
            LCD_Digital7__;
            LCD_Digital8__; 
        }
    }
//    
    if(Key_SET_Function_Typedef == Key_SET_Zoom)
    {
        if(SysInfo.Dis_Glitter_flag)
        {
            LCD_Dis_S24_OFF;
            LCD_Digital7_OFF;
            LCD_Digital8_OFF;
        }
    }
}
///**************************************************************************************
//* FunctionName   : Aperture_Handler()
//* Description    : 光圈显示
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
//static void Aperture_Handler(void)
//{
//    LCD_Digital40_F;
//    LCD_Dis_42_ON; 
//    if(comm_state.link)
//    {
//        Display_Aper(ModeInfo.Aper_value);
//    }
//    else
//    {
//        Display_Aper(0);
//    }
//}
///**************************************************************************************
//* FunctionName   : Mode_Handler()
//* Description    : 模式
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
static void Mode_Handler(void)
{
    LCD_Dis_S32_ON;
    
    if(SysInfo.Low_Battery_flag)
    {
        LCD_Dis_S14_ON;
    }
    
    switch(Sys_Mode_Typedef)
    {
      case Sys_Mode_TTL:
        {
            LCD_Dis_S01_ON;
            DisPlay_M_Gear(100,0,TTL_Ev_TypeDef);
            if(mainPara.synMode == 0x01)
            {
                LCD_Dis_S21_ON;
            }
        }break;
      case Sys_Mode_M:
        {
            LCD_Dis_S02_ON;
            DisPlay_M_Gear(Flash_Output_Typedef,0,100); 
            if(mainPara.synMode == 0x01)
            {
                LCD_Dis_S21_ON;
            }
        }break;
      case  Sys_Mode_RPT:
        {
            LCD_Dis_S02_ON;
            LCD_Dis_S03_ON;
            DisPlay_M_Gear(Multi_Output_Typedef,0,100);
            Multi_Handler();
        }break;
      case   Sys_Mode_S1:
        {
            LCD_Dis_S02_ON;
            LCD_Dis_S04_ON;
            LCD_Dis_S15_ON;
            LCD_Dis_S16_ON;
            DisPlay_M_Gear(Flash_Output_Typedef,0,100);
        }break;
      case  Sys_Mode_S2:
        {
            LCD_Dis_S02_ON;
            LCD_Dis_S04_ON;
            LCD_Dis_S15_ON;
            LCD_Dis_S17_ON;
            DisPlay_M_Gear(Flash_Output_Typedef,0,100);
        }break;
      default:break;    
    }
    
////////////////////////////////////////////////////////////////////////////////////////
//    LCD_Dis_68_ON;                      //温度图标    
//    switch(Lamp_Temp_TypeDef)
//    {
//      case Lamp_Temp_level_2:LCD_Dis_69_ON;
//      case Lamp_Temp_level_1:LCD_Dis_70_ON;
//      case Lamp_Temp_level_0:LCD_Dis_71_ON;
//    }
//    if((SysInfo.Lamp_Glitter_flag) & (SysInfo.Dis_Glitter_flag))//温度到第三级图标闪烁
//    {
//        LCD_Dis_68_OFF;
//        LCD_Dis_69_OFF;
//        LCD_Dis_70_OFF;
//        LCD_Dis_71_OFF;
//    }

}
/**************************************************************************************
* FunctionName   : LCD_Handler()
* Description    : LCD刷新状态
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void LCD_Handler(void)
{
    LCD_Dis_All_Clear();
    
#ifdef DEBUG_MODE
    
    Mode_Handler();
    Zoom_Handler();
    BuzLock_Hndler();

//    static uint8_t discnet_stest =0;
//    
//    discnet_stest++;
//    
//    if(discnet_stest>20)
//    {
//        discnet_stest = 0;
//        static uint8_t ssdfi =0;
//
//        LCD_Data[ssdfi++]= 0xff;
//
//    }

    
#else
    
    if(SysInfo.Sys_Power_Flag)
    {
        Mode_Handler();
        Zoom_Handler();
        BuzLock_Hndler();
    }
    else
    {
        LCD_Digital1_P;
        LCD_Digital2_O;
    }
#endif
}
/**************************************************************************************
* FunctionName   : Display_Zoom()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void Display_Zoom(uint8_t Num)
{
    uint8_t num1,num2,num3;
    
    num1 = Num%10;
    num2 = Num/10%10;
    num3 = Num/100%10;
    
    if(Num == 0)
    {
        LCD_Digital7__;
        LCD_Digital8__; 
    }
    else if(Num > 0)
    {
        switch(num3)
        {   
          case 1: LCD_Dis_S24_ON;break; 
        }
        switch(num2)
        {   
          case 1: LCD_Digital7_1;break;
          case 2: LCD_Digital7_2;break;
          case 3: LCD_Digital7_3;break;
          case 4: LCD_Digital7_4;break;
          case 5: LCD_Digital7_5;break;      
          case 6: LCD_Digital7_6;break;
          case 7: LCD_Digital7_7;break;
          case 8: LCD_Digital7_8;break;
          case 9: LCD_Digital7_9;break;
          case 0: LCD_Digital7_0;break;      
        }
        switch(num1)
        {   
          case 1: LCD_Digital8_1;break;
          case 2: LCD_Digital8_2;break;
          case 3: LCD_Digital8_3;break;
          case 4: LCD_Digital8_4;break;
          case 5: LCD_Digital8_5;break;      
          case 6: LCD_Digital8_6;break;
          case 7: LCD_Digital8_7;break;
          case 8: LCD_Digital8_8;break;
          case 9: LCD_Digital8_9;break;
          case 0: LCD_Digital8_0;break;      
        }
    }
}
/**************************************************************************************
* FunctionName   : Display_Multi_Hz()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void Display_Multi_Hz(uint8_t Num)
{
    uint8_t num1,num2,num3;
    
    num1 = Num%10;
    num2 = Num/10%10;
    num3 = Num/100%10;

    switch(num3)
    {   
      case 1: LCD_Dis_S18_ON;break;
    }
    switch(num2)
    {   
      case 1: LCD_Digital5_1;break;
      case 2: LCD_Digital5_2;break;
      case 3: LCD_Digital5_3;break;
      case 4: LCD_Digital5_4;break;
      case 5: LCD_Digital5_5;break;      
      case 6: LCD_Digital5_6;break;
      case 7: LCD_Digital5_7;break;
      case 8: LCD_Digital5_8;break;
      case 9: LCD_Digital5_9;break;
      case 0: LCD_Digital5_0;break;      
    }
    switch(num1)
    {   
      case 1: LCD_Digital6_1;break;
      case 2: LCD_Digital6_2;break;
      case 3: LCD_Digital6_3;break;
      case 4: LCD_Digital6_4;break;
      case 5: LCD_Digital6_5;break;      
      case 6: LCD_Digital6_6;break;
      case 7: LCD_Digital6_7;break;
      case 8: LCD_Digital6_8;break;
      case 9: LCD_Digital6_9;break;
      case 0: LCD_Digital6_0;break;      
    }
}
/**************************************************************************************
* FunctionName   : Display_Multi_Times()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void Display_Multi_Times(uint8_t Num)
{
    uint8_t num1,num2;
    
    num1 = Num%10;
    num2 = Num/10%10;
    
    if(Num == 0)
    {
        LCD_Digital3__;
        LCD_Digital4__;
    }
    else
    {
        switch(num2)
        {   
          case 1: LCD_Digital3_1;break;
          case 2: LCD_Digital3_2;break;
          case 3: LCD_Digital3_3;break;
          case 4: LCD_Digital3_4;break;
          case 5: LCD_Digital3_5;break;      
          case 6: LCD_Digital3_6;break;
          case 7: LCD_Digital3_7;break;
          case 8: LCD_Digital3_8;break;
          case 9: LCD_Digital3_9;break;
          case 0: LCD_Digital3_0;break;      
        }
        switch(num1)
        {   
          case 1: LCD_Digital4_1;break;
          case 2: LCD_Digital4_2;break;
          case 3: LCD_Digital4_3;break;
          case 4: LCD_Digital4_4;break;
          case 5: LCD_Digital4_5;break;      
          case 6: LCD_Digital4_6;break;
          case 7: LCD_Digital4_7;break;
          case 8: LCD_Digital4_8;break;
          case 9: LCD_Digital4_9;break;
          case 0: LCD_Digital4_0;break;      
        }
    }
}
/**************************************************************************************
* FunctionName   : DisPlay_M_Gear()
* Description    : 
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
static void DisPlay_M_Gear(int8_t M_Gear, uint8_t Large_num, int8_t Ev_num)
{
    uint8_t num1,num2;//,num3;
   
    if(SysInfo.KEY_UP_DOWN_Flag)
    {
        switch(M_Gear)
        {
          case Flash_Output_128:  {Large_num = 128;Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_128s3:{Large_num = 64; Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_128s7:{Large_num = 64; Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_64:   {Large_num = 64; Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_64s3: {Large_num = 32; Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_64s7: {Large_num = 32; Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_32:   {Large_num = 32; Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_32s3: {Large_num = 16; Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_32s7: {Large_num = 16; Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_16:   {Large_num = 16; Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_16s3: {Large_num = 8;  Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_16s7: {Large_num = 8;  Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_8:    {Large_num = 8;  Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_8s3:  {Large_num = 4;  Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_8s7:  {Large_num = 4;  Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_4:    {Large_num = 4;  Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_4s3:  {Large_num = 2;  Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_4s7:  {Large_num = 2;  Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_2:    {Large_num = 2;  Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_2s3:  {Large_num = 1;  Ev_num = TTL_Ev_P0s7;     }break;
          case Flash_Output_2s7:  {Large_num = 1;  Ev_num = TTL_Ev_P0s3;     }break;
          case Flash_Output_1:    {Large_num = 1;  Ev_num = TTL_Ev_0;        }break;
          default: break;
        }
    }
    else
    {        
        switch(M_Gear)
        {
          case Flash_Output_128:  {Large_num = 128;Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_128s3:{Large_num = 128;Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_128s7:{Large_num = 128;Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_64:   {Large_num = 64; Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_64s3: {Large_num = 64; Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_64s7: {Large_num = 64; Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_32:   {Large_num = 32; Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_32s3: {Large_num = 32; Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_32s7: {Large_num = 32; Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_16:   {Large_num = 16; Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_16s3: {Large_num = 16; Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_16s7: {Large_num = 16; Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_8:    {Large_num = 8;  Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_8s3:  {Large_num = 8;  Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_8s7:  {Large_num = 8;  Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_4:    {Large_num = 4;  Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_4s3:  {Large_num = 4;  Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_4s7:  {Large_num = 4;  Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_2:    {Large_num = 2;  Ev_num = TTL_Ev_0;        }break;
          case Flash_Output_2s3:  {Large_num = 2;  Ev_num = TTL_Ev_N0s3;     }break;
          case Flash_Output_2s7:  {Large_num = 2;  Ev_num = TTL_Ev_N0s7;     }break;
          case Flash_Output_1:    {Large_num = 1;  Ev_num = TTL_Ev_0;        }break;
          default: break;
        }
    }
    
    num1 = Large_num%10;
    num2 = Large_num/10%10;
//    num3 = Large_num/100%10;
    
    if(Ev_num > TTL_Ev_0)
    {            
        LCD_Dis_S10_ON;
        LCD_Dis_S11_ON;
        LCD_Dis_S12_ON;
        LCD_Dis_S13_ON;
    }
    else if(Ev_num < TTL_Ev_0)
    {
        LCD_Dis_S11_ON;
        LCD_Dis_S12_ON;
        LCD_Dis_S13_ON;
    }
    
    switch(Ev_num)
    {
      case TTL_Ev_P3s0:{LCD_Digital3_3;LCD_Digital4_0;};break;
      case TTL_Ev_P2s7:{LCD_Digital3_2;LCD_Digital4_7;};break;
      case TTL_Ev_P2s3:{LCD_Digital3_2;LCD_Digital4_3;};break;
      case TTL_Ev_P2s0:{LCD_Digital3_2;LCD_Digital4_0;};break;
      case TTL_Ev_P1s7:{LCD_Digital3_1;LCD_Digital4_7;};break;
      case TTL_Ev_P1s3:{LCD_Digital3_1;LCD_Digital4_3;};break;
      case TTL_Ev_P1s0:{LCD_Digital3_1;LCD_Digital4_0;};break;
      case TTL_Ev_P0s7:{LCD_Digital3_0;LCD_Digital4_7;};break;
      case TTL_Ev_P0s3:{LCD_Digital3_0;LCD_Digital4_3;};break;
      case TTL_Ev_0:   {LCD_Digital3_OFF;LCD_Digital4_OFF;};break;
      case TTL_Ev_N0s3:{LCD_Digital3_0;LCD_Digital4_3;};break;
      case TTL_Ev_N0s7:{LCD_Digital3_0;LCD_Digital4_7;};break;
      case TTL_Ev_N1s0:{LCD_Digital3_1;LCD_Digital4_0;};break;
      case TTL_Ev_N1s3:{LCD_Digital3_1;LCD_Digital4_3;};break;
      case TTL_Ev_N1s7:{LCD_Digital3_1;LCD_Digital4_7;};break;
      case TTL_Ev_N2s0:{LCD_Digital3_2;LCD_Digital4_0;};break;
      case TTL_Ev_N2s3:{LCD_Digital3_2;LCD_Digital4_3;};break;
      case TTL_Ev_N2s7:{LCD_Digital3_2;LCD_Digital4_7;};break;
      case TTL_Ev_N3s0:{LCD_Digital3_3;LCD_Digital4_0;};break;
      default: break;
    }       
    
    if(Large_num>=100)
    {
        LCD_Dis_S07_ON;
        LCD_Dis_S08_ON;
        LCD_Dis_S09_ON;
        switch(num2)
        {
          case 1: LCD_Digital1_1;break;
          case 2: LCD_Digital1_2;break;
          case 3: LCD_Digital1_3;break;
          case 4: LCD_Digital1_4;break;
          case 5: LCD_Digital1_5;break;      
          case 6: LCD_Digital1_6;break;
          case 7: LCD_Digital1_7;break;
          case 8: LCD_Digital1_8;break;
          case 9: LCD_Digital1_9;break;
          case 0: LCD_Digital1_OFF;break;   
        }
        switch(num1)
        {
          case 1: LCD_Digital2_1;break;
          case 2: LCD_Digital2_2;break;
          case 3: LCD_Digital2_3;break;
          case 4: LCD_Digital2_4;break;
          case 5: LCD_Digital2_5;break;      
          case 6: LCD_Digital2_6;break;
          case 7: LCD_Digital2_7;break;
          case 8: LCD_Digital2_8;break;
          case 9: LCD_Digital2_9;break;
          case 0: LCD_Digital2_OFF;break;   
        }
    }
    else if((Large_num>=10)&&(Large_num<100))
    {
        LCD_Dis_S07_ON;
        LCD_Dis_S08_ON;
     
        switch(num2)
        {
          case 1: LCD_Digital1_1;break;
          case 2: LCD_Digital1_2;break;
          case 3: LCD_Digital1_3;break;
          case 4: LCD_Digital1_4;break;
          case 5: LCD_Digital1_5;break;      
          case 6: LCD_Digital1_6;break;
          case 7: LCD_Digital1_7;break;
          case 8: LCD_Digital1_8;break;
          case 9: LCD_Digital1_9;break;
          case 0: LCD_Digital1_OFF;break;   
        }
        switch(num1)
        {
          case 1: LCD_Digital2_1;break;
          case 2: LCD_Digital2_2;break;
          case 3: LCD_Digital2_3;break;
          case 4: LCD_Digital2_4;break;
          case 5: LCD_Digital2_5;break;      
          case 6: LCD_Digital2_6;break;
          case 7: LCD_Digital2_7;break;
          case 8: LCD_Digital2_8;break;
          case 9: LCD_Digital2_9;break;
          case 0: LCD_Digital2_OFF;break;   
        }
    }
    else if(Large_num<10)
    {
        if(Large_num == 0)
        {
            ;
        }
        else
        {
            LCD_Dis_S07_ON;
            LCD_Dis_S08_ON;
        }

        switch(num1)
        {
          case 1: LCD_Digital1_1;break;
          case 2: LCD_Digital1_2;break;
          case 3: LCD_Digital1_3;break;
          case 4: LCD_Digital1_4;break;
          case 5: LCD_Digital1_5;break;      
          case 6: LCD_Digital1_6;break;
          case 7: LCD_Digital1_7;break;
          case 8: LCD_Digital1_8;break;
          case 9: LCD_Digital1_9;break;
          case 0: LCD_Digital1_OFF;break;
        }
    }    
}
