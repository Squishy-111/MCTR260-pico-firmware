/**
 * @file safety.h
 * @brief Motor safety watchdog
 */

#ifndef SAFETY_H
#define SAFETY_H

/**
 * @brief Initialize safety watchdog
 */
void safety_init();

/**
 * @brief Feed the watchdog (call when command received)
 */
void safety_feed();

/**
 * @brief Check if timeout has occurred
 * @return true if motors should be stopped
 */
bool safety_check_timeout();

/**
 * @brief Get time since last command (milliseconds)
 */
unsigned long safety_get_idle_time();

#endif // SAFETY_H
