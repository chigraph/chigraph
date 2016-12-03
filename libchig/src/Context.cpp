#include "chig/Context.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

using namespace chig;
using namespace llvm;

Context::Context() {}
ChigModule* Context::getModuleByName(const char* moduleName) noexcept
{
	Result res;

	for (auto& module : modules) {
		if (module->name == moduleName) {
			return module.get();
		}
	}
	return nullptr;
}

Result Context::addModule(std::unique_ptr<ChigModule> modToAdd) noexcept
{
	Result res;

	assert(modToAdd != nullptr);

	// make sure it's unique
	auto ptr = getModuleByName(modToAdd->name.c_str());
	if (ptr != nullptr) {
		res.add_entry(
			"E24", "Cannot add already existing module again", {{"moduleName", modToAdd->name}});
		return res;
	}

	modules.emplace_back(std::move(modToAdd));

	return res;
}

Result Context::getType(const char* module, const char* name, llvm::Type** toFill) noexcept
{
	Result res;

	ChigModule* mod = getModuleByName(module);
	if (mod == nullptr) {
		res.add_entry("E36", "Could not find module", {{"module", module}});
		return res;
	}

	*toFill = mod->getType(name);
	if (*toFill == nullptr) {
		res.add_entry("E37", "Could not find type in module", {{"type", name}, {"module", module}});
	}

	return res;
}

Result Context::getNodeType(const char* moduleName, const char* typeName, const nlohmann::json& data,
	std::unique_ptr<NodeType>* toFill) noexcept
{
	Result res;

	auto module = getModuleByName(moduleName);
	if (module == nullptr) {
		res.add_entry("E36", "Could not find module", {{"module", moduleName}});
		return res;
	}

	res += module->createNodeType(typeName, data, toFill);

	return res;
}

std::string Context::stringifyType(llvm::Type* ty)
{
	std::string data;
	llvm::raw_string_ostream stream{data};
	ty->print(stream);
	return stream.str();
}
