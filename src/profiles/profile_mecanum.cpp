/**
 * @file profile_mecanum.cpp
 * @brief Mecanum drive motion profile implementation
 *
 * Converts joystick commands to mecanum wheel speeds.
 * Works with both DC motors (PWM) and steppers (steps/sec).
 */

#include "profile_mecanum.h"
#include "drivers/mecanum_kinematics.h"
#include "core/motor_manager.h"
#include "core/motor_dc.h"
#include "core/motor_stepper.h"
#include "project_config.h"

#include <Arduino.h>

void profile_mecanum_apply(const control_command_t* cmd) {
    // Extract inputs from command
    // Left joystick: strafe (X) and forward (Y)
    // Right joystick: rotation
    
    float vx = 0;  // Strafe
    float vy = 0;  // Forward
    float omega = 0;  // Rotation
    
    if (cmd->left.isJoystick) {
        vx = cmd->left.x;
        vy = cmd->left.y;
    }
    
    if (cmd->right.isJoystick) {
        omega = cmd->right.x;  // Use X for rotation
    } else {
        omega = cmd->right.value;  // Dial value
    }
    
    // Speed multiplier (0-100 → 0.0-1.0)
    float speedMult = cmd->speed / 100.0f;
    
    // Calculate wheel speeds
    WheelSpeeds wheels;
    mecanum_calculate(vx, vy, omega, speedMult, MOTOR_DEADZONE, &wheels);
    
    // Apply to motors based on type
    if (motors_has_dc()) {
        // DC motors: convert -100..+100 to -255..+255 PWM
        motor_set_pwm(0, (int16_t)(wheels.frontLeft * 2.55f));
        motor_set_pwm(1, (int16_t)(wheels.frontRight * 2.55f));
        motor_set_pwm(2, (int16_t)(wheels.backLeft * 2.55f));
        motor_set_pwm(3, (int16_t)(wheels.backRight * 2.55f));
    } else if (motors_has_steppers()) {
        // Steppers: convert -100..+100 to steps/sec
        // Scale by max speed (e.g., 100% = STEPPER_MAX_SPEED)
        float scale = STEPPER_MAX_SPEED / 100.0f;
        motor_set_speed(0, wheels.frontLeft * scale);
        motor_set_speed(1, wheels.frontRight * scale);
        motor_set_speed(2, wheels.backLeft * scale);
        motor_set_speed(3, wheels.backRight * scale);
    }
}
