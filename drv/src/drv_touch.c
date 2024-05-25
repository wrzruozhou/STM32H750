#include "drv_touch.h"

_m_tp_dev tp_dev = {
	tp_init,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

/**
 * @brief   触摸屏初始化
 * @param   无
 * @retval  0，没有进行校准
 *          1，进行过校准
 */
uint8_t tp_init(void)
{
	tp_dev.touchtype = 0;				   /*默认设置：电阻屏 竖屏*/
	tp_dev.touchtype |= lcddev.dir & 0x01; /*根据LCD判定是横屏还是竖屏*/
	gt9xxx_init();
	tp_dev.scan = gt9xxx_scan;
	tp_dev.touchtype |= 0x80;
	return 0;
}

/*10个触控点的颜色(电容屏用)*/
const uint16_t POINT_COLOR_TBL[10] = { RED, GREEN, BLUE, BROWN, YELLOW, MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY };
/**
 * @brief   电容触摸屏测试函数
 * @param   无
 * @param   无
 */
void ctp_test(void)
{
	uint8_t t = 0;
	uint8_t i = 0;
	uint16_t lastpos[10][2]; /*最后一次数据*/
	uint8_t maxp = 5;

	while (1)
	{
		tp_dev.scan(0);
		for (t = 0; t < maxp; t++)
		{
			if ((tp_dev.sta) & (1 << t))
			{
				if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height) /*坐标在屏幕的范围内*/
				{
					if (lastpos[t][0] == 0xffff)

					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]); /*画线*/
					lastpos[t][0] = tp_dev.x[t];
					lastpos[t][1] = tp_dev.y[t];

					if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)
					{
						load_draw_dialog();
					}
				}
			}
			else
			{
				lastpos[t][0] = 0xffff;
			}
		}
#if FREE_RTOS
		vTaskDelay(10);
#else
		delay_ms(10);
		i++;
		if (i % 20 == 0)
		{
			HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN);
			HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN);
			HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_GPIO_PIN);
		}
#endif

	}
}
