#include "buffer_manager.h"
#include "ime_core.h"
#include "input_handler.h"

void BufferManager::toggleBufferMode(GlobalState& state) {
    state.bufferMode = !state.bufferMode;
    if (state.hBufferWnd) ShowWindow(state.hBufferWnd, state.bufferMode ? SW_SHOW : SW_HIDE);
}

void BufferManager::insertTextAtCursor(GlobalState& state, const std::wstring& text) {
    state.bufferText.insert(state.bufferCursorPos, text);
    state.bufferCursorPos += text.length();
}

void BufferManager::sendBufferContent(GlobalState& state) {
    InputHandler::sendTextDirectUnicode(state.bufferText);
    state.bufferText.clear();
    state.bufferCursorPos = 0;
}
