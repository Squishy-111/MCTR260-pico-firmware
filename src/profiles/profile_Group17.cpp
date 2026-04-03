#include "project_config.h"

#ifdef MOTION_PROFILE_GROUP17

#include "core/command_packet.h"
#include "core/safety.h"
#include "pico/mutex.h"
#include <Arduino.h>
#include <math.h>

extern mutex_t        g_speedMutex;
extern volatile float g_targetSpeeds[];
extern volatile bool  g_speedsUpdated;

// ============================================================================
// SPEED
// ============================================================================

static const float DRIVE_SPEED = 900.0f;  // steps/sec (max is 4000)

// ============================================================================
// MISSION TIMING
// ============================================================================

static const float SEG0_STRAFE_M  = 0.8f;
static const float SEG1_FORWARD_M = 1.2f;
static const float SEG2_STRAFE_M  = 0.8f;
static const float SEG3_FORWARD_M = 1.2f;

static const float DIAG_X1 = 2.0f, DIAG_Y1 = 2.0f;
static const float DIAG_X2 = 4.0f, DIAG_Y2 = 4.0f;

static const float TRIM_SEG0 = 1.0f;
static const float TRIM_SEG1 = 0.8f;
static const float TRIM_SEG2 = 1.0f;
static const float TRIM_SEG3 = 0.8f;
static const float TRIM_SEG4 = 1.15f;

// ============================================================================
// PHYSICS (do not edit)
// ============================================================================

static const float WHEEL_CIRC_M = (2.0f * M_PI * WHEEL_RADIUS_MM) / 1000.0f;
static const float STEPS_PER_M  = (STEPPER_STEPS_PER_REV * STEPPER_MICROSTEPPING) / WHEEL_CIRC_M;
static const float SECS_PER_M   = STEPS_PER_M / DRIVE_SPEED;
static const float DIAG_DIST_M  = sqrtf((DIAG_X2-DIAG_X1)*(DIAG_X2-DIAG_X1) + (DIAG_Y2-DIAG_Y1)*(DIAG_Y2-DIAG_Y1));
static const float DIAG_SECS    = DIAG_DIST_M / ((DRIVE_SPEED / STEPS_PER_M) / sqrtf(2.0f));

// ============================================================================
// SEGMENT TABLE
// ============================================================================

struct Segment { float fl, fr, bl, br; unsigned long durationMs; };

static const Segment SEGMENTS[] = {
  {  DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED,  DRIVE_SPEED, (unsigned long)(SEG0_STRAFE_M  * SECS_PER_M * TRIM_SEG0 * 1000) }, // 0: strafe right
  { -DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED, (unsigned long)(SEG1_FORWARD_M * SECS_PER_M * TRIM_SEG1 * 1000) }, // 1: forward
  {  DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED,  DRIVE_SPEED, (unsigned long)(SEG2_STRAFE_M  * SECS_PER_M * TRIM_SEG2 * 1000) }, // 2: strafe right
  { -DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED, (unsigned long)(SEG3_FORWARD_M * SECS_PER_M * TRIM_SEG3 * 1000) }, // 3: forward
  {  0.0f,       -DRIVE_SPEED, -DRIVE_SPEED,  0.0f,         (unsigned long)(DIAG_SECS               * TRIM_SEG4 * 1000) }, // 4: diagonal
};

static const int NUM_SEGMENTS = (int)(sizeof(SEGMENTS) / sizeof(SEGMENTS[0]));

// ============================================================================
// TOGGLE MAP
//  toggle[0] = full mission
//  toggle[1] = seg 0 only (strafe right)
//  toggle[2] = seg 1 only (forward)
//  toggle[3] = seg 2 only (strafe right)
//  toggle[4] = seg 3 only (forward)
//  toggle[5] = stop
// ============================================================================

// ============================================================================
// STATE
// ============================================================================

static bool          s_running     = false;
static int           s_currentStep = 0;
static int           s_lastStep    = -1;
static unsigned long s_stepStart   = 0;
static unsigned long s_lastWrite   = 0;

// Previous toggle states for rising edge detection.
// A mission only starts on the transition from false -> true,
// so holding a toggle down after the mission ends does nothing.
static bool s_prevToggles[6] = { false, false, false, false, false, false };

static const unsigned long WRITE_INTERVAL_MS = 20;

// ============================================================================
// PUBLIC FLAG
// ============================================================================

bool profile_group17_is_active() { return s_running; }

// ============================================================================
// HELPERS
// ============================================================================

static bool tryPushSpeeds(float fl, float fr, float bl, float br) {
  if (mutex_try_enter(&g_speedMutex, nullptr)) {
    g_targetSpeeds[0] = fl;  g_targetSpeeds[1] = fr;
    g_targetSpeeds[2] = bl;  g_targetSpeeds[3] = br;
    g_speedsUpdated = true;
    mutex_exit(&g_speedMutex);
    return true;
  }
  return false;
}

static void beginMission(int startSegment, int stopAfterSegment, unsigned long now) {
  s_running     = true;
  s_currentStep = startSegment;
  s_lastStep    = stopAfterSegment;
  s_stepStart   = now;
  s_lastWrite   = 0;
}

// Returns true only on the rising edge (was off, now on)
static bool risingEdge(int i, const control_command_t *cmd) {
  return cmd->toggles[i] && !s_prevToggles[i];
}

// ============================================================================
// MAIN ENTRY POINT
// ============================================================================

void profile_group17_apply(const control_command_t *cmd) {
  unsigned long now = millis();

  // Feed watchdog while running
  if (s_running) safety_feed();

  // --- STOP: toggle[5] rising edge or held ---
  // Stop is intentionally level-triggered (not edge) so holding it
  // always guarantees a stop regardless of previous state.
  if (cmd->toggles[5]) {
    s_running = false;
    tryPushSpeeds(0.0f, 0.0f, 0.0f, 0.0f);
    // Update previous states before returning
    for (int i = 0; i < 6; i++) s_prevToggles[i] = cmd->toggles[i];
    return;
  }

  // --- START: rising edge only so holding a toggle never restarts ---
  if (!s_running) {
    if      (risingEdge(0, cmd)) beginMission(0, -1, now);  // full mission
    else if (risingEdge(1, cmd)) beginMission(0,  0, now);  // seg 0 only
    else if (risingEdge(2, cmd)) beginMission(1,  1, now);  // seg 1 only
    else if (risingEdge(3, cmd)) beginMission(2,  2, now);  // seg 2 only
    else if (risingEdge(4, cmd)) beginMission(3,  3, now);  // seg 3 only
  }

  // Update previous toggle states for next call
  for (int i = 0; i < 6; i++) s_prevToggles[i] = cmd->toggles[i];

  if (!s_running) return;

  // --- Check if current step has finished ---
  if (now - s_stepStart >= SEGMENTS[s_currentStep].durationMs) {
    bool reachedEnd  = (s_lastStep != -1 && s_currentStep >= s_lastStep);
    bool missionDone = (s_lastStep == -1 && s_currentStep >= NUM_SEGMENTS - 1);

    if (reachedEnd || missionDone) {
      s_running = false;
      tryPushSpeeds(0.0f, 0.0f, 0.0f, 0.0f);
      return;
    }

    s_currentStep++;
    s_stepStart = now;
    s_lastWrite = 0;
    return;
  }

  // --- Throttle writes ---
  if (now - s_lastWrite < WRITE_INTERVAL_MS) return;

  // --- Push speeds to Core 1 ---
  const Segment &seg = SEGMENTS[s_currentStep];
  if (tryPushSpeeds(seg.fl, seg.fr, seg.bl, seg.br)) {
    s_lastWrite = now;
  }
}

#endif // MOTION_PROFILE_GROUP17
