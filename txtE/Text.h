// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once

#include <string>
#include <vector>
#include <chaiscript/chaiscript.hpp>
#include <tchar.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

class Cursor;

enum class el_types { elNone, elWin, elUnix, elMac };

class Text
{
    string text;
    el_types original_endl;
    bool changed;

    el_types find_endl_type();
protected:
    void check_cursor(const Cursor&);
public:
    Text();
    Text(const string&);
    ~Text();

    size_t load(const fs::path&);
    size_t write(const fs::path&);
    size_t write(const fs::path&, el_types);

    string get(const Cursor&, size_t);
    string get_to_endl(const Cursor&);
    string get_line(const Cursor& start);
    string get_word(const Cursor& start);
    string get_between(const Cursor&, const Cursor&);

    void set(const Cursor&, const string&);
    void set_line(const Cursor&, const string&);

    void insert(const Cursor&, const string&);
    void insert_line(const Cursor&, const string&);

    void add(const string&);
    void add(const Text&);

    Cursor diff(const Cursor& start, const string& text1, string& result);
    Cursor diff(const Cursor& start, const Text& text1, string& result);

    size_t size() const;

    void erase(const Cursor& pos, size_t count);
    void erase_between(const Cursor& from, const Cursor& to);
    void erase_line(const Cursor& pos);
    void clear();

    el_types get_endl_type() const { return original_endl; }

    operator const std::string& () const { return text; }
    bool is_changed() { return changed; };
    friend class Cursor;
    //TODO sort
};




