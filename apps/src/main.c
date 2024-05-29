#include "main.h"
#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"

#include "lv_demo_music.h"
#include "lv_demo_stress.h"
#include "lv_demo_widgets.h"
#include "lv_demo_keypad_encoder.h"
#include "lv_demo_benchmark.h"

static void MPU_Config(void);
uint32_t sysclock = 0;

#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif


int main(void)
{

  char* str = 0;
  uint8_t key = 0;
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable(); /* 打开L1-Cache */

  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  mpu_memory_protection();
  //LED_Config();
  usart_init(115200);
  //usmart_dev.init(240);
  // Key_Init();
  //remote_init();
  //at24cxx_init();

  BaseTim_init(10 - 1, 24000 - 1);
  lv_init();    /*系统初始化*/
  lv_port_disp_init();
  lv_port_indev_init();

  //lv_demo_music();
  //lv_demo_stress();
	//lv_demo_widgets();
	//lv_demo_keypad_encoder();
	lv_demo_benchmark();


  while (1)
  {
    lv_timer_handler(); /* LVGL 管理函数相当于 RTOS 触发任务调度函数 */
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
