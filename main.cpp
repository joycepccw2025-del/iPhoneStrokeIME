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
#include <string>

GlobalState g_state;
HHOOK g_hKeyboardHook = NULL;
TrayManager::TrayIconData g_trayIcon;

// 初始化目錄結構 - 修正語法錯誤版本
static void initDirectories(GlobalState& state) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        exeDir = exeDir.substr(0, lastSlash + 1);
    }

    state.systemDir = exeDir + L"system\\";
    state.userDir   = exeDir + L"user\\";

    // 確保目錄存在
    CreateDirectoryW(state.systemDir.c_str(), NULL);
    CreateDirectoryW(state.userDir.c_str(), NULL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    initDirectories(g_state);
    IMEManager::initialize();
    ConfigLoader::loadAllConfigs(g_state);
    Dictionary::loadMainDict(g_state);
    Dictionary::loadUserDict(g_state);
    PositionManager::loadPositions(g_state);

    if (!WindowManager::registerOptimizedWindowClasses(hInstance)) return 0;
    if (!WindowManager::createOptimizedWindows(hInstance, g_state)) return 0;

    TrayManager::createTrayIcon(g_state.hWnd, &g_trayIcon);

    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInstance, 0);

    WindowManager::switchMode(g_state, InputMode::IDLE);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
    IMEManager::restoreWindowsIME();
    Dictionary::saveUserDict(g_state);
    PositionManager::savePositions(g_state);
    TrayManager::removeTrayIcon(&g_trayIcon);

    return 0;
}
