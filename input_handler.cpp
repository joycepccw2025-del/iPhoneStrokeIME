// input_handler.cpp - 修正編譯錯誤並僅保留 Numpad 功能
#include "input_handler.h"
#include "ime_core.h"
#include "dictionary.h"
#include <windows.h>

// 關鍵修正：宣告在 main.cpp 中定義的全域變數
extern GlobalState g_state;

LRESULT CALLBACK InputHandler::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        
        // --- 數字鍵盤輸入 (Numpad 7,8,9,4,5) ---
        // 映射關係：7->橫(7), 8->豎(8), 9->撇(9), 4->點(4), 5->折(5)
        if (pKey->vkCode >= VK_NUMPAD4 && pKey->vkCode <= VK_NUMPAD9) {
            if (pKey->vkCode == VK_NUMPAD6) return CallNextHookEx(NULL, nCode, wParam, lParam);
            
            wchar_t stroke = 0;
            switch(pKey->vkCode) {
                case VK_NUMPAD7: stroke = L'7'; break; 
                case VK_NUMPAD8: stroke = L'8'; break; 
                case VK_NUMPAD9: stroke = L'9'; break; 
                case VK_NUMPAD4: stroke = L'4'; break; 
                case VK_NUMPAD5: stroke = L'5'; break; 
            }
            
            if (stroke) {
                g_state.inputBuffer += stroke;
                // updateCandidates 內部已包含 484 -> 585 的容錯邏輯
                Dictionary::updateCandidates(g_state);
                return 1; // 攔截訊息，防止數字輸入到應用程式
            }
        }
        
        // --- 數字鍵盤選字 (Numpad 1, 2, 3) ---
        if (pKey->vkCode >= VK_NUMPAD1 && pKey->vkCode <= VK_NUMPAD3) {
            if (!g_state.candidates.empty()) {
                int index = pKey->vkCode - VK_NUMPAD1;
                Dictionary::selectCandidate(g_state, index);
                return 1;
            }
        }

        // --- Backspace 刪除鍵 ---
        if (pKey->vkCode == VK_BACK && !g_state.inputBuffer.empty()) {
            g_state.inputBuffer.pop_back();
            Dictionary::updateCandidates(g_state);
            return 1;
        }

        // --- 空白鍵 (自動選取第一個候選字) ---
        if (pKey->vkCode == VK_SPACE && !g_state.candidates.empty()) {
            Dictionary::selectCandidate(g_state, 0);
            return 1;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
