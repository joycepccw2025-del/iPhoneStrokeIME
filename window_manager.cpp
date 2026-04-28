#include "window_manager.h"
#include "ime_core.h"
#include <algorithm>

// 修正：計算視窗高度邏輯，使用全域常數
static int CalculateWindowHeight(const GlobalState& state) {
    if (state.candidates.empty() && state.inputBuffer.empty()) {
        return MIN_HEIGHT;
    }
    int lineCount = (state.candidates.size() + 4) / 5; // 每行5個候選字
    if (lineCount < 1) lineCount = 1;
    
    int contentHeight = 40 + (lineCount * LINE_HEIGHT);
    int totalHeight = contentHeight + CONTROL_BAR_HEIGHT;
    
    return std::min(std::max(totalHeight, MIN_HEIGHT), MAX_HEIGHT);
}

bool WindowManager::registerOptimizedWindowClasses(HINSTANCE hInst) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProcW; // 簡化為系統默認處理，或使用您的 WndProc
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"StrokeIME_Window";
    return RegisterClassExW(&wc);
}

bool WindowManager::createOptimizedWindows(HINSTANCE hInst, GlobalState& state) {
    state.hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"StrokeIME_Window", L"IME",
        WS_POPUP | WS_BORDER,
        100, 100, FIXED_WIDTH, MIN_HEIGHT,
        NULL, NULL, hInst, NULL
    );
    return state.hWnd != NULL;
}
