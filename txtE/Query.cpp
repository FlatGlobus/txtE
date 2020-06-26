#include <chaiscript/chaiscript.hpp>
#include"util.h"
#include "Cursor.h"
#include "Query.h"


//////////////////////////////////////////////////////////////////////////
Cursor* QueryBase::cursor = nullptr;
bool QueryBase::case_insensitive = true;
QueryBase::QueryBase()
{

}

QueryBase::~QueryBase()
{

}

bool QueryBase::execute() const
{
    return false;
}
//////////////////////////////////////////////////////////////////////////
Query::Query(Cursor* c)
{
    cursor = c;
}

Query::Query(Cursor* c, bool ci)
{
    cursor = c;
    case_insensitive = ci;
}

Cursor* Query::get_cursor()
{
    return cursor;
}

bool Query::execute() const
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
IsExact::IsExact(const std::string& p) :pattern(p)
{
}

IsExact::IsExact(const std::string& p, std::string* o) : pattern(p), out(o)
{
}

bool IsExact::execute() const
{
    if (cursor && cursor->is_eof() == false)
    {
        std::string text = cursor->get_text().substr(cursor->get_pos(), pattern.size());

        if (text == pattern)
        {
            if (out != nullptr)
            {
                *out = text;
            }
            cursor->inc(pattern.size() + 1);
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
IsAny::IsAny(const std::string& p, int c) :pattern(p), count(c)
{
}

IsAny::IsAny(const std::string& p, int c, std::string* o) : pattern(p), count(c), out(o)
{
}

bool IsAny::execute() const
{
    if (cursor && cursor->is_eof() == false)
    {
        const std::string& text = cursor->get_text();
        auto pos = cursor->get_pos();
        auto i = text.begin() + pos;
        size_t qty = 0;
        for (; i < text.end(); ++i)
        {
            if (pattern.find(*i) != std::string::npos)
            {
                ++qty;
                if (count != -1)
                {
                    if (count == qty)
                        break;
                }
                else
                {
                    break;
                }
            }
        }

        if (qty > 0)
        {
            qty = distance(text.begin() + pos, i) + 1;
            if (out != nullptr)
            {
                *out = text.substr(pos, qty);
            }
            cursor->inc(qty);
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Is::Is(const is_func f, const int c):func(f), count(c)
{
}

Is::Is(const is_func f, const int c, std::string* o) : func(f), count(c), out(o)
{

}

bool Is::execute() const
{
    if (cursor && cursor->is_eof() == false)
    {
        const std::string &text = cursor->get_text();
        auto pos = cursor->get_pos();
        auto i = text.begin() + pos;

        for (; i < text.end(); ++i)
        {
            if (func(*i) == 0)
                break;
        }

        size_t qty = distance(text.begin() + pos, i);
        if(count == -1)
        {
            if (qty > 0)
            {
                cursor->inc(qty);
                return true;
            }
        }
        else
        {
            if (qty >= count)
            {
                if (out != nullptr)
                {
                    *out = text.substr(pos, std::min(qty, count) );
                }
                cursor->inc(std::min(qty, count));
                return true;
            }
        }
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(QUERY)
m->add(chaiscript::constructor<QueryBase()>(), "QueryBase");
m->add(chaiscript::user_type<QueryBase>(), "QueryBase");
m->add(chaiscript::type_conversion<QueryBase, bool>([](const QueryBase& q)-> bool { return q.execute(); }));

m->add(chaiscript::constructor<Query(Cursor*)>(), "Query");
m->add(chaiscript::constructor<Query(Cursor*, bool)>(), "Query");
m->add(chaiscript::base_class<QueryBase, Query>());
m->add(chaiscript::type_conversion<Query, bool>([](const Query& q)-> bool {return q.execute(); }));

m->add(chaiscript::constructor<IsExact(const std::string&)>(), "IsExact");
m->add(chaiscript::constructor<IsExact(const std::string&, std::string*)>(), "IsExact");
m->add(chaiscript::user_type<IsExact>(), "IsExact");
m->add(chaiscript::base_class<QueryBase, IsExact>());
m->add(chaiscript::type_conversion<IsExact, bool>([](const IsExact& q)-> bool { return q.execute(); }));

m->add(chaiscript::constructor<IsAny(const std::string&, int)>(), "IsAny");
m->add(chaiscript::constructor<IsAny(const std::string&, int, std::string*)>(), "IsAny");
m->add(chaiscript::user_type<IsAny>(), "IsAny");
m->add(chaiscript::base_class<QueryBase, IsAny>());
m->add(chaiscript::type_conversion<IsAny, bool>([](const IsAny& q)-> bool { return q.execute(); }));

m->add(chaiscript::constructor<Is(const is_func, const int)>(), "Is");
m->add(chaiscript::constructor<Is(const is_func, const int, std::string*)>(), "Is");

m->add(chaiscript::user_type<Is>(), "Is");
m->add(chaiscript::base_class<QueryBase, Is>());
m->add(chaiscript::type_conversion<Is, bool>([](const Is& q)-> bool { return q.execute(); }));


END_DECLARE(QUERY)