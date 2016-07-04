#include "chig/Context.hpp"
#include "chig/ImportedModule.hpp"

using namespace chig;

ImportedModule* Context::loadModule(const char* path) {
	// TOOD: implement
	return NULL;
}

void Context::unloadModule(ImportedModule* toUnload) {
	auto iter = std::find_if(modules.begin(), modules.end(), [=](auto& ptr){return ptr.get() == toUnload; });
	if(iter != modules.end()) {
		modules.erase(iter);
	}
}

ImportedModule* Context::getModuleByName(const char* moduleName) {
	for(auto& module : modules) {
		if(module->module->getName() == moduleName) {
			return module.get();
		}
	}
	return NULL;
}
