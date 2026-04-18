#pragma once
// boss.h — Global state, constants, and string utilities for BOSS BBS C++ port
// Translated from BOSS.BAS (GWBASIC/QBasic circa 1993) by C++ conversion tool.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <algorithm>
#include <cctype>

// ---------------------------------------------------------------------------
// Build-time paths (mirror the originals in BOSS.BAS)
// ---------------------------------------------------------------------------
static constexpr char BOSS_PATH[]     = "c:\\BOSS\\";
static constexpr char MENU_PATH[]     = "c:\\BOSS\\MENU\\";
static constexpr int  MAX_MSG_LINES   = 80;
static constexpr int  MAX_MENU_ITEMS  = 100;
static constexpr int  MAX_FILE_ITEMS  = 100;
static constexpr int  MAX_REPLY       = 200;
static constexpr int  MAX_MARK        = 50;
static constexpr int  MAX_MESS_AREAS  = 15;
static constexpr int  MAX_FILE_AREAS  = 7;
static constexpr int  MSG_LINE_WIDTH  = 72;
static constexpr int  USERS_REC_LEN   = 190;
static constexpr int  INDEX_REC_LEN   = 30;
static constexpr int  HEADER_REC_LEN  = 125;
static constexpr int  MESSAGE_REC_LEN = 72;
static constexpr int  LASTREAD_REC_LEN= 100;

// ---------------------------------------------------------------------------
// String helpers (mirror BASIC built-ins)
// ---------------------------------------------------------------------------
inline std::string strTrim(std::string s) {
    auto b = s.find_first_not_of(' ');
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(' ');
    return s.substr(b, e - b + 1);
}

inline std::string strLeft(const std::string& s, size_t n) {
    return s.substr(0, std::min(n, s.size()));
}

inline std::string strRight(const std::string& s, size_t n) {
    if (n >= s.size()) return s;
    return s.substr(s.size() - n);
}

inline std::string strMid(const std::string& s, size_t start, size_t len = std::string::npos) {
    if (start == 0 || start > s.size()) return "";
    return s.substr(start - 1, len);  // BASIC is 1-based
}

inline int strVal(const std::string& s) {
    if (s.empty()) return 0;
    try { return std::stoi(strTrim(s)); } catch (...) { return 0; }
}

// BASIC STR$(n) — produces leading space for positive, no space for negative
inline std::string strStr(int n) {
    if (n >= 0) return " " + std::to_string(n);
    return std::to_string(n);
}

// Numeric part without leading space (mid$(str$(n),2,...))
inline std::string numStr(int n) {
    return std::to_string(std::abs(n));
}

inline void toUpper(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
}

inline void toLower(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

// LSET: left-pad/truncate to exactly width, space-padded on right
inline std::string lset(const std::string& val, int width) {
    if ((int)val.size() >= width) return val.substr(0, width);
    return val + std::string(width - val.size(), ' ');
}

// Strip trailing spaces (mirrors CALL STRIPBLANKS)
inline std::string stripBlanks(const std::string& s) {
    return strTrim(s);
}

// ---------------------------------------------------------------------------
// Message-area and file-area name tables (BOSS.BAS lines 171)
// ---------------------------------------------------------------------------
static const std::array<std::string, MAX_MESS_AREAS + 1> MESS_AREA_NAME = {
    "",             // [0] unused
    "Town Cntr",    // [1]
    "Movies",       // [2]
    "Stomp",        // [3]
    "Books",        // [4]
    "Poems",        // [5]
    "Reviews",      // [6]
    "Program",      // [7]
    "Music",        // [8]
    "Sports",       // [9]
    "Debate",       // [10]
    "Humour",       // [11]
    "Notices",      // [12]
    "Fantasy",      // [13]
    "Adult",        // [14]
    "SysOp"         // [15]
};

static const std::array<std::string, MAX_FILE_AREAS + 1> FILE_AREA_NAME = {
    "",                 // [0] unused
    "Stomp",            // [1]
    "Games",            // [2]
    "Utilities",        // [3]
    "Communications",   // [4]
    "Programmers",      // [5]
    "Windows Utilities",// [6]
    "Private"           // [7]
};

static const std::array<std::string, MAX_FILE_AREAS + 1> FILE_AREA_PATH = {
    "",
    "c:\\BOSS\\FILE1\\",
    "c:\\BOSS\\FILE2\\",
    "c:\\BOSS\\FILE3\\",
    "c:\\BOSS\\FILE4\\",
    "c:\\BOSS\\FILE5\\",
    "c:\\BOSS\\FILE6\\",
    "c:\\BOSS\\FILE7\\"
};

// ---------------------------------------------------------------------------
// BossState — all global variables from the BASIC program
// ---------------------------------------------------------------------------
struct BossState {
    // --- Connection / session ---
    bool   localMode   = false;    // -local flag; skips COM port
    std::string comPort = "COM1";  // e.g. "COM1"
    std::string baud   = "2400";   // baud rate string
    int    baudVal     = 2400;
    int    bv          = 2;        // chars/tick at current baud
    int    delay_      = 2200;     // inter-char delay loop count
    HANDLE hCom        = INVALID_HANDLE_VALUE;

    // --- User info ---
    std::string name1;             // First name
    std::string name2;             // Last name
    std::string password;          // Current password
    std::string city;
    std::string phone1;            // Data phone
    std::string phone2;            // Voice phone
    std::string ansi;              // "Y" or "N"
    std::string bdate;             // Birth date mm-dd-yyyy
    std::string sl;                // Screen length string
    std::string xpt;               // Screen clearing "Y"/"N"
    std::string secur;             // Security level string
    std::string tcal;              // Total calls
    std::string tuse;              // Time used today
    std::string tpost;             // Total posts
    std::string bbdate;            // Date of last call (today)
    std::string lcall;             // Last call date
    std::string ul;                // Upload count
    std::string ulkb;              // Upload KB
    std::string dl;                // Download count
    std::string dlkb;              // Download KB
    std::string usernum;           // User number string
    std::string lread;             // Last message read
    std::string del1;              // Deleted flag
    std::string tse;               // Time elapsed this session
    std::string tyear;             // Age string

    // --- Session flags ---
    int    userRecNum  = 0;        // 1-based record number in USERS.BBS
    int    userCount   = 0;        // Total registered users
    int    mesNum      = 0;        // Total messages
    bool   know        = false;    // User is logged in (KNOW%)
    bool   find        = false;    // User found in scan
    int    inval       = 0;        // Invalid password attempts
    bool   abort_      = false;    // Abort display flag
    bool   no_         = false;    // No-clear flag
    bool   sec_        = false;    // Sub-menu return flag
    bool   can_        = false;    // Chat cancel flag

    // --- Message state ---
    std::string  send1, send2;     // Recipient first/last
    std::string  sbj;              // Subject
    std::string  pri;              // Private "Y"/"N"
    std::string  tra1;             // Trace root
    std::array<std::string, MAX_MSG_LINES + 1> tessage;  // Message lines
    int    mlen       = 0;         // Lines used in tessage
    int    lca        = 0;
    int    mess       = 0;
    bool   ret_       = false;
    bool   news2      = false;
    bool   repl       = false;
    bool   fed        = false;
    bool   ma         = false;
    int    way        = 0;         // Read direction +1/-1

    // --- Menu state ---
    std::string menuFile;          // e.g. "menu0.mnu"
    int    menuLen    = 0;
    std::array<std::string, MAX_MENU_ITEMS + 1> letter;
    std::array<std::string, MAX_MENU_ITEMS + 1> menuDest;
    std::array<std::string, MAX_MENU_ITEMS + 1> ms;      // min security
    std::array<std::string, MAX_MENU_ITEMS + 1> op;      // operation code
    int    km         = 0;         // current menu item index
    std::string ant;               // last key pressed
    std::string sa;                // auto-selected key from menu display
    std::string ps;                // valid key string

    // --- File download state ---
    std::array<std::string, MAX_FILE_ITEMS + 1> chat2;   // also used as char buffer
    std::array<std::string, MAX_FILE_ITEMS + 1> chat;
    int    chat2cnt   = 0;

    // --- Message read pointers ---
    std::array<std::string, MAX_MESS_AREAS + 1> pnt2;   // lastread per area
    std::array<std::string, MAX_REPLY + 1>  reply;       // input reply chars

    // --- Misc display ---
    std::string titl[MAX_MENU_ITEMS + 1];
    int    titlCount  = 0;
    std::string spaz;              // Login time string
    std::string fdt;               // Formatted time
    std::string message_[MAX_MSG_LINES + 1]; // editor buffer (alias for tessage)

    // --- Mail scan ---
    std::array<std::string, MAX_MARK + 1> mark;
    std::array<std::string, MAX_MARK + 1> mark2;
    std::array<std::string, MAX_MARK + 1> mark3;
    std::array<std::string, MAX_MARK + 1> mark4;
    std::array<std::string, MAX_MARK + 1> mark5;
    std::array<int,         MAX_MARK + 1> perm;

    // --- Scan state ---
    bool   qscan      = false;     // quick scan mode
    int    qscanMode  = 0;         // 1=new msgs only, 2=new file scan
    bool   nsq        = false;     // new-since-last flag
    int    how        = 0;         // lines-per-page counter
    bool   ind        = false;     // individual message mode
    int    getm       = 0;
    int    z2         = 0;
    bool   trc        = false;     // trace mode
    int    ap         = 0, ar = 0, bfr = 0;
    bool   wam        = false;
    int    mend       = 0;
    bool   ot         = false;
    int    sure       = 0;
    int    conf       = 0;
    int    re1        = 0;

    // --- Colour / screen state ---
    int    v          = 15;        // current colour
    bool   ansiOn     = false;     // ansi=Y

    // --- Timer ---
    double ini        = 0.0;       // session start (seconds since midnight)
    int    remain     = 60;        // minutes remaining
    int    qt         = 60;        // minutes left (computed)
    double zt         = 0.0;       // minutes elapsed
    double pt         = 0.0;       // last activity minutes
    int    loa        = 0;         // warning level issued
    bool   flav       = false;     // suppress timer check
    bool   mo         = false;     // shell returned
    bool   fx         = false;     // screen restore flag
    int    ju         = 0;
    bool   jp         = false;

    // --- Misc flags ---
    bool   ntf        = false;     // no-title-file flag
    int    colr       = 1;
    int    sc         = 2;         // screen page
    int    loca       = 0;         // 1=local (no modem)
    bool   chng       = false;     // in-registration change mode
    bool   pse        = false;     // partial-name search
    bool   des        = false;     // description mode
    bool   yu         = false;     // new file found flag
    bool   cl         = false;     // download confirm
    int    a1_ma      = 0;         // MA% message area override
    std::string a1r;               // AR$ for messages
    std::string mnu;               // current message area number
    int    inval2     = 0;
    bool   ok         = false;     // save OK
    bool   sec2       = false;     // SEC%

    // Sysop name constants
    static constexpr char SYSOP_FIRST[] = "Mark";
    static constexpr char SYSOP_LAST[]  = "Longo";
};
