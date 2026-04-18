#pragma once
// messages.h — Message base: post, read, reply, delete, scan
// BOSS.BAS lines: 5250–10260, 9240–10191, 8600, 8700

#include "boss.h"
#include "fileio.h"
#include <string>

// ---------------------------------------------------------------------------
// Message-base file paths (relative to BOSS_PATH)
// ---------------------------------------------------------------------------
//   index.bbs   — 30 bytes/rec: MESSAGE$(5) HEAD$(5) SLOC$(10) LLOC$(10)
//   header.bbs  — 125 bytes/rec: MSG$(5) AR$(2) STA$(1) RECI$(1) DAT$(8)
//                                TYME$(11) TO1$(19) TO2$(19) FROM$(19)
//                                SJ$(35) TRA$(5)
//   message.bbs — 72 bytes/rec: SAVEMES$(72)

// Open index.bbs and read record 1 to get mesNum (total messages).
// Also reads the last index record to get SLOC%/LLOC%.
// BOSS.BAS line 8600.
void msgLoadIndex(BossState& st);

// Post a message. Prompts for recipient, subject, and body.
// BOSS.BAS lines 5250–8700.
void postMessage(BossState& st);

// Internal: after body is collected in st.tessage[], save to disk.
// BOSS.BAS line 8700.
void msgSave(BossState& st);

// Read messages (forward/reverse/individual/new-only).
// BOSS.BAS line 9240.
void readMessages(BossState& st);

// Quick-scan: check for unread mail addressed to current user.
// BOSS.BAS line 11050.
void checkMail(BossState& st, int startMsg);

// Delete message (mark AR$ = "D ").
// BOSS.BAS line 10710.
void deleteMessage(BossState& st, int headRec);

// Reply to a message already displayed.
// BOSS.BAS line 10870.
void replyMessage(BossState& st);

// Quick-scan all areas — show message headers for new messages.
// BOSS.BAS lines 9440+, qscan%=2.
void quickScanMessages(BossState& st);

// Inline helper: strip blanks from string (mirrors CALL STRIPBLANKS).
inline std::string msgStrip(const std::string& s) { return stripBlanks(s); }

// Open the three message files and return RecordFile objects.
RecordFile msgOpenIndex();
RecordFile msgOpenHeader();
RecordFile msgOpenMessage();
