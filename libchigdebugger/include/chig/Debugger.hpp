#pragma once

#ifndef CHIG_DEBUGGER_DEBUGGER_HPP
#define CHIG_DEBUGGER_DEBUGGER_HPP

#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBTarget.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBBreakpoint.h>
#include <lldb/API/SBListener.h>

#include <chig/GraphModule.hpp>
#include <chig/GraphFunction.hpp>

namespace chig {

class Debugger {
public:
	/// Default constructor
	/// \param pathToChig The path to the chig executable for JIT
	Debugger(const char* pathToChig, GraphModule& mod);
	Debugger(const Debugger& other) = delete;
	Debugger(Debugger&& other) = delete;
	
	~Debugger();
	
	Debugger& operator=(const Debugger&) = delete;
	Debugger& operator=(Debugger&&) = delete;
	
	/// Start debugging the process
	/// \param argv A null-terminated array of arguments. If it isn't null-terminated then this will _probably_ crash
	/// \param envp A null-terminated array of environment variables. If it isn't null-terminated then this will _probably_ crash
	Result start(const char** argv = nullptr, const char** envp = nullptr, const boost::filesystem::path& workingDirectory = boost::filesystem::current_path());
	Result terminate();
	
	bool isAttached() const {
		return mProcess.IsValid();
	}
	
	bool running() {
		if (!isAttached()) {
			return false;
		}
		return mProcess.GetState() == lldb::eStateRunning;
	}
	
	Result processContinue();
	Result pause();
	
	Result setBreakpoint(NodeInstance& node, lldb::SBBreakpoint* bp = nullptr);
	bool removeBreakpoint(NodeInstance& node);
	
	std::vector<NodeInstance*> listBreakpoints();
	
	GraphModule& module() const { return *mModule; }
	
	lldb::SBTarget lldbTarget() const { return mTarget; }
	lldb::SBProcess lldbProcess() const { return mProcess; }
	lldb::SBDebugger lldbDebugger() const { return mDebugger; }
	
private:
	
	lldb::SBDebugger mDebugger;
	lldb::SBTarget mTarget;
	lldb::SBProcess mProcess;
	
	GraphModule* mModule = nullptr;
	
	std::unordered_map<NodeInstance*, lldb::SBBreakpoint> mBreakpoints;
};

} // namespace chig

#endif // CHIG_DEBUGGER_DEBUGGER_HPP
