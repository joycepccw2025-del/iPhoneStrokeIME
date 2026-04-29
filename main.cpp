#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"

extern GlobalState g_state;
HHOOK g_hHook = NULL;

// iPhone 介面按鈕定義
struct StrokeBtn { RECT rect; std::wstring label; std::wstring code; COLORREF color; };
std::vector<StrokeBtn> strokeBtns;

void InitLayout(HWND hWnd) {
    // 設置 iPhone 筆劃按鈕 (區域在視窗下方)
    strokeBtns = {
        {{10, 260, 65, 320},  L"一", L"7", RGB(245, 245, 245)},
        {{70, 260, 125, 320}, L"丨", L"8", RGB(245, 245, 245)},
        {{130, 260, 185, 320}, L"丿", L"9", RGB(245, 245, 245)},
        {{190, 260, 245, 320}, L"丶", L"4", RGB(245, 245, 245)},
        {{250, 260, 305, 320}, L"乙", L"5", RGB(245, 245, 245)},
        {{10, 330, 305, 370},  L"標點 / Emoji (鍵盤按 6)", L"6", RGB(220, 220, 220)}
    };
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            InitLayout(hWnd);
            return 0;
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam); int y = HIWORD(lParam);
            // 處理 iPhone 按鈕點擊
            for (auto& btn : strokeBtns) {
                if (x >= btn.rect.left && x <= btn.rect.right && y >= btn.rect.top && y <= btn.rect.bottom) {
                    g_state.inputBuffer += btn.code;
                    Dictionary::updateCandidates(g_state);
                    InvalidateRect(hWnd, NULL, TRUE);
                    return 0;
                }
            }
            // 處理候選字點擊 (點擊對應行)
            if (y >= 45 && y <= 245 && !g_state.candidates.empty()) {
                int idx = (y - 45) / 30;
                if (idx >= 0 && idx < (int)g_state.candidates.size()) {
                    Dictionary::selectCandidate(g_state, idx);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            SetBkMode(hdc, TRANSPARENT);

            // 1. 繪製編碼
            SetTextColor(hdc, RGB(0, 50, 200));
            std::wstring inputDisp = L"編碼: " + g_state.inputBuffer;
            TextOutW(hdc, 15, 10, inputDisp.c_str(), (int)inputDisp.length());

            // 2. 繪製候選字 (iPhone 清單樣式)
            SetTextColor(hdc, RGB(0, 0, 0));
            for (size_t i = 0; i < g_state.candidates.size() && i < 6; i++) {
                std::wstring item = std::to_wstring(i + 1) + L"." + g_state.candidates[i];
                TextOutW(hdc, 15, 45 + (i * 30), item.c_str(), (int)item.length());
            }

            // 3. 繪製 iPhone 按鈕
            for (auto& btn : strokeBtns) {
                HBRUSH hBrush = CreateSolidBrush(btn.color);
                FillRect(hdc, &btn.rect, hBrush);
                FrameRect(hdc, &btn.rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
                DrawTextW(hdc, btn.label.c_str(), -1, &btn.rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                DeleteObject(hBrush);
            }

            // 4. 繪製狀態資訊 (字典載入結果)
            SetTextColor(hdc, RGB(128, 128, 128));
            TextOutW(hdc, 10, 375, g_state.statusInfo.c_str(), (int)g_state.statusInfo.length());

            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    g_state.hInstance = hInst;
    
    // 初始化字典路徑
    wchar_t p[MAX_PATH];
    GetModuleFileNameW(NULL, p, MAX_PATH);
    std::wstring d = p;
    g_state.systemDir = d.substr(0, d.find_last_of(L"\\/") + 1);
    Dictionary::loadMainDict(g_state);

    // --- 修正後的 WNDCLASSEXW 初始化 (順序必須正確) ---
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"IPhoneStrokeIME";

    if (!RegisterClassExW(&wc)) return 0;

    g_state.hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, 
                                   L"IPhoneStrokeIME", L"Stroke IME", 
                                   WS_POPUP | WS_BORDER, 100, 100, 320, 400, 
                                   NULL, NULL, hInst, NULL);
    
    ShowWindow(g_state.hWnd, SW_SHOW);
    
    // 安裝全域鍵盤鉤子 (處理 Numpad 78945)
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInst, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hHook) UnhookWindowsHookEx(g_hHook);
    return (int)msg.wParam;
}
