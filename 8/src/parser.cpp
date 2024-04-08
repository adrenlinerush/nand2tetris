#include <parser.h>

Parser::Parser(std::string file) {
	std::ifstream vmcode(file);
	std::string line;

	while (std::getline(vmcode, line))
	{
		unsigned int rem = line.find("//");
		line = line.substr(0,rem);
		trim(line);
		if (line.length() >= 2) {
			printf ("Adding line to vector: %s\n", line.c_str());
			commands.push_back(line);
		}
	}

}

bool Parser::hasMoreLines(){
	if (commands.size() == 0) { return false; }
	else { return true; }
}

void Parser::advance(){
	printf ("Parser::advance()\n");
	std::string line = commands.front();
	commands.erase(commands.begin());

	std::vector<std::string> args = split(line, " ");
	long int nArgs = args.size();
	printf ("Command has %li arguments.\n", nArgs); 

	if (nArgs > 1) { 
		printf("Processing 2 or more args.\n");
		command = args.front();
		args.erase(args.begin());
		arg1 = args.front();
		args.erase(args.begin());
		if (nArgs > 2) {
			arg2 = args.front();
			args.erase(args.begin());
		}
	} else if (args.size() == 1) {
		printf("Processing 1 arg.\n");
		command = args.front();
		arg1 = args.front();
		args.erase(args.begin());
	}
	setCommandType();
}

void Parser::setCommandType(){
	printf ("setCommandType for %s.\n", command.c_str());
	if (command == "push") { printf("push\n");  commandType = c::C_PUSH; return; };
	if (command == "pop") { printf("pop\n"); commandType = c::C_POP; return; };
	if (command == "add" || command == "sub" || command == "neg" || 
		command == "eq" || command == "gt" || command == "lt" ||
		command == "and" || command == "or" || command == "not") 
		{ printf("arithmetic\n"); commandType = c::C_ARITHMETIC; return; }
	if (command == "label") { printf("label\n");  commandType = c::C_LABEL; return; };
	if (command == "goto") { printf("goto\n");  commandType = c::C_GOTO; return; };
	if (command == "if-goto") { printf("if-goto\n");  commandType = c::C_IF; return; };
	if (command == "function") { printf("function\n");  commandType = c::C_FUNCTION; return; };
	if (command == "call") { printf("call\n");  commandType = c::C_CALL; return; };
	if (command == "return") { printf("return\n");  commandType = c::C_RETURN; return; };
}

int Parser::getCommandType(){ return commandType; }
std::string Parser::getArg1(){ return arg1; }
std::string Parser::getArg2(){ return arg2; }
