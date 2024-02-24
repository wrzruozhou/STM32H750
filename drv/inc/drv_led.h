#ifndef __DRV_LED_H
#define __DRV_LED_H

#include "stm32h7xx_hal.h"

/*****************************************************************************/
/* 引脚 定义 */
#define LED0_GPIO_PORT GPIOB
#define LED0_GPIO_PIN GPIO_PIN_4
/* PB 口时钟使能 */
#define LED0_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define LED1_GPIO_PORT GPIOE
#define LED1_GPIO_PIN GPIO_PIN_6
/* PE 口时钟使能 */
#define LED1_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)
#define LED2_GPIO_PORT GPIOE
#define LED2_GPIO_PIN GPIO_PIN_5
/* PE 口时钟使能 */
#define LED2_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)
/*****************************************************************************/

void LED_Config(void);

#endif // !__DRV_LED_H
