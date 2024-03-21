#include "cli.h"

#include <windows.h>

#include "debug.h"

#include "arguments/arguments.h"
#include "arguments/help.h"

#include "../core/constants.h"
#include "../core/iterator.h"
#include "../core/monitor.h"
#include "hash/crc32/crc32.h"
#include "../core/utils.h"

// Root verb
static Verb *rootVerb;
static Option *optionVersion;
static Option *optionVersionOnly;

// Listing verb & options
static Verb *listVerb;
static Option *optionListCountOnly;
//static Option *optionListEmptySlots;
//static Option *optionListNumericSlots;

// Monitoring verb & options
static Verb *monitorVerb;
static Option *optionMonitorScanDelay;
static Option *optionMonitorTimeout;
static Option *optionMonitorIgnoreExisting;
static Option *optionMonitorAdditionPrefix;
static Option *optionMonitorDeletionPrefix;
static Option *optionMonitorChangePrefix;
static Option *optionMonitorSplitChange;
//static Option *optionMonitorTimestamp; // TODO: Add a ms & s variant
// TODO: Add possibility to print removals to stderr

// Common formatting options (Shared between 'list' & 'monitor')
//static Option *optionFormattingHexNumber;
static Option *optionFormattingSeparator;
static Option *optionFormattingHeaders;
//static Option *optionFormattingCommaSeparator;
//static Option *optionFormattingTabSeparator;

// Recursively shared options
static Option *helpOption;

// Placeholder verb
// This verb is only used for the help text since a check for its usage is done in
//  the program's main function without the argument parser.
static Verb *guiVerb;

// Prototypes
void cb_printAtom(int atomId, LPWSTR atomName, int atomNameLength, void *extraCbData);
void cb_monitorAtom(int atomId, LPWSTR atomName, int atomNameLength, EAtomMonitorChange changeType,
                    LPWSTR oldAtomName, void *extraCbData);
// Globals
// ???

bool prepareLaunchArguments() {
    // Regular verbs
    rootVerb = args_createVerb(L"root", L"Root verb");
    optionVersion = args_createOption(
            'v', L"version",
            L"Shows the program's version information",
            FLAG_OPTION_STOPS_PARSING);
    optionVersionOnly = args_createOption(
            'V', L"version-only",
            L"Shows the program's version number",
            FLAG_OPTION_STOPS_PARSING);

    // Listing verb & options
    listVerb = args_createVerb(L"list", L"List global atom table's entries");
    optionListCountOnly = args_createOption(
            'c', L"count",
            L"Shows the total amount of entries instead of iterating",
            FLAG_OPTION_NONE);

    // Monitoring verb & options
    monitorVerb = args_createVerb(L"monitor", L"Monitor the global atom table's entries");
    optionMonitorScanDelay = args_createOption(
            'd', L"delay",
            L"Delay in milliseconds between each scan (Default: 500, Min: 50, Max: 60000)",
            FLAG_OPTION_HAS_VALUE);
    optionMonitorTimeout = args_createOption(
            't', L"timeout",
            L"Time in milliseconds before the scan stops and the program exits (Default: -1)",
            FLAG_OPTION_HAS_VALUE);
    optionMonitorIgnoreExisting = args_createOption(
            'i', L"ignore-existing",
            L"Prevents existing table entries from being printed out when monitoring starts",
            FLAG_OPTION_NONE);

    optionMonitorAdditionPrefix = args_createOption(
            'A', L"addition-prefix",
            L"Prefix used for lines representing an addition being detected by the monitor (Default: '+')",
            FLAG_OPTION_HAS_VALUE);
    optionMonitorDeletionPrefix = args_createOption(
            'D', L"deletion-prefix",
            L"Prefix used for lines representing a deletion being detected by the monitor (Default: '-')",
            FLAG_OPTION_HAS_VALUE);
    optionMonitorChangePrefix = args_createOption(
            'M', L"modification-prefix",
            L"Prefix used for lines representing a modification being detected by the monitor (Default: '*')",
            FLAG_OPTION_HAS_VALUE);

    optionMonitorSplitChange = args_createOption(
            'S', L"split-modifications",
            L"Treat modifications as a distinct deletion and addition",
            FLAG_OPTION_NONE);

    // Common formatting options (Shared between 'list' & 'monitor')
    optionFormattingSeparator = args_createOption(
            's', L"separator",
            L"Specifies the value separator to use when listing and monitoring (Default: ';')",
            FLAG_OPTION_HAS_VALUE);
    optionFormattingHeaders = args_createOption(
            'H', L"headers",
            L"Print CSV headers when listing and monitoring",
            FLAG_OPTION_NONE);

    // Recursively shared options
    helpOption = args_createOption(
            'h', L"help", L"Shows this help text and exit.", FLAG_OPTION_STOPS_PARSING);

    // Placeholder verb
    guiVerb = args_createVerb(L"gui", L"Open the application's GUI and shows logs in the console");

    return rootVerb != NULL &&
           args_registerOption(optionVersion, rootVerb) &&
           args_registerOption(optionVersionOnly, rootVerb) &&

           listVerb != NULL &&
           args_registerVerb(listVerb, rootVerb) &&
           args_registerOption(optionListCountOnly, listVerb) &&

           monitorVerb != NULL &&
           args_registerVerb(monitorVerb, rootVerb) &&
           args_registerOption(optionMonitorScanDelay, monitorVerb) &&
           args_registerOption(optionMonitorTimeout, monitorVerb) &&
           args_registerOption(optionMonitorIgnoreExisting, monitorVerb) &&
           args_registerOption(optionMonitorAdditionPrefix, monitorVerb) &&
           args_registerOption(optionMonitorDeletionPrefix, monitorVerb) &&
           args_registerOption(optionMonitorChangePrefix, monitorVerb) &&
           args_registerOption(optionMonitorSplitChange, monitorVerb) &&

           args_registerOption(optionFormattingSeparator, listVerb) &&
           args_registerOption(optionFormattingSeparator, monitorVerb) &&

           args_registerOption(optionFormattingHeaders, listVerb) &&
           args_registerOption(optionFormattingHeaders, monitorVerb) &&

           helpOption != NULL && guiVerb != NULL &&
           args_registerOptionRecursively(helpOption, rootVerb) &&
           args_registerVerb(guiVerb, rootVerb);
}

int cliMain(int argc, wchar_t * wargv[]) {
    int cliReturnCode = 0;
    trace_println("Start of cliMain(...) !");

    // Preparing launch arguments
    if(!prepareLaunchArguments()) {
        fprintf(stderr, "Failed to initialize the launch arguments !");
        error_println("Failed to initialize the launch arguments !");
        cliReturnCode = 10;
        goto END_CLI;
    }

    // Parsing launch arguments
    Verb *lastVerb = rootVerb;
    enum EArgumentParserErrors argError = args_parseArguments(
            rootVerb, wargv, 1, argc, &lastVerb);
    if(argError != ERROR_ARGUMENTS_NONE) {
        fprintf(stderr, "Failed to parse launch arguments ! (Internal error: %d)", argError);
        error_println("Failed to parse launch arguments ! (Internal error: %d)", argError);
        cliReturnCode = 11;
        goto END_CLEAN_ROOT_VERB;
    }

    // This shouldn't be happening since there is a check for it in "wWinMain" !
    if(guiVerb->wasUsed) {
        error_println("The 'gui' verb was used in the CLI context !");
        cliReturnCode = 12;
        goto END_CLEAN_ROOT_VERB;
    }

    // Processing options that can stop execution early.
    if(args_wasOptionUsed(helpOption)) {
        debug_println("Printing help text");
        args_printHelpText(lastVerb, L"AtomUtil.exe", 80);
        goto END_CLEAN_ROOT_VERB;
    }
    if(args_wasOptionUsed(optionVersion)) {
        debug_println("Printing version info");
        goto END_CLEAN_ROOT_VERB;
    }
    if(args_wasOptionUsed(optionVersionOnly)) {
        debug_println("Printing version number only");
        goto END_CLEAN_ROOT_VERB;
    }

    // Preparing default value for shared arguments
    if(!args_wasOptionUsed(optionFormattingSeparator)) {
        args_addValueToOption(optionFormattingSeparator, L";");
    }

    if(lastVerb == listVerb) {
        // Printing the CSV header
        if (args_wasOptionUsed(optionFormattingHeaders)) {
            wchar_t *dataSeparator = optionFormattingSeparator->arguments->first->data;
            fprintf(stdout, "AtomId%wsType%wsName\n", dataSeparator, dataSeparator);
        }

        if (args_wasOptionUsed(optionListCountOnly)) {
            printf("%d\n", iterateAtoms(false, true, false, NULL, NULL));
        } else {
            iterateAtoms(false, true, false, &cb_printAtom, NULL);
        }
    } else if(lastVerb == monitorVerb) {
        // Printing the CSV header
        if (args_wasOptionUsed(optionFormattingHeaders)) {
            wchar_t *dataSeparator = optionFormattingSeparator->arguments->first->data;
            fprintf(stdout, "Action%wsAtomId%wsType%wsNewName%wsOldName\n",
                    dataSeparator, dataSeparator, dataSeparator, dataSeparator);
        }

        // Preparing default arguments values where needed
        if(!args_wasOptionUsed(optionMonitorAdditionPrefix)) {
            args_addValueToOption(optionMonitorAdditionPrefix, L"+");
        }
        if(!args_wasOptionUsed(optionMonitorDeletionPrefix)) {
            args_addValueToOption(optionMonitorDeletionPrefix, L"-");
        }
        if(!args_wasOptionUsed(optionMonitorChangePrefix)) {
            args_addValueToOption(optionMonitorChangePrefix, L"*");
        }
        if(!args_wasOptionUsed(optionMonitorScanDelay)) {
            args_addValueToOption(optionMonitorScanDelay, L"500");
        }

        // Reading and parsing the scan delay
        int scanDelay = _wtoi(optionMonitorScanDelay->arguments->first->data);
        if(scanDelay == 0) {
            fprintf(stderr, "Failed to parse the given delay '%ws' as an integer !",
                    (wchar_t *) optionMonitorScanDelay->arguments->first->data);
            cliReturnCode = 13;
            goto END_CLEAN_MONITOR_DATA;
        }
        scanDelay = min(max(50, scanDelay), 60000);
        trace_println("Delay used: %d", scanDelay);

        // Preparing monitor
        AtomMonitorData *monitorData = createMonitorData();
        if(monitorData == NULL) {
            fprintf(stderr, "Failed to prepare the monitor !");
            cliReturnCode = 13;
            goto END_CLEAN_ROOT_VERB;
        }

        // If we should ignore the existing entries, we do a run without the callback.
        // This will populate the `monitorData` in the same way as done below.
        if(args_wasOptionUsed(optionMonitorIgnoreExisting)) {
            monitorAtoms(monitorData, NULL, NULL);
        }

        // Running the main loop
        uint64_t lastScanTime = 0;
        uint64_t firstScanTime = GetUnixMilliUTC();

        while(true) {
            // Checking if we need run the loop
            uint64_t currentTime = GetUnixMilliUTC();
            if(currentTime - lastScanTime < scanDelay) {
                //debug_println("Skipping scan ! '%llu' & '%llu'", currentTime, lastScanTime);
                Sleep(50);
                continue;
            }

            //debug_println("Running monitor scan !");
            monitorAtoms(monitorData, &cb_monitorAtom, NULL);

            lastScanTime = currentTime;
        }

        END_CLEAN_MONITOR_DATA:
        freeMonitorData(monitorData);
    } else {
        error_println("No action was implemented, and none were taken !");
    }

    END_CLEAN_ROOT_VERB:
    trace_println("Cleaning verbs and options...");
    args_freeVerb(rootVerb);

    END_CLI:
    return cliReturnCode;
}

void cb_printAtom(int atomId, LPWSTR atomName, int atomNameLength, void *extraCbData) {
    fprintf(stdout, "%d%ws%ws\n",
            atomId,
            (wchar_t *) optionFormattingSeparator->arguments->first->data,
            atomName);
}

void cb_monitorAtom(int atomId, LPWSTR atomName, int atomNameLength, EAtomMonitorChange changeType, LPWSTR oldAtomName,
                    void *extraCbData) {
    wchar_t *separator = optionFormattingSeparator->arguments->first->data;

    switch(changeType) {
        case ATOM_CHANGE_CHANGED:
            if(args_wasOptionUsed(optionMonitorSplitChange)) {
                cb_monitorAtom(atomId, atomName, atomNameLength, ATOM_CHANGE_REMOVED, oldAtomName, extraCbData);
                cb_monitorAtom(atomId, atomName, atomNameLength, ATOM_CHANGE_ADDED, oldAtomName, extraCbData);
            } else {
                fprintf(stdout, "%ws%ws%d%ws%ws%ws%ws\n",
                        (wchar_t *) optionMonitorChangePrefix->arguments->first->data,
                        separator, atomId, separator, atomName, separator, oldAtomName);
            }
            break;
        case ATOM_CHANGE_REMOVED:
            fprintf(stdout, "%ws%ws%d%ws%ws%ws\n",
                    (wchar_t *) optionMonitorDeletionPrefix->arguments->first->data,
                    separator, atomId, separator, oldAtomName, separator);
            break;
        case ATOM_CHANGE_ADDED:
            fprintf(stdout, "%ws%ws%d%ws%ws%ws\n",
                    (wchar_t *) optionMonitorAdditionPrefix->arguments->first->data,
                    separator, atomId, separator, atomName, separator);
            break;
        case ATOM_CHANGE_NONE:
            break;
    }
}
