#ifndef GLOBAL_CPP
#define GLOBAL_CPP

#include <string>
#include <sstream>
#include <algorithm>

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

#endif

