#ifndef _AES_BOOT_
#define _AES_BOOT_

#include "type.h"

extern void Boot_Encrypt(u8 *plainBlock, u8 *keyStr,u8 *cipherBlock, u32 size);
extern void Boot_Decrypt(u8 *cipherBlock, u8 *keyStr,u8 *plainBlock,u32 size);

#endif