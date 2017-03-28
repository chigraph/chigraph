#pragma once

#ifndef CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP
#define CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP

#include <QString>
#include <QTabWidget>

#include <KXMLGUIClient>

#include <chi/Fwd.hpp>

#include <unordered_map>

#include <boost/filesystem.hpp>

class FunctionView;

class FunctionTabView : public QTabWidget, public KXMLGUIClient {
	Q_OBJECT
public:
	FunctionTabView(QWidget* parent = nullptr);

	void selectNewFunction(chi::GraphFunction& func);
	void centerOnNode(chi::NodeInstance& inst);
	void selectNode(chi::NodeInstance& inst);


	// refresh all the functions in the module
	void refreshModule(chi::GraphModule& mod);

	FunctionView* viewFromFunction(chi::GraphFunction& func);
	FunctionView* viewFromName(const QString& fullName);
	FunctionView* viewFromName(const boost::filesystem::path& mod, const std::string& function);

	void closeView(FunctionView* view);
	
	FunctionView* currentView();

signals:
	void dirtied(chi::GraphModule& mod);
	void functionOpened(FunctionView* func);

private:
	void closeTab(int idx);

	std::map<QString, FunctionView*> mOpenFunctions;
};

#endif  // CHIGRAPHGUI_FUNCTION_TAB_VIEW_HPP
