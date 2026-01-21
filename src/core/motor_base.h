/**
 * @file motor_base.h
 * @brief Abstract base class for all motor types
 *
 * Provides common interface for DC motors and steppers.
 * Enables polymorphic operations like emergency stop.
 */

#ifndef MOTOR_BASE_H
#define MOTOR_BASE_H

#include <stdint.h>

/**
 * @brief Motor type enumeration
 */
enum class MotorType {
    DC,         // DC motor with PWM control
    Stepper,    // Stepper with step/dir control
    Servo       // Future: servo motor
};

/**
 * @brief Abstract base class for all motor types
 */
class MotorBase {
public:
    virtual ~MotorBase() = default;
    
    /**
     * @brief Initialize motor hardware
     * @return true if successful
     */
    virtual bool init() = 0;
    
    /**
     * @brief Stop motor immediately (emergency stop)
     */
    virtual void stop() = 0;
    
    /**
     * @brief Update motor control (call at 50Hz)
     * @param dtSec Time since last update in seconds
     */
    virtual void update(float dtSec) = 0;
    
    /**
     * @brief Get motor index (0-based)
     */
    virtual uint8_t getIndex() const = 0;
    
    /**
     * @brief Check if motor is enabled
     */
    virtual bool isEnabled() const = 0;
    
    /**
     * @brief Get motor type
     */
    virtual MotorType getType() const = 0;
};

#endif // MOTOR_BASE_H
