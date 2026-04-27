#ifndef IME_CORE_H
#define IME_CORE_H

#include <string>
#include <vector>
#include <map>
#include <windows.h>

enum class InputMode { IDLE, SYMBOL_MODE, PRED_MODE };

struct UserDictEntry {
    int frequency;
};

struct GlobalState {
    HWND hWnd;
    std::wstring inputBuffer; // 確保名稱為 inputBuffer
    std::vector<std::wstring> candidates;
    std::vector<std::wstring> phrases;
    std::map<std::wstring, UserDictEntry> userDict;
    struct DictEntry { std::wstring word; std::wstring code; };
    std::vector<DictEntry> dict;
    
    std::wstring systemDir;
    std::wstring userDir;
    int currentPage = 0;
    int totalPages = 0;
    bool enableWordPrediction = true;
};

#endif
