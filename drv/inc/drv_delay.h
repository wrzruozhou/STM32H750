#ifndef __DRV_DELAY_H
#define __DRV_DELAY_H

#include "stm32h7xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

#define USE_FREERTOS 0

void delay_us(uint16_t us);
void delay_ms(uint16_t ms);


#endif // !__DRV_DELAY_H
