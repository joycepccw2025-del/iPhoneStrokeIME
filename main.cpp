#include "ime_core.h"
#include "window_manager.h"
#include "input_handler.h"
#include "dictionary.h"

// 宣告全域變數
extern GlobalState g_state;
HHOOK g_hHook = NULL;

// --- 視窗訊息處理函數 (負責繪製介面) ---
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // 1. 背景填充 (白色)
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

            // 2. 設定字體與顏色
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));

            // 3. 繪製目前的輸入碼 (例如: 編碼: 789)
            std::wstring inputDisp = L"編碼: " + g_state.inputBuffer;
            TextOutW(hdc, 10, 10, inputDisp.c_str(), (int)inputDisp.length());

            // 4. 繪製候選字清單
            if (g_state.candidates.empty()) {
                if (!g_state.inputBuffer.empty()) {
                    TextOutW(hdc, 10, 40, L"無匹配字詞", 5);
                }
            } else {
                int y = 40;
                for (size_t i = 0; i < g_state.candidates.size() && i < 10; ++i) {
                    std::wstring cand = std::to_wstring(i + 1) + L"." + g_state.candidates[i];
                    TextOutW(hdc, 10, y, cand.c_str(), (int)cand.length());
                    y += 25; // 每一行間隔 25 像素
                }
            }

            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1; // 減少閃爍
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// --- 程式進入點 ---
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    g_state.hInstance = hInst;

    // 1. 初始化路徑與載入字典
    wchar_t p[MAX_PATH];
    GetModuleFileNameW(NULL, p, MAX_PATH);
    std::wstring d = p;
    g_state.systemDir = d.substr(0, d.find_last_of(L"\\/") + 1);
    
    Dictionary::loadMainDict(g_state);

    // 2. 註冊視窗類別
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc; // 關鍵：指向上面定義的 WndProc
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"StrokeIME_Window";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"視窗註冊失敗！", L"錯誤", MB_ICONERROR);
        return 0;
    }

    // 3. 建立視窗 (初始位置 100, 100, 寬 300, 高 400)
    g_state.hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"StrokeIME_Window", L"Stroke IME",
        WS_POPUP | WS_BORDER,
        100, 100, 300, 400,
        NULL, NULL, hInst, NULL
    );

    if (!g_state.hWnd) {
        MessageBoxW(NULL, L"視窗建立失敗！", L"錯誤", MB_ICONERROR);
        return 0;
    }

    // 4. 顯示視窗
    ShowWindow(g_state.hWnd, SW_SHOW);
    UpdateWindow(g_state.hWnd);

    // 5. 安裝鍵盤鉤子
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, InputHandler::KeyboardHookProc, hInst, 0);

    // 6. 訊息循環
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 7. 解除鉤子
    if (g_hHook) UnhookWindowsHookEx(g_hHook);

    return (int)msg.wParam;
}
