#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <time.h>
#include <filesystem>
#include <iostream>
#include <chaiscript/chaiscript.hpp>
#include "util.h"
#include <cstdio>

namespace fs = std::filesystem;
typedef std::vector<fs::path> VectorPath;
typedef std::vector<std::string> VectorString;
//////////////////////////////////////////////////////////////////////////

__time64_t FileTimeToUnixTime(FILETIME& ft)
{
    ULARGE_INTEGER ull;

    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;

    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////