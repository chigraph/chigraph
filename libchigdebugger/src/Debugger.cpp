/// \file Debugger.cpp

#include "chig/Debugger.hpp"

#include <chig/NodeInstance.hpp>
#include <chig/NameMangler.hpp>

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

#if LLVM_VERSION_MAJOR <= 3
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <lldb/API/SBListener.h>
#include <lldb/API/SBThread.h>

namespace fs = boost::filesystem;

namespace chig {

Debugger::Debugger(const char* pathToChig, GraphModule& mod) : mModule{&mod} {
	lldb::SBDebugger::Initialize();
	mDebugger = lldb::SBDebugger::Create();

	// set the logger to stderr for testing
	mDebugger.SetLoggingCallback(
	    [](const char* msg, void* dbg) {
		    std::cerr << msg;
		    std::cerr.flush();
		},
	    this);

	const char* val[] = {"api", nullptr};
	mDebugger.EnableLog("lldb", val);

	mDebugger.SetCloseInputOnEOF(false);

	// create target
	mTarget = mDebugger.CreateTarget(pathToChig);
}

Debugger::~Debugger() { lldb::SBDebugger::Terminate(); }

Result Debugger::terminate() {
	Result res;

	if (isAttached()) {
		auto err = mProcess.Kill();

		if (err.Fail()) {
			res.addEntry("EUKN", "Failed to terminate process",
			             {{"Error Code", err.GetError()}, {"Error String", err.GetCString()}});

			return res;
		}
	}

	return res;
}

Result Debugger::processContinue() {
	Result res;

	if (isAttached()) {
		auto err = mProcess.Continue();

		if (err.Fail()) {
			res.addEntry("EUKN", "Failed to continue process",
			             {{"Error Code", err.GetError()}, {"Error String", err.GetCString()}});

			return res;
		}
	}

	return res;
}
Result Debugger::pause() {
	Result res;

	if (isAttached()) {
		auto err = mProcess.Stop();

		if (err.Fail()) {
			res.addEntry("EUKN", "Failed to pause process",
			             {{"Error Code", err.GetError()}, {"Error String", err.GetCString()}});

			return res;
		}
	}

	return res;
}

Result Debugger::setBreakpoint(NodeInstance& node, lldb::SBBreakpoint* bp) {
	Result res;
	
	int linenum = lineNumberFromNode(node);
	
	// create the breakpoint
	auto breakpoint = mTarget.BreakpointCreateByLocation(node.module().sourceFilePath().string().c_str(),
	                                                     linenum);

	// make sure that
	if (!breakpoint.IsValid()) {
		res.addEntry("EUKN", "Could not set breakpoint on node",
		             {{"nodeid", node.stringId()},
		              {"File Name", node.module().sourceFilePath().string()},
		              {"Line  Number", linenum}});

		return res;
	}

	mBreakpoints[&node] = breakpoint;

	if (bp != nullptr) { *bp = breakpoint; }

	breakpoint.SetEnabled(true);

	return res;
}

bool Debugger::removeBreakpoint(NodeInstance& node) {
	auto iter = mBreakpoints.find(&node);
	if (iter == mBreakpoints.end()) { return false; }

	return mTarget.BreakpointDelete(iter->second.GetID());
}

Result Debugger::start(const char** argv, const char** envp,
                       const boost::filesystem::path& workingDirectory) {
	Expects(boost::filesystem::is_directory(workingDirectory));
	
	Result res;

	if (!mTarget.IsValid()) {
		res.addEntry("EUKN", "Cannot start a debugger process with an invalid target", {});
		return res;
	}

	// generate IR
	std::unique_ptr<llvm::Module> mod;
	{
		res = module().context().compileModule(module(), &mod);
		if (!res) { return res; }
	}

	// write it to a file
	fs::path tmpIRPath;
	{
		tmpIRPath = boost::filesystem::temp_directory_path() / fs::unique_path();
		std::error_code      ec;  // TODO: use ec
		llvm::raw_fd_ostream file{tmpIRPath.string(), ec, llvm::sys::fs::F_RW};
		llvm::WriteBitcodeToFile(mod.get(), file);
	}

	// create args
	std::vector<const char*> args;
	{
		args.push_back("interpret");
		args.push_back("-i");
		args.push_back(tmpIRPath.string().c_str());
		args.push_back("-O");
		args.push_back("0");

		if (argv != nullptr) {
			for (; *argv != nullptr; ++argv) { args.push_back(*argv); }
		}

		args.push_back(nullptr);
	}

	// start the process
	{
		lldb::SBError    err;
		lldb::SBListener invalidListener;
		mProcess = mTarget.Launch(invalidListener, args.data(), envp, nullptr, nullptr, nullptr,
		                          workingDirectory.string().c_str(), lldb::eLaunchFlagDebug, false, err);

		if (err.Fail()) {
			res.addEntry("EUKN", "Failed to launch process", {{"Error Message", err.GetCString()}});
		}
	}

	return res;
}

std::vector<const NodeInstance*> Debugger::listBreakpoints() const {
	std::vector<const NodeInstance*> ret;
	ret.reserve(mBreakpoints.size());
	for (const auto& bpts : mBreakpoints) { ret.push_back(bpts.first); }
	return ret;
}


lldb::SBValue Debugger::inspectNodeOutput(const NodeInstance& inst, size_t id, lldb::SBFrame frame) {
	Expects(id < inst.outputDataConnections.size());

	// if frame isn't valid, use the default
	if (!frame.IsValid()) {
		auto thread= lldbProcess().GetSelectedThread();
		if (!thread.IsValid()) {
			return {};
		}
		frame = thread.GetSelectedFrame();
	}
	
	if (!frame.IsValid()) {
		return {};
	}

	// make sure it's in scope
	auto func = &nodeFromFrame(frame)->function();
	if (func != &inst.function()) {
		return {};
	}
	
	auto variableName = inst.stringId() + "__" + std::to_string(id);
	return frame.FindVariable(variableName.c_str());
}

NodeInstance* Debugger::nodeFromFrame(lldb::SBFrame frame) {
	
	using namespace std::string_literals;
	
	// if frame isn't valid, use the default
	if (!frame.IsValid()) {
		auto thread= lldbProcess().GetSelectedThread();
		if (!thread.IsValid()) {
			return {};
		}
		frame = thread.GetSelectedFrame();
	}
	
	if (!frame.IsValid()) {
		return {};
	}

	
	auto mangledFunctionName = frame.GetFunctionName();
	// demangle that
	std::string moduleName, functionName;
	std::tie(moduleName, functionName) = unmangleFunctionName(mangledFunctionName);
	
	GraphFunction* func = nullptr;
	
	if (mangledFunctionName == "main"s) {
		func = module().functionFromName("main");
	}
	
	if (func == nullptr) {
		auto mod = static_cast<GraphModule*>(module().context().moduleByFullName(moduleName));
		if (!mod) {
			return nullptr;
		}
		func = mod->functionFromName(functionName);
	}
	
	
	unsigned lineNo = frame.GetLineEntry().GetLine();
	
	// create assoc TODO: cache these
	auto assoc = func->module().createLineNumberAssoc();
	
	auto nodeIter = assoc.left.find(lineNo);
	if (nodeIter == assoc.left.end()) {
		return nullptr;
	}
	
	return nodeIter->second;
}



unsigned lineNumberFromNode(NodeInstance& inst) {
	// TODO: cache these, they're kinda expensive to make
	auto lineAssoc =
		inst.module()
			.createLineNumberAssoc();  
	auto lineNumberIter = lineAssoc.right.find(&inst);
	if (lineNumberIter == lineAssoc.right.end()) {
		return -1;
	}
	
	return lineNumberIter->second;
}

}  // namespace chig
