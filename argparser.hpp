#pragma once
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <variant>

class ArgParser {
	private:		
		/*
		 * For simplicy, we use std::variant to accept any type for command line parameters.
		 */
		 
		using AnyVar = std::variant<bool, double, char, std::string, std::string_view, int, float, const char*, char*>;
		/*
		 * We store the value in a struct, this way we can check if the param has a value.
		 * std::pair is too illegible.
		 */
		struct Param {
			bool has_param{};
			AnyVar desc{};
			AnyVar value{}; // value of param, like --input file.txt, file.txt is the value
		};

	public:
		ArgParser(int argc, char **argv) : cmdMap() {
			this->argv = argv;
			this->argc = argc;
		};
		virtual ~ArgParser() {

		};
	/*
	 * Adds a command line param, has_param tells us if it has a value.
	 */
	template<typename T, typename T1>
	constexpr void addOption(T title, AnyVar param_key, T1 desc, bool has_param) {
		Param p;
		p.has_param = has_param;
		p.desc = desc;
		if(is_string_type<T>() && std::holds_alternative<const char*>(param_key)) {
			auto param_str = std::string_view(std::get<const char*>(param_key));
			title_map[std::string_view(title)] = param_str;
			cmdMap[param_str] = p;
		}
		else if(is_string_type<T>()) {
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
	template <typename T>
	constexpr bool has_Option(T title) {
		return title_map.find(title) != title_map.end();
	}
	/*
	 * Gets command line option by value.
	 */
	auto getOptionValue(AnyVar title) {
		/*
		 * First, we convert char* or const char * to string_view, so we have 1 unique type to check after
		 */
		if(std::holds_alternative<char*>(title))
			title = std::string_view(std::get<char*>(title));
		else if(std::holds_alternative<const char*>(title))
			title = std::string_view(std::get<const char*>(title));
		if(!has_Option(title)) {
			PrintVariant(title);
			throw std::invalid_argument("Error! Expected valid Option, but got non-existing.");
		}	
		if(!parsedArgs)
			parse(); // First we have to of course parse our command line arguments before we can do anything with the value.
		Param member = cmdMap[title_map[title]];
		if(!member.has_param) {
			PrintVariant(title);
			throw std::invalid_argument("Error! Specified command line argument doesn't require a param.");
		}
		return member.value;
	}
		
	private:	
		char **argv{};
		int argc{};
		/*
		 * This function checks if the function is this specific type, by value.
		 */
		template <typename T, typename type>
		bool is_type(T val) {
			return std::is_same<T, type>::value;
		}
		/*
		 * This function gets the orignal function type.
		 */
		AnyVar get_value(std::string_view var) {
			if(is_type<std::string_view, int>(var)) 
				return std::stoi(var.data());
			else if(is_type<std::string_view, bool>(var)) 
				return (var == "true") ? true : false;
			else if(is_type<std::string_view, float>(var))
				return std::stof(var.data());
			return var;
		}
		/*
		 * This function simply prints out the variant gotten by std::visit.
		 */
		void PrintVariant(AnyVar &val) {
			std::visit([](auto &val){
				std::cout << std::endl << "Argument : " << val << std::endl;
			}, val);
		}
		template <typename T>
		constexpr bool is_string_type() {
		   return std::is_same_v<T, const char*> ||
		   std::is_same_v<T, std::string> ||
		   std::is_same_v<T, std::string_view> ||
		   std::is_same_v<T, char*>;
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
		 * Indicator if we already parsed the cli arguments or not.
		 */
		bool parsedArgs{false};
		/*
		 * This function parses argv params into the map.
		 */
		bool parse() {
			Param *last_param;
			for(int i = 1; i < argc; i++) {
				auto val = get_value(std::string_view(argv[i])); // getting orignal value type from char *
				auto it_param = cmdMap.find(val);
				if(it_param == cmdMap.end()) {
					if(last_param->has_param)
						last_param->value = val;
					else
						throw std::invalid_argument("Wrong order of command line arguments, expected value but got something else");
				} else {
					//PrintVariant(val);
					//throw std::invalid_argument("Expected new command line argument but got the same argument twice.");
					last_param = &it_param->second;
				}
			}
			parsedArgs = true;
			return true;
		}
};


