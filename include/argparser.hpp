/*
Copyright (c) 2023 ScriptedDev

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <array>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <functional>

template <typename T>
concept is_string_type = std::is_same_v<T, const char *> || std::is_same_v<T, std::string> ||
						 std::is_same_v<T, std::string_view> || std::is_same_v<T, char *>;

class ArgParser {
  private:
	/*
	 * For simplicy, we use std::variant to accept any type for command line parameters.
	 */

	using AnyVar = std::variant<bool, double, char, std::string, std::string_view, int, float, const char *, char *>;
	/*
	 * We store the value in a struct, this way we can check if the param has a value.
	 * std::pair is too illegible.
	 */
	struct Param {
		bool has_param{};
		bool param_filled{}; // checking whether value is empty or not
		AnyVar desc{};
		AnyVar value{}; // value of param, like --input file.txt, file.txt is the value
	};

  public:
	// TODO : Add a lambda function pointer bullshit so programmer can define a good help message
	template<typename helptype = std::string_view>
	ArgParser(int argc, char **argv, const bool c_show_help = true, helptype c_help_param = "-h", const std::function<std::string(void)> c_help_func = nullptr) : cmdMap(), show_help(c_show_help) {
		this->argv = argv;
		this->argc = argc;
		help_param = c_help_param;
		help_ptr = c_help_func;
	};
	virtual ~ArgParser(){

	};
	void ShowHelp() {
		if(help_ptr != nullptr)
			std::cout << help_ptr() << std::endl;
		for(auto i : cmdMap) {
			auto mem_param = i.second;
			std::cout << std::endl;
			PrintVariant(i.first, false);
			std::cout << " : ";
			PrintVariant(mem_param.desc, false);
			std::cout << std::endl;
		}
	}	
	/*
	 * This function parses argv params into the map.
	 */
	bool parse() {
		Param *last_param{nullptr};
		if(argc == 1)
			return true; // no arguments
		for (int i = 1; i < argc; i++) {
			auto val = get_value(std::string_view(argv[i])); // getting orignal value type from char *
			auto it_param = cmdMap.find(val);
			if (it_param == cmdMap.end()) {
				if(last_param == nullptr)
					throw noarguments("Invalid argument parsed.");
				if (last_param->has_param)
					last_param->value = val;
				else
					throw std::invalid_argument(
						"Wrong order of command line arguments, expected value but got something else");
				last_param->param_filled = true;
			} else {
				// PrintVariant(val);
				// throw std::invalid_argument("Expected new command line argument but got the same argument twice.");
				if(it_param->second.has_param)
					last_param = &it_param->second;
				else {
					it_param->second.value = val;
					it_param->second.param_filled = true;
				}
			}
		}
		if ((last_param != nullptr && !last_param->param_filled) || (!cmdMap.begin()->second.param_filled)) {
			throw noarguments("Expected arguments, but got none");
		}
		if(show_help)
			ShowHelp();
		parsedArgs = true;
		return true;
	}
	/*
	 * Adds a command line param, has_param tells us if it has a value.
	 */
	template <typename T, typename T1>
	constexpr void addOption(T title, std::string_view param_key, T1 desc, bool has_param) {
		Param p;
		p.has_param = has_param;
		p.desc = desc;
		if constexpr (is_string_type<T>) {
			title_map[std::string_view(title)] = param_key;
			cmdMap[param_key] = p;
		} else {
			title_map[title] = param_key;
			cmdMap[param_key] = p;
		}
	}
	/*
	 * Checks if the command line argument exists.
	 */
	template <typename T> bool has_Option(T title) {
		if constexpr (is_string_type<T>) {
			AnyVar view = std::string_view(title);
			auto it = title_map.find(view);
			return it != title_map.end() && it->first == view;
		} else {
			auto it = title_map.find(title);
			if(it == title_map.end())
				return false;
			auto it_cmd_map = cmdMap.find(it->second);
			return title_map.find(title) != title_map.end();
		}
	}
	/*
	 * Gets command line option by value.
	 */
	template <typename Tval> constexpr Tval getOptionValue(AnyVar title) {
		/*
		 * First, we convert char* or const char * to string_view, so we have 1 unique type to check after
		 */
		turn_string(title);
		if (!has_Option(title)) {
			PrintVariant(title);
			throw std::invalid_argument("Error! Expected valid Option, but got non-existing.");
		}
		if (!parsedArgs)
			parse(); // First we have to of course parse our command line arguments before we can do anything with the
					 // value.
		Param member = cmdMap[title_map[title]];
		if (!member.has_param) {
			PrintVariant(title);
			throw std::invalid_argument("Error! Specified command line argument doesn't require a param.");
		}

		if (!std::holds_alternative<Tval>(member.value)) {
			PrintVariant(title);
			throw std::invalid_argument(
				std::string("Error! Expected command line argument of same type. Expected valid type but type is "
							"invalid. \nNote that const char* or char* is std::string_view."));
		}
		
		return std::get<Tval>(member.value);
	}
	/*
	 * Exceptions
	 */
	class noarguments {
	  public:
		noarguments(std::string_view error_message) { what(error_message); };
		std::string_view what(std::string_view msg) { return msg; }
	};

  private:
	static constexpr std::array<std::string_view, 1> ParamArray{};
	char **argv{};
	int argc{};
	/*
	 * This function checks if the function is this specific type, by value.
	 */
	template <typename T>
	auto is_type(std::string_view str) {
		auto result = T();
		auto istream = std::istringstream(str.data());
		istream >> result;
		return (istream.eof() && !istream.fail());
	}
	/*
	 * This function gets the orignal function type.
	 */
	const AnyVar get_value(std::string_view var) {
		if (is_type<int>(var))
			return std::stoi(var.data());
		else if (is_type<bool>(var))
			return (var == "true") ? true : false;
		else if (is_type<float>(var))
			return std::stof(var.data());
		return var;
	}
	/*
	 * This function turns an AnyVar variable into string_view, if its a string type.
	 */
	bool turn_string(AnyVar &var) {
		if (std::holds_alternative<char *>(var))
			var = std::string_view(std::get<char *>(var));
		else if (std::holds_alternative<const char *>(var))
			var = std::string_view(std::get<const char *>(var));
		else
			return false;
		return true;
	}
	/*
	 * This function simply prints out the variant gotten by std::visit.
	 */
	void PrintVariant(const AnyVar &val, const bool raw_print = true) {
		std::visit([&](auto &val) { 
				if(raw_print)
					std::cout << std::endl << "Argument : " << val << std::endl; 
				else
					std::cout << val;
				}, val);
	}
	/*
	 * We store the command line parameters in an unordered_map, an ordered map is not needed here
	 */
	std::unordered_map<AnyVar, Param> cmdMap{};
	/*
	 * We also store the titles and map them to the params.
	 */
	std::unordered_map<AnyVar, AnyVar> title_map{};
	/*
	 * Contains a function pointer to the help lambda/function.
	 */
	std::function<std::string(void)> help_ptr{};
	/*
	 * Indicator if we already parsed the cli arguments or not.
	 */
	bool parsedArgs{false};
	/*
	 * Indicator if user wants a help message or not.
	 */
	bool show_help{true};
	/*
	 * Help parameter specified by constructor.
	 */
	AnyVar help_param{};
	/*
	 * Iterates through argv, finding -h.
	 */
	bool find_help() {
		int i = 0;
		bool is_string = turn_string(help_param);
		std::string_view help_view;
		if(is_string)
			help_view = std::get<std::string_view>(help_param);
		for(char *it = argv[i]; i < argc; i++) {
			it = argv[i];
			std::string_view it_view{it};
			if((is_string && it_view == help_view) || (help_param == get_value(it_view)))
				return true; 
		}
		return false;
	}
};
