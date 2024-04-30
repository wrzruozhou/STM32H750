#ifndef __DRV_GT911_H
#define __DRV_GT911_H

#include "stm32h7xx_hal.h"
#include "drv_tciic.h"
#include "drv_touch.h"
#include "drv_lcd.h"

#define GT9XXX_RST_GPIO_PORT    GPIOC
#define GT9XXX_RST_GPIO_PIN     GPIO_PIN_5
#define GT9XXX_RST_CLK_ENABLE()   __HAL_RCC_GPIOC_CLK_ENABLE()

#define GT9XXX_INT_GPIO_PORT    GPIOB
#define GT9XXX_INT_GPIO_PIN     GPIO_PIN_1
#define GT9XXX_INT_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

/* 与电容触摸屏连接的芯片引脚(未包含IIC引脚) */
#define GT9XXX_RST(x)   {x?\
                            HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT,GT9XXX_RST_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_RESET);}

#define GT9XXX_INT      HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_PORT,GT9XXX_INT_GPIO_PIN)

/* IIC读写命令 */           //具体要看芯片的数据手册
#define GT9XXX_CMD_WR       0x28    //INT电平为高时为   0x28    否则为  0xba
#define GT9XXX_CMD_RD       0x29    //INT电平为高时为   0x29    否则为  0xbb

/* GT9XXX 部分寄存器定义 */
#define GT9XXX_CTRL_REG     0x8040          /*GT9XXX控制寄存器地址*/
#define GT9XXX_CFGS_REG     0x8047          /*GT9XXX配置起始地址寄存器*/
#define GT9XXX_CHECK_REG    0X80FF      /* GT9XXX校验和寄存器*/
#define GT9XXX_PID_REG      0X8140      /* GT9XXX产品ID寄存器*/

#define GT9XXX_GSTID_REG    0X814E      /* GT9XXX当前检测到的触摸情况 */
#define GT9XXX_TP1_REG      0X8150      /* 第一个触摸点数据地址 */
#define GT9XXX_TP2_REG      0X8158      /* 第二个触摸点数据地址 */
#define GT9XXX_TP3_REG      0X8160      /* 第三个触摸点数据地址 */
#define GT9XXX_TP4_REG      0X8168      /* 第四个触摸点数据地址 */
#define GT9XXX_TP5_REG      0X8170      /* 第五个触摸点数据地址 */
#define GT9XXX_TP6_REG      0X8178      /* 第六个触摸点数据地址 */
#define GT9XXX_TP7_REG      0X8180      /* 第七个触摸点数据地址 */
#define GT9XXX_TP8_REG      0X8188      /* 第八个触摸点数据地址 */
#define GT9XXX_TP9_REG      0X8190      /* 第九个触摸点数据地址 */
#define GT9XXX_TP10_REG     0X8198      /* 第十个触摸点数据地址 */

uint8_t gt9xxx_init(void);
uint8_t gt9xxx_scan(uint8_t mode);

#endif // !__DRV_GT911_H


