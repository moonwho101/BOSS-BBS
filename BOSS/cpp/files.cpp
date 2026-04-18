// files.cpp — File library implementation
// BOSS.BAS reference lines: 13600–15140

#include "files.h"
#include "console.h"
#include "editor.h"
#include "timer.h"
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <sstream>
#include <algorithm>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

// Current file area index (1-based), from menu destination
static int fileArea(BossState& st) {
    return strVal(st.menuDest[st.km]);
}

// Path for current file area
static std::string fileAreaPath(BossState& st) {
    int a = fileArea(st);
    if (a >= 1 && a <= MAX_FILE_AREAS) return FILE_AREA_PATH[a];
    return std::string(BOSS_PATH) + "FILE1\\";
}

// Name for current file area
static std::string fileAreaName(BossState& st) {
    int a = fileArea(st);
    if (a >= 1 && a <= MAX_FILE_AREAS) return FILE_AREA_NAME[a];
    return "Unknown";
}

// Format file size: returns string like "1024"
static std::string fmtSize(DWORD lo) {
    return std::to_string(lo);
}

// Format file date from FILETIME → "MM-DD-YYYY"
static std::string fmtFileDate(FILETIME ft, bool newFlag, const std::string& lcall) {
    FILETIME localFt;
    FileTimeToLocalFileTime(&ft, &localFt);
    SYSTEMTIME st2;
    FileTimeToSystemTime(&localFt, &st2);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02d-%02d-%04d",
        st2.wMonth, st2.wDay, st2.wYear);
    return buf;
}

// Returns true if file date is newer than last logon (BASIC: lcall$ comparison)
static bool isNewSinceLastCall(FILETIME ft, const std::string& lcall) {
    // lcall format: MM-DD-YYYY or MM-DD-YY
    if (lcall.size() < 8) return false;
    FILETIME localFt;
    FileTimeToLocalFileTime(&ft, &localFt);
    SYSTEMTIME fst;
    FileTimeToSystemTime(&localFt, &fst);

    int fm = fst.wMonth, fd = fst.wDay, fy = fst.wYear % 100;
    int lm = strVal(lcall.substr(0, 2));
    int ld = strVal(lcall.substr(3, 2));
    int ly = strVal(lcall.size() >= 10 ? lcall.substr(6, 4) : lcall.substr(6, 2));
    if (ly < 100) { if (ly < 70) ly += 2000; else ly += 1900; }

    if (fy > ly) return true;
    if (fy < ly) return false;
    if (fm > lm) return true;
    if (fm < lm) return false;
    return fd >= ld;
}

// Look up description for a filename from files.bbs
static std::string lookupDescription(const std::string& areaPath, const std::string& filename) {
    std::string fbbs = areaPath + "files.bbs";
    std::ifstream fin(fbbs);
    if (!fin) return "No Description";
    std::string fname, desc;
    while (!fin.eof()) {
        if (!std::getline(fin, fname)) break;
        if (!std::getline(fin, desc)) break;
        // Trim
        while (!fname.empty() && fname.back() == '\r') fname.pop_back();
        while (!desc.empty() && desc.back() == '\r') desc.pop_back();
        std::string fu = fname, nu = filename;
        toUpper(fu); toUpper(nu);
        if (fu == nu) return desc;
    }
    return "No Description";
}

// Prompt user for transfer protocol (BASIC line 15010)
// Returns "" (abort), "X" (Xmodem), "1" (Xmodem-1k), "Y" (Ymodem-batch), "Z" (Zmodem)
static std::string promptProtocol(BossState& st) {
    setColor(14, 0);
    printLine(st, "\r\nTransfer Protocols:");
    setColor(11, 0);
    printLine(st, "  [X] Xmodem");
    printLine(st, "  [1] Xmodem-1k");
    printLine(st, "  [Y] Ymodem Batch");
    printLine(st, "  [Z] Zmodem Batch");
    printLine(st, "  [Q] Quit/Abort");
    setColor(14, 0);
    printStr(st, "\r\nEnter Protocol Choice: ");
    for (;;) {
        timerCheck(st);
        char c = inkey(st);
        if (c == '\0') continue;
        timerResetInactivity(st);
        c = (char)toupper((unsigned char)c);
        if (c == 'X') { printStr(st, "X"); return "X"; }
        if (c == '1') { printStr(st, "1"); return "1"; }
        if (c == 'Y') { printStr(st, "Y"); return "Y"; }
        if (c == 'Z') { printStr(st, "Z"); return "Z"; }
        if (c == 'Q' || c == '\r') { printStr(st, "Q"); return ""; }
    }
}

// Run DSZ via CreateProcess — STUBBED with TODO
// BASIC: SHELL "DSZ " + dsz1$ + dsz2$
static void runDsz(const std::string& dsz1, const std::string& dsz2) {
    // TODO: implement DSZ file transfer via CreateProcess.
    // Command would be: "DSZ " + dsz1 + dsz2
    // Example: DSZ pV1 pa9000 sz -k @c:\BOSS\FILE.DAT
    //
    // PROCESS_INFORMATION pi; STARTUPINFOA si = { sizeof(si) };
    // std::string cmd = "DSZ " + dsz1 + dsz2;
    // CreateProcessA(nullptr, &cmd[0], nullptr, nullptr, FALSE,
    //                0, nullptr, "c:\\BOSS", &si, &pi);
    // WaitForSingleObject(pi.hProcess, INFINITE);
    // CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    (void)dsz1; (void)dsz2;
}

// ---------------------------------------------------------------------------
// DORINFO1.DEF + shell to external program (BASIC line 13600)
// ---------------------------------------------------------------------------
void loadFiles(BossState& st) {
    // Write DORINFO1.DEF
    {
        std::ofstream f(std::string(BOSS_PATH) + "dorinfo1.def");
        f << "BOSS BBS\n";
        f << "MARK\n";
        f << "LONGO\n";
        f << (st.loca ? "COM0" : "COM1") << "\n";
        std::string baud = st.loca ? "0" : st.baud;
        f << baud << " BAUD,N,8,1\n";
        f << "0\n";
        f << st.name1 << "\n";
        f << st.name2 << "\n";
        f << st.city << "\n";
        f << (st.ansiOn ? "1" : "0") << "\n";
        f << st.secur << "\n";
        f << std::to_string(timerRemaining(st)) << "\n";
    }

    // Shell to external program (menu$(km%) is the program path)
    std::string prog = st.menuDest[st.km];
    if (!prog.empty()) {
        // TODO: CreateProcess to run prog in c:\BOSS
        // PROCESS_INFORMATION pi; STARTUPINFOA si = { sizeof(si) };
        // CreateProcessA(nullptr, &prog[0], nullptr, nullptr, FALSE,
        //                0, nullptr, "c:\\BOSS", &si, &pi);
        // WaitForSingleObject(pi.hProcess, INFINITE);
        // CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    }
    st.fx = true;
}

// ---------------------------------------------------------------------------
// Browse files listing (BASIC line 14050)
// ---------------------------------------------------------------------------
void browseFiles(BossState& st) {
    clearIfEnabled(st);
    int area = fileArea(st);
    setColor(11, 0);
    printLine(st, "File Area: " + std::to_string(area) + ", [" + fileAreaName(st) + "]\r\n");
    printLine(st, "[P]ause [S]top\r\n");
    printLine(st, "File          Size       Date         Description");
    setColor(12, 0);
    printLine(st, "\xC4\xC4\xC4\xC4          \xC4\xC4\xC4\xC4       \xC4\xC4\xC4\xC4         \xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");

    std::string areaPath = fileAreaPath(st);
    std::string fbbs = areaPath + "files.bbs";
    std::ifstream fin(fbbs);
    if (!fin) {
        setColor(10, 0);
        printLine(st, "[No file listing available for this area]");
        return;
    }

    int slLen = strVal(st.sl); if (slLen < 10) slLen = 24;
    int how = 3;
    bool anyNew = false;
    st.abort_ = false;

    std::string fname, desc;
    while (!st.abort_) {
        timerCheck(st);
        if (!std::getline(fin, fname)) break;
        if (!std::getline(fin, desc)) break;
        while (!fname.empty() && (fname.back() == '\r' || fname.back() == '\n')) fname.pop_back();
        while (!desc.empty() && (desc.back() == '\r' || desc.back() == '\n')) desc.pop_back();

        // Check if file exists
        std::string fullPath = areaPath + fname;
        WIN32_FIND_DATAA fd;
        HANDLE hf = FindFirstFileA(fullPath.c_str(), &fd);
        bool offline = (hf == INVALID_HANDLE_VALUE);
        if (hf != INVALID_HANDLE_VALUE) FindClose(hf);

        if (offline && !st.nsq) {
            // Show as OFF-LINE
            setColor(11, 0);
            std::string pad = fname;
            while ((int)pad.size() < 14) pad += ' ';
            printStr(st, pad);
            setColor(15, 0);
            printStr(st, "OFF-LINE                ");
            setColor(10, 0);
            printLine(st, desc);
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
            continue;
        }
        if (offline) continue; // nsq mode: skip missing files

        std::string sizeStr = fmtSize(fd.nFileSizeLow);
        bool isNew = isNewSinceLastCall(fd.ftLastWriteTime, st.lcall);
        std::string dateStr = fmtFileDate(fd.ftLastWriteTime, isNew, st.lcall);
        if (isNew) dateStr += " \x04"; // bullet for new files
        std::string tt = isNew ? " " : "   ";

        if (st.nsq && !isNew) continue; // new-files-scan: skip old files
        anyNew = true;

        // Print row
        setColor(11, 0);
        std::string pad = fname;
        while ((int)pad.size() < 14) pad += ' ';
        printStr(st, pad);
        setColor(14, 0);
        std::string szPad = sizeStr;
        while ((int)szPad.size() < 11) szPad += ' ';
        printStr(st, szPad);
        setColor(12, 0);
        printStr(st, dateStr + tt);
        setColor(10, 0);
        printLine(st, desc);

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

        // [S]top / [P]ause check
        char key = inkey(st);
        if (key != '\0') {
            key = (char)toupper((unsigned char)key);
            if (key == 'S') { st.abort_ = true; break; }
            if (key == 'P') { waitKey(st); timerResetInactivity(st); }
        }
    }
    fin.close();

    if (!anyNew && st.nsq) {
        setColor(10, 0);
        printLine(st, "[There are no new files in this area since you last logged in]");
    }

    setColor(14, 0);
    printStr(st, "\r\n[Press Return]: ");
    waitKey(st); timerResetInactivity(st);
    printLine(st, "");
}

// ---------------------------------------------------------------------------
// Search files by wildcard (BASIC line 14060)
// ---------------------------------------------------------------------------
void searchFiles(BossState& st) {
    clearIfEnabled(st);
    int area = fileArea(st);
    setColor(14, 0);
    printLine(st, "Locating a File");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");
    setColor(12, 0);
    printLine(st, "File Area: " + std::to_string(area) + ", [" + fileAreaName(st) + "]\r\n");
    setColor(11, 0);
    printLine(st, "[P]ause [S]top\r\n");
    setColor(10, 0);
    printStr(st, "Enter File Name [Wild Cards (*,?) are Ok, RETURN to QUIT]: ");

    std::string pattern = editorReadLine(st, 12);
    if (pattern.empty()) return;

    std::string areaPath = fileAreaPath(st);
    std::string fullPat = areaPath + pattern;

    int slLen = strVal(st.sl); if (slLen < 10) slLen = 24;
    int how = 6;
    bool found = false;
    st.abort_ = false;

    WIN32_FIND_DATAA fd;
    HANDLE hf = FindFirstFileA(fullPat.c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE) {
        setColor(12, 0);
        printLine(st, "\r\nSorry, no Matches made");
        setColor(14, 0);
        printStr(st, "\r\n[Press Return]: ");
        waitKey(st); timerResetInactivity(st);
        printLine(st, "");
        return;
    }

    do {
        timerCheck(st);
        if (st.abort_) break;
        std::string fn = fd.cFileName;
        toUpper(fn);

        // Get description
        std::string desc = lookupDescription(areaPath, fn);
        std::string sizeStr = fmtSize(fd.nFileSizeLow);

        found = true;
        char row[120];
        snprintf(row, sizeof(row), "%-13s %-11s  %s",
            fn.c_str(), sizeStr.c_str(), desc.substr(0, 40).c_str());
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
    } while (FindNextFileA(hf, &fd));
    FindClose(hf);

    if (!found) {
        setColor(12, 0);
        printLine(st, "\r\nSorry, no Matches made");
    }

    setColor(14, 0);
    printStr(st, "\r\n[Press Return]: ");
    waitKey(st); timerResetInactivity(st);
    printLine(st, "");
}

// ---------------------------------------------------------------------------
// Download files via DSZ (BASIC line 14080)
// ---------------------------------------------------------------------------
void downloadFiles(BossState& st) {
    // Open FILE.DAT for appending (download queue)
    std::string fileDat = std::string(BOSS_PATH) + "file.dat";
    // Prompt protocol
    clearIfEnabled(st);
    setColor(12, 0);
    printLine(st, "Downloading Files");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4");

    std::string proto = promptProtocol(st);
    if (proto.empty()) {
        printLine(st, "\r\n[Download Aborted]");
        return;
    }

    // Build DSZ command prefix
    std::string dsz1, dsz2;
    double ru = 3.58;
    if (proto == "X") { dsz1 = "pV1 sx "; ru = 3.58; }
    else if (proto == "1") { dsz1 = "pV1 sx -k "; ru = 3.29; }
    else if (proto == "Y") { dsz1 = "pV1 pa9000 sb -k "; dsz2 = "@" + std::string(BOSS_PATH) + "FILE.DAT"; ru = 3.8; }
    else if (proto == "Z") { dsz1 = "pV1 pa9000 z pL1024 pT600 sz -k "; dsz2 = "@" + std::string(BOSS_PATH) + "FILE.DAT"; ru = 3.34; }

    setColor(12, 0); printStr(st, "\r\nNOTE: ");
    setColor(11, 0);
    printLine(st, "You can enter Wildcards [*,?] when requesting files");

    std::string areaPath = fileAreaPath(st);
    int area = fileArea(st);

    // Queue of file names and sizes
    std::vector<std::string> qFiles, qSizes;
    double totMin = 0.0;
    long long totBytes = 0;

    std::ofstream qFile;
    bool qOpened = false;

    for (;;) {
        timerCheck(st);
        setColor(14, 0);
        if (proto == "Z" || proto == "Y")
            printStr(st, "Enter File #" + std::to_string((int)qFiles.size() + 1) + ": ");
        else
            printStr(st, "Enter Filename: ");

        std::string fn = editorReadLine(st, 12);
        if (fn.empty()) break;
        if ((int)qFiles.size() >= 50) {
            setColor(11, 0);
            printLine(st, "There is 50 file maximum restriction.");
            break;
        }

        // Wildcard search
        std::string pat = areaPath + fn;
        WIN32_FIND_DATAA fd;
        HANDLE hf = FindFirstFileA(pat.c_str(), &fd);
        if (hf == INVALID_HANDLE_VALUE) {
            setColor(12, 0);
            printLine(st, "\r\n\r\nFile not found on disk.\r\n");
            if (proto != "Z" && proto != "Y") continue;
            // batch protocols: continue adding next file
            continue;
        }

        if (!qOpened) {
            qFile.open(fileDat, std::ios::out | std::ios::app);
            qOpened = true;
            // Print header
            setColor(10, 0);
            printLine(st, "\r\n\r\nFile           Size       Time    Description");
            printLine(st, "\xC4\xC4\xC4\xC4           \xC4\xC4\xC4\xC4       \xC4\xC4\xC4\xC4    \xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");
        }

        do {
            std::string realName = fd.cFileName;
            toUpper(realName);

            // Check already in queue
            bool dup = false;
            for (auto& q : qFiles) { if (q == realName) { dup = true; break; } }
            if (dup) {
                setColor(11, 0);
                printLine(st, realName + " is already in your download queue.");
                if (proto != "Z" && proto != "Y") break;
                continue;
            }

            DWORD fsize = fd.nFileSizeLow;
            std::string sizeStr = fmtSize(fsize);
            std::string desc = lookupDescription(areaPath, realName);

            // Calculate transfer time
            double mins = (ru * fsize) / 24515.0;
            if (st.baud == "2400") mins /= 2.0;
            int imins = (int)(mins * 100 + 0.5); mins = imins / 100.0;

            char row[120];
            snprintf(row, sizeof(row), "%-13s  %-10s  %-6.2f  %s",
                realName.c_str(), sizeStr.c_str(), mins, desc.substr(0, 30).c_str());
            setColor(12, 0);
            printLine(st, row);

            qFiles.push_back(realName);
            qSizes.push_back(sizeStr);
            totMin  += mins;
            totBytes += fsize;
            if (qFile.is_open()) qFile << realName << "\n";

            if ((int)qFiles.size() >= 50) break;
        } while ((proto == "Z" || proto == "Y") && FindNextFileA(hf, &fd));
        FindClose(hf);

        if (proto == "X" || proto == "1") {
            // Single file mode: we have the name
            dsz2 = qFiles.empty() ? fn : qFiles.back();
            break;
        }
    }

    if (qFile.is_open()) qFile.close();

    if (qFiles.empty()) {
        setColor(11, 0);
        st.no_ = true;
        printLine(st, "\r\n\r\nFile Transfer Aborted.\r\n");
        // Clean up
        DeleteFileA(fileDat.c_str());
        return;
    }

    // Show totals
    setColor(10, 0);
    char totLine[120];
    snprintf(totLine, sizeof(totLine),
        "\r\n%d file(s) requested.   Total Bytes:%lld.   Total Dl Time:%.2f min.\r\n",
        (int)qFiles.size(), totBytes, totMin);
    printLine(st, totLine);

    // Time check
    int rem = timerRemaining(st);
    if ((int)totMin >= rem) {
        setColor(12, 0);
        printLine(st, "Sorry, Not enough time for file transfer!");
        DeleteFileA(fileDat.c_str());
        st.no_ = true;
        return;
    }

    setColor(14, 0);
    printStr(st, "Ready For Download [Y/n]: ");
    char yn = waitKey(st); timerResetInactivity(st);
    yn = (char)toupper((unsigned char)yn);
    printStr(st, std::string(1, yn));
    if (yn == 'N') {
        setColor(12, 0);
        printLine(st, "\r\n[Download Aborted]");
        DeleteFileA(fileDat.c_str());
        st.no_ = true;
        return;
    }

    setColor(11, 0);
    printLine(st, "\r\nSending file(s) [Send several CTRL X's to abort]\r\n");

    // TODO: run DSZ
    // runDsz(dsz1, dsz2);
    printLine(st, "[DSZ transfer stub — not implemented]");

    // Parse DSZ.LOG (stub — just credit the files)
    int sent = 0;
    long long sentBytes = 0;
    // In real impl: open DSZ.LOG, check each file's transfer size
    // Stub: credit all queued files
    for (size_t i = 0; i < qFiles.size(); i++) {
        sent++;
        sentBytes += strVal(qSizes[i]);
    }

    setColor(11, 0);
    printLine(st, "\r\n" + std::to_string(sent) + " file(s) successfully sent.");

    // Update download stats
    int dl = strVal(st.dl); dl += sent;
    st.dl = std::to_string(dl);
    long long dlkb = strVal(st.dlkb); dlkb += sentBytes;
    st.dlkb = std::to_string(dlkb);

    // Reset DSZ.LOG
    {
        std::ofstream reset(std::string(BOSS_PATH) + "DSZ.LOG");
        (void)reset;
    }
    DeleteFileA(fileDat.c_str());
    st.no_ = true;
}

// ---------------------------------------------------------------------------
// Upload files via DSZ (BASIC line 15100)
// ---------------------------------------------------------------------------
void uploadFiles(BossState& st) {
    clearIfEnabled(st);
    setColor(12, 0);
    printLine(st, "Upload Files");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4");

    std::string proto = promptProtocol(st);
    if (proto.empty()) {
        printLine(st, "\r\n[Upload Aborted]");
        st.no_ = true;
        return;
    }

    std::string dsz1, dsz2;
    if (proto == "X") { dsz1 = "pV1 rx "; }
    else if (proto == "1") { dsz1 = "pV1 rx -k "; }
    else if (proto == "Y") { dsz1 = "pV1 pa9000 rb -k "; }
    else if (proto == "Z") { dsz1 = "pV1 pa9000 z pL1024 pT600 rz -k "; }

    std::string fname;
    if (proto == "X" || proto == "1") {
        setColor(14, 0);
        printStr(st, "\r\nEnter Filename: ");
        fname = editorReadLine(st, 12);
        if (fname.empty()) { printLine(st, "\r\n[Upload Aborted]"); st.no_ = true; return; }
        dsz2 = fname;
    }

    setColor(11, 0);
    printLine(st, "\r\nReady to receive your file(s) [Send several CTRL X's to abort]\r\n");

    // TODO: run DSZ receive
    // runDsz(dsz1, dsz2);
    printLine(st, "[DSZ receive stub — not implemented]");

    // Prompt for description
    std::string areaPath = fileAreaPath(st);
    std::string fbbs = areaPath + "files.bbs";
    std::string descLine;
    setColor(14, 0);
    printStr(st, "\r\nEnter file description (up to 40 chars): ");
    descLine = editorReadLine(st, 40);

    // Write to files.bbs
    {
        std::ofstream f(fbbs, std::ios::app);
        if (f) {
            if (fname.empty()) fname = "UNKNOWN";
            f << fname << "\n" << descLine << "\n";
        }
    }

    // Update upload stats
    int ul = strVal(st.ul) + 1;
    st.ul = std::to_string(ul);

    setColor(11, 0);
    printLine(st, "\r\nUpload complete. Thank you!");
    st.no_ = true;
}
