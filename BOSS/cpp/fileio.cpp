// fileio.cpp — BASIC random-access file implementation

#include "fileio.h"
#include <cstring>
#include <stdexcept>
#include <algorithm>

// ---------------------------------------------------------------------------
bool rfOpen(RecordFile& rf, const std::string& path, int recLen,
            const std::vector<FieldDesc>& fields) {
    // Validate that fields sum to recLen
    int total = 0;
    for (auto& f : fields) total += f.width;
    if (total > recLen) return false;

    // Open for update; create if not present
    rf.fp = fopen(path.c_str(), "r+b");
    if (!rf.fp) {
        rf.fp = fopen(path.c_str(), "w+b");
        if (!rf.fp) return false;
    }
    rf.recLen = recLen;
    rf.fields = fields;
    rf.current.resize(fields.size(), std::string());
    rf.path   = path;
    return true;
}

// ---------------------------------------------------------------------------
void rfClose(RecordFile& rf) {
    if (rf.fp) { fclose(rf.fp); rf.fp = nullptr; }
}

// ---------------------------------------------------------------------------
bool rfGet(RecordFile& rf, int recNum) {
    if (!rf.fp || recNum < 1) return false;
    long offset = (long)(recNum - 1) * rf.recLen;
    if (fseek(rf.fp, offset, SEEK_SET) != 0) return false;

    std::vector<char> buf(rf.recLen, ' ');
    size_t got = fread(buf.data(), 1, rf.recLen, rf.fp);
    // Pad with spaces if short read (e.g. new file)
    if ((int)got < rf.recLen)
        std::fill(buf.begin() + got, buf.end(), ' ');

    // Decode fields
    int pos = 0;
    for (size_t i = 0; i < rf.fields.size(); i++) {
        int w = rf.fields[i].width;
        rf.current[i] = std::string(buf.data() + pos, w);
        pos += w;
    }
    return true;
}

// ---------------------------------------------------------------------------
bool rfPut(RecordFile& rf, int recNum) {
    if (!rf.fp || recNum < 1) return false;
    long offset = (long)(recNum - 1) * rf.recLen;

    // If file is shorter than offset, fill with spaces
    fseek(rf.fp, 0, SEEK_END);
    long fileSize = ftell(rf.fp);
    if (fileSize < offset) {
        std::vector<char> pad(offset - fileSize, ' ');
        fwrite(pad.data(), 1, pad.size(), rf.fp);
    }

    if (fseek(rf.fp, offset, SEEK_SET) != 0) return false;

    // Encode fields
    std::vector<char> buf(rf.recLen, ' ');
    int pos = 0;
    for (size_t i = 0; i < rf.fields.size(); i++) {
        int w = rf.fields[i].width;
        const std::string& v = rf.current[i];
        int copyLen = std::min((int)v.size(), w);
        memcpy(buf.data() + pos, v.c_str(), copyLen);
        // Rest is already spaces from initialisation
        pos += w;
    }
    fwrite(buf.data(), 1, rf.recLen, rf.fp);
    fflush(rf.fp);
    return true;
}

// ---------------------------------------------------------------------------
void rfLset(RecordFile& rf, const std::string& fieldName, const std::string& value) {
    for (size_t i = 0; i < rf.fields.size(); i++) {
        if (rf.fields[i].name == fieldName) {
            rfLsetIdx(rf, (int)i, value);
            return;
        }
    }
}

// ---------------------------------------------------------------------------
std::string rfGet(const RecordFile& rf, const std::string& fieldName) {
    for (size_t i = 0; i < rf.fields.size(); i++) {
        if (rf.fields[i].name == fieldName)
            return rf.current[i];
    }
    return "";
}

// ---------------------------------------------------------------------------
std::string rfGetIdx(const RecordFile& rf, int idx) {
    if (idx < 0 || idx >= (int)rf.current.size()) return "";
    return rf.current[idx];
}

// ---------------------------------------------------------------------------
void rfLsetIdx(RecordFile& rf, int idx, const std::string& value) {
    if (idx < 0 || idx >= (int)rf.fields.size()) return;
    int w = rf.fields[idx].width;
    std::string padded = value;
    if ((int)padded.size() > w) padded = padded.substr(0, w);
    else padded += std::string(w - padded.size(), ' ');
    rf.current[idx] = padded;
}

// ---------------------------------------------------------------------------
void rfFlush(RecordFile& rf) {
    if (rf.fp) fflush(rf.fp);
}

// ---------------------------------------------------------------------------
int rfCount(RecordFile& rf) {
    if (!rf.fp) return 0;
    fseek(rf.fp, 0, SEEK_END);
    long size = ftell(rf.fp);
    return (int)(size / rf.recLen);
}
