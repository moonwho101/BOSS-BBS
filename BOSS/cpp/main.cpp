// main.cpp — BOSS BBS entry point and modem loop
// BOSS.BAS reference lines: 10–239, 12690–12865

#include "boss.h"
#include "console.h"
#include "serial.h"
#include "users.h"
#include "menus.h"
#include "timer.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <windows.h>
#include <direct.h>   // _chdir

// Forward declaration of boss_main (defined in boss.cpp)
void boss_main(BossState& st);

// ---------------------------------------------------------------------------
// Parse command-line arguments
// Flags: -local, -port COMn, -baud NNNN
// ---------------------------------------------------------------------------
static void parseArgs(int argc, char* argv[], BossState& st) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-local" || arg == "--local") {
            st.loca = 1;
        } else if ((arg == "-port" || arg == "--port") && i + 1 < argc) {
            st.comPort = argv[++i];
        } else if ((arg == "-baud" || arg == "--baud") && i + 1 < argc) {
            st.baud = argv[++i];
            st.bv   = strVal(st.baud) >= 2400 ? 2 : 1;
        }
    }
}

// ---------------------------------------------------------------------------
// Modem init + wait-for-call sequence (BASIC lines 12690–12865)
// Called after each logoff to await the next caller.
// ---------------------------------------------------------------------------
static bool waitForCaller(BossState& st) {
    // Open COM port
    openSerial(st, st.comPort.c_str(), strVal(st.baud));
    if (st.hCom == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "[serial] Cannot open %s — aborting.\n", st.comPort.c_str());
        return false;
    }

    // Check if carrier is already present (previous call still connected)
    if (carrierPresent(st)) {
        dropCarrier(st);
        Sleep(500);
    }

    // Init modem: ATZ then profile
    initModem(st);

    printf("\xFB Waiting for call on %s at %s bps...\n", st.comPort.c_str(), st.baud.c_str());
    fflush(stdout);

    // waitForCall blocks until CONNECT is received
    waitForCall(st);
    return true;
}

// ---------------------------------------------------------------------------
// Reset BossState for a new call (BASIC: CLEAR, GOTO 32)
// ---------------------------------------------------------------------------
static void resetForNewCall(BossState& st) {
    bool savedLoca    = st.loca;
    bool savedSerial  = (st.hCom != INVALID_HANDLE_VALUE);
    HANDLE hSave      = st.hCom;
    std::string savedPort = st.comPort;
    std::string savedBaud = st.baud;
    int savedBv = st.bv;

    // Full struct reset
    st = BossState{};

    // Restore connection state
    st.loca    = savedLoca;
    st.hCom    = hSave;
    st.comPort = savedPort;
    st.baud    = savedBaud;
    st.bv      = savedBv;

    // Default BASIC line 32 values
    st.sl     = "23";
    st.remain = 60;
}

// ---------------------------------------------------------------------------
// main()
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    // Change to BOSS directory (equivalent to CHDIR "c:\boss")
    _chdir(BOSS_PATH);

    // Initialise state
    BossState st{};
    st.sl     = "23";
    st.remain = 60;
    st.baud   = "2400";
    st.bv     = 2;

    // Parse args
    parseArgs(argc, argv, st);

    // Initialise Win32 console
    consoleInit();

    // Set default COM port if not specified
    if (st.comPort.empty()) st.comPort = "COM1";

    // Main BBS loop — keep re-running after each logoff/hangup
    for (;;) {
        // -------- Connection phase --------
        if (!st.loca) {
            if (!waitForCaller(st)) break;
        } else {
            // Local login: no serial needed
            st.delay_ = 0;
        }

        // -------- Session phase --------
        // Initialise session timer (default 60 minutes)
        timerInit(st, st.remain > 0 ? (int)st.remain : 60);

        // Run the BBS
        boss_main(st);

        // -------- Post-call cleanup --------
        if (!st.loca && st.hCom != INVALID_HANDLE_VALUE) {
            dropCarrier(st);
            closeSerial(st);
        }

        // If running in local mode with -local flag, exit after one session
        if (argc > 1) {
            bool hasLocal = false;
            for (int i = 1; i < argc; i++) {
                if (std::string(argv[i]) == "-local" || std::string(argv[i]) == "--local")
                    hasLocal = true;
            }
            if (hasLocal) break;
        }

        // Reset for next caller
        resetForNewCall(st);
    }

    return 0;
}
