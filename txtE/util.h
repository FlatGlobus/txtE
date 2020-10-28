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

using namespace std;
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
    operator string() const
    {
        return m_stream.str();
    }
protected:
    stringstream m_stream;
};

#define ENDL "\n"
#define CHAR_SIZE sizeof(std::string::value_type)

extern bool enable_trace;
extern size_t _TRACE_TEXT_DELTA;

inline string _text_at(const string & text, size_t pos)
{
    string s;
    if (pos < text.size())
    {
        size_t left = pos > _TRACE_TEXT_DELTA ? pos - _TRACE_TEXT_DELTA : 0;
        s += text.substr(left, pos - left);
        s += ">|<";
        s += text.substr(pos, min(_TRACE_TEXT_DELTA, text.size() - pos));

        boost::replace_all(s, "\r", "");
    }
    else
    {
        s = "EOF";
    }

    return s;
}

class _Trace
{
    const char* func_name;
    static size_t level;
public:
    _Trace(const char* s):func_name(s)
    {
        if (enable_trace) cout << tab(">") << ">" << func_name << endl;
        ++level;
    }

    ~_Trace()
    {
        level--;
        if (enable_trace) cout << tab("<") << "<" << func_name << endl << endl;
    }
    static string tab(string sym = " ");
};

#define _ENABLE_TRACE  // comment this to disable tracing

#ifdef _ENABLE_TRACE
#define TRACE_FUNC _Trace _tTr(__FUNCTION__);
#define TRACE_POS(pos_val) if(enable_trace) { cout << _Trace::tab() << "pos = " << pos_val << "; text = \"" << _text_at(text, pos_val) << "\"" << endl;}
#define TRACE_OUT if (enable_trace) { cout << _Trace::tab()
#define TRACE_END << endl;}
#else
#define TRACE_FUNC 
#define TRACE_POS(pos_val) 
#define TRACE_OUT /##/
#define TRACE_END 
#endif
//////////////////////////////////////////////////////////////////////////

using VectorString = vector<std::string>;

class ChaiEngine
{
protected:
    using module_type = std::list<ChaiEngine*>;
    static module_type *_modules;
    chaiscript::ModulePtr m = make_shared<chaiscript::Module>();
    static unique_ptr<chaiscript::ChaiScript> engine;
    static vector<string>* program_options;

    virtual void register_module() = 0;
public:
    ChaiEngine()
    {
        if (_modules == nullptr)
            _modules = new module_type();

        _modules->push_front(this);

        program_options = new vector<string>();
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
        
    static vector<string> * get_program_options();
    static chaiscript::ChaiScript* get_engine();
    static void start();
    static void stop();
};

#define DECLARE_MODULE(NAME) class ChaiEngine##NAME:public ChaiEngine { protected:  virtual void register_module() {

#define END_DECLARE(NAME) }}; static ChaiEngine##NAME module##NAME;

//////////////////////////////////////////////////////////////////////////