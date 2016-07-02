#ifndef CHIG_CONTEXT_H
#define CHIG_CONTEXT_H

#pragma once

#include "chig/Module.h"
#include "chig/NodeType.h"

#include <llvm-c/Core.h>

#include <json-c/json.h>

typedef struct {

	ChigModule* modules;
	size_t numModules;

	LLVMContextRef context;

} ChigContext;

ChigContext* ChigCreateContext();
void ChigDestroyContext(ChigContext* context);

// path should be a path to a .bc file
ChigModule* ChigLoadModule(ChigContext* context, const char* path);
void ChigUnloadModule(ChigContext* context, ChigModule* toUnload);

ChigModule* ChigGetModuleByName(ChigContext* context, const char* moduleName);

ChigNodeType ChigGetNodeType(ChigContext* context, const char* module, const char* name);


// parse from JSON
ChigModule* ChigParseFromJSON(json_object* inJson);

#endif // CHIG_CONTEXT_H
