/// \file GraphModule.cpp

#include "chi/GraphModule.hpp"
#include "chi/Context.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/JsonDeserializer.hpp"
#include "chi/JsonSerializer.hpp"
#include "chi/NameMangler.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Result.hpp"

#include "ctollvm.hpp"

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include <boost/filesystem.hpp>
#include <boost/range.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace fs = boost::filesystem;

namespace chi {

namespace {

// Compile C code to a llvm::Module
std::unique_ptr<llvm::Module> compileCCode(const char* execPath, boost::string_view code,
                                           const std::vector<std::string>& args,
                                           llvm::LLVMContext& ctx, Result& res) {
	std::vector<const char*> cArgs;
	for (const auto& arg : args) { cArgs.push_back(arg.c_str()); }
	cArgs.push_back("-nostdlib");
	cArgs.push_back("-I");

	auto stdlibPath =
	    fs::path(execPath).parent_path().parent_path() / "lib" / "chigraph" / "stdlib" / "include";
	cArgs.push_back(stdlibPath.string().c_str());

	std::string errors;

	auto mod = cToLLVM(ctx, execPath, code.to_string().c_str(), "internal.c", cArgs, errors);

	if (mod == nullptr) {
		res.addEntry("EUKN", "Failed to generate IR with clang", {{"Error", errors}});
	} else if (!errors.empty()) {
		res.addEntry("WUKN", "Warnings encountered while generating IR with clang",
		             {{"Error", errors}});
	}

	return mod;
}

/// The NodeType for calling C functions
struct CFuncNode : NodeType {
	CFuncNode(GraphModule& mod, std::string cCode, std::string functionName,
	          std::vector<std::string> extraArgs, std::vector<NamedDataType> inputs,
	          DataType output)
	    : NodeType{mod, "c-call", "call C code"},
	      mFunctionName{std::move(functionName)},
	      mCCode(std::move(cCode)),
	      mExtraArguments{std::move(extraArgs)},
	      mInputs{std::move(inputs)},
	      mOutput{std::move(output)},
	      mGraphModule{&mod} {
		setExecInputs({""});
		setExecOutputs({""});

		setDataInputs(mInputs);
		if (mOutput.valid()) { setDataOutputs({{"", mOutput}}); }
	}

	Result codegen(
	    size_t /*inID*/, const llvm::DebugLoc& nodeLocation, const gsl::span<llvm::Value*> io,
	    llvm::BasicBlock* codegenInto, const gsl::span<llvm::BasicBlock*> outputBlocks,
	    std::unordered_map<std::string, std::shared_ptr<void>>& /*compileCache*/) override {
		Expects(io.size() == dataInputs().size() + dataOutputs().size() && codegenInto != nullptr &&
		        outputBlocks.size() == 1);

		Result res;

		// compile the c code if it hasn't already been compiled
		if (llcompiledmod == nullptr) {
			
			auto args = mExtraArguments;
			
			// add -I for the .c dir
			args.push_back("-I");
			args.push_back(mGraphModule->pathToCSources().string());

			
			llcompiledmod = compileCCode(llvm::sys::fs::getMainExecutable(nullptr, nullptr).c_str(),
			                             mCCode, args, context().llvmContext(), res);

			if (!res) { return res; }
		}

		// create a copy of the module
		auto copymod = llvm::CloneModule(llcompiledmod.get());

// link it in

	auto parentModule = codegenInto->
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 6
			getParent()->getParent()
#else
			getModule()
#endif
	;
		
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		llvm::Linker::LinkModules(parentModule, copymod);
#else
		llvm::Linker::linkModules(*parentModule, std::move(copymod));
#endif

		parentModule->setDataLayout("");

		auto llfunc = parentModule->getFunction(mFunctionName);
		Expects(llfunc != nullptr);

		llvm::IRBuilder<> builder(codegenInto);

		gsl::span<llvm::Value*> inputs = io;

		std::string outputName;

		// remove the return type if there is one
		if (!dataOutputs().empty()) {
			inputs     = inputs.subspan(0, inputs.size() - 1);
			outputName = dataOutputs()[0].name;
		}

		auto callinst = builder.CreateCall(
		    llfunc, {inputs.data(), static_cast<size_t>(inputs.size())}, outputName);
		callinst->setDebugLoc(nodeLocation);

		// store theoutput if there are any
		if (!dataOutputs().empty()) {
			auto stoInst = builder.CreateStore(callinst, io[dataInputs().size()]);
			stoInst->setDebugLoc(nodeLocation);
		}

		auto brInst = builder.CreateBr(outputBlocks[0]);
		brInst->setDebugLoc(nodeLocation);

		return res;
	}

	nlohmann::json toJSON() const override {
		auto j = nlohmann::json{};

		j             = nlohmann::json::object();
		j["code"]     = mCCode;
		j["function"] = mFunctionName;

		auto& jsonExtraFlags = j["extraflags"];
		jsonExtraFlags       = nlohmann::json::array();
		for (const auto& flag : mExtraArguments) { jsonExtraFlags.push_back(flag); }

		auto& jsonInputs = j["inputs"];
		jsonInputs       = nlohmann::json::array();
		for (const auto& in : mInputs) {
			jsonInputs.push_back({{in.name, in.type.qualifiedName()}});
		}

		if (mOutput.valid()) {
			j["output"] = mOutput.qualifiedName();
		} else {
			j["output"] = nlohmann::json();
		}

		return j;
	}

	std::unique_ptr<NodeType> clone() const override {
		Result res;

		return std::make_unique<CFuncNode>(*mGraphModule, mCCode, mFunctionName, mExtraArguments,
		                                   mInputs, mOutput);
	}

	std::string                mFunctionName;
	std::string                mCCode;
	std::vector<std::string>   mExtraArguments;
	std::vector<NamedDataType> mInputs;
	DataType                   mOutput;
	GraphModule* mGraphModule;

	std::unique_ptr<llvm::Module> llcompiledmod;
};

struct GraphFuncCallType : public NodeType {
	GraphFuncCallType(GraphModule& json_module, std::string funcname, Result* resPtr)
	    : NodeType(json_module, std::move(funcname), ""), JModule(&json_module) {
		Result& res = *resPtr;

		auto* mygraph = JModule->functionFromName(name());
		setDescription(mygraph->description());

		if (mygraph == nullptr) {
			res.addEntry("EINT", "Graph doesn't exist in module",
			             {{"Module Name", JModule->fullName()}, {"Requested Name", name()}});
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
		    codegenInto->
#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 6
				getParent()->getParent()
#else
				getModule()
#endif
				->getFunction(mangleFunctionName(module().fullName(), name()));

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
	SetLocalNodeType(ChiModule& mod, NamedDataType ty) : NodeType(mod), mDataType{std::move(ty)} {
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
	GetLocalNodeType(ChiModule& mod, NamedDataType ty) : NodeType(mod), mDataType{std::move(ty)} {
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
    : ChiModule(cont, fullName) {
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

	// if C support was enabled, compile the C files
	if (cEnabled()) {
		fs::path cPath = pathToCSources();
		if (fs::is_directory(cPath)) {
			// compile the files
			for (auto direntry : boost::make_iterator_range(
			         fs::recursive_directory_iterator{cPath, fs::symlink_option::recurse}, {})) {
				const fs::path& CFile = direntry;
			
				if (!fs::is_regular_file(CFile) || !(
					CFile.extension() == ".c" ||
					CFile.extension() == ".C" ||
					CFile.extension() == ".cpp" ||
					CFile.extension() == ".cxx" ||
					CFile.extension() == ".c++" ||
					CFile.extension() == ".cc"
				)) {
					continue;
				}

				// compile it
				auto generatedModule =
				    compileCCode(llvm::sys::fs::getMainExecutable(nullptr, nullptr).c_str(), "",
				                 {CFile.string()}, context().llvmContext(), res);

				if (!res) { return res; }

// link it

#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
				llvm::Linker::LinkModules(&module, generatedModule.get());
#else
				llvm::Linker::linkModules(module, std::move(generatedModule));
#endif
			}
		}
	}

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

GraphFunction* GraphModule::getOrCreateFunction(std::string                name,
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

	mFunctions.push_back(std::make_unique<GraphFunction>(*this, std::move(name), std::move(dataIns),
	                                                     std::move(dataOuts), std::move(execIns),
	                                                     std::move(execOuts)));

	if (inserted != nullptr) { *inserted = true; }
	return mFunctions[mFunctions.size() - 1].get();
}

bool GraphModule::removeFunction(boost::string_view name, bool deleteReferences) {
	auto funcPtr = functionFromName(name);

	if (funcPtr == nullptr) { return false; }

	removeFunction(*funcPtr, deleteReferences);

	return true;
}

void GraphModule::removeFunction(GraphFunction& func, bool deleteReferences) {
	if (deleteReferences) {
		auto references = context().findInstancesOfType(fullName(), func.name());

		for (auto node : references) { node->function().removeNode(*node); }
	}

	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [&func](auto& uPtr) { return uPtr.get() == &func; });
	if (iter == mFunctions.end()) { return; }

	mFunctions.erase(iter);
}

GraphFunction* GraphModule::functionFromName(boost::string_view name) const {
	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [&](auto& ptr) { return ptr->name() == name; });

	if (iter != mFunctions.end()) { return iter->get(); }
	return nullptr;
}

Result GraphModule::nodeTypeFromName(boost::string_view name, const nlohmann::json& jsonData,
                                     std::unique_ptr<NodeType>* toFill) {
	Result res = {};

	// see if it's a C call
	if (cEnabled() && name == "c-call") {
		if (!jsonData.is_object()) {
			res.addEntry("WUKN", "Data for c-call must be an object", {{"Given Data"}, jsonData});
		}

		std::string code;
		if (jsonData.is_object() && jsonData.find("code") != jsonData.end() &&
		    jsonData["code"].is_string()) {
			code = jsonData["code"];
		} else {
			res.addEntry(
			    "WUKN",
			    "Data for c-call must have a pair with the key of code and that the data is a "
			    "string",
			    {{"Given Data"}, jsonData});
		}

		std::string function;
		if (jsonData.is_object() && jsonData.find("function") != jsonData.end() &&
		    jsonData["function"].is_string()) {
			function = jsonData["function"];
		} else {
			res.addEntry(
			    "WUKN",
			    "Data for c-call must have a pair with the key of function and that the data is a "
			    "string",
			    {{"Given Data"}, jsonData});
		}

		std::vector<std::string> extraFlags;
		if (jsonData.is_object() && jsonData.find("extraflags") != jsonData.end() &&
		    jsonData["extraflags"].is_array()) {
			for (const auto& flag : jsonData["extraflags"]) {
				std::string str = flag;
				extraFlags.emplace_back(std::move(str));
			}
		} else {
			res.addEntry("WUKN", "Data for c-call must have an extraflags array",
			             {{"Given Data"}, jsonData});
		}

		std::vector<NamedDataType> inputs;
		if (jsonData.is_object() && jsonData.find("inputs") != jsonData.end() &&
		    jsonData["inputs"].is_array()) {
			inputs.reserve(jsonData["inputs"].size());
			for (const auto& jsonArg : jsonData["inputs"]) {
				std::string docstring, qualifiedName;
				std::tie(docstring, qualifiedName) = parseObjectPair(jsonArg);

				std::string moduleName, typeName;
				std::tie(moduleName, typeName) = parseColonPair(qualifiedName);

				DataType ty;
				res += context().typeFromModule(moduleName, typeName, &ty);

				inputs.emplace_back(docstring, ty);
			}

		} else {
			res.addEntry("WUKN", "Data for c-call must have an inputs array",
			             {{"Given Data"}, jsonData});
		}

		DataType output;
		if (jsonData.is_object() && jsonData.find("output") != jsonData.end()) {
			auto& outputJson = jsonData["output"];

			// if it's a string, there's a return type
			if (outputJson.is_string()) {
				std::string moduleName, typeName;
				std::tie(moduleName, typeName) = parseColonPair(outputJson);

				res += context().typeFromModule(moduleName, typeName, &output);
			}
			// now if it isn't null, then there was an error
			else if (!outputJson.is_null()) {
				res.addEntry("WUKN",
				             R"("output" element in c-call must be either null or a string)",
				             {{"Given Data"}, jsonData});
			}
		} else {
			res.addEntry(
			    "WUKN",
			    "Data for c-call must have an output element that is either null or a string",
			    {{"Given Data"}, jsonData});
		}

		*toFill = std::make_unique<CFuncNode>(*this, code, function, extraFlags, inputs, output);
		return res;

	} else if (name == "c-call") {
		res.addEntry("EUKN", "Tried to use a c-call node without c support enabled in the module",
		             {});
		return res;
	}

	auto graph = functionFromName(name);

	if (graph == nullptr) {
		// if it wasn't found, then see if it's a struct breaker or maker
		std::string nameStr{name};
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
		             {{"Module Name", fullName()}, {"Requested Graph", name.to_string()}});
		return res;
	}

	*toFill = std::make_unique<GraphFuncCallType>(*this, name.to_string(), &res);
	return res;
}

DataType GraphModule::typeFromName(boost::string_view name) {
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
	
	if (cEnabled()) {
		ret.push_back("c-call");
	}

	return ret;
}

boost::bimap<unsigned int, NodeInstance*> GraphModule::createLineNumberAssoc() const {
	// create a sorted list of GraphFunctions
	std::vector<NodeInstance*> nodes;
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

	boost::bimap<unsigned, NodeInstance*> ret;
	for (unsigned i = 0; i < nodes.size(); ++i) {
		ret.left.insert({i + 1, nodes[i]});  // + 1 because line numbers start at 1
	}

	return ret;
}

GraphStruct* GraphModule::structFromName(boost::string_view name) const {
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

bool GraphModule::removeStruct(boost::string_view name) {
	for (auto iter = structs().begin(); iter != structs().end(); ++iter) {
		if ((*iter)->name() == name) {
			mStructs.erase(iter);
			return true;
		}
	}

	// TODO: remove referencing nodes
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

boost::filesystem::path GraphModule::sourceFilePath() const {
	return context().workspacePath() / "src" / (fullName() + ".chimod");
}

Result GraphModule::createNodeTypeFromCCode(boost::string_view              code,
                                            boost::string_view              functionName,
                                            std::vector<std::string> clangArgs,
                                            std::unique_ptr<NodeType>*      toFill) {
	Expects(toFill != nullptr);

	Result res;
	
	// add -I for the .c dir
	clangArgs.push_back("-I");
	clangArgs.push_back(pathToCSources().string());

	auto mod = compileCCode(llvm::sys::fs::getMainExecutable(nullptr, nullptr).c_str(), code,
	                        clangArgs, context().llvmContext(), res);

	if (!res) { return res; }

	auto llFunc = mod->getFunction(functionName.to_string());

	if (llFunc == nullptr) {
		res.addEntry("EUKN", "Failed to find function in C code",
		             {{"Function Name", functionName.to_string()}, {"C Code", code.to_string()}});
		return res;
	}

	std::vector<NamedDataType> dInputs;
	for (const auto& argument : llFunc->args()) {
		DataType ty;
		context().typeFromModule("lang", stringifyLLVMType(argument.getType()), &ty);
		dInputs.emplace_back(argument.getName(), ty);
	}

	DataType output;
	auto     ret = llFunc->getReturnType();

	if (!ret->isVoidTy()) { context().typeFromModule("lang", stringifyLLVMType(ret), &output); }

	*toFill = std::make_unique<CFuncNode>(*this, code.to_string(), functionName.to_string(),
	                                      clangArgs, dInputs, output);

	return res;
}

}  // namespace chi
