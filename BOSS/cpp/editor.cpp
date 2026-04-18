// editor.cpp — Full-screen BASIC-style line editor
// BOSS.BAS reference lines: 5420–7850, 6870

#include "editor.h"
#include "console.h"
#include "timer.h"
#include <algorithm>
#include <cstring>
#include <sstream>

// ---------------------------------------------------------------------------
// Print line number prefix "NN▓  " (BOSS.BAS line 5790)
static std::string linePrefix(int n) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d\xB2  ", n);
    return buf;
}

// Print all buffered lines (BOSS.BAS line 6230)
void editorList(BossState& st) {
    setColor(14, 0);
    printLine(st, "The Boss Message Editor");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\r\n");
    for (int i = 1; i <= st.mlen; i++) {
        setColor(11, 0);
        printStr(st, linePrefix(i));
        setColor(15, 0);
        printLine(st, st.tessage[i]);
    }
    printLine(st, "");
}

// ---------------------------------------------------------------------------
// Read a line of input up to maxChars, with word-wrap at 73.
// Used for subject / filename prompts. BOSS.BAS GOSUB 6870.
std::string editorReadLine(BossState& st, int maxChars, bool password) {
    std::string line;
    line.reserve(maxChars);
    for (;;) {
        timerCheck(st);
        char c = inkey(st);
        if (c == '\0') continue;
        timerResetInactivity(st);
        if (c == '\r' || c == '\n') { printLine(st, ""); break; }
        if (c == '\b' && !line.empty()) {
            line.pop_back();
            printBackspace(st);
            continue;
        }
        if (c < 32 || c > 122) continue;
        if ((int)line.size() >= maxChars) continue;
        line += c;
        if (password) {
            printStr(st, "#");
        } else {
            // Capitalise first char, lowercase the rest
            if (line.size() == 1)
                line[0] = (char)toupper((unsigned char)line[0]);
            printStr(st, std::string(1, c));
        }
    }
    return line;
}

// ---------------------------------------------------------------------------
// Editor command menu — BOSS.BAS line 5900
static char editorMenu(BossState& st) {
    setColor(14, 0);
    printLine(st, "\r\nMessage Editor Commands: ");
    setColor(11, 0);
    printLine(st, "\r\n  [C]ontinue  [E]dit    [I]nsert  [L]ist  [D]elete");
    printLine(st,   "  [R]eplace   [F]ormat  [S]ave    [A]bort");
    setColor(12, 0);
    printStr(st, "\r\nEnter Message Editor Command: ");

    const std::string valid = "LCEDIFRSA";
    for (;;) {
        timerCheck(st);
        char c = inkey(st);
        if (c == '\0') continue;
        timerResetInactivity(st);
        c = (char)toupper((unsigned char)c);
        if (valid.find(c) != std::string::npos) {
            printLine(st, std::string(1, c));
            return c;
        }
        if (c == '\r') return '\0';
    }
}

// ---------------------------------------------------------------------------
// Edit a single existing line — BOSS.BAS line 6980
static void cmdEdit(BossState& st) {
    int last = st.mlen;
    setColor(14, 0);
    printStr(st, "\r\nEnter Line Number To Edit [1-" + std::to_string(last) + "]: ");
    std::string s;
    for (;;) {
        char c = waitKey(st); timerResetInactivity(st);
        if (c == '\r') { printLine(st, ""); break; }
        if (c < '0' || c > '9') continue;
        if ((int)s.size() >= 2) continue;
        s += c; printStr(st, std::string(1, c));
    }
    if (s.empty()) return;
    int e = strVal(s);
    if (e < 1 || e > last) { printLine(st, "\r\nInvalid line number."); return; }

    setColor(10, 0);
    printLine(st, "\r\n    \xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB");
    setColor(11, 0);
    printStr(st, linePrefix(e));
    setColor(15, 0);
    printLine(st, st.tessage[e]);

    setColor(11, 0);
    printStr(st, "New text: ");
    std::string newLine = editorReadLine(st, MSG_LINE_WIDTH);
    if (newLine.empty()) { printLine(st, "\r\nEdit aborted."); return; }
    st.tessage[e] = newLine;
}

// ---------------------------------------------------------------------------
// Replace a string in a line — BOSS.BAS line 6640
static void cmdReplace(BossState& st) {
    int last = st.mlen;
    setColor(14, 0);
    printStr(st, "\r\nEnter Line Number To Replace In [1-" + std::to_string(last) + "]: ");
    std::string s;
    for (;;) {
        char c = waitKey(st); timerResetInactivity(st);
        if (c == '\r') { printLine(st, ""); break; }
        if (c < '0' || c > '9') continue;
        if ((int)s.size() >= 2) continue;
        s += c; printStr(st, std::string(1, c));
    }
    if (s.empty()) return;
    int e = strVal(s);
    if (e < 1 || e > last) { printLine(st, "\r\nInvalid line number."); return; }

    setColor(11, 0);
    printStr(st, "\r\nSearch for: ");
    std::string srch = editorReadLine(st, MSG_LINE_WIDTH);
    if (srch.empty()) { printLine(st, "\r\nReplace aborted."); return; }
    printStr(st, "Replace with: ");
    std::string repl = editorReadLine(st, MSG_LINE_WIDTH);

    size_t pos = st.tessage[e].find(srch);
    if (pos == std::string::npos) { printLine(st, "\r\nString not found."); return; }
    std::string newLine = st.tessage[e].substr(0, pos) + repl + st.tessage[e].substr(pos + srch.size());
    if ((int)newLine.size() > MSG_LINE_WIDTH) { printLine(st, "\r\nThat string is too big to fit on that line."); return; }

    setColor(11, 0);
    printLine(st, "\r\nLine " + s + " now reads:\r\n");
    printStr(st, linePrefix(e)); printLine(st, newLine);
    setColor(14, 0);
    printStr(st, "\r\nIs this change correct [Y/n]: ");
    char yn = waitKey(st); timerResetInactivity(st);
    yn = (char)toupper((unsigned char)yn);
    printStr(st, std::string(1, yn));
    if (yn != 'N') st.tessage[e] = newLine;
}

// ---------------------------------------------------------------------------
// Insert a line before a given line — BOSS.BAS line 7300
static void cmdInsert(BossState& st) {
    if (st.mlen >= 79) { printLine(st, "\r\n   Message buffer is full."); return; }
    int last = st.mlen;
    setColor(14, 0);
    printStr(st, "\r\nEnter Line Number To Insert Before [2-" + std::to_string(last) + "]: ");
    std::string s;
    for (;;) {
        char c = waitKey(st); timerResetInactivity(st);
        if (c == '\r') { printLine(st, ""); break; }
        if (c < '0' || c > '9') continue;
        if ((int)s.size() >= 2) continue;
        s += c; printStr(st, std::string(1, c));
    }
    if (s.empty()) return;
    int e = strVal(s);
    if (e < 2 || e > last) { printLine(st, "\r\nInvalid line number."); return; }

    setColor(15, 0);
    printLine(st, "\r\n\r\nEnter new text below, RETURN aborts insert option.\r\n");
    printStr(st, linePrefix(e - 1));
    std::string newLine = editorReadLine(st, MSG_LINE_WIDTH);
    if (newLine.empty()) { printLine(st, "\r\nInsert aborted."); return; }

    // Shift lines down
    st.mlen++;
    for (int i = st.mlen; i > e; i--)
        st.tessage[i] = st.tessage[i - 1];
    st.tessage[e] = newLine;
}

// ---------------------------------------------------------------------------
// Delete a line — BOSS.BAS line 7590
static void cmdDelete(BossState& st) {
    if (st.mlen < 1) { printLine(st, "\r\nThere are no lines to delete."); return; }
    int last = st.mlen;
    setColor(14, 0);
    printStr(st, "\r\nEnter Line Number To Delete [1-" + std::to_string(last) + "]: ");
    std::string s;
    for (;;) {
        char c = waitKey(st); timerResetInactivity(st);
        if (c == '\r') { printLine(st, ""); break; }
        if (c < '0' || c > '9') continue;
        if ((int)s.size() >= 2) continue;
        s += c; printStr(st, std::string(1, c));
    }
    if (s.empty()) return;
    int e = strVal(s);
    if (e < 1 || e > last) { printLine(st, "\r\nInvalid line number."); return; }

    setColor(12, 0);
    printLine(st, "\r\n\r\nLine " + s + " reads:\r\n");
    printStr(st, linePrefix(e)); printLine(st, st.tessage[e]);
    printStr(st, "\r\nAre you sure you want to delete this line [Y/n]: ");
    char yn = waitKey(st); timerResetInactivity(st);
    yn = (char)toupper((unsigned char)yn);
    printStr(st, std::string(1, yn));
    if (yn == 'N') { printLine(st, ""); return; }

    for (int i = e; i < st.mlen; i++)
        st.tessage[i] = st.tessage[i + 1];
    st.tessage[st.mlen] = "";
    st.mlen--;
    printLine(st, "\r\nDeletion Complete");
}

// ---------------------------------------------------------------------------
// Format a line (Centre/Right/Left justify) — BOSS.BAS line 7850
static void cmdFormat(BossState& st) {
    setColor(14, 0);
    printLine(st, "\r\nLine Formatter");
    printLine(st, "\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4");
    printLine(st, "\r\n[C]enter Line\r\n[R]ight Justify\r\n[L]eft Justify");
    printStr(st, "\r\nEnter Formatting Command: ");

    char jj = '\0';
    for (;;) {
        timerCheck(st);
        char c = inkey(st); if (c == '\0') continue;
        timerResetInactivity(st);
        c = (char)toupper((unsigned char)c);
        if (c == 'C' || c == 'R' || c == 'L') { jj = c; printLine(st, std::string(1, c)); break; }
    }

    int last = st.mlen;
    printStr(st, "\r\nEnter Line Number To Format [1-" + std::to_string(last) + "]: ");
    std::string s;
    for (;;) {
        char c = waitKey(st); timerResetInactivity(st);
        if (c == '\r') { printLine(st, ""); break; }
        if (c < '0' || c > '9') continue;
        if ((int)s.size() >= 2) continue;
        s += c; printStr(st, std::string(1, c));
    }
    if (s.empty()) return;
    int e = strVal(s);
    if (e < 1 || e > last) { printLine(st, "\r\nInvalid line number."); return; }

    std::string line = st.tessage[e];
    std::string result;
    if (jj == 'L') {
        // Strip leading spaces
        result = strTrim(line);
    } else if (jj == 'R') {
        // Right justify to 72
        std::string trimmed = strTrim(line);
        int spaces = MSG_LINE_WIDTH - (int)trimmed.size();
        if (spaces < 0) spaces = 0;
        result = std::string(spaces, ' ') + trimmed;
    } else if (jj == 'C') {
        // Centre
        std::string trimmed = strTrim(line);
        int spaces = (MSG_LINE_WIDTH - (int)trimmed.size()) / 2;
        if (spaces < 0) spaces = 0;
        result = std::string(spaces, ' ') + trimmed;
    }

    setColor(11, 0);
    printLine(st, "\r\n\r\nLine " + s + " now reads:\r\n");
    printStr(st, linePrefix(e)); printLine(st, result);
    setColor(14, 0);
    printStr(st, "\r\nIs this change correct [Y/n]: ");
    char yn = waitKey(st); timerResetInactivity(st);
    yn = (char)toupper((unsigned char)yn);
    printStr(st, std::string(1, yn));
    if (yn != 'N') st.tessage[e] = result;
}

// ---------------------------------------------------------------------------
// Main editor loop — BOSS.BAS lines 5420–5900
void runEditor(BossState& st) {
    // Print first line number prompt
    setColor(15, 0);
    st.mlen++;
    printStr(st, linePrefix(st.mlen));

    std::string curLine;
    curLine.reserve(MSG_LINE_WIDTH + 4);

    for (;;) {
        // Check buffer full
        if (st.mlen > MAX_MSG_LINES) {
            setColor(14, 0);
            printLine(st, "\r\n\r\n   Message buffer is full.");
            // Drop to editor menu
            goto menu;
        }

        {
            timerCheck(st);
            char c = inkey(st);
            if (c == '\0') continue;
            timerResetInactivity(st);

            // Tab → 8 spaces
            if (c == '\t') {
                int spaces = 8 - ((int)curLine.size() % 8);
                for (int k = 0; k < spaces && (int)curLine.size() < MSG_LINE_WIDTH; k++) {
                    curLine += ' ';
                    printStr(st, " ");
                }
                continue;
            }

            // Backspace
            if (c == '\b') {
                if (!curLine.empty()) {
                    curLine.pop_back();
                    printBackspace(st);
                }
                continue;
            }

            // Enter — save current line, move to next
            if (c == '\r') {
                // If line is empty at line start → go to editor menu
                if (curLine.empty() && st.mlen >= 1) {
                    printLine(st, "");
                    goto menu;
                }
                st.tessage[st.mlen] = curLine;
                curLine.clear();
                printLine(st, "");
                st.mlen++;
                if (st.mlen <= MAX_MSG_LINES) {
                    setColor(15, 0);
                    printStr(st, linePrefix(st.mlen));
                }
                continue;
            }

            // Printable chars
            if (c < 32 || (unsigned char)c > 127) continue;
            if ((int)curLine.size() >= MSG_LINE_WIDTH) {
                // Word-wrap: backtrack to last space
                size_t sp = curLine.rfind(' ');
                if (sp != std::string::npos && sp > MSG_LINE_WIDTH / 2) {
                    // Move wrap portion to next line
                    std::string wrap = curLine.substr(sp + 1);
                    curLine = curLine.substr(0, sp);
                    // Erase wrapped portion on screen
                    for (size_t k = 0; k < wrap.size() + 1; k++) printBackspace(st);
                    st.tessage[st.mlen] = curLine;
                    curLine = wrap;
                    printLine(st, "");
                    st.mlen++;
                    if (st.mlen <= MAX_MSG_LINES) {
                        setColor(15, 0);
                        printStr(st, linePrefix(st.mlen));
                        printStr(st, curLine);
                    }
                } else {
                    // Hard wrap
                    st.tessage[st.mlen] = curLine;
                    curLine.clear();
                    printLine(st, "");
                    st.mlen++;
                    if (st.mlen <= MAX_MSG_LINES) {
                        setColor(15, 0);
                        printStr(st, linePrefix(st.mlen));
                    }
                }
                // Now add current char
                curLine += c;
                printStr(st, std::string(1, c));
                continue;
            }
            curLine += c;
            printStr(st, std::string(1, c));
            continue;
        }

        menu:
        {
            char cmd = editorMenu(st);
            switch (cmd) {
                case 'L': editorList(st);
                          // Show current line number again
                          if (st.mlen <= MAX_MSG_LINES)
                              printStr(st, linePrefix(st.mlen));
                          break;
                case 'C': // Continue
                          setColor(15, 0);
                          if (st.mlen <= MAX_MSG_LINES) {
                              printStr(st, linePrefix(st.mlen));
                              printStr(st, curLine);
                          }
                          goto editing;
                case 'R': cmdReplace(st); break;
                case 'E': cmdEdit(st);    break;
                case 'I': cmdInsert(st);  break;
                case 'D': cmdDelete(st);  break;
                case 'F': cmdFormat(st);  break;
                case 'A': // Abort
                          st.ok = false;
                          return;
                case 'S': // Save
                          {
                              // Save any partial line
                              if (!curLine.empty()) {
                                  st.tessage[st.mlen] = curLine;
                              } else {
                                  st.mlen--;
                              }
                              setColor(14, 0);
                              printLine(st, "\r\nSaving your message to disk. " + std::to_string(st.mlen) + " line(s) in length.");
                              st.ok = true;
                              return;
                          }
                default:
                          // Clear bad edit for remaining lines
                          for (int i = st.mlen; i <= MAX_MSG_LINES; i++) st.tessage[i] = "";
                          break;
            }
            goto menu;
        }
        editing:;
    }
}
