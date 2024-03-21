#include "iterator.h"

#include "./constants.h"

int iterateAtoms(bool doNumerical, bool doString, bool doEmpty,
                 void (*cb_handleAtom)(int atomId, LPWSTR atomName, int atomNameLength, void *extraCbData),
                 void *extraCbData) {
    int handledAtomCount = 0;
    LPWSTR atomNameBuffer = malloc(ATOM_TABLE_STR_MAX_LENGTH * sizeof(wchar_t));

    int iAtom = (doNumerical ? ATOM_TABLE_MIN : ATOM_TABLE_STR_MIN);
    int iAtomMax = (doString ? ATOM_TABLE_STR_MAX : ATOM_TABLE_NUM_MAX);
    for(iAtom; iAtom < iAtomMax; iAtom++) {
        // Clearing out the buffer
        memset(atomNameBuffer, 0x00, ATOM_TABLE_STR_MAX_LENGTH * sizeof(wchar_t));

        // Grabbing the atom's name and name length.
        int atomNameSize = GlobalGetAtomName(iAtom, atomNameBuffer, ATOM_TABLE_STR_MAX_LENGTH);

        // Checking if we skip an empty one.
        if(atomNameSize == 0 && !doEmpty) {
            continue;
        }

        // Handling the atom
        if(cb_handleAtom != NULL) {
            cb_handleAtom(iAtom, atomNameBuffer, atomNameSize, extraCbData);
        }
        handledAtomCount++;
    }

    free(atomNameBuffer);
    return handledAtomCount;
}
