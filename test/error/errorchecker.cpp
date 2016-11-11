#include <iostream>
#include <fstream>

#include <chig/json.hpp>

int main(int argc, char** argv) {
	
	if(argc != 2) {
		std::cerr << "Usage: errorchecker outputtoparse.json E10" << std::endl;
		return 1;
	}
	
	// get all data
	nlohmann::json newData;
	std::ifstream stream(argv[1]);
	stream >> newData;
	
	if(newData[0]["errorcode"] == argv[2]) {
		return 0;
	} else {
		return 1;
	}
	
} 
