#ifndef CHIGRAPHGUI_DEBUGGER_BREAKPOINT_VIEW_HPP
#define CHIGRAPHGUI_DEBUGGER_BREAKPOINT_VIEW_HPP

#include <KLocalizedString>

#include <chi/Debugger.hpp>

#include "../toolview.hpp"

#include <QTreeView>

class BreakpointView : public QTreeView, public ToolView {
public:
	
	void addBreakpoints(chi::NodeInstance* inst);
	
	void setBreakpoints(chi::Debugger& dbg);
	
	const std::vector<chi::NodeInstance*>& breakpoints() { return mBreakpoints; }
	
private:
	
	// ToolView interface
	QWidget*           toolView() override { return this; }
	Qt::DockWidgetArea defaultArea() const override { return Qt::BottomDockWidgetArea; }
	QString            label() override { return i18n("Breakpoints"); }

	
	std::vector<chi::NodeInstance*> mBreakpoints;
	
};

#endif  // CHIGRAPHGUI_DEBUGGER_BREAKPOINT_VIEW_HPP
