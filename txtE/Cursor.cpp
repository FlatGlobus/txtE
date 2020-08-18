#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <chaiscript/chaiscript.hpp>
#include "text.h"
#include <memory>
#include <limits.h>
#include"util.h"
#include "Cursor.h"
//////////////////////////////////////////////////////////////////////////
Position Position::eof = { string::npos };
//////////////////////////////////////////////////////////////////////////
Cursor::Cursor(Text& t) :text(t)
{
}

Cursor::Cursor(const Cursor& c) : text(c.text), pos(c.pos)
{
}

Cursor::Cursor(Cursor& c) : text(c.text), pos(c.pos)
{
}

Cursor::Cursor(Text& t, const string& pattern, find_func func) : text(t)
{
    move_to(pattern, func);
}

Cursor::Cursor(const Cursor& c, const string& pattern, find_func func) : text(c.text), pos(c.pos)
{
    move_to(pattern, func);
}

Position Cursor::rfind_first_of(const string& pattern, const Position& p)
{
    TRACE_FUNC;
    TRACE_OUT << "pattern = " << pattern << "\nposition = " << p TRACE_END;

    for (Position i = p; 0 < i; --i)
    {
        if (pattern.find(text[i]) != Position::eof)
        {
            i += 1;
            TRACE_POS(i);
            return i;
        }
    }

    TRACE_POS(Position::eof);
    return Position::eof;
}

void Cursor::check_cursor(const Cursor& c)
{
    if (AreEqual(c.text, text) == false)
    {
        throw runtime_error("Cursor object : \"" + c.get_name() + "\" is created for other Text object.");
    }
}

Position Cursor::multi_find(const vector<string>& pattern, find_func func)
{
    Position ret_pos = Position::eof;

    TRACE_FUNC;
    for (auto p : pattern)
    {
        TRACE_OUT << "pattern = " << p TRACE_END;
        ret_pos = func(text, p, pos);
        if (ret_pos.is_eof() == false)
        {
            TRACE_POS(ret_pos);
            break;
        }
    }
    return ret_pos;
}

int Cursor::find_count(const string& pattern, const Position& from_pos, const Position& until_pos, find_func func)
{
    TRACE_FUNC;
    int count = 0;

    Position i = from_pos;
    for (; is_eof(i) == false;)
    {
        i = func(text, pattern, i);

        if (is_eof(i) == false && i <= until_pos)
        {
            ++count;
            i += 1;
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

    return *this;
}

Cursor& Cursor::operator += (size_t p)
{
    pos += p;
    TRACE_FUNC;
    TRACE_OUT << "increment = " << p TRACE_END;
    TRACE_POS(pos);

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
    ++pos;

    TRACE_FUNC;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::operator -= (size_t p)
{
    pos -= p;

    TRACE_FUNC;
    TRACE_OUT << "decrement = " << p TRACE_END;
    TRACE_POS(pos);

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
    --pos;

    TRACE_FUNC;
    TRACE_POS(pos);

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
    labels = c.labels;

    return *this;
}

Cursor& Cursor::operator = (size_t p)
{
    pos = p;

    TRACE_FUNC;
    TRACE_OUT << "set position = " << p TRACE_END;
    TRACE_POS(pos);

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

    return *this;
}

Cursor& Cursor::move_to(const vector < string > & pattern, find_func func)
{
    TRACE_FUNC;
    if (!func)
    {
        throw runtime_error("Cursor::move_to Find function in move_to is NULL.");
    }
    pos = multi_find(pattern, func);

    TRACE_POS(pos);

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

    Position pos_orig = pos;
    Position pos1, pos2;
    int count = 1;

    set_eof();
    pos1 = func(text, pattern1, pos_orig);//{
    pos1 += 1;
    if (is_eof(pos1) == true)
    {
        return *this;
    }

    for (; count > 0 && is_eof(pos2) == false; )
    {
        pos2 = func(text, pattern2, pos1); //}
        if (is_eof(pos2) == true)
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
    }

    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::move_while(const string& pattern)
{
    TRACE_FUNC;
    for (; is_eof() == false; ++pos)
    {
        if (pattern.find(text[pos]) != Position::eof)
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

    for (; is_eof() == false; ++pos)
    {
        if (pattern.find(text[pos]) == Position::eof)
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

    return *this;
}

Cursor& Cursor::move_to_end(const vector<string>& pattern, find_func func)
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

    return *this;
}

Cursor& Cursor::next_word(const string& pattern)
{
    TRACE_FUNC;

    pos = get_string().find_first_of(pattern, pos);
    pos = get_string().find_first_not_of(pattern, pos);

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::next_word()
{
    return next_word(space_pattern);
}

Cursor& Cursor::move_to_begin_of_word(const string& pattern)
{
    TRACE_FUNC;

    if (pattern.find_first_of(text[pos]) == Position::eof) //pos должна быть в слове, иначе не понятно в каком слове искать начало
    {
        pos = rfind_first_of(pattern, pos);
    }
    else
    {
        pos = Position::eof;
    }

    TRACE_OUT << "pattern = " << pattern TRACE_END;
    TRACE_POS(pos);

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

    if (pattern.find_first_of(text[pos]) == Position::eof)
    {
        pos = get_string().find_first_of(space_pattern, pos);
        TRACE_POS(pos);
        return *this;
    }

    pos = Position::eof;
    TRACE_POS(pos);
    return *this;
}

Cursor& Cursor::move_to_end_of_word()
{
    return move_to_end_of_word(space_pattern);
}

Cursor& Cursor::goto_line(size_t line_num)
{
    TRACE_FUNC;
    begin();
    next_line(line_num);

    return *this;
}

Cursor& Cursor::next_line()
{
    TRACE_FUNC;

    size_t p = get_string().find(ENDL, pos);
    if (is_eof(p) == false && is_eof(p += ENDL_SIZE) == false)
    {
        pos = p;
        TRACE_POS(pos);
        return *this;
    }
    pos = Position::eof;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::next_line(size_t count)
{
    if (count < 0)
    {
        throw runtime_error("Cursor::next_line, argument cannot be less zero.");
    }
    size_t i;
    for ( i = 0; i < count; i++)
    {
        next_line();
        if (is_eof())
            break;
    }

    return *this;
}

Cursor& Cursor::prev_line()
{
    TRACE_FUNC;
    Position p = get_string().rfind(ENDL, pos);
    if (p.is_eof() == false)
    {
        pos = p;
        TRACE_POS(pos);
        return *this;
    }

    if (p > 1)
    {
        pos = p - ENDL_SIZE;
        TRACE_POS(pos);

        return *this;
    }

    pos = Position::eof;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::prev_line(size_t count)
{
    if (count < 0)
    {
        throw runtime_error("Cursor::prev_line, argument cannot be less zero.");
    }

    for (size_t i = 0; i < count || is_eof() == false; ++i)
    {
        prev_line();
    }
    return *this;
}

Cursor& Cursor::move_to_begin_of_line()
{
    TRACE_FUNC;
    if (pos == 0)
    {
        TRACE_POS(pos);
        return *this;
    }

    pos = get_string().rfind(ENDL, pos);
    if (pos.is_eof())
    {
        TRACE_POS(pos);
        return *this;
    }

    pos += 1;
    TRACE_POS(pos);

    return *this;
}

Cursor& Cursor::move_to_end_of_line()
{
    TRACE_FUNC;
    pos = get_string().find(ENDL, pos);
    TRACE_POS(pos);
    return *this;
}

Cursor& Cursor::begin()
{
    TRACE_FUNC;
    pos = 0;
    TRACE_POS(pos);
    return *this;
}

Cursor& Cursor::end()
{
    TRACE_FUNC;
    pos = text.size() - ENDL_SIZE;
    TRACE_POS(pos);
    return *this;
}

void Cursor::label(const string& key)
{
    TRACE_FUNC;
    labels[key] = pos;
    TRACE_OUT << "key = " << key TRACE_END;
    TRACE_POS(pos);
}

bool Cursor::goto_label(const string& key)
{
    TRACE_FUNC;
    //if (positions.contains(key))
    if (labels.count(key))
    {
        pos = labels[key];

        TRACE_OUT << "key = " << key TRACE_END;
        TRACE_POS(pos);

        return true;
    }
    TRACE_OUT << "key = " << key TRACE_END;
    TRACE_POS(pos);
    return false;
}

const Position& Cursor::operator [](const string& key)
{
    TRACE_FUNC;
    Position& p = labels[key];

    TRACE_OUT << "key = " << key TRACE_END;
    TRACE_POS(p);
    return p;
}

size_t Cursor::label_size()
{
    TRACE_FUNC;
    TRACE_OUT << "size = " << labels.size() TRACE_END;

    return labels.size();
}

bool Cursor::is_eof() const
{
    return is_eof(pos);
}

bool Cursor::is_eof(size_t p) const
{
    return !(p < text.size()) || pos.is_eof(p);
}

string Cursor::to_string() const
{
    stringstream ss;
    ss << pos;
    return ss.str();
}

bool Cursor::set_range(size_t min, size_t max)
{
    TRACE_FUNC;
    pos.set_min(min);
    pos.set_max(max);

    TRACE_OUT << "min = " << min << " max = " << max TRACE_END;
    return true;
}

void Cursor::set_min_range(size_t min)
{
    TRACE_FUNC;
    pos.set_min(min);
    TRACE_OUT << "min = " << min TRACE_END;
}

void Cursor::set_max_range(size_t max)
{
    TRACE_FUNC;
    pos.set_max(max);
    TRACE_OUT << " max = " << max TRACE_END;
}

size_t Cursor::get_min_range() const
{
    TRACE_FUNC;
    return pos.get_min();
}

size_t Cursor::get_max_range() const
{
    TRACE_FUNC;
    return pos.get_max();
}

void Cursor::reset()
{
    TRACE_FUNC;
    pos.reset();
}

Cursor& Cursor::move_to_line(size_t line_num)
{
    TRACE_FUNC;

    begin();
    next_line(line_num);
    if(is_eof() == false)
        move_to_begin_of_line();

    return *this;
}

Cursor& Cursor::move_to_col(size_t col)
{
    TRACE_FUNC;
    move_to_begin_of_line();
    
    size_t line_end = get_string().find(ENDL, pos);

    if (is_eof(line_end) == false && pos + col <= line_end)
    {
        inc(col);
    }
    else
        pos = Position::eof;

    return *this;
}
//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(CURSOR)
m->add(chaiscript::fun(&Cursor::inc), "inc");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, find_func)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const vector<string>&, find_func)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, const string&, find_func)>(&Cursor::move_to)), "move_to");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_while)), "move_while");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_until)), "move_until");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, find_func)>(&Cursor::move_to_end)), "move_to_end");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const vector<string>&, find_func)>(&Cursor::move_to_end)), "move_to_end");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::goto_line)), "goto_line");
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

m->add(chaiscript::fun(static_cast<bool(Cursor::*)() const>(&Cursor::is_eof)), "is_eof");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t)const>(&Cursor::is_eof)), "is_eof");

m->add(chaiscript::fun(&Cursor::operator[]), "[]");
m->add(chaiscript::fun(&Cursor::label), "label");
m->add(chaiscript::fun(&Cursor::goto_label), "goto_label");
m->add(chaiscript::fun(&Cursor::label_size), "label_size");

m->add(chaiscript::fun(&Cursor::to_string), "to_string");

m->add(chaiscript::fun(&Cursor::set_name), "set_name");
m->add(chaiscript::fun(&Cursor::get_name), "get_name");

m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t, size_t)>(&Cursor::set_range)), "set_range");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t, size_t)>(&Cursor::set_range)), "set_range");
m->add(chaiscript::fun(static_cast<void(Cursor::*)(size_t)>(&Cursor::set_min_range)), "set_min_range");
m->add(chaiscript::fun(static_cast<void(Cursor::*)(size_t)>(&Cursor::set_max_range)), "set_max_range");
m->add(chaiscript::fun(&Cursor::reset), "reset");
m->add(chaiscript::fun(&Cursor::get_min_range), "get_min_range");
m->add(chaiscript::fun(&Cursor::get_max_range), "get_max_range");

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
m->add(chaiscript::constructor<Cursor(const Cursor&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(Cursor&)>(), "Cursor");
m->add(chaiscript::user_type<Cursor>(), "Cursor");

m->add(chaiscript::constructor<Cursor(Text&, const string&, find_func)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(const Cursor&, const string&, find_func)>(), "Cursor");

//m->add(chaiscript::type_conversion<Cursor, Position>());
m->add(chaiscript::type_conversion<Cursor, bool>([](const Cursor& c)-> bool { return c.is_eof() == false;}));

m->add(chaiscript::type_conversion<Position, size_t>());
m->add(chaiscript::user_type<Position>(), "Position");

m->add_global_const(chaiscript::const_var(Position::eof), "eof");
END_DECLARE(CURSOR)
