#include <chaiscript/chaiscript.hpp>
#include"util.h"
#include "Query.h"
#include "Cursor.h"

//////////////////////////////////////////////////////////////////////////
Cursor* QueryBase::cursor = nullptr;
bool QueryBase::case_insensitive = true;
QueryBase::QueryBase()
{

}

QueryBase::QueryBase(int c): count(c)
{
}

QueryBase::QueryBase(int c, string* o): count(c), out(o)
{
}

QueryBase::QueryBase(int c, vector<chaiscript::Boxed_Value>* o): count(c), outs(o)
{
}

QueryBase::~QueryBase()
{
}

bool QueryBase::execute() const
{
    if (cursor == nullptr)
    {
        throw runtime_error("QueryBase: Cursor is null.");
    }
    return cursor->is_eof() == false;
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
Exact::Exact(const string& p) :pattern(p)
{
}

Exact::Exact(const string& p, string* o) : QueryBase(-1, o),pattern(p)
{
}

Exact::Exact(const string& p, vector<chaiscript::Boxed_Value>* o) : QueryBase(-1, o), pattern(p)
{

}
bool Exact::execute() const
{
    TRACE_FUNC;

    if (QueryBase::execute())
    {
        string text = cursor->get_text().substr(cursor->get_pos(), pattern.size());
        if (text == pattern)
        {
            set_out(text);
            TRACE_OUT << "found " << "text = " << text TRACE_END
            cursor->inc(pattern.size());
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Any::Any(const string& p) :pattern(p)
{
}

Any::Any(const string& p, int c) : QueryBase(c), pattern(p)
{
    
}

Any::Any(const string& p, int c, string* o) : QueryBase(c, o), pattern(p)
{
}

Any::Any(int c, string* o): QueryBase(c, o)
{

}

Any::Any(const string& p, int c, vector<chaiscript::Boxed_Value>* o) : QueryBase(c, o), pattern(p)
{
}

Any::Any(int c, vector<chaiscript::Boxed_Value>* o) : QueryBase(c, o)
{
}

bool Any::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        const string& text = cursor->get_text();
        const auto& pos = cursor->get_pos();
        auto i = text.begin() + pos;
        size_t found_qty = 0;

        for (; i < text.end(); ++i)
        {
            if ((pattern.find(*i) != string::npos) || (pattern.size() == 0 && count != -1))
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
            size_t qty = distance(text.begin() + pos, i);
            string result = text.substr(pos, qty);
            set_out(result);
            TRACE_OUT << "found " << "text = " << result TRACE_END
            cursor->inc(qty);
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Is::Is(const is_func f) :func(f)
{
}

Is::Is(const is_func f, int c):func(f), QueryBase(c)
{
}

Is::Is(const is_func f, int c, string* o) : func(f), QueryBase(c, o)
{
}

Is::Is(const is_func f, int c, vector<chaiscript::Boxed_Value>* o) : func(f), QueryBase(c, o)
{
}

bool Is::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        const string& text = cursor->get_text();
        auto pos = cursor->get_pos();
        auto i = text.begin() + pos;
        size_t found_qty = 0;
        for (; i < text.end(); ++i)
        {
            if (func(*i) != 0)
            {
                ++found_qty;
                if (count != (size_t)-1)
                {
                    if (count == found_qty)
                        break;
                }
            }
            else
            {
                break;
            }
        }

        if (found_qty > 0)
        {
            size_t qty = distance(text.begin() + pos, i);
            string result = text.substr(pos, qty);
            set_out(result);
            TRACE_OUT << "found text = " << result TRACE_END
            cursor->inc(qty);
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Range::Range(char f, char t) : from(f), to(t)
{
}

Range::Range(char f, char t, int c): from(f), to(t), QueryBase(c)
{
}
    
Range::Range(char f, char t, int c, string* o) : from(f), to(t), QueryBase(c, o)
{
}

Range::Range(char f, char t, int c, vector<chaiscript::Boxed_Value>* o) : from(f), to(t), QueryBase(c, o)
{
}

bool Range::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        const string& text = cursor->get_text();
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
            size_t qty = distance(text.begin() + pos, i);
            string result = text.substr(pos, qty);
            set_out(result);
            TRACE_OUT << "found text = " << result TRACE_END
            cursor->inc(qty);
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Set::Set(const VectorString& p) :pattern(p)
{
}

Set::Set(const VectorString& p, string* o) : pattern(p), QueryBase(-1, o)
{
}

Set::Set(const VectorString& p, vector<chaiscript::Boxed_Value>* o) : pattern(p), QueryBase(-1, o)
{
}

bool Set::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        for (auto p : pattern)
        {
            string text = cursor->get_text().substr(cursor->get_pos(), p.size());

            if (text == p)
            {
                set_out(text);
                TRACE_OUT << "found text = " << text TRACE_END
                cursor->inc(p.size());
                return true;
            }
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Endl::Endl()
{
}

bool Endl::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        string text = cursor->get_text().substr(cursor->get_pos(), ENDL_SIZE);

        if (text == ENDL)
        {
            TRACE_OUT << "found ENDL" TRACE_END
            cursor->inc(ENDL_SIZE);
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Word::Word()
{
}

Word::Word(string* o):QueryBase(-1, o)
{
}

Word::Word(vector<chaiscript::Boxed_Value>* o) : QueryBase(-1, o)
{
}

bool Word::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        const string& text = cursor->get_text();
        size_t p = cursor->get_pos();
        
        if(space_pattern.find(*(text.begin() + p)) == string::npos)
        {
            auto p1 = text.find_first_of(space_pattern, p);
            if (cursor->is_eof(p1) == false)
            {
                string result = text.substr(p, p1 - p);
                set_out(result);
                TRACE_OUT << "found text = " << result TRACE_END;
                cursor->move_to(p1);
                return true;
            }
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Number::Number()
{
}

Number::Number(string* o) : QueryBase(-1, o)
{
}

Number::Number(vector<chaiscript::Boxed_Value>* o) : QueryBase(-1, o)
{
}

bool Number::execute() const
{
    const string pattern = "-+,.0123456789eE";
    TRACE_FUNC;
    if (QueryBase::execute())
    {
        const string& text = cursor->get_text();
        size_t p = cursor->get_pos();

        if (pattern.find(*(text.begin() + p)) != string::npos)
        {
            auto p1 = text.find_first_not_of(pattern, p);
            if (cursor->is_eof(p1) == false)
            {
                string number = text.substr(p, p1 - p);
                if (is_number(number))
                {
                    set_out(number);
                    TRACE_OUT << "found number = " << number TRACE_END;
                    cursor->move_to(p1);
                    return true;
                }
            }
        }
    }
    return false;
}

bool Number::is_number(const string& number) const //TODO check number
{
    
    return true;
}
//////////////////////////////////////////////////////////////////////////
Space::Space()
{
}

bool Space::execute() const
{
    TRACE_FUNC;
    if (cursor && cursor->is_eof() == false)
    {
        const string& text = cursor->get_text();
        size_t p = cursor->get_pos();

        if (space_pattern.find(*(text.begin() + p)) != string::npos)
        {
            auto p1 = text.find_first_not_of(space_pattern, p);
            if (cursor->is_eof(p1) == false)
            {
                TRACE_OUT << "found space" TRACE_END;
                cursor->move_to(p1);
                return true;
            }
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Starts::Starts(const string& p):pattern(p)
{
}

bool Starts::execute() const
{
    TRACE_FUNC;
    if (cursor && cursor->is_eof() == false)
    {
        auto p = cursor->get_text().find(pattern, cursor->get_pos());
        if (cursor->is_eof(p) == false)
        {
            TRACE_OUT << "found " << "text = " << pattern TRACE_END
            cursor->move_to(p + pattern.size());
            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
Group::Group(int c, const vector<chaiscript::Boxed_Value>& v):QueryBase(c)
{
    chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();
    for (auto p : v)
    {
        query.push_back((QueryBase*)p.get_ptr());//TODO check pointer
    }
}

bool Group::execute() const
{
    TRACE_FUNC;
    if (QueryBase::execute() && query.size())
    {
        int c = 0;
        do 
        {
            for (auto i : query)
            {
                if (i->execute() == false)
                    return c > 0 && count == -1;
            }
            ++c;
        } while ((count != -1 && c < count) || count == -1);

        return (count == -1 && c > 0) || (count != -1 && c == count);
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(QUERY)

//m->add(chaiscript::constructor<QueryBase()>(), "QueryBase");
m->add(chaiscript::user_type<QueryBase>(), "QueryBase");
//m->add(chaiscript::type_conversion<QueryBase, bool>([](QueryBase& q) { return q.execute(); }));

m->add(chaiscript::constructor<Query(Cursor*)>(), "Query");
m->add(chaiscript::constructor<Query(Cursor*, bool)>(), "Query");
m->add(chaiscript::base_class<QueryBase, Query>());
m->add(chaiscript::type_conversion<Query, bool>([](const Query& q) {return q.execute(); }));

m->add(chaiscript::constructor<Exact(const string&)>(), "Exact");
m->add(chaiscript::constructor<Exact(const string&, string*)>(), "Exact");
m->add(chaiscript::constructor<Exact(const string&, vector<chaiscript::Boxed_Value>*)>(), "Exact");
m->add(chaiscript::user_type<Exact>(), "Exact");
m->add(chaiscript::base_class<QueryBase, Exact>());
m->add(chaiscript::type_conversion<Exact, bool>([](const Exact& q) { return q.execute(); }));

m->add(chaiscript::constructor<Any(const string&)>(), "Any");
m->add(chaiscript::constructor<Any(const string&, int)>(), "Any");
m->add(chaiscript::constructor<Any(const string&, int, string*)>(), "Any");
m->add(chaiscript::constructor<Any(int, string*)>(), "Any");
m->add(chaiscript::constructor<Any(const string&, int, vector<chaiscript::Boxed_Value>*)>(), "Any");
m->add(chaiscript::constructor<Any(int, vector<chaiscript::Boxed_Value>*)>(), "Any");

m->add(chaiscript::user_type<Any>(), "Any");
m->add(chaiscript::base_class<QueryBase, Any>());
m->add(chaiscript::type_conversion<Any, bool>([](const Any& q) { return q.execute(); }));

m->add(chaiscript::constructor<Is(const is_func)>(), "Is");
m->add(chaiscript::constructor<Is(const is_func, int)>(), "Is");
m->add(chaiscript::constructor<Is(const is_func, int, string*)>(), "Is");
m->add(chaiscript::constructor<Is(const is_func, int, vector<chaiscript::Boxed_Value>*)>(), "Is");
m->add(chaiscript::user_type<Is>(), "Is");
m->add(chaiscript::base_class<QueryBase, Is>());
m->add(chaiscript::type_conversion<Is, bool>([](const Is& q) { return q.execute(); }));

m->add(chaiscript::constructor<Range(char, char)>(), "Range");
m->add(chaiscript::constructor<Range(char, char, int)>(), "Range");
m->add(chaiscript::constructor<Range(char, char, int, string*)>(), "Range");
m->add(chaiscript::constructor<Range(char, char, int, vector<chaiscript::Boxed_Value>*)>(), "Range");
m->add(chaiscript::user_type<Range>(), "Range");
m->add(chaiscript::base_class<QueryBase, Range>());
m->add(chaiscript::type_conversion<Range, bool>([](const Range& q) { return q.execute(); }));

m->add(chaiscript::constructor<Set(const VectorString&)>(), "Set");
m->add(chaiscript::constructor<Set(const VectorString&, string*)>(), "Set");
m->add(chaiscript::constructor<Set(const VectorString&, vector<chaiscript::Boxed_Value>*)>(), "Set");
m->add(chaiscript::user_type<Set>(), "Set");
m->add(chaiscript::base_class<QueryBase, Set>());
m->add(chaiscript::type_conversion<Set, bool>([](const Set& q) { return q.execute(); }));

m->add(chaiscript::constructor<Endl()>(), "Endl");
m->add(chaiscript::base_class<QueryBase, Endl>());
m->add(chaiscript::type_conversion<Endl, bool>([](const Endl& q) {return q.execute(); }));

m->add(chaiscript::constructor<Word()>(), "Word");
m->add(chaiscript::constructor<Word(string*)>(), "Word");
m->add(chaiscript::constructor<Word(vector<chaiscript::Boxed_Value>*)>(), "Word");
m->add(chaiscript::user_type<Word>(), "Word");
m->add(chaiscript::base_class<QueryBase, Word>());
m->add(chaiscript::type_conversion<Word, bool>([](const Word& q) { return q.execute(); }));

m->add(chaiscript::constructor<Number()>(), "Number");
m->add(chaiscript::constructor<Number(string*)>(), "Number");
m->add(chaiscript::constructor<Number(vector<chaiscript::Boxed_Value>*)>(), "Number");
m->add(chaiscript::user_type<Number>(), "Number");
m->add(chaiscript::base_class<QueryBase, Number>());
m->add(chaiscript::type_conversion<Number, bool>([](const Number& q) { return q.execute(); }));

m->add(chaiscript::constructor<Space()>(), "Space");
m->add(chaiscript::user_type<Space>(), "Space");
m->add(chaiscript::base_class<QueryBase, Space>());
m->add(chaiscript::type_conversion<Space, bool>([](const Space& q) { return q.execute(); }));

m->add(chaiscript::constructor<Starts(const string&)>(), "Starts");
m->add(chaiscript::user_type<Starts>(), "Starts");
m->add(chaiscript::base_class<QueryBase, Starts>());
m->add(chaiscript::type_conversion<Starts, bool>([](const Starts& q) { return q.execute(); }));

m->add(chaiscript::constructor<Group(int, const vector<chaiscript::Boxed_Value>&)>(), "Group");
m->add(chaiscript::user_type<Group>(), "Count");
m->add(chaiscript::base_class<QueryBase, Group>());
m->add(chaiscript::type_conversion<Group, bool>([](const Group& q) { return q.execute(); }));

ChaiEngine::get_engine()->add(chaiscript::bootstrap::standard_library::vector_type<VectorQuery>("VectorQuery"));

END_DECLARE(QUERY)