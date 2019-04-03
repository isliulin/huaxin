#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm8l15x.h"

typedef enum 
{ 
    //   Key_Motor__         = (uint8_t)0, 
    Key_Motor_18        = (uint8_t)18,
    Key_Motor_24        = (uint8_t)24,
    Key_Motor_28        = (uint8_t)28,
    Key_Motor_35        = (uint8_t)35,
    Key_Motor_50        = (uint8_t)50,
    Key_Motor_70        = (uint8_t)70,
    Key_Motor_85        = (uint8_t)85,
    Key_Motor_105       = (uint8_t)105,
    Key_Motor_120       = (uint8_t)120, 
    Key_Motor_135       = (uint8_t)135,
    Key_Motor_180       = (uint8_t)180,
    Key_Motor_200       = (uint8_t)200,
    
    Transfer_Motor_18   = (uint8_t)18,
    Transfer_Motor_21   = (uint8_t)21,
    Transfer_Motor_22   = (uint8_t)22,
    Transfer_Motor_24   = (uint8_t)24,
    Transfer_Motor_25   = (uint8_t)25,
    Transfer_Motor_26   = (uint8_t)26,
    Transfer_Motor_28   = (uint8_t)28,
    Transfer_Motor_30   = (uint8_t)30,  
    Transfer_Motor_32   = (uint8_t)32,
    Transfer_Motor_33   = (uint8_t)33,   
    Transfer_Motor_34   = (uint8_t)34,
    Transfer_Motor_35   = (uint8_t)35,
    Transfer_Motor_38   = (uint8_t)38,
    Transfer_Motor_40   = (uint8_t)40,
    Transfer_Motor_42   = (uint8_t)42,
    Transfer_Motor_45   = (uint8_t)45,
    Transfer_Motor_48   = (uint8_t)48,
    Transfer_Motor_50   = (uint8_t)50,
    Transfer_Motor_52   = (uint8_t)52,
    Transfer_Motor_55   = (uint8_t)55,   
    Transfer_Motor_58   = (uint8_t)58,
    Transfer_Motor_60   = (uint8_t)60,
    Transfer_Motor_62   = (uint8_t)62,
    Transfer_Motor_66   = (uint8_t)66,
    Transfer_Motor_70   = (uint8_t)70,
    Transfer_Motor_75   = (uint8_t)75,
    Transfer_Motor_80   = (uint8_t)80,
    Transfer_Motor_85   = (uint8_t)85,
    Transfer_Motor_90   = (uint8_t)90,
    Transfer_Motor_92   = (uint8_t)92,
    Transfer_Motor_95   = (uint8_t)95,
    Transfer_Motor_98   = (uint8_t)98,
    Transfer_Motor_105  = (uint8_t)105,
    Transfer_Motor_112  = (uint8_t)112,
    Transfer_Motor_120  = (uint8_t)120, 
    Transfer_Motor_130  = (uint8_t)130,
    Transfer_Motor_135  = (uint8_t)135,
    Transfer_Motor_140  = (uint8_t)140,
    Transfer_Motor_180  = (uint8_t)180,
    Transfer_Motor_200  = (uint8_t)200  
} _Motor_Value_TypeDef;

//typedef enum
//{
//    Motor_Step_0        = (signed int)0,
//    Motor_Step_24       = (signed int)201,
//    Motor_Step_25       = (signed int)197,
//    Motor_Step_26       = (signed int)190,
//    Motor_Step_27       = (signed int)185,
//    Motor_Step_28       = (signed int)180,
//    Motor_Step_29       = (signed int)178,
//    Motor_Step_30       = (signed int)175,
//    Motor_Step_32       = (signed int)168,
//    Motor_Step_33       = (signed int)162,
//    Motor_Step_34       = (signed int)146,
//    Motor_Step_35       = (signed int)105,
//    Motor_Step_36       = (signed int)100,
//    Motor_Step_38       = (signed int)100,
//    Motor_Step_40       = (signed int)100,
//    Motor_Step_41       = (signed int)100,
//    Motor_Step_42       = (signed int)100,
//    Motor_Step_44       = (signed int)100,
//    Motor_Step_45       = (signed int)100,
//    Motor_Step_46       = (signed int)100, 
//    Motor_Step_48       = (signed int)100, 
//    Motor_Step_49       = (signed int)100, 
//    Motor_Step_50       = (signed int)100, 
//    Motor_Step_52       = (signed int)90, 
//    Motor_Step_55       = (signed int)87, 
//    Motor_Step_58       = (signed int)81,
//    Motor_Step_60       = (signed int)75, 
//    Motor_Step_62       = (signed int)68, 
//    Motor_Step_65       = (signed int)62,  
//    Motor_Step_66       = (signed int)56, 
//    Motor_Step_70       = (signed int)50, 
//    Motor_Step_75       = (signed int)43, 
//    Motor_Step_80       = (signed int)37, 
//    Motor_Step_85       = (signed int)31, 
//    Motor_Step_90       = (signed int)25, 
//    Motor_Step_92       = (signed int)18,
//    Motor_Step_95       = (signed int)15, 
//    Motor_Step_98       = (signed int)12, 
//    Motor_Step_105      = (signed int)-12,  
//    Motor_Step_112      = (signed int)-14, 
//    Motor_Step_120      = (signed int)-16, 
//    Motor_Step_130      = (signed int)-18,
//    Motor_Step_135      = (signed int)-20,
//    Motor_Step_140      = (signed int)-22,
//    Motor_Step_150      = (signed int)-25,
//    Motor_Step_170      = (signed int)-26,
//    Motor_Step_180      = (signed int)-28,
//    Motor_Step_200      = (signed int)-30
//}_Motor_Step_TypeDef;

#define MOTO_INCRE   30

typedef enum
{
    Motor_Step_0        = (signed int)0 + MOTO_INCRE,
    Motor_Step_24       = (signed int)201+ MOTO_INCRE,
    Motor_Step_25       = (signed int)197+ MOTO_INCRE,
    Motor_Step_26       = (signed int)190+ MOTO_INCRE,
    Motor_Step_27       = (signed int)185+ MOTO_INCRE,
    Motor_Step_28       = (signed int)180+ MOTO_INCRE,
    Motor_Step_29       = (signed int)178+ MOTO_INCRE,
    Motor_Step_30       = (signed int)175+ MOTO_INCRE,
    Motor_Step_32       = (signed int)168+ MOTO_INCRE,
    Motor_Step_33       = (signed int)162+ MOTO_INCRE,
    Motor_Step_34       = (signed int)146+ MOTO_INCRE,
    Motor_Step_35       = (signed int)105+ MOTO_INCRE,
    Motor_Step_36       = (signed int)100+ MOTO_INCRE,
    Motor_Step_38       = (signed int)100+ MOTO_INCRE,
    Motor_Step_40       = (signed int)100+ MOTO_INCRE,
    Motor_Step_41       = (signed int)100+ MOTO_INCRE,
    Motor_Step_42       = (signed int)100+ MOTO_INCRE,
    Motor_Step_44       = (signed int)100+ MOTO_INCRE,
    Motor_Step_45       = (signed int)100+ MOTO_INCRE,
    Motor_Step_46       = (signed int)100+ MOTO_INCRE, 
    Motor_Step_48       = (signed int)100+ MOTO_INCRE, 
    Motor_Step_49       = (signed int)100+ MOTO_INCRE, 
    Motor_Step_50       = (signed int)100+ MOTO_INCRE, 
    Motor_Step_52       = (signed int)90+ MOTO_INCRE, 
    Motor_Step_55       = (signed int)87+ MOTO_INCRE, 
    Motor_Step_58       = (signed int)81+ MOTO_INCRE,
    Motor_Step_60       = (signed int)75+ MOTO_INCRE, 
    Motor_Step_62       = (signed int)68+ MOTO_INCRE, 
    Motor_Step_65       = (signed int)62+ MOTO_INCRE,  
    Motor_Step_66       = (signed int)56+ MOTO_INCRE, 
    Motor_Step_70       = (signed int)50+ MOTO_INCRE, 
    Motor_Step_75       = (signed int)43+ MOTO_INCRE, 
    Motor_Step_80       = (signed int)37+ MOTO_INCRE, 
    Motor_Step_85       = (signed int)31+ MOTO_INCRE, 
    Motor_Step_90       = (signed int)25+ MOTO_INCRE, 
    Motor_Step_92       = (signed int)18+ MOTO_INCRE,
    Motor_Step_95       = (signed int)15+ MOTO_INCRE, 
    Motor_Step_98       = (signed int)12+ MOTO_INCRE, 
    Motor_Step_105      = (signed int)-12+ MOTO_INCRE,  
    Motor_Step_112      = (signed int)-14+ MOTO_INCRE, 
    Motor_Step_120      = (signed int)-16+ MOTO_INCRE, 
    Motor_Step_130      = (signed int)-18+ MOTO_INCRE,
    Motor_Step_135      = (signed int)-20+ MOTO_INCRE,
    Motor_Step_140      = (signed int)-22+ MOTO_INCRE,
    Motor_Step_150      = (signed int)-25+ MOTO_INCRE,
    Motor_Step_170      = (signed int)-26+ MOTO_INCRE,
    Motor_Step_180      = (signed int)-28+ MOTO_INCRE,
    Motor_Step_200      = (signed int)-30+ MOTO_INCRE,
}_Motor_Step_TypeDef;
    
typedef enum
{
    Moto_Zoom_18        =(uint8_t)18,
    Moto_Zoom_24        =(uint8_t)24,
    Moto_Zoom_28        =(uint8_t)28,
    Moto_Zoom_35        =(uint8_t)35,
    Moto_Zoom_50        =(uint8_t)50,
    Moto_Zoom_70        =(uint8_t)70,
    Moto_Zoom_105       =(uint8_t)105, 
    Moto_Zoom_135       =(uint8_t)135,
    Moto_Zoom_180       =(uint8_t)180,
    Moto_Zoom_200       =(uint8_t)200,
}_Moto_Zoom_TypeDef;

typedef enum
{
    Motor_Forward_1s4   =(uint8_t)0x00,
    Motor_Forward_2s4   =(uint8_t)0x01,
    Motor_Forward_3s4   =(uint8_t)0x02,
    Motor_Forward_4s4   =(uint8_t)0x03,
    Motor_Reverse_1s4   =(uint8_t)0x00,
    Motor_Reverse_2s4   =(uint8_t)0x01,
    Motor_Reverse_3s4   =(uint8_t)0x02,
    Motor_Reverse_4s4   =(uint8_t)0x03   
} _Motor_Period_TypeDef;

//typedef enum
//{
//    Camera_Zoom_18      =(uint8_t)0x21,  //
//    Camera_Zoom_24      =(uint8_t)0x27,  //
//    Camera_Zoom_28      =(uint8_t)0x2b,  //
//    Camera_Zoom_35      =(uint8_t)0x2f,  //
//    Camera_Zoom_50      =(uint8_t)0x37,  //
//    Camera_Zoom_70      =(uint8_t)0x3e,  //
//    Camera_Zoom_105     =(uint8_t)0x49,  //
//    Camera_Zoom_135     =(uint8_t)0x4e,  //
//    Camera_Zoom_180     =(uint8_t)0x55,  //
////    Camera_Zoom_200            =(uint8_t)0x56,  //
//}_Camera_Zoom_TypeDef;

typedef enum
{
    Camera_Zoom_18      =(uint8_t)15,  //
    Camera_Zoom_24      =(uint8_t)20,  //
    Camera_Zoom_28      =(uint8_t)25,  //
    Camera_Zoom_35      =(uint8_t)30,  //
    Camera_Zoom_50      =(uint8_t)45,  //
    Camera_Zoom_70      =(uint8_t)65,  //
    Camera_Zoom_80      =(uint8_t)75,  //
    Camera_Zoom_105     =(uint8_t)100,  //
    Camera_Zoom_135     =(uint8_t)130,  //
    Camera_Zoom_180     =(uint8_t)175,  //
    Camera_Zoom_200     =(uint8_t)195,  //
}_Canon_Data_Zoom_TypeDef;

typedef struct 
{  
    unsigned char fMotor_Reset          : 1;
    unsigned char fMotor_Run            : 1;
    unsigned char fMotor_Forward        : 1;
    unsigned char fMotor_Revise         : 1;
} _fMotor_State_TypeDef;

typedef struct 
{  
    unsigned char fMotor_M              : 1;
    unsigned char fMotor_M_Rem          : 1;
} _fMotor_Mode_TypeDef;

typedef enum 
{  
    Motor_Normal_Run           = (uint8_t)0x00,
    Motor_Reset_Check          = (uint8_t)0x01,
    Motor_Reset_Forward        = (uint8_t)0x02,
    Motor_Reset_Reverse        = (uint8_t)0x03,
    Motor_Reset_Error          = (uint8_t)0x04,
} _Motor_State_TypeDef;

typedef enum 
{  
    Motor_Move_End             = (uint8_t)0x00,
    Motor_Move_In              = (uint8_t)0x01,
}_Motor_Move_TypeDef;

typedef struct
{
    _fMotor_State_TypeDef       fMotor_State;
    signed int                 Motor_Step;
    unsigned char              Motor_Mem;
    uint32_t                    Motor_tickstart;
    _Motor_State_TypeDef        Motor_State;  
    _Motor_Period_TypeDef       Motor_Period;
    _Motor_Move_TypeDef         Motor_Move_State;   //运行状态，方便禁止充电动作
}_Motor_Manger_TypeDef;

typedef struct
{
    _fMotor_Mode_TypeDef        fMotor_Mode;
    _Motor_Value_TypeDef        Motor_Value;  
}_Motor_Contorl_TypeDef;

#define MOTO_RES_IN             GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3)

#define MOTO_IN1_ON             GPIOE->ODR |= GPIO_Pin_0;
#define MOTO_IN1_OFF            GPIOE->ODR &= (uint8_t)(~GPIO_Pin_0);
#define MOTO_ENA_ON             GPIOE->ODR |= GPIO_Pin_1;
#define MOTO_ENA_OFF            GPIOE->ODR &= (uint8_t)(~GPIO_Pin_1);
#define MOTO_IN2_ON             GPIOE->ODR |= GPIO_Pin_2;
#define MOTO_IN2_OFF            GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);

#define Motor_IN12_OFF          GPIOE->ODR &= 0xFA;

//#define Motor_Forward_1         GPIOE->ODR |= 0x04 //{MOTO_IN1_OFF;MOTO_IN2_ON;}
//#define Motor_Forward_2         GPIOE->ODR |= 0x05 //{MOTO_IN1_ON; MOTO_IN2_ON;}
//#define Motor_Forward_3         GPIOE->ODR |= 0x01 //{MOTO_IN1_ON; MOTO_IN2_OFF;}
//#define Motor_Forward_4         GPIOE->ODR |= 0x00 //{MOTO_IN1_OFF;MOTO_IN2_OFF;}
//#define Motor_Reverse_1         GPIOE->ODR |= 0x01 //{MOTO_IN1_ON; MOTO_IN2_OFF;}
//#define Motor_Reverse_2         GPIOE->ODR |= 0x05 //{MOTO_IN1_ON; MOTO_IN2_ON;}
//#define Motor_Reverse_3         GPIOE->ODR |= 0x04 //{MOTO_IN1_OFF;MOTO_IN2_ON;}
//#define Motor_Reverse_4         GPIOE->ODR |= 0x00 //{MOTO_IN1_OFF;MOTO_IN2_OFF;}

#define Motor_Forward_1        {MOTO_IN1_OFF;MOTO_IN2_ON;} 
#define Motor_Forward_2        {MOTO_IN1_ON; MOTO_IN2_ON;} 
#define Motor_Forward_3        {MOTO_IN1_ON; MOTO_IN2_OFF;}
#define Motor_Forward_4        {MOTO_IN1_OFF;MOTO_IN2_OFF;}
#define Motor_Reverse_1        {MOTO_IN1_ON; MOTO_IN2_OFF;}
#define Motor_Reverse_2        {MOTO_IN1_ON; MOTO_IN2_ON;} 
#define Motor_Reverse_3        {MOTO_IN1_OFF;MOTO_IN2_ON;} 
#define Motor_Reverse_4        {MOTO_IN1_OFF;MOTO_IN2_OFF;}


#define MOTOR_RESET_STEP        (signed int)210
#define MOTOR_RESET_EN          {Motor_Manger_TypeDef.Motor_State = Motor_Reset_Check;}
#define Motor_Res_TimeOut       3000               //马达复位动作超时时间

#define MOTOR_RESET_DIS         {MOTO_ENA_OFF; \
                                 Motor_Manger_TypeDef.Motor_Step = 0; \
                                 Motor_Manger_TypeDef.Motor_Mem = 0xFF; \
                                 Motor_Manger_TypeDef.Motor_State = Motor_Normal_Run;}

#define MOTOR_MOVE_STATE        MOTO_RES_IN
#define MOTOR_AT_RESET          SET

extern _Motor_Manger_TypeDef    Motor_Manger_TypeDef;
extern _Motor_Contorl_TypeDef   Motor_Contorl_TypeDef;

void Aper_Acquire(void);
void Moto_Operation(void);
void Motor_Contorl(void);
void Motor_Oper(void);

#endif