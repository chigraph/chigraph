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
class FunctionTabView;

class MainWindow : public KXmlGuiWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	chi::GraphModule* currentModule() const { return mModule; }
	chi::Context&     context() const { return *mChigContext; }
	FunctionTabView&  tabView() const { return *mFunctionTabs; }

private:
	static MainWindow* mInstance;

public:
	static MainWindow* instance() { return mInstance; }

public slots:
	void openWorkspaceDialog();           // this one opens a dialog
	void openWorkspace(const QUrl& url);  // and this one doesn't
	void openModule(const QString& fullName);
	void save();
	void newFunction();
	void newModule();
	void moduleDirtied(chi::GraphModule& mod);
	void discardChangesInModule(chi::GraphModule& mod);

signals:
	void workspaceOpened(chi::Context& workspace);
	void moduleOpened(chi::GraphModule& mod);
	void functionOpened(FunctionView* func);

	void newModuleCreated(chi::GraphModule& newModule);
	void newFunctionCreated(chi::GraphFunction& func);

private:
	void closeEvent(QCloseEvent* event) override;

	KRecentFilesAction* mOpenRecentAction;  // keep this so we can save the entries

	// the tabs for open functions
	FunctionTabView* mFunctionTabs = nullptr;

	// context & module
	std::unique_ptr<chi::Context> mChigContext   = nullptr;
	chi::GraphModule*             mModule        = nullptr;
	ModuleBrowser*                mModuleBrowser = nullptr;

	std::unique_ptr<ThemeManager> mThemeManager;
};

#endif  // CHIGGUI_MAINWINDOW_H
