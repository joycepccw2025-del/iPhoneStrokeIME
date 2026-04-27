// dictionary.cpp - 核心字典與排序邏輯 (同步 HTML v23 功能)
#include "dictionary.h"
#include "ime_core.h"
#include "window_manager.h"
#include "buffer_manager.h"
#include "input_handler.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

namespace Dictionary {

// 載入主碼表 (Zi-Ma-Biao2.txt)
void loadMainDict(GlobalState& state) {
    state.dict.clear();
    std::wstring dictPath = state.systemDir + L"Zi-Ma-Biao2.txt";
    std::ifstream file(Utils::wstrToUtf8(dictPath));
    
    if (!file.is_open()) {
        Utils::updateStatus(state, L"錯誤：找不到 Zi-Ma-Biao2.txt");
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string word, code;
        if (ss >> word >> code) {
            state.dict.push_back({Utils::utf8ToWstr(word), Utils::utf8ToWstr(code)});
        }
    }
    file.close();
}

// 核心搜尋邏輯 (含 484 容錯)
void updateCandidates(GlobalState& state) {
    state.candidates.clear();
    state.candidateCodes.clear();
    if (state.inputBuffer.empty()) return;

    std::wstring searchCode = state.inputBuffer;

    // --- 同步 HTML v23: 484 容錯處理 ---
    // 在 HTML 版中，484 是為了方便輸入「忄」部首 (實際碼為 585)
    if (searchCode == L"484") {
        searchCode = L"585"; 
    }

    // 前綴匹配搜尋
    for (const auto& item : state.dict) {
        if (item.code.find(searchCode) == 0) {
            // 避免重複
            if (std::find(state.candidates.begin(), state.candidates.end(), item.word) == state.candidates.end()) {
                state.candidates.push_back(item.word);
            }
        }
    }

    // 智能排序
    sortCandidatesBySmartScore(state);
    
    state.totalPages = (state.candidates.size() + CANDIDATES_PER_PAGE - 1) / CANDIDATES_PER_PAGE;
    state.currentPage = 0;
}

// 智能排序：優先參考 user_dict.txt 的頻率
void sortCandidatesBySmartScore(GlobalState& state) {
    if (state.candidates.empty()) return;

    std::sort(state.candidates.begin(), state.candidates.end(), 
        [&](const std::wstring& a, const std::wstring& b) {
            int freqA = state.userDict.count(a) ? state.userDict.at(a).frequency : 0;
            int freqB = state.userDict.count(b) ? state.userDict.at(b).frequency : 0;

            if (freqA != freqB) return freqA > freqB;
            return a.length() < b.length(); // 頻率相同時，短字優先
        }
    );
}

// 聯想字功能 (從 word_phrases.txt 讀取)
void showPredictions(GlobalState& state, const std::wstring& lastWord) {
    state.candidates.clear();
    if (!state.enableWordPrediction) return;

    // 搜尋以 lastWord 開頭的詞組
    // 假設 state.phrases 已經在啟動時加載了 word_phrases.txt
    for (const auto& phrase : state.phrases) {
        if (phrase.find(lastWord) == 0 && phrase != lastWord) {
            state.candidates.push_back(phrase.substr(lastWord.length()));
        }
    }
    
    if (!state.candidates.empty()) {
        WindowManager::switchMode(state, InputMode::PRED_MODE);
    }
}

// 離線版字典更新 (不執行動作)
bool updateDictFromGitHub(GlobalState& state, bool showProgress) {
    return false; 
}

} // namespace Dictionary