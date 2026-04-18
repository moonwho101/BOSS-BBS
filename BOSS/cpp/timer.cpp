// timer.cpp — Session timer and inactivity watchdog
// BOSS.BAS reference lines: 13050–13052

#include "timer.h"
#include "console.h"
#include "serial.h"
#include <ctime>
#include <cstdio>

// ---------------------------------------------------------------------------
void timerInit(BossState& st, int remainMinutes) {
    // BOSS.BAS line 13052
    st.zt   = (double)time(nullptr);  // session start epoch
    st.pt   = 0;                       // minutes elapsed at last check
    st.qt   = 0;                       // inactivity minutes at last check
    st.ini  = remainMinutes;           // minutes allowed for this session
    st.remain = remainMinutes;
}

// ---------------------------------------------------------------------------
void timerResetInactivity(BossState& st) {
    // BOSS.BAS: PT% = ZT (mark current elapsed as last activity)
    st.pt  = (double)time(nullptr);  // last activity epoch
    st.loa = 0;                       // reset inactivity warning level
}

// ---------------------------------------------------------------------------
int timerRemaining(BossState& st) {
    double now  = (double)time(nullptr);
    double elapsed = (now - st.zt) / 60.0;  // minutes elapsed
    int rem = st.ini - (int)elapsed;
    if (rem < 0) rem = 0;
    return rem;
}

// ---------------------------------------------------------------------------
// Called once per second from input loops.
// BOSS.BAS lines 13050–13052 ON TIMER(60) logic.
void timerCheck(BossState& st) {
    double now = (double)time(nullptr);

    // Carrier check — BASIC INP(&H3FE)<128 maps to MS_RLSD_ON absence
    if (!st.loca && st.hCom != INVALID_HANDLE_VALUE) {
        if (!carrierPresent(st)) {
            setColor(12, 0);
            printLine(st, "\r\n[Carrier Dropped]");
            st.flav = true;   // flag logoff
            return;
        }
    }

    // Inactivity check (4 minutes = 240 seconds)
    if (st.pt > 0.0) {
        double inactive = (now - st.pt) / 60.0;  // minutes inactive

        if (inactive >= 4.0 && !st.loca) {
            // Inactivity logout
            setColor(12, 0);
            printLine(st, "\r\n\r\n[Inactivity Timeout — Logging off]");
            st.flav = true;
            return;
        }
        if (inactive >= 3.0 && !st.loca) {
            // 1 minute warning
            if (st.qt < 3) {
                st.qt = 3;
                setColor(14, 0);
                printLine(st, "\r\n\r\nINACTIVITY WARNING: You will be logged off in 1 minute.");
            }
        } else if (inactive >= 2.0 && !st.loca) {
            // 2 minute warning
            if (st.qt < 2) {
                st.qt = 2;
                setColor(14, 0);
                printLine(st, "\r\n\r\nINACTIVITY WARNING: You will be logged off in 2 minutes.");
            }
        }
    }

    // Session time check
    int rem = timerRemaining(st);
    st.remain = rem;

    double elapsed = (now - st.zt) / 60.0;
    int elMin = (int)elapsed;

    if (elMin > st.ju) {
        st.ju = elMin;
        // Warnings at specific remaining times
        if (!st.loca) {
            if (rem == 5) {
                setColor(12, 0);
                printLine(st, "\r\n\r\nYou have 5 minutes remaining.");
            } else if (rem == 2) {
                setColor(12, 0);
                printLine(st, "\r\n\r\nYou have 2 minutes remaining.");
            } else if (rem == 1) {
                setColor(12, 0);
                printLine(st, "\r\n\r\nYou have 1 minute remaining.");
            }
        }
        if (rem <= 0 && !st.loca) {
            setColor(12, 0);
            printLine(st, "\r\n\r\n[Time Limit Reached — Logging off]");
            st.flav = true;
            return;
        }
    }
}
