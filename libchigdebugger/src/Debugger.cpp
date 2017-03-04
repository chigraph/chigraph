#include "chig/Debugger.hpp"

namespace chig {
	
Debugger::Debugger(const char* pathToChig) : mDebugger{lldb::SBDebugger::Create()} {
	lldb::SBDebugger::Initialize();
	
	mDebugger.SetCloseInputOnEOF(false);
	
	
}

Debugger::~Debugger() {
	lldb::SBDebugger::Terminate();
}
	
} // namespace chig
