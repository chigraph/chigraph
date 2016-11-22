#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QListView>
#include <QTabWidget>

#include <nodes/FlowView>
#include <nodes/FlowScene>

#include <chig/Context.hpp>
#include <chig/ChigModule.hpp>
#include <chig/JsonModule.hpp>

#include "functionspane.hpp"
#include "functionview.hpp"

class MainWindow : public KXmlGuiWindow {
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	
	void setupActions();
	
	QTabWidget* functabs;
	std::shared_ptr<DataModelRegistry> reg;
	
	FunctionsPane* functionpane;
	
	chig::Context ccontext;
	
	void addModule(std::unique_ptr<chig::ChigModule> c);
	
	chig::JsonModule* module = nullptr;
	
public slots:
	void openFile();
	void newFunctionSelected(QString name);
	
signals:
	void openJsonModule(chig::JsonModule* mod);
	
	
};

#endif // CHIGGUI_MAINWINDOW_H
