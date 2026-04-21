#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "makalu_hal.h"

void main_app_init(CAN_HandleTypeDef *hcan1, I2C_HandleTypeDef *hi2c1);
void main_app_run(void);

#endif