#include <parser.h>

Parser::Parser(std::string file) {
	std::ifstream vmcode(file);
	std::string line;

	while (std::getline(vmcode, line))
	{
		trim(line);
		if ((line[0] == '/') && (line[1] == '/')) {
    			printf ("Comment...\n");
		} else if (line.length() >= 2) {
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
	printf ("Command has %li arguments.\n", args.size()); 

	if (args.size() == 3) { 
		printf("Processing 3 args.\n");
		command = args.front();
		args.erase(args.begin());
		arg1 = args.front();
		args.erase(args.begin());
		arg2 = args.front();
		args.erase(args.begin());
	} else if (args.size() == 1) {
		printf("Processing 1 arg.\n");
		command = args.front();
		arg1 = args.front();
		args.erase(args.begin());
	}
	setCommandType();
}

std::vector<std::string> Parser::split(std::string s, const char* d){
	std::vector<std::string> args;
	unsigned int pos = s.find(d);
	while (pos < s.size()){
		args.push_back(s.substr(0,pos));
		printf ("Split item: %s\n", args.back().c_str());
		s.erase(0,pos+1);
		pos = s.find(d);
	}
	printf ("Remaining string: %s\n", s.c_str());
	args.push_back(s);
	return args;
}

void Parser::setCommandType(){
	printf ("setCommandType for %s.\n", command.c_str());
	if (command == "push") { printf("push\n");  commandType = c::C_PUSH; return; };
	if (command == "pop") { printf("pop\n"); commandType = c::C_POP; return; };
	if (command == "add" || command == "sub" || command == "neg" || 
		command == "eq" || command == "gt" || command == "lt" ||
		command == "and" || command == "or" || command == "not") 
		{ printf("arithmetic\n"); commandType = c::C_ARITHMETIC; return; }
}

int Parser::getCommandType(){ return commandType; }
std::string Parser::getArg1(){ return arg1; }
std::string Parser::getArg2(){ return arg2; }
