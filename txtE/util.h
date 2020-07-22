// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once
#include <sstream>
#include <iostream>
#include <algorithm> 
#include <chaiscript/chaiscript.hpp>
#include <list>
#include <tchar.h>
#include <boost/algorithm/string/replace.hpp>

//////////////////////////////////////////////////////////////////////////
//https://habr.com/ru/post/131977/
class MakeString
{
public:
    template<class T>
    MakeString& operator<< (const T& arg)
    {
        m_stream << arg;
        return *this;
    }
    operator std::string() const
    {
        return m_stream.str();
    }
protected:
    std::stringstream m_stream;
};

#define ENDL "\n"
#define ENDL_SIZE 1

extern bool enable_trace;
extern size_t _MAX_TRACE_TEXT_SIZE;

inline std::string _text_at(const std::string & text, size_t pos)
{
    std::string _s;
    if (pos < text.size())
    {
        _s = text.substr(pos, std::min(_MAX_TRACE_TEXT_SIZE, text.size() - pos));
        boost::replace_all(_s, "\r", "");
    }
    else
    {
        _s = "EOF";
    }

    return _s;
}

class _Trace
{
    const char* func_name;
public:
    _Trace(const char* s):func_name(s)
    {
        if (enable_trace) std::cout << "---> " << func_name << std::endl;
    }
    ~_Trace()
    {
        if (enable_trace) std::cout << "<--- " << func_name << std::endl << std::endl;
    }
};

#define _ENABLE_TRACE  // comment this to disable tracing

#ifdef _ENABLE_TRACE
#define TRACE_FUNC _Trace _tTr(__FUNCTION__);
#define TRACE_POS(pos_val) if(enable_trace) { std::cout << "pos = " << pos_val << "\n" << "text = \"" << _text_at(text, pos_val) << "\"" << std::endl;}
#define TRACE_OUT if (enable_trace) { std::cout
#define TRACE_END << std::endl;}
#else
#define TRACE_FUNC 
#define TRACE_POS(pos_val) 
#define TRACE_OUT /##/
#define TRACE_END 
#endif
//////////////////////////////////////////////////////////////////////////

class ChaiEngine
{
protected:
    using module_type = std::list<ChaiEngine*>;
    static module_type *_modules;
    chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();
    static std::unique_ptr<chaiscript::ChaiScript> engine;
    static std::vector<std::string>* program_options;

    virtual void register_module() = 0;
    static void init_std();
public:
    ChaiEngine()
    {
        if (_modules == nullptr)
            _modules = new module_type();

        _modules->push_front(this);

        program_options = new std::vector<std::string>();
    }
    
    ~ChaiEngine()
    {
        if (_modules)
        {
            delete _modules;
            _modules = nullptr;
        }
        if (program_options)
        {
            delete program_options;
            program_options = nullptr;
        }
    }
        
    static std::vector<std::string> * get_program_options();
    static chaiscript::ChaiScript* get_engine();
    static void start();
    static void stop();
};

#define DECLARE_MODULE(NAME) class ChaiEngine##NAME:public ChaiEngine { protected:  virtual void register_module() {

#define END_DECLARE(NAME) }}; static ChaiEngine##NAME module##NAME;

//////////////////////////////////////////////////////////////////////////