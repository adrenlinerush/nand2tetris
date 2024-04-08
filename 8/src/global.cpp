#ifndef GLOBAL_CPP
#define GLOBAL_CPP

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

namespace c {
	const int C_ARITHMETIC = 0;
	const int C_PUSH = 1;
	const int C_POP = 2;
	const int C_LABEL = 3;
	const int C_GOTO = 4;
	const int C_IF = 5;
	const int C_FUNCTION = 6;
	const int C_RETURN = 7;
	const int C_CALL = 8;
}

inline std::string itoa(int i) {
	std::stringstream ss;
	ss << i;
	std::string s;
	ss >> s;
	return s;
}

// trim from start (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

inline std::vector<std::string> split(std::string s, const char* d){
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

#endif

