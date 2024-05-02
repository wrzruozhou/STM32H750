/**
 * @file    drv_gt9cc.c
 * @author  RuoZhou Wan
 * @version V1.0
 * @date    2024-4-28
 * @brief   GT911屏幕的驱动代码
 *  @note   可应用多种GT系列的屏幕驱动
 */

#include "string.h"
#include "drv_gt911.h"
#include "drv_usart.h"
#include "drv_delay.h"

/* GT911只支持5点触摸*/
uint8_t g_gt_num = 5;
/**
 * @brief   向gt9xxx写入一次数据
 * @param   reg :   起始寄存器地址
 * @param   buf :   数据缓冲区
 * @param   len :   写数据长度
 * @retval  0，成功 1，失败
 */
uint8_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	uint8_t ret = 0;
	ct_iic_start();
	ct_iic_send_byte(GT9XXX_CMD_WR);
	ct_iic_wait_ack();
	ct_iic_send_byte(reg >> 8); /*先发送高位*/
	ct_iic_wait_ack();
	ct_iic_send_byte(reg & 0xff);
	ct_iic_wait_ack();

	for (i = 0; i < len; i++)
	{
		ct_iic_send_byte(buf[i]);
		ret = ct_iic_wait_ack();
		if (ret)
			break;
	}
	ct_iic_stop();
	return ret;
}

/**
 * @brief   向gt9xxx读取一次数据
 * @param   reg:    起始寄存器
 * @param   buf:    要保存数据的位置
 * @param   len:    读数据的长度
 * @retval  无
 */
void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	ct_iic_start();
	ct_iic_send_byte(GT9XXX_CMD_WR);
	ct_iic_wait_ack();
	ct_iic_send_byte(reg >> 8);
	ct_iic_wait_ack();
	ct_iic_send_byte(reg & 0xff);
	ct_iic_wait_ack();

	ct_iic_start();
	ct_iic_send_byte(GT9XXX_CMD_RD);
	ct_iic_wait_ack();

	for (i = 0; i < 8; i++)
	{
		buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
	}
	ct_iic_stop();
}

/**
 * @brief   初始化GT911触摸屏
 * @param   无
 * @retval  0,初始化成功；1，初始化失败
 */
uint8_t gt9xxx_init(void)
{
	GPIO_InitTypeDef GPIO_Init;
	uint8_t temp[5];

	GT9XXX_INT_CLK_ENABLE();
	GT9XXX_RST_CLK_ENABLE();

	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pin = GT9XXX_RST_GPIO_PIN;
	GPIO_Init.Pull = GPIO_PULLUP;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GT9XXX_RST_GPIO_PORT, &GPIO_Init);
	;

	GPIO_Init.Mode = GPIO_MODE_INPUT;
	GPIO_Init.Pin = GT9XXX_INT_GPIO_PIN;
	HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &GPIO_Init);

	ct_iic_init(); /*初始化IIC时钟线*/
	GT9XXX_RST(0);
	delay_ms(10);
	GT9XXX_RST(1);
	delay_ms(10);

	/*INT引脚模式，输入模式，浮空输入*/
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &GPIO_Init);

	delay_ms(100);
	/*读取触摸屏的ID*/
	gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);
	temp[4] = 0;

	/*这段代码预留*/
	if (strcmp((char *)temp, "911") && strcmp((char *)temp, "9147") && strcmp((char *)temp, "1158") && strcmp((char *)temp, "9271"))
	{
		return 1;
	}
	printf("%s\n", temp);

	temp[0] = 0x2;
	gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1); /*软复位*/
	delay_ms(10);
	temp[0] = 0x0;
	gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1); /*开始读坐标*/
	return 0;
}

/* 对应寄存器表 */
const uint16_t GT9XXX_TPX_TBL[10] =
	{
		GT9XXX_TP1_REG,
		GT9XXX_TP2_REG,
		GT9XXX_TP3_REG,
		GT9XXX_TP4_REG,
		GT9XXX_TP5_REG,
		GT9XXX_TP6_REG,
		GT9XXX_TP7_REG,
		GT9XXX_TP8_REG,
		GT9XXX_TP9_REG,
		GT9XXX_TP10_REG,
};

/**
 * @brief   扫描触摸屏幕(采用查询的方式)
 * @param   mode:电容屏未用到此参数，为了兼容电阻屏
 * @retval  当前触摸屏状态
 *  @arg   0，屏幕无触摸
 *  @arg   1，屏幕有触摸
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i = 0;
	uint8_t res = 0;
	uint16_t temp;
	uint16_t tempsta;
	static uint8_t t = 0; /*控制查询间隔，从而降低CPU的占用率*/
	t++;

	if ((t % 10) == 0 || t < 10) /*空闲时,每10次CTP_Scan函数才检测一次，从而节省CPU的使用率*/
	{
		gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);
		if ((mode & 0x80) && ((mode & 0xf) <= g_gt_num))
		{
			i = 0;
			gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1); /*清除标志位*/
		}
		if ((mode & 0xf) && ((mode & 0xf) <= g_gt_num))
		{
			temp = 0xffff << (mode & 0xf); /*将点的个数转化为1的位数，匹配tp_dev.sta定义*/
			tempsta = tp_dev.sta;		   /*保存当前的tp_dev.sta的值*/
			tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
			tp_dev.x[g_gt_num - 1] = tp_dev.x[0]; /*保存触电0的数据，保存再最后一个上*/
			tp_dev.y[g_gt_num - 1] = tp_dev.y[0];

			for (i = 0; i < g_gt_num; i++)
			{
				if (tp_dev.sta & (1 << i)) /*触摸有效*/
				{
					gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);							   /*读取XY坐标值，连续读4次*/
					if (lcddev.id == 0x5510 || lcddev.id == 0x9806 || lcddev.id == 0x7796) /*4.3寸800*480和3.5寸480*320MCU屏幕*/
					{
						if (tp_dev.touchtype & 0x01) /*横屏*/
						{
							tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3] << 8) + buf[2]);
							tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
						}
						else
						{
							tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
							tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
						}
					}
					else
					{
						if (tp_dev.touchtype & 0x01) /*横屏*/
						{
							tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
							tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
						}
						else
						{
							tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3] << 8) + buf[2]);
							tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
						}
					}
					/*打印日志*/
					printf("x[%d]:%d,y[%d]:[%d]\r\n", i, tp_dev.x[i], i, tp_dev.y[i]);
				}
			}
			res = 1; /*为什么*/
			if (tp_dev.x[0] > lcddev.width || tp_dev.y[0] > lcddev.height)
			{
				if ((mode & 0xf) > 1) /*如果其他点有数据，则复制到这个地方*/
				{
					tp_dev.x[0] = tp_dev.x[1];
					tp_dev.y[0] = tp_dev.y[1];
					t = 0; /*触发一次则会最少连续检测10次，从而提高命中率*/
				}
				else /*非法数据，则忽略此数据*/
				{
					tp_dev.x[0] = tp_dev.x[g_gt_num - 1];
					tp_dev.y[0] = tp_dev.y[g_gt_num - 1];
					mode = 0x80;
					tp_dev.sta = tempsta;
				}
			}
			else
			{
				t = 0;
			}
		}
	}

	if ((mode & 0x8f) == 0x80) /*无触摸点按下*/
	{
		if (tp_dev.sta & TP_PRES_DOWN) /*之前是按下的*/
		{
			tp_dev.sta &= ~TP_PRES_DOWN; /*标记按键松开*/
		}
		else
		{
			tp_dev.x[0] = 0xffff;
			tp_dev.y[0] = 0xffff;
			tp_dev.sta &= 0xe000;
		}
	}
	if (t > 240)
		t = 10;
	return res;
}
