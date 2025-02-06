#include "main.h"

static void MPU_Config(void);
uint32_t sysclock = 0;

#if 0
#if !(__ARMCC_VERSION >= 6010050)
uint8_t mpudata[128] __attribute__((at(0X20002000)));
#else
uint8_t mpudata[128] __attribute__((section(".bss.ARM.__at_0X20002000")));
#endif
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
  uint8_t key = 0;
  uint8_t res = 0;
  uint32_t total, free;
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

  my_mem_init(SRAMIN);
  my_mem_init(SRAM12);
  my_mem_init(SRAM4);
  my_mem_init(SRAMDTCM);
  my_mem_init(SRAMITCM);

  at24cxx_init();
  lcd_init();
  /* 打印SD卡相关信息 */
  show_sdcard_info();


  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "FATFS TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 110, 200, 16, 16, "Use USMART for test", RED);

  while (sd_init())
  {
    lcd_show_string(30, 150, 200, 16, 16, "SD Card Error!", RED);
    delay_ms(500);
    lcd_show_string(30, 150, 200, 16, 16, "Please Check! ", RED);
    delay_ms(500);
    HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
  }
  exfuns_init();        /*为fatfs相关变量申请内存*/
  res = f_mount(fs[0], "0", 1);     /*挂载SD卡*/
  lcd_fill(30, 150, 240, 150 + 16, WHITE);        /* 清除显示 */
  while (exfuns_get_free("0", &total, &free))     /* 得到SD卡的总容量和剩余容量 */
  {
    lcd_show_string(30, 150, 200, 16, 16, "SD Card Fatfs Error!", RED);
    delay_ms(200);
    lcd_fill(30, 150, 240, 150 + 16, WHITE);    /* 清除显示 */
    delay_ms(200);
    HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);                            /* LED0闪烁 */
  }
  lcd_show_string(30, 150, 200, 16, 16, "FATFS OK!", BLUE);
  lcd_show_string(30, 170, 200, 16, 16, "SD Total Size:     MB", BLUE);
  lcd_show_string(30, 190, 200, 16, 16, "SD Free Size:     MB", BLUE);
  lcd_show_num(30 + 8 * 14, 170, total >> 10, 5, 16, BLUE);   /* 显示SD卡总容量 MB */
  lcd_show_num(30 + 8 * 14, 190, free >> 10, 5, 16, BLUE);    /* 显示SD卡剩余容量 MB */
  while (1)
  {
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);
    delay_ms(2000);
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
