#ifndef __DRV_TIM_H
#define __DRV_TIM_H
#if 1
#include "stm32h7xx_hal.h"
#include "stdio.h"

extern uint8_t g_timxchy_cap_sta;      /*输入捕获状态*/
extern uint16_t g_timxchy_cap_val;     /*输入捕获值*/

void TIMx_Int_Init(uint16_t arr, uint16_t psc);
void TIMx_InputCapture_Init(uint16_t arr, uint16_t psc);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim);

#define GTIM_TIMX_CAP_CHY                      TIM_CHANNEL_1     /* Í¨µÀY,  1<= Y <=4 */
#define GTIM_TIMX_CAP                          TIM5    
#define GTIM_TIMX_CAP_IRQHandler               TIM5_IRQHandler

#else

#include "stm32h7xx_hal.h"
extern uint8_t g_timxchy_cap_sta;      /*输入捕获状态*/
extern uint16_t g_timxchy_cap_val;     /*输入捕获值*/

#define GTIM_TIMX_CAP_CHY_GPIO_PORT            GPIOA
#define GTIM_TIMX_CAP_CHY_GPIO_PIN             GPIO_PIN_0
#define GTIM_TIMX_CAP_CHY_GPIO_AF              GPIO_AF2_TIM5                                 /* AF¹¦ÄÜÑ¡Ôñ */
#define GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA¿ÚÊ±ÖÓÊ¹ÄÜ */

#define GTIM_TIMX_CAP                          TIM5                       
#define GTIM_TIMX_CAP_IRQn                     TIM5_IRQn
#define GTIM_TIMX_CAP_IRQHandler               TIM5_IRQHandler
#define GTIM_TIMX_CAP_CHY                      TIM_CHANNEL_1     /* Í¨µÀY,  1<= Y <=4 */
#define GTIM_TIMX_CAP_CHY_CCRX                 TIM5->CCR1        /* Í¨µÀYµÄÊä³ö±È½Ï¼Ä´æÆ÷ */
#define GTIM_TIMX_CAP_CHY_CLK_ENABLE()         do{ __HAL_RCC_TIM5_CLK_ENABLE(); }while(0)    /* TIMX Ê±ÖÓÊ¹ÄÜ */

void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc);  /* Í¨ÓÃ¶¨Ê±Æ÷ ÊäÈë²¶»ñ³õÊ¼»¯º¯Êý */
#endif

#endif // !__DRV_TIM_H
