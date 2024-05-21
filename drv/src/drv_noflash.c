#include "drv_noflash.h"

uint16_t g_norflash_type = W25Q128;     /*默认是W25Q128*/

/*SPI FLASH地址位宽*/
volatile uint8_t g_norflash_addrw = 2;  /*这个会在read id函数里面修改*/

/**
 * @brief   初始化SPI NOR FLASH
 * @note    使用内存映射后千万别用这个函数
 * @param   无
 * @retval  无
*/
void noreflash_init(void)
{
    uint8_t temp;
    qspi_init();            /*初始化QSPI*/

}

/**
 * @brief   等待空闲
 * @param   无
 * @retval  无
*/
static void norflash_wait_busy(void)
{
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /*等待BUSY位清空*/
}

/**
 * @brief   退出QSPI模式
 * @param   无
 * @retval  无
*/
static void norflash_qspi_disable(void)
{
    qspi_send_cmd(FLASH_ExitQPIMode, 0, (0 << 6) | (0 << 4) | (0 << 2) | (3 << 0), 0);
}

/**
 * @brief   使能FLASH QE位，使能IO2/IO3,也就是启用4线SPI
 * @param   无
 * @param   无
*/
static void norflash_qe_enabel(void)
{
    uint8_t stareg2 = 0;
    stareg2 = norflash_read_sr(2);      /*先读出状态寄存器2的原始值*/
    /*调试用*/
    // printf("stareg2:%x\r\n", stareg2);
    if ((stareg2 & 0x02) == 0)                  /*QE位未使能*/
    {
        norflash_write_enable();                    /*写使能*/
        stareg2 |= 1 << 1;                         /*使能QE位*/
        norflash_write_sr(2, stareg2);  /*写状态寄存器*/
    }
}

/**
 * @brief   NM25Q128写使能
 *  @note   将S1寄存器的WEL置位
 * @param   无
 * @retval  无
*/
void norflash_write_enable(void)
{
    qspi_send_cmd(FLASH_WriteEnable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
}

/**
 * @brief   25Q128写禁止
 *  @note   将S1寄存器的WEL清零
 * @param   无
 * @retval  无
*/
void norflash_write_disable(void)
{
    qspi_send_cmd(FLASH_WriteDisable, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
}

/**
 * @brief   读取25QXX的状态寄存器，25QXX一共有3个状态寄存器
 *  @note   这个貌似以  W25Q128 为准来写的
 *          状态寄存器1:
 *          BIT7    6   5   4   3   2   1   0
 *          SPR     RV  TB  BP2 BP1 BP0 WEL BYST
 *          SPR:默认0，状态寄存器保护位，配合WP使用
 *          TB,BP2,BP1,BP0:FLASH区域写保护设置
 *          WEL:写使能锁定
 *          BUSY:忙标记位(1,忙;0,空闲)
 *          默认:0x00
 *
 *          状态寄存器2:
 *          BIT7    6   5   4   3   2   1   0
 *          SUS     CMP LB3 LB2 LB1 (R) QE  SPR1
 *
 *          状态寄存器3:
 *          BIT7        6       5       4    3   2   1   0
 *          HOLD/RST    DRV1    DRV0    (R)  (R) WPS ADP ADS
 * @param   regno:状态寄存器号,1~3
 * @retval  状态寄存器值
*/
uint8_t norflash_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;
    switch (regno)
    {
    case 1:
        command = FLASH_ReadStatusReg1;
        break;

    case 2:
        command = FLASH_ReadStatusReg2;
        break;

    case 3:
        command = FLASH_ReadStatusReg3;
        break;

    default:
        command = FLASH_ReadStatusReg1;
        break;
    }

    /* SPI，写command指令，地址为0，单线传输数据 8位地址 无地址 单线传输指令 无空周期 1字节数据*/
    qspi_send_cmd(command, 0, (1 << 0) | (0 << 2) | (0 << 4) | (1 << 6), 0);
    qspi_receive(&byte, 1);
    return byte;
}

/**
 * @brief   写NM25Q128状态寄存器
 *  @note   寄存器状态说明见上函数
 * @param   regno:状态寄存器的编号，1-3
 * @param   sr:要写入状态寄存器的值
 * @retval  无
*/
void norflash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;
    switch (regno)
    {
    case 1:
        command = FLASH_WriteStatusReg1;
        break;

    case 2:
        command = FLASH_WriteStatusReg2;
        break;

    case 3:
        command = FLASH_WriteStatusReg3;
        break;
    default:
        command = FLASH_WriteStatusReg1;
        break;
    }
    /*SPI写command指令*/
    qspi_send_cmd(command, 0, (1 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    qspi_transmit(&sr, 1);
}


/**
 * @brief   读取芯片的ID
 * @param   无
 * @retval  FLASH芯片ID
*/
uint16_t norflash_read_id(void)
{
    uint8_t temp[2];
    uint16_t deviceid;
    qspi_init();            /*进行库函数调用前要先初始化*/

    qspi_send_cmd(FLASH_ManufactDeviceID, 0, (1 << 6) | (2 << 4) | (1 << 2) | (1 << 0), 0);
    qspi_receive(temp, 2);
    deviceid = (temp[0] << 8) | temp[1];
    //    printf("the flash id is %x\r\n", deviceid);
        // if (deviceid == W25Q128)
        // {
            //     printf("the flash id is %x\r\n", deviceid);
            // }
    return deviceid;
}

/**
 * @brief   读取SPI FLASH，仅支持QSPI模式
 *    @note 在指定地址开始读取指定长度的数据
 * @param   pbuf    :数据存储区域
 * @param   addr    :开始读取的地址（最大32Bit）
 * @param   datalen :要读取的字节数(最大65536)
 * @retval  无
*/
void norflash_read(uint8_t* pbuf, uint32_t addr, uint16_t datalen)
{
    /* QSPI，快速读数据，地址位addr,4线传输数据，24位地址， */
    qspi_send_cmd(FLASH_FastReadQuad, addr, (3 << 6) | (g_norflash_addrw << 4) | (3 << 2) | (1 << 0), 6);
    qspi_receive(pbuf, datalen);
}

/**
 * @brief   SPI在一页(0~65536)内写入少于256个字节的数据
 *  @note   在指定地址开始写入最大256字节的数据
 * @param   pbuf:数据存储区
 * @param   addr:开始写入的地址(最大32bit)
 * @param   datalen:要写入的字节数(最大256)，该数不应该超过该页的剩余字节数!!!
 * @retval  无
*/
static void norflash_write_page(uint8_t* pbuf, uint32_t addr, uint16_t datalen)
{
    norflash_write_enable();

    /* QSPI，页写指令 */
    qspi_send_cmd(FLASH_PageProgramQuad, addr, (3 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);;

    qspi_transmit(pbuf, datalen);
    norflash_wait_busy();
}

/**
 * @brief   无检验写SPI FLASH
 *  @note   必须确保所写的地址范围内的数据全位0XFF，否则在非0XFF处写入的数据将失败！
 *          具有自动换页功能
 *          在指定地址开始写入指定长度的数据，但是要确保地址不越界
 *
 * @param   pbuf:数据存储区
 * @param   addr:开始写入的地址(最大32bit)
 * @param   datalen:要写入的字节数(最大65535)
 * @retval  无
*/
static void norflash_write_nocheck(uint8_t* pbuf, uint32_t addr, uint16_t datalen)
{
    // uint16_t pagermain;
    // pagermain = 256 - addr % 256;
    // if (datalen < pagermain)
    // {
    //     pagermain = datalen;
    // }

    // while (1)
    // {
    //     /*当写入字节比页内剩余地址还少的时候，一次写完*/
    //     /*当写入字节比页内剩余地址还多的时候，先写完整个页内剩余地址，然后根据剩余长度不同进行处理*/
    //     norflash_write_page(pbuf, addr, pagermain);
    //     if (datalen == pagermain)break;
    //     else {              /*datalen > pagermain*/
    //         pbuf += pagermain;          /*pbuf指针便宜地址，前面已经写了pagermain字节*/
    //         addr += pagermain;          /*写地址偏移，前面已经写了pagermain字节*/
    //         datalen -= pagermain;       /*写入总长度减去已经写入了的字节数*/

    //         if (datalen > 256)          /*剩余数据还大于一页，可以一次写入一页*/
    //         {
    //             pagermain = 256;        /*一次可以写入256个字节*/
    //         }
    //         else                        /*剩余数据小于一页，可以一次全部写完*/
    //         {
    //             pagermain = datalen;    /*不够256个字节*/
    //         }
    //     }
    // }
    uint16_t pageremain;
    pageremain = 256 - addr % 256;  /* µ¥Ò³Ê£ÓàµÄ×Ö½ÚÊý */

    if (datalen <= pageremain)      /* ²»´óÓÚ256¸ö×Ö½Ú */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* µ±Ð´Èë×Ö½Ú±ÈÒ³ÄÚÊ£ÓàµØÖ·»¹ÉÙµÄÊ±ºò, Ò»´ÎÐÔÐ´Íê
         * µ±Ð´ÈëÖ±½Ó±ÈÒ³ÄÚÊ£ÓàµØÖ·»¹¶àµÄÊ±ºò, ÏÈÐ´ÍêÕû¸öÒ³ÄÚÊ£ÓàµØÖ·, È»ºó¸ù¾ÝÊ£Óà³¤¶È½øÐÐ²»Í¬´¦Àí
         */
        norflash_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain)   /* Ð´Èë½áÊøÁË */
        {
            break;
        }
        else     /* datalen > pageremain */
        {
            pbuf += pageremain;         /* pbufÖ¸ÕëµØÖ·Æ«ÒÆ,Ç°ÃæÒÑ¾­Ð´ÁËpageremain×Ö½Ú */
            addr += pageremain;         /* Ð´µØÖ·Æ«ÒÆ,Ç°ÃæÒÑ¾­Ð´ÁËpageremain×Ö½Ú */
            datalen -= pageremain;      /* Ð´Èë×Ü³¤¶È¼õÈ¥ÒÑ¾­Ð´ÈëÁËµÄ×Ö½ÚÊý */

            if (datalen > 256)          /* Ê£ÓàÊý¾Ý»¹´óÓÚÒ»Ò³,¿ÉÒÔÒ»´ÎÐ´Ò»Ò³ */
            {
                pageremain = 256;       /* Ò»´Î¿ÉÒÔÐ´Èë256¸ö×Ö½Ú */
            }
            else     /* Ê£ÓàÊý¾ÝÐ¡ÓÚÒ»Ò³,¿ÉÒÔÒ»´ÎÐ´Íê */
            {
                pageremain = datalen;   /* ²»¹»256¸ö×Ö½ÚÁË */
            }
        }
    }
}

/**
 * @brief   写SPI FLASH
 *  @note   在指定地址开始写指定长度的数据，该函数带擦除操作
 *          SPI FLASH 一般是：256个字节位一个Page,4K位一个sector,16个Sector为一个Block，
 *          擦除的最小单位为Sector
 * @param   pbuf:数据存储区
 * @param   addr:开始写入的地址(最大32bit)
 * @param   datalen:要写入的字节数(最大65535)
 * @retval  无
*/
uint8_t g_norflash_buf[4096];   /*扇区缓存*/

void norflash_write(uint8_t* pbuf, uint32_t addr, uint16_t datalen)
{
#if 0
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t* norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;           /*扇区地址*/
    secoff = addr % 4096;           /*在扇区内的偏移*/
    secremain = 4096 - secoff;      /*扇区剩余空间大小*/

    // printf("ad:%X,nb:%X\r\n", addr, datalen);       /*测试用*/
    if (datalen < secremain)secremain = datalen;

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /*读出整个扇区的内容*/
        for (i = 0; i < secremain; i++)        /*校验数据*/
        {
            if (norflash_buf[secoff + i] != 0xff)
            {
                break;                          /*需要擦除，退出for循环*/
            }
        }
        if (i < secremain)        /*需要擦除*/
        {
            norflash_erase_sector(secpos);      /*擦除这个扇区*/
            for (i = 0; i < secremain; i++)            /*复制*/
            {
                norflash_buf[i + secoff] = pbuf[i];
            }
            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);
        }
        else                    /*写已经擦除了的，直接写入扇区剩余空间*/
        {
            norflash_write_nocheck(pbuf, addr, secremain);      /*直接写扇区*/
        }

        if (datalen == secremain)break;      /*写入结束了*/
        else {
            secpos++;       /*扇区地址+1*/
            secoff = 0;     /*偏移位置设置为0*/

            pbuf += secremain;  /*指针偏移*/
            addr += secremain;  /*写地址偏移*/
            datalen -= secremain;   /*字节数递减*/

            if (datalen > 4096)
            {
                secremain = 4096;   /*下一个扇区还是写不完*/
            }
            else
            {
                secremain = datalen;/*下一个扇区可以直接写完了*/
            }
}
}
#else
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t* norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       /* ÉÈÇøµØÖ· */
    secoff = addr % 4096;       /* ÔÚÉÈÇøÄÚµÄÆ«ÒÆ */
    secremain = 4096 - secoff;  /* ÉÈÇøÊ£Óà¿Õ¼ä´óÐ¡ */

    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* ²âÊÔÓÃ */
    if (datalen <= secremain)
    {
        secremain = datalen;    /* ²»´óÓÚ4096¸ö×Ö½Ú */
    }

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /* ¶Á³öÕû¸öÉÈÇøµÄÄÚÈÝ */

        for (i = 0; i < secremain; i++)   /* Ð£ÑéÊý¾Ý */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;      /* ÐèÒª²Á³ý, Ö±½ÓÍË³öforÑ­»· */
            }
        }

        if (i < secremain)   /* ÐèÒª²Á³ý */
        {
            norflash_erase_sector(secpos);  /* ²Á³ýÕâ¸öÉÈÇø */

            for (i = 0; i < secremain; i++)   /* ¸´ÖÆ */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* Ð´ÈëÕû¸öÉÈÇø */
        }
        else        /* Ð´ÒÑ¾­²Á³ýÁËµÄ,Ö±½ÓÐ´ÈëÉÈÇøÊ£ÓàÇø¼ä. */
        {
            norflash_write_nocheck(pbuf, addr, secremain);  /* Ö±½ÓÐ´ÉÈÇø */
        }

        if (datalen == secremain)
        {
            break;  /* Ð´Èë½áÊøÁË */
        }
        else        /* Ð´ÈëÎ´½áÊø */
        {
            secpos++;               /* ÉÈÇøµØÖ·Ôö1 */
            secoff = 0;             /* Æ«ÒÆÎ»ÖÃÎª0 */

            pbuf += secremain;      /* Ö¸ÕëÆ«ÒÆ */
            addr += secremain;      /* Ð´µØÖ·Æ«ÒÆ */
            datalen -= secremain;   /* ×Ö½ÚÊýµÝ¼õ */

            if (datalen > 4096)
            {
                secremain = 4096;   /* ÏÂÒ»¸öÉÈÇø»¹ÊÇÐ´²»Íê */
            }
            else
            {
                secremain = datalen;/* ÏÂÒ»¸öÉÈÇø¿ÉÒÔÐ´ÍêÁË */
            }
        }
    }
#endif

}

/**
 * @brief   擦除整个芯片
 *  @note   等待时间超长
 * @param   无
 * @retval  无
*/
void norflash_erase_chip(void)
{
    norflash_write_enable();    /*写使能*/
    norflash_wait_busy();       /*等待空闲*/
    /*QPI,写全片擦除指令，地址为0，无数据 8位地址 无地址 1线传输指令 无空周期 0字节数据*/
    qspi_send_cmd(FLASH_ChipErase, 0, (0 << 6) | (0 << 4) | (0 << 2) | (1 << 0), 0);
    norflash_wait_busy();
}

/**
 * @brief   擦除整个扇区
 *  @note   注意这里是扇区地址，不是字节地址！！
 *          擦除一个扇区的最少时间为:150ms
 *
 * @param   saddr:扇区地址 根据实际容量设置
 * @retval  无
*/
void norflash_erase_sector(uint32_t saddr)
{
    // saddr *= 4096;
    // norflash_write_enable();        /*写使能*/
    // norflash_wait_busy();           /*等待空闲*/

    // /*QPI,写扇区擦除指令，地址为0，无数据， 24、32位地址*/
    // qspi_send_cmd(FLASH_SectorErase, saddr, (0 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);
    // norflash_wait_busy();
    //printf("fe:%x\r\n", saddr);   /* ¼àÊÓfalsh²Á³ýÇé¿ö,²âÊÔÓÃ */

    saddr *= 4096;
    norflash_write_enable();        /* Ð´Ê¹ÄÜ */
    norflash_wait_busy();           /* µÈ´ý¿ÕÏÐ */

    /* QPI,Ð´ÉÈÇø²Á³ýÖ¸Áî,µØÖ·Îª0,ÎÞÊý¾Ý_24/32Î»µØÖ·_1Ïß´«ÊäµØÖ·_1Ïß´«ÊäÖ¸Áî,ÎÞ¿ÕÖÜÆÚ,0¸ö×Ö½ÚÊý¾Ý */
    qspi_send_cmd(FLASH_SectorErase, saddr, (0 << 6) | (g_norflash_addrw << 4) | (1 << 2) | (1 << 0), 0);

    norflash_wait_busy();           /* µÈ´ý²Á³ýÍê³É */
}
