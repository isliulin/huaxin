





































































  

 


























 

 

 


























 

 



 


 
       
       
      
       
    
    
    
     



























 


 
 
 
 








 



 


 





 

 

 
 


 

 

 

 



 





 

 
typedef   signed char     int8_t;
typedef   signed short    int16_t;
typedef   signed long     int32_t;

 
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned long     uint32_t;

 

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;


typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus, BitStatus, BitAction;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;




 



 

 
 
 

 


 
typedef struct GPIO_struct
{
  volatile uint8_t ODR;  
  volatile uint8_t IDR;  
  volatile uint8_t DDR;  
  volatile uint8_t CR1;  
  volatile uint8_t CR2;  
}
GPIO_TypeDef;



 


 
 



 
typedef struct RTC_struct
{
  volatile uint8_t TR1;  
  volatile uint8_t TR2;  
  volatile uint8_t TR3;  

  uint8_t RESERVED0;

  volatile uint8_t DR1;  
  volatile uint8_t DR2;  
  volatile uint8_t DR3;  

  uint8_t RESERVED1;

  volatile uint8_t CR1;  
  volatile uint8_t CR2;  
  volatile uint8_t CR3;  

  uint8_t RESERVED2;

  volatile uint8_t ISR1;  
  volatile uint8_t ISR2;  

  uint8_t RESERVED3;
  uint8_t RESERVED4;

  volatile uint8_t SPRERH;  
  volatile uint8_t SPRERL;  
  volatile uint8_t APRER;   

  uint8_t RESERVED5;

  volatile uint8_t WUTRH;  
  volatile uint8_t WUTRL;  

  uint8_t RESERVED6;

  volatile uint8_t SSRH;  
  volatile uint8_t SSRL;  

  volatile uint8_t WPR;   

  volatile uint8_t SHIFTRH;  
  volatile uint8_t SHIFTRL;  

  volatile uint8_t ALRMAR1;  
  volatile uint8_t ALRMAR2;  
  volatile uint8_t ALRMAR3;  
  volatile uint8_t ALRMAR4;  

  uint8_t RESERVED7[4];

  volatile uint8_t ALRMASSRH;  
  volatile uint8_t ALRMASSRL;  
  volatile uint8_t ALRMASSMSKR;  

  uint8_t RESERVED8[3];

  volatile uint8_t CALRH;  
  volatile uint8_t CALRL;  

  volatile uint8_t TCR1;  
  volatile uint8_t TCR2;  
}
RTC_TypeDef;



 














 



 

 

 

 

 

 

 

 


 



 


 


 

 

 


 

 

 

 

 

 

 


 

 

 

 


 



 



 
typedef struct CSSLSE_struct
{
  volatile uint8_t CSR;  
}
CSSLSE_TypeDef;



 



 



 

 



 
 


 

typedef struct BEEP_struct
{
  volatile uint8_t CSR1;  
  uint8_t RSERVED1;
  uint8_t RESERVED2;
  volatile uint8_t CSR2;  
}
BEEP_TypeDef;



 



 



 





 

 



 

typedef struct CFG_struct
{
  volatile uint8_t GCR;  
}
CFG_TypeDef;



 




 



 




 
 



 

typedef struct SYSCFG_struct
{
  volatile uint8_t RMPCR3;  
  volatile uint8_t RMPCR1;  
  volatile uint8_t RMPCR2;  
}
SYSCFG_TypeDef;



 



 



 

 


 





 
 



 
typedef struct CLK_struct
{
  volatile uint8_t CKDIVR;       
  volatile uint8_t CRTCR;   
  volatile uint8_t ICKCR;      
  volatile uint8_t PCKENR1;   
  volatile uint8_t PCKENR2;   
  volatile uint8_t CCOR;        
  volatile uint8_t ECKCR;      
  volatile uint8_t SCSR;      
  volatile uint8_t SWR;       
  volatile uint8_t SWCR;      
  volatile uint8_t CSSR;      
  volatile uint8_t CBEEPR;      
  volatile uint8_t HSICALR;      
  volatile uint8_t HSITRIMR;  
  volatile uint8_t HSIUNLCKR;  
  volatile uint8_t REGCSR;   
  volatile uint8_t PCKENR3;   
}
CLK_TypeDef;



 


 



 




















 
 



 

typedef struct COMP_struct
{
  volatile uint8_t CSR1;    
  volatile uint8_t CSR2;    
  volatile uint8_t CSR3;    
  volatile uint8_t CSR4;    
  volatile uint8_t CSR5;    
}
COMP_TypeDef;




 



 



 

 

 

 

 

 



 

 



 
typedef struct EXTI_struct
{
  volatile uint8_t CR1;            
  volatile uint8_t CR2;            
  volatile uint8_t CR3;            
  volatile uint8_t SR1;            
  volatile uint8_t SR2;            
  volatile uint8_t CONF1;          
  uint8_t RESERVED[4];   
  volatile uint8_t CR4;            
  volatile uint8_t CONF2;          
}
EXTI_TypeDef;



 




 



 
 

 

 

 

 

 



 

 



 
typedef struct FLASH_struct
{
  volatile uint8_t CR1;         
  volatile uint8_t CR2;         
  volatile uint8_t PUKR;        
  volatile uint8_t DUKR;        
  volatile uint8_t IAPSR;       
}
FLASH_TypeDef;



 




 



 








 

 



 
typedef struct I2C_struct
{
  volatile uint8_t CR1;     
  volatile uint8_t CR2;     
  volatile uint8_t FREQR;   
  volatile uint8_t OARL;    
  volatile uint8_t OARH;    
  volatile uint8_t OAR2;    
  volatile uint8_t DR;      
  volatile uint8_t SR1;     
  volatile uint8_t SR2;     
  volatile uint8_t SR3;     
  volatile uint8_t ITR;     
  volatile uint8_t CCRL;    
  volatile uint8_t CCRH;    
  volatile uint8_t TRISER;  
  volatile uint8_t PECR;    
}
I2C_TypeDef;



 



 



 


















 

 



 
typedef struct IRTIM_struct
{
  volatile uint8_t CR;  
}
IRTIM_TypeDef;


 




 



 
 



 

 



 
typedef struct ITC_struct
{
  volatile uint8_t ISPR1;  
  volatile uint8_t ISPR2;  
  volatile uint8_t ISPR3;  
  volatile uint8_t ISPR4;  
  volatile uint8_t ISPR5;  
  volatile uint8_t ISPR6;  
  volatile uint8_t ISPR7;  
  volatile uint8_t ISPR8;  
}
ITC_TypeDef;



 


 

 



 
typedef struct IWDG_struct
{
  volatile uint8_t KR;   
  volatile uint8_t PR;   
  volatile uint8_t RLR;  
}
IWDG_TypeDef;



 


 
 




 


 
typedef struct WFE_struct
{
  volatile uint8_t CR1;   
  volatile uint8_t CR2;   
  volatile uint8_t CR3;   
  volatile uint8_t CR4;   
}
WFE_TypeDef;


 


 



 



 








 

 


 
typedef struct OPT_struct
{
  volatile uint8_t ROP;         
  uint8_t  RESERVED1;
  volatile uint8_t UBC;         
  uint8_t  RESERVED2;
  uint8_t  RESERVED3;
  uint8_t  RESERVED4;
  uint8_t  RESERVED5;
  volatile uint8_t PCODESIZE;
  volatile uint8_t WDG;         
  volatile uint8_t XTSTARTUP;   
  volatile uint8_t BOR;         
}
OPT_TypeDef;



 
 



 
typedef struct RST_struct
{
  volatile uint8_t CR;   
  volatile uint8_t SR;   

}
RST_TypeDef;


 



 




 



 



 
 



 
typedef struct PWR_struct
{
  volatile uint8_t CSR1;           
  volatile uint8_t CSR2;           

}
PWR_TypeDef;


 



 




 



 




 
 



 
typedef struct RI_struct
{
  uint8_t RESERVED;
  volatile uint8_t ICR1;    
  volatile uint8_t ICR2;    
  volatile uint8_t IOIR1;   
  volatile uint8_t IOIR2;   
  volatile uint8_t IOIR3;   
  volatile uint8_t IOCMR1;  
  volatile uint8_t IOCMR2;  
  volatile uint8_t IOCMR3;  
  volatile uint8_t IOSR1;   
  volatile uint8_t IOSR2;   
  volatile uint8_t IOSR3;   
  volatile uint8_t IOGCR;   
  volatile uint8_t ASCR1;   
  volatile uint8_t ASCR2;   
  volatile uint8_t RCR;     
  uint8_t RESERVED1[16];
  volatile uint8_t CR;      
  volatile uint8_t IOMR1;    
  volatile uint8_t IOMR2;    
  volatile uint8_t IOMR3;    
  volatile uint8_t IOMR4;    
  volatile uint8_t IOIR4;   
  volatile uint8_t IOCMR4;  
  volatile uint8_t IOSR4;   
}RI_TypeDef;


 



 









 



 


















 
 



 
typedef struct SPI_struct
{
  volatile uint8_t CR1;     
  volatile uint8_t CR2;     
  volatile uint8_t CR3;     
  volatile uint8_t SR;      
  volatile uint8_t DR;      
  volatile uint8_t CRCPR;   
  volatile uint8_t RXCRCR;  
  volatile uint8_t TXCRCR;  
}
SPI_TypeDef;



 




 



 






 

 



 
typedef struct TIM1_struct
{
  volatile uint8_t CR1;    
  volatile uint8_t CR2;    
  volatile uint8_t SMCR;   
  volatile uint8_t ETR;    
  volatile uint8_t DER;    
  volatile uint8_t IER;    
  volatile uint8_t SR1;    
  volatile uint8_t SR2;    
  volatile uint8_t EGR;    
  volatile uint8_t CCMR1;  
  volatile uint8_t CCMR2;  
  volatile uint8_t CCMR3;  
  volatile uint8_t CCMR4;  
  volatile uint8_t CCER1;  
  volatile uint8_t CCER2;  
  volatile uint8_t CNTRH;  
  volatile uint8_t CNTRL;  
  volatile uint8_t PSCRH;  
  volatile uint8_t PSCRL;  
  volatile uint8_t ARRH;   
  volatile uint8_t ARRL;   
  volatile uint8_t RCR;    
  volatile uint8_t CCR1H;  
  volatile uint8_t CCR1L;  
  volatile uint8_t CCR2H;  
  volatile uint8_t CCR2L;  
  volatile uint8_t CCR3H;  
  volatile uint8_t CCR3L;  
  volatile uint8_t CCR4H;  
  volatile uint8_t CCR4L;  
  volatile uint8_t BKR;    
  volatile uint8_t DTR;    
  volatile uint8_t OISR;   
  volatile uint8_t DCR1;   
  volatile uint8_t DCR2;   
  volatile uint8_t DMAR;   
}
TIM1_TypeDef;



 



 



 
 
 
 
 
 
 
 
 
 
 

 
 
 
 
 
 
 
 
 
 
 
 
 

 

 

 




 
 



 
typedef struct TIM_struct
{
  volatile uint8_t CR1;     
  volatile uint8_t CR2;     
  volatile uint8_t SMCR;    
  volatile uint8_t ETR;     
  volatile uint8_t DER;     
  volatile uint8_t IER;     
  volatile uint8_t SR1;     
  volatile uint8_t SR2;     
  volatile uint8_t EGR;     
  volatile uint8_t CCMR1;   
  volatile uint8_t CCMR2;   
  volatile uint8_t CCER1;   
  volatile uint8_t CNTRH;   
  volatile uint8_t CNTRL;   
  volatile uint8_t PSCR;    
  volatile uint8_t ARRH;    
  volatile uint8_t ARRL;    
  volatile uint8_t CCR1H;   
  volatile uint8_t CCR1L;   
  volatile uint8_t CCR2H;   
  volatile uint8_t CCR2L;   
  volatile uint8_t BKR;     
  volatile uint8_t OISR;    
}
TIM_TypeDef;



 








 



 
 

 


 


 

 

 

 

 

 

 


 

 

 

 


 

 


 

 


 


 



 
typedef struct TIM4_struct
{
  volatile uint8_t CR1;    
  volatile uint8_t CR2;    
  volatile uint8_t SMCR;   
  volatile uint8_t DER;    
  volatile uint8_t IER;    
  volatile uint8_t SR1;    
  volatile uint8_t EGR;    
  volatile uint8_t CNTR;   
  volatile uint8_t PSCR;   
  volatile uint8_t ARR;    
}
TIM4_TypeDef;


 



 



 
 

 

 

 

 

 

 

 

 

 


 

 



 
typedef struct USART_struct
{
  volatile uint8_t SR;    
  volatile uint8_t DR;    
  volatile uint8_t BRR1;  
  volatile uint8_t BRR2;  
  volatile uint8_t CR1;   
  volatile uint8_t CR2;   
  volatile uint8_t CR3;   
  volatile uint8_t CR4;   
  volatile uint8_t CR5;   
  volatile uint8_t GTR;   
  volatile uint8_t PSCR;  
}
USART_TypeDef;




 



 



 









 
 



 
typedef struct ADC_struct
{
  volatile uint8_t   CR1;       
  volatile uint8_t   CR2;       
  volatile uint8_t   CR3;       
  volatile uint8_t   SR;        
  volatile uint8_t   DRH;       
  volatile uint8_t   DRL;       
  volatile uint8_t   HTRH;      
  volatile uint8_t   HTRL;      
  volatile uint8_t   LTRH;      
  volatile uint8_t   LTRL;      
  volatile uint8_t   SQR[4];    
  volatile uint8_t   TRIGR[4];  
}
ADC_TypeDef;



 




 



 













 
 
 



 
typedef struct DAC_struct
{
  volatile uint8_t  CH1CR1;    
  volatile uint8_t  CH1CR2;    
  volatile uint8_t  CH2CR1;    
  volatile uint8_t  CH2CR2;    
  volatile uint8_t  SWTRIGR;  
  volatile uint8_t  SR;       

   
  volatile uint8_t  RESERVED0[2];
  volatile uint8_t  CH1RDHRH;  
  volatile uint8_t  CH1RDHRL;  
  volatile uint8_t  RESERVED1[2];
  volatile uint8_t  CH1LDHRH;  
  volatile uint8_t  CH1LDHRL;  
  volatile uint8_t  RESERVED2[2];
  volatile uint8_t  CH1DHR8;  

   
  volatile uint8_t  RESERVED3[3];
  volatile uint8_t  CH2RDHRH;  
  volatile uint8_t  CH2RDHRL;  
  volatile uint8_t  RESERVED4[2];
  volatile uint8_t  CH2LDHRH;  
  volatile uint8_t  CH2LDHRL;  
  volatile uint8_t  RESERVED5[2];
  volatile uint8_t  CH2DHR8;  


   
  volatile uint8_t  RESERVED6[3];
  volatile uint8_t  DCH1RDHRH;  
  volatile uint8_t  DCH1RDHRL;  
  volatile uint8_t  DCH2RDHRH;  
  volatile uint8_t  DCH2RDHRL;  
  volatile uint8_t  DCH1LDHRH;  
  volatile uint8_t  DCH1LDHRL;  
  volatile uint8_t  DCH2LDHRH;  
  volatile uint8_t  DCH2LDHRL;  
  volatile uint8_t  DCH1DHR8;  
  volatile uint8_t  DCH2DHR8;  

   
  volatile uint8_t  RESERVED7[2];
  volatile uint8_t  CH1DORH;  
  volatile uint8_t  CH1DORL;  
  volatile uint8_t  RESERVED8[2];
  volatile uint8_t  CH2DORH;  
  volatile uint8_t  CH2DORL;  
}
DAC_TypeDef;



 


 



 

 

 

 

 

 

 

 

 

 

 

 



 

 



 
typedef struct DMA_struct
{
  volatile uint8_t  GCSR;      
  volatile uint8_t  GIR1;      
}
DMA_TypeDef;


 
typedef struct DMA_Channel_struct
{
  volatile uint8_t  CCR;        
  volatile uint8_t  CSPR;       
  volatile uint8_t  CNBTR;      
  volatile uint8_t  CPARH;      
  volatile uint8_t  CPARL;      
  volatile uint8_t  CM0EAR;     
  volatile uint8_t  CM0ARH;     
  volatile uint8_t  CM0ARL;     
}
DMA_Channel_TypeDef;


 



 


 



 


 

 


 


 


 


 


 
 


 




 
 



 
typedef struct WWDG_struct
{
  volatile uint8_t CR;  
  volatile uint8_t WR;  
}
WWDG_TypeDef;



 




 



 






 

 


 
typedef struct LCD_struct
{
  volatile uint8_t CR1;       
  volatile uint8_t CR2;       
  volatile uint8_t CR3;       
  volatile uint8_t FRQ;       
  volatile uint8_t PM[6];     
  uint8_t RESERVED1[2];   
  volatile uint8_t RAM[22];   
  uint8_t RESERVED2[13];  
  volatile uint8_t CR4;       
}
LCD_TypeDef;


 




 



 









 

 
 


 
typedef struct AES_struct
{
  volatile uint8_t CR;       
  volatile uint8_t SR;       
  volatile uint8_t DINR;     
  volatile uint8_t DOUTR;    
}
AES_TypeDef;



 


 



 





 
 
 
 

 
 
 




























 

 

 


























 

 



 



 

 

 


























 

 

 



 
  


  
 



 




 
typedef enum
{
  ADC_Channel_0           = ((uint16_t)0x0301),  
  ADC_Channel_1           = ((uint16_t)0x0302),  
  ADC_Channel_2           = ((uint16_t)0x0304),  
  ADC_Channel_3           = ((uint16_t)0x0308),  
  ADC_Channel_4           = ((uint16_t)0x0310),  
  ADC_Channel_5           = ((uint16_t)0x0320),  
  ADC_Channel_6           = ((uint16_t)0x0340),  
  ADC_Channel_7           = ((uint16_t)0x0380),  

  ADC_Channel_8           = ((uint16_t)0x0201),  
  ADC_Channel_9           = ((uint16_t)0x0202),  
  ADC_Channel_10          = ((uint16_t)0x0204),  
  ADC_Channel_11          = ((uint16_t)0x0208),  
  ADC_Channel_12          = ((uint16_t)0x0210),  
  ADC_Channel_13          = ((uint16_t)0x0220),  
  ADC_Channel_14          = ((uint16_t)0x0240),  
  ADC_Channel_15          = ((uint16_t)0x0280),  

  ADC_Channel_16          = ((uint16_t)0x0101),  
  ADC_Channel_17          = ((uint16_t)0x0102),  
  ADC_Channel_18          = ((uint16_t)0x0104),  
  ADC_Channel_19          = ((uint16_t)0x0108),  
  ADC_Channel_20          = ((uint16_t)0x0110),  
  ADC_Channel_21          = ((uint16_t)0x0120),  
  ADC_Channel_22          = ((uint16_t)0x0140),  
  ADC_Channel_23          = ((uint16_t)0x0180),  

  ADC_Channel_24          = ((uint16_t)0x0001),  
  ADC_Channel_25          = ((uint16_t)0x0002),  
  ADC_Channel_26          = ((uint16_t)0x0004),  
  ADC_Channel_27          = ((uint16_t)0x0008),  

  ADC_Channel_Vrefint     = ((uint16_t)0x0010),  
  ADC_Channel_TempSensor  = ((uint16_t)0x0020),  

   
  ADC_Channel_00To07      = ((uint16_t)0x03FF),  
  ADC_Channel_08To15      = ((uint16_t)0x02FF),  
  ADC_Channel_16To23      = ((uint16_t)0x01FF),  
  ADC_Channel_24To27      = ((uint16_t)0x00FF)   

}ADC_Channel_TypeDef;



 
  


 
typedef enum
{
  ADC_ConversionMode_Single     = ((uint8_t)0x00),  
  ADC_ConversionMode_Continuous = ((uint8_t)0x04)   
}ADC_ConversionMode_TypeDef;



 
  


 
typedef enum
{
  ADC_Resolution_12Bit = ((uint8_t)0x00),  
  ADC_Resolution_10Bit = ((uint8_t)0x20),  
  ADC_Resolution_8Bit  = ((uint8_t)0x40),  
  ADC_Resolution_6Bit  = ((uint8_t)0x60)   
}ADC_Resolution_TypeDef;



 
  


 
typedef enum
{
  ADC_Prescaler_1 = ((uint8_t)0x00),  
  ADC_Prescaler_2 = ((uint8_t)0x80)   
}ADC_Prescaler_TypeDef;



 
  


 
typedef enum
{
  ADC_ExtTRGSensitivity_Rising  = ((uint8_t)0x20),  
  ADC_ExtTRGSensitivity_Falling = ((uint8_t)0x40),  
  ADC_ExtTRGSensitivity_All     = ((uint8_t)0x60)   
}ADC_ExtTRGSensitivity_TypeDef;



 
  


 
typedef enum
{
  ADC_ExtEventSelection_None     = ((uint8_t)0x00),  
  ADC_ExtEventSelection_Trigger1 = ((uint8_t)0x08),  
  ADC_ExtEventSelection_Trigger2 = ((uint8_t)0x10),  
  ADC_ExtEventSelection_Trigger3 = ((uint8_t)0x18)   
}ADC_ExtEventSelection_TypeDef;



 
  


 
typedef enum
{
  ADC_Group_SlowChannels = ((uint8_t)0x00),  
  ADC_Group_FastChannels = ((uint8_t)0x01)  
 
}ADC_Group_TypeDef;



 
  


 
typedef enum
{
  ADC_SamplingTime_4Cycles   = ((uint8_t)0x00),  
  ADC_SamplingTime_9Cycles   = ((uint8_t)0x01),  
  ADC_SamplingTime_16Cycles  = ((uint8_t)0x02),  
  ADC_SamplingTime_24Cycles  = ((uint8_t)0x03),  
  ADC_SamplingTime_48Cycles  = ((uint8_t)0x04),  
  ADC_SamplingTime_96Cycles  = ((uint8_t)0x05),  
  ADC_SamplingTime_192Cycles = ((uint8_t)0x06),  
  ADC_SamplingTime_384Cycles = ((uint8_t)0x07)   
}ADC_SamplingTime_TypeDef;



 
  


 
typedef enum
{
  ADC_AnalogWatchdogSelection_Channel0  = ((uint8_t)0x00),  
  ADC_AnalogWatchdogSelection_Channel1  = ((uint8_t)0x01),  
  ADC_AnalogWatchdogSelection_Channel2  = ((uint8_t)0x02),  
  ADC_AnalogWatchdogSelection_Channel3  = ((uint8_t)0x03),  
  ADC_AnalogWatchdogSelection_Channel4  = ((uint8_t)0x04),  
  ADC_AnalogWatchdogSelection_Channel5  = ((uint8_t)0x05),  
  ADC_AnalogWatchdogSelection_Channel6  = ((uint8_t)0x06),  
  ADC_AnalogWatchdogSelection_Channel7  = ((uint8_t)0x07),  
  ADC_AnalogWatchdogSelection_Channel8  = ((uint8_t)0x08),  
  ADC_AnalogWatchdogSelection_Channel9  = ((uint8_t)0x09),  
  ADC_AnalogWatchdogSelection_Channel10 = ((uint8_t)0x0A),  
  ADC_AnalogWatchdogSelection_Channel11 = ((uint8_t)0x0B),  
  ADC_AnalogWatchdogSelection_Channel12 = ((uint8_t)0x0C),  
  ADC_AnalogWatchdogSelection_Channel13 = ((uint8_t)0x0D),  
  ADC_AnalogWatchdogSelection_Channel14 = ((uint8_t)0x0E),  
  ADC_AnalogWatchdogSelection_Channel15 = ((uint8_t)0x0F),  
  ADC_AnalogWatchdogSelection_Channel16 = ((uint8_t)0x10),  
  ADC_AnalogWatchdogSelection_Channel17 = ((uint8_t)0x11),  
  ADC_AnalogWatchdogSelection_Channel18 = ((uint8_t)0x12),  
  ADC_AnalogWatchdogSelection_Channel19 = ((uint8_t)0x13),  
  ADC_AnalogWatchdogSelection_Channel20 = ((uint8_t)0x14),  
  ADC_AnalogWatchdogSelection_Channel21 = ((uint8_t)0x15),  
  ADC_AnalogWatchdogSelection_Channel22 = ((uint8_t)0x16),  
  ADC_AnalogWatchdogSelection_Channel23 = ((uint8_t)0x17),  

  ADC_AnalogWatchdogSelection_Channel24  = ((uint8_t)0x18),  
  ADC_AnalogWatchdogSelection_Channel25  = ((uint8_t)0x19),  
  ADC_AnalogWatchdogSelection_Channel26  = ((uint8_t)0x1A),  
  ADC_AnalogWatchdogSelection_Channel27  = ((uint8_t)0x1B),  

  ADC_AnalogWatchdogSelection_Vrefint    = ((uint8_t)0x1C),  
  ADC_AnalogWatchdogSelection_TempSensor = ((uint8_t)0x1D)   
}ADC_AnalogWatchdogSelection_TypeDef;



 
  


 
typedef enum
{
  ADC_IT_EOC  = ((uint8_t)0x08),  
  ADC_IT_AWD  = ((uint8_t)0x10),  
  ADC_IT_OVER = ((uint8_t)0x80)   
}ADC_IT_TypeDef;



 
  


 
typedef enum
{
  ADC_FLAG_EOC  = ((uint8_t)0x01),  
  ADC_FLAG_AWD  = ((uint8_t)0x02),  
  ADC_FLAG_OVER = ((uint8_t)0x04)   
}ADC_FLAG_TypeDef;



 



 
   
 


 



 

 
  
 
void ADC_DeInit(ADC_TypeDef* ADCx);

  
void ADC_Init(ADC_TypeDef* ADCx,
              ADC_ConversionMode_TypeDef ADC_ConversionMode,
              ADC_Resolution_TypeDef ADC_Resolution,
              ADC_Prescaler_TypeDef ADC_Prescaler);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_SoftwareStartConv(ADC_TypeDef* ADCx);
void ADC_ExternalTrigConfig(ADC_TypeDef* ADCx,
                            ADC_ExtEventSelection_TypeDef ADC_ExtEventSelection,
                            ADC_ExtTRGSensitivity_TypeDef ADC_ExtTRGSensitivity);
                            
 
void ADC_AnalogWatchdogChannelSelect(ADC_TypeDef* ADCx,
                                     ADC_AnalogWatchdogSelection_TypeDef ADC_AnalogWatchdogSelection);
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold,
                                        uint16_t LowThreshold);
void ADC_AnalogWatchdogConfig(ADC_TypeDef* ADCx,
                              ADC_AnalogWatchdogSelection_TypeDef ADC_AnalogWatchdogSelection,
                              uint16_t HighThreshold,
                              uint16_t LowThreshold);

 
void ADC_TempSensorCmd(FunctionalState NewState);
void ADC_VrefintCmd(FunctionalState NewState);

                               
void ADC_ChannelCmd(ADC_TypeDef* ADCx, ADC_Channel_TypeDef ADC_Channels,
                    FunctionalState NewState);

void ADC_SamplingTimeConfig(ADC_TypeDef* ADCx, ADC_Group_TypeDef ADC_GroupChannels,
                            ADC_SamplingTime_TypeDef ADC_SamplingTime);
void ADC_SchmittTriggerConfig(ADC_TypeDef* ADCx, ADC_Channel_TypeDef ADC_Channels,
                              FunctionalState NewState);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);



 
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);

 
void ADC_ITConfig(ADC_TypeDef* ADCx, 
                  ADC_IT_TypeDef ADC_IT, 
                  FunctionalState NewState);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, ADC_FLAG_TypeDef ADC_FLAG);
void ADC_ClearFlag(ADC_TypeDef* ADCx, ADC_FLAG_TypeDef ADC_FLAG);
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, ADC_IT_TypeDef ADC_IT);
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, ADC_IT_TypeDef ADC_IT);




 



 

 


























 

 

 



 
  


  

 



 



 
typedef enum
{
  AES_Operation_Encryp            = (uint8_t)0x00,  
  AES_Operation_KeyDeriv          = (uint8_t)0x02,  
  AES_Operation_Decryp            = (uint8_t)0x04,  
  AES_Operation_KeyDerivAndDecryp = (uint8_t)0x06   
} AES_Operation_TypeDef;



 
  


 
typedef enum
{
  AES_FLAG_CCF      = (uint8_t)0x01,   
  AES_FLAG_RDERR    = (uint8_t)0x02,   
  AES_FLAG_WRERR    = (uint8_t)0x04    
}AES_FLAG_TypeDef;

   


 
  


 
typedef enum
{
  AES_IT_CCIE   = (uint16_t)0x20,  
  AES_IT_ERRIE  = (uint16_t)0x40   
}AES_IT_TypeDef;

   


 
  


 
typedef enum
{
  AES_DMATransfer_InOut = (uint8_t) 0x80 
 
}
AES_DMATransfer_TypeDef;



 
  


 
  
 
 
 
   
void AES_DeInit(void);

 
void AES_OperationModeConfig(AES_Operation_TypeDef AES_Operation);
void AES_Cmd(FunctionalState NewState);

 
void AES_WriteSubData(uint8_t Data);
void AES_WriteSubKey(uint8_t Key);
uint8_t AES_ReadSubData(void);
uint8_t AES_ReadSubKey(void);

 
void AES_DMAConfig(AES_DMATransfer_TypeDef AES_DMATransfer, FunctionalState NewState);

 
void AES_ITConfig(AES_IT_TypeDef AES_IT, FunctionalState NewState);
FlagStatus AES_GetFlagStatus(AES_FLAG_TypeDef AES_FLAG);
void AES_ClearFlag(AES_FLAG_TypeDef AES_FLAG);
ITStatus AES_GetITStatus(AES_IT_TypeDef AES_IT);
void AES_ClearITPendingBit(AES_IT_TypeDef AES_IT);




 



 

 


























 

 

 



 
  


  
  
 


 
  


 
typedef enum {
  BEEP_Frequency_1KHz = (uint8_t)0x00,   
  BEEP_Frequency_2KHz = (uint8_t)0x40,   
  BEEP_Frequency_4KHz = (uint8_t)0x80    
} BEEP_Frequency_TypeDef;

 


 



 
      
 


 




 

 


 



 

 

   
void BEEP_DeInit(void);

 
void BEEP_Init(BEEP_Frequency_TypeDef BEEP_Frequency);
void BEEP_Cmd(FunctionalState NewState);

 
void BEEP_LSClockToTIMConnectCmd(FunctionalState NewState);
void BEEP_LSICalibrationConfig(uint32_t LSIFreqHz);





 



 

 


























 


 

 



 



  
 



 
  


 
typedef enum {
  CLK_HSE_OFF    = (uint8_t)0x00,  
  CLK_HSE_ON     = (uint8_t)0x01,  
  CLK_HSE_Bypass = (uint8_t)0x11   
} CLK_HSE_TypeDef;



 
                              


 
typedef enum {
  CLK_LSE_OFF    = (uint8_t)0x00,  
  CLK_LSE_ON     = (uint8_t)0x04,  
  CLK_LSE_Bypass = (uint8_t)0x24   
} CLK_LSE_TypeDef;



 



 
typedef enum {
  CLK_SYSCLKSource_HSI = (uint8_t)0x01,  
  CLK_SYSCLKSource_LSI = (uint8_t)0x02,  
  CLK_SYSCLKSource_HSE = (uint8_t)0x04,  
  CLK_SYSCLKSource_LSE = (uint8_t)0x08   
} CLK_SYSCLKSource_TypeDef;



 



 
typedef enum {
  CLK_CCOSource_Off = (uint8_t)0x00,  
  CLK_CCOSource_HSI = (uint8_t)0x02,  
  CLK_CCOSource_LSI = (uint8_t)0x04,  
  CLK_CCOSource_HSE = (uint8_t)0x08,  
  CLK_CCOSource_LSE = (uint8_t)0x10   
} CLK_CCOSource_TypeDef;



 
     


 
typedef enum {
  CLK_CCODiv_1  = (uint8_t)0x00,  
  CLK_CCODiv_2  = (uint8_t)0x20,  
  CLK_CCODiv_4  = (uint8_t)0x40,  
  CLK_CCODiv_8  = (uint8_t)0x60,  
  CLK_CCODiv_16 = (uint8_t)0x80,  
  CLK_CCODiv_32 = (uint8_t)0xA0,  
  CLK_CCODiv_64 = (uint8_t)0xC0   
} CLK_CCODiv_TypeDef;



 
  


 
typedef enum {
  CLK_BEEPCLKSource_Off = (uint8_t)0x00,  
  CLK_BEEPCLKSource_LSI = (uint8_t)0x02,  
  CLK_BEEPCLKSource_LSE = (uint8_t)0x04   
} CLK_BEEPCLKSource_TypeDef;



 
  


 
typedef enum {
  CLK_RTCCLKSource_Off = (uint8_t)0x00,  
  CLK_RTCCLKSource_HSI = (uint8_t)0x02,  
  CLK_RTCCLKSource_LSI = (uint8_t)0x04,  
  CLK_RTCCLKSource_HSE = (uint8_t)0x08,  
  CLK_RTCCLKSource_LSE = (uint8_t)0x10   
} CLK_RTCCLKSource_TypeDef;



 
  


 
typedef enum {
  CLK_RTCCLKDiv_1  = (uint8_t)0x00,  
  CLK_RTCCLKDiv_2  = (uint8_t)0x20,  
  CLK_RTCCLKDiv_4  = (uint8_t)0x40,  
  CLK_RTCCLKDiv_8  = (uint8_t)0x60,  
  CLK_RTCCLKDiv_16 = (uint8_t)0x80,  
  CLK_RTCCLKDiv_32 = (uint8_t)0xA0,  
  CLK_RTCCLKDiv_64 = (uint8_t)0xC0   
} CLK_RTCCLKDiv_TypeDef;



 
  


 






         
typedef enum {
  CLK_Peripheral_TIM2    = (uint8_t)0x00,  
  CLK_Peripheral_TIM3    = (uint8_t)0x01,  
  CLK_Peripheral_TIM4    = (uint8_t)0x02,  
  CLK_Peripheral_I2C1    = (uint8_t)0x03,  
  CLK_Peripheral_SPI1    = (uint8_t)0x04,  
  CLK_Peripheral_USART1  = (uint8_t)0x05,  
  CLK_Peripheral_BEEP    = (uint8_t)0x06,  
  CLK_Peripheral_DAC     = (uint8_t)0x07,  
  CLK_Peripheral_ADC1    = (uint8_t)0x10,  
  CLK_Peripheral_TIM1    = (uint8_t)0x11,  
  CLK_Peripheral_RTC     = (uint8_t)0x12,  
  CLK_Peripheral_LCD     = (uint8_t)0x13,  
  CLK_Peripheral_DMA1    = (uint8_t)0x14,  
  CLK_Peripheral_COMP    = (uint8_t)0x15,  
  CLK_Peripheral_BOOTROM = (uint8_t)0x17, 
  CLK_Peripheral_AES     = (uint8_t)0x20,  
  CLK_Peripheral_TIM5    = (uint8_t)0x21,  
  CLK_Peripheral_SPI2    = (uint8_t)0x22,  
  CLK_Peripheral_USART2  = (uint8_t)0x23,  
  CLK_Peripheral_USART3  = (uint8_t)0x24,   
  CLK_Peripheral_CSSLSE  = (uint8_t)0x25    
} CLK_Peripheral_TypeDef;



 
     


 
typedef enum {
  CLK_SYSCLKDiv_1   = (uint8_t)0x00,  
  CLK_SYSCLKDiv_2   = (uint8_t)0x01,  
  CLK_SYSCLKDiv_4   = (uint8_t)0x02,  
  CLK_SYSCLKDiv_8   = (uint8_t)0x03,  
  CLK_SYSCLKDiv_16  = (uint8_t)0x04,  
  CLK_SYSCLKDiv_32  = (uint8_t)0x05,  
  CLK_SYSCLKDiv_64  = (uint8_t)0x06,  
  CLK_SYSCLKDiv_128 = (uint8_t)0x07   
} CLK_SYSCLKDiv_TypeDef;



 
        


 












 
typedef enum {
  CLK_FLAG_RTCSWBSY    = (uint8_t)0x00,  
  CLK_FLAG_HSIRDY      = (uint8_t)0x11,  
  CLK_FLAG_LSIRDY      = (uint8_t)0x13,  
  CLK_FLAG_CCOBSY      = (uint8_t)0x20,  
  CLK_FLAG_HSERDY      = (uint8_t)0x31,  
  CLK_FLAG_LSERDY      = (uint8_t)0x33,  
  CLK_FLAG_SWBSY       = (uint8_t)0x40,  
  CLK_FLAG_AUX         = (uint8_t)0x51,  
  CLK_FLAG_CSSD        = (uint8_t)0x53,  
  CLK_FLAG_BEEPSWBSY   = (uint8_t)0x60,  
  CLK_FLAG_EEREADY     = (uint8_t)0x77,  
  CLK_FLAG_EEBUSY      = (uint8_t)0x76,  
  CLK_FLAG_LSEPD       = (uint8_t)0x75,  
  CLK_FLAG_HSEPD       = (uint8_t)0x74,  
  CLK_FLAG_LSIPD       = (uint8_t)0x73,  
  CLK_FLAG_HSIPD       = (uint8_t)0x72,  
  CLK_FLAG_REGREADY    = (uint8_t)0x70,   
  CLK_FLAG_LSECSSF     = (uint8_t)0x83,   
  CLK_FLAG_RTCCLKSWF   = (uint8_t)0x84    
}CLK_FLAG_TypeDef;



 



 
typedef enum {
  CLK_IT_CSSD     = (uint8_t)0x0C,   
  CLK_IT_SWIF     = (uint8_t)0x1C,   
  CLK_IT_LSECSSF  = (uint8_t)0x2C    
}CLK_IT_TypeDef;
                       


 



 
typedef enum {
  CLK_Halt_BEEPRunning = (uint8_t)0x40,  
  CLK_Halt_FastWakeup  = (uint8_t)0x20,  
  CLK_Halt_SlowWakeup  = (uint8_t)0x10   
}
CLK_Halt_TypeDef;



 
  


 
    
 
 
 

 
void CLK_DeInit(void);

 
void CLK_HSICmd(FunctionalState NewState);
void CLK_AdjustHSICalibrationValue(uint8_t CLK_HSICalibrationValue);
void CLK_LSICmd(FunctionalState NewState);
void CLK_HSEConfig(CLK_HSE_TypeDef CLK_HSE);
void CLK_LSEConfig(CLK_LSE_TypeDef CLK_LSE);
void CLK_ClockSecuritySystemEnable(void);
void CLK_ClockSecuritySytemDeglitchCmd(FunctionalState NewState);
void CLK_CCOConfig(CLK_CCOSource_TypeDef CLK_CCOSource, CLK_CCODiv_TypeDef CLK_CCODiv);

 
void CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_TypeDef CLK_SYSCLKSource);
CLK_SYSCLKSource_TypeDef CLK_GetSYSCLKSource(void);
uint32_t CLK_GetClockFreq(void);
void CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_TypeDef CLK_SYSCLKDiv);
void CLK_SYSCLKSourceSwitchCmd(FunctionalState NewState);

 
void CLK_RTCClockConfig(CLK_RTCCLKSource_TypeDef CLK_RTCCLKSource, CLK_RTCCLKDiv_TypeDef CLK_RTCCLKDiv);
void CLK_BEEPClockConfig(CLK_BEEPCLKSource_TypeDef CLK_BEEPCLKSource);
void CLK_PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState);

 
void CLK_LSEClockSecuritySystemEnable(void);
void CLK_RTCCLKSwitchOnLSEFailureEnable(void);

 
void CLK_HaltConfig(CLK_Halt_TypeDef CLK_Halt, FunctionalState NewState);
void CLK_MainRegulatorCmd(FunctionalState NewState);

 
void CLK_ITConfig(CLK_IT_TypeDef CLK_IT, FunctionalState NewState);
FlagStatus CLK_GetFlagStatus(CLK_FLAG_TypeDef CLK_FLAG);
void CLK_ClearFlag(void);
ITStatus CLK_GetITStatus(CLK_IT_TypeDef CLK_IT);
void CLK_ClearITPendingBit(CLK_IT_TypeDef CLK_IT);





 
  


 

 


























 

 


 



 



  
  
 



 



 
typedef enum
{
  COMP_Selection_COMP1 = ((uint8_t)0x01),  
  COMP_Selection_COMP2 = ((uint8_t)0x02)   
}COMP_Selection_TypeDef;



 
  


 
typedef enum
{
  COMP_Edge_Falling         = ((uint8_t)0x01),  
  COMP_Edge_Rising          = ((uint8_t)0x02),  
  COMP_Edge_Rising_Falling  = ((uint8_t)0x03)   
}COMP_Edge_TypeDef;



 
  


 
typedef enum
{
  COMP_InvertingInput_IO         = ((uint8_t)0x08),  
  COMP_InvertingInput_VREFINT    = ((uint8_t)0x10),  
  COMP_InvertingInput_3_4VREFINT = ((uint8_t)0x18),  
  COMP_InvertingInput_1_2VREFINT = ((uint8_t)0x20),  
  COMP_InvertingInput_1_4VREFINT = ((uint8_t)0x28),  
  COMP_InvertingInput_DAC1       = ((uint8_t)0x30),   
  COMP_InvertingInput_DAC2       = ((uint8_t)0x38)   
}COMP_InvertingInput_Typedef;



 
  


 
typedef enum
{
  COMP_OutputSelect_TIM2IC2       = ((uint8_t)0x00),  
  COMP_OutputSelect_TIM3IC2       = ((uint8_t)0x40),  
  COMP_OutputSelect_TIM1BRK       = ((uint8_t)0x80),  
  COMP_OutputSelect_TIM1OCREFCLR  = ((uint8_t)0xC0)   
}COMP_OutputSelect_Typedef;



 
  


 
typedef enum
{
  COMP_Speed_Slow  = ((uint8_t)0x00),  
  COMP_Speed_Fast  = ((uint8_t)0x04)   
}COMP_Speed_TypeDef;



 
  


 
typedef enum
{
  COMP_TriggerGroup_InvertingInput     = ((uint8_t)0x01),  
  COMP_TriggerGroup_NonInvertingInput  = ((uint8_t)0x02),  
  COMP_TriggerGroup_VREFINTOutput      = ((uint8_t)0x03),  
  COMP_TriggerGroup_DACOutput          = ((uint8_t)0x04)   
}COMP_TriggerGroup_TypeDef;



 
  


 
typedef enum
{
  COMP_TriggerPin_0 = ((uint8_t)0x01), 



 
  COMP_TriggerPin_1 = ((uint8_t)0x02), 



 
  COMP_TriggerPin_2 = ((uint8_t)0x04)  


 
}COMP_TriggerPin_TypeDef;



 
  


 
typedef enum
{
  COMP_OutputLevel_Low   = ((uint8_t)0x00),  
  COMP_OutputLevel_High  = ((uint8_t)0x01)   
}COMP_OutputLevel_TypeDef;



 
  


 

 
 
 
 
void COMP_DeInit(void);
 
void COMP_Init(COMP_InvertingInput_Typedef COMP_InvertingInput, COMP_OutputSelect_Typedef COMP_OutputSelect,
               COMP_Speed_TypeDef COMP_Speed);
void COMP_VrefintToCOMP1Connect(FunctionalState NewState);
void COMP_EdgeConfig(COMP_Selection_TypeDef COMP_Selection, COMP_Edge_TypeDef COMP_Edge);
COMP_OutputLevel_TypeDef COMP_GetOutputLevel(COMP_Selection_TypeDef COMP_Selection);
 
void COMP_WindowCmd(FunctionalState NewState);
 
void COMP_VrefintOutputCmd(FunctionalState NewState);
 
void COMP_SchmittTriggerCmd(FunctionalState NewState);
void COMP_TriggerConfig(COMP_TriggerGroup_TypeDef COMP_TriggerGroup,
                        COMP_TriggerPin_TypeDef COMP_TriggerPin,
                        FunctionalState NewState);
 
void COMP_ITConfig(COMP_Selection_TypeDef COMP_Selection, FunctionalState NewState);
FlagStatus COMP_GetFlagStatus(COMP_Selection_TypeDef COMP_Selection);
void COMP_ClearFlag(COMP_Selection_TypeDef COMP_Selection);
ITStatus COMP_GetITStatus(COMP_Selection_TypeDef COMP_Selection);
void COMP_ClearITPendingBit(COMP_Selection_TypeDef COMP_Selection);



 




 

 


























 

 

 



 



 
  
 



 
  


 
typedef enum
{
  DAC_Trigger_None     = ((uint8_t)0x30),  
  DAC_Trigger_T4_TRGO  = ((uint8_t)0x00),  
  DAC_Trigger_T5_TRGO  = ((uint8_t)0x08),  
  DAC_Trigger_Ext      = ((uint8_t)0x10),  
  DAC_Trigger_Software = ((uint8_t)0x38)   
}DAC_Trigger_TypeDef;



 



 
typedef enum
{
  DAC_Align_12b_R = ((uint8_t)0x00),  
  DAC_Align_12b_L = ((uint8_t)0x04),  
  DAC_Align_8b_R  = ((uint8_t)0x08)   
}DAC_Align_TypeDef;



 
  


 
typedef enum
{
  DAC_Channel_1 = ((uint8_t)0x00),  
  DAC_Channel_2 = ((uint8_t)0x01)   
}DAC_Channel_TypeDef;

                                 


 
  


 

typedef enum
{
  DAC_Wave_Noise    = ((uint8_t)0x40),  
  DAC_Wave_Triangle = ((uint8_t)0x80)   
}DAC_Wave_TypeDef;



 
  


 
typedef enum
{
  DAC_OutputBuffer_Enable  = ((uint8_t)0x00),  
  DAC_OutputBuffer_Disable = ((uint8_t)0x02)   
}DAC_OutputBuffer_TypeDef;



 
  


 
typedef enum
{
  DAC_IT_DMAUDR = ((uint8_t)0x20)  
}DAC_IT_TypeDef;



 
  


  
typedef enum
{
  DAC_FLAG_DMAUDR = ((uint8_t)0x01)  
}DAC_FLAG_TypeDef;




 
  


 
  
typedef enum
{
  DAC_LFSRUnmask_Bit0       =  ((uint8_t)0x00),  
  DAC_LFSRUnmask_Bits1_0    =  ((uint8_t)0x01),  
  DAC_LFSRUnmask_Bits2_0    =  ((uint8_t)0x02),  
  DAC_LFSRUnmask_Bits3_0    =  ((uint8_t)0x03),  
  DAC_LFSRUnmask_Bits4_0    =  ((uint8_t)0x04),  
  DAC_LFSRUnmask_Bits5_0    =  ((uint8_t)0x05),  
  DAC_LFSRUnmask_Bits6_0    =  ((uint8_t)0x06),  
  DAC_LFSRUnmask_Bits7_0    =  ((uint8_t)0x07),  
  DAC_LFSRUnmask_Bits8_0    =  ((uint8_t)0x08),  
  DAC_LFSRUnmask_Bits9_0    =  ((uint8_t)0x09),  
  DAC_LFSRUnmask_Bits10_0   =  ((uint8_t)0x0A),  
  DAC_LFSRUnmask_Bits11_0   =  ((uint8_t)0x0B)   
}DAC_LFSRUnmask_TypeDef;



 
  


 
typedef enum
{
  DAC_TriangleAmplitude_1     =  ((uint8_t)0x00),  
  DAC_TriangleAmplitude_3     =  ((uint8_t)0x01),  
  DAC_TriangleAmplitude_7     =  ((uint8_t)0x02),  
  DAC_TriangleAmplitude_15    =  ((uint8_t)0x03),  
  DAC_TriangleAmplitude_31    =  ((uint8_t)0x04),  
  DAC_TriangleAmplitude_63    =  ((uint8_t)0x05),  
  DAC_TriangleAmplitude_127   =  ((uint8_t)0x06),  
  DAC_TriangleAmplitude_255   =  ((uint8_t)0x07),  
  DAC_TriangleAmplitude_511   =  ((uint8_t)0x08),  
  DAC_TriangleAmplitude_1023  =  ((uint8_t)0x09),  
  DAC_TriangleAmplitude_2047  =  ((uint8_t)0x0A),  
  DAC_TriangleAmplitude_4095  =  ((uint8_t)0x0B)   
}DAC_TriangleAmplitude_TypeDef;



 
  


 
 


 



 



 


   


 



 



 
     


  
 
  

   
void DAC_DeInit(void);

 
void DAC_Init(DAC_Channel_TypeDef DAC_Channel,
              DAC_Trigger_TypeDef DAC_Trigger,
              DAC_OutputBuffer_TypeDef DAC_OutputBuffer);
void DAC_Cmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState);
void DAC_SoftwareTriggerCmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState);
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState);
void DAC_WaveGenerationCmd(DAC_Channel_TypeDef DAC_Channel, DAC_Wave_TypeDef DAC_Wave, FunctionalState NewState);
void DAC_SetNoiseWaveLFSR(DAC_Channel_TypeDef DAC_Channel, DAC_LFSRUnmask_TypeDef DAC_LFSRUnmask);
void DAC_SetTriangleWaveAmplitude(DAC_Channel_TypeDef DAC_Channel, DAC_TriangleAmplitude_TypeDef DAC_TriangleAmplitude);
void DAC_SetChannel1Data(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data);
void DAC_SetChannel2Data(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data);
void DAC_SetDualChannelData(DAC_Align_TypeDef DAC_Align, uint16_t DAC_Data2, uint16_t DAC_Data1);
uint16_t DAC_GetDataOutputValue(DAC_Channel_TypeDef DAC_Channel);

                             
void DAC_DMACmd(DAC_Channel_TypeDef DAC_Channel, FunctionalState NewState);

 
void DAC_ITConfig(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT, FunctionalState NewState);
FlagStatus DAC_GetFlagStatus(DAC_Channel_TypeDef DAC_Channel, DAC_FLAG_TypeDef DAC_FLAG);
void DAC_ClearFlag(DAC_Channel_TypeDef DAC_Channel, DAC_FLAG_TypeDef DAC_FLAG);
ITStatus DAC_GetITStatus(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT);
void DAC_ClearITPendingBit(DAC_Channel_TypeDef DAC_Channel, DAC_IT_TypeDef DAC_IT);




 
  


 

 


























 
 

 



 



  

 



 



 
typedef enum
{
  DMA_DIR_PeripheralToMemory  = ((uint8_t)0x00),  
  DMA_DIR_MemoryToPeripheral  = ((uint8_t)0x08),  
  DMA_DIR_Memory0ToMemory1    = ((uint8_t)0x40)   
}DMA_DIR_TypeDef;



 
  


 
typedef enum
{
  DMA_Mode_Normal = ((uint8_t)0x00),   
  DMA_Mode_Circular = ((uint8_t)0x10)  
}DMA_Mode_TypeDef;



 
  


 
typedef enum
{
  DMA_MemoryIncMode_Dec = ((uint8_t)0x00),  
  DMA_MemoryIncMode_Inc = ((uint8_t)0x20)   
}DMA_MemoryIncMode_TypeDef;



 
  


 
typedef enum
{
  DMA_Priority_Low      = ((uint8_t)0x00),  
  DMA_Priority_Medium   = ((uint8_t)0x10),  
  DMA_Priority_High     = ((uint8_t)0x20),  
  DMA_Priority_VeryHigh = ((uint8_t)0x30)   
}DMA_Priority_TypeDef;



 
  


 
typedef enum
{
  DMA_MemoryDataSize_Byte     = ((uint8_t)0x00), 
  DMA_MemoryDataSize_HalfWord = ((uint8_t)0x08)  
}DMA_MemoryDataSize_TypeDef;



 
  


 
typedef enum
{
  DMA1_FLAG_GB    = ((uint16_t)0x0002),  

  DMA1_FLAG_IFC0  = ((uint16_t)0x1001),  
  DMA1_FLAG_IFC1  = ((uint16_t)0x1002),  
  DMA1_FLAG_IFC2  = ((uint16_t)0x1004),  
  DMA1_FLAG_IFC3  = ((uint16_t)0x1008),  

  DMA1_FLAG_TC0   = ((uint16_t)0x0102),  
  DMA1_FLAG_TC1   = ((uint16_t)0x0202),  
  DMA1_FLAG_TC2   = ((uint16_t)0x0402),  
  DMA1_FLAG_TC3   = ((uint16_t)0x0802),  

  DMA1_FLAG_HT0   = ((uint16_t)0x0104),  
  DMA1_FLAG_HT1   = ((uint16_t)0x0204),  
  DMA1_FLAG_HT2   = ((uint16_t)0x0404),  
  DMA1_FLAG_HT3   = ((uint16_t)0x0804),  

  DMA1_FLAG_PEND0 = ((uint16_t)0x0140),  
  DMA1_FLAG_PEND1 = ((uint16_t)0x0240),  
  DMA1_FLAG_PEND2 = ((uint16_t)0x0440),  
  DMA1_FLAG_PEND3 = ((uint16_t)0x0840),  

  DMA1_FLAG_BUSY0 = ((uint16_t)0x0180),  
  DMA1_FLAG_BUSY1 = ((uint16_t)0x0280),  
  DMA1_FLAG_BUSY2 = ((uint16_t)0x0480),  
  DMA1_FLAG_BUSY3 = ((uint16_t)0x0880)   
}DMA_FLAG_TypeDef;




 
  


 
typedef enum
{
  DMA_ITx_TC = ((uint8_t)0x02), 
  DMA_ITx_HT = ((uint8_t)0x04)  
}DMA_ITx_TypeDef;



 
  


 
typedef enum
{
   
  DMA1_IT_TC0 = ((uint8_t)0x12),  
  DMA1_IT_TC1 = ((uint8_t)0x22),  
  DMA1_IT_TC2 = ((uint8_t)0x42),  
  DMA1_IT_TC3 = ((uint8_t)0x82),  
   
  DMA1_IT_HT0 = ((uint8_t)0x14),  
  DMA1_IT_HT1 = ((uint8_t)0x24),  
  DMA1_IT_HT2 = ((uint8_t)0x44),  
  DMA1_IT_HT3 = ((uint8_t)0x84)   
}DMA_IT_TypeDef;




 



 
     
 


 



 


 




 



 



 



 
  


 

  
  
  
void DMA_GlobalDeInit(void);
void DMA_DeInit(DMA_Channel_TypeDef* DMA_Channelx);

 
void DMA_Init(DMA_Channel_TypeDef* DMA_Channelx,
              uint32_t DMA_Memory0BaseAddr,
              uint16_t DMA_PeripheralMemory1BaseAddr,
              uint8_t DMA_BufferSize,
              DMA_DIR_TypeDef DMA_DIR,
              DMA_Mode_TypeDef DMA_Mode,
              DMA_MemoryIncMode_TypeDef DMA_MemoryIncMode,
              DMA_Priority_TypeDef DMA_Priority,
              DMA_MemoryDataSize_TypeDef DMA_MemoryDataSize );
void DMA_GlobalCmd(FunctionalState NewState);
void DMA_Cmd(DMA_Channel_TypeDef* DMA_Channelx, FunctionalState NewState);
void DMA_SetTimeOut(uint8_t DMA_TimeOut);

 
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx, uint8_t DataNumber);
uint8_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx);

 
void DMA_ITConfig(DMA_Channel_TypeDef* DMA_Channelx, DMA_ITx_TypeDef DMA_ITx, FunctionalState NewState);
FlagStatus DMA_GetFlagStatus(DMA_FLAG_TypeDef DMA_FLAG);
void DMA_ClearFlag(DMA_FLAG_TypeDef DMA_FLAG);
ITStatus DMA_GetITStatus(DMA_IT_TypeDef DMA_IT);
void DMA_ClearITPendingBit(DMA_IT_TypeDef DMA_IT);




 
  


 

 


























 

 

 



 



 

 



 



 
typedef enum
{
  EXTI_Trigger_Falling_Low    = (uint8_t)0x00,  
  EXTI_Trigger_Rising         = (uint8_t)0x01,  
  EXTI_Trigger_Falling        = (uint8_t)0x02,  
  EXTI_Trigger_Rising_Falling = (uint8_t)0x03   
} EXTI_Trigger_TypeDef;



 









 
typedef enum
{
  EXTI_HalfPort_B_LSB  = (uint8_t)0x01,  
  EXTI_HalfPort_B_MSB  = (uint8_t)0x02,  
  EXTI_HalfPort_D_LSB  = (uint8_t)0x04,  
  EXTI_HalfPort_D_MSB  = (uint8_t)0x08,  
  EXTI_HalfPort_E_LSB  = (uint8_t)0x10,  
  EXTI_HalfPort_E_MSB  = (uint8_t)0x20,  
  EXTI_HalfPort_F_LSB  = (uint8_t)0x40,  
  EXTI_HalfPort_F_MSB  = (uint8_t)0x81,  
  EXTI_HalfPort_G_LSB  = (uint8_t)0x82,  
  EXTI_HalfPort_G_MSB  = (uint8_t)0x84,  
  EXTI_HalfPort_H_LSB  = (uint8_t)0x88,  
  EXTI_HalfPort_H_MSB  = (uint8_t)0x90   
} EXTI_HalfPort_TypeDef;



 










 
typedef enum
{
  EXTI_Port_B = (uint8_t)0x00,  
  EXTI_Port_D = (uint8_t)0x02,  
  EXTI_Port_E = (uint8_t)0x04,  
  EXTI_Port_F = (uint8_t)0x06,  
  EXTI_Port_G = (uint8_t)0x10,  
  EXTI_Port_H = (uint8_t)0x12   
} EXTI_Port_TypeDef;



 










 
typedef enum
{
  EXTI_Pin_0 = (uint8_t)0x00,  
  EXTI_Pin_1 = (uint8_t)0x02,  
  EXTI_Pin_2 = (uint8_t)0x04,  
  EXTI_Pin_3 = (uint8_t)0x06,  
  EXTI_Pin_4 = (uint8_t)0x10,  
  EXTI_Pin_5 = (uint8_t)0x12,  
  EXTI_Pin_6 = (uint8_t)0x14,  
  EXTI_Pin_7 = (uint8_t)0x16   
} EXTI_Pin_TypeDef;



 










 
typedef enum
{
  EXTI_IT_Pin0    = (uint16_t)0x0001,  
  EXTI_IT_Pin1    = (uint16_t)0x0002,  
  EXTI_IT_Pin2    = (uint16_t)0x0004,  
  EXTI_IT_Pin3    = (uint16_t)0x0008,  
  EXTI_IT_Pin4    = (uint16_t)0x0010,  
  EXTI_IT_Pin5    = (uint16_t)0x0020,  
  EXTI_IT_Pin6    = (uint16_t)0x0040,  
  EXTI_IT_Pin7    = (uint16_t)0x0080,  
  EXTI_IT_PortB   = (uint16_t)0x0101,  
  EXTI_IT_PortD   = (uint16_t)0x0102,  
  EXTI_IT_PortE   = (uint16_t)0x0104,  
  EXTI_IT_PortF   = (uint16_t)0x0108,  
  EXTI_IT_PortG   = (uint16_t)0x0110,  
  EXTI_IT_PortH   = (uint16_t)0x0120   
} EXTI_IT_TypeDef;



 



 

 
 



 



 




 




 



 



 



 


 

 
 
void EXTI_DeInit(void);
void EXTI_SetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin, EXTI_Trigger_TypeDef EXTI_Trigger);
void EXTI_SelectPort(EXTI_Port_TypeDef EXTI_Port);
void EXTI_SetHalfPortSelection(EXTI_HalfPort_TypeDef EXTI_HalfPort, FunctionalState NewState);
void EXTI_SetPortSensitivity(EXTI_Port_TypeDef EXTI_Port, EXTI_Trigger_TypeDef EXTI_Trigger);
EXTI_Trigger_TypeDef EXTI_GetPinSensitivity(EXTI_Pin_TypeDef EXTI_Pin);
EXTI_Trigger_TypeDef EXTI_GetPortSensitivity(EXTI_Port_TypeDef EXTI_Port);

 
ITStatus EXTI_GetITStatus(EXTI_IT_TypeDef EXTI_IT);
void EXTI_ClearITPendingBit(EXTI_IT_TypeDef EXTI_IT);




 
  


 

 


























 

 

 



 



 

 



 

 
 
  

 



 

 



 



 
  typedef enum
  {
    FLASH_MemType_Program      = (uint8_t)0xFD,  
    FLASH_MemType_Data         = (uint8_t)0xF7   
  } FLASH_MemType_TypeDef;



 
  


 
typedef enum
{
  FLASH_ProgramMode_Standard = (uint8_t)0x00,  
  FLASH_ProgramMode_Fast     = (uint8_t)0x10   
} FLASH_ProgramMode_TypeDef;



 
  


 
typedef enum
{
  FLASH_ProgramTime_Standard = (uint8_t)0x00,  
  FLASH_ProgramTime_TProg    = (uint8_t)0x01   
} FLASH_ProgramTime_TypeDef;



 
  


 
typedef enum
{
  FLASH_Power_IDDQ = (uint8_t)0x00,   
  FLASH_Power_On    = (uint8_t)0x01   
} FLASH_Power_TypeDef;



 
  


 
typedef enum
{
  FLASH_Status_Write_Protection_Error = (uint8_t)0x01,  
  FLASH_Status_TimeOut                = (uint8_t)0x02,  
  FLASH_Status_Successful_Operation   = (uint8_t)0x04   
} FLASH_Status_TypeDef;



 
  


 
typedef enum
{
  FLASH_PowerStatus_IDDQDuringWaitMode          = (uint8_t)0x04, 
 
  FLASH_PowerStatus_IDDQDuringRunMode           = (uint8_t)0x08, 
 
  FLASH_PowerStatus_IDDQDuringWaitAndRunModes   = (uint8_t)0x0C,  
 
  FLASH_PowerStatus_On                          = (uint8_t)0x00  
 
} FLASH_PowerStatus_TypeDef;



 



 
typedef enum {
  FLASH_FLAG_HVOFF     = (uint8_t)0x40,      
  FLASH_FLAG_DUL       = (uint8_t)0x08,      
  FLASH_FLAG_EOP       = (uint8_t)0x04,      
  FLASH_FLAG_PUL       = (uint8_t)0x02,      
  FLASH_FLAG_WR_PG_DIS = (uint8_t)0x01       
} FLASH_FLAG_TypeDef;



 
  


 

 



 




 






 





 




 




 




 




 




 



 



 



 



 



 

 
 
FLASH_ProgramTime_TypeDef FLASH_GetProgrammingTime(void);
void FLASH_SetProgrammingTime(FLASH_ProgramTime_TypeDef FLASH_ProgTime);
void FLASH_PowerWaitModeConfig(FLASH_Power_TypeDef FLASH_Power);

 
void FLASH_DeInit(void);
void FLASH_Unlock(FLASH_MemType_TypeDef FLASH_MemType);
void FLASH_Lock(FLASH_MemType_TypeDef FLASH_MemType);
void FLASH_ProgramByte(uint32_t Address, uint8_t Data);
void FLASH_EraseByte(uint32_t Address);
void FLASH_ProgramWord(uint32_t Address, uint32_t Data);
uint8_t FLASH_ReadByte(uint32_t Address);

 
uint16_t FLASH_GetBootSize(void);
uint16_t FLASH_GetCodeSize(void);
FunctionalState FLASH_GetReadOutProtectionStatus(void);
void FLASH_ProgramOptionByte(uint16_t Address, uint8_t Data);
void FLASH_EraseOptionByte(uint16_t Address);

 
void FLASH_ITConfig(FunctionalState NewState);
FlagStatus FLASH_GetFlagStatus(FLASH_FLAG_TypeDef FLASH_FLAG);

 














 

void FLASH_PowerRunModeConfig(FLASH_Power_TypeDef FLASH_Power);
FLASH_PowerStatus_TypeDef FLASH_GetPowerStatus(void);

void FLASH_ProgramBlock(uint16_t BlockNum, FLASH_MemType_TypeDef FLASH_MemType, FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer);
void FLASH_EraseBlock(uint16_t BlockNum, FLASH_MemType_TypeDef FLASH_MemType);

FLASH_Status_TypeDef FLASH_WaitForLastOperation(FLASH_MemType_TypeDef FLASH_MemType);




 
  


 

 


























 

 

 



 
  


 

 



 















 
typedef enum
{
  GPIO_Mode_In_FL_No_IT      = (uint8_t)0x00,    
  GPIO_Mode_In_PU_No_IT      = (uint8_t)0x40,    
  GPIO_Mode_In_FL_IT         = (uint8_t)0x20,    
  GPIO_Mode_In_PU_IT         = (uint8_t)0x60,    
  GPIO_Mode_Out_OD_Low_Fast  = (uint8_t)0xA0,    
  GPIO_Mode_Out_PP_Low_Fast  = (uint8_t)0xE0,    
  GPIO_Mode_Out_OD_Low_Slow  = (uint8_t)0x80,    
  GPIO_Mode_Out_PP_Low_Slow  = (uint8_t)0xC0,    
  GPIO_Mode_Out_OD_HiZ_Fast  = (uint8_t)0xB0,    
  GPIO_Mode_Out_PP_High_Fast = (uint8_t)0xF0,    
  GPIO_Mode_Out_OD_HiZ_Slow  = (uint8_t)0x90,    
  GPIO_Mode_Out_PP_High_Slow = (uint8_t)0xD0     
}GPIO_Mode_TypeDef;



 
  


 

typedef enum
{
  GPIO_Pin_0    = ((uint8_t)0x01),    
  GPIO_Pin_1    = ((uint8_t)0x02),    
  GPIO_Pin_2    = ((uint8_t)0x04),    
  GPIO_Pin_3    = ((uint8_t)0x08),    
  GPIO_Pin_4    = ((uint8_t)0x10),    
  GPIO_Pin_5    = ((uint8_t)0x20),    
  GPIO_Pin_6    = ((uint8_t)0x40),    
  GPIO_Pin_7    = ((uint8_t)0x80),    
  GPIO_Pin_LNib = ((uint8_t)0x0F),    
  GPIO_Pin_HNib = ((uint8_t)0xF0),    
  GPIO_Pin_All  = ((uint8_t)0xFF)     
}GPIO_Pin_TypeDef;



 
  


 
  
 
 



 



 




 




 



 


 
 
void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_Init(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, GPIO_Mode_TypeDef GPIO_Mode);
void GPIO_ExternalPullUpConfig(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin, FunctionalState NewState);

 
void GPIO_Write(GPIO_TypeDef* GPIOx, uint8_t GPIO_PortVal);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin, BitAction GPIO_BitVal);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin);
void GPIO_ToggleBits(GPIO_TypeDef* GPIOx, uint8_t GPIO_Pin);
uint8_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
BitStatus GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin);
BitStatus GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin);




 
  


 

 


























  

 

 



 



 

 


 


 
typedef enum
{
  I2C_Mode_I2C           = (uint8_t)0x00,    
  I2C_Mode_SMBusDevice   = (uint8_t)0x02,    
  I2C_Mode_SMBusHost     = (uint8_t)0x0A     
} I2C_Mode_TypeDef;



 



 
typedef enum
{
  I2C_DutyCycle_2    = (uint8_t)0x00,   
  I2C_DutyCycle_16_9 = (uint8_t)0x40    
} I2C_DutyCycle_TypeDef;



 



 
typedef enum
{
  I2C_Ack_Disable = (uint8_t)0x00,   
  I2C_Ack_Enable  = (uint8_t)0x04    
} I2C_Ack_TypeDef;


 



 
typedef enum
{
  I2C_AckPosition_Current = (uint8_t)0x00,    
  I2C_AckPosition_Next    = (uint8_t)0x08     
} I2C_AckPosition_TypeDef;



 



 
typedef enum
{
  I2C_AcknowledgedAddress_7bit  = (uint8_t)0x00,   
  I2C_AcknowledgedAddress_10bit = (uint8_t)0x80    
} I2C_AcknowledgedAddress_TypeDef;


 



 
  


 
typedef enum
{
  I2C_Direction_Transmitter = (uint8_t)0x00,   
  I2C_Direction_Receiver    = (uint8_t)0x01    
} I2C_Direction_TypeDef;



 



 
typedef enum
{
  I2C_SMBusAlert_High = (uint8_t)0x00,   
  I2C_SMBusAlert_Low  =  (uint8_t)0x01    
} I2C_SMBusAlert_TypeDef;



 



 
typedef enum
{
  I2C_PECPosition_Current = (uint8_t)0x00,   
  I2C_PECPosition_Next    = (uint8_t)0x08    
} I2C_PECPosition_TypeDef;


 



 







 

typedef enum
{
   
  I2C_FLAG_TXE        = (uint16_t)0x0180,   
  I2C_FLAG_RXNE       = (uint16_t)0x0140,   
  I2C_FLAG_STOPF      = (uint16_t)0x0110,   
  I2C_FLAG_ADD10      = (uint16_t)0x0108,   
  I2C_FLAG_BTF        = (uint16_t)0x0104,   
  I2C_FLAG_ADDR       = (uint16_t)0x0102,   
  I2C_FLAG_SB         = (uint16_t)0x0101,   

   
  I2C_FLAG_SMBALERT   = (uint16_t)0x0280,   
  I2C_FLAG_TIMEOUT     = (uint16_t)0x0240,   
  I2C_FLAG_WUFH       = (uint16_t)0x0220,   
  I2C_FLAG_PECERR     = (uint16_t)0x0210,   
  I2C_FLAG_OVR        = (uint16_t)0x0208,   
  I2C_FLAG_AF         = (uint16_t)0x0204,   
  I2C_FLAG_ARLO       = (uint16_t)0x0202,   
  I2C_FLAG_BERR       = (uint16_t)0x0201,   

   
  I2C_FLAG_DUALF    = (uint16_t)0x0380,   
  I2C_FLAG_SMBHOST    = (uint16_t)0x0340,   
  I2C_FLAG_SMBDEFAULT = (uint16_t)0x0320,   
  I2C_FLAG_GENCALL    = (uint16_t)0x0310,   
  I2C_FLAG_TRA        = (uint16_t)0x0304,   
  I2C_FLAG_BUSY       = (uint16_t)0x0302,   
  I2C_FLAG_MSL        = (uint16_t)0x0301    
} I2C_FLAG_TypeDef;




 



 










 

typedef enum
{
  I2C_IT_ERR             = (uint16_t)0x0001,    
  I2C_IT_EVT             = (uint16_t)0x0002,    
  I2C_IT_BUF             = (uint16_t)0x0004,    
   
  I2C_IT_TXE             = (uint16_t)0x1680,    
  I2C_IT_RXNE            = (uint16_t)0x1640,    
  I2C_IT_STOPF           = (uint16_t)0x1210,    
  I2C_IT_ADD10           = (uint16_t)0x1208,    
  I2C_IT_BTF             = (uint16_t)0x1204,    
  I2C_IT_ADDR            = (uint16_t)0x1202,    
  I2C_IT_SB              = (uint16_t)0x1201,    

   
  I2C_IT_SMBALERT        = (uint16_t)0x2180,    
  I2C_IT_TIMEOUT         = (uint16_t)0x2140,    
  I2C_IT_WUFH            = (uint16_t)0x2220,    
  I2C_IT_PECERR          = (uint16_t)0x2110,    
  I2C_IT_OVR             = (uint16_t)0x2108,    
  I2C_IT_AF              = (uint16_t)0x2104,    
  I2C_IT_ARLO            = (uint16_t)0x2102,    
  I2C_IT_BERR            = (uint16_t)0x2101     
} I2C_IT_TypeDef;


 
  


 








 

typedef enum
{
  



 







 
   
  I2C_EVENT_MASTER_MODE_SELECT               = (uint16_t)0x0301,   


























 
   
  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = (uint16_t)0x0782,   
  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED    = (uint16_t)0x0302,   
   
  I2C_EVENT_MASTER_MODE_ADDRESS10            = (uint16_t)0x0308,   




























 
   
   
  I2C_EVENT_MASTER_BYTE_RECEIVED             = (uint16_t)0x0340,   

   
   
  I2C_EVENT_MASTER_BYTE_TRANSMITTING         = (uint16_t)0x0780,   
   

  I2C_EVENT_MASTER_BYTE_TRANSMITTED          = (uint16_t)0x0784,   





 
 
























 

   
   
  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED    = (uint16_t)0x0202,   
  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED = (uint16_t)0x0682,   

   
  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    = (uint16_t)0x8200,   
  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED = (uint16_t)0x8680,   

   
  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED  = (uint16_t)0x1200,   

  

























 
   
   
  I2C_EVENT_SLAVE_BYTE_RECEIVED              = (uint16_t)0x0240,   
   
  I2C_EVENT_SLAVE_STOP_DETECTED              = (uint16_t)0x0010,   

   
   
  I2C_EVENT_SLAVE_BYTE_TRANSMITTED           = (uint16_t)0x0684,   
  I2C_EVENT_SLAVE_BYTE_TRANSMITTING          = (uint16_t)0x0680,   
   
  I2C_EVENT_SLAVE_ACK_FAILURE                = (uint16_t)0x0004   
} I2C_Event_TypeDef;



 



 
typedef enum
{
  I2C_Register_CR1    = (uint8_t)0x00,    
  I2C_Register_CR2    = (uint8_t)0x01,    
  I2C_Register_FREQR  = (uint8_t)0x02,    
  I2C_Register_OARL   = (uint8_t)0x03,    
  I2C_Register_OARH   = (uint8_t)0x04,    
  I2C_Register_DR     = (uint8_t)0x06,    
  I2C_Register_SR1    = (uint8_t)0x07,    
  I2C_Register_SR2    = (uint8_t)0x08,    
  I2C_Register_SR3    = (uint8_t)0x09,    
  I2C_Register_ITR    = (uint8_t)0x0A,    
  I2C_Register_CCRL   = (uint8_t)0x0B,    
  I2C_Register_CCRH   = (uint8_t)0x0C,    
  I2C_Register_TRISER = (uint8_t)0x0D,    
  I2C_Register_PECR   = (uint8_t)0x0E     
} I2C_Register_TypeDef;


 
  


 
 


 



 

 


 



 



 



 



 


 



 



 



 



 



 



 




 




 



 


 


 


 




 



 



 

 

 
void I2C_DeInit(I2C_TypeDef* I2Cx);

 
void I2C_Init(I2C_TypeDef* I2Cx, uint32_t OutputClockFrequency, uint16_t OwnAddress,
              I2C_Mode_TypeDef I2C_Mode, I2C_DutyCycle_TypeDef I2C_DutyCycle,
              I2C_Ack_TypeDef I2C_Ack, I2C_AcknowledgedAddress_TypeDef I2C_AcknowledgedAddress);
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_ARPCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address);
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AckPositionConfig(I2C_TypeDef* I2Cx, I2C_AckPosition_TypeDef I2C_AckPosition);
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, I2C_DutyCycle_TypeDef I2C_DutyCycle);
void I2C_SMBusAlertConfig(I2C_TypeDef* I2Cx, I2C_SMBusAlert_TypeDef I2C_SMBusAlert);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, I2C_Direction_TypeDef I2C_Direction);

  
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);

  
void I2C_PECPositionConfig(I2C_TypeDef* I2Cx, I2C_PECPosition_TypeDef I2C_PECPosition);
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx);
void I2C_TransmitPEC(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState);

 
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_DMALastTransferCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

 

void I2C_ITConfig(I2C_TypeDef* I2Cx, I2C_IT_TypeDef I2C_IT, FunctionalState NewState);
uint8_t I2C_ReadRegister(I2C_TypeDef* I2Cx, I2C_Register_TypeDef I2C_Register);
















































































 





   
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, I2C_Event_TypeDef I2C_Event);




 
I2C_Event_TypeDef I2C_GetLastEvent(I2C_TypeDef* I2Cx);




 
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, I2C_FLAG_TypeDef I2C_FLAG);

void I2C_ClearFlag(I2C_TypeDef* I2Cx, I2C_FLAG_TypeDef I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, I2C_IT_TypeDef I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, I2C_IT_TypeDef I2C_IT);




 
  


 

 


























 

 

 



 



 
 
 
 
 
 

 
void IRTIM_DeInit(void);
void IRTIM_Cmd(FunctionalState NewState);
void IRTIM_HighSinkODCmd(FunctionalState NewState);

 
FunctionalState IRTIM_GetStatus(void);
FunctionalState IRTIM_GetHighSinkODStatus(void);



 




 

 


























 

 

 



 
  


  
 



 



 
typedef enum {
  FLASH_IRQn                               = (uint8_t)1,    
  DMA1_CHANNEL0_1_IRQn                     = (uint8_t)2,    
  DMA1_CHANNEL2_3_IRQn                     = (uint8_t)3,    
  EXTIE_F_PVD_IRQn                         = (uint8_t)5,    
  EXTI0_IRQn                               = (uint8_t)8,    
  EXTI1_IRQn                               = (uint8_t)9,    
  EXTI2_IRQn                               = (uint8_t)10,   
  EXTI3_IRQn                               = (uint8_t)11,   
  EXTI4_IRQn                               = (uint8_t)12,   
  EXTI5_IRQn                               = (uint8_t)13,   
  EXTI6_IRQn                               = (uint8_t)14,   
  EXTI7_IRQn                               = (uint8_t)15,   
  ADC1_COMP_IRQn                           = (uint8_t)18,   
  TIM4_UPD_OVF_TRG_IRQn                    = (uint8_t)25,   
  SPI1_IRQn                                = (uint8_t)26,   
  RTC_CSSLSE_IRQn                          = (uint8_t)4,    
  EXTIB_G_IRQn                             = (uint8_t)6,    
  EXTID_H_IRQn                             = (uint8_t)7,    
  LCD_AES_IRQn                             = (uint8_t)16,   
	SWITCH_CSS_BREAK_DAC_IRQn                = (uint8_t)17,   
  TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn      = (uint8_t)19,   
  TIM2_CC_USART2_RX_IRQn                   = (uint8_t)20,   
  TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn      = (uint8_t)21,   
  TIM3_CC_USART3_RX_IRQn                   = (uint8_t)22,   
	TIM1_UPD_OVF_TRG_IRQn                    = (uint8_t)23,   
  TIM1_CC_IRQn                             = (uint8_t)24,   
  USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn      = (uint8_t)27,   
  USART1_RX_TIM5_CC_IRQn                   = (uint8_t)28,   
  I2C1_SPI2_IRQn                           = (uint8_t)29    
}IRQn_TypeDef;




 
  


 
typedef enum {
  ITC_PriorityLevel_0 = (uint8_t)0x02,  
  ITC_PriorityLevel_1 = (uint8_t)0x01,  
  ITC_PriorityLevel_2 = (uint8_t)0x00,  
  ITC_PriorityLevel_3 = (uint8_t)0x03   
} ITC_PriorityLevel_TypeDef;




 
  


 
 



 




 

 



 



 

 
  
void ITC_DeInit(void);

  
uint8_t ITC_GetCPUCC(void);
uint8_t ITC_GetSoftIntStatus(void);
void ITC_SetSoftwarePriority(IRQn_TypeDef IRQn, ITC_PriorityLevel_TypeDef ITC_PriorityLevel);
ITC_PriorityLevel_TypeDef ITC_GetSoftwarePriority(IRQn_TypeDef IRQn);




 



 
  
 

























 

 

 



 



 
  
 
 



 



 


 
 


 


 


 

 
 



 



 
typedef enum
{
  IWDG_WriteAccess_Enable  = (uint8_t)0x55, 
  IWDG_WriteAccess_Disable = (uint8_t)0x00
} IWDG_WriteAccess_TypeDef;


 
 


 
typedef enum
{
  IWDG_Prescaler_4   = (uint8_t)0x00,  
  IWDG_Prescaler_8   = (uint8_t)0x01,  
  IWDG_Prescaler_16  = (uint8_t)0x02,  
  IWDG_Prescaler_32  = (uint8_t)0x03,  
  IWDG_Prescaler_64  = (uint8_t)0x04,  
  IWDG_Prescaler_128 = (uint8_t)0x05,  
  IWDG_Prescaler_256 = (uint8_t)0x06   
} IWDG_Prescaler_TypeDef;


         
                                 


 

 
 
void IWDG_WriteAccessCmd(IWDG_WriteAccess_TypeDef IWDG_WriteAccess);
void IWDG_SetPrescaler(IWDG_Prescaler_TypeDef IWDG_Prescaler);
void IWDG_SetReload(uint8_t IWDG_Reload);
void IWDG_ReloadCounter(void);

 
void IWDG_Enable(void);




 
  


 

 


























 

 

 



 
  


  
  
 



 




 

typedef enum {
  LCD_Duty_Static = (uint8_t)0x00,  
  LCD_Duty_1_2    = (uint8_t)0x02,  
  LCD_Duty_1_3    = (uint8_t)0x04,  
  LCD_Duty_1_4    = (uint8_t)0x06,  
  LCD_Duty_1_8    = (uint8_t)0x20   
} LCD_Duty_TypeDef;



 
  



 
typedef enum {
  LCD_Bias_1_4  = (uint8_t)0x10,   
  LCD_Bias_1_3  = (uint8_t)0x00,   
  LCD_Bias_1_2  = (uint8_t)0x01    
} LCD_Bias_TypeDef;




 
                           



 
typedef enum {
  LCD_Prescaler_1     = (uint8_t)0x00,  
  LCD_Prescaler_2     = (uint8_t)0x10,  
  LCD_Prescaler_4     = (uint8_t)0x20,  
  LCD_Prescaler_8     = (uint8_t)0x30,  
  LCD_Prescaler_16    = (uint8_t)0x40,  
  LCD_Prescaler_32    = (uint8_t)0x50,  
  LCD_Prescaler_64    = (uint8_t)0x60,  
  LCD_Prescaler_128   = (uint8_t)0x70,  
  LCD_Prescaler_256   = (uint8_t)0x80,  
  LCD_Prescaler_512   = (uint8_t)0x90,  
  LCD_Prescaler_1024  = (uint8_t)0xA0,  
  LCD_Prescaler_2048  = (uint8_t)0xB0,  
  LCD_Prescaler_4096  = (uint8_t)0xC0,  
  LCD_Prescaler_8192  = (uint8_t)0xD0,  
  LCD_Prescaler_16384 = (uint8_t)0xE0,  
  LCD_Prescaler_32768 = (uint8_t)0xF0   
} LCD_Prescaler_TypeDef;

    


 
  



 
typedef enum {
  LCD_Divider_16 = (uint8_t)0x00,  
  LCD_Divider_17 = (uint8_t)0x01,  
  LCD_Divider_18 = (uint8_t)0x02,  
  LCD_Divider_19 = (uint8_t)0x03,  
  LCD_Divider_20 = (uint8_t)0x04,  
  LCD_Divider_21 = (uint8_t)0x05,  
  LCD_Divider_22 = (uint8_t)0x06,  
  LCD_Divider_23 = (uint8_t)0x07,  
  LCD_Divider_24 = (uint8_t)0x08,  
  LCD_Divider_25 = (uint8_t)0x09,  
  LCD_Divider_26 = (uint8_t)0x0A,  
  LCD_Divider_27 = (uint8_t)0x0B,  
  LCD_Divider_28 = (uint8_t)0x0C,  
  LCD_Divider_29 = (uint8_t)0x0D,  
  LCD_Divider_30 = (uint8_t)0x0E,  
  LCD_Divider_31 = (uint8_t)0x0F   
} LCD_Divider_TypeDef;

                                       


 
  



 
typedef enum {
  LCD_Contrast_Level_0 = (uint8_t)0x00,  
  LCD_Contrast_Level_1 = (uint8_t)0x02,  
  LCD_Contrast_Level_2 = (uint8_t)0x04,  
  LCD_Contrast_Level_3 = (uint8_t)0x06,  
  LCD_Contrast_Level_4 = (uint8_t)0x08,  
  LCD_Contrast_Level_5 = (uint8_t)0x0A,  
  LCD_Contrast_Level_6 = (uint8_t)0x0C,  
  LCD_Contrast_Level_7 = (uint8_t)0x0E   
} LCD_Contrast_TypeDef;

                                   


 




 
typedef enum {
  LCD_VoltageSource_Internal = (uint8_t)0x00,  
  LCD_VoltageSource_External = (uint8_t)0x01   
} LCD_VoltageSource_TypeDef;

                                       


 




 
typedef enum {
  LCD_PulseOnDuration_0 = (uint8_t)0x00,  
  LCD_PulseOnDuration_1 = (uint8_t)0x20,  
  LCD_PulseOnDuration_2 = (uint8_t)0x40,  
  LCD_PulseOnDuration_3 = (uint8_t)0x60,  
  LCD_PulseOnDuration_4 = (uint8_t)0x80,  
  LCD_PulseOnDuration_5 = (uint8_t)0xA0,  
  LCD_PulseOnDuration_6 = (uint8_t)0xC0,  
  LCD_PulseOnDuration_7 = (uint8_t)0xE0   
} LCD_PulseOnDuration_TypeDef;

    


 
  



 
typedef enum {
  LCD_DeadTime_0 = (uint8_t)0x00,  
  LCD_DeadTime_1 = (uint8_t)0x01,  
  LCD_DeadTime_2 = (uint8_t)0x02,  
  LCD_DeadTime_3 = (uint8_t)0x03,  
  LCD_DeadTime_4 = (uint8_t)0x04,  
  LCD_DeadTime_5 = (uint8_t)0x05,  
  LCD_DeadTime_6 = (uint8_t)0x06,  
  LCD_DeadTime_7 = (uint8_t)0x07   
} LCD_DeadTime_TypeDef;

                                


 
  


 
typedef enum {
  LCD_BlinkMode_Off           = (uint8_t)0x00,  
  LCD_BlinkMode_SEG0_COM0     = (uint8_t)0x40,  
  LCD_BlinkMode_SEG0_AllCOM   = (uint8_t)0x80,  
  LCD_BlinkMode_AllSEG_AllCOM = (uint8_t)0xC0   
} LCD_BlinkMode_TypeDef;

                                  


 
  



 
typedef enum {
  LCD_BlinkFrequency_Div8    = (uint8_t)0x00,  
  LCD_BlinkFrequency_Div16   = (uint8_t)0x08,  
  LCD_BlinkFrequency_Div32   = (uint8_t)0x10,  
  LCD_BlinkFrequency_Div64   = (uint8_t)0x18,  
  LCD_BlinkFrequency_Div128  = (uint8_t)0x20,  
  LCD_BlinkFrequency_Div256  = (uint8_t)0x28,  
  LCD_BlinkFrequency_Div512  = (uint8_t)0x30,  
  LCD_BlinkFrequency_Div1024 = (uint8_t)0x38   
} LCD_BlinkFrequency_TypeDef;



 
  


 
typedef enum {
  LCD_RAMRegister_0   = (uint8_t)0x00,   
  LCD_RAMRegister_1   = (uint8_t)0x01,   
  LCD_RAMRegister_2   = (uint8_t)0x02,   
  LCD_RAMRegister_3   = (uint8_t)0x03,   
  LCD_RAMRegister_4   = (uint8_t)0x04,   
  LCD_RAMRegister_5   = (uint8_t)0x05,   
  LCD_RAMRegister_6   = (uint8_t)0x06,   
  LCD_RAMRegister_7   = (uint8_t)0x07,   
  LCD_RAMRegister_8   = (uint8_t)0x08,   
  LCD_RAMRegister_9   = (uint8_t)0x09,   
  LCD_RAMRegister_10  = (uint8_t)0x0A,   
  LCD_RAMRegister_11  = (uint8_t)0x0B,   
  LCD_RAMRegister_12  = (uint8_t)0x0C,   
  LCD_RAMRegister_13  = (uint8_t)0x0D,   
  LCD_RAMRegister_14  = (uint8_t)0x0E,   
  LCD_RAMRegister_15  = (uint8_t)0x0F,   
  LCD_RAMRegister_16  = (uint8_t)0x10,   
  LCD_RAMRegister_17  = (uint8_t)0x11,   
  LCD_RAMRegister_18  = (uint8_t)0x12,   
  LCD_RAMRegister_19  = (uint8_t)0x13,   
  LCD_RAMRegister_20  = (uint8_t)0x14,   
  LCD_RAMRegister_21  = (uint8_t)0x15    
} LCD_RAMRegister_TypeDef;

                                       


 
  


 
typedef enum {
  LCD_PortMaskRegister_0 = (uint8_t)0x00,   
  LCD_PortMaskRegister_1 = (uint8_t)0x01,   
  LCD_PortMaskRegister_2 = (uint8_t)0x02,   
  LCD_PortMaskRegister_3 = (uint8_t)0x03,   
  LCD_PortMaskRegister_4 = (uint8_t)0x04,   
  LCD_PortMaskRegister_5 = (uint8_t)0x05    
} LCD_PortMaskRegister_TypeDef;

                                


 
  


 
typedef enum {
  LCD_PageSelection_FirstPage  = (uint8_t)0x00,   
  LCD_PageSelection_SecondPage = (uint8_t)0x04    
} LCD_PageSelection_TypeDef;



 
  


 

 
 



 


 
 
 

 
void LCD_DeInit(void);

 
void LCD_Init(LCD_Prescaler_TypeDef LCD_Prescaler, LCD_Divider_TypeDef LCD_Divider,
              LCD_Duty_TypeDef LCD_Duty, LCD_Bias_TypeDef LCD_Bias, LCD_VoltageSource_TypeDef LCD_VoltageSource);
void LCD_PortMaskConfig(LCD_PortMaskRegister_TypeDef LCD_PortMaskRegister, uint8_t LCD_Mask);
void LCD_Cmd(FunctionalState NewState);
void LCD_HighDriveCmd(FunctionalState NewState);
void LCD_PulseOnDurationConfig(LCD_PulseOnDuration_TypeDef LCD_PulseOnDuration);
void LCD_DeadTimeConfig(LCD_DeadTime_TypeDef LCD_DeadTime);
void LCD_BlinkConfig(LCD_BlinkMode_TypeDef LCD_BlinkMode, LCD_BlinkFrequency_TypeDef LCD_BlinkFrequency);
void LCD_ContrastConfig(LCD_Contrast_TypeDef LCD_Contrast);

 
void LCD_WriteRAM(LCD_RAMRegister_TypeDef LCD_RAMRegister, uint8_t LCD_Data);
void LCD_PageSelect(LCD_PageSelection_TypeDef LCD_PageSelection);

 
void LCD_ITConfig(FunctionalState NewState);
FlagStatus LCD_GetFlagStatus(void);
void LCD_ClearFlag(void);
ITStatus LCD_GetITStatus(void);
void LCD_ClearITPendingBit(void);




 



 

 


























 

 

 



 



  

 



 



 

typedef enum {
  PWR_PVDLevel_1V85  = (uint8_t)0x00,  
  PWR_PVDLevel_2V05  = (uint8_t)0x02,  
  PWR_PVDLevel_2V26  = (uint8_t)0x04,  
  PWR_PVDLevel_2V45  = (uint8_t)0x06,  
  PWR_PVDLevel_2V65  = (uint8_t)0x08,  
  PWR_PVDLevel_2V85  = (uint8_t)0x0A,  
  PWR_PVDLevel_3V05  = (uint8_t)0x0C,  
  PWR_PVDLevel_PVDIn = (uint8_t)0x0E   
} PWR_PVDLevel_TypeDef;



 
  


 
  
typedef enum {
  PWR_FLAG_PVDOF    = (uint8_t)0x40, 
  PWR_FLAG_PVDIF    = (uint8_t)0x20,  
  PWR_FLAG_VREFINTF = (uint8_t)0x01   
} PWR_FLAG_TypeDef;




 
  


 
  
 
 
 

  
void PWR_DeInit(void);

  
void PWR_PVDLevelConfig(PWR_PVDLevel_TypeDef PWR_PVDLevel);
void PWR_PVDCmd(FunctionalState NewState);

  
void PWR_FastWakeUpCmd(FunctionalState NewState);
void PWR_UltraLowPowerCmd(FunctionalState NewState);

 
void PWR_PVDITConfig(FunctionalState NewState);
ITStatus PWR_PVDGetITStatus(void);
FlagStatus PWR_GetFlagStatus(PWR_FLAG_TypeDef PWR_FLAG);
void PWR_PVDClearFlag(void);
void PWR_PVDClearITPendingBit(void);




 



 

 


























 

 

 



 
  


  
  
 
 


 



 
typedef enum {
  RST_FLAG_PORF   = (uint8_t)0x01,  
  RST_FLAG_SWIMF  = (uint8_t)0x08,  
  RST_FLAG_ILLOPF = (uint8_t)0x04,  
  RST_FLAG_IWDGF  = (uint8_t)0x02,  
  RST_FLAG_WWDGF  = (uint8_t)0x10,  
  RST_FLAG_BORF   = (uint8_t)0x20   
} RST_FLAG_TypeDef;



 
  


 
 
 
 
 
FlagStatus RST_GetFlagStatus(RST_FLAG_TypeDef RST_Flag);
void RST_ClearFlag(RST_FLAG_TypeDef RST_Flag);

 
void RST_GPOutputEnable(void);



 
  


 

 


























 
 

 



 



  
  
 


 



  
typedef enum
{
  RTC_Weekday_Monday      =  ((uint8_t)0x01),  
  RTC_Weekday_Tuesday     =  ((uint8_t)0x02),  
  RTC_Weekday_Wednesday   =  ((uint8_t)0x03),  
  RTC_Weekday_Thursday    =  ((uint8_t)0x04),  
  RTC_Weekday_Friday      =  ((uint8_t)0x05),  
  RTC_Weekday_Saturday    =  ((uint8_t)0x06),  
  RTC_Weekday_Sunday      =  ((uint8_t)0x07)   
}
RTC_Weekday_TypeDef;



 
  


  
typedef enum
{
  RTC_Month_January   =  ((uint8_t)0x01),  
  RTC_Month_February  =  ((uint8_t)0x02),  
  RTC_Month_March     =  ((uint8_t)0x03),  
  RTC_Month_April     =  ((uint8_t)0x04),  
  RTC_Month_May       =  ((uint8_t)0x05),  
  RTC_Month_June      =  ((uint8_t)0x06),  
  RTC_Month_July      =  ((uint8_t)0x07),  
  RTC_Month_August    =  ((uint8_t)0x08),  
  RTC_Month_September =  ((uint8_t)0x09),  
  RTC_Month_October   =  ((uint8_t)0x10),  
  RTC_Month_November  =  ((uint8_t)0x11),  
  RTC_Month_December  =  ((uint8_t)0x12)   
}
RTC_Month_TypeDef;



 
  


  
typedef enum
{
  RTC_HourFormat_24  = ((uint8_t)0x00),  
  RTC_HourFormat_12  = ((uint8_t)0x40)   
}
RTC_HourFormat_TypeDef;



 
  


  
typedef enum
{
  RTC_H12_AM     = ((uint8_t)0x00),  
  RTC_H12_PM     = ((uint8_t)0x40)   
}
RTC_H12_TypeDef;



 
  


  
typedef enum
{
  RTC_AlarmDateWeekDaySel_Date     = ((uint8_t)0x00),  
  RTC_AlarmDateWeekDaySel_WeekDay  = ((uint8_t)0x40)   
}
RTC_AlarmDateWeekDaySel_TypeDef;



 
  


  
typedef enum
{
  RTC_AlarmMask_None         =  ((uint8_t)0x00),  
  RTC_AlarmMask_Seconds      =  ((uint8_t)0x80),  
  RTC_AlarmMask_Minutes      =  ((uint8_t)0x40),  
  RTC_AlarmMask_Hours        =  ((uint8_t)0x20),  
  RTC_AlarmMask_DateWeekDay  =  ((uint8_t)0x10),  
  RTC_AlarmMask_All          =  ((uint8_t)0xF0)   
}
RTC_AlarmMask_TypeDef;



 
  


  
typedef enum
{
  RTC_AlarmSubSecondMask_All      =  ((uint8_t)0x00),  
  RTC_AlarmSubSecondMask_None     =  ((uint8_t)0x0F),  
  RTC_AlarmSubSecondMask_SS14_1   =  ((uint8_t)0x01),  
  RTC_AlarmSubSecondMask_SS14_2   =  ((uint8_t)0x02),  
  RTC_AlarmSubSecondMask_SS14_3   =  ((uint8_t)0x03),  
  RTC_AlarmSubSecondMask_SS14_4   =  ((uint8_t)0x04),  
  RTC_AlarmSubSecondMask_SS14_5   =  ((uint8_t)0x05),  
  RTC_AlarmSubSecondMask_SS14_6   =  ((uint8_t)0x06),  
  RTC_AlarmSubSecondMask_SS14_7   =  ((uint8_t)0x07),  
  RTC_AlarmSubSecondMask_SS14_8   =  ((uint8_t)0x08),  
  RTC_AlarmSubSecondMask_SS14_9   =  ((uint8_t)0x09),  
  RTC_AlarmSubSecondMask_SS14_10  =  ((uint8_t)0x0A),  
  RTC_AlarmSubSecondMask_SS14_11  =  ((uint8_t)0x0B),  
  RTC_AlarmSubSecondMask_SS14_12  =  ((uint8_t)0x0C),  
  RTC_AlarmSubSecondMask_SS14_13  =  ((uint8_t)0x0D),  
  RTC_AlarmSubSecondMask_SS14     =  ((uint8_t)0x0E)  
}
RTC_AlarmSubSecondMask_TypeDef;



 
  


  
typedef enum
{
  RTC_WakeUpClock_RTCCLK_Div16    = ((uint8_t)0x00),  
  RTC_WakeUpClock_RTCCLK_Div8     = ((uint8_t)0x01),  
  RTC_WakeUpClock_RTCCLK_Div4     = ((uint8_t)0x02),  
  RTC_WakeUpClock_RTCCLK_Div2     = ((uint8_t)0x03),  
  RTC_WakeUpClock_CK_SPRE_16bits  = ((uint8_t)0x04),  
  RTC_WakeUpClock_CK_SPRE_17bits  = ((uint8_t)0x06)   
}
RTC_WakeUpClock_TypeDef;



 
  


  
typedef enum
{
  RTC_Output_Disable = ((uint8_t)0x00),  
  RTC_Output_Alarm   = ((uint8_t)0x20),  
  RTC_Output_WakeUp  = ((uint8_t)0x60)   
}
RTC_Output_TypeDef;



 
  


  
typedef enum
{
  RTC_OutputPolarity_High  = ((uint8_t)0x00),  
  RTC_OutputPolarity_Low   = ((uint8_t)0x10)   
}
RTC_OutputPolarity_TypeDef;



 
  


  
typedef enum
{
  RTC_CalibOutput_512Hz  = ((uint8_t)0x00),  
  RTC_CalibOutput_1Hz    = ((uint8_t)0x80)   
}
RTC_CalibOutput_TypeDef;



 
  


  
typedef enum
{
  RTC_DayLightSaving_SUB1H  = ((uint8_t)0x02),  
  RTC_DayLightSaving_ADD1H  = ((uint8_t)0x01)   
}
RTC_DayLightSaving_TypeDef;



 
  


  
typedef enum
{
  RTC_StoreOperation_Set   = ((uint8_t)0x04),  
  RTC_StoreOperation_Reset = ((uint8_t)0x00)   
}
RTC_StoreOperation_TypeDef;



 
  


  
typedef enum
{
  RTC_Format_BIN  = ((uint8_t)0x00),  
  RTC_Format_BCD  = ((uint8_t)0x01)   
}
RTC_Format_TypeDef;



 
  


  
typedef enum
{
  RTC_FLAG_TAMP3F  = ((uint16_t)0x0080),  
  RTC_FLAG_TAMP2F  = ((uint16_t)0x0040),  
  RTC_FLAG_TAMP1F  = ((uint16_t)0x0020),  

  RTC_FLAG_WUTF    = ((uint16_t)0x0004),  
  RTC_FLAG_ALRAF   = ((uint16_t)0x0001),  

  RTC_FLAG_INITF   = ((uint16_t)0x4000),  
  RTC_FLAG_RSF     = ((uint16_t)0x2000),  
  RTC_FLAG_INITS   = ((uint16_t)0x1000),  

  RTC_FLAG_SHPF    = ((uint16_t)0x0800), 

 

  RTC_FLAG_WUTWF   = ((uint16_t)0x0400),  
  RTC_FLAG_RECALPF = ((uint16_t)0x0200), 

 
  RTC_FLAG_ALRAWF  = ((uint16_t)0x0100)   
}
RTC_Flag_TypeDef;



 
  


  
typedef enum
{
  RTC_IT_WUT  = ((uint16_t)0x0040),   
  RTC_IT_ALRA = ((uint16_t)0x0010),   
  RTC_IT_TAMP = ((uint16_t)0x0F01)    
}
RTC_IT_TypeDef;



 



  
typedef enum
{
  RTC_TamperLevel_Low    = ((uint8_t)0x00),  
  RTC_TamperLevel_High   = ((uint8_t)0x54)   
}
RTC_TamperLevel_TypeDef;



 
  


  
typedef enum
{
  RTC_TamperState_Disable  = ((uint8_t)0x00),  
  RTC_TamperState_Enable   = ((uint8_t)0x01)   
}
RTC_TamperState_TypeDef;



 
  


  
typedef enum
{
  RTC_Tamper_1 = ((uint8_t)0x02),   
  RTC_Tamper_2 = ((uint8_t)0x08),   
  RTC_Tamper_3 = ((uint8_t)0x20)    
}
RTC_Tamper_TypeDef;



 
  


  
typedef enum
{
  RTC_TamperPrechargeDuration_None    = ((uint8_t)0x80),   
  RTC_TamperPrechargeDuration_1RTCCLK = ((uint8_t)0x00),   
  RTC_TamperPrechargeDuration_2RTCCLK = ((uint8_t)0x20),   
  RTC_TamperPrechargeDuration_4RTCCLK = ((uint8_t)0x40),   
  RTC_TamperPrechargeDuration_8RTCCLK = ((uint8_t)0x60)    
}
RTC_TamperPrechargeDuration_TypeDef;



 
  


  
typedef enum
{
  RTC_TamperFilter_1Sample    = ((uint8_t)0x00),  
  RTC_TamperFilter_2Sample    = ((uint8_t)0x08),  
  RTC_TamperFilter_4Sample    = ((uint8_t)0x10),  
  RTC_TamperFilter_8Sample    = ((uint8_t)0x18)   
}
RTC_TamperFilter_TypeDef;



 
  


  
typedef enum
{
  RTC_TamperSamplingFreq_RTCCLK_Div32768  = ((uint8_t)0x00),  
  RTC_TamperSamplingFreq_RTCCLK_Div16384  = ((uint8_t)0x01),  
  RTC_TamperSamplingFreq_RTCCLK_Div8192   = ((uint8_t)0x02),  
  RTC_TamperSamplingFreq_RTCCLK_Div4096   = ((uint8_t)0x03),  
  RTC_TamperSamplingFreq_RTCCLK_Div2048   = ((uint8_t)0x04),  
  RTC_TamperSamplingFreq_RTCCLK_Div1024   = ((uint8_t)0x05),  
  RTC_TamperSamplingFreq_RTCCLK_Div512    = ((uint8_t)0x06),  
  RTC_TamperSamplingFreq_RTCCLK_Div256    = ((uint8_t)0x07)   
}
RTC_TamperSamplingFreq_TypeDef;



 
  


  
typedef enum
{
  RTC_ShiftAdd1S_Set    = ((uint8_t)0x80),  
  RTC_ShiftAdd1S_Reset  = ((uint8_t)0x00)   
}
RTC_ShiftAdd1S_TypeDef;



 
  


  
typedef enum
{
  RTC_SmoothCalibPeriod_32sec  = ((uint8_t)0x00),  
  RTC_SmoothCalibPeriod_16sec  = ((uint8_t)0x20),  
  RTC_SmoothCalibPeriod_8sec   = ((uint8_t)0x40)   
}
RTC_SmoothCalibPeriod_TypeDef;



 
  


  
typedef enum
{
  RTC_SmoothCalibPlusPulses_Set    = ((uint8_t)0x80), 
 
  RTC_SmoothCalibPlusPulses_Reset  = ((uint8_t)0x00)   

}
RTC_SmoothCalibPlusPulses_TypeDef;



 



 

typedef struct
{

  RTC_HourFormat_TypeDef RTC_HourFormat;  

  uint8_t RTC_AsynchPrediv;   

 

  uint16_t RTC_SynchPrediv;   

 
}
RTC_InitTypeDef;




 
typedef struct
{
  uint8_t RTC_Hours;       











 
  uint8_t RTC_Minutes;     



 
  uint8_t RTC_Seconds;     



 
  RTC_H12_TypeDef RTC_H12;   
}
RTC_TimeTypeDef;



 
typedef struct
{
  RTC_Weekday_TypeDef  RTC_WeekDay;  

  RTC_Month_TypeDef RTC_Month;       

  uint8_t RTC_Date;                 



 

  uint8_t RTC_Year;                 



 
}
RTC_DateTypeDef;



 
typedef struct
{
  RTC_TimeTypeDef RTC_AlarmTime;                            

  uint8_t RTC_AlarmMask;                                    

  RTC_AlarmDateWeekDaySel_TypeDef RTC_AlarmDateWeekDaySel;  

  uint8_t RTC_AlarmDateWeekDay;                            





 
}
RTC_AlarmTypeDef;
  

 
 
 


 




 





 




 




 




 




 




 




 





 




 




 




 




 




 




 




 



 




 




 





 




 


 




 




 




 




 




 




 




 




 





 




 
    



 



 




 




 
                                       



 




 




 




 
                                 


 
 

  
ErrorStatus RTC_DeInit(void);

  
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);
void RTC_WriteProtectionCmd(FunctionalState NewState);
ErrorStatus RTC_EnterInitMode(void);
void RTC_ExitInitMode(void);
ErrorStatus RTC_WaitForSynchro(void);
void RTC_RatioCmd(FunctionalState NewState);
void RTC_BypassShadowCmd(FunctionalState NewState);

  
ErrorStatus RTC_SetTime(RTC_Format_TypeDef RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_GetTime(RTC_Format_TypeDef RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
uint16_t RTC_GetSubSecond(void);
ErrorStatus RTC_SetDate(RTC_Format_TypeDef RTC_Format, RTC_DateTypeDef* RTC_DateStruct);
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct);
void RTC_GetDate(RTC_Format_TypeDef RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

  
void RTC_SetAlarm(RTC_Format_TypeDef RTC_Format,  RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_GetAlarm(RTC_Format_TypeDef RTC_Format,  RTC_AlarmTypeDef* RTC_AlarmStruct);
ErrorStatus RTC_AlarmCmd(FunctionalState NewState);
ErrorStatus RTC_AlarmSubSecondConfig(uint16_t RTC_AlarmSubSecondValue,
                                     RTC_AlarmSubSecondMask_TypeDef RTC_AlarmSubSecondMask);

  
void RTC_WakeUpClockConfig(RTC_WakeUpClock_TypeDef RTC_WakeUpClock);
void RTC_SetWakeUpCounter(uint16_t RTC_WakeupCounter);
uint16_t RTC_GetWakeUpCounter(void);
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);

  
void RTC_DayLightSavingConfig(RTC_DayLightSaving_TypeDef RTC_DayLightSaving,
                              RTC_StoreOperation_TypeDef RTC_StoreOperation);
RTC_StoreOperation_TypeDef  RTC_GetStoreOperation(void);

  
void RTC_OutputConfig(RTC_Output_TypeDef RTC_Output,
                      RTC_OutputPolarity_TypeDef RTC_OutputPolarity);

  
ErrorStatus RTC_SynchroShiftConfig(RTC_ShiftAdd1S_TypeDef RTC_ShiftAdd1S,
                                   uint16_t RTC_ShiftSubFS);

 
ErrorStatus RTC_SmoothCalibConfig(RTC_SmoothCalibPeriod_TypeDef RTC_SmoothCalibPeriod,
                                  RTC_SmoothCalibPlusPulses_TypeDef RTC_SmoothCalibPlusPulses,
                                  uint16_t RTC_SmouthCalibMinusPulsesValue);

  
void RTC_CalibOutputConfig(RTC_CalibOutput_TypeDef RTC_CalibOutput);
void RTC_CalibOutputCmd(FunctionalState NewState);

  
void RTC_TamperLevelConfig(RTC_Tamper_TypeDef RTC_Tamper,
                           RTC_TamperLevel_TypeDef RTC_TamperLevel);
void RTC_TamperFilterConfig(RTC_TamperFilter_TypeDef RTC_TamperFilter);
void RTC_TamperSamplingFreqConfig(RTC_TamperSamplingFreq_TypeDef RTC_TamperSamplingFreq);
void RTC_TamperPinsPrechargeDuration(RTC_TamperPrechargeDuration_TypeDef RTC_TamperPrechargeDuration);
void RTC_TamperCmd(RTC_Tamper_TypeDef RTC_Tamper,
                   FunctionalState NewState);

  
void RTC_ITConfig(RTC_IT_TypeDef RTC_IT, FunctionalState NewState);
FlagStatus RTC_GetFlagStatus(RTC_Flag_TypeDef RTC_FLAG);
void RTC_ClearFlag(RTC_Flag_TypeDef RTC_FLAG);
ITStatus RTC_GetITStatus(RTC_IT_TypeDef RTC_IT);
void RTC_ClearITPendingBit(RTC_IT_TypeDef RTC_IT);




 
  


 

 


























 

 

 



 
  


 
  
 


 




 
typedef enum {
  SPI_Direction_2Lines_FullDuplex = (uint8_t)0x00,  
  SPI_Direction_2Lines_RxOnly     = (uint8_t)0x04,  
  SPI_Direction_1Line_Rx          = (uint8_t)0x80,  
  SPI_Direction_1Line_Tx          = (uint8_t)0xC0   
} SPI_DirectionMode_TypeDef;



 




   
typedef enum
{
  SPI_NSS_Soft  = (uint8_t)0x02,  
  SPI_NSS_Hard  = (uint8_t)0x00   
} SPI_NSS_TypeDef;



 
  


 
typedef enum 
{
  SPI_Direction_Rx = (uint8_t)0x00,  
  SPI_Direction_Tx = (uint8_t)0x01   
} SPI_Direction_TypeDef;



 
  


 
typedef enum
{
  SPI_Mode_Master = (uint8_t)0x04,  
  SPI_Mode_Slave  = (uint8_t)0x00   
} SPI_Mode_TypeDef;



 
  


 
typedef enum {
  SPI_BaudRatePrescaler_2   = (uint8_t)0x00,  
  SPI_BaudRatePrescaler_4   = (uint8_t)0x08,  
  SPI_BaudRatePrescaler_8   = (uint8_t)0x10,  
  SPI_BaudRatePrescaler_16  = (uint8_t)0x18,  
  SPI_BaudRatePrescaler_32  = (uint8_t)0x20,  
  SPI_BaudRatePrescaler_64  = (uint8_t)0x28,  
  SPI_BaudRatePrescaler_128 = (uint8_t)0x30,  
  SPI_BaudRatePrescaler_256 = (uint8_t)0x38   
} SPI_BaudRatePrescaler_TypeDef;



 
  


 
typedef enum 
{
  SPI_CPOL_Low  = (uint8_t)0x00,  
  SPI_CPOL_High = (uint8_t)0x02   
} SPI_CPOL_TypeDef;



 
  


 
typedef enum 
{
  SPI_CPHA_1Edge = (uint8_t)0x00,  
  SPI_CPHA_2Edge = (uint8_t)0x01   
} SPI_CPHA_TypeDef;



 
  


 
typedef enum 
{
  SPI_FirstBit_MSB = (uint8_t)0x00,  
  SPI_FirstBit_LSB = (uint8_t)0x80   
} SPI_FirstBit_TypeDef;



 
  


 
typedef enum {
  SPI_DMAReq_RX = (uint8_t)0x01,     
  SPI_DMAReq_TX = (uint8_t)0x02      
} SPI_DMAReq_TypeDef;



 
  


 
typedef enum {
  SPI_CRC_RX = (uint8_t)0x00,  
  SPI_CRC_TX = (uint8_t)0x01   
} SPI_CRC_TypeDef;



 
  


 
typedef enum {
  SPI_FLAG_BSY    = (uint8_t)0x80,  
  SPI_FLAG_OVR    = (uint8_t)0x40,  
  SPI_FLAG_MODF   = (uint8_t)0x20,  
  SPI_FLAG_CRCERR = (uint8_t)0x10,  
  SPI_FLAG_WKUP   = (uint8_t)0x08,  
  SPI_FLAG_TXE    = (uint8_t)0x02,  
  SPI_FLAG_RXNE   = (uint8_t)0x01   
} SPI_FLAG_TypeDef;



 
  






 
typedef enum
{
  SPI_IT_WKUP   = (uint8_t)0x34,  
  SPI_IT_OVR    = (uint8_t)0x65,   
  SPI_IT_MODF   = (uint8_t)0x55,  
  SPI_IT_CRCERR = (uint8_t)0x45,  
  SPI_IT_TXE    = (uint8_t)0x17,  
  SPI_IT_RXNE   = (uint8_t)0x06,  
  SPI_IT_ERR    = (uint8_t)0x05   
} SPI_IT_TypeDef;



 
  


 

 
 




 



 



 



 





 



 



 



 



 




 



 



 



 




 




 




 




 



 

 
  
void SPI_DeInit(SPI_TypeDef* SPIx);
 
void SPI_Init(SPI_TypeDef* SPIx, SPI_FirstBit_TypeDef SPI_FirstBit,
              SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler,
              SPI_Mode_TypeDef SPI_Mode, SPI_CPOL_TypeDef SPI_CPOL,
              SPI_CPHA_TypeDef SPI_CPHA, SPI_DirectionMode_TypeDef SPI_Data_Direction,
              SPI_NSS_TypeDef SPI_Slave_Management, uint8_t CRCPolynomial);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_NSSInternalSoftwareCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, SPI_Direction_TypeDef SPI_Direction);

 
void SPI_SendData(SPI_TypeDef* SPIx, uint8_t Data);
uint8_t SPI_ReceiveData(SPI_TypeDef* SPIx);

 
void SPI_TransmitCRC(SPI_TypeDef* SPIx);
void SPI_CalculateCRCCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
uint8_t SPI_GetCRC(SPI_TypeDef* SPIx, SPI_CRC_TypeDef SPI_CRC);
void SPI_ResetCRC(SPI_TypeDef* SPIx);
uint8_t SPI_GetCRCPolynomial(SPI_TypeDef* SPIx);

 
void SPI_DMACmd(SPI_TypeDef* SPIx, SPI_DMAReq_TypeDef SPI_DMAReq, FunctionalState NewState);

 
void SPI_ITConfig(SPI_TypeDef* SPIx, SPI_IT_TypeDef SPI_IT, FunctionalState NewState);
FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef SPI_FLAG);
void SPI_ClearFlag(SPI_TypeDef* SPIx, SPI_FLAG_TypeDef SPI_FLAG);
ITStatus SPI_GetITStatus(SPI_TypeDef* SPIx, SPI_IT_TypeDef SPI_IT);
void SPI_ClearITPendingBit(SPI_TypeDef* SPIx, SPI_IT_TypeDef SPI_IT);




 
  


 

 


























 

 

 



 
  


  
 


 



 
typedef enum
{
  RI_InputCapture_IC2 = ((uint8_t) 0x02),  
  RI_InputCapture_IC3 = ((uint8_t) 0x03)   
}RI_InputCapture_TypeDef;



 



 
typedef enum
{
  RI_InputCaptureRouting_0  = ((uint8_t) 0x00),  
  RI_InputCaptureRouting_1  = ((uint8_t) 0x01),  
  RI_InputCaptureRouting_2  = ((uint8_t) 0x02),  
  RI_InputCaptureRouting_3  = ((uint8_t) 0x03),  
  RI_InputCaptureRouting_4  = ((uint8_t) 0x04),  
  RI_InputCaptureRouting_5  = ((uint8_t) 0x05),  
  RI_InputCaptureRouting_6  = ((uint8_t) 0x06),  
  RI_InputCaptureRouting_7  = ((uint8_t) 0x07),  
  RI_InputCaptureRouting_8  = ((uint8_t) 0x08),  
  RI_InputCaptureRouting_9  = ((uint8_t) 0x09),  
  RI_InputCaptureRouting_10 = ((uint8_t) 0x0A),  
  RI_InputCaptureRouting_11 = ((uint8_t) 0x0B),  
  RI_InputCaptureRouting_12 = ((uint8_t) 0x0C),  
  RI_InputCaptureRouting_13 = ((uint8_t) 0x0D),  
  RI_InputCaptureRouting_14 = ((uint8_t) 0x0E),  
  RI_InputCaptureRouting_15 = ((uint8_t) 0x0F),  
  RI_InputCaptureRouting_16 = ((uint8_t) 0x10),  
  RI_InputCaptureRouting_17 = ((uint8_t) 0x11),  
  RI_InputCaptureRouting_18 = ((uint8_t) 0x12),  
  RI_InputCaptureRouting_19 = ((uint8_t) 0x13),  
  RI_InputCaptureRouting_20 = ((uint8_t) 0x14),  
  RI_InputCaptureRouting_21 = ((uint8_t) 0x15),  
  RI_InputCaptureRouting_22 = ((uint8_t) 0x16)   
}RI_InputCaptureRouting_TypeDef;



 



 





 
typedef enum
{
  RI_AnalogSwitch_0  = ((uint8_t) 0x10),  
  RI_AnalogSwitch_1  = ((uint8_t) 0x11),  
  RI_AnalogSwitch_2  = ((uint8_t) 0x12),  
  RI_AnalogSwitch_3  = ((uint8_t) 0x13),  
  RI_AnalogSwitch_4  = ((uint8_t) 0x14),  
  RI_AnalogSwitch_5  = ((uint8_t) 0x15),  
  RI_AnalogSwitch_6  = ((uint8_t) 0x16),  
  RI_AnalogSwitch_7  = ((uint8_t) 0x17),  
  RI_AnalogSwitch_8  = ((uint8_t) 0x20),  
  RI_AnalogSwitch_9  = ((uint8_t) 0x21),  
  RI_AnalogSwitch_10 = ((uint8_t) 0x22),  
  RI_AnalogSwitch_11 = ((uint8_t) 0x23),  
  RI_AnalogSwitch_14 = ((uint8_t) 0x26)   
}RI_AnalogSwitch_TypeDef;



 



 





 
typedef enum
{
  RI_IOSwitch_1   = ((uint8_t) 0x10),   
  RI_IOSwitch_2   = ((uint8_t) 0x20),   
  RI_IOSwitch_3   = ((uint8_t) 0x30),   
  RI_IOSwitch_4   = ((uint8_t) 0x11),   
  RI_IOSwitch_5   = ((uint8_t) 0x21),   
  RI_IOSwitch_6   = ((uint8_t) 0x31),   
  RI_IOSwitch_7   = ((uint8_t) 0x12),   
  RI_IOSwitch_8   = ((uint8_t) 0x22),   
  RI_IOSwitch_9   = ((uint8_t) 0x32),   
  RI_IOSwitch_10  = ((uint8_t) 0x13),   
  RI_IOSwitch_11  = ((uint8_t) 0x23),   
  RI_IOSwitch_12  = ((uint8_t) 0x33),   
  RI_IOSwitch_13  = ((uint8_t) 0x14),   
  RI_IOSwitch_14  = ((uint8_t) 0x24),   
  RI_IOSwitch_15  = ((uint8_t) 0x34),   
  RI_IOSwitch_16  = ((uint8_t) 0x15),   
  RI_IOSwitch_17  = ((uint8_t) 0x25),   
  RI_IOSwitch_18  = ((uint8_t) 0x35),   
  RI_IOSwitch_19  = ((uint8_t) 0x16),   
  RI_IOSwitch_20  = ((uint8_t) 0x26),   
  RI_IOSwitch_21  = ((uint8_t) 0x36),   
  RI_IOSwitch_22  = ((uint8_t) 0x17),   
  RI_IOSwitch_23  = ((uint8_t) 0x27),   
  RI_IOSwitch_24  = ((uint8_t) 0x37),   
  RI_IOSwitch_26  = ((uint8_t) 0x41),   
  RI_IOSwitch_27  = ((uint8_t) 0x46),   
  RI_IOSwitch_28  = ((uint8_t) 0x47),   
  RI_IOSwitch_29  = ((uint8_t) 0x40)    
}RI_IOSwitch_TypeDef;



 



 


 
typedef enum
{
  RI_Resistor_10KPU  = ((uint8_t) 0x01),
  RI_Resistor_400KPU = ((uint8_t) 0x02),
  RI_Resistor_10KPD  = ((uint8_t) 0x04),
  RI_Resistor_400KPD = ((uint8_t) 0x08)
}RI_Resistor_TypeDef;



 



 








 
typedef enum
{
   
  REMAP_Pin_USART1TxRxPortA = ((uint16_t)0x011C),  
  REMAP_Pin_USART1TxRxPortC = ((uint16_t)0x012C),  
  REMAP_Pin_USART1Clk       = ((uint16_t)0x014B),  
  REMAP_Pin_SPI1Full        = ((uint16_t)0x0187), 
 
   
  REMAP_Pin_ADC1ExtTRIG1    = ((uint16_t)0x0201),  
  REMAP_Pin_TIM2TRIGPortA   = ((uint16_t)0x0202),  
  REMAP_Pin_TIM3TRIGPortA   = ((uint16_t)0x0204),  
  REMAP_Pin_TIM2TRIGLSE     = ((uint16_t)0x0208),  
  REMAP_Pin_TIM3TRIGLSE     = ((uint16_t)0x0210),  
  REMAP_Pin_SPI2Full        = ((uint16_t)0x0220), 
 
  REMAP_Pin_TIM3TRIGPortG   = ((uint16_t)0x0240),  
  REMAP_Pin_TIM23BKIN       = ((uint16_t)0x0280), 
 
   
  REMAP_Pin_SPI1PortF       = ((uint16_t)0x0301), 
 
  REMAP_Pin_USART3TxRxPortF = ((uint16_t)0x0302),  
  REMAP_Pin_USART3Clk       = ((uint16_t)0x0304),  
  REMAP_Pin_TIM3Channel1    = ((uint16_t)0x0308),  
  REMAP_Pin_TIM3Channel2    = ((uint16_t)0x0310),  
  REMAP_Pin_CCO             = ((uint16_t)0x0320),  
  REMAP_Pin_TIM2Channel1    = ((uint16_t)0x0340),  
  REMAP_Pin_TIM2Channel2    = ((uint16_t)0x0380)   
}REMAP_Pin_TypeDef;



 



 
typedef enum
{
  REMAP_DMA1Channel_ADC1ToChannel0  = ((uint8_t)0x00),  
  REMAP_DMA1Channel_ADC1ToChannel1  = ((uint8_t)0x01),  
  REMAP_DMA1Channel_ADC1ToChannel2  = ((uint8_t)0x02),  
  REMAP_DMA1Channel_ADC1ToChannel3  = ((uint8_t)0x03),  
  REMAP_DMA1Channel_TIM4ToChannel0  = ((uint8_t)0xF0),  
  REMAP_DMA1Channel_TIM4ToChannel1  = ((uint8_t)0xF4),  
  REMAP_DMA1Channel_TIM4ToChannel2  = ((uint8_t)0xF8),  
  REMAP_DMA1Channel_TIM4ToChannel3  = ((uint8_t)0xFC)   
}REMAP_DMAChannel_TypeDef;



 



 
 
 


 




 




 




 




 




 



 





 


 
  
 
 
void SYSCFG_RIDeInit(void);
void SYSCFG_RITIMInputCaptureConfig(RI_InputCapture_TypeDef RI_InputCapture,
                                    RI_InputCaptureRouting_TypeDef RI_InputCaptureRouting);
void SYSCFG_RIAnalogSwitchConfig(RI_AnalogSwitch_TypeDef RI_AnalogSwitch,
                                 FunctionalState NewState);
void SYSCFG_RIIOSwitchConfig(RI_IOSwitch_TypeDef RI_IOSwitch, FunctionalState NewState);
void SYSCFG_RIResistorConfig(RI_Resistor_TypeDef RI_Resistor, FunctionalState NewState);

 
void SYSCFG_REMAPDeInit(void);
void SYSCFG_REMAPPinConfig(REMAP_Pin_TypeDef REMAP_Pin, FunctionalState NewState);
void SYSCFG_REMAPDMAChannelConfig(REMAP_DMAChannel_TypeDef REMAP_DMAChannel);




 
  


 

 


























 

 

 



 



  
  
   


 



 
typedef enum
{
  TIM1_OCMode_Timing     = ((uint8_t)0x00),
  TIM1_OCMode_Active     = ((uint8_t)0x10),
  TIM1_OCMode_Inactive   = ((uint8_t)0x20),
  TIM1_OCMode_Toggle     = ((uint8_t)0x30),
  TIM1_OCMode_PWM1       = ((uint8_t)0x60),
  TIM1_OCMode_PWM2       = ((uint8_t)0x70)
}TIM1_OCMode_TypeDef;



 
  


 
typedef enum
{
  TIM1_OPMode_Single                 = ((uint8_t)0x01),
  TIM1_OPMode_Repetitive             = ((uint8_t)0x00)
}TIM1_OPMode_TypeDef;



 
  


 
typedef enum
{
  TIM1_Channel_1                     = ((uint8_t)0x00),
  TIM1_Channel_2                     = ((uint8_t)0x01),
  TIM1_Channel_3                     = ((uint8_t)0x02),
  TIM1_Channel_4                     = ((uint8_t)0x03)
}TIM1_Channel_TypeDef;



 
  


 
typedef enum
{
  TIM1_CounterMode_Up                = ((uint8_t)0x00),
  TIM1_CounterMode_Down              = ((uint8_t)0x10),
  TIM1_CounterMode_CenterAligned1    = ((uint8_t)0x20),
  TIM1_CounterMode_CenterAligned2    = ((uint8_t)0x40),
  TIM1_CounterMode_CenterAligned3    = ((uint8_t)0x60)
}TIM1_CounterMode_TypeDef;



 
  


 
typedef enum
{
  TIM1_OCPolarity_High               = ((uint8_t)0x00),
  TIM1_OCPolarity_Low                = ((uint8_t)0x22)
}TIM1_OCPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM1_OCNPolarity_High              = ((uint8_t)0x00),
  TIM1_OCNPolarity_Low               = ((uint8_t)0x88)
}TIM1_OCNPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM1_OutputState_Disable           = ((uint8_t)0x00),
  TIM1_OutputState_Enable            = ((uint8_t)0x11)
}TIM1_OutputState_TypeDef;



 
  


 
typedef enum
{
  TIM1_OutputNState_Disable = ((uint8_t)0x00),
  TIM1_OutputNState_Enable  = ((uint8_t)0x44)
} TIM1_OutputNState_TypeDef;



 
  


 
typedef enum
{
  TIM1_BreakState_Enable             = ((uint8_t)0x10),
  TIM1_BreakState_Disable            = ((uint8_t)0x00)
}TIM1_BreakState_TypeDef;



 
  


 
typedef enum
{
  TIM1_BreakPolarity_Low             = ((uint8_t)0x00),
  TIM1_BreakPolarity_High            = ((uint8_t)0x20)
}TIM1_BreakPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM1_AutomaticOutput_Enable        = ((uint8_t)0x40),
  TIM1_AutomaticOutput_Disable       = ((uint8_t)0x00)
}TIM1_AutomaticOutput_TypeDef;



 
  


 
typedef enum
{
  TIM1_LockLevel_Off                 = ((uint8_t)0x00),
  TIM1_LockLevel_1                   = ((uint8_t)0x01),
  TIM1_LockLevel_2                   = ((uint8_t)0x02),
  TIM1_LockLevel_3                   = ((uint8_t)0x03)
}TIM1_LockLevel_TypeDef;



 
  


 
typedef enum
{
  TIM1_OSSIState_Enable              = ((uint8_t)0x04),
  TIM1_OSSIState_Disable             = ((uint8_t)0x00)
}TIM1_OSSIState_TypeDef;



 
  


 
typedef enum
{
  TIM1_OCIdleState_Set               = ((uint8_t)0x55),
  TIM1_OCIdleState_Reset             = ((uint8_t)0x00)
}TIM1_OCIdleState_TypeDef;



 



 
typedef enum
{
  TIM1_OCNIdleState_Set             = ((uint8_t)0x2A),
  TIM1_OCNIdleState_Reset           = ((uint8_t)0x00)
}TIM1_OCNIdleState_TypeDef;



 



 
typedef enum
{
  TIM1_ICPolarity_Rising            = ((uint8_t)0x00),
  TIM1_ICPolarity_Falling           = ((uint8_t)0x01)
}TIM1_ICPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM1_ICSelection_DirectTI          = ((uint8_t)0x01),
  TIM1_ICSelection_IndirectTI        = ((uint8_t)0x02),
  TIM1_ICSelection_TRGI              = ((uint8_t)0x03)
}TIM1_ICSelection_TypeDef;



 
  


 
typedef enum
{
  TIM1_ICPSC_DIV1                    = ((uint8_t)0x00),
  TIM1_ICPSC_DIV2                    = ((uint8_t)0x04),
  TIM1_ICPSC_DIV4                    = ((uint8_t)0x08),
  TIM1_ICPSC_DIV8                    = ((uint8_t)0x0C)
}TIM1_ICPSC_TypeDef;



 
  


 
typedef enum
{
  TIM1_OCReferenceClear_ETRF       = ((uint8_t)0x08),
  TIM1_OCReferenceClear_OCREFCLR   = ((uint8_t)0x00)
}
TIM1_OCReferenceClear_TypeDef;



 
  


 
typedef enum
{
  TIM1_IT_Update                     = ((uint8_t)0x01),
  TIM1_IT_CC1                        = ((uint8_t)0x02),
  TIM1_IT_CC2                        = ((uint8_t)0x04),
  TIM1_IT_CC3                        = ((uint8_t)0x08),
  TIM1_IT_CC4                        = ((uint8_t)0x10),
  TIM1_IT_COM                        = ((uint8_t)0x20),
  TIM1_IT_Trigger                    = ((uint8_t)0x40),
  TIM1_IT_Break                      = ((uint8_t)0x80)
}TIM1_IT_TypeDef;



 
  


 
typedef enum
{
  TIM1_ExtTRGPSC_OFF                 = ((uint8_t)0x00),
  TIM1_ExtTRGPSC_DIV2                = ((uint8_t)0x10),
  TIM1_ExtTRGPSC_DIV4                = ((uint8_t)0x20),
  TIM1_ExtTRGPSC_DIV8                = ((uint8_t)0x30)
}TIM1_ExtTRGPSC_TypeDef;



 
  


 
typedef enum
{
  TIM1_TRGSelection_TIM4             = ((uint8_t)0x00),   
  TIM1_TRGSelection_TIM5             = ((uint8_t)0x10),   
  TIM1_TRGSelection_TIM3             = ((uint8_t)0x20),   
  TIM1_TRGSelection_TIM2             = ((uint8_t)0x30),   
  TIM1_TRGSelection_TI1F_ED          = ((uint8_t)0x40),
  TIM1_TRGSelection_TI1FP1           = ((uint8_t)0x50),
  TIM1_TRGSelection_TI2FP2           = ((uint8_t)0x60),
  TIM1_TRGSelection_ETRF             = ((uint8_t)0x70)
}TIM1_TRGSelection_TypeDef;



 
  


 
typedef enum
{
  TIM1_TIxExternalCLK1Source_TI1ED   = ((uint8_t)0x40),
  TIM1_TIxExternalCLK1Source_TI1     = ((uint8_t)0x50),
  TIM1_TIxExternalCLK1Source_TI2     = ((uint8_t)0x60)
}TIM1_TIxExternalCLK1Source_TypeDef;



 
  


 
typedef enum
{
  TIM1_ExtTRGPolarity_Inverted       = ((uint8_t)0x80),
  TIM1_ExtTRGPolarity_NonInverted    = ((uint8_t)0x00)
}TIM1_ExtTRGPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM1_PSCReloadMode_Update          = ((uint8_t)0x00),
  TIM1_PSCReloadMode_Immediate       = ((uint8_t)0x01)
}TIM1_PSCReloadMode_TypeDef;



 
  


 
typedef enum
{
  TIM1_EncoderMode_TI1               = ((uint8_t)0x01),
  TIM1_EncoderMode_TI2               = ((uint8_t)0x02),
  TIM1_EncoderMode_TI12              = ((uint8_t)0x03)
}TIM1_EncoderMode_TypeDef;



 
  


 
typedef enum
{
  TIM1_EventSource_Update            = ((uint8_t)0x01),
  TIM1_EventSource_CC1               = ((uint8_t)0x02),
  TIM1_EventSource_CC2               = ((uint8_t)0x04),
  TIM1_EventSource_CC3               = ((uint8_t)0x08),
  TIM1_EventSource_CC4               = ((uint8_t)0x10),
  TIM1_EventSource_COM               = ((uint8_t)0x20),
  TIM1_EventSource_Trigger           = ((uint8_t)0x40),
  TIM1_EventSource_Break             = ((uint8_t)0x80)
}TIM1_EventSource_TypeDef;



 
  


 
typedef enum
{
  TIM1_UpdateSource_Global           = ((uint8_t)0x00),
  TIM1_UpdateSource_Regular          = ((uint8_t)0x01)
}TIM1_UpdateSource_TypeDef;



 
  


 
typedef enum
{
  TIM1_TRGOSource_Reset              = ((uint8_t)0x00),
  TIM1_TRGOSource_Enable             = ((uint8_t)0x10),
  TIM1_TRGOSource_Update             = ((uint8_t)0x20),
  TIM1_TRGOSource_OC1                = ((uint8_t)0x30),
  TIM1_TRGOSource_OC1REF             = ((uint8_t)0x40),
  TIM1_TRGOSource_OC2REF             = ((uint8_t)0x50),
  TIM1_TRGOSource_OC3REF             = ((uint8_t)0x60),
  TIM1_TRGOSource_OC4REF             = ((uint8_t)0x70)
}TIM1_TRGOSource_TypeDef;



 
  


 
typedef enum
{
  TIM1_SlaveMode_Reset               = ((uint8_t)0x04),
  TIM1_SlaveMode_Gated               = ((uint8_t)0x05),
  TIM1_SlaveMode_Trigger             = ((uint8_t)0x06),
  TIM1_SlaveMode_External1           = ((uint8_t)0x07)
}TIM1_SlaveMode_TypeDef;



 



 
typedef enum
{
  TIM1_FLAG_Update                   = ((uint16_t)0x0001),
  TIM1_FLAG_CC1                      = ((uint16_t)0x0002),
  TIM1_FLAG_CC2                      = ((uint16_t)0x0004),
  TIM1_FLAG_CC3                      = ((uint16_t)0x0008),
  TIM1_FLAG_CC4                      = ((uint16_t)0x0010),
  TIM1_FLAG_COM                      = ((uint16_t)0x0020),
  TIM1_FLAG_Trigger                  = ((uint16_t)0x0040),
  TIM1_FLAG_Break                    = ((uint16_t)0x0080),
  TIM1_FLAG_CC1OF                    = ((uint16_t)0x0200),
  TIM1_FLAG_CC2OF                    = ((uint16_t)0x0400),
  TIM1_FLAG_CC3OF                    = ((uint16_t)0x0800),
  TIM1_FLAG_CC4OF                    = ((uint16_t)0x1000)
}TIM1_FLAG_TypeDef;



 
  


 
typedef enum
{
  TIM1_ForcedAction_Active           = ((uint8_t)0x50),
  TIM1_ForcedAction_Inactive         = ((uint8_t)0x40)
}TIM1_ForcedAction_TypeDef;



 
  


 
typedef enum
{
  TIM1_DMASource_Update     = ((uint8_t)0x01),   
  TIM1_DMASource_CC1        = ((uint8_t)0x02),   
  TIM1_DMASource_CC2        = ((uint8_t)0x04),   
  TIM1_DMASource_CC3        = ((uint8_t)0x08),   
  TIM1_DMASource_CC4        = ((uint8_t)0x10),   
  TIM1_DMASource_COM        = ((uint8_t)0x20)   
} TIM1_DMASource_TypeDef;



 
  


 
typedef enum
{
  TIM1_DMABase_CR1      = ((uint8_t)0x00),
  TIM1_DMABase_CR2      = ((uint8_t)0x01),
  TIM1_DMABase_SMCR     = ((uint8_t)0x02),
  TIM1_DMABase_ETR      = ((uint8_t)0x03),
  TIM1_DMABase_DER      = ((uint8_t)0x04),
  TIM1_DMABase_IER      = ((uint8_t)0x05),
  TIM1_DMABase_SR1      = ((uint8_t)0x06),
  TIM1_DMABase_SR2      = ((uint8_t)0x07),
  TIM1_DMABase_EGR      = ((uint8_t)0x08),
  TIM1_DMABase_CCMR1    = ((uint8_t)0x09),
  TIM1_DMABase_CCMR2    = ((uint8_t)0x0A),
  TIM1_DMABase_CCMR3    = ((uint8_t)0x0B),
  TIM1_DMABase_CCMR4    = ((uint8_t)0x0C),
  TIM1_DMABase_CCER1    = ((uint8_t)0x0D),
  TIM1_DMABase_CCER2    = ((uint8_t)0x0E),
  TIM1_DMABase_CNTH     = ((uint8_t)0x0F),
  TIM1_DMABase_CNTL     = ((uint8_t)0x10),
  TIM1_DMABase_PSCH     = ((uint8_t)0x11),
  TIM1_DMABase_PSCL     = ((uint8_t)0x12),
  TIM1_DMABase_ARRH     = ((uint8_t)0x13),
  TIM1_DMABase_ARRL     = ((uint8_t)0x14),
  TIM1_DMABase_RCR      = ((uint8_t)0x15),
  TIM1_DMABase_CCR1H    = ((uint8_t)0x16),
  TIM1_DMABase_CCR1L    = ((uint8_t)0x17),
  TIM1_DMABase_CCR2H    = ((uint8_t)0x18),
  TIM1_DMABase_CCR2L    = ((uint8_t)0x19),
  TIM1_DMABase_CCR3H    = ((uint8_t)0x1A),
  TIM1_DMABase_CCR3L    = ((uint8_t)0x1B),
  TIM1_DMABase_CCR4H    = ((uint8_t)0x1C),
  TIM1_DMABase_CCR4L    = ((uint8_t)0x1D),
  TIM1_DMABase_BKR      = ((uint8_t)0x1E),
  TIM1_DMABase_DTR      = ((uint8_t)0x1F)

} TIM1_DMABase_TypeDef;



 
  


 
typedef enum
{
  TIM1_DMABurstLength_1Byte      = ((uint8_t)0x00),
  TIM1_DMABurstLength_2Byte      = ((uint8_t)0x01),
  TIM1_DMABurstLength_3Byte      = ((uint8_t)0x02),
  TIM1_DMABurstLength_4Byte      = ((uint8_t)0x03),
  TIM1_DMABurstLength_5Byte      = ((uint8_t)0x04),
  TIM1_DMABurstLength_6Byte      = ((uint8_t)0x05),
  TIM1_DMABurstLength_7Byte      = ((uint8_t)0x06),
  TIM1_DMABurstLength_8Byte      = ((uint8_t)0x07),
  TIM1_DMABurstLength_9Byte      = ((uint8_t)0x08),
  TIM1_DMABurstLength_10Byte     = ((uint8_t)0x09),
  TIM1_DMABurstLength_11Byte     = ((uint8_t)0x0A),
  TIM1_DMABurstLength_12Byte     = ((uint8_t)0x0B),
  TIM1_DMABurstLength_13Byte     = ((uint8_t)0x0C),
  TIM1_DMABurstLength_14Byte     = ((uint8_t)0x0D),
  TIM1_DMABurstLength_15Byte     = ((uint8_t)0x0E),
  TIM1_DMABurstLength_16Byte     = ((uint8_t)0x0F),
  TIM1_DMABurstLength_17Byte     = ((uint8_t)0x10),
  TIM1_DMABurstLength_18Byte     = ((uint8_t)0x11),
  TIM1_DMABurstLength_19Byte     = ((uint8_t)0x12),
  TIM1_DMABurstLength_20Byte     = ((uint8_t)0x13),
  TIM1_DMABurstLength_21Byte     = ((uint8_t)0x14),
  TIM1_DMABurstLength_22Byte     = ((uint8_t)0x15),
  TIM1_DMABurstLength_23Byte     = ((uint8_t)0x16),
  TIM1_DMABurstLength_24Byte     = ((uint8_t)0x17),
  TIM1_DMABurstLength_25Byte     = ((uint8_t)0x18),
  TIM1_DMABurstLength_26Byte     = ((uint8_t)0x19),
  TIM1_DMABurstLength_27Byte     = ((uint8_t)0x1A),
  TIM1_DMABurstLength_28Byte     = ((uint8_t)0x1B),
  TIM1_DMABurstLength_29Byte     = ((uint8_t)0x1C),
  TIM1_DMABurstLength_30Byte     = ((uint8_t)0x1D),
  TIM1_DMABurstLength_31Byte     = ((uint8_t)0x1E),
  TIM1_DMABurstLength_32Byte     = ((uint8_t)0x1F)

} TIM1_DMABurstLength_TypeDef;



 
  


 

 
 


 












































 

 


 

 

 
void TIM1_DeInit(void);
void TIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       TIM1_CounterMode_TypeDef TIM1_CounterMode,
                       uint16_t TIM1_Period,
                       uint8_t TIM1_RepetitionCounter);
void TIM1_PrescalerConfig(uint16_t Prescaler,
                          TIM1_PSCReloadMode_TypeDef TIM1_PSCReloadMode);
void TIM1_CounterModeConfig(TIM1_CounterMode_TypeDef TIM1_CounterMode);
void TIM1_SetCounter(uint16_t Counter);
void TIM1_SetAutoreload(uint16_t Autoreload);
uint16_t TIM1_GetCounter(void);
uint16_t TIM1_GetPrescaler(void);
void TIM1_UpdateDisableConfig(FunctionalState NewState);
void TIM1_UpdateRequestConfig(TIM1_UpdateSource_TypeDef TIM1_UpdateSource);
void TIM1_ARRPreloadConfig(FunctionalState NewState);
void TIM1_SelectOnePulseMode(TIM1_OPMode_TypeDef TIM1_OPMode);
void TIM1_Cmd(FunctionalState NewState);

 
void TIM1_OC1Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);
void TIM1_OC2Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);
void TIM1_OC3Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);
void TIM1_BDTRConfig(TIM1_OSSIState_TypeDef TIM1_OSSIState,
                     TIM1_LockLevel_TypeDef TIM1_LockLevel,
                     uint8_t TIM1_DeadTime,
                     TIM1_BreakState_TypeDef TIM1_Break,
                     TIM1_BreakPolarity_TypeDef TIM1_BreakPolarity,
                     TIM1_AutomaticOutput_TypeDef TIM1_AutomaticOutput);
void TIM1_CtrlPWMOutputs(FunctionalState NewState);
void TIM1_SelectOCxM(TIM1_Channel_TypeDef TIM1_Channel, TIM1_OCMode_TypeDef TIM1_OCMode);
void TIM1_SetCompare1(uint16_t Compare1);
void TIM1_SetCompare2(uint16_t Compare2);
void TIM1_SetCompare3(uint16_t Compare3);
void TIM1_SetCompare4(uint16_t Compare4);
void TIM1_CCPreloadControl(FunctionalState NewState);
void TIM1_ForcedOC1Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction);
void TIM1_ForcedOC2Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction);
void TIM1_ForcedOC3Config(TIM1_ForcedAction_TypeDef TIM1_ForcedAction);
void TIM1_OC1PreloadConfig(FunctionalState NewState);
void TIM1_OC2PreloadConfig(FunctionalState NewState);
void TIM1_OC3PreloadConfig(FunctionalState NewState);
void TIM1_OC4PreloadConfig(FunctionalState NewState);
void TIM1_OC1FastConfig(FunctionalState NewState);
void TIM1_OC2FastConfig(FunctionalState NewState);
void TIM1_OC3FastConfig(FunctionalState NewState);
void TIM1_ClearOC1Ref(FunctionalState NewState);
void TIM1_ClearOC2Ref(FunctionalState NewState);
void TIM1_ClearOC3Ref(FunctionalState NewState);
void TIM1_ClearOC4Ref(FunctionalState NewState);
void TIM1_OC1PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity);
void TIM1_OC1NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity);
void TIM1_OC2PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity);
void TIM1_OC2NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity);
void TIM1_OC3PolarityConfig(TIM1_OCPolarity_TypeDef TIM1_OCPolarity);
void TIM1_OC3NPolarityConfig(TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity);
void TIM1_SelectOCREFClear(TIM1_OCReferenceClear_TypeDef TIM1_OCReferenceClear);
void TIM1_SelectCOM(FunctionalState NewState);
void TIM1_CCxCmd(TIM1_Channel_TypeDef TIM1_Channel, FunctionalState NewState);
void TIM1_CCxNCmd(TIM1_Channel_TypeDef TIM1_Channel, FunctionalState NewState);

 
void TIM1_ICInit(TIM1_Channel_TypeDef TIM1_Channel,
                 TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                 TIM1_ICSelection_TypeDef TIM1_ICSelection,
                 TIM1_ICPSC_TypeDef TIM1_ICPrescaler,
                 uint8_t TIM1_ICFilter);
void TIM1_PWMIConfig(TIM1_Channel_TypeDef TIM1_Channel,
                     TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                     TIM1_ICSelection_TypeDef TIM1_ICSelection,
                     TIM1_ICPSC_TypeDef TIM1_ICPrescaler,
                     uint8_t TIM1_ICFilter);
uint16_t TIM1_GetCapture1(void);
uint16_t TIM1_GetCapture2(void);
uint16_t TIM1_GetCapture3(void);
uint16_t TIM1_GetCapture4(void);
void TIM1_SetIC1Prescaler(TIM1_ICPSC_TypeDef TIM1_IC1Prescaler);
void TIM1_SetIC2Prescaler(TIM1_ICPSC_TypeDef TIM1_IC2Prescaler);
void TIM1_SetIC3Prescaler(TIM1_ICPSC_TypeDef TIM1_IC3Prescaler);
void TIM1_SetIC4Prescaler(TIM1_ICPSC_TypeDef TIM1_IC4Prescaler);

 
void TIM1_ITConfig(TIM1_IT_TypeDef TIM1_IT, FunctionalState NewState);
void TIM1_GenerateEvent(TIM1_EventSource_TypeDef TIM1_EventSource);
FlagStatus TIM1_GetFlagStatus(TIM1_FLAG_TypeDef TIM1_FLAG);
void TIM1_ClearFlag(TIM1_FLAG_TypeDef TIM1_FLAG);
ITStatus TIM1_GetITStatus(TIM1_IT_TypeDef TIM1_IT);
void TIM1_ClearITPendingBit(TIM1_IT_TypeDef TIM1_IT);
void TIM1_DMAConfig(TIM1_DMABase_TypeDef TIM1_DMABase,
                    TIM1_DMABurstLength_TypeDef TIM1_DMABurstLength);
void TIM1_DMACmd(TIM1_DMASource_TypeDef TIM1_DMASource, FunctionalState NewState);
void TIM1_SelectCCDMA(FunctionalState NewState);

 
void TIM1_InternalClockConfig(void);
void TIM1_TIxExternalClockConfig(TIM1_TIxExternalCLK1Source_TypeDef TIM1_TIxExternalCLKSource,
                                 TIM1_ICPolarity_TypeDef TIM1_ICPolarity,
                                 uint8_t ICFilter);
void TIM1_ETRClockMode1Config(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                              TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM1_ETRClockMode2Config(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                              TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
                              
 
void TIM1_SelectInputTrigger(TIM1_TRGSelection_TypeDef TIM1_InputTriggerSource);
void TIM1_SelectOutputTrigger(TIM1_TRGOSource_TypeDef TIM1_TRGOSource);
void TIM1_SelectSlaveMode(TIM1_SlaveMode_TypeDef TIM1_SlaveMode);
void TIM1_SelectMasterSlaveMode(FunctionalState NewState);
void TIM1_ETRConfig(TIM1_ExtTRGPSC_TypeDef TIM1_ExtTRGPrescaler,
                    TIM1_ExtTRGPolarity_TypeDef TIM1_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

 
void TIM1_EncoderInterfaceConfig(TIM1_EncoderMode_TypeDef TIM1_EncoderMode,
                                 TIM1_ICPolarity_TypeDef TIM1_IC1Polarity,
                                 TIM1_ICPolarity_TypeDef TIM1_IC2Polarity);
void TIM1_SelectHallSensor(FunctionalState NewState);




 
  


 

 


























 

 


 



 
  


 
   
 



 



 
typedef enum
{
  TIM2_ForcedAction_Active   = ((uint8_t)0x50),    
  TIM2_ForcedAction_Inactive = ((uint8_t)0x40)     
}
TIM2_ForcedAction_TypeDef;



 
  


 
typedef enum
{
  TIM2_Prescaler_1     = ((uint8_t)0x00),    
  TIM2_Prescaler_2     = ((uint8_t)0x01),    
  TIM2_Prescaler_4     = ((uint8_t)0x02),    
  TIM2_Prescaler_8     = ((uint8_t)0x03),    
  TIM2_Prescaler_16    = ((uint8_t)0x04),    
  TIM2_Prescaler_32    = ((uint8_t)0x05),    
  TIM2_Prescaler_64    = ((uint8_t)0x06),    
  TIM2_Prescaler_128   = ((uint8_t)0x07)     
}TIM2_Prescaler_TypeDef;



 
  


 
typedef enum
{
  TIM2_OCMode_Timing    = ((uint8_t)0x00),    
  TIM2_OCMode_Active    = ((uint8_t)0x10),    
  TIM2_OCMode_Inactive  = ((uint8_t)0x20),    
  TIM2_OCMode_Toggle    = ((uint8_t)0x30),    
  TIM2_OCMode_PWM1      = ((uint8_t)0x60),    
  TIM2_OCMode_PWM2      = ((uint8_t)0x70)     
}TIM2_OCMode_TypeDef;



 
  


 
typedef enum
{
  TIM2_OPMode_Single      = ((uint8_t)0x01),  
  TIM2_OPMode_Repetitive  = ((uint8_t)0x00)   
}TIM2_OPMode_TypeDef;



 
  


 
typedef enum
{
  TIM2_Channel_1  = ((uint8_t)0x00),   
  TIM2_Channel_2  = ((uint8_t)0x01)    
}TIM2_Channel_TypeDef;



 
  


 
typedef enum
{
  TIM2_CounterMode_Up               = ((uint8_t)0x00),    
  TIM2_CounterMode_Down             = ((uint8_t)0x10),    
  TIM2_CounterMode_CenterAligned1   = ((uint8_t)0x20),    
  TIM2_CounterMode_CenterAligned2   = ((uint8_t)0x40),    
  TIM2_CounterMode_CenterAligned3   = ((uint8_t)0x60)     
}TIM2_CounterMode_TypeDef;



 
  


 
typedef enum
{
  TIM2_OCPolarity_High   = ((uint8_t)0x00),    
  TIM2_OCPolarity_Low    = ((uint8_t)0x01)     
}TIM2_OCPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM2_OutputState_Disable   = ((uint8_t)0x00),    
  TIM2_OutputState_Enable    = ((uint8_t)0x01)     
}TIM2_OutputState_TypeDef;



 
  


 
typedef enum
{
  TIM2_BreakState_Disable  = ((uint8_t)0x00),    
  TIM2_BreakState_Enable   = ((uint8_t)0x10)     

}TIM2_BreakState_TypeDef;



 
  


 
typedef enum
{
  TIM2_BreakPolarity_High  = ((uint8_t)0x20),   
  TIM2_BreakPolarity_Low   = ((uint8_t)0x00)    

}TIM2_BreakPolarity_TypeDef;



 
  


   
typedef enum
{
  TIM2_AutomaticOutput_Enable    = ((uint8_t)0x40),    
  TIM2_AutomaticOutput_Disable   = ((uint8_t)0x00)     
}TIM2_AutomaticOutput_TypeDef;



 
  


 
typedef enum
{
  TIM2_LockLevel_Off  = ((uint8_t)0x00),    
  TIM2_LockLevel_1    = ((uint8_t)0x01),    
  TIM2_LockLevel_2    = ((uint8_t)0x02),    
  TIM2_LockLevel_3    = ((uint8_t)0x03)     
}TIM2_LockLevel_TypeDef;



 
  


 
typedef enum
{
  TIM2_OSSIState_Enable    = ((uint8_t)0x04),    
  TIM2_OSSIState_Disable   = ((uint8_t)0x00)     
}TIM2_OSSIState_TypeDef;



 
  


 
typedef enum
{
  TIM2_OCIdleState_Reset  = ((uint8_t)0x00),    
  TIM2_OCIdleState_Set    = ((uint8_t)0x01)     
}TIM2_OCIdleState_TypeDef;



 
  


 
typedef enum
{
  TIM2_ICPolarity_Rising   = ((uint8_t)0x00),    
  TIM2_ICPolarity_Falling  = ((uint8_t)0x01)     
}TIM2_ICPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM2_ICSelection_DirectTI    = ((uint8_t)0x01),    
  TIM2_ICSelection_IndirectTI  = ((uint8_t)0x02),    
  TIM2_ICSelection_TRGI        = ((uint8_t)0x03)     
}TIM2_ICSelection_TypeDef;



 
  


 
typedef enum
{
  TIM2_ICPSC_DIV1  = ((uint8_t)0x00),   
  TIM2_ICPSC_DIV2  = ((uint8_t)0x04),   
  TIM2_ICPSC_DIV4  = ((uint8_t)0x08),   
  TIM2_ICPSC_DIV8  = ((uint8_t)0x0C)    
}TIM2_ICPSC_TypeDef;



 
  


 
typedef enum
{
  TIM2_IT_Update   = ((uint8_t)0x01),    
  TIM2_IT_CC1      = ((uint8_t)0x02),    
  TIM2_IT_CC2      = ((uint8_t)0x04),    
  TIM2_IT_Trigger  = ((uint8_t)0x40),    
  TIM2_IT_Break    = ((uint8_t)0x80)     
}TIM2_IT_TypeDef;



 
  


 
typedef enum
{
  TIM2_ExtTRGPSC_OFF   = ((uint8_t)0x00),    
  TIM2_ExtTRGPSC_DIV2  = ((uint8_t)0x10),    
  TIM2_ExtTRGPSC_DIV4  = ((uint8_t)0x20),    
  TIM2_ExtTRGPSC_DIV8  = ((uint8_t)0x30)     
}TIM2_ExtTRGPSC_TypeDef;



 
  


 
typedef enum
{
  TIM2_TRGSelection_TIM4     = ((uint8_t)0x00),   
  TIM2_TRGSelection_TIM1     = ((uint8_t)0x10),   
  TIM2_TRGSelection_TIM3     = ((uint8_t)0x20),   
  TIM2_TRGSelection_TIM5     = ((uint8_t)0x30),   
  TIM2_TRGSelection_TI1F_ED  = ((uint8_t)0x40),   
  TIM2_TRGSelection_TI1FP1   = ((uint8_t)0x50),   
  TIM2_TRGSelection_TI2FP2   = ((uint8_t)0x60),   
  TIM2_TRGSelection_ETRF     = ((uint8_t)0x70)    
}TIM2_TRGSelection_TypeDef;



 
  


 
typedef enum
{
  TIM2_TIxExternalCLK1Source_TI1ED  = ((uint8_t)0x40),    
  TIM2_TIxExternalCLK1Source_TI1    = ((uint8_t)0x50),    
  TIM2_TIxExternalCLK1Source_TI2    = ((uint8_t)0x60)     
}TIM2_TIxExternalCLK1Source_TypeDef;



 
  


 
typedef enum
{
  TIM2_ExtTRGPolarity_Inverted     = ((uint8_t)0x80),    
  TIM2_ExtTRGPolarity_NonInverted  = ((uint8_t)0x00)     
}TIM2_ExtTRGPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM2_PSCReloadMode_Update      = ((uint8_t)0x00),    
  TIM2_PSCReloadMode_Immediate   = ((uint8_t)0x01)     
}TIM2_PSCReloadMode_TypeDef;



 
  


 
typedef enum
{
  TIM2_EncoderMode_TI1     = ((uint8_t)0x01),    
  TIM2_EncoderMode_TI2     = ((uint8_t)0x02),    
  TIM2_EncoderMode_TI12    = ((uint8_t)0x03)     
}TIM2_EncoderMode_TypeDef;



 
  


 
typedef enum
{
  TIM2_EventSource_Update   = ((uint8_t)0x01),    
  TIM2_EventSource_CC1      = ((uint8_t)0x02),    
  TIM2_EventSource_CC2      = ((uint8_t)0x04),    
  TIM2_EventSource_Trigger  = ((uint8_t)0x40),    
  TIM2_EventSource_Break    = ((uint8_t)0x80)     
}TIM2_EventSource_TypeDef;



 
  


 
typedef enum
{
  TIM2_UpdateSource_Global   = ((uint8_t)0x00),    
  TIM2_UpdateSource_Regular  = ((uint8_t)0x01)     
}TIM2_UpdateSource_TypeDef;



 
  


 
typedef enum
{
  TIM2_TRGOSource_Reset    = ((uint8_t)0x00),    
  TIM2_TRGOSource_Enable   = ((uint8_t)0x10),    
  TIM2_TRGOSource_Update   = ((uint8_t)0x20),    
  TIM2_TRGOSource_OC1      = ((uint8_t)0x30),    
  TIM2_TRGOSource_OC1REF   = ((uint8_t)0x40),    
  TIM2_TRGOSource_OC2REF   = ((uint8_t)0x50)     
}TIM2_TRGOSource_TypeDef;



 
  


 
typedef enum
{
  TIM2_SlaveMode_Reset      = ((uint8_t)0x04),   
  TIM2_SlaveMode_Gated      = ((uint8_t)0x05),   
  TIM2_SlaveMode_Trigger    = ((uint8_t)0x06),   
  TIM2_SlaveMode_External1  = ((uint8_t)0x07)    
}TIM2_SlaveMode_TypeDef;



 
  


 
typedef enum
{
  TIM2_FLAG_Update   = ((uint16_t)0x0001),   
  TIM2_FLAG_CC1      = ((uint16_t)0x0002),   
  TIM2_FLAG_CC2      = ((uint16_t)0x0004),   
  TIM2_FLAG_Trigger  = ((uint16_t)0x0040),   
  TIM2_FLAG_Break    = ((uint16_t)0x0080),   
  TIM2_FLAG_CC1OF    = ((uint16_t)0x0200),   
  TIM2_FLAG_CC2OF    = ((uint16_t)0x0400)    
}TIM2_FLAG_TypeDef;



 
  


 
typedef enum
{
  TIM2_DMASource_Update   = ((uint8_t)0x01),   
  TIM2_DMASource_CC1      = ((uint8_t)0x02),   
  TIM2_DMASource_CC2      = ((uint8_t)0x04)    
}TIM2_DMASource_TypeDef;



 
  


 

 
 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 




 



 



 



 




 


 





 



 



 



 



 



 



 



 


 


 




 




 

 
 
void TIM2_DeInit(void);
void TIM2_TimeBaseInit(TIM2_Prescaler_TypeDef TIM2_Prescaler,
                       TIM2_CounterMode_TypeDef TIM2_CounterMode, uint16_t TIM2_Period);
void TIM2_PrescalerConfig(TIM2_Prescaler_TypeDef Prescaler,
                          TIM2_PSCReloadMode_TypeDef TIM2_PSCReloadMode);
void TIM2_CounterModeConfig(TIM2_CounterMode_TypeDef TIM2_CounterMode);
void TIM2_SetCounter(uint16_t Counter);
void TIM2_SetAutoreload(uint16_t Autoreload);
uint16_t TIM2_GetCounter(void);
TIM2_Prescaler_TypeDef TIM2_GetPrescaler(void);
void TIM2_UpdateDisableConfig(FunctionalState NewState);
void TIM2_UpdateRequestConfig(TIM2_UpdateSource_TypeDef TIM2_UpdateSource);
void TIM2_ARRPreloadConfig(FunctionalState NewState);
void TIM2_SelectOnePulseMode(TIM2_OPMode_TypeDef TIM2_OPMode);
void TIM2_Cmd(FunctionalState NewState);

 
void TIM2_OC1Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState);
void TIM2_OC2Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity,
                  TIM2_OCIdleState_TypeDef TIM2_OCIdleState);
void TIM2_BKRConfig(TIM2_OSSIState_TypeDef TIM2_OSSIState,
                    TIM2_LockLevel_TypeDef TIM2_LockLevel,
                    TIM2_BreakState_TypeDef TIM2_BreakState,
                    TIM2_BreakPolarity_TypeDef TIM2_BreakPolarity,
                    TIM2_AutomaticOutput_TypeDef TIM2_AutomaticOutput);
void TIM2_CtrlPWMOutputs(FunctionalState NewState);
void TIM2_SelectOCxM(TIM2_Channel_TypeDef TIM2_Channel, TIM2_OCMode_TypeDef TIM2_OCMode);
void TIM2_SetCompare1(uint16_t Compare);
void TIM2_SetCompare2(uint16_t Compare);
void TIM2_ForcedOC1Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction);
void TIM2_ForcedOC2Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction);
void TIM2_OC1PreloadConfig(FunctionalState NewState);
void TIM2_OC2PreloadConfig(FunctionalState NewState);
void TIM2_OC1FastConfig(FunctionalState NewState);
void TIM2_OC2FastConfig(FunctionalState NewState);
void TIM2_OC1PolarityConfig(TIM2_OCPolarity_TypeDef TIM2_OCPolarity);
void TIM2_OC2PolarityConfig(TIM2_OCPolarity_TypeDef TIM2_OCPolarity);
void TIM2_CCxCmd(TIM2_Channel_TypeDef TIM2_Channel, FunctionalState NewState);

 
void TIM2_ICInit(TIM2_Channel_TypeDef TIM2_Channel,
                 TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                 TIM2_ICSelection_TypeDef TIM2_ICSelection,
                 TIM2_ICPSC_TypeDef TIM2_ICPrescaler,
                 uint8_t TIM2_ICFilter);
void TIM2_PWMIConfig(TIM2_Channel_TypeDef TIM2_Channel,
                     TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                     TIM2_ICSelection_TypeDef TIM2_ICSelection,
                     TIM2_ICPSC_TypeDef TIM2_ICPrescaler,
                     uint8_t TIM2_ICFilter);
uint16_t TIM2_GetCapture1(void);
uint16_t TIM2_GetCapture2(void);
void TIM2_SetIC1Prescaler(TIM2_ICPSC_TypeDef TIM2_IC1Prescaler);
void TIM2_SetIC2Prescaler(TIM2_ICPSC_TypeDef TIM2_IC2Prescaler);

 
void TIM2_ITConfig(TIM2_IT_TypeDef TIM2_IT, FunctionalState NewState);
void TIM2_GenerateEvent(TIM2_EventSource_TypeDef TIM2_EventSource);
FlagStatus TIM2_GetFlagStatus(TIM2_FLAG_TypeDef TIM2_FLAG);
void TIM2_ClearFlag(TIM2_FLAG_TypeDef TIM2_FLAG);
ITStatus TIM2_GetITStatus(TIM2_IT_TypeDef TIM2_IT);
void TIM2_ClearITPendingBit(TIM2_IT_TypeDef TIM2_IT);
void TIM2_DMACmd(TIM2_DMASource_TypeDef TIM2_DMASource, FunctionalState NewState);
void TIM2_SelectCCDMA(FunctionalState NewState);

 
void TIM2_InternalClockConfig(void);
void TIM2_TIxExternalClockConfig(TIM2_TIxExternalCLK1Source_TypeDef TIM2_TIxExternalCLKSource,
                                 TIM2_ICPolarity_TypeDef TIM2_ICPolarity,
                                 uint8_t ICFilter);
void TIM2_ETRClockMode1Config(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                              TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM2_ETRClockMode2Config(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                              TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);

 
void TIM2_SelectInputTrigger(TIM2_TRGSelection_TypeDef TIM2_InputTriggerSource);
void TIM2_SelectOutputTrigger(TIM2_TRGOSource_TypeDef TIM2_TRGOSource);
void TIM2_SelectSlaveMode(TIM2_SlaveMode_TypeDef TIM2_SlaveMode);
void TIM2_SelectMasterSlaveMode(FunctionalState NewState);
void TIM2_ETRConfig(TIM2_ExtTRGPSC_TypeDef TIM2_ExtTRGPrescaler,
                    TIM2_ExtTRGPolarity_TypeDef TIM2_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

 
void TIM2_EncoderInterfaceConfig(TIM2_EncoderMode_TypeDef TIM2_EncoderMode,
                                 TIM2_ICPolarity_TypeDef TIM2_IC1Polarity,
                                 TIM2_ICPolarity_TypeDef TIM2_IC2Polarity);
void TIM2_SelectHallSensor(FunctionalState NewState);




 
  


 

 


























 

 


 



 



  
  
 



 



 
typedef enum
{
  TIM3_ForcedAction_Active    = ((uint8_t)0x50),  
  TIM3_ForcedAction_Inactive  = ((uint8_t)0x40)   
}
TIM3_ForcedAction_TypeDef;



 



 
typedef enum
{
  TIM3_Prescaler_1     = ((uint8_t)0x00),  
  TIM3_Prescaler_2     = ((uint8_t)0x01),  
  TIM3_Prescaler_4     = ((uint8_t)0x02),  
  TIM3_Prescaler_8     = ((uint8_t)0x03),  
  TIM3_Prescaler_16    = ((uint8_t)0x04),  
  TIM3_Prescaler_32    = ((uint8_t)0x05),  
  TIM3_Prescaler_64    = ((uint8_t)0x06),  
  TIM3_Prescaler_128   = ((uint8_t)0x07)   
}TIM3_Prescaler_TypeDef;



 



 
typedef enum
{
  TIM3_OCMode_Timing    = ((uint8_t)0x00),  
  TIM3_OCMode_Active    = ((uint8_t)0x10),  
  TIM3_OCMode_Inactive  = ((uint8_t)0x20),  
  TIM3_OCMode_Toggle    = ((uint8_t)0x30),  
  TIM3_OCMode_PWM1      = ((uint8_t)0x60),  
  TIM3_OCMode_PWM2      = ((uint8_t)0x70)   
}TIM3_OCMode_TypeDef;



 



 
typedef enum
{
  TIM3_OPMode_Single      = ((uint8_t)0x01),  
  TIM3_OPMode_Repetitive  = ((uint8_t)0x00)   
}TIM3_OPMode_TypeDef;



 



 
typedef enum
{
  TIM3_Channel_1     = ((uint8_t)0x00),   
  TIM3_Channel_2     = ((uint8_t)0x01)    
}TIM3_Channel_TypeDef;



 



 
typedef enum
{
  TIM3_CounterMode_Up              = ((uint8_t)0x00),  
  TIM3_CounterMode_Down            = ((uint8_t)0x10),  
  TIM3_CounterMode_CenterAligned1  = ((uint8_t)0x20),  
  TIM3_CounterMode_CenterAligned2  = ((uint8_t)0x40),  
  TIM3_CounterMode_CenterAligned3  = ((uint8_t)0x60)   
}TIM3_CounterMode_TypeDef;



 



 
typedef enum
{
  TIM3_OCPolarity_High   = ((uint8_t)0x00),  
  TIM3_OCPolarity_Low    = ((uint8_t)0x01)   
}TIM3_OCPolarity_TypeDef;



 



 
typedef enum
{
  TIM3_OutputState_Disable  = ((uint8_t)0x00), 
 
  TIM3_OutputState_Enable   = ((uint8_t)0x01)  
 
}TIM3_OutputState_TypeDef;



 



 
typedef enum
{
  TIM3_BreakState_Disable   = ((uint8_t)0x00),  
  TIM3_BreakState_Enable    = ((uint8_t)0x10)   
}TIM3_BreakState_TypeDef;



 



 
typedef enum
{
  TIM3_BreakPolarity_High   = ((uint8_t)0x20),   
  TIM3_BreakPolarity_Low    = ((uint8_t)0x00)    
}TIM3_BreakPolarity_TypeDef;



 



 
typedef enum
{
  TIM3_AutomaticOutput_Enable   = ((uint8_t)0x40),  
  TIM3_AutomaticOutput_Disable  = ((uint8_t)0x00)   
}TIM3_AutomaticOutput_TypeDef;



 



 
typedef enum
{
  TIM3_LockLevel_Off        = ((uint8_t)0x00),  
  TIM3_LockLevel_1          = ((uint8_t)0x01),  
  TIM3_LockLevel_2          = ((uint8_t)0x02),  
  TIM3_LockLevel_3          = ((uint8_t)0x03)   
}TIM3_LockLevel_TypeDef;



 



 
typedef enum
{
  TIM3_OSSIState_Enable   = ((uint8_t)0x04),  
  TIM3_OSSIState_Disable  = ((uint8_t)0x00)   
}TIM3_OSSIState_TypeDef;



 



 
typedef enum
{
  TIM3_OCIdleState_Reset  = ((uint8_t)0x00),  
  TIM3_OCIdleState_Set    = ((uint8_t)0x01)   
}TIM3_OCIdleState_TypeDef;



 



 
typedef enum
{
  TIM3_ICPolarity_Rising  = ((uint8_t)0x00),  
  TIM3_ICPolarity_Falling  = ((uint8_t)0x01)   
}TIM3_ICPolarity_TypeDef;



 



 
typedef enum
{
  TIM3_ICSelection_DirectTI    = ((uint8_t)0x01),  
  TIM3_ICSelection_IndirectTI  = ((uint8_t)0x02),  
  TIM3_ICSelection_TRGI        = ((uint8_t)0x03)   
}TIM3_ICSelection_TypeDef;



 



 
typedef enum
{
  TIM3_ICPSC_DIV1  = ((uint8_t)0x00),   
  TIM3_ICPSC_DIV2  = ((uint8_t)0x04),   
  TIM3_ICPSC_DIV4  = ((uint8_t)0x08),   
  TIM3_ICPSC_DIV8  = ((uint8_t)0x0C)    
}TIM3_ICPSC_TypeDef;



 



 
typedef enum
{
  TIM3_IT_Update   = ((uint8_t)0x01),  
  TIM3_IT_CC1      = ((uint8_t)0x02),  
  TIM3_IT_CC2      = ((uint8_t)0x04),  
  TIM3_IT_Trigger  = ((uint8_t)0x40),  
  TIM3_IT_Break    = ((uint8_t)0x80)   
}TIM3_IT_TypeDef;



 



 
typedef enum
{
  TIM3_ExtTRGPSC_OFF   = ((uint8_t)0x00),  
  TIM3_ExtTRGPSC_DIV2  = ((uint8_t)0x10),  
  TIM3_ExtTRGPSC_DIV4  = ((uint8_t)0x20),  
  TIM3_ExtTRGPSC_DIV8  = ((uint8_t)0x30)   
}TIM3_ExtTRGPSC_TypeDef;



 



 
typedef enum
{
  TIM3_TRGSelection_TIM4     = ((uint8_t)0x00),  
  TIM3_TRGSelection_TIM1     = ((uint8_t)0x10),  
  TIM3_TRGSelection_TIM5     = ((uint8_t)0x20),  
  TIM3_TRGSelection_TIM2     = ((uint8_t)0x30),  
  TIM3_TRGSelection_TI1F_ED  = ((uint8_t)0x40),  
  TIM3_TRGSelection_TI1FP1   = ((uint8_t)0x50),  
  TIM3_TRGSelection_TI2FP2   = ((uint8_t)0x60),  
  TIM3_TRGSelection_ETRF     = ((uint8_t)0x70)   
}TIM3_TRGSelection_TypeDef;



 



 
typedef enum
{
  TIM3_TIxExternalCLK1Source_TI1ED  = ((uint8_t)0x40),   
  TIM3_TIxExternalCLK1Source_TI1    = ((uint8_t)0x50),   
  TIM3_TIxExternalCLK1Source_TI2    = ((uint8_t)0x60)    
}TIM3_TIxExternalCLK1Source_TypeDef;



 



 
typedef enum
{
  TIM3_ExtTRGPolarity_Inverted     = ((uint8_t)0x80),  
  TIM3_ExtTRGPolarity_NonInverted  = ((uint8_t)0x00)   
}TIM3_ExtTRGPolarity_TypeDef;



 



 
typedef enum
{
  TIM3_PSCReloadMode_Update     = ((uint8_t)0x00),  
  TIM3_PSCReloadMode_Immediate  = ((uint8_t)0x01)   
}TIM3_PSCReloadMode_TypeDef;



 



 
typedef enum
{
  TIM3_EncoderMode_TI1    = ((uint8_t)0x01),  
  TIM3_EncoderMode_TI2    = ((uint8_t)0x02),  
  TIM3_EncoderMode_TI12   = ((uint8_t)0x03)   
}TIM3_EncoderMode_TypeDef;



 



 
typedef enum
{
  TIM3_EventSource_Update   = ((uint8_t)0x01),  
  TIM3_EventSource_CC1      = ((uint8_t)0x02),  
  TIM3_EventSource_CC2      = ((uint8_t)0x04),  
  TIM3_EventSource_Trigger  = ((uint8_t)0x40),  
  TIM3_EventSource_Break    = ((uint8_t)0x80)   
}TIM3_EventSource_TypeDef;



 



 
typedef enum
{
  TIM3_UpdateSource_Global  = ((uint8_t)0x00),  
  TIM3_UpdateSource_Regular = ((uint8_t)0x01)   
}TIM3_UpdateSource_TypeDef;



 



 
typedef enum
{
  TIM3_TRGOSource_Reset   = ((uint8_t)0x00),  
  TIM3_TRGOSource_Enable  = ((uint8_t)0x10),  
  TIM3_TRGOSource_Update  = ((uint8_t)0x20),  
  TIM3_TRGOSource_OC1     = ((uint8_t)0x30),  
  TIM3_TRGOSource_OC1REF  = ((uint8_t)0x40),  
  TIM3_TRGOSource_OC2REF  = ((uint8_t)0x50)   
}TIM3_TRGOSource_TypeDef;



 



 
typedef enum
{
  TIM3_SlaveMode_Reset      = ((uint8_t)0x04),  
  TIM3_SlaveMode_Gated      = ((uint8_t)0x05),  
  TIM3_SlaveMode_Trigger    = ((uint8_t)0x06),  
  TIM3_SlaveMode_External1  = ((uint8_t)0x07)   
}TIM3_SlaveMode_TypeDef;



 



 
typedef enum
{
  TIM3_FLAG_Update   = ((uint16_t)0x0001),  
  TIM3_FLAG_CC1      = ((uint16_t)0x0002),  
  TIM3_FLAG_CC2      = ((uint16_t)0x0004),  
  TIM3_FLAG_Trigger  = ((uint16_t)0x0040),  
  TIM3_FLAG_Break    = ((uint16_t)0x0080),  
  TIM3_FLAG_CC1OF    = ((uint16_t)0x0200),  
  TIM3_FLAG_CC2OF    = ((uint16_t)0x0400)   
}TIM3_FLAG_TypeDef;



 



 
typedef enum
{
  TIM3_DMASource_Update   = ((uint8_t)0x01),   
  TIM3_DMASource_CC1      = ((uint8_t)0x02),
  TIM3_DMASource_CC2      = ((uint8_t)0x04)
}TIM3_DMASource_TypeDef;



 



 

 
 



 



 



 



 



 



 



 



 



 



 




 



 



 



 



 



 



 




 




 



 



 




 



 





 



 



 



 



 



 



 



 


 


 




 




 

 
 
void TIM3_DeInit(void);
void TIM3_TimeBaseInit(TIM3_Prescaler_TypeDef TIM3_Prescaler,
                       TIM3_CounterMode_TypeDef TIM3_CounterMode,
                       uint16_t TIM3_Period);
void TIM3_PrescalerConfig(TIM3_Prescaler_TypeDef Prescaler,
                          TIM3_PSCReloadMode_TypeDef TIM3_PSCReloadMode);
void TIM3_CounterModeConfig(TIM3_CounterMode_TypeDef TIM3_CounterMode);
void TIM3_SetCounter(uint16_t Counter);
void TIM3_SetAutoreload(uint16_t Autoreload);
uint16_t TIM3_GetCounter(void);
TIM3_Prescaler_TypeDef TIM3_GetPrescaler(void);
void TIM3_UpdateDisableConfig(FunctionalState NewState);
void TIM3_UpdateRequestConfig(TIM3_UpdateSource_TypeDef TIM3_UpdateSource);
void TIM3_ARRPreloadConfig(FunctionalState NewState);
void TIM3_SelectOnePulseMode(TIM3_OPMode_TypeDef TIM3_OPMode);
void TIM3_Cmd(FunctionalState NewState);

 
void TIM3_OC1Init(TIM3_OCMode_TypeDef TIM3_OCMode,
                  TIM3_OutputState_TypeDef TIM3_OutputState,
                  uint16_t TIM3_Pulse,
                  TIM3_OCPolarity_TypeDef TIM3_OCPolarity,
                  TIM3_OCIdleState_TypeDef TIM3_OCIdleState);
void TIM3_OC2Init(TIM3_OCMode_TypeDef TIM3_OCMode,
                  TIM3_OutputState_TypeDef TIM3_OutputState,
                  uint16_t TIM3_Pulse,
                  TIM3_OCPolarity_TypeDef TIM3_OCPolarity,
                  TIM3_OCIdleState_TypeDef TIM3_OCIdleState);
void TIM3_BKRConfig(TIM3_OSSIState_TypeDef TIM3_OSSIState,
                    TIM3_LockLevel_TypeDef TIM3_LockLevel,
                    TIM3_BreakState_TypeDef TIM3_BreakState,
                    TIM3_BreakPolarity_TypeDef TIM3_BreakPolarity,
                    TIM3_AutomaticOutput_TypeDef TIM3_AutomaticOutput);
void TIM3_CtrlPWMOutputs(FunctionalState NewState);
void TIM3_SelectOCxM(TIM3_Channel_TypeDef TIM3_Channel,
                     TIM3_OCMode_TypeDef TIM3_OCMode);
void TIM3_SetCompare1(uint16_t Compare);
void TIM3_SetCompare2(uint16_t Compare);
void TIM3_ForcedOC1Config(TIM3_ForcedAction_TypeDef TIM3_ForcedAction);
void TIM3_ForcedOC2Config(TIM3_ForcedAction_TypeDef TIM3_ForcedAction);
void TIM3_OC1PreloadConfig(FunctionalState NewState);
void TIM3_OC2PreloadConfig(FunctionalState NewState);
void TIM3_OC1FastConfig(FunctionalState NewState);
void TIM3_OC2FastConfig(FunctionalState NewState);
void TIM3_OC1PolarityConfig(TIM3_OCPolarity_TypeDef TIM3_OCPolarity);
void TIM3_OC2PolarityConfig(TIM3_OCPolarity_TypeDef TIM3_OCPolarity);
void TIM3_CCxCmd(TIM3_Channel_TypeDef TIM3_Channel, FunctionalState NewState);

 
void TIM3_ICInit(TIM3_Channel_TypeDef TIM3_Channel,
                 TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                 TIM3_ICSelection_TypeDef TIM3_ICSelection,
                 TIM3_ICPSC_TypeDef TIM3_ICPrescaler,
                 uint8_t TIM3_ICFilter);
void TIM3_PWMIConfig(TIM3_Channel_TypeDef TIM3_Channel,
                     TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                     TIM3_ICSelection_TypeDef TIM3_ICSelection,
                     TIM3_ICPSC_TypeDef TIM3_ICPrescaler,
                     uint8_t TIM3_ICFilter);
uint16_t TIM3_GetCapture1(void);
uint16_t TIM3_GetCapture2(void);
void TIM3_SetIC1Prescaler(TIM3_ICPSC_TypeDef TIM3_IC1Prescaler);
void TIM3_SetIC2Prescaler(TIM3_ICPSC_TypeDef TIM3_IC2Prescaler);

 
void TIM3_ITConfig(TIM3_IT_TypeDef TIM3_IT, FunctionalState NewState);
void TIM3_GenerateEvent(TIM3_EventSource_TypeDef TIM3_EventSource);
FlagStatus TIM3_GetFlagStatus(TIM3_FLAG_TypeDef TIM3_FLAG);
void TIM3_ClearFlag(TIM3_FLAG_TypeDef TIM3_FLAG);
ITStatus TIM3_GetITStatus(TIM3_IT_TypeDef TIM3_IT);
void TIM3_ClearITPendingBit(TIM3_IT_TypeDef TIM3_IT);
void TIM3_DMACmd(TIM3_DMASource_TypeDef TIM3_DMASource, FunctionalState NewState);
void TIM3_SelectCCDMA(FunctionalState NewState);

 
void TIM3_InternalClockConfig(void);
void TIM3_TIxExternalClockConfig(TIM3_TIxExternalCLK1Source_TypeDef TIM3_TIxExternalCLKSource,
                                 TIM3_ICPolarity_TypeDef TIM3_ICPolarity,
                                 uint8_t ICFilter);
void TIM3_ETRClockMode1Config(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                              TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM3_ETRClockMode2Config(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                              TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);

 
void TIM3_SelectInputTrigger(TIM3_TRGSelection_TypeDef TIM3_InputTriggerSource);
void TIM3_SelectOutputTrigger(TIM3_TRGOSource_TypeDef TIM3_TRGOSource);
void TIM3_SelectSlaveMode(TIM3_SlaveMode_TypeDef TIM3_SlaveMode);
void TIM3_SelectMasterSlaveMode(FunctionalState NewState);
void TIM3_ETRConfig(TIM3_ExtTRGPSC_TypeDef TIM3_ExtTRGPrescaler,
                    TIM3_ExtTRGPolarity_TypeDef TIM3_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

 
void TIM3_EncoderInterfaceConfig(TIM3_EncoderMode_TypeDef TIM3_EncoderMode,
                                 TIM3_ICPolarity_TypeDef TIM3_IC1Polarity,
                                 TIM3_ICPolarity_TypeDef TIM3_IC2Polarity);
void TIM3_SelectHallSensor(FunctionalState NewState);




 
  


 

 


























 

 

 



 



  
  
 



 




 
typedef enum
{
  TIM4_Prescaler_1      = ((uint8_t)0x00),    
  TIM4_Prescaler_2      = ((uint8_t)0x01),   
  TIM4_Prescaler_4      = ((uint8_t)0x02),   
  TIM4_Prescaler_8      = ((uint8_t)0x03),   
  TIM4_Prescaler_16     = ((uint8_t)0x04),   
  TIM4_Prescaler_32     = ((uint8_t)0x05),   
  TIM4_Prescaler_64     = ((uint8_t)0x06),   
  TIM4_Prescaler_128    = ((uint8_t)0x07),   
  TIM4_Prescaler_256    = ((uint8_t)0x08),   
  TIM4_Prescaler_512    = ((uint8_t)0x09),   
  TIM4_Prescaler_1024   = ((uint8_t)0x0A),   
  TIM4_Prescaler_2048   = ((uint8_t)0x0B),   
  TIM4_Prescaler_4096   = ((uint8_t)0x0C),   
  TIM4_Prescaler_8192   = ((uint8_t)0x0D),   
  TIM4_Prescaler_16384  = ((uint8_t)0x0E),   
  TIM4_Prescaler_32768  = ((uint8_t)0x0F)    
}TIM4_Prescaler_TypeDef;



 
  


 
typedef enum
{
  TIM4_OPMode_Single      = ((uint8_t) 0x01),   
  TIM4_OPMode_Repetitive  = ((uint8_t) 0x00)    
}TIM4_OPMode_TypeDef;



 
  


 
typedef enum
{
  TIM4_PSCReloadMode_Update      = ((uint8_t)0x00),  
  TIM4_PSCReloadMode_Immediate  = ((uint8_t)0x01)   
}TIM4_PSCReloadMode_TypeDef;



 
  


 
typedef enum
{
  TIM4_UpdateSource_Global  = ((uint8_t)0x00),   
  TIM4_UpdateSource_Regular  = ((uint8_t)0x01)    
}TIM4_UpdateSource_TypeDef;



 
  


 
typedef enum
{
  TIM4_EventSource_Update    = ((uint8_t)0x01),   
  TIM4_EventSource_Trigger  = ((uint8_t)0x40)    
}TIM4_EventSource_TypeDef;



 
  


 
typedef enum
{
  TIM4_TRGOSource_Reset   = ((uint8_t)0x00),   
  TIM4_TRGOSource_Enable  = ((uint8_t)0x10),   
  TIM4_TRGOSource_Update  = ((uint8_t)0x20)    
}TIM4_TRGOSource_TypeDef;



 
  


 
typedef enum
{
  TIM4_SlaveMode_Disable    = ((uint8_t)0x00),   
 
  TIM4_SlaveMode_Reset      = ((uint8_t)0x04),    
  TIM4_SlaveMode_Gated      = ((uint8_t)0x05),    
  TIM4_SlaveMode_Trigger    = ((uint8_t)0x06),   
  TIM4_SlaveMode_External1  = ((uint8_t)0x07)     
}TIM4_SlaveMode_TypeDef;



 
  


 
typedef enum
{
  TIM4_FLAG_Update  = ((uint8_t)0x01),   
  TIM4_FLAG_Trigger  = ((uint8_t)0x40)     
}TIM4_FLAG_TypeDef;



 
  


 
typedef enum
{
  TIM4_IT_Update  = ((uint8_t)0x01),   
  TIM4_IT_Trigger  = ((uint8_t)0x40)    
}TIM4_IT_TypeDef;



 
  


 
typedef enum
{
  TIM4_TRGSelection_TIM5  = ((uint8_t)0x00),  
  TIM4_TRGSelection_TIM1  = ((uint8_t)0x10),  
  TIM4_TRGSelection_TIM3  = ((uint8_t)0x20),  
  TIM4_TRGSelection_TIM2  = ((uint8_t)0x30)   
}TIM4_TRGSelection_TypeDef;



 
  


 
typedef enum
{
  TIM4_DMASource_Update    = ((uint8_t)0x01)   
}TIM4_DMASource_TypeDef;


 



 
  
 
 



 



 



 


 



 


 


 



 


 


 



 



 



 



 

 

 
void TIM4_DeInit(void);
void TIM4_TimeBaseInit(TIM4_Prescaler_TypeDef TIM4_Prescaler,
                       uint8_t TIM4_Period);
void TIM4_PrescalerConfig(TIM4_Prescaler_TypeDef Prescaler,
                          TIM4_PSCReloadMode_TypeDef TIM4_PSCReloadMode);
void TIM4_SetCounter(uint8_t Counter);
void TIM4_SetAutoreload(uint8_t Autoreload);
uint8_t TIM4_GetCounter(void);
TIM4_Prescaler_TypeDef TIM4_GetPrescaler(void);
void TIM4_UpdateDisableConfig(FunctionalState NewState);
void TIM4_UpdateRequestConfig(TIM4_UpdateSource_TypeDef TIM4_UpdateSource);
void TIM4_ARRPreloadConfig(FunctionalState NewState);
void TIM4_SelectOnePulseMode(TIM4_OPMode_TypeDef TIM4_OPMode);
void TIM4_Cmd(FunctionalState NewState);

 
void TIM4_ITConfig(TIM4_IT_TypeDef TIM4_IT, FunctionalState NewState);
void TIM4_GenerateEvent(TIM4_EventSource_TypeDef TIM4_EventSource);
FlagStatus TIM4_GetFlagStatus(TIM4_FLAG_TypeDef TIM4_FLAG);
void TIM4_ClearFlag(TIM4_FLAG_TypeDef TIM4_FLAG);
ITStatus TIM4_GetITStatus(TIM4_IT_TypeDef TIM4_IT);
void TIM4_ClearITPendingBit(TIM4_IT_TypeDef TIM4_IT);
void TIM4_DMACmd(TIM4_DMASource_TypeDef TIM4_DMASource, FunctionalState NewState);

 
void TIM4_InternalClockConfig(void);

 
void TIM4_SelectInputTrigger(TIM4_TRGSelection_TypeDef TIM4_InputTriggerSource);
void TIM4_SelectOutputTrigger(TIM4_TRGOSource_TypeDef TIM4_TRGOSource);
void TIM4_SelectSlaveMode(TIM4_SlaveMode_TypeDef TIM4_SlaveMode);
void TIM4_SelectMasterSlaveMode(FunctionalState NewState);




 




 

 


























 

 


 



 
  


  
 



 



 
typedef enum
{
  TIM5_ForcedAction_Active   = ((uint8_t)0x50),    
  TIM5_ForcedAction_Inactive = ((uint8_t)0x40)     
}
TIM5_ForcedAction_TypeDef;



 
  


 
typedef enum
{
  TIM5_Prescaler_1     = ((uint8_t)0x00),    
  TIM5_Prescaler_2     = ((uint8_t)0x01),    
  TIM5_Prescaler_4     = ((uint8_t)0x02),    
  TIM5_Prescaler_8     = ((uint8_t)0x03),    
  TIM5_Prescaler_16    = ((uint8_t)0x04),    
  TIM5_Prescaler_32    = ((uint8_t)0x05),    
  TIM5_Prescaler_64    = ((uint8_t)0x06),    
  TIM5_Prescaler_128   = ((uint8_t)0x07)     
}TIM5_Prescaler_TypeDef;



 
  


 
typedef enum
{
  TIM5_OCMode_Timing    = ((uint8_t)0x00),    
  TIM5_OCMode_Active    = ((uint8_t)0x10),    
  TIM5_OCMode_Inactive  = ((uint8_t)0x20),    
  TIM5_OCMode_Toggle    = ((uint8_t)0x30),    
  TIM5_OCMode_PWM1      = ((uint8_t)0x60),    
  TIM5_OCMode_PWM2      = ((uint8_t)0x70)     
}TIM5_OCMode_TypeDef;




 
  


 
typedef enum
{
  TIM5_OPMode_Single      = ((uint8_t)0x01),  
  TIM5_OPMode_Repetitive  = ((uint8_t)0x00)   
}TIM5_OPMode_TypeDef;



 
  


 
typedef enum
{
  TIM5_Channel_1  = ((uint8_t)0x00),   
  TIM5_Channel_2  = ((uint8_t)0x01)    
}TIM5_Channel_TypeDef;



 
  


 
typedef enum
{
  TIM5_CounterMode_Up               = ((uint8_t)0x00),    
  TIM5_CounterMode_Down             = ((uint8_t)0x10),    
  TIM5_CounterMode_CenterAligned1   = ((uint8_t)0x20),    
  TIM5_CounterMode_CenterAligned2   = ((uint8_t)0x40),    
  TIM5_CounterMode_CenterAligned3   = ((uint8_t)0x60)     
}TIM5_CounterMode_TypeDef;



 
  


 
typedef enum
{
  TIM5_OCPolarity_High   = ((uint8_t)0x00),    
  TIM5_OCPolarity_Low    = ((uint8_t)0x01)     
}TIM5_OCPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM5_OutputState_Disable   = ((uint8_t)0x00),    
  TIM5_OutputState_Enable    = ((uint8_t)0x01)     
}TIM5_OutputState_TypeDef;




 
  


 
typedef enum
{
  TIM5_BreakState_Disable  = ((uint8_t)0x00),    
  TIM5_BreakState_Enable   = ((uint8_t)0x10)     
}TIM5_BreakState_TypeDef;



 
  


 
typedef enum
{
  TIM5_BreakPolarity_High  = ((uint8_t)0x20),   
  TIM5_BreakPolarity_Low   = ((uint8_t)0x00)    
}TIM5_BreakPolarity_TypeDef;



 



 
typedef enum
{
  TIM5_AutomaticOutput_Enable    = ((uint8_t)0x40),    
  TIM5_AutomaticOutput_Disable   = ((uint8_t)0x00)     
}TIM5_AutomaticOutput_TypeDef;



 
  


 
typedef enum
{
  TIM5_LockLevel_Off  = ((uint8_t)0x00),    
  TIM5_LockLevel_1    = ((uint8_t)0x01),    
  TIM5_LockLevel_2    = ((uint8_t)0x02),    
  TIM5_LockLevel_3    = ((uint8_t)0x03)     
}TIM5_LockLevel_TypeDef;



 
  


 
typedef enum
{
  TIM5_OSSIState_Enable    = ((uint8_t)0x04),    
  TIM5_OSSIState_Disable   = ((uint8_t)0x00)     
}TIM5_OSSIState_TypeDef;



 
  


 
typedef enum
{
  TIM5_OCIdleState_Reset  = ((uint8_t)0x00),    
  TIM5_OCIdleState_Set    = ((uint8_t)0x01)     
}TIM5_OCIdleState_TypeDef;



 
  


 
typedef enum
{
  TIM5_ICPolarity_Rising   = ((uint8_t)0x00),    
  TIM5_ICPolarity_Falling  = ((uint8_t)0x01)     
}TIM5_ICPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM5_ICSelection_DirectTI    = ((uint8_t)0x01),    
  TIM5_ICSelection_IndirectTI  = ((uint8_t)0x02),    
  TIM5_ICSelection_TRGI        = ((uint8_t)0x03)     
}TIM5_ICSelection_TypeDef;



 
  


 
typedef enum
{
  TIM5_ICPSC_DIV1  = ((uint8_t)0x00),   
  TIM5_ICPSC_DIV2  = ((uint8_t)0x04),   
  TIM5_ICPSC_DIV4  = ((uint8_t)0x08),   
  TIM5_ICPSC_DIV8  = ((uint8_t)0x0C)    
}TIM5_ICPSC_TypeDef;



 
  


 
typedef enum
{
  TIM5_IT_Update   = ((uint8_t)0x01),    
  TIM5_IT_CC1      = ((uint8_t)0x02),    
  TIM5_IT_CC2      = ((uint8_t)0x04),    
  TIM5_IT_Trigger  = ((uint8_t)0x40),    
  TIM5_IT_Break    = ((uint8_t)0x80)     
}TIM5_IT_TypeDef;




 
  


 
typedef enum
{
  TIM5_ExtTRGPSC_OFF   = ((uint8_t)0x00),    
  TIM5_ExtTRGPSC_DIV2  = ((uint8_t)0x10),    
  TIM5_ExtTRGPSC_DIV4  = ((uint8_t)0x20),    
  TIM5_ExtTRGPSC_DIV8  = ((uint8_t)0x30)     
}TIM5_ExtTRGPSC_TypeDef;



 
  


 
typedef enum
{
  TIM5_TRGSelection_TIM4     = ((uint8_t)0x00),   
  TIM5_TRGSelection_TIM1     = ((uint8_t)0x10),   
  TIM5_TRGSelection_TIM3     = ((uint8_t)0x20),   
  TIM5_TRGSelection_TIM2     = ((uint8_t)0x30),   
  TIM5_TRGSelection_TI1F_ED  = ((uint8_t)0x40),   
  TIM5_TRGSelection_TI1FP1   = ((uint8_t)0x50),   
  TIM5_TRGSelection_TI2FP2   = ((uint8_t)0x60),   
  TIM5_TRGSelection_ETRF     = ((uint8_t)0x70)    
}TIM5_TRGSelection_TypeDef;





 
  


 
typedef enum
{
  TIM5_TIxExternalCLK1Source_TI1ED  = ((uint8_t)0x40),    
  TIM5_TIxExternalCLK1Source_TI1    = ((uint8_t)0x50),    
  TIM5_TIxExternalCLK1Source_TI2    = ((uint8_t)0x60)     
}TIM5_TIxExternalCLK1Source_TypeDef;




 
  


 
typedef enum
{
  TIM5_ExtTRGPolarity_Inverted     = ((uint8_t)0x80),    
  TIM5_ExtTRGPolarity_NonInverted  = ((uint8_t)0x00)     
}TIM5_ExtTRGPolarity_TypeDef;



 
  


 
typedef enum
{
  TIM5_PSCReloadMode_Update      = ((uint8_t)0x00),    
  TIM5_PSCReloadMode_Immediate   = ((uint8_t)0x01)     
}TIM5_PSCReloadMode_TypeDef;



 
  


 
typedef enum
{
  TIM5_EncoderMode_TI1     = ((uint8_t)0x01),    
  TIM5_EncoderMode_TI2     = ((uint8_t)0x02),    
  TIM5_EncoderMode_TI12    = ((uint8_t)0x03)     
}TIM5_EncoderMode_TypeDef;



 
  


 
typedef enum
{
  TIM5_EventSource_Update   = ((uint8_t)0x01),    
  TIM5_EventSource_CC1      = ((uint8_t)0x02),    
  TIM5_EventSource_CC2      = ((uint8_t)0x04),    
  TIM5_EventSource_Trigger  = ((uint8_t)0x40),    
  TIM5_EventSource_Break    = ((uint8_t)0x80)     
}TIM5_EventSource_TypeDef;



 
  


 
typedef enum
{
  TIM5_UpdateSource_Global   = ((uint8_t)0x00),    
  TIM5_UpdateSource_Regular  = ((uint8_t)0x01)     
}TIM5_UpdateSource_TypeDef;




 
  


 
typedef enum
{
  TIM5_TRGOSource_Reset    = ((uint8_t)0x00),    
  TIM5_TRGOSource_Enable   = ((uint8_t)0x10),    
  TIM5_TRGOSource_Update   = ((uint8_t)0x20),    
  TIM5_TRGOSource_OC1      = ((uint8_t)0x30),    
  TIM5_TRGOSource_OC1REF   = ((uint8_t)0x40),    
  TIM5_TRGOSource_OC2REF   = ((uint8_t)0x50)     
}TIM5_TRGOSource_TypeDef;



 
  


 
typedef enum
{
  TIM5_SlaveMode_Reset      = ((uint8_t)0x04),   
  TIM5_SlaveMode_Gated      = ((uint8_t)0x05),   
  TIM5_SlaveMode_Trigger    = ((uint8_t)0x06),   
  TIM5_SlaveMode_External1  = ((uint8_t)0x07)    
}TIM5_SlaveMode_TypeDef;



 
  


 
typedef enum
{
  TIM5_FLAG_Update   = ((uint16_t)0x0001),   
  TIM5_FLAG_CC1      = ((uint16_t)0x0002),   
  TIM5_FLAG_CC2      = ((uint16_t)0x0004),   
  TIM5_FLAG_Trigger  = ((uint16_t)0x0040),   
  TIM5_FLAG_Break    = ((uint16_t)0x0080),   
  TIM5_FLAG_CC1OF    = ((uint16_t)0x0200),   
  TIM5_FLAG_CC2OF    = ((uint16_t)0x0400)    
}TIM5_FLAG_TypeDef;




 
  


 
typedef enum
{
  TIM5_DMASource_Update   = ((uint8_t)0x01),   
  TIM5_DMASource_CC1      = ((uint8_t)0x02),   
  TIM5_DMASource_CC2      = ((uint8_t)0x04)    
}TIM5_DMASource_TypeDef;



 
  


 

 
 



 



 



 



 

 

 
void TIM5_DeInit(void);
void TIM5_TimeBaseInit(TIM5_Prescaler_TypeDef TIM5_Prescaler,
                       TIM5_CounterMode_TypeDef TIM5_CounterMode, uint16_t TIM5_Period);
void TIM5_PrescalerConfig(TIM5_Prescaler_TypeDef Prescaler,
                          TIM5_PSCReloadMode_TypeDef TIM5_PSCReloadMode);
void TIM5_CounterModeConfig(TIM5_CounterMode_TypeDef TIM5_CounterMode);
void TIM5_SetCounter(uint16_t Counter);
void TIM5_SetAutoreload(uint16_t Autoreload);
uint16_t TIM5_GetCounter(void);
TIM5_Prescaler_TypeDef TIM5_GetPrescaler(void);
void TIM5_UpdateDisableConfig(FunctionalState NewState);
void TIM5_UpdateRequestConfig(TIM5_UpdateSource_TypeDef TIM5_UpdateSource);
void TIM5_ARRPreloadConfig(FunctionalState NewState);
void TIM5_SelectOnePulseMode(TIM5_OPMode_TypeDef TIM5_OPMode);
void TIM5_Cmd(FunctionalState NewState);

 
void TIM5_OC1Init(TIM5_OCMode_TypeDef TIM5_OCMode,
                  TIM5_OutputState_TypeDef TIM5_OutputState,
                  uint16_t TIM5_Pulse,
                  TIM5_OCPolarity_TypeDef TIM5_OCPolarity,
                  TIM5_OCIdleState_TypeDef TIM5_OCIdleState);
void TIM5_OC2Init(TIM5_OCMode_TypeDef TIM5_OCMode,
                  TIM5_OutputState_TypeDef TIM5_OutputState,
                  uint16_t TIM5_Pulse,
                  TIM5_OCPolarity_TypeDef TIM5_OCPolarity,
                  TIM5_OCIdleState_TypeDef TIM5_OCIdleState);
void TIM5_BKRConfig(TIM5_OSSIState_TypeDef TIM5_OSSIState,
                    TIM5_LockLevel_TypeDef TIM5_LockLevel,
                    TIM5_BreakState_TypeDef TIM5_BreakState,
                    TIM5_BreakPolarity_TypeDef TIM5_BreakPolarity,
                    TIM5_AutomaticOutput_TypeDef TIM5_AutomaticOutput);
void TIM5_CtrlPWMOutputs(FunctionalState NewState);
void TIM5_SelectOCxM(TIM5_Channel_TypeDef TIM5_Channel, TIM5_OCMode_TypeDef TIM5_OCMode);
void TIM5_SetCompare1(uint16_t Compare);
void TIM5_SetCompare2(uint16_t Compare);
void TIM5_ForcedOC1Config(TIM5_ForcedAction_TypeDef TIM5_ForcedAction);
void TIM5_ForcedOC2Config(TIM5_ForcedAction_TypeDef TIM5_ForcedAction);
void TIM5_OC1PreloadConfig(FunctionalState NewState);
void TIM5_OC2PreloadConfig(FunctionalState NewState);
void TIM5_OC1FastConfig(FunctionalState NewState);
void TIM5_OC2FastConfig(FunctionalState NewState);
void TIM5_OC1PolarityConfig(TIM5_OCPolarity_TypeDef TIM5_OCPolarity);
void TIM5_OC2PolarityConfig(TIM5_OCPolarity_TypeDef TIM5_OCPolarity);
void TIM5_CCxCmd(TIM5_Channel_TypeDef TIM5_Channel, FunctionalState NewState);

 
void TIM5_ICInit(TIM5_Channel_TypeDef TIM5_Channel,
                 TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                 TIM5_ICSelection_TypeDef TIM5_ICSelection,
                 TIM5_ICPSC_TypeDef TIM5_ICPrescaler,
                 uint8_t TIM5_ICFilter);
void TIM5_PWMIConfig(TIM5_Channel_TypeDef TIM5_Channel,
                     TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                     TIM5_ICSelection_TypeDef TIM5_ICSelection,
                     TIM5_ICPSC_TypeDef TIM5_ICPrescaler,
                     uint8_t TIM5_ICFilter);
uint16_t TIM5_GetCapture1(void);
uint16_t TIM5_GetCapture2(void);
void TIM5_SetIC1Prescaler(TIM5_ICPSC_TypeDef TIM5_IC1Prescaler);
void TIM5_SetIC2Prescaler(TIM5_ICPSC_TypeDef TIM5_IC2Prescaler);

 
void TIM5_ITConfig(TIM5_IT_TypeDef TIM5_IT, FunctionalState NewState);
void TIM5_GenerateEvent(TIM5_EventSource_TypeDef TIM5_EventSource);
FlagStatus TIM5_GetFlagStatus(TIM5_FLAG_TypeDef TIM5_FLAG);
void TIM5_ClearFlag(TIM5_FLAG_TypeDef TIM5_FLAG);
ITStatus TIM5_GetITStatus(TIM5_IT_TypeDef TIM5_IT);
void TIM5_ClearITPendingBit(TIM5_IT_TypeDef TIM5_IT);
void TIM5_DMACmd(TIM5_DMASource_TypeDef TIM5_DMASource, FunctionalState NewState);
void TIM5_SelectCCDMA(FunctionalState NewState);

 
void TIM5_InternalClockConfig(void);
void TIM5_TIxExternalClockConfig(TIM5_TIxExternalCLK1Source_TypeDef TIM5_TIxExternalCLKSource,
                                 TIM5_ICPolarity_TypeDef TIM5_ICPolarity,
                                 uint8_t ICFilter);
void TIM5_ETRClockMode1Config(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                              TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);
void TIM5_ETRClockMode2Config(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                              TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                              uint8_t ExtTRGFilter);

 
void TIM5_SelectInputTrigger(TIM5_TRGSelection_TypeDef TIM5_InputTriggerSource);
void TIM5_SelectOutputTrigger(TIM5_TRGOSource_TypeDef TIM5_TRGOSource);
void TIM5_SelectSlaveMode(TIM5_SlaveMode_TypeDef TIM5_SlaveMode);
void TIM5_SelectMasterSlaveMode(FunctionalState NewState);
void TIM5_ETRConfig(TIM5_ExtTRGPSC_TypeDef TIM5_ExtTRGPrescaler,
                    TIM5_ExtTRGPolarity_TypeDef TIM5_ExtTRGPolarity,
                    uint8_t ExtTRGFilter);

 
void TIM5_EncoderInterfaceConfig(TIM5_EncoderMode_TypeDef TIM5_EncoderMode,
                                 TIM5_ICPolarity_TypeDef TIM5_IC1Polarity,
                                 TIM5_ICPolarity_TypeDef TIM5_IC2Polarity);
void TIM5_SelectHallSensor(FunctionalState NewState);




 



 

 


























 

 



 
  


 

 


























 

 

 



 
  


  
 



 












 
typedef enum {
  WFE_Source_TIM2_EV0     = (uint16_t)0x0101,  
  WFE_Source_TIM2_EV1     = (uint16_t)0x0102,  
  WFE_Source_TIM1_EV0     = (uint16_t)0x0104,  
  WFE_Source_TIM1_EV1     = (uint16_t)0x0108,  
  WFE_Source_EXTI_EV0     = (uint16_t)0x0110,  
  WFE_Source_EXTI_EV1     = (uint16_t)0x0120,  
  WFE_Source_EXTI_EV2     = (uint16_t)0x0140,  
  WFE_Source_EXTI_EV3     = (uint16_t)0x0180,  
  WFE_Source_EXTI_EV4     = (uint16_t)0x0201,  
  WFE_Source_EXTI_EV5     = (uint16_t)0x0202,  
  WFE_Source_EXTI_EV6     = (uint16_t)0x0204,  
  WFE_Source_EXTI_EV7     = (uint16_t)0x0208,  
  WFE_Source_EXTI_EVB_G   = (uint16_t)0x0210,  
  WFE_Source_EXTI_EVD_H   = (uint16_t)0x0220,  
  WFE_Source_EXTI_EVE_F   = (uint16_t)0x0240,  
  WFE_Source_ADC1_COMP_EV = (uint16_t)0x0280,  
  WFE_Source_TIM3_EV0     = (uint16_t)0x0301,  
  WFE_Source_TIM3_EV1     = (uint16_t)0x0302,  
  WFE_Source_TIM4_EV      = (uint16_t)0x0304,  
  WFE_Source_SPI1_EV      = (uint16_t)0x0308,  
  WFE_Source_I2C1_EV      = (uint16_t)0x0310,  
  WFE_Source_USART1_EV    = (uint16_t)0x0320,  
  WFE_Source_DMA1CH01_EV  = (uint16_t)0x0340,  
  WFE_Source_DMA1CH23_EV  = (uint16_t)0x0380,   
  WFE_Source_RTC_CSS_EV   = (uint16_t)0x0401,   
  WFE_Source_SPI2_EV      = (uint16_t)0x0402,   
  WFE_Source_USART2_EV    = (uint16_t)0x0404,   
  WFE_Source_USART3_EV    = (uint16_t)0x0408,   
  WFE_Source_TIM5_EV0     = (uint16_t)0x0410,   
  WFE_Source_TIM5_EV1     = (uint16_t)0x0420,   
  WFE_Source_AES_EV       = (uint16_t)0x0440    
} WFE_Source_TypeDef;




 



 
 
 
 
  
void WFE_DeInit(void);

  
void WFE_WakeUpSourceEventCmd(WFE_Source_TypeDef WFE_Source, FunctionalState NewState);
FunctionalState WFE_GetWakeUpSourceEventStatus(WFE_Source_TypeDef WFE_Source);




 



 

 


























 

 

 



 
  


  
  
 
 
 



 



  



 



 


 
  


 

 
 
void WWDG_Init(uint8_t Counter, uint8_t WindowValue);
void WWDG_SetWindowValue(uint8_t WindowValue);
void WWDG_SetCounter(uint8_t Counter);

 
void WWDG_Enable(uint8_t Counter);

 
uint8_t WWDG_GetCounter(void);
void WWDG_SWReset(void);




 



 

 

 
 

 
  

 


 

 

 








 


  #pragma system_include

#pragma language=save
#pragma language=extended




 

typedef unsigned char __istate_t;


  __intrinsic void __enable_interrupt(void);      
  __intrinsic void __disable_interrupt(void);     

  __intrinsic __istate_t __get_interrupt_state(void);
  __intrinsic void       __set_interrupt_state(__istate_t);

   
  __intrinsic void __no_operation(void);          
  __intrinsic void __halt(void);                  
  __intrinsic void __trap(void);                  
  __intrinsic void __wait_for_event(void);        
  __intrinsic void __wait_for_interrupt(void);    

   
  __intrinsic void __BCPL(unsigned char __near *, unsigned char);
  __intrinsic void __BRES(unsigned char __near *, unsigned char);
  __intrinsic void __BSET(unsigned char __near *, unsigned char);



#pragma language=restore


 




 
 
 












 




 








 







 



 

 



 


  
  
 



 



 
typedef enum 
{ 
 USART_FLAG_TXE   = (uint16_t)0x0080,  
 USART_FLAG_TC    = (uint16_t)0x0040,  
 USART_FLAG_RXNE  = (uint16_t)0x0020,  
 USART_FLAG_IDLE  = (uint16_t)0x0010,  
 USART_FLAG_OR    = (uint16_t)0x0008,  
 USART_FLAG_NF    = (uint16_t)0x0004,  
 USART_FLAG_FE    = (uint16_t)0x0002,  
 USART_FLAG_PE    = (uint16_t)0x0001,  
 USART_FLAG_SBK   = (uint16_t)0x0101   
} USART_FLAG_TypeDef;

   
   


 



 
    







 
typedef enum 
{ 
  USART_IT_TXE        = (uint16_t)0x0277,  
  USART_IT_TC         = (uint16_t)0x0266,  
  USART_IT_RXNE       = (uint16_t)0x0255,  
  USART_IT_IDLE       = (uint16_t)0x0244,  
  USART_IT_OR         = (uint16_t)0x0235,  
  USART_IT_PE         = (uint16_t)0x0100,  
  USART_IT_ERR        = (uint16_t)0x0500,  
  USART_IT_NF         = (uint16_t)0x0102,  
  USART_IT_FE         = (uint16_t)0x0101   
} USART_IT_TypeDef;





 
  


 
typedef enum
{
  USART_WakeUp_IdleLine       = (uint8_t)0x00,  
  USART_WakeUp_AddressMark    = (uint8_t)0x08   
} USART_WakeUp_TypeDef;



 
  


 
typedef enum
{
  USART_StopBits_1   = (uint8_t)0x00,  
  USART_StopBits_2   = (uint8_t)0x20,  
  USART_StopBits_1_5 = (uint8_t)0x30   
} USART_StopBits_TypeDef;



 
  


 
typedef enum
{
  USART_Parity_No    = (uint8_t)0x00,       
  USART_Parity_Even  = (uint8_t)0x04,       
  USART_Parity_Odd   = (uint8_t)0x06        
} USART_Parity_TypeDef;



 
  


 
typedef enum
{
  USART_LINBreakDetectionLength_10BITS = (uint8_t)0x00,  
  USART_LINBreakDetectionLength_11BITS = (uint8_t)0x01   
} USART_LINBreakDetectionLength_TypeDef;



 
  


 
typedef enum
{
  USART_WordLength_8b = (uint8_t)0x00,   
  USART_WordLength_9b = (uint8_t)0x10    
} USART_WordLength_TypeDef;

   


 
  


 
typedef enum
{
  USART_Mode_Rx    = (uint8_t)0x04,   
  USART_Mode_Tx    = (uint8_t)0x08    
} USART_Mode_TypeDef;



 
  


 
typedef enum
{
  USART_DMAReq_TX    = (uint8_t)0x80,   
  USART_DMAReq_RX    = (uint8_t)0x40    
} USART_DMAReq_TypeDef;



 
  


 
typedef enum
{
  USART_IrDAMode_Normal    = (uint8_t)0x00,  
  USART_IrDAMode_LowPower  = (uint8_t)0x01   
} USART_IrDAMode_TypeDef;



 
  


 
typedef enum
{
  USART_Clock_Disable    = (uint8_t)0x00,   
  USART_Clock_Enable     = (uint8_t)0x08    
} USART_Clock_TypeDef;



 
  


 
typedef enum
{
  USART_CPOL_Low      = (uint8_t)0x00,   
  USART_CPOL_High     = (uint8_t)0x04    
} USART_CPOL_TypeDef;



 
  


 
typedef enum
{
  USART_CPHA_1Edge     = (uint8_t)0x00,   
  USART_CPHA_2Edge     = (uint8_t)0x02    
} USART_CPHA_TypeDef;



 
  


 
typedef enum
{
  USART_LastBit_Disable  = (uint8_t)0x00,   
  USART_LastBit_Enable   = (uint8_t)0x01    
} USART_LastBit_TypeDef;



 



 
  
 
 


 
 





 

 
	
  
void USART_DeInit(USART_TypeDef* USARTx);

 
void USART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, USART_WordLength_TypeDef
                USART_WordLength, USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity, USART_Mode_TypeDef USART_Mode);
void USART_ClockInit(USART_TypeDef* USARTx, USART_Clock_TypeDef USART_Clock,
                     USART_CPOL_TypeDef USART_CPOL, USART_CPHA_TypeDef USART_CPHA,
                     USART_LastBit_TypeDef USART_LastBit);
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler);
void USART_SendBreak(USART_TypeDef* USARTx);

  
void USART_SendData8(USART_TypeDef* USARTx, uint8_t Data);
void USART_SendData9(USART_TypeDef* USARTx, uint16_t Data);
uint8_t USART_ReceiveData8(USART_TypeDef* USARTx);
uint16_t USART_ReceiveData9(USART_TypeDef* USARTx);

 
void USART_WakeUpConfig(USART_TypeDef* USARTx, USART_WakeUp_TypeDef USART_WakeUp);
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address);

 
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState);

 
void USART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState);
void USART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime);

 
void USART_IrDAConfig(USART_TypeDef* USARTx, USART_IrDAMode_TypeDef USART_IrDAMode);
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState);

 
void USART_DMACmd(USART_TypeDef* USARTx, USART_DMAReq_TypeDef USART_DMAReq,
                  FunctionalState NewState);

 
void USART_ITConfig(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT,
                    FunctionalState NewState);
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG);
void USART_ClearFlag(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG);
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT);
void USART_ClearITPendingBit(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT);






 
  


 

 



 




 
	
 
 
 
 
 
 



 














































 





 
void USART_DeInit(USART_TypeDef* USARTx)
{

  
 
  (void) USARTx->SR;
  (void) USARTx->DR;

  USARTx->BRR2 = ((uint8_t)0x00);   
  USARTx->BRR1 = ((uint8_t)0x00);   

  USARTx->CR1 = ((uint8_t)0x00);   
  USARTx->CR2 = ((uint8_t)0x00);   
  USARTx->CR3 = ((uint8_t)0x00);   
  USARTx->CR4 = ((uint8_t)0x00);   
}



























 
void USART_Init(USART_TypeDef* USARTx, uint32_t BaudRate, USART_WordLength_TypeDef
                USART_WordLength, USART_StopBits_TypeDef USART_StopBits,
                USART_Parity_TypeDef USART_Parity,  USART_Mode_TypeDef USART_Mode)
{
  uint32_t BaudRate_Mantissa = 0;

   
  ((void)0);

  ((void)0);

  ((void)0);

  ((void)0);

  ((void)0);

   
  USARTx->CR1 &= (uint8_t)(~(((uint8_t)0x04) | ((uint8_t)0x02) | ((uint8_t)0x10)));
   
   
  USARTx->CR1 |= (uint8_t)((uint8_t)USART_WordLength | (uint8_t)USART_Parity);

   
  USARTx->CR3 &= (uint8_t)(~((uint8_t)0x30));
   
  USARTx->CR3 |= (uint8_t)USART_StopBits;

   
  USARTx->BRR1 &= (uint8_t)(~((uint8_t)0xFF));
   
  USARTx->BRR2 &= (uint8_t)(~((uint8_t)0xF0));
   
  USARTx->BRR2 &= (uint8_t)(~((uint8_t)0x0F));

  BaudRate_Mantissa  = (uint32_t)(CLK_GetClockFreq() / BaudRate );
   
  USARTx->BRR2 = (uint8_t)((BaudRate_Mantissa >> (uint8_t)8) & (uint8_t)0xF0);
   
  USARTx->BRR2 |= (uint8_t)(BaudRate_Mantissa & (uint8_t)0x0F);
   
  USARTx->BRR1 = (uint8_t)(BaudRate_Mantissa >> (uint8_t)4);

   
  USARTx->CR2 &= (uint8_t)~(((uint8_t)0x08) | ((uint8_t)0x04));
   
  USARTx->CR2 |= (uint8_t)USART_Mode;
}























 
void USART_ClockInit(USART_TypeDef* USARTx, USART_Clock_TypeDef USART_Clock,
                     USART_CPOL_TypeDef USART_CPOL, USART_CPHA_TypeDef USART_CPHA,
                     USART_LastBit_TypeDef USART_LastBit)
{
   
  ((void)0);
  ((void)0);
  ((void)0);
  ((void)0);

   
  USARTx->CR3 &= (uint8_t)~(((uint8_t)0x04) | ((uint8_t)0x02) | ((uint8_t)0x01));
   
  USARTx->CR3 |= (uint8_t)((uint8_t)((uint8_t)(USART_CPOL | (uint8_t)USART_CPHA ) | USART_LastBit));

  if (USART_Clock != USART_Clock_Disable)
  {
    USARTx->CR3 |= (uint8_t)(((uint8_t)0x08));  
  }
  else
  {
    USARTx->CR3 &= (uint8_t)(~((uint8_t)0x08));  
  }
}







 
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    USARTx->CR1 &= (uint8_t)(~((uint8_t)0x20));  
  }
  else
  {
    USARTx->CR1 |= ((uint8_t)0x20);   
  }
}





















 
void USART_SetPrescaler(USART_TypeDef* USARTx, uint8_t USART_Prescaler)
{
   
  USARTx->PSCR = USART_Prescaler;
}





 
void USART_SendBreak(USART_TypeDef* USARTx)
{
  USARTx->CR2 |= ((uint8_t)0x01);
}



 



























 





 
uint8_t USART_ReceiveData8(USART_TypeDef* USARTx)
{
  return USARTx->DR;
}





 
uint16_t USART_ReceiveData9(USART_TypeDef* USARTx)
{
  uint16_t temp = 0;

  temp = ((uint16_t)(((uint16_t)((uint16_t)USARTx->CR1 & (uint16_t)((uint8_t)0x80))) << 1));
  return (uint16_t)( ((uint16_t)((uint16_t)USARTx->DR) | temp) & ((uint16_t)0x01FF));
}





 
void USART_SendData8(USART_TypeDef* USARTx, uint8_t Data)
{
   
  USARTx->DR = Data;
}







 
void USART_SendData9(USART_TypeDef* USARTx, uint16_t Data)
{
  ((void)0);

   
  USARTx->CR1 &= ((uint8_t)~((uint8_t)0x40));

   
  USARTx->CR1 |= (uint8_t)(((uint8_t)(Data >> 2)) & ((uint8_t)0x40));

   
  USARTx->DR   = (uint8_t)(Data);
}


 






























 
	






 
void USART_ReceiverWakeUpCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  ((void)0);

  if (NewState != DISABLE)
  {
     
    USARTx->CR2 |= ((uint8_t)0x02);
  }
  else
  {
     
    USARTx->CR2 &= ((uint8_t)~((uint8_t)0x02));
  }
}







 
void USART_SetAddress(USART_TypeDef* USARTx, uint8_t USART_Address)
{
   
  ((void)0);

   
  USARTx->CR4 &= ((uint8_t)~((uint8_t)0x0F));
   
  USARTx->CR4 |= USART_Address;
}








 
void USART_WakeUpConfig(USART_TypeDef* USARTx, USART_WakeUp_TypeDef USART_WakeUp)
{
  ((void)0);

  USARTx->CR1 &= ((uint8_t)~((uint8_t)0x08));
  USARTx->CR1 |= (uint8_t)USART_WakeUp;
}


 
































 







 
void USART_HalfDuplexCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  ((void)0);

  if (NewState != DISABLE)
  {
    USARTx->CR5 |= ((uint8_t)0x08);   
  }
  else
  {
    USARTx->CR5 &= (uint8_t)~((uint8_t)0x08);  
  }
}



 




















































 
	






 
void USART_SmartCardCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  ((void)0);

  if (NewState != DISABLE)
  {
     
    USARTx->CR5 |= ((uint8_t)0x20);
  }
  else
  {
     
    USARTx->CR5 &= ((uint8_t)(~((uint8_t)0x20)));
  }
}







 
void USART_SmartCardNACKCmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
  ((void)0);

  if (NewState != DISABLE)
  {
     
    USARTx->CR5 |= ((uint8_t)0x10);
  }
  else
  {
     
    USARTx->CR5 &= ((uint8_t)~(((uint8_t)0x10)));
  }
}







 
void USART_SetGuardTime(USART_TypeDef* USARTx, uint8_t USART_GuardTime)
{
   
  USARTx->GTR = USART_GuardTime;
}



 









































 
	








 
void USART_IrDAConfig(USART_TypeDef* USARTx, USART_IrDAMode_TypeDef USART_IrDAMode)
{
  ((void)0);

  if (USART_IrDAMode != USART_IrDAMode_Normal)
  {
    USARTx->CR5 |= ((uint8_t)0x04);
  }
  else
  {
    USARTx->CR5 &= ((uint8_t)~((uint8_t)0x04));
  }
}







 
void USART_IrDACmd(USART_TypeDef* USARTx, FunctionalState NewState)
{

   
  ((void)0);

  if (NewState != DISABLE)
  {
     
    USARTx->CR5 |= ((uint8_t)0x02);
  }
  else
  {
     
    USARTx->CR5 &= ((uint8_t)~((uint8_t)0x02));
  }
}



 











 












 
void USART_DMACmd(USART_TypeDef* USARTx, USART_DMAReq_TypeDef USART_DMAReq,
                  FunctionalState NewState)
{
   
  ((void)0);
  ((void)0);

  if (NewState != DISABLE)
  {
    
 
    USARTx->CR5 |= (uint8_t) USART_DMAReq;
  }
  else
  {
    
 
    USARTx->CR5 &= (uint8_t)~USART_DMAReq;
  }
}



 
  















































































 















 
void USART_ITConfig(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT, FunctionalState NewState)
{
  uint8_t usartreg, itpos = 0x00;
  ((void)0);
  ((void)0);

   
  usartreg = (uint8_t)((uint16_t)USART_IT >> 0x08);
   
  itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)USART_IT & (uint8_t)0x0F));

  if (NewState != DISABLE)
  {
     
    if (usartreg == 0x01)
    {
      USARTx->CR1 |= itpos;
    }
    else if (usartreg == 0x05)
    {
      USARTx->CR5 |= itpos;
    }
     
    else
    {
      USARTx->CR2 |= itpos;
    }
  }
  else
  {
     
    if (usartreg == 0x01)
    {
      USARTx->CR1 &= (uint8_t)(~itpos);
    }
    else if (usartreg == 0x05)
    {
      USARTx->CR5 &= (uint8_t)(~itpos);
    }
     
    else
    {
      USARTx->CR2 &= (uint8_t)(~itpos);
    }
  }
}
















 
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG)
{
  FlagStatus status = RESET;

   
  ((void)0);

  if (USART_FLAG == USART_FLAG_SBK)
  {
    if ((USARTx->CR2 & (uint8_t)USART_FLAG) != (uint8_t)0x00)
    {
       
      status = SET;
    }
    else
    {
       
      status = RESET;
    }
  }
  else
  {
    if ((USARTx->SR & (uint8_t)USART_FLAG) != (uint8_t)0x00)
    {
       
      status = SET;
    }
    else
    {
       
      status = RESET;
    }
  }
   
  return status;
}





















 
void USART_ClearFlag(USART_TypeDef* USARTx, USART_FLAG_TypeDef USART_FLAG)
{
   
  ((void)0);
   
  USARTx->SR = (uint8_t)((uint16_t)~((uint16_t)USART_FLAG));
}















 
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT)
{
  ITStatus pendingbitstatus = RESET;
  uint8_t temp = 0;
  uint8_t itpos = 0;
  uint8_t itmask1 = 0;
  uint8_t itmask2 = 0;
  uint8_t enablestatus = 0;

   
  ((void)0);

   
  itpos = (uint8_t)((uint8_t)1 << (uint8_t)((uint8_t)USART_IT & (uint8_t)0x0F));
   
  itmask1 = (uint8_t)((uint8_t)USART_IT >> (uint8_t)4);
   
  itmask2 = (uint8_t)((uint8_t)1 << itmask1);

   
  if (USART_IT == USART_IT_PE)
  {
     
    enablestatus = (uint8_t)((uint8_t)USARTx->CR1 & itmask2);
     

    if (((USARTx->SR & itpos) != (uint8_t)0x00) && enablestatus)
    {
       
      pendingbitstatus = SET;
    }
    else
    {
       
      pendingbitstatus = RESET;
    }
  }

  else if (USART_IT == USART_IT_OR)
  {
     
    enablestatus = (uint8_t)((uint8_t)USARTx->CR2 & itmask2);
     

    temp = (uint8_t)(USARTx->CR5 & ((uint8_t)0x01));

    if (( (USARTx->SR & itpos) != 0x00) && ((enablestatus || temp)))
    {
       
      pendingbitstatus = SET;
    }
    else
    {
       
      pendingbitstatus = RESET;
    }
  }

  else
  {
     
    enablestatus = (uint8_t)((uint8_t)USARTx->CR2 & itmask2);
     
    if (((USARTx->SR & itpos) != (uint8_t)0x00) && enablestatus)
    {
       
      pendingbitstatus = SET;
    }
    else
    {
       
      pendingbitstatus = RESET;
    }
  }

   
  return  pendingbitstatus;
}





















 
void USART_ClearITPendingBit(USART_TypeDef* USARTx, USART_IT_TypeDef USART_IT)
{
  ((void)0);

   
  USARTx->SR &= (uint8_t)(~((uint8_t)0x40));
}




 



 



 



 

 
