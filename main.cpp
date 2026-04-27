// main.cpp - 修正路徑語法錯誤並定義全域變數
#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"
#include "ime_manager.h"
#include "tray_manager.h"
#include <windows.h>
#include <string>

// 定義全域變數
GlobalState g_state;
HHOOK g_hKeyboardHook = NULL;
TrayManager::TrayIconData g_trayIcon;

// 初始化目錄結構，修正 std::wstring 拼接錯誤
static void initDirectories(GlobalState& state) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        exeDir = exeDir.substr(0, lastSlash + 1);
    }

    // 使用 L"" 確保類型正確匹配
    state.systemDir = exeDir; 
    state.userDir   = exeDir; 

    // 嘗試建立資料夾（可選）
    CreateDirectoryW((exeDir + L"system").c_str(), NULL);
    CreateDirectoryW((exeDir + L"user").c_str(), NULL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // 1. 環境初始化
    initDirectories(g_state);
    IMEManager::initialize();
    
    // 2. 載入本地字典 (Zi-Ma-Biao2.txt)
    Dictionary::loadMainDict(g_state);
    Dictionary::loadUserDict(g_state);
    
    // 3. 建立 UI 視窗
    if (!WindowManager::registerOptimizedWindowClasses(hInstance)) return 0;
    if (!WindowManager::createOptimizedWindows(hInstance, g_state)) return 0;

    // 4. 安裝鍵盤鉤子
    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInstance, 0);

    // 5. 訊息迴圈
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 6. 清理退出
    if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
    IMEManager::restoreWindowsIME();
    Dictionary::saveUserDict(g_state);

    return 0;
}
