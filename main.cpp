#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"

GlobalState g_state;
HHOOK g_hHook = NULL;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    g_state.hInstance = hInst;
    wchar_t p[MAX_PATH];
    GetModuleFileNameW(NULL, p, MAX_PATH);
    std::wstring d = p;
    g_state.systemDir = d.substr(0, d.find_last_of(L"\\/") + 1);

    Dictionary::loadMainDict(g_state);
    WindowManager::registerOptimizedWindowClasses(hInst);
    WindowManager::createOptimizedWindows(hInst, g_state);

    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInst, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
