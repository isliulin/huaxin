#ifndef __CONFIG_H
#define __CONFIG_H


//260  270  280  290  300  310  320  330

//标准光圈值：
//1.0,//1.1,//1.2,//1.4,//1.6,//1.8,//2.0,//2.2,//2.5,//2.8,//3.2,//3.5,//4.0,//4.5,//5.0,//5.6,//6.3,//7.1,
//8.0,//9.0,//10,//11,//13,//14,//16,//18,//20,//22,//25,//29,//32,//36,//40,//45,//51,//57,//64,

//typedef enum //闪光档位
//{
//    Flash_Time_512      = (uint16_t)36,//
//    Flash_Time_512s3    = (uint16_t)38,//
//    Flash_Time_512s7    = (uint16_t)40,//
//    
//    Flash_Time_256      = (uint16_t)42,//
//    Flash_Time_256s3    = (uint16_t)46,//
//    Flash_Time_256s7    = (uint16_t)50,//
//    
//    Flash_Time_128      = (uint16_t)54,//
//    Flash_Time_128s3    = (uint16_t)60,//
//    Flash_Time_128s7    = (uint16_t)67,//
//    
//    Flash_Time_64       = (uint16_t)72,//
//    Flash_Time_64s3     = (uint16_t)82,//
//    Flash_Time_64s7     = (uint16_t)90,//
//    
//    Flash_Time_32       = (uint16_t)99,//
//    Flash_Time_32s3     = (uint16_t)106,//108,//
//    Flash_Time_32s7     = (uint16_t)120,//118,//120,//
//    
//    Flash_Time_16       = (uint16_t)138,//130,//138,//
//    Flash_Time_16s3     = (uint16_t)160,//152,//160,//
//    Flash_Time_16s7     = (uint16_t)188,//180,//188,//
//    
//    Flash_Time_8        = (uint16_t)220,//
//    Flash_Time_8s3      = (uint16_t)255,//
//    Flash_Time_8s7      = (uint16_t)310,//
//  
//    Flash_Time_4        = (uint16_t)365,//
//    Flash_Time_4s3      = (uint16_t)485,//
//    Flash_Time_4s7      = (uint16_t)655,//
//    
//    Flash_Time_2        = (uint16_t)860, //
//    Flash_Time_2s3      = (uint16_t)1350,//
//    Flash_Time_2s7      = (uint16_t)2100,//
//    
//    Flash_Time_1        = (uint16_t)6000,//42
//    
//}_Flash_Time_TypeDef;

#define Flash_Time(x,y)   Flash_Time_##x##_V##y

typedef enum //闪光档位
{
    Flash_Time_512_V320      = (uint16_t)20,//20//34  
    Flash_Time_512s3_V320    = (uint16_t)22,//23//42  
    Flash_Time_512s7_V320    = (uint16_t)24,//25//50  
    
    Flash_Time_256_V320      = (uint16_t)26,//26//58  
    Flash_Time_256s3_V320    = (uint16_t)28,//27//64  
    Flash_Time_256s7_V320    = (uint16_t)30,//28//68  
    
    Flash_Time_128_V320      = (uint16_t)33,////26,//27//30//
    Flash_Time_128s3_V320    = (uint16_t)37,////29,//30//35//
    Flash_Time_128s7_V320   = (uint16_t)41,////33//34//38//
    
    Flash_Time_64_V320       = (uint16_t)44,//37//38//
    Flash_Time_64s3_V320     = (uint16_t)48,//41,//42//45//
    Flash_Time_64s7_V320     = (uint16_t)58,//52//48//
    
    Flash_Time_32_V320       = (uint16_t)61,//55//50//
    Flash_Time_32s3_V320     = (uint16_t)68,//60//140 
    Flash_Time_32s7_V320     = (uint16_t)82,//78//70//
    
    Flash_Time_16_V320       = (uint16_t)98,//90//175 
    Flash_Time_16s3_V320     = (uint16_t)110,//100//20
    Flash_Time_16s7_V320     = (uint16_t)148,//110//23
    
    Flash_Time_8_V320        = (uint16_t)160,//150//28
    Flash_Time_8s3_V320      = (uint16_t)200,//160//33
    Flash_Time_8s7_V320      = (uint16_t)235,//215//17
    
    Flash_Time_4_V320        = (uint16_t)275,//260//23
    Flash_Time_4s3_V320      = (uint16_t)400,//325//57
    Flash_Time_4s7_V320      = (uint16_t)510,//420//81
    
    Flash_Time_2_V320        = (uint16_t)600,//560//12
    Flash_Time_2s3_V320      = (uint16_t)900,//890//18
    Flash_Time_2s7_V320      = (uint16_t)1700,//1600//
    
    Flash_Time_1_V320        = (uint16_t)5530,//5530//   
}_Flash_Time_TypeDef_V320;

typedef enum //闪光档位
{
    Flash_Time_512_V310      = (uint16_t)20,//20//34  
    Flash_Time_512s3_V310    = (uint16_t)22,//23//42  
    Flash_Time_512s7_V310    = (uint16_t)24,//25//50  
    
    Flash_Time_256_V310      = (uint16_t)26,//26//58  
    Flash_Time_256s3_V310    = (uint16_t)28,//27//64  
    Flash_Time_256s7_V310    = (uint16_t)30,//28//68  
    
    Flash_Time_128_V310      = (uint16_t)33,////26,//27//30//
    Flash_Time_128s3_V310    = (uint16_t)37,////29,//30//35//
    Flash_Time_128s7_V310    = (uint16_t)41,////33//34//38//
    
    Flash_Time_64_V310       = (uint16_t)44,//37//38//
    Flash_Time_64s3_V310     = (uint16_t)48,//41,//42//45//
    Flash_Time_64s7_V310     = (uint16_t)58,//52//48//
    
    Flash_Time_32_V310       = (uint16_t)61,//55//50//
    Flash_Time_32s3_V310     = (uint16_t)68,//60//140 
    Flash_Time_32s7_V310     = (uint16_t)82,//78//70//
    
    Flash_Time_16_V310       = (uint16_t)98,//90//175 
    Flash_Time_16s3_V310    = (uint16_t)110,//100//20
    Flash_Time_16s7_V310     = (uint16_t)148,//110//23
    
    Flash_Time_8_V310        = (uint16_t)160,//150//28
    Flash_Time_8s3_V310      = (uint16_t)200,//160//33
    Flash_Time_8s7_V310      = (uint16_t)235,//215//17
    
    Flash_Time_4_V310        = (uint16_t)275,//260//23
    Flash_Time_4s3_V310      = (uint16_t)400,//325//57
    Flash_Time_4s7_V310      = (uint16_t)510,//420//81
    
    Flash_Time_2_V310        = (uint16_t)600,//560//12
    Flash_Time_2s3_V310      = (uint16_t)900,//890//18
    Flash_Time_2s7_V310      = (uint16_t)1700,//1600//
    
    Flash_Time_1_V310        = (uint16_t)5530,//5530//   
}_Flash_Time_TypeDef_V310;

typedef enum //闪光档位
{
    Flash_Time_512_V300      = (uint16_t)25,//20//34  
    Flash_Time_512s3_V300    = (uint16_t)30,//23//42  
    Flash_Time_512s7_V300    = (uint16_t)36,//25//50  
    
    Flash_Time_256_V300      = (uint16_t)40,//26//58  
    Flash_Time_256s3_V300    = (uint16_t)47,//27//64  
    Flash_Time_256s7_V300    = (uint16_t)51,//28//68  
    
    Flash_Time_128_V300      = (uint16_t)57,////26,//27//30//
    Flash_Time_128s3_V300    = (uint16_t)61,////29,//30//35//
    Flash_Time_128s7_V300    = (uint16_t)70,////33//34//38//
    
    Flash_Time_64_V300       = (uint16_t)79,//37//38//
    Flash_Time_64s3_V300     = (uint16_t)86,//41,//42//45//
    Flash_Time_64s7_V300     = (uint16_t)99,//52//48//
    
    Flash_Time_32_V300       = (uint16_t)117,//55//50//
    Flash_Time_32s3_V300     = (uint16_t)124,//60//140 
    Flash_Time_32s7_V300     = (uint16_t)145,//78//70//
    
    Flash_Time_16_V300       = (uint16_t)186,//90//175 
    Flash_Time_16s3_V300     = (uint16_t)213,//100//20
    Flash_Time_16s7_V300     = (uint16_t)258,//110//23
    
    Flash_Time_8_V300        = (uint16_t)313,//150//28
    Flash_Time_8s3_V300      = (uint16_t)372,//160//33
    Flash_Time_8s7_V300      = (uint16_t)465,//215//17
    
    Flash_Time_4_V300        = (uint16_t)570,//260//23
    Flash_Time_4s3_V300      = (uint16_t)707,//325//57
    Flash_Time_4s7_V300      = (uint16_t)942,//420//81
    
    Flash_Time_2_V300        = (uint16_t)1495,//560//12
    Flash_Time_2s3_V300      = (uint16_t)2047,//890//18
    Flash_Time_2s7_V300      = (uint16_t)4016,//1600//
    
    Flash_Time_1_V300        = (uint16_t)5530,//5530//   
}_Flash_Time_TypeDef_V300;

typedef enum //闪光档位
{
    Flash_Time_512_V290      = (uint16_t)27,//20//34  
    Flash_Time_512s3_V290    = (uint16_t)32,//23//42  
    Flash_Time_512s7_V290    = (uint16_t)38,//25//50  
    
    Flash_Time_256_V290      = (uint16_t)43,//26//58  
    Flash_Time_256s3_V290    = (uint16_t)50,//27//64  
    Flash_Time_256s7_V290    = (uint16_t)54,//28//68  
    
    Flash_Time_128_V290      = (uint16_t)61,////26,//27//30//
    Flash_Time_128s3_V290    = (uint16_t)66,////29,//30//35//
    Flash_Time_128s7_V290    = (uint16_t)75,////33//34//38//
    
    Flash_Time_64_V290       = (uint16_t)86,//37//38//
    Flash_Time_64s3_V290     = (uint16_t)93,//41,//42//45//
    Flash_Time_64s7_V290     = (uint16_t)108,//52//48//
    
    Flash_Time_32_V290       = (uint16_t)128,//55//50//
    Flash_Time_32s3_V290     = (uint16_t)136,//60//140 
    Flash_Time_32s7_V290     = (uint16_t)158,//78//70//
    
    Flash_Time_16_V290       = (uint16_t)207,//90//175 
    Flash_Time_16s3_V290     = (uint16_t)237,//100//20
    Flash_Time_16s7_V290     = (uint16_t)287,//110//23
    
    Flash_Time_8_V290        = (uint16_t)336,//150//28
    Flash_Time_8s3_V290      = (uint16_t)400,//160//33
    Flash_Time_8s7_V290      = (uint16_t)499,//215//17
    
    Flash_Time_4_V290        = (uint16_t)647,//260//23
    Flash_Time_4s3_V290     = (uint16_t)802,//325//57
    Flash_Time_4s7_V290      = (uint16_t)1069,//420//81
    
    Flash_Time_2_V290        = (uint16_t)1916,//560//12
    Flash_Time_2s3_V290      = (uint16_t)2624,//890//18
    Flash_Time_2s7_V290      = (uint16_t)5200,//1600//
    
    Flash_Time_1_V290        = (uint16_t)5530,//5530//   
}_Flash_Time_TypeDef_V290;

typedef enum //闪光档位
{
    Flash_Time_512_V280      = (uint16_t)29,//20//34  
    Flash_Time_512s3_V280    = (uint16_t)35,//23//42  
    Flash_Time_512s7_V280    = (uint16_t)41,//25//50  
    
    Flash_Time_256_V280      = (uint16_t)46,//26//58  
    Flash_Time_256s3_V280    = (uint16_t)53,//27//64  
    Flash_Time_256s7_V280    = (uint16_t)58,//28//68  
    
    Flash_Time_128_V280      = (uint16_t)66,////26,//27//30//
    Flash_Time_128s3_V280    = (uint16_t)71,////29,//30//35//
    Flash_Time_128s7_V280    = (uint16_t)81,////33//34//38//
    
    Flash_Time_64_V280       = (uint16_t)93,//37//38//
    Flash_Time_64s3_V280     = (uint16_t)101,//41,//42//45//
    Flash_Time_64s7_V280     = (uint16_t)116,//52//48//
    
    Flash_Time_32_V280       = (uint16_t)139,//55//50//
    Flash_Time_32s3_V280     = (uint16_t)147,//60//140 
    Flash_Time_32s7_V280     = (uint16_t)172,//78//70//
    
    Flash_Time_16_V280       = (uint16_t)223,//90//175 
    Flash_Time_16s3_V280     = (uint16_t)255,//100//20
    Flash_Time_16s7_V280     = (uint16_t)310,//110//23
    
    Flash_Time_8_V280        = (uint16_t)369,//150//28
    Flash_Time_8s3_V280      = (uint16_t)439,//160//33
    Flash_Time_8s7_V280      = (uint16_t)548,//215//17
    
    Flash_Time_4_V280        = (uint16_t)761,//260//23
    Flash_Time_4s3_V280      = (uint16_t)944,//325//57
    Flash_Time_4s7_V280      = (uint16_t)1257,//420//81
    
    Flash_Time_2_V280        = (uint16_t)2395,//560//12
    Flash_Time_2s3_V280      = (uint16_t)3279,//890//18
    Flash_Time_2s7_V280      = (uint16_t)5200,//1600//
    
    Flash_Time_1_V280        = (uint16_t)5530,//5530//   
}_Flash_Time_TypeDef_V280;

typedef enum //闪光档位
{
    Flash_Time_512_V270      = (uint16_t)31,//20//34  
    Flash_Time_512s3_V270    = (uint16_t)37,//23//42  
    Flash_Time_512s7_V270    = (uint16_t)44,//25//50  
    
    Flash_Time_256_V270      = (uint16_t)49,//26//58  
    Flash_Time_256s3_V270    = (uint16_t)57,//27//64  
    Flash_Time_256s7_V270    = (uint16_t)63,//28//68  
    
    Flash_Time_128_V270      = (uint16_t)71,////26,//27//30//
    Flash_Time_128s3_V270    = (uint16_t)76,////29,//30//35//
    Flash_Time_128s7_V270    = (uint16_t)87,////33//34//38//
    
    Flash_Time_64_V270       = (uint16_t)100,//37//38//
    Flash_Time_64s3_V270     = (uint16_t)108,//41,//42//45//
    Flash_Time_64s7_V270     = (uint16_t)125,//52//48//
    
    Flash_Time_32_V270       = (uint16_t)150,//55//50//
    Flash_Time_32s3_V270    = (uint16_t)159,//60//140 
    Flash_Time_32s7_V270     = (uint16_t)185,//78//70//
    
    Flash_Time_16_V270       = (uint16_t)239,//90//175 
    Flash_Time_16s3_V270     = (uint16_t)274,//100//20
    Flash_Time_16s7_V270     = (uint16_t)332,//110//23
    
    Flash_Time_8_V270        = (uint16_t)425,//150//28
    Flash_Time_8s3_V270      = (uint16_t)506,//160//33
    Flash_Time_8s7_V270      = (uint16_t)631,//215//17
    
    Flash_Time_4_V270        = (uint16_t)895,//260//23
    Flash_Time_4s3_V270      = (uint16_t)1110,//325//57
    Flash_Time_4s7_V270      = (uint16_t)1479,//420//81
    
    Flash_Time_2_V270        = (uint16_t)3421,//560//12
    Flash_Time_2s3_V270      = (uint16_t)4684,//890//18
    Flash_Time_2s7_V270      = (uint16_t)5200,//1600//
    
    Flash_Time_1_V270        = (uint16_t)5530,//5530//   
}_Flash_Time_TypeDef_V270;




#define LIGHT_TTL_T1_ZL   95

typedef enum //相机码对应的档位
{
    Light_Low_Level_Data_512         = (uint8_t)(LIGHT_TTL_T1_ZL+0 ),
    Light_Low_Level_Data_512s3       = (uint8_t)(LIGHT_TTL_T1_ZL+2 ),
    Light_Low_Level_Data_512s7       = (uint8_t)(LIGHT_TTL_T1_ZL+5 ),
    Light_Low_Level_Data_256         = (uint8_t)(LIGHT_TTL_T1_ZL+8 ),
    Light_Low_Level_Data_256s3       = (uint8_t)(LIGHT_TTL_T1_ZL+10),
    Light_Low_Level_Data_256s7       = (uint8_t)(LIGHT_TTL_T1_ZL+13),
    Light_Low_Level_Data_128         = (uint8_t)(LIGHT_TTL_T1_ZL+16),
    Light_Low_Level_Data_128s3       = (uint8_t)(LIGHT_TTL_T1_ZL+18),
    Light_Low_Level_Data_128s7       = (uint8_t)(LIGHT_TTL_T1_ZL+21),
    Light_Low_Level_Data_64          = (uint8_t)(LIGHT_TTL_T1_ZL+24),
    Light_Low_Level_Data_64s3        = (uint8_t)(LIGHT_TTL_T1_ZL+26),
    Light_Low_Level_Data_64s7        = (uint8_t)(LIGHT_TTL_T1_ZL+29),
    Light_Low_Level_Data_32          = (uint8_t)(LIGHT_TTL_T1_ZL+32),
    Light_Low_Level_Data_32s3        = (uint8_t)(LIGHT_TTL_T1_ZL+34),
    Light_Low_Level_Data_32s7        = (uint8_t)(LIGHT_TTL_T1_ZL+37),
    Light_Low_Level_Data_16          = (uint8_t)(LIGHT_TTL_T1_ZL+40),
    Light_Low_Level_Data_16s3        = (uint8_t)(LIGHT_TTL_T1_ZL+42),
    Light_Low_Level_Data_16s7        = (uint8_t)(LIGHT_TTL_T1_ZL+45),
    Light_Low_Level_Data_8           = (uint8_t)(LIGHT_TTL_T1_ZL+48),
    Light_Low_Level_Data_8s3         = (uint8_t)(LIGHT_TTL_T1_ZL+50),
    Light_Low_Level_Data_8s7         = (uint8_t)(LIGHT_TTL_T1_ZL+53),
    Light_Low_Level_Data_4           = (uint8_t)(LIGHT_TTL_T1_ZL+56),
    Light_Low_Level_Data_4s3         = (uint8_t)(LIGHT_TTL_T1_ZL+58),
    Light_Low_Level_Data_4s7         = (uint8_t)(LIGHT_TTL_T1_ZL+61),
    Light_Low_Level_Data_2           = (uint8_t)(LIGHT_TTL_T1_ZL+64),
    Light_Low_Level_Data_2s3         = (uint8_t)(LIGHT_TTL_T1_ZL+66),
    Light_Low_Level_Data_2s7         = (uint8_t)(LIGHT_TTL_T1_ZL+69),
    Light_Low_Level_Data_1           = (uint8_t)(LIGHT_TTL_T1_ZL+72),
}_Light_Level_Data_TypeDef; 


#define LIGHT_CMD_PER_ZL    -10

typedef enum //CMD从属对应预闪的档位
{    
    Light_CMD_Pre_Data_F1_0        =  (uint16_t)(LIGHT_CMD_PER_ZL+  820),
    Light_CMD_Pre_Data_F1_1        =  (uint16_t)(LIGHT_CMD_PER_ZL+  850),
    Light_CMD_Pre_Data_F1_2        =  (uint16_t)(LIGHT_CMD_PER_ZL+  870),
    Light_CMD_Pre_Data_F1_4        =  (uint16_t)(LIGHT_CMD_PER_ZL+  900),
    Light_CMD_Pre_Data_F1_6        =  (uint16_t)(LIGHT_CMD_PER_ZL+  930),
    Light_CMD_Pre_Data_F1_8        =  (uint16_t)(LIGHT_CMD_PER_ZL+  950),
    Light_CMD_Pre_Data_F2_0        =  (uint16_t)(LIGHT_CMD_PER_ZL+  980),
    Light_CMD_Pre_Data_F2_2        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1010),
    Light_CMD_Pre_Data_F2_5        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1030),
    Light_CMD_Pre_Data_F2_8        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1060),
    Light_CMD_Pre_Data_F3_2        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1090),
    Light_CMD_Pre_Data_F3_5        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1110),
    Light_CMD_Pre_Data_F4_0        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1140),
    Light_CMD_Pre_Data_F4_5        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1170),
    Light_CMD_Pre_Data_F5_0        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1190),
    Light_CMD_Pre_Data_F5_6        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1220),
    Light_CMD_Pre_Data_F6_3        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1240),
    Light_CMD_Pre_Data_F7_1        =  (uint16_t)(LIGHT_CMD_PER_ZL+ 1250),
}_Light_CMD_Pre_Data_TypeDef; 

#define LIGHT_CMD_T1_ZL    760

typedef enum //CMD从属对应正闪的档位
{   
    Light_CMD_Level_Data_512                 = (int16_t)(LIGHT_CMD_T1_ZL+ 0   ), 
    Light_CMD_Level_Data_512s3               = (int16_t)(LIGHT_CMD_T1_ZL+ 20  ), 
    Light_CMD_Level_Data_512s7               = (int16_t)(LIGHT_CMD_T1_ZL+ 50  ), 
    
    Light_CMD_Level_Data_256                 = (int16_t)(LIGHT_CMD_T1_ZL+ 80  ), 
    Light_CMD_Level_Data_256s3               = (int16_t)(LIGHT_CMD_T1_ZL+ 100 ), 
    Light_CMD_Level_Data_256s7               = (int16_t)(LIGHT_CMD_T1_ZL+ 130 ), 
    
    Light_CMD_Level_Data_128                 = (int16_t)(LIGHT_CMD_T1_ZL+ 160 ), 
    Light_CMD_Level_Data_128s3               = (int16_t)(LIGHT_CMD_T1_ZL+ 180 ), 
    Light_CMD_Level_Data_128s7               = (int16_t)(LIGHT_CMD_T1_ZL+ 210 ), 
    
    Light_CMD_Level_Data_64                  = (int16_t)(LIGHT_CMD_T1_ZL+ 240 ), 
    Light_CMD_Level_Data_64s3                = (int16_t)(LIGHT_CMD_T1_ZL+ 260 ), 
    Light_CMD_Level_Data_64s7                = (int16_t)(LIGHT_CMD_T1_ZL+ 290 ), 
    
    Light_CMD_Level_Data_32                  = (int16_t)(LIGHT_CMD_T1_ZL+ 320 ), 
    Light_CMD_Level_Data_32s3                = (int16_t)(LIGHT_CMD_T1_ZL+ 340 ), 
    Light_CMD_Level_Data_32s7                = (int16_t)(LIGHT_CMD_T1_ZL+ 370 ), 
    
    Light_CMD_Level_Data_16                  = (int16_t)(LIGHT_CMD_T1_ZL+ 400 ), 
    Light_CMD_Level_Data_16s3                = (int16_t)(LIGHT_CMD_T1_ZL+ 420 ), 
    Light_CMD_Level_Data_16s7                = (int16_t)(LIGHT_CMD_T1_ZL+ 450 ), 
    
    Light_CMD_Level_Data_8                   = (int16_t)(LIGHT_CMD_T1_ZL+ 480 ), 
    Light_CMD_Level_Data_8s3                 = (int16_t)(LIGHT_CMD_T1_ZL+ 500 ), 
    Light_CMD_Level_Data_8s7                 = (int16_t)(LIGHT_CMD_T1_ZL+ 530 ), 
    
    Light_CMD_Level_Data_4                   = (int16_t)(LIGHT_CMD_T1_ZL+ 560 ), 
    Light_CMD_Level_Data_4s3                 = (int16_t)(LIGHT_CMD_T1_ZL+ 580 ), 
    Light_CMD_Level_Data_4s7                 = (int16_t)(LIGHT_CMD_T1_ZL+ 610 ), 
    
    Light_CMD_Level_Data_2                   = (int16_t)(LIGHT_CMD_T1_ZL+ 640 ), 
    Light_CMD_Level_Data_2s3                 = (int16_t)(LIGHT_CMD_T1_ZL+ 660 ), 
    Light_CMD_Level_Data_2s7                 = (int16_t)(LIGHT_CMD_T1_ZL+ 690 ), 
    
    Light_CMD_Level_Data_1                   = (int16_t)(LIGHT_CMD_T1_ZL+ 720 ), 
    
}_Light_CMD_Level_Data_TypeDef;  


//#define Flash_Per_Time  100
#define Flash_Cmd_Time  Flash_Time_128


#define Lamp_Temp_1        0
#define Lamp_Temp_2        3300    //3300    //58度
#define Lamp_Temp_3        3528    //70度

#endif