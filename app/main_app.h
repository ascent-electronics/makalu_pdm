#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "eeprom_driver.h"
#include "makalu_hal.h"


void main_app_init(CAN_HandleTypeDef *hcan1, I2C_HandleTypeDef *hi2c1);
void main_app_run(void);

eeprom_driver_t *pdmCore_getEEPROMDRV(void);


#endif