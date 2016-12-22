#include "chig/JsonModule.hpp"

#include <chig/NodeInstance.hpp>
#include "chig/GraphFunction.hpp"
#include "chig/NameMangler.hpp"
#include "chig/NodeType.hpp"
#include "chig/Result.hpp"

#include <llvm/IR/Module.h>

namespace chig
{
JsonModule::JsonModule(
	Context& cont, std::string fullName, const nlohmann::json& json_data, Result* res)
	: ChigModule(cont, fullName)
{
	Expects(res != nullptr);

	// load dependencies
	{
		auto iter = json_data.find("dependencies");
		if (iter == json_data.end()) {
			res->addEntry("E38", "No dependencies element in module", {});
			return;
		}
		if (!iter->is_array()) {
			res->addEntry("E39", "dependencies element isn't an array", {});
			return;
		}

		for (const auto& dep : *iter) {
			if (!dep.is_string()) {
				res->addEntry("E40", "dependency isn't a string", {{"Actual Data", dep}});
				continue;
			}
			*res += addDependency(dep);

			if (!*res) {
				return;
			}
		}
	}

	// load graphs
	{
		auto iter = json_data.find("graphs");
		if (iter == json_data.end()) {
			res->addEntry("E41", "no graphs element in module", {});
			return;
		}
		if (!iter->is_array()) {
			res->addEntry("E42", "graph element isn't an array", {{"Actual Data", *iter}});
			return;
		}
		mFunctions.reserve(iter->size());
		for (const auto& graph : *iter) {
			std::unique_ptr<GraphFunction> newf;
			*res += GraphFunction::fromJSON(*this, graph, &newf);

			if (!*res) {
				return;
			}

			Expects(newf != nullptr);

			mFunctions.push_back(std::move(newf));
		}
	}
}

JsonModule::JsonModule(Context& cont, std::string fullName, gsl::span<std::string> dependencies)
	: ChigModule(cont, fullName)
{
	// load the dependencies from the context
	for (const auto& dep : dependencies) {
		addDependency(dep);
	}
}

Result JsonModule::generateModule(std::unique_ptr<llvm::Module>* mod)
{
	Result res = {};

	// create prototypes
	for (auto& graph : mFunctions) {
		(*mod)->getOrInsertFunction(
			mangleFunctionName(fullName(), graph->name()), graph->functionType());
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
	ret["dependencies"] = dependencies();

	auto& graphsjson = ret["graphs"];
	graphsjson = nlohmann::json::array();
	for (auto& graph : mFunctions) {
		nlohmann::json to_fill = {};
		res += graph->toJSON(&to_fill);
		graphsjson.push_back(to_fill);
	}

	return res;
}

Result JsonModule::createFunction(gsl::cstring_span<> name,
	std::vector<std::pair<DataType, std::string> > ins,
	std::vector<std::pair<DataType, std::string> > outs, GraphFunction** toFill)
{
	Result res;
	// make sure there already isn't one by this name
	if (graphFuncFromName(name) != nullptr) {
		res.addEntry(
			"EUKN", "Function already exists", {{"Requested Name", gsl::to_string(name)}});
		return res;
	}

	mFunctions.push_back(std::make_unique<GraphFunction>(*this, name, ins, outs));
	if (toFill) {
		*toFill = mFunctions[mFunctions.size() - 1].get();
	}

    return res;
}

bool JsonModule::removeFunction(gsl::cstring_span<> name)
{
    auto funcPtr = graphFuncFromName(name);

    if(funcPtr == nullptr ) {
        return false;
    }

    removeFunction(funcPtr);
}

void JsonModule::removeFunction(GraphFunction *func)
{
    Expects(func != nullptr);

    auto iter = std::find_if(mFunctions.begin(), mFunctions.end(), [func](auto& uPtr) {
        return uPtr.get() == func;
    });
    if(iter == mFunctions.end()) {
        return;
    }

    mFunctions.erase(iter);
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
		res.addEntry("EUKN", "Graph not found in module",
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
		Expects(graph != nullptr);
		res += graph->loadGraph();
	}

	return {};  // res;
}

JsonFuncCallNodeType::JsonFuncCallNodeType(
	JsonModule& json_module, gsl::cstring_span<> funcname, Result* resPtr)
	: NodeType(json_module, funcname, ""), JModule(&json_module)  // TODO: description
{
	Result& res = *resPtr;

	auto* mygraph = JModule->graphFuncFromName(funcname);

	if (mygraph == nullptr) {
		res.addEntry("EUKN", "Graph doesn't exist in module",
			{{"Module Name", JModule->name()}, {"Requested Name", gsl::to_string(funcname)}});
		return;
	}

	setDataOutputs(mygraph->outputs());

	setDataInputs(mygraph->inputs());

	setExecInputs({""});
	setExecOutputs({""});
}

Result JsonFuncCallNodeType::codegen(size_t /*execInputID*/, llvm::Module* mod,
	llvm::Function* /*f*/, const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	const gsl::span<llvm::BasicBlock*> outputBlocks) const
{
	Result res = {};

	llvm::IRBuilder<> builder(codegenInto);

	auto func = mod->getFunction(mangleFunctionName(module().fullName(), name()));

	if (func == nullptr) {
		res.addEntry(
			"EUKN", "Could not find function in llvm module", {{"Requested Function", name()}});
		return res;
	}

	// TODO: output blocks
	builder.CreateCall(func, {io.data(), (size_t)io.size()});

	builder.CreateBr(outputBlocks[0]);

	return res;
}

nlohmann::json JsonFuncCallNodeType::toJSON() const { return {}; }
std::unique_ptr<NodeType> JsonFuncCallNodeType::clone() const
{
	Result res = {};  // there shouldn't be an error but check anywayss
	// TODO: better way to do this?
	return std::make_unique<JsonFuncCallNodeType>(*JModule, name(), &res);
}

}  // namespace chig
