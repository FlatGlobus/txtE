#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <time.h>
#include <filesystem>
#include <iostream>
#include <chaiscript/chaiscript.hpp>
#include "util.h"

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
std::string GetLastErrorStr()
{
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string();

    LPTSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

void AddBackSlash(fs::path& p)
{
    if (!p.empty() && p.string().back() != fs::path::preferred_separator)
        p += fs::path::preferred_separator;
}

bool IsDots(WIN32_FIND_DATA* pFindData)
{
    bool bResult = false;
    if (pFindData != NULL && pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (pFindData->cFileName[0] == _T('.'))
        {
            if (pFindData->cFileName[1] == _T('\0') ||
                (pFindData->cFileName[1] == _T('.') &&
                    pFindData->cFileName[2] == _T('\0')))
            {
                bResult = true;
            }
        }
    }
    return bResult;
}

bool GetAllFilesFromFolder(const fs::path& source, const bool recursively, VectorPath& files, const std::vector<std::string>& maskVector)
{
    fs::path sourceFolder(source);
    AddBackSlash(sourceFolder);

    fs::path current(sourceFolder);
    current /= _T("*");

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    if (recursively)
    {
        hFind = FindFirstFile(current.string().c_str(), &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            return false;
        do
        {
            if (IsDots(&FindFileData))
                continue;

            if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (recursively)
                    GetAllFilesFromFolder(fs::path(sourceFolder) /= fs::path(FindFileData.cFileName), recursively, files, maskVector);
            }

        } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }


    for (auto mask = maskVector.begin(); mask != maskVector.end(); ++mask)
    {
        current = sourceFolder;
        AddBackSlash(current);
        current /= (*mask);
        hFind = FindFirstFile(current.string().c_str(), &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            continue;
        do
        {
            if (IsDots(&FindFileData))
                continue;

            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                files.push_back(fs::path(sourceFolder) /= fs::path(FindFileData.cFileName));
            }

        } while (FindNextFile(hFind, &FindFileData));

        FindClose(hFind);
    }
    return !files.empty();
}

VectorPath GetAllFilesFromFolderProxy(fs::path sourceFolder, bool recursively, const std::vector<chaiscript::Boxed_Value>& masks)
{
    VectorString maskVector;
    VectorPath files;

    if (fs::exists(sourceFolder) == false)
    {
        std::cout << "The path  " << sourceFolder.string() << " does not exist."<< std::endl;
        return files;
    }
        
    chaiscript::Type_Info ut = chaiscript::user_type<std::string>();
    for (auto m : masks)
    {
        if (m.is_type(ut) == false)
        {
            continue;
        }
        maskVector.push_back(chaiscript::boxed_cast<std::string>(m));
    }
    
    if (maskVector.empty())
    {
        maskVector.push_back(std::string(_T("*")));
    }
    
    GetAllFilesFromFolder(sourceFolder, recursively, files, maskVector);
    return files;
}

void PrintError(std::error_code& e)
{
    if (e)
    {
        std::cout << e.message() << std::endl;
        e.clear();
    }
}
//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(DIRFUNC)

m->add(chaiscript::user_type<fs::path>(), "path");
m->add(chaiscript::constructor<fs::path()>(), "path");
m->add(chaiscript::constructor<fs::path(fs::path&)>(), "path");
m->add(chaiscript::constructor<fs::path(const fs::path&)>(), "path");
m->add(chaiscript::constructor<fs::path(const std::string&)>(), "path");

m->add(chaiscript::fun(&fs::path::clear), "clear");
m->add(chaiscript::fun(&fs::path::make_preferred), "make_preferred");
m->add(chaiscript::fun(&fs::path::remove_filename), "remove_filename");
m->add(chaiscript::fun(&fs::path::replace_filename), "replace_filename");

m->add(chaiscript::fun(&fs::path::root_name), "root_name");
m->add(chaiscript::fun(&fs::path::root_directory), "root_directory");
m->add(chaiscript::fun(&fs::path::root_path), "root_path");
m->add(chaiscript::fun(&fs::path::relative_path), "relative_path");
m->add(chaiscript::fun(&fs::path::parent_path), "parent_path");
m->add(chaiscript::fun(&fs::path::filename), "filename");
m->add(chaiscript::fun(&fs::path::stem), "stem");
m->add(chaiscript::fun(&fs::path::extension), "extension");

m->add(chaiscript::fun(&fs::path::has_root_path), "has_root_path");
m->add(chaiscript::fun(&fs::path::has_root_name), "has_root_name");
m->add(chaiscript::fun(&fs::path::has_root_directory), "has_root_directory");
m->add(chaiscript::fun(&fs::path::has_relative_path), "has_relative_path");
m->add(chaiscript::fun(&fs::path::has_parent_path), "has_parent_path");
m->add(chaiscript::fun(&fs::path::has_filename), "has_filename");
m->add(chaiscript::fun(&fs::path::has_stem), "has_stem");
m->add(chaiscript::fun(&fs::path::has_extension), "has_extension");

m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)()>(&fs::path::replace_extension)), "replace_extension");
m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const fs::path&)>(&fs::path::replace_extension)), "replace_extension");

m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const fs::path &)>(&fs::path::operator =)), "=");
m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const std::string&)>(&fs::path::operator =)), "=");

m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const fs::path&)>(&fs::path::operator /=)), "/=");
m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const std::string&)>(&fs::path::operator /=)), "/=");

m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const fs::path&)>(&fs::path::operator +=)), "+=");
m->add(chaiscript::fun(static_cast<fs::path& (fs::path::*)(const std::string&)>(&fs::path::operator +=)), "+=");

m->add(chaiscript::type_conversion<fs::path, std::string>([](const fs::path& p) { return p.string();}));
m->add(chaiscript::type_conversion<std::string, fs::path>([](const std::string& s) { return fs::path(s); }));

m->add(chaiscript::fun(&GetAllFilesFromFolderProxy), "get_files_from_folder");

ChaiEngine::get_engine()->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<fs::path>>("VectorPath"));

m->add(chaiscript::fun(static_cast<bool (*)(const fs::path&)>(&fs::create_directory)), "create_directory");
m->add(chaiscript::fun(static_cast<fs::path (*)()>(&fs::current_path)), "current_path");
m->add(chaiscript::fun(static_cast<bool (*)(const fs::path&)>(&fs::exists)), "exists");
m->add(chaiscript::fun(static_cast<bool (*)(const fs::path&, const fs::path&)>(&fs::equivalent)), "equivalent");
m->add(chaiscript::fun(static_cast<bool (*)(const fs::path&)>(&fs::remove)), "remove");
m->add(chaiscript::fun(static_cast<std::uintmax_t (*)(const fs::path&)>(&fs::remove_all)), "remove_all");
m->add(chaiscript::fun(static_cast<void (*)(const fs::path&, const fs::path&)>(&fs::copy)), "copy");
m->add(chaiscript::fun(static_cast<fs::path(*)()>(&fs::temp_directory_path)), "temp_directory_path");

END_DECLARE(DIRFUNC)
//////////////////////////////////////////////////////////////////////////