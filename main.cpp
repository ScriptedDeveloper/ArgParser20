#include "arg_parser.h"

int main(int argc, char* argv[]) {
  // Create an instance of the argument parser
  ArgParser parser;

  // Define the command line options
  parser.addOption("input", "i", "Path to input file", true);
  parser.addOption("output", "o", "Path to output file", false);
  parser.addOption("verbose", "v", "Enable verbose mode", false);
  parser.addOption("help", "h", "Show help message", false);

  // Parse the command line arguments
  if (!parser.parse(argc, argv)) {
    // Parsing failed, display error message
    std::cerr << "Error parsing command line arguments." << std::endl;
    std::cerr << parser.getErrorMessage() << std::endl;
    return 1;
  }

  // Check if the help option is specified
  if (parser.hasOption("help")) {
    // Display help message and exit
    std::cout << parser.getHelpMessage() << std::endl;
    return 0;
  }

  // Access the parsed values
  std::string inputFile = parser.getOptionValue("input");
  std::string outputFile = parser.getOptionValue("output");
  bool verboseMode = parser.hasOption("verbose");

  // Perform application logic
  // ...

  return 0;
}

