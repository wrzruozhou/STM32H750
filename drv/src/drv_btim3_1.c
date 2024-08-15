#include "drv_btim3_1.h"

extern uint8_t g_timeout;

TIM_HandleTypeDef g_timx_handle;

/**
 * @brief   基本定时器初始化
 * @param   arr:自动重装载值
 * @param   psc:预分频值
 * @retval  无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    g_timx_handle.Instance = TIM6;
    g_timx_handle.Init.Period = arr;
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_handle.Init.Prescaler = psc;
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_TIM_Base_Start_IT(&g_timx_handle);
}

/**
 * @brief   定时器底层驱动
 * @param   无
 * @param   无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
}

/**
 * @brief   定时器中断服务函数
 * @param   无
 * @param   无
 */
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);
}

/**
 * @brief   定时器更新中断回调函数
 * @param   htim:定时器句柄指针
 * @retval  无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6)
    {
        g_timeout++;
    }

}