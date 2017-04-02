#include "debuggerworkerthread.hpp"

Q_DECLARE_METATYPE(lldb::SBEvent)

DebuggerWorkerThread::DebuggerWorkerThread(std::shared_ptr<chi::Debugger> dbg)
    : mDebugger(std::move(dbg)) {}

void DebuggerWorkerThread::process() {
	lldb::SBEvent ev;
	while (true) {
		// lock the listener
		auto strongPtr = mDebugger.lock();
		if (strongPtr == nullptr) { return; }
		auto listener = strongPtr->lldbDebugger().GetListener();

		listener.WaitForEvent(1, ev);
		if (ev.IsValid()) { eventOccured(ev); }
	}
}
