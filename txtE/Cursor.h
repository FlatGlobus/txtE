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

//////////////////////////////////////////////////////////////////////////
namespace cursor
{
    class Position
    {
        size_t pos = 0;
        size_t min_pos = 0;
        size_t max_pos = SIZE_MAX - 1;

        inline size_t inc(size_t delta) const
        {
            if (eof(pos))
                return EOF;

            size_t temp_pos = pos + delta;
            if (temp_pos < pos || max_pos < temp_pos)
                return EOF;

            return pos + delta;
        };

        inline size_t dec(size_t delta) const
        {
            if (eof(pos))
                return EOF;

            size_t temp_pos = pos - delta;
            if (temp_pos > pos || temp_pos < min_pos)
                return EOF;

            return pos - delta;
        };
    public:


        Position()
        {
        }

        Position(int p)
        {
            pos = p;
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

        inline Position& operator = (size_t p)
        {
            pos = p;
            return *this;
        }

        inline operator size_t () const { return pos; }
        inline operator std::string () const { return to_string(pos); }

        inline Position operator + (size_t delta)
        {
            Position ret(*this);
            return  ret.inc(delta);
        }

        inline Position operator - (size_t delta)
        {
            Position ret(*this);
            return  ret.dec(delta);
        }

        inline Position& operator ++()
        {
            pos = inc(1);
            return *this;
        }

        inline Position& operator --()
        {
            pos = dec(1);
            return *this;
        }

        inline Position& operator +=(size_t delta)
        {
            pos = inc(delta);
            return *this;
        }

        inline Position& operator -=(size_t delta)
        {
            pos = dec(delta);
            return *this;
        }

        inline Position& operator +=(const Position& delta)
        {
            pos = inc(delta.pos);
            return *this;
        }

        inline Position& operator -=(const Position& delta)
        {
            pos = dec(delta.pos);
            return *this;
        }

        inline void set_min(size_t m) { min_pos = m; }
        inline void set_max(size_t m) { max_pos = m; }

        inline size_t get_min() const { return min_pos; }
        inline size_t get_max() const { return max_pos; }

        inline void set_eof() { pos = EOF; }
        inline bool eof() const { return eof(pos); }
        inline bool eof(size_t p) const { return !(min_pos <= p && p <= max_pos) || p == EOF; }

        inline void set_to_min() { pos = min_pos; }
        inline void set_to_max() { pos = max_pos; }

        inline void reset()
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

    const std::string space_pattern = " \t\n\r;:.,?!";
    using posMap = map<std::string, Position>;
    using strVector = vector<std::string>;
    using find_func = function<size_t(const std::string&, const std::string&, size_t)>;

    class Cursor
    {
    protected:
        posMap labels;
        text::Text& text;
        Position pos;
        std::string name;

        Position rfind_first_of(const std::string& pattern, const Position& p);
        void check_cursor(const Cursor& c);
        Position multi_find(const vector<std::string>& pattern, find_func func, size_t& idx);
        int find_count(const std::string& pattern, const Position& from_pos, const Position& until_pos, find_func func);

    public:
        Cursor(text::Text& t);
        Cursor(const Cursor& c);
        Cursor(Cursor& c);

        Cursor(text::Text& t, const std::string& pattern, find_func func);
        Cursor(const Cursor& c, const std::string& pattern, find_func func);

        Cursor& inc(size_t);
        Cursor& dec(size_t);

        Cursor& operator += (const Cursor&);
        Cursor& operator += (size_t);
        Cursor operator + (const Cursor&);
        Cursor operator + (size_t);
        Cursor& operator ++ ();

        Cursor& operator -= (const Cursor&);
        Cursor& operator -= (size_t);
        Cursor operator - (const Cursor&);
        Cursor operator - (size_t);
        Cursor& operator -- ();

        Cursor& operator = (const Cursor&);
        Cursor& operator = (size_t);

        bool operator == (const Cursor&);
        bool operator != (const Cursor&);
        bool operator < (const Cursor&);
        bool operator <= (const Cursor&);
        bool operator >= (const Cursor&);
        bool operator > (const Cursor&);

        bool operator == (size_t);
        bool operator != (size_t);
        bool operator < (size_t);
        bool operator <= (size_t);
        bool operator >= (size_t);
        bool operator > (size_t);

        Cursor& move_to(size_t p);
        Cursor& move_to(const std::string& pattern, find_func func);
        Cursor& move_to(const vector<std::string>& pattern, find_func func);
        // pattern1 = {, pattern2 =}, moves pos to } ; from here ->{ {abcd} ->}<- to here }, count { must be == count }
        Cursor& move_to(const std::string& pattern1, const std::string& pattern2, find_func func);

        Cursor& move_while(const std::string& pattern);
        Cursor& move_until(const std::string& pattern);

        Cursor& move_to_end(const std::string& pattern, find_func func);
        Cursor& move_to_end(const vector<std::string>& pattern, find_func func);

        Cursor& next_word(const std::string& pattern);
        Cursor& next_word();

        Cursor& move_to_word_begin(const std::string& pattern);
        Cursor& move_to_word_begin();

        Cursor& move_to_word_end(const std::string& pattern);
        Cursor& move_to_word_end();

        Cursor& goto_line(size_t line_num);
        Cursor& goto_col(size_t col);

        Cursor& next_line();
        Cursor& next_line(size_t count);

        Cursor& prev_line();
        Cursor& prev_line(size_t count);

        Cursor& move_to_line_begin();
        Cursor& move_to_line_end();

        Cursor& begin();
        Cursor& end();

        void label(const std::string& key);
        bool goto_label(const std::string& key);
        const Position& operator [](const std::string& key);
        size_t label_size();

        bool eof() const;
        bool eof(size_t p) const;

        inline const Position& get_pos() const
        {
            return pos;
        }

        inline const text::Text& get_text() const
        {
            return text;
        }

        inline const std::string& get_string() const
        {
            return text;
        }

        std::string to_string() const;

        inline void set_name(const std::string& n)
        {
            name = n;
        }

        inline const std::string& get_name() const
        {
            return name;
        }

        inline operator bool() const
        {
            return eof() == false;
        }

        bool set_range(size_t min, size_t max);
        void set_min_range(size_t m);
        void set_max_range(size_t m);

        size_t get_min_range() const;
        size_t get_max_range() const;

        void reset();

        inline void set_eof()
        {
            pos = EOF;
        }
    };
}