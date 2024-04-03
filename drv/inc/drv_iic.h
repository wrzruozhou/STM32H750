#ifndef __DRV_IIC_H
#define __DRV_IIC_H

#include "stm32h7xx_hal.h"
#include "drv_delay.h"
/**
 * SCL PB10
 * SDA PB11
*/
#define IIC_SCL_GPIO_PORT GPIOB
#define IIC_SCL_GPIO_PIN    GPIO_PIN_10
#define IIC_SCL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define IIC_SDA_GPIO_PORT GPIOB
#define IIC_SDA_GPIO_PIN    GPIO_PIN_11
#define IIC_SDA_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

/*IO操作*/
#define IIC_SCL(x) {x? \
                        HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT,IIC_SCL_GPIO_PIN,GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(IIC_SCL_GPIO_PORT,IIC_SCL_GPIO_PIN,GPIO_PIN_RESET);}

#define IIC_SDA(x) {x? \
                        HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT,IIC_SDA_GPIO_PIN,GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(IIC_SDA_GPIO_PORT,IIC_SDA_GPIO_PIN,GPIO_PIN_RESET);}
/*
    读取SDA
*/
#define IIC_READ_SDA    HAL_GPIO_ReadPin(IIC_SDA_GPIO_PORT,IIC_SDA_GPIO_PIN)

void iic_init(void);
void iic_start(void);
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);

void iic_send_byte(uint8_t data);
uint8_t iic_read_byte(uint8_t ack);


#endif // !__DRV_IIC_H
