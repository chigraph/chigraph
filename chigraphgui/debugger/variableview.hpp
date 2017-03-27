#pragma once

#ifndef CHIGRAPHGUI_DEBUGGER_VARIABLE_VIEW_HPP
#define CHIGRAPHGUI_DEBUGGER_VARIABLE_VIEW_HPP

#include "../toolview.hpp"

#include <QTreeWidget>

#include <KLocalizedString>

#include <lldb/API/SBFrame.h>

class VariableView : public QTreeWidget, public ToolView {
public:
	VariableView();
	
	void setFrame(lldb::SBFrame frame);
	
private:
	
	// ToolView interface
	QWidget*           toolView()          { return this; }
	Qt::DockWidgetArea defaultArea() const { return Qt::LeftDockWidgetArea; }
	QString            label()             { return i18n("Variables"); }
};

#endif // CHIGRAPHGUI_DEBUGGER_VARIABLE_VIEW_HPP
