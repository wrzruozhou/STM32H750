#include "main.h"

static void MPU_Config(void);
uint32_t sysclock = 0;

const char* SRAM_NAME_BUF[SRAMBANK] = { "SRAMIN", "SRAM12", "SRAM4   ", "SRAMDTCM", "SRAMITCM" };

#if 0
#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif
#endif
char temp_read[128];

int main(void)
{
  uint8_t paddr[20];                  /* 存放P Addr:+p地址的ASCII的值 */
  uint16_t memused = 0;
  uint8_t* p = 0;
  uint8_t* tp = 0;
  uint8_t sramx = 0;                  /*默认为内部SRAM*/

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
  mpu_memory_protection();
  LED_Config();
  usart_init(115200);
  usmart_dev.init(240);
  Key_Init();

  at24cxx_init();
  lcd_init();
  // gt9xxx_init();
  tp_dev.init();
  load_draw_dialog();
  //ctp_test();
  my_mem_init(SRAMIN);  /*初始化内部内存池(AXI)*/
  my_mem_init(SRAM12);  /*初始化SRAM12内存池(SRAM1+SRAM2)*/
  my_mem_init(SRAM4);   /*初始化SRAM4内存池(SRAM4)*/
  my_mem_init(SRAMDTCM);/*初始化DTCM内存池(DTCM)*/
  my_mem_init(SRAMITCM);/*初始化ITCM内存池(ITCM)*/

  lcd_show_string(30, 40, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 60, 200, 16, 16, "MALLOC TEST", RED);
  lcd_show_string(30, 80, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 100, 200, 16, 16, "KEY0:Malloc & WR & Show", RED);
  lcd_show_string(30, 120, 200, 16, 16, "KEY_UP:SRAMx KEY1:Free", RED);

  lcd_show_string(30, 140, 200, 16, 16, "SRAMIN ", BLUE);
  lcd_show_string(30, 156, 200, 16, 16, "SRAMIN   USED:", BLUE);
  lcd_show_string(30, 172, 200, 16, 16, "SRAM12   USED:", BLUE);
  lcd_show_string(30, 188, 200, 16, 16, "SRAM4    USED:", BLUE);
  lcd_show_string(30, 204, 200, 16, 16, "SRAMDTCM USED:", BLUE);
  lcd_show_string(30, 220, 200, 16, 16, "SRAMITCM USED:", BLUE);

  while (1)
  {
    key = key_scan();     /*不支持连按*/
    switch (key)
    {
    case KEY0_PRES:     /* KEY0按下 */
      p = mymalloc(sramx, 2048);  /* 申请2K字节，并写入内容，显示在LCD上面 */

      if (p != NULL)
      {
        sprintf((char*)p, "Memory Malloc Test%03d", i);        /* 向p写入一些内容 */
        lcd_show_string(30, 280, 209, 16, 16, (char*)p, BLUE); /* 显示p的内容 */
      }
      break;

    case KEY1_PRES:         /* KEY1按下 */
      myfree(sramx, p);   /* 释放内存 */
      p = 0;              /* 指向空地址 */
      break;

    case WKUP_PRES:         /* KEY UP按下 */
      sramx++;
      if (sramx > 4)sramx = 0;
      lcd_show_string(30, 140, 200, 16, 16, (char*)SRAM_NAME_BUF[sramx], BLUE);
      break;
    }

    if (tp != p && p != NULL)
    {
      tp = p;
      sprintf((char*)paddr, "P Addr:0X%08X", (uint32_t)tp);
      lcd_show_string(30, 260, 209, 16, 16, (char*)paddr, BLUE); /* 显示p的地址· */

      if (p)
      {
        lcd_show_string(30, 280, 280, 16, 16, (char*)p, BLUE); /* 显示p的内容 */
      }
      else
      {
        lcd_fill(30, 280, 209, 296, WHITE); /* p=0，清除显示*/
      }
    }

    delay_ms(10);
    i++;

    if ((i % 20) == 0)
    {
      memused = my_mem_perused(SRAMIN);
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      lcd_show_string(30 + 112, 156, 200, 16, 16, (char*)paddr, BLUE);   /* 显示内部内存使用率 */

      memused = my_mem_perused(SRAM12);
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      lcd_show_string(30 + 112, 172, 200, 16, 16, (char*)paddr, BLUE);   /* 显示SRAM12内存使用率 */

      memused = my_mem_perused(SRAM4);
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      lcd_show_string(30 + 112, 188, 200, 16, 16, (char*)paddr, BLUE);   /* 显示SRAM4使用率 */

      memused = my_mem_perused(SRAMDTCM);
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      lcd_show_string(30 + 112, 204, 200, 16, 16, (char*)paddr, BLUE);   /* 显示外部内存使用率 */

      memused = my_mem_perused(SRAMITCM);
      sprintf((char*)paddr, "%d.%01d%%", memused / 10, memused % 10);
      lcd_show_string(30 + 112, 220, 200, 16, 16, (char*)paddr, BLUE);   /* 显示ITCM使用率 */

      HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
    }
#if 0
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
