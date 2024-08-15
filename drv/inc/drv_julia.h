#ifndef __DRV_JULIA_H
#define __DRV_JULIA_H

#include "stm32h7xx_hal.h"
#include "drv_lcd.h"

void julia_clut_init(uint16_t* clut);
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);

#endif // !__DRV_JULIA_H