#ifndef __DRV_BEEP_H
#define __DRV_BEEP_H

#include "stm32h7xx_hal.h"

#define BEEP_PORT   GPIOE
#define BEEP_PIN    GPIO_PIN_5
#define BEEP_CLK_ENABLE() do{__HAL_RCC_GPIOE_CLK_ENABLE();}while(0)

#define BEEP_NPORT   GPIOE
#define BEEP_NPIN    GPIO_PIN_4
#define BEEP_NCLK_ENABLE() do{__HAL_RCC_GPIOE_CLK_ENABLE();}while(0)

#define BEEP_BIN_PORT   GPIOE
#define BEEP_BIN_PIN    GPIO_PIN_3
#define BEEP_BIN_CLK_ENABLE() do{__HAL_RCC_GPIOE_CLK_ENABLE();}while(0)

void BEEP_Init(void);
void BEEP_PWM_Init(uint16_t psc, uint16_t arr);
void BEEP_SETPWM(uint16_t ccr, uint8_t dtg);
void BEEP_SETPSC(uint16_t psc);

#endif // !__DRV_BEEP_H

