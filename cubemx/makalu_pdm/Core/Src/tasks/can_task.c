//
// Created by Finn Carmichael on 4/26/26.
//

#include "can_task.h"
#include "makalu_bxcan.h"
#include "cmsis_os2.h"

osMessageQueueId_t can_tx_queue;

void can_tx_task(void *argument)
{
    (void)argument;
    can_tx_queue = osMessageQueueNew(16, sizeof(makalu_bxcan_frame_t), NULL);

    for (;;)
    {
        makalu_bxcan_frame_t frame;
        if (osMessageQueueGet(can_tx_queue, &frame, NULL, osWaitForever) == osOK)
        {
            makalu_bxcan_send(&frame);
        }
    }
}

void can_rx_task(void *argument)
{
    (void)argument;

    for (;;)
    {
        makalu_bxcan_process();
        osDelay(1);
    }
}