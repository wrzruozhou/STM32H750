#ifndef __DRV_ADC_H
#define __DRV_ADC_H

#include "stm32h7xx_hal.h"
#include "drv_delay.h"

void adc_init(void);
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);

#endif // !__DRV_ADC_H

