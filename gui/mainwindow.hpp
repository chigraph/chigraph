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
#include "outputview.hpp"

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

	FunctionsPane* functionpane = nullptr;

	chig::Context ccontext;

	void addModule(std::unique_ptr<chig::ChigModule> toAdd);

	chig::JsonModule* module = nullptr;
	QString filename;

public slots:
	void openFile();
	void openUrl(const QUrl& url);
	void newFunctionSelected(QString name);
	void save();
	void closeTab(int idx);
	void run();

signals:
	void openJsonModule(chig::JsonModule* mod);

private:
	KRecentFilesAction* openRecentAction;
};

#endif  // CHIGGUI_MAINWINDOW_H
