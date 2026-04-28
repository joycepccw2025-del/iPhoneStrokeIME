#ifndef IME_CORE_H
#define IME_CORE_H

#include <windows.h>
#include <string>
#include <vector>
#include <map>

// UI 佈局常數 - 解決所有編譯器找不到常數的錯誤
const int FIXED_WIDTH = 400;
const int MIN_HEIGHT = 100;
const int MAX_HEIGHT = 600;
const int CONTROL_BAR_HEIGHT = 40;
const int LINE_HEIGHT = 25;
const int CHARS_PER_LINE = 20;

struct DictEntry {
    std::wstring word;
    std::wstring code;
};

struct GlobalState {
    HINSTANCE hInstance;
    HWND hWnd;
    HWND hBufferWnd;
    
    std::wstring inputBuffer;
    std::vector<std::wstring> candidates;
    std::vector<DictEntry> dict;
    
    std::wstring bufferText;
    int bufferCursorPos = 0;
    bool bufferMode = false;
    bool bufferHasFocus = false;

    std::wstring systemDir;
    std::wstring statusInfo;
};

namespace Utils {
    std::string wstrToUtf8(const std::wstring& wstr);
    std::wstring utf8ToWstr(const std::string& str);
    void updateStatus(GlobalState& state, const std::wstring& msg);
}

#endif
