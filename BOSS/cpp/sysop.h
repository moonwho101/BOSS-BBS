#pragma once
// sysop.h — Sysop tools: paging, chat, user edit, shell
// BOSS.BAS reference lines: 3630–4220, 4710–4830, 11590–11680,
//                            13200–13244, 13300–13355

#include "boss.h"

// Page the sysop with audio alert (BASIC line 4710)
void pageSysop(BossState& st);

// Real-time sysop/user chat session (BASIC line 3630)
void sysopChat(BossState& st);

// Change caller's security level from local console (BASIC line 11590)
void changeSecurityLevel(BossState& st);

// Toggle screen-clearing after menus (BASIC line 13200)
void toggleScreenClear(BossState& st);

// Toggle ANSI graphics mode (BASIC line 13205)
void toggleANSI(BossState& st);

// Change caller's password (BASIC line 13210)
void changePassword(BossState& st);

// Change caller's city/province (BASIC line 13240)
void changeCity(BossState& st);

// Change caller's data phone number (BASIC line 13241)
void changeDataPhone(BossState& st);

// Change caller's voice phone number (BASIC line 13242)
void changeVoicePhone(BossState& st);

// Change caller's birth date (BASIC line 13243)
void changeBirthDate(BossState& st);

// Change caller's screen length (BASIC line 13244)
void changeScreenLength(BossState& st);

// Shell to DOS from sysop hotkey (BASIC line 13300)
void shellToDos(BossState& st);

// Scan user list (BASIC line 13350)
void scanUsers(BossState& st);
