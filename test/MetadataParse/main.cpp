#include <chig/Context.hpp>
#include <chig/NodeType.hpp>

#include <iostream>
#include <cassert>

using namespace chig;

int main() {
	
	using namespace std::string_literals;
	
	Context c;
	
	auto module = c.loadModule("add_test.bc");
	
	assert(module->nodes.size() == 1);
	
	auto& node = module->nodes[0];
	
	assert(node->description == "Add two numbers");
	assert(node->numOutputExecs == 1);
	
	assert(node->inputs.size() == 2);
	assert((node->inputs[0] == std::make_pair((llvm::Type*)llvm::Type::getInt32Ty(c.context), "a"s)));
	assert((node->inputs[1] == std::make_pair((llvm::Type*)llvm::Type::getInt32Ty(c.context), "b"s)));
	
	assert(node->outputs.size() == 1);
	assert((node->outputs[0] == std::make_pair((llvm::Type*)llvm::Type::getInt32PtrTy(c.context), "out"s)));
	
	
	
}


