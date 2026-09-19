#ifndef MAKALU_PDM_EEPROM_MAP_H
#define MAKALU_PDM_EEPROM_MAP_H

// ─── AT24C02 — 256 bytes total (0x00–0xFF) ───────────────────────────────────

#define PDM_REG_VEHICLE_MODEL_BASE      0x90
#define PDM_REG_VEHICLE_MODEL_LEN       16

// Vehicle Manufacturer — 16 bytes (0xA0–0xAF)
#define PDM_REG_VEHICLE_MANF_BASE       0xA0
#define PDM_REG_VEHICLE_MANF_LEN        16

// Vehicle VIN — 17 bytes (0xB0–0xC0)
#define PDM_REG_VEHICLE_VIN_BASE        0xB0
#define PDM_REG_VEHICLE_VIN_LEN         17

// Vehicle Year — 2 bytes uint16 big endian (0xC1–0xC2)
#define PDM_REG_VEHICLE_YEAR_H          0xC1
#define PDM_REG_VEHICLE_YEAR_L          0xC2

// ─── System / Identity (0x00–0x0F) ───────────────────────────────────────────
#define PDM_REG_MAGIC_0             0x00  // Magic byte 0 — must be 0xC0
#define PDM_REG_MAGIC_1             0x01  // Magic byte 1 — must be 0xDE
#define PDM_REG_SCHEMA_VER          0x02  // Register map version
#define PDM_REG_NODE_ID             0x03  // CAN node ID
#define PDM_REG_HW_VERSION          0x04  // Hardware variant
#define PDM_REG_FW_VERSION_MAJOR    0x05  // Firmware major version
#define PDM_REG_FW_VERSION_MINOR    0x06  // Firmware minor version
#define PDM_REG_SERIAL_0            0x07  // Serial number byte 0 (MSB)
#define PDM_REG_SERIAL_1            0x08  // Serial number byte 1
#define PDM_REG_SERIAL_2            0x09  // Serial number byte 2
#define PDM_REG_SERIAL_3            0x0A  // Serial number byte 3 (LSB)


// 0x0C–0x0F reserved

// ─── Global Config (0x10–0x1F) ───────────────────────────────────────────────
#define PDM_REG_HEARTBEAT_RATE_MS   0x10  // Heartbeat interval in ms (uint16 high)
#define PDM_REG_HEARTBEAT_RATE_MS_L 0x11  // Heartbeat interval in ms (uint16 low)
#define PDM_REG_TELEM_RATE_MS       0x12  // Telemetry broadcast rate in ms (uint16 high)
#define PDM_REG_TELEM_RATE_MS_L     0x13  // Telemetry broadcast rate in ms (uint16 low)
#define PDM_REG_CAN_BAUD            0x14  // CAN baud rate index (0=500k, 1=1M)
#define PDM_REG_BOOT_MODE           0x15  // Boot behaviour (0=restore last, 1=all off, 2=all on)
// 0x16–0x1F reserved

// ─── Channel Boot State Bitmap (0x20–0x23) ───────────────────────────────────
// uint32 bitmap — bit N = channel N default state on boot (0=off, 1=on)
#define PDM_REG_BOOT_STATE_0        0x20  // ch0–7   boot state
#define PDM_REG_BOOT_STATE_1        0x21  // ch8–15  boot state
#define PDM_REG_BOOT_STATE_2        0x22  // ch16–23 boot state
#define PDM_REG_BOOT_STATE_3        0x23  // ch24–31 boot state

// ─── Channel Enabled Bitmap (0x24–0x27) ──────────────────────────────────────
// uint32 bitmap — bit N = channel N enabled (0=disabled, 1=enabled)
#define PDM_REG_CH_ENABLED_0        0x24  // ch0–7   enabled
#define PDM_REG_CH_ENABLED_1        0x25  // ch8–15  enabled
#define PDM_REG_CH_ENABLED_2        0x26  // ch16–23 enabled
#define PDM_REG_CH_ENABLED_3        0x27  // ch24–31 enabled

// ─── Channel Current Limits (0x30–0x6F) ──────────────────────────────────────
// Each channel = 2 bytes (uint16 mA, big endian)
// Address = 0x30 + (channel * 2)
// ch0  = 0x30–0x31
// ch1  = 0x32–0x33
// ...
// ch31 = 0x6E–0x6F
#define PDM_REG_CH_CURRENT_LIMIT_BASE  0x30
#define PDM_REG_CH_CURRENT_LIMIT(ch)  (PDM_REG_CH_CURRENT_LIMIT_BASE + ((ch) * 2))

// ─── Channel Fuse Ratings (0x70–0x8F) ────────────────────────────────────────
// Each channel = 1 byte (uint8 Amps)
// Address = 0x70 + channel
// ch0  = 0x70
// ch1  = 0x71
// ...
// ch31 = 0x8F
#define PDM_REG_CH_FUSE_BASE          0x70
#define PDM_REG_CH_FUSE(ch)          (PDM_REG_CH_FUSE_BASE + (ch))

// ─── Reserved / Future Use (0x90–0xFF) ───────────────────────────────────────
// 112 bytes free for expansion

// ─── Magic values ─────────────────────────────────────────────────────────────
#define PDM_EEPROM_MAGIC_0          0xC0
#define PDM_EEPROM_MAGIC_1          0xDE
#define PDM_EEPROM_SCHEMA_VER       0x01

// ─── Boot mode values ─────────────────────────────────────────────────────────
#define PDM_BOOT_MODE_RESTORE       0x00  // Restore last known state
#define PDM_BOOT_MODE_ALL_OFF       0x01  // All channels off on boot
#define PDM_BOOT_MODE_ALL_ON        0x02  // All channels on on boot

// ─── CAN baud index values ────────────────────────────────────────────────────
#define PDM_CAN_BAUD_500K           0x00
#define PDM_CAN_BAUD_1M             0x01

#endif /* MAKALU_PDM_EEPROM_MAP_H */