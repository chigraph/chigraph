#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/GraphFunction.hpp>
#include <chig/LangModule.hpp>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_os_ostream.h>

#include <fstream>

using namespace chig;

TEST_CASE("Code can be generated", "[GraphFunction]")
{
	// create some random nodes
	Context c;

	GraphFunction func(c, "main");

	std::vector<std::pair<llvm::Type*, std::string>> inputs = {
		{llvm::Type::getInt1Ty(c.context), "in1"}};
	auto entry = func.insertNode(std::make_unique<EntryNodeType>(c, inputs), 32, 32);
	auto ifNode = func.insertNode(c.getNodeType("lang", "if", {}), 44.f, 23.f);
	auto ret1Node = func.insertNode(
		c.getNodeType("lang", "exit", nlohmann::json::parse("{\"Hello\": \"lang:i32\"}")), 23.f,
		234.f);
	auto ret2Node = func.insertNode(
		c.getNodeType("lang", "exit", nlohmann::json::parse("{\"Hello\": \"lang:i32\"}")), 23.f,
		234.f);

	auto constant1 = func.insertNode(c.getNodeType("lang", "const-int", 1), 32.f, 23.f);
	auto constant2 = func.insertNode(c.getNodeType("lang", "const-int", 2), 32.f, 23.f);

	connectExec(*entry, 0, *constant1, 0);
	connectExec(*constant1, 0, *constant2, 0);
	connectExec(*constant2, 0, *ifNode, 0);

	connectData(*entry, 0, *ifNode, 0);

	connectExec(*ifNode, 0, *ret1Node, 0);
	connectExec(*ifNode, 1, *ret2Node, 0);

	connectData(*constant1, 0, *ret1Node, 0);
	connectData(*constant2, 0, *ret2Node, 0);

	// compile!
	auto mod = std::make_unique<llvm::Module>("testout", c.context);
	auto compiledfunc = func.compile(mod.get());

	std::ofstream file("test.ll");
	llvm::raw_os_ostream stream(file);
	mod->print(stream, nullptr);
}
