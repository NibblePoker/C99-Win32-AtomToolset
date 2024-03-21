#include "monitor.h"

#include <stdlib.h>

#include "text.h"
#include "debug.h"

AtomMonitorData* createMonitorData() {
    AtomMonitorData *monitorData = malloc(sizeof(AtomMonitorData));

    if(monitorData != NULL) {
        memset(monitorData, 0x00, sizeof(AtomMonitorData));
    }

    return monitorData;
}

void freeMonitorData(AtomMonitorData* monitorData) {
    if(monitorData != NULL) {
        for(int i = 0; i < MONITOR_CRC_COUNT; i++) {
            if(monitorData->dataCRCs[i] != NULL) {
                free(monitorData->dataCRCs[i]);
            }
        }
        free(monitorData);
    }
}

int monitorAtoms(AtomMonitorData* monitorData,
                 void (*cb_handleAtom)(
                         int atomId,
                         LPWSTR atomName,
                         int atomNameLength,
                         EAtomMonitorChange changeType,
                         LPWSTR oldAtomName,
                         void *extraCbData),
                 void *extraCbData) {
    // Preliminary checks
    if(monitorData == NULL) {
        return 0;
    }

    int updatedAtomCount = 0;
    LPWSTR atomNameBuffer = malloc(ATOM_TABLE_STR_MAX_LENGTH * sizeof(wchar_t));

    for(int iAtom = ATOM_TABLE_STR_MIN; iAtom < ATOM_TABLE_STR_MAX; iAtom++) {
        // Clearing out the buffer & grabbing the atom's name and name length.
        memset(atomNameBuffer, 0x00, ATOM_TABLE_STR_MAX_LENGTH * sizeof(wchar_t));
        int atomNameSize = GlobalGetAtomName(iAtom, atomNameBuffer, ATOM_TABLE_STR_MAX_LENGTH);

        // Preparing the index in the struct array.
        int iAtomStruct = iAtom - ATOM_TABLE_STR_MIN;

        // Attempting to determine the change type
        EAtomMonitorChange changeType = ATOM_CHANGE_NONE;
        LPWSTR oldAtomName = NULL;

        if(atomNameSize == 0 && monitorData->dataCRCs[iAtomStruct] == NULL) {
            // The atom was and is still empty.
            continue;
        } else if(atomNameSize == 0 && monitorData->dataCRCs[iAtomStruct] != NULL) {
            changeType = ATOM_CHANGE_REMOVED;
            oldAtomName = monitorData->dataCRCs[iAtomStruct];
            monitorData->dataCRCs[iAtomStruct] = NULL;
            //trace_println("Removed atom #%d of len %d", iAtom, atomNameSize);
        } else if(atomNameSize != 0 && monitorData->dataCRCs[iAtomStruct] == NULL) {
            changeType = ATOM_CHANGE_ADDED;
            monitorData->dataCRCs[iAtomStruct] = text_copyW(atomNameBuffer);
            //trace_println("Added atom #%d of len %d", iAtom, atomNameSize);
        } else if(!text_areStringsEqualW(atomNameBuffer, monitorData->dataCRCs[iAtomStruct])) {
            changeType = ATOM_CHANGE_CHANGED;
            oldAtomName = monitorData->dataCRCs[iAtomStruct];
            monitorData->dataCRCs[iAtomStruct] = text_copyW(atomNameBuffer);
            //trace_println("Changed atom #%d of len %d", iAtom, atomNameSize);
        } else {
            // No other change.
            continue;
        }

        // Handling the change
        cb_handleAtom(iAtom, atomNameBuffer, atomNameSize, changeType, oldAtomName, extraCbData);
        if(oldAtomName != NULL) {
            free(oldAtomName);
        }
        updatedAtomCount++;
    }

    free(atomNameBuffer);
    return updatedAtomCount;
}
