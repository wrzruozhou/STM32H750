#include "drv_remote.h"

/*采用了定时器1*/
TIM_HandleTypeDef htim1_remote;
/**
 * @brief   红外遥控初始化
 * @note    设置IO及定时器的输入捕获
 * @param   无
 * @retval  无
*/
void remote_init(void)
{
    /*初始化TIM1输入捕获参数*/
    TIM_IC_InitTypeDef sConfig;

    htim1_remote.Instance = TIM1;
    htim1_remote.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1_remote.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1_remote.Init.Period = 10000;
    htim1_remote.Init.Prescaler = 239;
    HAL_TIM_IC_Init(&htim1_remote);


    sConfig.ICPolarity = TIM_ICPOLARITY_RISING;
    sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;             /*映射到T1上*/
    sConfig.ICFilter = 0x03;
    sConfig.ICPrescaler = TIM_ICPSC_DIV1;
    HAL_TIM_IC_ConfigChannel(&htim1_remote, &sConfig, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim1_remote, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_IT(&htim1_remote, TIM_IT_UPDATE);
}

/**
 * @brief   定时器1的底层驱动，初始化PAB
 * @param   htim:定时器句柄
 * @note    此函数会被HAL_TIM_IC_Init()调用
 * @retval  无
*/
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM1)
    {
        GPIO_InitTypeDef GPIO_Init;
        __HAL_RCC_TIM1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_Init.Mode = GPIO_MODE_AF_PP;
        GPIO_Init.Pin = GPIO_PIN_8;
        GPIO_Init.Pull = GPIO_PULLUP;
        GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_Init.Alternate = GPIO_AF1_TIM1;
        HAL_GPIO_Init(GPIOA, &GPIO_Init);
    }
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);

    /*如果是通用定时器就不需要设置CC中断*/
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 1, 2);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
}


/**遥控器接收状态
 *  [7] 收到了引导码标志
 *  [6] 得到了一个按键的所有信息
 *  [5] 保留
 *  [4] 标记上升沿是否被捕获
 *  [3:0]   溢出时间计数器
*/
uint8_t g_remote_sta = 0;
uint32_t g_remote_data = 0; /*红外接收到的数据*/
uint8_t g_remote_cnt = 0;   /*按键按下的次数*/

/**
 * @brief   定时器中断服务函数
 * @param   无
 * @retval  无
 * @author  万若州
*/
void TIM1_UP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1_remote);
}

/**
 * @brief   定时器输入捕获中断服务函数
 * @param   无
 * @retval  无
*/
void TIM1_CC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim1_remote);

}

/**
 * @brief   定时器更新中断回调函数
 * @param   htim:定时器句柄
 * @retval  无
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM1)
    {
        if (g_remote_sta & 0x80)     /*上次有数据被接收到了*/
        {
            g_remote_sta &= ~0x10;
            if ((g_remote_sta & 0x0f) == 0)
            {
                g_remote_sta |= (1 << 6);   /*标记完成一次按键数据采集*/
            }
            if ((g_remote_sta & 0x0f) < 14)
            {
                g_remote_sta++;
            }
            else
            {
                g_remote_sta &= ~(1 << 7);  /*清除引导标识*/
                g_remote_sta &= 0xf0;       /*清空计数器*/
            }
        }
    }
}

/**
 * @brief   定时器输入捕获中断回调函数
 * @param   htim:定时器句柄
 * @retval  无
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM1)
    {
        uint16_t dval;  /*下降沿时计数器的值*/
        if (RDATA)
        {
            TIM_RESET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1);
            TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
            __HAL_TIM_SET_COUNTER(htim, 0);
            g_remote_sta |= 0X10;       /*标记上升沿被捕获*/
        }
        else
        {
            dval = HAL_TIM_ReadCapturedValue(&htim1_remote, TIM_CHANNEL_1);
            TIM_RESET_CAPTUREPOLARITY(&htim1_remote, TIM_CHANNEL_1);
            TIM_SET_CAPTUREPOLARITY(&htim1_remote, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING);

            if (g_remote_sta & 0x10)        /*完成一次高电平捕获*/
            {
                if (g_remote_sta & 0x80)     /*接收到了引导码*/
                {
                    if (dval > 300 && dval < 800)     /*560为标准值,560us*/
                    {
                        // printf("low");
                        g_remote_data >>= 1;
                        g_remote_data &= ~(0x80000000);
                    }
                    else if (dval > 1400 && dval < 1800)
                    {
                        // printf("high");
                        g_remote_data >>= 1;
                        g_remote_data |= 0x80000000;
                    }
                    else if (dval > 2000 && dval < 3000)
                    {
                        g_remote_cnt++;                     /*cnt计数值+1*/
                        g_remote_sta &= 0xf0;               /*清除累计计数值*/
                    }
                }
                else if (dval > 4200 && dval < 4700)        /*4500为标准值4.5ms*/
                {
                    g_remote_sta |= 1 << 7;
                    g_remote_cnt = 0;                       /*清除按键次数计数器*/
                }
            }
            g_remote_sta &= ~(1 << 4);
        }
    }
}

/**
 * @brief   处理红外按键
 * @param   无
 * @retval  0,没有任何按键按下
 *          其他，按下的按键值
*/
uint8_t remote_scan(void)
{
    uint8_t sta = 0;
    uint8_t t1, t2;
    if (g_remote_sta & (1 << 6))        /*得到一个按键的所有信息了*/
    {
        t1 = g_remote_data;                             /*得到地址码*/
        t2 = (g_remote_data >> 8) & 0xff;               /*得到地址反码*/
        if ((t1 == (uint8_t)~t2) && t1 == REMOTE_ID)    /*检验遥控识别码(ID)及地址*/
        {
            t1 = (g_remote_data >> 16) & 0xff;
            t2 = (g_remote_data >> 24) & 0xff;

            if (t1 == (uint8_t)~t2)
            {
                sta = t1;                               /*键值正确*/
            }
        }

        if ((sta == 0) || ((g_remote_sta & 0x80) == 0))
        {
            g_remote_sta &= ~(1 << 6);
            g_remote_cnt = 0;
        }
    }
    return sta;
}
