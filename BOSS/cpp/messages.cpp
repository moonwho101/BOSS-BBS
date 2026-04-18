// messages.cpp — Message base implementation
// BOSS.BAS reference lines: 5250–10260, 8600, 8700, 9240, 10710, 10870, 11050

#include "messages.h"
#include "editor.h"
#include "console.h"
#include "timer.h"
#include <fstream>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <cstdio>

// ---------------------------------------------------------------------------
// Field descriptors
// ---------------------------------------------------------------------------
static std::vector<FieldDesc> indexFields() {
    return { {"MESSAGE",5},{"HEAD",5},{"SLOC",10},{"LLOC",10} };
}

static std::vector<FieldDesc> headerFields() {
    return {
        {"MSG",5},{"AR",2},{"STA",1},{"RECI",1},{"DAT",8},
        {"TYME",11},{"TO1",19},{"TO2",19},{"FROM",19},{"SJ",35},{"TRA",5}
    };
}

static std::vector<FieldDesc> messageFields() {
    return { {"SAVEMES",72} };
}

// ---------------------------------------------------------------------------
RecordFile msgOpenIndex() {
    RecordFile rf;
    rfOpen(rf, std::string(BOSS_PATH) + "index.bbs", INDEX_REC_LEN, indexFields());
    return rf;
}
RecordFile msgOpenHeader() {
    RecordFile rf;
    rfOpen(rf, std::string(BOSS_PATH) + "header.bbs", HEADER_REC_LEN, headerFields());
    return rf;
}
RecordFile msgOpenMessage() {
    RecordFile rf;
    rfOpen(rf, std::string(BOSS_PATH) + "message.bbs", MESSAGE_REC_LEN, messageFields());
    return rf;
}

// ---------------------------------------------------------------------------
void msgLoadIndex(BossState& st) {
    // BOSS.BAS line 8600
    RecordFile rf = msgOpenIndex();
    if (!rf.fp) { st.mesNum = 0; return; }
    rfGet(rf, 1);
    st.mesNum = strVal(msgStrip(rfGet(rf, "MESSAGE")));
    if (st.mesNum > 0) {
        rfGet(rf, st.mesNum + 1);
        // SLOC/LLOC are needed for new posts
    }
    rfClose(rf);
}

// ---------------------------------------------------------------------------
// Format current time as " H:MM am/pm" (BOSS.BAS line 9140)
static std::string formatTime() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    int h = t->tm_hour, m = t->tm_min;
    const char* ampm = (h < 12) ? "am" : "pm";
    if (h == 0) h = 12;
    else if (h > 12) h -= 12;
    char buf[16];
    snprintf(buf, sizeof(buf), "%2d:%02d %s", h, m, ampm);
    return buf;
}

// Format today as "MM-DD-YY" (BOSS.BAS: LSET DAT$)
static std::string formatDate() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[10];
    snprintf(buf, sizeof(buf), "%02d-%02d-%02d", t->tm_mon+1, t->tm_mday, t->tm_year % 100);
    return buf;
}

// ---------------------------------------------------------------------------
void msgSave(BossState& st) {
    // BOSS.BAS lines 8700–9030
    if (st.lca == 0 && st.mlen == 0) {
        setColor(14, 0);
        printLine(st, "\r\nMessage Buffer is Empty!");
        return;
    }
    st.ok = true;

    // Increment post count
    int tp = strVal(st.tpost) + 1;
    st.tpost = std::to_string(tp);

    // Increment message counter
    st.mesNum++;

    // Load current SLOC/LLOC from index
    RecordFile idx = msgOpenIndex();
    int slocCur = 0, llocCur = 0;
    if (idx.fp && st.mesNum > 1) {
        rfGet(idx, st.mesNum); // last entry before new one
        slocCur = strVal(msgStrip(rfGet(idx, "SLOC")));
        llocCur = strVal(msgStrip(rfGet(idx, "LLOC")));
    }
    rfClose(idx);

    // Write body lines to message.bbs
    RecordFile msg = msgOpenMessage();
    if (!msg.fp) { st.ok = false; return; }
    int tempLloc = llocCur;
    for (int i = 1; i <= st.mlen; i++) {
        rfGet(msg, 1); // initialise buffer (any record is fine for init)
        rfLset(msg, "SAVEMES", st.tessage[i]);
        rfPut(msg, llocCur + i);
    }
    int newSloc = tempLloc + 1;
    int newLloc = llocCur + st.mlen;
    rfClose(msg);

    // Update index.bbs
    RecordFile idx2 = msgOpenIndex();
    if (idx2.fp) {
        rfGet(idx2, 1);
        rfLset(idx2, "MESSAGE", std::to_string(st.mesNum));
        rfLset(idx2, "HEAD", "");
        rfLset(idx2, "SLOC", "");
        rfLset(idx2, "LLOC", "");
        rfPut(idx2, 1);

        rfGet(idx2, 1);
        rfLset(idx2, "MESSAGE", std::to_string(st.mesNum));
        rfLset(idx2, "HEAD",    std::to_string(st.mesNum));
        rfLset(idx2, "SLOC",    std::to_string(newSloc));
        rfLset(idx2, "LLOC",    std::to_string(newLloc));
        rfPut(idx2, st.mesNum + 1);
    }
    rfClose(idx2);

    // Write header.bbs
    RecordFile hdr = msgOpenHeader();
    if (hdr.fp) {
        rfGet(hdr, 1);
        rfLset(hdr, "MSG",  std::to_string(st.mesNum));
        // Area: from current menu item
        std::string area;
        if (st.a1_ma == 1)
            area = st.a1r;
        else if (st.fed)
            area = "15";
        else
            area = st.menuDest[st.km];
        rfLset(hdr, "AR",   lset(area, 2));
        rfLset(hdr, "STA",  st.pri);
        rfLset(hdr, "RECI", "N");
        rfLset(hdr, "DAT",  formatDate());
        rfLset(hdr, "TYME", formatTime());
        rfLset(hdr, "TO1",  st.send1);
        rfLset(hdr, "TO2",  st.send2);
        rfLset(hdr, "FROM", st.name1 + " " + st.name2);
        rfLset(hdr, "SJ",   st.sbj);
        rfLset(hdr, "TRA",  st.tra1);
        rfPut(hdr, st.mesNum);
    }
    rfClose(hdr);
}

// ---------------------------------------------------------------------------
void postMessage(BossState& st) {
    // BOSS.BAS lines 5250–5420 (setup), body in editor.cpp
    msgLoadIndex(st);

    // Set recipient
    if (st.fed) {
        st.send1 = BossState::SYSOP_FIRST;
        st.send2 = BossState::SYSOP_LAST;
    }
    std::string fdt = formatTime();
    st.fdt = fdt;

    // Subject prompt — BOSS.BAS line 5270
    for (;;) {
        setColor(10, 0);
        printLine(st, "\r\n\r\n               \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
        setColor(14, 0);
        printStr(st, "Enter Subject:  ");
        st.sbj = editorReadLine(st, 21);
        if (st.sbj.empty()) {
            setColor(12, 0);
            printLine(st, "\r\n\r\n[Message Aborted]");
            st.no_ = true;
            return;
        }
        if ((int)st.sbj.size() < 3) {
            setColor(14, 0);
            printLine(st, "\r\n\r\nSubject MUST be more descriptive.");
            continue;
        }
        break;
    }
    st.tra1 = "Root";

    // Private?
    std::string pri = "N";
    if (st.send1 != "All") {
        if (st.send1 == "Sysop") { st.send1 = BossState::SYSOP_FIRST; st.send2 = BossState::SYSOP_LAST; }
        setColor(12, 0);
        printStr(st, "\r\n\r\nIs this a PRIVATE message [y/N]: ");
        char yn = waitKey(st);
        timerResetInactivity(st);
        yn = (char)toupper((unsigned char)yn);
        printStr(st, std::string(1, yn));
        pri = (yn == 'Y') ? "Y" : "N";
    }
    st.pri = pri;

    // Enter editor — BOSS.BAS lines 5350–5420
    clearIfEnabled(st);
    setColor(14, 0);
    printLine(st, "The Boss Message Editor");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4");
    setColor(13, 0);
    std::string area = st.menuDest[st.km];
    if (st.fed) area = "15";
    int areaNum = strVal(area);
    std::string areaName = (areaNum >= 1 && areaNum <= MAX_MESS_AREAS) ? MESS_AREA_NAME[areaNum] : area;
    std::string typeStr = (st.pri == "Y") ? "Private " : "Public ";
    printLine(st, "This is a " + typeStr + "message to " + st.send1 + " " + st.send2
              + ". Area [" + area + "]," + areaName + "\r\n");
    setColor(15, 0);
    printLine(st, "         You have a maximum of 79 lines which can be 72 Letters in Length.");
    printLine(st, "         To use the message editor options, press RETURN at a Blank line.\r\n");

    // Reset message buffer
    st.mlen = 0; st.lca = 0; st.mess = 0; st.ret_ = false;
    st.news2 = true;
    for (int i = 1; i <= MAX_MSG_LINES; i++) st.tessage[i] = "";

    runEditor(st);

    if (st.ok) {
        st.ok = false;
        msgSave(st);
        st.no_ = true;
    }
}

// ---------------------------------------------------------------------------
// Internal: display a single message header + body
// Returns true if we should continue, false if user aborted.
// BOSS.BAS lines 9820–10089
struct MsgFields {
    std::string mes, area, status, recvd, dat, tyme, to1, to2, from, subj, trac;
    int slo, llo, headRec;
};

static bool displayMessage(BossState& st, const MsgFields& mf) {
    clearIfEnabled(st);
    int how = 6;
    int slLen = strVal(st.sl); if (slLen < 10) slLen = 24;

    auto pr = [&](int lc, const std::string& label, int vc, const std::string& val) {
        setColor(lc, 0); printStr(st, label);
        setColor(vc, 0); printLine(st, val);
        how++;
    };

    std::string statusStr;
    if (mf.to1 == "All") statusStr = "None [A Public message]";
    else statusStr = (mf.recvd == "Y") ? "Received" : "Not Received";

    pr(10, "  Msg #: ",  15, mf.mes + " of " + std::to_string(st.mesNum));
    pr(10, "Status: ",   15, statusStr);
    pr(10, "  Area : ",  15, mf.area + ", [" + (strVal(mf.area) >= 1 && strVal(mf.area) <= MAX_MESS_AREAS ? MESS_AREA_NAME[strVal(mf.area)] : mf.area) + "]");
    int typeColor = (mf.status == "Y") ? 12 : 15;
    pr(10, "Type  : ", typeColor, (mf.status == "Y") ? "Private Message" : "Public Message");
    pr(10, "  To   : ",  15, mf.to1 + " " + mf.to2);
    pr(10, "Date  : ",   15, mf.dat);
    pr(10, "  From : ",  15, mf.from);
    pr(10, "Time  : ",   15, mf.tyme);
    pr(10, "  Ref. : ",  st.trc ? 11 : 15, mf.trac + (st.trc ? " [Tracing]" : ""));
    pr(10, "Sbjct : ", 13, mf.subj + "\r\n");
    printLine(st, "");

    // Message body
    RecordFile msg = msgOpenMessage();
    if (!msg.fp) return true;
    for (int qi = mf.slo; qi <= mf.llo; qi++) {
        if (!rfGet(msg, qi)) break;
        std::string line = "  " + msgStrip(rfGet(msg, "SAVEMES"));
        // Strip trailing null bytes
        line.erase(std::find(line.begin(), line.end(), '\0'), line.end());
        setColor(st.trc ? 12 : 11, 0);
        printLine(st, line);
        how++;
        if (how >= slLen) {
            // More prompt
            setColor(14, 0);
            printStr(st, "More [Y/n]: ");
            char yn = waitKey(st);
            timerResetInactivity(st);
            yn = (char)toupper((unsigned char)yn);
            printStr(st, std::string(1, yn));
            for (int k = 0; k < 13; k++) printBackspace(st);
            if (yn == 'N') {
                printLine(st, "[Aborted]");
                rfClose(msg);
                st.abort_ = true;
                return false;
            }
            how = 0;
        }
    }
    rfClose(msg);
    return true;
}

// ---------------------------------------------------------------------------
void readMessages(BossState& st) {
    // BOSS.BAS line 9240
    setColor(14, 0);
    std::string areaNum = st.menuDest[st.km];
    int area = strVal(areaNum);
    std::string areaName = (area >= 1 && area <= MAX_MESS_AREAS) ? MESS_AREA_NAME[area] : areaNum;
    printLine(st, "\r\n\r\nMessage Area " + areaNum + ", [" + areaName + "]\r\n");

    setColor(11, 0);
    printLine(st, " [N]ew  [F]orward  [R]everse  [I]ndividual  [A]bort");
    setColor(10, 0);
    printStr(st, "\r\nMessage Command [N F R A I]: ");

    const std::string validKeys = "NFRIA";
    std::string choice;
    for (;;) {
        timerCheck(st);
        char c = inkey(st);
        if (c == '\0') continue;
        c = (char)toupper((unsigned char)c);
        if (c == '\r') { choice = "N"; break; }
        if (validKeys.find(c) != std::string::npos) { choice = std::string(1, c); break; }
    }
    printStr(st, choice);

    msgLoadIndex(st);

    if (choice == "A") return;

    st.ind  = (choice == "I");
    st.way  = (choice == "R") ? -1 : 1;
    bool qNew = (choice == "N");

    // Determine starting message
    int startMsg = 1;
    if (qNew) {
        // Start from last read pointer + 1
        std::string lr = st.pnt2[area];
        startMsg = strVal(lr) + 1;
        if (startMsg < 1) startMsg = 1;
    } else if (st.ind) {
        setColor(11, 0);
        printStr(st, "\r\nEnter INDIVIDUAL number [1 -" + std::to_string(st.mesNum) + "]: ");
        std::string s;
        for (;;) {
            char c = waitKey(st);
            timerResetInactivity(st);
            if (c == '\r') { printLine(st, ""); break; }
            if (c < '0' || c > '9') continue;
            if ((int)s.size() >= 5) continue;
            s += c; printStr(st, std::string(1, c));
        }
        startMsg = strVal(s);
        if (startMsg < 1 || startMsg > st.mesNum) {
            setColor(14, 0);
            printLine(st, "\r\nChoice is between 1 and " + std::to_string(st.mesNum) + ".");
            return;
        }
    } else {
        setColor(11, 0);
        printStr(st, "\r\nEnter STARTING number [1 -" + std::to_string(st.mesNum) + "]: ");
        std::string s;
        for (;;) {
            char c = waitKey(st);
            timerResetInactivity(st);
            if (c == '\r') { printLine(st, ""); break; }
            if (c < '0' || c > '9') continue;
            if ((int)s.size() >= 5) continue;
            s += c; printStr(st, std::string(1, c));
        }
        printLine(st, "");
        startMsg = strVal(s);
        if (startMsg < 1 || startMsg > st.mesNum) return;
    }

    // Open files
    RecordFile idx = msgOpenIndex();
    RecordFile hdr = msgOpenHeader();
    if (!idx.fp || !hdr.fp) { rfClose(idx); rfClose(hdr); return; }

    int getm = startMsg;
    int z2   = 0;
    bool mend = false;
    st.trc  = false;
    st.abort_ = false;

    for (;;) {
        timerCheck(st);

        // Advance pointer
        if (st.way == 1) z2++;
        else             z2--;

        int recIdx = getm + z2;
        if (recIdx > st.mesNum + 1 || recIdx < 2) {
            mend = true;
            setColor(12, 0);
            printLine(st, "\r\n[No more messages in this base]");
            // Update lastread pointer
            st.pnt2[area] = std::to_string(st.mesNum);
            break;
        }

        rfGet(idx, recIdx);
        int mesRec = strVal(msgStrip(rfGet(idx, "MESSAGE")));
        int headRec= strVal(msgStrip(rfGet(idx, "HEAD")));
        int slo    = strVal(msgStrip(rfGet(idx, "SLOC")));
        int llo    = strVal(msgStrip(rfGet(idx, "LLOC")));

        rfGet(hdr, headRec);
        std::string ar   = msgStrip(rfGet(hdr, "AR"));
        std::string sta  = msgStrip(rfGet(hdr, "STA"));

        // Deleted?
        if (ar == "D" && st.trc) {
            setColor(10, 0);
            printLine(st, "\r\nMessage #" + std::to_string(mesRec) + " has been marked for deletion.");
            break;
        }

        // Area filter (unless reading mail cross-area)
        if (!st.ma && strVal(ar) != area) continue;

        // Visibility filter
        bool visible = false;
        std::string to1  = msgStrip(rfGet(hdr, "TO1"));
        std::string to2  = msgStrip(rfGet(hdr, "TO2"));
        std::string from = msgStrip(rfGet(hdr, "FROM"));
        {
            std::string t1u = to1, t2u = to2, fu = from;
            std::string n1u = st.name1, n2u = st.name2;
            toUpper(t1u); toUpper(t2u); toUpper(fu);
            toUpper(n1u); toUpper(n2u);
            if (t1u.find(n1u) != std::string::npos && t2u.find(n2u) != std::string::npos) visible = true;
            if (fu.find(n1u + " " + n2u) != std::string::npos) visible = true;
            if (st.name1 == BossState::SYSOP_FIRST && st.name2 == BossState::SYSOP_LAST) visible = true;
            if (sta != "Y") { // public
                if (to1.substr(0,3) == "All") visible = true;
            }
        }
        if (!visible) continue;

        MsgFields mf;
        mf.mes     = std::to_string(mesRec);
        mf.area    = ar;
        mf.status  = sta;
        mf.recvd   = msgStrip(rfGet(hdr, "RECI"));
        mf.dat     = msgStrip(rfGet(hdr, "DAT"));
        mf.tyme    = msgStrip(rfGet(hdr, "TYME"));
        mf.to1     = to1;
        mf.to2     = to2;
        mf.from    = from;
        mf.subj    = msgStrip(rfGet(hdr, "SJ"));
        mf.trac    = msgStrip(rfGet(hdr, "TRA"));
        mf.slo     = slo;
        mf.llo     = llo;
        mf.headRec = headRec;

        displayMessage(st, mf);

        // Mark received if addressed to us
        {
            std::string t1u = mf.to1, n1u = st.name1;
            toUpper(t1u); toUpper(n1u);
            if (t1u.find(n1u) != std::string::npos) {
                // Mark as received
                rfGet(hdr, headRec);
                rfLset(hdr, "RECI", "Y");
                rfPut(hdr, headRec);
            }
        }

        // Post-message prompt
        setColor(14, 0);
        bool canDelete = (mf.to1.find(st.name1) != std::string::npos || st.name1 == BossState::SYSOP_FIRST);
        std::string prompt = "\r\n[N]ext [R]eply [A]gain";
        std::string pkeys  = "NRAS";
        if (canDelete) { prompt += " [D]elete"; pkeys += "D"; }
        prompt += " [S]top";
        if (mf.trac != "Root" && mf.trac != "Null") { prompt += " [T]race"; pkeys += "T"; }
        prompt += ": ";
        printStr(st, prompt);

        char choice2 = 'N';
        for (;;) {
            timerCheck(st);
            char c = inkey(st);
            if (c == '\0') continue;
            c = (char)toupper((unsigned char)c);
            if (c == '\r') { choice2 = 'N'; break; }
            if (pkeys.find(c) != std::string::npos) { choice2 = c; break; }
        }
        printStr(st, std::string(1, choice2));

        if (choice2 == 'A') { z2 -= st.way; continue; }
        if (choice2 == 'R') {
            // Reply
            rfClose(idx); rfClose(hdr);
            // Store reply state
            st.send1 = ""; st.send2 = "";
            size_t sp = mf.from.find(' ');
            if (sp != std::string::npos) {
                st.send1 = mf.from.substr(0, sp);
                st.send2 = mf.from.substr(sp + 1);
            } else {
                st.send1 = mf.from;
            }
            st.sbj  = (mf.subj.find("Re:") != std::string::npos) ? mf.subj : ("Re:" + mf.subj);
            st.pri  = mf.status;
            st.tra1 = mf.mes;
            // Reset buffer
            st.mlen = 0; st.lca = 0;
            for (int i = 1; i <= MAX_MSG_LINES; i++) st.tessage[i] = "";
            postMessage(st);
            // Re-open for continued reading
            idx = msgOpenIndex();
            hdr = msgOpenHeader();
            if (!idx.fp || !hdr.fp) break;
            z2 -= st.way;
            continue;
        }
        if (choice2 == 'D' && canDelete) {
            rfGet(hdr, headRec);
            rfLset(hdr, "AR", "D ");
            rfPut(hdr, headRec);
            setColor(12, 0);
            printLine(st, "\r\nMessage #" + mf.mes + " has been deleted.");
        }
        if (choice2 == 'S') { mend = true; break; }
        if (st.ind) break;
    }

    rfClose(idx);
    rfClose(hdr);

    if (!mend) st.no_ = true;
}

// ---------------------------------------------------------------------------
void checkMail(BossState& st, int startMsg) {
    // BOSS.BAS line 11050
    msgLoadIndex(st);

    setColor(10, 0);
    printLine(st, "\r\nScanning message areas for mail addressed to " + st.name1 + " " + st.name2);

    RecordFile hdr = msgOpenHeader();
    if (!hdr.fp) return;

    std::vector<std::string> markMsgs, markAreas;

    for (int q = startMsg; q <= st.mesNum; q++) {
        timerCheck(st);
        rfGet(hdr, q);
        std::string t1   = msgStrip(rfGet(hdr, "TO1"));
        std::string t2   = msgStrip(rfGet(hdr, "TO2"));
        std::string mg   = msgStrip(rfGet(hdr, "MSG"));
        std::string ar   = msgStrip(rfGet(hdr, "AR"));
        std::string reci = msgStrip(rfGet(hdr, "RECI"));

        std::string t1u = t1, t2u = t2, n1u = st.name1, n2u = st.name2;
        toUpper(t1u); toUpper(t2u); toUpper(n1u); toUpper(n2u);

        if (t1u.find(n1u) != std::string::npos &&
            t2u.find(n2u) != std::string::npos &&
            reci == "N" && ar != "D") {
            markMsgs.push_back(mg);
            markAreas.push_back(ar);
        }
    }
    rfClose(hdr);

    if (markMsgs.empty()) {
        setColor(14, 0);
        printLine(st, "\r\nThere is no mail waiting for you, " + st.name1 + "\r\n");
        st.no_ = true;
        return;
    }

    // Display mail waiting header
    setColor(12, 0);
    printLine(st, "\r\n\r\nYou have mail waiting:");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");
    setColor(14, 0);
    printLine(st, "Area                Message Number");
    printLine(st, "----                --------------");
    for (size_t i = 0; i < markMsgs.size(); i++) {
        int aNum = strVal(markAreas[i]);
        std::string aName = (aNum >= 1 && aNum <= MAX_MESS_AREAS) ? MESS_AREA_NAME[aNum] : markAreas[i];
        setColor(10, 0);
        std::string line = "[" + markAreas[i] + "], " + aName;
        while ((int)line.size() < 20) line += ' ';
        setColor(11, 0);
        printLine(st, line + markMsgs[i]);
    }

    setColor(12, 0);
    printStr(st, "\r\n\r\nWould you like to read your mail now [Y/n]: ");
    char yn = waitKey(st);
    timerResetInactivity(st);
    yn = (char)toupper((unsigned char)yn);
    printStr(st, std::string(1, yn));
    if (yn == 'N') return;

    // Read the mail messages
    st.ma = true;
    RecordFile idx2 = msgOpenIndex();
    RecordFile hdr2 = msgOpenHeader();
    if (!idx2.fp || !hdr2.fp) { rfClose(idx2); rfClose(hdr2); st.ma = false; return; }

    for (size_t a = 0; a < markMsgs.size(); a++) {
        timerCheck(st);
        int msgNum = strVal(markMsgs[a]);
        rfGet(idx2, msgNum + 1);
        int slo = strVal(msgStrip(rfGet(idx2, "SLOC")));
        int llo = strVal(msgStrip(rfGet(idx2, "LLOC")));
        rfGet(hdr2, msgNum);

        MsgFields mf;
        mf.mes     = markMsgs[a];
        mf.area    = msgStrip(rfGet(hdr2, "AR"));
        mf.status  = msgStrip(rfGet(hdr2, "STA"));
        mf.recvd   = msgStrip(rfGet(hdr2, "RECI"));
        mf.dat     = msgStrip(rfGet(hdr2, "DAT"));
        mf.tyme    = msgStrip(rfGet(hdr2, "TYME"));
        mf.to1     = msgStrip(rfGet(hdr2, "TO1"));
        mf.to2     = msgStrip(rfGet(hdr2, "TO2"));
        mf.from    = msgStrip(rfGet(hdr2, "FROM"));
        mf.subj    = msgStrip(rfGet(hdr2, "SJ"));
        mf.trac    = msgStrip(rfGet(hdr2, "TRA"));
        mf.slo     = slo; mf.llo = llo;
        mf.headRec = msgNum;

        displayMessage(st, mf);

        // Mark received
        rfGet(hdr2, msgNum);
        rfLset(hdr2, "RECI", "Y");
        rfPut(hdr2, msgNum);
    }
    rfClose(idx2);
    rfClose(hdr2);
    st.ma = false;
}

// ---------------------------------------------------------------------------
void quickScanMessages(BossState& st) {
    // BOSS.BAS: qscan%=2, reads area set by current menu
    // Shows one-line header per message
    msgLoadIndex(st);
    std::string areaNum = st.menuDest[st.km];
    int area = strVal(areaNum);

    setColor(11, 0);
    printLine(st, " Message    To                    From                    Subject");
    printLine(st, " \xC4\xC4\xC4\xC4\xC4\xC4\xC4    \xC4\xC4                    \xC4\xC4\xC4\xC4                    \xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");

    RecordFile hdr = msgOpenHeader();
    if (!hdr.fp) return;

    int slLen = strVal(st.sl); if (slLen < 10) slLen = 24;
    int how = 5;

    for (int q = 1; q <= st.mesNum && !st.abort_; q++) {
        timerCheck(st);
        rfGet(hdr, q);
        std::string ar = msgStrip(rfGet(hdr, "AR"));
        if (strVal(ar) != area) continue;
        if (ar == "D") continue;

        std::string mg  = msgStrip(rfGet(hdr, "MSG"));
        std::string sta = msgStrip(rfGet(hdr, "STA"));
        std::string t1  = msgStrip(rfGet(hdr, "TO1"));
        std::string t2  = msgStrip(rfGet(hdr, "TO2"));
        std::string frm = msgStrip(rfGet(hdr, "FROM"));
        std::string sbj = msgStrip(rfGet(hdr, "SJ"));

        // Private marker
        if (sta == "Y") mg += " \xA0";  // bullet

        // Format line
        char line[120];
        snprintf(line, sizeof(line), " %-7s   %-20s  %-22s  %-18s",
            mg.c_str(),
            (t1+" "+t2).substr(0,20).c_str(),
            frm.substr(0,22).c_str(),
            sbj.substr(0,18).c_str());
        setColor(10, 0);
        printLine(st, line);

        how++;
        if (how >= slLen) {
            setColor(14, 0);
            printStr(st, "More [Y/n]: ");
            char yn = waitKey(st);
            timerResetInactivity(st);
            yn = (char)toupper((unsigned char)yn);
            for (int k = 0; k < 13; k++) printBackspace(st);
            if (yn == 'N') { st.abort_ = true; break; }
            how = 0;
        }

        // [S]top / [P]ause
        char key = inkey(st);
        if (key != '\0') {
            key = (char)toupper((unsigned char)key);
            if (key == 'S') { st.abort_ = true; break; }
            if (key == 'P') { waitKey(st); timerResetInactivity(st); }
        }
    }
    rfClose(hdr);
}
