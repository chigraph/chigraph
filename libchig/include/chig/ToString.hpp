#pragma once

#include <sstream>

#ifdef MYTABLET
namespace std {
template <typename T>
string to_string(T to_conv) {
	std::stringstream ss;
	ss << to_conv;
	return ss.str();
}
inline long double strtold(const char* start, char** end) { return strtold(start, end); }
inline float strtof(const char* start, char** end) { return strtof(start, end); }
inline int stoi(const string& s, size_t* pos = 0, int base = 10) { return atoi(s.c_str()); }
}
#endif
