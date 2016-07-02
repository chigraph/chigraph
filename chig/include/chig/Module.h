#ifndef CHIG_MODULE_H
#define CHIG_MODULE_H

#pragma once

#include "chig/NodeType.h"

#include <llvm-c/Core.h>

typedef struct {
	LLVMModuleRef module;

	ChigNodeTypeImported* nodes;
	size_t numNodes;

	const char* name;

} ChigModule;



#endif // CHIG_MODULE_H
