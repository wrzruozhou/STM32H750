#ifndef __DRV_REMOTE_H
#define __DRV_REMOTE_H

#include "stm32h7xx_hal.h"
#include "stdio.h"
#include "lvgl.h"

#define RDATA HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)
#define REMOTE_ID 0

void remote_init(void);
uint8_t remote_scan(void);


#endif // !__DRV_REMOTE_H

