#include "drv_at24cxx.h"

void at24cxx_init(void)
{
    iic_init();
}

uint8_t at24cxx_read_one_byte(uint8_t addr)
{
    uint8_t temp = 0;
    iic_start();
    iic_send_byte(0xa0);
    iic_wait_ack();
    iic_send_byte(addr);
    iic_wait_ack();

    iic_start();
    iic_send_byte(0xa1);
    iic_wait_ack();
    temp = iic_read_byte(0);
    iic_stop();
    return temp;
}

void at24cxx_write_one_byte(uint8_t addr, uint8_t data)
{
    uint8_t flag = 0;
    iic_start();
    iic_send_byte(0xa0);
    flag = iic_wait_ack();
    iic_send_byte(addr);
    flag = iic_wait_ack();
    iic_send_byte(data);
    flag = iic_wait_ack();
    iic_stop();
    delay_ms(10); /* iic写入较慢，必须延时10个ms */
}

/**
 * @brief   读取多个数据里面的值
 * @param   addr    从哪个地址开始读取
 * @param   pbuf    读取的数据存放的位置
 * @param   datalen 要读取的数据的长度
 */
void at24cxx_read(uint8_t addr, uint8_t* pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief   向寄存器里面写入多个数据
 * @param   addr    从哪个地址开始写入
 * @param   pbuf    需要写入的数据
 * @param   datalen 需要写入数据的长度
 */
void at24cxx_write(uint8_t addr, uint8_t* pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
