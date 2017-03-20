/// \file chi/Debugger.hpp

#pragma once

#ifndef CHI_DEBUGGER_DEBUGGER_HPP
#define CHI_DEBUGGER_DEBUGGER_HPP

#include <lldb/API/SBBreakpoint.h>
#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBListener.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBTarget.h>

#include <chi/GraphFunction.hpp>
#include <chi/GraphModule.hpp>

namespace chi {

/// Helper class for using LLDB with chigraph
class Debugger {
public:
	/// Default constructor
	/// \param pathToChig The path to the chig executable for JIT
	/// \param mod The GraphModule to debug, should be a main module
	Debugger(const char* pathToChig, GraphModule& mod);

	// no copy or move, doesn't really make sense
	Debugger(const Debugger& other) = delete;
	Debugger(Debugger&& other)      = delete;

	/// Destructor
	~Debugger();

	Debugger& operator=(const Debugger&) = delete;
	Debugger& operator=(Debugger&&) = delete;

	/// Start debugging the process
	/// \param[in] argv A null-terminated array of arguments. If it isn't null-terminated then this
	/// will _probably_ crash
	/// \param[in] envp A null-terminated array of environment variables. If it isn't
	/// null-terminated then this will _probably_ crash
	/// \param[in] workingDirectory The working directory to debug in, by default it's the current
	/// working directory of this process
	/// \pre `boost::filesystem::is_directory(workingDirectory)`
	/// \return The result
	Result start(
	    const char** argv = nullptr, const char** envp = nullptr,
	    const boost::filesystem::path& workingDirectory = boost::filesystem::current_path());

	/// Terminate the inferior process
	/// \return The result
	Result terminate();

	/// Check if this debugger is attached to a process
	/// It's true if the process is stopped or if it's curretnly executing.
	/// \return True if it's attached
	bool isAttached() const { return mProcess.IsValid(); }

	/// Check if the process is executing
	/// \return If it's running
	bool running() const {
		if (!isAttached()) { return false; }
		return lldbProcess().GetState() == lldb::eStateRunning;
	}

	/// Continue the execution of the process.
	/// \return The Result
	Result processContinue();

	/// Pause the execution state
	/// \return The Result
	Result pause();

	/// Set a breakpoint on a given node
	/// \param[in] node The node to set the breakpoint on
	/// \param[out] bp The (optional) `SBBreakpoint` object to fill
	/// \return The result
	Result setBreakpoint(NodeInstance& node, lldb::SBBreakpoint* bp = nullptr);

	/// Remove a breakpoint from a node
	/// \param[in] node The node to remove the breakpoint from
	/// \return True if a breakpoint was removed, false otherwise
	bool removeBreakpoint(NodeInstance& node);

	/// List the curretnly set breakpoints
	/// \return The breakpoints
	std::vector<const NodeInstance*> listBreakpoints() const;

	/// Get the output of a node
	/// \param inst The instance
	/// \param id The ID of output to get
	/// \pre `id < inst.outputDataConnections.size()`
	/// \param frame The frame. If this isn't a valid `SBFrame` (the default), then it uses
	/// `lldbProcess().GetSelectedThread().GetSelectedFrame()`
	lldb::SBValue inspectNodeOutput(const NodeInstance& inst, size_t id, lldb::SBFrame frame = {});

	/// Get the value of a local variable
	/// \param name The name ofthe local variable to get the value of
	/// \return The value, or an invalid `SBValue` if the local variable wasn't found
	lldb::SBValue inspectLocalVariable(boost::string_view name);

	/// Get a NodeInstance from a frame
	/// \param frame The frame to get the function for. If this isn't a valid `SBFrame` (the
	/// default), then it uses `lldbProcess().GetSelectedThread().GetSelectedFrame()`
	/// \return The NodeInstance, or nullptr if one wasn't found
	NodeInstance* nodeFromFrame(lldb::SBFrame frame = {});

	/// Get the module that's being debugged by this debugger
	/// \return The module
	GraphModule& module() const { return *mModule; }

	/// Get the target
	/// Keep in mind the return isn't necessarily valid, although it should be
	lldb::SBTarget lldbTarget() const { return mTarget; }

	/// Get the process
	/// This will only be valid if there's an attached process
	lldb::SBProcess lldbProcess() const { return mProcess; }

	/// Get the debugger
	/// This should probably be valid...not sure why it wouldn't be
	lldb::SBDebugger lldbDebugger() const { return mDebugger; }

private:
	lldb::SBDebugger mDebugger;
	lldb::SBTarget   mTarget;
	lldb::SBProcess  mProcess;

	GraphModule* mModule = nullptr;

	std::unordered_map<const NodeInstance*, lldb::SBBreakpoint> mBreakpoints;
};

unsigned lineNumberFromNode(NodeInstance& inst);

}  // namespace chi

#endif  // CHI_DEBUGGER_DEBUGGER_HPP
