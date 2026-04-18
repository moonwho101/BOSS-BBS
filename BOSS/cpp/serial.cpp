// serial.cpp — Win32 COM-port implementation
// BOSS.BAS reference lines: 239, 12690–12865, 13000–13001

#include "serial.h"
#include <cstdio>
#include <thread>
#include <chrono>
#include <string>

// ---------------------------------------------------------------------------
bool openSerial(BossState& st, const std::string& port, int baud) {
    // Build device name; handle COM10+ ("\\.\COM10")
    std::string devName = (port.size() > 4) ? "\\\\.\\" + port : port;

    st.hCom = CreateFileA(devName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0, nullptr, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, nullptr);

    if (st.hCom == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Cannot open %s (error %lu)\n", port.c_str(), GetLastError());
        return false;
    }

    // Configure port
    DCB dcb = {};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(st.hCom, &dcb);
    dcb.BaudRate = (DWORD)baud;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDtrControl   = DTR_CONTROL_ENABLE;
    dcb.fRtsControl   = RTS_CONTROL_ENABLE;
    dcb.fOutxCtsFlow  = FALSE;
    dcb.fOutxDsrFlow  = FALSE;
    dcb.fBinary       = TRUE;
    dcb.fParity       = FALSE;
    if (!SetCommState(st.hCom, &dcb)) {
        fprintf(stderr, "SetCommState failed (%lu)\n", GetLastError());
        CloseHandle(st.hCom);
        st.hCom = INVALID_HANDLE_VALUE;
        return false;
    }

    // Generous timeouts: let ReadFile return immediately if no data
    COMMTIMEOUTS to = {};
    to.ReadIntervalTimeout         = MAXDWORD;
    to.ReadTotalTimeoutMultiplier  = MAXDWORD;
    to.ReadTotalTimeoutConstant    = 1;   // 1 ms → nearly non-blocking
    to.WriteTotalTimeoutMultiplier = 0;
    to.WriteTotalTimeoutConstant   = 2000;
    SetCommTimeouts(st.hCom, &to);

    SetupComm(st.hCom, 4096, 4096);
    PurgeComm(st.hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);

    st.comPort = port;
    st.baudVal = baud;
    st.baud    = std::to_string(baud);
    return true;
}

// ---------------------------------------------------------------------------
void closeSerial(BossState& st) {
    if (st.hCom != INVALID_HANDLE_VALUE) {
        CloseHandle(st.hCom);
        st.hCom = INVALID_HANDLE_VALUE;
    }
}

// ---------------------------------------------------------------------------
void writeSerial(BossState& st, const std::string& s) {
    if (st.hCom == INVALID_HANDLE_VALUE || s.empty()) return;
    DWORD written;
    WriteFile(st.hCom, s.c_str(), (DWORD)s.size(), &written, nullptr);
}

// ---------------------------------------------------------------------------
int readSerial(BossState& st, char* buf, int max, int timeoutMs) {
    if (st.hCom == INVALID_HANDLE_VALUE) return 0;
    if (timeoutMs > 0) {
        // Temporarily adjust total timeout
        COMMTIMEOUTS to = {};
        to.ReadIntervalTimeout         = MAXDWORD;
        to.ReadTotalTimeoutMultiplier  = MAXDWORD;
        to.ReadTotalTimeoutConstant    = (DWORD)timeoutMs;
        SetCommTimeouts(st.hCom, &to);
    }
    DWORD got = 0;
    ReadFile(st.hCom, buf, (DWORD)max, &got, nullptr);
    if (timeoutMs > 0) {
        // Restore to non-blocking
        COMMTIMEOUTS to = {};
        to.ReadIntervalTimeout         = MAXDWORD;
        to.ReadTotalTimeoutMultiplier  = MAXDWORD;
        to.ReadTotalTimeoutConstant    = 1;
        SetCommTimeouts(st.hCom, &to);
    }
    return (int)got;
}

// ---------------------------------------------------------------------------
bool readSerialByte(BossState& st, char& c, int timeoutMs) {
    return readSerial(st, &c, 1, timeoutMs) == 1;
}

// ---------------------------------------------------------------------------
bool carrierPresent(BossState& st) {
    // BASIC: INP(&H3FE) > 48  ↔  MS_RLSD_ON (RLSD = Received Line Signal Detect = DCD)
    if (st.hCom == INVALID_HANDLE_VALUE) return false;
    DWORD modemStatus = 0;
    GetCommModemStatus(st.hCom, &modemStatus);
    return (modemStatus & MS_RLSD_ON) != 0;
}

// ---------------------------------------------------------------------------
void dropCarrier(BossState& st) {
    // BASIC: OUT &H3FC, 0  → clear DTR + RTS
    if (st.hCom == INVALID_HANDLE_VALUE) return;
    EscapeCommFunction(st.hCom, CLRDTR);
    EscapeCommFunction(st.hCom, CLRRTS);
    // Brief delay (~200 ms) then restore
    Sleep(200);
}

void raiseDTR(BossState& st) {
    // BASIC: OUT &H3FC, 1 → restore DTR
    if (st.hCom == INVALID_HANDLE_VALUE) return;
    EscapeCommFunction(st.hCom, SETDTR);
    EscapeCommFunction(st.hCom, SETRTS);
    Sleep(100);
}

// ---------------------------------------------------------------------------
void initModem(BossState& st) {
    // BOSS.BAS lines 12700-12790
    printf("Initialising modem on %s...\n", st.comPort.c_str());

    // Flush any pending data
    PurgeComm(st.hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // ATZ — reset modem
    writeSerial(st, "ATZ1\r");
    Sleep(1500);

    // Init string: auto-answer, no speaker, hang-up on DTR drop, verbose result
    writeSerial(st, "AT&D2M0H0Q0V0X4S0=1\r");
    Sleep(1000);

    // Drain any response
    {
        char buf[256];
        readSerial(st, buf, sizeof(buf) - 1, 200);
    }
    printf("Modem initialised.\n");
}

// ---------------------------------------------------------------------------
void waitForCall(BossState& st) {
    // BOSS.BAS lines 12795–12865
    // Drain Rx buffer
    {
        char buf[256];
        while (readSerial(st, buf, sizeof(buf) - 1, 10) > 0) {}
    }

    printf("Waiting for RING...\n");

    std::string acc;
    acc.reserve(64);

    for (;;) {
        char c = '\0';
        if (readSerialByte(st, c, 200)) {
            acc += c;

            // CONNECT string detection: modem returns numeric result codes
            // "5" = CONNECT 1200, "10" = CONNECT 2400 (numeric mode X4)
            if (acc.find('5') != std::string::npos) {
                st.baud    = "1200";
                st.baudVal = 1200;
                st.bv      = 1;
                st.delay_  = 2400;
                printf("Connected at 1200 bps\n");
                return;
            }
            if (acc.find("10") != std::string::npos) {
                st.baud    = "2400";
                st.baudVal = 2400;
                st.bv      = 2;
                st.delay_  = 2200;
                printf("Connected at 2400 bps\n");
                return;
            }

            // Keep only last 32 chars to avoid unbounded growth
            if (acc.size() > 64) acc = acc.substr(acc.size() - 32);
        }

        // Re-init modem and keep waiting after ~40 000 half-ms polls
        // (approximates BOSS.BAS: IF I>40000 THEN... GOTO 12701)
        static int pollCount = 0;
        pollCount++;
        if (pollCount > 80000) {
            pollCount = 0;
            printf("Didn't connect; re-initialising...\n");
            initModem(st);
        }
    }
}
