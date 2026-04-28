#ifndef IME_CORE_H
#define IME_CORE_H

#include <windows.h>
#include <string>
#include <vector>
#include <map>

// UI 佈局常數 - 解決 buffer_manager.cpp 報錯
const int FIXED_WIDTH = 400;
const int MIN_HEIGHT = 100;
const int MAX_HEIGHT = 600;
const int CONTROL_BAR_HEIGHT = 40;
const int LINE_HEIGHT = 25;
const int CHARS_PER_LINE = 20;

enum class InputMode { IDLE, SYMBOL_MODE, PRED_MODE };

struct DictEntry {
    std::wstring word;
    std::wstring code;
};

struct UserDictEntry {
    int frequency;
};

struct GlobalState {
    HINSTANCE hInstance;
    HWND hWnd;              // 主視窗
    HWND hBufferWnd;        // 暫放區視窗
    
    std::wstring inputBuffer;   
    std::vector<std::wstring> candidates;
    std::vector<std::wstring> phrases;
    std::vector<DictEntry> dict;
    std::map<std::wstring, UserDictEntry> userDict;
    
    // 暫放區狀態
    std::wstring bufferText;
    int bufferCursorPos = 0;
    bool bufferMode = false;
    bool bufferHasFocus = false;
    bool useOptimizedUI = true;

    std::wstring systemDir;
    std::wstring userDir;
    std::wstring statusInfo; 
    
    int currentPage = 0;
    int totalPages = 0;
    bool enableWordPrediction = true;
};

namespace Utils {
    std::string wstrToUtf8(const std::wstring& wstr);
    std::wstring utf8ToWstr(const std::string& str);
    void updateStatus(GlobalState& state, const std::wstring& msg);
}

#endif
