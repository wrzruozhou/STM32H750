/*this Project was changed from usart*/

#include "main.h"


static void MPU_Config(void);
uint32_t sysclock = 0;
char READ_DA0[] = { 0x2, 0x30, 0x31, 0x30, 0x45, 0x43, 0x30, 0x32, 0x03, 0x3, 0x37, 0x45 };
char READ_DA1[] = { 0x2, 0x30, 0x31, 0x30, 0x45, 0x45, 0x30, 0x32, 0x03, 0x3, 0x38, 0x45 };
char WRITE_AD0[15] = { 0x2, 0x31, 0x31, 0x30, 0x45, 0x45, 0x30, 0x32, 0x0, 0x0, 0x0, 0x0, 0x3, 0, 0 };
void (DEC_To_ASCII)(uint16_t value, char* buf)
{
  // buf[0] = ((value << 8) >> 12) & (0x0F) + 0x30;
  // buf[1] = ((value << 8) >> 8) & (0x0f) + 0x30;

  buf[0] = ((((value & (0x00f0)) << 8) >> 12) & 0xf);
  buf[1] = ((((value & (0x000f)) << 8) >> 8) & 0xf);
  buf[2] = (((value & (0xf000)) >> 12) & 0xf);
  buf[3] = ((((value & (0x0f00)) << 4) >> 12) & 0xf);
  for (size_t i = 0; i < 4; i++)
  {
    if (buf[i] >= 0x0a && buf[i] <= 0x0f)
    {
      buf[i] += 0x37;
    }
    else
    {
      buf[i] += 0x30;
    }
  }

  // for (size_t i = 0; i < 4; i++)
  // {
  //   printf("%x ", buf[i]);
  // }
  // printf("\n");
}


int temp = 15;


int main(void)
{
  uint32_t adc_temp;
  uint16_t sum = 0;
  int i;
  int temp2;
  char temp_real[4];
  float volate;
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable();                  /* 打开L1-Cache */

  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  LED_Config();
  usart_init(115200);

  // dac_init();
  // dac_channel_set(DAC1_CHANNEL_1, 0);
  // dac_channel_set(DAC1_CHANNEL_2, 0);

  adc_init();


  while (1)
  {
    adc_temp = adc_get_result_average(ADC_CHANNEL_19, 10);
    printf("the single value is %d\n", adc_temp);

    HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
    DEC_To_ASCII(1111, temp_real);
    WRITE_AD0[8] = temp_real[0];
    WRITE_AD0[9] = temp_real[1];
    WRITE_AD0[10] = temp_real[2];
    WRITE_AD0[11] = temp_real[3];

    for (size_t i = 1; i <= 12; i++)
    {
      sum += WRITE_AD0[i];
    }
    WRITE_AD0[13] = ((((sum & (0x00f0)) << 8) >> 12) & 0xf);
    WRITE_AD0[14] = ((((sum & (0x000f)) << 8) >> 8) & 0xf);
    if (WRITE_AD0[13] >= 0x0a && WRITE_AD0[13] <= 0x0f)
      WRITE_AD0[13] += 0x37;
    else
      WRITE_AD0[13] += 0x30;

    if (WRITE_AD0[14] >= 0x0a && WRITE_AD0[14] <= 0x0f)
      WRITE_AD0[14] += 0x37;
    else
      WRITE_AD0[14] += 0x30;
    sum = 0;
    // HAL_UART_Transmit(&g_uart1_handle, WRITE_AD0, sizeof(WRITE_AD0), 10);
    if (idle_flag == 1)
    {
      idle_flag = 0;
      volate = (float)recv_dac1_data / 4095 * 3.3;
      dac_channel_set(DAC1_CHANNEL_1, recv_dac1_data);
      dac_channel_set(DAC1_CHANNEL_2, recv_dac1_data);
      printf("理论电压为:%f", volate);
    }


    if (g_rx_flag == 1)
    {
      HAL_UART_Transmit(&g_uart1_handle, g_usart_rx_buf, g_usart_rx_sta, 10);
      for (i = 0;i < g_usart_rx_sta;i++)
      {
        g_usart_rx_buf[i] = 0;
        g_usart_rx_sta = 0;
        g_rx_flag = 0;
      }
    }


    HAL_Delay(1000);
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

