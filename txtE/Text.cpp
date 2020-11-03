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

    void Text::load(const fs::path& fn)
    {
        file_name = fn;
        TRACE_FUNC;
        TRACE_OUT << "filename = " << file_name TRACE_END;
        text.clear();

        ifstream in(file_name, ios::in | ios::binary | ios::ate);
        if (!in)
        {
            throw runtime_error("Error loading file " + file_name.string() + " : " + strerror(errno));
        }

        ifstream::pos_type fileSize = in.tellg();
        if (fileSize == ifstream::pos_type(-1))
        {
            throw runtime_error("File size error " + file_name.string() + " : " + strerror(errno));
        }
        
        in.seekg(0, ios::beg);

        vector<char> bytes(fileSize);
        in.read(&bytes[0], fileSize);

        text = string(&bytes[0], fileSize);

        original_endl = find_endl_type();
        reset_endl(text);
        changed = false;
    }

    void Text::write(const fs::path& fn, el_types t)
    {
        if (file_name != fn && fn.empty() == false)
        {
            file_name = fn;
            changed = true;
        }
        else
        {
            fs::path backup = file_name;
            backup.replace_extension(".bak");
            fs::copy(file_name, backup, fs::copy_options::overwrite_existing);
        }

        if (original_endl != t)
            changed = true;

        TRACE_FUNC;
        TRACE_OUT << "filename = " << file_name TRACE_END;

        ofstream out(file_name, ios::out | ios::binary);
        if (!out)
        {
            throw runtime_error("Error writing file " + file_name.string() + " : " + strerror(errno));
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

        changed = false;
    }

    void Text::write()
    {
        write("", original_endl);
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
            end = text.size() - CHAR_SIZE;
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

    bool Text::set(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof() || str.empty())
            return false;

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;
        for (size_t i = 0; i < str.size() && (pos.get_pos() + i) < text.size(); i++)
        {
            if (pos.eof(pos.get_pos() + i))
                return false;
            text[pos.get_pos() + i] = str[i];
        }
        changed = true;

        return true;
    }

    bool Text::set_line(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);
        if (pos.eof() || str.empty())
            return false;

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        size_t spos = text.rfind(ENDL, pos.get_pos());
        spos = spos != string::npos ? spos + CHAR_SIZE : 0;
        erase_line(pos);

        if (pos.eof(spos) == false)
        {
            text.insert(spos, str);
            changed = true;

            return true;
        }
        return false;
    }

    bool Text::insert(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof() || str.empty())
            return false;

        TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

        text.insert(pos.get_pos(), str);
        changed = true;

        return true;
    }

    bool Text::insert_line(const Cursor& pos, const string& str)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof() || str.empty())
            return false;

        size_t p = text.find(ENDL, pos.get_pos());

        if (pos.eof(p) == false)
        {
            text.insert(p, ENDL + str);
            changed = true;

            return true;
        }

        return false;
    }

    bool Text::add(const Text& t)
    {
        TRACE_FUNC;
        text += t.text;
        changed = true;

        return true;
    }

    bool Text::add(const string& str)
    {
        TRACE_FUNC;
        text += str;
        changed = true;

        return true;
    }

    size_t Text::size() const
    {
        return text.size();
    }

    bool Text::erase(const Cursor& pos, size_t count)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof() || pos.eof(pos.get_pos() + count))
            return false;

        text.erase(pos.get_pos(), count);
        changed = true;

        return true;
    }

    bool Text::erase_between(const Cursor& from, const Cursor& to)
    {
        TRACE_FUNC;
        check_cursor(from);
        check_cursor(to);

        if (from.eof() || to.eof() || to.get_pos() < from.get_pos())
            return false;

        text.erase(from.get_pos(), to.get_pos() - from.get_pos() + 1);
        changed = true;

        return true;
    }

    bool Text::erase_line(const Cursor& pos)
    {
        TRACE_FUNC;
        check_cursor(pos);

        if (pos.eof())
            return false;

        size_t spos = text.rfind(ENDL, pos.get_pos());
        size_t epos = text.find(ENDL, pos.get_pos());

        spos = spos != string::npos ? spos + CHAR_SIZE : 0;
        epos = epos != string::npos ? epos : text.size() - 1;

        if (pos.eof(spos) == false && pos.eof(epos) == false && spos <= epos)
        {
            text.erase(spos, epos - spos);
            changed = true;

            return true;
        }
        return false;
    }

    Cursor Text::diff(const Cursor& pos, const string& text1, string& result)
    {
        TRACE_FUNC;
        //TODO
        result.clear();
        Cursor found_cur(pos);
        found_cur.set_eof();

        return found_cur;
    }

    Cursor Text::diff(const Cursor& pos, const Text& text1, string& result)
    {
        TRACE_FUNC;
        //TODO
        result.clear();

        Cursor found_cur(pos);
        found_cur.set_eof();

        return found_cur;
    }

    bool Text::clear()
    {
        TRACE_FUNC;
        text.clear();
        changed = true;

        return true;
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
        changed = true;

        return *this;
    }

    Text& Text::operator = (const Text& t)
    {
        text = t.text;
        original_endl = t.original_endl;
        changed = true;

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
        if (pos1.eof() || text.size() <= pos1.get_pos())
            return "";

        if (pos2.eof())
            pos2 = text.size() - 1;

        if (pos2.get_pos() < pos1.get_pos() || text.size() < pos2.get_pos())
            return "";

        return text.substr(pos1.get_pos(), (pos2.get_pos() - pos1.get_pos()) + 1);
    }

    string Text::substr(const Cursor& pos, size_t count) const
    {
        if (pos.eof())
            return "";

        if (pos.eof(pos.get_pos() + count - sizeof(string::value_type)))
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

    m->add(chaiscript::fun(static_cast<void(Text::*)()>(&Text::write)), "write");
    m->add(chaiscript::fun(static_cast<void(Text::*)(const fs::path&, el_types)>(&Text::write)), "write");

    m->add(chaiscript::fun(&Text::get), "get");
    m->add(chaiscript::fun(&Text::get_line), "get_line");
    m->add(chaiscript::fun(&Text::get_word), "get_word");
    m->add(chaiscript::fun(&Text::get_to_endl), "get_to_endl");
    m->add(chaiscript::fun(&Text::get_between), "get_between");

    m->add(chaiscript::fun(&Text::set), "set");
    m->add(chaiscript::fun(&Text::set_line), "set_line");
    m->add(chaiscript::fun(&Text::insert), "insert");
    m->add(chaiscript::fun(&Text::insert_line), "insert_line");
    m->add(chaiscript::fun(static_cast<bool (Text::*)(const string&)>(&Text::add)), "add");
    m->add(chaiscript::fun(static_cast<bool (Text::*)(const Text&)>(&Text::add)), "add");
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