#ifndef __DRV_USART_H
#define __DRV_USART_H

#include "stm32h7xx_hal.h"
#include "stdio.h"
#include "string.h"

#define USART_REC_LEN 200 /* 定义最大接收字节数 200 */
#define USART_EN_RX 1 /* 使能（1） /禁止（0）串口 1 接收 */
#define RXBUFFERSIZE 1 /* 缓存大小 */

extern uint8_t idle_flag;

extern UART_HandleTypeDef g_uart1_handle;
extern uint8_t g_rx_flag;
extern uint16_t g_usart_rx_sta;
extern UART_HandleTypeDef g_uart1_handle;
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
extern uint16_t recv_dac1_data;

/* 串口 1 的 GPIO */
#define USART_TX_GPIO_PORT GPIOA
#define USART_TX_GPIO_PIN GPIO_PIN_9
#define USART_TX_GPIO_AF GPIO_AF7_USART1

/* 发送引脚时钟使能 */
#define USART_TX_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define USART_RX_GPIO_PORT GPIOA
#define USART_RX_GPIO_PIN GPIO_PIN_10
#define USART_RX_GPIO_AF GPIO_AF7_USART1
/* 接收引脚时钟使能 */
#define USART_RX_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define USART_UX USART1
#define USART_UX_IRQn USART1_IRQn
#define USART_UX_IRQHandler USART1_IRQHandler
/* USART1 时钟使能 */
#define USART_UX_CLK_ENABLE() do{ __HAL_RCC_USART1_CLK_ENABLE(); }while(0)

void usart_init(uint32_t baudrate);


#endif // !__DRV_USART_H
