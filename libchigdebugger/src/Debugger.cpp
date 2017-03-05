#include "chig/Debugger.hpp"

#include <chig/NodeInstance.hpp>

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

#if LLVM_VERSION_MAJOR <= 3
#include <llvm/Bitcode/ReaderWriter.h>
#else
#include <llvm/Bitcode/BitcodeWriter.h>
#endif

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include <lldb/API/SBListener.h>

namespace fs = boost::filesystem;

namespace chig {

Debugger::Debugger(const char* pathToChig, GraphModule& mod) : mModule{&mod} {
	lldb::SBDebugger::Initialize();
	mDebugger = lldb::SBDebugger::Create();
	
	// set the logger to stderr for testing
	mDebugger.SetLoggingCallback([](const char* msg, void* dbg) {
		std::cerr << msg;
		std::cerr.flush();
	}, this);
	
	const char* val[] = {"api", nullptr};
	mDebugger.EnableLog("lldb", val);
	
	mDebugger.SetCloseInputOnEOF(false);
	
	// create target
	mTarget = mDebugger.CreateTarget(pathToChig);
}

Debugger::~Debugger() {
	lldb::SBDebugger::Terminate();
}

Result Debugger::terminate() {
	Result res;
	
	if (isAttached()) {
		auto err = mProcess.Kill();
		
		if (err.Fail()) {
			res.addEntry("EUKN", "Failed to terminate process", {{"Error Code", err.GetError()}, {"Error String", err.GetCString()}});
			
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
			res.addEntry("EUKN", "Failed to continue process", {{"Error Code", err.GetError()}, {"Error String", err.GetCString()}});
			
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
			res.addEntry("EUKN", "Failed to pause process", {{"Error Code", err.GetError()}, {"Error String", err.GetCString()}});
			
			return res;
		}
	}
	
	return res;
}

Result Debugger::setBreakpoint(NodeInstance& node, lldb::SBBreakpoint* bp) {
	
	Result res;
	
	auto lineAssoc = node.module().createLineNumberAssoc(); // TODO: cache these, they're kinda expensive to make
	auto lineNumberIter = lineAssoc.right.find(&node);
	if (lineNumberIter == lineAssoc.right.end()) {
		res.addEntry("EINT", "Could not find no in lineNumberAssoc", {});
		return res;
	}
	
	auto breakpoint =  mTarget.BreakpointCreateByLocation(mModule->sourceFilePath().c_str(), lineNumberIter->second);

	
	mBreakpoints[&node] = breakpoint;
	
	if (bp != nullptr) {
		*bp = breakpoint;
	}
	
	breakpoint.SetEnabled(true);
	
	return res;
}

bool Debugger::removeBreakpoint(NodeInstance& node) {
	auto iter = mBreakpoints.find(&node);
	if (iter == mBreakpoints.end()) {
		return false;
	}
	
	return mTarget.BreakpointDelete(iter->second.GetID());
}

Result Debugger::start(const char** argv, const char** envp, const boost::filesystem::path& workingDirectory) {
	Result res;
	
	if (!mTarget.IsValid()) {
		res.addEntry("EUKN", "Cannot start a debugger process with an invalid target", {});
		return res;
	}
	
	// generate IR
	std::unique_ptr<llvm::Module> mod;
	{
		res = module().context().compileModule(module(), &mod);
		if (!res) {
			return res;
		}
	}
	
	// write it to a file
	fs::path tmpIRPath;
	{
		tmpIRPath = boost::filesystem::temp_directory_path() / fs::unique_path();
		std::error_code ec; // TODO: use ec
		llvm::raw_fd_ostream file{tmpIRPath.string(), ec, llvm::sys::fs::F_RW};
		llvm::WriteBitcodeToFile(mod.get(), file);
	}
	
	// create args
	std::vector<const char*> args;
	{
		args.push_back("interpret");
		args.push_back("-i");
		args.push_back(tmpIRPath.c_str());
		
		if (argv != nullptr) {
			for(; *argv != nullptr; ++argv) {
				args.push_back(*argv);
			}
		}
		
		args.push_back(nullptr);
	}
	
	// start the process
	{
		lldb::SBError err;
		lldb::SBListener invalidListener;
		mProcess = mTarget.Launch(invalidListener,
			args.data(), envp, nullptr, nullptr, nullptr, workingDirectory.c_str(), lldb::eLaunchFlagDebug, false, err);
		
		if (err.Fail()) {
			res.addEntry("EUKN", "Failed to launch process", {{"Error Message", err.GetCString()}});
		}
	}
	
	return res;
}

std::vector<NodeInstance*> Debugger::listBreakpoints() {
	std::vector<NodeInstance*> ret;
	ret.reserve(mBreakpoints.size());
	for (const auto& bpts : mBreakpoints) {
		ret.push_back(bpts.first);
	}
	return ret;
}

} // namespace chig
