#include "alltask.h"

TaskHandle_t task1_handle;
void task1(void* pvParameters)
{
    LED_Config();
    while (1)
    {
        HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_SET);
        vTaskDelay(1000);
        HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_RESET);
        // HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET);
        // HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_GPIO_PIN, GPIO_PIN_RESET);
        vTaskDelay(1000);
    }
}

TaskHandle_t beep_handle;
void beep_task(void* pvParameters)
{
    // BEEP_PWM_Init(240 - 1, 249);
    while (1)
    {
        BEEP_SETPSC(100);       /*频率261*/
        vTaskDelay(2000);
        BEEP_SETPSC(2000);       /*频率523.264*/
        vTaskDelay(2000);
        BEEP_SETPSC(10000);       /*频率2093*/
        vTaskDelay(2000);
    }
}

TaskHandle_t LCD_handle;
void LCD_Task(void* pvParameters)
{

    while (1)
    {
    }
}

TaskHandle_t Touch_handle;
void Touch_Task(void* pvParameters)
{
    tp_dev.init();        /*触摸初始化*/
    load_draw_dialog();
    ctp_test();
    while (1)
    {
    }
}

TaskHandle_t Remote_handle;
void Remote_Task(void* pvParameters)
{
    uint8_t key = 0;
    char* str = 0;
    remote_init();
    while (1)
    {
        key = remote_scan();
        if (key)
        {
            switch (key)
            {
            case 0:
                str = "ERROR";
                break;

            case 69:
                str = "POWER";
                break;

            case 70:
                str = "UP";
                break;

            case 64:
                str = "PLAY";
                break;

            case 71:
                str = "ALIENTEK";
                break;

            case 67:
                str = "RIGHT";
                break;

            case 68:
                str = "LEFT";
                break;

            case 7:
                str = "VOL-";
                break;

            case 21:
                str = "DOWN";
                break;

            case 9:
                str = "VOL+";
                break;

            case 22:
                str = "1";
                break;

            case 25:
                str = "2";
                break;

            case 13:
                str = "3";
                break;

            case 12:
                str = "4";
                break;

            case 24:
                str = "5";
                break;

            case 94:
                str = "6";
                break;

            case 8:
                str = "7";
                break;

            case 28:
                str = "8";
                break;

            case 90:
                str = "9";
                break;

            case 66:
                str = "0";
                break;

            case 74:
                str = "DELETE";
                break;
            }/* ÏÔÊ¾SYMBOL */
            printf("%s\n", str);
        }

    }
}

/**
 * @brief   所有任务
 * @param   无
 * @retval  无
*/
inline void All_TaskStart(void)
{
    xTaskCreate(task1, "task1", 128, NULL, 2, &task1_handle);
    xTaskCreate(beep_task, "beep_task", 128, NULL, 2, &beep_handle);
    xTaskCreate(LCD_Task, "LCD_Task", 128, NULL, 2, &LCD_handle);
    xTaskCreate(Touch_Task, "Touch_Task", 128, NULL, 2, &Touch_handle);
    xTaskCreate(Remote_Task, "Remote_Task", 128, NULL, 2, &Remote_handle);
}

