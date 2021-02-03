#include <clutils.hpp>

int main() {
	using namespace std::string_literals;
	CLUtils::Opts cl;
	cl.parse(1, 2, 3);
}
