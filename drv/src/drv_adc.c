#include "drv_adc.h"

ADC_HandleTypeDef adc_handle;
DMA_HandleTypeDef adc_dma_handle;
uint8_t g_adc_dma_flag = 0;

#if NO_USE_DMA
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
#else
/**
 * @brief  ADC初始化函数
 *  @note   16位精度 32Mhz 采样周期+8.5个ADC周期 810.5，转换时间为 819个周期为25.6us
 * @param   par 外设地址
 * @param   mar 存储地址
 * @retval  无
 */
void adc_init_dma(uint32_t par, uint32_t mar)
{
    GPIO_InitTypeDef GPIO_Init;
    __HAL_RCC_DMA1_CLK_ENABLE();
    /*初始化必要的外设ADC时钟*/
    __HAL_RCC_ADC12_CLK_ENABLE();
    /*ADC外设时钟选择*/
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
    GPIO_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5; /* GPIOA0~5 */
    GPIO_Init.Mode = GPIO_MODE_ANALOG;
    GPIO_Init.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

    // adc_dma_handle.Instance = DMA1_Stream0;
    // adc_dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    // adc_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    // adc_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    // adc_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    // adc_dma_handle.Init.Mode = DMA_NORMAL;
    // adc_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    // adc_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    // adc_dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    // adc_dma_handle.Init.Request = DMA_REQUEST_ADC1;

    adc_dma_handle.Instance = DMA1_Stream0;
    adc_dma_handle.Init.Request = DMA_REQUEST_ADC1;
    adc_dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    adc_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    adc_dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    adc_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    adc_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    adc_dma_handle.Init.Mode = DMA_NORMAL;
    adc_dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    adc_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    HAL_DMA_Init(&adc_dma_handle);                                                                  /*初始化DMA*/

    __HAL_LINKDMA(&adc_handle, DMA_Handle, adc_dma_handle);

    adc_handle.Instance = ADC1;                                                                         /*选择哪一个ADC*/
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;                                              /*输入时钟2分频,adc_ker_ck=per_ck/2=32Mhz*/
    adc_handle.Init.Resolution = ADC_RESOLUTION_12B;                                                    /*16位模式*/
    adc_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;                                                    /*扫描模式*/
    adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                                                 /*关闭EOC中断*/
    adc_handle.Init.LowPowerAutoWait = DISABLE;                                                         /*自动低功耗模式关闭*/
    adc_handle.Init.ContinuousConvMode = ENABLE;                                                       /*关闭持续转化*/
    adc_handle.Init.NbrOfConversion = 6;                                                            /*赋值范围是1-16，用到几个通道*/
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                                              /*禁用常规转换组不连续采样模式*/
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;                               /*根据上一位描述此位被忽略*/
    adc_handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;                                   /*数据存储到DR寄存器*/
    adc_handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                                                 /*有新的数据后直接覆盖掉旧的数据*/
    adc_handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;                                               /*设置ADC转化结果的左移位数*/
    adc_handle.Init.OversamplingMode = DISABLE;                                                         /*禁止过采样*/
    HAL_ADC_Init(&adc_handle);                                                                      /*初始化ADC*/
    /*ADC校准*/
    HAL_ADCEx_Calibration_Start(&adc_handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    /*ADC通道配置*/
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    sConfig.OffsetRightShift = DISABLE;
    sConfig.OffsetSignedSaturation = DISABLE;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);
    sConfig.Channel = ADC_CHANNEL_14;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    sConfig.Channel = ADC_CHANNEL_15;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    sConfig.Channel = ADC_CHANNEL_16;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    sConfig.Channel = ADC_CHANNEL_17;
    sConfig.Rank = ADC_REGULAR_RANK_4;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    sConfig.Channel = ADC_CHANNEL_18;
    sConfig.Rank = ADC_REGULAR_RANK_5;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    sConfig.Channel = ADC_CHANNEL_19;
    sConfig.Rank = ADC_REGULAR_RANK_6;
    HAL_ADC_ConfigChannel(&adc_handle, &sConfig);

    /*开启DMA中断*/
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

    HAL_DMA_Start_IT(&adc_dma_handle, par, mar, 0);
    HAL_ADC_Start_DMA(&adc_handle, &mar, 0);

}
#endif


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
#if NO_USE_DMA
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
#else
    if (hadc->Instance == ADC1)
    {
        // GPIO_InitTypeDef GPIO_init;
        // /*初始化必要的外设ADC时钟*/
        // __HAL_RCC_ADC12_CLK_ENABLE();
        // /*ADC外设时钟选择*/
        // __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);

        // GPIO_init.Mode = GPIO_MODE_ANALOG;
        // GPIO_init.Pin = GPIO_PIN_0;
        // HAL_GPIO_Init(GPIOA, &GPIO_init);

        // GPIO_init.Pin = GPIO_PIN_1;
        // HAL_GPIO_Init(GPIOA, &GPIO_init);

        // GPIO_init.Pin = GPIO_PIN_2;
        // HAL_GPIO_Init(GPIOA, &GPIO_init);

        // GPIO_init.Pin = GPIO_PIN_3;
        // HAL_GPIO_Init(GPIOA, &GPIO_init);

        // GPIO_init.Pin = GPIO_PIN_4;
        // HAL_GPIO_Init(GPIOA, &GPIO_init);

        // GPIO_init.Pin = GPIO_PIN_5;
        // HAL_GPIO_Init(GPIOA, &GPIO_init);
    }

#endif
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

/**
 *  @brief  使能一次ADC DMA传输
 *  @note   使用寄存器来操作，防止用HALL库操作对其他参数有修改
 * @param   ndtr:DMA传输的次数
 * @retval  无
 */
void adc_dma_enable(uint16_t ndtr)
{
    /*关闭ADC1*/
    ADC1->CR &= ~(1 << 0);

    /*关闭DMA传输*/
    DMA1_Stream0->CR &= ~(1 << 0);
    while (DMA1_Stream0->CR & 0x1);
    DMA1_Stream0->NDTR = ndtr;
    DMA1_Stream0->CR |= (1 << 0);

    ADC1->CR |= 1 << 0;         /*重新启动ADC*/
    ADC1->CR |= 1 << 2;         /*启动规则转换通道*/
}

/**
 * @brief   ADC DMA采集中断服务函数
 *  @param  无
 *  @retval 无
 */
void DMA1_Stream0_IRQHandler(void)
{
    if (__HAL_DMA_GET_FLAG(&adc_dma_handle, DMA_FLAG_TCIF0_4))
    {
        g_adc_dma_flag = 1;
        __HAL_DMA_CLEAR_FLAG(&adc_dma_handle, DMA_FLAG_TCIF0_4);
    }
}
