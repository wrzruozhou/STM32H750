#include "main.h"

static void MPU_Config(void);
uint32_t sysclock = 0;

#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif

char temp[128] = "中原一点红";
char temp_read[128];

int main(void)
{
  int i;
  uint8_t t = 0;
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable(); /* 打开L1-Cache */

  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  LED_Config();
  usart_init(115200);
  Key_Init();
  mpu_memory_protection();
  // lcd_init();
  at24cxx_init();
  at24cxx_write(0x0a, temp, strlen(temp));
  while (1)
  {
#if 0
    for (i = 0; i < 100; i++)
    {
      at24cxx_write_one_byte(0x05, 0x30 + i);
      if (i % 2)
      {
        t = at24cxx_read_one_byte(0x05);
        printf("%d\n", t);
      }
      delay_ms(1000);
    }
#else
    at24cxx_read(0x0a, temp_read, 20);
    printf("%s\n", temp_read);
    delay_ms(1000);
#endif
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
