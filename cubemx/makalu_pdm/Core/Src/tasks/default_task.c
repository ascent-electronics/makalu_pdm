//
// Created by Finn Carmichael on 4/19/26.
//
#include "main_app.h"
#include "cmsis_os2.h"
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN 5 */
    /* Infinite loop */
    for(;;)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        main_app_run();
        osDelay(1);
    }
    /* USER CODE END 5 */
}