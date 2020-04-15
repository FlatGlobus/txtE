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
    void remove_to_end(std::string& str, size_t pos);
    void remove_to_pos(std::string& str, size_t pos);
    bool is_sign_end_of_sentence(unsigned char c);
    std::string insert(std::string& str, std::string& instr, size_t pos);
    std::string erase(std::string& str, size_t pos, size_t count);
    std::string removeDuplicates(const std::string& str, const std::string& pattern);
    size_t _match(const std::string& str, const std::string& pattern, size_t pos);
}
