#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>

#include <nodes/FlowView>
#include <nodes/FlowScene>

class MainWindow : public KXmlGuiWindow {
	
public:
	MainWindow(QWidget* parent = nullptr);
	
	void setupActions();
	
	FlowScene* scene = nullptr;
	FlowView* view = nullptr;
	
public slots:
	void openFile();
	
};

#endif // CHIGGUI_MAINWINDOW_H
