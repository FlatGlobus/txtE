#pragma once
#include "cursor.h"
//////////////////////////////////////////////////////////////////////////
namespace query
{
    class Query;
    class QueryBase
    {
    protected:
        int count = -1;
        std::string* out = nullptr;
        std::vector<chaiscript::Boxed_Value>* outs = nullptr;
        const Query* query = nullptr;

        inline void set_out(const std::string& s) const
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

        void reset();
        bool check_count(int) const;
        bool skip_space() const;

        QueryBase();
    public:

        QueryBase(const Query*);
        QueryBase(const Query*, int);
        QueryBase(const Query*, int, std::string*);
        QueryBase(const Query*, int, std::vector<chaiscript::Boxed_Value>*);

        virtual ~QueryBase();
        virtual bool execute() const;

    };


    using VectorQuery = std::vector<QueryBase*>;
    //////////////////////////////////////////////////////////////////////////
    class Query :public QueryBase
    {
    protected:
        cursor::Cursor* cursor = nullptr;
        bool case_insensitive = true;
        bool skip_space = false; // TODO после найденного совпадени€ будет устанавливать текущцю позицию курсора на следующий не пробельный символ
                                 // таким образом можно будет избавитс€ от использовани€ Space в query
    public:
        Query(cursor::Cursor*);
        Query(cursor::Cursor*, bool case_insensitive);

        inline bool get_I() const { return case_insensitive; }
        inline bool get_skip_space() const { return skip_space; }
        inline void set_skip_space(bool skip) { skip_space = skip; }

        inline cursor::Cursor* get_cursor() const { return cursor; }
        virtual bool execute() const;
    };

    class Match : public QueryBase
    {
        std::string pattern;
    public:
        Match(const Query*, const std::string&);
        Match(const Query*, const std::string&, std::string* out);
        Match(const Query*, const std::string&, std::vector<chaiscript::Boxed_Value>* out);
        virtual bool execute() const;
    };


    class AnyOf : public QueryBase
    {
        std::string pattern;
    public:
        AnyOf(const Query*, const std::string&);
        AnyOf(const Query*, const std::string&, int count);
        AnyOf(const Query*, const std::string&, int count, std::string* out);
        AnyOf(const Query*, const std::string&, int count, std::vector<chaiscript::Boxed_Value>* outs);
        AnyOf(const Query*, int count, std::string* out);
        AnyOf(const Query*, int count, std::vector<chaiscript::Boxed_Value>* outs);
        virtual bool execute() const;
    };

    class AnyNot : public QueryBase
    {
        std::string pattern;
    public:
        AnyNot(const Query*, const std::string&);
        AnyNot(const Query*, const std::string&, int count);
        AnyNot(const Query*, const std::string&, int count, std::string* out);
        AnyNot(const Query*, const std::string&, int count, std::vector<chaiscript::Boxed_Value>* outs);
        AnyNot(const Query*, int count, std::string* out);
        AnyNot(const Query*, int count, std::vector<chaiscript::Boxed_Value>* outs);
        virtual bool execute() const;
    };

    using is_func = function<int(unsigned char)>;
    class Is : public QueryBase
    {
    public:
        is_func func;

        Is(const Query*, const is_func func);
        Is(const Query*, const is_func func, int count);
        Is(const Query*, const is_func func, int count, std::string* out);
        Is(const Query*, const is_func func, int count, std::vector<chaiscript::Boxed_Value>* out);

        virtual bool execute() const;
    };

    class Range : public QueryBase
    {
    public:
        char from = '\0', to = '\0';

        Range(const Query*, char from, char to);
        Range(const Query*, char from, char to, int count);
        Range(const Query*, char from, char to, int count, std::string* out);
        Range(const Query*, char from, char to, int count, std::vector<chaiscript::Boxed_Value>* out);

        virtual bool execute() const;
    };

    class Set : public QueryBase
    {
        VectorString pattern;
    public:
        Set(const Query*, const VectorString&);
        Set(const Query*, const VectorString&, std::string* out);
        Set(const Query*, const VectorString&, std::vector<chaiscript::Boxed_Value>* out);

        virtual bool execute() const;
    };

    class Endl : public QueryBase
    {
    public:
        Endl(const Query* q);
        virtual bool execute() const;
    };

    class Word : public QueryBase
    {
    public:
        Word(const Query*);
        Word(const Query*, std::string* out);
        Word(const Query*, std::vector<chaiscript::Boxed_Value>* out);

        virtual bool execute() const;
    };

    class RegEx : public QueryBase
    {
    protected:
        const VectorString& rg_pattern;
        virtual std::tuple<std::string::const_iterator, std::string::const_iterator> extract_string() const;
    public:
        RegEx(const Query*, const VectorString& rg_pattern);
        RegEx(const Query*, const VectorString& rg_pattern, std::string* out);
        RegEx(const Query*, const VectorString& rg_pattern, std::vector<chaiscript::Boxed_Value>* out);

        virtual bool execute() const;
    };

    class Number : public RegEx
    {
    public:
        Number(const Query*);
        Number(const Query*, std::string* out);
        Number(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Hex : public RegEx
    {
    public:
        Hex(const Query*);
        Hex(const Query*, std::string* out);
        Hex(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Date : public RegEx
    {
    public:
        Date(const Query*);
        Date(const Query*, std::string* out);
        Date(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Time : public RegEx
    {
    public:
        Time(const Query*);
        Time(const Query*, std::string* out);
        Time(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Url : public RegEx
    {
    public:
        Url(const Query*);
        Url(const Query*, std::string* out);
        Url(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class IPv4 : public RegEx
    {
    public:
        IPv4(const Query*);
        IPv4(const Query*, std::string* out);
        IPv4(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class IPv6 : public RegEx
    {
    public:
        IPv6(const Query*);
        IPv6(const Query*, std::string* out);
        IPv6(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Phone : public RegEx
    {
    public:
        Phone(const Query*);
        Phone(const Query*, std::string* out);
        Phone(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Path : public RegEx
    {
    public:
        Path(const Query*);
        Path(const Query*, std::string* out);
        Path(const Query*, std::vector<chaiscript::Boxed_Value>* out);
    };

    class Space : public QueryBase
    {
    public:
        Space(const Query*);
        virtual bool execute() const;
    };

    class Group : public QueryBase
    {
        VectorQuery query_vector;
        bool shift = false;
    public:
        Group(const Query*, const std::vector<chaiscript::Boxed_Value>&);
        Group(const Query*, int, const std::vector<chaiscript::Boxed_Value>&);
        Group(const Query*, int, bool, const std::vector<chaiscript::Boxed_Value>&);
        virtual bool execute() const;
    };

    class SkipSpace : public QueryBase
    {
    public:
        SkipSpace(Query*, bool);
        virtual bool execute() const;
    };

    class ZeroOne : public QueryBase
    {
//        chaiscript::Boxed_Value& query_to_execute;
    public:
        ZeroOne(const Query*, chaiscript::Boxed_Value&);
        virtual bool execute() const;
    };

}