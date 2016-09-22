#include "chig/Context.hpp"
#include "chig/ImportedModule.hpp"
#include "chig/LangModule.hpp"
#include "chig/NodeType.hpp"

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/filesystem.hpp>

using namespace chig;
using namespace llvm;

Context::Context() { addModule(std::make_unique<LangModule>(*this)); }
ImportedModule* Context::loadModuleFromBc(const char* path)
{
	using namespace std::string_literals;

	// resolve the file
	std::string abs_path = resolveModulePath(path);
	if (abs_path == "") {
		throw std::runtime_error("Failed to file module: "s + path);
	}

	auto buf = MemoryBuffer::getFile(abs_path);
	if (!buf) {
		throw std::runtime_error("Error reading file: " + buf.getError().message());
	}

	auto module = parseBitcodeFile(**buf, context);
	if (!module) {
		throw std::runtime_error("Failed to parse file: " + module.getError().message());
	}

	// create `ImportedModule`
	auto impModule = std::make_unique<ImportedModule>(*this, std::move(*module));

	// cache the result because impModule will be moved into the vector
	auto ret = impModule.get();

	modules.push_back(std::move(impModule));

	return ret;
}

bool Context::unloadModule(ChigModule* toUnload)
{
	auto iter = std::find_if(
		modules.begin(), modules.end(), [=](auto& ptr) { return ptr.get() == toUnload; });
	if (iter != modules.end()) {
		modules.erase(iter);
		return true;
	}

	return false;
}

ChigModule* Context::getModuleByName(const char* moduleName)
{
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
			return search_path;
		}
	}

	return {};
}

void Context::addModule(std::unique_ptr<ChigModule> modToAdd)
{
	modules.emplace_back(std::move(modToAdd));
}

llvm::Type* Context::getType(const char* module, const char* name)
{
	return getModuleByName(module)->getType(name);
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
