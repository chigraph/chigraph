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
#include "functiondetails.hpp"
#include "functionspane.hpp"
#include "functionview.hpp"
#include "modulebrowser.hpp"
#include "outputview.hpp"
#include "moduledependencies.hpp"

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
public:
    
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
    
    FunctionView* functionView(const QString& name) const {
        auto iter = mOpenFunctions.find(name);
        if (iter != mOpenFunctions.end()) {
            return iter->second;
        }
        return nullptr;
    }

    chig::JsonModule* currentModule() const { return mModule; }
    chig::Context& context() const { return *mChigContext; }
    
public slots:
	void openWorkspaceDialog(); // this one opens a dialog
	void openWorkspace(QUrl url); // and this one doesn't
	void openModule(const QString& fullName);
	void newFunctionSelected(chig::GraphFunction* name);
	void save();
	void closeTab(int idx);
	void run();
	void newFunction();
	void newModule();

signals:
	void workspaceOpened(chig::Context& workspace);
	void moduleOpened(chig::JsonModule* mod);
	void functionOpened(FunctionView* func);
    
    void newModuleCreated(chig::JsonModule* newModule);
    void newFunctionCreated(chig::GraphFunction* func);
    
    void runStarted(QProcess* process);

private:
    
	void setupActions();
    
	KRecentFilesAction* mOpenRecentAction; // keep this so we can save the entries
	std::unordered_map<QString, FunctionView*> mOpenFunctions;
	
    // the tabs for open functions
    QTabWidget* mFunctionTabs = nullptr;
    
    // context & module
	std::unique_ptr<chig::Context> mChigContext = nullptr;
	chig::JsonModule* mModule = nullptr;
};

#endif  // CHIGGUI_MAINWINDOW_H
