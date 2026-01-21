/**
 * @file motor_dc.cpp
 * @brief DC motor control implementation
 */

#include "motor_dc.h"
#include <Arduino.h>

// =============================================================================
// CONSTRUCTOR
// =============================================================================

MotorDC::MotorDC(const MotorDCConfig& config)
    : cfg_(config)
    , targetPWM_(0)
    , currentPWM_(0) {
}

// =============================================================================
// MOTOR BASE INTERFACE
// =============================================================================

bool MotorDC::init() {
    if (!cfg_.enabled) {
        return true;  // Disabled motors always succeed
    }
    
    // Configure pins based on driver type
    switch (cfg_.driverType) {
        case DCDriverType::DRV8871:
        case DCDriverType::DRV8833:
            pinMode(cfg_.pinA, OUTPUT);
            pinMode(cfg_.pinB, OUTPUT);
            break;
            
        case DCDriverType::L298N:
            pinMode(cfg_.pinA, OUTPUT);     // IN1
            pinMode(cfg_.pinB, OUTPUT);     // IN2
            if (cfg_.pinEnable >= 0) {
                pinMode(cfg_.pinEnable, OUTPUT);
            }
            break;
    }
    
    // Set PWM frequency for quiet operation
    analogWriteFreq(20000);  // 20kHz
    analogWriteResolution(8);  // 8-bit (0-255)
    
    // Ensure motor is stopped
    stop();
    
    Serial.printf("[MotorDC] Motor %d initialized (%s)\n", 
                  cfg_.index, 
                  cfg_.driverType == DCDriverType::DRV8871 ? "DRV8871" :
                  cfg_.driverType == DCDriverType::DRV8833 ? "DRV8833" : "L298N");
    
    return true;
}

void MotorDC::stop() {
    targetPWM_ = 0;
    currentPWM_ = 0;
    applyPWM(0);
}

void MotorDC::update(float dtSec) {
    if (!cfg_.enabled) {
        return;
    }
    
    // Open-loop: directly apply target
    // Could add ramping here for smoother acceleration
    if (currentPWM_ != targetPWM_) {
        currentPWM_ = targetPWM_;
        applyPWM(currentPWM_);
    }
}

uint8_t MotorDC::getIndex() const {
    return cfg_.index;
}

bool MotorDC::isEnabled() const {
    return cfg_.enabled;
}

MotorType MotorDC::getType() const {
    return MotorType::DC;
}

// =============================================================================
// DC MOTOR SPECIFIC
// =============================================================================

void MotorDC::setTarget(int16_t pwm) {
    // Apply direction correction
    targetPWM_ = pwm * cfg_.direction;
    
    // Clamp to valid range
    if (targetPWM_ > 255) targetPWM_ = 255;
    if (targetPWM_ < -255) targetPWM_ = -255;
}

int16_t MotorDC::getTarget() const {
    return targetPWM_;
}

void MotorDC::applyPWM(int16_t pwm) {
    if (!cfg_.enabled) {
        return;
    }
    
    switch (cfg_.driverType) {
        case DCDriverType::DRV8871:
        case DCDriverType::DRV8833:
            // 2-pin control: one pin HIGH, other PWM (or vice versa for reverse)
            if (pwm >= 0) {
                analogWrite(cfg_.pinA, pwm);
                analogWrite(cfg_.pinB, 0);
            } else {
                analogWrite(cfg_.pinA, 0);
                analogWrite(cfg_.pinB, -pwm);
            }
            break;
            
        case DCDriverType::L298N:
            // Enable + 2 direction pins
            if (cfg_.pinEnable >= 0) {
                analogWrite(cfg_.pinEnable, abs(pwm));
            }
            if (pwm >= 0) {
                digitalWrite(cfg_.pinA, HIGH);
                digitalWrite(cfg_.pinB, LOW);
            } else {
                digitalWrite(cfg_.pinA, LOW);
                digitalWrite(cfg_.pinB, HIGH);
            }
            break;
    }
}
