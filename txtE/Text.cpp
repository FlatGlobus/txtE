#include "Text.h"
#include "Cursor.h"
#include <strtk/strtk.hpp>
#include "StrFun.h"
#include "util.h"

extern bool enable_trace;
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

    ifstream in(file_name, ios::in | ios::binary);
    if (!in)
    {
        TRACE_OUT << "error opening file" TRACE_END;
        return 0;
    }

    string temp;
    while (getline(in, temp))
    {
        text += temp;
    }

    return text.size();
}

size_t Text::write(const string& file_name)
{
    TRACE_FUNC;
    TRACE_OUT << "filename = " << file_name TRACE_END;

    ofstream out(file_name, ios::out);
    if (!out)
    {
        TRACE_OUT << "error writing file" TRACE_END;
        return 0;
    }

    ostringstream contents;
    out << text;
    out.close();
    return text.size();
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

    spos = spos != string::npos ? spos + 1 : spos;
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
        text.insert(pos, "\n" + str);
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

Cursor Text::erase_line(Cursor& pos)
{
    TRACE_FUNC;
    check_cursor(pos);

    Cursor ret(pos);
    size_t spos = text.rfind(ENDL, pos);
    size_t epos = text.find(ENDL, pos);

    spos = spos != string::npos ? spos + 1 : spos;
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

DECLARE_MODULE(TEXT)
m->add(chaiscript::fun(&Text::load), "load");
m->add(chaiscript::fun(&Text::write), "write");

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
END_DECLARE(TEXT)
