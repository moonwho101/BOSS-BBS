#pragma once
// serial.h — Win32 COM-port wrapper
// Maps BASIC lines 239, 12690–12865, 13000–13001

#include "boss.h"
#include <string>

// Open the serial port (e.g. "COM1") at the given baud rate.
// Returns true on success; sets st.hCom.
bool openSerial(BossState& st, const std::string& port, int baud);

// Close the serial port.
void closeSerial(BossState& st);

// Write a string to the serial port (non-blocking write).
void writeSerial(BossState& st, const std::string& s);

// Read up to `max` bytes; returns actual bytes read.
// timeoutMs=0 → non-blocking poll.
int readSerial(BossState& st, char* buf, int max, int timeoutMs = 100);

// Read exactly one byte; returns true if a byte was available within timeoutMs.
bool readSerialByte(BossState& st, char& c, int timeoutMs = 100);

// Check if carrier detect (RLSD/DCD) is present.
bool carrierPresent(BossState& st);

// Drop DTR to hang up (BASIC: OUT &H3FC, 0 ... OUT &H3FC, 1)
void dropCarrier(BossState& st);

// Restore DTR after hangup.
void raiseDTR(BossState& st);

// Full modem init: send ATZ + AT init string, wait for OK.
// BOSS.BAS lines 12700-12790.
void initModem(BossState& st);

// Wait in a loop for RING, then detect baud from CONNECT string.
// Sets st.baud, st.baudVal, st.bv, st.delay_.
// BOSS.BAS lines 12795–12865.
void waitForCall(BossState& st);
