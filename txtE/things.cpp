#include "util.h"
#include <intrin.h>
#include <string>
#include <vector>

extern bool enable_trace;
void trace(bool t)
{
    enable_trace = t;
}

std::string get_options_value(std::vector<std::string> & program_options, const std::string& opt)
{
    bool found = false;
    for (auto value : program_options)
    {
        if (found == true)
        {
            return value;
        }

        if (value == opt)
        {
            found = true;
        }
    }
    return "";
}

std::vector<std::string> get_options_values(std::vector<std::string>& program_options, const std::string& opt)
{
    bool found = false;
    std::vector<std::string> values;
    for (auto value : program_options)
    {
        if (found == true && value.size() > 0 && value[0] != '-')
        {
            values.push_back(value);
            continue;
        }

        if (found == true && value.size() > 0 && value[0] == '-')
        {
            return values;
        }

        if (value == opt && found == false)
        {
            found = true;
        }
    }
    return values;
}

bool is_options_key_exist(std::vector<std::string>& program_options, const std::string& opt)
{
    for (auto value : program_options)
    {
        if (value == opt)
        {
            return true;
        }
    }
    return false;
}

DECLARE_MODULE(THINGS)
m->add(chaiscript::type_conversion<int, size_t>([](const int& t_bt) { return size_t(t_bt); }));
m->add(chaiscript::fun(trace), "trace");
m->add(chaiscript::fun([](){__debugbreak();}), "debug");
m->add(chaiscript::fun(get_options_value), "get_options_value");
m->add(chaiscript::fun(get_options_values), "get_options_values");
m->add(chaiscript::fun(is_options_key_exist), "is_options_key_exist");
END_DECLARE(THINGS)