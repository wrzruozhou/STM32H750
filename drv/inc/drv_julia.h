#ifndef __DRV_JULIA_H
#define __DRV_JULIA_H

#include "stm32h7xx_hal.h"
#include "drv_lcd.h"

/*FPU模式提示*/
#if __FPU_USED == 1
#define SCORE_FPU_MODE  "FPU On"
#else
#define SCORE_FPU_MODE  "FPU Off"
#endif

#define ITERATION       128     /*迭代次数*/
#define REAL_CONSTANT   0.285f  /*实部常量*/
#define IMG_CONSTANT    0.01f   /*虚部常量*/

void julia_clut_init(uint16_t* clut);
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);

#endif // !__DRV_JULIA_H
