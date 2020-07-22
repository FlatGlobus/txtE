#include "util.h"
#include "chaiscript/extras/string_methods.hpp"
#include <chaiscript/chaiscript.hpp>
#include <intrin.h>

//////////////////////////////////////////////////////////////////////////
ChaiEngine::module_type * ChaiEngine::_modules;
std::unique_ptr<chaiscript::ChaiScript> ChaiEngine::engine;
std::vector<std::string>* ChaiEngine::program_options = nullptr;

void ChaiEngine::start()
{
	init_std();
	if (_modules)
	{
		for (auto modul : *_modules)
		{
			modul->register_module();
			ChaiEngine::get_engine()->add(modul->m);
		}
	}
}

void ChaiEngine::stop()
{
	ChaiEngine::engine.release();
}

chaiscript::ChaiScript* ChaiEngine::get_engine()
{
    if (ChaiEngine::engine == nullptr)
        ChaiEngine::engine = std::make_unique<chaiscript::ChaiScript>();
	return ChaiEngine::engine.get();
}

std::vector<std::string>* ChaiEngine::get_program_options()
{
    return program_options;
}

void  ChaiEngine::init_std()
{
    auto stringmethods = chaiscript::extras::string_methods::bootstrap();
    ChaiEngine::get_engine()->add(stringmethods);
    ChaiEngine::get_engine()->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("VectorString"));
	ChaiEngine::get_engine()->add(chaiscript::bootstrap::standard_library::string_type<std::wstring>("wstring"));
}
//////////////////////////////////////////////////////////////////////////
size_t _MAX_TRACE_TEXT_SIZE = 32;
bool enable_trace = false;

void trace(bool t)
{
    enable_trace = t;
#ifndef _ENABLE_TRACE
    std::cout << "----trace is disabled----" << std::endl;
#endif
}

void set_max_trace_text_size(size_t sz)
{
    _MAX_TRACE_TEXT_SIZE = sz;
}

std::string get_options_value(const std::string& opt)
{
    TRACE_FUNC;
    bool found = false;
    for (auto value : *ChaiEngine::get_program_options())
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

    throw std::runtime_error("No command line option : \"" + opt + "\"");
    //return "";
}

std::vector<std::string> get_options_values(const std::string& opt)
{
    TRACE_FUNC;
    bool found = false;
    std::vector<std::string> values;
    for (auto value : *ChaiEngine::get_program_options())
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

    if(values.size())
        throw std::runtime_error("No arguments for command line option : \"" + opt + "\"");

    return values;
}

bool is_options_key_exist(const std::string& opt)
{
    TRACE_FUNC;
    for (auto value : *ChaiEngine::get_program_options())
    {
        if (value == opt)
        {
            return true;
        }
    }
    return false;
}

bool member(const std::string & member, const std::vector<std::string>& set_of_members)
{
    TRACE_FUNC;

    for (auto p : set_of_members._Make_iterator_offset())
    {
        
        if (member == p)
        {
            TRACE_OUT << "value \"" << member << "\" is member of the set" TRACE_END;
            return true;
        }
    }
    TRACE_OUT << "value \"" << member << "\" is not member of the set" TRACE_END;

    return false;
}
//////////////////////////////////////////////////////////////////////////
void exit_if(bool flag)
{
    if (flag)
    {
        throw std::runtime_error("Exit with condition.");
    }
}

void exit_if(bool flag, const std::string& msg)
{
    if (flag)
    {
        throw std::runtime_error(msg);
    }
}

//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(THINGS)
m->add(chaiscript::type_conversion<int, size_t>([](const int& t_bt) { return size_t(t_bt); }));
m->add(chaiscript::fun(trace), "trace");
m->add(chaiscript::fun(set_max_trace_text_size), "trace_text_size");
m->add(chaiscript::fun([]() {__debugbreak(); }), "debugbreak");
m->add(chaiscript::fun(get_options_value), "get_options_value");
m->add(chaiscript::fun(get_options_values), "get_options_values");
m->add(chaiscript::fun(is_options_key_exist), "is_options_key_exist");
m->add(chaiscript::fun(member), "member");
m->add_global_const(chaiscript::const_var(ENDL), "endl");

m->add(chaiscript::fun(static_cast<void (*)(bool)>(&exit_if)), "exit_if");
m->add(chaiscript::fun(static_cast<void (*)(bool, const std::string&)>(&exit_if)), "exit_if");

m->add(chaiscript::type_conversion<std::vector<chaiscript::Boxed_Value>, std::vector<std::string>>(
    [](const std::vector<chaiscript::Boxed_Value>& vec) {
        std::vector < std::string > ret;
        for (const auto& bv : vec) ret.emplace_back(boxed_cast<std::string>(bv));
        return ret;})
);

END_DECLARE(THINGS)