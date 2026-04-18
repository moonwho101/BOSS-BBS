// boss.cpp — Main BBS session loop
// BOSS.BAS reference lines: 900–1200

#include "boss.h"
#include "console.h"
#include "users.h"
#include "menus.h"
#include "messages.h"
#include "files.h"
#include "sysop.h"
#include "timer.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <windows.h>

// ---------------------------------------------------------------------------
// Login / registration sequence (BASIC lines 350–870)
// Populates st with the logged-in user's data.
// Returns false if session should end (bad carrier, max-invalid, etc.)
// ---------------------------------------------------------------------------
static bool doLogin(BossState& st) {
    st.ansiOn = false;

    for (;;) {
        timerCheck(st);
        setColor(15, 0);
        printStr(st, "\r\nPlease Enter Your First and Last Name: ");

        // Read name into st.name1 / st.name2 (from users.h)
        readName(st);
        std::string first = st.name1;
        std::string last  = st.name2;

        if (first.empty() || last.empty()) {
            printLine(st, "\r\n\r\nFirst and Last Names MUST be entered!");
            continue;
        }
        if ((int)first.size() <= 1 || (int)last.size() <= 1) {
            printLine(st, "\r\n\r\nName is not long enough, Full name MUST be used!\r\n");
            continue;
        }

        setColor(14, 0);
        printLine(st, "\r\n\r\nScanning user list...");

        bool found = findUser(st, first, last);  // returns bool; sets st.userRecNum + loads user

        if (found) {
            // Set today's date as this session's date
            {
                time_t now = time(nullptr);
                tm* t = localtime(&now);
                char datebuf[12];
                snprintf(datebuf, sizeof(datebuf), "%02d-%02d-%04d",
                    t->tm_mon + 1, t->tm_mday, 1900 + t->tm_year);
                st.bbdate = datebuf;
            }
            if (!loginUser(st)) return false;  // password verification (3-strike lockout)
            loadLastRead(st);
            return true;
        }

        // New user — confirm name
        printStr(st, "\r\n\r\nYour Name is " + first + " " + last + ", is this correct [Y/n]: ");
        char yn = waitKey(st); timerResetInactivity(st);
        yn = (char)toupper((unsigned char)yn);
        printStr(st, std::string(1, yn));
        if (yn == 'N') continue;

        printStr(st, "\r\nHello " + first + "...");

        // Run full registration (sets st.bbdate, st.lcall, all fields, writes to USERS.BBS)
        registerNewUser(st);
        loadLastRead(st);
        return true;
    }
}

// ---------------------------------------------------------------------------
// Display status bar and HELLO.TXT / lastcall.txt (BASIC line 900)
// ---------------------------------------------------------------------------
static void doWelcome(BossState& st) {
    drawStatusBar(st);
    displayTextFile(st, std::string(BOSS_PATH) + "HELLO.TXT");
    clearIfEnabled(st);
    displayTextFile(st, std::string(BOSS_PATH) + "lastcall.txt");
    // Prompt at end of lastcall.txt
    setColor(14, 0);
    printStr(st, "\r\n[Press Return]: ");
    waitKey(st); timerResetInactivity(st);
    printLine(st, "");
}

// ---------------------------------------------------------------------------
// Quick-scan new messages since last read (BASIC line 3390)
// ---------------------------------------------------------------------------
static void quickScanNewMail(BossState& st) {
    int r = strVal(st.lread);
    checkMail(st, r);
}

// ---------------------------------------------------------------------------
// Main BBS dispatch loop (BASIC lines 930–1200)
// ---------------------------------------------------------------------------
void boss_main(BossState& st) {

    // --- Login ---
    // Display BOSS.TXT (intro screen)
    displayTextFile(st, std::string(BOSS_PATH) + "boss.txt");

    // Copyright banner (BASIC lines 240)
    setColor(15, 0);
    printLine(st, "  \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
    printLine(st, "  \xBA                   Boss Bulletin Board System, Ver 3.00                   \xBA");
    printLine(st, "  \xBA             Copyright (C) By Mark Longo, All Rights Reserved             \xBA");
    printLine(st, "  \xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");

    if (!doLogin(st)) return;

    // Security gate (BASIC line 900)
    if (strVal(st.secur) == 0) {
        setColor(12, 0);
        printLine(st, "\r\nYou do not have access to the first menu ... so BYE!\r\n");
        displayTextFile(st, std::string(BOSS_PATH) + "bye.txt");
        return;
    }

    // Welcome and scan mail
    doWelcome(st);
    quickScanNewMail(st);

    // Load first menu
    std::string menuFile = "menu0.mnu";
    st.know = true;

    // --- Main menu loop ---
    for (;;) {
        timerCheck(st);

        // Reload menu if changed (op=12 or sec level change)
        loadMenu(st, menuFile);
        displayMenuArt(st);

        // Build valid key list (filtered by secur)
        buildValidKeys(st);

        // Check for forced logoff
        if (st.flav) break;

        // Get choice
        showMenu(st);
        if (st.flav) break;

        std::string ant = st.sa;
        st.sa = "";

        // Look up op code
        int km = -1;
        for (int i = 0; i < st.menuLen; i++) {
            if (st.letter[i] == ant) { km = i; break; }
        }
        if (km < 0) continue;
        st.km = km;

        int op = strVal(st.op[km]);

        // Dispatch
        switch (op) {
        case 3: // Page sysop
            pageSysop(st);
            if (st.can_) {
                setColor(12, 0);
                st.no_ = true;
                printLine(st, "\r\n\r\nIf I heard you, i'll break in.");
            }
            st.can_ = false;
            break;

        case 4: // Check mail from last-read
        {
            int r = strVal(st.lread);
            checkMail(st, r);
            st.no_ = true;
        }
        break;

        case 5: // Check all mail
            setColor(12, 0);
            printLine(st, "\r\nChecking for LOST mail...");
            checkMail(st, 0);
            st.no_ = true;
            break;

        case 6: // Show user info
            printStr(st, "");
            showUserInfo(st);
            break;

        case 9: // Post message
            postMessage(st);
            break;

        case 10: // Read messages
            readMessages(st);
            // Stay on same menu after reading
            continue;

        case 11: // Goodbye + logoff
            displayTextFile(st, std::string(BOSS_PATH) + "bye.txt");
            saveUser(st);
            return;

        case 12: // Load new menu
            menuFile = st.menuDest[km];
            st.sec_ = false;
            continue; // re-enter loop with new menu

        case 13: // Display text file
            displayTextFile(st, st.menuDest[km]);
            break;

        case 14: // Toggle screen clearing
            toggleScreenClear(st);
            break;

        case 15: // Toggle ANSI
            toggleANSI(st);
            break;

        case 16: // Change password
            changePassword(st);
            break;

        case 17: // Change city
            changeCity(st);
            break;

        case 18: // Change data phone
            changeDataPhone(st);
            break;

        case 19: // Change voice phone
            changeVoicePhone(st);
            break;

        case 20: // Change birth date
            changeBirthDate(st);
            break;

        case 21: // Change screen length
            changeScreenLength(st);
            break;

        case 22: // Feed message to sysop (area 15)
        {
            // BASIC: fed%=1: first$="Mark": last$="Longo": GOSUB 5250
            st.fed = true;
            postMessage(st);
            st.fed = false;
        }
        break;

        case 23: // Scan users
            scanUsers(st);
            pressReturn(st);
            break;

        case 25: // Quick-scan messages
        {
            // qscan%=1
            st.qscan = true; st.qscanMode = 1;
            readMessages(st);
            st.qscan = false;
        }
        break;

        case 26: // Shell to external program (via DORINFO1.DEF)
        {
            clearIfEnabled(st);
            printLine(st, "Please wait, loading files.");
            loadFiles(st);
        }
        break;

        case 27: // Quick-scan file areas
        {
            st.qscan = true; st.qscanMode = 2;
            readMessages(st);
            st.qscan = false;
        }
        break;

        case 29: // Browse files
            browseFiles(st);
            break;

        case 30: // Search files
            searchFiles(st);
            break;

        case 31: // Download files
            st.cl = true;
            downloadFiles(st);
            st.cl = false;
            break;

        case 32: // Upload files
            st.cl = true;
            uploadFiles(st);
            st.cl = false;
            break;

        case 33: // New-files scan
            st.nsq = true;
            browseFiles(st);
            st.nsq = false;
            break;

        default:
            break;
        }

        // Post-dispatch reset
        st.abort_ = false;
        st.how    = 0;
        st.qscan  = false;
        st.nsq    = false;
        st.cl     = false;

        // If security changed, reload menu from top
        if (st.sec_) {
            st.sec_ = false;
            menuFile = "menu0.mnu";
            continue;
        }
    } // for (;;)

    // Session ended (time limit / carrier loss / logoff)
    saveUser(st);
}
