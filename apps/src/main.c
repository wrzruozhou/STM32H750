#include "main.h"

static void MPU_Config(void);
uint32_t sysclock = 0;

int main(void)
{
  int i;
  /* Configure the MPU attributes */
  MPU_Config();
  sys_cache_enable();                  /* 打开L1-Cache */

  HAL_Init();

  sys_stm32_clock_init(240, 2, 2, 4);
  sysclock = HAL_RCC_GetSysClockFreq();
  LED_Config();
  usart_init(115200);
  // extix_init();
  Key_Init_IT();

  while (1)
  {
    // HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN);
    // delay_ms(1000);
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == 0)
      printf("fuck!\r\n");
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

