#include <catch.hpp>

#include <chig/Context.hpp>
#include <chig/GraphModule.hpp>
#include <chig/NodeInstance.hpp>
#include <chig/Result.hpp>

#include <chig/Debugger.hpp>

#include <llvm/Support/FileSystem.h>

#include <lldb/API/SBEvent.h>
#include <lldb/API/SBFrame.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBThread.h>

using namespace chig;
using namespace nlohmann;

namespace fs = boost::filesystem;

TEST_CASE("Debugger", "") {
	// load the ctx
	Context ctx{fs::current_path() / "codegen" / "workspace"};

	Result res;

	// load a mod
	GraphModule* gMod;
	{
		ChigModule* cMod;
		res += ctx.loadModule("if/main", &cMod);
		gMod = static_cast<GraphModule*>(cMod);
	}
	REQUIRE(res.dump() == "");

	GraphFunction* func = gMod->functionFromName("main");
	REQUIRE(func != nullptr);

	NodeInstance* entryNode = func->entryNode();
	REQUIRE(entryNode != nullptr);

	NodeInstance* ifNode = entryNode->outputExecConnections[0].first;
	REQUIRE(ifNode != nullptr);
	REQUIRE(ifNode->type().qualifiedName() == "lang:if");

	NodeInstance* callNode = ifNode->outputExecConnections[0].first;
	REQUIRE(callNode != nullptr);
	REQUIRE(callNode->type().qualifiedName() == "c:func");

	NodeInstance* boolLiteral = ifNode->inputDataConnections[0].first;
	REQUIRE(boolLiteral != nullptr);
	REQUIRE(boolLiteral->type().qualifiedName() == "lang:const-bool");

	// make a debugger
	boost::filesystem::path chigPath =
	    boost::filesystem::path(llvm::sys::fs::getMainExecutable(nullptr, nullptr)).parent_path() /
	        "chig"
#ifdef _WIN32
	    + ".exe"
#endif
	    ;

	Debugger dbg{chigPath.c_str(), *gMod};

	dbg.setBreakpoint(*callNode);

	dbg.start();

	auto          listener = dbg.lldbDebugger().GetListener();
	lldb::SBEvent ev;
	while (true) {
		listener.WaitForEvent(3, ev);
		if (ev.IsValid())
			std::cout << "EVENT" << ev.GetDataFlavor() << ", " << ev.GetBroadcasterClass() << ", "
			          << lldb::SBProcess::GetStateFromEvent(ev) << std::endl;
		std::cout.flush();

		if (lldb::SBProcess::GetStateFromEvent(ev) == lldb::eStateStopped) break;
	}

	dbg.processContinue();

	while (true) {
		listener.WaitForEvent(3, ev);
		if (ev.IsValid())
			std::cout << "EVENT" << ev.GetDataFlavor() << ", " << ev.GetBroadcasterClass() << ", "
			          << lldb::SBProcess::GetStateFromEvent(ev) << std::endl;
		std::cout.flush();

		if (lldb::SBProcess::GetStateFromEvent(ev) == lldb::eStateStopped) break;
	}

	auto value = dbg.inspectNodeOutput(*boolLiteral, 0);
	REQUIRE(value.IsValid());

	REQUIRE(value.GetValue() == std::string("true"));

	dbg.terminate();
}
