#include <iostream>
#include "include/argparser.hpp"

int main(int argc, char **argv) {
	ArgParser p(argc, argv);
	p.addOption(3, "-i", "Shows file output", true);
	// TODO: DONE!
	// Need to remove this extra std::get call, make it handled by the library somehow
	std::cout << p.getOptionValue<std::string_view>(3) << std::endl;
}
