#include "chig/Context.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

using namespace chig;
using namespace llvm;

Context::Context() { }

Result Context::getModuleByName(const char* moduleName, ChigModule** ret) noexcept
{
    return
	
	for (auto& module : modules) {
		if (module->name == moduleName) {
			return module.get();
		}
	}
	return nullptr;
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
