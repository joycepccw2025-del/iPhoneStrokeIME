// input_handler.cpp - 僅保留小鍵盤映射
#include "input_handler.h"
#include "ime_core.h"
#include "dictionary.h"
#include <windows.h>

LRESULT CALLBACK InputHandler::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        
        // 僅定義小鍵盤映射
        // Numpad 7,8,9 -> 橫,豎,撇 (7,8,9)
        // Numpad 4,5 -> 點,折 (4,5)
        
        if (pKey->vkCode >= VK_NUMPAD4 && pKey->vkCode <= VK_NUMPAD9) {
            if (pKey->vkCode == VK_NUMPAD6) return CallNextHookEx(NULL, nCode, wParam, lParam);
            
            wchar_t stroke;
            switch(pKey->vkCode) {
                case VK_NUMPAD7: stroke = L'7'; break; // 一
                case VK_NUMPAD8: stroke = L'8'; break; // 丨
                case VK_NUMPAD9: stroke = L'9'; break; // 丿
                case VK_NUMPAD4: stroke = L'4'; break; // 丶 (同步 HTML 5)
                case VK_NUMPAD5: stroke = L'5'; break; // 乙 (同步 HTML 4)
                default: stroke = 0;
            }
            
            if (stroke) {
                g_state.inputBuffer += stroke;
                Dictionary::updateCandidates(g_state);
                return 1; // 攔截訊息
            }
        }
        
        // Numpad 1,2,3 選字
        if (pKey->vkCode >= VK_NUMPAD1 && pKey->vkCode <= VK_NUMPAD3) {
            int index = pKey->vkCode - VK_NUMPAD1;
            Dictionary::selectCandidate(g_state, index);
            return 1;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
