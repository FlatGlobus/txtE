#pragma once
#include "cursor.h"
//#include "Mustache.h"
#include "map"
//////////////////////////////////////////////////////////////////////////
namespace query
{
    using qdata_map = std::map < std::string, std::string > ;
    class QData
    {
        bool freeze_data = false;
        qdata_map data;
    public:
        QData();
       
        void set(const std::string&, const std::string&);
        const string& get(const std::string&);
        void reset_data();
        void reset_data(const std::string&);
        void set_freeze_data(bool val);

        const qdata_map& get_data() { return data; }
    };

    class Query;
    class QueryBase
    {
    protected:
        int count = -1;
        Query* query = nullptr;
        std::string key;

        void set_out(const std::string& s) const;
        void reset();
        bool check_count(int) const;
        bool skip_space() const;

        QueryBase();
    public:

        QueryBase(Query*);
        QueryBase(Query*, int);
        QueryBase(Query*, const std::string&);
        QueryBase(Query*, int, const std::string&);

        virtual ~QueryBase();
        virtual bool execute() const;
    };

    using VectorQuery = std::vector<QueryBase*>;
    //////////////////////////////////////////////////////////////////////////
    class Query :public QueryBase, public QData
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
        void reset_last_data();
        const string& get(const std::string& key) { return QData::get(key); }
    };

    class Match : public QueryBase
    {
        std::string pattern;
    public:
        Match(Query*, const std::string&);
        Match(Query*, const std::string&, const std::string& out);
        virtual bool execute() const;
    };


    class AnyOf : public QueryBase
    {
        std::string pattern;
    public:
        AnyOf(Query*, const std::string&);
        AnyOf(Query*, const std::string&, int count);
        AnyOf(Query*, const std::string&, int count, const std::string& out);
        AnyOf(Query*, const std::string&, const std::string& out);
        virtual bool execute() const;
    };

    class AnyNot : public QueryBase
    {
        std::string pattern;
    public:
        AnyNot(Query*, const std::string&);
        AnyNot(Query*, const std::string&, int count);
        AnyNot(Query*, const std::string&, int count, const std::string& out);
        AnyNot(Query*, const std::string&, const std::string& out);
        virtual bool execute() const;
    };

    using is_func = function<int(unsigned char)>;
    class Is : public QueryBase
    {
    public:
        is_func func;

        Is(Query*, const is_func func);
        Is(Query*, const is_func func, int count);
        Is(Query*, const is_func func, int count, const std::string& out);
        Is(Query*, const is_func func, const std::string& out);

        virtual bool execute() const;
    };

    class Range : public QueryBase
    {
    public:
        char from = '\0', to = '\0';

        Range(Query*, char from, char to);
        Range(Query*, char from, char to, int count);
        Range(Query*, char from, char to, int count, const std::string& out);
        Range(Query*, char from, char to, const std::string& out);

        virtual bool execute() const;
    };

    class Set : public QueryBase
    {
        VectorString pattern;
    public:
        Set(Query*, const VectorString&);
        Set(Query*, const VectorString&, const std::string&);

        virtual bool execute() const;
    };

    class Endl : public QueryBase
    {
    public:
        Endl(Query* q);
        virtual bool execute() const;
    };

    class Word : public QueryBase
    {
    public:
        Word(Query*);
        Word(Query*, const std::string& out);
        

        virtual bool execute() const;
    };

    class RegEx : public QueryBase
    {
    protected:
        const VectorString& rg_pattern;
        virtual std::tuple<std::string::const_iterator, std::string::const_iterator> extract_string() const;
    public:
        RegEx(Query*, const VectorString& rg_pattern);
        RegEx(Query*, const VectorString& rg_pattern, const std::string& out);
        
        virtual bool execute() const;
    };

    class Number : public RegEx
    {
    public:
        Number(Query*);
        Number(Query*, const std::string& out);
    };

    class Hex : public RegEx
    {
    public:
        Hex(Query*);
        Hex(Query*, const std::string& out);
    };

    class Date : public RegEx
    {
    public:
        Date(Query*);
        Date(Query*, const std::string& out);
    };

    class Time : public RegEx
    {
    public:
        Time(Query*);
        Time(Query*, const std::string& out);
    };

    class Url : public RegEx
    {
    public:
        Url(Query*);
        Url(Query*, const std::string& out);
    };

    class IPv4 : public RegEx
    {
    public:
        IPv4(Query*);
        IPv4(Query*, const std::string& out);
    };

    class IPv6 : public RegEx
    {
    public:
        IPv6(Query*);
        IPv6(Query*, const std::string& out);
    };

    class Phone : public RegEx
    {
    public:
        Phone(Query*);
        Phone(Query*, const std::string& out);
    };

    class Path : public RegEx
    {
    public:
        Path(Query*);
        Path(Query*, const std::string& out);
    };

    class Space : public QueryBase
    {
    public:
        Space(Query*);
        virtual bool execute() const;
    };

    class Group : public QueryBase
    {
        VectorQuery query_vector;
        bool shift = false;
    public:
        Group(Query*, const std::vector<chaiscript::Boxed_Value>&);
        Group(Query*, int, const std::vector<chaiscript::Boxed_Value>&);
        Group(Query*, int, bool, const std::vector<chaiscript::Boxed_Value>&);
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
        ZeroOne(Query*, chaiscript::Boxed_Value&);
        virtual bool execute() const;
    };

}