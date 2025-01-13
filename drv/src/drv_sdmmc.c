#include "drv_sdmmc.h"

#include <string.h>

SD_HandleTypeDef g_sd_handle;                   /*SD卡句柄*/
HAL_SD_CardInfoTypeDef g_sd_card_info_handle;   /*SD卡信息结构体*/

/**
 *  @brief   初始化SD卡
 *  @param  无
 *  @retval 0:初始化完成
 *          other：初始化错误
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;

    HAL_SD_DeInit(&g_sd_handle);                        /*清空相关参数*/

    /*初始化时钟不能大于400Khz*/
    g_sd_handle.Instance = SDMMC1;
    g_sd_handle.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;                           /*上升沿*/
    g_sd_handle.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;               /*空闲时不关闭时钟*/
    g_sd_handle.Init.BusWide = SDMMC_BUS_WIDE_4B;                                   /*4位数据线*/
    g_sd_handle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;     /*关闭硬件流控制*/
    g_sd_handle.Init.ClockDiv = SDMMC_NSpeed_CLK_DIV;                               /*SD最大传输速度为25Mhz*/
    SD_Error = HAL_SD_Init(&g_sd_handle);

    if (SD_Error != HAL_OK)
    {
        return 1;
    }

    HAL_SD_GetCardInfo(&g_sd_handle, &g_sd_card_info_handle);
    return 0;
}

/**
 *  @brief  SDMMC底层驱动:时钟使能；引脚配置
 */
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_SDMMC1_CLK_ENABLE();          /*使能SDMMC1时钟*/
    SD1_D0_GPIO_CLK_ENABLE();               /*D0引脚IO时钟使能*/
    SD1_D1_GPIO_CLK_ENABLE();               /*D1引脚IO时钟使能*/
    SD1_D2_GPIO_CLK_ENABLE();               /*D2引脚IO时钟使能*/
    SD1_D3_GPIO_CLK_ENABLE();               /*D3引脚IO时钟使能*/
    SD1_CLK_GPIO_CLK_ENABLE();              /*CLK引脚IO时钟使能*/
    SD1_CMD_GPIO_CLK_ENABLE();              /*CMD引脚IO时钟使能*/

    gpio_init_struct.Alternate = GPIO_AF12_SDIO1;   /*服用为SDIO*/
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pin = SD1_D0_GPIO_PIN;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(SD1_D0_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = SD1_D1_GPIO_PIN;
    HAL_GPIO_Init(SD1_D1_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = SD1_D2_GPIO_PIN;
    HAL_GPIO_Init(SD1_D2_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin - SD1_D3_GPIO_PIN;
    HAL_GPIO_Init(SD1_D3_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = SD1_CLK_GPIO_PIN;
    HAL_GPIO_Init(SD1_CLK_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = SD1_CMD_GPIO_PIN;
    HAL_GPIO_Init(SD1_CMD_GPIO_PORT, &gpio_init_struct);
}

/**
 *  @brief  获取卡信息函数
 *  @param  cardinfo:SD卡信息句柄
 *  @retval 返回值:读取卡信息状态值
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef* cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&g_sd_handle, cardinfo);
    return sta;
}

/**
 *  @brief  判断SD卡是否可以读写数据
 *  @param  无
 *  @retval 返回值: SD_TRANSFER_OK  传输完成可以继续下一次传输
 *                  SD_TRANSFER_BUSY    SD卡正忙,不可以进行下一次传输
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&g_sd_handle) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}



/**
 *  @brief  读SD卡数据
 *  @param  buf:读数据缓存区
 *  @param  sector:扇区地址
 *  @param  cnt:扇区个数
 *  @retval 返回值0:正常;其他:错误
 */
uint8_t sd_read_disk(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = sector;

    __ASM volatile("cpsid i");      /*关闭所有中断*/
    sta = HAL_SD_ReadBlocks(&g_sd_handle, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT); /* 多个sector的读操作 */
    /*等待SD卡读完*/
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
            break;
        }
    }

    __ASM volatile("cpsie i");      /*开启所有中断*/
    return sta;
}

/**
 *  @brief  写数据到SD卡
 *  @param  buf:写数据缓冲区
 *  @param  sector:扇区地址
 *  @param  cnt:扇区个数
 *  @retval 返回值:0正常;其他:错误
 */
uint8_t sd_write_disk(uint8_t* buf, uint32_t sector, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = sector;
    __ASM volatile("cpsid i");      /*关闭所有中断*/

    sta = HAL_SD_WriteBlocks(&g_sd_handle, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT);    /*多个sector写操作*/
    /*等待SD卡写完*/
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
            break;
        }
    }
    __ASM volatile("cpsid i");      /*关闭所有中断*/
    return sta;
}

/*-----------------------------------以下内容为测试SD卡-------------------------------------------*/
/**
 *  @brief  通过串口打印SD卡相关信息
 *  @param  无
 *  @param  无
 */
void show_sdcard_info(void)
{
    uint64_t card_capacity;     /*SD卡容量*/
    HAL_SD_CardCIDTypeDef sd_card_cid;

    HAL_SD_GetCardCID(&g_sd_handle, &sd_card_cid);  /*获取CID*/
    get_sd_card_info(&g_sd_card_info_handle);       /*获取SD卡信息*/

    switch (g_sd_card_info_handle.CardType)
    {
    case CARD_SDSC:
        if (g_sd_card_info_handle.CardVersion == CARD_V1_X)
        {
            printf("Card Type:SDSC V1\r\n");
        }
        else if (g_sd_card_info_handle.CardVersion == CARD_V2_X)
        {
            printf("Card Type:SDSC V2\r\n");
        }

        break;
    case CARD_SDHC_SDXC:
        printf("Card Type:SDHC\r\n");

    default:
        break;
    }

    card_capacity = (uint64_t)(g_sd_card_info_handle.LogBlockNbr) * (uint64_t)(g_sd_card_info_handle.LogBlockSize); /*计算SD卡容量*/
    printf("Card ManufacturerID:%d\r\n", sd_card_cid.ManufacturerID);
    printf("Card RCA:%d\r\n", g_sd_card_info_handle.RelCardAdd);
    printf("LogBlockNbr:%d\r\n", (uint32_t)(g_sd_card_info_handle.LogBlockNbr));
    printf("LogBlockSize:%d\r\n", (uint32_t)(g_sd_card_info_handle.LogBlockSize));
    printf("Card Capacity:%d MB\r\n", (uint32_t)(card_capacity >> 20));
    printf("Card BlockSize:%d\r\n\r\n", g_sd_card_info_handle.BlockSize);
}

/**
 *  @brief  测试SD卡的读取
 * @note
 *  @param  secaddr:扇区地址
 *  @param  seccnt:扇区数
 *  @retval 无
 */
void sd_test_read(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t* buf;
    uint8_t sta = 0;

    buf = mymalloc(SRAMIN, seccnt * 512);   /*申请内存，从SRAM里面申请内存*/
    sta = sd_read_disk(buf, secaddr, seccnt);   /*读取secadr扇区开始的内容*/

    if (sta == 0)
    {
        printf("SECTOR %d DATA:\r\n", secaddr);
        for (i = 0; i < seccnt * 512; i++)
        {
            printf("%x", buf[i]);       /*打印secaddr开始的扇区数据*/
        }
        printf("\r\nDATA ENDED\r\n");
    }
    else {
        printf("err:%d\r\n", sta);
    }
    myfree(SRAMIN, buf);        /*释放内存*/
}

/**
 *  @brief  测试SD卡的写入
 * @note    从secaddr地址开始，写入seccnt给扇区的数据
 *          慎用!!最好写全是0xFF的扇区，否则可能损坏SD卡
 *  @param  secaddr:扇区地址
 *  @param  seccnt:扇区数
 *  @retval 无
 */
void sd_test_write(uint32_t secaddr, uint32_t seccnt)
{
    uint32_t i;
    uint8_t* buf;
    uint8_t sta = 0;
    buf = mymalloc(SRAMIN, seccnt * 512);       /*从SRAM申请内存*/

    for (i = 0; i < seccnt * 512; i++)                      /*初始化写入的数据，是3的倍数*/
    {
        buf[i] = i * 3;
    }
    sta = sd_write_disk(buf, secaddr, seccnt);  /*从secaddr扇区开始写入seccnt个扇区内容*/
    if (sta == 0)
    {
        printf("Write over!\r\n");
    }
    else
    {
        printf("err:%d\r\n", sta);
    }
    myfree(SRAMIN, buf);        /*释放内存*/
}
