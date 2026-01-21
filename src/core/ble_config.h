/**
 * @file ble_config.h
 * @brief BLE UUIDs and constants for Pico W
 *
 * These UUIDs MUST match the ESP32 firmware and Flutter app.
 */

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

// =============================================================================
// SERVICE UUIDs (Must match ESP32 and Flutter app)
// =============================================================================

// Main control service
#define CONTROL_SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

// =============================================================================
// CHARACTERISTIC UUIDs
// =============================================================================

// Control commands (app writes JSON here)
#define CONTROL_CHAR_UUID               "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Telemetry/messages (firmware sends notifications here)
#define TELEMETRY_CHAR_UUID             "beb5483f-36e1-4688-b7f5-ea07361b26a8"

// =============================================================================
// BLE PARAMETERS
// =============================================================================

// Maximum attribute value length (MTU-3)
#define BLE_ATT_MAX_VALUE_LEN           512

// Connection interval (in 1.25ms units)
#define BLE_CONN_INTERVAL_MIN           6   // 7.5ms
#define BLE_CONN_INTERVAL_MAX           24  // 30ms

#endif // BLE_CONFIG_H
