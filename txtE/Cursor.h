// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

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

const string space_pattern = " \t\n\r;:.,?!";
using posMap = map<string, size_t>;
using strVector = vector<string>;
using find_func = function<size_t(const string&, const string&, size_t)>;

class Cursor
{
protected:
    posMap positions;
    string& text;
    size_t pos = 0;
    string name;
    size_t min_ = 0;
    size_t max_ = ULLONG_MAX - 1;

    size_t rfind_first_of(const string& pattern, size_t p);
    void check_cursor(const Cursor& c);
    size_t multi_find(const std::vector<std::string>& pattern, find_func func);
    int find_count(const std::string& pattern, size_t from_pos, size_t until_pos, find_func func);

    inline void set_eof()
    {
        pos = string::npos;
    }

public:
    Cursor(Text& t);
    Cursor(string& t);
    Cursor(const Cursor& c);
    Cursor(Cursor& c);

    Cursor(Text& t, const string& pattern, find_func func);
    Cursor(string& t, const string& pattern, find_func func);
    Cursor(const Cursor& c, const string& pattern, find_func func);
    Cursor(Cursor& c, const string& pattern, find_func func);

    Cursor& inc(size_t p);
    Cursor& operator += (size_t p);
    size_t operator + (size_t p);
    Cursor& operator ++ ();
    Cursor& operator -= (size_t p);
    size_t operator - (size_t p);
    Cursor& operator -- ();
    bool operator == (size_t p);
    bool operator != (size_t p);
    Cursor& operator = (const Cursor& c);
    Cursor& operator = (size_t p);
    bool operator < (size_t p);
    bool operator <= (size_t p);
    bool operator >= (size_t p);
    bool operator > (size_t p);

    Cursor& move_to(size_t p);
    Cursor& move_to(const string& pattern, find_func func);
    Cursor& move_to(const std::vector<std::string>& pattern, find_func func);
    // pattern1 = {, pattern2 =}, moves pos to } ; from here ->{ {abcd} ->}<- to here }
    Cursor& move_to(const string& pattern1, const string& pattern2, find_func func);

    Cursor& move_while(const string& pattern);
    Cursor& move_until(const string& pattern);

    Cursor& move_to_end(const string& pattern, find_func func);
    Cursor& move_to_end(const std::vector<std::string>& pattern, find_func func);

    Cursor& next_word(const string& pattern);
    Cursor& next_word();

    Cursor& move_to_begin_of_word(const string& pattern);
    Cursor& move_to_begin_of_word();

    Cursor& move_to_end_of_word(const string& pattern);
    Cursor& move_to_end_of_word();

    Cursor& next_line();
    Cursor& next_line(size_t count);

    Cursor& prev_line();
    Cursor& prev_line(size_t count);

    void move_to_begin_of_line();
    void move_to_end_of_line();

    Cursor& move_to_line(size_t line_num);
    Cursor& move_to_col(size_t col);

    Cursor& begin();
    Cursor& end();

    void label(const string& key);
    bool goto_label(const string& key);
    size_t operator [](const string& key);
    size_t label_size();

    bool is_eof();
    bool is_eof() const;
    bool is_eof(size_t p);
    bool is_eof(size_t p) const;

    operator size_t()
    {
        return pos;
    }

    size_t get_pos()
    {
        return pos;
    }

    const string& get_text() const
    {
        return text;
    }

    string to_string();

    void set_name(const string& n)
    {
        name = n;
    }

    const string& get_name() const
    {
        return name;
    }

    bool set_range_limit(void);
    bool set_range_limit(size_t mi, size_t ma);
    bool set_range_limit(const string& mi, const string& ma);

    void set_min_range_limit(size_t m);
    bool set_min_range_limit(const string& m);

    void set_max_range_limit(size_t m);
    bool set_max_range_limit(const string& m);

    size_t get_min_range_limit();
    size_t get_max_range_limit();

    void reset_range_limit();

    bool check_range();
    bool check_range(size_t& p);
};
