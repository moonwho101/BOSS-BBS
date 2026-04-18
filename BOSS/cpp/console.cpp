// console.cpp — Win32 console implementation
// BOSS.BAS reference lines: 1212, 2530, 2550, 2560, 4870, 5040, 5120, 5180, 6200

#include "console.h"
#include "serial.h"
#include <conio.h>
#include <cstdio>
#include <cassert>
#include <sstream>
#include <iomanip>

// Win32 colour attribute table indexed by BASIC colour number 0-15
static const WORD kColorAttr[16] = {
    0,                                                          // 0  Black
    FOREGROUND_BLUE,                                            // 1  Blue
    FOREGROUND_GREEN,                                           // 2  Green
    FOREGROUND_GREEN | FOREGROUND_BLUE,                         // 3  Cyan
    FOREGROUND_RED,                                             // 4  Red
    FOREGROUND_RED   | FOREGROUND_BLUE,                         // 5  Magenta
    FOREGROUND_RED   | FOREGROUND_GREEN,                        // 6  Brown/Yellow
    FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE,      // 7  White
    FOREGROUND_INTENSITY,                                       // 8  Dark Grey
    FOREGROUND_BLUE  | FOREGROUND_INTENSITY,                    // 9  Bright Blue
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,                    // 10 Bright Green
    FOREGROUND_GREEN | FOREGROUND_BLUE  | FOREGROUND_INTENSITY, // 11 Bright Cyan
    FOREGROUND_RED   | FOREGROUND_INTENSITY,                    // 12 Bright Red
    FOREGROUND_RED   | FOREGROUND_BLUE  | FOREGROUND_INTENSITY, // 13 Bright Magenta
    FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY, // 14 Bright Yellow
    FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY // 15 Bright White
};

// Background colour bits shifted left 4 positions
static const WORD kBgAttr[16] = {
    0,                                    // 0 Black
    BACKGROUND_BLUE,                      // 1 Blue
    BACKGROUND_GREEN,                     // 2 Green
    BACKGROUND_GREEN | BACKGROUND_BLUE,   // 3 Cyan
    BACKGROUND_RED,                       // 4 Red
    BACKGROUND_RED   | BACKGROUND_BLUE,   // 5 Magenta
    BACKGROUND_RED   | BACKGROUND_GREEN,  // 6 Brown
    BACKGROUND_RED   | BACKGROUND_GREEN | BACKGROUND_BLUE, // 7 White
    BACKGROUND_INTENSITY,                 // 8 Dark grey bg
    BACKGROUND_BLUE  | BACKGROUND_INTENSITY,
    BACKGROUND_GREEN | BACKGROUND_INTENSITY,
    BACKGROUND_GREEN | BACKGROUND_BLUE  | BACKGROUND_INTENSITY,
    BACKGROUND_RED   | BACKGROUND_INTENSITY,
    BACKGROUND_RED   | BACKGROUND_BLUE  | BACKGROUND_INTENSITY,
    BACKGROUND_RED   | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
    BACKGROUND_RED   | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY
};

static HANDLE hOut = INVALID_HANDLE_VALUE;
static int gScrollTop = 1, gScrollBot = 25;

// ---------------------------------------------------------------------------
void consoleInit() {
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // Enable VT processing for ANSI passthrough to remote
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    // Set 80×25 window
    COORD size = { 80, 25 };
    SetConsoleScreenBufferSize(hOut, size);
}

// ---------------------------------------------------------------------------
void setColor(int fg, int bg) {
    if (fg < 0 || fg > 15) fg = 15;
    if (bg < 0 || bg > 15) bg = 0;
    SetConsoleTextAttribute(hOut, kColorAttr[fg] | kBgAttr[bg]);
}

// ---------------------------------------------------------------------------
void locate(int row, int col) {
    COORD c;
    c.X = (SHORT)(col - 1);
    c.Y = (SHORT)(row - 1);
    SetConsoleCursorPosition(hOut, c);
}

// ---------------------------------------------------------------------------
void cls() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    COORD origin = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacterA(hOut, ' ', size, origin, &written);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, size, origin, &written);
    SetConsoleCursorPosition(hOut, origin);
}

// ---------------------------------------------------------------------------
void viewPrint(int top, int bot) {
    gScrollTop = top;
    gScrollBot = bot;
    // On Win32 console true scroll regions aren't directly supported;
    // we track top/bot for the status-bar lines and handle paging in software.
}

// ---------------------------------------------------------------------------
std::string ansiColor(int v) {
    // Map BASIC colour index to ANSI SGR code (bold + colour)
    static const char* ansiCode[16] = {
        "0;30","0;34","0;32","0;36","0;31","0;35","0;33","0;37",
        "1;30","1;34","1;32","1;36","1;31","1;35","1;33","1;37"
    };
    if (v < 0 || v > 15) v = 15;
    std::string s = "\x1B[";
    s += ansiCode[v];
    s += "m";
    return s;
}

// ---------------------------------------------------------------------------
// Core output — sends to local console, and if not localMode also to serial
// ---------------------------------------------------------------------------
static void rawOut(BossState& st, const std::string& s) {
    // Local console
    DWORD written;
    WriteConsoleA(hOut, s.c_str(), (DWORD)s.size(), &written, nullptr);
    // Serial / remote
    if (!st.localMode && st.hCom != INVALID_HANDLE_VALUE) {
        writeSerial(st, s);
    }
}

static void rawOutLine(BossState& st, const std::string& s) {
    rawOut(st, s);
    rawOut(st, "\r\n");
}

// ---------------------------------------------------------------------------
void printStr(BossState& st, const std::string& s) {
    // If ANSI is on and there is a pending colour change, emit ANSI code first
    // (The caller sets st.v before calling printT; we do this transparently)
    rawOut(st, s);
}

void printLine(BossState& st, const std::string& s) {
    rawOutLine(st, s);
}

void printT(BossState& st, const std::string& t, bool noNewline) {
    if (noNewline)
        printStr(st, t);
    else
        printLine(st, t);
}

// ---------------------------------------------------------------------------
void printChar(BossState& st, char c) {
    char buf[2] = { c, '\0' };
    rawOut(st, std::string(1, c));
}

void printBackspace(BossState& st) {
    // BASIC CHR$(29) = cursor-left, CHR$(8) = backspace
    // We use backspace-space-backspace
    rawOut(st, "\b \b");
    if (!st.localMode && st.hCom != INVALID_HANDLE_VALUE) {
        // Already written by rawOut through serial
    }
}

// ---------------------------------------------------------------------------
char inkey(BossState& st) {
    // Check local keyboard first
    if (_kbhit()) {
        int c = _getch();
        if (c == 0 || c == 0xE0) {
            _getch(); // discard extended key scan code
            return '\0';
        }
        return (char)c;
    }
    // Check serial if connected
    if (!st.localMode && st.hCom != INVALID_HANDLE_VALUE) {
        char c = '\0';
        if (readSerialByte(st, c, 0))
            return c;
    }
    return '\0';
}

char waitKey(BossState& st) {
    char c = '\0';
    while (c == '\0')
        c = inkey(st);
    return c;
}

// ---------------------------------------------------------------------------
void getCursorPos(int& row, int& col) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    row = csbi.dwCursorPosition.Y + 1;  // 1-based
    col = csbi.dwCursorPosition.X + 1;
}

// ---------------------------------------------------------------------------
// Status bar — BOSS.BAS line 4870
// Saves cursor, draws lines 23/25 (outside scroll region), restores cursor.
void drawStatusBar(BossState& st) {
    int savedRow, savedCol;
    getCursorPos(savedRow, savedCol);

    setColor(14, 1); // bright yellow on blue
    locate(25, 60); rawOut(st, "                     ");

    locate(1, 23);
    char buf[80];
    std::snprintf(buf, sizeof(buf), "Posts: %-5s", st.tpost.c_str());
    rawOut(st, buf);
    locate(1, 36);
    std::snprintf(buf, sizeof(buf), "Ul: %-6s", st.ulkb.c_str());
    rawOut(st, buf);
    locate(1, 49);
    std::snprintf(buf, sizeof(buf), "Dl: %-6s", st.dlkb.c_str());
    rawOut(st, buf);
    locate(1, 62);
    std::snprintf(buf, sizeof(buf), "Called: %-8s", st.lcall.c_str());
    rawOut(st, buf);

    locate(23, 1);
    std::string nameLine = "Name: " + st.name1 + " " + st.name2 + " [" + st.baud + "]";
    rawOut(st, nameLine);
    locate(23, 34);
    rawOut(st, "City: " + st.city);
    locate(23, 59);
    rawOut(st, "Age: " + st.tyear);

    locate(25, 1);
    std::snprintf(buf, sizeof(buf), "Voice: %-14s  Data: %-13s  Lvl: %-5s  #%s",
        st.phone2.c_str(), st.phone1.c_str(), st.secur.c_str(), st.usernum.c_str());
    rawOut(st, buf);

    setColor(15, 0);
    locate(savedRow, savedCol);
}

// ---------------------------------------------------------------------------
// Header bar — BOSS.BAS line 5120
void drawHeader() {
    if (hOut == INVALID_HANDLE_VALUE) return;
    setColor(14, 1);
    locate(1, 1);
    // 80-char line
    WriteConsoleA(hOut,
        " BOSS BULLETIN BOARD                                                              ",
        80, nullptr, nullptr);
    setColor(15, 1);
    for (int i = 1; i <= 80; i++) {
        COORD c = { (SHORT)(i - 1), 1 };   // row 2 (0-based = 1)
        SetConsoleCursorPosition(hOut, c);
        WriteConsoleA(hOut, "\xCD", 1, nullptr, nullptr);  // ═ (box line)
        c.Y = 21;  // row 22
        SetConsoleCursorPosition(hOut, c);
        WriteConsoleA(hOut, "\xCD", 1, nullptr, nullptr);
    }
}

// ---------------------------------------------------------------------------
// Corner tag — BOSS.BAS line 5180
void drawCornerTag(const std::string& tag) {
    int savedRow, savedCol;
    getCursorPos(savedRow, savedCol);
    setColor(15, 1);
    locate(23, 70);
    WriteConsoleA(hOut, tag.c_str(), (DWORD)tag.size(), nullptr, nullptr);
    setColor(15, 0);
    locate(savedRow, savedCol);
}

// ---------------------------------------------------------------------------
// Clear screen if enabled (xpt$="Y") — BOSS.BAS line 6200
void clearIfEnabled(BossState& st) {
    if (st.no_) { st.no_ = false; return; }
    if (st.xpt == "N") {
        // Just emit a blank line
        printLine(st, "");
        return;
    }
    cls();
    if (!st.localMode && st.hCom != INVALID_HANDLE_VALUE) {
        // Send form-feed to remote terminal
        writeSerial(st, std::string(1, '\x0C'));
    }
}

// ---------------------------------------------------------------------------
// PRINT USING — handles "\  \" (fixed-width string field) only
// (BOSS only uses string fields)
std::string printUsing(const std::string& fmt, const std::string& val) {
    // Find first "\" ... "\" pair
    size_t start = fmt.find('\\');
    if (start == std::string::npos) return val;
    size_t end = fmt.find('\\', start + 1);
    if (end == std::string::npos) return val;
    int width = (int)(end - start + 1);
    if ((int)val.size() >= width)
        return val.substr(0, width);
    return val + std::string(width - val.size(), ' ');
}
