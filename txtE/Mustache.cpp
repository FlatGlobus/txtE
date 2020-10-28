#include "mustache.h"


namespace _mustache
{

    DECLARE_MODULE(MUSTACHE)

    m->add(chaiscript::constructor<Mustache(const string&)>(), "Mustache");
    m->add(chaiscript::user_type<Mustache>(), "Mustache");
    m->add(chaiscript::fun(&Mustache::error_message), "error_message");
    m->add(chaiscript::fun(static_cast<string(Mustache::*)(const Data&)>(&Mustache::render)), "render");
    m->add(chaiscript::fun(static_cast<string(Mustache::*)(query::Query&)>(&Mustache::render)), "render");
    m->add(chaiscript::fun(static_cast<string(Mustache::*)(query::Query& , const string& , const string&)>(&Mustache::render)), "render");

    m->add(chaiscript::constructor<Data()>(), "Data");
    m->add(chaiscript::constructor<Data(const string&)>(), "Data");
    m->add(chaiscript::constructor<Data(const string&, const string&)>(), "Data");
    m->add(chaiscript::constructor<Data(const vector<kainjow::mustache::data>&)>(), "Data");
    m->add(chaiscript::constructor<Data(const string&, const Data&)>(), "Data");
    m->add(chaiscript::constructor<Data(const string&, bool)>(), "Data");
    m->add(chaiscript::fun(static_cast<void(Data::*)(const string&, const Data&)>(&Data::set)), "set");
    m->add(chaiscript::fun(static_cast<void(Data::*)(const string&, const string&)>(&Data::set)), "set");
    m->add(chaiscript::fun(&Data::get), "get");
    m->add(chaiscript::fun(&Data::is_empty_object), "is_empty_object");
    m->add(chaiscript::fun(&Data::push_back), "push_back");
    m->add(chaiscript::fun(&Data::at), "at");
    m->add(chaiscript::user_type<Data>(), "Data");

    END_DECLARE(MUSTACHE)
}
