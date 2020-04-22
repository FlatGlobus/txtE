// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once
#include <string>
#include <chaiscript/chaiscript.hpp>

namespace strfun
{
    size_t regex_search(const std::string& str, const std::string& pattern, size_t pos);
    size_t find(const std::string& str, const std::string& pattern, size_t pos);
    size_t rfind(const std::string& str, const std::string& pattern, size_t pos);
    size_t find_first_not_of(const std::string& str, const std::string& pattern, size_t pos);
    size_t find_last_of(const std::string& str, const std::string& pattern, size_t pos);
    size_t find_last_not_of(const std::string& str, const std::string& pattern, size_t pos);
}
