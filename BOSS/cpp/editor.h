#pragma once
// editor.h — Full-screen BASIC-style line editor (72 col, 80 lines)
// BOSS.BAS lines: 5420–7850

#include "boss.h"
#include <string>

// Run the interactive message editor.
// Fills st.tessage[1..st.mlen] with body lines.
// Sets st.ok = true if user chose [S]ave, leaves it false for [A]bort.
// BOSS.BAS lines 5420–5900.
void runEditor(BossState& st);

// Read a single line (up to maxChars) using BOSS-style input routing.
// Used for subject, filename prompts, etc.
// Returns the entered string. Mirrors GOSUB 6870.
std::string editorReadLine(BossState& st, int maxChars, bool password = false);

// List current message buffer to screen. BOSS.BAS line 6230.
void editorList(BossState& st);
