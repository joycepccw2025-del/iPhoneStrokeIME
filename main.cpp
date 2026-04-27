#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"
#include <windows.h>

GlobalState g_state;
HHOOK g_hKeyboardHook = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    g_state.inputBuffer = L""; // 初始化
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
