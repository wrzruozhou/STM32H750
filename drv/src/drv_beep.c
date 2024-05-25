#include "drv_beep.h"
#include "stdio.h"

/**
 * @brief   BEEP初始化函数
 *  @note   只能输出高低电平，只能适用于有源蜂鸣器
 * @param   无
 * @retval  无
*/
void BEEP_Init(void)
{
    BEEP_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Init;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pin = BEEP_PIN;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(BEEP_PORT, &GPIO_Init);
}

/**
 * @brief   使用PWM驱动蜂鸣器
 *  @note   适用于有源和无源蜂鸣器
 * @param   psc:定时器预分频数
 * @param   arr:重装载值
 * @retval  无
*/
TIM_HandleTypeDef htim_beep;                            /*定时器句柄*/
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;    /*死区时间设置*/
void BEEP_PWM_Init(uint16_t psc, uint16_t arr)
{
    TIM_OC_InitTypeDef sConfig;
    htim_beep.Instance = TIM15;
    htim_beep.Init.Prescaler = psc;                     /*预分频数*/
    htim_beep.Init.Period = arr;                        /*自动重装载值*/
    htim_beep.Init.CounterMode = TIM_COUNTERMODE_UP;    /*向上计数模式*/
    htim_beep.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;   /*使能影子寄存器*/
    HAL_TIM_PWM_Init(&htim_beep);                 /*初始化定时器*/


    sConfig.OCMode = TIM_OCMODE_PWM1;                   /*使用PWM1*/
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;            /*输出极性比较为低*/
    sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;          /*互补输出极性为低*/
    sConfig.OCIdleState = TIM_OCIDLESTATE_SET;          /*OC = 1*/
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_SET;        /*OCN = 1*/
    // sConfig.Pulse = arr / 2;                            /*占空比为50*/
    HAL_TIM_PWM_ConfigChannel(&htim_beep, &sConfig, TIM_CHANNEL_2);     /*配置通道2*/

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;            /*运行模式的关闭输出状态*/
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;           /*空闲模式的关闭输出状态*/
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;                 /*不用寄存器锁功能*/
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;                 /*使能刹车输入*/
    sBreakDeadTimeConfig.BreakFilter = 0;                               /*刹车输入信号滤波设置*/
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW;         /*刹车输入有效信号极性*/
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;              /*不适用断路2*/
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;  /*使能AOE位，允许刹车结束后自动恢复输出*/
    HAL_TIMEx_ConfigBreakDeadTime(&htim_beep, &sBreakDeadTimeConfig);

    HAL_TIM_PWM_Start(&htim_beep, TIM_CHANNEL_1);               /*使能PWM*/
    HAL_TIMEx_PWMN_Start(&htim_beep, TIM_CHANNEL_1);            /*使能OCN输出*/
}

/**
 * @brief   底层硬件初始化
 *  @note   会被HAL_TIM_PWM_Init调用
*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM15)
    {
        /*必须按照相应的功能初始化相关的引脚*/

        //使能时钟
        BEEP_CLK_ENABLE();
        __HAL_RCC_TIM15_CLK_ENABLE();
        //初始化PE6
        GPIO_InitTypeDef GPIO_Init;
        GPIO_Init.Alternate = GPIO_AF4_TIM15;
        GPIO_Init.Mode = GPIO_MODE_AF_PP;
        GPIO_Init.Pin = BEEP_PIN;
        GPIO_Init.Pull = GPIO_NOPULL;
        GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(BEEP_PORT, &GPIO_Init);

        GPIO_Init.Pin = BEEP_NPIN;
        HAL_GPIO_Init(BEEP_NPORT, &GPIO_Init);
        /*
            GPIO_Init.Pin = BEEP_BIN_PIN;
            HAL_GPIO_Init(BEEP_BIN_PORT, &GPIO_Init);
        */
    }
}

/**
 * @brief   定时器TIMX 设置输出比较值&和死区时间
 * @param   ccr:输出比较值
 * @param   dtg:死区时间
 *  @arg    dtg[7:5]=0xx时，死区时间 = dtg[7:0] * tDTS
 *  @arg    dtg[7:5]=10x时，死区时间 = (64 + dtg[5:0] * 2 *tDTS)
 *  @arg    dtg[7:5]=110时，死区时间 = (64 + dtg[5:0] * 8 *tDTS)
 *  @arg    dtg[7:5]=111时，死区时间 = (64 + dtg[5:0] * 16 *tDTS)
 *  @note   tDTS = 1 /(Ft / CKD[1:0]) = 1 / 60M = 16.67ns
 * @retval  无
*/
void BEEP_SETPWM(uint16_t ccr, uint8_t dtg)
{
    sBreakDeadTimeConfig.DeadTime = dtg;        /*死区时间设置*/
    HAL_TIMEx_ConfigBreakDeadTime(&htim_beep, &sBreakDeadTimeConfig);
    __HAL_TIM_MOE_ENABLE(&htim_beep);           /*MOE = 1,使能主输出*/
    TIM15->CCR1 = ccr;                          /*设置比较寄存器*/
}

/**
 * @brief   设置预分频数，更改TIM的频率
 * @param   psc:预分频数
 * @retval  无
*/
void BEEP_SETPSC(uint16_t psc)
{
    __HAL_TIM_SET_PRESCALER(&htim_beep, psc);
}




