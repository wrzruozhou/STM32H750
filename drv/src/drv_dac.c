#include "drv_dac.h"

DAC_HandleTypeDef dac1_handle;

/**
 * @brief   DAC初始化
 * @param   无
 * @retval  无
 */
void dac_init(void)
{
    GPIO_InitTypeDef dac1_gpio;

    __HAL_RCC_DAC12_CLK_ENABLE();                           /*启动DAC的传输时钟*/
    __HAL_RCC_GPIOA_CLK_ENABLE();                           /*开启GPIOA的时钟*/

    dac1_gpio.Mode = GPIO_MODE_ANALOG;
    dac1_gpio.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &dac1_gpio);       /*初始化DAC的GPIO*/

    dac1_handle.Instance = DAC1;
    HAL_DAC_Init(&dac1_handle);                         /*初始化DAC*/

    DAC_ChannelConfTypeDef dac1_config;
    dac1_config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    dac1_config.DAC_Trigger = DAC_TRIGGER_NONE;

    HAL_DAC_ConfigChannel(&dac1_handle, &dac1_config, DAC_CHANNEL_1);       /*配置DAC通道1*/
    HAL_DAC_Start(&dac1_handle, DAC_CHANNEL_1);                                     /*启动DAC通道1*/
    HAL_DAC_ConfigChannel(&dac1_handle, &dac1_config, DAC_CHANNEL_2);       /*配置DAC通道2*/
    HAL_DAC_Start(&dac1_handle, DAC1_CHANNEL_2);                                    /*启动DAC通道2*/
}

/**
 *  @brief  设置DAC通道输出值
 *  @param  channel:配置的通道
 *  @param  value:  数字量
 *  @retval 无
 */
void dac_channel_set(uint32_t channel, uint16_t value)
{
    uint16_t dac1_temp = 0;
    if (value > 4096)
    {
        dac1_temp = 4095;
    }
    else {
        dac1_temp = value;
    }
    HAL_DAC_SetValue(&dac1_handle, channel, DAC_ALIGN_12B_R, dac1_temp);
}


/**
 *  @brief  配置DMA
 *  @param  无
 *  @retval 无
 */
static void DAC_DMA_Config(void)
{
    DMA_HandleTypeDef dac1_hdma;
    /*使能DMA1/2时钟*/
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    dac1_hdma.Instance = DMA1_Stream0;                                                  /*数据流选择*/
    dac1_hdma.Init.Request = DMA_REQUEST_DAC1;                                          /*DMA通道选择*/
    // dac1_hdma.Init.Direction =
    HAL_DMA_Init(&dac1_hdma);
}
