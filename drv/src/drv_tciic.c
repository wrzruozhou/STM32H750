/**
 * @file    tciic.c
 * @author  RuoZHou Wan
 * @version V1.0
 * @date    2024-4-27
 * @brief   GT911触摸屏代码
 * @license GPL
*/
#include "drv_tciic.h"
#include "drv_delay.h"

/**
 * @brief   控制I2C速度的延时
 * @param   无
 * @retval   无
*/
static void ct_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   触摸屏iic接口初始化
 * @param   无
 * @retval  无
*/
void ct_iic_init(void)
{
    GPIO_InitTypeDef GPIO_Init;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Init.Pin = CT_IIC_SCL_GPIO_PIN;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(CT_IIC_SCL_GPIO_PORT, &GPIO_Init);

    GPIO_Init.Pin = CT_IIC_SDA_GPIO_PIN;
    HAL_GPIO_Init(CT_IIC_SDA_GPIO_PORT, &GPIO_Init);

    /*停止总线上的所有设备*/
}

/**
 * @brief   起始信号
 * @param   无
 * @retval  无
*/
void ct_iic_start(void)
{
    CT_IIC_SCL(1);
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(0);  /*释放SCL总线*/
}

/**
 * @brief   停止信号
 * @param   无
 * @param   无
*/
void ct_iic_stop(void)
{
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SCL(0);
}

/**
 * @brief   接收应答信号
 * @param   无
 * @retval  0:接收应答成功
            1:接收应答失败
*/
uint8_t ct_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    CT_IIC_SDA(1);          /*拉高释放时钟线*/
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();         /*拉高SCL，从机可以返回ACK*/

    while (CT_READ_SDA)     /*等待应答*/
    {
        waittime++;
        if (waittime > 250)
        {
            ct_iic_stop();
            rack = 1;
            return rack;
        }
    }
    CT_IIC_SCL(0);          /*SCL = 0,结束ACK的检查*/
    ct_iic_delay();
    return rack;
}

/**
 * @brief   产生应答信号
 * @param   无
 * @retval  无
*/
void ct_iic_ack(void)
{
    CT_IIC_SDA(0);          /*scl 0->1 时 SDA = 0，表示应答*/
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SCL(0);          /*拉低SCL，防止其他IIC设备响应*/
    ct_iic_delay();
    CT_IIC_SDA(1);          /*释放SDA时钟线*/
}

/**
 * @brief   不产生应答信号
 * @param   无
 * @param   无
*/
void ct_iic_noack(void)
{
    CT_IIC_SDA(1);          /*scl 0->1 时 SDA = 1，表示非应答*/
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SCL(0);          /*拉低SCL，防止其他IIC设备响应*/
    ct_iic_delay();
}

/**
 * @brief   IIC发送一个字节
 * @param   data:需要发送的数据
 * @retval  无
*/
void ct_iic_send_byte(uint8_t data)
{
    uint8_t t;
    for (t = 0; t < 8; t++)
    {
        CT_IIC_SDA((data & 0x80) >> 7);
        ct_iic_delay();
        CT_IIC_SCL(1);
        ct_iic_delay();
        CT_IIC_SCL(0);
        data <<= 1;
    }
    CT_IIC_SDA(1);          /*传输完成 释放SDA数据线*/
}

/**
 * @brief   IIC接收一个字节
 * @param   ack: ack=1时，发送ack； ack=0的时候，发送nack
 * @retval  接收到的数据
*/
uint8_t ct_iic_read_byte(unsigned char ack)
{
    uint8_t i, receive = 0;
    for (i = 0; i < 8; i++)
    {
        receive <<= 1;          /*先收到的数据要左移一位*/
        CT_IIC_SCL(1);
        ct_iic_delay();
        if (CT_READ_SDA)
        {
            receive++;
        }
        CT_IIC_SCL(0);
        ct_iic_delay();
    }
    if (!ack)
    {
        ct_iic_noack();
    }
    else
    {
        ct_iic_ack();
    }
    return receive;
}
