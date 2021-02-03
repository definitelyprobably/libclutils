#include <set>
#include <string>
#include <clutils.hpp>

int main() {
	using namespace std::string_literals;
	CLUtils::Opts cl;
	cl.add_bare("-a");

	std::set<std::string> args{"a"s, "bc"s};

	cl.parse(args);
}
