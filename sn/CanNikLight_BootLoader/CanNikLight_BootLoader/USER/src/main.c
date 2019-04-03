/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：main.c
 * 描述    ：STM8S BooterLoader  
 * 实验平台：风驰STM8S NOTE开发板
 * 库版本  ：V3.0.0
 * 作者    ：风驰  QQ：779814207/2366260464
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2015-10-15



*******************************************************************************/
#include "flash_if.h"
#include "menu.h"
#include "uart1.h"
/* Includes ------------------------------------------------------------------*/
/** @addtogroup STM8Sxx_IAP
  * @{
  */
__root const long reintvec[]@".intvec"=

{   

    0x82008080,0x8200A804,0x8200A808,0x8200A80c,
    
    0x8200A810,0x8200A814,0x8200A818,0x8200A81c,

    0x8200A820,0x8200A824,0x8200A828,0x8200A82c,

    0x8200A830,0x8200A834,0x8200A838,0x8200A83c,

    0x8200A840,0x8200A844,0x8200A848,0x8200A84c,

    0x8200A850,0x8200A854,0x8200A858,0x8200A85c,

    0x8200A860,0x8200A864,0x8200A868,0x8200A86c,

    0x8200A870,0x8200A874,0x8200A878,0x8200A87c,

};                                
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
 
  IAP_Init();
  /* Infinite loop */
  while (1)
  {
    Main_Menu();
  }
}

/**
  * @brief  Initialize the IAP.
  * @param  None
  * @retval None
  */
static void IAP_Init(void)
{
  /* Unlock the Flash Program Erase controller */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  FLASH_If_Init();
  Uart1_Init();
  
  //FLASH_If_Erase(APPLICATION_ADDRESS);
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
