#include "Text.h"
#include "Cursor.h"
#include <strtk/strtk.hpp>
#include "StrFun.h"
#include "util.h"
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <algorithm>

namespace fs = std::filesystem;

void reset_endl(std::string& str);
void set_endl(std::string& str, el_types t);

extern bool enable_trace;
//////////////////////////////////////////////////////////////////////////
Text::Text():original_endl(el_types::elWin), changed(false)
{

}

Text::Text(const string &t): text(t)
{
    original_endl = find_endl_type();
}

Text::~Text()
{
}

void Text::check_cursor(const Cursor& c)
{
    if (AreEqual(c.get_text(), text) == false)
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
        std::cerr << "Error loading file " << file_name <<" : " << strerror(errno) << std::endl;
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

    changed = false;

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

    string str;

    if (is_eof(start.get_pos() + count) == false)
    {
        str = text.substr(start, count);
    }

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    return str;
}

string Text::get_to_endl(const Cursor& start)
{
    TRACE_FUNC;
    check_cursor(start);

    string str;
    size_t pos = text.find(ENDL, start);

    if (pos != string::npos)
    {
        str = text.substr(start, pos - start);
    }

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    return str;
}

string Text::get_line(const Cursor& start)
{
    TRACE_FUNC;
    check_cursor(start);

    string str;
    size_t spos = text.rfind(ENDL, start);
    size_t epos = text.find(ENDL, start);

    spos = spos != string::npos ? spos + ENDL_SIZE : spos;
    epos = epos != string::npos ? epos + ENDL_SIZE : epos;

    if (epos != string::npos && spos == string::npos)
        spos = 0;
    else
        if (epos == string::npos && spos != string::npos)
            epos = text.size() - ENDL_SIZE;
        else
            if (epos == spos)
            {
                spos -= 1;
                //epos += ENDL_SIZE;
            }

    if (spos != string::npos && epos != string::npos && (spos <= epos))
    {
        str = text.substr(spos, epos - spos);
    }

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    return str;
}

string Text::get_word(const Cursor& pos)
{
    TRACE_FUNC;
    check_cursor(pos);

    Cursor s(*this);
    Cursor e(*this);
    string str;

    s = pos;
    e = pos;
    s.move_to_begin_of_word();
    e.move_to_end_of_word();

    if (s.is_eof() == false && e.is_eof() == false)
    {
        str = text.substr(s, e - s);
    }

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    return str;
}

string Text::get_between(const Cursor& start, const Cursor& end)
{
    TRACE_FUNC;
    check_cursor(start);
    check_cursor(end);

    string str;

    if (start.check_range() && end.check_range() && start < end)
    {
        str = text.substr(start, end - start);
    }

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    return str;
}

void Text::set(const Cursor& pos, const string& str)
{
    TRACE_FUNC;
    check_cursor(pos);

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;
    for (size_t i = 0; i < str.size() && (pos + i) < text.size(); i++)
    {
        text[pos + i] = str[i];
        changed = true;
    }
}

void Text::set_line(const Cursor& pos, const string& str)
{
    TRACE_FUNC;
    check_cursor(pos);

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;
    Cursor start_line = erase_line(pos);
    if (start_line.is_eof() == false)
    {
        insert(start_line, str);
    }
}

void Text::insert(const Cursor& start, const string& str)
{
    TRACE_FUNC;
    check_cursor(start);

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    text.insert(start, str);
    changed = true;
}

void Text::insert_line(const Cursor& start, const string& str)
{
    TRACE_FUNC;
    check_cursor(start);

    size_t pos = text.find(ENDL, start);

    if (pos != string::npos)
    {
        text.insert(pos, ENDL + str);
        changed = true;
    }
}

void Text::add(const Text& t)
{
    TRACE_FUNC;
    text += t.text;
    changed = true;
}

void Text::add(const string& str)
{
    TRACE_FUNC;
    text += str;
    changed = true;
}

size_t Text::size() const 
{
    return text.size();
}

void Text::erase(const Cursor& pos, size_t count)
{
    TRACE_FUNC;
    check_cursor(pos);

    text.erase(pos, count);
    changed = true;
}

void Text::erase_between(const Cursor& from, const Cursor& to)
{
    TRACE_FUNC;
    check_cursor(from);
    check_cursor(to);

    text.erase(std::min((size_t)from, (size_t)to), std::max((size_t)from, (size_t)to) - std::min((size_t)from, (size_t)to));
    changed = true;
}

Cursor Text::erase_line(const Cursor& pos)
{
    TRACE_FUNC;
    check_cursor(pos);

    Cursor ret(pos);
    size_t spos = text.rfind(ENDL, pos);
    size_t epos = text.find(ENDL, pos);

    spos = spos != string::npos ? spos + ENDL_SIZE : spos;
    if (spos != string::npos && epos != string::npos && (spos <= epos))
    {
        text.erase(spos, epos - spos);
        ret = spos;
        changed = true;
    }
    else
    {
        ret = string::npos;
    }

    return ret;
}

Cursor Text::diff(const Cursor& start, const string& text1, string & result)
{
    TRACE_FUNC;

    result.clear();
    Cursor found_cur(start);
    found_cur.set_eof();

    size_t i = 0;
    size_t found_idx = string::npos;

    for (;start + i < text.size() && i < text1.size(); ++i)
    {
        if (text[start + i] == text1[i] && found_idx != string::npos)
        {
            break;
        }

        if (text[start + i] != text1[i])
        {
            result += text1[i];
            found_cur = found_idx = start + i;
        }
    }

    return found_cur;
}

Cursor Text::diff(const Cursor& start, const Text& text1, string& result)
{
    TRACE_FUNC;

    result.clear();

    Cursor found_cur(start);
    found_cur.set_eof();

    size_t i = 0;
    size_t found_idx = string::npos;

    for (; start + i < text.size() && start + i < text1.text.size(); ++i)
    {
        if (text[start + i] == text1.text[i] && found_idx != string::npos)
        {
            break;
        }

        if (text[start + i] != text1.text[i])
        {
            result += text1.text[i];
            found_cur = found_idx = start + i;
        }
    }

    return found_cur;
}

void Text::clear()
{
    TRACE_FUNC;
    text.clear();
    changed = true;
}

bool Text::is_eof(size_t p)
{
    bool ret = text.size() <= (size_t)p || p == string::npos;
    if (enable_trace)
    {
        cout << "Text::is_eof(size_t " << p << ") returned " << ret << endl;
    }

    return ret;
}

el_types Text::find_endl_type()
{
    size_t p = text.find("\n", 0);

    if (p == std::string::npos)
    {
        return el_types::elNone;
    }

    if (p > 1 && text[p - 1] == '\r' )
    {
        return el_types::elWin;
    }

    return el_types::elUnix;
}
//////////////////////////////////////////////////////////////////////////
void reset_endl(std::string & str)
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
DECLARE_MODULE(TEXT)
m->add(chaiscript::fun(&Text::load), "load");

m->add(chaiscript::fun(static_cast<size_t (Text::*)(const fs::path&)>(&Text::write)), "write");
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
m->add(chaiscript::fun(&Text::erase_line), "erase_line");
m->add(chaiscript::fun(&Text::clear), "clear");

m->add(chaiscript::fun(static_cast<Cursor(Text::*)(const Cursor&, const string&, string&)>(&Text::diff)), "diff");
m->add(chaiscript::fun(static_cast<Cursor(Text::*)(const Cursor&, const Text&, string&)>(&Text::diff)), "diff");

m->add(chaiscript::constructor<Text()>(), "Text");
m->add(chaiscript::constructor<Text(const string&)>(), "Text");
m->add(chaiscript::user_type<Text>(), "Text");

m->add(chaiscript::fun(&Text::get_endl_type), "get_endl_type");

m->add(chaiscript::type_conversion<Text, const std::string&>([](const Text& t) { return (const std::string&)t;}));

chaiscript::utility::add_class<el_types>(*m,   "el_types",
    {
        { el_types::elNone, "elNone" },
        { el_types::elWin, "elWin" },
        { el_types::elUnix, "elUnix" },
        { el_types::elMac, "elMac" }
    }
);

END_DECLARE(TEXT)