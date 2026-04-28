#include "ime_core.h"

namespace Utils {
    void updateStatus(GlobalState& state, const std::wstring& msg) {
        state.statusInfo = msg;
        if (state.hWnd) InvalidateRect(state.hWnd, NULL, TRUE);
    }

    std::wstring utf8ToWstr(const std::string& str) {
        if (str.empty()) return L"";
        int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstr(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
        return wstr;
    }

    std::string wstrToUtf8(const std::wstring& wstr) {
        if (wstr.empty()) return "";
        int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string str(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, NULL, NULL);
        return str;
    }
}
