#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>
#include <KStandardAction>

#include <QListView>
#include <QTabWidget>
#include <QUrl>

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chig/ChigModule.hpp>
#include <chig/Context.hpp>
#include <chig/JsonModule.hpp>

#include "functionspane.hpp"
#include "functionview.hpp"
#include <QProcess>

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

	void setupActions();

	std::unordered_map<QString, QWidget*> openFunctions;
	QTabWidget* functabs;
	std::shared_ptr<DataModelRegistry> reg;

	FunctionsPane* functionpane;

	chig::Context ccontext;

	void addModule(std::unique_ptr<chig::ChigModule> c);

	chig::JsonModule* module = nullptr;
	QString filename;

public slots:
	void openFile();
    void openUrl(QUrl url);
	void newFunctionSelected(QString name);
	void save();
	void closeTab(int idx);
    void run();
    void onSubprocessStdIn(QProcess* process);

signals:
	void openJsonModule(chig::JsonModule* mod);
    
private:
    KRecentFilesAction* openRecentAction;
};

#endif  // CHIGGUI_MAINWINDOW_H
