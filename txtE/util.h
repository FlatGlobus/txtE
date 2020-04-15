#pragma once
#include <sstream>
#include <iostream>
#include <algorithm> 
#include <chaiscript/chaiscript.hpp>
#include <list>
#include <tchar.h>

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

extern bool enable_trace;

#define TRACE_ if (enable_trace) { std::cout <<  __FUNCTION__ << " pos = " << pos << " text = \"" << (!is_eof(pos) ? text.substr(pos, std::min((size_t)16, text.size() - pos)): "EOF") << "\" " 
#define ENDLTRACE_ << endl;}
#define ENDTRACE_ ;}

class ChaiEngine
{
protected:
    using module_type = std::list<ChaiEngine*>;
    static module_type *_modules;
    chaiscript::ModulePtr m = std::make_shared<chaiscript::Module>();
    static std::unique_ptr<chaiscript::ChaiScript> engine;

    virtual void register_module() = 0;
    static void init_std();
public:
    ChaiEngine()
    {
        if (_modules == nullptr)
            _modules = new module_type();

        _modules->push_front(this);
    }
    
    virtual ~ChaiEngine()
    {
        if (_modules)
        {
            delete _modules;
            _modules = nullptr;
        }
    }
    static chaiscript::ChaiScript* get_engine();
    static void start();
    static void stop();
};

#define DECLARE_MODULE(NAME) class ChaiEngine##NAME:public ChaiEngine { protected:  virtual void register_module() {

#define END_DECLARE(NAME) }}; static ChaiEngine##NAME module##NAME;

//////////////////////////////////////////////////////////////////////////