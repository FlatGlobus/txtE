#include "util.h"
#include "chaiscript/extras/string_methods.hpp"
#include <intrin.h>

//////////////////////////////////////////////////////////////////////////
ChaiEngine::module_type * ChaiEngine::_modules;
std::unique_ptr<chaiscript::ChaiScript> ChaiEngine::engine;

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

std::string get_options_value(std::vector<std::string>& program_options, const std::string& opt)
{
    TRACE_FUNC;
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
    TRACE_FUNC;
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
    TRACE_FUNC;
    for (auto value : program_options)
    {
        if (value == opt)
        {
            return true;
        }
    }
    return false;
}

bool member_of(const std::string & member, const std::vector<chaiscript::Boxed_Value>& set_of_members)
{
    TRACE_FUNC;

    chaiscript::Type_Info ut = chaiscript::user_type<std::string>();
    for (auto p : set_of_members)
    {
        if (p.is_type(ut) == false)
        {
            TRACE_OUT << "error: a set value is not a string" TRACE_END;
            continue;
        }
        
        if (member == chaiscript::boxed_cast<std::string>(p))
        {
            TRACE_OUT << "value \"" << member << "\" is member of the set" TRACE_END;
            return true;
        }
    }
    TRACE_OUT << "value \"" << member << "\" is not member of the set" TRACE_END;

    return false;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(THINGS)
m->add(chaiscript::type_conversion<int, size_t>([](const int& t_bt) { return size_t(t_bt); }));
m->add(chaiscript::fun(trace), "trace");
m->add(chaiscript::fun(set_max_trace_text_size), "trace_text_size");
m->add(chaiscript::fun([]() {__debugbreak(); }), "debugbreak");
m->add(chaiscript::fun(get_options_value), "get_options_value");
m->add(chaiscript::fun(get_options_values), "get_options_values");
m->add(chaiscript::fun(is_options_key_exist), "is_options_key_exist");
m->add(chaiscript::fun(member_of), "member_of");
m->add_global_const(chaiscript::const_var(ENDL), "endl");
END_DECLARE(THINGS)