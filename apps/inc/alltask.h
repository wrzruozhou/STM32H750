#ifndef __ALLTASK_H
#define __ALLTASK_H

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "drv_delay.h"
#include "drv_led.h"
#include "drv_rcc.h"
#include "drv_usart.h"
#include "drv_key.h"
#include "drv_mpu.h"
#include "drv_lcd.h"
#include "drv_iic.h"
#include "drv_at24cxx.h"
#include "USMART\usmart.h"
#include "drv_TIM.h"
#include "drv_remote.h"
#include "drv_gt911.h"
#include "drv_touch.h"
#include "drv_norflash_ex.h"
#include "drv_beep.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"

#include "lv_demo_music.h"
#include "lv_demo_benchmark.h"
#include "lv_demo_keypad_encoder.h"
#include "lv_demo_stress.h"
#include "lv_demo_widgets.h"

#define FREE_RTOS 1

void task1(void* pvParameters);
void beep_task(void* pvParameters);
void All_TaskStart(void);

#endif // !__ALLTASK_H
