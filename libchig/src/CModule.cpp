/// \file CModule.cpp

#include "chig/CModule.hpp"
#include "chig/GraphFunction.hpp"
#include "chig/JsonDeserializer.hpp"

#include <libexecstream/exec-stream.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "../ctollvm/ctollvm.hpp"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace chig {

namespace {

std::unique_ptr<llvm::Module> compileCCode(const char* execPath, const std::string& code,
                                           const std::vector<std::string>& args,
                                           llvm::LLVMContext& ctx, Result& res) {
	std::vector<const char*> cArgs;
	for (const auto& arg : args) { 
		cArgs.push_back(arg.c_str()); 
		
	}
	cArgs.push_back("-nostdlib");
	cArgs.push_back("-I");
	
	auto stdlibPath = fs::path(execPath).parent_path().parent_path() / "lib" / "chig" / "stdlib" / "include";
	cArgs.push_back(stdlibPath.string().c_str());

	std::string errors;

	auto mod = cToLLVM(ctx, execPath, code.c_str(), "internal.c", cArgs, errors);

	if (mod == nullptr) {
		res.addEntry("EUKN", "Failed to generate IR with clang", {{"Error", errors}});
	} else if (!errors.empty()) {
		res.addEntry("WUKN", "Warnings encountered while generating IR with clang",
		             {{"Error", errors}});
	}

	return mod;
}

}  // anonymous namespace

/// The NodeType for calling C functions
struct CFuncNode : NodeType {
	CFuncNode(CModule& mod, std::string cCode, std::string functionName,
	          std::vector<std::string> extraArgs, std::vector<NamedDataType> inputs,
	          DataType output)
	    : NodeType{mod, "func", "call C code"},
	      mFunctionName{std::move(functionName)},
	      mCCode(std::move(cCode)),
	      mExtraArguments{std::move(extraArgs)},
	      mInputs{std::move(inputs)},
	      mOutput{std::move(output)},
	      mCModule{mod} {
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
			std::vector<std::string> args = mCModule.extraArguments();
			std::copy(mExtraArguments.begin(), mExtraArguments.end(), std::back_inserter(args));
			llcompiledmod = compileCCode(llvm::sys::fs::getMainExecutable(nullptr, nullptr).c_str(),
			                             mCCode, args, context().llvmContext(), res);

			if (!res) { return res; }
		}

		// create a copy of the module
		auto copymod = llvm::CloneModule(llcompiledmod.get());

// link it in

#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		llvm::Linker::LinkModules(codegenInto->getModule(), copymod);
#else
		llvm::Linker::linkModules(*codegenInto->getModule(), std::move(copymod));
#endif

		codegenInto->getModule()->setDataLayout("");

		auto llfunc = codegenInto->getModule()->getFunction(mFunctionName);
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

		return std::make_unique<CFuncNode>(mCModule, mCCode, mFunctionName, mExtraArguments,
		                                   mInputs, mOutput);
	}

	std::string                mFunctionName;
	std::string                mCCode;
	std::vector<std::string>   mExtraArguments;
	std::vector<NamedDataType> mInputs;
	DataType                   mOutput;
	CModule&                   mCModule;

	std::unique_ptr<llvm::Module> llcompiledmod;
};

CModule::CModule(Context& ctx) : ChigModule(ctx, "c") {}

DataType CModule::typeFromName(boost::string_view /*typeName*/) {
	// TODO(#8) : implement

	return {};
}

Result CModule::nodeTypeFromName(boost::string_view typeName, const nlohmann::json& jsonData,
                                 std::unique_ptr<NodeType>* toFill) {
	Result res;

	if (typeName == "func") {
		if (!jsonData.is_object()) {
			res.addEntry("WUKN", "Data for c:func must be an object", {{"Given Data"}, jsonData});
		}

		std::string code;
		if (jsonData.is_object() && jsonData.find("code") != jsonData.end() &&
		    jsonData["code"].is_string()) {
			code = jsonData["code"];
		} else {
			res.addEntry(
			    "WUKN",
			    "Data for c:func must have a pair with the key of code and that the data is a "
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
			    "Data for c:func must have a pair with the key of function and that the data is a "
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
			res.addEntry("WUKN", "Data for c:func must have an extraflags array",
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
			res.addEntry("WUKN", "Data for c:func must have an inputs array",
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
				             R"("output" element in c:func must be either null or a string)",
				             {{"Given Data"}, jsonData});
			}
		} else {
			res.addEntry(
			    "WUKN",
			    "Data for c:func must have an output element that is either null or a string",
			    {{"Given Data"}, jsonData});
		}

		*toFill = std::make_unique<CFuncNode>(*this, code, function, extraFlags, inputs, output);
		return res;
	}

	res.addEntry("E37", "Unrecognized node type in module",
	             {{"Module", "c"}, {"Requested Type", fullName()}});
	return res;
}

Result chig::CModule::createNodeTypeFromCCode(const std::string&              code,
                                              const std::string&              functionName,
                                              const std::vector<std::string>& clangArgs,
                                              std::unique_ptr<NodeType>*      toFill) {
	Expects(toFill != nullptr);

	Result res;

	auto mod = compileCCode(llvm::sys::fs::getMainExecutable(nullptr, nullptr).c_str(), code,
	                        clangArgs, context().llvmContext(), res);

	if (!res) { return res; }

	auto llFunc = mod->getFunction(functionName);

	if (llFunc == nullptr) {
		res.addEntry("EUKN", "Failed to find function in C code",
		             {{"Function Name", functionName}, {"C Code", code}});
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

	*toFill = std::make_unique<CFuncNode>(*this, code, functionName, clangArgs, dInputs, output);

	return res;
}
}  // namespace chig
