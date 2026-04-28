#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H
#include <windows.h>
#include <string>

class InputHandler {
public:
    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static void sendTextDirectUnicode(const std::wstring& text);
};
#endif
