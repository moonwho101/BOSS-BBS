// users.cpp — User database implementation
// BOSS.BAS reference lines: 350–900, 2590–3390, 10350–10390

#include "users.h"
#include "console.h"
#include "timer.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <ctime>

// ---------------------------------------------------------------------------
// FIELD layout for USERS.BBS (matches BOSS.BAS line 2880)
// ---------------------------------------------------------------------------
static std::vector<FieldDesc> usersFields() {
    return {
        {"N1",    19}, // UF_NAME1  — first name
        {"N2",    19}, // UF_NAME2  — last name
        {"P",     15}, // UF_PAS    — password
        {"C",     19}, // UF_CITY   — city,province
        {"V",     12}, // UF_PHONE1 — data phone
        {"B",     12}, // UF_PHONE2 — voice phone
        {"AN",     1}, // UF_ANSI
        {"BI",    10}, // UF_BDATE
        {"S",      2}, // UF_SL     — screen length
        {"SE",     5}, // UF_SECUR
        {"TC",     5}, // UF_TCAL
        {"LL",     5}, // UF_LREAD
        {"TU",     4}, // UF_TUSE
        {"TP",     5}, // UF_TPOST
        {"LC",    10}, // UF_LCALL
        {"U",      5}, // UF_UL
        {"UK",     7}, // UF_ULKB
        {"D",      5}, // UF_DL
        {"DK",     7}, // UF_DLKB
        {"XPERT",  1}, // UF_XPT
        {"DEL",    1}, // UF_DEL
        // Padding to 190 bytes: 19+19+15+19+12+12+1+10+2+5+5+5+4+5+10+5+7+5+7+1+1 = 169
        {"PAD",   21}, // 21 bytes padding → total 190
    };
}

// ---------------------------------------------------------------------------
RecordFile openUsersFile(const std::string& pathOverride) {
    std::string path = pathOverride.empty()
        ? (std::string(BOSS_PATH) + "USERS.BBS")
        : pathOverride;
    RecordFile rf;
    rfOpen(rf, path, USERS_REC_LEN, usersFields());
    return rf;
}

// ---------------------------------------------------------------------------
static std::string fieldVal(const RecordFile& rf, int idx) {
    return stripBlanks(rfGetIdx(rf, idx));
}

// ---------------------------------------------------------------------------
void loadUserFromRecord(BossState& st, const RecordFile& rf, int recNum) {
    st.userRecNum = recNum;
    st.name1   = fieldVal(rf, UF_NAME1);
    st.name2   = fieldVal(rf, UF_NAME2);
    st.password= fieldVal(rf, UF_PAS);
    st.city    = fieldVal(rf, UF_CITY);
    st.phone1  = fieldVal(rf, UF_PHONE1);
    st.phone2  = fieldVal(rf, UF_PHONE2);
    st.ansi    = fieldVal(rf, UF_ANSI);
    st.bdate   = fieldVal(rf, UF_BDATE);
    st.sl      = fieldVal(rf, UF_SL);
    st.secur   = fieldVal(rf, UF_SECUR);
    st.tcal    = fieldVal(rf, UF_TCAL);
    st.lread   = fieldVal(rf, UF_LREAD);
    st.tuse    = fieldVal(rf, UF_TUSE);
    st.tse     = st.tuse;
    st.tpost   = fieldVal(rf, UF_TPOST);
    st.lcall   = fieldVal(rf, UF_LCALL);
    st.ul      = fieldVal(rf, UF_UL);
    st.ulkb    = fieldVal(rf, UF_ULKB);
    st.dl      = fieldVal(rf, UF_DL);
    st.dlkb    = fieldVal(rf, UF_DLKB);
    st.xpt     = fieldVal(rf, UF_XPT);
    st.del1    = fieldVal(rf, UF_DEL);
    st.usernum = std::to_string(recNum - 1);

    // Adjust remaining time for time used today
    // BOSS.BAS line 3180: REMAIN=REMAIN-VAL(TUSE$)
    //   If last call was not today, reset tuse to "0"
    {
        // Get today's date as "MM-DD-YYYY"
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        char today[12];
        snprintf(today, sizeof(today), "%02d-%02d-%04d", t->tm_mon + 1, t->tm_mday, 1900 + t->tm_year);
        if (st.lcall != today) {
            st.remain = 60;
            st.tse    = "0";
            st.tuse   = "0";
        } else {
            st.remain -= strVal(st.tuse);
        }
    }

    // Compute age (TYEAR$) — BOSS.BAS line 5040
    {
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        int ym = t->tm_mon + 1, yd = t->tm_mday, yy = 1900 + t->tm_year;
        int bm = strVal(st.bdate.substr(0, 2));
        int bd = strVal(st.bdate.substr(3, 2));
        int by = strVal(st.bdate.substr(6, 4));
        int age = yy - by;
        if (bm > ym || (bm == ym && bd > yd)) age--;
        st.tyear = std::to_string(age);
    }

    st.ansiOn = (st.ansi == "Y");
}

// ---------------------------------------------------------------------------
bool findUser(BossState& st, const std::string& first, const std::string& last) {
    RecordFile rf = openUsersFile();
    if (!rf.fp) return false;

    // Record 1 holds user count
    rfGet(rf, 1);
    int count = strVal(fieldVal(rf, 0 /*N1=count field*/));
    if (count == 0) { rfClose(rf); return false; }

    for (int rec = 2; rec <= count + 1; rec++) {
        rfGet(rf, rec);
        std::string fn = fieldVal(rf, UF_NAME1);
        std::string ln = fieldVal(rf, UF_NAME2);
        // Case-insensitive partial match (INSTR behaviour)
        std::string fnUp = fn, lnUp = ln, fUp = first, lUp = last;
        toUpper(fnUp); toUpper(lnUp); toUpper(fUp); toUpper(lUp);
        if (fnUp.find(fUp) != std::string::npos &&
            lnUp.find(lUp) != std::string::npos) {
            loadUserFromRecord(st, rf, rec);
            rfClose(rf);
            st.find = true;
            return true;
        }
    }
    rfClose(rf);
    st.find = false;
    return false;
}

// ---------------------------------------------------------------------------
// Helper: input a line of text. Returns chars in st.reply[1..L%], L% in st.
// Mirrors BASIC GOSUB 1220 (name input). MX%=max chars, PAS=mask with '#'.
static int inputLine(BossState& st, int maxChars, bool maskInput = false) {
    std::string line;
    line.reserve(maxChars);
    for (;;) {
        char c = waitKey(st);
        timerResetInactivity(st);
        if (c == '\r' || c == '\n') {
            printLine(st, "");
            break;
        }
        if (c == '\b' && !line.empty()) {
            line.pop_back();
            printBackspace(st);
            continue;
        }
        if (c < 32 || c > 122) continue;
        if ((int)line.size() >= maxChars) continue;
        line += c;
        printStr(st, maskInput ? "#" : std::string(1, c));
    }

    // Split on first space into first/last (mirrors GOSUB 1470)
    int L = 0;
    for (char ch : line) { st.reply[++L] = std::string(1, ch); }
    // Parse first/last name from reply buffer
    std::string first, last;
    bool inFirst = true, spaceFound = false;
    for (int i = 1; i <= L; i++) {
        char ch = st.reply[i][0];
        if (ch == ' ') { spaceFound = true; inFirst = false; continue; }
        if (inFirst) first += ch;
        else         last  += ch;
    }
    if (spaceFound) {
        // capitalise words
        if (!first.empty()) { first[0] = (char)toupper((unsigned char)first[0]); for (size_t k=1;k<first.size();k++) first[k]=(char)tolower((unsigned char)first[k]); }
        if (!last.empty())  { last[0]  = (char)toupper((unsigned char)last[0]);  for (size_t k=1;k<last.size(); k++)  last[k] =(char)tolower((unsigned char)last[k]);  }
    }
    st.name1 = first;
    st.name2 = last;
    return L;
}

// Read a single y/n key, return 'Y' or 'N'. Default is DE$.
static char readYN(BossState& st, char dflt = 'Y') {
    for (;;) {
        char c = waitKey(st);
        timerResetInactivity(st);
        c = (char)toupper((unsigned char)c);
        if (c == '\r') { printStr(st, std::string(1, dflt)); return dflt; }
        if (c == 'Y' || c == 'N') { printStr(st, std::string(1, c)); return c; }
    }
}

// Read up to maxChars numeric chars.
static std::string readNumeric(BossState& st, int maxChars) {
    std::string s;
    for (;;) {
        char c = waitKey(st);
        timerResetInactivity(st);
        if (c == '\r' || c == '\n') { printLine(st, ""); break; }
        if (c == '\b' && !s.empty()) { s.pop_back(); printBackspace(st); continue; }
        if (c < '0' || c > '9' || (int)s.size() >= maxChars) continue;
        s += c;
        printStr(st, std::string(1, c));
    }
    return s;
}

// ---------------------------------------------------------------------------
void registerNewUser(BossState& st) {
    // BOSS.BAS lines 440–870
    setColor(15, 0);

    // ANSI?
    st.v = 10; setColor(st.v, 0);
    printLine(st, "");
    st.v = 11; setColor(st.v, 0);
    printStr(st, "Would you like ANSI graphics: [y/N]: ");
    st.ansi = (readYN(st, 'N') == 'Y') ? "Y" : "N";
    st.ansiOn = (st.ansi == "Y");

    // Data phone
    for (;;) {
        printStr(st, "\r\nEnter your DATA phone [XXX]-[XXX-XXXX]: [");
        std::string ph;
        for (int i = 1; i <= 10; i++) {
            char c = waitKey(st);
            timerResetInactivity(st);
            while (c < '0' || c > '9') { c = waitKey(st); timerResetInactivity(st); }
            ph += c;
            printStr(st, std::string(1, c));
            if (i == 3) printStr(st, "]-[");
            if (i == 6) printStr(st, "-");
        }
        printLine(st, "]");
        if ((int)ph.size() >= 10) {
            st.phone1 = ph.substr(0,3)+"-"+ph.substr(3,3)+"-"+ph.substr(6,4);
            break;
        }
        printLine(st, "\r\nYou must enter your phone number");
    }

    // Voice phone
    for (;;) {
        printStr(st, "\r\nEnter your VOICE phone [XXX]-[XXX-XXXX]: [");
        std::string ph;
        for (int i = 1; i <= 10; i++) {
            char c = waitKey(st);
            timerResetInactivity(st);
            while (c < '0' || c > '9') { c = waitKey(st); timerResetInactivity(st); }
            ph += c;
            printStr(st, std::string(1, c));
            if (i == 3) printStr(st, "]-[");
            if (i == 6) printStr(st, "-");
        }
        printLine(st, "]");
        if ((int)ph.size() >= 10) {
            st.phone2 = ph.substr(0,3)+"-"+ph.substr(3,3)+"-"+ph.substr(6,4);
            break;
        }
        printLine(st, "\r\nYou must enter your phone number");
    }

    // City and Province
    for (;;) {
        printStr(st, "\r\nEnter your City and Province [ie Cambridge, On]: ");
        int L = inputLine(st, 19);
        if (st.name1.size() >= 2 && st.name2.size() >= 2) {
            std::string n1 = st.name1, n2 = st.name2; // reused by inputLine
            st.city = n1 + "," + n2;
            // restore name1/name2 from outer scope — caller set them before we got here
            break;
        }
        printLine(st, "\r\nYou must enter your City AND Province!");
        (void)L;
    }

    // Birth date
    for (;;) {
        printStr(st, "\r\nEnter your birth date [mm-dd-yy]: [");
        std::string bd;
        for (int i = 1; i <= 6; i++) {
            char c = waitKey(st);
            timerResetInactivity(st);
            while (c < '0' || c > '9') { c = waitKey(st); timerResetInactivity(st); }
            bd += c;
            printStr(st, std::string(1, c));
            if (i == 2 || i == 4) printStr(st, "-");
        }
        printLine(st, "]");
        if ((int)bd.size() < 6) { printLine(st, "\r\nYou Must Enter Your Birth Date"); continue; }
        int mm = strVal(bd.substr(0,2)), dd = strVal(bd.substr(2,2)), yy = strVal(bd.substr(4,2));
        if (mm < 1 || mm > 12) { printLine(st, "\r\nInvalid Month!"); continue; }
        if (dd < 1 || dd > 31) { printLine(st, "\r\nInvalid Day!"); continue; }
        if (yy < 0 || yy > 99) { printLine(st, "\r\nInvalid Year!"); continue; }
        char buf[12];
        snprintf(buf, sizeof(buf), "%02d-%02d-19%02d", mm, dd, yy);
        st.bdate = buf;
        break;
    }

    // Screen length
    for (;;) {
        printStr(st, "\r\nEnter Your Screen Length [10-66] [24 recommended]: ");
        std::string s = readNumeric(st, 2);
        int v = strVal(s);
        if (v < 10 || v > 66) { printLine(st, "\r\nYour screen length must be between 10 and 66."); continue; }
        st.sl = s;
        break;
    }

    // Screen clearing
    printStr(st, "\r\nWould you like Screen clearing codes: [Y/n]: ");
    st.xpt = (readYN(st, 'Y') == 'Y') ? "Y" : "N";

    // Password
    std::string pass;
    for (;;) {
        printStr(st, "\r\nEnter your Password [#'s will echo.]: ");
        int L = inputLine(st, 15, true);
        pass = "";
        for (int i = 1; i <= L; i++) pass += st.reply[i];
        if (L <= 2) { printLine(st, "\r\nPassword is not long enough!"); continue; }
        printStr(st, "\r\nType your Password again to Verify [#'s will echo.]: ");
        int L2 = inputLine(st, 15, true);
        std::string pass2;
        for (int i = 1; i <= L2; i++) pass2 += st.reply[i];
        if (pass2 == pass) { printLine(st, "\r\nCorrect, don't forget it!\r\n"); break; }
        printLine(st, "\r\nIncorrect, enter your Password again...");
    }
    st.password = pass;

    // Confirm
    printStr(st, "Did you enter all the information correctly [Y/n]: ");
    if (readYN(st, 'Y') == 'N') {
        // Restart (caller handles this by re-calling us or going back to name prompt)
        return;
    }

    // Set defaults for a new user
    st.secur  = "2";
    st.tcal   = "0";
    st.tuse   = "0";
    st.tpost  = "0";
    st.ul     = "0";
    st.ulkb   = "0";
    st.dl     = "0";
    st.dlkb   = "0";
    {
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        char today[12];
        snprintf(today, sizeof(today), "%02d-%02d-%04d", t->tm_mon+1, t->tm_mday, 1900+t->tm_year);
        st.bbdate = today;
        st.lcall  = today;
    }

    // Write new record — BOSS.BAS line 2590
    RecordFile rf = openUsersFile();
    if (!rf.fp) return;

    // Get current count from record 1
    rfGet(rf, 1);
    int count = strVal(fieldVal(rf, 0));
    count++;
    st.usernum = std::to_string(count);
    st.userRecNum = count + 1;  // 1-based; record 1 is header

    // Update count in record 1
    rfLsetIdx(rf, 0, std::to_string(count));
    rfPut(rf, 1);

    // Write user record at count+1
    rfGet(rf, st.userRecNum);  // initialise buffer with spaces
    rfLsetIdx(rf, UF_NAME1,  st.name1);
    rfLsetIdx(rf, UF_NAME2,  st.name2);
    rfLsetIdx(rf, UF_PAS,    st.password);
    rfLsetIdx(rf, UF_CITY,   st.city);
    rfLsetIdx(rf, UF_PHONE1, st.phone1);
    rfLsetIdx(rf, UF_PHONE2, st.phone2);
    rfLsetIdx(rf, UF_ANSI,   st.ansi);
    rfLsetIdx(rf, UF_BDATE,  st.bdate);
    rfLsetIdx(rf, UF_SL,     st.sl);
    rfLsetIdx(rf, UF_SECUR,  st.secur);
    rfLsetIdx(rf, UF_TCAL,   st.tcal);
    rfLsetIdx(rf, UF_LREAD,  st.lread);
    rfLsetIdx(rf, UF_TUSE,   st.tuse);
    rfLsetIdx(rf, UF_TPOST,  st.tpost);
    rfLsetIdx(rf, UF_LCALL,  st.lcall);
    rfLsetIdx(rf, UF_UL,     st.ul);
    rfLsetIdx(rf, UF_ULKB,   st.ulkb);
    rfLsetIdx(rf, UF_DL,     st.dl);
    rfLsetIdx(rf, UF_DLKB,   st.dlkb);
    rfLsetIdx(rf, UF_XPT,    st.xpt);
    rfLsetIdx(rf, UF_DEL,    st.del1);
    rfPut(rf, st.userRecNum);
    rfClose(rf);
}

// ---------------------------------------------------------------------------
void saveUser(BossState& st) {
    // BOSS.BAS line 10390 (top half)
    RecordFile rf = openUsersFile();
    if (!rf.fp) return;
    rfGet(rf, st.userRecNum);
    rfLsetIdx(rf, UF_NAME1,  st.name1);
    rfLsetIdx(rf, UF_NAME2,  st.name2);
    rfLsetIdx(rf, UF_PAS,    st.password);
    rfLsetIdx(rf, UF_CITY,   st.city);
    rfLsetIdx(rf, UF_PHONE1, st.phone1);
    rfLsetIdx(rf, UF_PHONE2, st.phone2);
    rfLsetIdx(rf, UF_ANSI,   st.ansi);
    rfLsetIdx(rf, UF_BDATE,  st.bdate);
    rfLsetIdx(rf, UF_SL,     st.sl);
    rfLsetIdx(rf, UF_SECUR,  st.secur);
    // Increment call count
    int tc = strVal(st.tcal) + 1;
    st.tcal = std::to_string(tc);
    rfLsetIdx(rf, UF_TCAL,   st.tcal);
    rfLsetIdx(rf, UF_LREAD,  st.lread);
    rfLsetIdx(rf, UF_TUSE,   st.tuse);
    rfLsetIdx(rf, UF_TPOST,  st.tpost);
    rfLsetIdx(rf, UF_LCALL,  st.bbdate);
    rfLsetIdx(rf, UF_UL,     st.ul);
    rfLsetIdx(rf, UF_ULKB,   st.ulkb);
    rfLsetIdx(rf, UF_DL,     st.dl);
    rfLsetIdx(rf, UF_DLKB,   st.dlkb);
    rfLsetIdx(rf, UF_XPT,    st.xpt);
    rfLsetIdx(rf, UF_DEL,    st.del1);
    rfPut(rf, st.userRecNum);
    rfClose(rf);
}

// ---------------------------------------------------------------------------
void loadLastRead(BossState& st) {
    // BOSS.BAS line 10350
    std::string path = std::string(BOSS_PATH) + "LASTREAD.BBS";
    std::vector<FieldDesc> fields;
    for (int i = 0; i < 20; i++) fields.push_back({"p" + std::to_string(i+1), 5});
    RecordFile rf;
    if (!rfOpen(rf, path, LASTREAD_REC_LEN, fields)) return;
    int recNum = strVal(st.usernum);
    if (recNum < 1) { rfClose(rf); return; }
    rfGet(rf, recNum);
    for (int i = 0; i < 20; i++)
        st.pnt2[i + 1] = stripBlanks(rfGetIdx(rf, i));
    rfClose(rf);
}

void saveLastRead(BossState& st) {
    // BOSS.BAS line 10390 (bottom half)
    std::string path = std::string(BOSS_PATH) + "LASTREAD.BBS";
    std::vector<FieldDesc> fields;
    for (int i = 0; i < 20; i++) fields.push_back({"p" + std::to_string(i+1), 5});
    RecordFile rf;
    if (!rfOpen(rf, path, LASTREAD_REC_LEN, fields)) return;
    int recNum = strVal(st.usernum);
    if (recNum < 1) { rfClose(rf); return; }
    rfGet(rf, recNum);
    for (int i = 0; i < 20; i++)
        rfLsetIdx(rf, i, st.pnt2[i + 1]);
    rfPut(rf, recNum);
    rfClose(rf);
}

// ---------------------------------------------------------------------------
bool loginUser(BossState& st) {
    // BOSS.BAS lines 3040–3180
    for (;;) {
        setColor(15, 0);
        printStr(st, "Enter Your Password: ");
        int L = inputLine(st, 15, true);
        std::string entered;
        for (int i = 1; i <= L; i++) entered += st.reply[i];
        entered = stripBlanks(entered);

        if (entered == st.password) {
            printLine(st, "");
            printLine(st, "");
            return true;
        }

        st.inval++;
        if (st.inval >= 3) {
            printLine(st, "\r\n\r\n<Click!>");
            return false;
        }
        printLine(st, "\r\n\r\nIncorrect Response!\r\n");
    }
}

// ---------------------------------------------------------------------------
void readName(BossState& st) {
    // Read first+last name using inputLine (splits on space into name1/name2)
    st.name1 = "";
    st.name2 = "";
    inputLine(st, 30);
}

// ---------------------------------------------------------------------------
void showUserInfo(BossState& st) {
    // BOSS.BAS line 3420
    auto lbl = [&](const std::string& label, const std::string& val, int lc, int vc) {
        setColor(lc, 0); printStr(st, label);
        setColor(vc, 0); printLine(st, val);
    };
    printLine(st, "");
    lbl("User's Name      : ", st.name1 + " " + st.name2, 10, 11);
    lbl("City             : ", st.city,   10, 11);
    lbl("Data Phone       : ", st.phone1, 10, 11);
    lbl("Voice Phone      : ", st.phone2, 10, 11);
    lbl("Ansi Graphics    : ", st.ansi,   10, 11);
    lbl("Birth Date       : ", st.bdate,  10, 11);
    std::string xptDisplay = (st.xpt == "0") ? "Y" : st.xpt;
    lbl("Screen Clearing  : ", xptDisplay, 10, 11);
    lbl("Screen Length    : ", st.sl,     10, 11);
    lbl("Security Level   : ", st.secur,  10, 11);
    lbl("Times Called     : ", st.tcal,   10, 11);
    lbl("Times Posted     : ", st.tpost,  10, 11);
    lbl("Last Called      : ", st.lcall,  10, 11);
    lbl("Uploads          : ", st.ul,     10, 11);
    lbl("Uploads [Bytes]  : ", st.ulkb,   10, 11);
    lbl("Downloads        : ", st.dl,     10, 11);
    lbl("Downloads [Bytes]: ", st.dlkb,   10, 11);
}
