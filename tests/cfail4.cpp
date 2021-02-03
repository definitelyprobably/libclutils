#include <vector>
#include <clutils.hpp>

int main() {
	using namespace std::string_literals;
	CLUtils::Opts cl;
	cl.parse(std::vector<int>{1, 2, 3});
}
