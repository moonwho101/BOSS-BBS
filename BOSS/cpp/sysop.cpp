// sysop.cpp — Sysop tools implementation
// BOSS.BAS reference lines: 3630–4220, 4710–4830, 11590–11680,
//                            13200–13244, 13300–13355

#include "sysop.h"
#include "console.h"
#include "serial.h"
#include "users.h"
#include "menus.h"
#include "timer.h"
#include "editor.h"
#include "fileio.h"
#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Phone input: exactly 10 digits formatted as [xxx]-[xxx-xxxx]
// Returns formatted string or "" on abort
static std::string inputPhone(BossState& st, const std::string& prompt, int color) {
    for (;;) {
        setColor(color, 0);
        printStr(st, "\r\n" + prompt);
        setColor(15, 0);
        printStr(st, "[");

        char digits[11];
        int len = 0;
        bool abort = false;
        for (;;) {
            timerCheck(st);
            char c = inkey(st);
            if (c == '\0') continue;
            timerResetInactivity(st);
            if (c == '\r') { if (len == 10) break; continue; }
            if (c == '\b' || c == 127) {
                if (len > 0) {
                    len--;
                    printBackspace(st);
                    // Also undo the bracket/dash echoes if needed
                }
                continue;
            }
            if (c < '0' || c > '9') continue;
            digits[len++] = c;
            char echo[2] = {c, 0};
            printStr(st, echo);
            if (len == 3) printStr(st, "]-[");
            else if (len == 6) printStr(st, "-");
            else if (len == 10) { printStr(st, "]"); break; }
        }
        if (len < 10) {
            setColor(14, 0);
            printLine(st, "\r\n\r\nYou must enter your phone number");
            continue;
        }
        digits[10] = '\0';
        // Format: "XXX-XXX-XXXX"
        char formatted[14];
        snprintf(formatted, sizeof(formatted),
            "%c%c%c-%c%c%c-%c%c%c%c",
            digits[0], digits[1], digits[2],
            digits[3], digits[4], digits[5],
            digits[6], digits[7], digits[8], digits[9]);
        return formatted;
    }
}

// City + Province input: "City, Province"
static std::string inputCity(BossState& st) {
    for (;;) {
        setColor(10, 0);
        printStr(st, "\r\nEnter your City and Province [ie Cambridge, On]: ");
        // Read two words separated by space/comma
        std::string first, last;
        // Use editorReadLine for simplicity, split on comma or space
        std::string line = editorReadLine(st, 30);
        if (line.empty()) continue;
        // Split on comma
        size_t comma = line.find(',');
        if (comma != std::string::npos) {
            first = line.substr(0, comma);
            last  = line.substr(comma + 1);
            // Trim spaces
            while (!last.empty() && last[0] == ' ') last = last.substr(1);
        } else {
            // Split on last space
            size_t sp = line.rfind(' ');
            if (sp != std::string::npos) {
                first = line.substr(0, sp);
                last  = line.substr(sp + 1);
            } else {
                first = line;
                last  = "";
            }
        }
        if (first.size() < 2 || last.size() < 2) {
            setColor(14, 0);
            printLine(st, "\r\n\r\nYou must enter your City AND Province!");
            continue;
        }
        return first + "," + last;
    }
}

// Birth date input: "mm-dd-19yy"
static std::string inputBirthDate(BossState& st) {
    for (;;) {
        setColor(11, 0);
        printStr(st, "\r\nEnter your birth date [mm-dd-yy]: ");
        setColor(15, 0);
        printStr(st, "[");

        char d[7] = {};
        int len = 0;
        for (;;) {
            timerCheck(st);
            char c = inkey(st);
            if (c == '\0') continue;
            timerResetInactivity(st);
            if (c == '\r') { if (len == 6) break; continue; }
            if (c == '\b' || c == 127) {
                if (len > 0) { len--; printBackspace(st); } continue;
            }
            if (c < '0' || c > '9') continue;
            d[len++] = c;
            char echo[2] = {c, 0};
            printStr(st, echo);
            if (len == 2) printStr(st, "-");
            else if (len == 4) printStr(st, "-");
            else if (len == 6) { printStr(st, "]"); break; }
        }
        if (len < 6) {
            setColor(14, 0);
            printLine(st, "\r\n\r\nYou Must Enter Your Birth Date");
            continue;
        }
        // Validate
        int mm = (d[0]-'0')*10+(d[1]-'0');
        int dd = (d[2]-'0')*10+(d[3]-'0');
        int yy = (d[4]-'0')*10+(d[5]-'0');
        if (mm<1||mm>12) { setColor(14,0); printLine(st,"\r\n\r\nInvalid Month!"); continue; }
        if (dd<1||dd>31) { setColor(14,0); printLine(st,"\r\n\r\nInvalid Day!");   continue; }
        if (yy<0||yy>99) { setColor(14,0); printLine(st,"\r\n\r\nInvalid Year!");  continue; }
        char buf[12];
        snprintf(buf, sizeof(buf), "%02d-%02d-19%02d", mm, dd, yy);
        return buf;
    }
}

// ---------------------------------------------------------------------------
// Page sysop (BASIC line 4710)
// ---------------------------------------------------------------------------
void pageSysop(BossState& st) {
    st.can_ = true;
    setColor(12, 0);
    printStr(st, "\r\nPaging Mark Longo: ");
    Beep(880, 100); Beep(784, 100); Beep(587, 100); Beep(698, 100);
    setColor(11, 0);

    for (int z = 0; z <= 3 && st.can_; z++) {
        for (int i = 0; i < 200 && st.can_; i++) {
            timerCheck(st);
            char c = inkey(st);
            if (c != '\0') {
                c = (char)toupper((unsigned char)c);
                if (c == 'C') {
                    sysopChat(st);
                    st.can_ = false;
                    return;
                }
                timerResetInactivity(st);
            }
        }
        char zstr[12];
        snprintf(zstr, sizeof(zstr), "%d \xB0\xB0 ", z + 1);
        printStr(st, zstr);
        Beep(880, 100); Beep(784, 100); Beep(587, 100); Beep(698, 100);
    }
    // st.can_ remains true = not answered
}

// ---------------------------------------------------------------------------
// Sysop chat (BASIC line 3630)
// ---------------------------------------------------------------------------
void sysopChat(BossState& st) {
    setColor(14, 0);
    printLine(st, "\r\n\r\n\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD  Mark is here to chat  \xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xB8");
    setColor(15, 0);

    // Determine local vs. remote typing mode
    // local output: white (15), remote output: green (10)
    // ESC ends chat

    st.can_ = false;
    bool localTurn = true;   // last input source: true=local, false=remote
    std::string lineBuffer;
    int colPos = 0;

    auto flushLine = [&]() {
        printLine(st, "");
        lineBuffer.clear();
        colPos = 0;
    };
    auto wordWrap = [&](char ch) {
        lineBuffer += ch;
        colPos++;
        if (colPos >= 78) {
            // Find last space to wrap
            size_t sp = lineBuffer.rfind(' ');
            if (sp != std::string::npos) {
                std::string carry = lineBuffer.substr(sp + 1);
                lineBuffer = lineBuffer.substr(0, sp);
                printLine(st, "");
                lineBuffer = carry;
                colPos = (int)carry.size();
            } else {
                printLine(st, "");
                lineBuffer.clear();
                colPos = 0;
            }
        }
    };

    for (;;) {
        timerCheck(st);

        // Check remote input (serial)
        char remote = '\0';
        if (!st.loca) {
            remote = inkey(st); // inkey polls serial too in console.cpp
        }
        // Check local keyboard
        char local = '\0';
        if (_kbhit()) {
            local = (char)_getch();
        }

        if (remote == '\0' && local == '\0') continue;

        char ch = '\0';
        bool fromLocal = false;
        if (local != '\0') { ch = local; fromLocal = true; }
        else { ch = remote; fromLocal = false; }

        timerResetInactivity(st);

        if (ch == 27) { // ESC — end chat
            setColor(14, 0);
            printLine(st, "\r\n\r\n\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD  Mark has ended chat.  \xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
            return;
        }

        // Switch color based on who is typing
        if (fromLocal && !localTurn) {
            setColor(15, 0); // local = white
            localTurn = true;
        } else if (!fromLocal && localTurn) {
            setColor(10, 0); // remote = green
            localTurn = false;
        }

        if (ch == '\r' || ch == '\n') {
            flushLine();
        } else if (ch == '\b' || ch == 127) {
            if (colPos > 0) {
                printBackspace(st);
                if (!lineBuffer.empty()) { lineBuffer.pop_back(); colPos--; }
            }
        } else if (ch >= 32) {
            char echo[2] = {ch, 0};
            printStr(st, echo);
            wordWrap(ch);
        }
    }
}

// ---------------------------------------------------------------------------
// Change security level — local sysop console (BASIC line 11590)
// ---------------------------------------------------------------------------
void changeSecurityLevel(BossState& st) {
    st.sec2 = true;
    printLine(st, "\r\nSysOp is updating your security...");
    // This input is local-only (console)
    printf("\nEnter Security Level [1-99999]: ");
    fflush(stdout);
    char buf[8] = {};
    int len = 0;
    for (;;) {
        if (!_kbhit()) { Sleep(10); continue; }
        char c = (char)_getch();
        if (c == '\r') break;
        if ((c == '\b' || c == 127) && len > 0) {
            len--;
            printf("\b \b"); fflush(stdout);
            continue;
        }
        if (c >= '0' && c <= '9' && len < 5) {
            buf[len++] = c;
            printf("%c", c); fflush(stdout);
        }
    }
    buf[len] = '\0';
    if (len > 0) st.secur = buf;
    setColor(15, 0);
    printLine(st, "\r\nThanks for waiting...");
}

// ---------------------------------------------------------------------------
// Toggle screen clearing (BASIC line 13200)
// ---------------------------------------------------------------------------
void toggleScreenClear(BossState& st) {
    setColor(14, 0);
    printStr(st, "");
    if (st.xpt == "Y") {
        printLine(st, "\xFB Screen clearing is now Off.");
        st.xpt = "N";
    } else {
        st.no_ = true;
        printLine(st, "\xFB Screen clearing is now On.");
        st.xpt = "Y";
    }
}

// ---------------------------------------------------------------------------
// Toggle ANSI graphics (BASIC line 13205)
// ---------------------------------------------------------------------------
void toggleANSI(BossState& st) {
    setColor(14, 0);
    printStr(st, "");
    if (st.ansiOn) {
        printLine(st, "\xFB ANSI graphics are now Off.");
        st.ansiOn = false;
    } else {
        st.no_ = true;
        printLine(st, "\xFB ANSI graphics are now On.");
        st.ansiOn = true;
    }
}

// ---------------------------------------------------------------------------
// Change password (BASIC line 13210)
// ---------------------------------------------------------------------------
void changePassword(BossState& st) {
    setColor(14, 0);
    printStr(st, "\r\n\r\nEnter Your Current Password: ");
    std::string pw1 = editorReadLine(st, 15, /*password=*/true);
    pw1 = stripBlanks(pw1);
    if (pw1 != st.password) {
        setColor(12, 0);
        printLine(st, "\r\n\r\nIncorrect Password!");
        return;
    }
    for (;;) {
        setColor(11, 0);
        printStr(st, "\r\n\r\nEnter your New Password [#'s will echo.]: ");
        std::string newpw = editorReadLine(st, 15, /*password=*/true);
        if ((int)newpw.size() <= 2) {
            setColor(14, 0);
            printLine(st, "\r\n\r\nPassword is not long enough!\r\n");
            continue;
        }
        setColor(10, 0);
        printStr(st, "\r\n\r\nType your New Password again to Verify [#'s will echo.]: ");
        std::string confirm = editorReadLine(st, 15, /*password=*/true);
        if (confirm == newpw) {
            st.password = newpw;
            setColor(12, 0);
            printLine(st, "\r\n\r\nCorrect, don't forget it!\r\n");
            setColor(11, 0);
            printLine(st, "Password change completed...");
            st.no_ = true;
            return;
        }
        setColor(14, 0);
        printLine(st, "\r\n\r\nIncorrect, enter your Password again...");
    }
}

// ---------------------------------------------------------------------------
// Change city (BASIC line 13240 → GOSUB 580)
// ---------------------------------------------------------------------------
void changeCity(BossState& st) {
    st.no_ = true;
    st.city = inputCity(st);
    setColor(14, 0);
    printLine(st, "\r\n\r\n\xFB City and Province Change Complete.");
}

// ---------------------------------------------------------------------------
// Change data phone (BASIC line 13241 → GOSUB 500)
// ---------------------------------------------------------------------------
void changeDataPhone(BossState& st) {
    st.no_ = true;
    st.phone1 = inputPhone(st, "Enter your DATA phone [XXX]-[XXX-XXXX]: ", 10);
    setColor(14, 0);
    printLine(st, "\r\n\r\n\xFB Data Phone Changed.");
}

// ---------------------------------------------------------------------------
// Change voice phone (BASIC line 13242 → GOSUB 540)
// ---------------------------------------------------------------------------
void changeVoicePhone(BossState& st) {
    st.no_ = true;
    st.phone2 = inputPhone(st, "Enter your VOICE phone [XXX]-[XXX-XXXX]: ", 11);
    setColor(14, 0);
    printLine(st, "\r\n\r\n\xFB Voice Phone Changed.");
}

// ---------------------------------------------------------------------------
// Change birth date (BASIC line 13243 → GOSUB 620)
// ---------------------------------------------------------------------------
void changeBirthDate(BossState& st) {
    st.no_ = true;
    st.bdate = inputBirthDate(st);
    setColor(14, 0);
    printLine(st, "\r\n\r\n\xFB Birth Date Changed.");
}

// ---------------------------------------------------------------------------
// Change screen length (BASIC line 13244 → GOSUB 690)
// ---------------------------------------------------------------------------
void changeScreenLength(BossState& st) {
    for (;;) {
        setColor(10, 0);
        printStr(st, "\r\nEnter Your Screen Length [10-66] [24 recommended]: ");
        std::string val = editorReadLine(st, 2);
        int v = strVal(val);
        if (v < 10 || v > 66) {
            setColor(14, 0);
            printLine(st, "\r\n\r\nYour screen length must be between 10 and 66.");
            continue;
        }
        st.sl = val;
        st.no_ = true;
        setColor(14, 0);
        printLine(st, "\r\n\r\n\xFB Screen Length Changed.");
        return;
    }
}

// ---------------------------------------------------------------------------
// Shell to DOS (BASIC line 13300)
// F5/F6 sysop hotkey
// ---------------------------------------------------------------------------
void shellToDos(BossState& st) {
    setColor(14, 0);
    printLine(st, "\r\n\xFB SysOp Has Shelled To Dos, Please wait...");
    // Launch cmd.exe
    PROCESS_INFORMATION pi; STARTUPINFOA si; memset(&si, 0, sizeof(si)); si.cb = sizeof(si);
    char cmd[] = "cmd.exe";
    if (CreateProcessA(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, BOSS_PATH, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    st.fx  = true;
    setColor(14, 0);
    printLine(st, "\r\n\xFB SysOp Is Back from Dos...");
}

// ---------------------------------------------------------------------------
// Scan users (BASIC line 13350)
// ---------------------------------------------------------------------------
void scanUsers(BossState& st) {
    clearIfEnabled(st);

    setColor(14, 0);
    printLine(st, "Locating A BOSS User");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4");

    setColor(11, 0);
    printStr(st, "\r\nEnter PARTIAL NAME or RETURN for complete list: ");
    std::string who = editorReadLine(st, 19);
    toUpper(who);

    bool partial = !who.empty();
    if (partial) {
        setColor(10, 0);
        printLine(st, "\r\n\r\nScanning users...");
    }

    setColor(10, 0);
    printLine(st, "\r\n\r\nPress [S]top [P]ause.\r\n");
    setColor(14, 0);
    printLine(st, "\r\n     User Name                        City                   Last Logon");
    printLine(st, "     \xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4                        \xC4\xC4\xC4\xC4                   \xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");

    // Open USERS.BBS
    std::string upath = std::string(BOSS_PATH) + "USERS.BBS";
    FILE* fp = fopen(upath.c_str(), "rb");
    if (!fp) {
        setColor(12, 0);
        printLine(st, "[Cannot open users file]");
        return;
    }

    // Read user count from record 1 (first 10 bytes)
    char rec[USERS_REC_LEN];
    memset(rec, ' ', sizeof(rec));
    fread(rec, 1, USERS_REC_LEN, fp);
    char numbuf[20] = {};
    strncpy(numbuf, rec, 10); numbuf[10] = '\0';
    int total = strVal(numbuf);

    int slLen = strVal(st.sl); if (slLen < 10) slLen = 24;
    int how = 3;
    st.abort_ = false;

    for (int rec2 = 2; rec2 <= total + 1 && !st.abort_; rec2++) {
        timerCheck(st);
        memset(rec, ' ', sizeof(rec));
        fseek(fp, (long)(rec2 - 1) * USERS_REC_LEN, SEEK_SET);
        if (fread(rec, 1, USERS_REC_LEN, fp) < USERS_REC_LEN) break;

        // N1=19, N2=19, P=15, C=19, ...
        char n1[20], n2[20], city[20], lc[12];
        strncpy(n1, rec, 19); n1[19] = '\0';
        strncpy(n2, rec + 19, 19); n2[19] = '\0';
        strncpy(city, rec + 53, 19); city[19] = '\0'; // offset 19+19+15=53
        strncpy(lc, rec + 53+19+12+12+1+10+2+5+5+5+5+4+5, 10); lc[10] = '\0'; // LC$ offset

        std::string fn1 = strTrim(n1), fn2 = strTrim(n2);
        std::string fc = strTrim(city), flc = strTrim(lc);

        if (partial) {
            std::string full = fn1 + fn2;
            toUpper(full);
            if (full.find(who) == std::string::npos) {
                // Also check [P]/[S] keys
                char key = inkey(st);
                if (key != '\0') {
                    key = (char)toupper((unsigned char)key);
                    if (key == 'S') { st.abort_ = true; break; }
                    if (key == 'P') { waitKey(st); timerResetInactivity(st); }
                }
                continue;
            }
        }

        char row[100];
        snprintf(row, sizeof(row), "     %-28s %-20s %-12s",
            (fn1 + " " + fn2).c_str(), fc.c_str(), flc.c_str());
        setColor(11, 0);
        printLine(st, row);

        how++;
        if (how >= slLen) {
            how = 0;
            setColor(14, 0);
            printStr(st, "More [Y/n]: ");
            char yn = waitKey(st); timerResetInactivity(st);
            yn = (char)toupper((unsigned char)yn);
            for (int k = 0; k < 13; k++) printBackspace(st);
            if (yn == 'N') { st.abort_ = true; break; }
        }

        char key = inkey(st);
        if (key != '\0') {
            key = (char)toupper((unsigned char)key);
            if (key == 'S') { st.abort_ = true; break; }
            if (key == 'P') { waitKey(st); timerResetInactivity(st); }
        }
    }
    fclose(fp);

    printStr(st, "");
    pressReturn(st);
}
