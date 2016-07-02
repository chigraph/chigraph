#ifndef CHIG_MODULE_H
#define CHIG_MODULE_H

#pragma once

#include "chig/NodeType.h"

#include <llvm-c/Core.h>

typedef struct ChigModule {
	LLVMModuleRef module;

	ChigNodeType* nodes;
	size_t numNodes;

	const char* name;

} ChigModule;

ChigModule* ChigCreateModuleFromLLVMModule(LLVMModuleRef module, const char* name);
void ChigDestroyModule(ChigModule* module);

ChigNodeType* ChigGetNodeTypeByName(ChigModule* module, const char* name);



#endif // CHIG_MODULE_H
