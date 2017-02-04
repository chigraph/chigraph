/// \file CModule.cpp

#include "chig/CModule.hpp"
#include "chig/Config.hpp"

#include <process.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/Cloning.h>

using namespace chig;

/// The NodeType for calling C functions
struct CFuncNode : NodeType {
	CFuncNode(ChigModule& mod, gsl::cstring_span<> cCode, gsl::cstring_span<> functionName,
	          Result& res)
	    : NodeType{mod, "func", "call C code"},
	      functocall{gsl::to_string(functionName)},
	      ccode(gsl::to_string(cCode)) {
		setExecInputs({""});
		setExecOutputs({""});

		std::string bitcode;
		std::string error;
		try {
			// compile the C code
			Process clangexe(std::string(CHIG_CLANG_EXE) + " -xc - -c -g -emit-llvm -O0 -o -", {},
			                 [&bitcode](const char* bytes, size_t n) {
				                 // read stdin
				                 bitcode.append(bytes, n);
				             },
			                 [&error](const char* bytes, size_t n) { error.append(bytes, n); },
			                 true);
			clangexe.write(ccode);
			clangexe.close_stdin();

		} catch (std::exception& e) {
			res.addEntry("EUKN", "Failed to run clang and generate bitcode", {{"Error", e.what()}});
			return;
		}

		if (!error.empty()) {
			res.addEntry("EUKN", "Error encountered while generating IR", {{"Error Text", error}});
			return;
		}

		if (bitcode.empty()) {
			res.addEntry("EUKN", "Unknown error encountered while generating IR", {});
			return;
		}

		llvm::SMDiagnostic diag;
		auto modorerror = llvm::parseBitcodeFile(llvm::MemoryBufferRef(bitcode, "clang-generated"),
		                                         context().llvmContext());

		if (!modorerror) {
			std::string              errorString;
			llvm::raw_string_ostream errorStream(errorString);
			diag.print("chig compile", errorStream);

			res.addEntry(
			    "EUKN", "Error parsing clang-generated bitcode module",
			    {{"Error Code", modorerror.getError().value()}, {"Error Text", errorString}});
			return;
		}
		llcompiledmod = std::move(*modorerror);

		auto llfunc = llcompiledmod->getFunction(functocall);

		if (llfunc == nullptr) {
			res.addEntry("EUKN", "Failed to get function in clang-compiled module",
			             {{"Requested Function", functocall}});
			return;
		}

		// get arguments
		std::vector<NamedDataType> dInputs;
		for (const auto& argument : llfunc->args()) {
			DataType ty;
			context().typeFromModule("lang", stringifyLLVMType(argument.getType()), &ty);
			dInputs.emplace_back(
			    argument.getName(),
				ty);
		}
		setDataInputs(std::move(dInputs));

		// get return type
		auto ret = llfunc->getReturnType();

		if (!ret->isVoidTy()) {
			DataType ty;
			context().typeFromModule("lang", stringifyLLVMType(ret), &ty);
			setDataOutputs(
			    {{"", ty}});
		}
	}

	Result codegen(size_t /*inID*/, const llvm::DebugLoc& nodeLocation,
	               const gsl::span<llvm::Value*> io,
	               llvm::BasicBlock*                  codegenInto,
	               const gsl::span<llvm::BasicBlock*> outputBlocks) const override {
		Expects(io.size() == dataInputs().size() + dataOutputs().size() &&
		        codegenInto != nullptr && outputBlocks.size() == 1);

		// create a copy of the module
		auto copymod = llvm::CloneModule(llcompiledmod.get());

// link it in

#if LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR <= 7
		llvm::Linker::LinkModules(codegenInto->getModule(), copymod);
#else
		llvm::Linker::linkModules(*codegenInto->getModule(), std::move(copymod));
#endif

		codegenInto->getModule()->setDataLayout("");

		auto llfunc = codegenInto->getModule()->getFunction(functocall);
		Expects(llfunc != nullptr);

		llvm::IRBuilder<> builder(codegenInto);

		gsl::span<llvm::Value*> inputs = io;

		std::string outputName;

		// remove the return type if there is one
		if (!dataOutputs().empty()) {
			inputs     = inputs.subspan(0, inputs.size() - 1);
			outputName = dataOutputs()[0].name;
		}

		auto callinst =
		    builder.CreateCall(llfunc, {inputs.data(), (size_t)inputs.size()}, outputName);
		callinst->setDebugLoc(nodeLocation);

		// store theoutput if there are any
		if (!dataOutputs().empty()) {
			auto stoInst = builder.CreateStore(callinst, io[dataInputs().size()]);
			stoInst->setDebugLoc(nodeLocation);
		}

		auto brInst = builder.CreateBr(outputBlocks[0]);
		brInst->setDebugLoc(nodeLocation);

		return {};
	}

	nlohmann::json toJSON() const override {
		auto j = nlohmann::json{};

		j             = nlohmann::json::object();
		j["code"]     = ccode;
		j["function"] = functocall;

		return j;
	}

	std::unique_ptr<NodeType> clone() const override {
		Result res;

		return std::make_unique<CFuncNode>(module(), ccode, functocall, res);
	}

	std::string functocall;
	std::string ccode;

	std::unique_ptr<llvm::Module> llcompiledmod;
};

CModule::CModule(Context& ctx) : ChigModule(ctx, "c") {}

DataType                  CModule::typeFromName(gsl::cstring_span<> /*typeName*/) {
	// TODO: implement

	return {};
}

Result CModule::nodeTypeFromName(gsl::cstring_span<> typeName, const nlohmann::json& jsonData,
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

		*toFill = std::make_unique<CFuncNode>(*this, code, function, res);
		return res;
	}

	res.addEntry("E37", "Unrecognized node type in module",
	             {{"Module", "c"}, {"Requested Type", name()}});
	return res;
}
