#include "include/argparser.hpp"
#include <iostream>

int main(int argc, char **argv) {
	auto help_function = [] {
		return "Welcome to our Game! Here are the commands.";
	};
	ArgParser p(argc, argv, true, "-h", help_function);
	p.addOption("Input", "-i", "Get console input.", true);
	p.addOption("Money", "-m", "Get currency.", true);
	if(p.has_Option("Money") && p.has_Option("Input"))
		std::cout << "Successfully parsed all commands inputs!" << std::endl;
	auto money = p.getOptionValue<int>("Money");
	auto input = p.getOptionValue<std::string_view>("Input");
	p.parse();
}
