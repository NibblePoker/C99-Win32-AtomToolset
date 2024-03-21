#pragma once

#include <stdbool.h>
#include <windows.h>

int iterateAtoms(
        bool doNumerical,
        bool doString,
        bool doEmpty,
        void (*cb_handleAtom)(int atomId, LPWSTR atomName, int atomNameLength, void *extraCbData),
        void *extraCbData);
