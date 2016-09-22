#include <chig/Context.hpp>
#include <chig/NodeType.hpp>

#include <cassert>
#include <iostream>

using namespace chig;

int main()
{
	using namespace std::string_literals;

	Context c;

	auto module = c.loadModuleFromBc("add_test.bc");

	assert(module->nodes.size() == 1);

	auto& node = module->nodes[0];

	assert(node->description == "Add two numbers");
	assert(node->execOutputs.size() == 1);

	assert(node->dataInputs.size() == 2);
	assert((node->dataInputs[0] ==
			std::make_pair((llvm::Type*)llvm::Type::getInt32Ty(c.context), "a"s)));
	assert((node->dataInputs[1] ==
			std::make_pair((llvm::Type*)llvm::Type::getInt32Ty(c.context), "b"s)));

	assert(node->dataOutputs.size() == 1);
	assert((node->dataOutputs[0] ==
			std::make_pair((llvm::Type*)llvm::Type::getInt32PtrTy(c.context), "out"s)));
}
