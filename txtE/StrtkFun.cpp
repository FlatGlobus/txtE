#include "util.h"
#include "strtk\strtk.hpp"

DECLARE_MODULE(STRTK)
m->add(chaiscript::fun<void, const std::string::value_type, std::string&>(strtk::remove_consecutives_inplace), "remove_consecutives_inplace");
m->add(chaiscript::fun<void, const std::string&, std::string&>(strtk::remove_consecutives_inplace), "remove_consecutives_inplace");

m->add(chaiscript::fun(strtk::remove_duplicates), "remove_duplicates");
m->add(chaiscript::fun(strtk::remove_duplicates_inplace), "remove_duplicates_inplace");

m->add(chaiscript::fun<void, const std::string::value_type, std::string&>(strtk::remove_trailing), "remove_trailing");
m->add(chaiscript::fun<void, const std::string&, std::string&>(strtk::remove_trailing), "remove_trailing");

m->add(chaiscript::fun<void, const std::string::value_type, std::string&>(strtk::remove_leading), "remove_leading");
m->add(chaiscript::fun<void, const std::string&, std::string&>(strtk::remove_leading), "remove_leading");

m->add(chaiscript::fun<void, const std::string&, std::string&>(strtk::remove_leading_trailing), "remove_leading_trailing");


m->add(chaiscript::fun<void, const std::string::value_type&, const std::string::value_type&, std::string&>(strtk::replace), "replace");
m->add(chaiscript::fun<void, const std::string&, const std::string&, std::string&>(strtk::remove_pattern), "remove_pattern");

m->add(chaiscript::fun(strtk::sort), "sort");

m->add(chaiscript::fun(strtk::ifind), "ifind");

m->add(chaiscript::fun(strtk::begins_with), "begins_with");
m->add(chaiscript::fun(strtk::ibegins_with), "ibegins_with");

m->add(chaiscript::fun(strtk::ends_with), "ends_with");
m->add(chaiscript::fun(strtk::iends_with), "iends_with");

m->add(chaiscript::fun(strtk::index_of), "index_of");

m->add(chaiscript::fun<void, std::string&>(strtk::lexicographically_canonicalize), "lexicographically_canonicalize");

m->add(chaiscript::fun<std::size_t, const std::string&>(strtk::first_non_repeated_char), "first_non_repeated_char");

m->add(chaiscript::fun<std::string, const std::string&>(strtk::convert_bin_to_hex), "bin_to_hex");
m->add(chaiscript::fun<bool, const std::string&, std::string&>(strtk::convert_hex_to_bin), "hex_to_bin");

m->add(chaiscript::fun<void, const std::string&, std::string&>(strtk::convert_bin_to_base64), "bin_to_base64");
m->add(chaiscript::fun<void, const std::string&, std::string&>(strtk::convert_base64_to_bin), "base64_to_bin");

m->add(chaiscript::fun<void, std::string&>(strtk::convert_to_uppercase), "to_upper");
m->add(chaiscript::fun<void, std::string&>(strtk::convert_to_lowercase), "to_lower");

m->add(chaiscript::fun<bool, const std::string&, const std::string&, std::string&>(strtk::twoway_bitwise_interleave), "twoway_bitwise_interleave");

m->add(chaiscript::fun<std::string, const std::size_t&, const std::string& >(strtk::replicate), "replicate");


//join
//bracketize
END_DECLARE(STRTK)