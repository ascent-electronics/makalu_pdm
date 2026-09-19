//
// Created by Finn Carmichael on 4/19/26.
//
#include "main_app.h"
#include "cmsis_os2.h"

extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c1;

void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN 5 */
    main_app_init(&hcan1, &hi2c1);
    /* Infinite loop */
    for(;;)
    {
        main_app_run();
        osDelay(1);
    }
    /* USER CODE END 5 */
}