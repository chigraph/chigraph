#include "chig/LangModule.hpp"
#include "chig/Context.hpp"

#include <llvm/AsmParser/Parser.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/AsmParser/Parser.h>

using namespace chig;

LangModule::LangModule(Context& contextArg) : ChigModule(contextArg)
{
	using namespace std::string_literals;

	name = "lang";

	// populate them
	nodes = {
		{"if"s, [this](const nlohmann::json&) { return std::make_unique<IfNodeType>(*context); }},
		{"entry"s,
			[this](const nlohmann::json& data) {

				// transform the JSON data into this data structure
				std::vector<std::pair<llvm::Type*, std::string>> inputs;

				for (auto iter = data.begin(); iter != data.end(); ++iter) {
					std::string qualifiedType = iter.value();
					std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
					std::string type = qualifiedType.substr(qualifiedType.find(':') + 1);

					inputs.emplace_back(context->getType(module.c_str(), type.c_str()), iter.key());
				}

				return std::make_unique<EntryNodeType>(*context, inputs);

			}},
		{"exit"s, [this](const nlohmann::json& data) {
			 // transform the JSON data into this data structure
			 std::vector<std::pair<llvm::Type*, std::string>> outputs;

			 for (auto iter = data.begin(); iter != data.end(); ++iter) {
				 std::string qualifiedType = iter.value();
				 std::string module = qualifiedType.substr(0, qualifiedType.find(':'));
				 std::string type = qualifiedType.substr(qualifiedType.find(':') + 1);

				 outputs.emplace_back(context->getType(module.c_str(), type.c_str()), iter.key());
			 }

			 return std::make_unique<ExitNodeType>(*context, outputs);

		 }},
		{"const-int"s, [this](const nlohmann::json& data) {
			int num = data;
			
			return std::make_unique<ConstIntNodeType>(*context, num);
		}}
	};
}

std::unique_ptr<NodeType> LangModule::createNodeType(
	const char* name, const nlohmann::json& json_data) const
{
	auto iter = nodes.find(name);
	if (iter != nodes.end()) {
		return iter->second(json_data);
	}
	return nullptr;
}

// the lang module just has the basic llvm types.
llvm::Type* LangModule::getType(const char* name) const
{
	using namespace std::string_literals;
	
	// just parse the type
	auto IR = "@G = external global "s + name;
	auto err = llvm::SMDiagnostic();
	auto tmpModule = llvm::parseAssemblyString(IR, err, context->context);
	if(!tmpModule) return nullptr;
	
	// returns the pointer type, so get the contained type
	return tmpModule->getNamedValue("G")->getType()->getContainedType(0); 
}
