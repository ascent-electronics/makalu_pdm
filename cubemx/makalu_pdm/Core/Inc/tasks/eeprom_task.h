//
// Created by Finn Carmichael on 4/24/26.
//

#ifndef MAKALU_PDM_EEPROM_TASK_H
#define MAKALU_PDM_EEPROM_TASK_H

#include "eeprom_driver.h"
#include "cmsis_os2.h"
#include <stdint.h>

typedef enum {
    EEPROM_CMD_WRITE_BYTE,
    EEPROM_CMD_WRITE_BUFFER,
    EEPROM_CMD_READ_BYTE,
    EEPROM_CMD_READ_BUFFER,
} eeprom_cmd_type_t;

typedef struct {
    eeprom_cmd_type_t  type;
    uint16_t           addr;
    uint8_t            data[8];
    uint8_t           *ext_buf;
    uint16_t           len;
} eeprom_cmd_t;

void       eeprom_task_init      (eeprom_driver_t *driver);
void       eeprom_task_run       (void *argument);

osStatus_t eeprom_task_write_byte  (uint16_t addr, uint8_t data);
osStatus_t eeprom_task_write_buffer(uint16_t addr, const uint8_t *buf, uint16_t len);
osStatus_t eeprom_task_read_byte   (uint16_t addr, uint8_t *data);
osStatus_t eeprom_task_read_buffer (uint16_t addr, uint8_t *buf, uint16_t len);

#endif //MAKALU_PDM_EEPROM_TASK_H
