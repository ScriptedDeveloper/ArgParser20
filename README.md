# CppArgs: C++20/17 Argument Parser Library

CppArgs is a C++20/17 single-header argument parser library that simplifies command-line argument parsing in C++ programs. It provides a modern, type-safe interface and abstracts away the complexities of C-style argument parsing during compile time.

## Features

-  Modern C++ Support: Works with C++20 and C++17, leveraging the latest language features.
- Abstraction from C: Provides a higher-level interface, making argument parsing easier for C++ developers.
- Type Safety: Enables type-safe argument parsing and handling.
- Flexible Parsing: Supports flags, options, and positional arguments with customizable parsing rules.
- Error Handling: Offers robust error handling mechanisms and meaningful error messages.

## Getting Started

```
cmake .
```
This will generate a dynamic shared library (.so) which you can use to compile your project. Otherwise, you can also simply copy and paste the single header file.


## Example
The library is very straightforward to use.
```c++
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
```



