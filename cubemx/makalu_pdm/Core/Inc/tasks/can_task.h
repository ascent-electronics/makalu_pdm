//
// Created by Finn Carmichael on 4/24/26.
//

#ifndef MAKALU_PDM_CAN_TASK_H
#define MAKALU_PDM_CAN_TASK_H

#include "cmsis_os2.h"

extern osMessageQueueId_t can_tx_queue;

void can_tx_task(void *argument);
void can_rx_task(void *argument);

#endif //MAKALU_PDM_CAN_TASK_H
