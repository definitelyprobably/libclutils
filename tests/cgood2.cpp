#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <clutils.hpp>

using std::clog;
using namespace std::string_literals;
using namespace CLUtils::Literals;
using CLUtils::ErrorKey;
using CLUtils::FlagClass;

struct MapCheck {
	bool a{false};
	bool aflag{false};
	bool o{false};
	bool oflag{false};
	bool x{false};
	bool xflag{false};
	bool operator()() const {
		return !(a && aflag && o && oflag && x && xflag);
	}
};

std::ostream& operator<<(std::ostream& out, const FlagClass f) {
	switch(f) {
		case FlagClass::bare:
			out << "bare";
			return out;
		case FlagClass::optional:
			out << "optional";
			return out;
		case FlagClass::mandatory:
			out << "mandatory";
			return out;
		case FlagClass::stop:
			out << "stop";
			return out;
		default:
			out << "unknown";
			return out;
	}
}

std::ostream& operator<<(std::ostream& out, const MapCheck m) {
	out << "{" << m.a << ", " << m.aflag << ", " << m.o << ", " << m.oflag
			<< ", " << m.x << ", " << m.xflag << "}";
	return out;
}

int main() {
	std::string refstr{"refstr"};
	const std::string crefstr{"crefstr"};

	CLUtils::Opts cl_char;
	CLUtils::WOpts cl_wchar;
	CLUtils::U8Opts cl_char8;
	CLUtils::U16Opts cl_char16;
	CLUtils::U32Opts cl_char32;
	CLUtils::BasicOpts<char> cl_char_expl;
	CLUtils::BasicOpts<wchar_t> cl_wchar_expl;
	CLUtils::BasicOpts<char8_t> cl_char8_expl;
	CLUtils::BasicOpts<char16_t> cl_char16_expl;
	CLUtils::BasicOpts<char32_t> cl_char32_expl;

	CLUtils::Opts& cl = cl_char;

	cl.add_bare("-a");
	cl.add_optional("-o", "-O"s, std::string{"--oflag"}, "o"_short, "oflag"_long);
	cl.add_mandatory(1, "-m");

	cl.add_bare(-12, "-b");
	cl.add_bare(refstr);
	cl.add_bare(3, crefstr);

	cl.allow_empty_arguments();
	cl.allow_empty_arguments(true);

	cl.allow_empty_inputs();
	cl.allow_empty_inputs(false);

	cl.allow_arguments();
	cl.allow_arguments(0);
	cl.allow_arguments(1);
	cl.allow_arguments(-1);

	cl.allow_unrecognized_flags();
	cl.allow_unrecognized_flags(0);
	cl.allow_unrecognized_flags(1);
	cl.allow_unrecognized_flags(-1);
	cl.allow_unrecognized_opts();
	cl.allow_unrecognized_opts(0);
	cl.allow_unrecognized_opts(1);
	cl.allow_unrecognized_opts(-1);

	cl.allow_chaining();
	cl.allow_chaining(true);

	cl.set_greedy();
	cl.set_greedy(false);
	cl.set_not_greedy();
	cl.set_lax_greedy();
	cl.set_greedy(CLUtils::Greedy::yes);
	cl.set_greedy(CLUtils::Greedy::no);
	cl.set_greedy(CLUtils::Greedy::lax);

	cl.set_preamble("X");
	cl.set_preamble("Y"s);
	cl.set_preamble(refstr);
	cl.set_preamble(crefstr);

	cl.set_postscript("X");
	cl.set_postscript("Y"s);
	cl.set_postscript(refstr);
	cl.set_postscript(crefstr);

	cl.set_opt_marker('b');
	cl.set_opt_markers(12, 'd');
	cl.add_flag_marker('e');
	cl.add_flag_markers('f', 0);
	cl.add_opt_marker('h', ' ');
	cl.add_opt_markers(1);
	cl.set_flag_marker('a', '@');
	cl.set_flag_markers('-');

	cl.set_input_marker('!');
	cl.set_input_marker('=');

	cl.format_error(ErrorKey::ArgEmpty, refstr);
	cl.format_error(ErrorKey::Surplus, crefstr);
	cl.format_error(ErrorKey::Surplus_, refstr);
	cl.format_error(ErrorKey::Unrecognized, crefstr);
	cl.format_error(ErrorKey::EmptyInput, refstr);
	cl.format_error(ErrorKey::BareEmptyInput, crefstr);
	cl.format_error(ErrorKey::BareInput, refstr);
	cl.format_error(ErrorKey::BareInput_, crefstr);
	cl.format_error(ErrorKey::MissingInput, refstr);
	cl.format_error(ErrorKey::MissingInput_, crefstr);
	cl.format_error(ErrorKey::StopEmptyInput, refstr);
	cl.format_error(ErrorKey::StopInput, crefstr);
	cl.format_error(ErrorKey::StopInput_, refstr);

	for (const auto& key: CLUtils::error_keys) {
		cl.format_error(key, "%errno");
	}

	cl.clear();
	cl.clear_errors();
	cl.clear_declarations();
	cl.default_error_formats();

	cl.add_bare("-a", "--aflag");
	cl.add_optional("-o", "--oflag");
	cl.add_mandatory("-x", "--xflag");

	int val = 0;
	for (const auto& opt: cl.registered_opts()) {
		if (opt == "-a") val += 1;
		else if (opt == "-o") val += 4;
		else if (opt == "-x") val += 16;
	}
	if (val != 21)
		return 1;

	val = 0;
	for (const auto& opt: cl.registered_flags()) {
		if (opt == "-a") val += 1;
		else if (opt == "-o") val += 4;
		else if (opt == "-x") val += 16;
		else val+=256;
	}
	if (val != 21)
		return 2;

	cl.parse();

	bool errors = false;

	bool ret = cl.parse("-a", "-o", "-aoA", "-oB", "-o");
	if (ret) {
		clog << "[parse 1]: unexpected errors on parsing:\n";
		cl.write_errors(clog);
		return 3;
	}

	const auto map = cl.get_map();
	// check some entries of the map
	// string_view -> {NamePtr, FlagPtr}

	MapCheck mapcheck;
	for (const auto& m: map) {
		if ( m.first == "-a" ) {
			mapcheck.a = true;
			if (auto x = std::get<0>(m.second)->name(); x != "-a") {
				clog << "[get_map]: -a name: returned: " << x << "\n";
				errors = true;
			}
			if (auto x = std::get<1>(m.second)->flag_class(); x != FlagClass::bare) {
				clog << "[get_map]: -a class: returned: " << x << "\n";
				errors = true;
			}
		}
		else if ( m.first == "--aflag" ) {
			mapcheck.aflag = true;
			if (auto x = std::get<0>(m.second)->name(); x != "--aflag") {
				clog << "[get_map]: --aflag name: returned: " << x << "\n";
				errors = true;
			}
			if (auto x = std::get<1>(m.second)->flag_class(); x != FlagClass::bare) {
				clog << "[get_map]: --aflag class: returned: " << x << "\n";
				errors = true;
			}
		}
		else if ( m.first == "-o" ) {
			mapcheck.o = true;
			if (auto x = std::get<0>(m.second)->name(); x != "-o") {
				clog << "[get_map]: -o name: returned: " << x << "\n";
				errors = true;
			}
			if (auto x = std::get<1>(m.second)->flag_class(); x != FlagClass::optional) {
				clog << "[get_map]: -o class: returned: " << x << "\n";
				errors = true;
			}
		}
		else if ( m.first == "--oflag" ) {
			mapcheck.oflag = true;
			if (auto x = std::get<0>(m.second)->name(); x != "--oflag") {
				clog << "[get_map]: --oflag name: returned: " << x << "\n";
				errors = true;
			}
			if (auto x = std::get<1>(m.second)->flag_class() != FlagClass::optional) {
				clog << "[get_map]: --oflag class: returned: " << x << "\n";
				errors = true;
			}
		}
		else if ( m.first == "-x" ) {
			mapcheck.x = true;
			if (auto x = std::get<0>(m.second)->name(); x != "-x") {
				clog << "[get_map]: -x name: returned: " << x << "\n";
				errors = true;
			}
			if (auto x = std::get<1>(m.second)->flag_class() != FlagClass::mandatory)
		 	{
				clog << "[get_map]: -x class: returned: " << x << "\n";
				errors = true;
			}
		}
		else if ( m.first == "--xflag" ) {
			mapcheck.xflag = true;
			if (auto x = std::get<0>(m.second)->name(); x != "--xflag") {
				clog << "[get_map]: --xflag name: returned: " << x << "\n";
				errors = true;
			}
			if (auto x = std::get<1>(m.second)->flag_class();
					x != FlagClass::mandatory) {
				clog << "[get_map]: --xflag class: returned: " << x << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_map]: unknown map, name: " << m.first << "\n";
			errors = true;
		}
	}
	if ( mapcheck() ) {
		clog << "[get_map]: cases missed: " << mapcheck << "\n";
		errors = true;
	}

	// TODO get_all_errors


	// -- have_opt --

	if (!cl.has_opt("-a")) {
		clog << "[has_opt]: -a\n";
		errors = true;
	}
	if (!cl.have_opt("--aflag")) {
		clog << "[have_opt]: --aflag\n";
		errors = true;
	}
	if (!cl.has_flag("-o")) {
		clog << "[has_flag]: -o\n";
		errors = true;
	}
	if (!cl.have_flag("--oflag")) {
		clog << "[have_flag]: --oflag\n";
		errors = true;
	}
	if (cl.has_flag("-x")) {
		clog << "[has_flag]: -x\n";
		errors = true;
	}
	if (cl.have_flag("--xflag")) {
		clog << "[have_flag]: --xflag\n";
		errors = true;
	}
	try {
		(void)cl.have_opt("X");
		clog << "[have_opt]: X\n";
		errors = true;
	}
	catch (std::exception&) {}


	// -- has_input --

	try {
		if ( cl.has_input("-a") ) {
			clog << "[has_input]: -a\n";
			errors = true;
		}
		if ( cl.has_input("-a", 0) ) {
			clog << "[has_input]: -a, 0\n";
			errors = true;
		}
		if ( cl.has_input("-a", 1) ) {
			clog << "[has_input]: -a, 1\n";
			errors = true;
		}
		if ( cl.has_input("-a", -1) ) {
			clog << "[has_input]: -a, -1\n";
			errors = true;
		}
		if ( cl.has_input("-a", -2) ) {
			clog << "[has_input]: -a, -2\n";
			errors = true;
		}
		if ( cl.has_input("--aflag") ) {
			clog << "[has_input]: --aflag\n";
			errors = true;
		}
		if ( cl.has_input("--aflag", 0) ) {
			clog << "[has_input]: --aflag, 0\n";
			errors = true;
		}
		if ( cl.has_input("--aflag", 1) ) {
			clog << "[has_input]: --aflag, 1\n";
			errors = true;
		}
		if ( cl.has_input("--aflag", -1) ) {
			clog << "[has_input]: --aflag, -1\n";
			errors = true;
		}
		if ( cl.has_input("--aflag", -2) ) {
			clog << "[has_input]: --aflag, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[has_input]: -a/--aflag throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		(void)cl.has_input("-a", 2);
		clog << "[has_input]: -a, 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("-a", -3);
		clog << "[has_input]: -a, -3\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--aflag", 2);
		clog << "[has_input]: --aflag, 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--aflag", -3);
		clog << "[has_input]: --aflag, -3\n";
		errors = true;
	}
	catch (std::exception&) {}


	try {
		if ( cl.has_input("-o") ) {
			clog << "[has_input]: -o\n";
			errors = true;
		}
		if ( cl.has_input("-o", 0) ) {
			clog << "[has_input]: -o, 0\n";
			errors = true;
		}
		if ( !cl.has_input("-o", 1) ) {
			clog << "[has_input]: -o, 1\n";
			errors = true;
		}
		if ( !cl.has_input("-o", 2) ) {
			clog << "[has_input]: -o, 2\n";
			errors = true;
		}
		if ( cl.has_input("-o", 3) ) {
			clog << "[has_input]: -o, 3\n";
			errors = true;
		}
		if ( cl.has_input("-o", -1) ) {
			clog << "[has_input]: -o, -1\n";
			errors = true;
		}
		if ( !cl.has_input("-o", -2) ) {
			clog << "[has_input]: -o, -2\n";
			errors = true;
		}
		if ( !cl.has_input("-o", -3) ) {
			clog << "[has_input]: -o, -3\n";
			errors = true;
		}
		if ( cl.has_input("-o", -4) ) {
			clog << "[has_input]: -o, -4\n";
			errors = true;
		}
		if ( cl.has_input("--oflag") ) {
			clog << "[has_input]: --oflag\n";
			errors = true;
		}
		if ( cl.has_input("--oflag", 0) ) {
			clog << "[has_input]: --oflag, 0\n";
			errors = true;
		}
		if ( !cl.has_input("--oflag", 1) ) {
			clog << "[has_input]: --oflag, 1\n";
			errors = true;
		}
		if ( !cl.has_input("--oflag", 2) ) {
			clog << "[has_input]: --oflag, 2\n";
			errors = true;
		}
		if ( cl.has_input("--oflag", 3) ) {
			clog << "[has_input]: --oflag, 3\n";
			errors = true;
		}
		if ( cl.has_input("--oflag", -1) ) {
			clog << "[has_input]: --oflag, -1\n";
			errors = true;
		}
		if ( !cl.has_input("--oflag", -2) ) {
			clog << "[has_input]: --oflag, -2\n";
			errors = true;
		}
		if ( !cl.has_input("--oflag", -3) ) {
			clog << "[has_input]: --oflag, -3\n";
			errors = true;
		}
		if ( cl.has_input("--oflag", -4) ) {
			clog << "[has_input]: --oflag, -4\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[has_input]: -o/--oflag throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		(void)cl.has_input("-o", 4);
		clog << "[has_input]: -o, 4\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("-o", -5);
		clog << "[has_input]: -o, -5\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--oflag", 4);
		clog << "[has_input]: --oflag, 4\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--oflag", -5);
		clog << "[has_input]: --oflag, -5\n";
		errors = true;
	}
	catch (std::exception&) {}


	try {
		(void)cl.has_input("-x");
		clog << "[has_input]: -x\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("-x", 0);
		clog << "[has_input]: -x, 0\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("-x", -1);
		clog << "[has_input]: -x, -1\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--xflag");
		clog << "[has_input]: --xflag\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--xflag", 0);
		clog << "[has_input]: --xflag, 0\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.has_input("--xflag", -1);
		clog << "[has_input]: --xflag, -1\n";
		errors = true;
	}
	catch (std::exception&) {}

	try {
		(void)cl.has_input("X");
		clog << "[has_input]: X\n";
		errors = true;
	}
	catch (std::exception&) {}


	// -- get_input --

	try{
		if (auto [has, inp] = cl.get_input("-a"); has) {
			clog << "[get_input]: -a\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -a: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-a", 0); has) {
			clog << "[get_input]: -a, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -a, 0: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-a", 1); has) {
			clog << "[get_input]: -a, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -a, 1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-a", -1); has) {
			clog << "[get_input]: -a, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -a, -1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-a", -2); has) {
			clog << "[get_input]: -a, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -a, -2: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		(void)cl.get_input("-a", 2);
		clog << "[get_input]: -a, 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("-a", -3);
		clog << "[get_input]: -a, -3\n";
		errors = true;
	}
	catch (std::exception&) {}

	try{
		if (auto [has, inp] = cl.get_input("--aflag"); has) {
			clog << "[get_input]: --aflag\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --aflag: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--aflag", 0); has) {
			clog << "[get_input]: --aflag, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --aflag, 0: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--aflag", 1); has) {
			clog << "[get_input]: --aflag, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --aflag, 1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--aflag", -1); has) {
			clog << "[get_input]: --aflag, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --aflag, -1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--aflag", -2); has) {
			clog << "[get_input]: --aflag, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --aflag, -2: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		(void)cl.get_input("--aflag", 2);
		clog << "[get_input]: --aflag, 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("--aflag", -3);
		clog << "[get_input]: --aflag, -3\n";
		errors = true;
	}
	catch (std::exception&) {}


	try{
		if (auto [has, inp] = cl.get_input("-o"); has) {
			clog << "[get_input]: -o\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", 0); has) {
			clog << "[get_input]: -o, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, 0: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", 1); has) {
			if (inp != "A") {
				clog << "[get_input]: -o, 1: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: -o, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, 1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", 2); has) {
			if (inp != "B") {
				clog << "[get_input]: -o, 2: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: -o, 2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, 2: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", 3); has) {
			clog << "[get_input]: -o, 3\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, 3: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", -1); has) {
			clog << "[get_input]: -o, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, -1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", -2); has) {
			if (inp != "B") {
				clog << "[get_input]: -o, -2: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: -o, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, -2: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", -3); has) {
			if (inp != "A") {
				clog << "[get_input]: -o, -3: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: -o, -3\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, -3: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("-o", -4); has) {
			clog << "[get_input]: -o, -4\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: -o, -4: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		(void)cl.get_input("-o", 4);
		clog << "[get_input]: -o, 4\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("-o", -5);
		clog << "[get_input]: -o, -5\n";
		errors = true;
	}
	catch (std::exception&) {}

	try{
		if (auto [has, inp] = cl.get_input("--oflag"); has) {
			clog << "[get_input]: --oflag\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", 0); has) {
			clog << "[get_input]: --oflag, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, 0: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", 1); has) {
			if (inp != "A") {
				clog << "[get_input]: --oflag, 1: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: --oflag, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, 1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", 2); has) {
			if (inp != "B") {
				clog << "[get_input]: --oflag, 2: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: --oflag, 2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, 2: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", 3); has) {
			clog << "[get_input]: --oflag, 3\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, 3: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", -1); has) {
			clog << "[get_input]: --oflag, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, -1: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", -2); has) {
			if (inp != "B") {
				clog << "[get_input]: --oflag, -2: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: --oflag, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, -2: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", -3); has) {
			if (inp != "A") {
				clog << "[get_input]: --oflag, -3: input returned: " << inp << "\n";
				errors = true;
			}
		}
		else {
			clog << "[get_input]: --oflag, -3\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, -3: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto [has, inp] = cl.get_input("--oflag", -4); has) {
			clog << "[get_input]: --oflag, -4\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_input]: --oflag, -4: thrown: " << exc.what() << "\n";
		errors = true;
	}
	try{
		(void)cl.get_input("--oflag", 4);
		clog << "[get_input]: --oflag, 4\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("--oflag", -5);
		clog << "[get_input]: --oflag, -5\n";
		errors = true;
	}
	catch (std::exception&) {}


	try{
		(void)cl.get_input("-x");
		clog << "[get_input]: -x\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("-x", 0);
		clog << "[get_input]: -x, 0\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("-x", -1);
		clog << "[get_input]: -x, -1\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("--xflag");
		clog << "[get_input]: --xflag\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("--xflag", 0);
		clog << "[get_input]: --xflag, 0\n";
		errors = true;
	}
	catch (std::exception&) {}
	try{
		(void)cl.get_input("--xflag", -1);
		clog << "[get_input]: --xflag, -1\n";
		errors = true;
	}
	catch (std::exception&) {}


	// -- is_input_internal --
	// -- is_input_external --

	cl.add_bare("-b", "--bflag");
	cl.add_optional("-p", "--pflag");
	cl.add_mandatory("-y", "--yflag");

	ret = cl.parse("-b", "--pflag", "-bpA", "-y", "B", "--yflag=C");
	if (ret) {
		clog << "[parse 2]: unexpected errors on parsing:\n";
		cl.write_errors(clog);
		return 4;
	}


	try {
		(void)cl.is_input_internal("-a");
		clog << "[is_input_internal]: -a\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_external("-a");
		clog << "[is_input_external]: -a\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_internal("--aflag");
		clog << "[is_input_internal]: --aflag\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_external("--aflag");
		clog << "[is_input_external]: --aflag\n";
		errors = true;
	}
	catch (std::exception&) {}

	try {
		if ( cl.is_input_internal("-b") ) {
			clog << "[is_input_internal]: -b\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: -b: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if ( cl.is_input_external("-b") ) {
			clog << "[is_input_external]: -b\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: -b: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if ( cl.is_input_internal("--bflag") ) {
			clog << "[is_input_internal]: --bflag\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if ( cl.is_input_external("--bflag") ) {
			clog << "[is_input_external]: --bflag\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.is_input_internal("-x");
		clog << "[is_input_internal]: -x\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_external("-x");
		clog << "[is_input_external]: -x\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_internal("--xflag");
		clog << "[is_input_internal]: --xflag\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_external("--xflag");
		clog << "[is_input_external]: --xflag\n";
		errors = true;
	}
	catch (std::exception&) {}

	try {
		if (auto x = cl.is_input_internal("-y"); !x) {
			clog << "[is_input_internal]: -y\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("-y"); x) {
			clog << "[is_input_external]: -y\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("-y", 0); x) {
			clog << "[is_input_internal]: -y, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("-y", 0); !x) {
			clog << "[is_input_external]: -y, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("-y", 1); !x) {
			clog << "[is_input_internal]: -y, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("-y", 1); x) {
			clog << "[is_input_external]: -y, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("-y", -1); !x) {
			clog << "[is_input_internal]: -y, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("-y", -1); x) {
			clog << "[is_input_external]: -y, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("-y", -2); x) {
			clog << "[is_input_internal]: -y, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("-y", -2); !x) {
			clog << "[is_input_external]: -y, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.is_input_internal("--yflag"); !x) {
			clog << "[is_input_internal]: --yflag\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("--yflag"); x) {
			clog << "[is_input_external]: --yflag\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("--yflag", 0); x) {
			clog << "[is_input_internal]: --yflag, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("--yflag", 0); !x) {
			clog << "[is_input_external]: --yflag, 0\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("--yflag", 1); !x) {
			clog << "[is_input_internal]: --yflag, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("--yflag", 1); x) {
			clog << "[is_input_external]: --yflag, 1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("--yflag", -1); !x) {
			clog << "[is_input_internal]: --yflag, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("--yflag", -1); x) {
			clog << "[is_input_external]: --yflag, -1\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_internal("--yflag", -2); x) {
			clog << "[is_input_internal]: --yflag, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_internal]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.is_input_external("--yflag", -2); !x) {
			clog << "[is_input_external]: --yflag, -2\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[is_input_external]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.is_input_internal("-y",2);
		clog << "[is_input_internal]: -y, 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_internal("-y",-3);
		clog << "[is_input_internal]: -y, -3\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_internal("--yflag",2);
		clog << "[is_input_internal]: --yflag, 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.is_input_internal("--yflag",-3);
		clog << "[is_input_internal]: --yflag, -3\n";
		errors = true;
	}
	catch (std::exception&) {}

	// -- get_name --
	// -- get_opt_name --
	// -- get_flag_name --

	try {
		(void)cl.get_name("-a");
		clog << "[get_name]: -a\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-a");
		clog << "[get_flag_name]: -a\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-a");
		clog << "[get_opt_name]: -a\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--aflag");
		clog << "[get_name]: --aflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--aflag");
		clog << "[get_flag_name]: --aflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--aflag");
		clog << "[get_opt_name]: --aflag\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_name("-o");
		clog << "[get_name]: -o\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-o");
		clog << "[get_flag_name]: -o\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-o");
		clog << "[get_opt_name]: -o\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--oflag");
		clog << "[get_name]: --oflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--oflag");
		clog << "[get_flag_name]: --oflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--oflag");
		clog << "[get_opt_name]: --oflag\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_name("-x");
		clog << "[get_name]: -x\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-x");
		clog << "[get_flag_name]: -x\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-x");
		clog << "[get_opt_name]: -x\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--xflag");
		clog << "[get_name]: --xflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--xflag");
		clog << "[get_flag_name]: --xflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--xflag");
		clog << "[get_opt_name]: --xflag\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		if (auto x = cl.get_name("-b"); x != "-b") {
			clog << "[get_name]: -b: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -b: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-b"); x != "-b") {
			clog << "[get_flag_name]: -b: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -b: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-b"); x != "-b") {
			clog << "[get_opt_name]: -b: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -b: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--bflag"); x != "-b") {
			clog << "[get_name]: --bflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--bflag"); x != "-b") {
			clog << "[get_flag_name]: --bflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--bflag"); x != "-b") {
			clog << "[get_opt_name]: --bflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-b", 0); x != "-b") {
			clog << "[get_name]: -b, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -b, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-b", 0); x != "-b") {
			clog << "[get_flag_name]: -b, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -b, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-b", 0); x != "-b") {
			clog << "[get_opt_name]: -b, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -b, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--bflag", 0); x != "-b") {
			clog << "[get_name]: --bflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --bflag, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--bflag", 0); x != "-b") {
			clog << "[get_flag_name]: --bflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --bflag, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--bflag", 0); x != "-b") {
			clog << "[get_opt_name]: --bflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --bflag, 0: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-b", 1); x != "-b") {
			clog << "[get_name]: -b, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -b, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-b", 1); x != "-b") {
			clog << "[get_flag_name]: -b, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -b, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-b", 1); x != "-b") {
			clog << "[get_opt_name]: -b, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -b, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--bflag", 1); x != "-b") {
			clog << "[get_name]: --bflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --bflag, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--bflag", 1); x != "-b") {
			clog << "[get_flag_name]: --bflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --bflag, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--bflag", 1); x != "-b") {
			clog << "[get_opt_name]: --bflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --bflag, 1: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-b", -1); x != "-b") {
			clog << "[get_name]: -b, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -b, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-b", -1); x != "-b") {
			clog << "[get_flag_name]: -b, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -b, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-b", -1); x != "-b") {
			clog << "[get_opt_name]: -b, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -b, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--bflag", -1); x != "-b") {
			clog << "[get_name]: --bflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --bflag, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--bflag", -1); x != "-b") {
			clog << "[get_flag_name]: --bflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --bflag, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--bflag", -1); x != "-b") {
			clog << "[get_opt_name]: --bflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --bflag, -1: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-b", -2); x != "-b") {
			clog << "[get_name]: -b, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -b, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-b", -2); x != "-b") {
			clog << "[get_flag_name]: -b, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -b, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-b", -2); x != "-b") {
			clog << "[get_opt_name]: -b, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -b, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--bflag", -2); x != "-b") {
			clog << "[get_name]: --bflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --bflag, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--bflag", -2); x != "-b") {
			clog << "[get_flag_name]: --bflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --bflag, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--bflag", -2); x != "-b") {
			clog << "[get_opt_name]: --bflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --bflag, -2: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		(void)cl.get_name("-b", 2);
		clog << "[get_name]: -b, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-b", 2);
		clog << "[get_flag_name]: -b, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-b", 2);
		clog << "[get_opt_name]: -b, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--bflag", 2);
		clog << "[get_name]: --bflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--bflag", 2);
		clog << "[get_flag_name]: --bflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--bflag", 2);
		clog << "[get_opt_name]: --bflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_name("-b", -3);
		clog << "[get_name]: -b, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-b", -3);
		clog << "[get_flag_name]: -b, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-b", -3);
		clog << "[get_opt_name]: -b, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--bflag", -3);
		clog << "[get_name]: --bflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--bflag", -3);
		clog << "[get_flag_name]: --bflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--bflag", -3);
		clog << "[get_opt_name]: --bflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		if (auto x = cl.get_name("-p"); x != "-p") {
			clog << "[get_name]: -p: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -p: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-p"); x != "-p") {
			clog << "[get_flag_name]: -p: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -p: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-p"); x != "-p") {
			clog << "[get_opt_name]: -p: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -p: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--pflag"); x != "-p") {
			clog << "[get_name]: --pflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --pflag: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--pflag"); x != "-p") {
			clog << "[get_flag_name]: --pflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --pflag: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--pflag"); x != "-p") {
			clog << "[get_opt_name]: --pflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --pflag: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-p", 0); x != "--pflag") {
			clog << "[get_name]: -p, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -p, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-p", 0); x != "--pflag") {
			clog << "[get_flag_name]: -p, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -p, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-p", 0); x != "--pflag") {
			clog << "[get_opt_name]: -p, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -p, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--pflag", 0); x != "--pflag") {
			clog << "[get_name]: --pflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --pflag, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--pflag", 0); x != "--pflag") {
			clog << "[get_flag_name]: --pflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --pflag, 0: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--pflag", 0); x != "--pflag") {
			clog << "[get_opt_name]: --pflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --pflag, 0: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-p", 1); x != "-p") {
			clog << "[get_name]: -p, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -p, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-p", 1); x != "-p") {
			clog << "[get_flag_name]: -p, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -p, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-p", 1); x != "-p") {
			clog << "[get_opt_name]: -p, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -p, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--pflag", 1); x != "-p") {
			clog << "[get_name]: --pflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --pflag, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--pflag", 1); x != "-p") {
			clog << "[get_flag_name]: --pflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --pflag, 1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--pflag", 1); x != "-p") {
			clog << "[get_opt_name]: --pflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --pflag, 1: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-p", -1); x != "-p") {
			clog << "[get_name]: -p, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -p, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-p", -1); x != "-p") {
			clog << "[get_flag_name]: -p, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -p, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-p", -1); x != "-p") {
			clog << "[get_opt_name]: -p, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -p, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--pflag", -1); x != "-p") {
			clog << "[get_name]: --pflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --pflag, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--pflag", -1); x != "-p") {
			clog << "[get_flag_name]: --pflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --pflag, -1: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--pflag", -1); x != "-p") {
			clog << "[get_opt_name]: --pflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --pflag, -1: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		if (auto x = cl.get_name("-p", -2); x != "--pflag") {
			clog << "[get_name]: -p, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -p, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("-p", -2); x != "--pflag") {
			clog << "[get_flag_name]: -p, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -p, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("-p", -2); x != "--pflag") {
			clog << "[get_opt_name]: -p, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -p, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_name("--pflag", -2); x != "--pflag") {
			clog << "[get_name]: --pflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --pflag, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_flag_name("--pflag", -2); x != "--pflag") {
			clog << "[get_flag_name]: --pflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --pflag, -2: throw: " << exc.what() << "\n";
		errors = true;
  }
	try {
		if (auto x = cl.get_opt_name("--pflag", -2); x != "--pflag") {
			clog << "[get_opt_name]: --pflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --pflag, -2: throw: " << exc.what() << "\n";
		errors = true;
  }

	try {
		(void)cl.get_name("-p", 2);
		clog << "[get_name]: -p, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-p", 2);
		clog << "[get_flag_name]: -p, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-p", 2);
		clog << "[get_opt_name]: -p, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--pflag", 2);
		clog << "[get_name]: --pflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--pflag", 2);
		clog << "[get_flag_name]: --pflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--pflag", 2);
		clog << "[get_opt_name]: --pflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_name("-p", -3);
		clog << "[get_name]: -p, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-p", -3);
		clog << "[get_flag_name]: -p, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-p", -3);
		clog << "[get_opt_name]: -p, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--pflag", -3);
		clog << "[get_name]: --pflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--pflag", -3);
		clog << "[get_flag_name]: --pflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--pflag", -3);
		clog << "[get_opt_name]: --pflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		if (auto x = cl.get_name("-y"); x != "--yflag") {
			clog << "[get_name]: -y: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("-y"); x != "--yflag") {
			clog << "[get_flag_name]: -y: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("-y"); x != "--yflag") {
			clog << "[get_opt_name]: -y: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_name("--yflag"); x != "--yflag") {
			clog << "[get_name]: --yflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("--yflag"); x != "--yflag") {
			clog << "[get_flag_name]: --yflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("--yflag"); x != "--yflag") {
			clog << "[get_opt_name]: --yflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_name("-y", 0); x != "-y") {
			clog << "[get_name]: -y, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("-y", 0); x != "-y") {
			clog << "[get_flag_name]: -y, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("-y", 0); x != "-y") {
			clog << "[get_opt_name]: -y, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_name("--yflag", 0); x != "-y") {
			clog << "[get_name]: --yflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("--yflag", 0); x != "-y") {
			clog << "[get_flag_name]: --yflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("--yflag", 0); x != "-y") {
			clog << "[get_opt_name]: --yflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_name("-y", 1); x != "--yflag") {
			clog << "[get_name]: -y, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("-y", 1); x != "--yflag") {
			clog << "[get_flag_name]: -y, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("-y", 1); x != "--yflag") {
			clog << "[get_opt_name]: -y, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_name("--yflag", 1); x != "--yflag") {
			clog << "[get_name]: --yflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("--yflag", 1); x != "--yflag") {
			clog << "[get_flag_name]: --yflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("--yflag", 1); x != "--yflag") {
			clog << "[get_opt_name]: --yflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_name("-y", -1); x != "--yflag") {
			clog << "[get_name]: -y, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("-y", -1); x != "--yflag") {
			clog << "[get_flag_name]: -y, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("-y", -1); x != "--yflag") {
			clog << "[get_opt_name]: -y, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_name("--yflag", -1); x != "--yflag") {
			clog << "[get_name]: --yflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("--yflag", -1); x != "--yflag") {
			clog << "[get_flag_name]: --yflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("--yflag", -1); x != "--yflag") {
			clog << "[get_opt_name]: --yflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_name("-y", -2); x != "-y") {
			clog << "[get_name]: -y, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("-y", -2); x != "-y") {
			clog << "[get_flag_name]: -y, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("-y", -2); x != "-y") {
			clog << "[get_opt_name]: -y, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_name("--yflag", -2); x != "-y") {
			clog << "[get_name]: --yflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_name]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_flag_name("--yflag", -2); x != "-y") {
			clog << "[get_flag_name]: --yflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_flag_name]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_opt_name("--yflag", -2); x != "-y") {
			clog << "[get_opt_name]: --yflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_opt_name]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.get_name("-y", 2);
		clog << "[get_name]: -y, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-y", 2);
		clog << "[get_flag_name]: -y, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-y", 2);
		clog << "[get_opt_name]: -y, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--yflag", 2);
		clog << "[get_name]: --yflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--yflag", 2);
		clog << "[get_flag_name]: --yflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--yflag", 2);
		clog << "[get_opt_name]: --yflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_name("-y", -3);
		clog << "[get_name]: -y, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("-y", -3);
		clog << "[get_flag_name]: -y, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("-y", -3);
		clog << "[get_opt_name]: -y, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_name("--yflag", -3);
		clog << "[get_name]: --yflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_flag_name("--yflag", -3);
		clog << "[get_flag_name]: --yflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_opt_name("--yflag", -3);
		clog << "[get_opt_name]: --yflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}

	// -- get_pos --
	// -- get_subpos --

	try {
		(void)cl.get_pos("-a");
		clog << "[get_pos]: -a\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-a");
		clog << "[get_subpos]: -a\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--aflag");
		clog << "[get_pos]: --aflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--aflag");
		clog << "[get_subpos]: --aflag\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_pos("-o");
		clog << "[get_pos]: -o\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-o");
		clog << "[get_subpos]: -o\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--oflag");
		clog << "[get_pos]: --oflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--oflag");
		clog << "[get_subpos]: --oflag\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_pos("-x");
		clog << "[get_pos]: -x\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-x");
		clog << "[get_subpos]: -x\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--xflag");
		clog << "[get_pos]: --xflag\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--xflag");
		clog << "[get_subpos]: --xflag\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		if (auto x = cl.get_pos("-b"); x != 3) {
			clog << "[get_pos]: -b: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -b: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-b"); x != 1) {
			clog << "[get_subpos]: -b: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -b: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--bflag"); x != 3) {
			clog << "[get_pos]: --bflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--bflag"); x != 1) {
			clog << "[get_subpos]: --bflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --bflag: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-b", 0); x != 1) {
			clog << "[get_pos]: -b, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -b, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-b", 0); x != 0) {
			clog << "[get_subpos]: -b, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -b, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--bflag", 0); x != 1) {
			clog << "[get_pos]: --bflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --bflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--bflag", 0); x != 0) {
			clog << "[get_subpos]: --bflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --bflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-b", 1); x != 3) {
			clog << "[get_pos]: -b, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -b, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-b", 1); x != 1) {
			clog << "[get_subpos]: -b, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -b, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--bflag", 1); x != 3) {
			clog << "[get_pos]: --bflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --bflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--bflag", 1); x != 1) {
			clog << "[get_subpos]: --bflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --bflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-b", -1); x != 3) {
			clog << "[get_pos]: -b, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -b, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-b", -1); x != 1) {
			clog << "[get_subpos]: -b, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -b, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--bflag", -1); x != 3) {
			clog << "[get_pos]: --bflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --bflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--bflag", -1); x != 1) {
			clog << "[get_subpos]: --bflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --bflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-b", -2); x != 1) {
			clog << "[get_pos]: -b, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -b, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-b", -2); x != 0) {
			clog << "[get_subpos]: -b, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -b, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--bflag", -2); x != 1) {
			clog << "[get_pos]: --bflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --bflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--bflag", -2); x != 0) {
			clog << "[get_subpos]: --bflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --bflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.get_pos("-b", 2);
		clog << "[get_pos]: -b, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-b", 2);
		clog << "[get_subpos]: -b, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--bflag", 2);
		clog << "[get_pos]: --bflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--bflag", 2);
		clog << "[get_subpos]: --bflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_pos("-b", -3);
		clog << "[get_pos]: -b, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-b", -3);
		clog << "[get_subpos]: -b, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--bflag", -3);
		clog << "[get_pos]: --bflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--bflag", -3);
		clog << "[get_subpos]: --bflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		if (auto x = cl.get_pos("-p"); x != 3) {
			clog << "[get_pos]: -p: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -p: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-p"); x != 2) {
			clog << "[get_subpos]: -p: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -p: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--pflag"); x != 3) {
			clog << "[get_pos]: --pflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --pflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--pflag"); x != 2) {
			clog << "[get_subpos]: --pflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --pflag: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-p", 0); x != 2) {
			clog << "[get_pos]: -p, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -p, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-p", 0); x != 0) {
			clog << "[get_subpos]: -p, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -p, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--pflag", 0); x != 2) {
			clog << "[get_pos]: --pflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --pflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--pflag", 0); x != 0) {
			clog << "[get_subpos]: --pflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --pflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-p", 1); x != 3) {
			clog << "[get_pos]: -p, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -p, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-p", 1); x != 2) {
			clog << "[get_subpos]: -p, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -p, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--pflag", 1); x != 3) {
			clog << "[get_pos]: --pflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --pflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--pflag", 1); x != 2) {
			clog << "[get_subpos]: --pflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --pflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-p", -1); x != 3) {
			clog << "[get_pos]: -p, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -p, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-p", -1); x != 2) {
			clog << "[get_subpos]: -p, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -p, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--pflag", -1); x != 3) {
			clog << "[get_pos]: --pflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --pflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--pflag", -1); x != 2) {
			clog << "[get_subpos]: --pflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --pflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-p", -2); x != 2) {
			clog << "[get_pos]: -p, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -p, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-p", -2); x != 0) {
			clog << "[get_subpos]: -p, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -p, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--pflag", -2); x != 2) {
			clog << "[get_pos]: --pflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --pflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--pflag", -2); x != 0) {
			clog << "[get_subpos]: --pflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --pflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.get_pos("-p", 2);
		clog << "[get_pos]: -p, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-p", 2);
		clog << "[get_subpos]: -p, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--pflag", 2);
		clog << "[get_pos]: --pflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--pflag", 2);
		clog << "[get_subpos]: --pflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_pos("-p", -3);
		clog << "[get_pos]: -p, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-p", -3);
		clog << "[get_subpos]: -p, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--pflag", -3);
		clog << "[get_pos]: --pflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--pflag", -3);
		clog << "[get_subpos]: --pflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		if (auto x = cl.get_pos("-y"); x != 6) {
			clog << "[get_pos]: -y: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-y"); x != 0) {
			clog << "[get_subpos]: -y: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -y: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--yflag"); x != 6) {
			clog << "[get_pos]: --yflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--yflag"); x != 0) {
			clog << "[get_subpos]: --yflag: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --yflag: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-y", 0); x != 4) {
			clog << "[get_pos]: -y, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-y", 0); x != 0) {
			clog << "[get_subpos]: -y, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -y, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--yflag", 0); x != 4) {
			clog << "[get_pos]: --yflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--yflag", 0); x != 0) {
			clog << "[get_subpos]: --yflag, 0: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --yflag, 0: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-y", 1); x != 6) {
			clog << "[get_pos]: -y, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-y", 1); x != 0) {
			clog << "[get_subpos]: -y, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -y, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--yflag", 1); x != 6) {
			clog << "[get_pos]: --yflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--yflag", 1); x != 0) {
			clog << "[get_subpos]: --yflag, 1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --yflag, 1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-y", -1); x != 6) {
			clog << "[get_pos]: -y, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-y", -1); x != 0) {
			clog << "[get_subpos]: -y, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -y, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--yflag", -1); x != 6) {
			clog << "[get_pos]: --yflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--yflag", -1); x != 0) {
			clog << "[get_subpos]: --yflag, -1: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --yflag, -1: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		if (auto x = cl.get_pos("-y", -2); x != 4) {
			clog << "[get_pos]: -y, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("-y", -2); x != 0) {
			clog << "[get_subpos]: -y, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: -y, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_pos("--yflag", -2); x != 4) {
			clog << "[get_pos]: --yflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_pos]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_subpos("--yflag", -2); x != 0) {
			clog << "[get_subpos]: --yflag, -2: got: " << x << "\n";
			errors = true;
		}
	}
	catch(std::exception& exc) {
		clog << "[get_subpos]: --yflag, -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.get_pos("-y", 2);
		clog << "[get_pos]: -y, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-y", 2);
		clog << "[get_subpos]: -y, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--yflag", 2);
		clog << "[get_pos]: --yflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--yflag", 2);
		clog << "[get_subpos]: --yflag, 2\n";
		errors = true;
	}
	catch(std::exception&) {}

	try {
		(void)cl.get_pos("-y", -3);
		clog << "[get_pos]: -y, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("-y", -3);
		clog << "[get_subpos]: -y, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_pos("--yflag", -3);
		clog << "[get_pos]: --yflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}
	try {
		(void)cl.get_subpos("--yflag", -3);
		clog << "[get_subpos]: --yflag, -3\n";
		errors = true;
	}
	catch(std::exception&) {}

	// -- get_unrecognized_flag --
	// -- get_unrecognized_opt --

	try {
		(void)cl.get_unrecognized_flag();
		clog << "[get_unrecognized_flag]: none\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.get_unrecognized_opt();
		clog << "[get_unrecognized_opt]: none\n";
		errors = true;
	}
	catch (std::exception&) {}

	// -- get_argument --

	try {
		(void)cl.get_argument();
		clog << "[get_argument]: none\n";
		errors = true;
	}
	catch (std::exception&) {}

	cl.allow_unrecognized_flags();
	cl.allow_arguments();

	ret = cl.parse("-b", "-A", "B", "--yflag", "C", "D", "-abp", "-E");
	if (ret) {
		clog << "[parse 3]: unexpected errors on parsing:\n";
		cl.write_errors(clog);
		return 4;
	}

	// -- get_unrecognized_flag --
	// -- get_unrecognized_opt --

	try{
		if (auto x = cl.get_unrecognized_flag(); x.name() != "-E") {
			clog << "[get_unrecognized_flag]: present: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_flag(); x.pos() != 8) {
			clog << "[get_unrecognized_flag]: present: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_flag]: present: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_opt(); x.name() != "-E") {
			clog << "[get_unrecognized_opt]: present: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_opt(); x.pos() != 8) {
			clog << "[get_unrecognized_opt]: present: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_opt]: present: throw " << exc.what() << "\n";
		errors = true;
	}

	try{
		if (auto x = cl.get_unrecognized_flag(0); x.name() != "-A") {
			clog << "[get_unrecognized_flag]: present 0: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_flag(0); x.pos() != 2) {
			clog << "[get_unrecognized_flag]: present 0: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_flag]: present 0: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_opt(0); x.name() != "-A") {
			clog << "[get_unrecognized_opt]: present 0: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_opt(0); x.pos() != 2) {
			clog << "[get_unrecognized_opt]: present 0: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_opt]: present 0: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_flag(1); x.name() != "-E") {
			clog << "[get_unrecognized_flag]: present 1: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_flag(1); x.pos() != 8) {
			clog << "[get_unrecognized_flag]: present 1: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_flag]: present 1: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_opt(1); x.name() != "-E") {
			clog << "[get_unrecognized_opt]: present 1: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_opt(1); x.pos() != 8) {
			clog << "[get_unrecognized_opt]: present 1: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_opt]: present 1: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_flag(-1); x.name() != "-E") {
			clog << "[get_unrecognized_flag]: present -1: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_flag(-1); x.pos() != 8) {
			clog << "[get_unrecognized_flag]: present -1: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_flag]: present -1: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_opt(-1); x.name() != "-E") {
			clog << "[get_unrecognized_opt]: present -1: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_opt(-1); x.pos() != 8) {
			clog << "[get_unrecognized_opt]: present -1: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_opt]: present -1: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_flag(-2); x.name() != "-A") {
			clog << "[get_unrecognized_flag]: present -2: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_flag(-2); x.pos() != 2) {
			clog << "[get_unrecognized_flag]: present -2: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_flag]: present -2: throw " << exc.what() << "\n";
		errors = true;
	}
	try{
		if (auto x = cl.get_unrecognized_opt(-2); x.name() != "-A") {
			clog << "[get_unrecognized_opt]: present -2: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_unrecognized_opt(-2); x.pos() != 2) {
			clog << "[get_unrecognized_opt]: present -2: pos got: " << x.name() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_unrecognized_opt]: present -2: throw " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.get_unrecognized_flag(2);
		clog << "[get_unrecognized_flag]: present 2\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.get_unrecognized_opt(2);
		clog << "[get_unrecognized_opt]: present 2\n";
		errors = true;
	}
	catch (std::exception&) {}

	try {
		(void)cl.get_unrecognized_flag(-3);
		clog << "[get_unrecognized_flag]: present -3\n";
		errors = true;
	}
	catch (std::exception&) {}
	try {
		(void)cl.get_unrecognized_opt(-3);
		clog << "[get_unrecognized_opt]: present -3\n";
		errors = true;
	}
	catch (std::exception&) {}

	// -- get_argument --

	try {
		if (auto x = cl.get_argument(); x.name() != "D") {
			clog << "[get_argument]: present: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_argument(); x.pos() != 6) {
			clog << "[get_argument]: present: pos got: " << x.pos() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_argument]: present: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_argument(0); x.name() != "B") {
			clog << "[get_argument]: present 0: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_argument(0); x.pos() != 3) {
			clog << "[get_argument]: present 0: pos got: " << x.pos() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_argument]: present 0: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_argument(1); x.name() != "D") {
			clog << "[get_argument]: present 1: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_argument(1); x.pos() != 6) {
			clog << "[get_argument]: present 1: pos got: " << x.pos() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_argument]: present 1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_argument(-1); x.name() != "D") {
			clog << "[get_argument]: present -1: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_argument(-1); x.pos() != 6) {
			clog << "[get_argument]: present -1: pos got: " << x.pos() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_argument]: present -1: throw: " << exc.what() << "\n";
		errors = true;
	}
	try {
		if (auto x = cl.get_argument(-2); x.name() != "B") {
			clog << "[get_argument]: present -2: name got: " << x.name() << "\n";
			errors = true;
		}
		if (auto x = cl.get_argument(-2); x.pos() != 3) {
			clog << "[get_argument]: present -2: pos got: " << x.pos() << "\n";
			errors = true;
		}
	}
	catch (std::exception& exc) {
		clog << "[get_argument]: present -2: throw: " << exc.what() << "\n";
		errors = true;
	}

	try {
		(void)cl.get_argument(2);
		clog << "[get_argument]: present 2\n";
		errors = true;
	}
	catch (std::exception&) {}

	try {
		(void)cl.get_argument(-3);
		clog << "[get_argument]: present -3\n";
		errors = true;
	}
	catch (std::exception&) {}


	// -- get_preamble --
	// -- get_postscript --

	if ( cl.get_preamble() != crefstr ) {
		clog << "[get_preamble]\n";
		errors = true;
	}
	if ( cl.get_postscript() != crefstr ) {
		clog << "[get_postscript]\n";
		errors = true;
	}


	// name conflict error

	cl.add_mandatory("-x", "--xflag");

	try {
		cl.parse();
		clog << "[name conflict error]\n";
		errors = true;
	}
	catch (CLUtils::Exception::NameConflictError&) {}
	catch (std::exception& exc) {
		clog << "[name conflict error]: exception thrown: " << exc.what() << "\n";
		errors = true;
	}


	// redefining flag markers

	cl.clear();
	cl.clear_errors();
	cl.clear_declarations();

	cl.add_mandatory("-x", "--xflag"_short, "+x");

	ret = cl.parse("-x=A", "+x=B", "--xflag=C");
	if (ret) {
		clog << "[parse 4]: unexpected errors on parsing:\n";
		cl.write_errors(clog);
		return 5;
	}

	if (auto [has, inp] = cl.get_input("--xflag", 0); has) {
		if ( inp != "=A" ) {
			clog << "[redef flag markers (pre)]: --xflag, 0: input: " << inp << "\n";
			errors = true;
		}
	}
	else {
		clog << "[redef flag markers (pre)]: --xflag, 0\n";
		errors = true;
	}

	if (auto [has, inp] = cl.get_input("--xflag", 1); has) {
		if ( inp != "B" ) {
			clog << "[redef flag markers (pre)]: --xflag, 1: input: " << inp << "\n";
			errors = true;
		}
	}
	else {
		clog << "[redef flag markers (pre)]: --xflag, 1\n";
		errors = true;
	}

	if (auto [has, inp] = cl.get_input("-x", 2); has) {
		if ( inp != "=C" ) {
			clog << "[redef flag markers (pre)]: -x, 2: input: " << inp << "\n";
			errors = true;
		}
	}
	else {
		clog << "[redef flag markers (pre)]: -x, 2\n";
		errors = true;
	}


	cl.set_flag_marker('+');

	ret = cl.parse("-x=A", "+x=B", "--xflag=C");
	if (ret) {
		clog << "[parse 5]: unexpected errors on parsing:\n";
		cl.write_errors(clog);
		return 5;
	}


	if (auto [has, inp] = cl.get_input("--xflag", 0); has) {
		if ( inp != "A" ) {
			clog << "[redef flag markers (post)]: --xflag, 0: input: " << inp << "\n";
			errors = true;
		}
	}
	else {
		clog << "[redef flag markers (post)]: --xflag, 0\n";
		errors = true;
	}

	if (auto [has, inp] = cl.get_input("--xflag", 1); has) {
		if ( inp != "=B" ) {
			clog << "[redef flag markers (post)]: --xflag, 1: input: " << inp << "\n";
			errors = true;
		}
	}
	else {
		clog << "[redef flag markers (post)]: --xflag, 1\n";
		errors = true;
	}

	if (auto [has, inp] = cl.get_input("-x", 2); has) {
		if ( inp != "=C" ) {
			clog << "[redef flag markers (post)]: -x, 2: input: " << inp << "\n";
			errors = true;
		}
	}
	else {
		clog << "[redef flag markers (post)]: -x, 2\n";
		errors = true;
	}


	// write_errors

	cl.set_flag_marker('-');
	cl.add_bare("-a", "--aflag");

	cl.set_preamble("");
	cl.set_postscript("");
	cl.format_error(CLUtils::ErrorKey::BareInput,
									"%pos:%subpos:%type:%opt:%input");
	cl.format_error(CLUtils::ErrorKey::MissingInput,
									"%pos:%subpos:%type:%opt");

	ret = cl.parse("-aA", "-x");
	if (!ret) {
		clog << "[parse 6]: no errors on parsing\n";
		return 6;
	}

	std::stringstream ss;
	cl.write_errors(ss);

	if (auto x = ss.str(); x != R"(1:%subpos:flag:-a:A
2:%subpos:flag:-x
)") {
		clog << "[write_errors]: all: got: " << x;
		errors = true;
	}

	ss.clear();
	ss.str("");

	cl.write_errors(ss);

	if (auto x = ss.str(); x != R"(1:%subpos:flag:-a:A
2:%subpos:flag:-x
)") {
		clog << "[write_errors]: all repeat: got: " << x;
		errors = true;
	}

	ss.clear();
	ss.str("");
	cl.write_error(ss, 0);
	if (auto x = ss.str(); x != "1:%subpos:flag:-a:A\n") {
		clog << "[write_errors]: ss, 0: got: " << x;
		errors = true;
	}

	ss.clear();
	ss.str("");
	cl.write_error(ss, 1);
	if (auto x = ss.str(); x != "2:%subpos:flag:-x\n") {
		clog << "[write_errors]: ss, 1: got: " << x;
		errors = true;
	}

	ss.clear();
	ss.str("");
	try {
		cl.write_error(ss, 2);
		clog << "[write_errors]: 2\n";
		errors = true;
	}
	catch (std::exception&) {}

	ss.clear();
	ss.str("");
	cl.write_error(ss, -2);
	if (auto x = ss.str(); x != "1:%subpos:flag:-a:A\n") {
		clog << "[write_errors]: ss, -2: got: " << x;
		errors = true;
	}

	ss.clear();
	ss.str("");
	cl.write_error(ss, -1);
	if (auto x = ss.str(); x != "2:%subpos:flag:-x\n") {
		clog << "[write_errors]: ss, -1: got: " << x;
		errors = true;
	}

	ss.clear();
	ss.str("");
	try {
		cl.write_error(ss, -3);
		clog << "[write_errors]: -3\n";
		errors = true;
	}
	catch (std::exception&) {}


	ss.clear();
	ss.str("");
	cl.write_errors_with(ss, ErrorKey::EmptyInput);
	if ( !ss.str().empty() ) {
		clog << "[write_errors]: keys empty\n";
		errors = true;
	}

	cl.write_errors_with(ss,
			ErrorKey::BareInput, ErrorKey::Surplus, ErrorKey::ArgEmpty);
	if (auto x = ss.str(); x != "1:%subpos:flag:-a:A\n") {
		clog << "[write_errors]: key missing: got: " << x;
		errors = true;
	}



	if (errors) {
		return 51;
	}
	return 0;
}
