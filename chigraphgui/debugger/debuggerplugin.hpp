#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP
#define CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP

#include "../chigraphplugin.hpp"

#include <QAction>

#include <chi/Debugger.hpp>

class DebuggerPlugin : public QObject, public ChigraphPlugin {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID ChigraphPlugin_iid FILE "debugger.json")
    Q_INTERFACES(ChigraphPlugin)
	
public:
	
	
	DebuggerPlugin();
	
	
private:
	
	// ChigraphPlugin interface
	QVector<ToolView*> toolViews() override {
		return {};
	}
	
	QAction* toggleBreakpointAction;
	QAction* debugAction;
	QAction* stepAction;
	QAction* stepInAction;
	QAction* stepOutAction;
	QAction* continueAction;
	
	void debugStart();
	
	
	std::unique_ptr<chi::Debugger> mDebugger;

};


#endif // CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP
