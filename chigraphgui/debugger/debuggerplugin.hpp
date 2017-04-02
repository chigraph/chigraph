#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP
#define CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP

#include "../chigraphplugin.hpp"

#include "breakpointview.hpp"
#include "debuggerworkerthread.hpp"
#include "variableview.hpp"

#include <QAction>

#include <chi/Debugger.hpp>

class DebuggerPlugin : public QObject, public ChigraphPlugin {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID ChigraphPlugin_iid FILE "debugger.json")
	Q_INTERFACES(ChigraphPlugin)

public:
	DebuggerPlugin();

	BreakpointView& breakpointView() { return *mBreakpointView; }
	VariableView&   variableView() { return *mVariableView; }

private slots:
	void toggleBreakpoint();
	void debugStart();
	void continueDebugging();

private:
	// ChigraphPlugin interface
	QVector<ToolView*> toolViews() override {
		return QVector<ToolView*>({mBreakpointView, mVariableView});
	}

	BreakpointView* mBreakpointView;
	VariableView*   mVariableView;

	QAction* toggleBreakpointAction;
	QAction* debugAction;
	QAction* stepAction;
	QAction* stepInAction;
	QAction* stepOutAction;
	QAction* continueAction;

	std::shared_ptr<chi::Debugger>        mDebugger;
	std::unique_ptr<DebuggerWorkerThread> mEventListener;
	QThread*                              mThread = nullptr;
};

#endif  // CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP
