#pragma once
// menus.h — Load .MNU files, display .ANS/.ASC menu art, read valid keystrokes
// BOSS.BAS lines: 4240–4460, 12180–12350

#include "boss.h"
#include <string>

// Load a .MNU file into st.letter[], st.menuDest[], st.ms[], st.op[]
// and set st.menuLen.
// BOSS.BAS line 4240.
void loadMenu(BossState& st, const std::string& menuFile);

// Load and display the .ANS or .ASC menu art file corresponding to
// the current menu and user's security level.
// BOSS.BBS line 4460.
void displayMenuArt(BossState& st);

// Build valid-key string ps$ from the loaded menu items the user can access.
// Returns the string of valid chars. BOSS.BAS lines 930–941.
std::string buildValidKeys(const BossState& st);

// Display the menu art, then block until user presses a valid key.
// Returns the km% index (1-based) of the chosen item.
// Also sets st.km and st.ant.
// BOSS.BAS lines 930–1080.
int showMenu(BossState& st);

// Display a raw text file (HELLO.TXT, BYE.TXT, etc.)
// Handles embedded colour codes (CHR$(1)+code), form-feed (CHR$(2)),
// press-return (CHR$(3)), and [S]top / [P]ause.
// BOSS.BAS line 11710.
void displayTextFile(BossState& st, const std::string& filename);

// [Press Return] prompt — BOSS.BAS line 13800.
void pressReturn(BossState& st);
