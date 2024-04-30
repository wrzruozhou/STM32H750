#include "drv_iic.h"

void iic_init(void)
{
    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Init;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pin = IIC_SCL_GPIO_PIN;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &GPIO_Init);

    GPIO_Init.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_Init.Pin = IIC_SDA_GPIO_PIN;
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &GPIO_Init);

    /* 需要加一个功能停止所有总线上的设备 */
    iic_stop();
}

/**
 * @brief   IIC延时函数
 * @param   无
 * @retval  无
 */
static void iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief   起始信号
 * @param   无
 * @param   无
 */
void iic_start(void)
{
    IIC_SCL(1);
    IIC_SDA(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief   停止信号
 * @param   无
 * @retval  无
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief   响应信号
 * @param   无
 * @retval 1 接收应答失败
 *          0 接收应答成功
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0; /*记录时间*/
    uint8_t rack = 0;     /*返回值*/

    IIC_SDA(1); /*主机释放SDA线*/
    iic_delay();
    IIC_SCL(1); /*从机可以返回ACK*/
    iic_delay();

    while (IIC_READ_SDA) /*等待应答*/
    {
        waittime++;
        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }
    IIC_SCL(0); /*拉低SCL*/
    iic_delay();
    return rack;
}

/**
 * @brief   产生ack应答
 * @param   无
 * @param   无
 */
void iic_ack(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1); /*在scl高电平期间，SDA为低电平代表响应*/
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1); /*释放时钟线*/
    iic_delay();
}

/**
 * @brief   产生ack应答
 * @param   无
 * @param   无
 */
void iic_nack(void)
{
    IIC_SDA(1);
    iic_delay();
    IIC_SCL(1); /*在scl高电平期间，SDA为低电平代表响应*/
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    /*释放时钟线*/
}

/**
 * @brief   iic发送一个字节
 * param    data 要发送的数据
 * retval   无
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;
    for (t = 0; t < 8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1;
    }

    IIC_SDA(1); /*拉高电平*/
    // IIC_SCL(0); /*拉低时钟线*/
}

/**
 * @brief   iic读取一个字节
 * @param   ack ack=1时，发送ACK;ack=0时，发送nack
 * @retval  接收到的数据
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;
    for (i = 0; i < 8; i++)
    {
        receive <<= 1;
        IIC_SCL(1);
        iic_delay();
        if (IIC_READ_SDA)
        {
            receive++;
        }
        IIC_SCL(0);
        iic_delay();
    }

    if (!ack)
    {
        iic_nack(); /*发送nack*/
    }
    else
    {
        iic_ack();
    }
    return receive;
}
