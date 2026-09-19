//
// Created by Finn Carmichael on 4/24/26.
//

#include "eeprom_task.h"
#include <string.h>

static eeprom_driver_t   *s_driver    = NULL;
static osMessageQueueId_t s_queue     = NULL;

/* ── Step 3a — init: store driver, create queue ─────────────────────────── */
void eeprom_task_init(eeprom_driver_t *driver) {
    s_driver = driver;
    s_queue  = osMessageQueueNew(16, sizeof(eeprom_cmd_t), NULL);
}

/* ── Step 3b — task: blocks on queue, executes commands ─────────────────── */
void eeprom_task_run(void *argument) {
    (void)argument;
    eeprom_cmd_t cmd;

    for (;;) {
        /* sleeps here — zero CPU — until a command arrives */
        if (osMessageQueueGet(s_queue, &cmd, NULL, osWaitForever) != osOK) continue;

        switch (cmd.type) {
            case EEPROM_CMD_WRITE_BYTE:
                s_driver->write_byte(s_driver->ctx,
                                     cmd.addr,
                                     cmd.data[0]);
                break;

            case EEPROM_CMD_WRITE_BUFFER:
                s_driver->write(s_driver->ctx,
                                cmd.addr,
                                cmd.ext_buf ? cmd.ext_buf : cmd.data,
                                cmd.len);
                break;

            case EEPROM_CMD_READ_BYTE:
                s_driver->read_byte(s_driver->ctx,
                                    cmd.addr,
                                    cmd.ext_buf);
                break;

            case EEPROM_CMD_READ_BUFFER:
                s_driver->read(s_driver->ctx,
                               cmd.addr,
                               cmd.ext_buf,
                               cmd.len);
                break;
        }
    }
}

/* ── Step 3c — post helpers: callable from any task ─────────────────────── */
osStatus_t eeprom_task_write_byte(uint16_t addr, uint8_t data) {
    eeprom_cmd_t cmd = {
        .type    = EEPROM_CMD_WRITE_BYTE,
        .addr    = addr,
        .data[0] = data,
        .ext_buf = NULL,
        .len     = 1,
    };
    return osMessageQueuePut(s_queue, &cmd, 0, 10);
}

osStatus_t eeprom_task_write_buffer(uint16_t addr, const uint8_t *buf, uint16_t len) {
    eeprom_cmd_t cmd = {
        .type    = EEPROM_CMD_WRITE_BUFFER,
        .addr    = addr,
        .ext_buf = (uint8_t *)buf,
        .len     = len,
    };
    /* small payloads copy inline so caller stack doesnt need to stay alive */
    if (len <= sizeof(cmd.data)) {
        memcpy(cmd.data, buf, len);
        cmd.ext_buf = NULL;
    }
    return osMessageQueuePut(s_queue, &cmd, 0, 10);
}

osStatus_t eeprom_task_read_byte(uint16_t addr, uint8_t *data) {
    eeprom_cmd_t cmd = {
        .type    = EEPROM_CMD_READ_BYTE,
        .addr    = addr,
        .ext_buf = data,
        .len     = 1,
    };
    return osMessageQueuePut(s_queue, &cmd, 0, 10);
}

osStatus_t eeprom_task_read_buffer(uint16_t addr, uint8_t *buf, uint16_t len) {
    eeprom_cmd_t cmd = {
        .type    = EEPROM_CMD_READ_BUFFER,
        .addr    = addr,
        .ext_buf = buf,
        .len     = len,
    };
    return osMessageQueuePut(s_queue, &cmd, 0, 10);
}