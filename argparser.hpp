#pragma once
#include <iostream>
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
			AnyVar title{};
			AnyVar desc{};
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
		p.title = title;
		cmdMap[param_key] = p;
	}
		
	private:	
		char **argv{};
		int argc{};
		/*
		 * This function checks if the function is this specific type.
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
		 * We store the command line parameters in an unordered_map, an ordered map is not needed here
		 */
		std::unordered_map<AnyVar, Param> cmdMap{};
		/*
		 * This function parses argv params into the map.
		 */
		bool parse() {
			for(int i = 1; i < argc; i++) {
				auto val = get_value(std::string_view(argv[i])); // getting orignal value type from char *
				auto it_param = cmdMap.find(val);
				if(it_param == cmdMap.end())
					throw std::invalid_argument("Expected valid iterator but got nothing");
			}
			return true;
		}
};


