#include "chig/Module.h"

#include <stdlib.h>
#include <string.h>

ChigModule* ChigCreateModuleFromLLVMModule(LLVMModuleRef module, const char* name) {
	ChigModule* ret = (ChigModule*)malloc(sizeof(ChigModule));

	// copy the name into it
	ret->name = (char*)malloc(strlen(name) + 1);
	strcpy(ret->name, name);

	ret->module = module;
	ret->numNodes = 0;

	// start finding nodes
	LLVMValueRef function = LLVMGetFirstFunction(module);
	while(function) {

		// create the chig-<func name> string
		char buffer[1000];
		strcpy(buffer, "chig-");
		strcat(buffer, LLVMGetValueName(function));

		// see if it has the metadata tags. It is a named metadata in the module called chig-<func name>
		LLVMValueRef metadata;
		LLVMGetNamedMetadataOperands(module, buffer, &metadata);
		if(metadata) {
			// extract the metadata
			unsigned num_operands = LLVMGetMDNodeNumOperands(metadata);
			LLVMValueRef* nodes = (LLVMValueRef*)malloc(sizeof(LLVMValueRef) * num_operands);
			LLVMGetMDNodeOperands(metadata, nodes);

			// allocate more space in the array
			if(!ret->nodes) {
				ret->nodes = (ChigNodeType*)malloc(sizeof(ChigNodeType));
				ret->numNodes = 1;

			} else {
				ret->nodes = (ChigNodeType*)realloc(ret->nodes, sizeof(ChigNodeType) * (ret->numNodes + 1));
			}
			// zero the new data
			memset(&ret->nodes[ret->numNodes - 1], 0, sizeof(ChigNodeType));

			ChigNodeType* node = &ret->nodes[ret->numNodes - 1];

			node->type = CHIG_NODE_TYPE_IMPORTED;
			strcpy(node->name, LLVMGetValueName(function));



			free(nodes);
		}

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
		if(strcmp(module->nodes[i].name, name)) {
			return module->nodes + i;
		}
	}
	return NULL;
}
