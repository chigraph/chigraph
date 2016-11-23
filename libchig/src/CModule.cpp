#include "chig/CModule.hpp"
#include "chig/Config.hpp"

#include <iterator>

#include <exec-stream.h>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Transforms/Utils/Cloning.h>

using namespace chig;


CModule::CModule(Context& cont) : ChigModule(cont) {
	name = "c";

}

llvm::Type* CModule::getType(const char* name) const {
	// TODO: implement

	return nullptr;
}

std::unique_ptr<NodeType> CModule::createNodeType(const char* name, const nlohmann::json& json_data) const {

	if(strcmp(name, "func") == 0) {
		
		std::string code;
		if(json_data.is_object() && json_data.find("code") != json_data.end() && json_data["code"].is_string()) {
			code = json_data["code"];
		}
		
		std::string function;
		if(json_data.is_object() && json_data.find("function") != json_data.end() && json_data["function"].is_string()) {
			function = json_data["function"];
		}
		
		return std::make_unique<CFuncNode>(*context, code, function);
	}

	return nullptr;
}

CFuncNode::CFuncNode(chig::Context& con, const std::string& Ccode, const std::string& functocall_) : NodeType{con}, functocall{functocall_}, ccode(Ccode)
{
	module = "c";
	name = "func";
	description = "call C code";

	execInputs = {""};
	execOutputs = {""};

	std::string bitcode;
	std::string error;
	try {

		// compile the C code
		exec_stream_t clangexe;
		clangexe.set_wait_timeout(exec_stream_t::s_out, 100000);
		std::vector<std::string> arguments = {"-xc", "-", "-c", "-emit-llvm", "-O0", "-o-"};
		clangexe.start(CHIG_CLANG_EXE, arguments.begin(), arguments.end());
		clangexe.in() << Ccode;
		clangexe.close_in();

		bitcode = std::string{std::istreambuf_iterator<char>(clangexe.out()),
			std::istreambuf_iterator<char>()};


		error = std::string{std::istreambuf_iterator<char>(clangexe.err()),
			std::istreambuf_iterator<char>()};
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		return;
	}

	if(!error.empty()) {
		std::cerr << "Error encountered while generating IR: " << error << std::endl;
	}

	if (bitcode.empty()) {
		std::cerr << "Failed to gen IR!" << std::endl;
		return;
	}

	llvm::SMDiagnostic diag;
	auto modorerror = llvm::parseBitcodeFile(llvm::MemoryBufferRef(bitcode, "clang-generated"), con.llcontext);
	
	if(!modorerror) {
		std::cerr << "error parsing clang-generated bitcode module: " << modorerror.getError() << std::endl;
		return;
	}
	llcompiledmod = std::move(*modorerror);
	
	if(!llcompiledmod) {
		diag.print("chig compile", llvm::errs());
		return;
	}

	auto llfunc = llcompiledmod->getFunction(functocall);

	if(!llfunc) {
		std::cerr << "Failed to get function " << functocall << " in C module." << std::endl;
		return;
	}

	// get arguments
	std::transform(llfunc->arg_begin(), llfunc->arg_end(), std::back_inserter(dataInputs), [](const llvm::Argument& argument) {
		return std::make_pair(argument.getType(), argument.getName());
	});

	// get return type
	dataOutputs = {{llfunc->getReturnType(), ""}};


}



std::unique_ptr<NodeType> CFuncNode::clone() const
{
	// TODO: implement
	return nullptr;
}

Result CFuncNode::codegen(size_t, llvm::Module* mod, llvm::Function* f, const std::vector<llvm::Value *>& io, llvm::BasicBlock* codegenInto, const std::vector<llvm::BasicBlock *>& outputBlocks) const
{
	// create a copy of the module
	auto copymod = llvm::CloneModule(llcompiledmod.get());

	// link it in
	llvm::Linker::linkModules(*mod, std::move(copymod));

	auto llfunc = mod->getFunction(functocall);
	assert(llfunc);

	llvm::IRBuilder<> builder(codegenInto);

	std::vector<llvm::Value*> inputs = io;
	if(dataOutputs.size()) {
		inputs.pop_back();
	}
	
	auto callinst = builder.CreateCall(llfunc, inputs);

	if(dataOutputs.size()) {

		builder.CreateStore(callinst, io[dataInputs.size()]);
	}

	builder.CreateBr(outputBlocks[0]);


	return {};
}

nlohmann::json CFuncNode::toJSON() const
{
	auto j = nlohmann::json{};

	j = nlohmann::json::object();
	j["code"] = ccode;
	j["function"] = functocall;

	return j;
}
