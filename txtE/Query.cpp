#include <chaiscript/chaiscript.hpp>
#include"util.h"
#include "Query.h"
#include <regex>

using namespace std;
using namespace cursor;
using namespace text;

//////////////////////////////////////////////////////////////////////////
namespace query
{
    QueryBase::QueryBase()
    {
    }

    QueryBase::QueryBase(const Query* q) :query(q)
    {
    }

    QueryBase::QueryBase(const Query* q, int c) : query(q), count(c)
    {
    }

    QueryBase::QueryBase(const Query* q, int c, string* o) : query(q), count(c), out(o)
    {
        reset();
    }

    QueryBase::QueryBase(const Query* q, int c, vector<chaiscript::Boxed_Value>* o) : query(q), count(c), outs(o)
    {
        reset();
    }

    QueryBase::~QueryBase()
    {
    }

    bool QueryBase::execute() const
    {
        if (query->get_cursor() == nullptr)
        {
            throw runtime_error("QueryBase: Cursor is null.");
        }

        //reset();
        if (query->get_skip_space() == true && (dynamic_cast<const Space*> (this) == nullptr || dynamic_cast<const Endl*> (this) == nullptr))
            skip_space();

        return query->get_cursor()->eof() == false;
    }

    void QueryBase::reset()
    {
        if (out)
            out->clear();

        if (outs)
            outs->clear();
    }

    bool QueryBase::check_count(int c) const
    {
        return (count == -1 && c > 0) || (count != -1 && c == count);
    }

    bool QueryBase::skip_space() const
    {
        const string& text = query->get_cursor()->get_text();
        size_t p = query->get_cursor()->get_pos();

        if (space_pattern.find(*(text.begin() + p)) != string::npos)
        {
            Position p1 = text.find_first_not_of(space_pattern, p);
            if (p1.eof() == false)
            {
                query->get_cursor()->move_to(p1);
                return true;
            }
        }
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

    bool Query::execute() const
    {
        return QueryBase::execute();
    }

    //////////////////////////////////////////////////////////////////////////
    Match::Match(const Query* q, const string& p) : QueryBase(q), pattern(p)
    {
    }

    Match::Match(const Query* q, const string& p, string* o) : QueryBase(q, -1, o), pattern(p)
    {
    }

    Match::Match(const Query* q, const string& p, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, -1, o), pattern(p)
    {

    }
    bool Match::execute() const
    {
        TRACE_FUNC;

        if (QueryBase::execute())
        {
            string text = query->get_cursor()->get_text().substr(query->get_cursor()->get_pos(), pattern.size());
            if (text == pattern)
            {
                set_out(text);
                TRACE_OUT << "found " << "text = " << text TRACE_END;
                query->get_cursor()->inc(pattern.size());
                return true;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    AnyOf::AnyOf(const Query* q, const string& p) : QueryBase(q), pattern(p)
    {
    }

    AnyOf::AnyOf(const Query* q, const string& p, int c) : QueryBase(q, c), pattern(p)
    {

    }

    AnyOf::AnyOf(const Query* q, const string& p, int c, string* o) : QueryBase(q, c, o), pattern(p)
    {
    }

    AnyOf::AnyOf(const Query* q, int c, string* o) : QueryBase(q, c, o)
    {

    }

    AnyOf::AnyOf(const Query* q, const string& p, int c, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, c, o), pattern(p)
    {
    }

    AnyOf::AnyOf(const Query* q, int c, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, c, o)
    {
    }

    bool AnyOf::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute() && pattern.size() > 0)
        {
            const auto& pos = query->get_cursor()->get_pos();
            auto i = query->get_cursor()->get_string().begin() + pos;
            int found_qty = 0;

            for (; i != query->get_cursor()->get_string().end(); ++i)
            {
                if (pattern.find(*i) != string::npos)
                {
                    ++found_qty;
                    if (count != -1 && count == found_qty)
                    {
                        ++i;
                        break;
                    }
                    continue;
                }
                break;
            }

            if (check_count(found_qty))
            {
                size_t qty = distance(query->get_cursor()->get_string().begin() + pos, i);
                string result = query->get_cursor()->get_text().substr(pos, qty);
                set_out(result);
                TRACE_OUT << "found " << "text = " << result TRACE_END
                    query->get_cursor()->inc(qty);
                return true;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    AnyNot::AnyNot(const Query* q, const string& p) : QueryBase(q), pattern(p)
    {
    }

    AnyNot::AnyNot(const Query* q, const string& p, int c) : QueryBase(q, c), pattern(p)
    {

    }

    AnyNot::AnyNot(const Query* q, const string& p, int c, string* o) : QueryBase(q, c, o), pattern(p)
    {
    }

    AnyNot::AnyNot(const Query* q, int c, string* o) : QueryBase(q, c, o)
    {

    }

    AnyNot::AnyNot(const Query* q, const string& p, int c, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, c, o), pattern(p)
    {
    }

    AnyNot::AnyNot(const Query* q, int c, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, c, o)
    {
    }

    bool AnyNot::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute() && pattern.size() > 0)
        {
            const auto& pos = query->get_cursor()->get_pos();
            auto i = query->get_cursor()->get_string().begin() + pos;
            int found_qty = 0;

            for (; i != query->get_cursor()->get_string().end(); ++i)
            {
                if (pattern.find(*i) == string::npos)
                {
                    ++found_qty;
                    if (count != -1 && count == found_qty)
                    {
                        ++i;
                        break;
                    }
                    continue;
                }
                break;
            }

            if (check_count(found_qty))
            {
                size_t qty = distance(query->get_cursor()->get_string().begin() + pos, i);
                string result = query->get_cursor()->get_text().substr(pos, qty);
                set_out(result);
                TRACE_OUT << "found " << "text = " << result TRACE_END;
                query->get_cursor()->inc(qty);
                return true;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Is::Is(const Query* q, const is_func f) : QueryBase(q), func(f)
    {
    }

    Is::Is(const Query* q, const is_func f, int c) : func(f), QueryBase(q, c)
    {
    }

    Is::Is(const Query* q, const is_func f, int c, string* o) : func(f), QueryBase(q, c, o)
    {
    }

    Is::Is(const Query* q, const is_func f, int c, vector<chaiscript::Boxed_Value>* o) : func(f), QueryBase(q, c, o)
    {
    }

    bool Is::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute())
        {
            const auto& pos = query->get_cursor()->get_pos();
            auto i = query->get_cursor()->get_string().begin() + pos;
            int found_qty = 0;
            for (; i < query->get_cursor()->get_string().end(); ++i)
            {
                if (func(*i) != 0)
                {
                    ++found_qty;
                    if (count != -1 && count == found_qty)
                    {
                        ++i;
                        break;
                    }
                    continue;
                }
                break;
            }

            if (check_count(found_qty))
            {
                size_t qty = distance(query->get_cursor()->get_string().begin() + pos, i);
                string result = query->get_cursor()->get_text().substr(pos, qty);
                set_out(result);
                TRACE_OUT << "found text = " << result TRACE_END;
                query->get_cursor()->inc(qty);
                return true;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Range::Range(const Query* q, char f, char t) : QueryBase(q), from(f), to(t)
    {
    }

    Range::Range(const Query* q, char f, char t, int c) : from(f), to(t), QueryBase(q, c)
    {
    }

    Range::Range(const Query* q, char f, char t, int c, string* o) : from(f), to(t), QueryBase(q, c, o)
    {
    }

    Range::Range(const Query* q, char f, char t, int c, vector<chaiscript::Boxed_Value>* o) : from(f), to(t), QueryBase(q, c, o)
    {
    }

    bool Range::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute())
        {
            const auto& pos = query->get_cursor()->get_pos();
            auto i = query->get_cursor()->get_string().begin() + pos;
            int found_qty = 0;
            for (; i < query->get_cursor()->get_string().end(); ++i)
            {
                if (from <= *i && *i <= to)
                {
                    ++found_qty;
                    if (count != -1 && count == found_qty)
                    {
                        ++i;
                        break;
                    }
                    continue;
                }
                break;
            }

            if (check_count(found_qty))
            {
                size_t qty = distance(query->get_cursor()->get_string().begin() + pos, i);
                string result = query->get_cursor()->get_text().substr(pos, qty);
                set_out(result);
                TRACE_OUT << "found text = " << result TRACE_END;
                query->get_cursor()->inc(qty);
                return true;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Set::Set(const Query* q, const VectorString& p) : QueryBase(q), pattern(p)
    {
    }

    Set::Set(const Query* q, const VectorString& p, string* o) : pattern(p), QueryBase(q, -1, o)
    {
    }

    Set::Set(const Query* q, const VectorString& p, vector<chaiscript::Boxed_Value>* o) : pattern(p), QueryBase(q, -1, o)
    {
    }

    bool Set::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute())
        {
            for (const auto& p : pattern)
            {
                string text = query->get_cursor()->get_text().substr(query->get_cursor()->get_pos(), p.size());
                if (text == p)
                {
                    set_out(text);
                    TRACE_OUT << "found text = " << text TRACE_END;
                    query->get_cursor()->inc(p.size());
                    return true;
                }
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Endl::Endl(const  Query* q) : QueryBase(q)
    {
    }

    bool Endl::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute())
        {
            string text;
            text += query->get_cursor()->get_text().substr(query->get_cursor()->get_pos());//TODO

            if (text == ENDL)
            {
                TRACE_OUT << "found ENDL" TRACE_END;
                query->get_cursor()->inc(CHAR_SIZE);
                return true;
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Word::Word(const Query* q) : QueryBase(q)
    {
    }

    Word::Word(const Query* q, string* o) : QueryBase(q, -1, o)
    {
    }

    Word::Word(const Query* q, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, -1, o)
    {
    }

    bool Word::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute())
        {
            const auto& p = query->get_cursor()->get_pos();
            if (space_pattern.find(*(query->get_cursor()->get_string().begin() + p)) == string::npos)
            {
                Position p1 = (query->get_cursor()->get_string()).find_first_of(space_pattern, p);
                if (query->get_cursor()->eof(p1) == false)
                {
                    string result = query->get_cursor()->get_text().substr(p, (size_t)(p1 - p));
                    set_out(result);
                    TRACE_OUT << "found text = " << result TRACE_END;
                    query->get_cursor()->move_to(p1);
                    return true;
                }
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    RegEx::RegEx(const Query* q, const VectorString& rg) : QueryBase(q), rg_pattern(rg)
    {
    }
    RegEx::RegEx(const Query* q, const VectorString& rg, std::string* o) : QueryBase(q, -1, o), rg_pattern(rg)
    {
    }

    RegEx::RegEx(const Query* q, const VectorString& rg, vector<chaiscript::Boxed_Value>* o) : QueryBase(q, -1, o), rg_pattern(rg)
    {
    }

    tuple<string::const_iterator, string::const_iterator> RegEx::extract_string() const
    {
        string sp_pattern{ " \t\r\n\f" };

        auto begin = query->get_cursor()->get_string().begin() + (size_t)query->get_cursor()->get_pos();
        if (sp_pattern.find(*begin) != string::npos)
            return { query->get_cursor()->get_string().end(), query->get_cursor()->get_string().end() };//found space, skip

        auto  p = (query->get_cursor()->get_string()).find_first_of(sp_pattern, query->get_cursor()->get_pos() + 1);
        auto end = p == string::npos ? query->get_cursor()->get_string().end() : query->get_cursor()->get_string().begin() + p;

        return { begin, end };

    }

    bool RegEx::execute() const
    {

        TRACE_FUNC;
        if (QueryBase::execute())
        {
            string::const_iterator begin;
            string::const_iterator end;

            tie(begin, end) = extract_string();

            if (begin == query->get_cursor()->get_string().end())
                return false;

            smatch match;
            for (const auto& i : rg_pattern)
            {
                if (regex_match(begin, end, match, regex(i)))
                {
                    if (match.size())
                    {
                        set_out(match.str(0));
                        TRACE_OUT << "match = " << match.str(0) TRACE_END;
                        query->get_cursor()->inc(match.str(0).size());
                        return true;
                    }
                }
            }
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString number_rg_pattern
    {
        "^[+-]?([0-9]*\\.?[0-9]+|[0-9]+\\.?[0-9]*)([eE][+-]?[0-9]+)?$"//,
        //"^0x[0-9a-f]+"
    };
    Number::Number(const Query* q) : RegEx(q, number_rg_pattern)
    {
    }

    Number::Number(const Query* q, string* o) : RegEx(q, number_rg_pattern, o)
    {
    }

    Number::Number(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, number_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString hex_rg_pattern
    {
        "^0x[0-9a-f]+"
    };
    Hex::Hex(const Query* q) : RegEx(q, hex_rg_pattern)
    {
    }

    Hex::Hex(const Query* q, string* o) : RegEx(q, hex_rg_pattern, o)
    {
    }

    Hex::Hex(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, hex_rg_pattern, o)
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    //https://digitalfortress.tech/tricks/top-15-commonly-used-regex/
    const VectorString time_rg_pattern
    {
        "^((0?[1-9]|1[0-2]):[0-5][0-9])",
        "^((1[0-2]|0?[1-9]):([0-5][0-9]) ?([AaPp][Mm]))",
        "^((0[0-9]|1[0-9]|2[0-3]):[0-5][0-9])",
        "^(([0-9]|0[0-9]|1[0-9]|2[0-3]):[0-5][0-9])",
        "^(?:[01]\\d|2[0-3]):(?:[0-5]\\d):(?:[0-5])"
    };
    Time::Time(const Query* q) : RegEx(q, time_rg_pattern)
    {
    }

    Time::Time(const Query* q, string* o) : RegEx(q, time_rg_pattern, o)
    {
    }

    Time::Time(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, time_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString date_rg_pattern
    {
        "^([12]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01]))",
        "^(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1,3-9]|1[0-2])\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)0?2\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9])|(?:1[0-2]))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})",
        "^(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]|(?:Jan|Mar|May|Jul|Aug|Oct|Dec)))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1,3-9]|1[0-2]|(?:Jan|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec))\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)(?:0?2|(?:Feb))\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9]|(?:Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep))|(?:1[0-2]|(?:Oct|Nov|Dec)))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})"
    };
    Date::Date(const Query* q) : RegEx(q, date_rg_pattern)
    {
    }

    Date::Date(const Query* q, string* o) : RegEx(q, date_rg_pattern, o)
    {
    }

    Date::Date(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, date_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString url_rg_pattern
    {
        "(https?:\\/\\/)?(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)"
    };
    Url::Url(const Query* q) : RegEx(q, url_rg_pattern)
    {
    }

    Url::Url(const Query* q, string* o) : RegEx(q, url_rg_pattern, o)
    {
    }

    Url::Url(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, url_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString ipv4_rg_pattern
    {
        "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])"
    };
    IPv4::IPv4(const Query* q) : RegEx(q, ipv4_rg_pattern)
    {
    }

    IPv4::IPv4(const Query* q, string* o) : RegEx(q, ipv4_rg_pattern, o)
    {
    }

    IPv4::IPv4(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, ipv4_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString ipv6_rg_pattern
    {
        "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))"
    };
    IPv6::IPv6(const Query* q) : RegEx(q, ipv6_rg_pattern)
    {
    }

    IPv6::IPv6(const Query* q, string* o) : RegEx(q, ipv6_rg_pattern, o)
    {
    }

    IPv6::IPv6(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, ipv6_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString phone_rg_pattern
    {
        "^(?:(?:\\(?(?:00|\\+)([1-4]\\d\\d|[1-9]\\d?)\\)?)?[\\-\\.\\ \\\\\\/]?)?((?:\\(?\\d{1,}\\)?[\\-\\.\\ \\\\\\/]?){0,})(?:[\\-\\.\\ \\\\\\/]?(?:#|ext\\.?|extension|x)[\\-\\.\\ \\\\\\/]?(\\d+))?"
    };
    Phone::Phone(const Query* q) : RegEx(q, phone_rg_pattern)
    {
    }

    Phone::Phone(const Query* q, string* o) : RegEx(q, phone_rg_pattern, o)
    {
    }

    Phone::Phone(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, phone_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString path_rg_pattern
    {
        "^((\\/|\\\\|\\/\\/|https?:\\\\\\\\|https?:\\/\\/)[a-z0-9_@\\-^!#$%&+={}.\\/\\\\\\[\\]]+)+\\.[a-z]+",
        "^(.+)/([^/]+)",
        "^[\\w,\\s-]+\\.[A-Za-z]{3}"
    };
    Path::Path(const Query* q) : RegEx(q, path_rg_pattern)
    {
    }

    Path::Path(const Query* q, string* o) : RegEx(q, path_rg_pattern, o)
    {
    }

    Path::Path(const Query* q, vector<chaiscript::Boxed_Value>* o) : RegEx(q, path_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////

    Space::Space(const Query* q) : QueryBase(q)
    {
    }

    bool Space::execute() const
    {
        TRACE_FUNC;

        if (QueryBase::execute())
        {
            return skip_space();
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Group::Group(const Query* q, const vector<chaiscript::Boxed_Value>& v) :QueryBase(q, -1)
    {
        chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();
        for (auto p : v)
        {
            query_vector.push_back((QueryBase*)p.get_ptr());//TODO check pointer
        }
    }

    Group::Group(const Query* q, int c, const vector<chaiscript::Boxed_Value>& v) :QueryBase(q, c)
    {
        chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();
        for (auto p : v)
        {
            query_vector.push_back((QueryBase*)p.get_ptr());//TODO check pointer
        }
    }

    Group::Group(const Query* q, int c, bool s, const vector<chaiscript::Boxed_Value>& v) :QueryBase(q, c), shift(s)
    {
        chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();
        for (auto p : v)
        {
            query_vector.push_back((QueryBase*)p.get_ptr());//TODO check pointer
        }
    }

    bool Group::execute() const
    {
        TRACE_FUNC;
        if (QueryBase::execute() && query_vector.size())
        {
            int c = 0;
            do
            {
                bool flag = true;
                for (auto i : query_vector)
                {
                    if (i->execute() == false)
                    {
                        flag = false;
                        if (shift)
                            query->get_cursor()->inc(1);
                        break;
                    }
                }
                if (flag == true)
                    c += 1;

            } while (((count != -1 && c < count) || (shift == true && count == -1)) && query->get_cursor()->eof() == false);

            return (count == -1 && c > 0) || (count != -1 && c == count);
        }
        return false;
    }
//////////////////////////////////////////////////////////////////////////
    SkipSpace::SkipSpace(Query* q, bool skip) :QueryBase(q)
    {
        q->set_skip_space(skip);
    }

    bool SkipSpace::execute() const
    {
        return true;
    }
//////////////////////////////////////////////////////////////////////////
    ZeroOne::ZeroOne(const Query* q, chaiscript::Boxed_Value& _query) :QueryBase(q)//, query_to_execute(_query)
    {
        //chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();TODO
        //to fix the chaiscript behaviour. it destroys the object on the stack before calling the bool operator
        QueryBase* query_to_execute = (QueryBase*)_query.get_ptr();
        if (query_to_execute)
        {
            Position p = query->get_cursor()->get_pos();
            if (query_to_execute->execute() == false)
            {
                query->get_cursor()->move_to(p);
            }
        }
    }

    bool ZeroOne::execute() const
    {
        //if (query_to_execute.get_ptr()) 
        //{
        //    Position p = query->get_cursor()->get_pos();
        //    if (static_cast<QueryBase*>(query_to_execute.get_ptr())->execute() == false)
        //    {
        //        query->get_cursor()->move_to(p);
        //    }
        //}

        return true;
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


    m->add(chaiscript::constructor<Match(const Query*, const string&)>(), "Match");
    m->add(chaiscript::constructor<Match(const Query*, const string&, string*)>(), "Match");
    m->add(chaiscript::constructor<Match(const Query*, const string&, vector<chaiscript::Boxed_Value>*)>(), "Match");
    m->add(chaiscript::user_type<Match>(), "Match");
    m->add(chaiscript::base_class<QueryBase, Match>());
    m->add(chaiscript::type_conversion<Match, bool>([](const Match& q) { return q.execute(); }));

    m->add(chaiscript::constructor<AnyOf(const Query*, const string&)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(const Query*, const string&, int)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(const Query*, const string&, int, string*)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(const Query*, int, string*)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(const Query*, const string&, int, vector<chaiscript::Boxed_Value>*)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(const Query*, int, vector<chaiscript::Boxed_Value>*)>(), "AnyOf");
    m->add(chaiscript::user_type<AnyOf>(), "Any");
    m->add(chaiscript::base_class<QueryBase, AnyOf>());
    m->add(chaiscript::type_conversion<AnyOf, bool>([](const AnyOf& q) { return q.execute(); }));

    m->add(chaiscript::constructor<AnyNot(const Query*, const string&)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(const Query*, const string&, int)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(const Query*, const string&, int, string*)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(const Query*, int, string*)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(const Query*, const string&, int, vector<chaiscript::Boxed_Value>*)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(const Query*, int, vector<chaiscript::Boxed_Value>*)>(), "AnyNot");
    m->add(chaiscript::user_type<AnyNot>(), "AnyNot");
    m->add(chaiscript::base_class<QueryBase, AnyNot>());
    m->add(chaiscript::type_conversion<AnyNot, bool>([](const AnyNot& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Is(const Query*, const is_func)>(), "Is");
    m->add(chaiscript::constructor<Is(const Query*, const is_func, int)>(), "Is");
    m->add(chaiscript::constructor<Is(const Query*, const is_func, int, string*)>(), "Is");
    m->add(chaiscript::constructor<Is(const Query*, const is_func, int, vector<chaiscript::Boxed_Value>*)>(), "Is");
    m->add(chaiscript::user_type<Is>(), "Is");
    m->add(chaiscript::base_class<QueryBase, Is>());
    m->add(chaiscript::type_conversion<Is, bool>([](const Is& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Range(const Query*, char, char)>(), "Range");
    m->add(chaiscript::constructor<Range(const Query*, char, char, int)>(), "Range");
    m->add(chaiscript::constructor<Range(const Query*, char, char, int, string*)>(), "Range");
    m->add(chaiscript::constructor<Range(const Query*, char, char, int, vector<chaiscript::Boxed_Value>*)>(), "Range");
    m->add(chaiscript::user_type<Range>(), "Range");
    m->add(chaiscript::base_class<QueryBase, Range>());
    m->add(chaiscript::type_conversion<Range, bool>([](const Range& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Set(const Query*, const VectorString&)>(), "Set");
    m->add(chaiscript::constructor<Set(const Query*, const VectorString&, string*)>(), "Set");
    m->add(chaiscript::constructor<Set(const Query*, const VectorString&, vector<chaiscript::Boxed_Value>*)>(), "Set");
    m->add(chaiscript::user_type<Set>(), "Set");
    m->add(chaiscript::base_class<QueryBase, Set>());
    m->add(chaiscript::type_conversion<Set, bool>([](const Set& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Endl(const Query*)>(), "Endl");
    m->add(chaiscript::base_class<QueryBase, Endl>());
    m->add(chaiscript::type_conversion<Endl, bool>([](const Endl& q) {return q.execute(); }));

    m->add(chaiscript::constructor<Word(const Query*)>(), "Word");
    m->add(chaiscript::constructor<Word(const Query*, string*)>(), "Word");
    m->add(chaiscript::constructor<Word(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Word");
    m->add(chaiscript::user_type<Word>(), "Word");
    m->add(chaiscript::base_class<QueryBase, Word>());
    m->add(chaiscript::type_conversion<Word, bool>([](const Word& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Number(const Query*)>(), "Number");
    m->add(chaiscript::constructor<Number(const Query*, string*)>(), "Number");
    m->add(chaiscript::constructor<Number(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Number");
    m->add(chaiscript::user_type<Number>(), "Number");
    m->add(chaiscript::base_class<QueryBase, Number>());
    m->add(chaiscript::type_conversion<Number, bool>([](const Number& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Hex(const Query*)>(), "Hex");
    m->add(chaiscript::constructor<Hex(const Query*, string*)>(), "Hex");
    m->add(chaiscript::constructor<Hex(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Hex");
    m->add(chaiscript::user_type<Hex>(), "Hex");
    m->add(chaiscript::base_class<QueryBase, Hex>());
    m->add(chaiscript::type_conversion<Hex, bool>([](const Hex& q) { return q.execute(); }));

    m->add(chaiscript::constructor<RegEx(const Query*, const VectorString&)>(), "RegEx");
    m->add(chaiscript::constructor<RegEx(const Query*, const VectorString&, string*)>(), "RegEx");
    m->add(chaiscript::constructor<RegEx(const Query*, const VectorString&, vector<chaiscript::Boxed_Value>*)>(), "RegEx");
    m->add(chaiscript::user_type<RegEx>(), "RegEx");
    m->add(chaiscript::base_class<QueryBase, RegEx>());
    m->add(chaiscript::type_conversion<RegEx, bool>([](const RegEx& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Date(const Query*)>(), "Date");
    m->add(chaiscript::constructor<Date(const Query*, string*)>(), "Date");
    m->add(chaiscript::constructor<Date(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Date");
    m->add(chaiscript::user_type<Date>(), "Date");
    m->add(chaiscript::base_class<QueryBase, Date>());
    m->add(chaiscript::type_conversion<Date, bool>([](const Date& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Time(const Query*)>(), "Time");
    m->add(chaiscript::constructor<Time(const Query*, string*)>(), "Time");
    m->add(chaiscript::constructor<Time(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Time");
    m->add(chaiscript::user_type<Time>(), "Time");
    m->add(chaiscript::base_class<QueryBase, Time>());
    m->add(chaiscript::type_conversion<Time, bool>([](const Time& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Space(const Query*)>(), "Space");
    m->add(chaiscript::user_type<Space>(), "Space");
    m->add(chaiscript::base_class<QueryBase, Space>());
    m->add(chaiscript::type_conversion<Space, bool>([](const Space& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Group(const Query*, const vector<chaiscript::Boxed_Value>&)>(), "Group");
    m->add(chaiscript::constructor<Group(const Query*, int, const vector<chaiscript::Boxed_Value>&)>(), "Group");
    m->add(chaiscript::constructor<Group(const Query*, int, bool, const vector<chaiscript::Boxed_Value>&)>(), "Group");
    m->add(chaiscript::user_type<Group>(), "Count");
    m->add(chaiscript::base_class<QueryBase, Group>());
    m->add(chaiscript::type_conversion<Group, bool>([](const Group& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Url(const Query*)>(), "Url");
    m->add(chaiscript::constructor<Url(const Query*, string*)>(), "Url");
    m->add(chaiscript::constructor<Url(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Url");
    m->add(chaiscript::user_type<Url>(), "Url");
    m->add(chaiscript::base_class<QueryBase, Url>());
    m->add(chaiscript::type_conversion<Url, bool>([](const Url& q) { return q.execute(); }));

    m->add(chaiscript::constructor<IPv4(const Query*)>(), "IPv4");
    m->add(chaiscript::constructor<IPv4(const Query*, string*)>(), "IPv4");
    m->add(chaiscript::constructor<IPv4(const Query*, vector<chaiscript::Boxed_Value>*)>(), "IPv4");
    m->add(chaiscript::user_type<IPv4>(), "IPv4");
    m->add(chaiscript::base_class<QueryBase, IPv4>());
    m->add(chaiscript::type_conversion<IPv4, bool>([](const IPv4& q) { return q.execute(); }));

    m->add(chaiscript::constructor<IPv6(const Query*)>(), "IPv6");
    m->add(chaiscript::constructor<IPv6(const Query*, string*)>(), "IPv6");
    m->add(chaiscript::constructor<IPv6(const Query*, vector<chaiscript::Boxed_Value>*)>(), "IPv6");
    m->add(chaiscript::user_type<IPv4>(), "IPv6");
    m->add(chaiscript::base_class<QueryBase, IPv6>());
    m->add(chaiscript::type_conversion<IPv6, bool>([](const IPv6& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Phone(const Query*)>(), "Phone");
    m->add(chaiscript::constructor<Phone(const Query*, string*)>(), "Phone");
    m->add(chaiscript::constructor<Phone(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Phone");
    m->add(chaiscript::user_type<IPv4>(), "Phone");
    m->add(chaiscript::base_class<QueryBase, Phone>());
    m->add(chaiscript::type_conversion<Phone, bool>([](const Phone& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Path(const Query*)>(), "Path");
    m->add(chaiscript::constructor<Path(const Query*, string*)>(), "Path");
    m->add(chaiscript::constructor<Path(const Query*, vector<chaiscript::Boxed_Value>*)>(), "Path");
    m->add(chaiscript::user_type<IPv4>(), "Path");
    m->add(chaiscript::base_class<QueryBase, Path>());
    m->add(chaiscript::type_conversion<Path, bool>([](const Path& q) { return q.execute(); }));

    m->add(chaiscript::constructor<SkipSpace(Query*, bool)>(), "SkipSpace");
    m->add(chaiscript::user_type<SkipSpace>(), "SkipSpace");
    m->add(chaiscript::base_class<QueryBase, SkipSpace>());
    m->add(chaiscript::type_conversion<SkipSpace, bool>([](const SkipSpace& q) { return q.execute(); }));

    m->add(chaiscript::constructor<ZeroOne(Query*, chaiscript::Boxed_Value&)>(), "ZeroOne");
    m->add(chaiscript::user_type<ZeroOne>(), "ZeroOne");
    m->add(chaiscript::base_class<QueryBase, ZeroOne>());
    m->add(chaiscript::type_conversion<ZeroOne, bool>([](const ZeroOne& q) { return q.execute(); }));

    chaiscript::bootstrap::standard_library::vector_type<std::vector<VectorQuery> >("VectorQuery", *m);
    
    END_DECLARE(QUERY)
}