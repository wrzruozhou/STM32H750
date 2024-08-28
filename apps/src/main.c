/**
 * @note这个工程是依据3.0Touch工程改编过来的
 */
#include "main.h"

 /*要写到STM32 FLASH的字符串数组*/
const uint8_t g_text_buf[] = {"jkaskfjhskah"};
#define TEXT_LENTH sizeof(g_text_buf)                 /*数组长度*/
#define SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)

#define FLASH_SAVE_ADDR 0x08004000                    /*设置FLASH 保存地址*/

static void MPU_Config(void);
uint32_t sysclock = 0;

#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif

char temp_read[128];

int main(void)
{
  int i;
  uint16_t t = 0;
  uint8_t datatemp[SIZE];
  uint8_t key = 0;
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable(); /* 打开L1-Cache */

  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  mpu_memory_protection();
  LED_Config();
  usart_init(115200);
  usmart_dev.init(240);
  Key_Init();

  at24cxx_init();
  lcd_init();
  // gt9xxx_init();
  tp_dev.init();
  // load_draw_dialog();
  // ctp_test();
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "FLASH EEPROM TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED);
  while (1)
  {
    key = key_scan();
    if (key == KEY1_PRES)
    {
      lcd_fill(0, 150, 239, 319, WHITE);
      lcd_show_string(30, 150, 200, 16, 16, "start write flash...", RED);
      stmflash_write(FLASH_SAVE_ADDR, (uint32_t*)g_text_buf, SIZE);
      lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", RED);  /*提示传送完成*/
    }
    if (key == KEY0_PRES)
    {
      lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH...", RED);
      stmflash_read(FLASH_SAVE_ADDR, (uint32_t*)datatemp, SIZE);
      lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is: ", RED);
      lcd_show_string(30, 170, 200, 16, 16, (char*)datatemp, BLUE);
    }
    delay_ms(10);
  }
}


static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
