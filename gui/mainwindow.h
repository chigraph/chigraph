#pragma once

#ifndef CHIGGUI_MAINWINDOW_H
#define CHIGGUI_MAINWINDOW_H

#include <KXmlGuiWindow>

class GraphicsNodeView;
class GraphicsNodeScene;


class MainWindow : public KXmlGuiWindow {
	
public:
	MainWindow(QWidget* parent = nullptr);
	
	void setupActions();
	
	GraphicsNodeView *view = nullptr;
	GraphicsNodeScene *scene = nullptr;
};

#endif // CHIGGUI_MAINWINDOW_H
