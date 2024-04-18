#ifndef __DRV_AT24CXX_H
#define __DRV_AT24CXX_H

#include "stm32h7xx_hal.h"
#include "drv_iic.h"
#include "drv_delay.h"
#include "stdio.h"

void at24cxx_init(void);
uint8_t at24cxx_read_one_byte(uint8_t addr);
void at24cxx_write_one_byte(uint8_t addr, uint8_t data);
void at24cxx_read(uint8_t addr, uint8_t* pbuf, uint16_t datalen);
void at24cxx_write(uint8_t addr, uint8_t* pbuf, uint16_t datalen);
#endif
