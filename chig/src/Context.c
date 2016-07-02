#include "chig/Context.h"
#include "chig/Module.h"

#include <llvm-c/BitReader.h>
#include <llvm-c/Core.h>


#include <stdlib.h>
#include <string.h>

ChigContext* ChigCreateContext() {
	ChigContext* context = (ChigContext*)malloc(sizeof(ChigContext));

	context->modules = NULL;
	context->numModules = 0;

	return context;
}

void ChigDestroyContext(ChigContext* context) {

	// destroy the modules
	for(size_t i = 0; i < context->numModules; ++i) {
		LLVMDisposeModule(context->modules[i].module);
		free(context->modules[i].nodes);
	}

	free(context);
}

ChigModule* ChigLoadModule(ChigContext* context, const char* path) {
	// TOOD: implement
	return NULL;
}

void ChigUnloadModule(ChigContext* context, ChigModule* toUnload) {
	// TODO: implement
}

ChigModule* ChigGetModuleByName(ChigContext* context, const char* moduleName) {
	for(size_t i = 0; i < context->numModules; ++i) {
		if(strcmp(context->modules[i].name, moduleName)) {
			return &context->modules[i];
		}
	}
}

ChigNodeType ChigGetNodeType(ChigContext* context, const char* module, const char* name) {
	// if it is from lang
	int is_lang = strcmp(module, "lang");

	// get how many pointers
	unsigned int num_pointers = 0;
	while(name[strlen(name) - 1 - num_pointers] == '*') ++num_pointers;

	// find the ChigModule that holds it
}

ChigModule* ChigParseFromJSON(json_object* inJson) {
	// get the module header
	json_object* module_object;
	json_bool suceeded = json_object_object_get_ex(inJson, "module", &module_object);
	if(!suceeded) {
		return NULL;
	}

	// get name
	json_object* module_name_json;
	suceeded = json_object_object_get_ex(module_object, "name", &module_name_json);
	if(!suceeded) {
		return NULL;
	}

	const char* moduleName = json_object_get_string(module_name_json);

	// load dependencies
	// TODO: load dependencies

	// start compiling the graph

	json_object* graph_json;
	suceeded = json_object_object_get_ex(inJson, "graphs", &graph_json);
	if(!suceeded) {
		return NULL;
	}



}
