#include "Cursor.h"
#include "Text.h"
#include <strtk/strtk.hpp>
#include "StrFun.h"
#include "util.h"
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <algorithm>

namespace fs = std::filesystem;
using namespace cursor;

extern bool enable_trace;

namespace text
{
    void reset_endl(std::string& str);
    void set_endl(std::string& str, el_types t);

    //////////////////////////////////////////////////////////////////////////
    Text::Text() :original_endl(el_types::elWin)
    {

    }

    Text::Text(const string& t) : text(t)
    {
        original_endl = find_endl_type();
    }

    Text::~Text()
    {
    }

    void Text::check_cursor(const Cursor& c)
    {
        if (AreEqual(c.get_text(), *this) == false)
        {
            throw runtime_error("Cursor object : \"" + c.get_name() + "\" is created for other Text object.");
        }
    }

    size_t Text::load(const fs::path& file_name)
    {
        TRACE_FUNC;
        TRACE_OUT << "filename = " << file_name TRACE_END;
        text.clear();

        ifstream in(file_name, ios::in | ios::binary | ios::ate);
        if (!in)
        {
            std::cerr << "Error loading file " << file_name << " : " << strerror(errno) << std::endl;
            return 0;
        }

        ifstream::pos_type fileSize = in.tellg();
        if (fileSize < 0)
            return 0;

        in.seekg(0, ios::beg);

        vector<char> bytes(fileSize);
        in.read(&bytes[0], fileSize);

        text = string(&bytes[0], fileSize);

        original_endl = find_endl_type();
        reset_endl(text);

        return text.size();
    }

    size_t Text::write(const fs::path& file_name, el_types t)
    {
        TRACE_FUNC;
        TRACE_OUT << "filename = " << file_name TRACE_END;

        ofstream out(file_name, ios::out | ios::binary);
        if (!out)
        {
            std::cerr << "Error writing file " << file_name << " : " << strerror(errno) << std::endl;
            return 0;
        }

        fs::resize_file(file_name, 0);
        out.seekp(0);

        if (t == el_types::elWin)
        {
            set_endl(text, t);
        }

        ostringstream contents;
        out << text;
        out.close();

        if (t == el_types::elWin)
        {
            reset_endl(text);
        }

        return text.size();
    }

    size_t Text::write(const fs::path& file_name)
    {
        return write(file_name, original_endl);
    }

    string Text::get(const Cursor& start, size_t count)
    {
        TRACE_FUNC;
        check_cursor(start);

        string str = substr(start, count);

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        return str;
    }

    string Text::get_to_endl(const Cursor& start)
    {
        TRACE_FUNC;
        check_cursor(start);

        Cursor end(start);
        end = text.find(ENDL, start.get_pos());
        if (end.eof())
        {
            end = text.size();
            end -= ENDL_SIZE;
        }

        string str = substr(start, end);

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        return str;
    }

    string Text::get_line(const Cursor& start)
    {
        TRACE_FUNC;
        check_cursor(start);

        string str;

        if (start.eof())
            return str;

        Cursor spos = start;
        Cursor epos = start;

        if (spos.move_to_line_begin() && epos.move_to_line_end())
        {
            str = substr(spos, epos);
        }

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        return str;
    }

    string Text::get_word(const Cursor& pos)
    {
        TRACE_FUNC;
        check_cursor(pos);

        string str;

        if (pos.eof())
            return str;

        Cursor s = pos;
        Cursor e = pos;

        if (s.move_to_word_begin() && e.move_to_word_end())
            str = substr(s, e);

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        return str;
    }

    string Text::get_between(const Cursor& start, const Cursor& end)
    {
        TRACE_FUNC;
        check_cursor(start);
        check_cursor(end);

        string str = substr(start, end);

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        return str;
    }

    void Text::set(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof())
            return;

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;
        for (size_t i = 0; i < str.size() && (pos.get_pos() + i) < text.size(); i++)
        {
            if (pos.eof(pos.get_pos() + i))
                break;
            text[pos.get_pos() + i] = str[i];
        }
    }

    void Text::set_line(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);
        if (pos.eof())
            return;

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        size_t spos = text.rfind(ENDL, pos.get_pos());
        spos = spos != string::npos ? spos + ENDL_SIZE : 0;
        erase_line(pos);

        if (pos.eof(spos) == false)
        {
            text.insert(spos, str);
        }
    }

    void Text::insert(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof())
            return;

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        text.insert(pos.get_pos(), str);
    }

    void Text::insert_line(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof())
            return;

        size_t p = text.find(ENDL, pos.get_pos());

        if (pos.eof(p) == false)
        {
            text.insert(p, ENDL + str);
        }
    }

    void Text::add(const Text& t)
    {
        TRACE_FUNC;
        text += t.text;
    }

    void Text::add(const string& str)
    {
        TRACE_FUNC;
        text += str;
    }

    size_t Text::size() const
    {
        return text.size();
    }

    void Text::erase(const Cursor& pos, size_t count)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof() || pos.eof(pos.get_pos() + count))
            return;

        text.erase(pos.get_pos(), count);
    }

    void Text::erase_between(const Cursor& from, const Cursor& to)
    {
        TRACE_FUNC;
        check_cursor(from);
        check_cursor(to);

        if (from.eof() || to.eof() || to < from)
            return;

        text.erase(from.get_pos(), to.get_pos() - from.get_pos() + 1);
    }

    void Text::erase_line(const Cursor& pos)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof())
            return;

        size_t spos = text.rfind(ENDL, pos.get_pos());
        size_t epos = text.find(ENDL, pos.get_pos());

        spos = spos != string::npos ? spos + ENDL_SIZE : 0;
        epos = epos != string::npos ? epos : text.size() - 1;

        if (pos.eof(spos) == false && pos.eof(epos) == false && spos <= epos)
        {
            text.erase(spos, epos - spos);
        }
    }

    Cursor Text::diff(const Cursor& pos, const string& text1, string& result)
    {
        TRACE_FUNC;

        result.clear();
        Cursor found_cur(pos);
        found_cur.set_eof();

        return found_cur;
    }

    Cursor Text::diff(const Cursor& pos, const Text& text1, string& result)
    {
        TRACE_FUNC;

        result.clear();

        Cursor found_cur(pos);
        found_cur.set_eof();

        return found_cur;
    }

    void Text::clear()
    {
        TRACE_FUNC;
        text.clear();
    }

    el_types Text::find_endl_type()
    {
        size_t p = text.find("\n", 0);

        if (p == std::string::npos)
        {
            return el_types::elNone;
        }

        if (p > 1 && text[p - 1] == '\r')
        {
            return el_types::elWin;
        }

        return el_types::elUnix;
    }

    Text& Text::operator = (const string& t)
    {
        text = t;
        original_endl = find_endl_type();
        reset_endl(text);
        return *this;
    }

    Text& Text::operator = (const Text& t)
    {
        text = t.text;
        original_endl = t.original_endl;

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    void reset_endl(std::string& str)
    {
        boost::erase_all(str, "\r");
    }

    void set_endl(std::string& str, el_types t)
    {
        reset_endl(str);
        switch (t)
        {
        case el_types::elUnix:
        case el_types::elMac:
            break;
        case el_types::elNone:
            break;
        case el_types::elWin:
        default:
            boost::replace_all(str, ENDL, "\r\n");
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Text::contains(const std::string& pattern)
    {
        TRACE_FUNC;
        return text.find(pattern, 0) != string::npos;
    }
    //////////////////////////////////////////////////////////////////////////
    string Text::substr(Cursor pos1, Cursor pos2) const
    {
        if (pos1.eof() || text.size() <= pos1)
            return "";

        if (pos2.eof())
            pos2 = text.size() - 1;

        if (pos2 < pos1 || text.size() < pos2)
            return "";

        return text.substr(pos1.get_pos(), (pos2.get_pos() - pos1.get_pos()) + 1);
    }

    string Text::substr(const Cursor& pos, size_t count) const
    {
        if (pos.eof())
            return "";

        if (pos.eof(pos + count))
            return "";

        return text.substr(pos.get_pos(), count);
    }

    char Text::substr(const Cursor& pos) const
    {
        if (pos.eof())
            return '\0';

        return text[pos.get_pos()];
    }

    string Text::substr(Position pos1, Position pos2) const
    {
        if (pos1.eof() || text.size() <= pos1)
            return "";

        if (pos2.eof())
            pos2 = text.size() - 1;

        if (pos2 < pos1 || text.size() < pos2)
            return "";

        return text.substr(pos1, (size_t)(pos2 - pos1) + 1);
    }

    string Text::substr(const Position& pos, size_t count) const
    {
        if (pos.eof() || text.size() <= pos)
            return "";

        if (text.size() < (pos + count) || pos.eof(pos + count))
            return "";

        return text.substr(pos, count);
    }

    char Text::substr(const Position& pos) const
    {
        if (pos.eof() || text.size() <= pos)
            return '\0';

        return text[pos];
    }

    //////////////////////////////////////////////////////////////////////////
    DECLARE_MODULE(TEXT)
    m->add(chaiscript::fun(&Text::load), "load");

    m->add(chaiscript::fun(static_cast<size_t(Text::*)(const fs::path&)>(&Text::write)), "write");
    m->add(chaiscript::fun(static_cast<size_t(Text::*)(const fs::path&, el_types)>(&Text::write)), "write");

    m->add(chaiscript::fun(&Text::get), "get");
    m->add(chaiscript::fun(&Text::get_line), "get_line");
    m->add(chaiscript::fun(&Text::get_word), "get_word");
    m->add(chaiscript::fun(&Text::get_to_endl), "get_to_endl");
    m->add(chaiscript::fun(&Text::get_between), "get_between");

    m->add(chaiscript::fun(&Text::set), "set");
    m->add(chaiscript::fun(&Text::set_line), "set_line");
    m->add(chaiscript::fun(&Text::insert), "insert");
    m->add(chaiscript::fun(&Text::insert_line), "insert_line");
    m->add(chaiscript::fun(static_cast<void (Text::*)(const string&)>(&Text::add)), "add");
    m->add(chaiscript::fun(static_cast<void (Text::*)(const Text&)>(&Text::add)), "add");
    m->add(chaiscript::fun(&Text::size), "size");
    m->add(chaiscript::fun(&Text::erase), "erase");
    m->add(chaiscript::fun(&Text::erase_between), "erase_between");
    m->add(chaiscript::fun(&Text::erase_line), "erase_line");
    m->add(chaiscript::fun(&Text::clear), "clear");
    m->add(chaiscript::fun(&Text::contains), "contains");

    m->add(chaiscript::fun(static_cast<Cursor(Text::*)(const Cursor&, const string&, string&)>(&Text::diff)), "diff");
    m->add(chaiscript::fun(static_cast<Cursor(Text::*)(const Cursor&, const Text&, string&)>(&Text::diff)), "diff");

    m->add(chaiscript::constructor<Text()>(), "Text");
    m->add(chaiscript::constructor<Text(const string&)>(), "Text");
    m->add(chaiscript::user_type<Text>(), "Text");

    m->add(chaiscript::fun(static_cast<Text& (Text::*)(const string&)>(&Text::operator =)), "=");
    m->add(chaiscript::fun(static_cast<Text& (Text::*)(const Text&)>(&Text::operator =)), "=");

    m->add(chaiscript::fun(&Text::get_endl_type), "get_endl_type");

    m->add(chaiscript::type_conversion<Text, const std::string&>([](const Text& t) { return (const std::string&)t; }));

    chaiscript::utility::add_class<el_types>(*m, "el_types",
        {
            { el_types::elNone, "elNone" },
            { el_types::elWin, "elWin" },
            { el_types::elUnix, "elUnix" },
            { el_types::elMac, "elMac" }
        }
    );

    END_DECLARE(TEXT)
}