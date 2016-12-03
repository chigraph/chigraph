#include "chig/Context.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

#include <gsl/gsl>

using namespace chig;
using namespace llvm;

Context::Context() {}
ChigModule* Context::getModuleByName(gsl::cstring_span<> moduleName) noexcept
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
    Expects(modToAdd != nullptr);
  
	Result res;

	// make sure it's unique
	auto ptr = getModuleByName(modToAdd->name);
	if (ptr != nullptr) {
		res.add_entry(
			"E24", "Cannot add already existing module again", {{"moduleName", modToAdd->name}});
		return res;
	}

	modules.emplace_back(std::move(modToAdd));

	return res;
}

Result Context::getType(gsl::cstring_span<> module, gsl::cstring_span<> name, llvm::Type** toFill) noexcept
{
	Result res;

	ChigModule* mod = getModuleByName(module);
	if (mod == nullptr) {
		res.add_entry("E36", "Could not find module", {{"module", gsl::to_string(module)}});
		return res;
	}

	*toFill = mod->getType(name);
	if (*toFill == nullptr) {
		res.add_entry("E37", "Could not find type in module", {{"type", gsl::to_string(name)}, {"module", gsl::to_string(module)}});
	}

	return res;
}

Result Context::getNodeType(gsl::cstring_span<> moduleName, gsl::cstring_span<> typeName, const nlohmann::json& data,
	std::unique_ptr<NodeType>* toFill) noexcept
{
	Result res;

	auto module = getModuleByName(moduleName);
	if (module == nullptr) {
		res.add_entry("E36", "Could not find module", {{"module", gsl::to_string(moduleName)}});
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
