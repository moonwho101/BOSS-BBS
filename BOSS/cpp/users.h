#pragma once
// users.h — User database (USERS.BBS) scan, login, registration, save
// BOSS.BAS lines: 350–900, 2590–3390, 10350–10390

#include "boss.h"
#include "fileio.h"
#include <string>

// ---------------------------------------------------------------------------
// USERS.BBS record layout (190 bytes, 1-based records)
// Record 1: user count in NUMBER$ field (19 bytes, rest padding)
// Records 2..N+1: user data
// ---------------------------------------------------------------------------
// Field layout matches BOSS.BAS line 2880 FIELD statement:
//   N1$(19) N2$(19) p$(15) C$(19) V$(12) B$(12) AN$(1) BI$(10)
//   S$(2) SE$(5) TC$(5) LL$(5) TU$(4) TP$(5) LC$(10) U$(5) UK$(7) D$(5) DK$(7)
//   XPERT$(1) DEL$(1)
// Total = 19+19+15+19+12+12+1+10+2+5+5+5+4+5+10+5+7+5+7+1+1 = 169
// Padded to 190.

// Open the USERS.BBS record file with the correct FIELD layout.
// Path defaults to BOSS_PATH + "USERS.BBS".
RecordFile openUsersFile(const std::string& path = "");

// Field indices (0-based) in the USERS.BBS FIELD layout
enum UserField {
    UF_NAME1 = 0, UF_NAME2, UF_PAS, UF_CITY, UF_PHONE1, UF_PHONE2,
    UF_ANSI, UF_BDATE, UF_SL, UF_SECUR, UF_TCAL, UF_LREAD,
    UF_TUSE, UF_TPOST, UF_LCALL, UF_UL, UF_ULKB, UF_DL, UF_DLKB,
    UF_XPT, UF_DEL,
    UF_COUNT
};

// Scan for a user by first+last name.
// Returns true and fills st.userRecNum if found.
// Also loads raw password into st.password for verification.
bool findUser(BossState& st, const std::string& first, const std::string& last);

// Populate all st.* user fields from a record already loaded into rf.current.
void loadUserFromRecord(BossState& st, const RecordFile& rf, int recNum);

// Save current st.* user fields back to USERS.BBS at st.userRecNum.
// BOSS.BAS line 10390.
void saveUser(BossState& st);

// Register a new user: interactive questionnaire, then write new record.
// BOSS.BAS lines 440–870, 2590.
void registerNewUser(BossState& st);

// Load per-area lastread pointers from LASTREAD.BBS.
// BOSS.BAS line 10350.
void loadLastRead(BossState& st);

// Save per-area lastread pointers to LASTREAD.BBS.
// BOSS.BAS line 10390 (bottom half).
void saveLastRead(BossState& st);

// Prompt for name and password; return true on successful login.
// Handles 3-strike lockout. BOSS.BAS lines 350–440.
bool loginUser(BossState& st);

// Read first+last name from terminal into st.name1, st.name2.
// BOSS.BAS GOSUB 1220 (name input at login prompt).
void readName(BossState& st);

// Display user info screen. BOSS.BAS line 3420.
void showUserInfo(BossState& st);
