#pragma once
// timer.h — Session timer and inactivity watchdog
// BOSS.BAS reference lines: 13050–13052

#include "boss.h"

// Initialize timer: set session start time and allowed minutes.
// BOSS.BAS line 13052: ZT# = TIMER : PT% = 0 : QT% = 0
void timerInit(BossState& st, int remainMinutes);

// Check timers: inactivity logout, time-remaining warnings, carrier loss.
// Should be called frequently (in input wait loops).
// BOSS.BAS line 13050 ON TIMER(60) GOSUB
void timerCheck(BossState& st);

// Return remaining session minutes.
int timerRemaining(BossState& st);

// Reset inactivity counter.
// BOSS.BAS: PS% = 0
void timerResetInactivity(BossState& st);
