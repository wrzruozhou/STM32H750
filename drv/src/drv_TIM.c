#include "drv_TIM.h"

TIM_HandleTypeDef htim_Int;

/**
 * @brief   通用定时器TIM定时中断初始化函数
 * @note    通用定时器的时钟来自apb1,当D2PPRE1>=2分频的时候
 *          通用定时器的时钟为ApB1时钟的两倍，而APB1为120Mhz，所以定时器时钟为240Mhz
 *          定时器溢出时间的计算方法为:Tout = ((arr + 1) * (psc + 1))/Ft us
 *          Ft = 定时器工作频率，单位为Mhz
 * @param   arr:自动重装值
 * @param   psc:时钟预分频数
 * @retval  无
*/
void TIMx_Int_Init(uint16_t arr, uint16_t psc)
{
    __HAL_RCC_TIM3_CLK_ENABLE();                                            //使能tim3的时钟
    htim_Int.Instance = TIM3;                                      //通用定时器x
    htim_Int.Init.Period = arr;                                    //自动重装载值
    htim_Int.Init.Prescaler = psc;                                 //预分频系数
    htim_Int.Init.CounterMode = TIM_COUNTERMODE_UP;                //向上计数模式
    HAL_TIM_Base_Init(&htim_Int);

    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);      //设置中断优先级
    HAL_NVIC_EnableIRQ(TIM3_IRQn);                                          //开启tim3中断

    HAL_TIM_Base_Start_IT(&htim_Int);                              //使能tim3和更新中断
}

TIM_HandleTypeDef htim_InputCaptue;
/**
 * @brief   通用定时器TIM5 通道Y    输入捕获    初始化函数
 * @note    apb*2也就是120Mhz*2=240Mhz
 * @param   arr:自动重装值
 * @param   psc:时钟预分频数
 * @retval  无
*/

void TIMx_InputCapture_Init(uint16_t arr, uint16_t psc)
{
    TIM_IC_InitTypeDef sConfig_InputCapture;
    __HAL_RCC_TIM5_CLK_ENABLE();
    htim_InputCaptue.Instance = TIM5;
    htim_InputCaptue.Init.Prescaler = psc;
    htim_InputCaptue.Init.Period = arr;
    htim_InputCaptue.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_IC_Init(&htim_InputCaptue);

    sConfig_InputCapture.ICPolarity = TIM_ICPOLARITY_RISING;
    sConfig_InputCapture.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfig_InputCapture.ICPrescaler = TIM_ICPSC_DIV1;
    sConfig_InputCapture.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htim_InputCaptue, &sConfig_InputCapture, TIM_CHANNEL_1);

    __HAL_TIM_ENABLE_IT(&htim_InputCaptue, TIM_IT_UPDATE);
    HAL_TIM_IC_Start_IT(&htim_InputCaptue, TIM_CHANNEL_1);
}

/**
 * @brief   通用定时器输入捕获初始化接口
 *          HAL库调用的接口，用于配置不同的输入捕获
 * @param   htim:定时器句柄
 * @retval  无
*/
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM5)
    {
        GPIO_InitTypeDef  GPIO_Init;
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_TIM5_CLK_ENABLE();
        GPIO_Init.Mode = GPIO_MODE_AF_PP;
        GPIO_Init.Pin = GPIO_PIN_0;
        GPIO_Init.Pull = GPIO_PULLDOWN;
        GPIO_Init.Alternate = GPIO_AF2_TIM5;
        GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_Init);

        /*开启中断*/
        HAL_NVIC_SetPriority(TIM5_IRQn, 1, 3);
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
    }
}

uint8_t g_timxchy_cap_sta = 0;      /*输入捕获状态*/
uint16_t g_timxchy_cap_val = 0;     /*输入捕获值*/

/**
 * @brief   定时器输入捕获中断处理回调函数
 * @param   无
 * @tetval  无
*/
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim_InputCaptue);
}

/**
 * @brief   定时器输入捕获中断处理回调函数
 * @param   hitm:定时器句柄指针
 * @note    该函数会在HAL_TIM_IRQHandler中会被调用
 * @retval  无
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM5)
    {
        if ((g_timxchy_cap_sta & 0x80) == 0)  /*还没成功捕获*/
        {
            if (g_timxchy_cap_sta & 0x40)    /*捕获到一个下降沿*/
            {
                g_timxchy_cap_sta |= 0x80;  /*标记成功捕获到一个高电平脉宽*/
                g_timxchy_cap_val = HAL_TIM_ReadCapturedValue(&htim_InputCaptue, TIM_CHANNEL_1);    /*获取当前的捕获值*/
                TIM_RESET_CAPTUREPOLARITY(&htim_InputCaptue, TIM_CHANNEL_1);                                /*清除原来的设置*/
                TIM_SET_CAPTUREPOLARITY(&htim_InputCaptue, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);          /*配置TIM5通道1上升沿捕获*/
            }
            else
            {
                g_timxchy_cap_sta = 0;
                g_timxchy_cap_sta = 0;
                g_timxchy_cap_sta |= 0x40;  /*标记捕获到了上升沿*/
                __HAL_TIM_SET_COUNTER(&htim_InputCaptue, 0);
                TIM_RESET_CAPTUREPOLARITY(&htim_InputCaptue, TIM_CHANNEL_1);
                TIM_SET_CAPTUREPOLARITY(&htim_InputCaptue, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
            }
        }
    }
}

/**
 * @brief   定时器更新中断回调函数
 * @param   htim:定时器句柄指针
 * @note    此函数会被定时器中断函数共同调用的
 * @retval  无
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM5)
    {
        if (g_timxchy_cap_sta & 0x80 == 0)      /*还没捕获到高电平*/
        {
            if (g_timxchy_cap_sta & 0x40)       /*已经捕获到高电平*/
            {
                if ((g_timxchy_cap_sta & 0x3f) == 0x3f)     /*高电平时间太长了*/
                {
                    TIM_RESET_CAPTUREPOLARITY(&htim_InputCaptue, TIM_CHANNEL_1);                        /*复位捕获极性*/
                    TIM_SET_CAPTUREPOLARITY(&htim_InputCaptue, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);   /*设置为上升沿触发*/
                    g_timxchy_cap_sta |= 0x80;
                    g_timxchy_cap_val = 0xffff;
                }
                else
                {
                    g_timxchy_cap_sta++;
                }
            }
        }
    }
}

