#include "drv_norflash_ex.h"

extern uint8_t g_norflash_addrw;        /*表示当前是24bit/32bit数据位宽，在drv_norflash.c里面定义*/


/**
 * @brief   QSPI接口进入内存映射模式
 *  @note   屌用该函数之前务必已经初始化了QSPI接口
 * @param   无
 * @retval  无
*/
static void norflash_ex_enter_mmap(void)
{
    uint32_t tempreg = 0;

    /* BY/W25QXX Ð´Ê¹ÄÜ£¨0X06Ö¸Áî£© */
    while (QUADSPI->SR & (1 << 5)); /* µÈ´ýBUSYÎ»ÇåÁã */

    QUADSPI->CCR = 0X00000106;      /* ·¢ËÍ0X06Ö¸Áî£¬BY/W25QXXÐ´Ê¹ÄÜ */

    while ((QUADSPI->SR & (1 << 1)) == 0);  /* µÈ´ýÖ¸Áî·¢ËÍÍê³É */

    QUADSPI->FCR |= 1 << 1;

    if (qspi_wait_flag(1 << 5, 0, 0XFFFF) == 0) /* µÈ´ýBUSY¿ÕÏÐ */
    {
        tempreg = 0XEB;         /* INSTRUCTION[7:0]=0XEB,·¢ËÍ0XEBÖ¸Áî£¨Fast Read QUAD I/O£© */
        tempreg |= 1 << 8;      /* IMODE[1:0]=1,µ¥Ïß´«ÊäÖ¸Áî */
        tempreg |= 3 << 10;     /* ADDRESS[1:0]=3,ËÄÏß´«ÊäµØÖ· */
        tempreg |= (uint32_t)g_norflash_addrw << 12;    /* ADSIZE[1:0]=2,24/32Î»µØÖ·³¤¶È */
        tempreg |= 3 << 14;     /* ABMODE[1:0]=3,ËÄÏß´«Êä½»Ìæ×Ö½Ú */
        tempreg |= 0 << 16;     /* ABSIZE[1:0]=0,8Î»½»Ìæ×Ö½Ú(M0~M7) */
        tempreg |= 4 << 18;     /* DCYC[4:0]=4,4¸ödummyÖÜÆÚ */
        tempreg |= 3 << 24;     /* DMODE[1:0]=3,ËÄÏß´«ÊäÊý¾Ý */
        tempreg |= 3 << 26;     /* FMODE[1:0]=3,ÄÚ´æÓ³ÉäÄ£Ê½ */
        QUADSPI->CCR = tempreg; /* ÉèÖÃCCR¼Ä´æÆ÷ */
    }
    __ASM volatile("cpsie i");
    //sys_intx_enable();          /* ¿ªÆôÖÐ¶Ï */
}

/**
 * @brief   QSPI推出内存映射模式
 *  @note   屌用该函数之前务必已经初始化了QSPI接口
 *          sys_qspi_enable_memapmode or norflahs_init
 * @param   无
 * @retval  0:OK;   其他:错误码
*/
static uint8_t norflash_ex_exit_mmap(void)
{
    uint8_t res = 0;

    //sys_intx_disable();         /* ¹Ø±ÕÖÐ¶Ï */
    __ASM volatile("cpsid i");
    SCB_InvalidateICache();     /* Çå¿ÕI CACHE */
    SCB_InvalidateDCache();     /* Çå¿ÕD CACHE */
    QUADSPI->CR &= ~(1 << 0);   /* ¹Ø±Õ QSPI ½Ó¿Ú */
    QUADSPI->CR |= 1 << 1;      /* ÍË³öMEMMAPEDÄ£Ê½ */
    res = qspi_wait_flag(1 << 5, 0, 0XFFFF);    /* µÈ´ýBUSY¿ÕÏÐ */

    if (res == 0)
    {
        QUADSPI->CCR = 0;       /* CCR¼Ä´æÆ÷ÇåÁã */
        QUADSPI->CR |= 1 << 0;  /* Ê¹ÄÜ QSPI ½Ó¿Ú */
    }

    return res;
}

/**
 * @brief   往QSPI FLASH写入数据
 *  @note   在指定地址开始写入指定长度的数据
 *          该函数带有差出操作
 * @param   pbuf:数据存储区
 * @param   addr:开始写入的地址
 * @param   datalen:要写入的字节数
 * @retval  0,OK    其他,错误代码
*/
uint8_t norflash_ex_write(uint8_t* pbuf, uint32_t addr, uint16_t datalen)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();      /*退出内存映射模式*/
    if (res == 0)
    {
        norflash_write(pbuf, addr, datalen);
    }
    norflash_ex_enter_mmap();           /*进入内存映射模式*/
    return res;
}

/**
 * @brief   从QSPI FLASH读取数据
 *  @note   在指定地址开始读取指定长度的数据(必须处于内存映射模式下才能执行)
 *
 * @param   pbuf:数据存储区
 * @param   addr:开始读取的地址(最大32bit)
 * @param   datalen:要读取的字节数(最大65535)
 * @retval  0,OK;   其他,错误代码
*/
void norflash_ex_read(uint8_t* pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i = 0;
    addr += 0x90000000;     /*使用内存映射模式读取，QSPI的基地址是0x90000000，这里所以要加上基址*/
    __ASM volatile("cpsid i");          /*关闭中断*/
    for (i = 0; i < datalen; i++)
    {
        pbuf[i] = *(volatile uint8_t*)(addr + i);
    }
    __ASM volatile("cpsie i");          /*开启中断*/
}

/**
 * @brief   读取QSPI FLASH的ID
 * @param   无
 * @retval  NOR FLASH ID
*/
uint16_t norflash_ex_read_id(void)
{
    uint8_t res = 0;
    uint16_t id = 0;
    res = norflash_ex_exit_mmap();  /* 退出内存映射 */

    if (res == 0)
    {
        id = norflash_read_id();
    }

    norflash_ex_enter_mmap();       /* 进入内存映射 */
    return id;
}

/**
 * @brief   擦除QSPI FLASH的某个扇区
 *  @note   注意，这是扇区地址，不是字节地址！！
 *
 * @param   saddr:扇区地址
 * @retval  无
*/
void norflash_ex_erase_sector(uint32_t saddr)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();      /*退出内存映射模式*/
    if (res == 0)
    {
        norflash_ex_erase_sector(saddr);
    }
    norflash_ex_enter_mmap();
}

/**
 * @brief   擦除QSPI FLASH整个芯片
 *      @note   等待时间超长
 * @param   无
 * @param   无
*/
void norflash_ex_erase_chip(void)
{
    uint8_t res = 0;
    res = norflash_ex_exit_mmap();      /*退出内存映射*/
    if (res == 0)
    {
        norflash_ex_erase_chip();
    }
    norflash_ex_enter_mmap();           /*进入内存映射*/
}
