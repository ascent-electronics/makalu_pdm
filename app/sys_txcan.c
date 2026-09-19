//
// Created by Finn Carmichael on 4/26/26.
//

#include "sys_txcan.h"

#include <string.h>

#include "cmsis_os2.h"
#include "makalu_fdcan_frames.h"

void sendHeartbeat(void *argument) {
    (void)argument;
    for (;;) {
        makalu_heartbeat_frame_t hbFrameData = {
            .node_id = MAKALU_FDCAN_N_PDM_B_VR0,
            .op_state = MAKALU_STATE_NORMAL // get from config store or eeprom in future
        };

        makalu_bxcan_frame_t hbFrame = {0};
        hbFrame.id = MAKALU_FDCAN_BUILD_ID(MAKALU_FDCAN_PRIO_LOW, MAKALU_FDCAN_N_PDM_B_VR0, MAKALU_MSG_HEARTBEAT, 0x00);
        hbFrame.len = sizeof(makalu_heartbeat_frame_t);
        memcpy(hbFrame.data, &hbFrameData, hbFrame.len);

        makalu_bxcan_send(&hbFrame);
        osDelay(1000);
    }
}