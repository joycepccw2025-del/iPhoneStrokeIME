// input_handler.cpp - 輸入處理實作
#include "input_handler.h"
#include "dictionary.h"
#include "window_manager.h"
#include "ime_manager.h"

extern GlobalState g_state;

namespace InputHandler {

// 鍵盤鉤子回呼
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
    DWORD key = kbd->vkCode;

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        // 檢查是否按下切換鍵 (例如 Shift 或 Ctrl+Space)
        if (key == VK_SHIFT) {
            // toggleInputMode(g_state);
        }

        // 如果在中文模式
        if (g_state.chineseMode) {
            bool isStrokeKey = (key == 'U' || key == 'I' || key == 'O' || key == 'J' || key == 'K');
            
            if (isStrokeKey) {
                // 禁用 Windows IME 以防干擾
                IMEManager::disableWindowsIME();
                
                // 處理筆劃輸入
                wchar_t stroke;
                if (key == 'U') stroke = L'u';
                else if (key == 'I') stroke = L'i';
                else if (key == 'O') stroke = L'o';
                else if (key == 'J') stroke = L'j';
                else stroke = L'k';

                g_state.currentInput += stroke;
                g_state.isInputting = true;
                Dictionary::updateCandidates(g_state);
                return 1; // 攔截按鍵
            }

            // 處理數字鍵選字 (1-9)
            if (g_state.isInputting && key >= '1' && key <= '9') {
                Dictionary::selectCandidate(g_state, key - '1');
                return 1;
            }

            // 處理 Backspace
            if (g_state.isInputting && key == VK_BACK) {
                if (!g_state.currentInput.empty()) {
                    g_state.currentInput.pop_back();
                    Dictionary::updateCandidates(g_state);
                }
                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void sendTextDirectUnicode(const std::wstring& text) {
    if (text.empty()) return;
    
    // 使用 SendInput API 將 Unicode 字元發送到當前焦點視窗
    for (wchar_t ch : text) {
        INPUT input[2] = {0};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wScan = ch;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE;
        
        input[1] = input[0];
        input[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        
        SendInput(2, input, sizeof(INPUT));
    }
}

} // namespace InputHandler