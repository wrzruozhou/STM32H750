#include "main.h"
static void MPU_Config(void);
uint32_t sysclock = 0;
uint8_t g_timeout;
extern uint16_t g_color_map[ITERATION];
extern uint16_t zoom_ratio[26];

#if 0
#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif
#endif

int main(void)
{
  uint8_t i;
  uint8_t t = 0, key, autorun = 0;
  char buf[50];
  float time;
  /* Configure the MPU attributes */
  //MPU_Config();
  sys_cache_enable();                  /* 打开L1-Cache */
  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
	mpu_memory_protection();
  LED_Config();
  usart_init(115200);
  usmart_dev.init(240);
  Key_Init();
  
  lcd_init();
  btim_timx_int_init(65535, 24000 - 1);/*10Khz计数 6.5s超出*/
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "FPU TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 110, 200, 16, 16, "KEY0:+    KEY1:-", RED);
  lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:AUTO/MANUL", RED);
  delay_ms(1200);
  julia_clut_init(g_color_map);
  while (1)
  {

    key = key_scan(0);
    switch (key)
    {
    case KEY0_PRES:
      i++;
      if (i > sizeof(zoom_ratio) / 2 - 1)
        i = 0;
      break;
    case KEY1_PRES:
      if (i)i--;
      else i = sizeof(zoom_ratio) / 2 - 1;
      break;
    case WKUP_PRES:
      autorun = !autorun;
      break;
    default:
      break;
    }
    if (autorun == 1)
    {
      i++;

      if (i > sizeof(zoom_ratio) / 2 - 1)
      {
        i = 0;      /* 限制范围 */
      }
    }
    lcd_set_window(0, 0, lcddev.width, lcddev.height);  /* 设置窗口 */
    lcd_write_ram_prepare();

    TIM6->CNT = 0;    /*重设置TIM6的计数值*/
    g_timeout = 0;
    julia_generate_fpu(lcddev.width, lcddev.height, lcddev.width / 2, lcddev.height / 2, zoom_ratio[i]);

    time = TIM6->CNT + (uint32_t)g_timeout * 65536;
    sprintf(buf, "%s: zoom:%d  runtime:%0.1fms\r\n", SCORE_FPU_MODE, zoom_ratio[i], time / 10);
    lcd_show_string(5, lcddev.height - 5 - 12, lcddev.width - 5, 12, 12, buf, RED); /* 显示当前运行的情况 */
    // printf("%s", buf); /* 打印到窗口 */
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

