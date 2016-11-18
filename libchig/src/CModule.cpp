#include "chig/CModule.hpp"
#include "chig/Config.h"

#include <iterator>

#include <exec-stream.h>

#include <llvm/IRReader/IRReader.h>
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
		
		return std::make_unique<CFuncNode>(*context, json_data["code"], json_data["function"]);
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
	
	std::string ir;
	std::string error;
	try {
	
		// compile the C code
		exec_stream_t clangexe;
		clangexe.set_wait_timeout(exec_stream_t::s_out, 100000);
		std::vector<std::string> arguments = {"-xc", "-", "-S", "-emit-llvm", "-O0", "-o-"};
		clangexe.start(CHIG_CLANG_EXE, arguments.begin(), arguments.end());
		clangexe.in() << Ccode;
		clangexe.close_in();
		
		ir = std::string{std::istreambuf_iterator<char>(clangexe.out()), 
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

	if (ir.empty()) {
		std::cerr << "Failed to gen IR!" << std::endl;
		return;
	}
	
	llvm::SMDiagnostic diag;
	llcompiledmod = llvm::parseIR({ir, "clang-generated"}, diag, con.llcontext);
	
	auto llfunc = llcompiledmod->getFunction(functocall);
	
	if(!llfunc) {
		std::cerr << "Failed to get function " << functocall << " in C module." << std::endl;
		return;
	}
	
	// get arguments
	std::transform(llfunc->arg_begin(), llfunc->arg_end(), std::back_inserter(dataInputs), [](const llvm::Argument& argument) {
		return std::make_pair(argument.getType(), argument.getName());
	});
	
	
}



std::unique_ptr<NodeType> CFuncNode::clone() const
{
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
	
	builder.CreateCall(llfunc, io);
	
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

