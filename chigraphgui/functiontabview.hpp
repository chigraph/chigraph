#pragma once

#ifndef CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP
#define CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP

#include <QTabWidget>

#include <KXMLGUIClient>

class FunctionTabView : public QTabWidget, public KXMLGUIClient {
public:
	FunctionTabView(QWidget* parent = nullptr);
};

#endif // CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP
