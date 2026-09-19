//
// Created by Finn Carmichael on 4/26/26.
//

#include "cdg_session.h"
#include "PDM_EEPROM_REGISTERS.h"
#include "makalu_bxcan.h"
#include "makalu_fdcan_ids.h"
#include <string.h>

#include "eeprom_driver.h"
#include "main_app.h"

/* ── Session state ────────────────────────────────────────────────────────── */
static bool    s_init_session_open = false;

/* ── Chunk write state ────────────────────────────────────────────────────── */
static uint8_t s_chunk_reg        = 0x00;
static uint8_t s_chunk_buf[17]    = {0};
static uint8_t s_chunk_received   = 0;
static uint8_t s_chunk_expected   = 0;
static bool    s_chunk_active     = false;

/* ── Internal helpers ─────────────────────────────────────────────────────── */

static void send_ack(uint8_t reg, uint8_t *data, uint8_t len)
{
    makalu_bxcan_frame_t frame = {0};
    frame.id      = MAKALU_FDCAN_BUILD_ID(
                        MAKALU_FDCAN_PRIO_NORMAL,
                        MAKALU_FDCAN_N_PDM_B_VR0,
                        MAKALU_MSG_CDG_ACK,
                        reg);
    frame.data[0] = reg;
    if (data && len > 0) {
        uint8_t copy_len = len > 7 ? 7 : len;
        memcpy(&frame.data[1], data, copy_len);
        frame.len = 1 + copy_len;
    } else {
        frame.len = 1;
    }
    makalu_bxcan_send(&frame);
}

static void send_nack(uint8_t reg, uint8_t reason)
{
    makalu_bxcan_frame_t frame = {0};
    frame.id      = MAKALU_FDCAN_BUILD_ID(
                        MAKALU_FDCAN_PRIO_NORMAL,
                        MAKALU_FDCAN_N_PDM_B_VR0,
                        MAKALU_MSG_CDG_NACK,
                        reg);
    frame.data[0] = reg;
    frame.data[1] = reason;
    frame.len     = 2;
    makalu_bxcan_send(&frame);
}

static void send_handshake(uint8_t mode)
{
    makalu_bxcan_frame_t frame = {0};
    frame.id      = MAKALU_FDCAN_BUILD_ID(
                        MAKALU_FDCAN_PRIO_NORMAL,
                        MAKALU_FDCAN_N_PDM_B_VR0,
                        MAKALU_MSG_CDG_HANDSHAKE,
                        0x00);
    frame.data[0] = mode;
    frame.len     = 1;
    makalu_bxcan_send(&frame);
}

static uint8_t get_reg_size(uint8_t reg)
{
    /* channel current limits — 2 bytes each */
    if (reg >= PDM_REG_CH_CURRENT_LIMIT_BASE &&
        reg <  PDM_REG_CH_CURRENT_LIMIT_BASE + 64) return 2;

    /* channel fuse ratings — 1 byte each */
    if (reg >= PDM_REG_CH_FUSE_BASE &&
        reg <  PDM_REG_CH_FUSE_BASE + 32) return 1;

    /* multi-byte string registers */
    if (reg == PDM_REG_VEHICLE_MODEL_BASE) return PDM_REG_VEHICLE_MODEL_LEN;
    if (reg == PDM_REG_VEHICLE_MANF_BASE)  return PDM_REG_VEHICLE_MANF_LEN;
    if (reg == PDM_REG_VEHICLE_VIN_BASE)   return PDM_REG_VEHICLE_VIN_LEN;

    switch (reg) {
        /* 1 byte registers */
        case PDM_REG_MAGIC_0:
        case PDM_REG_MAGIC_1:
        case PDM_REG_SCHEMA_VER:
        case PDM_REG_NODE_ID:
        case PDM_REG_HW_VERSION:
        case PDM_REG_FW_VERSION_MAJOR:
        case PDM_REG_FW_VERSION_MINOR:
        case PDM_REG_SERIAL_0:
        case PDM_REG_SERIAL_1:
        case PDM_REG_SERIAL_2:
        case PDM_REG_SERIAL_3:
        case PDM_REG_CAN_BAUD:
        case PDM_REG_BOOT_MODE:
        case PDM_REG_BOOT_STATE_0:
        case PDM_REG_BOOT_STATE_1:
        case PDM_REG_BOOT_STATE_2:
        case PDM_REG_BOOT_STATE_3:
        case PDM_REG_CH_ENABLED_0:
        case PDM_REG_CH_ENABLED_1:
        case PDM_REG_CH_ENABLED_2:
        case PDM_REG_CH_ENABLED_3:
            return 1;

        /* 2 byte registers */
        case PDM_REG_HEARTBEAT_RATE_MS:
        case PDM_REG_TELEM_RATE_MS:
        case PDM_REG_VEHICLE_YEAR_H:
            return 2;

        default:
            return 0;  /* unknown register */
    }
}

static bool is_write_protected(uint8_t reg)
{
    /* magic bytes and schema version are read only */
    return (reg == PDM_REG_MAGIC_0 ||
            reg == PDM_REG_MAGIC_1 ||
            reg == PDM_REG_SCHEMA_VER);
}

/* ── Public API ───────────────────────────────────────────────────────────── */

void cdg_session_init(void)
{
    s_init_session_open = false;
    s_chunk_active      = false;
    s_chunk_received    = 0;
    s_chunk_expected    = 0;
    s_chunk_reg         = 0x00;
    memset(s_chunk_buf, 0, sizeof(s_chunk_buf));
}

/* ── Announce ─────────────────────────────────────────────────────────────── */
void cdg_session_on_announce(const makalu_bxcan_frame_t *frame)
{
    uint8_t mode = frame->data[0];

    if (mode == CDG_MODE_INIT) {
        s_init_session_open = true;
        s_chunk_active      = false;
    }

    send_handshake(mode);
}

/* ── Read ─────────────────────────────────────────────────────────────────── */
void cdg_session_on_read(const makalu_bxcan_frame_t *frame)
{
    uint8_t mode = frame->data[0];
    uint8_t reg  = frame->data[1];

    if (mode == CDG_MODE_INIT && !s_init_session_open) {
        send_nack(reg, CDG_NACK_SESSION_CLOSED);
        return;
    }

    uint8_t size = get_reg_size(reg);
    if (size == 0) {
        send_nack(reg, CDG_NACK_UNKNOWN_REG);
        return;
    }

    eeprom_driver_t *drv = pdmCore_getEEPROMDRV();
    uint8_t buf[17]      = {0};

    for (uint8_t i = 0; i < size; i++) {
        if (drv->read_byte(drv->ctx, reg + i, &buf[i]) != EEPROM_OK) {
            send_nack(reg, CDG_NACK_READ_FAIL);
            return;
        }
    }

    send_ack(reg, buf, size);
}

/* ── Write ────────────────────────────────────────────────────────────────── */
void cdg_session_on_write(const makalu_bxcan_frame_t *frame)
{
    uint8_t mode = frame->data[0];
    uint8_t reg  = frame->data[1];
    uint8_t len  = frame->len - 2;

    if (mode == CDG_MODE_INIT && !s_init_session_open) {
        send_nack(reg, CDG_NACK_SESSION_CLOSED);
        return;
    }

    if (is_write_protected(reg)) {
        send_nack(reg, CDG_NACK_UNKNOWN_REG);
        return;
    }

    uint8_t size = get_reg_size(reg);
    if (size == 0) {
        send_nack(reg, CDG_NACK_UNKNOWN_REG);
        return;
    }

    if (len != size) {
        send_nack(reg, CDG_NACK_BAD_LEN);
        return;
    }

    eeprom_driver_t *drv = pdmCore_getEEPROMDRV();
    const uint8_t *data  = &frame->data[2];

    for (uint8_t i = 0; i < len; i++) {
        if (drv->write_byte(drv->ctx, reg + i, data[i]) != EEPROM_OK) {
            send_nack(reg, CDG_NACK_WRITE_FAIL);
            return;
        }
    }

    send_ack(reg, NULL, 0);
}

/* ── Chunk write ──────────────────────────────────────────────────────────── */
void cdg_session_on_write_chunk(const makalu_bxcan_frame_t *frame)
{
    uint8_t mode   = frame->data[0];
    uint8_t reg    = frame->data[1];
    uint8_t offset = frame->data[2];
    uint8_t len    = frame->data[3];

    if (mode == CDG_MODE_INIT && !s_init_session_open) {
        send_nack(reg, CDG_NACK_SESSION_CLOSED);
        return;
    }

    if (is_write_protected(reg)) {
        send_nack(reg, CDG_NACK_UNKNOWN_REG);
        return;
    }

    uint8_t reg_size = get_reg_size(reg);
    if (reg_size == 0) {
        send_nack(reg, CDG_NACK_UNKNOWN_REG);
        return;
    }

    if (offset + len > reg_size) {
        send_nack(reg, CDG_NACK_CHUNK_OVERFLOW);
        return;
    }

    /* start new chunk session if register changed */
    if (!s_chunk_active || s_chunk_reg != reg) {
        s_chunk_reg      = reg;
        s_chunk_received = 0;
        s_chunk_expected = reg_size;
        s_chunk_active   = true;
        memset(s_chunk_buf, 0, sizeof(s_chunk_buf));
    }

    memcpy(&s_chunk_buf[offset], &frame->data[4], len);
    s_chunk_received += len;

    send_ack(reg, NULL, 0);
}

/* ── Chunk write end ──────────────────────────────────────────────────────── */
void cdg_session_on_write_end(const makalu_bxcan_frame_t *frame)
{
    uint8_t mode = frame->data[0];
    uint8_t reg  = frame->data[1];

    if (mode == CDG_MODE_INIT && !s_init_session_open) {
        send_nack(reg, CDG_NACK_SESSION_CLOSED);
        return;
    }

    if (!s_chunk_active || s_chunk_reg != reg) {
        send_nack(reg, CDG_NACK_CHUNK_MISSING);
        return;
    }

    if (s_chunk_received < s_chunk_expected) {
        send_nack(reg, CDG_NACK_CHUNK_MISSING);
        s_chunk_active = false;
        return;
    }

    eeprom_driver_t *drv = pdmCore_getEEPROMDRV();

    for (uint8_t i = 0; i < s_chunk_expected; i++) {
        if (drv->write_byte(drv->ctx, s_chunk_reg + i, s_chunk_buf[i]) != EEPROM_OK) {
            send_nack(reg, CDG_NACK_WRITE_FAIL);
            s_chunk_active = false;
            return;
        }
    }

    s_chunk_active = false;
    send_ack(reg, NULL, 0);
}

