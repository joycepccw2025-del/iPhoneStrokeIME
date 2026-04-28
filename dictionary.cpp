#include "dictionary.h"
#include "ime_core.h"
#include <fstream>
#include <sstream>
#include <algorithm>

void Dictionary::loadMainDict(GlobalState& state) {
    state.dict.clear();
    std::wstring path = state.systemDir + L"Zi-Ma-Biao2.txt";
    std::ifstream file(Utils::wstrToUtf8(path));
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string word, code;
        if (ss >> word >> code) {
            state.dict.push_back({Utils::utf8ToWstr(word), Utils::utf8ToWstr(code)});
        }
    }
}

void Dictionary::updateCandidates(GlobalState& state) {
    state.candidates.clear();
    if (state.inputBuffer.empty()) return;

    std::wstring searchCode = state.inputBuffer;
    if (searchCode == L"484") searchCode = L"585"; // 容錯

    for (const auto& item : state.dict) {
        if (item.code.find(searchCode) == 0) {
            state.candidates.push_back(item.word);
            if (state.candidates.size() > 50) break;
        }
    }
}

void Dictionary::selectCandidate(GlobalState& state, int index) {
    if (index >= 0 && index < (int)state.candidates.size()) {
        std::wstring word = state.candidates[index];
        for (wchar_t c : word) {
            INPUT input[2] = {0};
            input[0].type = INPUT_KEYBOARD; input[0].ki.wScan = c; input[0].ki.dwFlags = KEYEVENTF_UNICODE;
            input[1] = input[0]; input[1].ki.dwFlags |= KEYEVENTF_KEYUP;
            SendInput(2, input, sizeof(INPUT));
        }
        state.inputBuffer.clear();
        state.candidates.clear();
    }
}

void Dictionary::loadPhrases(GlobalState& state, const std::wstring& last) {}
void Dictionary::loadUserDict(GlobalState& state) {}
void Dictionary::saveUserDict(GlobalState& state) {}
