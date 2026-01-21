/**
 * @file main.cpp
 * @brief Main Entry Point for Raspberry Pi Pico W Robot Controller
 *
 * This firmware enables BLE control of a mecanum wheel robot
 * using the RC Controller Flutter app.
 */

#include <Arduino.h>

// Project configuration
#include "project_config.h"

// Core modules
#include "core/ble_controller.h"
#include "core/command_parser.h"
#include "core/motor_manager.h"
#include "core/safety.h"

// Profiles
#include "profiles/profile_mecanum.h"

// =============================================================================
// GLOBAL STATE
// =============================================================================

static bool s_bleConnected = false;
static unsigned long s_lastUpdateTime = 0;

// =============================================================================
// BLE CALLBACKS
// =============================================================================

/**
 * @brief Called when a command is received over BLE
 */
void onBleCommand(const char* jsonData, uint16_t length) {
    // Feed safety watchdog
    safety_feed();
    
    // Parse command
    control_command_t cmd;
    if (!command_parse(jsonData, &cmd)) {
        return;
    }
    
    // Skip heartbeats (they just keep connection alive)
    if (command_is_heartbeat()) {
        return;
    }
    
    // Apply motion profile based on vehicle type
    if (strcmp(cmd.vehicle, "mecanum") == 0) {
#ifdef MOTION_PROFILE_MECANUM
        profile_mecanum_apply(&cmd);
#endif
    }
    // Add other profiles here as needed
}

/**
 * @brief Called when BLE connection state changes
 */
void onBleConnectionChange(bool connected) {
    s_bleConnected = connected;
    
    if (connected) {
        Serial.println(">>> Client connected!");
    } else {
        Serial.println(">>> Client disconnected - stopping motors");
        motors_stop_all();
    }
}

// =============================================================================
// ARDUINO SETUP
// =============================================================================

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    while (!Serial && millis() < 3000) {
        // Wait for serial (up to 3 seconds)
    }
    
    Serial.println();
    Serial.println("===========================================");
    Serial.println("   RC Robot Controller - Pico W Edition");
    Serial.println("===========================================");
    Serial.printf("Device Name: %s\n", DEVICE_NAME);
    Serial.printf("PIN Code: %06d\n", BLE_PASSKEY);
    Serial.println();
    
    // Initialize LED for status
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    // Initialize safety watchdog
    safety_init();
    
    // Initialize motors
    if (!motors_init()) {
        Serial.println("ERROR: Motor initialization failed!");
        while (1) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(100);  // Fast blink = error
        }
    }
    
    // Initialize BLE (starts advertising)
    ble_init(onBleCommand, onBleConnectionChange);
    
    Serial.println();
    Serial.println("Setup complete - waiting for BLE connection...");
    Serial.println();
    
    s_lastUpdateTime = millis();
}

// =============================================================================
// ARDUINO LOOP
// =============================================================================

void loop() {
    // Process BLE events
    ble_update();
    
    // Motor update at 50Hz
    unsigned long now = millis();
    if (now - s_lastUpdateTime >= MOTOR_UPDATE_INTERVAL_MS) {
        float dtSec = (now - s_lastUpdateTime) / 1000.0f;
        s_lastUpdateTime = now;
        
        // Check safety timeout
        if (s_bleConnected && safety_check_timeout()) {
            // Timeout triggered - motors already stopped by safety module
        }
        
        // Update motor control loops
        motors_update(dtSec);
    }
    
    // Status LED: blink when disconnected, solid when connected
    if (!s_bleConnected) {
        static unsigned long lastBlink = 0;
        if (now - lastBlink >= 500) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            lastBlink = now;
        }
    }
}
