#ifndef IME_CORE_H
#define IME_CORE_H

#include <windows.h>
#include <string>
#include <vector>
#include <map>

// --- UI 佈局常數 (解決 buffer_manager.cpp 報錯) ---
// 修正 4_Build.txt 中 'FIXED_WIDTH' was not declared 等錯誤
const int FIXED_WIDTH = 400;
const int MIN_HEIGHT = 100;
const int MAX_HEIGHT = 600;
const int CONTROL_BAR_HEIGHT = 40;
const int LINE_HEIGHT = 25;
const int CHARS_PER_LINE = 20;

// --- 資料結構定義 ---
enum class InputMode { IDLE, SYMBOL_MODE, PRED_MODE };

struct UserDictEntry {
    int frequency;
};

struct DictEntry {
    std::wstring word;
    std::wstring code;
};

// --- 全域狀態結構 ---
// 整合了所有模組所需的成員變數，解決 member not found 錯誤
struct GlobalState {
    HINSTANCE hInstance;
    HWND hWnd;              // 主視窗控制代碼
    HWND hBufferWnd;        // 暫放區視窗控制代碼
    
    // 輸入與候選字
    std::wstring inputBuffer;   
    std::vector<std::wstring> candidates;
    std::vector<std::wstring> phrases;
    std::vector<DictEntry> dict;
    std::map<std::wstring, UserDictEntry> userDict;
    
    // 暫放區 (Buffer) 狀態
    std::wstring bufferText;
    int bufferCursorPos = 0;
    bool bufferMode = false;
    bool bufferHasFocus = false;
    bool useOptimizedUI = true;

    // 路徑與狀態訊息
    std::wstring systemDir;
    std::wstring userDir;
    std::wstring statusInfo; 
    
    // 分頁與設定
    int currentPage = 0;
    int totalPages = 0;
    bool enableWordPrediction = true;
};

// --- 工具函數宣告 ---
namespace Utils {
    // 解決 MinGW 路徑編碼問題
    std::string wstrToUtf8(const std::wstring& wstr);
    std::wstring utf8ToWstr(const std::string& str);
    
    // 更新介面狀態
    void updateStatus(GlobalState& state, const std::wstring& msg);
}

#endif // IME_CORE_H
