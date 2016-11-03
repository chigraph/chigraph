#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_os_ostream.h>

#include <fstream>

using namespace chig;

int main() {
	
	// create some random nodes
	Context c;
	
	GraphFunction func(c, "main");
	
	std::vector<std::pair<llvm::Type*, std::string>> inputs = {{llvm::Type::getInt1Ty(c.context), "in1"}};
	auto entry = func.insertNode(std::make_unique<EntryNodeType>(c, inputs), 32, 32);
	auto ifNode = func.insertNode(c.getNodeType("lang", "if", {}), 44.f, 23.f);
	
	
	connectExec(*entry, 0, *ifNode, 0);
	connectData(*entry, 0, *ifNode, 0);
	
	// compile!
	auto mod = std::make_unique<llvm::Module>("testout", c.context);
	auto compiledfunc = func.compile(mod.get());
	
	std::ofstream file("test.ll");
	llvm::raw_os_ostream stream(file);
	mod->print(stream, nullptr);
	
	
}


