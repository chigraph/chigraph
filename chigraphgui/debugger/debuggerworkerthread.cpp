#include "debuggerworkerthread.hpp"

Q_DECLARE_METATYPE(lldb::SBEvent)

DebuggerWorkerThread::DebuggerWorkerThread(chi::Debugger& dbg) : mDebugger(&dbg)
{
}

void DebuggerWorkerThread::process()
{
	auto listener = mDebugger->lldbDebugger().GetListener();
	
	lldb::SBEvent ev;
	while (true) {
		listener.WaitForEvent(3, ev);
		if (ev.IsValid()) {
			eventOccured(ev);
		}

	}
}


