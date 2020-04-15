#include "StrFun.h"
#include <regex>
#include "Text.h"
#include "util.h"
//#include "regXwild\regXwildAPI.h "
////////////////////////////////////////////////////////////
namespace strfun
{
    size_t regex_search(const std::string& str, const std::string& pattern, size_t pos)
    {
        std::regex re(pattern);
        std::smatch sm;
        if (std::regex_search(str.begin() + pos, str.end(), sm, re) == true)
            return sm.position(0);
        return -1L;
    }

    size_t find(const std::string& str, const std::string& pattern, size_t pos)
    {
        return str.find(pattern, pos);
    }

    size_t rfind(const std::string& str, const std::string& pattern, size_t pos)
    {
        return str.rfind(pattern, pos);
    }

    size_t find_first_not_of(const std::string& str, const std::string& pattern, size_t pos)
    {
        return str.find_first_not_of(pattern, pos);
    }

    size_t find_last_of(const std::string& str, const std::string& pattern, size_t pos)
    {
        return str.find_last_of(pattern, pos);
    }

    size_t find_last_not_of(const std::string& str, const std::string& pattern, size_t pos)
    {
        return str.find_last_not_of(pattern, pos);
    }

    void remove_to_end(std::string& str, size_t pos)
    {
        str = str.substr(0, pos);
    }

    void remove_to_pos(std::string& str, size_t pos)
    {
        str = str.substr(pos, str.size() - pos);
    }

    bool is_sign_end_of_sentence(unsigned char c)
    {
        return c == '.' || c == '?' || c == '!';
    }

    std::string insert(std::string& str, std::string& instr, size_t pos)
    {
        std::string ret(str);
        ret.insert(pos, instr);
        return ret;
    }

    std::string erase(std::string& str, size_t pos, size_t count)
    {
        std::string ret(str);
        ret.erase(pos, count);
        return ret;
    }

    std::string removeDuplicates(const std::string& str, const std::string& pattern)
    {
        std::string ret;
        for (int i = 0; i < str.size(); i++)
        {
            if (pattern.find(str[i], 0) != -1)
            {
                if (ret.size() == 0 || ret.back() != str[i])
                    ret.push_back(str[i]);
                continue;
            }
            ret.push_back(str[i]);
        }
        return ret;
    }

    //size_t _match(const std::string& str, const std::string& pattern, size_t pos)
    //{
    //   net::r_eg::regXwild::searchEss(str, pattern, false);
    //   return 1;
    //}
    //TODO поиск места различия строк, поиск места различия строк с подавлением пробелов или любых других символов

    DECLARE_MODULE(STR)
    m->add(chaiscript::fun(regex_search), "regex_search");
    m->add(chaiscript::fun(find), "find");
    m->add(chaiscript::fun(rfind), "rfind");
    m->add(chaiscript::fun(find_first_not_of), "find_first_not_of");
    m->add(chaiscript::fun(find_last_of), "find_last_of");
    m->add(chaiscript::fun(find_last_not_of), "find_last_not_of");
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
    //  m->add(chaiscript::fun(_match), "match");
    m->add(chaiscript::fun(removeDuplicates), "remove_duplicates");
    m->add_global_const(chaiscript::const_var(std::string::npos), "npos");
    END_DECLARE(STR)
}