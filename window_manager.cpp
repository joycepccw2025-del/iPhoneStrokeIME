#include "window_manager.h"
#include "ime_core.h"
#include <algorithm>

static int CalculateWindowHeight(const GlobalState& state) {
    if (state.candidates.empty() && state.inputBuffer.empty()) return MIN_HEIGHT;
    int lineCount = (int)((state.candidates.size() + 4) / 5);
    int contentHeight = 40 + (lineCount * LINE_HEIGHT);
    return std::min(std::max(contentHeight + CONTROL_BAR_HEIGHT, MIN_HEIGHT), MAX_HEIGHT);
}

bool WindowManager::registerOptimizedWindowClasses(HINSTANCE hInst) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"StrokeIME_Window";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    return RegisterClassExW(&wc) != 0;
}

bool WindowManager::createOptimizedWindows(HINSTANCE hInst, GlobalState& state) {
    state.hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"StrokeIME_Window", L"IME", WS_POPUP | WS_BORDER,
        100, 100, FIXED_WIDTH, MIN_HEIGHT, NULL, NULL, hInst, NULL);
    return state.hWnd != NULL;
}
