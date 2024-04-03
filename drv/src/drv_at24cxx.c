#if 0
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

    iic_start();
    iic_send_byte(addr);
    iic_wait_ack();
    temp = iic_read_byte(0);
    iic_stop();
    return temp;
}

void at24cxx_write_one_byte(uint8_t addr, uint8_t data)
{
    iic_start();
    iic_send_byte(0xa0);
    iic_wait_ack();
    iic_send_byte(addr);
    iic_wait_ack();
    iic_send_byte(data);
    iic_wait_ack();
    iic_stop();
    delay_ms(10); /* iic写入较慢，必须延时10个ms */
}

/**
 * @brief   读取多个数据里面的值
 * @param   addr    从哪个地址开始读取
 * @param   pbuf    读取的数据存放的位置
 * @param   datalen 要读取的数据的长度
 */
void at24cxx_read(uint8_t addr, uint8_t *pbuf, uint16_t datalen)
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
void at24cxx_write(uint8_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
#else
#include "drv_at24cxx.h"

/**
 * @brief       ³õÊ¼»¯IIC½Ó¿Ú
 * @param       ÎÞ
 * @retval      ÎÞ
 */
void at24cxx_init(void)
{
    iic_init();
}

/**
 * @brief       ÔÚAT24CXXÖ¸¶¨µØÖ·¶Á³öÒ»¸öÊý¾Ý
 * @param       readaddr: ¿ªÊ¼¶ÁÊýµÄµØÖ·
 * @retval      ¶Áµ½µÄÊý¾Ý
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t temp = 0;
    iic_start(); /* ·¢ËÍÆðÊ¼ÐÅºÅ */

    /* ¸ù¾Ý²»Í¬µÄ24CXXÐÍºÅ, ·¢ËÍ¸ßÎ»µØÖ·
     * 1, 24C16ÒÔÉÏµÄÐÍºÅ, ·Ö2¸ö×Ö½Ú·¢ËÍµØÖ·
     * 2, 24C16¼°ÒÔÏÂµÄÐÍºÅ, ·Ö1¸öµÍ×Ö½ÚµØÖ· + Õ¼ÓÃÆ÷¼þµØÖ·µÄbit1~bit3Î» ÓÃÓÚ±íÊ¾¸ßÎ»µØÖ·, ×î¶à11Î»µØÖ·
     *    ¶ÔÓÚ24C01/02, ÆäÆ÷¼þµØÖ·¸ñÊ½(8bit)Îª: 1  0  1  0  A2  A1  A0  R/W
     *    ¶ÔÓÚ24C04,    ÆäÆ÷¼þµØÖ·¸ñÊ½(8bit)Îª: 1  0  1  0  A2  A1  a8  R/W
     *    ¶ÔÓÚ24C08,    ÆäÆ÷¼þµØÖ·¸ñÊ½(8bit)Îª: 1  0  1  0  A2  a9  a8  R/W
     *    ¶ÔÓÚ24C16,    ÆäÆ÷¼þµØÖ·¸ñÊ½(8bit)Îª: 1  0  1  0  a10 a9  a8  R/W
     *    R/W      : ¶Á/Ð´¿ØÖÆÎ» 0,±íÊ¾Ð´; 1,±íÊ¾¶Á;
     *    A0/A1/A2 : ¶ÔÓ¦Æ÷¼þµÄ1,2,3Òý½Å(Ö»ÓÐ24C01/02/04/8ÓÐÕâÐ©½Å)
     *    a8/a9/a10: ¶ÔÓ¦´æ´¢ÕûÁÐµÄ¸ßÎ»µØÖ·, 11bitµØÖ·×î¶à¿ÉÒÔ±íÊ¾2048¸öÎ»ÖÃ,¿ÉÒÔÑ°Ö·24C16¼°ÒÔÄÚµÄÐÍºÅ
     */
    // if (EE_TYPE > AT24C16) /* 24C16ÒÔÉÏµÄÐÍºÅ, ·Ö2¸ö×Ö½Ú·¢ËÍµØÖ· */
    // {
    //     iic_send_byte(0XA0);      /* ·¢ËÍÐ´ÃüÁî, IIC¹æ¶¨×îµÍÎ»ÊÇ0, ±íÊ¾Ð´Èë */
    //     iic_wait_ack();           /* Ã¿´Î·¢ËÍÍêÒ»¸ö×Ö½Ú,¶¼ÒªµÈ´ýACK */
    //     iic_send_byte(addr >> 8); /* ·¢ËÍ¸ß×Ö½ÚµØÖ· */
    // }
    // else
    // {
    // }
    iic_send_byte(0XA0 + ((addr >> 8) << 1)); /* ·¢ËÍÆ÷¼þ 0XA0 + ¸ßÎ»a8/a9/a10µØÖ·,Ð´Êý¾Ý */
    iic_wait_ack();                           /* Ã¿´Î·¢ËÍÍêÒ»¸ö×Ö½Ú,¶¼ÒªµÈ´ýACK */
    iic_send_byte(addr % 256);                /* ·¢ËÍµÍÎ»µØÖ· */
    iic_wait_ack();                           /* µÈ´ýACK, ´ËÊ±µØÖ··¢ËÍÍê³ÉÁË */

    iic_start();             /* ÖØÐÂ·¢ËÍÆðÊ¼ÐÅºÅ */
    iic_send_byte(0XA1);     /* ½øÈë½ÓÊÕÄ£Ê½, IIC¹æ¶¨×îµÍÎ»ÊÇ0, ±íÊ¾¶ÁÈ¡ */
    iic_wait_ack();          /* Ã¿´Î·¢ËÍÍêÒ»¸ö×Ö½Ú,¶¼ÒªµÈ´ýACK */
    temp = iic_read_byte(0); /* ½ÓÊÕÒ»¸ö×Ö½ÚÊý¾Ý */
    iic_stop();              /* ²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ */
    return temp;
}

/**
 * @brief       ÔÚAT24CXXÖ¸¶¨µØÖ·Ð´ÈëÒ»¸öÊý¾Ý
 * @param       addr: Ð´ÈëÊý¾ÝµÄÄ¿µÄµØÖ·
 * @param       data: ÒªÐ´ÈëµÄÊý¾Ý
 * @retval      ÎÞ
 */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    /* Ô­ÀíËµÃ÷¼û:at24cxx_read_one_byteº¯Êý, ±¾º¯ÊýÍêÈ«ÀàËÆ */
    iic_start(); /* ·¢ËÍÆðÊ¼ÐÅºÅ */

    // if (EE_TYPE > AT24C16) /* 24C16ÒÔÉÏµÄÐÍºÅ, ·Ö2¸ö×Ö½Ú·¢ËÍµØÖ· */
    // {
    //     iic_send_byte(0XA0);      /* ·¢ËÍÐ´ÃüÁî, IIC¹æ¶¨×îµÍÎ»ÊÇ0, ±íÊ¾Ð´Èë */
    //     iic_wait_ack();           /* Ã¿´Î·¢ËÍÍêÒ»¸ö×Ö½Ú,¶¼ÒªµÈ´ýACK */
    //     iic_send_byte(addr >> 8); /* ·¢ËÍ¸ß×Ö½ÚµØÖ· */
    // }
    // else
    // {

    // }+ ((addr >> 8) << 1)
    iic_send_byte(0XA0);       /* ·¢ËÍÆ÷¼þ 0XA0 + ¸ßÎ»a8/a9/a10µØÖ·,Ð´Êý¾Ý */
    iic_wait_ack();            /* Ã¿´Î·¢ËÍÍêÒ»¸ö×Ö½Ú,¶¼ÒªµÈ´ýACK */
    iic_send_byte(addr % 256); /* ·¢ËÍµÍÎ»µØÖ· */
    iic_wait_ack();            /* µÈ´ýACK, ´ËÊ±µØÖ··¢ËÍÍê³ÉÁË */

    /* ÒòÎªÐ´Êý¾ÝµÄÊ±ºò,²»ÐèÒª½øÈë½ÓÊÕÄ£Ê½ÁË,ËùÒÔÕâÀï²»ÓÃÖØÐÂ·¢ËÍÆðÊ¼ÐÅºÅÁË */
    iic_send_byte(data); /* ·¢ËÍ1×Ö½Ú */
    iic_wait_ack();      /* µÈ´ýACK */
    iic_stop();          /* ²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ */
    delay_ms(10);        /* ×¢Òâ: EEPROM Ð´Èë±È½ÏÂý,±ØÐëµÈµ½10msºóÔÙÐ´ÏÂÒ»¸ö×Ö½Ú */
}

/**
 * @brief       ¼ì²éAT24CXXÊÇ·ñÕý³£
 *   @note      ¼ì²âÔ­Àí: ÔÚÆ÷¼þµÄÄ©µØÖ·Ð´Èç0X55, È»ºóÔÙ¶ÁÈ¡, Èç¹û¶ÁÈ¡ÖµÎª0X55
 *              Ôò±íÊ¾¼ì²âÕý³£. ·ñÔò,Ôò±íÊ¾¼ì²âÊ§°Ü.
 *
 * @param       ÎÞ
 * @retval      ¼ì²â½á¹û
 *              0: ¼ì²â³É¹¦
 *              1: ¼ì²âÊ§°Ü
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;
    temp = at24cxx_read_one_byte(addr); /* ±ÜÃâÃ¿´Î¿ª»ú¶¼Ð´AT24CXX */

    if (temp == 0X55) /* ¶ÁÈ¡Êý¾ÝÕý³£ */
    {
        return 0;
    }
    else /* ÅÅ³ýµÚÒ»´Î³õÊ¼»¯µÄÇé¿ö */
    {
        at24cxx_write_one_byte(addr, 0X55); /* ÏÈÐ´ÈëÊý¾Ý */
        temp = at24cxx_read_one_byte(255);  /* ÔÙ¶ÁÈ¡Êý¾Ý */

        if (temp == 0X55)
            return 0;
    }

    return 1;
}

/**
 * @brief       ÔÚAT24CXXÀïÃæµÄÖ¸¶¨µØÖ·¿ªÊ¼¶Á³öÖ¸¶¨¸öÊýµÄÊý¾Ý
 * @param       addr    : ¿ªÊ¼¶Á³öµÄµØÖ· ¶Ô24c02Îª0~255
 * @param       pbuf    : Êý¾ÝÊý×éÊ×µØÖ·
 * @param       datalen : Òª¶Á³öÊý¾ÝµÄ¸öÊý
 * @retval      ÎÞ
 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief       ÔÚAT24CXXÀïÃæµÄÖ¸¶¨µØÖ·¿ªÊ¼Ð´ÈëÖ¸¶¨¸öÊýµÄÊý¾Ý
 * @param       addr    : ¿ªÊ¼Ð´ÈëµÄµØÖ· ¶Ô24c02Îª0~255
 * @param       pbuf    : Êý¾ÝÊý×éÊ×µØÖ·
 * @param       datalen : ÒªÐ´ÈëÊý¾ÝµÄ¸öÊý
 * @retval      ÎÞ
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}

#endif