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
#include <chig/GraphModule.hpp>

#include <QProcess>

#include "functionview.hpp"

class MainWindow : public KXmlGuiWindow {
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	chig::GraphModule* currentModule() const { return mModule; }
	chig::Context&     context() const { return *mChigContext; }
public slots:
	void openWorkspaceDialog();           // this one opens a dialog
	void openWorkspace(const QUrl& url);  // and this one doesn't
	void openModule(const QString& fullName);
	void newFunctionSelected(chig::GraphFunction* func);
	void save();
	void closeTab(int idx);
	void newFunction();
	void newModule();

signals:
	void workspaceOpened(chig::Context& workspace);
	void moduleOpened(chig::GraphModule& mod);
	void functionOpened(FunctionView* func);

	void newModuleCreated(chig::GraphModule* newModule);
	void newFunctionCreated(chig::GraphFunction* func);

private:
	KRecentFilesAction* mOpenRecentAction;  // keep this so we can save the entries
	std::unordered_map<QString, FunctionView*> mOpenFunctions;

	// the tabs for open functions
	QTabWidget* mFunctionTabs = nullptr;

	// context & module
	std::unique_ptr<chig::Context> mChigContext = nullptr;
	chig::GraphModule*             mModule      = nullptr;
};

#endif  // CHIGGUI_MAINWINDOW_H
