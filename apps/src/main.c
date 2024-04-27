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
  char* str = 0;
  uint8_t key = 0;
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable(); /* 打开L1-Cache */

  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  LED_Config();
  usart_init(115200);
  usmart_dev.init(240);

  mpu_memory_protection();
  usmart_dev.init(240);
  // Key_Init();
  mpu_memory_protection();
  lcd_init();

  // lcd_fill(100, 100, 300, 200, WHITE);
  // lcd_draw_point(400, 400, WHITE);
  // lcd_draw_line(0, 0, 250, 600, YELLOW);
  // lcd_draw_hline(200, 700, 200, BLUE);
  // lcd_draw_rectangle(250, 200, 450, 600, WHITE);
  // lcd_draw_circle(300, 50, 30, BLUE);
  // lcd_fill_circle(150, 200, 100, GREEN);
  lcd_show_char(100, 200, '%', 32, 1, YELLOW);
  lcd_show_num(100, 300, 550, 3, 32, GREEN);
  lcd_show_xnum(100, 350, 100, 3, 24, 1, GREEN);
  lcd_show_string(20, 600, 100, 100, 12, "xihaiqingge", GREEN);
  while (1)
  {
    key = remote_scan();
    if (key)
    {
      switch (key)
      {
      case 0:
        str = "ERROR";
        break;

      case 69:
        str = "POWER";
        break;

      case 70:
        str = "UP";
        break;

      case 64:
        str = "PLAY";
        break;

      case 71:
        str = "ALIENTEK";
        break;

      case 67:
        str = "RIGHT";
        break;

      case 68:
        str = "LEFT";
        break;

      case 7:
        str = "VOL-";
        break;

      case 21:
        str = "DOWN";
        break;

      case 9:
        str = "VOL+";
        break;

      case 22:
        str = "1";
        break;

      case 25:
        str = "2";
        break;

      case 13:
        str = "3";
        break;

      case 12:
        str = "4";
        break;

      case 24:
        str = "5";
        break;

      case 94:
        str = "6";
        break;

      case 8:
        str = "7";
        break;

      case 28:
        str = "8";
        break;

      case 90:
        str = "9";
        break;

      case 66:
        str = "0";
        break;

      case 74:
        str = "DELETE";
        break;
      }/* ÏÔÊ¾SYMBOL */
      printf("%s\n", str);
    }

    t++;
    if (t > 20)
    {
      t = 0;
      HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
      HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);
      HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN);
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
