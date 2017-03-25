#pragma once

#ifndef CHIGRAPHGUI_TOOL_VIEW_HPP
#define CHIGRAPHGUI_TOOL_VIEW_HPP

#include <kxmlguiclient.h>

class ToolView : public KXMLGUIClient {
public:
	virtual QWidget*           toolView()          = 0;
	virtual Qt::DockWidgetArea defaultArea() const = 0;
	virtual QString            label()             = 0;
};

#endif  // CHIGRAPHGUI_TOOL_VIEW_HPP
