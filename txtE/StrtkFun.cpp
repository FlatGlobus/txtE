#include "util.h"
#include "strtk\strtk.hpp"
//m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return *(s1, s2); }), "*");
using namespace strtk;
DECLARE_MODULE(STRTK)

m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return remove_consecutives_inplace(s1, s2); }), "remove_consecutives_inplace");
m->add(chaiscript::fun(strtk::remove_duplicates), "remove_duplicates");
m->add(chaiscript::fun(strtk::remove_duplicates_inplace), "remove_duplicates_inplace");
m->add(chaiscript::fun ([](const std::string& c, std::string& str) { return remove_trailing(c, str); }), "remove_trailing");
m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return remove_leading(s1, s2); }), "remove_leading");
m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return remove_leading_trailing(s1, s2); }), "remove_leading_trailing");
m->add(chaiscript::fun([](const std::string::value_type & s1, const std::string::value_type& s2, std::string& s3) { return replace(s1, s2, s3); }), "replace");
m->add(chaiscript::fun([](const std::string& s1, const std::string& s2, std::string& s3) { return remove_pattern(s1, s2, s3); }), "remove_pattern");
m->add(chaiscript::fun(strtk::sort), "sort");
m->add(chaiscript::fun(strtk::ifind), "ifind");
m->add(chaiscript::fun(strtk::begins_with), "begins_with");
m->add(chaiscript::fun(strtk::ibegins_with), "ibegins_with");
m->add(chaiscript::fun(strtk::ends_with), "ends_with");
m->add(chaiscript::fun(strtk::iends_with), "iends_with");
m->add(chaiscript::fun(strtk::index_of), "index_of");
m->add(chaiscript::fun([](std::string& s1) { return lexicographically_canonicalize(s1); }), "lexicographically_canonicalize");
m->add(chaiscript::fun([](const std::string& s1) { return first_non_repeated_char(s1); }), "first_non_repeated_char");
m->add(chaiscript::fun([](const std::string& s1) { return convert_bin_to_hex(s1); }), "convert_bin_to_hex");
m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return convert_hex_to_bin(s1, s2); }), "convert_hex_to_bin");
m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return convert_bin_to_base64(s1, s2); }), "convert_bin_to_base64");
m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return convert_base64_to_bin(s1, s2); }), "convert_base64_to_bin");
m->add(chaiscript::fun([](std::string& s1) { return convert_to_uppercase(s1); }), "convert_to_uppercase");
m->add(chaiscript::fun([](std::string& s1) { return convert_to_lowercase(s1); }), "convert_to_lowercase");
m->add(chaiscript::fun([](const std::string& s1, const std::string& s2, std::string& s3) { return twoway_bitwise_interleave(s1, s2, s3); }), "twoway_bitwise_interleave");
m->add(chaiscript::fun([](const std::size_t& s1, const std::string& s2) { return replicate(s1, s2); }), "replicate");
m->add(chaiscript::fun([](const std::string& s1, std::string& s2) { return match(s1, s2); }), "match");

//join
//bracketize
END_DECLARE(STRTK)