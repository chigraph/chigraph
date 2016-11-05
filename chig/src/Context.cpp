#include "chig/Context.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

using namespace chig;
using namespace llvm;

Context::Context() { }

ChigModule* Context::getModuleByName(const char* moduleName)
{
	if(!moduleName) throw std::runtime_error("Cannot pass nullptr into getModuleByName");
	
	for (auto& module : modules) {
		if (module->name == moduleName) {
			return module.get();
		}
	}
	return nullptr;
}

std::string chig::Context::resolveModulePath(const char* path)
{
	using namespace std::string_literals;

	namespace fs = boost::filesystem;

	std::string file_name = path;

	// see if it is in `pwd`
	if (fs::is_regular_file(file_name)) {
		return file_name;
	}

	// search in searchPaths
	for (auto& search_path : searchPaths) {
		if (fs::is_regular_file(search_path + '/' + file_name)) {
            return search_path + '/' + file_name;
		}
	}

	return {};
}

void Context::addModule(std::unique_ptr<ChigModule> modToAdd)
{
	if(!modToAdd) throw std::runtime_error("Cannot add nullptr as module to a Context");
	
	// make sure it's unique
	auto ptr = getModuleByName(modToAdd->name.c_str());
	if(ptr != nullptr) throw std::runtime_error("Cannot add module named " + modToAdd->name + " for a second time.");
	
	modules.emplace_back(std::move(modToAdd));
}

llvm::Type* Context::getType(const char* module, const char* name)
{
	auto mod = getModuleByName(module);
	if(!mod) return nullptr;
	return mod->getType(name);
}

std::unique_ptr<NodeType> Context::getNodeType(
	const char* moduleName, const char* name, const nlohmann::json& data)
{
	auto module = getModuleByName(moduleName);
	if (!module) return nullptr;

	return module->createNodeType(name, data);
}

std::string chig::Context::stringifyType(llvm::Type* ty)
{
	std::string data;
	llvm::raw_string_ostream stream{data};
	ty->print(stream);
	return stream.str();
}
