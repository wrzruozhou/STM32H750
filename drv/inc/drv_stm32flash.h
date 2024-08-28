#ifndef __DRV_STM32FLASH_H
#define __DRV_STM32FLASH_H

#include "stm32h7xx_hal.h"

/*FLASH的起始地址*/
#define STM32_FLASH_BASE    0x08000000  /*STM32 FLASH的起始地址*/
#define STM32_FLASH_SIZE    0x20000     /*STM32 FLASH的总大小*/
#define BOOT_FLASH_SIZE     0x4000      /*前16K FLASH用来保存Bootloader*/
#define FLASH_WAITETIME     50000       /*FLASH超时等待时间*/

/*FLASH扇区的起始地址，H750XX只有BANK1的扇区0有效，共128KB*/
#define BANK1_FLASH_SECTOR_0    ((uint32_t)0x08000000)  /*Bank1扇区0起始地址，128Kbytes*/

uint32_t stmflash_read_word(uint32_t faddr);
void stmflash_read(uint32_t raddr, uint32_t* pbuf, uint32_t length);
void stmflash_write(uint32_t waddr, uint32_t* pbuf, uint32_t length);

#endif // !__DRV_STM32FLASH_H