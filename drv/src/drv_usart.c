#include "drv_usart.h"
/* ************************************************************************************************************************************ */

#if (__ARMCC_VERSION >= 6010050)            /* 使用A成编译时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时，要在这定义__FILE和不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重新定义å_ttywrch\_sys_exit\_sys_command_string函数，以同时兼任AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义sys_exit()避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char* _sys_command_string(char* cmd, int len)
{
    return NULL;
}

/* FILE在stdio.h里面定义 */
FILE __stdout;
/* ************************************************************************************************************************************ */


UART_HandleTypeDef g_uart1_handle;    /* UART¾ä±ú */
/* 最大缓冲区,接收SART_REC_LEN个字节 */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接受状态
 *  bit15，     接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;
uint8_t g_rx_buffer[RXBUFFERSIZE];    /* uart缓冲区 */
uint8_t g_rx_flag = 0;

void usart_init(uint32_t baudrate)
{
    g_uart1_handle.Instance = USART_UX; /* USART1 */
    g_uart1_handle.Init.BaudRate = baudrate; /* 波特率 */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B; /* 字长为 8 位数据格式 */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1; /* 一个停止位 */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE; /* 无奇偶校验位 */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX; /* 收发模式 */
    HAL_UART_Init(&g_uart1_handle); /* HAL_UART_Init()会使能 UART1 */
    /*该函数会开启接收中断：标志位 UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量*/
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t*)g_rx_buffer, RXBUFFERSIZE);
}

/**
* @brief UART 底层初始化函数
* @param huart: UART 句柄类型指针
* @note 此函数会被 HAL_UART_Init()调用
* 完成时钟使能，引脚配置，中断配置
* @retval 无
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if (huart->Instance == USART1) /* 如果是串口 1，进行串口 1 MSP 初始化 */
    {
        USART_UX_CLK_ENABLE(); /* USART1 时钟使能 */
        USART_TX_GPIO_CLK_ENABLE(); /* 发送引脚时钟使能 */
        USART_RX_GPIO_CLK_ENABLE(); /* 接收引脚时钟使能 */
        gpio_init_struct.Pin = USART_TX_GPIO_PIN; /* TX 引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP; /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP; /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF; /* 复用为 USART1 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct); /* 初始化发送引脚 */
        gpio_init_struct.Pin = USART_RX_GPIO_PIN; /* RX 引脚 */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF; /* 复用为 USART1 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct); /* 初始化接收引脚 */
#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn); /* 使能 USART1 中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3); /* 抢占优先级 3，子优先级 3 */
#endif
    }
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&g_uart1_handle);
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t*)g_rx_buffer, RXBUFFERSIZE);
}
/* 可以定制化你所最爱的中断回调函数了 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART1)
    {
        g_usart_rx_buf[g_usart_rx_sta] = g_rx_buffer[0];
        g_usart_rx_sta++;
        if (g_rx_buffer[0] == 0x0a)
        {
            g_rx_flag = 1;
        }
    }
}


#if 0
/* 重定义fputc函数，printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE* f)
{
    while ((USART_UX->ISR & 0X40) == 0);    /* 等待上一个字节发送完成 */

    USART_UX->TDR = (uint8_t)ch;            /* 要讲发送的字符存入寄存器 */
    return ch;
}
#else
int fputc(int ch, FILE* f)
{
    HAL_UART_Transmit(&g_uart1_handle, (uint8_t*)&ch, 1, 10);
    while (__HAL_UART_GET_FLAG(&g_uart1_handle, UART_FLAG_TC) != SET);
    __HAL_UART_CLEAR_FLAG(&g_uart1_handle, UART_FLAG_TC);
    return ch;
}
#endif