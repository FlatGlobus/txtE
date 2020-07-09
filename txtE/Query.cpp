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
Exact::Exact(const std::string& p) :pattern(p)
{
}

Exact::Exact(const std::string& p, std::string* o) : pattern(p), out(o)
{
}

bool Exact::execute() const
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
Any::Any(const std::string& p, int c) :pattern(p), count(c)
{
}

Any::Any(const std::string& p, int c, std::string* o) : pattern(p), count(c), out(o)
{
}

Any::Any(int c, std::string* o): count(c), out(o)
{

}

bool Any::execute() const
{
    if (cursor && cursor->is_eof() == false)
    {
        const std::string& text = cursor->get_text();
        auto pos = cursor->get_pos();
        auto i = text.begin() + pos;
        size_t found_qty = 0;
        for (; i < text.end(); ++i)
        {
            if ((pattern.find(*i) != std::string::npos) || (pattern.size() == 0 && count != -1))
            {
                ++found_qty;
                if (count != -1)
                {
                    if (count == found_qty)
                        break;
                }
                continue;
            }
            break;
        }

        if (found_qty > 0)
        {
            size_t qty = distance(text.begin() + pos, i) + 1;
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
        const std::string& text = cursor->get_text();
        auto pos = cursor->get_pos();
        auto i = text.begin() + pos;
        size_t found_qty = 0;
        for (; i < text.end(); ++i)
        {
            if (func(*i) != 0)
            {
                ++found_qty;
                if (count != -1)
                {
                    if (count == found_qty)
                        break;
                }
                else
                {
                    break;
                }
            }
        }

        if (found_qty > 0)
        {
            size_t qty = distance(text.begin() + pos, i) + 1;
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

Range::Range(char f, char t, const int c): from(f), to(t), count(c)
{
}
    
Range::Range(const char f, const char t, const int c, std::string* o) : from(f), to(t), count(c), out(o)
{
}

bool Range::execute() const
{
    if (cursor && cursor->is_eof() == false)
    {
        const std::string& text = cursor->get_text();
        auto pos = cursor->get_pos();
        auto i = text.begin() + pos;
        size_t found_qty = 0;
        for (; i < text.end(); ++i)
        {
            if (from <= *i && *i <= to)
            {
                ++found_qty;
                if (count != -1)
                {
                    if (count == found_qty)
                        break;
                }
                else
                {
                    break;
                }
            }
        }

        if (found_qty > 0)
        {
            size_t qty = distance(text.begin() + pos, i) + 1;
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
Set::Set(const std::vector<std::string>& p) :pattern(p)
{
}

Set::Set(const std::vector<std::string>& p, std::string* o) : pattern(p), out(o)
{
}

bool Set::execute() const
{
    if (cursor && cursor->is_eof() == false)
    {
        for (auto p : pattern)
        {
            std::string text = cursor->get_text().substr(cursor->get_pos(), p.size());

            if (text == p)
            {
                if (out != nullptr)
                {
                    *out = text;
                }
                cursor->inc(p.size() + 1);
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
m->add(chaiscript::type_conversion<QueryBase, bool>([](const QueryBase& q) { return q.execute(); }));

m->add(chaiscript::constructor<Query(Cursor*)>(), "Query");
m->add(chaiscript::constructor<Query(Cursor*, bool)>(), "Query");
m->add(chaiscript::base_class<QueryBase, Query>());
m->add(chaiscript::type_conversion<Query, bool>([](const Query& q) {return q.execute(); }));

m->add(chaiscript::constructor<Exact(const std::string&)>(), "Exact");
m->add(chaiscript::constructor<Exact(const std::string&, std::string*)>(), "Exact");
m->add(chaiscript::user_type<Exact>(), "Exact");
m->add(chaiscript::base_class<QueryBase, Exact>());
m->add(chaiscript::type_conversion<Exact, bool>([](const Exact& q) { return q.execute(); }));

m->add(chaiscript::constructor<Any(const std::string&, int)>(), "Any");
m->add(chaiscript::constructor<Any(const std::string&, int, std::string*)>(), "Any");
m->add(chaiscript::constructor<Any(int, std::string*)>(), "Any");
m->add(chaiscript::user_type<Any>(), "Any");
m->add(chaiscript::base_class<QueryBase, Any>());
m->add(chaiscript::type_conversion<Any, bool>([](const Any& q) { return q.execute(); }));

m->add(chaiscript::constructor<Is(const is_func, const int)>(), "Is");
m->add(chaiscript::constructor<Is(const is_func, const int, std::string*)>(), "Is");
m->add(chaiscript::user_type<Is>(), "Is");
m->add(chaiscript::base_class<QueryBase, Is>());
m->add(chaiscript::type_conversion<Is, bool>([](const Is& q) { return q.execute(); }));

m->add(chaiscript::constructor<Range(const char, const char, const int)>(), "Range");
m->add(chaiscript::constructor<Range(const char, const char, const int, std::string*)>(), "Range");
m->add(chaiscript::user_type<Range>(), "Range");
m->add(chaiscript::base_class<QueryBase, Range>());
m->add(chaiscript::type_conversion<Range, bool>([](const Range& q) { return q.execute(); }));

m->add(chaiscript::constructor<Set(const std::vector<std::string>&)>(), "Set");
m->add(chaiscript::constructor<Set(const std::vector<std::string>&, std::string*)>(), "Set");
m->add(chaiscript::user_type<Set>(), "Set");
m->add(chaiscript::base_class<QueryBase, Set>());
m->add(chaiscript::type_conversion<Set, bool>([](const Set& q) { return q.execute(); }));

END_DECLARE(QUERY)