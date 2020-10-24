#pragma once
#include <chaiscript/chaiscript.hpp>
#include"util.h"
#include "Mustache/mustache.hpp"
#include "query.h"

namespace _mustache
{
    class Data :public kainjow::mustache::data
    {
    public:

        Data() :kainjow::mustache::data()
        {
        }

        Data(const std::string& n) :kainjow::mustache::data(n)
        {
        }

        Data(const std::string& name, const std::string& var) :kainjow::mustache::data(name, var)
        {
        }

        Data(const vector<kainjow::mustache::data>& v) :kainjow::mustache::data(v)
        {
        }

        Data(const std::string& name, const Data& var) :kainjow::mustache::data(name, var)
        {
        }

        Data(const std::string& name, bool var) :kainjow::mustache::data(name, var)
        {
        }

        inline Data& at(const std::string& key)
        {
            return reinterpret_cast<Data&>(kainjow::mustache::data::operator[](key));
        }

        inline void push_back(const Data& v)
        {
            kainjow::mustache::data::push_back(v);
        }

        inline void set(const std::string& name, const Data& var)
        {
            kainjow::mustache::data::set(name, var);
        }

        inline void set(const std::string& name, const std::string& var)
        {
            kainjow::mustache::data::set(name, var);
        }

        inline const Data* get(const std::string& key)
        {
            return reinterpret_cast<const Data*>(kainjow::mustache::data::get(key));
        }
    };

    class Mustache :public kainjow::mustache::mustache
    {
    public:
        Mustache(const std::string& str) :kainjow::mustache::mustache(str)
        {
        }

        inline std::string render(const Data& d)
        {
            std::string str = kainjow::mustache::mustache::render(d);
            if (is_valid() == false)
                throw runtime_error("Mustache :" + error_message());

            return str;
        }

        inline std::string render(const query::Query& query)
        {
            kainjow::mustache::data d;

            for(auto var : ((query::QData&)query).get_data())
            {
                d.set(var.first, var.second);
            }

            std::string str = kainjow::mustache::mustache::render(d);
            if (is_valid() == false)
                throw runtime_error("Mustache :" + error_message());

            return str;
        }

    };
}