#include "chig/Module.h"

#include <stdlib.h>
#include <string.h>

ChigModule* ChigCreateModuleFromLLVMModule(LLVMModuleRef module, const char* name) {
	ChigModule* ret = (ChigModule*)malloc(sizeof(ChigModule));

	// copy the name into it
	char* name_tmp = (char*)malloc(strlen(name) + 1);
	memcpy(name_tmp, name, strlen(name) + 1);
	ret->name = name_tmp;

	ret->module = module;
	ret->numNodes = 0;

	// start finding nodes
	LLVMValueRef function = LLVMGetFirstFunction(module);
	while(function) {

		// see if it has the metadata tags

		// go on to the next one
		function = LLVMGetNextFunction(function);
	}

}


void ChigDestroyModule(ChigModule* module) {
	free(module->nodes);
	free(module->name);
	free(module);
}

ChigNodeType* ChigGetNodeTypeByName(ChigModule* module, const char* name) {

	for(size_t i = 0; i < module->numNodes; ++i) {
		if(strcmp(module->nodes[i].name, name) {
			return module->nodes + i;
		}
	}

}
