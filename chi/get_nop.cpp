#include <iostream>
#include <string>
#include <vector>

int get(const std::vector<std::string>& opts) {
	std::cerr << "Cannot get because chigraph wasn't compiled with get support" << std::endl;

	return 1;
}
