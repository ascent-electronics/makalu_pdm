#include "main_app.h"

#include <string.h>

#include "at24c02.h"
#include "at24c02_adapter.h"
#include "can_task.h"
#include "cdg_session.h"
#include "makalu_hal.h"
#include "makalu_bxcan.h"
#include "makalu_fdcan_ids.h"
#include "eeprom_task.h"
#include "PDM_EEPROM_REGISTERS.h"
#include "sys_rxcb.h"
#include "sys_txcan.h"

static eeprom_driver_t g_eeprom_drv;


// define tasks
static const osThreadAttr_t eeprom_attr = {
    .name       = "EEPROM",
    .stack_size = 256,
    .priority   = osPriorityBelowNormal,
};

const osThreadAttr_t can_tx_attr = {
    .name       = "can_tx",
    .stack_size = 512,
    .priority   = osPriorityHigh,
};

const osThreadAttr_t can_rx_attr = {
    .name       = "can_rx",
    .stack_size = 512,
    .priority   = osPriorityHigh,
};

const osThreadAttr_t can_tx_heartbeat_attr = {
    .name       = "can_tx_heartbeat",
    .stack_size = 256,
    .priority   = osPriorityLow,
};

void spawn_tasks() {
    osThreadNew(eeprom_task_run, NULL, &eeprom_attr);
    osThreadNew(can_tx_task, NULL, &can_tx_attr);
    osThreadNew(can_rx_task, NULL, &can_rx_attr);
    osThreadNew(sendHeartbeat, NULL, &can_tx_heartbeat_attr);
}


void registerIncomingCanCallbacks() {
    makalu_bxcan_register(MAKALU_MSG_COMMISSIONED, isControlUnitCommissioned);
}


void main_app_init(CAN_HandleTypeDef *hcan1, I2C_HandleTypeDef *hi2c1) {

    // setup canbus peripheral / init
    makalu_bxcan_init(hcan1);

    // setup eeprom driver
    at24c02_adapter_init(&g_eeprom_drv, hi2c1);

    // init eeprom task with driver
    eeprom_task_init(&g_eeprom_drv);

    // register cbs
    registerIncomingCanCallbacks();
    cdg_session_init();
    makalu_bxcan_register(MAKALU_MSG_CDG_ANNOUNCE,    cdg_session_on_announce);
    makalu_bxcan_register(MAKALU_MSG_CDG_READ,        cdg_session_on_read);
    makalu_bxcan_register(MAKALU_MSG_CDG_WRITE,       cdg_session_on_write);
    makalu_bxcan_register(MAKALU_MSG_CDG_WRITE_CHUNK, cdg_session_on_write_chunk);
    makalu_bxcan_register(MAKALU_MSG_CDG_WRITE_END,   cdg_session_on_write_end);

    // spawn all tasks
    spawn_tasks();

}

eeprom_driver_t *pdmCore_getEEPROMDRV(void)
{
    return &g_eeprom_drv;
}

// called from default task
void main_app_run(void)
{

    /* send a heartbeat every 1000ms */
}