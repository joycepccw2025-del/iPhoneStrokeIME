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
        if (ss >> word >> code) state.dict.push_back({ Utils::utf8ToWstr(word), Utils::utf8ToWstr(code) });
    }
}

void Dictionary::updateCandidates(GlobalState& state) {
    state.candidates.clear();
    if (state.inputBuffer.empty()) return;
    std::wstring search = (state.inputBuffer == L"484") ? L"585" : state.inputBuffer;
    for (const auto& item : state.dict) {
        if (item.code.find(search) == 0) {
            state.candidates.push_back(item.word);
            if (state.candidates.size() >= 50) break;
        }
    }
}

void Dictionary::selectCandidate(GlobalState& state, int index) {
    if (index < 0 || index >= (int)state.candidates.size()) return;
    std::wstring word = state.candidates[index];
    for (wchar_t c : word) {
        INPUT in[2] = {0};
        in[0].type = INPUT_KEYBOARD; in[0].ki.wScan = c; in[0].ki.dwFlags = KEYEVENTF_UNICODE;
        in[1] = in[0]; in[1].ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(2, in, sizeof(INPUT));
    }
    state.inputBuffer.clear();
    state.candidates.clear();
}
void Dictionary::loadPhrases(GlobalState& s, const std::wstring& l) {}
void Dictionary::loadUserDict(GlobalState& s) {}
void Dictionary::saveUserDict(GlobalState& s) {}
