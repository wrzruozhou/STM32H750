#include "main.h"

static void MPU_Config(void);




uint32_t sysclock = 0;

#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif

/*这里是测试FLASH的*/
const uint8_t g_text_buf[] = { "zhong yu zuo le zhe ge jue ding" };
#define TEXT_SIZE sizeof(g_text_buf)

char temp_read[128];
uint16_t id = 0;
int main(void)
{
  int i;
  uint8_t t = 0;
  uint32_t flashsize;
  uint8_t datatemp[TEXT_SIZE];
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable(); /* 打开L1-Cache */
  HAL_Init();
  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  mpu_memory_protection();

  usart_init(115200);
  usmart_dev.init(240);

  // BEEP_PWM_Init(920, 1000);    /*频率261*/
  // BEEP_SETPWM(300, 100);      /*占空比为30%  死区时间是100 * Ttds*/

  // Key_Init();

  at24cxx_init();

  id = norflash_ex_read_id();
  printf("the flash id is %x\n", id);
  flashsize = 16 * 1024 * 1024;
  sprintf((char*)datatemp, "%s%d", (char*)g_text_buf, i);
  norflash_ex_write((uint8_t*)datatemp, flashsize - 100, TEXT_SIZE);

  All_TaskStart();
  vTaskStartScheduler();

  //   delay_ms(1000);
  //   norflash_ex_read(datatemp, flashsize - 100, TEXT_SIZE);
  // }
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
