#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QListView>

#include <nodes/FlowView>
#include <nodes/FlowScene>

#include <chig/Context.hpp>
#include <chig/ChigModule.hpp>

#include "functionspane.hpp"

class MainWindow : public KXmlGuiWindow {
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	
	void setupActions();
	
	FlowScene* scene = nullptr;
	FlowView* view = nullptr;
	
	FunctionsPane* functionpane;
	
	chig::Context ccontext;
	
	void addModule(std::unique_ptr<chig::ChigModule> c);
	
public slots:
	void openFile();
	
signals:
	void openJsonModule(chig::JsonModule* mod);
	
};

#endif // CHIGGUI_MAINWINDOW_H
