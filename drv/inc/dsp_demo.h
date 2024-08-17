#ifndef __DSP_DEMO_H
#define __DSP_DEMO_H

#include "stm32h7xx_hal.h"
#include "arm_math.h"

#define DELTA 0.0001f   /*误差值*/

uint8_t sin_cos_test(float angle, uint32_t times, uint8_t mode);

#endif // !__DSP_DEMO_H
