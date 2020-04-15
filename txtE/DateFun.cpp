#include <string>
#include <chaiscript/chaiscript.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "util.h"

using namespace boost::gregorian;
/*int main() {
    // . / - ' '
    // Converting ISO Format (YYYYMMDD) String to date object
    date d1 = date_from_iso_string("20160221");
    std::cout << d1 << std::endl;
    // Converting Simple Format (YYYY-MM-DD) String to date object
    date d2_1 = from_simple_string("2016-2-21");
    // Converting Simple Format (YYYY MM DD) String to date object
    date d2_2 = from_simple_string("2016 2 21");
    // Converting Simple Format (YYYY/MM/DD) String to date object
    date d2_3 = from_simple_string("2016/2/21");
    // Converting Simple Format (YYYY.MM.DD) String to date object
    date d2_4 = from_simple_string("2016.2.21");
    std::cout << d2_1 << std::endl;
    std::cout << d2_2 << std::endl;
    std::cout << d2_3 << std::endl;
    std::cout << d2_4 << std::endl;
    // Converting US Format (MM-DD-YYYY) String to date object
    date d3_1 = from_us_string("02-21-2016");
    // Converting US Format (MM/DD/YYYY) String to date object
    date d3_2 = from_us_string("02/21/2016");
    // Converting US Format (MM DD YYYY) String to date object
    date d3_3 = from_us_string("02 21 2016");
    // Converting US Format (MM,DD.YYYY) String to date object
    date d3_4 = from_us_string("02,21.2016");
    std::cout << d3_1 << std::endl;
    std::cout << d3_2 << std::endl;
    std::cout << d3_3 << std::endl;
    std::cout << d3_4 << std::endl;
    // Converting UK Format (DD-MM-YYYY) String to date object
    date d4_1 = from_uk_string("21-02-2016");
    // Converting UK Format (DD/MM/YYYY) String to date object
    date d4_2 = from_uk_string("21/02/2016");
    // Converting UK Format (DD MM YYYY) String to date object
    date d4_3 = from_uk_string("21 02 2016");
    // Converting UK Format (DD,MM.YYYY) String to date object
    date d4_4 = from_uk_string("21,02.2016");
    std::cout << d4_1 << std::endl;
    std::cout << d4_2 << std::endl;
    std::cout << d4_3 << std::endl;
    std::cout << d4_4 << std::endl;
    // Converting UK Format (DD-<MONTH>-YYYY) String to date object
    date d5 = from_uk_string("21-FEBRUARY-2016");
    std::cout << d5 << std::endl;
    // Converting UK Format (DD-<MONTH>-YYYY) String to date object
    date d6 = from_uk_string("21-FEB-2016");
    std::cout << d6 << std::endl;
    return 0;
}

*/

date add_monts(date& d, const months& m)
{
    return d += m;
}

date add_years(date& d, const years& y)
{
    return d += y;
}

date add_days(date& d, const days& da)
{
    return d += da;
}

//std::string iso_extended_string(const date& d)
//{
//    return to_iso_extended_string(d);
//}

DECLARE_MODULE(DATE)

m->add(chaiscript::fun(&date::year), "year");
m->add(chaiscript::fun(&date::month), "month");
m->add(chaiscript::fun(&date::day), "day");
m->add(chaiscript::fun(&date::day_of_week), "day_of_week");

// m->add(chaiscript::fun(&date::add_years), "add_years");
// m->add(chaiscript::fun(&date::add_months), "add_months");
// m->add(chaiscript::fun(&date::add_days), "add_days");
// m->add(chaiscript::fun(&date::to_string), "to_string");
// m->add(chaiscript::fun(&date::parse), "parse");
// m->add(chaiscript::fun(&date::operator=), "=");

m->add(chaiscript::fun(&to_iso_extended_string), "to_iso_extended_string");
//m->add(chaiscript::fun(&to_simple_string), "to_simple_string");
m->add(chaiscript::fun(&from_us_string), "from_us_string");
m->add(chaiscript::fun(&from_simple_string), "from_simple_string");


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