#ifndef __DRV_TOUCH_H
#define __DRV_TOUCH_H

#include "stm32h7xx_hal.h"
#include "drv_gt911.h"
#include "drv_lcd.h"
#include "drv_led.h"

#define TP_PRES_DOWN    0x8000  /*触摸屏被按下*/
#define TP_CATH_PRES    0x4000  /*有按键按下了*/
#define CT_MAX_TOUCH    10       /*电容屏支持的点数默认为5*/

/*触摸屏控制器*/
typedef struct
{
    uint8_t(*init)(void);           /*初始化触摸屏控制器*/
    uint8_t(*scan)(uint8_t);        /*扫描触摸屏 0：屏幕扫描 1：物理坐标*/
    void(*adjust)(void);            /*触摸屏校准*/
    uint16_t x[CT_MAX_TOUCH];       /*当前坐标*/
    uint16_t y[CT_MAX_TOUCH];       /*电容屏最多有10组坐标，电阻屏用x[0],y[0]代表：此次扫描时，触屏的坐标，用x[9,y[9]存储第一次按下时的坐标]*/

    uint16_t sta;                   /**笔的状态
                                    *   b15:按下1/松开0
                                    *   b14:0：没有按键按下；1：有按键按下
                                    *   b13~b10：保留
                                    *   b9~b0:电容触摸屏按下的点数(0表示未按下，1表示按下)
                                    */
                                    /*5点校准触摸屏校准参数(电容屏不需要校准)*/
    float xfac;     /*5点校准法x方向比例英子*/
    float yfac;     /*5点校准法y方向比例英子*/
    short xc;       /*中心X坐标物理值(AD值)*/
    short yc;       /*中心y坐标物理值(AD)*/

    /*新增的参数,当触摸屏的左右上下完全颠倒时要用到
    * b0:0  竖屏(适合左右未x坐标，上下为y坐标的TP)
    *    1  横屏
    * b1~6  保留
    * b7    :0电阻屏
    *       :1电容屏
    */
    uint8_t touchtype;
} _m_tp_dev;

extern _m_tp_dev tp_dev;

uint8_t tp_init(void);
void ctp_test(void);


#endif // !__DRV_TOUCH_H
