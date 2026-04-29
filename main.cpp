#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"

extern GlobalState g_state;
HHOOK g_hHook = NULL;

// iPhone 筆劃按鈕定義
struct StrokeBtn { RECT rect; std::wstring label; std::wstring code; };
std::vector<StrokeBtn> strokeBtns;

void InitStrokeLayout(HWND hWnd) {
    // 簡單佈局：橫排 5 個按鈕
    strokeBtns = {
        {{10, 200, 60, 250}, L"一", L"7"}, 
        {{65, 200, 115, 250}, L"丨", L"8"},
        {{120, 200, 170, 250}, L"丿", L"9"},
        {{175, 200, 225, 250}, L"丶", L"4"},
        {{230, 200, 280, 250}, L"乙", L"5"}
    };
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            InitStrokeLayout(hWnd);
            return 0;

        case WM_LBUTTONDOWN: { // 處理滑鼠點擊虛擬鍵盤
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            for (auto& btn : strokeBtns) {
                if (x >= btn.rect.left && x <= btn.rect.right && y >= btn.rect.top && y <= btn.rect.bottom) {
                    g_state.inputBuffer += btn.code;
                    Dictionary::updateCandidates(g_state);
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                }
            }
            // 處理候選字點擊 (簡化版：點選第一行)
            if (y > 40 && y < 150 && !g_state.candidates.empty()) {
                Dictionary::selectCandidate(g_state, 0);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            SetBkMode(hdc, TRANSPARENT);

            // 1. 繪製輸入碼
            std::wstring disp = L"輸入: " + g_state.inputBuffer;
            TextOutW(hdc, 10, 10, disp.c_str(), (int)disp.length());

            // 2. 繪製候選字
            int y = 40;
            for (size_t i = 0; i < g_state.candidates.size() && i < 5; ++i) {
                std::wstring cand = std::to_wstring(i+1) + L"." + g_state.candidates[i];
                TextOutW(hdc, 10, y, cand.c_str(), (int)cand.length());
                y += 30;
            }

            // 3. 繪製 iPhone 風格按鈕
            SelectObject(hdc, GetStockObject(DC_PEN));
            for (auto& btn : strokeBtns) {
                Rectangle(hdc, btn.rect.left, btn.rect.top, btn.rect.right, btn.rect.bottom);
                DrawTextW(hdc, btn.label.c_str(), -1, &btn.rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            // 4. 顯示狀態
            SetTextColor(hdc, RGB(0, 128, 0));
            TextOutW(hdc, 10, 370, g_state.statusInfo.c_str(), (int)g_state.statusInfo.length());

            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    g_state.hInstance = hInst;
    
    // 初始化路徑與字典
    wchar_t p[MAX_PATH];
    GetModuleFileNameW(NULL, p, MAX_PATH);
    std::wstring d = p;
    g_state.systemDir = d.substr(0, d.find_last_of(L"\\/") + 1);
    Dictionary::loadMainDict(g_state);

    // 註冊與建立視窗
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst, 
                       LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), NULL, L"iPhoneIME", NULL };
    RegisterClassExW(&wc);

    g_state.hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"iPhoneIME", L"Stroke IME",
                                   WS_POPUP | WS_BORDER, 100, 100, 300, 400, NULL, NULL, hInst, NULL);

    ShowWindow(g_state.hWnd, SW_SHOW);
    
    // 全域鍵盤鉤子 (處理 Numpad 78945)
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInst, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hHook);
    return 0;
}
