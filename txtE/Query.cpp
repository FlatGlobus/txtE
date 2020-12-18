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
    QData::QData()
    {
        next_data();
    }

    QData::~QData()
    {

    }

    void QData::set(const std::string& key, const std::string& val)
    {
        //TRACE_FUNC;
        if (key.empty())
            throw std::runtime_error("Query::set: key is empty");

        (data_vector.begin() + current_data)->insert_or_assign(key, val);
    }

    const string& QData::get(const std::string& key) const
    {
        //TRACE_FUNC;
        auto val = (data_vector.begin() + current_data)->find(key);
        if (val != (data_vector.begin() + current_data)->end())
            return val->second;

        throw std::runtime_error("Query::get: no value for " + key + " key");
    }

    VectorString QData::get_vector(const std::string& key) const
    {
        //TRACE_FUNC;
        VectorString vs;

        for (auto& m : data_vector)
        {
            auto val = m.find(key);
            if (val != m.end())
                vs.push_back(val->second);
        }
        if (vs.empty())
            throw std::runtime_error("Query::get_vector: no value for " + key + " key");
        return vs;
    }

    bool QData::exists(const std::string& key) const
    {
        auto val = (data_vector.begin() + current_data)->find(key);
        if (val != (data_vector.begin() + current_data)->end())
            return true;

        for (auto& m : data_vector)
        {
            if (m.find(key) != m.end())
                return true;
        }

        return false;
    }

    void QData::reset_all_data()
    {
        //TRACE_FUNC;
        if (freeze == false)
        {
            data_vector.clear();
            next_data();
        }
    }

    void QData::reset_data(const std::string& key)
    {
        //TRACE_FUNC;
        if (key.size() && freeze == false)
            (data_vector.begin() + current_data)->erase(key);
    }

    void QData::freeze_data(bool val)
    {
        freeze = val;
    }

    void QData::next_data()
    {
        //TRACE_FUNC;
        data_vector.push_back(qdata_map());
        current_data = data_vector.size() - 1;
    }

    void QData::set_current(size_t idx)
    {
        //TRACE_FUNC;
        if (idx < data_vector.size())
            current_data = idx;
    }
    //////////////////////////////////////////////////////////////////////////
    QueryBase::QueryBase()
    {
    }

    QueryBase::QueryBase(Query* q) :query(q)
    {
    }

    QueryBase::QueryBase(Query* q, int c) : query(q), count(c)
    {
    }

    QueryBase::QueryBase(Query* q, int c, const std::string& o) : query(q), count(c), key(o)
    {
        reset();
    }

    QueryBase::QueryBase(Query* q, const std::string& o) : query(q), key(o)
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

    void QueryBase::set_out(const std::string& s) const
    {
        if (query && key.empty() == false)
            query->set(key, s);
    }

    void QueryBase::reset()
    {
        if (query && key.empty() == false)
            query->reset_data(key);
    }

    bool QueryBase::check_count(int c) const
    {
        return (count == -1 && c > 0) || (count != -1 && c == count);
    }

    bool QueryBase::skip_space() const
    {
        const string& text = query->get_cursor()->get_text();
        size_t p = query->get_cursor()->get_pos();

        if (p < text.size() &&  space_pattern.find(*(text.begin() + p)) != string::npos)
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

    void Query::reset_last_data()
    {
        reset_data(key);
    }
    //////////////////////////////////////////////////////////////////////////
    Match::Match(Query* q, const string& p) : QueryBase(q), pattern(p)
    {
    }

    Match::Match(Query* q, const string& p, const std::string& o) : QueryBase(q, o), pattern(p)
    {
    }

    bool Match::execute() const
    {
        TRACE_FUNC;
        TRACE_OUT << "pattern = " << pattern TRACE_END;
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    AnyOf::AnyOf(Query* q, const string& p) : QueryBase(q), pattern(p)
    {
    }

    AnyOf::AnyOf(Query* q, const string& p, int c) : QueryBase(q, c), pattern(p)
    {

    }

    AnyOf::AnyOf(Query* q, const string& p, int c, const std::string& o) : QueryBase(q, c, o), pattern(p)
    {
    }

    AnyOf::AnyOf(Query* q, const string& p, const std::string& o) : QueryBase(q, o), pattern(p)
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    AnyNot::AnyNot(Query* q, const string& p) : QueryBase(q), pattern(p)
    {
    }

    AnyNot::AnyNot(Query* q, const string& p, int c) : QueryBase(q, c), pattern(p)
    {

    }

    AnyNot::AnyNot(Query* q, const string& p, int c, const std::string& o) : QueryBase(q, c, o), pattern(p)
    {
    }

    AnyNot::AnyNot(Query* q, const string& p, const std::string& o) : QueryBase(q, o), pattern(p)
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
                        //++i;
                        break;
                    }
                    continue;
                }
                else
                {
                    //++i;
                    break;
                }
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Is::Is(Query* q, const is_func f) : QueryBase(q), func(f)
    {
    }

    Is::Is(Query* q, const is_func f, int c) : func(f), QueryBase(q, c)
    {
    }

    Is::Is(Query* q, const is_func f, int c, const std::string& o) : func(f), QueryBase(q, c, o)
    {
    }

    Is::Is(Query* q, const is_func f, const std::string& o) : func(f), QueryBase(q, o)
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Range::Range(Query* q, char f, char t) : QueryBase(q), from(f), to(t)
    {
    }

    Range::Range(Query* q, char f, char t, int c) : from(f), to(t), QueryBase(q, c)
    {
    }

    Range::Range(Query* q, char f, char t, int c, const std::string& o) : from(f), to(t), QueryBase(q, c, o)
    {
    }

    Range::Range(Query* q, char f, char t, const std::string& o) : from(f), to(t), QueryBase(q, o)
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Set::Set(Query* q, const VectorString& p) : QueryBase(q), pattern(p)
    {
    }

    Set::Set(Query* q, const VectorString& p, const std::string& o) : pattern(p), QueryBase(q, o)
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Endl::Endl(Query* q) : QueryBase(q)
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Word::Word(Query* q) : QueryBase(q)
    {
    }

    Word::Word(Query* q, const std::string& o) : QueryBase(q, o)
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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    CStr::CStr(Query* q) : QueryBase(q)
    {
    }

    CStr::CStr(Query* q, const std::string& o) : QueryBase(q, o)
    {
    }

    bool CStr::execute() const
    {
        string str_pattern = "\"";
        TRACE_FUNC;
        if (QueryBase::execute())
        {
            Position p = query->get_cursor()->get_pos();
            if (str_pattern.find(*(query->get_cursor()->get_string().begin() + p)) != string::npos)
            {
                Position p1(p);
                p1 += 1;
                do 
                {
                    p1 = (query->get_cursor()->get_string()).find_first_of(str_pattern, p1);
                    if (p1.eof() == false && query->get_cursor()->get_string()[p1 - 1] != '\\')
                    {
                        p1 += 1;
                        break;
                    }
                } while (++p1 && p1.eof() == false);

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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    RegEx::RegEx(Query* q, const VectorString& rg) : QueryBase(q), rg_pattern(rg)
    {
    }
    RegEx::RegEx(Query* q, const VectorString& rg, const std::string& o) : QueryBase(q, o), rg_pattern(rg)
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
            {
                query->reset_last_data();
                return false;
            }

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
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString number_rg_pattern
    {
        "^[+-]?([0-9]*\\.?[0-9]+|[0-9]+\\.?[0-9]*)([eE][+-]?[0-9]+)?$"//,
        //"^0x[0-9a-f]+"
    };
    Number::Number(Query* q) : RegEx(q, number_rg_pattern)
    {
    }

    Number::Number(Query* q, const std::string& o) : RegEx(q, number_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString hex_rg_pattern
    {
        "^0x[0-9a-f]+"
    };
    Hex::Hex(Query* q) : RegEx(q, hex_rg_pattern)
    {
    }

    Hex::Hex(Query* q, const std::string& o) : RegEx(q, hex_rg_pattern, o)
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
    Time::Time(Query* q) : RegEx(q, time_rg_pattern)
    {
    }

    Time::Time(Query* q, const std::string& o) : RegEx(q, time_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString date_rg_pattern
    {
        "^([12]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01]))",
        "^(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1,3-9]|1[0-2])\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)0?2\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9])|(?:1[0-2]))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})",
        "^(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]|(?:Jan|Mar|May|Jul|Aug|Oct|Dec)))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1,3-9]|1[0-2]|(?:Jan|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec))\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)(?:0?2|(?:Feb))\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9]|(?:Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep))|(?:1[0-2]|(?:Oct|Nov|Dec)))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})"
    };
    Date::Date(Query* q) : RegEx(q, date_rg_pattern)
    {
    }

    Date::Date(Query* q, const std::string& o) : RegEx(q, date_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString url_rg_pattern
    {
        "(https?:\\/\\/)?(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)"
    };
    Url::Url(Query* q) : RegEx(q, url_rg_pattern)
    {
    }

    Url::Url(Query* q, const std::string& o) : RegEx(q, url_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString ipv4_rg_pattern
    {
        "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])"
    };
    IPv4::IPv4(Query* q) : RegEx(q, ipv4_rg_pattern)
    {
    }

    IPv4::IPv4(Query* q, const std::string& o) : RegEx(q, ipv4_rg_pattern, o)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    const VectorString ipv6_rg_pattern
    {
        "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))"
    };
    IPv6::IPv6(Query* q) : RegEx(q, ipv6_rg_pattern)
    {
    }

    IPv6::IPv6(Query* q, const std::string& o) : RegEx(q, ipv6_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString phone_rg_pattern
    {
        "^(?:(?:\\(?(?:00|\\+)([1-4]\\d\\d|[1-9]\\d?)\\)?)?[\\-\\.\\ \\\\\\/]?)?((?:\\(?\\d{1,}\\)?[\\-\\.\\ \\\\\\/]?){0,})(?:[\\-\\.\\ \\\\\\/]?(?:#|ext\\.?|extension|x)[\\-\\.\\ \\\\\\/]?(\\d+))?"
    };
    Phone::Phone(Query* q) : RegEx(q, phone_rg_pattern)
    {
    }

    Phone::Phone(Query* q, const std::string& o) : RegEx(q, phone_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorString path_rg_pattern
    {
        "^((\\/|\\\\|\\/\\/|https?:\\\\\\\\|https?:\\/\\/)[a-z0-9_@\\-^!#$%&+={}.\\/\\\\\\[\\]]+)+\\.[a-z]+",
        "^(.+)/([^/]+)",
        "^[\\w,\\s-]+\\.[A-Za-z]{3}"
    };
    Path::Path(Query* q) : RegEx(q, path_rg_pattern)
    {
    }

    Path::Path(Query* q, const std::string& o) : RegEx(q, path_rg_pattern, o)
    {
    }
    //////////////////////////////////////////////////////////////////////////

    Space::Space(Query* q) : QueryBase(q)
    {
    }

    bool Space::execute() const
    {
        TRACE_FUNC;

        if (QueryBase::execute())
        {
            return skip_space();
        }
        query->reset_last_data();
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Group::Group(Query* q, const vector<chaiscript::Boxed_Value>& v) :QueryBase(q, -1)
    {
        chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();
        for (auto p : v)
        {
            query_vector.push_back((QueryBase*)p.get_ptr());//TODO check pointer
        }
    }

    Group::Group(Query* q, int c, const vector<chaiscript::Boxed_Value>& v) :QueryBase(q, c)
    {
        chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();
        for (auto p : v)
        {
            query_vector.push_back((QueryBase*)p.get_ptr());//TODO check pointer
        }
    }

    Group::Group(Query* q, int c, bool s, const vector<chaiscript::Boxed_Value>& v) :QueryBase(q, c), shift(s)
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
        query->reset_last_data();
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
    ZeroOne::ZeroOne(Query* q, chaiscript::Boxed_Value& _query) :QueryBase(q)//, query_to_execute(_query)
    {
        TRACE_FUNC;
        //chaiscript::Type_Info ut = chaiscript::user_type<QueryBase>();TODO
        //to fix the chaiscript behaviour. it destroys the object on the stack before calling the bool operator
        QueryBase* query_to_execute = (QueryBase*)_query.get_ptr();
        if (query_to_execute)
        {
            Position p = query->get_cursor()->get_pos();
            query->freeze_data(true);
            if (query_to_execute->execute() == false)
            {
                query->get_cursor()->move_to(p);
            }
            query->freeze_data(false);
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
    NextData::NextData(Query* q) :QueryBase(q)
    {
    }

    bool NextData::execute() const
    {
        TRACE_FUNC;
        query->next_data();
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MODULE(QUERY)

    m->add(chaiscript::user_type<QData>(), "QData");
    m->add(chaiscript::user_type<QueryBase>(), "QueryBase");

    m->add(chaiscript::constructor<Query(Cursor*)>(), "Query");
    m->add(chaiscript::constructor<Query(Cursor*, bool)>(), "Query");
    m->add(chaiscript::base_class<QueryBase, Query>());
    m->add(chaiscript::base_class<QData, Query>());
    m->add(chaiscript::type_conversion<Query, bool>([](const Query& q) {return q.execute(); }));
    m->add(chaiscript::fun(static_cast<void(QData::*)(const string&, const string&)>(&QData::set)), "set");
    m->add(chaiscript::fun(static_cast<const string& (QData::*)(const string&) const>(&QData::get)), "get");
    m->add(chaiscript::fun(static_cast<VectorString(QData::*)(const string&) const>(&QData::get_vector)), "get_vector");
    m->add(chaiscript::fun(static_cast<bool (QData::*)(const string&) const>(&QData::exists)), "exists");
    m->add(chaiscript::fun(static_cast<void(QData::*)()>(&QData::reset_all_data)), "reset");
    m->add(chaiscript::fun(static_cast<void(QData::*)(const string&)>(&QData::reset_data)), "reset");
    m->add(chaiscript::fun(static_cast<size_t(QData::*)() const>(&QData::size)), "size");
    m->add(chaiscript::fun(static_cast<void(QData::*)(size_t)>(&QData::set_current)), "set_current");

    m->add(chaiscript::constructor<Match(Query*, const string&)>(), "Match");
    m->add(chaiscript::constructor<Match(Query*, const string&, const std::string&)>(), "Match");
    m->add(chaiscript::user_type<Match>(), "Match");
    m->add(chaiscript::base_class<QueryBase, Match>());
    m->add(chaiscript::type_conversion<Match, bool>([](const Match& q) { return q.execute(); }));

    m->add(chaiscript::constructor<AnyOf(Query*, const string&)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(Query*, const string&, int)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(Query*, const string&, int, const std::string&)>(), "AnyOf");
    m->add(chaiscript::constructor<AnyOf(Query*, const string&, const std::string&)>(), "AnyOf");
    m->add(chaiscript::user_type<AnyOf>(), "Any");
    m->add(chaiscript::base_class<QueryBase, AnyOf>());
    m->add(chaiscript::type_conversion<AnyOf, bool>([](const AnyOf& q) { return q.execute(); }));

    m->add(chaiscript::constructor<AnyNot(Query*, const string&)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(Query*, const string&, int)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(Query*, const string&, int, const std::string&)>(), "AnyNot");
    m->add(chaiscript::constructor<AnyNot(Query*, const string&, const std::string&)>(), "AnyNot");
    m->add(chaiscript::user_type<AnyNot>(), "AnyNot");
    m->add(chaiscript::base_class<QueryBase, AnyNot>());
    m->add(chaiscript::type_conversion<AnyNot, bool>([](const AnyNot& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Is(Query*, const is_func)>(), "Is");
    m->add(chaiscript::constructor<Is(Query*, const is_func, int)>(), "Is");
    m->add(chaiscript::constructor<Is(Query*, const is_func, int, const std::string&)>(), "Is");
    m->add(chaiscript::constructor<Is(Query*, const is_func, const std::string&)>(), "Is");
    m->add(chaiscript::user_type<Is>(), "Is");
    m->add(chaiscript::base_class<QueryBase, Is>());
    m->add(chaiscript::type_conversion<Is, bool>([](const Is& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Range(Query*, char, char)>(), "Range");
    m->add(chaiscript::constructor<Range(Query*, char, char, int)>(), "Range");
    m->add(chaiscript::constructor<Range(Query*, char, char, int, const std::string&)>(), "Range");
    m->add(chaiscript::constructor<Range(Query*, char, char, const std::string&)>(), "Range");
    m->add(chaiscript::user_type<Range>(), "Range");
    m->add(chaiscript::base_class<QueryBase, Range>());
    m->add(chaiscript::type_conversion<Range, bool>([](const Range& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Set(Query*, const VectorString&)>(), "Set");
    m->add(chaiscript::constructor<Set(Query*, const VectorString&, const std::string&)>(), "Set");
    m->add(chaiscript::user_type<Set>(), "Set");
    m->add(chaiscript::base_class<QueryBase, Set>());
    m->add(chaiscript::type_conversion<Set, bool>([](const Set& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Endl(Query*)>(), "Endl");
    m->add(chaiscript::base_class<QueryBase, Endl>());
    m->add(chaiscript::type_conversion<Endl, bool>([](const Endl& q) {return q.execute(); }));

    m->add(chaiscript::constructor<Word(Query*)>(), "Word");
    m->add(chaiscript::constructor<Word(Query*, const std::string&)>(), "Word");
    m->add(chaiscript::user_type<Word>(), "Word");
    m->add(chaiscript::base_class<QueryBase, Word>());
    m->add(chaiscript::type_conversion<Word, bool>([](const Word& q) { return q.execute(); }));

    m->add(chaiscript::constructor<CStr(Query*)>(), "CStr");
    m->add(chaiscript::constructor<CStr(Query*, const std::string&)>(), "CStr");
    m->add(chaiscript::user_type<CStr>(), "CStr");
    m->add(chaiscript::base_class<QueryBase, CStr>());
    m->add(chaiscript::type_conversion<CStr, bool>([](const CStr& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Number(Query*)>(), "Number");
    m->add(chaiscript::constructor<Number(Query*, const std::string&)>(), "Number");
    m->add(chaiscript::user_type<Number>(), "Number");
    m->add(chaiscript::base_class<QueryBase, Number>());
    m->add(chaiscript::type_conversion<Number, bool>([](const Number& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Hex(Query*)>(), "Hex");
    m->add(chaiscript::constructor<Hex(Query*, const std::string&)>(), "Hex");
    m->add(chaiscript::user_type<Hex>(), "Hex");
    m->add(chaiscript::base_class<QueryBase, Hex>());
    m->add(chaiscript::type_conversion<Hex, bool>([](const Hex& q) { return q.execute(); }));

    m->add(chaiscript::constructor<RegEx(Query*, const VectorString&)>(), "RegEx");
    m->add(chaiscript::constructor<RegEx(Query*, const VectorString&, const std::string&)>(), "RegEx");
    m->add(chaiscript::user_type<RegEx>(), "RegEx");
    m->add(chaiscript::base_class<QueryBase, RegEx>());
    m->add(chaiscript::type_conversion<RegEx, bool>([](const RegEx& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Date(Query*)>(), "Date");
    m->add(chaiscript::constructor<Date(Query*, const std::string&)>(), "Date");
    m->add(chaiscript::user_type<Date>(), "Date");
    m->add(chaiscript::base_class<QueryBase, Date>());
    m->add(chaiscript::type_conversion<Date, bool>([](const Date& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Time(Query*)>(), "Time");
    m->add(chaiscript::constructor<Time(Query*, const std::string&)>(), "Time");
    m->add(chaiscript::user_type<Time>(), "Time");
    m->add(chaiscript::base_class<QueryBase, Time>());
    m->add(chaiscript::type_conversion<Time, bool>([](const Time& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Space(Query*)>(), "Space");
    m->add(chaiscript::user_type<Space>(), "Space");
    m->add(chaiscript::base_class<QueryBase, Space>());
    m->add(chaiscript::type_conversion<Space, bool>([](const Space& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Group(Query*, const vector<chaiscript::Boxed_Value>&)>(), "Group");
    m->add(chaiscript::constructor<Group(Query*, int, const vector<chaiscript::Boxed_Value>&)>(), "Group");
    m->add(chaiscript::constructor<Group(Query*, int, bool, const vector<chaiscript::Boxed_Value>&)>(), "Group");
    m->add(chaiscript::user_type<Group>(), "Count");
    m->add(chaiscript::base_class<QueryBase, Group>());
    m->add(chaiscript::type_conversion<Group, bool>([](const Group& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Url(Query*)>(), "Url");
    m->add(chaiscript::constructor<Url(Query*, const std::string&)>(), "Url");
    m->add(chaiscript::user_type<Url>(), "Url");
    m->add(chaiscript::base_class<QueryBase, Url>());
    m->add(chaiscript::type_conversion<Url, bool>([](const Url& q) { return q.execute(); }));

    m->add(chaiscript::constructor<IPv4(Query*)>(), "IPv4");
    m->add(chaiscript::constructor<IPv4(Query*, const std::string&)>(), "IPv4");
    m->add(chaiscript::user_type<IPv4>(), "IPv4");
    m->add(chaiscript::base_class<QueryBase, IPv4>());
    m->add(chaiscript::type_conversion<IPv4, bool>([](const IPv4& q) { return q.execute(); }));

    m->add(chaiscript::constructor<IPv6(Query*)>(), "IPv6");
    m->add(chaiscript::constructor<IPv6(Query*, const std::string&)>(), "IPv6");
    m->add(chaiscript::user_type<IPv6>(), "IPv6");
    m->add(chaiscript::base_class<QueryBase, IPv6>());
    m->add(chaiscript::type_conversion<IPv6, bool>([](const IPv6& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Phone(Query*)>(), "Phone");
    m->add(chaiscript::constructor<Phone(Query*, const std::string&)>(), "Phone");
    m->add(chaiscript::user_type<Phone>(), "Phone");
    m->add(chaiscript::base_class<QueryBase, Phone>());
    m->add(chaiscript::type_conversion<Phone, bool>([](const Phone& q) { return q.execute(); }));

    m->add(chaiscript::constructor<Path(Query*)>(), "Path");
    m->add(chaiscript::constructor<Path(Query*, const std::string&)>(), "Path");
    m->add(chaiscript::user_type<Path>(), "Path");
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

    m->add(chaiscript::constructor<NextData(Query*)>(), "NextData");
    m->add(chaiscript::user_type<NextData>(), "NextData");
    m->add(chaiscript::base_class<QueryBase, NextData>());
    m->add(chaiscript::type_conversion<NextData, bool>([](const NextData& q) { return q.execute(); }));

    chaiscript::bootstrap::standard_library::vector_type<std::vector<VectorQuery> >("VectorQuery", *m);

    END_DECLARE(QUERY)
}