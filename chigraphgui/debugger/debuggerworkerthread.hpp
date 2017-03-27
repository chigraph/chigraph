#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_DEBUGGER_WORKER_THREAD_HPP
#define CHIGRAPHGUI_DEBUGGER_DEBUGGER_WORKER_THREAD_HPP

#include <QObject>

#include <chi/Debugger.hpp>

#include <lldb/API/SBEvent.h>

class DebuggerWorkerThread : public QObject {
	Q_OBJECT
	
public:
	DebuggerWorkerThread(chi::Debugger& dbg);
	
public slots:
	void process();
	
signals:
	
	void eventOccured(lldb::SBEvent ev);
	
private:
	chi::Debugger* mDebugger;
};

#endif // CHIGRAPHGUI_DEBUGGER_DEBUGGER_WORKER_THREAD_HPP
