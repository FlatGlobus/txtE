// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once

#include <string>
#include <vector>
#include <chaiscript/chaiscript.hpp>
#include <tchar.h>

using namespace std;

class Cursor;

enum class el_types { elNone, elWin, elUnix, elMac };

class Text
{
    string text;
    el_types original;

    el_types find_endl_type();
protected:
    void check_cursor(Cursor&);
    bool is_eof(size_t);
public:
    Text();
    ~Text();

    size_t load(const string&);
    size_t write(const string&);
    size_t write(const string&, el_types);

    string get(Cursor&, size_t);
    string get_to_endl(Cursor&);
    string get_line(Cursor& start);
    string get_word(Cursor& start);
    string get_between(Cursor&, Cursor&);

    void set(Cursor&, const string&);
    void set_line(Cursor&, const string&);

    void insert(Cursor&, const string&);
    void insert_line(Cursor&, const string&);

    void add(const string&);
    void add(const Text&);

    Cursor diff(string&, Cursor&);
    Cursor diff(Text&, Cursor&);

    size_t size();

    void erase(Cursor& pos, size_t count);
    void erase_between(Cursor& from, Cursor& to);
    Cursor erase_line(Cursor& pos);
    void clear();

    el_types get_endl_type() { return original; }

    operator const std::string& () { return text; }
    friend class Cursor;
};




