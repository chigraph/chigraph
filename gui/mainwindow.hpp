#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QListView>
#include <QTabWidget>

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <chig/ChigModule.hpp>
#include <chig/Context.hpp>
#include <chig/JsonModule.hpp>

#include "functionspane.hpp"
#include "functionview.hpp"

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);

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
	void newFunctionSelected(QString name);
	void save();
	void closeTab(int idx);

signals:
	void openJsonModule(chig::JsonModule* mod);
};

#endif  // CHIGGUI_MAINWINDOW_H
