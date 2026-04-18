#pragma once
// files.h — File library: browse, search, download, upload
// BOSS.BAS reference lines: 13600–15140

#include "boss.h"
#include <string>

// Write DORINFO1.DEF and shell to external program (BASIC line 13600).
// Used by op$(km%)=26.
void loadFiles(BossState& st);

// Browse files.bbs listing for current file area (BASIC line 14050).
void browseFiles(BossState& st);

// Search files.bbs by wildcard filename (BASIC line 14060).
void searchFiles(BossState& st);

// Queue files for DSZ download (BASIC line 14080).
void downloadFiles(BossState& st);

// Receive uploaded file via DSZ (BASIC line 15100).
void uploadFiles(BossState& st);
