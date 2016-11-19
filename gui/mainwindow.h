#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>

class MainWindow : public KXmlGuiWindow {
	
public:
	MainWindow(QWidget* parent = nullptr);
	
	void setupActions();
	
};

#endif // CHIGGUI_MAINWINDOW_H
