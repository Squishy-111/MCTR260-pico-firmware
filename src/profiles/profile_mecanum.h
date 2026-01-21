/**
 * @file profile_mecanum.h
 * @brief Mecanum drive motion profile
 */

#ifndef PROFILE_MECANUM_H
#define PROFILE_MECANUM_H

#include "core/command_packet.h"

/**
 * @brief Apply mecanum profile to current command
 * @param cmd Control command from app
 */
void profile_mecanum_apply(const control_command_t* cmd);

#endif // PROFILE_MECANUM_H
