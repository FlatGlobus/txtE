// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once
#include <string>
#include <chaiscript/chaiscript.hpp>

//////////////////////////////////////////////////////////////////////////
using namespace std;
namespace strfun
{
    size_t regex_search(const string& str, const string& pattern, size_t pos);
    size_t find(const string& str, const string& pattern, size_t pos);
    size_t rfind(const string& str, const string& pattern, size_t pos);
    size_t find_first_not_of(const string& str, const string& pattern, size_t pos);
    size_t find_last_of(const string& str, const string& pattern, size_t pos);
    size_t find_last_not_of(const string& str, const string& pattern, size_t pos);
}
