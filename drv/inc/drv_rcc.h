#ifndef __DRV_RCC_H
#define __DRV_RCC_H

#include "stm32h7xx_hal.h"

void sys_cache_enable(void);
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);

#endif //!__DRV_RCC_H
