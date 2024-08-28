#include "drv_stm32flash.h"

/**
 * @brief   得到FLASH的错误状态,通关检查SR状态寄存器
 * @param   无
 * @retval  错误代码
 *  @arg    0,无错误
 *  @arg    其他，错误编号
 */
static uint8_t stmflash_get_error_status(void)
{
    uint32_t res = 0;
    res = FLASH->SR1;

    if (res & (1 << 17))return 1;   /*WRPERR=1,写保护错误*/
    if (res & (1 << 18))return 2;   /*PGSERR1=1,编程序列错误*/
    if (res & (1 << 19))return 3;   /*STRBERR=1,复写错误*/
    if (res & (1 << 21))return 4;   /*INCERR1=1,数据一致性错误*/
    if (res & (1 << 22))return 5;   /*OPERR1=1,写/擦除错误*/
    if (res & (1 << 23))return 6;   /*RDPERR=1,读保护错误*/
    if (res & (1 << 24))return 7;   /*RDSERR=1,非法访问加密区错误*/
    if (res & (1 << 25))return 8;   /*SNECCERR=1,1bit ecc校验错误*/
    if (res * (1 << 26))return 9;   /*DBECCERR1=1,2bit ecc校验错误*/

    return 0;       /*没有任何状态/操作完成*/
}

/**
 * @brief   等待操作完成
 * @param   time:要延时的长短
 * @retval  错误代码
 *  @arg    0:已完成
 *  @arg    1~9:错误代码
 *  @arg    0xFF:超时
 */
static uint8_t stmflash_wait_done(uint32_t time)
{
    uint8_t res = 0;
    uint32_t tempreg = 0;
    while (1)
    {
        tempreg = FLASH->SR1;
        if ((tempreg & 0x07) == 0)
        {
            /*BSY = 0,WBEN = 0,QW = 0表示操作完成*/
            break;
        }
        time--;
        if (time == 0)return 0xff;
    }
    res = stmflash_get_error_status();
    if (res)
    {
        FLASH->CCR1 = 0x07EE0000;   /*清除所有错误标志*/
    }
    return res;
}

/**
 * @brief   擦除扇区
 * @param   无
 * @param   错误代码
 *  @arg    0:已完成
 *  @arg    1~9:错误代码
 *  @arg    0xFF:超时
 */
static uint8_t stmflash_erase_sector0(void)
{
    uint8_t res = 0;
    res = stmflash_wait_done(0xFFFFFFFF);       /*等待上次操作结束*/

    if (res == 0)       /*等待完成*/
    {
        FLASH->CR1 &= ~(7 << 8);                        /*SNB1[2:0]=0,清除原来的设置   对哪个扇区进行编程*/
        FLASH->CR1 &= ~(3 << 4);                        /*PSIZE1[1:0]=0,清除原来的设置 使用多大字大小来执行编程*/
        FLASH->CR1 |= (uint32_t)0 << 8;                 /*设置要擦除的扇区编号为0*/
        FLASH->CR1 |= 2 << 4;                           /*设置为32bit宽*/
        FLASH->CR1 |= 1 << 2;                           /*SER1=1,扇区擦除*/
        FLASH->CR1 |= 1 << 7;                           /*START1=1,开始擦除*/
        res = stmflash_wait_done(0xFFFFFFFF);      /*等待扇区操作结束*/
        FLASH->CR1 &= ~(1 << 2);                        /*SER1=0,清除扇区擦除标志*/
    }
    return res;
}

/**
 * @brief   在FLASH指定地址写8个字，即256bit
 *  @note   必须以256bit为单位(32字节)编程!!
 * @param   faddr:指定地址(此地址必须是4的倍数!!)
 * @param   pdata:要写入的数据
 * @retval  错误代码
 *  @arg    0:写入成功
 *  @arg    其他:错误代码
 */
static uint8_t stmflash_write_8word(uint32_t faddr, uint32_t* pdata)
{
    volatile uint8_t nword = 8;     /*每次写8个字，256bit*/
    uint8_t res;
    res = stmflash_wait_done(0xFFFF);

    if (res == 0)        /*OK的*/
    {
        FLASH->CR1 &= ~(3 << 4);        /*PSIZE1[1:0] = 0,清除原来的设置*/
        FLASH->CR1 |= 2 << 4;           /*设置为32bit位宽*/
        FLASH->CR1 |= 1 << 1;           /*PG=1,编程使能*/

        __DSB();

        while (nword)
        {
            *(volatile uint32_t*)faddr = *pdata;        /*写入数据*/
            faddr += 4;                                 /*写地址+4*/
            pdata++;                                    /*偏移到下一个数据的首地址*/
            nword--;
        }
        __DSB();                                        /*写操作完成后，屏蔽数据同步，使CPU重新执行指令序列*/
        res = stmflash_wait_done(0xFFFF);           /*等待操作完成，一个字编程，最多100us*/
        FLASH->CR1 &= ~(1 << 1);                        /*PG1 = 0,清除扇区擦除标志*/
    }
    return res;
}

/**
 * @brief   读取指定一个地址的一个字(32位数据)
 * @param   faddr:要读取的地址
 * @retval  读取到的数据
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t*)faddr;
}

/**
 * @brief   从指定地址开始读出指定长度的数据
 * @param   raddr:起始地址
 * @param   pbuf:数据指针
 * @param   length:要读取的字(32)数，即4个字节的整数倍
 * @retval  无
 */
void stmflash_read(uint32_t raddr, uint32_t* pbuf, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_word(raddr);        /*读取4个字节*/
        raddr += 4;                                         /*偏移4个字节*/
    }

}


/**
 * @brief   从指定地址开始写入指定长度的数据
 *  @note   特别注意：因为STM32H750只有一个扇区(128KB),因此我们规定,前
 *          16K留作BootLoader用，后112K用作App用，我们要做写入测试，
 *          尽量使用16K以后的地址，否则容易出问题。另外，由于在写入数据
 *          时，必须是0xFF才能写入数据，因此补课避免的需要擦除扇区。
 *          所以在擦除时需要先对前16K的数据做备份保存(读取到RAM)，然后再写入
 *          以保证前16K数据的完整性。且执行写入操作的时候，不能发生任何中断
 *          (凡是再写入时执行内部的FLASH代码，必将导致hardfalut).
 * @param   waddr:起始地址(此地址必须是32的倍数!!,否则写入出错)
 * @param   length:字(32位)数(就是要写入的32位数据的个数，一次至少写入32字节
 *          ，即8个字)
 * @retval  无
 */
 /* FLASH写入数据的缓存 */
uint32_t g_flashbuf[BOOT_FLASH_SIZE / 4];

void stmflash_write(uint32_t waddr, uint32_t* pbuf, uint32_t length)
{
    uint8_t status = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    uint16_t wbfcyc = BOOT_FLASH_SIZE / 32;     /*写入bootflashbuf是，需要执行的循环数*/
    uint32_t* wbfptr;
    uint32_t wbfaddr;

    if (waddr < (STM32_FLASH_BASE + BOOT_FLASH_SIZE))return;    /*写入地址小于STM32基地址加Bootlader长度的地址 非法*/

    if (waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE))return;   /*写入地址大于STM32总FLASH地址范围，非法*/

    if (waddr % 32)return;                                      /*写入地址不是32字节倍数，非法*/

    HAL_FLASH_Unlock();                 /*解锁*/
    addrx = waddr;                      /*写入的起始地址*/
    endaddr = waddr + length * 4;

    while (addrx < endaddr)             /*扫清一切障碍(对非FFFFFFFF的地方，先擦除)*/
    {
        if (stmflash_read_word(addrx) != 0xFFFFFFFF)    /*对又非0xFFFFFFFF的地方，要擦除这个扇区*/
        {
            stmflash_read(STM32_FLASH_BASE, g_flashbuf, BOOT_FLASH_SIZE / 4);/*读bootloader这个地方的数据*/
            __ASM volatile("cpsid i");      /*关闭所有中断*/

            status = stmflash_erase_sector0();

            if (status)                     /*发生错误了*/
            {
                __ASM volatile("cpsie i");  /*允许中断*/
                break;                      /*发生错误了*/
            }
            SCB_CleanInvalidateDCache();    /*清除无效的D-Cache*/
            wbfptr = g_flashbuf;            /*指向g_flashbuf的首地址*/
            wbfaddr = STM32_FLASH_BASE;     /*指向STM32 FLASH首地址*/

            while (wbfcyc)
            {
                if (stmflash_write_8word(wbfaddr, wbfptr))  /*写入数据*/
                {
                    break;                                              /*写入异常*/
                }

                wbfaddr += 32;
                wbfptr += 8;
                wbfcyc--;
            }
            __ASM volatile("cpsie i");  /*允许中断*/
        }
        else
        {
            addrx += 4;                 /*偏移到下一个位置*/
        }
    }
    if (status == HAL_OK)
    {
        while (waddr < endaddr)     /*写数据*/
        {
            if (stmflash_write_8word(waddr, pbuf))     /*写入数据*/
            {
                break;      /*写入异常*/
            }
            waddr += 32;
            pbuf += 8;
        }
    }
    HAL_FLASH_Lock();       /*上锁*/
}

