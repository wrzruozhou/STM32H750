#include "main.h"
static void MPU_Config(void);

#define complex 1           /*使用简单的DSP还是复杂的DSP*/
#define FFT_LENGTH 1024

#if complex
float fft_inputbuf[FFT_LENGTH * 2];   /*FFT输入数组*/
float fft_outputbuf[FFT_LENGTH];      /*FFT输出数组*/
#endif

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


  uint16_t i;
  uint8_t t = 0, key, autorun = 0, res;
  char buf[50];
  float time;
  arm_cfft_radix4_instance_f32 scfft;
  /* Configure the MPU attributes */
  MPU_Config();
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
#if !complex
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "DSP BasicMath TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 120, 200, 16, 16, "No DSP runtime:", RED);
  lcd_show_string(30, 150, 200, 16, 16, "Use DSP runtime:", RED);

  while (1)
  {
    /*不使用DSP优化*/
    TIM6->CNT = 0;
    g_timeout = 0;
    res = sin_cos_test(PI / 6, 1000000, 0);
    time = TIM6->CNT + (uint32_t)g_timeout * 65536;
    sprintf(buf, "%0.1fms\r\n", time / 10);

    if (res == 0)
    {
      lcd_show_string(30 + 16 * 8, 120, 100, 16, 16, buf, BLUE);      /* 显示运行时间 */
    }
    else {
      lcd_show_string(30 + 16 * 8, 120, 100, 16, 16, "error!", BLUE); /* 显示当前运行状态 */
    }

    /*使用DSP优化*/
    TIM6->CNT = 0;
    g_timeout = 0;
    res = sin_cos_test(PI / 6, 1000000, 1);
    time = TIM6->CNT + (uint32_t)g_timeout * 65536;
    sprintf(buf, "%0.1fms\r\n", time / 10);

    if (res == 0)
    {
      lcd_show_string(30 + 16 * 8, 150, 100, 16, 16, buf, BLACK);      /* 显示运行时间 */
    }
    else {
      lcd_show_string(30 + 16 * 8, 150, 100, 16, 16, "error!", BLACK); /* 显示当前运行状态 */
    }
  }
#else
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "DSP FFT TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 130, 200, 16, 16, "KEY0:Run FFT", RED); /* 显示提示信息 */
  lcd_show_string(30, 160, 200, 16, 16, "FFT runtime:", RED); /* 显示提示信息 */

  arm_cfft_radix4_init_f32(&scfft, FFT_LENGTH, 0, 1);

  while (1)
  {
    key = key_scan(0);

    if (key == KEY0_PRES)
    {
      for (i = 0; i < FFT_LENGTH; i++)    /*生成信号序列*/
      {
        fft_inputbuf[2 * i] = 100 +
          10 * arm_sin_f32(2 * PI * i / FFT_LENGTH) +
          30 * arm_sin_f32(2 * PI * i * 4 / FFT_LENGTH) +
          50 * arm_cos_f32(2 * PI * i * 8 / FFT_LENGTH);      /*生成输入信号实部*/

        fft_inputbuf[2 * i + 1] = 0;                            /*虚部全为0*/
      }
      TIM6->CNT = 0;
      g_timeout = 0;
      arm_cfft_radix4_f32(&scfft, fft_inputbuf);        /*FFT计算(基4)*/
      /*计算所用的时间*/
      time = TIM6->CNT + (uint32_t)g_timeout * 65536;
      /*显示运行时间*/
      sprintf((char*)buf, "%0.3fms\r\n", time / 1000);
      lcd_show_string(30 + 12 * 8, 160, 100, 16, 16, buf, BLUE);
      /*把运算结果复数求模得幅值*/
      arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);
      printf("\r\n%d pint FFT runtime:%0.3fms\r\n", FFT_LENGTH, time / 1000);
      printf("FFT Result:\r\n");

      for (i = 0; i < FFT_LENGTH; i++)
      {
        printf("fft_outputbuf[%d]:%f\r\n", i, fft_outputbuf[i]);
      }
    }
    else {
      delay_ms(10);
    }
  }
#endif
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

