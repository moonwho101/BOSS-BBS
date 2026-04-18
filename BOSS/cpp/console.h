#pragma once
// console.h — Win32 console + serial dual-output layer
// Maps BASIC COLOR/LOCATE/CLS/VIEW PRINT/PRINT/INKEY$ to Win32 Console API.
// BOSS.BAS lines: 1212, 4870, 5040, 5120, 5180, 6200, 2550–2560

#include "boss.h"
#include <string>

// ---- Colour mapping (BASIC colour index → Win32 attribute) ---------------
// BASIC fg indices: 9=blue 10=green 11=cyan 12=red 13=magenta 14=yellow 15=white
// plus background 0=black 1=blue

void consoleInit();

// COLOR v, bg  (BASIC line 1212)
void setColor(int fg, int bg = 0);

// LOCATE row, col  (1-based)
void locate(int row, int col);

// CLS page  (page 2 = secondary screen buffer used by BOSS)
void cls();

// VIEW PRINT top TO bot  — set scroll region (no-op on Win32 console, tracked in state)
void viewPrint(int top = 1, int bot = 25);

// Print string inline (no newline) to local console and optionally serial
void printStr(BossState& st, const std::string& s);
// Print string + newline
void printLine(BossState& st, const std::string& s);

// T$ output functions matching BASIC's t$ / tk% pattern
// tk%=1 → printStr (no newline)
// tk%=0 → printLine (with newline)
void printT(BossState& st, const std::string& t, bool noNewline);

// Non-blocking key read: returns '\0' if no key available
// Checks local _kbhit()/_getch() first; if not local also polls serial
char inkey(BossState& st);

// Blocking: wait for any key, return it
char waitKey(BossState& st);

// Status bar update (line 4870)
void drawStatusBar(BossState& st);

// Header bar (line 5120)
void drawHeader();

// CH$ overlay in corner (line 5180)
void drawCornerTag(const std::string& tag);

// Screen-clear if xpt$="Y" (line 6200)
void clearIfEnabled(BossState& st);

// Backspace display helper (lines 2550/2560)
void printChar(BossState& st, char c);
void printBackspace(BossState& st);

// Get current console cursor position
void getCursorPos(int& row, int& col);

// ANSI escape string for a colour (sent to remote when ANSI$="Y")
std::string ansiColor(int v);

// Print using PRINT USING format$ with a single string argument
std::string printUsing(const std::string& fmt, const std::string& val);
