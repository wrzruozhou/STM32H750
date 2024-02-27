#include "drv_mpu.h"

/*
 * @brief   设置某个区域的MPU保护
 * @param   baseaddr: MPU保护区域的基址(首地址)
 * @param   size:MPU保护区域的大小（必须是32的倍数，单位为字节）
 * @param   rnum:MPU保护区编号，范围:0~7，最大支持8个保护区域
 * @param   de:禁止指令访问;0，允许指令访问;1，禁止指令访问
 * @param   ap:访问权限,访问广西如下：
 *   @arg   1,无访问(特权&用户都不可访问)
 *   @arg 2,禁止用户写访问（特权可读写访问）
 *   @arg 3,全访问（特权&用户都可访问）
 *   @arg 4,无法预测(禁止设置为 4!!!)
 *   @arg 5,仅支持特权读访问
 *   @arg 6,只读（特权&用户都不可以写）
 * @note 详见:STM32H7 编程手册.pdf,4.6.6 节,Table 91.
 * @param sen:是否允许共用;0,不允许;1,允许
 * @param cen:是否允许 cache;0,不允许;1,允许
 * @param ben:是否允许缓冲;0,不允许;1,允许
 * @retval 0, 成功; 1, 错误;
*/
uint8_t mpu_set_protection(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t de, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben)
{
    MPU_Region_InitTypeDef MPU_Init;
    HAL_MPU_Disable();
    MPU_Init.Enable = MPU_REGION_ENABLE;    /* 使能该保护区域 */
    MPU_Init.Number = rnum;                 /* 设置保护区域 */
    MPU_Init.BaseAddress = baseaddr;        /* 设置基地址 */
    MPU_Init.Size = size;                   /* 设置保护区域大小 */
    MPU_Init.SubRegionDisable = 0x00;       /* 禁止子区域 */
    MPU_Init.TypeExtField = MPU_TEX_LEVEL0; /* 设置类型访问拓展域为level0 */
    MPU_Init.AccessPermission = ap;         /* 设置访问权限 */
    MPU_Init.DisableExec = de;              /* 是否允许指令访问 */
    MPU_Init.IsShareable = sen;             /* 是否允许共用 */
    MPU_Init.IsCacheable = cen;             /* 是否允许cache */
    MPU_Init.IsBufferable = ben;            /* 是否允许缓冲 */
    HAL_MPU_ConfigRegion(&MPU_Init);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
    return 0;
}

/**
* @brief 设置需要保护的存储块
* @note 必须对部分存储区域进行 MPU 保护,否则可能导致程序运行异常
* 比如 MCU 屏不显示,摄像头采集数据出错等等问题
* @param 无
* @retval nbytes 以 2 为底的指数值
*/
void mpu_memory_protection(void)
{
    /* 保护整个 DTCM,共 128K 字节,允许指令访问,禁止共用,允许 cache,允许缓冲 */
    mpu_set_protection(0x20000000, MPU_REGION_SIZE_128KB, MPU_REGION_NUMBER1, MPU_INSTRUCTION_ACCESS_ENABLE, MPU_REGION_FULL_ACCESS, MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);

    /* 保护整个 AXI SRAM,共 512K 字节,允许指令访问,禁止共用,允许 cache,允许缓冲 */
    mpu_set_protection(0x24000000, MPU_REGION_SIZE_512KB, MPU_REGION_NUMBER2,
        MPU_INSTRUCTION_ACCESS_ENABLE, MPU_REGION_FULL_ACCESS,
        MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    /* 保护整个 SRAM1~SRAM3,共 288K 字节,允许指令访问,禁止共用,允许 cache,允许缓冲 */
    mpu_set_protection(0x30000000, MPU_REGION_SIZE_512KB, MPU_REGION_NUMBER3,
        MPU_INSTRUCTION_ACCESS_ENABLE, MPU_REGION_FULL_ACCESS,
        MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    /* 保护整个 SRAM4,共 64K 字节,允许指令访问,禁止共用,允许 cache,允许缓冲 */
    mpu_set_protection(0x38000000, MPU_REGION_SIZE_64KB, MPU_REGION_NUMBER4,
        MPU_INSTRUCTION_ACCESS_ENABLE, MPU_REGION_FULL_ACCESS,
        MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    /*保护 MCU LCD 屏所在的 FMC 区域,,共 64M 字节,允许指令访问,禁止共用,禁止 cache,禁止缓冲*/
    mpu_set_protection(0x60000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER5,
        MPU_INSTRUCTION_ACCESS_ENABLE, MPU_REGION_FULL_ACCESS,
        MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE,
        MPU_ACCESS_NOT_BUFFERABLE);
    /* 保护 SDRAM 区域,共 64M 字节,允许指令访问,禁止共用,允许 cache,允许缓冲 */
    mpu_set_protection(0XC0000000, MPU_REGION_SIZE_64MB, MPU_REGION_NUMBER6,
        MPU_INSTRUCTION_ACCESS_ENABLE, MPU_REGION_FULL_ACCESS,
        MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_CACHEABLE, MPU_ACCESS_BUFFERABLE);
    /* 保护整个 NAND FLASH 区域,共 256M 字节,禁止指令访问,禁止共用,禁止 cache,禁止缓冲 */
    mpu_set_protection(0X80000000, MPU_REGION_SIZE_256MB, MPU_REGION_NUMBER7,
        MPU_INSTRUCTION_ACCESS_DISABLE, MPU_REGION_FULL_ACCESS,
        MPU_ACCESS_NOT_SHAREABLE, MPU_ACCESS_NOT_CACHEABLE,
        MPU_ACCESS_NOT_BUFFERABLE);
}

void MemManage_Handler(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
    printf("Mem Access Error!!\r\n"); /* 输出错误信息 */
    delay_ms(1000);
    printf("Soft Reseting...\r\n"); /* 提示软件重启 */
    delay_ms(1000);
    NVIC_SystemReset();
}
