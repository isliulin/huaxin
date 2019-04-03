#ifndef __FUNCT_T
#define __FUNCT_T

#include "stm8l15x.h"

typedef enum
{
    Sys_Mode_None,
    Sys_Mode_TTL,
    Sys_Mode_M,
    Sys_Mode_RPT,
    Sys_Mode_S1,
    Sys_Mode_S2,
}_Sys_Mode_Typedef;
    
typedef enum
{
    Key_SET_None,
    Key_SET_Freq,
    Key_SET_Times,
    Key_SET_Position,
    Key_SET_Zoom,
}_Key_SET_Function_Typedef;

typedef enum
{
    Flash_Output_128            = (int8_t)-9,
    Flash_Output_128s3,         Flash_Output_128s7,
    Flash_Output_64,            Flash_Output_64s3,      Flash_Output_64s7,
    Flash_Output_32,            Flash_Output_32s3,      Flash_Output_32s7,
    Flash_Output_16,            Flash_Output_16s3,      Flash_Output_16s7,
    Flash_Output_8,             Flash_Output_8s3,       Flash_Output_8s7,
    Flash_Output_4,             Flash_Output_4s3,       Flash_Output_4s7,
    Flash_Output_2,             Flash_Output_2s3,
    Flash_Output_2s7,    
    Flash_Output_1    
}_Flash_Output_Typedef;

typedef enum
{
    Multi_Output_128            = (int8_t)-9,
    Multi_Output_128s3,         Multi_Output_128s7,
    Multi_Output_64,            Multi_Output_64s3,      Multi_Output_64s7,
    Multi_Output_32,            Multi_Output_32s3,      Multi_Output_32s7,
    Multi_Output_16,            Multi_Output_16s3,      Multi_Output_16s7,
    Multi_Output_8,             Multi_Output_8s3,       Multi_Output_8s7,
    Multi_Output_4,             Multi_Output_4s3,       Multi_Output_4s7,
    Multi_Output_2,             Multi_Output_2s3,
    Multi_Output_2s7,    
    Multi_Output_1    
}_Multi_Output_Typedef;

//typedef enum
//{
//    Slave_Output_128            = (int8_t)-9,
//    Slave_Output_128s3,         Slave_Output_128s7,
//    Slave_Output_64,            Slave_Output_64s3,      Slave_Output_64s7,
//    Slave_Output_32,            Slave_Output_32s3,      Slave_Output_32s7,
//    Slave_Output_16,            Slave_Output_16s3,      Slave_Output_16s7,
//    Slave_Output_8,             Slave_Output_8s3,       Slave_Output_8s7,
//    Slave_Output_4,             Slave_Output_4s3,       Slave_Output_4s7,
//    Slave_Output_2,             Slave_Output_2s3,
//    Slave_Output_2s7,    
//    Slave_Output_1    
//}_Slave_Output_Typedef;

typedef enum
{
    TTL_Ev_P3s0     =(int8_t)-9,
    TTL_Ev_P2s7,    TTL_Ev_P2s3,
    TTL_Ev_P2s0,    TTL_Ev_P1s7,    TTL_Ev_P1s3,
    TTL_Ev_P1s0,    TTL_Ev_P0s7,    TTL_Ev_P0s3,
    TTL_Ev_0,
    TTL_Ev_N0s3,    TTL_Ev_N0s7,    TTL_Ev_N1s0,
    TTL_Ev_N1s3,    TTL_Ev_N1s7,    TTL_Ev_N2s0,
    TTL_Ev_N2s3,    TTL_Ev_N2s7,    TTL_Ev_N3s0
}_TTL_Ev_TypeDef;


//typedef enum
//{
//    Multi_Freq_1       =(uint8_t)1, 
//    Multi_Freq_2       =(uint8_t)2,    
//    Multi_Freq_3       =(uint8_t)3,
//    Multi_Freq_4       =(uint8_t)4,
//    Multi_Freq_5       =(uint8_t)5,
//    Multi_Freq_6       =(uint8_t)6,
//    Multi_Freq_7       =(uint8_t)7,
//    Multi_Freq_8       =(uint8_t)8,
//    Multi_Freq_9       =(uint8_t)9,
//    Multi_Freq_10      =(uint8_t)10,
//    Multi_Freq_20      =(uint8_t)20,
//    Multi_Freq_30      =(uint8_t)30,
//    Multi_Freq_40      =(uint8_t)40,
//    Multi_Freq_50      =(uint8_t)50,
//    Multi_Freq_60      =(uint8_t)60,
//    Multi_Freq_70      =(uint8_t)70,
//    Multi_Freq_80      =(uint8_t)80,
//    Multi_Freq_90      =(uint8_t)90,
//    Multi_Freq_100     =(uint8_t)100
//}_Multi_Freq_TypeDef;
//
//typedef enum
//{
//    Multi_Times_0       =(uint8_t)0, 
//    Multi_Times_2       =(uint8_t)2,    
//    Multi_Times_3       =(uint8_t)3,
//    Multi_Times_4       =(uint8_t)4,
//    Multi_Times_5       =(uint8_t)5,
//    Multi_Times_6       =(uint8_t)6,
//    Multi_Times_7       =(uint8_t)7,
//    Multi_Times_8       =(uint8_t)8,
//    Multi_Times_9       =(uint8_t)9,
//    Multi_Times_10      =(uint8_t)10,
//    Multi_Times_15      =(uint8_t)15,
//    Multi_Times_20      =(uint8_t)20,
//    Multi_Times_25      =(uint8_t)25,
//    Multi_Times_30      =(uint8_t)30,
//    Multi_Times_35      =(uint8_t)35,
//    Multi_Times_40      =(uint8_t)40,
//    Multi_Times_45      =(uint8_t)45,
//    Multi_Times_50      =(uint8_t)50,
//    Multi_Times_60      =(uint8_t)60,
//    Multi_Times_70      =(uint8_t)70,
//    Multi_Times_80      =(uint8_t)80,
//    Multi_Times_90      =(uint8_t)90,
//    Multi_Times_99      =(uint8_t)99
//}_Multi_Times_TypeDef;


typedef enum
{
    Multi_Freq_1       =(uint8_t)1,  
    Multi_Freq_2       =(uint8_t)2,  
    Multi_Freq_3       =(uint8_t)3,  
    Multi_Freq_4       =(uint8_t)4,  
    Multi_Freq_5       =(uint8_t)5,  
    Multi_Freq_6       =(uint8_t)6,  
    Multi_Freq_7       =(uint8_t)7,  
    Multi_Freq_8       =(uint8_t)8,  
    Multi_Freq_9       =(uint8_t)9,  
    Multi_Freq_10      =(uint8_t)10, 
    Multi_Freq_11      =(uint8_t)11, 
    Multi_Freq_12      =(uint8_t)12, 
    Multi_Freq_13      =(uint8_t)13, 
    Multi_Freq_14      =(uint8_t)14, 
    Multi_Freq_15      =(uint8_t)15, 
    Multi_Freq_16      =(uint8_t)16, 
    Multi_Freq_17      =(uint8_t)17, 
    Multi_Freq_18      =(uint8_t)18, 
    Multi_Freq_19      =(uint8_t)19, 
    Multi_Freq_20      =(uint8_t)20, 
    Multi_Freq_25      =(uint8_t)25, 
    Multi_Freq_30      =(uint8_t)30, 
    Multi_Freq_35      =(uint8_t)35, 
    Multi_Freq_40      =(uint8_t)40, 
    Multi_Freq_45      =(uint8_t)45, 
    Multi_Freq_50      =(uint8_t)50, 
    Multi_Freq_60      =(uint8_t)60, 
    Multi_Freq_70      =(uint8_t)70, 
    Multi_Freq_80      =(uint8_t)80, 
    Multi_Freq_90      =(uint8_t)90, 
    Multi_Freq_100     =(uint8_t)100,
    Multi_Freq_110     =(uint8_t)110,
    Multi_Freq_120     =(uint8_t)120,
    Multi_Freq_130     =(uint8_t)130,
    Multi_Freq_140     =(uint8_t)140,
    Multi_Freq_150     =(uint8_t)150,
    Multi_Freq_160     =(uint8_t)160,
    Multi_Freq_170     =(uint8_t)170,
    Multi_Freq_180     =(uint8_t)180,
    Multi_Freq_190     =(uint8_t)190,
    Multi_Freq_199     =(uint8_t)199,
}_Multi_Freq_TypeDef;

typedef enum
{
    Multi_Times_0       =(uint8_t)0, 
    Multi_Times_1       =(uint8_t)1, 
    Multi_Times_2       =(uint8_t)2, 
    Multi_Times_3       =(uint8_t)3, 
    Multi_Times_4       =(uint8_t)4, 
    Multi_Times_5       =(uint8_t)5, 
    Multi_Times_6       =(uint8_t)6, 
    Multi_Times_7       =(uint8_t)7, 
    Multi_Times_8       =(uint8_t)8, 
    Multi_Times_9       =(uint8_t)9, 
    Multi_Times_10      =(uint8_t)10,
    Multi_Times_11      =(uint8_t)11,
    Multi_Times_12      =(uint8_t)12,
    Multi_Times_13      =(uint8_t)13,
    Multi_Times_14      =(uint8_t)14,
    Multi_Times_15      =(uint8_t)15,
    Multi_Times_16      =(uint8_t)16,
    Multi_Times_17      =(uint8_t)17,
    Multi_Times_18      =(uint8_t)18,
    Multi_Times_19      =(uint8_t)19,
    Multi_Times_20      =(uint8_t)20,
    Multi_Times_25      =(uint8_t)25,
    Multi_Times_30      =(uint8_t)30,
    Multi_Times_35      =(uint8_t)35,
    Multi_Times_40      =(uint8_t)40,
    Multi_Times_45      =(uint8_t)45,
    Multi_Times_50      =(uint8_t)50,
    Multi_Times_60      =(uint8_t)60,
    Multi_Times_70      =(uint8_t)70,
    Multi_Times_80      =(uint8_t)80,
    Multi_Times_90      =(uint8_t)90,
}_Multi_Times_TypeDef;

typedef enum
{
    M_Zoom_AUTO     =(uint8_t)0, 
    M_Zoom_24mm     =(uint8_t)24,    
    M_Zoom_28mm     =(uint8_t)28,
    M_Zoom_35mm     =(uint8_t)35,
    M_Zoom_50mm     =(uint8_t)50,
    M_Zoom_70mm     =(uint8_t)70,
    M_Zoom_80mm     =(uint8_t)80,
    M_Zoom_105mm    =(uint8_t)105,
    M_Zoom_135mm    =(uint8_t)135,
    M_Zoom_180mm    =(uint8_t)180,
    M_Zoom_200mm    =(uint8_t)200
}_M_Zoom_TypeDef;

typedef enum
{
    Beep_Mode_None,
    Beep_Mode_1,
    Beep_Mode_2,
    Beep_Mode_3,
    Beep_Mode_4,
    Beep_Mode_5,
    Beep_Mode_6,
    Beep_Mode_7,
}_Beep_Mode_TypeDef;

typedef enum
{
    Lamp_Temp_level_0,
    Lamp_Temp_level_1,
    Lamp_Temp_level_2,
}_Lamp_Temp_TypeDef;

typedef struct
{
    uint8_t Beep_Start          :1;
    uint8_t Beep_Key_Flag       :1;
    uint8_t Beep_CapFull_Flag   :1;
    uint8_t Beep_TrueFlash_Flag :1;
    uint8_t Beep_FailFlash_Flag :1;
    uint8_t Beep_PowerOff_Flag  :1;
    uint8_t Beep_BattLow_Flag   :1;
    uint8_t  Beep_LampTemp_Flag :1;
    uint16_t Beep_LampTemp_Cnt;
}_Beep_Flag;

typedef struct
{
  uint8_t Dis_Glitter_flag    :1;
  uint8_t Dis_Glitter_flag_en :1;
  uint8_t Beep_En_flag        :1;
  uint8_t frash_Start_Flag    :1;
  uint8_t frash_delay_Flag    :1;
  uint8_t frash_Multi_Flag    :1;
  uint8_t Sys_Power_Flag      :1;
  uint8_t Sys_Power_Flag_On   :1;
  uint8_t Sys_Power_Flag_Off  :1;
  uint8_t BK_Light_Flag       :1;
  uint8_t Cap_Full_Flag       :1;
  uint8_t Pilot_Set_flag      :1;
  uint8_t Change_Cap_En_flag  :1;
  uint8_t Low_Battery_flag    :1;
  uint8_t Lamp_Glitter_flag   :1;
  uint8_t KEY_SET_Change_flag :1;
  uint8_t KEY_UP_DOWN_Flag    :1;
  uint8_t KEY_UP_DOWN_Flag_Change:1;
  uint8_t KEY_AUTO_SET_Count;
  uint16_t Sys_Sleep_Count;
  uint8_t BK_Light_Count;
  uint8_t Multi_Times;
  uint8_t Cap_Value_Count;
  uint8_t Cap_Value_Beep_Count;
}_SysInfo;

extern _SysInfo   SysInfo;
extern _Beep_Flag BeepFlag;

extern _Sys_Mode_Typedef                Sys_Mode_Typedef;
extern _Key_SET_Function_Typedef        Key_SET_Function_Typedef;
extern _Flash_Output_Typedef            Flash_Output_Typedef;
//extern _Slave_Output_Typedef            Slave_Output_Typedef;
extern _Multi_Output_Typedef            Multi_Output_Typedef;
extern _TTL_Ev_TypeDef                  TTL_Ev_TypeDef;
extern _M_Zoom_TypeDef                  M_Zoom_TypeDef;
extern _Multi_Freq_TypeDef              Multi_Freq_TypeDef;
extern _Multi_Times_TypeDef             Multi_Times_TypeDef;
extern _Beep_Mode_TypeDef               Beep_Mode_TypeDef;

extern _Lamp_Temp_TypeDef               Lamp_Temp_TypeDef;

extern _M_Zoom_TypeDef                  T_Zoom_Group[11];
extern _Multi_Freq_TypeDef              T_Multi_Freq[41];
extern _Multi_Times_TypeDef             T_Multi_Times[31];


void Save_EepromData(void);
void Read_EepromData(void);
void Beep_Mode_Handle(void);
void Canon_Link_Camera(void);
void Camera_Link_Canon(void); 
void Slave_S2_TimeOut(void);
void GPIO_Pin_1_Callback(void);
#endif