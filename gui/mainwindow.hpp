#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KStandardAction>
#include <KXmlGuiWindow>

#include <QListView>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QUrl>

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chig/ChigModule.hpp>
#include <chig/Context.hpp>
#include <chig/JsonModule.hpp>

#include <QProcess>
#include "functionspane.hpp"
#include "functionview.hpp"
#include "modulebrowser.hpp"
#include "outputview.hpp"
#include "functiondetails.hpp"

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	void setupActions();

	std::unordered_map<QString, QWidget*> openFunctions;

	QTabWidget* functabs = nullptr;
	std::shared_ptr<DataModelRegistry> reg;
	OutputView* outputView = nullptr;
	ModuleBrowser* moduleBrowser = nullptr;
    FunctionDetails* funcDetails = nullptr;

	FunctionsPane* functionpane = nullptr;

	std::unique_ptr<chig::Context> ccontext = nullptr;

	void addModule(std::unique_ptr<chig::ChigModule> toAdd);

	chig::JsonModule* module = nullptr;

public slots:
	void openWorkspaceDialog();
	void openWorkspace(QUrl url);
	void openModule(QString path);
	void newFunctionSelected(chig::GraphFunction* name);
	void save();
	void closeTab(int idx);
	void run();
	void newFunction();

signals:
	void workspaceOpened(QString workspace);
	void openJsonModule(chig::JsonModule* mod);
    void newFunctionOpened(chig::GraphFunction* func);

private:
	KRecentFilesAction* openRecentAction;
};

#endif  // CHIGGUI_MAINWINDOW_H
