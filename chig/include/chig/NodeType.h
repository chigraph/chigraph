#ifndef CHIG_NODE_TYPE_H
#define CHIG_NODE_TYPE_H

#pragma once

#include "chig/Module.h"

#include <llvm-c/Core.h>

typedef enum EChigNodeType {
	CHIG_NODE_TYPE_BULIT_IN,
	CHIG_NODE_TYPE_IMPORTED
} EChigNodeType;

typedef enum EChigBuiltInNodeType {
	CHIG_BUILT_IN_NODE_TYPE_IF,
	CHIG_BUILT_IN_NODE_TYPE_ENTRY,
	CHIG_BUILT_IN_NODE_TYPE_RETURN
} EChigBuiltInNodeType;

// built-in types
typedef struct ChigNodeTypeBuiltIn {
	
	EChigBuiltInNodeType type;
	
} ChigNodeTypeBuiltIn;

// imported types
typedef struct ChigNodeTypeImported {

	struct ChigModule* module;
	
} ChigNodeTypeImported;

// generic type
typedef struct ChigNodeType {
	
	EChigNodeType type;
	const char* name;
	const char* description;

	// inputs and outputs
	LLVMTypeRef* inputs;
	size_t numInputs;

	LLVMTypeRef* outptus;
	size_t numOutputs;

	
	union {
		ChigNodeTypeBuiltIn builtInNode;
		ChigNodeTypeImported importedNode;
	};
	
} ChigNodeType;


#endif // CHIG_NODE_TYPE_H
