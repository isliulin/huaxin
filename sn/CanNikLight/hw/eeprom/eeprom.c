#include "eeprom.h"
#include"iostm8l152k4.h"

#define EEPROM_STARTADDR 0x1000  

u8 EEPROM_Read(u16 Addr,u8 *RxBuffer,u8 Length)  
{  
    //u16 T=0;  
    u8 *EEP;  
    EEP=(u8 *)(EEPROM_STARTADDR+Addr);  
    FLASH_DUKR=0xAE;  
    FLASH_DUKR=0x56;  
    FLASH_CR2=0x00;  
    //while((!(FLASH_IAPSR&S3))&(T<0xFFFF))T++;  
    //if(T==0xFFFF)return 0;  
    while(!(FLASH_IAPSR & 0x08));
    while(Length--){  
        *RxBuffer++=*EEP++;  
    }  
    return 1;  
}  

u8 EEPROM_Write(u16 Addr,u8 *RxBuffer,u8 Lenth)  
{  
    //u16 T=0;  
    u8 *EEP;  
    EEP=(u8 *)(EEPROM_STARTADDR+Addr);  
    FLASH_DUKR=0xAE;  
    FLASH_DUKR=0x56;  
    FLASH_CR2=0x00;  
    //while((!(FLASH_IAPSR&S3))&(T<0xFFFF))T++;  
    //if(T==0xFFFF)return 0;  
    while(!(FLASH_IAPSR & 0x08));
    
    while(Lenth--){  
        *EEP++=*RxBuffer++;  
        //T=0;  
        //while((!(FLASH_IAPSR&S2))&(T<0xFFFF))T++;  
        //if(T==0xFFFF)return 0;  
        while(!(FLASH_IAPSR & 0x04));
    }  
    //FLASH_IAPSR&=~S3;  
    FLASH_IAPSR &= ~0x08;  
    return 1;  
}

