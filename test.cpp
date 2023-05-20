#include <iostream>
#include "argparser.hpp"

int main(int argc, char **argv) {
	ArgParser p(argc, argv);
	p.addOption("Help", "-h", "Shows output", true);
	// TODO:
	// Need to remove this extra std::get call, make it handled by the library somehow
	std::cout <<  std::get<std::string_view>(p.getOptionValue("Help")) << std::endl;
}
