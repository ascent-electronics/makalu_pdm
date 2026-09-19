//
// Created by Finn Carmichael on 4/26/26.
//

#include "sys_rxcb.h"

#include <string.h>


#include "eeprom_driver.h"
#include "main_app.h"
#include "makalu_bxcan.h"
#include "makalu_fdcan_frames.h"
#include "PDM_EEPROM_EXPECTED_MAP.h"
#include "PDM_EEPROM_REGISTERS.h"


void isControlUnitCommissioned(const makalu_bxcan_frame_t *frame) {
    if (MAKALU_FDCAN_GET_SRC_NODE(frame->id) != 0x01 && frame->data[0] != MAKALU_FDCAN_N_PDM_B_VR0) return;

    eeprom_driver_t *drv = pdmCore_getEEPROMDRV();
    // check both magic registers in EEPROM
    uint8_t magic0;
    uint8_t magic1;
    drv->read_byte(drv->ctx, PDM_REG_MAGIC_0, &magic0);
    drv->read_byte(drv->ctx, PDM_REG_MAGIC_1, &magic1);

    makalu_commissioned_frame_t cFrameData = {
        .codingCommissioned = 0,
        .configCommissioned = 0
    };

    if (magic0 != PDM_MAGIC0_EXP_V) {
        cFrameData.codingCommissioned = 0;
    } else {
        cFrameData.codingCommissioned = 1;
    }

    if (magic1 != PDM_MAGIC1_EXP_V) {
        cFrameData.configCommissioned = 0;
    } else {
        cFrameData.configCommissioned = 1;
    }

    makalu_bxcan_frame_t cFrame = {0};
    cFrame.id = MAKALU_FDCAN_BUILD_ID(MAKALU_FDCAN_PRIO_NORMAL, MAKALU_FDCAN_N_PDM_B_VR0, MAKALU_MSG_CFG_ACK, 0x00);
    cFrame.len = sizeof(makalu_commissioned_frame_t);
    memcpy(cFrame.data, &cFrameData, cFrame.len);

    makalu_bxcan_send(&cFrame);




}
