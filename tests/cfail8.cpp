#include <clutils.hpp>

int main() {
	using namespace std::string_literals;
	CLUtils::Opts cl;
	cl.add_opt_markers("--");
}
