#ifndef __DRV_SDMMC_H
#define __DRV_SDMMC_H

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <malloc.h>

#define SD1_D0_GPIO_PORT            GPIOC
#define SD1_D0_GPIO_PIN             GPIO_PIN_8
#define SD1_D0_GPIO_CLK_ENABLE()    do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0)

#define SD1_D1_GPIO_PORT            GPIOC
#define SD1_D1_GPIO_PIN             GPIO_PIN_9
#define SD1_D1_GPIO_CLK_ENABLE()    do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0)

#define SD1_D2_GPIO_PORT            GPIOC
#define SD1_D2_GPIO_PIN             GPIO_PIN_10
#define SD1_D2_GPIO_CLK_ENABLE()    do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0)

#define SD1_D3_GPIO_PORT            GPIOC
#define SD1_D3_GPIO_PIN             GPIO_PIN_11
#define SD1_D3_GPIO_CLK_ENABLE()    do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0)

#define SD1_CLK_GPIO_PORT            GPIOC
#define SD1_CLK_GPIO_PIN             GPIO_PIN_12
#define SD1_CLK_GPIO_CLK_ENABLE()    do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0)

#define SD1_CMD_GPIO_PORT            GPIOD
#define SD1_CMD_GPIO_PIN             GPIO_PIN_2
#define SD1_CMD_GPIO_CLK_ENABLE()    do{__HAL_RCC_GPIOC_CLK_ENABLE();}while(0)

/****************************************************************************** */
#define SD_TIMEOUT          ((uint32_t)100000000)   /*起始时间*/
#define SD_TRANSFER_OK      ((uint8_t)0x00)         /*传输完成*/
#define SD_TRANSFER_BUSY    ((uint8_t)0x01)         /*卡正忙*/

extern SD_HandleTypeDef g_sd_handle;                    /* SD卡句柄 */
extern HAL_SD_CardInfoTypeDef g_sd_card_info_handle;    /* SD卡信息结构体 */

/*函数声明*/
uint8_t sd_init();
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef* cardinfo);
uint8_t get_sd_card_state(void);
uint8_t sd_read_disk(uint8_t* buf, uint32_t sector, uint32_t cnt);
uint8_t sd_write_disk(uint8_t* buf, uint32_t sector, uint32_t cnt);

void show_sdcard_info(void);
void sd_test_read(uint32_t secaddr, uint32_t seccnt);
void sd_test_write(uint32_t secaddr, uint32_t seccnt);



#endif // !__DRV_SDMMC_H