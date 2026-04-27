#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "ime_core.h"
#include <string>
#include <vector>

class Dictionary {
public:
    // 基礎字典功能
    static void loadMainDict(GlobalState& state);
    static void updateCandidates(GlobalState& state);
    static void selectCandidate(GlobalState& state, int index);

    // 聯想詞與用戶詞庫 (修正 image_f28a7a.png 的關鍵)
    static void loadPhrases(GlobalState& state, const std::wstring& lastChar);
    static void loadUserDict(GlobalState& state);
    static void saveUserDict(GlobalState& state);
};

#endif
