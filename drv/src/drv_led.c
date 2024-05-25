#include "drv_led.h"

void LED_Config(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    LED0_GPIO_CLK_ENABLE(); /* LED0 时钟使能 */
    LED1_GPIO_CLK_ENABLE(); /* LED1 时钟使能 */
    LED2_GPIO_CLK_ENABLE(); /* LED2 时钟使能 */
    gpio_init_struct.Pin = LED0_GPIO_PIN; /* LED0 引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP; /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP; /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM; /* 中速 */
    HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* 初始化 LED0 引脚 */
    gpio_init_struct.Pin = LED1_GPIO_PIN; /* LED1 引脚 */
    HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct); /* 初始化 LED1 引脚 */

    /*因为使用了TIM15_ch1通道，使用了PE5，所以这里不初始化这个引脚*/
    // gpio_init_struct.Pin = LED2_GPIO_PIN; /* LED2 引脚 */
    // HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_struct); /* 初始化 LED2 引脚 */

    HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET);
}
