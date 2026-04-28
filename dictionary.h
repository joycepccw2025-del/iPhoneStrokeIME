#ifndef DICTIONARY_H
#define DICTIONARY_H
#include "ime_core.h"

class Dictionary {
public:
    static void loadMainDict(GlobalState& state);
    static void updateCandidates(GlobalState& state);
    static void selectCandidate(GlobalState& state, int index);
    static void loadPhrases(GlobalState& state, const std::wstring& last);
    static void loadUserDict(GlobalState& state);
    static void saveUserDict(GlobalState& state);
};
#endif
