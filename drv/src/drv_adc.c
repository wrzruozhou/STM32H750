#include "drv_adc.h"

ADC_HandleTypeDef adc_handle;

/**
 * @brief  ADC初始化函数
 *  @note   16位精度 32Mhz 采样周期+8.5个ADC周期 810.5，转换时间为 819个周期为25.6us
 * @param   无
 * @retval  无
 */
void adc_init(void)
{
    adc_handle.Instance = ADC1;                                                                         /*选择哪一个ADC*/
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;                                              /*输入时钟2分频,adc_ker_ck=per_ck/2=32Mhz*/
    adc_handle.Init.Resolution = ADC_RESOLUTION_16B;                                                    /*16位模式*/
    adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                                                    /*非扫描模式*/
    adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                                                 /*关闭EOC中断*/
    adc_handle.Init.LowPowerAutoWait = DISABLE;                                                         /*自动低功耗模式关闭*/
    adc_handle.Init.ContinuousConvMode = DISABLE;                                                       /*关闭持续转化*/
    adc_handle.Init.NbrOfDiscConversion = 1;                                                            /*赋值范围是1-16，用到几个通道*/
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                                              /*禁用常规转换组不连续采样模式*/
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;                               /*根据上一位描述此位被忽略*/
    adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;                                   /*数据存储到DR寄存器*/
    adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                                                 /*有新的数据后直接覆盖掉旧的数据*/
    adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;                                               /*设置ADC转化结果的左移位数*/
    adc_handle.Init.OversamplingMode = DISABLE;                                                         /*禁止过采样*/
    HAL_ADC_Init(&adc_handle);                                                                      /*初始化ADC*/

    /*ADC校准*/
    HAL_ADCEx_Calibration_Start(&adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
}

/**
  * @brief  Initialize the ADC MSP.
  * @param hadc ADC handle
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hadc);

    /* NOTE : This function should not be modified. When the callback is needed,
              function HAL_ADC_MspInit must be implemented in the user file.
     */

    if (hadc->Instance == ADC1)
    {
        GPIO_InitTypeDef GPIO_Init;

        /*初始化必要的外设ADC时钟*/
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /*ADC外设时钟选择*/
        __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);

        GPIO_Init.Pin = GPIO_PIN_5;
        GPIO_Init.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_Init);
    }
}

/**
 *  @brief  获得ADC转换后的结果
 *  @param  ch:通道值0~19,取值范围为:ADC_CHANNEL_0 ~ ADC_CHANNEL_19
 *  @retval 返回值:转换结果
 */
uint32_t adc_get_result(uint32_t ch)
{
    ADC_ChannelConfTypeDef sConfig = { 0 };
    sConfig.Channel = ch;                                                                               /*通道*/
    sConfig.Rank = ADC_REGULAR_RANK_1;                                                                  /*排名*/
    sConfig.SamplingTime = LL_ADC_SAMPLINGTIME_810CYCLES_5;                                             /*采样时间*/
    sConfig.SingleDiff = ADC_SINGLE_ENDED;                                                              /*单边采样*/
    sConfig.OffsetNumber = ADC_OFFSET_NONE;                                                             /*不使用偏移量的通道*/
    sConfig.Offset = 0;                                                                                 /*偏移量为0*/
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    /*开启ADC*/
    HAL_ADC_Start(&adc_handle);
    HAL_ADC_PollForConversion(&adc_handle, 10);
    return HAL_ADC_GetValue(&adc_handle);
}

/**
 *  @brief  获取通道ch的转换值,取times次然后取平均
 *  @param  ch  :通道号 0-19
 *  @param  times:获取的次数
 *  @retval 通道ch的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;            /*返回平均值*/

}
