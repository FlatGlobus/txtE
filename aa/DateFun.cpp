#include <string>
#include <chaiscript/chaiscript.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "util.h"

using namespace boost::gregorian;

date add_monts(date& d, const months& m)
{
    TRACE_FUNC;

    TRACE_OUT << "date = " << d TRACE_END;
    TRACE_OUT << "months = " << m.number_of_months() TRACE_END;
    
    date ret = d + m;

    TRACE_OUT << "result = " << ret TRACE_END;

    return ret;
}

date add_years(date& d, const years& y)
{
    TRACE_FUNC;

    TRACE_OUT << "date = " << d TRACE_END;
    TRACE_OUT << "years = " << y.number_of_years() TRACE_END;

    date ret = d + y;

    TRACE_OUT << "result = " << ret TRACE_END;

    return ret;
}

date add_days(date& d, const days& da)
{
    TRACE_FUNC;

    TRACE_OUT << "date = " << d TRACE_END;
    TRACE_OUT << "days = " << da TRACE_END;

    date ret = d + da;

    TRACE_OUT << "result = " << ret TRACE_END;

    return ret;
}

inline date _date_from_iso_string(const std::string& s)
{
    TRACE_FUNC;
    TRACE_OUT << "date = " << s TRACE_END;

    date d = date_from_iso_string(s);

    TRACE_OUT << "result date = " << d TRACE_END;

    return d;
}

inline date _from_us_string(const std::string& s)
{
    TRACE_FUNC;
    TRACE_OUT << "date = " << s TRACE_END;

    date d = from_us_string(s);

    TRACE_OUT << "result date = " << d TRACE_END;

    return d;
}

inline date _from_simple_string(const std::string& s)
{
    TRACE_FUNC;
    TRACE_OUT << "date = " << s TRACE_END;

    date d = from_simple_string(s);

    TRACE_OUT << "result date = " << d TRACE_END;

    return d;
}

inline date _from_uk_string(const std::string& s)
{
    TRACE_FUNC;
    TRACE_OUT << "date = " << s TRACE_END;

    date d = from_uk_string(s);

    TRACE_OUT << "result date = " << d TRACE_END;

    return d;
}


DECLARE_MODULE(DATE)

m->add(chaiscript::fun(&date::year), "year");
m->add(chaiscript::fun(&date::month), "month");
m->add(chaiscript::fun(&date::day), "day");
m->add(chaiscript::fun(&date::day_of_week), "day_of_week");

m->add(chaiscript::fun(&_date_from_iso_string), "date_from_iso_string");
m->add(chaiscript::fun(&_from_us_string), "from_us_string");
m->add(chaiscript::fun(&_from_simple_string), "from_simple_string");
m->add(chaiscript::fun(&_from_uk_string), "from_uk_string");

m->add(chaiscript::fun(&to_iso_extended_string), "to_iso_extended_string");
//m->add(chaiscript::fun(&to_simple_string), "to_simple_string");
//m->add(chaiscript::fun(&to_iso_string, "to_iso_string");

m->add(chaiscript::constructor<date(int, int, int)>(), "date");
m->add(chaiscript::constructor<date()>(), "date");
m->add(chaiscript::constructor<date(const date&)>(), "date");
m->add(chaiscript::user_type<date>(), "date");

m->add(chaiscript::user_type<years>(), "year");
m->add(chaiscript::constructor<years(int)>(), "years");

m->add(chaiscript::constructor<months(int)>(), "months");
m->add(chaiscript::user_type<months>(), "months");

m->add(chaiscript::constructor<days(int)>(), "days");
m->add(chaiscript::user_type<days>(), "days");

m->add(chaiscript::fun(&add_monts), "+=");
m->add(chaiscript::fun(&add_years), "+=");
m->add(chaiscript::fun(&add_days), "+=");

END_DECLARE(DATE)