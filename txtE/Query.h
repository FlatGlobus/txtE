#pragma once
#include <string>
//////////////////////////////////////////////////////////////////////////
class Cursor;
class Query;

class QueryBase
{
protected:
    static Cursor *cursor;
    static bool case_insensitive;
public:
    QueryBase();
    virtual ~QueryBase();
    virtual bool execute() const;
};
//////////////////////////////////////////////////////////////////////////
class Query:public QueryBase
{
protected:
    
public:
    Query(Cursor*);
    Query(Cursor*, bool case_insensitive);

    Cursor *get_cursor();
    virtual bool execute() const;
};

class Exact : public QueryBase
{
    std::string pattern;
    std::string* out = nullptr;
public:
    Exact(const std::string&);
    Exact(const std::string&, std::string* out);
    virtual bool execute() const;
};

class Any : public QueryBase
{
    std::string pattern;
    int count = 0;
    std::string* out = nullptr;
public:
    Any(const std::string&);
    Any(const std::string&, int count);
    Any(const std::string&, int count, std::string* out);
    Any(int count, std::string* out);
    virtual bool execute() const;
};

using is_func = function<int(unsigned char)>;

class Is : public QueryBase
{
public:
    size_t count = 0;
    is_func func;
    std::string* out= nullptr;

    Is(const is_func func);
    Is(const is_func func, int count);
    Is(const is_func func, int count, std::string* out);

    virtual bool execute() const;
};

class Range : public QueryBase
{
public:

    size_t count = 0;
    std::string* out = nullptr;
    char from = '\0', to = '\0';

    Range(char from, char to);
    Range(char from, char to, int count);
    Range(char from, char to, int count, std::string* out);

    virtual bool execute() const;
};

class Set : public QueryBase
{
    std::vector<std::string> pattern;
    std::string* out = nullptr;
public:
    Set(const std::vector<std::string>&);
    Set(const std::vector<std::string>&, std::string* out);
    virtual bool execute() const;
};

class Endl : public QueryBase
{
public:
    Endl();
    virtual bool execute() const;
};
