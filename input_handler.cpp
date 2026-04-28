#include "input_handler.h"
#include "ime_core.h"
#include "dictionary.h"

extern GlobalState g_state;

LRESULT CALLBACK InputHandler::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (pKey->vkCode >= VK_NUMPAD4 && pKey->vkCode <= VK_NUMPAD9 && pKey->vkCode != VK_NUMPAD6) {
            wchar_t s = 0;
            switch(pKey->vkCode) {
                case VK_NUMPAD7: s = L'7'; break; case VK_NUMPAD8: s = L'8'; break; 
                case VK_NUMPAD9: s = L'9'; break; case VK_NUMPAD4: s = L'4'; break; 
                case VK_NUMPAD5: s = L'5'; break;
            }
            if (s) {
                g_state.inputBuffer += s;
                Dictionary::updateCandidates(g_state);
                InvalidateRect(g_state.hWnd, NULL, TRUE);
                return 1;
            }
        }
        if (pKey->vkCode >= VK_NUMPAD1 && pKey->vkCode <= VK_NUMPAD3) {
            if (!g_state.candidates.empty()) {
                Dictionary::selectCandidate(g_state, pKey->vkCode - VK_NUMPAD1);
                InvalidateRect(g_state.hWnd, NULL, TRUE);
                return 1;
            }
        }
        if (pKey->vkCode == VK_BACK && !g_state.inputBuffer.empty()) {
            g_state.inputBuffer.pop_back();
            Dictionary::updateCandidates(g_state);
            InvalidateRect(g_state.hWnd, NULL, TRUE);
            return 1;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void InputHandler::sendTextDirectUnicode(const std::wstring& text) {
    for (wchar_t c : text) {
        INPUT in[2] = {0};
        in[0].type = INPUT_KEYBOARD; in[0].ki.wScan = c; in[0].ki.dwFlags = KEYEVENTF_UNICODE;
        in[1] = in[0]; in[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(2, in, sizeof(INPUT));
    }
}
