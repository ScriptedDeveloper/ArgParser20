#include <iostream>
#include "argparser.hpp"

int main(int argc, char **argv) {
	ArgParser p(argc, argv);
	p.addOption("erd", "nice", "ok", false);
}
