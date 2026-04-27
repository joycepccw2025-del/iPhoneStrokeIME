// window_manager.cpp - 視窗管理與繪製實作 (OptimizedUI 支援版)
#include "window_manager.h"
#include "buffer_manager.h"
#include "dictionary.h"
#include "input_handler.h"
#include "config_loader.h"
#include "screen_manager.h"
#include "position_manager.h"
#include "ime_manager.h"
#include <algorithm>

namespace WindowManager {

// ===============================================================
// 核心函數：切換輸入模式 (IDLE / CAND_MODE / PRED_MODE)
// ===============================================================
void switchMode(GlobalState& state, InputMode newMode) {
    state.currentMode = newMode;

    switch (newMode) {
        case InputMode::IDLE:
            // 完全隱藏所有輔助視窗
            if (state.hInputWnd) ShowWindow(state.hInputWnd, SW_HIDE);
            if (state.hCandWnd) ShowWindow(state.hCandWnd, SW_HIDE);
            if (state.hPredWnd) ShowWindow(state.hPredWnd, SW_HIDE);
            state.isInputting = false;
            state.isPredictionMode = false;
            break;

        case InputMode::CAND_MODE:
            // 顯示字碼視窗與候選字視窗，隱藏聯想視窗
            state.isInputting = true;
            state.isPredictionMode = false;
            
            positionInputWindow(state);
            positionCandidateWindow(state);
            
            if (state.hInputWnd) ShowWindow(state.hInputWnd, SW_SHOWNOACTIVATE);
            if (state.hCandWnd) ShowWindow(state.hCandWnd, SW_SHOWNOACTIVATE);
            if (state.hPredWnd) ShowWindow(state.hPredWnd, SW_HIDE);
            
            // 刷新內容
            InvalidateRect(state.hInputWnd, NULL, TRUE);
            InvalidateRect(state.hCandWnd, NULL, TRUE);
            break;

        case InputMode::PRED_MODE:
            // 進入聯想模式：顯示聯想視窗，隱藏字碼與一般候選視窗
            state.isInputting = false;
            state.isPredictionMode = true;
            
            positionPredictionWindow(state);
            
            if (state.hInputWnd) ShowWindow(state.hInputWnd, SW_HIDE);
            if (state.hCandWnd) ShowWindow(state.hCandWnd, SW_HIDE);
            if (state.hPredWnd) ShowWindow(state.hPredWnd, SW_SHOWNOACTIVATE);
            
            InvalidateRect(state.hPredWnd, NULL, TRUE);
            break;
    }
}

// ===============================================================
// 繪製字碼輸入視窗 (支援 3+3 模式顯示)
// ===============================================================
void drawInputWindow(HDC hdc, RECT rc, const GlobalState& state) {
    // 背景與邊框
    HBRUSH hBg = CreateSolidBrush(state.inputBackgroundColor);
    HPEN hBorder = CreatePen(PS_SOLID, 1, state.inputBorderColor);
    SelectObject(hdc, hBg);
    SelectObject(hdc, hBorder);
    Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
    
    // 設定字體
    HFONT hFont = CreateFontW(state.inputFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, state.inputFontName.c_str());
    SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);

    // 取得要顯示的文字（呼叫 Dictionary::getInputDisplay 處理 3+3 邏輯）
    std::wstring displayText = Dictionary::getInputDisplay(state);
    
    // 繪製文字
    RECT textRc = rc;
    textRc.left += 5;
    SetTextColor(hdc, state.inputTextColor);
    DrawTextW(hdc, displayText.c_str(), -1, &textRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // 清理資源
    DeleteObject(hFont);
    DeleteObject(hBg);
    DeleteObject(hBorder);
}

// ===============================================================
// 視窗定位邏輯
// ===============================================================
void positionCandidateWindow(GlobalState& state) {
    if (!state.hCandWnd) return;
    
    // 根據主工具列的位置自動垂直排列
    int x = PositionManager::g_toolbarPos.x;
    int y = PositionManager::g_toolbarPos.y + state.windowHeight + 2; // 工具列下方
    
    SetWindowPos(state.hCandWnd, HWND_TOPMOST, x, y, 
                state.candidateWidth, calculateCandidateWindowHeight(state), 
                SWP_NOACTIVATE);
}

void positionInputWindow(GlobalState& state, const RECT* knownListRect) {
    if (!state.hInputWnd) return;
    
    // 通常放在工具列上方或候選框上方
    int x = PositionManager::g_toolbarPos.x;
    int y = PositionManager::g_toolbarPos.y - state.inputWindowHeight - 2; 

    SetWindowPos(state.hInputWnd, HWND_TOPMOST, x, y, 
                state.inputWindowWidth, state.inputWindowHeight, 
                SWP_NOACTIVATE);
}

// ===============================================================
// 半透明處理 (OptimizedUI 特色)
// ===============================================================
void applyTransparency(GlobalState& state) {
    HWND windows[] = { state.hWnd, state.hCandWnd, state.hPredWnd, state.hBufferWnd, state.hInputWnd };
    
    for (HWND hwnd : windows) {
        if (!hwnd) continue;
        
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (state.useOptimizedUI) {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
            SetLayeredWindowAttributes(hwnd, 0, state.transparencyAlpha, LWA_ALPHA);
        } else {
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        }
    }
}

} // namespace WindowManager