#pragma once

#ifndef CHIGRAPHGUI_CHIGRAPH_PLUGIN
#define CHIGRAPHGUI_CHIGRAPH_PLUGIN

#include "toolview.hpp"

#include <QVector>
#include <QtPlugin>

class ChigraphPlugin : public KXMLGUIClient {
public:
	virtual ~ChigraphPlugin() = default;

	virtual QVector<ToolView*> toolViews() = 0;
};

// tell qt that it's a thing
#define ChigraphPlugin_iid "org.chigraph.chigraphgui.ChigraphPlugin"
Q_DECLARE_INTERFACE(ChigraphPlugin, ChigraphPlugin_iid)

#endif  // CHIGRAPHGUI_CHIGRAPH_PLUGIN
