// ime_core.h - 核心定義與全域狀態 (OptimizedUI支援版)
#ifndef IME_CORE_H
#define IME_CORE_H

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <ctime>

// 輸入模式枚舉
enum class InputMode {
    IDLE,       // 無輸入
    CAND_MODE,  // 字碼候選模式
    PRED_MODE   // 聯想字模式
};

#define APP_VERSION "3.1.0"

// 常數定義
const int CANDIDATES_PER_PAGE = 9;
const int MAX_INPUT_LEN = 30;

struct GlobalState {
    // 視窗句柄
    HWND hWnd = NULL;          // 工具列視窗
    HWND hCandWnd = NULL;      // 候選字視窗
    HWND hPredWnd = NULL;      // 聯想字視窗
    HWND hInputWnd = NULL;     // 字碼輸入視窗
    HWND hBufferWnd = NULL;    // 暫放視窗

    // 狀態變數
    bool chineseMode = true;
    bool isInputting = false;
    bool isPredictionMode = false;
    bool bufferMode = false;
    std::wstring currentInput;
    
    // 數據存儲
    std::vector<std::wstring> candidates;
    std::vector<std::wstring> candidateCodes;
    int selected = 0;
    int currentPage = 0;
    int totalPages = 0;

    // 路徑與配置
    std::wstring systemDir;
    std::wstring userDir;
    std::wstring statusInfo;
    
    // UI 設定 (OptimizedUI)
    bool useOptimizedUI = true;
    bool enableTransparency = true;
    int transparencyAlpha = 200;
    
    // 聯想引擎
    bool enableWordPrediction = true;
    std::map<std::wstring, std::vector<std::pair<std::wstring, int>>> contextMap;
    
    // 字型與顏色 (僅列出部分核心)
    int fontSize = 16;
    std::wstring fontName = L"Microsoft JhengHei";
    COLORREF inputTextColor = RGB(0,0,0);
    COLORREF inputBackgroundColor = RGB(255,255,255);
};

namespace Utils {
    std::wstring utf8ToWstr(const std::string& str);
    std::string wstrToUtf8(const std::wstring& ws);
    void updateStatus(GlobalState& state, const std::wstring& msg);
    bool isPunctuation(const std::wstring& word);
}

#endif