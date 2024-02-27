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

#define KEY_UP_Read HAL_GPIO_ReadPin(KEY_UP_Port,KEY_UP_Pin)
#define KEY_KEY0_Read HAL_GPIO_ReadPin(KEY_0_Port,KEY_0_Pin)
#define KEY_KEY1_Read HAL_GPIO_ReadPin(KEY_1_Port,KEY_1_Pin)

enum key_status {
    WKUP_PRES = 0,
    KEY0_PRES,
    KEY1_PRES
};

void Key_Init(void);
void Key_Init_IT(void);
uint8_t key_scan(void);


#endif // !__DRV_KEY_H
