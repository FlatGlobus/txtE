#pragma once
#include <string>
//////////////////////////////////////////////////////////////////////////
using namespace std;

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

using VectorQuery = vector<const QueryBase*> ;
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
    string pattern;
    string* out = nullptr;
public:
    Exact(const string&);
    Exact(const string&, string* out);
    virtual bool execute() const;
};

class Any : public QueryBase
{
    string pattern;
    int count = 0;
    string* out = nullptr;
public:
    Any(const string&);
    Any(const string&, int count);
    Any(const string&, int count, string* out);
    Any(int count, string* out);
    virtual bool execute() const;
};

using is_func = function<int(unsigned char)>;

class Is : public QueryBase
{
public:
    size_t count = 0;
    is_func func;
    string* out= nullptr;

    Is(const is_func func);
    Is(const is_func func, int count);
    Is(const is_func func, int count, string* out);

    virtual bool execute() const;
};

class Range : public QueryBase
{
public:

    size_t count = 0;
    string* out = nullptr;
    char from = '\0', to = '\0';

    Range(char from, char to);
    Range(char from, char to, int count);
    Range(char from, char to, int count, string* out);

    virtual bool execute() const;
};

class Set : public QueryBase
{
    vector<string> pattern;
    string* out = nullptr;
public:
    Set(const vector<string>&);
    Set(const vector<string>&, string* out);
    virtual bool execute() const;
};

class Endl : public QueryBase
{
public:
    Endl();
    virtual bool execute() const;
};

class Word : public QueryBase
{
    string* out = nullptr;
public:
    Word();
    Word(string* out);
    virtual bool execute() const;
};

class Number : public QueryBase
{
    string* out = nullptr;
    bool is_number(const string&) const;
public:
    Number();
    Number(string* out);
    virtual bool execute() const;
};

class Space : public QueryBase
{
public:
    Space();
    virtual bool execute() const;
};

class Starts : public QueryBase
{
    string pattern;
public:
    Starts(const string&);
    virtual bool execute() const;
};

class Count : public QueryBase
{
    VectorQuery query;
    int count;
public:
    Count(int, const vector<chaiscript::Boxed_Value>&);
    virtual bool execute() const;
};
