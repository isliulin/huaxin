#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "type.h"

u8 EEPROM_Read(u16 Addr,u8 *RxBuffer,u8 Length); 
u8 EEPROM_Write(u16 Addr,u8 *RxBuffer,u8 Lenth); 

#endif