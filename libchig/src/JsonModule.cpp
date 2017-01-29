/// \file JsonModule.cpp

#include "chig/JsonModule.hpp"

#include "chig/FunctionCompiler.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/NameMangler.hpp"
#include "chig/NodeType.hpp"
#include "chig/JsonSerializer.hpp"
#include "chig/NodeInstance.hpp"

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace chig {
struct JsonFuncCallNodeType : public NodeType {
	JsonFuncCallNodeType(JsonModule& json_module, gsl::cstring_span<> funcname, Result* resPtr)
	    : NodeType(json_module, funcname, ""),
	      JModule(&json_module)  // TODO: description
	{
		Result& res = *resPtr;

		auto* mygraph = JModule->graphFuncFromName(funcname);

		if (mygraph == nullptr) {
			res.addEntry(
			    "EUKN", "Graph doesn't exist in module",
			    {{"Module Name", JModule->name()}, {"Requested Name", gsl::to_string(funcname)}});
			return;
		}

		setDataOutputs(mygraph->dataOutputs());

		setDataInputs(mygraph->dataInputs());

		setExecInputs(mygraph->execInputs());
		setExecOutputs(mygraph->execOutputs());
	}

	Result codegen(size_t execInputID, llvm::Module* mod, const llvm::DebugLoc& nodeLocation,
	               llvm::Function* /*f*/, const gsl::span<llvm::Value*> io,
	               llvm::BasicBlock*                  codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Result res = {};

		llvm::IRBuilder<> builder(codegenInto);

		auto func = mod->getFunction(mangleFunctionName(module().fullName(), name()));

		if (func == nullptr) {
			res.addEntry("EUKN", "Could not find function in llvm module",
			             {{"Requested Function", name()}});
			return res;
		}

		// add the execInputID to the argument list
		std::vector<llvm::Value*> passingIO;
		passingIO.push_back(llvm::ConstantInt::get(
		    llvm::IntegerType::getInt32Ty(context().llvmContext()), execInputID));

		std::copy(io.begin(), io.end(), std::back_inserter(passingIO));

		auto ret = builder.CreateCall(func, passingIO, "call_function");
		ret->setDebugLoc(nodeLocation);

		// create switch on return
		auto switchInst = builder.CreateSwitch(ret, outputBlocks[0]);  // TODO: better default

		auto id = 0ull;
		for (auto out : outputBlocks) {
			switchInst->addCase(
			    llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context().llvmContext()), id),
			    out);
		}

		return res;
	}

	nlohmann::json            toJSON() const override { return {}; }
	std::unique_ptr<NodeType> clone() const override {
		Result res = {};  // there shouldn't be an error but check anywayss
		// TODO: better way to do this?
		return std::make_unique<JsonFuncCallNodeType>(*JModule, name(), &res);
	}

	JsonModule* JModule;
};

JsonModule::JsonModule(Context& cont, std::string fullName, gsl::span<std::string> dependencies)
    : ChigModule(cont, fullName) {
	// load the dependencies from the context
	for (const auto& dep : dependencies) { addDependency(dep); }
}

Result JsonModule::generateModule(llvm::Module& module) {
	Result res = {};

	// debug info
	llvm::DIBuilder debugBuilder(module);
	auto            compileUnit = debugBuilder.createCompileUnit(
	    llvm::dwarf::DW_LANG_C, sourceFilePath().filename().string(),
	    sourceFilePath().parent_path().string(), "Chigraph Compiler", false, "", 0);

	// create prototypes
	for (auto& graph : mFunctions) {
		module.getOrInsertFunction(mangleFunctionName(fullName(), graph->name()),
		                           graph->functionType());
	}

	for (auto& graph : mFunctions) {
		res += compileFunction(*graph, &module, compileUnit, debugBuilder);
	}

	debugBuilder.finalize();

	return res;
}

Result JsonModule::saveToDisk() const {
	Result res;

	// can't serialize without a workspace...
	if (!context().hasWorkspace()) {
		res.addEntry("EUKN", "Cannot serialize without a worksapce", {});
		return res;
	}

	auto modulePath = sourceFilePath();

	try {
		// create directories that conatain the path
		fs::create_directories(modulePath.parent_path());

	} catch (std::exception& e) {
		res.addEntry("EUKN", "Failed to create directoires in workspace",
		             {{"Module File", modulePath.string()}});
		return res;
	}

	// serialize
	nlohmann::json toFill = jsonModuleToJson(*this);

	// save
	fs::ofstream ostr(modulePath);
	ostr << toFill.dump(2);

	return res;
}

bool JsonModule::createFunction(gsl::cstring_span<> name,
                                std::vector<std::pair<DataType, std::string> > dataIns,
                                std::vector<std::pair<DataType, std::string> > dataOuts,
                                std::vector<std::string> execIns, std::vector<std::string> execOuts,
                                GraphFunction** toFill) {
	// make sure there already isn't one by this name
	auto foundFunc = graphFuncFromName(name);
	if (foundFunc != nullptr) {
		if (toFill != nullptr) { *toFill = foundFunc; }
		return false;
	}

	mFunctions.push_back(std::make_unique<GraphFunction>(*this, name, std::move(dataIns),
	                                                     std::move(dataOuts), std::move(execIns),
	                                                     std::move(execOuts)));
	if (toFill != nullptr) { *toFill = mFunctions[mFunctions.size() - 1].get(); }

	return true;
}

bool JsonModule::removeFunction(gsl::cstring_span<> name) {
	auto funcPtr = graphFuncFromName(name);

	if (funcPtr == nullptr) { return false; }

	removeFunction(funcPtr);

	return true;
}

void JsonModule::removeFunction(GraphFunction* func) {
	Expects(func != nullptr);

	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [func](auto& uPtr) { return uPtr.get() == func; });
	if (iter == mFunctions.end()) { return; }

	mFunctions.erase(iter);
}

GraphFunction* JsonModule::graphFuncFromName(gsl::cstring_span<> name) const {
	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [&](auto& ptr) { return ptr->name() == name; });

	if (iter != mFunctions.end()) { return iter->get(); }
	return nullptr;
}

Result JsonModule::nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& /*jsonData*/,
                                    std::unique_ptr<NodeType>* toFill) {
	Result res = {};

	auto graph = graphFuncFromName(name);

	if (graph == nullptr) {
		res.addEntry(
		    "EUKN", "Graph not found in module",
		    {{"Module Name", gsl::to_string(name)}, {"Requested Graph", gsl::to_string(name)}});
	}

	*toFill = std::make_unique<JsonFuncCallNodeType>(*this, name, &res);
	return res;
}

std::vector<std::string> JsonModule::nodeTypeNames() const {
	std::vector<std::string> ret;
	std::transform(mFunctions.begin(), mFunctions.end(), std::back_inserter(ret),
	               [](auto& gPtr) { return gPtr->name(); });

	return ret;
}

boost::bimap<unsigned int, NodeInstance*> JsonModule::createLineNumberAssoc() const {
	// create a sorted list of GraphFunctions
	std::vector<NodeInstance*> nodes;
	for (const auto& f : functions()) {
		for (const auto& node : f->nodes()) {
			Expects(node.second != nullptr);
			nodes.push_back(node.second.get());
		}
	}

	std::sort(nodes.begin(), nodes.end(), [](const auto& lhs, const auto& rhs) {
		return (lhs->function().name() + ":" + lhs->id()) <
		       (rhs->function().name() + ":" + rhs->id());
	});

	boost::bimap<unsigned, NodeInstance*> ret;
	for (unsigned i = 0; i < nodes.size(); ++i) {
		ret.left.insert({i + 1, nodes[i]});  // + 1 because line numbers start at 1
	}

	return ret;
}


}  // namespace chig
