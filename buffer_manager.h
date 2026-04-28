#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H
#include "ime_core.h"

class BufferManager {
public:
    static void toggleBufferMode(GlobalState& state);
    static void insertTextAtCursor(GlobalState& state, const std::wstring& text);
    static void sendBufferContent(GlobalState& state);
};
#endif
