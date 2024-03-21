#pragma once

#include <stdbool.h>
#include <wchar.h>
#include <windows.h>

#include "./constants.h"

#define MONITOR_CRC_COUNT (ATOM_TABLE_STR_MAX - ATOM_TABLE_STR_MIN)

typedef struct atomMonitorData {
    wchar_t *dataCRCs[MONITOR_CRC_COUNT];
} AtomMonitorData;

typedef enum enum_atomMonitorChange {
    ATOM_CHANGE_NONE = 0,
    ATOM_CHANGE_ADDED = 1,
    ATOM_CHANGE_REMOVED = 2,
    ATOM_CHANGE_CHANGED = 3,
} EAtomMonitorChange;

AtomMonitorData* createMonitorData();

void freeMonitorData(AtomMonitorData* monitorData);

/**
 * Monitors all non-numerical atoms in the global table and reports any changes to a given callback.
 * @param monitorData
 * @param cb_handleAtom
 * @param extraCbData
 * @return The amount of changes detected. (Does not count modifications as 2 if "-s" is used !)
 */
int monitorAtoms(
        AtomMonitorData* monitorData,
        void (*cb_handleAtom)(
                int atomId,
                LPWSTR atomName,
                int atomNameLength,
                EAtomMonitorChange changeType,
                LPWSTR oldAtomName,
                void *extraCbData),
        void *extraCbData);
