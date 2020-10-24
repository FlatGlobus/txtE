#include "mustache.h"

using namespace kainjow::mustache;
namespace _mustache
{
    //class Data :public kainjow::mustache::data
    //{
    //public:

    //    Data() :kainjow::mustache::data()
    //    {
    //    }

    //    Data(const string& n) :kainjow::mustache::data(n)
    //    {
    //    }

    //    Data(const string& name, const string& var) :kainjow::mustache::data(name, var)
    //    {
    //    }

    //    Data(const vector<kainjow::mustache::data>& v) :kainjow::mustache::data(v)
    //    {
    //    }

    //    Data(const string& name, const Data& var) :kainjow::mustache::data(name, var)
    //    {
    //    }

    //    Data(const string& name, bool var) :kainjow::mustache::data(name, var)
    //    {
    //    }
    //    
    //    Data& at(const string& key)
    //    {
    //        return reinterpret_cast<Data &>(kainjow::mustache::data::operator[](key));
    //    }

    //    void push_back(const Data& v)
    //    {
    //        kainjow::mustache::data::push_back(v);
    //    }

    //    void set(const string & name, const Data& var)
    //    {
    //        kainjow::mustache::data::set(name, var);
    //    }

    //    void set(const string& name, const string& var)
    //    {
    //        kainjow::mustache::data::set(name, var);
    //    }

    //    const Data* get(const string& key)
    //    {
    //        return reinterpret_cast<const Data*>(kainjow::mustache::data::get(key));
    //    }

    //};

    //class Mustache :public mustache
    //{
    //public:
    //    Mustache(const string& str) :mustache(str)
    //    {
    //    }

    //    string render(const Data& d)
    //    {
    //        string str = mustache::render(d);
    //        if (is_valid() == false)
    //            throw runtime_error("Mustache :" + error_message());

    //        return str;
    //    }
    //};

    DECLARE_MODULE(MUSTACHE)

    m->add(chaiscript::constructor<Mustache(const string&)>(), "Mustache");
    m->add(chaiscript::user_type<Mustache>(), "Mustache");
    m->add(chaiscript::fun(&Mustache::error_message), "error_message");
    m->add(chaiscript::fun(static_cast<string(Mustache::*)(const Data&)>(&Mustache::render)), "render");
    m->add(chaiscript::fun(static_cast<string(Mustache::*)(const query::Query&)>(&Mustache::render)), "render");

    m->add(chaiscript::constructor<Data()>(), "Data");
    m->add(chaiscript::constructor<Data(const string&)>(), "Data");
    m->add(chaiscript::constructor<Data(const string&, const string&)>(), "Data");
    m->add(chaiscript::constructor<Data(const vector<kainjow::mustache::data>&)>(), "Data");
    m->add(chaiscript::constructor<Data(const string&, const Data&)>(), "Data");
    m->add(chaiscript::constructor<Data(const string&, bool)>(), "Data");

    //m->add(chaiscript::fun(&Data::set), "set");
    //m->add(chaiscript::fun(&Data::set), "set");

    m->add(chaiscript::fun(static_cast<void(Data::*)(const string&, const Data&)>(&Data::set)), "set");
    m->add(chaiscript::fun(static_cast<void(Data::*)(const string&, const string&)>(&Data::set)), "set");

    m->add(chaiscript::fun(&Data::get), "get");
    m->add(chaiscript::fun(&Data::is_empty_object), "is_empty_object");
    m->add(chaiscript::fun(&Data::push_back), "push_back");
    m->add(chaiscript::fun(&Data::at), "at");
    

    m->add(chaiscript::user_type<Data>(), "Data");

    m->add(chaiscript::type_conversion<vector<chaiscript::Boxed_Value>, vector<kainjow::mustache::data>>(
        [](const vector<chaiscript::Boxed_Value>& vec) {
            vector<kainjow::mustache::data> ret;
            for (const auto& bv : vec) ret.emplace_back(chaiscript::boxed_cast<Data>(bv));
            return ret; })
    );


    END_DECLARE(MUSTACHE)
}
