#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"

GlobalState g_state;
HHOOK g_hKeyboardHook = NULL;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    g_state.hInstance = hInst;
    
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring dir = path;
    g_state.systemDir = dir.substr(0, dir.find_last_of(L"\\/") + 1);

    Dictionary::loadMainDict(g_state);
    WindowManager::registerOptimizedWindowClasses(hInst);
    WindowManager::createOptimizedWindows(hInst, g_state);

    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInst, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
