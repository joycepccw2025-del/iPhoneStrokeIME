#include "dictionary.h"
#include "ime_core.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// 1. 載入主字典 (Zi-Ma-Biao2.txt)
void Dictionary::loadMainDict(GlobalState& state) {
    state.dict.clear();
    std::wstring dictPath = state.systemDir + L"Zi-Ma-Biao2.txt";
    
    // 關鍵修正：MinGW 的 ifstream 不接受 wstring，必須轉為 UTF-8 string
    std::ifstream file(Utils::wstrToUtf8(dictPath)); 
    
    if (!file.is_open()) {
        Utils::updateStatus(state, L"找不到字典檔 Zi-Ma-Biao2.txt");
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string word, code;
        if (ss >> word >> code) {
            state.dict.push_back({ Utils::utf8ToWstr(word), Utils::utf8ToWstr(code) });
        }
    }
    file.close();
    Utils::updateStatus(state, L"字典載入成功");
}

// 2. 更新候選字 (同步 HTML v23 容錯與核心字邏輯)
void Dictionary::updateCandidates(GlobalState& state) {
    state.candidates.clear();
    if (state.inputBuffer.empty()) return;

    // --- 同步 HTML 的 484 -> 585 容錯邏輯 ---
    std::wstring searchCode = state.inputBuffer;
    if (searchCode == L"484") {
        searchCode = L"585";
    }

    // --- 同步 HTML CORE_WORDS 優先權 ---
    std::vector<std::wstring> coreWords = { L"快", L"我", L"真", L"的", L"一", L"是", L"有", L"在", L"日", L"也", L"懂", L"忙" };

    struct Match {
        std::wstring word;
        std::wstring code;
        int coreIndex;
    };
    std::vector<Match> matches;

    for (const auto& item : state.dict) {
        bool isMatch = false;
        // HTML 邏輯：484 模式下匹配 558 或 585 開頭
        if (state.inputBuffer == L"484") {
            if (item.code.find(L"558") == 0 || item.code.find(L"585") == 0) isMatch = true;
        } else {
            if (item.code.find(searchCode) == 0) isMatch = true;
        }

        if (isMatch) {
            int cIdx = -1;
            for (int i = 0; i < (int)coreWords.size(); ++i) {
                if (coreWords[i] == item.word) {
                    cIdx = i;
                    break;
                }
            }
            matches.push_back({ item.word, item.code, cIdx });
        }
    }

    // 排序：核心字 > 短編碼 (精確匹配)
    std::sort(matches.begin(), matches.end(), [&](const Match& a, const Match& b) {
        if (a.coreIndex != -1 && b.coreIndex != -1) return a.coreIndex < b.coreIndex;
        if (a.coreIndex != -1) return true;
        if (b.coreIndex != -1) return false;
        return a.code.length() < b.code.length();
    });

    std::vector<std::wstring> seen;
    for (const auto& m : matches) {
        if (std::find(seen.begin(), seen.end(), m.word) == seen.end()) {
            state.candidates.push_back(m.word);
            seen.push_back(m.word);
            if (state.candidates.size() >= 60) break;
        }
    }
}

// 3. 選擇候選字 (模擬鍵盤輸出)
void Dictionary::selectCandidate(GlobalState& state, int index) {
    if (index < 0 || index >= (int)state.candidates.size()) return;

    std::wstring selected = state.candidates[index];
    
    for (wchar_t c : selected) {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &input, sizeof(INPUT));
        
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    state.inputBuffer.clear();
    state.candidates.clear();
}

// 4. 其他預留函數
void Dictionary::loadPhrases(GlobalState& state, const std::wstring& lastChar) {
    state.candidates.clear();
}

void Dictionary::loadUserDict(GlobalState& state) {}
void Dictionary::saveUserDict(GlobalState& state) {}
