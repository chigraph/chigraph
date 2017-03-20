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

#include <chi/ChiModule.hpp>
#include <chi/Context.hpp>
#include <chi/GraphModule.hpp>

#include <QProcess>

#include "functionview.hpp"

class ModuleBrowser;
class ThemeManager;

class MainWindow : public KXmlGuiWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	chi::GraphModule* currentModule() const { return mModule; }
	chi::Context&     context() const { return *mChigContext; }
public slots:
	void openWorkspaceDialog();           // this one opens a dialog
	void openWorkspace(const QUrl& url);  // and this one doesn't
	void openModule(const QString& fullName);
	void newFunctionSelected(chi::GraphFunction* func);
	void save();
	void closeTab(int idx);
	void newFunction();
	void newModule();
	void moduleDirtied();
	void discardChangesInModule(chi::ChiModule& mod);

signals:
	void workspaceOpened(chi::Context& workspace);
	void moduleOpened(chi::GraphModule& mod);
	void functionOpened(FunctionView* func);

	void newModuleCreated(chi::GraphModule* newModule);
	void newFunctionCreated(chi::GraphFunction* func);

private:
	void closeEvent(QCloseEvent* event) override;

	KRecentFilesAction* mOpenRecentAction;  // keep this so we can save the entries
	std::unordered_map<QString, FunctionView*> mOpenFunctions;

	// the tabs for open functions
	QTabWidget* mFunctionTabs = nullptr;

	// context & module
	std::unique_ptr<chi::Context> mChigContext   = nullptr;
	chi::GraphModule*             mModule        = nullptr;
	ModuleBrowser*                 mModuleBrowser = nullptr;

	std::unique_ptr<ThemeManager> mThemeManager;
};

#endif  // CHIGGUI_MAINWINDOW_H
