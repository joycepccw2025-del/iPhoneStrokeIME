// main.cpp - 中文筆劃輸入法 (純離線版本)
#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"
#include "buffer_manager.h"
#include "config_loader.h"
#include "screen_manager.h"
#include "position_manager.h"
#include "tray_manager.h"
#include "ime_manager.h"
#include <windows.h>

// 全域變數實例
GlobalState g_state;
HHOOK g_hKeyboardHook = NULL;
TrayManager::TrayIconData g_trayIcon;

// 初始化目錄結構
static void initDirectories(GlobalState& state) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = exePath;
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\\\/") + 1);

    state.systemDir = exeDir + L"system\\\\\";
    state.userDir   = exeDir + L"user\\\\\";

    CreateDirectoryW(state.systemDir.c_str(), NULL);
    CreateDirectoryW(state.userDir.c_str(), NULL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    try {
        // 1. 基礎路徑初始化
        initDirectories(g_state);

        // 2. 初始化輸入法管理員 (避免 Windows 系統輸入法干擾)
        IMEManager::initialize();

        // 3. 載入本地設定與字典
        ConfigLoader::loadAllConfigs(g_state);
        Dictionary::loadMainDict(g_state);
        Dictionary::loadUserDict(g_state);
        Dictionary::loadContextLearning(g_state);
        PositionManager::loadPositions(g_state);

        // 4. 註冊與建立 UI 視窗
        if (!WindowManager::registerOptimizedWindowClasses(hInstance)) return 0;
        if (!WindowManager::createOptimizedWindows(hInstance, g_state)) return 0;

        // 5. 設定系統托盤
        TrayManager::createTrayIcon(g_state.hWnd, &g_trayIcon);

        // 6. 安裝全域鍵盤鉤子
        g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, 
                                          InputHandler::KeyboardHookProc, 
                                          hInstance, 0);

        // 7. 初始化 UI 狀態
        WindowManager::switchMode(g_state, InputMode::IDLE);
        WindowManager::applyTransparency(g_state);
        
        // 8. 啟動背景監控計時器 (僅保留位置檢查)
        SetTimer(g_state.hWnd, 996, 500, NULL);

        Utils::updateStatus(g_state, L"筆劃輸入法 V" + Utils::utf8ToWstr(APP_VERSION) + L" (離線版) 已就緒");

        // 9. 訊息迴圈
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // 10. 程式結束前的清理與儲存
        if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
        
        // 回復 Windows 原本的 IME 狀態
        IMEManager::restoreWindowsIME();

        Dictionary::saveUserDict(g_state);
        Dictionary::saveContextLearning(g_state);
        PositionManager::savePositions(g_state);
        
        TrayManager::removeTrayIcon(&g_trayIcon);

    } catch (...) {
        MessageBoxW(NULL, L"程式發生嚴重錯誤，即將關閉。", L"錯誤", MB_ICONERROR);
    }

    return 0;
}