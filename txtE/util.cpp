#include "util.h"

#include <chaiscript/chaiscript.hpp>
#include <intrin.h>
#include "strtk/strtk.hpp"
#include <time.h>
#include "extras/string_methods.hpp"
//////////////////////////////////////////////////////////////////////////
ChaiEngine::module_type * ChaiEngine::_modules;
unique_ptr<chaiscript::ChaiScript> ChaiEngine::engine;
vector<string>* ChaiEngine::program_options = nullptr;
size_t _Trace::level = 0;

string _Trace::tab(string sym) { return strtk::replicate(level, sym); }

void ChaiEngine::start()
{
	if (_modules)
	{
		for (auto module : *_modules)
		{
			module->register_module();
			ChaiEngine::get_engine()->add(module->m);
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
        ChaiEngine::engine = make_unique<chaiscript::ChaiScript>();
	return ChaiEngine::engine.get();
}

vector<string>* ChaiEngine::get_program_options()
{
    return program_options;
}
//////////////////////////////////////////////////////////////////////////
size_t _TRACE_TEXT_DELTA = 8;
bool enable_trace = false;

void trace(bool t)
{
    enable_trace = t;
#ifndef _ENABLE_TRACE
    cout << "----trace is disabled----" << endl;
#endif
}

void set_trace_text_delta(size_t sz)
{
    _TRACE_TEXT_DELTA = sz;
}

string get_options_value(const string& opt)
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

    throw runtime_error("No command line option : \"" + opt + "\"");

    return "";//
}

vector<string> get_options_values(const string& opt)
{
    TRACE_FUNC;
    bool found = false;
    vector<string> values;
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
        throw runtime_error("No arguments for command line option : \"" + opt + "\"");

    return values;
}

bool is_options_key_exist(const string& opt)
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

bool member(const string & member, const vector<string>& set_of_members)
{
    TRACE_FUNC;

    for (auto p : set_of_members)
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
    TRACE_FUNC;
    if (flag)
    {
        throw runtime_error("Exit with condition.");
    }
}

void exit_if(bool flag, const string& msg)
{
    TRACE_FUNC;
    if (flag)
    {
        throw runtime_error(msg);
    }
}

class Timer
{
    clock_t start_time { 0 };
    int result { 0 };
public:
    Timer() {}

    void start()
    {
        result = 0;
        start_time = clock();
    }

    void stop()
    {
        result = (clock() - start_time);
    }

    double time()
    {
        return result / 1000.0;
    }
};

bool to_true(const chaiscript::Boxed_Value&)
{
    return true;
}
//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(THINGS)

chaiscript::bootstrap::standard_library::vector_type<std::vector<string> >("VectorString", *m);
chaiscript::bootstrap::standard_library::string_type<wstring>("wstring", *m);

m->add(chaiscript::type_conversion<int, size_t>([](const int& t_bt) { return size_t(t_bt); }));
m->add(chaiscript::fun(trace), "trace");
m->add(chaiscript::fun(set_trace_text_delta), "trace_text_delta");
m->add(chaiscript::fun([]() {__debugbreak(); }), "debugbreak");
m->add(chaiscript::fun(get_options_value), "get_options_value");
m->add(chaiscript::fun(get_options_values), "get_options_values");
m->add(chaiscript::fun(is_options_key_exist), "is_options_key_exist");
m->add(chaiscript::fun(member), "member");
m->add_global_const(chaiscript::const_var(ENDL), "endl");

m->add(chaiscript::fun(static_cast<void (*)(bool)>(&exit_if)), "exit_if");
m->add(chaiscript::fun(static_cast<void (*)(bool, const string&)>(&exit_if)), "exit_if");

m->add(chaiscript::type_conversion<vector<chaiscript::Boxed_Value>, vector<string>>(
    [](const vector<chaiscript::Boxed_Value>& vec) {
        vector < string > ret;
        for (const auto& bv : vec) ret.emplace_back(chaiscript::boxed_cast<string>(bv));
        return ret;})
);

//m->add(chaiscript::type_conversion<vector<string>, vector<chaiscript::Boxed_Value>>(
    //[](const vector<string>& vec) {
        //vector<chaiscript::Boxed_Value> ret;
        //for (const auto& bv : vec) ret.emplace_back(chaiscript::Boxed_Value(bv));
        //return ret; })
//);

m->add(chaiscript::constructor<Timer()>(), "Timer");
m->add(chaiscript::user_type<Timer>(), "Timer");
m->add(chaiscript::fun(&Timer::start), "start");
m->add(chaiscript::fun(&Timer::stop), "stop");
m->add(chaiscript::fun(&Timer::time), "time");

m->add(chaiscript::fun(to_true), "to_true");
END_DECLARE(THINGS)