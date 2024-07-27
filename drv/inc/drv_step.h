#ifndef __DRV_STEP_H
#define __DRV_STEP_H

#include "stm32h7xx_hal.h"
#include "drv_delay.h"


void Step_Init(void);
void Step_rotate(uint32_t step, uint16_t delay);

#endif // !__DRV_STEP_H