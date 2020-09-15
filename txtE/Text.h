// This file is distributed under the BSD License.
// Copyright 2020, Vladislav Tembekov (wtlbuilder@gmail.com)

#pragma once

#include <string>
#include <vector>
#include <chaiscript/chaiscript.hpp>
#include <tchar.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace cursor
{
    class Cursor;
    class Position;
}

namespace text
{
    enum class el_types { elNone, elWin, elUnix, elMac };

    class Text
    {
        std::string text;
        el_types original_endl;

        el_types find_endl_type();
    protected:
        void check_cursor(const cursor::Cursor&);
    public:
        Text();
        Text(const std::string&);
        ~Text();

        size_t load(const fs::path&);
        size_t write(const fs::path&);
        size_t write(const fs::path&, el_types);

        std::string get(const cursor::Cursor&, size_t);
        std::string get_to_endl(const cursor::Cursor&);
        std::string get_line(const cursor::Cursor& start);
        std::string get_word(const cursor::Cursor& start);
        std::string get_between(const cursor::Cursor&, const cursor::Cursor&);

        void set(const cursor::Cursor&, const std::string&);
        void set_line(const cursor::Cursor&, const std::string&);

        void insert(const cursor::Cursor&, const std::string&);
        void insert_line(const cursor::Cursor&, const std::string&);

        void add(const std::string&);
        void add(const Text&);

        cursor::Cursor diff(const cursor::Cursor& start, const std::string& text1, std::string& result);
        cursor::Cursor diff(const cursor::Cursor& start, const Text& text1, std::string& result);

        size_t size() const;

        void erase(const cursor::Cursor& pos, size_t count);
        void erase_between(const cursor::Cursor& from, const cursor::Cursor& to);
        void erase_line(const cursor::Cursor& pos);
        void clear();

        el_types get_endl_type() const { return original_endl; }

        operator const std::string& () const { return text; }

        Text& operator = (const std::string&);
        Text& operator = (const Text&);

        //internal methods
        std::string substr(cursor::Cursor, cursor::Cursor) const;
        std::string substr(const cursor::Cursor&, size_t) const;
        char substr(const cursor::Cursor&) const;

        std::string substr(cursor::Position, cursor::Position) const;
        std::string substr(const cursor::Position&, size_t) const;
        char substr(const cursor::Position&) const;

        //TODO sort
    };
}



