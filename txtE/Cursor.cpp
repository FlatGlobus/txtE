#include "Cursor.h"

DECLARE_MODULE(CURSOR)
m->add(chaiscript::fun(&Cursor::inc), "inc");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, function<size_t(const string&, const string&, size_t)>)>(&Cursor::move_to)), "move_to");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const std::vector<chaiscript::Boxed_Value>&, function<size_t(const string&, const string&, size_t)>)>(&Cursor::move_to)), "move_to");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&, function<size_t(const string&, const string&, size_t)>)>(&Cursor::move_to_end)), "move_to_end");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const std::vector<chaiscript::Boxed_Value>&, function<size_t(const string&, const string&, size_t)>)>(&Cursor::move_to_end)), "move_to_end");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::next_line)), "next_line");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::next_line)), "next_line");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::prev_line)), "prev_line");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::prev_line)), "prev_line");

m->add(chaiscript::fun(&Cursor::begin_of_line), "begin_of_line");
m->add(chaiscript::fun(&Cursor::end_of_line), "end_of_line");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::next_word)), "next_word");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::next_word)), "next_word");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_to_begin_of_word)), "move_to_begin_of_word");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::move_to_begin_of_word)), "move_to_begin_of_word");

m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const string&)>(&Cursor::move_to_end_of_word)), "move_to_end_of_word");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)()>(&Cursor::move_to_end_of_word)), "move_to_end_of_word");


m->add(chaiscript::fun(&Cursor::begin), "begin");
m->add(chaiscript::fun(&Cursor::end), "end");

m->add(chaiscript::fun(static_cast<bool(Cursor::*)()>(&Cursor::is_eof)), "is_eof");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t)>(&Cursor::is_eof)), "is_eof");

m->add(chaiscript::fun(&Cursor::operator[]), "[]");
m->add(chaiscript::fun(&Cursor::label), "label");
m->add(chaiscript::fun(&Cursor::goto_label), "goto_label");
m->add(chaiscript::fun(&Cursor::label_size), "label_size");

m->add(chaiscript::fun(&Cursor::to_string), "to_string");

m->add(chaiscript::fun(&Cursor::set_name), "set_name");
m->add(chaiscript::fun(&Cursor::get_name), "get_name");

m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t, size_t)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&, const string&)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(size_t, size_t)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&, const string&)>(&Cursor::set_range_limit)), "set_range_limit");
m->add(chaiscript::fun(static_cast<void(Cursor::*)(size_t)>(&Cursor::set_min_range_limit)), "set_min_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&)>(&Cursor::set_min_range_limit)), "set_min_range_limit");
m->add(chaiscript::fun(static_cast<void(Cursor::*)(size_t)>(&Cursor::set_max_range_limit)), "set_max_range_limit");
m->add(chaiscript::fun(static_cast<bool(Cursor::*)(const string&)>(&Cursor::set_max_range_limit)), "set_max_range_limit");
m->add(chaiscript::fun(&Cursor::reset_range_limit), "reset_range_limit");
m->add(chaiscript::fun(&Cursor::get_min_range_limit), "get_min_range_limit");
m->add(chaiscript::fun(&Cursor::get_max_range_limit), "get_max_range_limit");


m->add(chaiscript::type_conversion<Cursor, size_t>());
m->add(chaiscript::fun(&Cursor::operator +=), "+=");
m->add(chaiscript::fun(&Cursor::operator ++), "++");
m->add(chaiscript::fun(&Cursor::operator +), "+");
m->add(chaiscript::fun(&Cursor::operator -=), "-=");
m->add(chaiscript::fun(&Cursor::operator --), "--");
m->add(chaiscript::fun(&Cursor::operator -), "-");

m->add(chaiscript::fun(&Cursor::operator <), "<");
m->add(chaiscript::fun(&Cursor::operator <=), "<=");
m->add(chaiscript::fun(&Cursor::operator >), ">");
m->add(chaiscript::fun(&Cursor::operator >=), ">=");
m->add(chaiscript::fun(&Cursor::operator ==), "==");
m->add(chaiscript::fun(&Cursor::operator !=), "!=");


m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(const Cursor&)>(&Cursor::operator =)), "=");
m->add(chaiscript::fun(static_cast<Cursor& (Cursor::*)(size_t)>(&Cursor::operator =)), "=");

m->add(chaiscript::constructor<Cursor(Text&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(string&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(const Cursor&)>(), "Cursor");
m->add(chaiscript::constructor<Cursor(Cursor&)>(), "Cursor");
m->add(chaiscript::user_type<Cursor>(), "Cursor");
END_DECLARE(CURSOR)
