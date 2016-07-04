#include "chig/ImportedModule.hpp"

#include <stdlib.h>
#include <string.h>

using namespace chig;

ImportedModule::ImportedModule(std::unique_ptr<llvm::Module> arg_module) {

	module = std::move(arg_module);

	// start finding nodes
	for(auto&& function : module->getFunctionList()) {

		// create the chig-<func name> string
		auto metadataTagName = "chig-" + function.getName();

		// see if it has the metadata tags. It is a named metadata in the module called chig-<func name>
		auto metadata = module->getNamedMetadata(metadataTagName);
		if(metadata) {
			// extract the metadata
			unsigned num_operands = metadata->getNumOperands();

			auto newNodeType = std::make_unique<FunctionCallNodeType>();

			// set the function type
			newNodeType->function = function;

			// get the description
			strcpy(node->description, LLVMGetMDString(mdnodes[0], NULL));

			// get the number of inputs
			int num_inputs = atoi(LLVMGetMDString(mdnodes[1], NULL));

			// allocate the inputs and outputs
			node->inputs = malloc(sizeof(LLVMTypeRef) * num_inputs)

			// read the types from the function
			LLVMValueRef value = LLVMGetFirstParam(function);
			while(value) {

				LLVMTypeOf()

				// go to the next value
				value = LLVMGetNextParam(value);
			}

		}

		// go on to the next one
		function = LLVMGetNextFunction(function);
	}

}


void ChigDestroyModule(ChigModule* module) {
	free(module->nodes);
	free(module);
}

ChigNodeType* ChigGetNodeTypeByName(ChigModule* module, const char* name) {

	for(size_t i = 0; i < module->numNodes; ++i) {
		if(strcmp(module->nodes[i].name, name)) {
			return module->nodes + i;
		}
	}
	return NULL;
}
