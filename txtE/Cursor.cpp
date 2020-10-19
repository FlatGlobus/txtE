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

using namespace std;
using namespace text;
//////////////////////////////////////////////////////////////////////////
namespace cursor
{
    inline size_t correct_pos(size_t pos, size_t text_size)
    {
        if (pos == string::npos)
            return text_size;

        return pos;
    }
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

        Position i = p;
        do
        {
            if (pattern.find(text.substr(i)) != string::npos || i == 0)
            {
                TRACE_POS(i);
                return i != 0 ? ++i : Position();
            }
            i -= 1;

        } while (eof(i) == false);

        TRACE_POS(EOF);
        return EOF;
    }


    Position Cursor::multi_find(const vector<string>& pattern, find_func func, size_t& idx)
    {
        Position ret_pos = EOF;
        idx = 0;
        TRACE_FUNC;
        for (const auto& p : pattern)
        {
            TRACE_OUT << "pattern = " << p TRACE_END;
            ret_pos = func(text, p, pos);
            if (ret_pos != string::npos)
            {
                TRACE_POS(ret_pos);
                break;
            }
            ++idx;
        }
        return ret_pos;
    }

    int Cursor::find_count(const string& pattern, const Position& from_pos, const Position& until_pos, find_func func)
    {
        //TRACE_FUNC;
        int count = 0;

        Position i = from_pos;
        for (; eof(i) == false;)
        {
            i = func(text, pattern, i);

            if (eof(i) == false && i <= until_pos)
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

    Cursor& Cursor::dec(size_t p)
    {
        pos -= p;

        TRACE_FUNC;
        TRACE_OUT << "decrement = " << p TRACE_END;
        TRACE_POS(pos);

        return *this;
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

    Cursor& Cursor::move_to(const vector < string >& pattern, find_func func)
    {
        TRACE_FUNC;
        if (!func)
        {
            throw runtime_error("Cursor::move_to Find function in move_to is NULL.");
        }
        size_t idx;
        pos = multi_find(pattern, func, idx);

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
        if (eof(pos1) == true)
        {
            return *this;
        }

        for (; count > 0 && eof(pos2) == false; )
        {
            pos2 = func(text, pattern2, pos1); //}
            if (eof(pos2) == true)
            {
                break;
            }
            count -= 1;
            count += find_count(pattern1, pos1, pos2, func);// count "{" between "{ }"
            if (count == 0)
            {
                break;
            }

            pos1 = ++pos2;
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
        bool found = false;
        for (; eof() == false; ++pos)
        {
            if (pattern.find(text.substr(pos, pos)) == string::npos)
            {
                found = true;
                break;
            }
        }

        if (found == false)
            pos = get_string().size();

        pos -= 1;
        TRACE_OUT << "pattern = " << pattern TRACE_END;
        TRACE_POS(pos);

        return *this;
    }

    Cursor& Cursor::move_until(const string& pattern)
    {
        TRACE_FUNC;
        bool found = false;
        for (; eof() == false; ++pos)
        {
            if (pattern.find(text.substr(pos)) != string::npos)
            {
                found = true;
                break;
            }
        }

        if (found == false)
            pos = get_string().size();

        pos -= 1;

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
        if (eof() == false)
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
        size_t idx;
        pos = multi_find(pattern, func, idx);
        if (eof() == false)
            pos += pattern[idx].size();
        TRACE_POS(pos);

        return *this;
    }

    Cursor& Cursor::next_word(const string& pattern)
    {
        TRACE_FUNC;

        pos = get_string().find_first_of(pattern, pos);
        bool flag = eof();
        pos = get_string().find_first_not_of(pattern, pos);

        if (flag == false && pos == string::npos)
        {
            pos = get_string().size() - 1;
        }

        TRACE_OUT << "pattern = " << pattern TRACE_END;
        TRACE_POS(pos);

        return *this;
    }

    Cursor& Cursor::next_word()
    {
        return next_word(space_pattern);
    }

    Cursor& Cursor::move_to_word_begin(const string& pattern)
    {
        TRACE_FUNC;

        if (pattern.find_first_of(text.substr(pos)) == string::npos) //pos должна быть в слове, иначе не понятно в каком слове искать начало
        {
            pos = rfind_first_of(pattern, pos);
        }
        else
        {
            pos = EOF;
        }

        TRACE_OUT << "pattern = " << pattern TRACE_END;
        TRACE_POS(pos);

        return *this;
    }

    Cursor& Cursor::move_to_word_begin()
    {
        return move_to_word_begin(space_pattern);
    }

    Cursor& Cursor::move_to_word_end(const string& pattern)
    {
        TRACE_FUNC;
        TRACE_OUT << "pattern = " << pattern TRACE_END;

        bool flag = eof();

        if (pattern.find_first_of(text.substr(pos)) == string::npos)// pos is in the word
        {
            pos = get_string().find_first_of(space_pattern, pos);//find end of the word
            if (flag == false && pos == string::npos) //end of the test/string
                pos = get_string().size() - 1;
            else
                pos -= 1;

            TRACE_POS(pos);
            return *this;
        }
        //pos is not in the word
        pos = EOF;
        TRACE_POS(pos);
        return *this;
    }

    Cursor& Cursor::move_to_word_end()
    {
        return move_to_word_end(space_pattern);
    }

    Cursor& Cursor::goto_line(size_t line_num)
    {
        TRACE_FUNC;
        begin();
        next_line(line_num - 1);

        return *this;
    }

    Cursor& Cursor::next_line()
    {
        TRACE_FUNC;

        size_t p = get_string().find(ENDL, pos);
        if (eof(p) == false && eof(p += CHAR_SIZE) == false)
        {
            pos = p;
            TRACE_POS(pos);
            return *this;
        }
        pos = EOF;
        TRACE_POS(pos);

        return *this;
    }

    Cursor& Cursor::next_line(size_t count)
    {
        size_t i;
        for (i = 0; i < count; i++)
        {
            next_line();
            if (eof())
                break;
        }

        return *this;
    }

    Cursor& Cursor::prev_line()
    {
        TRACE_FUNC;
        Position p = get_string().rfind(ENDL, pos);
        if (p.eof() == false)
        {
            pos = p - CHAR_SIZE;
        }

        TRACE_POS(pos);
        return *this;
    }

    Cursor& Cursor::prev_line(size_t count)
    {
        for (size_t i = 0; i < count && eof() == false; ++i)
        {
            prev_line();
        }
        return *this;
    }

    Cursor& Cursor::move_to_line_begin()
    {
        TRACE_FUNC;
        if (pos == 0)
        {
            TRACE_POS(pos);
            return *this;
        }

        pos = get_string().rfind(ENDL, pos);
        if (pos.eof())
        {
            TRACE_POS(pos);
            return *this;
        }

        pos += CHAR_SIZE;
        TRACE_POS(pos);

        return *this;
    }

    Cursor& Cursor::move_to_line_end()
    {
        TRACE_FUNC;
        pos = get_string().find(ENDL, pos);
        if (pos.eof())
            pos = get_string().size() - CHAR_SIZE;

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
        pos = text.size() - 1;
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

    bool Cursor::eof() const
    {
        return eof(pos);
    }

    bool Cursor::eof(size_t p) const
    {
        return !(p < text.size()) || pos.eof(p);
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

    Cursor& Cursor::goto_col(size_t col)
    {
        TRACE_FUNC;
        move_to_line_begin();

        size_t line_end = get_string().find(ENDL, pos);

        if (eof(line_end) == false && pos + col <= line_end)
        {
            inc(col);
        }
        else
            pos = EOF;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MODULE(CURSOR)
    m->add(chaiscript::fun(&Cursor::inc), "inc");
    m->add(chaiscript::fun(&Cursor::dec), "dec");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::move_to)), "move_to");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, find_func)>(&Cursor::move_to)), "move_to");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const vector<string>&, find_func)>(&Cursor::move_to)), "move_to");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, const string&, find_func)>(&Cursor::move_to)), "move_to");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_while)), "move_while");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_until)), "move_until");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, find_func)>(&Cursor::move_to_end)), "move_to_end");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const vector<string>&, find_func)>(&Cursor::move_to_end)), "move_to_end");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::goto_line)), "goto_line");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::goto_col)), "goto_col");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::next_line)), "next_line");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::next_line)), "next_line");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::prev_line)), "prev_line");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::prev_line)), "prev_line");

    m->add(chaiscript::fun(&Cursor::move_to_line_begin), "move_to_line_begin");
    m->add(chaiscript::fun(&Cursor::move_to_line_end), "move_to_line_end");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::next_word)), "next_word");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::next_word)), "next_word");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_to_word_begin)), "move_to_word_begin");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::move_to_word_begin)), "move_to_word_begin");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_to_word_end)), "move_to_word_end");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::move_to_word_end)), "move_to_word_end");

    m->add(chaiscript::fun(&Cursor::begin), "begin");
    m->add(chaiscript::fun(&Cursor::end), "end");

    m->add(chaiscript::fun(static_cast<bool(Cursor::*)() const>(&Cursor::eof)), "eof");
    m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t)const>(&Cursor::eof)), "eof");

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

    m->add(chaiscript::fun([](Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.inc(c2.get_pos()); }), "+=");
    m->add(chaiscript::fun([](Cursor& c1, size_t c2) { return c1.inc(c2);}), "+=");
    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); Cursor c(c1); c.inc(c2.get_pos()); return c; }), "+");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { Cursor c(c1); c.inc(c2); return c; }), "+");

    m->add(chaiscript::fun([](Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.dec(c2.get_pos()); }), "-=");
    m->add(chaiscript::fun([](Cursor& c1, size_t c2) { return c1.dec(c2); }), "-=");
    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); Cursor c(c1); c.dec(c2.get_pos()); return c; }), "-");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { Cursor c(c1); c.dec(c2); return c; }), "-");

    m->add(chaiscript::fun([](Cursor& c1) { return c1.inc(1); }), "++");
    m->add(chaiscript::fun([](Cursor& c1) { return c1.dec(1); }), "--");
    
    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.get_pos() < c2.get_pos(); }), "<");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { return c1.get_pos() < c2; }), "<");
    m->add(chaiscript::fun([](size_t c1, const Cursor& c2) { return c1 < c2.get_pos(); }), "<");

    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.get_pos() <= c2.get_pos(); }), "<=");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { return c1.get_pos() <= c2; }), "<=");
    m->add(chaiscript::fun([](size_t c1, const Cursor& c2) { return c1 <= c2.get_pos(); }), "<=");

    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.get_pos() > c2.get_pos(); }), ">");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { return c1.get_pos() > c2; }), ">");
    m->add(chaiscript::fun([](size_t c1, const Cursor& c2) { return c1 > c2.get_pos(); }), ">");

    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.get_pos() >= c2.get_pos(); }), ">=");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { return c1.get_pos() >= c2; }), ">=");
    m->add(chaiscript::fun([](size_t c1, const Cursor& c2) { return c1 >= c2.get_pos(); }), ">=");

    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.get_pos() == c2.get_pos(); }), "==");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { return c1.get_pos() == c2; }), "==");
    m->add(chaiscript::fun([](size_t c1, const Cursor& c2) { return c1 == c2.get_pos(); }), "==");

    m->add(chaiscript::fun([](const Cursor& c1, const Cursor& c2) { c1.check_cursor(c2); return c1.get_pos() != c2.get_pos(); }), "!=");
    m->add(chaiscript::fun([](const Cursor& c1, size_t c2) { return c1.get_pos() != c2; }), "!=");
    m->add(chaiscript::fun([](size_t c1, const Cursor& c2) { return c1 != c2.get_pos(); }), "!=");

    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const Cursor&)>(&Cursor::operator =)), "=");
    m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::operator =)), "=");

    m->add(chaiscript::constructor<Cursor(Text&)>(), "Cursor");
    m->add(chaiscript::constructor<Cursor(const Cursor&)>(), "Cursor");
    m->add(chaiscript::constructor<Cursor(Cursor&)>(), "Cursor");
    m->add(chaiscript::user_type<Cursor>(), "Cursor");

    m->add(chaiscript::constructor<Cursor(Text&, const string&, find_func)>(), "Cursor");
    m->add(chaiscript::constructor<Cursor(const Cursor&, const string&, find_func)>(), "Cursor");

    m->add(chaiscript::type_conversion<Cursor, bool>([](const Cursor& c)-> bool { return c.eof() == false; }));

//    m->add(chaiscript::type_conversion<Cursor, size_t>([](const Cursor& c) { return c.get_pos(); }));

    m->add_global_const(chaiscript::const_var(EOF), "EOF");
    END_DECLARE(CURSOR)
}