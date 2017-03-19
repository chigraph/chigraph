#include <catch.hpp>

#include <chi/Context.hpp>
#include <chi/GraphModule.hpp>
#include <chi/NodeInstance.hpp>
#include <chi/Result.hpp>
#include <chi/Debugger.hpp>

#include <llvm/Support/FileSystem.h>

#include <lldb/API/SBEvent.h>
#include <lldb/API/SBFrame.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBThread.h>

#include <thread>

using namespace chi;
using namespace nlohmann;
using namespace std::chrono_literals;

namespace fs = boost::filesystem;

TEST_CASE("Debugger", "") {
	// load the ctx
	Context ctx{fs::current_path() / "codegen" / "workspace"};

	Result res;

	// load a mod
	GraphModule* gMod;
	{
		ChiModule* cMod;
		res += ctx.loadModule("intermodule/main", &cMod);
		gMod = static_cast<GraphModule*>(cMod);
	}
	REQUIRE(res.dump() == "");

	GraphModule* printerMod;
	{
		ChiModule* cMod = ctx.moduleByFullName("intermodule/printer");
		printerMod       = static_cast<GraphModule*>(cMod);
	}

	auto mainFunc  = gMod->functionFromName("main");
	auto mainEntry = mainFunc->entryNode();

	auto firstCall          = mainEntry->outputExecConnections[0].first;
	auto yayHappyStrLiteral = firstCall->inputDataConnections[0].first;

	auto func = printerMod->functionFromName("docall");
	REQUIRE(func != nullptr);

	auto entry = func->entryNode();
	REQUIRE(entry != nullptr);

	auto putsNode = entry->outputExecConnections[0].first;
	REQUIRE(putsNode != nullptr);

	auto exitNode = putsNode->outputExecConnections[0].first;

	// make a debugger
	boost::filesystem::path chigPath =
	    boost::filesystem::path(llvm::sys::fs::getMainExecutable(nullptr, nullptr)).parent_path() /
	    "chig";
#ifdef _WIN32
	chigPath.replace_extension(boost::filesystem::path(".exe"));
#endif

	Debugger dbg{chigPath.string().c_str(), *gMod};

	dbg.setBreakpoint(*putsNode);
	dbg.setBreakpoint(*exitNode);

	res = dbg.start();
	REQUIRE(res.dump() == "");

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

	REQUIRE(dbg.nodeFromFrame() == putsNode);

	auto value = dbg.inspectNodeOutput(*entry, 0);
	REQUIRE(value.IsValid());

	REQUIRE(value.GetSummary() == std::string(R"("Yay happy!")"));

	// go up the call stack
	auto thread = dbg.lldbProcess().GetSelectedThread();

	// select frame 1
	thread.SetSelectedFrame(1);

	REQUIRE(dbg.nodeFromFrame() == firstCall);

	REQUIRE(dbg.inspectNodeOutput(*yayHappyStrLiteral, 0).GetSummary() ==
	        std::string(R"("Yay happy!")"));

	thread.SetSelectedFrame(0);

	// step
	dbg.processContinue();

	// wait until we're back in business
	while (true) {
		listener.WaitForEvent(5, ev);
		if (ev.IsValid())
			std::cout << "EVENT" << ev.GetDataFlavor() << ", " << ev.GetBroadcasterClass() << ", "
			          << lldb::SBProcess::GetStateFromEvent(ev) << std::endl;
		std::cout.flush();

		if (lldb::SBProcess::GetStateFromEvent(ev) == lldb::eStateStopped) break;
	}

	std::this_thread::sleep_for(3s);
	REQUIRE(dbg.isAttached());
	REQUIRE(!dbg.running());

	std::cout << std::endl
	          << std::endl
	          << "NODE TYPE: " << dbg.nodeFromFrame()->type().qualifiedName() << std::endl
	          << std::endl;
	std::cout.flush();
	REQUIRE(dbg.nodeFromFrame() == exitNode);

	value = dbg.inspectNodeOutput(*entry, 0);
	REQUIRE(value.IsValid());

	REQUIRE(value.GetSummary() == std::string(R"("Yay happy!")"));

	dbg.terminate();
}
