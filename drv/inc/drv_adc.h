#ifndef __DRV_ADC_H
#define __DRV_ADC_H

#include "stm32h7xx_hal.h"
#include "drv_delay.h"

#define NO_USE_DMA 0           /*1表示不用 0表示用*/
extern uint8_t g_adc_dma_flag;


#if NO_USE_DMA
void adc_init(void);
#else
void adc_init_dma(uint32_t par, uint32_t mar);
#endif

uint32_t adc_get_result_average(uint32_t ch, uint8_t times);
void adc_dma_enable(uint16_t ndtr);

#endif // !__DRV_ADC_H

