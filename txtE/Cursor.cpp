#include "Cursor.h"
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <chaiscript/chaiscript.hpp>
#include "text.h"
#include <memory>
#include <limits.h>
#include"util.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
Cursor::Cursor(Text& t) :text(t.text)
{
}

Cursor::Cursor(string& t) : text(t)
{
}

Cursor::Cursor(const Cursor& c) : text(c.text), pos(c.pos), min_(c.min_), max_(c.max_)
{
}

Cursor::Cursor(Cursor& c) : text(c.text), pos(c.pos), min_(c.min_), max_(c.max_)
{
}

Cursor::Cursor(Text& t, const string& pattern, find_func func) : text(t.text)
{
    move_to(pattern, func);
}

Cursor::Cursor(string& t, const string& pattern, find_func func) : text(t)
{
    move_to(pattern, func);
}

Cursor::Cursor(const Cursor& c, const string& pattern, find_func func) : text(c.text), pos(c.pos), min_(c.min_), max_(c.max_)
{
    move_to(pattern, func);
}

Cursor::Cursor(Cursor& c, const string& pattern, find_func func) : text(c.text), pos(c.pos), min_(c.min_), max_(c.max_)
{
    move_to(pattern, func);
}

size_t Cursor::rfind_first_of(const string& pattern, size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "pattern = " << pattern << "\nposition = " << p TRACE_END;

    for (size_t i = p; i > 0; i--)
    {
        if (pattern.find(text[i]) != string::npos)
        {
            i += 1;
            TRACE_POS(i);
            return i;
        }
    }

    TRACE_POS(string::npos);
    return string::npos;
}

void Cursor::check_cursor(const Cursor& c)
{
    if (AreEqual(c.text, text) == false)
    {
        throw runtime_error("Cursor object : \"" + c.get_name() + "\" is created for other Text object.");
    }
}

size_t Cursor::multi_find(const std::vector<chaiscript::Boxed_Value>& pattern, find_func func)
{
    size_t ret_pos = string::npos;
    chaiscript::Type_Info ut = chaiscript::user_type<string>();

    TRACE_FUNC;
    for (auto p : pattern)
    {
        if (p.is_type(ut) == false)
        {
            continue;
        }
        TRACE_OUT << "pattern = " << chaiscript::boxed_cast<std::string>(p) TRACE_END;
        ret_pos = func(text, chaiscript::boxed_cast<std::string>(p), pos);
        if (ret_pos != string::npos)
        {
            TRACE_POS(ret_pos);
            break;
        }
    }
    return ret_pos;
}

int Cursor::find_count(const std::string& pattern, size_t from_pos, size_t until_pos, find_func func)
{
    TRACE_FUNC;
    int count = 0;
    for (; check_range(from_pos) == true;)
    {
        from_pos = func(text, pattern, from_pos);

        if (check_range(from_pos) == true && from_pos <= until_pos)
        {
            count++;
            from_pos += 1;
            continue;
        }
        break;
    }
    return count;
}


Cursor& Cursor::inc(size_t p)
{
    pos += p;

    TRACE_FUNC;
    TRACE_OUT << "increment = " << p TRACE_END;
    TRACE_POS(pos);

    check_range();

    return *this;
}

Cursor& Cursor::operator += (size_t p)
{
    pos += p;
    TRACE_FUNC;
    TRACE_OUT << "increment = " << p TRACE_END;
    TRACE_POS(pos);

    check_range();

    return *this;
}

size_t Cursor::operator + (size_t p)
{
    size_t sum = pos + p;

    TRACE_FUNC;
    TRACE_OUT << "increment = " << p TRACE_END;
    TRACE_OUT << "sum = " << sum TRACE_END;
    TRACE_POS(sum);

    return sum;
}

Cursor& Cursor::operator ++ ()
{
    pos++;

    TRACE_FUNC;
    TRACE_POS(pos);

    check_range();
    return *this;
}

Cursor& Cursor::operator -= (size_t p)
{
    pos -= p;

    TRACE_FUNC;
    TRACE_OUT << "decrement = " << p TRACE_END;
    TRACE_POS(pos);

    check_range();
    return *this;
}

size_t Cursor::operator - (size_t p)
{
    size_t dec = pos - p;

    TRACE_FUNC;
    TRACE_OUT << "decrement = " << p TRACE_END;
    TRACE_POS(pos);

    return dec;
}

Cursor& Cursor::operator -- ()
{
    pos--;

    TRACE_FUNC;
    TRACE_POS(pos);

    check_range();
    return *this;
}

bool Cursor::operator == (size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "comparison result = " << (pos == p ? "true" : "false") TRACE_END;

    return pos == p;
}

bool Cursor::operator != (size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "comparison result = " << (pos != p ? "true" : "false") TRACE_END;

    return pos != p;
}

Cursor& Cursor::operator = (const Cursor& c)
{
    TRACE_FUNC;
    check_cursor(c);

    pos = c.pos;
    text = c.text;
    min_ = c.min_;
    max_ = c.max_;
    positions = c.positions;

    return *this;
}

Cursor& Cursor::operator = (size_t p)
{
    pos = p;

    TRACE_FUNC;
    TRACE_OUT << "set position = " << p TRACE_END;
    TRACE_POS(pos);

    check_range();

    return *this;
}

bool Cursor::operator < (size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "comparison result = " << (pos < p ? "true" : "false") TRACE_END;

    return pos < p;
}

bool Cursor::operator <= (size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "comparison result = " << (pos <= p ? "true" : "false") TRACE_END;
    return pos <= p;
}

bool Cursor::operator >= (size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "comparison result = " << (pos >= p ? "true" : "false") TRACE_END;
    return pos >= p;
}

bool Cursor::operator > (size_t p)
{
    TRACE_FUNC;
    TRACE_OUT << "comparison result = " << (pos > p ? "true" : "false") TRACE_END;

    return pos > p;
}

Cursor& Cursor::move_to(size_t p)
{
    pos = p;

    TRACE_FUNC;
    TRACE_POS(pos);

    check_range();
    return *this;
}

Cursor& Cursor::move_to(const string& pattern, find_func func)
{
    TRACE_FUNC;
    if (!func)
    {
        throw runtime_error("Find function in move_to is NULL.");
    }
    pos = func(text, pattern, pos);
    
    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    check_range();
    return *this;
}

Cursor& Cursor::move_to(const std::vector<chaiscript::Boxed_Value>& pattern, find_func func)
{
    TRACE_FUNC;
    if (!func)
    {
        throw runtime_error("Cursor::move_to Find function in move_to is NULL.");
    }
    pos = multi_find(pattern, func);

    TRACE_POS(pos);

    check_range();

    return *this;
}

// pattern1 = {, pattern2 =}, moves pos to } ; from here ->{ {abcd} ->}<- to here }
Cursor& Cursor::move_to(const string& pattern1, const string& pattern2, find_func func)
{
    TRACE_FUNC;

    if (!func)
    {
        throw runtime_error("Find function in move_to is NULL.");
    }

    TRACE_OUT << "pattern1 = " << pattern1 TRACE_END;
    TRACE_OUT << "pattern2 = " << pattern2 TRACE_END;

    size_t pos_orig = pos;
    size_t pos1, pos2;
    int count = 1;

    set_eof();
    pos1 = func(text, pattern1, pos_orig);//{
    pos1 += 1;
    if (check_range(pos1) == false)
    {
        return *this;
    }

    for (; count > 0 && check_range(pos2) == true; )
    {
        pos2 = func(text, pattern2, pos1); //}
        if (check_range(pos2) == false)
        {
            break;
        }
        count -= 1;
        count += find_count(pattern1, pos1, pos2, func);// count "{" between "{ }"
        if (count == 0)
        {
            break;
        }
        pos1 = pos2 + 1;
    }
    if (count == 0)
    {
        pos = pos2;
        check_range();
    }

    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::move_while(const string& pattern)
{
    TRACE_FUNC;

    for (; check_range(); pos++)
    {
        if (pattern.find(text[pos]) != string::npos)
        {
            break;
        }
    }

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::move_until(const string& pattern)
{
    TRACE_FUNC;

    for (; check_range(); pos++)
    {
        if (pattern.find(text[pos]) == string::npos)
        {
            break;
        }
    }

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::move_to_end(const string& pattern, find_func func)
{
    TRACE_FUNC;

    if (!func)
    {
        throw runtime_error("Cursor::move_to_end Find function is NULL.");
    }
    pos = func(text, pattern, pos);
    if (is_eof() == false)
        pos += pattern.size();

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    check_range();

    return *this;
}

Cursor& Cursor::move_to_end(const std::vector<chaiscript::Boxed_Value>& pattern, find_func func)
{
    TRACE_FUNC;

    if (!func)
    {
        throw runtime_error("Cursor::move_to_end Find function is NULL.");
    }

    pos = multi_find(pattern, func);
    if (is_eof() == false)
        pos += pattern.size();
    TRACE_POS(pos);

    check_range();

    return *this;
}

Cursor& Cursor::next_word(const string& pattern)
{
    TRACE_FUNC;

    pos = text.find_first_of(pattern, pos);
    pos = text.find_first_not_of(pattern, pos);

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    check_range();
    return *this;
}

Cursor& Cursor::next_word()
{
    return next_word(space_pattern);
}

Cursor& Cursor::move_to_begin_of_word(const string& pattern)
{
    TRACE_FUNC;

    if (pattern.find_first_of(text[pos]) == string::npos) //pos должна быть в слове, иначе не понятно в каком слове искать начало те не паттерн
    {
        pos = rfind_first_of(pattern, pos);
    }
    else
    {
        pos = string::npos;
    }

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    check_range();
    return *this;
}

Cursor& Cursor::move_to_begin_of_word()
{
    return move_to_begin_of_word(space_pattern);
}

Cursor& Cursor::move_to_end_of_word(const string& pattern)
{
    TRACE_FUNC;
    TRACE_OUT << "pattern = " << pattern TRACE_END;

    if (pattern.find_first_of(text[pos]) == string::npos)
    {
        pos = text.find_first_of(space_pattern, pos);
        TRACE_POS(pos);
        check_range();
        return *this;
    }

    pos = string::npos;
    TRACE_POS(pos);
    return *this;
}

Cursor& Cursor::move_to_end_of_word()
{
    return move_to_end_of_word(space_pattern);
}

Cursor& Cursor::next_line()
{
    TRACE_FUNC;
    size_t p = text.find(ENDL, pos);
    if (is_eof(p += ENDL_SIZE) == false)
    {
        pos = p;

        TRACE_POS(pos);
        check_range();
        return *this;
    }
    pos = string::npos;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::next_line(size_t count)
{
    if (count < 0)
    {
        throw runtime_error("Cursor::next_line, argument cannot be less zero.");
    }

    for (size_t i = 0; i < count || is_eof() == false; i++)
    {
        next_line();
    }

    check_range();
    return *this;
}

Cursor& Cursor::prev_line()
{
    TRACE_FUNC;
    size_t p = text.rfind(ENDL, pos);
    if (p != string::npos)
    {
        pos = p;
        TRACE_POS(pos);
        check_range();
        return *this;
    }

    if (p > 1)
    {
        pos = p - ENDL_SIZE;
        TRACE_POS(pos);

        check_range();
        return *this;
    }

    pos = string::npos;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::prev_line(size_t count)
{
    if (count < 0)
    {
        throw runtime_error("Cursor::prev_line, argument cannot be less zero.");
    }

    for (size_t i = 0; i < count || is_eof() == false; i++)
    {
        prev_line();
    }
    check_range();
    return *this;
}

void Cursor::move_to_begin_of_line()
{
    TRACE_FUNC;

    pos = text.rfind(ENDL, pos);
    TRACE_POS(pos);
    check_range();
}

void Cursor::move_to_end_of_line()
{
    TRACE_FUNC;
    pos = text.find(ENDL, pos);
    TRACE_POS(pos);
    check_range();
}

Cursor& Cursor::begin()
{
    TRACE_FUNC;
    pos = 0;
    TRACE_POS(pos);
    check_range();
    return *this;
}

Cursor& Cursor::end()
{
    TRACE_FUNC;
    pos = text.size() - ENDL_SIZE;
    TRACE_POS(pos);
    check_range();
    return *this;
}

void Cursor::label(const string& key)
{
    TRACE_FUNC;
    positions[key] = pos;
    TRACE_OUT << "key = " << key TRACE_END;
    TRACE_POS(pos);
    check_range();
}

bool Cursor::goto_label(const string& key)
{
    TRACE_FUNC;
    if (positions.contains(key))
    {
        pos = positions[key];

        TRACE_OUT << "key = " << key TRACE_END;
        TRACE_POS(pos);

        check_range();
        return true;
    }
    TRACE_OUT << "key = " << key TRACE_END;
    TRACE_POS(pos);
    check_range();
    return false;
}

size_t Cursor::operator [](const string& key)
{
    TRACE_FUNC;
    size_t p = positions[key];

    TRACE_OUT << "key = " << key TRACE_END;
    TRACE_POS(p);
    return p;
}

size_t Cursor::label_size()
{
    TRACE_FUNC;
    TRACE_OUT << "size = " << positions.size() TRACE_END;

    return positions.size();
}

bool Cursor::is_eof()
{
    return is_eof(pos);
}

bool Cursor::is_eof(size_t p)
{
    return text.size() <= p || p == string::npos;
}

bool Cursor::is_eof() const
{
    return is_eof(pos);
}

bool Cursor::is_eof(size_t p) const
{
    return text.size() <= p || p == string::npos;
}

string Cursor::to_string()
{
    stringstream ss;
    ss << pos;
    return ss.str();
}

bool Cursor::set_range_limit(void)
{
    TRACE_FUNC;
    min_ = text.rfind(ENDL, *this);
    max_ = text.find(ENDL, *this);

    min_ = min_ != string::npos ? min_ + ENDL_SIZE : min_;

    TRACE_OUT << "min = " << min_ << " max = " << max_ TRACE_END;
    check_range();

    return true;
}

bool Cursor::set_range_limit(size_t mi, size_t ma)
{
    TRACE_FUNC;
    min_ = mi;
    max_ = ma;
    TRACE_OUT << "min = " << min_ << " max = " << max_ TRACE_END;
    check_range();//may set pos to eof
    return true;
}

bool Cursor::set_range_limit(const string& mi, const string& ma)
{
    TRACE_FUNC;
    min_ = text.rfind(mi, pos);
    max_ = text.find(ma, pos);
    if (min_ == string::npos || max_ == string::npos)
    {
        return false;
    }
    TRACE_OUT << "min = " << min_ << " max = " << max_ << " min pattern = " << mi << " max pattern = " << ma TRACE_END;

    check_range();
    return true;
}

void Cursor::set_min_range_limit(size_t m)
{
    TRACE_FUNC;
    min_ = m;
    TRACE_OUT << "min = " << min_ TRACE_END;
    check_range();
}

bool Cursor::set_min_range_limit(const string& m)
{
    TRACE_FUNC;
    min_ = text.rfind(m, pos);
    if (min_ == string::npos)
    {
        return false;
    }
    TRACE_OUT << "min = " << min_ << " min pattern = " << m TRACE_END;
    check_range();
    return true;
}

void Cursor::set_max_range_limit(size_t m)
{
    TRACE_FUNC;
    max_ = m;
    TRACE_OUT << " max = " << max_ TRACE_END;
    check_range();
}

bool Cursor::set_max_range_limit(const string& m)
{
    TRACE_FUNC;
    TRACE_OUT << "max = " << max_ << " max pattern = " << m TRACE_END;

    max_ = text.find(m, pos);
    if (max_ == string::npos)
    {
        return false;
    }

    TRACE_POS(pos);
    check_range();
    return true;
}

size_t Cursor::get_min_range_limit()
{
    TRACE_FUNC;
    return min_;
}

size_t Cursor::get_max_range_limit()
{
    TRACE_FUNC;
    return max_;
}

void Cursor::reset_range_limit()
{
    TRACE_FUNC;
    min_ = 0;
    max_ = string::npos;
}

bool Cursor::check_range()
{
    bool ret = check_range(pos);
    if(ret == false)
        pos = string::npos;

    return ret;
}

bool Cursor::check_range(size_t& p)
{
    if (p != string::npos && min_ <= p && p <= max_)
        return true;
    TRACE_OUT << "position is out of range" TRACE_END;

    p = string::npos;
    return false;
}
//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(CURSOR)
m->add(chaiscript::fun(&Cursor::inc), "inc");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, find_func)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const std::vector<chaiscript::Boxed_Value>&, find_func)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, const string&, find_func)>(&Cursor::move_to)), "move_to");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_while)), "move_while");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_until)), "move_until");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, find_func)>(&Cursor::move_to_end)), "move_to_end");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const std::vector<chaiscript::Boxed_Value>&, find_func)>(&Cursor::move_to_end)), "move_to_end");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::next_line)), "next_line");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::next_line)), "next_line");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::prev_line)), "prev_line");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::prev_line)), "prev_line");

m->add(chaiscript::fun(&Cursor::move_to_begin_of_line), "move_to_begin_of_line");
m->add(chaiscript::fun(&Cursor::move_to_end_of_line), "move_to_end_of_line");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::next_word)), "next_word");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::next_word)), "next_word");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_to_begin_of_word)), "move_to_begin_of_word");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::move_to_begin_of_word)), "move_to_begin_of_word");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_to_end_of_word)), "move_to_end_of_word");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::move_to_end_of_word)), "move_to_end_of_word");


m->add(chaiscript::fun(&Cursor::begin), "begin");
m->add(chaiscript::fun(&Cursor::end), "end");

m->add(chaiscript::fun(static_cast<bool(Cursor::*)()>(&Cursor::is_eof)), "is_eof");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t)>(&Cursor::is_eof)), "is_eof");

m->add(chaiscript::fun(&Cursor::operator[]), "[]");
m->add(chaiscript::fun(&Cursor::label), "label");
m->add(chaiscript::fun(&Cursor::goto_label), "goto_label");
m->add(chaiscript::fun(&Cursor::label_size), "label_size");

m->add(chaiscript::fun(&Cursor::to_string), "to_string");

m->add(chaiscript::fun(&Cursor::set_name), "set_name");
m->add(chaiscript::fun(&Cursor::get_name), "get_name");


m->add(chaiscript::fun(static_cast<bool(Cursor::*)()>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t, size_t)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&, const string&)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t, size_t)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&, const string&)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<void(Cursor::*)(size_t)>(&Cursor::set_min_range_limit)), "set_min_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&)>(&Cursor::set_min_range_limit)), "set_min_range_limit");
m->add(chaiscript::fun(static_cast<void(Cursor::*)(size_t)>(&Cursor::set_max_range_limit)), "set_max_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&)>(&Cursor::set_max_range_limit)), "set_max_range_limit");
m->add(chaiscript::fun(&Cursor::reset_range_limit), "reset_range_limit");
m->add(chaiscript::fun(&Cursor::get_min_range_limit), "get_min_range_limit");
m->add(chaiscript::fun(&Cursor::get_max_range_limit), "get_max_range_limit");


m->add(chaiscript::type_conversion<Cursor, size_t>());
m->add(chaiscript::fun(&Cursor::operator +=), "+=");
m->add(chaiscript::fun(&Cursor::operator ++), "++");
m->add(chaiscript::fun(&Cursor::operator +), "+");
m->add(chaiscript::fun(&Cursor::operator -=), "-=");
m->add(chaiscript::fun(&Cursor::operator --), "--");
m->add(chaiscript::fun(&Cursor::operator -), "-");

m->add(chaiscript::fun(&Cursor::operator <), "<");
m->add(chaiscript::fun(&Cursor::operator <=), "<=");
m->add(chaiscript::fun(&Cursor::operator >), ">");
m->add(chaiscript::fun(&Cursor::operator >=), ">=");
m->add(chaiscript::fun(&Cursor::operator ==), "==");
m->add(chaiscript::fun(&Cursor::operator !=), "!=");


m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const Cursor&)>(&Cursor::operator =)), "=");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::operator =)), "=");

m->add(chaiscript::constructor<Cursor(Text&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(string&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(const Cursor&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(Cursor&)>(), "Cursor");
m->add(chaiscript::user_type<Cursor>(), "Cursor");

m->add(chaiscript::constructor<Cursor(Text&, const string&, find_func)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(string&, const string&, find_func)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(const Cursor&, const string&, find_func)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(Cursor&, const string&, find_func)>(), "Cursor");

m->add(chaiscript::type_conversion<Cursor, bool>([](const Cursor& c) { return c.is_eof() == false;}));

END_DECLARE(CURSOR)
