#include <iostream>
#include <string>
#include <unordered_map>

int main(int argc, char* argv[]) {
	bool errors = false;
	std::unordered_map<std::string, bool> expected;

	for (int i = 1; i < argc; ++i) {
		expected.insert( {argv[i], false} );
	}

	std::string input;
	while ( !std::getline(std::cin, input).eof() ) {
		if ( expected.find(input) == expected.end() ) {
			std::clog << "++ unexpected output: " << input << "\n";
			errors = true;
		}
		else {
			expected.at(input) = true;
		}
	}

	for (const auto& i: expected) {
		if ( !i.second ) {
			std::clog << "-- missing output:    " << i.first << "\n";
			errors = true;
		}
	}

	return errors ? 11 : 0;
}
