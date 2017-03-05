#include "chig/Debugger.hpp"

#include <chig/NodeInstance.hpp>

#include <boost/filesystem.hpp>

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
	
Debugger::Debugger(const char* pathToChig, GraphModule& mod) : mDebugger{lldb::SBDebugger::Create()}, mModule{&mod} {
	lldb::SBDebugger::Initialize();
	
	mDebugger.SetCloseInputOnEOF(false);
	
	// create target
	mTarget = mDebugger.CreateTarget(pathToChig);
}

Debugger::~Debugger() {
	lldb::SBDebugger::Terminate();
}


void Debugger::setBreakpoint(NodeInstance& node) {
	
	auto lineAssoc = node.module().createLineNumberAssoc(); // TODO: cache these, they're kinda expensive to make
	auto lineNumberIter = lineAssoc.right.find(&node);
	if (lineNumberIter == lineAssoc.right.end()) {
		return;
	}
	
	mBreakpoints[&node] = mTarget.BreakpointCreateByLocation(mModule->sourceFilePath().c_str(), lineNumberIter->second);
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
		tmpIRPath = fs::unique_path();
		std::error_code ec; // TODO: use ec
		llvm::raw_fd_ostream file{tmpIRPath.string(), ec, llvm::sys::fs::F_RW};
		llvm::WriteBitcodeToFile(mod.get(), file);
	}
	
	// start the process
	{
		lldb::SBError err;
		lldb::SBListener listener; // TODO: use a listener?
		mProcess = mTarget.Launch(listener,
			argv, envp, tmpIRPath.c_str(), nullptr, nullptr, workingDirectory.c_str(), lldb::eLaunchFlagDebug, false, err);
		
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
