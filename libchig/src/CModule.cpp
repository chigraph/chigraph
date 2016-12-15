#include "chig/CModule.hpp"
#include "chig/Config.hpp"

#include <iterator>

#include <exec-stream.h>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Transforms/Utils/Cloning.h>

using namespace chig;

struct CFuncNode : NodeType {
	CFuncNode(
		ChigModule& mod, gsl::cstring_span<> cCode, gsl::cstring_span<> functionName, Result& res)
		: NodeType{mod}, functocall{gsl::to_string(functionName)}, ccode(gsl::to_string(cCode))
	{
		setName("func");
		setDescription("call C code");

		setExecInputs({""});
		setExecOutputs({""});

		std::string bitcode;
		std::string error;
		try {
			// compile the C code
			exec_stream_t clangexe;
			clangexe.set_wait_timeout(exec_stream_t::s_out, 100000);
			std::vector<std::string> arguments = {"-xc", "-", "-c", "-emit-llvm", "-O0", "-o-"};
			clangexe.start(CHIG_CLANG_EXE, arguments.begin(), arguments.end());
			clangexe.in() << cCode.data();
			clangexe.close_in();

			bitcode = std::string{
				std::istreambuf_iterator<char>(clangexe.out()), std::istreambuf_iterator<char>()};

			error = std::string{
				std::istreambuf_iterator<char>(clangexe.err()), std::istreambuf_iterator<char>()};
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			return;
		}

		if (!error.empty()) {
			res.add_entry("EUKN", "Error encountered while generating IR", {{"Error Text", error}});
			return;
		}

		if (bitcode.empty()) {
			res.add_entry("EUKN", "Unknown error encountered while generating IR", {});
			return;
		}

		llvm::SMDiagnostic diag;
		auto modorerror = llvm::parseBitcodeFile(
			llvm::MemoryBufferRef(bitcode, "clang-generated"), context().llvmContext());

		if (!modorerror) {
			std::string errorString;
			llvm::raw_string_ostream errorStream(errorString);
			diag.print("chig compile", errorStream);

			res.add_entry("EUKN", "Error parsing clang-generated bitcode module",
				{{"Error Code", modorerror.getError().value()}, {"Error Text", errorString}});
			return;
		}
		llcompiledmod = std::move(*modorerror);

		auto llfunc = llcompiledmod->getFunction(functocall);

		if (llfunc == nullptr) {
			res.add_entry("EUKN", "Failed to get function in clang-compiled module",
				{{"Requested Function", functocall}});
			return;
		}

		// get arguments
		std::vector<std::pair<DataType, std::string>> dInputs;
		for (const auto& argument : llfunc->args()) {
			dInputs.emplace_back(
				DataType(context().moduleByName("lang"),
					stringifyLLVMType(argument.getType()), argument.getType()),
				argument.getName());
		}
		setDataInputs(std::move(dInputs));

		// get return type
		auto ret = llfunc->getReturnType();

		if (!ret->isVoidTy()) {
			setDataOutputs(
				{{{context().moduleByName("lang"), stringifyLLVMType(ret), ret}, ""}});
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
			res.add_entry("WUKN", "Data for c:func must be an object", {{"Given Data"}, json_data});
		}

		std::string code;
		if (json_data.is_object() && json_data.find("code") != json_data.end() &&
			json_data["code"].is_string()) {
			code = json_data["code"];
		} else {
			res.add_entry("WUKN",
				"Data for c:func must have a pair with the key of code and that the data is a "
				"string",
				{{"Given Data"}, json_data});
		}

		std::string function;
		if (json_data.is_object() && json_data.find("function") != json_data.end() &&
			json_data["function"].is_string()) {
			function = json_data["function"];
		} else {
			res.add_entry("WUKN",
				"Data for c:func must have a pair with the key of function and that the data is a "
				"string",
				{{"Given Data"}, json_data});
		}

		*toFill = std::make_unique<CFuncNode>(*this, code, function, res);
		return res;
	}

	res.add_entry(
		"E37", "Unrecognized node type in module", {{"Module", "c"}, {"Requested Type", name()}});
	return res;
}
