/// \file GraphModule.cpp

#include "chig/GraphModule.hpp"

#include "chig/FunctionCompiler.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/GraphStruct.hpp"
#include "chig/JsonSerializer.hpp"
#include "chig/NameMangler.hpp"
#include "chig/NodeInstance.hpp"
#include "chig/NodeType.hpp"

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <boost/filesystem.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace fs = boost::filesystem;

namespace chig {

namespace {
struct GraphFuncCallType : public NodeType {
	GraphFuncCallType(GraphModule& json_module, gsl::cstring_span<> funcname, Result* resPtr)
	    : NodeType(json_module, funcname, ""), JModule(&json_module) {
		Result& res = *resPtr;

		auto* mygraph = JModule->functionFromName(funcname);
		setDescription(mygraph->description());

		if (mygraph == nullptr) {
			res.addEntry("EINT", "Graph doesn't exist in module",
			             {{"Module Name", JModule->fullName()},
			              {"Requested Name", gsl::to_string(funcname)}});
			return;
		}

		setDataOutputs(mygraph->dataOutputs());

		setDataInputs(mygraph->dataInputs());

		setExecInputs(mygraph->execInputs());
		setExecOutputs(mygraph->execOutputs());
	}

	Result codegen(size_t execInputID, const llvm::DebugLoc& nodeLocation,
	               const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks,
	               std::unordered_map<std::string, std::shared_ptr<void>>& compileCache) override {
		Result res = {};

		llvm::IRBuilder<> builder(codegenInto);
		builder.SetCurrentDebugLocation(nodeLocation);

		auto func =
		    codegenInto->getModule()->getFunction(mangleFunctionName(module().fullName(), name()));

		if (func == nullptr) {
			res.addEntry("EINT", "Could not find function in llvm module",
			             {{"Requested Function", name()}});
			return res;
		}

		// add the execInputID to the argument list
		std::vector<llvm::Value*> passingIO;
		passingIO.push_back(builder.getInt32(execInputID));

		std::copy(io.begin(), io.end(), std::back_inserter(passingIO));

		auto ret = builder.CreateCall(func, passingIO, "call_function");

		// create switch on return
		auto switchInst = builder.CreateSwitch(ret, outputBlocks[0]);  // TODO: better default

		auto id = 0ull;
		for (auto out : outputBlocks) {
			switchInst->addCase(builder.getInt32(id), out);
			++id;
		}

		return res;
	}

	nlohmann::json            toJSON() const override { return {}; }
	std::unique_ptr<NodeType> clone() const override {
		Result res = {};  // there shouldn't be an error but check anyways
		return std::make_unique<GraphFuncCallType>(*JModule, name(), &res);
	}

	GraphModule* JModule;
};

struct MakeStructNodeType : public NodeType {
	MakeStructNodeType(GraphStruct& ty) : NodeType(ty.module()), mStruct{&ty} {
		setName("_make_" + ty.name());
		setDescription("Make a " + ty.name() + " structure");
		makePure();

		// set inputs
		setDataInputs(ty.types());

		// set output to just be the struct
		setDataOutputs({{"", ty.dataType()}});
	}

	Result codegen(size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	               const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks,
	               std::unordered_map<std::string, std::shared_ptr<void>>& compileCache) override {
		llvm::IRBuilder<> builder{codegenInto};
		builder.SetCurrentDebugLocation(nodeLocation);

		llvm::Value* out = io[io.size() - 1];  // output goes last
		for (auto id = 0; id < io.size() - 1; ++id) {
			auto ptr = builder.CreateStructGEP(mStruct->dataType().llvmType(), out, id);
			builder.CreateStore(io[id], ptr);
		}

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	nlohmann::json            toJSON() const override { return {}; }
	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<MakeStructNodeType>(*mStruct);
	}

	GraphStruct* mStruct;
};

struct BreakStructNodeType : public NodeType {
	BreakStructNodeType(GraphStruct& ty) : NodeType(ty.module()), mStruct{&ty} {
		setName("_break_" + ty.name());
		setDescription("Break a " + ty.name() + " structure");
		makePure();

		// set input to just be the struct
		setDataInputs({{"", ty.dataType()}});

		// set outputs
		setDataOutputs(ty.types());
	}

	Result codegen(size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	               const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks,
	               std::unordered_map<std::string, std::shared_ptr<void>>& compileCache) override {
		llvm::IRBuilder<> builder{codegenInto};
		builder.SetCurrentDebugLocation(nodeLocation);

		// create temp struct
		auto tempStruct = builder.CreateAlloca(mStruct->dataType().llvmType());
		builder.CreateStore(io[0], tempStruct);

		for (auto id = 1; id < io.size(); ++id) {
			auto        ptr = builder.CreateStructGEP(nullptr, tempStruct, id - 1);
			std::string s   = stringifyLLVMType(ptr->getType());

			auto val = builder.CreateLoad(ptr);
			builder.CreateStore(val, io[id]);
		}

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	nlohmann::json            toJSON() const override { return {}; }
	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<BreakStructNodeType>(*mStruct);
	}

	GraphStruct* mStruct;
};

struct SetLocalNodeType : public NodeType {
	SetLocalNodeType(ChigModule& mod, NamedDataType ty) : NodeType(mod), mDataType{std::move(ty)} {
		setName("_set_" + mDataType.name);
		setDescription("Set " + mDataType.name);

		setDataInputs({{"", mDataType.type}});

		setExecInputs({""});
		setExecOutputs({""});
	}

	Result codegen(size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	               const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks,
	               std::unordered_map<std::string, std::shared_ptr<void>>& compileCache) override {
		llvm::IRBuilder<> builder{codegenInto};
		builder.SetCurrentDebugLocation(nodeLocation);

		std::shared_ptr<void>& local = compileCache[mDataType.name];

		if (local == nullptr) {
			// create a new one!
			local = std::make_shared<llvm::Value*>(builder.CreateAlloca(mDataType.type.llvmType()));
		}

		// set the value!
		builder.CreateStore(io[0], *static_cast<llvm::Value**>(local.get()));

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	nlohmann::json            toJSON() const override { return mDataType.type.qualifiedName(); }
	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<SetLocalNodeType>(module(), mDataType);
	}

	NamedDataType mDataType;
};

struct GetLocalNodeType : public NodeType {
	GetLocalNodeType(ChigModule& mod, NamedDataType ty) : NodeType(mod), mDataType{std::move(ty)} {
		setName("_get_" + mDataType.name);
		setDescription("Get " + mDataType.name);

		setDataOutputs({{"", mDataType.type}});

		makePure();
	}

	Result codegen(size_t /*execInputID*/, const llvm::DebugLoc& nodeLocation,
	               const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks,
	               std::unordered_map<std::string, std::shared_ptr<void>>& compileCache) override {
		llvm::IRBuilder<> builder{codegenInto};
		builder.SetCurrentDebugLocation(nodeLocation);

		std::shared_ptr<void>& local = compileCache[mDataType.name];

		if (local == nullptr) {
			// create a new one!
			local = std::make_shared<llvm::Value*>(builder.CreateAlloca(mDataType.type.llvmType()));

			// TODO: create an error here
		}

		builder.CreateStore(builder.CreateLoad(*static_cast<llvm::Value**>(local.get())), io[0]);

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	nlohmann::json            toJSON() const override { return mDataType.type.qualifiedName(); }
	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<GetLocalNodeType>(module(), mDataType);
	}

	NamedDataType mDataType;
};

}  // anon namespace

GraphModule::GraphModule(Context& cont, boost::filesystem::path fullName,
                         gsl::span<boost::filesystem::path> dependencies)
    : ChigModule(cont, fullName) {
	// load the dependencies from the context
	for (const auto& dep : dependencies) { addDependency(dep); }
}

std::vector<std::string> GraphModule::typeNames() const {
	std::vector<std::string> ret;
	ret.reserve(structs().size());

	for (const auto& ty : structs()) { ret.push_back(ty->name()); }

	return ret;
}

Result GraphModule::generateModule(llvm::Module& module) {
	Result res = {};

	// debug info
	llvm::DIBuilder debugBuilder(module);

	auto compileUnit = debugBuilder.createCompileUnit(llvm::dwarf::DW_LANG_C,
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 9
	                                                  sourceFilePath().filename().string(),
	                                                  sourceFilePath().parent_path().string(),
#else
	                                                  debugBuilder.createFile(
	                                                      sourceFilePath().filename().string(),
	                                                      sourceFilePath().parent_path().string()),
#endif
	                                                  "Chigraph Compiler", false, "", 0);

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

Result GraphModule::saveToDisk() const {
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
	nlohmann::json toFill = graphModuleToJson(*this);

	// save
	fs::ofstream ostr(modulePath);
	ostr << toFill.dump(2);

	return res;
}

GraphFunction* GraphModule::getOrCreateFunction(gsl::cstring_span<>        name,
                                                std::vector<NamedDataType> dataIns,
                                                std::vector<NamedDataType> dataOuts,
                                                std::vector<std::string>   execIns,
                                                std::vector<std::string> execOuts, bool* inserted) {
	// make sure there already isn't one by this name
	auto foundFunc = functionFromName(name);
	if (foundFunc != nullptr) {
		if (inserted != nullptr) { *inserted = false; }
		return foundFunc;
	}

	mFunctions.push_back(std::make_unique<GraphFunction>(*this, name, std::move(dataIns),
	                                                     std::move(dataOuts), std::move(execIns),
	                                                     std::move(execOuts)));

	if (inserted != nullptr) { *inserted = true; }
	return mFunctions[mFunctions.size() - 1].get();
}

bool GraphModule::removeFunction(gsl::cstring_span<> name) {
	auto funcPtr = functionFromName(name);

	if (funcPtr == nullptr) { return false; }

	removeFunction(funcPtr);

	return true;
}

void GraphModule::removeFunction(GraphFunction* func) {
	Expects(func != nullptr);

	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [func](auto& uPtr) { return uPtr.get() == func; });
	if (iter == mFunctions.end()) { return; }

	mFunctions.erase(iter);
}

GraphFunction* GraphModule::functionFromName(gsl::cstring_span<> name) const {
	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [&](auto& ptr) { return ptr->name() == name; });

	if (iter != mFunctions.end()) { return iter->get(); }
	return nullptr;
}

Result GraphModule::nodeTypeFromName(gsl::cstring_span<> name, const nlohmann::json& jsonData,
                                     std::unique_ptr<NodeType>* toFill) {
	Result res = {};

	auto graph = functionFromName(name);

	if (graph == nullptr) {
		// if it wasn't found, then see if it's a struct breaker or maker
		std::string nameStr = gsl::to_string(name);
		if (nameStr.substr(0, 6) == "_make_") {
			auto str = structFromName(nameStr.substr(6));
			if (str != nullptr) {
				*toFill = std::make_unique<MakeStructNodeType>(*str);
				return res;
			}
		}
		if (nameStr.substr(0, 7) == "_break_") {
			auto str = structFromName(nameStr.substr(7));
			if (str != nullptr) {
				*toFill = std::make_unique<BreakStructNodeType>(*str);
				return res;
			}
		}
		if (nameStr.substr(0, 5) == "_get_") {
			if (jsonData.is_string()) {
				std::string module, typeName;

				std::tie(module, typeName) = parseColonPair(jsonData);

				DataType ty;
				res += context().typeFromModule(module, typeName, &ty);

				*toFill =
				    std::make_unique<GetLocalNodeType>(*this, NamedDataType{nameStr.substr(5), ty});
			} else {
				res.addEntry("EUKN", "Json data for _get_ node type isn't a string",
				             {{"Given Data", jsonData}});
			}
			return res;
		}
		if (nameStr.substr(0, 5) == "_set_") {
			if (jsonData.is_string()) {
				std::string module, typeName;

				std::tie(module, typeName) = parseColonPair(jsonData);

				DataType ty;
				res += context().typeFromModule(module, typeName, &ty);

				*toFill = std::make_unique<SetLocalNodeType>(
				    *this, NamedDataType{nameStr.substr(5), std::move(ty)});
			} else {
				res.addEntry("EUKN", "Json data for _set_ node type isn't a string",
				             {{"Given Data", jsonData}});
			}
			return res;
		}

		// if we get here than it's for sure not a thing
		res.addEntry("EUKN", "Graph not found in module",
		             {{"Module Name", fullName()}, {"Requested Graph", gsl::to_string(name)}});
	}

	*toFill = std::make_unique<GraphFuncCallType>(*this, name, &res);
	return res;
}

DataType GraphModule::typeFromName(gsl::cstring_span<> name) {
	auto func = structFromName(name);

	if (func == nullptr) { return {}; }

	return func->dataType();
}

std::vector<std::string> GraphModule::nodeTypeNames() const {
	std::vector<std::string> ret;
	std::transform(mFunctions.begin(), mFunctions.end(), std::back_inserter(ret),
	               [](auto& gPtr) { return gPtr->name(); });

	for (const auto& str : structs()) {
		ret.push_back("_make_" + str->name());
		ret.push_back("_break_" + str->name());
	}

	return ret;
}

boost::bimap<unsigned int, const NodeInstance*> GraphModule::createLineNumberAssoc() const {
	// create a sorted list of GraphFunctions
	std::vector<const NodeInstance*> nodes;
	for (const auto& f : functions()) {
		for (const auto& node : f->nodes()) {
			Expects(node.second != nullptr);
			nodes.push_back(node.second.get());
		}
	}

	std::sort(nodes.begin(), nodes.end(), [](const auto& lhs, const auto& rhs) {
		return (lhs->function().name() + ":" + boost::uuids::to_string(lhs->id())) <
		       (rhs->function().name() + ":" + boost::uuids::to_string(rhs->id()));
	});

	boost::bimap<unsigned, const NodeInstance*> ret;
	for (unsigned i = 0; i < nodes.size(); ++i) {
		ret.left.insert({i + 1, nodes[i]});  // + 1 because line numbers start at 1
	}

	return ret;
}

GraphStruct* GraphModule::structFromName(gsl::cstring_span<> name) const {
	for (const auto& str : structs()) {
		if (str->name() == name) { return str.get(); }
	}
	return nullptr;
}

GraphStruct* GraphModule::getOrCreateStruct(std::string name, bool* inserted) {
	auto str = structFromName(name);

	if (str != nullptr) {
		if (inserted != nullptr) { *inserted = false; }
		return str;
	}

	mStructs.push_back(std::make_unique<GraphStruct>(*this, std::move(name)));

	if (inserted != nullptr) { *inserted = true; }
	return mStructs[mStructs.size() - 1].get();
}

bool GraphModule::removeStruct(gsl::cstring_span<> name) {
	for (auto iter = structs().begin(); iter != structs().end(); ++iter) {
		if ((*iter)->name() == name) {
			mStructs.erase(iter);
			return true;
		}
	}
	return false;
}

void GraphModule::removeStruct(GraphStruct* tyToDel) {
	Expects(&tyToDel->module() == this);

	for (auto iter = structs().begin(); iter != structs().end(); ++iter) {
		if (iter->get() == tyToDel) {
			mStructs.erase(iter);
			return;
		}
	}
	Expects(false);
}

}  // namespace chig
