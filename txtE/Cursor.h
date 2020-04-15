#pragma once
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <chaiscript/chaiscript.hpp>
#include "text.h"
#include <memory>
#include <limits.h>
#include"util.h"

using namespace std;

template <class T> bool AreEqual(const T& a, const T& b)
{
    return addressof(a) == addressof(b);
}

const TCHAR ENDL = '\r';
const string word_space_pattern = " \t\n\r;:.,?!";
using posMap = map<string, size_t>;
using strVector = vector<string>;

class Cursor
{
protected:
    posMap positions;
    string& text;
    size_t pos = 0;
    string name;
    size_t min_ = 0;
    size_t max_ = ULLONG_MAX - 1;

    size_t rfind_first_of(const string& pattern, size_t p)
    {
        for (size_t i = p; i > 0; i--)
        {
            if (pattern.find(text[i]) != string::npos)
            {
                return pos = i + 1;// set pos to first letter of the word
            }
        }
        return pos = string::npos;
    }

    void check_cursor(const Cursor& c)
    {
        if (AreEqual(c.text, text) == false)
        {
            throw runtime_error("Cursor object : \"" + c.get_name() + "\" is created for other Text object.");
        }
    }

    size_t multi_find(const std::vector<chaiscript::Boxed_Value>& pattern, function<size_t(const string&, const string&, size_t)> func)
    {
        size_t ret_pos = string::npos;
        chaiscript::Type_Info ut = chaiscript::user_type<string>();
        for (auto p : pattern)
        {
            if (p.is_type(ut) == false)
            {
                continue;
            }

            ret_pos = func(text, chaiscript::boxed_cast<std::string>(p), pos);
            if (ret_pos != string::npos)
            {
                break;
            }
        }

        return ret_pos;
    }

public:
    Cursor(Text& t) :text(t.text)
    {
    }

    Cursor(string& t) :text(t)
    {
    }

    Cursor(const Cursor& c) :text(c.text), pos(c.pos), min_(c.min_), max_(c.max_)
    {
    }

    Cursor(Cursor& c) :text(c.text), pos(c.pos), min_(c.min_), max_(c.max_)
    {
    }

    Cursor& inc(size_t p)
    {
        pos += p;
        TRACE_ << "increment = " << p ENDLTRACE_;
        check_range();

        return *this;
    }

    Cursor& operator += (size_t p)
    {
        pos += p;
        TRACE_ << "increment = " << p ENDLTRACE_;
        check_range();
        return *this;
    }

    size_t operator + (size_t p)
    {
        size_t sum = pos + p;
        TRACE_ << "increment = " << p << "sum = " << sum ENDLTRACE_;
        return sum;
    }

    Cursor& operator ++ ()
    {
        pos++;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& operator -= (size_t p)
    {
        pos -= p;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    size_t operator - (size_t p)
    {
        TRACE_ ENDLTRACE_;
        return pos - p;
    }

    Cursor& operator -- ()
    {
        pos--;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    bool operator == (size_t p)
    {
        return pos == p;
    }

    bool operator != (size_t p)
    {
        return pos != p;
    }

    Cursor& operator = (const Cursor& c)
    {
        check_cursor(c);
        pos = c.pos;
        text = c.text;
        min_ = c.min_;
        max_ = c.max_;
        TRACE_ ENDLTRACE_;
        return *this;
    }

    Cursor& operator = (size_t p)
    {
        pos = p;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    bool operator < (size_t p)
    {
        return pos < p;
    }

    bool operator <= (size_t p)
    {
        return pos <= p;
    }

    bool operator >= (size_t p)
    {
        return pos >= p;
    }

    bool operator > (size_t p)
    {
        return pos > p;
    }

    Cursor& move_to(size_t p)
    {
        pos = p;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& move_to(const string& pattern, function<size_t(const string&, const string&, size_t)> func)
    {
        if (!func)
        {
            throw runtime_error("Find function in move_to is NULL.");
        }
        pos = func(text, pattern, pos);
        TRACE_ << "pattern = " << "\"" << pattern << "\"" ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& move_to(const std::vector<chaiscript::Boxed_Value>& pattern, function<size_t(const string&, const string&, size_t)> func)
    {
        if (!func)
        {
            throw runtime_error("Cursor::move_to Find function in move_to is NULL.");
        }
        pos = multi_find(pattern, func);
        //TRACE_ << "pattern = " << "\"" << pattern << "\"" ENDLTRACE_;
        check_range();

        return *this;
    }

    Cursor& move_to_end(const string& pattern, function<size_t(const string&, const string&, size_t)> func)
    {
        if (!func)
        {
            throw runtime_error("Cursor::move_to_end Find function is NULL.");
        }
        pos = func(text, pattern, pos);
        if (is_eof() == false)
            pos += pattern.size();
        check_range();
        TRACE_ << "pattern = " << pattern ENDLTRACE_;

        return *this;
    }

    Cursor& move_to_end(const std::vector<chaiscript::Boxed_Value>& pattern, function<size_t(const string&, const string&, size_t)> func)
    {
        if (!func)
        {
            throw runtime_error("Cursor::move_to_end Find function is NULL.");
        }
        pos = multi_find(pattern, func);
        if (is_eof() == false)
            pos += pattern.size();
        check_range();
//        TRACE_ << "pattern = " << pattern ENDLTRACE_;

        return *this;
    }

    Cursor& next_word(const string& pattern)
    {
        pos = text.find_first_of(pattern, pos);
        pos = text.find_first_not_of(pattern, pos);
        TRACE_ << "pattern = " << pattern ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& next_word()
    {
        return next_word(word_space_pattern);
    }

    Cursor& move_to_begin_of_word(const string& pattern)
    {
        if (pattern.find_first_of(text[pos]) == string::npos) //pos должна быть в слове, иначе не понятно в каком слове искать начало те не паттерн
        {
            pos = rfind_first_of(pattern, pos);
        }
        else
        {
            pos = string::npos;
        }
        TRACE_ << "pattern = " << pattern ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& move_to_begin_of_word()
    {
        return move_to_begin_of_word(word_space_pattern);
    }

    Cursor& move_to_end_of_word(const string& pattern)
    {
        if (pattern.find_first_of(text[pos]) == string::npos)
        {
            pos = text.find_first_of(word_space_pattern, pos);
            TRACE_ << "pattern = " << pattern ENDLTRACE_;
            check_range();
            return *this;
        }
        pos = string::npos;
        TRACE_ << "pattern = " << pattern ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& move_to_end_of_word()
    {
        return move_to_end_of_word(word_space_pattern);
    }

    Cursor& next_line()
    {
        size_t p = text.find(ENDL, pos);
        if (is_eof(p += sizeof(ENDL)) == false)
        {
            pos = p;
            TRACE_ ENDLTRACE_;
            check_range();
            return *this;
        }
        pos = string::npos;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& next_line(size_t count)
    {
        if (count < 0)
        {
            throw runtime_error("Cursor::next_line, argument cannot be less zero.");
        }

        for (size_t i = 0; i < count || is_eof() == false; i++)
        {
            next_line();
        }
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& prev_line()
    {
        size_t p = text.rfind(ENDL, pos);
        if (p != string::npos)
        {
            pos = p;
            TRACE_ ENDLTRACE_;
            check_range();
            return *this;
        }

        if (p > 1)
        {
            pos = p - sizeof(ENDL);
            TRACE_ ENDLTRACE_;
            check_range();
            return *this;
        }

        pos = string::npos;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& prev_line(size_t count)
    {
        if (count < 0)
        {
            throw runtime_error("Cursor::prev_line, argument cannot be less zero.");
        }

        for (size_t i = 0; i < count || is_eof() == false; i++)
        {
            prev_line();
        }
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    void begin_of_line()
    {
        pos = text.rfind(ENDL, pos);
        TRACE_ ENDLTRACE_;
        check_range();
    }

    void end_of_line()
    {
        pos = text.find(ENDL, pos);
        TRACE_ ENDLTRACE_;
        check_range();
    }

    Cursor& begin()
    {
        pos = 0;
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    Cursor& end()
    {
        pos = text.size() - sizeof(ENDL);
        TRACE_ ENDLTRACE_;
        check_range();
        return *this;
    }

    void label(const string& key)
    {
        positions[key] = pos;
        TRACE_ ENDLTRACE_;
        check_range();
    }

    bool goto_label(const string& key)
    {
        if (positions.contains(key))
        {
            pos = positions[key];
            TRACE_ << "key = " << key ENDLTRACE_;
            check_range();
            return true;
        }
        TRACE_ << "key = " << key ENDLTRACE_;
        check_range();
        return false;
    }

    size_t operator [](const string& key)
    {
        return positions[key];
    }

    size_t label_size()
    {
        return positions.size();
    }

    bool is_eof()
    {
        return is_eof(pos);
    }

    bool is_eof(size_t p)
    {
        return text.size() <= p || p == string::npos;
    }

    operator size_t()
    {
        return pos;
    }

    size_t get_pos()
    {
        return pos;
    }

    const string& get_string()
    {
        return text;
    }

    string to_string()
    {
        stringstream ss;
        ss << pos;
        return ss.str();
    }

    void set_name(const string& n)
    {
        name = n;
    }

    const string& get_name() const
    {
        return name;
    }

    bool set_range_limit(size_t mi, size_t ma)
    {
        min_ = mi;
        max_ = ma;
        TRACE_ << "min = " << min_ << " max = " << max_ ENDLTRACE_;
        check_range();
        return true;
    }

    bool set_range_limit(const string& mi, const string& ma)
    {
        min_ = text.rfind(mi, pos);
        max_ = text.find(ma, pos);
        if (min_ == string::npos || max_ == string::npos)
        {
            return false;
        }
        TRACE_ << "min = " << min_ << " max = " << max_ << " min pattern = " << mi << " max pattern = " << ma ENDLTRACE_;
        check_range();
        return true;
    }

    void set_min_range_limit(size_t m)
    {
        min_ = m;
        TRACE_ << "min = " << min_ ENDLTRACE_;
        check_range();
    }

    bool set_min_range_limit(const string& m)
    {
        min_ = text.rfind(m, pos);
        if (min_ == string::npos)
        {
            return false;
        }
        TRACE_ << "min = " << min_ << " min pattern = " << m ENDLTRACE_;
        check_range();
        return true;
    }

    void set_max_range_limit(size_t m)
    {
        max_ = m;
        TRACE_ << " max = " << max_ ENDLTRACE_;
        check_range();
    }

    bool set_max_range_limit(const string& m)
    {
        max_ = text.find(m, pos);
        if (max_ == string::npos)
        {
            return false;
        }
        TRACE_ << "max = " << max_ << " max pattern = " << m ENDLTRACE_;
        check_range();
        return true;
    }

    size_t get_min_range_limit()
    {
        return min_;
    }

    size_t get_max_range_limit()
    {
        return max_;
    }

    void reset_range_limit()
    {
        min_ = 0;
        max_ = string::npos;
    }

    bool check_range()
    {
        if (pos == string::npos || min_ <= pos && pos <= max_)
            return true;

        TRACE_ << "Cursor is out of range " << "min = " << min_ << "max = " << max_ ENDLTRACE_

            pos = string::npos;
        return false;
    }
};
