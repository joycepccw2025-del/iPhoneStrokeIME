#include "buffer_manager.h"
#include "ime_core.h"
#include "input_handler.h"
#include <algorithm>
#include <fstream>
#include <ctime>

// 計算視窗高度
int BufferManager::calculateBufferWindowHeight(const GlobalState& state) {
    int minRequiredHeight = 60 + CONTROL_BAR_HEIGHT;
    
    if (state.bufferText.empty()) {
        return std::max(MIN_HEIGHT, minRequiredHeight);
    }

    int lineCount = (state.bufferText.length() + CHARS_PER_LINE - 1) / CHARS_PER_LINE;
    if (lineCount < 1) lineCount = 1;
    
    int contentHeight = lineCount * LINE_HEIGHT + 30;
    int totalHeight = contentHeight + CONTROL_BAR_HEIGHT;
    
    return std::min(totalHeight, MAX_HEIGHT);
}

// 儲存到檔案
void BufferManager::saveBufferToFile(const GlobalState& state) {
    std::string utf8_text = Utils::wstrToUtf8(state.bufferText);
    std::ofstream file("buffer_content.txt");
    if (file.is_open()) {
        file << utf8_text;
        file.close();
    }
}

// 載入檔案
void BufferManager::loadBufferFromFile(GlobalState& state) {
    std::ifstream file("buffer_content.txt");
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        state.bufferText = Utils::utf8ToWstr(content);
        state.bufferCursorPos = state.bufferText.length();
        file.close();
    }
}

// 發送內容到應用程式
void BufferManager::sendBufferContent(GlobalState& state) {
    if (!state.bufferText.empty()) {
        bool wasBufferVisible = state.bufferMode && state.hBufferWnd && IsWindowVisible(state.hBufferWnd);
        RECT bufferRect = {0};
        if (wasBufferVisible) {
            GetWindowRect(state.hBufferWnd, &bufferRect);
        }

        InputHandler::sendTextDirectUnicode(state.bufferText);
        Utils::updateStatus(state, L"已發送暫放文字：" + std::to_wstring(state.bufferText.length()) + L"字");
        
        state.bufferText.clear();
        state.bufferCursorPos = 0;

        // 更新視窗大小
        if (state.hBufferWnd) {
            int newHeight = calculateBufferWindowHeight(state);
            SetWindowPos(state.hBufferWnd, NULL, 0, 0, FIXED_WIDTH, newHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            
            if (wasBufferVisible && state.bufferMode) {
                ShowWindow(state.hBufferWnd, SW_SHOWNOACTIVATE);
                state.bufferHasFocus = true;
                SetTimer(state.hBufferWnd, 1, 500, NULL);
            }
            InvalidateRect(state.hBufferWnd, nullptr, TRUE);
        }
    }
}

// 切換暫放區模式
void BufferManager::toggleBufferMode(GlobalState& state) {
    state.bufferMode = !state.bufferMode;
    if (state.bufferMode) {
        state.bufferHasFocus = true;
        if (state.hBufferWnd) {
            int height = calculateBufferWindowHeight(state);
            SetWindowPos(state.hBufferWnd, HWND_TOPMOST, 0, 0, FIXED_WIDTH, height, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
            SetTimer(state.hBufferWnd, 1, 500, NULL);
        }
    } else {
        state.bufferHasFocus = false;
        if (state.hBufferWnd) {
            KillTimer(state.hBufferWnd, 1);
            ShowWindow(state.hBufferWnd, SW_HIDE);
        }
    }
}

// 在游標處插入文字
void BufferManager::insertTextAtCursor(GlobalState& state, const std::wstring& text) {
    if (state.bufferCursorPos < 0) state.bufferCursorPos = 0;
    if (state.bufferCursorPos > (int)state.bufferText.length()) 
        state.bufferCursorPos = state.bufferText.length();

    state.bufferText.insert(state.bufferCursorPos, text);
    state.bufferCursorPos += text.length();

    if (state.hBufferWnd) {
        int newHeight = calculateBufferWindowHeight(state);
        SetWindowPos(state.hBufferWnd, NULL, 0, 0, FIXED_WIDTH, newHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(state.hBufferWnd, nullptr, TRUE);
    }
}
