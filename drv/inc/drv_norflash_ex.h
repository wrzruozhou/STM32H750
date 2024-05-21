#ifndef __DRV_NORFLASH_EX_H
#define __DRV_NORFLASH_EX_H

#include "stm32h7xx_hal.h"
#include "drv_qspi.h"
#include "drv_noflash.h"

uint8_t norflash_ex_write(uint8_t* pbuf, uint32_t addr, uint16_t datalen);
void norflash_ex_read(uint8_t* pbuf, uint32_t addr, uint16_t datalen);
uint16_t norflash_ex_read_id(void);
void norflash_ex_erase_sector(uint32_t saddr);
void norflash_ex_erase_chip(void);

#endif // !__DRV_NORFLASH_EX_H
