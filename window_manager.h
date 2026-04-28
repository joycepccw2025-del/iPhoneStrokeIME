#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H
#include "ime_core.h"

class WindowManager {
public:
    static bool registerOptimizedWindowClasses(HINSTANCE hInst);
    static bool createOptimizedWindows(HINSTANCE hInst, GlobalState& state);
};
#endif
