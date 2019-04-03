
#include "flash_if.h"
#include "common.h"

/** @addtogroup STM8S20x_IAP
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
  /* Unlock the Program memory */
  FLASH_Unlock(FLASH_MEMTYPE_PROG); 
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint8_t FLASH_If_Erase(uint32_t StartFlashAddress)
{
  uint16_t flashaddress;
  
  flashaddress = (StartFlashAddress - FLASH_PROG_START_PHYSICAL_ADDRESS)/(FLASH_BLOCK_SIZE);

  while (flashaddress < (uint16_t) FLASH_PROG_BLOCKS_NUMBER)
  {   
      FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
      FLASH_EraseBlock(flashaddress, FLASH_MEMTYPE_PROG);
      FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
      flashaddress ++;
      /*
      if(FLASH_STATUS_SUCCESSFUL_OPERATION == FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG)){
        flashaddress ++;
      }else{
        return (1);
      }*/
    
  }
  return (0);
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)   
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint8_t FLASH_If_Write(uint32_t *FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
#if 0
  uint32_t Address = (uint32_t) FlashAddress;
  uint8_t * DataPointer = Data;
  FLASH_Status_TypeDef FLASH_Status;
  
  while((Address % 4) && (DataLength)){
      FLASH_ProgramByte(Address,*DataPointer);
      FLASH_Status = FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
      /*
      if(FLASH_STATUS_SUCCESSFUL_OPERATION == FLASH_Status){
          Address++;
          DataPointer++;
          DataLength--;
      }else{
        return (1);
      }*/
      Address++;
      DataPointer++;
      DataLength--;
  }
  while((Address % FLASH_BLOCK_SIZE) && (DataLength >= 4)){
      FLASH_ProgramWord(Address,*DataPointer);
      FLASH_Status = FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
      /*
      if(FLASH_STATUS_SUCCESSFUL_OPERATION == FLASH_Status){
        Address    += 4;
        DataPointer+= 4;
        DataLength  -= 4;
      }else{
        return (1);
      }*/
      
        Address    += 4;
        DataPointer+= 4;
        DataLength  -= 4;
  }
  while(DataLength >= FLASH_BLOCK_SIZE)
  {
      FLASH_ProgramBlock((Address - APPLICATION_ADDRESS)/FLASH_BLOCK_SIZE, FLASH_MEMTYPE_PROG, FLASH_PROGRAMMODE_STANDARD,(uint8_t *)DataPointer);
      FLASH_Status = FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
      /*
      if(FLASH_STATUS_SUCCESSFUL_OPERATION == FLASH_Status){
        Address    += FLASH_BLOCK_SIZE;
        DataPointer+= FLASH_BLOCK_SIZE;    
        DataLength  -= FLASH_BLOCK_SIZE;
      }else{
        return (1);
      }*/
      
        Address    += FLASH_BLOCK_SIZE;
        DataPointer+= FLASH_BLOCK_SIZE;    
        DataLength  -= FLASH_BLOCK_SIZE;
  }
  
  while(DataLength >= 4){
      FLASH_ProgramWord(Address,*DataPointer);
      FLASH_Status = FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
      /*
      if(FLASH_STATUS_SUCCESSFUL_OPERATION == FLASH_Status){
        Address    += 4;
        DataPointer+= 4;
        DataLength  -= 4; 
      }else{
        return (1);
      }*/
        Address    += 4;
        DataPointer+= 4;
        DataLength  -= 4; 
  }
  while(DataLength){
      FLASH_ProgramByte(Address,*DataPointer);
      FLASH_Status = FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
      /*
      if(FLASH_STATUS_SUCCESSFUL_OPERATION == FLASH_Status){
        Address++;
        DataPointer++;
        DataLength--;
      }else{
        return (1);
      }*/
        Address++;
        DataPointer++;
        DataLength--;
  }
#endif
  
  uint32_t i = 0;

  for (i = 0; (i < DataLength) && (*FlashAddress <= (FLASH_PROG_END_PHYSICAL_ADDRESS-4)); i++)
  {
    /* the operation will be done by word */ 
    FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i));
    FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
     /* Check the written value */
      /* Increment FLASH destination address */
    *FlashAddress += 4;
 
  }

  return (0);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
