#pragma once
// fileio.h — BASIC random-access file helpers (FIELD/GET/PUT/LSET)
// Replaces OPEN ... FOR RANDOM AS #n LEN=k, FIELD, GET, PUT, LSET

#include <string>
#include <vector>
#include <cstdio>

// A field descriptor: name + byte width
struct FieldDesc {
    std::string name;
    int         width;
};

// Open record: wraps FILE*, record length, field layout
struct RecordFile {
    FILE*                    fp       = nullptr;
    int                      recLen   = 0;
    std::vector<FieldDesc>   fields;
    std::vector<std::string> current; // current record buffer (one slot per field)
    std::string              path;
};

// Open a random-access file for read+write (creates if missing).
// Returns false on failure.
bool rfOpen(RecordFile& rf, const std::string& path, int recLen,
            const std::vector<FieldDesc>& fields);

// Close the file.
void rfClose(RecordFile& rf);

// Read record recNum (1-based) into rf.current.
// Returns false if recNum is out of range or read failed.
bool rfGet(RecordFile& rf, int recNum);

// Write rf.current to record recNum (1-based).
// Expands file if necessary.
bool rfPut(RecordFile& rf, int recNum);

// LSET: assign to a named field, left-justified and space-padded to width.
void rfLset(RecordFile& rf, const std::string& fieldName, const std::string& value);

// Get raw value of a named field from rf.current.
std::string rfGet(const RecordFile& rf, const std::string& fieldName);

// Get value at field index (0-based) from rf.current.
std::string rfGetIdx(const RecordFile& rf, int idx);

// Set value at field index (0-based) — LSET semantics.
void rfLsetIdx(RecordFile& rf, int idx, const std::string& value);

// Flush underlying FILE* write buffers.
void rfFlush(RecordFile& rf);

// Return the number of valid records currently in the file (excludes record 0).
int rfCount(RecordFile& rf);
