#ifndef __DRV_LCD_H
#define __DRV_LCD_H

#include "stm32h7xx_hal.h"
#include "drv_delay.h"
#include "drv_usart.h"

/* RESET和系统复位脚共用 所以这里不用定义RESET引脚 */
//#define LCD_RST_GPIO_PORT GPIOX
//#define LCD_RST_GPIO_PIN
//#define LCD_RST_GPIO_CLK_ENABLE() do{__HAL_RCC_GPIOx_CLK_ENABLE(); }while (0)

#define LCD_WR_GPIO_PORT    GPIOD
#define LCD_WR_GPIO_PIN     GPIO_PIN_5
#define LCD_WR_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define LCD_RD_GPIO_PORT    GPIOD
#define LCD_RD_GPIO_PIN     GPIO_PIN_4
#define LCD_RD_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define LCD_BL_GPIO_PORT    GPIOB
#define LCD_BL_GPIO_PIN     GPIO_PIN_5
#define LCD_BL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define LCD_CS_GPIO_PORT    GPIOD
#define LCD_CS_GPIO_PIN     GPIO_PIN_7
#define LCD_CS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define LCD_RS_GPIO_PORT    GPIOE
#define LCD_RS_GPIO_PIN     GPIO_PIN_3
#define LCD_RS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

#define LCD_FMC_NEX 1   /* 使用FMC_NE1接LCD_CS，取值范围是1-4 */
#define LCD_FMC_AX 19   /* 使用FMC_A19接LCD_RS，取值范围是0-25 */

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint16_t id;
    uint8_t dir;
    uint16_t wramcmd;
    uint16_t setxcmd;
    uint16_t setycmd;
}_lcd_dev;

extern _lcd_dev lcddev;
/* LCD地址结构体 */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
}LCD_TypeDef;

/* LCD地址的详细解算方法 */
/* LCD -> LCD_RAM的地址为 0x6000 0000 + 2^19 * 2 = 0x6010 0000 */
/* LCD -> LCD_REG的地址可以是LCD->LCD_RAM之外的任意地址*/
#define LCD_BASE (uint32_t)((0x60000000 + (0x40000000)*(LCD_FMC_NEX-1))|(((1<<LCD_FMC_AX)*2)-2))
#define LCD ((LCD_TypeDef *)LCD_BASE)

/* LCD扫描方向和颜色定义 */
/* 扫描方向定义 */
#define L2R_U2D 0 /*从左到右，从上到下*/
#define L2R_D2U 1 /*从左到有，从下到上*/
#define R2L_U2D 2
#define R2L_D2U 3

#define U2D_L2R 4
#define U2D_R2L 5
#define D2U_L2R 6
#define D2U_R2L 7

#define DFT_SCAN_DIR L2R_U2D    /*默认的扫描方向*/

/* 常用的画笔颜色 */
#define WHITE 0xffff
#define BLACK 0x0000
#define RED 0xf800
#define GREEN 0x07e0
#define BLUE 0x001f
#define MAGENTA 0xFFE0  /*品红色 紫红色 = BULE + RED*/
#define YELLOW 0xFFE0   /*黄色 = GREEN + RED*/
#define CYAN   0x07ff   /*青色 = GREEN + BLUE*/

/*非常用颜色*/
#define BROWN           0XBC40  /*棕色*/
#define BRRED           0XFC07  /*棕红色*/
#define GRAY            0X8430  /*灰色*/
#define DARKBLUE        0X01CF  /*深蓝色*/
#define LIGHTBLUE       0X7D7C  /*浅蓝色*/
#define GRAYBLUE        0X5458  /*灰蓝色*/
#define LIGHTGREEN      0X841F  /*浅灰色*/
#define LGRAY           0XC618  /*浅灰色(PANNEL),窗体背景黑色*/
#define LGRAYBLUE       0XA651  /*浅灰蓝色(中间层颜色)*/
#define LBBLUE          0X2B12  /*浅棕蓝色(选择条目的反色)*/

/*nt35510相关的配置参数*/
#define SSD_HOR_RESOLUTION  800     /*LCD水平分辨率*/
#define SSD_VER_RESOLUTION  480     /*LCD垂直分辨率*/

/*LCD驱动参数设置*/
#define SSD_HOR_PULSE_WIDTH 1       /*水平脉宽*/
#define SSD_HOR_BACK_PORCH  46      /*水平前廊*/
#define SSD_HOR_FRONT_PORCH 210     /*水平后廊*/

#define SSD_VER_PULSE_WIDTH 1       /*垂直脉宽*/
#define SSD_VER_BACK_PORCH  23      /*垂直前廊*/
#define SSD_VER_FRONT_PORCH 22      /*垂直后廊*/

/* 如下几个参数自动计算 */
#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS SSD_HOR_BACK_PORCH
#define SSD_VT (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS SSD_VER_BACK_PORCH




/* 用hal库方式控制端口引脚 */
#define LCD_WR(x)       {x ? \
                            HAL_GPIO_WritePin(LCD_WR_GPIO_PORT,LCD_WR_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(LCD_WR_GPIO_PORT,LCD_WR_GPIO_PIN,GPIO_PIN_RESET);}
#define LCD_BL(x)       {x ? \
                            HAL_GPIO_WritePin(LCD_BL_GPIO_PORT,LCD_BL_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(LCD_BL_GPIO_PORT,LCD_BL_GPIO_PIN,GPIO_PIN_RESET);}
#define LCD_RD(x)       {x ? \
                            HAL_GPIO_WritePin(LCD_RD_GPIO_PORT,LCD_RD_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(LCD_RD_GPIO_PORT,LCD_RD_GPIO_PIN,GPIO_PIN_RESET);}
#define LCD_CS(x)       {x ? \
                            HAL_GPIO_WritePin(LCD_CS_GPIO_PORT,LCD_CS_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(LCD_CS_GPIO_PORT,LCD_CS_GPIO_PIN,GPIO_PIN_RESET);}
#define LCD_RS(x)       {x ? \
                            HAL_GPIO_WritePin(LCD_RS_GPIO_PORT,LCD_RS_GPIO_PIN,GPIO_PIN_SET):\
                            HAL_GPIO_WritePin(LCD_RS_GPIO_PORT,LCD_RS_GPIO_PIN,GPIO_PIN_RESET);}

void lcd_init(void);
//void lcd_wr_regno(uint16_t regno);

void lcd_clear(uint16_t color);
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_ex_nt35510_reginit(void);
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char* p, uint16_t color);

void load_draw_dialog(void);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color);


#endif // !__DRV_LCD_H
