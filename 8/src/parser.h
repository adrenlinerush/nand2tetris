#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>
#include <vector>
#include <global.cpp>

class Parser {

	public:
		Parser(std::string file);

		bool hasMoreLines();
		void advance();

		std::string getArg1();
		std::string getArg2();
		int getCommandType();

	private:
		std::string arg1;
		std::string arg2;
		std::string command;
		int commandType = -1;
		std::vector<std::string> commands;


		std::vector<std::string> split(std::string s, const char* d);
		void setCommandType();

};
#endif
