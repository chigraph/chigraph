/// \file GraphModule.cpp

#include "chi/GraphModule.hpp"
#include "chi/CCompiler.hpp"
#include "chi/ClangFinder.hpp"
#include "chi/Context.hpp"
#include "chi/FunctionCompiler.hpp"
#include "chi/GraphFunction.hpp"
#include "chi/GraphStruct.hpp"
#include "chi/JsonDeserializer.hpp"
#include "chi/JsonSerializer.hpp"
#include "chi/NameMangler.hpp"
#include "chi/NodeInstance.hpp"
#include "chi/NodeType.hpp"
#include "chi/Support/LibCLocator.hpp"
#include "chi/Support/Result.hpp"
#include "chi/Support/Subprocess.hpp"

#include <boost/range.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <filesystem>
#include <fstream>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/Linker.h>

namespace fs = std::filesystem;

namespace chi {

namespace {
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

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t /*execInputID*/,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		assert(io.size() == dataInputs().size() + dataOutputs().size() && outputBlocks.size() == 1);

		Result res;

		// compile the c code if it hasn't already been compiled
		if (llcompiledmod == nullptr) {
			auto args = mExtraArguments;

			// add -I for the .c dir
			args.push_back("-I");
			args.push_back(mGraphModule->pathToCSources().string());

			// find clang
			auto clangExe = findClang();
			if (clangExe.empty()) {
				res.addEntry("EUKN", "Failed to find clang in path", nlohmann::json::object());
				return res;
			}

			res += compileCToLLVM(clangExe, context().llvmContext(), args, mCCode, &llcompiledmod);

			if (!res) { return res; }
		}

		auto copymod = OwnedLLVMModule(LLVMCloneModule(*llcompiledmod));

		// link it in
		auto parentModule = compiler.llvmModule();

		if (LLVMLinkModules2(parentModule, copymod.take_ownership())) {
			res.addEntry("EUKN", "Failed to link modules", {});
			return res;
		}
		LLVMSetDataLayout(parentModule, "");

		auto llfunc = LLVMGetNamedFunction(parentModule, mFunctionName.c_str());
		assert(llfunc != nullptr);

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);

		size_t ioSize = io.size();

		std::string outputName;

		// remove the return type if there is one
		if (!dataOutputs().empty()) {
			--ioSize;
			outputName = dataOutputs()[0].name;
		}

		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));
		auto callInst =
		    LLVMBuildCall(*builder, llfunc, const_cast<LLVMValueRef*>(io.data()), io.size(), "");

		// store theoutput if there are any
		if (!dataOutputs().empty()) { LLVMBuildStore(*builder, callInst, io[dataInputs().size()]); }

		LLVMBuildBr(*builder, outputBlocks[0]);

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
	GraphModule*               mGraphModule;

	OwnedLLVMModule llcompiledmod;
};  // namespace

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

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		Result res = {};

		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto func = LLVMGetNamedFunction(compiler.funcCompiler().llvmModule(),
		                                 mangleFunctionName(module().fullName(), name()).c_str());

		if (func == nullptr) {
			res.addEntry("EINT", "Could not find function in llvm module",
			             {{"Requested Function", name()}});
			return res;
		}

		// add the execInputID to the argument list
		std::vector<LLVMValueRef> passingIO;
		passingIO.push_back(context().constI32(execInputID));

		std::copy(io.begin(), io.end(), std::back_inserter(passingIO));

		auto ret =
		    LLVMBuildCall(*builder, func, passingIO.data(), passingIO.size(), "call_function");

		// create switch on return
		auto switchInst = LLVMBuildSwitch(*builder, ret, outputBlocks[0],
		                                  outputBlocks.size());  // TODO: better default

		auto id = 0ull;
		for (auto out : outputBlocks) {
			LLVMAddCase(switchInst, context().constI32(id), out);
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

	Result codegen(NodeCompiler& /*compiler*/, LLVMBasicBlockRef codegenInto,
	               size_t /*execInputID*/, LLVMMetadataRef       nodeLocation,
	               const std::vector<LLVMValueRef>&      io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		LLVMValueRef out = io[io.size() - 1];  // output goes last
		for (auto id = 0ull; id < io.size() - 1; ++id) {
			auto ptr = LLVMBuildStructGEP2(*builder, mStruct->dataType().llvmType(), out, id, "");
			LLVMBuildStore(*builder, io[id], ptr);
		}

		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& /*compiler*/, LLVMBasicBlockRef codegenInto,
	               size_t /*execInputID*/, LLVMMetadataRef       nodeLocation,
	               const std::vector<LLVMValueRef>&      io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		// create temp struct
		auto tempStruct = LLVMBuildAlloca(*builder, mStruct->dataType().llvmType(), "temp_struct");
		LLVMBuildStore(*builder, io[0], tempStruct);

		for (auto id = 1ull; id < io.size(); ++id) {
			auto ptr = LLVMBuildStructGEP(*builder, tempStruct, id - 1, "");

			auto val = LLVMBuildLoad(*builder, ptr, "");
			LLVMBuildStore(*builder, val, io[id]);
		}

		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t /*execInputID*/,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto value = compiler.funcCompiler().localVariable(mDataType.name);
		assert(value != nullptr);

		// set the value!
		LLVMBuildStore(*builder, io[0], value);

		LLVMBuildBr(*builder, outputBlocks[0]);

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

	Result codegen(NodeCompiler& compiler, LLVMBasicBlockRef codegenInto, size_t execInputID,
	               LLVMMetadataRef nodeLocation, const std::vector<LLVMValueRef>& io,
	               const std::vector<LLVMBasicBlockRef>& outputBlocks) override {
		auto builder = OwnedLLVMBuilder(LLVMCreateBuilder());
		LLVMPositionBuilder(*builder, codegenInto, nullptr);
		LLVMSetCurrentDebugLocation(*builder,
		                            LLVMMetadataAsValue(context().llvmContext(), nodeLocation));

		auto value = compiler.funcCompiler().localVariable(mDataType.name);
		assert(value != nullptr);

		LLVMBuildStore(*builder, LLVMBuildLoad(*builder, value, ""), io[0]);

		LLVMBuildBr(*builder, outputBlocks[0]);

		return {};
	}

	nlohmann::json            toJSON() const override { return mDataType.type.qualifiedName(); }
	std::unique_ptr<NodeType> clone() const override {
		return std::make_unique<GetLocalNodeType>(module(), mDataType);
	}

	NamedDataType mDataType;
};

}  // namespace

GraphModule::GraphModule(Context& cont, std::filesystem::path fullName,
                         const std::vector<std::filesystem::path>& dependencies)
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

LLVMMetadataRef GraphModule::debugType(FunctionCompiler& compiler, const DataType& dType) const {
	auto struc = structFromName(dType.unqualifiedName());
	if (!struc) { return nullptr; }

	return struc->debugType(compiler);
}

Result GraphModule::addForwardDeclarations(LLVMModuleRef module) const {
	// create prototypes
	for (auto& graph : mFunctions) {
		LLVMAddFunction(module, mangleFunctionName(fullName(), graph->name()).c_str(),
		                graph->functionType());
	}

	return {};
}

Result GraphModule::generateModule(LLVMModuleRef module) {
	Result res = {};

	// if C support was enabled, compile the C files
	if (cEnabled()) {
		fs::path cPath = pathToCSources();
		if (fs::is_directory(cPath)) {
			// compile the files
			for (auto direntry : boost::make_iterator_range(
			         fs::recursive_directory_iterator{
			             cPath, fs::directory_options::follow_directory_symlink},
			         {})) {
				const fs::path& CFile = direntry;

				if (!fs::is_regular_file(CFile) ||
				    !(CFile.extension() == ".c" || CFile.extension() == ".C" ||
				      CFile.extension() == ".cpp" || CFile.extension() == ".cxx" ||
				      CFile.extension() == ".c++" || CFile.extension() == ".cc")) {
					continue;
				}

				// find clang
				auto clangExe = findClang();
				if (clangExe.empty()) {
					res.addEntry("EUKN", "Failed to find clang in path", nlohmann::json::object());
					return res;
				}

				// compile it
				OwnedLLVMModule generatedModule;
				res += compileCToLLVM(clangExe, context().llvmContext(), {CFile.string()}, "",
				                      &generatedModule);

				if (!res) { return res; }

				// link it
				if (LLVMLinkModules2(module, generatedModule.take_ownership())) {
					res.addEntry("EUKN", "Failed to link modules", {});
					return res;
				}
			}
		}
	}

	// debug info
	auto debugBuilder = OwnedLLVMDIBuilder(LLVMCreateDIBuilder(module));

	auto srcFile     = sourceFilePath().filename().c_str();
	auto srcPath     = sourceFilePath().parent_path().c_str();
	auto compilerID  = "Chigraph Compiler";
	auto compileUnit = LLVMDIBuilderCreateCompileUnit(
	    *debugBuilder, LLVMDWARFSourceLanguageC,
	    LLVMDIBuilderCreateFile(*debugBuilder, srcFile, strlen(srcFile), srcPath, strlen(srcPath)),
	    compilerID, strlen(compilerID), false, "", 0, 0, "", 0, LLVMDWARFEmissionFull, 0, true,
	    false);  // TODO: resarch these parameters, these are defaults

	// create prototypes
	addForwardDeclarations(module);

	for (auto& graph : mFunctions) {
		res += compileFunction(*graph, module, compileUnit, *debugBuilder);
	}

	LLVMDIBuilderFinalize(*debugBuilder);

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
	std::ofstream ostr(modulePath);
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

	// invalidate the cache
	updateLastEditTime();

	mFunctions.push_back(std::make_unique<GraphFunction>(*this, std::move(name), std::move(dataIns),
	                                                     std::move(dataOuts), std::move(execIns),
	                                                     std::move(execOuts)));

	if (inserted != nullptr) { *inserted = true; }
	return mFunctions[mFunctions.size() - 1].get();
}

bool GraphModule::removeFunction(std::string_view name, bool deleteReferences) {
	// invalidate the cache
	updateLastEditTime();

	auto funcPtr = functionFromName(name);

	if (funcPtr == nullptr) { return false; }

	removeFunction(*funcPtr, deleteReferences);

	return true;
}

void GraphModule::removeFunction(GraphFunction& func, bool deleteReferences) {
	// invalidate the cache
	updateLastEditTime();

	if (deleteReferences) {
		auto references = context().findInstancesOfType(fullName(), func.name());

		for (auto node : references) { node->function().removeNode(*node); }
	}

	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [&func](auto& uPtr) { return uPtr.get() == &func; });
	if (iter == mFunctions.end()) { return; }

	mFunctions.erase(iter);
}

GraphFunction* GraphModule::functionFromName(std::string_view name) const {
	auto iter = std::find_if(mFunctions.begin(), mFunctions.end(),
	                         [&](auto& ptr) { return ptr->name() == name; });

	if (iter != mFunctions.end()) { return iter->get(); }
	return nullptr;
}

Result GraphModule::nodeTypeFromName(std::string_view name, const nlohmann::json& jsonData,
                                     std::unique_ptr<NodeType>* toFill) {
	Result res = {};

	// see if it's a C call
	if (cEnabled() && name == "c-call") {
		if (!jsonData.is_object()) {
			res.addEntry("WUKN", "Data for c-call must be an object", {{"Given Data", jsonData}});
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
			    {{"Given Data", jsonData}});
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
			    {{"Given Data", jsonData}});
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
			             {{"Given Data", jsonData}});
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
			             {{"Given Data", jsonData}});
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
				             {{"Given Data", jsonData}});
			}
		} else {
			res.addEntry(
			    "WUKN",
			    "Data for c-call must have an output element that is either null or a string",
			    {{"Given Data", jsonData}});
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
		if (name.substr(0, 6) == "_make_") {
			auto str = structFromName(name.substr(6));
			if (str != nullptr) {
				*toFill = std::make_unique<MakeStructNodeType>(*str);
				return res;
			}
		}
		if (name.substr(0, 7) == "_break_") {
			auto str = structFromName(name.substr(7));
			if (str != nullptr) {
				*toFill = std::make_unique<BreakStructNodeType>(*str);
				return res;
			}
		}
		if (name.substr(0, 5) == "_get_") {
			if (jsonData.is_string()) {
				std::string module, typeName;

				std::tie(module, typeName) = parseColonPair(jsonData);

				DataType ty;
				res += context().typeFromModule(module, typeName, &ty);

				*toFill = std::make_unique<GetLocalNodeType>(
				    *this, NamedDataType{std::string(name.substr(5)), ty});
			} else {
				res.addEntry("EUKN", "Json data for _get_ node type isn't a string",
				             {{"Given Data", jsonData}});
			}
			return res;
		}
		if (name.substr(0, 5) == "_set_") {
			if (jsonData.is_string()) {
				std::string module, typeName;

				std::tie(module, typeName) = parseColonPair(jsonData);

				DataType ty;
				res += context().typeFromModule(module, typeName, &ty);

				*toFill = std::make_unique<SetLocalNodeType>(
				    *this, NamedDataType{std::string(name.substr(5)), std::move(ty)});
			} else {
				res.addEntry("EUKN", "Json data for _set_ node type isn't a string",
				             {{"Given Data", jsonData}});
			}
			return res;
		}

		// if we get here than it's for sure not a thing
		res.addEntry("EUKN", "Graph not found in module",
		             {{"Module Name", fullName()}, {"Requested Graph", name}});
		return res;
	}

	*toFill = std::make_unique<GraphFuncCallType>(*this, std::string(name), &res);
	return res;
}

DataType GraphModule::typeFromName(std::string_view name) {
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

	if (cEnabled()) { ret.push_back("c-call"); }

	return ret;
}

boost::bimap<unsigned int, NodeInstance*> GraphModule::createLineNumberAssoc() const {
	// create a sorted list of GraphFunctions
	std::vector<NodeInstance*> nodes;
	for (const auto& f : functions()) {
		for (const auto& node : f->nodes()) {
			assert(node.second != nullptr);
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

GraphStruct* GraphModule::structFromName(std::string_view name) const {
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
	// invalidate the cache
	updateLastEditTime();

	mStructs.push_back(std::make_unique<GraphStruct>(*this, std::move(name)));

	if (inserted != nullptr) { *inserted = true; }
	return mStructs[mStructs.size() - 1].get();
}

bool GraphModule::removeStruct(std::string_view name) {
	// invalidate the cache
	updateLastEditTime();

	for (auto iter = structs().begin(); iter != structs().end(); ++iter) {
		if ((*iter)->name() == name) {
			mStructs.erase(iter);
			return true;
		}
	}

	// TODO: remove referencing nodes
	return false;
}

void GraphModule::removeStruct(GraphStruct& tyToDel) {
	assert(&tyToDel.module() == this);

	BOOST_ATTRIBUTE_UNUSED bool succeeded = removeStruct(tyToDel.name());
	assert(succeeded);
}

std::filesystem::path GraphModule::sourceFilePath() const {
	return context().workspacePath() / "src" / (fullName() + ".chimod");
}

Result GraphModule::createNodeTypeFromCCode(std::string_view code, std::string_view functionName,
                                            std::vector<std::string>   clangArgs,
                                            std::unique_ptr<NodeType>* toFill) {
	assert(toFill != nullptr);

	Result res;

	// add -I for the .c dir
	clangArgs.push_back("-I");
	clangArgs.push_back(pathToCSources().string());

	OwnedLLVMModule mod;
	// find clang
	auto clangExe = findClang();
	if (clangExe.empty()) {
		res.addEntry("EUKN", "Failed to find clang in path", nlohmann::json::object());
		return res;
	}

	res += compileCToLLVM(clangExe, context().llvmContext(), clangArgs, code, &mod);

	if (!res) { return res; }

	auto llFunc = LLVMGetNamedFunction(*mod, std::string(functionName).c_str());

	if (llFunc == nullptr) {
		res.addEntry("EUKN", "Failed to find function in C code",
		             {{"Function Name", functionName}, {"C Code", code}});
		return res;
	}

	std::vector<NamedDataType> dInputs;
	for (auto argument = LLVMGetFirstParam(llFunc); argument != nullptr;
	     argument      = LLVMGetNextParam(argument)) {
		DataType ty;
		context().typeFromModule("lang", stringifyLLVMType(LLVMTypeOf(argument)), &ty);

		size_t      len;
		const char* name = LLVMGetValueName2(argument, &len);
		dInputs.emplace_back(std::string(name, len), ty);
	}

	DataType output;
	auto     ret = LLVMGetReturnType(LLVMTypeOf(llFunc));

	if (LLVMGetTypeKind(ret) != LLVMVoidTypeKind) {
		context().typeFromModule("lang", stringifyLLVMType(ret), &output);
	}

	*toFill = std::make_unique<CFuncNode>(*this, std::string(code), std::string(functionName),
	                                      clangArgs, dInputs, output);

	return res;
}

}  // namespace chi
