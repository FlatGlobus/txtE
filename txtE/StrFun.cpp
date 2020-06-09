#include "StrFun.h"
#include <regex>
#include "Text.h"
#include "util.h"
////////////////////////////////////////////////////////////
namespace strfun
{
    size_t regex_search(const std::string& str, const std::string& pattern, size_t pos)
    {
        TRACE_FUNC;
        TRACE_OUT << "pattern = \"" << pattern << "\"" TRACE_END;

        std::regex re(pattern);
        std::smatch sm;
        if (std::regex_search(str.begin() + pos, str.end(), sm, re) == true)
        {
            TRACE_OUT << "found at = " << sm.position(0) TRACE_END;
            return sm.position(0);
        }
        TRACE_OUT << "not found" TRACE_END;
        return string::npos;
    }

    size_t find(const std::string& str, const std::string& pattern, size_t pos)
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

    size_t rfind(const std::string& str, const std::string& pattern, size_t pos)
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

    size_t find_first_not_of(const std::string& str, const std::string& pattern, size_t pos)
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

    size_t find_last_of(const std::string& str, const std::string& pattern, size_t pos)
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

    size_t find_last_not_of(const std::string& str, const std::string& pattern, size_t pos)
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

    std::string remove_to_end(const std::string& str, size_t pos)
    {
        TRACE_FUNC;
        std::string ret_str = str.substr(0, pos);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    std::string remove_to_pos(const std::string& str, size_t pos)
    {
        TRACE_FUNC;
        std::string ret_str = str.substr(pos, str.size() - pos);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    std::string insert(const std::string& str, const std::string& instr, size_t pos)
    {
        TRACE_FUNC;
        std::string ret_str(str);
        ret_str.insert(pos, instr);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    std::string erase(const std::string& str, size_t pos, size_t count)
    {
        TRACE_FUNC;
        std::string ret_str(str);
        ret_str.erase(pos, count);
        TRACE_OUT << "text = \"" << ret_str << "\"" TRACE_END;
        return ret_str;
    }

    std::string remove_duplicates(const std::string& str, const std::string& pattern)
    {
        TRACE_FUNC;
        TRACE_OUT << "input text = \"" << str << "\"" TRACE_END;
        TRACE_OUT << "pattern = \"" << pattern << "\"" TRACE_END
        std::string ret_str;

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

    DECLARE_MODULE(STR)
    m->add(chaiscript::fun(regex_search), "regex_search");
//    m->add(chaiscript::fun(find), "find");
//    m->add(chaiscript::fun(rfind), "rfind");
//    m->add(chaiscript::fun(find_first_not_of), "find_first_not_of");
//    m->add(chaiscript::fun(find_last_of), "find_last_of");
//    m->add(chaiscript::fun(find_last_not_of), "find_last_not_of");
    m->add(chaiscript::fun(remove_to_end), "remove_to_end");
    m->add(chaiscript::fun(std::isupper<unsigned char>), "isupper");
    m->add(chaiscript::fun(std::islower<unsigned char>), "islower");
    m->add(chaiscript::fun(std::iscntrl<unsigned char>), "iscntrl");
    m->add(chaiscript::fun(std::isprint<unsigned char>), "isprint");
    m->add(chaiscript::fun(std::isspace<unsigned char>), "isspace");
    m->add(chaiscript::fun(std::isblank<unsigned char>), "isblank");
    m->add(chaiscript::fun(std::isgraph<unsigned char>), "isgraph");
    m->add(chaiscript::fun(std::ispunct<unsigned char>), "ispunct");
    m->add(chaiscript::fun(std::isalnum<unsigned char>), "isalnum");
    m->add(chaiscript::fun(std::isalpha<unsigned char>), "isalpha");
    m->add(chaiscript::fun(std::isdigit<unsigned char>), "isdigit");
    m->add(chaiscript::fun(std::isxdigit<unsigned char>), "isxdigit");
    m->add(chaiscript::fun(insert), "insert");
    m->add(chaiscript::fun(erase), "erase");
    m->add(chaiscript::fun(remove_duplicates), "remove_duplicates");
    m->add_global_const(chaiscript::const_var(std::string::npos), "npos");
    END_DECLARE(STR)
}