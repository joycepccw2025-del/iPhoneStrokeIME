#include "input_handler.h"
#include "ime_core.h"
#include "dictionary.h"
#include <windows.h>

extern GlobalState g_state;

void InputHandler::sendTextDirectUnicode(const std::wstring& text) {
    for (wchar_t c : text) {
        INPUT input[2] = {0};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wScan = c;
        input[0].ki.dwFlags = KEYEVENTF_UNICODE;
        input[1] = input[0];
        input[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(2, input, sizeof(INPUT));
    }
}

LRESULT CALLBACK InputHandler::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        
        // Numpad 7,8,9,4,5 輸入
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
                Dictionary::updateCandidates(g_state);
                return 1;
            }
        }
        
        // Numpad 1,2,3 選字
        if (pKey->vkCode >= VK_NUMPAD1 && pKey->vkCode <= VK_NUMPAD3) {
            if (!g_state.candidates.empty()) {
                Dictionary::selectCandidate(g_state, pKey->vkCode - VK_NUMPAD1);
                return 1;
            }
        }

        if (pKey->vkCode == VK_BACK && !g_state.inputBuffer.empty()) {
            g_state.inputBuffer.pop_back();
            Dictionary::updateCandidates(g_state);
            return 1;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
