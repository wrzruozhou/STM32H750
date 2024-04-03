#ifndef __DRV_AT24CXX_H
#define __DRV_AT24CXX_H

#include "stm32h7xx_hal.h"
#include "drv_iic.h"
#include "drv_delay.h"
#if 0
void at24cxx_init(void);
uint8_t at24cxx_read_one_byte(uint8_t addr);
void at24cxx_write_one_byte(uint8_t addr, uint8_t data);
void at24cxx_read(uint8_t addr, uint8_t* pbuf, uint16_t datalen);
#else
/**
 ****************************************************************************************************
 * @file        24cxx.h
 * @author      ÕýµãÔ­×ÓÍÅ¶Ó(ALIENTEK)
 * @version     V1.0
 * @date        2020-03-29
 * @brief       24CXX Çý¶¯´úÂë
 * @license     Copyright (c) 2020-2032, ¹ãÖÝÊÐÐÇÒíµç×Ó¿Æ¼¼ÓÐÏÞ¹«Ë¾
 ****************************************************************************************************
 * @attention
 *
 * ÊµÑéÆ½Ì¨:ÕýµãÔ­×Ó STM32H750¿ª·¢°å
 * ÔÚÏßÊÓÆµ:www.yuanzige.com
 * ¼¼ÊõÂÛÌ³:www.openedv.com
 * ¹«Ë¾ÍøÖ·:www.alientek.com
 * ¹ºÂòµØÖ·:openedv.taobao.com
 *
 * ÐÞ¸ÄËµÃ÷
 * V1.0 20200329
 * µÚÒ»´Î·¢²¼
 *
 ****************************************************************************************************
 */
#ifndef __24CXX_H
#define __24CXX_H

#define AT24C01 127
#define AT24C02 255
#define AT24C04 511
#define AT24C08 1023
#define AT24C16 2047
#define AT24C32 4095
#define AT24C64 8191
#define AT24C128 16383
#define AT24C256 32767

/* ¿ª·¢°åÊ¹ÓÃµÄÊÇ24c02£¬ËùÒÔ¶¨ÒåEE_TYPEÎªAT24C02 */

#define EE_TYPE AT24C02

void at24cxx_init(void);                                            /* ³õÊ¼»¯IIC */
uint8_t at24cxx_check(void);                                        /* ¼ì²éÆ÷¼þ */
uint8_t at24cxx_read_one_byte(uint16_t addr);                       /* Ö¸¶¨µØÖ·¶ÁÈ¡Ò»¸ö×Ö½Ú */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data);           /* Ö¸¶¨µØÖ·Ð´ÈëÒ»¸ö×Ö½Ú */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen); /* ´ÓÖ¸¶¨µØÖ·¿ªÊ¼Ð´ÈëÖ¸¶¨³¤¶ÈµÄÊý¾Ý */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);  /* ´ÓÖ¸¶¨µØÖ·¿ªÊ¼¶Á³öÖ¸¶¨³¤¶ÈµÄÊý¾Ý */

#endif

#endif

#endif // !__DRV_AT24CXX_H
