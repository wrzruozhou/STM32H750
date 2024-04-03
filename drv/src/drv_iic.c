#include "drv_iic.h"
#if 1
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
#if 1
    IIC_SCL(1);
    IIC_SDA(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
#else
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0); /* STARTÐÅºÅ: µ±SCLÎª¸ßÊ±, SDA´Ó¸ß±ä³ÉµÍ, ±íÊ¾ÆðÊ¼ÐÅºÅ */
    iic_delay();
    IIC_SCL(0); /* Ç¯×¡I2C×ÜÏß£¬×¼±¸·¢ËÍ»ò½ÓÊÕÊý¾Ý */
    iic_delay();
#endif
}

/**
 * @brief   停止信号
 * @param   无
 * @retval  无
 */
void iic_stop(void)
{
#if 0
    IIC_SCL(1);
    IIC_SDA(0);
    iic_delay();

    IIC_SDA(1);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
#else
    IIC_SDA(0); /* STOPÐÅºÅ: µ±SCLÎª¸ßÊ±, SDA´ÓµÍ±ä³É¸ß, ±íÊ¾Í£Ö¹ÐÅºÅ */
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1); /* ·¢ËÍI2C×ÜÏß½áÊøÐÅºÅ */
    iic_delay();
#endif
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
#else
#include "drv_iic.h"

/**
 * @brief       ³õÊ¼»¯IIC
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    IIC_SCL_GPIO_CLK_ENABLE(); /* SCLÒý½ÅÊ±ÖÓÊ¹ÄÜ */
    IIC_SDA_GPIO_CLK_ENABLE(); /* SDAÒý½ÅÊ±ÖÓÊ¹ÄÜ */

    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;         /* ÍÆÍìÊä³ö */
    gpio_init_struct.Pull = GPIO_PULLUP;                 /* ÉÏÀ­ */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* ¿ìËÙ */
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct); /* SCL */

    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;         /* ÍÆÍìÊä³ö */
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct); /* SDA */
    /* SDAÒý½ÅÄ£Ê½ÉèÖÃ,¿ªÂ©Êä³ö,ÉÏÀ­, ÕâÑù¾Í²»ÓÃÔÙÉèÖÃIO·½ÏòÁË, ¿ªÂ©Êä³öµÄÊ±ºò(=1), Ò²¿ÉÒÔ¶ÁÈ¡Íâ²¿ÐÅºÅµÄ¸ßµÍµçÆ½ */

    iic_stop(); /* Í£Ö¹×ÜÏßÉÏËùÓÐÉè±¸ */
}

/**
 * @brief       IICÑÓÊ±º¯Êý,ÓÃÓÚ¿ØÖÆIIC¶ÁÐ´ËÙ¶È
 * @param       ÎÞ
 * @retval      ÎÞ
 */
static void iic_delay(void)
{
    delay_us(2); /* 2usµÄÑÓÊ±, ¶ÁÐ´ËÙ¶ÈÔÚ250KhzÒÔÄÚ */
}

/**
 * @brief       ²úÉúIICÆðÊ¼ÐÅºÅ
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0); /* STARTÐÅºÅ: µ±SCLÎª¸ßÊ±, SDA´Ó¸ß±ä³ÉµÍ, ±íÊ¾ÆðÊ¼ÐÅºÅ */
    iic_delay();
    IIC_SCL(0); /* Ç¯×¡I2C×ÜÏß£¬×¼±¸·¢ËÍ»ò½ÓÊÕÊý¾Ý */
    iic_delay();
}

/**
 * @brief       ²úÉúIICÍ£Ö¹ÐÅºÅ
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void iic_stop(void)
{
    IIC_SDA(0); /* STOPÐÅºÅ: µ±SCLÎª¸ßÊ±, SDA´ÓµÍ±ä³É¸ß, ±íÊ¾Í£Ö¹ÐÅºÅ */
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1); /* ·¢ËÍI2C×ÜÏß½áÊøÐÅºÅ */
    iic_delay();
}

/**
 * @brief       µÈ´ýÓ¦´ðÐÅºÅµ½À´
 * @param       ÎÞ
 * @retval      1£¬½ÓÊÕÓ¦´ðÊ§°Ü
 *              0£¬½ÓÊÕÓ¦´ð³É¹¦
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    IIC_SDA(1); /* Ö÷»úÊÍ·ÅSDAÏß(´ËÊ±Íâ²¿Æ÷¼þ¿ÉÒÔÀ­µÍSDAÏß) */
    iic_delay();
    IIC_SCL(1); /* SCL=1, ´ËÊ±´Ó»ú¿ÉÒÔ·µ»ØACK */
    iic_delay();

    while (IIC_READ_SDA) /* µÈ´ýÓ¦´ð */
    {
        waittime++;

        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }

    IIC_SCL(0); /* SCL=0, ½áÊøACK¼ì²é */
    iic_delay();
    return rack;
}

/**
 * @brief       ²úÉúACKÓ¦´ð
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void iic_ack(void)
{
    IIC_SDA(0); /* SCL 0 -> 1  Ê± SDA = 0,±íÊ¾Ó¦´ð */
    iic_delay();
    IIC_SCL(1); /* ²úÉúÒ»¸öÊ±ÖÓ */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1); /* Ö÷»úÊÍ·ÅSDAÏß */
    iic_delay();
}

/**
 * @brief       ²»²úÉúACKÓ¦´ð
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void iic_nack(void)
{
    IIC_SDA(1); /* SCL 0 -> 1  Ê± SDA = 1,±íÊ¾²»Ó¦´ð */
    iic_delay();
    IIC_SCL(1); /* ²úÉúÒ»¸öÊ±ÖÓ */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief       IIC·¢ËÍÒ»¸ö×Ö½Ú
 * @param       data: Òª·¢ËÍµÄÊý¾Ý
 * @retval      ÎÞ
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;

    for (t = 0; t < 8; t++)
    {
        IIC_SDA((data & 0x80) >> 7); /* ¸ßÎ»ÏÈ·¢ËÍ */
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1; /* ×óÒÆ1Î»,ÓÃÓÚÏÂÒ»´Î·¢ËÍ */
    }
    IIC_SDA(1); /* ·¢ËÍÍê³É, Ö÷»úÊÍ·ÅSDAÏß */
}

/**
 * @brief       IIC¶ÁÈ¡Ò»¸ö×Ö½Ú
 * @param       ack:  ack=1Ê±£¬·¢ËÍack; ack=0Ê±£¬·¢ËÍnack
 * @retval      ½ÓÊÕµ½µÄÊý¾Ý
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++) /* ½ÓÊÕ1¸ö×Ö½ÚÊý¾Ý */
    {
        receive <<= 1; /* ¸ßÎ»ÏÈÊä³ö,ËùÒÔÏÈÊÕµ½µÄÊý¾ÝÎ»Òª×óÒÆ */
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
        iic_nack(); /* ·¢ËÍnACK */
    }
    else
    {
        iic_ack(); /* ·¢ËÍACK */
    }

    return receive;
}

#endif