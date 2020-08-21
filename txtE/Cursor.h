// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <compare>
#include <chaiscript/chaiscript.hpp>
#include "text.h"
#include <memory>
#include <limits.h>
#include"util.h"

//////////////////////////////////////////////////////////////////////////
using namespace std;

class Position
{
    size_t pos = 0;
    size_t min_pos = 0;
    size_t max_pos = SIZE_MAX - 1;

    inline size_t inc(size_t delta) const
    {
        if(is_eof(pos))
            return Position::eof;

        size_t temp_pos = pos + delta;
        if(temp_pos < pos || max_pos < temp_pos)
            return Position::eof;

        return pos + delta;
    };

    inline size_t dec(size_t delta) const
    {
        if (is_eof(pos))
            return Position::eof;

        size_t temp_pos = pos - delta;
        if (temp_pos > pos || temp_pos < min_pos)
            return Position::eof;

        return pos - delta;
    };
public:
    static Position eof;

    Position()
    {
    }

    Position(size_t p)
    {
        pos = p;
    }

    Position(Position& p)
    {
        min_pos = p.min_pos;
        max_pos = p.max_pos;
        pos = p.pos;
    }

    Position(const Position& p)
    {
        min_pos = p.min_pos;
        max_pos = p.max_pos;
        pos = p.pos;
    }

    inline Position& operator = (const Position& p)
    {
        pos = p.pos;
        min_pos = p.min_pos;
        max_pos = p.max_pos;
        return *this;
    }

    inline operator size_t () const { return pos; }
    inline operator string () const { return to_string(pos); }

    inline Position& operator ++()
    {
        pos = inc(1);
        return *this;
    }

    inline const Position& operator --()
    {
        pos = dec(1);
        return *this;
    }

    inline Position& operator +=(size_t delta)
    {
        pos = inc(delta);
        return *this;
    }

    inline const Position& operator -=(size_t delta)
    {
        pos = dec(delta);
        return *this;
    }

    inline Position& operator +=(const Position& delta)
    {
        pos = inc(delta.pos);
        return *this;
    }

    inline const Position& operator -=(const Position& delta)
    {
        pos = dec(delta.pos);
        return *this;
    }

    inline void set_min(size_t m) { min_pos = m; }
    inline void set_max(size_t m) { max_pos = m; }

    inline size_t get_min() const { return min_pos; }
    inline size_t get_max() const { return max_pos; }

    inline void set_eof() { pos = Position::eof; }
    inline bool is_eof() const { return is_eof(pos); }
    inline bool is_eof(size_t p) const { return !(min_pos <= p && p <= max_pos) || p == Position::eof; }

    inline void set_to_min() { pos = min_pos; }
    inline void set_to_max() { pos = max_pos; }

    void reset()
    {
        size_t pos = 0;
        size_t min_pos = 0;
        size_t max_pos = SIZE_MAX - 1;
    }
};

template <class T> bool AreEqual(const T& a, const T& b)
{
    return addressof(a) == addressof(b);
}

const string space_pattern = " \t\n\r;:.,?!";
using posMap = map<string, Position>;
using strVector = vector<string>;
using find_func = function<size_t(const string&, const string&, size_t)>;

class Cursor
{
protected:
    posMap labels;
    Text&  text;
    Position pos;
    string name;

    Position rfind_first_of(const string& pattern, const Position & p);
    void check_cursor(const Cursor& c);
    Position multi_find(const vector<string>& pattern, find_func func, size_t& idx);
    int find_count(const string& pattern, const Position& from_pos, const Position& until_pos, find_func func);

public:
    Cursor(Text& t);
    Cursor(const Cursor& c);
    Cursor(Cursor& c);

    Cursor(Text& t, const string& pattern, find_func func);
    Cursor(const Cursor& c, const string& pattern, find_func func);

    Cursor& inc(size_t p);
    Cursor& dec(size_t p);
    Cursor& operator += (size_t p);
    size_t operator + (size_t p);
    Cursor& operator ++ ();
    Cursor& operator -= (size_t p);
    size_t operator - (size_t p);
    Cursor& operator -- ();

    Cursor& operator = (const Cursor& c);
    Cursor& operator = (size_t p);

    bool operator == (size_t p);
    bool operator != (size_t p);
    bool operator < (size_t p);
    bool operator <= (size_t p);
    bool operator >= (size_t p);
    bool operator > (size_t p);

    Cursor& move_to(size_t p);
    Cursor& move_to(const string& pattern, find_func func);
    Cursor& move_to(const vector<string>& pattern, find_func func);
    // pattern1 = {, pattern2 =}, moves pos to } ; from here ->{ {abcd} ->}<- to here }, count { must be == count }
    Cursor& move_to(const string& pattern1, const string& pattern2, find_func func);

    Cursor& move_while(const string& pattern);
    Cursor& move_until(const string& pattern);

    Cursor& move_to_end(const string& pattern, find_func func);
    Cursor& move_to_end(const vector<string>& pattern, find_func func);

    Cursor& next_word(const string& pattern);
    Cursor& next_word();

    Cursor& move_to_begin_of_word(const string& pattern);
    Cursor& move_to_begin_of_word();

    Cursor& move_to_end_of_word(const string& pattern);
    Cursor& move_to_end_of_word();

    Cursor& goto_line(size_t line_num);
    Cursor& goto_col(size_t col);

    Cursor& next_line();
    Cursor& next_line(size_t count);

    Cursor& prev_line();
    Cursor& prev_line(size_t count);

    Cursor& move_to_begin_of_line();
    Cursor& move_to_end_of_line();

    Cursor& begin();
    Cursor& end();

    void label(const string& key);
    bool goto_label(const string& key);
    const Position& operator [](const string& key);
    size_t label_size();

    bool is_eof() const;
    bool is_eof(size_t p) const;

    inline operator Position () const
    {
        return pos;
    }

    inline const Position& get_pos() const
    {
        return pos;
    }

    inline const Text& get_text() const
    {
        return text;
    }

    inline const string& get_string() const
    {
        return text;
    }

    string to_string() const;

    inline void set_name(const string& n)
    {
        name = n;
    }

    inline const string& get_name() const
    {
        return name;
    }

    inline operator bool() const
    {
        return is_eof() == false;
    }

    bool set_range(size_t min, size_t max);
    void set_min_range(size_t m);
    void set_max_range(size_t m);

    size_t get_min_range() const;
    size_t get_max_range() const;

    void reset();

    inline void set_eof()
    {
        pos = Position::eof;
    }
};
