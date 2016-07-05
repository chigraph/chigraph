#include "chig/Context.hpp"
#include "chig/ImportedModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>

#include <boost/filesystem.hpp>

using namespace chig;
using namespace llvm;

Context::Context() {
}

ImportedModule* Context::loadModule(const char* path) {
	
	// resolve the file
	std::string abs_path = resolveModulePath(path);
	
	auto buf = MemoryBuffer::getFile(abs_path);
	if(!buf) {
		throw std::runtime_error("Error reading file: + " + buf.getError().message());
	}
	
	auto module = parseBitcodeFile(**buf, context);
	if(!module) {
		throw std::runtime_error("Failed to parse file: " + module.getError().message());
	}
	
	// create `ImportedModule`
	modules.push_back(std::make_unique<ImportedModule>(std::move(*module)));
	
	return modules[modules.size() - 1].get();
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

std::string chig::Context::resolveModulePath(const char* path)
{
	using namespace std::string_literals;
	
	namespace fs = boost::filesystem;
	
	std::string file_name = path + ".bc"s;
	
	// see if it is in `pwd`
	if(fs::is_regular_file(file_name)) {
		return file_name;
	}
	
	// search in searchPaths
	for(auto& search_path : searchPaths) {
		if(fs::is_regular_file(search_path + '/' + file_name)) {
			return search_path;
		}
	}
	
	return {};
}

