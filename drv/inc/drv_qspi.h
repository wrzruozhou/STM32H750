#ifndef __DRV_QSPI_H
#define __DRV_QSPI_H

#include "stm32h7xx_hal.h"

#define QSPI_BK1_NCS_PORT   GPIOB
#define QSPI_BK1_NCS_PIN    GPIO_PIN_6
#define QSPI_BK1_NCS_CLK    do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)
#define QSPI_BK1_NCS_AF     GPIO_AF10_QUADSPI

#define QSPI_BK1_IO0_PORT   GPIOD
#define QSPI_BK1_IO0_PIN    GPIO_PIN_11
#define QSPI_BK1_IO0_CLK    do{__HAL_RCC_GPIOD_CLK_ENABLE();}while(0)
#define QSPI_BK1_IO0_AF     GPIO_AF9_QUADSPI

#define QSPI_BK1_IO1_PORT   GPIOD
#define QSPI_BK1_IO1_PIN    GPIO_PIN_12
#define QSPI_BK1_IO1_CLK    do{__HAL_RCC_GPIOD_CLK_ENABLE();}while(0)
#define QSPI_BK1_IO1_AF     GPIO_AF9_QUADSPI

#define QSPI_BK1_IO2_PORT   GPIOE
#define QSPI_BK1_IO2_PIN    GPIO_PIN_2
#define QSPI_BK1_IO2_CLK    do{__HAL_RCC_GPIOE_CLK_ENABLE();}while(0)
#define QSPI_BK1_IO2_AF     GPIO_AF9_QUADSPI

#define QSPI_BK1_IO3_PORT   GPIOD
#define QSPI_BK1_IO3_PIN    GPIO_PIN_13
#define QSPI_BK1_IO3_CLK    do{__HAL_RCC_GPIOD_CLK_ENABLE();}while(0)
#define QSPI_BK1_IO3_AF     GPIO_AF9_QUADSPI

#define QSPI_BK1_CLK_PORT   GPIOB
#define QSPI_BK1_CLK_PIN    GPIO_PIN_2
#define QSPI_BK1_CLK_CLK    do{__HAL_RCC_GPIOB_CLK_ENABLE();}while(0)
#define QSPI_BK1_CLK_AF     GPIO_AF9_QUADSPI


#define QSPI_BK1_CLK_GPIO_PORT          GPIOB
#define QSPI_BK1_CLK_GPIO_PIN           GPIO_PIN_2
#define QSPI_BK1_CLK_GPIO_AF            GPIO_AF9_QUADSPI
#define QSPI_BK1_CLK_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOB_CLK_ENABLE; }while(0)   /* PB��ʱ��ʹ�� */

#define QSPI_BK1_NCS_GPIO_PORT          GPIOB
#define QSPI_BK1_NCS_GPIO_PIN           GPIO_PIN_6
#define QSPI_BK1_NCS_GPIO_AF            GPIO_AF10_QUADSPI
#define QSPI_BK1_NCS_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOB_CLK_ENABLE; }while(0)   /* PB��ʱ��ʹ�� */

#define QSPI_BK1_IO0_GPIO_PORT          GPIOD
#define QSPI_BK1_IO0_GPIO_PIN           GPIO_PIN_11
#define QSPI_BK1_IO0_GPIO_AF            GPIO_AF9_QUADSPI
#define QSPI_BK1_IO0_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOD_CLK_ENABLE; }while(0)   /* PD��ʱ��ʹ�� */

#define QSPI_BK1_IO1_GPIO_PORT          GPIOD
#define QSPI_BK1_IO1_GPIO_PIN           GPIO_PIN_12
#define QSPI_BK1_IO1_GPIO_AF            GPIO_AF9_QUADSPI
#define QSPI_BK1_IO1_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOD_CLK_ENABLE; }while(0)   /* PD��ʱ��ʹ�� */
#if 0
#define QSPI_BK1_IO2_GPIO_PORT          GPIOD
#define QSPI_BK1_IO2_GPIO_PIN           GPIO_PIN_13
#define QSPI_BK1_IO2_GPIO_AF            GPIO_AF9_QUADSPI
#define QSPI_BK1_IO2_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOD_CLK_ENABLE; }while(0)   /* PD��ʱ��ʹ�� */

#define QSPI_BK1_IO3_GPIO_PORT          GPIOE
#define QSPI_BK1_IO3_GPIO_PIN           GPIO_PIN_2
#define QSPI_BK1_IO3_GPIO_AF            GPIO_AF9_QUADSPI
#define QSPI_BK1_IO3_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOE_CLK_ENABLE; }while(0)   /* PE��ʱ��ʹ�� */
#else
#define QSPI_BK1_IO2_GPIO_PORT   GPIOE
#define QSPI_BK1_IO2_GPIO_PIN    GPIO_PIN_2
#define QSPI_BK1_IO2_GPIO_CLK    do{__HAL_RCC_GPIOE_CLK_ENABLE();}while(0)
#define QSPI_BK1_IO2_GPIO_AF     GPIO_AF9_QUADSPI

#define QSPI_BK1_IO3_GPIO_PORT   GPIOD
#define QSPI_BK1_IO3_GPIO_PIN    GPIO_PIN_13
#define QSPI_BK1_IO3_GPIO_CLK    do{__HAL_RCC_GPIOD_CLK_ENABLE();}while(0)
#define QSPI_BK1_IO3_GPIO_AF     GPIO_AF9_QUADSPI
#endif


uint8_t qspi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime);
uint8_t qspi_init(void);
void qspi_send_cmd(uint8_t cmd, uint32_t addr, uint8_t mode, uint8_t dmcycle);
uint8_t qspi_receive(uint8_t* buf, uint32_t datalen);
uint8_t qspi_transmit(uint8_t* buf, uint32_t datalen);

#endif // !__DRV_QSPI_H

