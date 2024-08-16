#include "drv_key.h"

void Key_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Init;
    GPIO_Init.Mode = GPIO_MODE_INPUT;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Pin = KEY_0_Pin | KEY_1_Pin;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

    GPIO_Init.Mode = GPIO_MODE_INPUT;
    GPIO_Init.Pull = GPIO_PULLDOWN;
    GPIO_Init.Pin = KEY_UP_Pin;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);
}
#if 0
/*
 * @brief 按键扫描函数
*/
uint8_t key_scan(uint8_t mode)
{
    /*两个低电平触发一个高电平触发*/
    if (KEY_UP_Read == 1)
    {
        delay_ms(10);
        if (KEY_UP_Read == 0)
        {
            return WKUP_PRES;
        }
    }
    else if (KEY_KEY0_Read == 0)
    {
        delay_ms(10);
        if (KEY_KEY0_Read == 1)
        {
            return KEY0_PRES;
        }
    }
    else if (KEY_KEY1_Read == 0)
    {
        delay_ms(10);
        if (KEY_KEY1_Read == 1)
        {
            return KEY1_PRES;
        }
    }
    return 10;
}
#else
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;  /* 按键松开标志 */
    uint8_t keyval = 0;

    if (mode) key_up = 1;   /* 支持连按 */

    if (key_up && (KEY_KEY0_Read == 0 || KEY_KEY1_Read == 0 || KEY_UP_Read == 1))   /* 按键松开标志位1，且有任一一个按键按下了 */
    {
        delay_ms(10);       /* 去抖动 */
        key_up = 0;

        if (KEY_KEY0_Read == 0)  keyval = KEY0_PRES;

        if (KEY_KEY1_Read == 0)  keyval = KEY1_PRES;

        if (KEY_UP_Read == 1) keyval = WKUP_PRES;
    }
    else if (KEY_KEY0_Read == 1 && KEY_KEY1_Read == 1 && KEY_UP_Read == 0)  /* 没有任何按键按下，标记按键松开 */
    {
        key_up = 1;
    }

    return keyval;  /* 返回键值 */
}
#endif

void Key_Init_IT(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Init;
    GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Pin = KEY_0_Pin;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

    GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Pin = KEY_1_Pin;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);

    GPIO_Init.Mode = GPIO_MODE_IT_RISING;
    GPIO_Init.Pull = GPIO_PULLDOWN;
    GPIO_Init.Pin = KEY_UP_Pin;
    HAL_GPIO_Init(GPIOA, &GPIO_Init);


    // HAL_NVIC_SetPendingIRQ(EXTI0_IRQn);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    // HAL_NVIC_SetPendingIRQ(EXTI0_IRQn);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    // HAL_NVIC_SetPendingIRQ(EXTI15_10_IRQn);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* 外部中断服务程序 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_UP_Pin);
    __HAL_GPIO_EXTI_CLEAR_FLAG(KEY_UP_Pin);
}
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_0_Pin);
    __HAL_GPIO_EXTI_CLEAR_FLAG(KEY_0_Pin);
}
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_1_Pin);
    __HAL_GPIO_EXTI_CLEAR_FLAG(KEY_1_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /*消抖 为了方便，实际在代码中禁用这个*/
    //delay_ms(10);
    switch (GPIO_Pin)
    {
    case KEY_0_Pin:
        // if (HAL_GPIO_ReadPin(KEY_0_Port, KEY_0_Pin) == 0)
        HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
        break;
    case KEY_1_Pin:
        // if (HAL_GPIO_ReadPin(KEY_1_Port, KEY_1_Pin) == 0)
        HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);

        break;
    case KEY_UP_Pin:
        // if (HAL_GPIO_ReadPin(KEY_UP_Port, KEY_UP_Pin) == 1)
        HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN);
        break;
    default:
        break;
    }

}

