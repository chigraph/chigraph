#pragma once

#ifndef CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP
#define CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP

#include <QString>
#include <QTabWidget>

#include <KXMLGUIClient>

#include <chi/Fwd.hpp>

#include <unordered_map>

class FunctionView;

class FunctionTabView : public QTabWidget, public KXMLGUIClient {
	Q_OBJECT
public:
	FunctionTabView(QWidget* parent = nullptr);

	void selectNewFunction(chi::GraphFunction& func);

	// delete and recreate the view, it has been reloaded
	void refreshFunction(chi::GraphFunction& func);

	// refresh all the functions in the module
	void refreshModule(chi::GraphModule& mod);

signals:
	void dirtied(chi::GraphModule& mod);
	void functionOpened(FunctionView* func);

private:
	void closeTab(int idx);

	std::map<QString, FunctionView*> mOpenFunctions;
};

#endif  // CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP
