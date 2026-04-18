// menus.cpp — Menu system implementation
// BOSS.BAS reference lines: 4240–4460, 12180–12350

#include "menus.h"
#include "console.h"
#include "serial.h"
#include "timer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

// ---------------------------------------------------------------------------
void loadMenu(BossState& st, const std::string& menuFile) {
    // BOSS.BAS line 4240: INPUT letter$, menu$, ms$, op$ for each item
    std::string path = std::string(MENU_PATH) + menuFile;
    std::ifstream f(path);
    if (!f.is_open()) {
        // Try relative path (current dir = BOSS dir)
        f.open(std::string(BOSS_PATH) + "MENU\\" + menuFile);
    }
    if (!f.is_open()) return;

    st.menuLen = 0;
    std::string line;
    while (std::getline(f, line)) {
        int L = st.menuLen + 1;
        if (L > MAX_MENU_ITEMS) break;
        st.letter[L]  = line;
        if (!std::getline(f, line)) break;
        st.menuDest[L] = line;
        if (!std::getline(f, line)) break;
        st.ms[L] = line;
        if (!std::getline(f, line)) break;
        st.op[L] = line;
        st.menuLen = L;
    }
    st.menuFile = menuFile;
}

// ---------------------------------------------------------------------------
void displayMenuArt(BossState& st) {
    // BOSS.BAS line 4400/4460
    // Build base name from menu file (strip extension)
    std::string base;
    size_t dot = st.menuFile.find('.');
    if (dot != std::string::npos)
        base = st.menuFile.substr(0, dot);
    else
        base = st.menuFile;

    // Security level string (strip leading space from STR$(n))
    int sl = strVal(st.secur);
    std::string slStr = std::to_string(sl);

    // Try ANS first (ANSI users), then ASC
    std::vector<std::string> exts;
    if (st.ansiOn)
        exts = {".ANS", ".ASC"};
    else
        exts = {".ASC"};

    for (const auto& ext : exts) {
        std::string path = std::string(MENU_PATH) + base + "-" + slStr + ext;
        std::ifstream f(path);
        if (!f.is_open()) continue;

        // Load all lines into st.titl[]
        st.titlCount = 0;
        std::string line;
        while (std::getline(f, line) && st.titlCount < MAX_MENU_ITEMS) {
            st.titl[++st.titlCount] = line;
        }
        st.ntf = false;
        return;
    }
    st.ntf = true;
}

// ---------------------------------------------------------------------------
static void displayMenuLines(BossState& st, int timeRemaining) {
    // BOSS.BAS line 12240 — stream menu art lines with colour codes
    clearIfEnabled(st);
    for (int L = 1; L <= st.titlCount; L++) {
        const std::string& raw = st.titl[L];

        // Scan for colour code CHR$(1) at position < 8
        int colourVal = -1;
        int colourPos = -1;
        for (int i = 0; i < (int)raw.size() && i < 8; i++) {
            if ((unsigned char)raw[i] == 1) {
                // Next two chars are the colour number string
                if (i + 2 < (int)raw.size()) {
                    std::string cs = raw.substr(i + 1, 2);
                    colourVal = strVal(cs);
                    colourPos = i;
                }
                break;
            }
        }

        // Build display string (strip control bytes)
        std::string display = raw;
        if (colourPos >= 0) {
            display = raw.substr(0, colourPos) + "   " + raw.substr(colourPos + 3);
            setColor(colourVal, 0);
            if (st.ansiOn) {
                // Send ANSI sequence to remote
                writeSerial(st, ansiColor(colourVal));
            }
        }

        // Last line gets " [XX minutes]: " appended
        if (L == st.titlCount) {
            display += " [" + std::to_string(timeRemaining) + " minutes]: ";
            printStr(st, display);
        } else {
            printLine(st, display);
        }
    }
}

// ---------------------------------------------------------------------------
std::string buildValidKeys(const BossState& st) {
    std::string ps;
    int userSec = strVal(st.secur);
    for (int i = 1; i <= st.menuLen; i++) {
        if (strVal(st.ms[i]) > userSec) continue;
        ps += st.letter[i];
        // Also allow lowercase equivalent
        if (st.letter[i] >= "A" && st.letter[i] <= "Z") {
            char lc = (char)(st.letter[i][0] + 32);
            ps += std::string(1, lc);
        }
    }
    return ps;
}

// ---------------------------------------------------------------------------
int showMenu(BossState& st) {
    displayMenuArt(st);

    // If ntf = true (no art file), just print a blank line
    if (st.ntf) {
        // Display the raw menu text as plain prompts
        displayMenuLines(st, st.qt);
    } else {
        displayMenuLines(st, st.qt);
    }

    // Build the set of valid keys the user can press
    std::string ps = buildValidKeys(st);

    // Read keystroke (BOSS.BAS line 6120)
    std::string ant;
    for (;;) {
        timerCheck(st);
        char c = inkey(st);
        if (c == '\0') continue;
        if (c == '\r') { ant = ""; break; }
        ant = std::string(1, (char)toupper((unsigned char)c));
        if (ps.find(c) != std::string::npos ||
            ps.find((char)toupper((unsigned char)c)) != std::string::npos) break;
        // Invalid key — ignore
        ant = "";
    }
    toUpper(ant);
    st.ant = ant;

    // Find matching menu item
    for (int i = 1; i <= st.menuLen; i++) {
        if (st.letter[i] == ant) {
            st.km = i;
            printStr(st, ant);
            return i;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
void displayTextFile(BossState& st, const std::string& filename) {
    // BOSS.BAS line 11710
    std::string path = std::string(BOSS_PATH) + filename;
    std::ifstream f(path);
    if (!f.is_open()) return;

    int how = 0;
    int slLen = strVal(st.sl);
    if (slLen < 10) slLen = 24;
    bool abort_ = false;

    // Special: if it's bye.txt, write last caller name
    if (filename == "bye.txt" || filename == "BYE.TXT") {
        std::ofstream last(std::string(BOSS_PATH) + "last.dat");
        if (last.is_open())
            last << st.name1 + " " + st.name2 << "\n";
    }

    std::string line;
    while (std::getline(f, line)) {
        if (abort_) break;

        // Scan for control codes in first 8 chars
        std::string display = line;
        for (int i = 0; i < (int)line.size() && i < 8; i++) {
            unsigned char ch = (unsigned char)line[i];
            if (ch == 1) {
                // Colour code: next 2 chars
                if (i + 2 < (int)line.size()) {
                    int v = strVal(line.substr(i + 1, 2));
                    setColor(v, 0);
                    if (st.ansiOn) writeSerial(st, ansiColor(v));
                    display = line.substr(0, i) + "   " + line.substr(i + 3);
                }
                break;
            }
            if (ch == 2) { clearIfEnabled(st); display = ""; break; }
            if (ch == 3) {
                pressReturn(st);
                display = "";
                break;
            }
        }

        if (!display.empty() || line.empty()) {
            printLine(st, display);
            how++;
            if (how >= slLen) {
                // "More [Y/n]:" prompt
                setColor(14, 0);
                printStr(st, "More [Y/n]: ");
                char yn = waitKey(st);
                timerResetInactivity(st);
                yn = (char)toupper((unsigned char)yn);
                printStr(st, std::string(1, yn));
                // Erase the prompt (backspace sequence)
                for (int k = 0; k < 13; k++) printBackspace(st);
                if (yn == 'N') { abort_ = true; printLine(st, "[Aborted]"); break; }
                how = 0;
            }
        }

        // Check [S]top or [P]ause while displaying
        char key = inkey(st);
        if (key != '\0') {
            key = (char)toupper((unsigned char)key);
            if (key == 'S') { abort_ = true; break; }
            if (key == 'P') {
                // Wait for any key
                waitKey(st);
                timerResetInactivity(st);
            }
        }
    }
    if (filename == "lastcall.txt" || filename == "LASTCALL.TXT") {
        setColor(14, 0);
        printStr(st, "\r\n[Press Return]: ");
        waitKey(st);
        timerResetInactivity(st);
        printLine(st, "");
    }
}

// ---------------------------------------------------------------------------
void pressReturn(BossState& st) {
    setColor(11, 0);
    printStr(st, "[Press Return]: ");
    waitKey(st);
    timerResetInactivity(st);
    printLine(st, "");
}
