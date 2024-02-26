#ifndef __DRV_KEY_H
#define __DRV_KEY_H

#include "stm32h7xx_hal.h"
#include "drv_led.h"
#include "drv_delay.h"

/* Key0和Key1都是低电平有效,Key_UP是高电平有效的
    KEY0 ----->PA0
    KEY1 ----->PA1
    KEY_UP---->PA15
*/
#define KEY_0_Port GPIOA
#define KEY_1_Port GPIOA
#define KEY_UP_Port GPIOA

#define KEY_UP_Pin GPIO_PIN_0
#define KEY_0_Pin GPIO_PIN_1
#define KEY_1_Pin GPIO_PIN_15

void Key_Init(void);
void Key_Init_IT(void);


#endif // !__DRV_KEY_H
