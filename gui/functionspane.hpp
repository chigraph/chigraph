#pragma once

#ifndef CHIGGUI_FUNCTIONS_PANE_HPP
#define CHIGGUI_FUNCTIONS_PANE_HPP

#include <QListWidget>

#include <chig/JsonModule.hpp>

class MainWindow;

class FunctionsPane : public QListWidget {
	
public:
	
	FunctionsPane(QWidget* parent, MainWindow* win);
	
public slots:
	
	void updateModule(chig::JsonModule* mod);
	
};

#endif // CHIGGUI_FUNCTIONS_PANE_HPP
