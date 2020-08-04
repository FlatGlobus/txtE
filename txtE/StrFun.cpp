#include "StrFun.h"
#include <regex>
#include "Text.h"
#include "util.h"
#include <ctype.h>

////////////////////////////////////////////////////////////
namespace strfun
{
    
    size_t regex_search(const string& str, const string& pattern, size_t pos)
    {
        TRACE_FUNC;
        TRACE_OUT << "pattern = \"" << pattern << "\"" TRACE_END;

        regex re(pattern);
        smatch sm;
        if (regex_search(str.begin() + pos, str.end(), sm, re) == true)
        {
            TRACE_OUT << "found at = " << sm.position(0) TRACE_END;
            return sm.position(0);
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    size_t find(const string& str, const string& pattern, size_t pos)
    {
        TRACE_FUNC;
        size_t p = str.find(pattern, pos);
        
        if (p != string::npos)
        {
            TRACE_OUT << "found at = " << p TRACE_END;
            return p;
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    size_t rfind(const string& str, const string& pattern, size_t pos)
    {
        TRACE_FUNC;
        size_t p = str.rfind(pattern, pos);
        if (p != string::npos)
        {
            TRACE_OUT << "found at = " << p TRACE_END;
            return p;
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    size_t find_first_not_of(const string& str, const string& pattern, size_t pos)
    {
        TRACE_FUNC;
        size_t p = str.find_first_not_of(pattern, pos);
        if (p != string::npos)
        {
            TRACE_OUT << "found at = " << p TRACE_END;
            return p;
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    size_t find_last_of(const string& str, const string& pattern, size_t pos)
    {
        TRACE_FUNC;
        size_t p = str.find_last_of(pattern, pos);
        if (p != string::npos)
        {
            TRACE_OUT << "found at = " << p TRACE_END;
            return p;
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    size_t find_last_not_of(const string& str, const string& pattern, size_t pos)
    {
        TRACE_FUNC;
        size_t p = str.find_last_not_of(pattern, pos);
        if (p != string::npos)
        {
            TRACE_OUT << "found at = " << p TRACE_END;
            return p;
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    string remove_to_end(const string& str, size_t pos)
    {
        TRACE_FUNC;
        string ret_str = str.substr(0, pos);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    string remove_to_pos(const string& str, size_t pos)
    {
        TRACE_FUNC;
        string ret_str = str.substr(pos, str.size() - pos);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    string insert(const string& str, const string& instr, size_t pos)
    {
        TRACE_FUNC;
        string ret_str(str);
        ret_str.insert(pos, instr);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    string erase(const string& str, size_t pos, size_t count)
    {
        TRACE_FUNC;
        string ret_str(str);
        ret_str.erase(pos, count);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    string remove_duplicates(const string& str, const string& pattern)
    {
        TRACE_FUNC;
        TRACE_OUT << "input text = \"" << str << "\"" TRACE_END;
        TRACE_OUT << "pattern = \"" << pattern << "\"" TRACE_END
        string ret_str;

        for (int i = 0; i < str.size(); i++)
        {
            if (pattern.find(str[i], 0) != -1)
            {
                if (ret_str.size() == 0 || ret_str.back() != str[i])
                    ret_str.push_back(str[i]);
                continue;
            }
            ret_str.push_back(str[i]);
        }
        TRACE_OUT << "output text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

//////////////////////////////////////////////////////////////////////////
    DECLARE_MODULE(STR)
    m->add(chaiscript::fun(strfun::regex_search), "regex_search");
    m->add(chaiscript::fun(strfun::find), "find");
    m->add(chaiscript::fun(strfun::rfind), "rfind");
    m->add(chaiscript::fun(strfun::find_first_not_of), "find_first_not_of");
    m->add(chaiscript::fun(strfun::find_last_of), "find_last_of");
    m->add(chaiscript::fun(strfun::find_last_not_of), "find_last_not_of");
    m->add(chaiscript::fun(strfun::remove_to_end), "remove_to_end");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isupper)), "isupper");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&islower)), "islower");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&iscntrl)), "iscntrl");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isprint)), "isprint");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isspace)), "isspace");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isblank)), "isblank");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isgraph)), "isgraph");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&ispunct)), "ispunct");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isalnum)), "isalnum");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isalpha)), "isalpha");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isdigit)), "isdigit");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&isxdigit)), "isxdigit");
    m->add(chaiscript::fun(static_cast<int (*)(int)>(&__iscsym)), "iscsym");
    m->add(chaiscript::fun([](unsigned char)->int {return 1; }), "isany");

    m->add(chaiscript::fun(insert), "insert");
    m->add(chaiscript::fun(erase), "erase");
    m->add(chaiscript::fun(remove_duplicates), "remove_duplicates");
    m->add_global_const(chaiscript::const_var(string::npos), "npos");
    END_DECLARE(STR)
}