#include "chig/ImportedModule.hpp"
#include "chig/NodeType.hpp"
#include "chig/Context.hpp"

#include <stdlib.h>
#include <string.h>

using namespace chig;

ImportedModule::ImportedModule(Context& contextArg, std::unique_ptr<llvm::Module> arg_module) : ChigModule(contextArg) {

	module = std::move(arg_module);
	name = module->getName();

	// start finding nodes
	for(auto&& function : module->getFunctionList()) {

		// create the chig-<func name> string
		auto metadataTagName = "chig-" + function.getName();

		// see if it has the metadata tags. It is a named metadata in the module called chig-<func name>
		auto root_metadata = module->getNamedMetadata(metadataTagName);
		if(root_metadata) {
			
			auto metadata = root_metadata->getOperand(0);
			
			// extract the metadata
			unsigned num_operands = metadata->getNumOperands();
			if(num_operands != 3) {
				throw std::runtime_error("Error: not the correct metadata found in an llvm module \"" + function.getName().str() + "\". See doc/meatadata.md for more info");
			}

			// get the desc
			std::string description = static_cast<llvm::MDString*>(metadata->getOperand(0).get())->getString();
			
			// get the number of output exec paths
			int numOutputExecPaths = atoi(static_cast<llvm::MDString*>(metadata->getOperand(1).get())->getString().operator std::string().c_str());

			
			// get the number of inputs
			int num_inputs = atoi(static_cast<llvm::MDString*>(metadata->getOperand(2).get())->getString().operator std::string().c_str());

			// get the rest of the descs from the variable names
			std::vector<std::string> ioDescriptions;
			for(const auto& arg : function.getArgumentList()) {
				ioDescriptions.push_back(arg.getName());
			}
			
			// construct it and add it to the vector
			nodes.push_back(std::make_unique<FunctionCallNodeType>(*context, module.get(), &function, num_inputs, numOutputExecPaths, description, ioDescriptions));

		}
	}

}

std::unique_ptr<NodeType> chig::ImportedModule::createNodeType(const char* name, const nlohmann::json&) const
{
	auto iter = std::find_if(nodes.begin(), nodes.end(), [&](auto& func) {
		return func->name == name;
	});
	return iter == nodes.end() ? std::make_unique<FunctionCallNodeType>(**iter) : nullptr;
}

