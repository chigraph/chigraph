#include "chig/JsonModule.hpp"

#include <chig/NodeInstance.hpp>
#include "chig/GraphFunction.hpp"
#include "chig/NodeType.hpp"
#include "chig/Result.hpp"

#include <llvm/IR/Module.h>

using namespace chig;

JsonModule::JsonModule(const nlohmann::json& json_data, Context& cont, Result* res)
	: ChigModule(cont)
{
	// load name
	{
		auto iter = json_data.find("name");
		if (iter == json_data.end()) {
			res->add_entry("E34", "No name element in module", {});
			return;
		}
		if (!iter->is_string()) {
			res->add_entry("E35", "name element in module isn't a string", {});
			return;
		}
		setName(*iter);
	}
	// load dependencies
	{
		auto iter = json_data.find("dependencies");
		if (iter == json_data.end()) {
			res->add_entry("E38", "No dependencies element in module", {});
			return;
		}
		if (!iter->is_array()) {
			res->add_entry("E39", "dependencies element isn't an array", {});
			return;
		}
		mDependencies.reserve(iter->size());
		for (const auto& dep : *iter) {
			if (!dep.is_string()) {
				res->add_entry("E40", "dependency isn't a string", {{"Actual Data", dep}});
				continue;
			}
			mDependencies.push_back(dep);
		}
	}
	// load the dependencies from the context
	for (const auto& dep : mDependencies) {
		context().addModule(dep);
	}

	// load graphs
	{
		auto iter = json_data.find("graphs");
		if (iter == json_data.end()) {
			res->add_entry("E41", "no graphs element in module", {});
			return;
		}
		if (!iter->is_array()) {
			res->add_entry("E42", "graph element isn't an array", {{"Actual Data", *iter}});
			return;
		}
		mFunctions.reserve(iter->size());
		for (const auto& graph : *iter) {
			std::unique_ptr<GraphFunction> newf;
			*res += GraphFunction::fromJSON(context(), graph, &newf);
			mFunctions.push_back(std::move(newf));
		}
	}
}

Result JsonModule::generateModule(std::unique_ptr<llvm::Module>* mod)
{
	// create llvm module
	*mod = std::make_unique<llvm::Module>(name(), context().llvmContext());

	Result res = {};

	// create prototypes
	for (auto& graph : mFunctions) {
		(*mod)->getOrInsertFunction(graph->name(), graph->functionType());
	}

	for (auto& graph : mFunctions) {
		llvm::Function* f;
		res += graph->compile(mod->get(), &f);
	}

	return res;
}

Result JsonModule::toJSON(nlohmann::json* to_fill) const
{
	auto& ret = *to_fill;
	ret = nlohmann::json::object();

	Result res = {};

	ret["name"] = name();
	ret["dependencies"] = mDependencies;

	auto& graphsjson = ret["graphs"];
	graphsjson = nlohmann::json::array();
	for (auto& graph : mFunctions) {
		nlohmann::json to_fill = {};
		res += graph->toJSON(&to_fill);
		graphsjson.push_back(to_fill);
	}

	return res;
}

GraphFunction* JsonModule::graphFuncFromName(gsl::cstring_span<> name) const
{
	auto iter = std::find_if(
		mFunctions.begin(), mFunctions.end(), [&](auto& ptr) { return ptr->name() == name; });

	if (iter != mFunctions.end()) {
		return iter->get();
	}
	return nullptr;
}

Result JsonModule::nodeTypeFromName(
	gsl::cstring_span<> name, const nlohmann::json& /*jsonData*/, std::unique_ptr<NodeType>* toFill)
{
	Result res = {};

	auto graph = graphFuncFromName(name);

	if (graph == nullptr) {
		res.add_entry("EUKN", "Graph not found in module",
			{{"Module Name", gsl::to_string(name)}, {"Requested Graph", gsl::to_string(name)}});
	}

	*toFill = std::make_unique<JsonFuncCallNodeType>(*this, name, &res);
	return res;
}

std::vector<std::string> JsonModule::nodeTypeNames() const
{
	std::vector<std::string> ret;
	std::transform(mFunctions.begin(), mFunctions.end(), std::back_inserter(ret),
		[](auto& gPtr) { return gPtr->name(); });

	return ret;
}

Result JsonModule::loadGraphs()
{
	Result res = {};

	for (auto& graph : mFunctions) {
		res += graph->loadGraph();
	}

	return res;
}

JsonFuncCallNodeType::JsonFuncCallNodeType(
	JsonModule& json_module, gsl::cstring_span<> funcname, Result* resPtr)
	: NodeType(json_module), JModule(&json_module)
{
	Result& res = *resPtr;

	auto* mygraph = JModule->graphFuncFromName(funcname);

	if (mygraph == nullptr) {
		res.add_entry("EUKN", "Graph doesn't exist in module",
			{{"Module Name", JModule->name()}, {"Requested Name", gsl::to_string(funcname)}});
		return;
	}

	dataOutputs = mygraph->outputs();

	dataInputs = mygraph->inputs();

	name = gsl::to_string(funcname);
	// TODO: description

	execInputs = {""};
	execOutputs = {""};
}

Result JsonFuncCallNodeType::codegen(size_t /*execInputID*/, llvm::Module* mod,
	llvm::Function* /*f*/, const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Result res = {};

	llvm::IRBuilder<> builder(codegenInto);

	// TODO: intermodule calls

	auto func = mod->getFunction(name);

	if (func == nullptr) {
		res.add_entry(
			"EUKN", "Could not find function in llvm module", {{"Requested Function", name}});
		return res;
	}

	// TODO: output blocks
	builder.CreateCall(mod->getFunction(name), {io.data(), (size_t)io.size()});

	builder.CreateBr(outputBlocks[0]);

	return res;
}

nlohmann::json JsonFuncCallNodeType::toJSON() const { return {}; }
std::unique_ptr<NodeType> JsonFuncCallNodeType::clone() const
{
	Result res = {};  // there shouldn't be an error but check anywayss
	// TODO: better way to do this?
	return std::make_unique<JsonFuncCallNodeType>(*const_cast<JsonModule*>(JModule), name, &res);
}
