#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP
#define CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP

#include "../chigraphplugin.hpp"

#include <QAction>



class DebuggerPlugin : public QObject, public ChigraphPlugin {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID ChigraphPlugin_iid FILE "debugger.json")
    Q_INTERFACES(ChigraphPlugin)
	
public:
	
	QAction* toggleBreakpointAction;
	QAction* debugAction;
	QAction* stepAction;
	QAction* stepInAction;
	QAction* stepOutAction;
	QAction* continueAction;
	
	DebuggerPlugin();
	
	virtual QVector<ToolView*> toolViews() {
		return {};
	}

};


#endif // CHIGRAPHGUI_DEBUGGER_DEBUGGER_PLUGIN_HPP
