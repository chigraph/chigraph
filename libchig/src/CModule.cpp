#include "chig/CModule.hpp"
#include "chig/Config.hpp"

#include <process.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Module.h>

#include <clang/Driver/Driver.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/CodeGen/CodeGenAction.h>


std::unique_ptr<llvm::Module> CToLLVM(gsl::cstring_span<> code, gsl::cstring_span<> fakeName,  llvm::LLVMContext& context) {
    
    using namespace std;
    using namespace clang;
    using namespace llvm;

    // Prepare compilation arguments
    vector<const char *> args;
    args.push_back(fakeName.data());

    // Prepare DiagnosticEngine 
    DiagnosticOptions DiagOpts;
    TextDiagnosticPrinter *textDiagPrinter =
            new clang::TextDiagnosticPrinter(errs(),
                                         &DiagOpts);
    IntrusiveRefCntPtr<clang::DiagnosticIDs> pDiagIDs;
    DiagnosticsEngine *pDiagnosticsEngine =
            new DiagnosticsEngine(pDiagIDs,
                                         &DiagOpts,
                                         textDiagPrinter);

    // Initialize CompilerInvocation
    CompilerInvocation *CI = new CompilerInvocation();
    CompilerInvocation::CreateFromArgs(*CI, &args[0], &args[0] +     args.size(), *pDiagnosticsEngine);

    // Map code filename to a memoryBuffer
    StringRef testCodeData(code.data(), code.length());
    unique_ptr<MemoryBuffer> buffer = MemoryBuffer::getMemBufferCopy(testCodeData);
    CI->getPreprocessorOpts().addRemappedFile(fakeName.data(), buffer.get());


    // Create and initialize CompilerInstance
    CompilerInstance Clang;
    Clang.setInvocation(CI);
    Clang.createDiagnostics();

    // Create and execute action
    CodeGenAction *compilerAction = new EmitLLVMOnlyAction(&context);
    Clang.ExecuteAction(*compilerAction);

    return compilerAction->takeModule();
}

using namespace chig;

/// The NodeType for calling C functions
struct CFuncNode : NodeType {
	CFuncNode(
		ChigModule& mod, gsl::cstring_span<> cCode, gsl::cstring_span<> functionName, Result& res)
		: NodeType{mod, "func", "call C code"},
		  functocall{gsl::to_string(functionName)},
		  ccode(gsl::to_string(cCode))
	{
		setExecInputs({""});
		setExecOutputs({""});

		llcompiledmod = CToLLVM(cCode, functionName, context().llvmContext());

		auto llfunc = llcompiledmod->getFunction(functocall);

		if (llfunc == nullptr) {
			res.addEntry("EUKN", "Failed to get function in clang-compiled module",
				{{"Requested Function", functocall}});
			return;
		}

		// get arguments
		std::vector<std::pair<DataType, std::string>> dInputs;
		for (const auto& argument : llfunc->args()) {
			dInputs.emplace_back(DataType(context().moduleByFullName("lang"),
									 stringifyLLVMType(argument.getType()), argument.getType()),
				argument.getName());
		}
		setDataInputs(std::move(dInputs));

		// get return type
		auto ret = llfunc->getReturnType();

		if (!ret->isVoidTy()) {
			setDataOutputs(
				{{{context().moduleByFullName("lang"), stringifyLLVMType(ret), ret}, ""}});
		}
	}

	Result codegen(size_t /*inID*/, llvm::Module* mod, llvm::Function* /*f*/,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const override
	{
		Expects(io.size() == dataInputs().size() + dataOutputs().size() && mod != nullptr &&
				codegenInto != nullptr && outputBlocks.size() == 1);

		// create a copy of the module
		auto copymod = llvm::CloneModule(llcompiledmod.get());

		// link it in
		llvm::Linker::linkModules(*mod, std::move(copymod));
		mod->setDataLayout("");

		auto llfunc = mod->getFunction(functocall);
		Expects(llfunc != nullptr);

		llvm::IRBuilder<> builder(codegenInto);

		gsl::span<llvm::Value*> inputs = io;

		std::string outputName;

		// remove the return type if there is one
		if (!dataOutputs().empty()) {
			inputs = inputs.subspan(0, inputs.size() - 1);
			outputName = dataOutputs()[0].second;
		}

		auto callinst =
			builder.CreateCall(llfunc, {inputs.data(), (size_t)inputs.size()}, outputName);

		// store theoutput if there are any
		if (!dataOutputs().empty()) {
			builder.CreateStore(callinst, io[dataInputs().size()]);
		}

		builder.CreateBr(outputBlocks[0]);

		return {};
	}

	nlohmann::json toJSON() const override
	{
		auto j = nlohmann::json{};

		j = nlohmann::json::object();
		j["code"] = ccode;
		j["function"] = functocall;

		return j;
	}

	std::unique_ptr<NodeType> clone() const override
	{
		Result res;

		return std::make_unique<CFuncNode>(module(), ccode, functocall, res);
	}

	std::string functocall;
	std::string ccode;

	std::unique_ptr<llvm::Module> llcompiledmod;
};

CModule::CModule(Context& ctx) : ChigModule(ctx, "c") {}
DataType CModule::typeFromName(gsl::cstring_span<> /*typeName*/)
{
	// TODO: implement

	return {};
}

Result CModule::nodeTypeFromName(gsl::cstring_span<> typeName, const nlohmann::json& json_data,
	std::unique_ptr<NodeType>* toFill)
{
	Result res;

	if (typeName == "func") {
		if (!json_data.is_object()) {
			res.addEntry("WUKN", "Data for c:func must be an object", {{"Given Data"}, json_data});
		}

		std::string code;
		if (json_data.is_object() && json_data.find("code") != json_data.end() &&
			json_data["code"].is_string()) {
			code = json_data["code"];
		} else {
			res.addEntry("WUKN",
				"Data for c:func must have a pair with the key of code and that the data is a "
				"string",
				{{"Given Data"}, json_data});
		}

		std::string function;
		if (json_data.is_object() && json_data.find("function") != json_data.end() &&
			json_data["function"].is_string()) {
			function = json_data["function"];
		} else {
			res.addEntry("WUKN",
				"Data for c:func must have a pair with the key of function and that the data is a "
				"string",
				{{"Given Data"}, json_data});
		}

		*toFill = std::make_unique<CFuncNode>(*this, code, function, res);
		return res;
	}

	res.addEntry(
		"E37", "Unrecognized node type in module", {{"Module", "c"}, {"Requested Type", name()}});
	return res;
}
