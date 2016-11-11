#include <iostream>
#include <fstream>

#include <chig/json.hpp>

int main(int argc, char** argv) {
	
	if(argc != 2) {
		std::cerr << "Usage: errorchecker E10" << std::endl;
		return 1;
	}
	
	// get all data
	nlohmann::json newData;
	std::cin >> newData;
	
	if(newData[0]["errorcode"] == argv[1]) {
		return 0;
	} else {
		return 1;
	}
	
} 
