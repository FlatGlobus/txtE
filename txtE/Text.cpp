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
Text::Text()
{

}

Text::~Text()
{

}

void Text::check_cursor(Cursor& c)
{
    if (AreEqual(c.get_text(), text) == false)
    {
        throw runtime_error("Cursor object : \"" + c.get_name() + "\" is created for other Text object.");
    }
}

size_t Text::load(const string& file_name)
{
    TRACE_FUNC;
    TRACE_OUT << "filename = " << file_name TRACE_END;
    text.clear();

    ifstream in(file_name, ios::in | ios::binary | ios::ate);
    if (!in)
    {
        std::cerr << "Error loading file " << file_name <<" : " << strerror(errno);
        return 0;
    }

    ifstream::pos_type fileSize = in.tellg();
    if (fileSize < 0)
        return 0;

    in.seekg(0, ios::beg);

    vector<char> bytes(fileSize);
    in.read(&bytes[0], fileSize);

    text = string(&bytes[0], fileSize);
    reset_endl(text);

    return text.size();
}

size_t Text::write(const string& file_name, el_types t)
{
    TRACE_FUNC;
    TRACE_OUT << "filename = " << file_name TRACE_END;

    ofstream out(file_name, ios::out | ios::binary);
    if (!out)
    {
        std::cerr << "Error writing file " << file_name << " : " << strerror(errno);
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

size_t Text::write(const string& file_name)
{
    return write(file_name, el_types::elWin);
}

string Text::get(Cursor& start, size_t count)
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

string Text::get_to_endl(Cursor& start)
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

string Text::get_line(Cursor& start)
{
    TRACE_FUNC;
    check_cursor(start);

    string str;
    size_t spos = text.rfind(ENDL, start);
    size_t epos = text.find(ENDL, start);

    spos = spos != string::npos ? spos + ENDL_SIZE : spos;
    if (spos != string::npos && epos != string::npos && (spos <= epos))
    {
        str = text.substr(spos, epos - spos);
    }

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    return str;
}

string Text::get_word(Cursor& pos)
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

string Text::get_between(Cursor& start, Cursor& end)
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

void Text::set(Cursor& pos, const string& str)
{
    TRACE_FUNC;
    check_cursor(pos);

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;
    for (size_t i = 0; i < str.size() && (pos + i) < text.size(); i++)
        text[pos + i] = str[i];
}

void Text::set_line(Cursor& pos, const string& str)
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

void Text::insert(Cursor& start, const string& str)
{
    TRACE_FUNC;
    check_cursor(start);

    TRACE_OUT << "text = \"" << str << "\"" TRACE_END;

    text.insert(start, str);
}

void Text::insert_line(Cursor& start, const string& str)
{
    TRACE_FUNC;
    check_cursor(start);

    size_t pos = text.find(ENDL, start);

    if (pos != string::npos)
    {
        text.insert(pos, ENDL + str);
    }
}

void Text::add(const Text& t)
{
    TRACE_FUNC;

    //TRACE_OUT << "text = \"" << t.text << "\"" TRACE_END;

    text += t.text;
}

void Text::add(const string& str)
{
    TRACE_FUNC;
    text += str;
}

size_t Text::size()
{
    return text.size();
}

void Text::erase(Cursor& pos, size_t count)
{
    TRACE_FUNC;
    check_cursor(pos);

    text.erase(pos, count);
}

void Text::erase_between(Cursor& from, Cursor& to)
{
    TRACE_FUNC;
    check_cursor(from);
    check_cursor(to);

    text.erase(std::min((size_t)from, (size_t)to), std::max((size_t)from, (size_t)to) - std::min((size_t)from, (size_t)to));
}

Cursor Text::erase_line(Cursor& pos)
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
    }
    else
    {
        ret = string::npos;
    }

    return ret;
}

//TODO diff
Cursor Text::diff(string& str, Cursor& c)
{
    TRACE_FUNC;
    Cursor cur(str);

    return cur;
}

Cursor Text::diff(Text& t, Cursor& pos)
{
    return diff(t.text, pos);
}

void Text::clear()
{
    TRACE_FUNC;
    text.clear();
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
    case el_types::elWin:
    default:
        boost::replace_all(str, ENDL, "\r\n");
    }
}

//////////////////////////////////////////////////////////////////////////
DECLARE_MODULE(TEXT)
m->add(chaiscript::fun(&Text::load), "load");

m->add(chaiscript::fun(static_cast<size_t (Text::*)(const string&)>(&Text::write)), "write");
m->add(chaiscript::fun(static_cast<size_t(Text::*)(const string&, el_types)>(&Text::write)), "write");

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

m->add(chaiscript::constructor<Text()>(), "Text");
m->add(chaiscript::user_type<Text>(), "Text");

m->add(chaiscript::fun(&reset_endl), "reset_endl");

chaiscript::utility::add_class<el_types>(*m,   "el_types",
    {
        { el_types::elWin, "elWin" },
        { el_types::elUnix, "elUnix" },
        { el_types::elMac, "elMac" }
    }
);


END_DECLARE(TEXT)