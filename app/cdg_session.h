#ifndef CDG_SESSION_H
#define CDG_SESSION_H

#include <stdint.h>
#include <stdbool.h>
#include "makalu_bxcan.h"
#include "makalu_fdcan_ids.h"

/* ── CDG session modes ────────────────────────────────────────────────────── */
#define CDG_MODE_INIT               0x01  /* full coding init session          */
#define CDG_MODE_STANDARD           0x02  /* standard read/write               */

/* ── NACK reason codes ────────────────────────────────────────────────────── */
#define CDG_NACK_UNKNOWN_REG        0x01  /* register not recognised           */
#define CDG_NACK_WRITE_FAIL         0x02  /* EEPROM write failed               */
#define CDG_NACK_READ_FAIL          0x03  /* EEPROM read failed                */
#define CDG_NACK_SESSION_CLOSED     0x04  /* init session not open             */
#define CDG_NACK_BAD_LEN            0x05  /* payload length mismatch           */
#define CDG_NACK_CHUNK_OVERFLOW     0x06  /* chunk offset + len exceeds reg    */
#define CDG_NACK_CHUNK_MISSING      0x07  /* end received but bytes missing    */

/* ── Public API ───────────────────────────────────────────────────────────── */
void cdg_session_init(void);
void cdg_session_on_announce(const makalu_bxcan_frame_t *frame);
void cdg_session_on_read(const makalu_bxcan_frame_t *frame);
void cdg_session_on_write(const makalu_bxcan_frame_t *frame);
void cdg_session_on_write_chunk(const makalu_bxcan_frame_t *frame);
void cdg_session_on_write_end(const makalu_bxcan_frame_t *frame);

#endif /* CDG_SESSION_H */