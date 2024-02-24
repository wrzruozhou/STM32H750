#include "drv_rcc.h"

/**
* @brief 使能 STM32H7 的 L1-Cache, 同时开启 D cache 的强制透写
* @param 无
* @retval 无
*/
void sys_cache_enable(void)
{
    SCB_EnableICache(); /* 使能 I-Cache,函数在 core_cm7.h 里面定义 */
    SCB_EnableDCache(); /* 使能 D-Cache,函数在 core_cm7.h 里面定义 */
    SCB->CACR |= 1 << 2; /* 强制 D-Cache 透写,如不开启透写,实际使用中可能遇到各种问题 */
}

/**
* @brief QSPI 进入内存映射模式（执行 QSPI 代码必备前提）
* @note 必须根据所使用 QSPI FLASH 的容量设置正确的 ftype 值!
* @param ftype: flash 类型
* @arg 0, 普通 FLASH, 容量在 128Mbit 及以内的
* @arg 1, 大容量 FLASH, 容量在 256Mbit 及以上的.
* @retval 无
*/
void sys_qspi_enable_memmapmode(uint8_t ftype)
{
    uint32_t tempreg = 0;
    GPIO_InitTypeDef qspi_gpio;
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* 使能 PORTB 时钟 */
    __HAL_RCC_GPIOD_CLK_ENABLE(); /* 使能 PORTD 时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE(); /* 使能 PORTE 时钟 */
    __HAL_RCC_QSPI_CLK_ENABLE(); /* QSPI 时钟使能 */
    qspi_gpio.Pin = GPIO_PIN_6; /* PB6 AF10 */
    qspi_gpio.Mode = GPIO_MODE_AF_PP;
    qspi_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    qspi_gpio.Pull = GPIO_PULLUP;
    qspi_gpio.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);
    qspi_gpio.Pin = GPIO_PIN_2; /* PB2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &qspi_gpio);
    qspi_gpio.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;/* PD11,12,13 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOD, &qspi_gpio);
    qspi_gpio.Pin = GPIO_PIN_2; /* PE2 AF9 */
    qspi_gpio.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOE, &qspi_gpio);
    /* QSPI 设置，参考 QSPI 实验的 QSPI_Init 函数 */
    RCC->AHB3RSTR |= 1 << 14; /* 复位 QSPI */
    RCC->AHB3RSTR &= ~(1 << 14); /* 停止复位 QSPI */
    while (QUADSPI->SR & (1 << 5)); /* 等待 BUSY 位清零 */
    /* QSPI 时钟源已经在 sys_stm32_clock_init()函数中设置 */
    /* 设置 CR 寄存器, 这些值怎么来的，请参考 QSPI 实验/看 H750 参考手册寄存器描述分析 */
    QUADSPI->CR = 0X01000310;
    /* 设置 DCR 寄存器(FLASH 容量 32M(最大容量设置为 32M, 默认用 16M 的), tSHSL=3 个时钟) */
    QUADSPI->DCR = 0X00180201;
    QUADSPI->CR |= 1 << 0; /* 使能 QSPI */
    /*
    * 注意:QSPI QE 位的使能，在 QSPI 烧写算法里面，就已经设置了
    * 所以, 这里可以不用设置 QE 位，否则需要加入对 QE 位置 1 的代码
    * 不过, 代码必须通过仿真器下载, 直接烧录到外部 QSPI FLASH, 是不可用的
    * 如果想直接烧录到外部 QSPI FLASH 也可以用, 则需要在这里添加 QE 位置 1 的代码
    *
    * 另外, 对与 W25Q256,还需要使能 4 字节地址模式,或者设置 S3 的 ADP 位为 1.
    * 我们在 QSPI 烧写算法里面已经设置了 ADP=1(上电即 32 位地址模式),因此这里也
    * 不需要发送进入 4 字节地址模式指令/设置 ADP=1 了, 否则还需要设置 ADP=1
    */
    /* BY/W25QXX 写使能（0X06 指令） */
    while (QUADSPI->SR & (1 << 5)); /* 等待 BUSY 位清零 */
    QUADSPI->CCR = 0X00000106; /* 发送 0X06 指令， BY/W25QXX 写使能 */
    while ((QUADSPI->SR & (1 << 1)) == 0);/* 等待指令发送完成 */
    QUADSPI->FCR |= 1 << 1; /* 清除发送完成标志位 */
    /* MemroyMap 模式设置 */
    while (QUADSPI->SR & (1 << 5)); /* 等待 BUSY 位清零 */
    QUADSPI->ABR = 0; /* 交替字节设置为 0，实际上就是 25QXX 0XEB 指令的, M0~M7 = 0 */
    tempreg = 0XEB; /*INSTRUCTION[7:0]=0XEB, 发送 0XEB 指令（Fast Read QUAD I/O） */
    tempreg |= 1 << 8; /* IMODE[1:0] = 1, 单线传输指令 */
    tempreg |= 3 << 10; /* ADDRESS[1:0] = 3, 四线传输地址 */
    tempreg |= (2 + ftype) << 12; /* ADSIZE[1:0] = 2/3,
    24 位(ftype = 0) / 32 位(ftype = 1)地址长度 */
    tempreg |= 3 << 14; /* ABMODE[1:0] = 3, 四线传输交替字节 */
    tempreg |= 0 << 16; /* ABSIZE[1:0] = 0, 8 位交替字节(M0~M7) */
    tempreg |= 4 << 18; /* DCYC[4:0] = 4, 4 个 dummy 周期 */
    tempreg |= 3 << 24; /* DMODE[1:0] = 3, 四线传输数据 */
    tempreg |= 3 << 26; /* FMODE[1:0] = 3, 内存映射模式 */
    QUADSPI->CCR = tempreg; /* 设置 CCR 寄存器 */
    /* 设置 QSPI FLASH 空间的 MPU 保护 */
    SCB->SHCSR &= ~(1 << 16); /* 禁止 MemManage */
    MPU->CTRL &= ~(1 << 0); /* 禁止 MPU */
    MPU->RNR = 0; /* 设置保护区域编号为 0(1~7 可以给其他内存用) */
    MPU->RBAR = 0X90000000; /* 基地址为 0X9000 000, 即 QSPI 的起始地址 */
    MPU->RASR = 0X0303002D; /* 设置保护参数(禁止共用, 允许 cache, 允许缓冲),
详见 MPU 实验解析 */
    MPU->CTRL = (1 << 2) | (1 << 0); /* 使能 PRIVDEFENA, 使能 MPU */
    SCB->SHCSR |= 1 << 16; /* 使能 MemManage */
}

/**
* @brief 时钟设置函数
* @param plln: PLL1 倍频系数(PLL 倍频), 取值范围: 4~512.
* @param pllm: PLL1 预分频系数(进 PLL 之前的分频), 取值范围: 2~63.
* @param pllp: PLL1 的 p 分频系数(PLL 之后的分频), 分频后作为系统时钟, 取值范围:
* 2~128.(且必 须是 2 的倍数)
* @param pllq: PLL1 的 q 分频系数(PLL 之后的分频), 取值范围: 1~128.
* @note
*
* Fvco: VCO 频率
* Fsys: 系统时钟频率, 也是 PLL1 的 p 分频输出时钟频率
* Fq: PLL1 的 q 分频输出时钟频率
* Fs: PLL 输入时钟频率, 可以是 HSI, CSI, HSE 等.
* Fvco = Fs * (plln / pllm);
* Fsys = Fvco / pllp = Fs * (plln / (pllm * pllp));
* Fq = Fvco / pllq = Fs * (plln / (pllm * pllq));
*
* 外部晶振为 25M 的时候, 推荐值:plln = 192, pllm = 5, pllp = 2, pllq = 4.
* 外部晶振为 8M 的时候, 推荐值:plln = 240, pllm = 2, pllp = 2, pllq = 4.
* 得到:Fvco = 8 * (240 / 2) = 960Mhz
* Fsys = pll1_p_ck = 960 / 2 = 480Mhz
* Fq = pll1_q_ck = 960 / 4 = 240Mhz
*
* H750 默认需要配置的频率如下:
* CPU 频率(rcc_c_ck) = sys_d1cpre_ck = 480Mhz
* rcc_aclk = rcc_hclk3 = 240Mhz
* AHB1/2/3/4(rcc_hclk1/2/3/4) = 240Mhz
* APB1/2/3/4(rcc_pclk1/2/3/4) = 120Mhz
* pll2_p_ck = (8 / 8) * 440 / 2) = 220Mhz
* pll2_r_ck = FMC 时钟频率 = ((8 / 8) * 440 / 2) = 220Mhz
*
* @retval 错误代码: 0, 成功; 1, 错误;
*/
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef rcc_osc_init;
    RCC_ClkInitTypeDef rcc_clk_init;
    RCC_PeriphCLKInitTypeDef rcc_periph_clk_init;
    /*SCUEN = 0, 锁定 LDOEN 和 BYPASS 位的设置 */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
    /* VOS = 0, Scale0, 1.35V 内核电压，使用最高主频 480MHz */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY); /* 等待电压稳定 */
    /* 使能 HSE，并选择 HSE 作为 PLL 时钟源，配置 PLL1，开启 USB 时钟 */
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE
        | RCC_OSCILLATORTYPE_HSI48;
    rcc_osc_init.HSEState = RCC_HSE_ON;
    rcc_osc_init.HSIState = RCC_HSI_OFF;
    rcc_osc_init.CSIState = RCC_CSI_OFF;
    rcc_osc_init.HSI48State = RCC_HSI48_ON;
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    rcc_osc_init.PLL.PLLN = plln;
    rcc_osc_init.PLL.PLLM = pllm;
    rcc_osc_init.PLL.PLLP = pllp;
    rcc_osc_init.PLL.PLLQ = pllq;
    rcc_osc_init.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    rcc_osc_init.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    rcc_osc_init.PLL.PLLFRACN = 0;
    ret = HAL_RCC_OscConfig(&rcc_osc_init);
    if (ret != HAL_OK)
    {
        return 1;
    }
    /*
    * 选择 PLL 的输出作为系统时钟
    * 配置 RCC_CLOCKTYPE_SYSCLK 系统时钟,480M
    * 配置 RCC_CLOCKTYPE_HCLK 时钟,240Mhz,对应 AHB1， AHB2， AHB3 和 AHB4 总线
    * 配置 RCC_CLOCKTYPE_PCLK1 时钟,120Mhz,对应 APB1 总线
    * 配置 RCC_CLOCKTYPE_PCLK2 时钟,120Mhz,对应 APB2 总线
    * 配置 RCC_CLOCKTYPE_D1PCLK1 时钟,120Mhz,对应 APB3 总线
    * 配置 RCC_CLOCKTYPE_D3PCLK1 时钟,120Mhz,对应 APB4 总线
    */
    rcc_clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK \
        | RCC_CLOCKTYPE_HCLK \
        | RCC_CLOCKTYPE_PCLK1 \
        | RCC_CLOCKTYPE_PCLK2 \
        | RCC_CLOCKTYPE_D1PCLK1 \
        | RCC_CLOCKTYPE_D3PCLK1);
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    rcc_clk_init.SYSCLKDivider = RCC_SYSCLK_DIV1;
    rcc_clk_init.AHBCLKDivider = RCC_HCLK_DIV2;
    rcc_clk_init.APB1CLKDivider = RCC_APB1_DIV2;
    rcc_clk_init.APB2CLKDivider = RCC_APB2_DIV2;
    rcc_clk_init.APB3CLKDivider = RCC_APB3_DIV2;
    rcc_clk_init.APB4CLKDivider = RCC_APB4_DIV2;
    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_4);
    if (ret != HAL_OK)
    {
        return 1;
    }
    /*
    * 配置 PLL2 的 R 分频输出, 为 220Mhz
    * 配置 FMC 时钟源是 PLL2R
    * 配置 QSPI 时钟源是 PLL2R
    * 配置串口 1 和 串口 6 的时钟源来自: PCLK2 = 120Mhz
    * 配置串口 2 / 3 / 4 / 5 / 7 / 8 的时钟源来自: PCLK1 = 120Mhz
    * USB 工作需要 48MHz 的时钟,可以由 PLL1Q,PLL3Q 和 HSI48 提供,这里配置时钟源是 HSI48
    */
    rcc_periph_clk_init.PeriphClockSelection = RCC_PERIPHCLK_USART2
        | RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USB
        | RCC_PERIPHCLK_QSPI | RCC_PERIPHCLK_FMC;
    rcc_periph_clk_init.PLL2.PLL2M = 8;
    rcc_periph_clk_init.PLL2.PLL2N = 440;
    rcc_periph_clk_init.PLL2.PLL2P = 2;
    rcc_periph_clk_init.PLL2.PLL2Q = 2;
    rcc_periph_clk_init.PLL2.PLL2R = 2;
    rcc_periph_clk_init.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
    rcc_periph_clk_init.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    rcc_periph_clk_init.PLL2.PLL2FRACN = 0;
    rcc_periph_clk_init.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
    rcc_periph_clk_init.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
    rcc_periph_clk_init.Usart234578ClockSelection =
        RCC_USART234578CLKSOURCE_D2PCLK1;
    rcc_periph_clk_init.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    rcc_periph_clk_init.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    ret = HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_init);
    if (ret != HAL_OK)
    {
        return 1;
    }
    sys_qspi_enable_memmapmode(0); /* 使能 QSPI 内存映射模式, FLASH 容量为普通类型 */
    HAL_PWREx_EnableUSBVoltageDetector(); /* 使能 USB 电压电平检测器 */
    __HAL_RCC_CSI_ENABLE(); /* 使能 CSI 时钟 */
    __HAL_RCC_SYSCFG_CLK_ENABLE(); /* 使能 SYSCFG 时钟 */
    HAL_EnableCompensationCell(); /* 使能 IO 补偿单元 */
    return 0;
}