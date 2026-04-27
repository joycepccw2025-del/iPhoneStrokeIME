#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"
#include <windows.h>

GlobalState g_state; // 全域定義
HHOOK g_hKeyboardHook = NULL;

static void initDirectories(GlobalState& state) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) exeDir = exeDir.substr(0, lastSlash + 1);

    state.systemDir = exeDir; 
    state.userDir   = exeDir; 
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    initDirectories(g_state);
    Dictionary::loadMainDict(g_state);
    
    if (!WindowManager::registerOptimizedWindowClasses(hInstance)) return 0;
    if (!WindowManager::createOptimizedWindows(hInstance, g_state)) return 0;

    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInstance, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
    return 0;
}
