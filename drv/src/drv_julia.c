#include "drv_julia.h"



/*颜色表*/
uint16_t g_color_map[ITERATION];

/*缩放因子列表*/
const uint16_t zoom_ratio[26] = {
    120, 110,100,150,200,275,350,450,
    600,800,1000,1200,1500,2000,1500,
    1200,1000,800,600,450,350,275,200,
    150,100,110,
};

/**
 * @brief   初始化颜色表
 * @param   clut:颜色指针
 * @retval  无
 */
void julia_clut_init(uint16_t* clut)
{
    uint32_t i = 0x00;
    uint16_t red = 0, green = 0, blue = 0;

    for (i = 0; i < ITERATION; i++)
    {
        /*产生RGB颜色*/
        red = (i * 8 * 256 / ITERATION) % 256;
        green = (i * 6 * 256 / ITERATION) % 256;
        blue = (i * 4 * 256 / ITERATION) % 256;

        /*将RGB888,转换为RGB565*/
        red = red >> 3;
        red = red << 11;
        green = green >> 2;
        green = green << 5;
        blue = blue >> 3;
        clut[i] = red + green + blue;
    }
}

/*RGL LCD缓存 TFT未用上*/
uint16_t g_lcdbuf[800];

/**
 * @brief   产生Julia分析图形
 * @param   size_x:屏幕x方向上的尺寸
 * @param   size_y:屏幕y方向上的尺寸
 * @param   offset_x:屏幕x方向上的偏移
 * @param   offset_y:屏幕y方向上的偏移
 * @param   zoom:缩放因子
 * @retval  无
 */
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
    uint8_t i;
    uint16_t x, y;
    float tmp1, tmp2;
    float num_real, num_img;
    float radius;

    for (y = 0;y < size_y; y++)
    {
        for (x = 0; x < size_x; x++)
        {
            num_real = y - offset_y;
            num_real = num_real / zoom;
            num_img = x - offset_x;
            num_img = num_img / zoom;
            i = 0;
            radius = 0;

            while ((i < ITERATION - 1) && (radius < 4))
            {
                tmp1 = num_real * num_real;
                tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                radius = tmp1 + tmp2;
                i++;
            }
            LCD->LCD_RAM = g_color_map[i];      /*绘制到屏幕*/
        }
    }
}
