#pragma once

#ifndef CHIG_DEBUGGER_DEBUGGER_HPP
#define CHIG_DEBUGGER_DEBUGGER_HPP

#include <lldb/API/SBDebugger.h>

#include <chig/GraphModule.hpp>
#include <chig/GraphFunction.hpp>

namespace chig {

class Debugger {
public:
	/// Default constructor
	/// \param pathToChig The path to the chig executable for JIT
	Debugger(const char* pathToChig);
	~Debugger();
	
	void run(GraphModule& mod, int argc, const char** argv);
	void terminate();
	
	bool running() const;
	
	void setBreakpoint(NodeInstance& node);
	void setBreakpoint(GraphFunction& func) {
		setBreakpoint(*func.entryNode());
	}
	
	bool removeBreakpoint(NodeInstance& node);
	bool removeBreakpoint(GraphFunction& func) {
		return removeBreakpoint(*func.entryNode());
	}
	
	std::vector<NodeInstance*> listBreakpoints();
	
private:
	lldb::SBDebugger mDebugger;
};

} // namespace chig

#endif // CHIG_DEBUGGER_DEBUGGER_HPP
