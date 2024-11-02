#ifndef __DRV_DAC_H
#define __DRV_DAC_H

#include "stm32h7xx_hal.h"

void dac_init(void);
void dac_channel_set(uint32_t channel, uint16_t value);

#endif // !__DRV_DAC_H
