#include "drv_qspi.h"

/**
 * @brief   等待状态标志
 * @param   flag:需要等待的标志位
 * @param   sta:需要等待的状态
 * @param   wtime:等待时间
 * @retval  0:等待成功；1，等待失败
*/
uint8_t qspi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime)
{
    uint8_t flagsta = 0;
    while (wtime)
    {
        flagsta = (QUADSPI->SR & flag) ? 1 : 0;
        if (flagsta == sta)break;
        wtime--;
    }
    if (wtime)return 0;
    else return 1;
}

/**
 * @brief   初始化QSPI接口
 * @param   无
 * @param   0，成功，1，失败
*/
QSPI_HandleTypeDef qspi_wr;
uint8_t qspi_init(void)
{
    qspi_wr.Instance = QUADSPI;
    qspi_wr.Init.ClockPrescaler = 1;
    qspi_wr.Init.FifoThreshold = 4;
    qspi_wr.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    qspi_wr.Init.FlashSize = 25 - 1;
    qspi_wr.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE;
    qspi_wr.Init.ClockMode = QSPI_CLOCK_MODE_3;
    qspi_wr.Init.FlashID = QSPI_FLASH_ID_1;
    qspi_wr.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
    if (HAL_QSPI_Init(&qspi_wr) == HAL_OK)
        return 0;
    else
        return 1;
}

/**
 * @brief   QSPI底层驱动
 * @param   hqspi：QSPI句柄
 * @note    此函数会被HAL_QSPI_Init调用
 * @retval  无
*/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef* hqspi)
{
    GPIO_InitTypeDef GPIO_Init;
    __HAL_RCC_QSPI_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_Init.Alternate = QSPI_BK1_NCS_AF;
    GPIO_Init.Mode = GPIO_MODE_AF_PP;
    GPIO_Init.Pull = GPIO_PULLUP;
    GPIO_Init.Pin = QSPI_BK1_NCS_PIN;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(QSPI_BK1_NCS_PORT, &GPIO_Init);

    GPIO_Init.Alternate = QSPI_BK1_CLK_AF;
    GPIO_Init.Pin = QSPI_BK1_CLK_PIN;
    HAL_GPIO_Init(QSPI_BK1_CLK_PORT, &GPIO_Init);

    GPIO_Init.Pin = QSPI_BK1_IO0_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO0_PORT, &GPIO_Init);

    GPIO_Init.Pin = QSPI_BK1_IO1_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO1_PORT, &GPIO_Init);

    GPIO_Init.Pin = QSPI_BK1_IO2_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO2_PORT, &GPIO_Init);

    GPIO_Init.Pin = QSPI_BK1_IO3_PIN;
    HAL_GPIO_Init(QSPI_BK1_IO3_PORT, &GPIO_Init);
}

/**
 * @brief   QSPI:发送命令
 * @param   cmd:要发送的命令
 * @param   addr:要发送到的目的地址
 * @param   mode:模式
 *  @arg    mode[1:0]:指令模式; 00:无指令;  01:单线传输指令;    10:双线传输按指令;  11:四线传输指令.
 *  @arg    mode[3:2]:地址模式; 00:无地址;  01:单线传输地址;    10:双线传输按地址;  11:四线传输地址.
 *  @arg    mode[5:4]:地址长度; 00:8位;     01:16位;           10:24位;             11:32位.
 *  @arg    mode[7:6]:数据模式; 00:无数据;  01:单线传输数据;    10:双线传输按数据;  11:四线传输数据.
 * @param   dmcycle:空指令周期数
 * @retval  无
*/
void qspi_send_cmd(uint8_t cmd, uint16_t addr, uint8_t mode, uint8_t dmcycle)
{
    QSPI_CommandTypeDef hcmd;
    hcmd.Instruction = cmd;
    hcmd.Address = addr;
    hcmd.DummyCycles = dmcycle;

    if (((mode >> 0) & 0x03) == 0)
        hcmd.InstructionMode = QSPI_INSTRUCTION_NONE;
    else if (((mode >> 0) & 0x03) == 1)
        hcmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    else if (((mode >> 0) & 0x03) == 2)
        hcmd.InstructionMode = QSPI_INSTRUCTION_2_LINES;
    else if (((mode >> 0) & 0x03) == 3)
        hcmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;

    if (((mode >> 2) & 0x03) == 0)
        hcmd.AddressMode = QSPI_ADDRESS_NONE;
    else if (((mode >> 2) & 0x03) == 1)
        hcmd.AddressMode = QSPI_ADDRESS_1_LINE;
    else if (((mode >> 2) & 0x03) == 2)
        hcmd.AddressMode = QSPI_ADDRESS_2_LINES;
    else if (((mode >> 2) & 0x03) == 3)
        hcmd.AddressMode = QSPI_ADDRESS_4_LINES;

    if (((mode >> 4) & 0x03) == 0)
        hcmd.AddressSize = QSPI_ADDRESS_8_BITS;
    else if (((mode >> 4) & 0x03) == 1)
        hcmd.AddressSize = QSPI_ADDRESS_16_BITS;
    else if (((mode >> 4) & 0x03) == 2)
        hcmd.AddressSize = QSPI_ADDRESS_24_BITS;
    else if (((mode >> 4) & 0x03) == 3)
        hcmd.AddressSize = QSPI_ADDRESS_32_BITS;

    if (((mode >> 6) & 0x03) == 0)
        hcmd.DataMode = QSPI_DATA_NONE;
    else if (((mode >> 6) & 0x03) == 1)
        hcmd.DataMode = QSPI_DATA_1_LINE;
    else if (((mode >> 6) & 0x03) == 2)
        hcmd.DataMode = QSPI_DATA_2_LINES;
    else if (((mode >> 6) & 0x03) == 3)
        hcmd.DataMode = QSPI_DATA_4_LINES;

    hcmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    hcmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    hcmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    hcmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    HAL_QSPI_Command(&qspi_wr, &hcmd, 5000);
}

/**
 * @brief   QSPI接受指定长度的数据
 * @param   buf:接受数据缓冲区首地址
 * @param   datalen:要传输的数据长度
 * @retval  0:成功  1:失败
*/
uint8_t qspi_receive(uint8_t* buf, uint32_t datalen)
{
    qspi_wr.Instance->DLR = datalen - 1;
    if (HAL_QSPI_Receive(&qspi_wr, buf, 5000) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief   QSPI发送指定长度的数据
 * @param   buf:发送数据缓冲区首地址
 * @param   datalen:要传输的数据长度
 * @retval  0,成功;1,失败
*/
uint8_t qspi_transmit(uint8_t* buf, uint32_t datalen)
{
    qspi_wr.Instance->DLR = datalen - 1;
    if (HAL_QSPI_Transmit(&qspi_wr, buf, 5000) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}