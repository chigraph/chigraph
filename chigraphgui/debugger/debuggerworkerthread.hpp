#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_DEBUGGER_WORKER_THREAD_HPP
#define CHIGRAPHGUI_DEBUGGER_DEBUGGER_WORKER_THREAD_HPP

#include <QObject>

#include <chi/Debugger.hpp>

#include <lldb/API/SBEvent.h>

class DebuggerWorkerThread : public QObject {
	Q_OBJECT
	
public:
	DebuggerWorkerThread(std::shared_ptr<chi::Debugger> dbg);
	
public slots:
	void process();
	
signals:
	
	void eventOccured(lldb::SBEvent ev);
	
private:
	std::weak_ptr<chi::Debugger> mDebugger;
};

#endif // CHIGRAPHGUI_DEBUGGER_DEBUGGER_WORKER_THREAD_HPP
