#include "dsp_demo.h"

/**
 * @brief   sin cos测试
 * @param   angle:起始角度
 * @param   times:运算次数
 * @param   mode:是否使用dsp库
 *  @arg    0,不使用DSP库
 *  @arg    1，使用DSP库
 *
 * @retval  无
 */
uint8_t sin_cos_test(float angle, uint32_t times, uint8_t mode)
{
    float sinx, cosx;
    float result;
    uint32_t i = 0;
    if (mode == 0)
    {
        for (i = 0; i < times; i++)
        {
            cosx = cosf(angle);         /*不使用DSP优化的sin, cos函数*/
            sinx = sinf(angle);

            result = sinx * sinx + cosx * cosx;     /*计算结果应该等于1*/
            result = fabsf(result - 1.0f);          /*计算对比与1的差值*/

            if (result > DELTA)
                return 0XFF;

            angle += 0.001f;                        /*角度自增*/
        }
    }
    else
    {
        for (i = 0; i < times; i++)
        {
            cosx = arm_cos_f32(angle);                  /*使用DSP优化的sin,cos函数*/
            sinx = arm_sin_f32(angle);

            result = sinx * sinx + cosx * cosx;     /*计算结果应该等于1*/
            result = fabsf(result - 1.0f);          /*计算对比与1的差值*/

            if (result > DELTA)
                return 0XFF;

            angle += 0.001f;                        /*角度自增*/
        }
    }
    return 0;                                       /*任务完成*/
}
