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

    int count = -1;
    string* out = nullptr;
    vector<chaiscript::Boxed_Value>* outs = nullptr;

    inline void set_out(const string& s) const
    {
        if (out != nullptr)
        {
            *out = s;
            return;
        }

        if (outs != nullptr)
        {
            outs->push_back(chaiscript::var(s));
        }
    }
public:
    QueryBase();
    QueryBase(int);
    QueryBase(int, string*);
    QueryBase(int, vector<chaiscript::Boxed_Value>*);

    virtual ~QueryBase();
    virtual bool execute() const;
};

using VectorQuery = vector<QueryBase*> ;
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
public:
    Exact(const string&);
    Exact(const string&, string* out);
    Exact(const string&, vector<chaiscript::Boxed_Value>* out);
    virtual bool execute() const;
};

class Any : public QueryBase
{
    string pattern;
public:
    Any(const string&);
    Any(const string&, int count);
    Any(const string&, int count, string* out);
    Any(const string&, int count, vector<chaiscript::Boxed_Value>* outs);
    Any(int count, string* out);
    Any(int count, vector<chaiscript::Boxed_Value>* outs);
    virtual bool execute() const;
};

using is_func = function<int(unsigned char)>;

class Is : public QueryBase
{
public:
    is_func func;

    Is(const is_func func);
    Is(const is_func func, int count);
    Is(const is_func func, int count, string* out);
    Is(const is_func func, int count, vector<chaiscript::Boxed_Value>* out);

    virtual bool execute() const;
};

class Range : public QueryBase
{
public:
    char from = '\0', to = '\0';

    Range(char from, char to);
    Range(char from, char to, int count);
    Range(char from, char to, int count, string* out);
    Range(char from, char to, int count, vector<chaiscript::Boxed_Value>* out);

    virtual bool execute() const;
};

class Set : public QueryBase
{
    VectorString pattern;
public:
    Set(const VectorString&);
    Set(const VectorString&, string* out);
    Set(const VectorString&, vector<chaiscript::Boxed_Value>* out);

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
public:
    Word();
    Word(string* out);
    Word(vector<chaiscript::Boxed_Value>* out);

    virtual bool execute() const;
};

class Number : public QueryBase
{
    bool is_number(const string&) const;
public:
    Number();
    Number(string* out);
    Number(vector<chaiscript::Boxed_Value>* out);

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

class Group : public QueryBase
{
    VectorQuery query;
public:
    Group(int, const vector<chaiscript::Boxed_Value>&);
    virtual bool execute() const;
};
