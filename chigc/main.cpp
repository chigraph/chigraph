#include <chig/Context.hpp>

#include "llvm/Support/raw_ostream.h"

#include <iostream>

using namespace chig;

int main() {
	Context c;
	
	c.loadModule("test");
	
	std::cout << "Nodes found: ";
	for(auto& node : c.modules[0]->nodes) {
		std::cout << node->name << ": " << node->description << std::endl 
			<< "Inputs: " << std::endl;
		for(size_t i = 0; i < node->inputDescs.size(); ++i) {
			std::cout << '\t' << node->inputDescs[i] << ":"; std::cout.flush();
			node->inputTypes[i]->print(llvm::outs(), true);
			std::cout << std::endl;
		}
		std::cout << "Outputs: " << std::endl;
		for(size_t i = 0; i < node->outputDescs.size(); ++i) {
			std::cout << '\t' << node->outputDescs[i] << ":"; std::cout.flush();
			node->inputTypes[i]->print(llvm::outs(), true);
			std::cout << std::endl;
		}
	}
	
	
}


