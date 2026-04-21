#include "main_app.h"

#include "at24c02.h"
#include "makalu_hal.h"
#include "makalu_bxcan.h"
#include "stm32f4xx_hal_can.h"
#include "makalu_fdcan_ids.h"
static void handleFrame(const makalu_bxcan_frame_t *frame) {
}
I2C_HandleTypeDef *hi;
void main_app_init(CAN_HandleTypeDef *hcan1, I2C_HandleTypeDef *hi2c1) {
    hi = hi2c1;
    makalu_bxcan_init(hcan1);
    makalu_bxcan_register(MAKALU_MSG_BROADCAST, handleFrame);



    uint8_t writeBuf[4] = {'F', 'I', 'N', 'N'};
    AT24C02_Status wStatus = AT24C02_WriteBuffer(hi, 0x00, writeBuf, 4);

    // Read it back
    uint8_t readBuf[4] = {0};
    AT24C02_Status rStatus = AT24C02_ReadBuffer(hi, 0x02, readBuf, 2);


}

void main_app_run(void)
{
    makalu_bxcan_process();



    /* send a heartbeat every 1000ms */
    static uint32_t last_tx = 0;
    if (HAL_GetTick() - last_tx >= 1000) {
        last_tx = HAL_GetTick();

        makalu_bxcan_frame_t frame = {0};
        frame.id     = MAKALU_FDCAN_BUILD_ID(MAKALU_FDCAN_PRIO_LOW, MAKALU_FDCAN_N_PDM_B_VR0, 0x01, 0x00);
        frame.data[0] = 0xAA;
        frame.len    = 1;

        makalu_bxcan_send(&frame);
    }
}