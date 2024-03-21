#include "utils.h"

#include <windows.h>

const uint64_t UNIX_TIME_START = 0x019DB1DED53E8000; //January 1, 1970 (start of Unix epoch) in "ticks"
const uint64_t TICKS_PER_SECOND = 10000000;
const uint64_t TICKS_PER_MILLISECOND = 10000;

// Source: https://stackoverflow.com/a/46024468 (Public Domain)
/*uint64_t GetSystemTimeAsUnixTime() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft); //returns ticks in UTC

    // Copying fields from `ft` to a `uint64_t` to prevent alignment faults.
    uint64_t li = ft.dwHighDateTime;
    li = li << 32;
    li = li | ft.dwLowDateTime;

    // Convert ticks since 1/1/1970 into seconds
    return (li - UNIX_TIME_START) / TICKS_PER_SECOND;
}*/

uint64_t GetUnixMilliUTC() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    // Copying fields from `ft` to a `uint64_t` to prevent alignment faults.
    uint64_t li = ft.dwHighDateTime;
    li = li << 32;
    li = li | ft.dwLowDateTime;

    // Convert ticks since 1/1/1970 into milliseconds
    return (li - UNIX_TIME_START) / TICKS_PER_MILLISECOND;
}
