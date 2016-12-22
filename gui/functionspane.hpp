#pragma once

#ifndef CHIGGUI_FUNCTIONS_PANE_HPP
#define CHIGGUI_FUNCTIONS_PANE_HPP

#include <QListWidget>

#include <chig/JsonModule.hpp>

class MainWindow;

class FunctionsPane : public QListWidget
{
	Q_OBJECT
public:
	FunctionsPane(QWidget* parent, MainWindow* win);

public slots:

	void updateModule(chig::JsonModule* mod);

private slots:

    void selectItem(QListWidgetItem* newitem);

signals:

	void functionSelected(chig::GraphFunction* funcs);
};

#endif  // CHIGGUI_FUNCTIONS_PANE_HPP
