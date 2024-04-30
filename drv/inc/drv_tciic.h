#ifndef __DRV_TCIIC_H
#define __DRV_TCIIC_H

#include "stm32h7xx_hal.h"

#define CT_IIC_SCL_GPIO_PORT    GPIOB
#define CT_IIC_SCL_GPIO_PIN     GPIO_PIN_0
#define CT_IIC_SCL_CLK_ENABLE   __HAL_RCC_GPIOB_CLK_ENABLE();

#define CT_IIC_SDA_GPIO_PORT    GPIOB
#define CT_IIC_SDA_GPIO_PIN     GPIO_PIN_3
#define CT_IIC_SDA_CLK_ENABLE   __HAL_RCC_GPIOB_CLK_ENABLE();


#define CT_IIC_SCL(x) {(x)?HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT,CT_IIC_SCL_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT,CT_IIC_SCL_GPIO_PIN,GPIO_PIN_RESET);}
#define CT_IIC_SDA(x) {(x)?HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT,CT_IIC_SDA_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT,CT_IIC_SDA_GPIO_PIN,GPIO_PIN_RESET);}

#define CT_READ_SDA         HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_PORT,CT_IIC_SDA_GPIO_PIN)

void ct_iic_init(void);
void ct_iic_start(void);
void ct_iic_stop(void);
uint8_t ct_iic_wait_ack(void);
void ct_iic_ack(void);
void ct_iic_noack(void);
void ct_iic_send_byte(uint8_t data);
uint8_t ct_iic_read_byte(unsigned char ack);
#endif // !__DRV_TCIIC_H

														