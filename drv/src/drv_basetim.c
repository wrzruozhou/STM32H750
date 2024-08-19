#include "drv_basetim.h"
#include "drv_led.h"

/**
 * @brief   基本定时器TIMX定时器中断初始化函数
 * @note    基本定时器的时钟来自APB1，当D2PPRE1>2分频的时候，基本定时器
 *           时钟为APB1时钟的2倍 APB1为120Mhz，所以定时器时钟= 240Mhz
 *              如果使用TIM6定时器作为LVGL的心跳，请使用
 *                  BaseTim_init(10-1,24000-1)  1ms刷新一次
 * @param   arr:自动重装载值
 * @param   psc:时钟预分频数
 * @retval  无
*/
TIM_HandleTypeDef htim_basetim;
void BaseTim_init(uint16_t arr, uint16_t psc)
{
    htim_basetim.Instance = TIM6;               /*定时器6*/
    htim_basetim.Init.Prescaler = psc;          /*设置预分频系数*/
    htim_basetim.Init.Period = arr;             /*设置计数值*/
    htim_basetim.Init.CounterMode = TIM_COUNTERMODE_UP; /*向上计数*/
    HAL_TIM_Base_Init(&htim_basetim);

    HAL_TIM_Base_Start_IT(&htim_basetim);       /*使能定时器x和定时器x更新中断*/
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();            /*开启TIM6的时钟*/
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 3);      /*设置中断优先级*/
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);  /*开启TIM6中断*/
    }
}

/**
 * @brief   定时器中断服务函数
 * @param   无
 * @retval  无
*/
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim_basetim);  /*定时器中断公共处理函数*/
}

/**
 * @brief   定时器中断回调函数
 * @param   htim:定时器句柄指针
 * @retval  无
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM6)
    {
        //lv_tick_inc(1);         /*lvgl的1ms心跳*/
			HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
    }

}
