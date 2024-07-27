#include "drv_step.h"
/*------------------------------------------GPIO模拟PWM------------------------------------------------------------------*/
/**
 * @brief   GPIO初始化
 * @param   无
 * @retval   无
*/
void Step_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef  GPIO_Init;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Init.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_Init);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
}

/**
 * @brief   电机停转函数
 * @param   无
 * @retval  无
*/
void Step_Stop(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
}

/**
 * @brief   四相整步驱动函数
 * @param   StepNum 整步编号，0~3对应A~D
 * @param   delay延时
 * @retval  无
*/
void Step_SingleStep(uint8_t StepNum, uint16_t delay)
{
    switch (StepNum)
    {
    case 0:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);break;
    case 1:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);break;
    case 2:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15, GPIO_PIN_RESET);break;
    case 3:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);break;
        break;
    default:
        break;
    }
    delay_ms(delay);                    /*延时一段时间*/
    Step_Stop();                            /*关闭电机防止过热*/
}

/**
 * @brief   步进电机按步旋转
 * @param   step 旋转的步数
 * @param   delay 延时的时间
 * @retval  无
*/
void Step_rotate(uint32_t step, uint16_t delay)
{
    uint8_t STEP = 0;
    for (size_t i = 0; i < step; i++)
    {
        /* code */

        if (STEP > 3)
        {
            STEP = 0;
        }
        Step_SingleStep(STEP, delay);
        STEP++;
    }

}
//下面是main函数的主要信息
#if 0
//首先要初始化

Step_rotate(30 * 2048, 3);
delay_ms(3);
#endif

/*------------------------------------------GPIO模拟PWM结束------------------------------------------------------------------*/




/*------------------------------------------高级TIM输出PWM控制步进电机------------------------------------------------------------------*/

